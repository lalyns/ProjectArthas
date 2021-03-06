﻿#include "pch.h"
#include "RushPig.h"
#include "SpriteComponent.h"
#include "GameManager.h"
#include "ComponentManager.h"
#include "DataManager.h"
#include "StageManager.h"
#include "ResourceManager.h"
#include "AnimationComponent.h"
#include "Corpse.h"
#include "Missile.h"

bool RushPig::init()
{
	if(!BaseComponent::init())
	{
		return false;
	}

	m_Type = OT_MONSTER_RUSH;

	//물리 초기화
	auto physics = GET_COMPONENT_MANAGER()->createComponent<PhysicsComponent>();
	addComponent(physics);
	physics->initPhysics(cocos2d::Rect(0, 0, RUSH_WIDTH, RUSH_HEIGHT), true, 0, 0, 0,
						 PHYC_BLOCK, PHYC_MONSTER, PHYC_BLOCK | PHYC_FLOOR);
	physics->addIgnoreCollision(OT_FLOOR, DIR_UP);

	auto meterial = cocos2d::PhysicsMaterial(0, 0, 0);
	m_Body = cocos2d::PhysicsBody::createBox(cocos2d::Size(RUSH_WIDTH, RUSH_HEIGHT), meterial, cocos2d::Point(0, 0));
	m_Body->setContactTestBitmask(PHYC_BLOCK | PHYC_PLAYER | PHYC_MISSILE);
	m_Body->setCategoryBitmask(PHYC_MONSTER);
	m_Body->setCollisionBitmask(PHYC_BLOCK | PHYC_FLOOR | PHYC_MISSILE);
	m_Body->setMass(10);
	m_Body->setRotationEnable(false);
	m_Body->setVelocityLimit(1000);
	m_Body->setVelocity(cocos2d::Vec2(0, 0));
	m_Body->setDynamic(true);
	m_Body->retain();
	setPhysicsBody(m_Body);

	//FSM 초기화
	initFSM(1);
	m_States[0] = STAT_IDLE;

	m_FSMs[0].resize(STAT_NUM);
	m_FSMs[0][STAT_IDLE] = nullptr;
	m_FSMs[0][STAT_MOVE] = move;

	m_Transitions[0].resize(STAT_NUM);
	m_Transitions[0][STAT_IDLE] = idleTransition;
	m_Transitions[0][STAT_MOVE] = moveTransition;


	m_Renders[0].resize(STAT_NUM);
	m_Renders[0][STAT_IDLE] = GET_COMPONENT_MANAGER()->createComponent<AnimationComponent>();
	( (AnimationComponent*) m_Renders[0][STAT_IDLE] )->setAnimation(AT_MONSTER_RUSH_IDLE, this);
	m_Renders[0][STAT_MOVE] = GET_COMPONENT_MANAGER()->createComponent<AnimationComponent>();
	( (AnimationComponent*) m_Renders[0][STAT_MOVE] )->setAnimation(AT_MONSTER_RUSH_MOVE, this);

	for(int i = 0; i < m_Renders[0].size(); i++)
	{
		addComponent(m_Renders[0][i]);
	}

	//info 설정
	auto data = GET_DATA_MANAGER()->getMonsterInfo(OT_MONSTER_RUSH);

	if (data != nullptr)
	{
		m_Info = *GET_DATA_MANAGER()->getMonsterInfo(OT_MONSTER_RUSH);
	}

	m_Info.m_CurrentHp = m_Info.m_MaxHp;

	return true;
}

void RushPig::enter()
{
	resume();
}

void RushPig::exit()
{
	auto corpse = GET_COMPONENT_MANAGER()->createComponent<Corpse>();
	int roomNum = GET_STAGE_MANAGER()->getRoomNum();
	GET_STAGE_MANAGER()->addObject(corpse, roomNum, getPosition(), RoomZOrder::GAME_OBJECT);
	removeFromParent();
}

void RushPig::idleTransition(Creature* target, double dTime, int idx)
{
	enterMove(target, dTime, DIR_RIGHT);
	target->setState(idx, RushPig::STAT_MOVE);
}

void RushPig::move(Creature* target, double dTime, int idx)
{
	auto monster = static_cast<RushPig*>( target );
	auto pos = monster->getPosition();
	auto velocity = monster->getBody()->getVelocity();
	pos += velocity*dTime;
	target->setPosition(pos);
}

