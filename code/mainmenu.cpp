#include "mainmenu.h"
#include "stdio.h"
#include "game.h"

const char *g_szString[3] = { "sprTownTab", "sprSandTab", "sprIceTab" };

CMainMenu::CMainMenu() : m_nMapType(1), m_sprTab(0)
{
	m_gui = new gstGUI();

	m_gui->AddCtrl( new CGUIButton( BTN_START, 575, 486, 149, 35,
		g_pResManager->GetSprite( "sprStartOver" ),
		g_pResManager->GetSprite( "sprStartDown" ),
		g_pResManager->GetSprite( "sprStartDisable" ) ) );
	m_gui->AddCtrl( new CGUIButton( BTN_EDIT, 575, 523, 149, 35,
		g_pResManager->GetSprite( "sprEditOver" ),
		g_pResManager->GetSprite( "sprEditDown" ) ) );
	m_gui->AddCtrl( new CGUIButton( BTN_EXIT, 757, 574, 54, 45,
		g_pResManager->GetSprite( "sprExitOver" ),
		g_pResManager->GetSprite( "sprExitDown" ) ) );

	CGUIListbox *lst = new CGUIListbox( LST_SELMAP, 543, 113, 213, 298,
		g_pResManager->GetFont( "fntSelMap" ),
		ARGB( 255,255,255,0 ),ARGB( 255,255,255,0 ),ARGB( 255, 8, 182, 255 ) );
	lst->SetRowSpace( 3.0f );
	m_gui->AddCtrl( lst );

	m_gui->AddCtrl( new CGUIButton( BTN_MAPTYPE1, 509, 48, 57, 31 ) );
	m_gui->AddCtrl( new CGUIButton( BTN_MAPTYPE2, 566, 52, 56, 25 ) );

	Restore();

	LoadTypeMap();

	m_spr = g_pResManager->GetSprite( "sprMainMenu" );
}

CMainMenu::~CMainMenu()
{
	SAFE_DELETE( m_gui );
	SAFE_DELETE( m_spr );
}

int CMainMenu::Update(float dt)
{
	return DoCommands( m_gui->Update( dt ) );
}

void CMainMenu::Render()
{
	m_spr->Render( 0, 0 );
	g_pGST->Gfx_SetTransform( 0, 0, 70, 110, 0, 0.5383333f, 0.5384615f );
	MAP.Render( true );
	g_pGST->Gfx_SetTransform();

	m_sprTab->Render( 509, 48 );
	m_gui->Render();
}

int CMainMenu::DoCommands(int nID)
{
	if( nID == BTN_START )
	{
		char buf[MAX_PATH];
		sprintf( buf, "%s\\map\\map_%02d_%03d.map", g_SysPath, m_nMapType,
			( (CGUIListbox *)m_gui->GetCtrl( LST_SELMAP ) )->GetSelectedItem() + 1 );
		g_pGame->SetMapName( buf );
	}
	else if( nID == LST_SELMAP )
	{
		char buf[MAX_PATH];
		sprintf( buf, "%s\\map\\map_%02d_%03d.map", g_SysPath, m_nMapType,
			( (CGUIListbox *)m_gui->GetCtrl( LST_SELMAP ) )->GetSelectedItem() + 1 );
		MAP.LoadMap( buf );

		m_gui->EnableCtrl( BTN_START, true );
	}
	else if( nID == BTN_MAPTYPE1 )
	{
		if(	m_nMapType != 1 )
		{
			m_nMapType = 1;
			Restore();
			( (CGUIListbox *)m_gui->GetCtrl( LST_SELMAP ) )->SetSelectedItem( -1 );
			LoadTypeMap();
		}
	}
	else if( nID == BTN_MAPTYPE2 )
	{
		if( m_nMapType != 2 )
		{
			m_nMapType = 2;
			Restore();
			( (CGUIListbox *)m_gui->GetCtrl( LST_SELMAP ) )->SetSelectedItem( -1 );
			LoadTypeMap();
		}
	}

	return nID;
}

void CMainMenu::LoadTypeMap()
{
	CGUIListbox *lst = ( (CGUIListbox *)m_gui->GetCtrl( LST_SELMAP ) );
	lst->Clear();

	char buf[MAX_PATH];
	char szName[80];
	sprintf( buf, "%s\\data\\ini\\MapList.ini", g_SysPath );
	g_pGST->System_SetState( GST_INIFILE, buf );

	sprintf( buf, "MapType%d", m_nMapType );
	int nMapNum = g_pGST->Ini_GetInt( "MapNum", buf, 0 );
	for( int i = 1; i <= nMapNum; i++ )
	{
		sprintf( szName, "map%03d", i );
		lst->AddItem( g_pGST->Ini_GetString( buf, szName, "" ) );
	}

	m_sprTab = g_pResManager->GetSprite( g_szString[m_nMapType - 1] );
}

void CMainMenu::Restore()
{
	( (CGUIListbox *)m_gui->GetCtrl( LST_SELMAP ) )->SetSelectedItem( -1 );
	m_gui->EnableCtrl( BTN_START, false );
	MAP.RestoreMap();
}