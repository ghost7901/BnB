#include "sound.h"

CSound &CSound::Instance()
{
	static CSound instance;
	return instance;
}

void CSound::Init()
{
	m_bMusic=true;
	m_bSound=true;

	m_hSound[sndPlayBg]=g_pResManager->GetEffect("sndBg");
	m_hSound[sndMainMenuBg] = g_pResManager->GetEffect( "sndMainMenu" );
	m_hSound[sndMapEdit] = g_pResManager->GetEffect( "sndMapEdit" );
	m_hSound[sndStart]=g_pResManager->GetEffect("sndStart");
	m_hSound[sndLay]=g_pResManager->GetEffect("sndLay");
	m_hSound[sndExplode]=g_pResManager->GetEffect("sndExplode");
	m_hSound[sndAppear]=g_pResManager->GetEffect("sndAppear");
	m_hSound[sndGet]=g_pResManager->GetEffect("sndGet");
	m_hSound[sndDie]=g_pResManager->GetEffect("sndDie");
	m_hSound[sndWin]=g_pResManager->GetEffect("sndWin");
	m_hSound[sndDraw]=g_pResManager->GetEffect("sndDraw");
	m_hSound[sndSave]=g_pResManager->GetEffect("sndSave");
}

void CSound::PlaySnd(int id)
{
	 if( ( id < sndStart ) || !m_bSound ) return;

	 g_pGST->Effect_PlayEx( m_hSound[id] );
}

void CSound::PlayBgMsc(int id)
{
	if( id >= sndStart || !m_bMusic ) return;

	StopMusic();
	g_pGST->Effect_PlayEx( m_hSound[id], 100, 0, 1.0f, true );
}

void CSound::EnableMusic(bool bEnable)
{
	if(bEnable==m_bMusic) return;

	if(bEnable)
	{
		PlayBgMsc(sndPlayBg);
	}
	else
		g_pGST->Channel_StopAll();
}