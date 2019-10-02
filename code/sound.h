#ifndef _SOUND_H
#define _SOUND_H

#pragma once

#include <gst.h>
#include "common.h"

#define SOUND CSound::Instance()

enum SoundName
{
	sndPlayBg,
	sndMainMenuBg,
	sndMapEdit,
	sndStart,
	sndLay,
	sndExplode,
	sndAppear,
	sndGet,
	sndDie,
	sndWin,
	sndDraw,
	sndSave,
};

class CSound
{
private:
	CSound() { }
	~CSound() { }
public:
	static CSound &Instance();
	void Init();
	void PlaySnd(int id);
	void PlayBgMsc(int id);
	void EnableMusic(bool bEnable);
	void EnableSound(bool bEnable) { m_bSound=bEnable; }
	void StopMusic() { g_pGST->Channel_StopAll(); }
private:
	HEFFECT m_hSound[12];
	bool m_bMusic;
	bool m_bSound;
};

#endif