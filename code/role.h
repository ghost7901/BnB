#ifndef _ROLE_H
#define _ROLE_H

#pragma once

#include "common.h"
#include "item.h"
#include "ability.h"

class CRoleState;
////////////////////////////////////////////
// CRole
////////////////////////////////////////////
class CRole
{
public:
	CRole();
	~CRole();

	void Win();
	void Normal();
	void Load(const char *szIniFile);
	void Start();
	void New();
	void Update(float dt);
	void Render();
	void SetPosition(int nCellY,int nCellX) { m_nCellY = nCellY; m_nCellX = nCellX; }
	int GetX() const { return m_nCellX; }
	int GetY() const { return m_nCellY; }
	float GetOffsetX() const { return m_fOffsetX; }
	float GetOffsetY() const { return m_fOffsetY; }
	int GetWinCount() const { return m_nWinCount; }
	bool IsDead() const { return m_bDead; }
	bool IsFlying() const { return m_bFlying; }
	Ability &GetAbility() { return m_Ability; }

	void Move(Direction dr);
	void LayPopo();
	void ChangeState(CRoleState *pState);

private:
	enum WalkProperty { wpNone, wpTent, wpStop };

	template <Direction dr>
	WalkProperty GetWalkProperty();

	template <Direction dr>
	void Push();

	void UpdateMoveUp(float fSpeed);
	void UpdateMoveDown(float fSpeed);
	void UpdateMoveLeft(float fSpeed);
	void UpdateMoveRight(float fSpeed);

	void EnterNewCell();
	void AdjustCell();

private:
	friend class CRoleState;
	friend class CRoleStateStart;
	friend class CRoleStateNormal;
	friend class CRoleStateIdle;
	friend class CRoleStateOnTrap;
	friend class CRoleStateTraped;
	friend class CRoleStateOnDie;
	friend class CRoleStateDead;
	friend class CRoleStateWin;
	friend class CRoleStateOnRide;
	friend class CRoleStateOnDismount;

private:
	gstAnimation *m_aniBigPopo;
	gstAnimation *m_aniBigPopoBomb;
	gstAnimation *m_aniFastTurtle;
	gstAnimation *m_aniSlowTurtle;
	gstAnimation *m_aniFastUFO;
	gstAnimation *m_aniOwl;
	int m_nCellX;
	int m_nCellY;
	float m_fOffsetX;
	float m_fOffsetY;
	int m_nWinCount;
	int m_nPushTime;
	Direction m_Direction;

	gstAnimation *m_aniRoleUp;
	gstAnimation *m_aniRoleDown;
	gstAnimation *m_aniRoleLeft;
	gstAnimation *m_aniRoleRight;
	gstAnimation *m_aniRole;
	gstAnimation *m_aniRoleWin;
	gstAnimation *m_aniRoleIdle;
	gstAnimation *m_aniRoleDie;
	gstAnimation *m_aniRoleStart;
	gstAnimation *m_aniRoleStartFlash;
	gstAnimation *m_aniRoleRide;
	gstSprite *m_sprShadow;
	CRoleState *m_pState;
	bool m_bMoving;
	bool m_bFlying;
	bool m_bDead;
	Ability m_Ability;
};

////////////////////////////////////////////
// CRoleState
////////////////////////////////////////////
enum RoleState
{
	rsStart,
	rsNormal,
	rsIdle,
	rsTraped,
	rsOnTrap,
	rsOnSave,
	rsOnDie,
	rsDead,
	rsWin,
	rsOnRide,
	rsOnDismount,
};

class CRoleState
{
public:
	CRoleState(CRole *pRole) : m_pRole(pRole) { }
	virtual ~CRoleState() { }
	virtual void Update(float dt) = 0;
	virtual void Render(float x,float y) const = 0;
	virtual RoleState Type() const = 0;
	void ChangeState(CRoleState *pState) { m_pRole->ChangeState( pState ); }

protected:
	void CheckChange();
	void RenderShadow(float x,float y) const;

protected:
	CRole *m_pRole;
};

////////////////////////////////////////////
// CRoleStateStart
////////////////////////////////////////////
class CRoleStateStart : public CRoleState
{
public:
	CRoleStateStart(CRole *pRole) : CRoleState(pRole), m_fTime(0.0f), m_bFlash(false) { m_pRole->m_aniRoleStart->Play(); m_pRole->m_aniRoleStartFlash->Play(); }
	virtual void Update(float dt);
	virtual void Render(float x,float y) const;
	virtual RoleState Type() const { return rsStart; }
private:
	float m_fTime;
	bool m_bFlash;
};

