#include "game.h"
#include "sound.h"
#include "map.h"

CGame *g_pGame = NULL;

gstSprite *CGame::m_sprBg;
gstSprite *CGame::m_sprKick;
gstSprite *CGame::m_sprCursor;
gstAnimation *CGame::m_aniWin;
gstAnimation *CGame::m_aniDraw;
gstAnimation *CGame::m_aniPlayer;
gstAnimation *CGame::m_aniStart;
gstAnimation *CGame::m_aniNumber;
gstGUI *CGame::m_gui;

CGame::~CGame()
{
	SAFE_DELETE( m_gui );
	SAFE_DELETE( m_sprBg );
	SAFE_DELETE( m_sprKick );
	SAFE_DELETE( m_sprCursor );
	SAFE_DELETE( m_aniWin );
	SAFE_DELETE( m_aniDraw );
	SAFE_DELETE( m_aniPlayer );
	SAFE_DELETE( m_aniStart );
	SAFE_DELETE( m_aniNumber );
}

void CGame::Init()
{
	m_sprBg = g_pResManager->GetSprite( "sprBg" );
	m_sprKick = g_pResManager->GetSprite( "sprKick" );
	m_sprCursor = g_pResManager->GetSprite("sprCursor");
	
	FACTORY.LoadFixedPrototype( "data\\ini\\FixedItem.ini" );
	ROLE[0].Load( "data\\ini\\role1.ini" );
	ROLE[1].Load( "data\\ini\\role2.ini" );

	m_aniStart = g_pResManager->GetAnimation( "aniStart" );
	m_aniPlayer = g_pResManager->GetAnimation( "aniPlayer" );
	m_aniWin = g_pResManager->GetAnimation( "aniWin" );
	m_aniDraw = g_pResManager->GetAnimation( "aniDraw" );
	m_aniNumber = g_pResManager->GetAnimation( "aniNumber" );

	m_gui = new gstGUI();
	m_gui->AddCtrl( new CGUIButton( BTN_RETURN, 649, 555, 134, 36,
		g_pResManager->GetSprite( "sprReturnOver" ),
		g_pResManager->GetSprite( "sprReturnDown" ) ) );

	SOUND.Init();
}

int CGame::Update(float dt)
{
	MAP.Update(dt);

	for( int i = 0; i < ROLE_NUM; i++ )
	{
		ROLE[i].Update( dt );
	}

	switch( m_State )
	{
	case gsStart:
		m_fTime += dt;
		if( m_fTime > 2.0f )
		{
			m_State = gsRun;
			m_fTime = 0.0f;
		}
		break;
	case gsRun:
		ProcessInput();
		//m_pGUI->Update( dt );

		m_fTime += dt;
		if( m_fTime >= 1.0f )
		{
			m_fTime -= 1.0f;
			m_nTimeRemain--;
		}

		if (m_nTimeRemain <= 0)
		{
			m_nTimeRemain = 0;

			if( !ROLE[0].IsDead() && !ROLE[1].IsDead() )
			{
				m_State = gsEnd;
				m_EndType = etDraw;
				SOUND.StopMusic();
				SOUND.PlaySnd( sndDraw );
				ClearBomb();
				for( int i = 0; i < ROLE_NUM; ++i )
				{
					ROLE[i].Normal();
				}
			}
		}

		break;
	case gsEnd:
		m_fTime += dt;
		if( m_fTime > 2.5f )
		{
			return -1;
		}
	}
	
	return m_gui->Update( dt );
}

void CGame::Render()
{
	m_sprBg->Render(0,0);
	MAP.Render();

	if( m_State == gsStart )
	{
		RenderStart();
	}
	else if( m_State == gsEnd )
	{
		RenderEnd();
	}

	// 剩余时间
	RenderNum( 708, 43, m_nTimeRemain / 60 / 10 );
	RenderNum( 722, 43, m_nTimeRemain / 60 % 10 );
	RenderNum( 740, 43, m_nTimeRemain % 60 / 10 );
	RenderNum( 754, 43, m_nTimeRemain % 60 % 10 );

	// 赢的次数
	RenderNum( 746, 72, ROLE[0].GetWinCount() );
	RenderNum( 746, 244, ROLE[1].GetWinCount() );

	// 属性
	RenderNum( 746, 115, ROLE[0].GetAbility().nPopoMax );
	RenderNum( 746, 158, ROLE[0].GetAbility().nPopoPower );
	if( ROLE[0].GetAbility().Vehicle == vtNone )
	{
		RenderNum( 746, 201, ROLE[0].GetAbility().nSpeed );
	}
	else
	{
		RenderNum( 746, 201, VEHICLE_SPEED[ ROLE[0].GetAbility().Vehicle ] );
	}
	if( ROLE[0].GetAbility().bKick )
	{
		m_sprKick->Render( 709, 70 );
	}

	RenderNum( 746, 287, ROLE[1].GetAbility().nPopoMax );
	RenderNum( 746, 330, ROLE[1].GetAbility().nPopoPower );
	if( ROLE[1].GetAbility().Vehicle == vtNone )
	{
		RenderNum( 746, 373, ROLE[1].GetAbility().nSpeed );
	}
	else
	{
		RenderNum( 746, 373, VEHICLE_SPEED[ ROLE[1].GetAbility().Vehicle ] );
	}
	if( ROLE[1].GetAbility().bKick )
	{
		m_sprKick->Render( 709, 242 );
	}

	m_gui->Render();
}

