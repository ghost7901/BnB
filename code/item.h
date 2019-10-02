#ifndef _ITEM_H
#define _ITEM_H

#pragma once

#include "common.h"
#include "ability.h"

enum Direction
{
	drUp,
	drDown,
	drLeft,
	drRight,
	drMiddle,
	drUpLeft,
	drUpRight,
	drDownLeft,
	drDownRight	
};

struct Cell;

class CItem
{
public:
	CItem(void) { }
	virtual ~CItem(void) { }	

	virtual CItem *Clone(void) const = 0;
	virtual void Update(float dt) { }
	virtual void Render(void) const { }
	
	void SetPosition(int y, int x) { m_nCellX = x; m_nCellY = y; }

protected:
	Cell *GetCell(Direction dr);

protected:
	int m_nCellX;
	int m_nCellY;
};

class CAniItem : public CItem
{
public:
	void LoadMainAni(gstAnimation *ani) { m_aniMain = ani; }
	void LoadShadowAni(gstAnimation *ani) { m_aniShadow = ani; }
	gstAnimation *GetShadow(void) { return m_aniShadow; }

	virtual void Update(float dt);
	virtual void Render() const;

protected:
	gstAnimation *m_aniMain;
	gstAnimation *m_aniShadow;
};

class CMedium : public CItem
{
public:
	CMedium(void) : m_sprMain(NULL), m_sprShadow(NULL) { }
	void LoadMainSprite(gstSprite *spr) { m_sprMain = spr; }
	void LoadShadowSprite(gstSprite *spr) { m_sprShadow = spr; }
	gstSprite *GetShadow(void) { return m_sprShadow; }

	virtual CItem* Clone(void) const { return new CMedium(*this); }
	virtual void Update(float dt) { }
	virtual void Render(void) const;

protected:
	gstSprite *m_sprMain;
	gstSprite *m_sprShadow;
};

class CMediumAni : public CAniItem
{
public:
	CMediumAni(void) { }
	virtual void Render(void) const { };
	virtual CItem *Clone(void) const { return new CMediumAni( *this ); }
};

class CLarge : public CMedium
{
public:
	CLarge() : m_fTime(0), m_bPlay(false) { }
	virtual void Update(float dt);
	virtual void Render(void) const;
	virtual CItem *Clone(void) const { return new CLarge( *this ); }

	void LoadMainAni(gstAnimation *ani) { m_aniMain = ani; m_aniMain->Play(); }

private:
	gstAnimation *m_aniMain;
	float m_fTime;
	bool m_bPlay;
};

class CBox : public CMedium
{
public:
	CBox() : m_fOffsetX(0), m_fOffsetY(0), m_bMoving(false) { }
	virtual void Update(float dt);
	virtual void Render() const;
	virtual CItem *Clone(void) const { return new CBox( *this ); }
	void Move(Direction dr);

private:
	float m_fOffsetX;
	float m_fOffsetY;
	bool m_bMoving;
	Direction m_Direction;
};

class CBlindage : public CMedium
{
public:
	CBlindage(void) :m_bWaggle(false) { }
	virtual void Update(float dt) = 0;
	virtual void Waggle(void) { m_bWaggle = true; }

protected:
	bool m_bWaggle;
};

class CBush : public CBlindage
{
public:
	CBush() : m_nStatus(0), m_fTime(0.0f) { }
	virtual void Update(float dt);
	virtual void Render(void) const;
	virtual CItem *Clone(void) const { return new CBush( *this ); }

protected:
	int m_nStatus;

private:
	float m_fTime;
};

class CTent : public CBlindage
{
public:
	virtual void Waggle() { m_aniMain->Play(); CBlindage::Waggle(); }
	virtual void Render(void) const;
	virtual void Update(float dt);
	virtual CItem *Clone(void) const
	{
		CTent *tent = new CTent( *this );
		tent->m_aniMain = new gstAnimation( *m_aniMain );
		return tent;
	}

	void LoadMainAni(gstAnimation *ani) { m_aniMain = ani; }

private:
	gstAnimation *m_aniMain;
};

class CGift : public CAniItem
{
public:
	CGift(void) : m_fTime(0.0f), m_nDir(1), m_bStart(true), m_nScale(1), m_nOffsetY(0) { }
	void SetAbility(const Ability &ab) { m_Ability = ab; }
	const Ability &GetAbility() const { return m_Ability; }
	virtual CItem *Clone(void) const
	{
		CGift *pGift = new CGift( *this );
		pGift->m_aniMain = new gstAnimation( *m_aniMain );
		pGift->m_aniShadow = new gstAnimation( *m_aniShadow );
		return pGift;
	}
	virtual void Update(float dt);
	virtual void Render(void) const;

private:
	bool m_bStart;	
	float m_fTime;
	int m_nScale;
	int m_nOffsetY;
	int m_nDir;
	Ability m_Ability;
};

class CRole;

class CPopo : public CAniItem
{
private:
	enum ExpProperty { epNone, epFirm, epPopo, epSolid };
	ExpProperty GetExpProperty(int nCellY,int nCellX,Direction dr);

public:
	CPopo() : m_nPower(1), m_fOffsetX(0), m_fOffsetY(0), m_fRemainTime(3.3f), m_bMoving(false) { }
	virtual void Update(float dt);
	virtual void Render(void) const;
	void SetPower(int nPower) { m_nPower = nPower; }
	void SetOwner(CRole *pOwner) { m_pOwner = pOwner; }
	CRole *GetOwner() { return m_pOwner; }
	virtual CItem *Clone(void) const
	{
		CPopo *pPopo = new CPopo( *this );
		pPopo->m_aniMain = new gstAnimation( *m_aniMain );
		pPopo->m_aniShadow = new gstAnimation( *m_aniShadow );
		return pPopo;
	}
	void Explode();
	void Move(Direction dr) { m_bMoving = true; m_Direction = dr; }

private:
	float m_fRemainTime;
	int m_nPower;
	float m_fOffsetX;
	float m_fOffsetY;
	CRole *m_pOwner;
	bool m_bMoving;
	Direction m_Direction;
};

#endif