#ifndef _GAME_H
#define _GAME_H

#pragma once

#include "map.h"
#include "gui.h"


class CGame
{
private:
	enum GameState { gsStart, gsRun, gsEnd };
	enum EndType { etDraw, etFstWin, etSecWin };

public:
	CGame() : m_fTime(0.0f) { }
	virtual ~CGame( );

public:
	static void Init();
	void Start();
	int Update(float dt);
	void Render();
	void LoadMap();
	void SetMapName(const char *szMapName) { strcpy( m_szMapName, szMapName ); }
	void CursorRender(float mx,float my) { m_sprCursor->Render( mx, my ); }
	void RoleOnDie(CRole *pRole);
	void RoleDead(CRole *pRole);
	void RenderNum(float x,float y,int num);

protected:
	void ProcessInput();
	void ClearBomb();
	void RenderStart();
	void RenderEnd();

protected:
	float m_fTime;
	int m_nAniSeq;
	int m_nTimeRemain;
	char m_szMapName[MAX_PATH];
	static gstSprite *m_sprBg;
	static gstSprite *m_sprKick;
	static gstSprite *m_sprCursor;
	static gstAnimation *m_aniWin;
	static gstAnimation *m_aniDraw;
	static gstAnimation *m_aniPlayer;
	static gstAnimation *m_aniStart;
	static gstAnimation *m_aniNumber;	
	GameState m_State;
	EndType m_EndType;

public:
	static gstGUI *m_gui;
};

extern CGame *g_pGame;

#endif