﻿	/************************************************************************/
/*
CLASS			: Player
Author			: 김연우
역할				: Player class
최종 수정일자	: 2014-10-29
최종 수정자		:
최종 수정사유	:
Comment			: Player 동작 정의.
*/
/************************************************************************/
#pragma once
#include "Creature.h"
#include "Util.h"

#define PLAYER_WIDTH 32
#define PLAYER_HEIGHT 50
class PlayerRenderer;
class CommonInfo;
class Tile;
class Floor;
class Missile;
class Player : public Creature
{
public:
	Player();
	virtual ~Player();

	enum State
	{
		STAT_NONE = -1,
		STAT_IDLE,
		STAT_MOVE,
		STAT_JUMP,
		STAT_JUMP_DOWN,
		STAT_FLY,
		STAT_KNOCKBACK,
		STAT_NUM,
	};

	enum AttackState
	{
		AS_NONE = -1,
		AS_ATK_IDLE,
		AS_ATTACK,
		AS_KNOCKBACK,
		AS_NUM,
	};

	OVERRIDE bool				init();
	OVERRIDE void				update(float dTime);
	OVERRIDE void				enter();
	OVERRIDE void				exit();

	CREATE_FUNC( Player );

	virtual bool				onContactBegin(cocos2d::PhysicsContact& contact);
	virtual void				onContactSeparate(cocos2d::PhysicsContact& contact);

	bool						contactMonster(cocos2d::PhysicsContact& contact, Creature* monster);
	bool						contactTrap(cocos2d::PhysicsContact& contact, BaseComponent* trap);
	bool						contactFloor(cocos2d::PhysicsContact& contact, Floor* floor, bool isComponentA);
	bool						contactMissile(cocos2d::PhysicsContact& contact, Missile* missile);

	void						idleInEagle(Creature* target, double dTime, int idx);
	void						move(Creature* target, double dTime, int idx);
	void						jump(Creature* target, double dTime, int idx);
	void						fly(Creature* target, double dTime, int idx);

	void						meleeAttack(Creature* target, double dTime, int idx);
	void						rangeAttack(Creature* target, double dTime, int idx);

	void						enterMove(Direction dir);
	void						exitMove();
		
	void						enterJump(bool isFall);
	void						enterDownJump();

	void						idleTransition(Creature* target, double dTime, int idx);
	void						idleTransitionInMonkey(Creature* target, double dTime, int idx);
	void						idleTransitionInEagle(Creature* target, double dTime, int idx);
	void						moveTransition(Creature* target, double dTime, int idx);
	void						jumpTransition(Creature* target, double dTime, int idx);
	void						downJumpTransition(Creature* target, double dTime, int idx);
	void						flyTransition(Creature* target, double dTime, int idx);
	
	void						attackIdleTransition(Creature* target, double dTime, int idx);
	void						attackIdleTransitionInMonkey(Creature* target, double dTime, int idx);
	void						attackIdleTransitionInEagle(Creature* target, double dTIme, int idx);
	void						meleeAttackTransition(Creature* target, double dTime, int idx);
	void						rangeAttackTransition(Creature* target, double dTime, int idx);

	void						knockbackTransition(Creature* target, double dTime, int idx);

	//get,set 함수
	virtual const PlayerInfo&	getInfo() const;
	void						setInfo(PlayerInfo info);
	void						setDirection(Direction dir);

	//gear 관련 처리
	void						gearSetting();

	//hp 관련 처리
	void						hit(float damage);

	//steam 관련 처리
	void						consumeFlySteam();
	void						consumeMeleeAttackSteam();
	void						consumeRangeAttackSteam();

protected:
	//FSM
	void						initFSMAndTransition();

	PlayerInfo					m_Info;
	int							m_FSMNum;
	PlayerRenderer*				m_PlayerRenderer;

private:

	float						m_GearDelay = 0.0f;
	int							m_KnockbackStartTime;
	bool						m_IsInvincible;
	bool						m_Jumping;
	int							m_JumpTime;
	int							m_InvincibleStartTime;
	int							m_AttackStartTime;
	std::vector<FSMFunctions>	m_GearFSMs[GEAR_NUM];
	std::vector<FSMFunctions>	m_GearTransitions[GEAR_NUM];
	double						m_FlyTime;
};

