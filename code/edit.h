#ifndef _EDIT_H
#define _EDIT_H

#pragma once

#define LST_MAPTYPE 2
#define BTN_NEW 3
#define BTN_OPEN 4
#define BTN_SAVE 5
#define BTN_SAVE_AS 6
#define BTN_PAGE_PREV 7
#define BTN_PAGE_NEXT 8
#define CHECK_GRID 9

#include "common.h"

class gstGUI;
class gstFontCN;
//class CItem;
//class GUI;

class CEdit
{
public:
	CEdit();
	~CEdit();
	int Update(float dt);
	void Render();
	void ShowGrid();
	void ShowCoordinate();

private:
	int DoCommands(int nID);
	void New();
	void LoadMapType(const char *szIniFile);
	void SaveAs();
	void OpenMap();
	void NewMapType(int nNewType);

private:
	gstGUI *m_gui;
	gstSprite *m_sprBg;
	gstSprite *m_sprHighlight;
	gstSprite *m_sprGroundBtn;
	gstSprite *m_sprItemBtn;

	int m_nItemX;
	int m_nItemY;
	int m_nSelID;
	int m_nSelType;
	int m_nType[55];
	int m_nItemType[55];
	int m_nMapType;
	int m_nPageNum;
	bool m_bShowGrid;
	char m_szSaveName[MAX_PATH];
	gstFont *m_fntDigit;
//	gstFontCN *m_fntcn;
};

extern CEdit *g_pEdit;

#endif