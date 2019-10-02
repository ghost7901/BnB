#ifndef _GAMEFRAME_H
#define _GAMEFRAME_H

#include "game.h"

class CMainMenu;
class CLoading;

class CGameFrame
{
private:
	enum SysState { ssLoading, ssMainMenu, ssEdit, ssGame };

public:
	CGameFrame();
	~CGameFrame();

public:
	bool InitGameFrame(bool (*pFrameFunc)(),bool (*pRenderFunc)());
	bool StartGameFrame();
	void NewGame();
	bool GameFrameUpdate();
	bool GameFrameRender();

private:
	float m_fMX;
	float m_fMY;
	CMainMenu *m_pMenu;
	SysState m_SysState;
	CLoading *m_pLoading;
};

#endif