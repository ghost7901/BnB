#ifndef _FACTORY_H
#define _FACTORY_H

#pragma once

#include "item.h"
#include <queue>

#define FACTORY CFactory::Instance()

class CFactory
{
private:
	CFactory() : m_nGiftNum(0) { }
	~CFactory() { ReleaseMapPrototype(); ReleaseFixedPrototype(); }

public:
	static CFactory &Instance();
	void LoadFixedPrototype(const char *szIniFile);
	void LoadMapPrototype(const char *szIniFile);

	CItem *MakeFixedItem(int nID) const { return m_vFixedPrototype[nID]->Clone(); }
	CItem *MakeMapItem(int nID) const { return m_vMapPrototype[nID]->Clone(); }
	gstAnimation *MakeMapGround(int nID) const { m_aniGround->SetFrame( nID ); return m_aniGround; }
	int GetGiftNum() { return m_nGiftNum; }
	int GetItemNum() { return m_nItemNum; }
	int GetGroundNum() { return m_aniGround->GetFrames(); }

	void ReleaseFixedPrototype();
	void ReleaseMapPrototype();

private:
	std::vector<CItem *> m_vFixedPrototype;
	std::vector<CItem *> m_vMapPrototype;
	gstAnimation *m_aniGround;
	//std::vector<gstAnimation *> m_vFixedAnimation;
	//std::vector<gstSprite *> m_vMapSprite;
	int m_nGiftNum;
	int m_nItemNum;
};

#endif