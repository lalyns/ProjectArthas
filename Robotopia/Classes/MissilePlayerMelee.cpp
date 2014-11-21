#include "pch.h"
#include "GameManager.h"
#include "PhysicsComponent.h"
#include "MissilePlayerMelee.h"
#include "ComponentManager.h"
#include "ObserverComponent.h"
#include "TriggerManager.h"
#include "PhysicsTrigger.h"
#include "AnimationComponent.h"
#include "DataManager.h"
#include "StageManager.h"
#include "StateChangeTrigger.h"


void MissilePlayerMelee::initMissile()
{
	m_IsPlayerMissile = true;
	m_IsUsable = true;
	m_Type = OT_MISSILE_PLAYER_MELEE;
	
	setAnchorPoint(cocos2d::Point::ANCHOR_MIDDLE);

	auto observer = GET_COMPONENT_MANAGER()->createComponent<ObserverComponent>();
	addComponent(observer);


	auto physics = GET_COMPONENT_MANAGER()->createComponent<PhysicsComponent>();
	addComponent(physics);
	physics->initPhysics(cocos2d::Rect(0, 0, 60, 45), false, 0, 0, 0, PHYC_ALL, PHYC_NONE, PHYC_NONE);
	physics->setEnabled(false);
	
	auto animation = GET_COMPONENT_MANAGER()->createComponent<AnimationComponent>();
	addComponent(animation);
	animation->setAnimation(AT_MISSILE_PLAYER_MELEE, this, 1);

}
	
void MissilePlayerMelee::setAttribute(cocos2d::Point pos, Direction attackDir,
											  float damage, cocos2d::Size contentsSize,
											  cocos2d::Vec2 velocity,
											  cocos2d::Point targetPos)
{
	//setAttribute는 Manager에서 부를 때 한다.
	//즉 이 때가 사용 시점
	m_Velocity = velocity;
	m_Damage = damage;
	m_AttackDir = attackDir;
	m_TargetPos = targetPos;
	cocos2d::Point setPos;
	float rotation = 0.f;
	if (attackDir == DIR_LEFT)
	{
		rotation = 180.f;
	}


	setPos.x = pos.x + cos(rotation / 90 * MATH_PIOVER2)*contentsSize.width * 1.5f;
	setPos.y = pos.y + sin(rotation / 90 * MATH_PIOVER2)*contentsSize.height;

	setPosition(setPos);
	setRotation(rotation);

	//여기서 physics를 켜야겠다. 
	auto physicsCompo = (PhysicsComponent*)getComponent(CT_PHYSICS);
	physicsCompo->setEnabled(true);
	auto body = physicsCompo->getBody();

	
	auto animationCompo = (AnimationComponent*)getComponent(CT_ANIMATION);
	animationCompo->enter();

	m_IsUsable = false;
}

void MissilePlayerMelee::update(float dTime)
{
	
	for (auto& BaseComponent : getChildren())
	{
		BaseComponent->update(dTime);
	}

	auto observer = (ObserverComponent*)getComponent(CT_OBSERVER);
	if (observer)
	{
		auto triggers = observer->getTriggers();

		for (auto& pTrigger : triggers)
		{
			auto aniEndTrigger = GET_TRIGGER_MANAGER()->createTrigger<StateChangeTrigger>();
			aniEndTrigger->initChangingStates(CT_ANIMATION, CT_NONE);
			if (*aniEndTrigger == *pTrigger)
			{
				auto physicsCompo = (PhysicsComponent*)getComponent(CT_PHYSICS);
				physicsCompo->setEnabled(false);
				m_IsUsable = true;
				removeFromParent();
			}
		}
	}

	
}

bool MissilePlayerMelee::init()
{
	if (!Missile::init())
	{
		return false;
	}

	return true;
}
void MissilePlayerMelee::enter()
{
}

void MissilePlayerMelee::exit()
{
}