////////////////////////////////////////////
// CRoleStateNormal
////////////////////////////////////////////
class CRoleStateNormal : public CRoleState
{
public:
	CRoleStateNormal(CRole *pRole) : CRoleState(pRole), m_fTime(0.0f) { }
	virtual void Update(float dt);
	virtual void Render(float x,float y) const;
	virtual RoleState Type() const { return rsNormal; }
private:
	float m_fTime;
};

////////////////////////////////////////////
// CRoleStateIdle
////////////////////////////////////////////
class CRoleStateIdle : public CRoleState
{
public:
	CRoleStateIdle(CRole *pRole) : CRoleState(pRole), m_fTime(0.0f) { m_pRole->m_aniRoleIdle->Play(); }
	virtual void Update(float dt);
	virtual void Render(float x,float y) const;
	virtual RoleState Type() const { return rsIdle; }
private:
	float m_fTime;
};

////////////////////////////////////////////
// CRoleStateOnTrap
////////////////////////////////////////////
class CRoleStateOnTrap : public CRoleState
{
public:
	CRoleStateOnTrap(CRole *pRole) : CRoleState(pRole)
	{ m_pRole->m_aniBigPopo->SetColor( ARGB( 200, 255, 255, 255 ) ); m_pRole->m_aniBigPopo->Play(); };
	virtual void Update(float dt);
	virtual void Render(float x,float y) const;
	virtual RoleState Type() const { return rsOnTrap; }
};

////////////////////////////////////////////
// CRoleStateTraped
////////////////////////////////////////////
class CRoleStateTraped : public CRoleState
{
public:
	CRoleStateTraped(CRole *pRole) : CRoleState(pRole), m_fTime(0.0f), m_nShake(0), m_fDeadTime(5.0f), m_nShakeDr(1)
	{ m_pRole->m_aniBigPopo->SetFrame( 3 ); m_pRole->m_aniBigPopo->SetColor( ARGB( 200, 255, 255, 255 ) ); }
	virtual void Update(float dt);
	virtual void Render(float x,float y) const;
	virtual RoleState Type() const { return rsTraped; }
private:
	float m_fTime;
	float m_fDeadTime;
	int m_nShake;
	int m_nShakeDr;
};

////////////////////////////////////////////
// CRoleStateOnDie
////////////////////////////////////////////
class CRoleStateOnDie : public CRoleState
{
public:
	CRoleStateOnDie(CRole *pRole);
	virtual void Update(float dt);
	virtual void Render(float x,float y) const;
	virtual RoleState Type() const { return rsOnDie; }
private:
	float m_fTime;
	bool m_bShow;
};

////////////////////////////////////////////
// CRoleStateDead
////////////////////////////////////////////
class CRoleStateDead : public CRoleState
{
public:
	CRoleStateDead(CRole *pRole) : CRoleState(pRole) { }
	virtual void Update(float dt) { }
	virtual void Render(float x,float y) const { }
	virtual RoleState Type() const { return rsDead; }
};

////////////////////////////////////////////
// CRoleStateWin
////////////////////////////////////////////
class CRoleStateWin : public CRoleState
{
public:
	CRoleStateWin(CRole *pRole) : CRoleState(pRole), m_fTime(0.0f) { m_pRole->m_aniRoleWin->Play(); }
	virtual void Update(float dt);
	virtual void Render(float x,float y) const;
	virtual RoleState Type() const { return rsWin; }
private:
	float m_fTime;
};

////////////////////////////////////////////
// CRoleStateOnRide
////////////////////////////////////////////
class CRoleStateOnRide : public CRoleState
{
public:
	CRoleStateOnRide(CRole *pRole);
	virtual void Update(float dt);
	virtual void Render(float x,float y) const;
	virtual RoleState Type() const { return rsOnRide; }
private:
	float m_fTime;
	int m_nAniSeq;
};

////////////////////////////////////////////
// CRoleStateOnDismount
////////////////////////////////////////////
class CRoleStateOnDismount : public CRoleState
{
public:
	CRoleStateOnDismount(CRole *pRole) : CRoleState(pRole), m_fTime(0.0f), m_nAniSeq(18) { }
	virtual void Update(float dt);
	virtual void Render(float x,float y) const;
	virtual RoleState Type() const { return rsOnDismount; }
private:
	float m_fTime;
	int m_nAniSeq;
};

const int ROLE_NUM = 2;
extern CRole ROLE[ROLE_NUM];

#endif