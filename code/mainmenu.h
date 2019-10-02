#ifndef _MAINMENU_H
#define _MAINMENU_H

#include "common.h"
#include "gui.h"

#define BTN_START 1
#define BTN_EDIT 2
#define BTN_EXIT 3
#define LST_SELMAP 4
#define BTN_MAPTYPE1 5
#define BTN_MAPTYPE2 6

class CMainMenu
{
public:
	CMainMenu();
	~CMainMenu();

	int Update(float dt);
	void Render();
	void Restore();

private:
	int DoCommands(int nID);
	void LoadTypeMap();

private:
	gstSprite *m_spr;
	gstGUI *m_gui;
	gstSprite *m_sprTab;

	int m_nMapType;
};

#endif