void CGame::Start()
{
	ROLE[0].New();
	ROLE[1].New();
	ROLE[0].Start();
	ROLE[1].Start();

	ROLE[0].SetPosition( 1, 1 );
	ROLE[1].SetPosition( 13, 15 );

	SOUND.PlayBgMsc( sndPlayBg );
	SOUND.PlaySnd( sndStart );

	m_nTimeRemain = 240;
	m_State = gsStart;
}

void CGame::LoadMap()
{
	if( strcmp( m_szMapName, "" ) == 0 ) return;
	MAP.LoadMap( m_szMapName );
}

void CGame::ProcessInput()
{
	if( g_pGST->Input_GetKeyState( GSTK_A ) )
	{
		ROLE[0].Move( drLeft );
	}
	else if( g_pGST->Input_GetKeyState( GSTK_D ) )
	{
		ROLE[0].Move( drRight );
	}
	else if( g_pGST->Input_GetKeyState( GSTK_W ) )
	{
		ROLE[0].Move( drUp );
	}
	else if( g_pGST->Input_GetKeyState( GSTK_S ) )
	{
		ROLE[0].Move( drDown );
	}
	else if( g_pGST->Input_GetKeyState( GSTK_T ) )
	{
		ROLE[0].LayPopo();
	}

	if( g_pGST->Input_GetKeyState( GSTK_LEFT ) )
	{
		ROLE[1].Move( drLeft );
	}
	else if( g_pGST->Input_GetKeyState( GSTK_RIGHT ) )
	{
		ROLE[1].Move( drRight );
	}
	else if( g_pGST->Input_GetKeyState( GSTK_UP ) )
	{
		ROLE[1].Move( drUp );
	}
	else if( g_pGST->Input_GetKeyState( GSTK_DOWN ) )
	{
		ROLE[1].Move( drDown );
	}
	else if( g_pGST->Input_GetKeyState( GSTK_PGDN ) )
	{
		ROLE[1].LayPopo();
	}
}

void CGame::RoleOnDie(CRole *pRole)
{
	for( int i = 0; i < ROLE_NUM; i++ )
	{
		if( &ROLE[i] != pRole )
		{
			ROLE[i].Normal();
		}
	}
}

void CGame::RoleDead(CRole *pRole)
{
	for( int i = 0; i < ROLE_NUM; i++ )
	{
		if( &ROLE[i] != pRole )
		{
			ROLE[i].Win();
		}
	}
	
	ClearBomb();
	SOUND.StopMusic();
	SOUND.PlaySnd( sndWin );
	m_State = gsEnd;
	if (&ROLE[0] == pRole)
	{
		m_EndType = etSecWin;
	}
	else
	{
		m_EndType = etFstWin;
	}
}

void CGame::ClearBomb()
{
	for( int y = 1; y <= CELL_Y; y++ )
	{
		for( int x = 1; x <= CELL_X; x++ )
		{
			if( MAP[y][x].Type == itPopo )
			{
				SAFE_DELETE( MAP[y][x].pItem );
				MAP[y][x].Type = itNone;
			}
		}
	}
}

void CGame::RenderStart()
{
	m_aniStart->SetFrame( 0 );
	m_aniStart->Render( 187, 73 - max( 0, m_fTime * 25 - 40 ) * 20 );
	m_aniStart->SetFrame( 1 );
	m_aniStart->Render( 280, 73 - max( 0, m_fTime * 25 - 43 ) * 20 );
	m_aniStart->SetFrame( 2 );
	m_aniStart->Render( 385, 73 - max( 0, m_fTime * 25 - 46 ) * 20 );
}

void CGame::RenderEnd()
{
	if( m_EndType == etDraw )
	{
		m_aniDraw->SetFrame( 0 );
		m_aniDraw->Render( 206, 115 );
		m_aniDraw->SetFrame( 1 );
		m_aniDraw->Render( 300, 115 );
		m_aniDraw->SetFrame( 2 );
		m_aniDraw->Render( 385, 115 );
	}
	else
	{
		m_aniWin->SetFrame( 0 );
		m_aniWin->Render( 206, 115 );
		m_aniWin->SetFrame( 1 );
		m_aniWin->Render( 300, 115 );
		m_aniWin->SetFrame( 2 );
		m_aniWin->Render( 385, 115 );
		if (m_EndType == etFstWin)
		{
			m_aniPlayer->SetFrame( 0 );
			m_aniPlayer->Render( 237, 86 );
		}
		else
		{
			m_aniPlayer->SetFrame( 1 );
			m_aniPlayer->Render( 237, 86 );
		}
	}
}

void CGame::RenderNum(float x,float y,int num)
{
	int digit[10];
	memset( digit, -1, sizeof( digit ) );
	int i = 0;
	do
	{		
		digit[i++] = num % 10;
		num /= 10;
	}while( num != 0 );

	float fx = x;
	float fw = m_aniNumber->GetWidth();
	for( i = 9; i >= 0; i--)
	{
		if( digit[i] == -1 ) continue;
		m_aniNumber->SetFrame( digit[i] );
		m_aniNumber->Render( fx, y );
		fx += fw;
	}
}