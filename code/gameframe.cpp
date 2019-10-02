#include "gameframe.h"
#include "resource.h"
#include "mainmenu.h"
#include "sound.h"
#include "edit.h"
#include "loading.h"

CGameFrame::CGameFrame()
{
	m_pMenu = NULL;
	m_SysState = ssLoading;
}

CGameFrame::~CGameFrame()
{
	SAFE_DELETE( m_pMenu );
//	SAFE_DELETE( CGame::m_gui );
	SAFE_DELETE( g_pGame );
}

bool CGameFrame::InitGameFrame(bool (*pFrameFunc)(),bool (*pRenderFunc)())
{
	if(!pFrameFunc) return false;

	g_pGST=gstCreate(GST_VERSION);
	if(!g_pGST) return false;

	g_pGST->System_SetState( GST_LOGFILE, "game.log" );
	g_pGST->System_SetState( GST_SCREENWIDTH, 800 );
	g_pGST->System_SetState( GST_SCREENHEIGHT, 600 );	
	g_pGST->System_SetState( GST_SCREENBPP, 32 );
	g_pGST->System_SetState( GST_FPS, 60 );
	g_pGST->System_SetState( GST_WINDOWED, true );
	g_pGST->System_SetState( GST_TITLE, "ÅÝÅÝÌÃË«ÈË¶ÔÕ½°æ" );
	g_pGST->System_SetState( GST_ICON, MAKEINTRESOURCE( IDI_ICON1 ) );
	g_pGST->System_SetState( GST_FRAMEFUNC, pFrameFunc );
	g_pGST->System_SetState( GST_RENDERFUNC, pRenderFunc );
	g_pGST->System_SetState( GST_USESOUND, true );
	g_pGST->System_SetState( GST_SHOWSPLASH, false );

	g_pGST->System_SetState( GST_INIFILE, "config.ini" );

	if( g_pGST->System_Initiate() )
	{		
		resManagerCreate();
		//g_pGame = new CGame();
		CGame::Init();
		//g_pResManager->Precache();
		m_pLoading = new CLoading();
				
		return true;
	}

	return false;
}

bool CGameFrame::StartGameFrame()
{
	return g_pGST->System_Start();
}

void CGameFrame::NewGame()
{
	//SAFE_DELETE( g_pGame );
	//g_pGame = new CGame();
	g_pGame->Start();
}

bool CGameFrame::GameFrameUpdate()
{
	if( g_pGST->Input_GetKeyState( GSTK_ESCAPE ) ) return true;

	g_pGST->Input_GetMousePos( &m_fMX, &m_fMY );

	if( m_SysState == ssLoading )
	{
		int nProcess = m_pLoading->Update();
		if( nProcess == 100 )
		{
			delete m_pLoading;
			g_pGame = new CGame();			
			m_pMenu = new CMainMenu();
			m_SysState = ssMainMenu;
			SOUND.PlayBgMsc( sndMainMenuBg );
		}
	}
	else
	{
		switch( m_SysState )
		{
		case ssMainMenu:
			switch( m_pMenu->Update( g_pGST->Timer_GetDelta() ) )
			{
			case BTN_START:
				m_SysState = ssGame;
				NewGame();
				break;
			case BTN_EDIT:
				SAFE_DELETE( g_pEdit );
				g_pEdit = new CEdit();
				SOUND.PlayBgMsc( sndMapEdit );
				m_SysState = ssEdit;
				break;
			case BTN_EXIT:
				return true;
			}
			break;
		case ssEdit:
			switch( g_pEdit->Update( g_pGST->Timer_GetDelta() ) )
			{
			case BTN_RETURN:
				SAFE_DELETE( g_pEdit );
				m_pMenu->Restore();
				SOUND.PlayBgMsc( sndMainMenuBg );
				m_SysState = ssMainMenu;
				break;
			}
			break;
		case ssGame:
			switch( g_pGame->Update( g_pGST->Timer_GetDelta() ) )
			{
			case -1:
				g_pGame->LoadMap();
				NewGame();
				break;
			case BTN_RETURN:
				m_pMenu->Restore();
				SOUND.PlayBgMsc( sndMainMenuBg );
				m_SysState = ssMainMenu;
				break;
			}
			break;
		}
	}

	return false;
}

bool CGameFrame::GameFrameRender()
{
	g_pGST->Gfx_BeginScene();

	switch( m_SysState )
	{
	case ssLoading:
		m_pLoading->Render();
		break;
	case ssMainMenu:
		m_pMenu->Render();
		break;
	case ssEdit:
		g_pEdit->Render();
		break;
	case ssGame:
		g_pGame->Render();
		break;
	}

	if( m_SysState != ssLoading && g_pGST->Input_IsMouseOver() ) g_pGame->CursorRender( m_fMX, m_fMY );

	g_pGST->Gfx_EndScene();

	return true;
}