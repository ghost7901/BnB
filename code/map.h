#ifndef _MAP_H
#define _MAP_H

#pragma once

#include "factory.h"
#include "role.h"

const int CELL_SIZE = 40;
const int CELL_X = 15;
const int CELL_Y = 13;
const int CELL_X_ALL = 17;
const int CELL_Y_ALL = 15;
const int CELL_NUM = CELL_X_ALL * CELL_Y_ALL;
const int SCENE_LEFT = 20;
const int SCENE_TOP = 41;

typedef unsigned char BYTE;

enum ItemType
{
	itNone,
	itFirm,
	itSolid,
	itBox,
	itPopo,
	itGift,
};

enum BlindageType
{
	btNone,
	btBlind,
	btBush,
	btTent,
};

struct ExplosionData
{
	BYTE Dr			: 4;
	BYTE IsHead		: 4;
	BYTE Gift		: 4;
	BYTE Timer		: 4;
	BYTE Status;
	BYTE Dead;
};

struct Cell
{
	Cell() : pItem(NULL), pBlindage(NULL) { }
	BYTE Ground;
	BYTE Type;
	BYTE Blind;
	BYTE ID;
	CItem *pItem;
	CBlindage *pBlindage;
	ExplosionData Explosion;
};

#define MAP CMap::Instance()

class CMap
{
private:
	CMap();
	~CMap();

//	friend class CEdit;
//	friend class CMainMenu;

public:
	static CMap &Instance();
	void Update(float dt);
	void Render(bool bEdit=false) const;

	int LoadMap(const char *szFileName);
	void SaveMap(const char *szFileName,int nMapType);
	void RestoreMap();

	Cell *operator[](int i) { return m_vCell + i * CELL_X_ALL; }

private:
	void ReleaseMap();
	void RenderGround() const;
	void RenderExplosion(int nCellY,int nCellX) const;

private:
//	gstAnimation *m_aniGround;
//	gstAnimation *m_aniExplosion;	
	Cell m_vCell[CELL_NUM];
	int m_vRoleSeq[ROLE_NUM];

	gstSprite *m_sprExplosion;
};

#endif