void RushPig::enterMove(Creature* target, double dTime, Direction dir)
{
	auto monster = static_cast<RushPig*>(target);
	auto velocity = monster->getBody()->getVelocity();
	monster->setDirection(dir);

	//속도 임시로 지정.
	if(dir == DIR_LEFT)
	{
		velocity.x = -100;
	}
	else if(dir == DIR_RIGHT)
	{
		velocity.x = 100;
	}
	monster->getBody()->setVelocity(velocity);
}

void RushPig::exitMove(Creature* target, double dTime)
{
	auto monster = static_cast<RushPig*>( target );
	auto velocity = monster->getBody()->getVelocity();

	velocity.x = 0;

	monster->getBody()->setVelocity(velocity);
}

void RushPig::moveTransition(Creature* target, double dTime, int idx)
{
	auto monster = static_cast<RushPig*>( target );
	//->move
	if( !monster->isStepForwardable() )
	{
		auto info = monster->getInfo();
		auto dir = ( info.m_UpperDir == DIR_LEFT ) ? DIR_RIGHT : DIR_LEFT;
		enterMove(target, dTime, dir);
	}
}

bool RushPig::onContactBegin(cocos2d::PhysicsContact& contact)
{
	auto bodyA = contact.getShapeA()->getBody();
	auto bodyB = contact.getShapeB()->getBody();
	auto componentA = (BaseComponent*)bodyA->getNode();
	auto componentB = (BaseComponent*)bodyB->getNode();
	BaseComponent* enemyComponent;
	bool isComponentA = true;

	if (componentA->getType() == getType())
	{
		enemyComponent = componentB;
		isComponentA = true;
	}
	else
	{
		enemyComponent = componentA;
		isComponentA = false;
	}

	//미사일이랑 충돌 처리
	if (enemyComponent->getPhysicsBody()->getCategoryBitmask() == PHYC_MISSILE)
	{
		Missile* missile = static_cast<Missile*>(enemyComponent);

		float damage = missile->getDamage();

		m_Info.m_CurrentHp -= damage * 100 / (100 + m_Info.m_DefensivePower);

		cocos2d::log("HP : %d / %d", m_Info.m_CurrentHp, m_Info.m_MaxHp);
		//사망
		if (m_Info.m_CurrentHp <= 0)
		{
			m_IsDead = true;
		}
	}
	return true;
}


void RushPig::onContactSeparate(cocos2d::PhysicsContact& contact)
{
}

cocos2d::PhysicsBody* RushPig::getBody()
{
	return m_Body;
}

const AllStatus& RushPig::getInfo() const
{
	return m_Info;
}

void RushPig::update(float dTime)
{
	_ASSERT(m_Info.m_CurrentHp <= m_Info.m_MaxHp);

	Creature::update(dTime);
	if(m_Info.m_UpperDir == DIR_LEFT)
	{
		for(int i = 0; i < m_Renders[0].size(); i++)
		{
			m_Renders[0][i]->setFlippedX(true);
		}
	}
	else
	{
		for(int i = 0; i < m_Renders[0].size(); i++)
		{
			m_Renders[0][i]->setFlippedX(false);
		}
	}

}

void RushPig::setDirection(Direction dir)
{
	m_Info.m_UpperDir = dir;
}


bool RushPig::isStepForwardable()
{
	cocos2d::Point currentPosition = getPosition();
	cocos2d::Point nextBelowPosition;
	cocos2d::Point nextStepPosition;
	cocos2d::Vec2 velocityComplement = m_Body->getVelocity()*0.3f;
	float addSize = ( m_Info.m_UpperDir == DIR_RIGHT ) ? m_Info.m_Size.width : -m_Info.m_Size.width;
	
	nextBelowPosition.x = currentPosition.x + addSize + velocityComplement.x;
	nextBelowPosition.y = currentPosition.y - m_Info.m_Size.height + velocityComplement.y;

	nextStepPosition.x = currentPosition.x + addSize + velocityComplement.x;
	nextStepPosition.y = currentPosition.y + velocityComplement.y;
	
	int stageNum = GET_STAGE_MANAGER()->getStageNum();
	int roomNum = GET_STAGE_MANAGER()->getRoomNum();

	int nextBelowTile = GET_DATA_MANAGER()->getTileData(stageNum, roomNum, nextBelowPosition);
	int nextStepTile = GET_DATA_MANAGER()->getTileData(stageNum, roomNum, nextStepPosition);
	bool forwardable =  ( nextBelowTile == OT_BLOCK || nextBelowTile == OT_FLOOR ) && 
						(nextStepTile != OT_BLOCK && nextStepTile != OT_FLOOR) 
						? true : false;

	return forwardable;
}
