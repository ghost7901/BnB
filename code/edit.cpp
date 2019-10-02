#include "edit.h"
#include "gui.h"
#include "map.h"
//#include "script.h"
//#include "GUI/gstFontCN.h"
//#include "GUI/gui.h"
//#include "GUI/guiimage.h"
//#include "tinyxml.h"

const int ITEM_BTN_X = 660;
const int ITEM_BTN_Y = 184;
const int ITEM_BTN_SIZE_X = 42;
const int ITEM_BTN_SIZE_Y = 31;
const int ITEM_BTN_SPACE_X = 26;
const int ITEM_BTN_SPACE_Y = 12;
const int ITEM_BTN_MAX_X = ITEM_BTN_X + ITEM_BTN_SIZE_X * 2 + ITEM_BTN_SPACE_X;
const int ITEM_BTN_MAX_Y = ITEM_BTN_Y + ITEM_BTN_SIZE_Y * 6 + ITEM_BTN_SPACE_Y * 5;

const int GROUND_BTN_X = 69;
const int GROUND_BTN_Y = 569;
const int GROUND_BTN_SIZE_X = 37;
const int GROUND_BTN_SIZE_Y = 29;
const int GROUND_BTN_SPACE = 3;
const int GROUND_BTN_MAX_X = GROUND_BTN_X + GROUND_BTN_SIZE_X * 13 + GROUND_BTN_SPACE * 12;

CEdit *g_pEdit = NULL;

CEdit::CEdit() : m_nItemX(-1), m_nItemY(-1), m_nSelID(-1), m_nSelType(0), m_nMapType(0), m_nPageNum(0)
, m_bShowGrid(true)
{
	//SCRIPT.SetFile("work/aa.lua");
	//int aa=SCRIPT.GetValue("width");
	//int bb=SCRIPT.GetValue("height");
	//int cc=SCRIPT.GetValue("back","r");
	//int dd=SCRIPT.GetValue("back","g");
	//int ee=SCRIPT.GetValue("back","b");
	//char ff[80];
	//strcpy( ff, SCRIPT.GetString("background","aa") );
	//SCRIPT.SetValue( "width", 99 );
	m_sprHighlight = new gstSprite( 0, 0, 0, 0, 0 );
	m_sprHighlight->SetColor( ARGB( 150, 8, 182, 255 ) );

	m_fntDigit = g_pResManager->GetFont( "fntDigit" );
//	char buf[MAX_PATH];
//	sprintf( buf, "%s\\%s", g_SysPath, "data\\fonts\\fontCN_宋体_12_Regular.bin" );
//	m_fntcn = new gstFontCN( buf );

	m_gui = new gstGUI();

	m_gui->AddCtrl( new CGUIButton( BTN_RETURN, 649, 555, 134, 36,
		g_pResManager->GetSprite( "sprReturnOver" ),
		g_pResManager->GetSprite( "sprReturnDown" ) ) );

	CGUIListbox *lst =  new CGUIListbox( LST_MAPTYPE, 642, 58, 129, 71, 0,
		0, 0, ARGB( 100, 8, 182, 255 ) );
	lst->SetRowHigh( 12.0f );
	lst->SetRowSpace( 6.0f );
	lst->AddItem("");
	lst->AddItem("");
	lst->SetSelectedItem( -1 );
	m_gui->AddCtrl( lst );

	m_gui->AddCtrl( new CGUIButton( BTN_NEW, 652, 471, 61, 21, 0, 0 ) );
	m_gui->AddCtrl( new CGUIButton( BTN_OPEN, 716, 471, 61, 21, 0, 0 ) );
	m_gui->AddCtrl( new CGUIButton( BTN_SAVE, 652, 501, 61, 21, 0, 0 ) );
	m_gui->AddCtrl( new CGUIButton( BTN_SAVE_AS, 716, 501, 61, 21, 0, 0 ) );

	CGUIButton *btn = new CGUIButton( BTN_PAGE_PREV, 662, 152, 43, 21,
		0,
		g_pResManager->GetSprite( "sprPrevBtnDown" ),
		g_pResManager->GetSprite( "sprPrevBtnDisable" ) );
	btn->bEnabled = false;
	m_gui->AddCtrl( btn );
	btn = new CGUIButton( BTN_PAGE_NEXT, 727, 152, 43, 21,
		0,
		g_pResManager->GetSprite( "sprNextBtnDown" ),
		g_pResManager->GetSprite( "sprNextBtnDisable" ) );
	btn->bEnabled = false;
	m_gui->AddCtrl( btn );

	m_gui->AddCtrl( new CGUICheckButton( CHECK_GRID, 638, 121, 15, 15,
		g_pResManager->GetSprite( "sprCheckDown" ),
		g_pResManager->GetSprite( "sprChecked" ) ) );
	

	m_sprBg = g_pResManager->GetSprite( "sprMapEdit" );
	//TiXmlDocument doc;
	//if( !doc.LoadFile( "work/xml/MapEdit.xml" ) )
	//{
	//	MessageBox( NULL, "载入失败！", "MSG", MB_OK );
	//}
	//TiXmlElement *pElem;
	//pElem = doc.FirstChild( "Interface" )->FirstChild( "GUI" )->FirstChildElement( "Image" );

	//GUIImage *img = new GUIImage( pElem );
	//img->SetParent( m_gui );
	//m_gui->AddCtrl( img );

	m_sprGroundBtn = NULL;
	m_sprItemBtn = NULL;

	New();
}

CEdit::~CEdit()
{
	SAFE_DELETE( m_sprHighlight );
//	SAFE_DELETE( m_gui );
//	SAFE_DELETE( m_sprBg );
}

int CEdit::Update(float dt)
{
	bool bLButtonKeyDown = g_pGST->Input_KeyDown( GSTK_LBUTTON );
	bool bLButtonDown = g_pGST->Input_GetKeyState( GSTK_LBUTTON );
	bool bRButtonDown = g_pGST->Input_GetKeyState( GSTK_RBUTTON );

	float x, y;
	g_pGST->Input_GetMousePos( &x, &y );

	if( bLButtonKeyDown && m_nMapType )
	{
		if( x >= ITEM_BTN_X && x <= ITEM_BTN_MAX_X && y >= ITEM_BTN_Y && y <= ITEM_BTN_MAX_Y ) //在Item范围
		{
			int nItemCX, nItemCY, nItemX, nItemY;
			nItemCY = int( y - ITEM_BTN_Y ) / ( ITEM_BTN_SIZE_Y + ITEM_BTN_SPACE_Y );
			nItemCX = int( x - ITEM_BTN_X ) / ( ITEM_BTN_SIZE_X + ITEM_BTN_SPACE_X );
			nItemX = ITEM_BTN_X + nItemCX * ( ITEM_BTN_SIZE_X + ITEM_BTN_SPACE_X );
			nItemY = ITEM_BTN_Y + nItemCY * ( ITEM_BTN_SIZE_Y + ITEM_BTN_SPACE_Y );

			if( x <= nItemX + ITEM_BTN_SIZE_X && y <= nItemY + ITEM_BTN_SIZE_Y )
			{				
				int nSelID = m_nPageNum * 12 + nItemCY * 2 + nItemCX + 1; 
				if( nSelID <= FACTORY.GetItemNum() )
				{
					m_nItemX = nItemX;
					m_nItemY = nItemY;
					m_nSelID = nSelID;
					if( m_nSelType == 0 || m_nSelType == 2 )
					{
						m_nSelType = 1;
						m_sprHighlight->SetTextureRect( 0, 0, ITEM_BTN_SIZE_X, ITEM_BTN_SIZE_Y );
					}
				}				
			}
		}
		else if( x >= GROUND_BTN_X && x <= GROUND_BTN_MAX_X && y >= GROUND_BTN_Y && y <= GROUND_BTN_Y + GROUND_BTN_SIZE_Y )  //在Ground范围
		{
			int nItemCX, nItemX, nItemY;
			nItemCX = int( x - GROUND_BTN_X ) / ( GROUND_BTN_SIZE_X + GROUND_BTN_SPACE );
			nItemX = GROUND_BTN_X + nItemCX * ( GROUND_BTN_SIZE_X + GROUND_BTN_SPACE );
			nItemY = GROUND_BTN_Y;

			if( x <= nItemX + GROUND_BTN_SIZE_X )
			{				
				int nSelID = nItemCX + 1;
				if( nSelID <= FACTORY.GetGroundNum() )
				{
					m_nItemX = nItemX;
					m_nItemY = nItemY;
					m_nSelID = nSelID;
					if( m_nSelType == 0 || m_nSelType == 1 )
					{
						m_nSelType = 2;
						m_sprHighlight->SetTextureRect( 0, 0, GROUND_BTN_SIZE_X, GROUND_BTN_SIZE_Y );
					}
				}
			//	MessageBox(NULL,"aa","MSG",MB_OK);
			}
		}
	}
	
	if( ( bLButtonDown || bRButtonDown ) &&
		x >= SCENE_LEFT && x <= SCENE_LEFT + CELL_SIZE * CELL_X &&
		y >= SCENE_TOP && y <= SCENE_TOP + CELL_SIZE * CELL_Y )  //在Cell范围
	{		
		int nItemCX, nItemCY;
		nItemCY = int( y - SCENE_TOP ) / CELL_SIZE + 1;
		nItemCX = int( x - SCENE_LEFT ) / CELL_SIZE + 1;

		enum Type { Medium = 1, MediumAni, Large, Box, Bush, Tent };
		if( bLButtonDown && m_nSelID != -1 )
		{
			if( m_nSelType == 2 )
			{
				MAP[nItemCY][nItemCX].Ground = m_nSelID;
			}
			else if( m_nSelType == 1 &&
				MAP[nItemCY][nItemCX].Ground != itNone && 
				MAP[nItemCY][nItemCX].Type == itNone && 
				MAP[nItemCY][nItemCX].Blind == btNone )
			{				
				if( m_nItemType[m_nSelID] == Large )
				{
					if( nItemCX > CELL_X - 2 ||
						MAP[nItemCY][nItemCX + 1].Type != itNone ||
						MAP[nItemCY][nItemCX + 1].Blind != btNone ||
						MAP[nItemCY][nItemCX + 1].Ground == itNone ||
						MAP[nItemCY][nItemCX + 2].Type != itNone ||
						MAP[nItemCY][nItemCX + 2].Blind != btNone ||
						MAP[nItemCY][nItemCX + 2].Ground == itNone )
					{
						return 0;
					}
					MAP[nItemCY][nItemCX + 1].Type = itFirm;
					MAP[nItemCY][nItemCX + 2].Type = itFirm;
				}

				if( m_nItemType[m_nSelID] <= Box )  //
				{
					MAP[nItemCY][nItemCX].Type = m_nType[m_nSelID];
					MAP[nItemCY][nItemCX].ID = m_nSelID;
					//MAP[nItemCY][nItemCX].Blind = btNone;/////
					MAP[nItemCY][nItemCX].pItem = FACTORY.MakeMapItem( m_nSelID );
					MAP[nItemCY][nItemCX].pItem->SetPosition( nItemCY, nItemCX );
				}
				else
				{
					MAP[nItemCY][nItemCX].Type = itNone;
					MAP[nItemCY][nItemCX].ID = m_nSelID;
					MAP[nItemCY][nItemCX].Blind = m_nItemType[m_nSelID] - 3;
					MAP[nItemCY][nItemCX].pBlindage = ( CBlindage * )FACTORY.MakeMapItem( m_nSelID );
					MAP[nItemCY][nItemCX].pBlindage->SetPosition( nItemCY, nItemCX );
				}			
			}
		}

		if( bRButtonDown )
		{
			if( m_nItemType[ MAP[nItemCY][nItemCX].ID ] == Large )
			{
				MAP[nItemCY][nItemCX + 1].Type = itNone;
				MAP[nItemCY][nItemCX + 2].Type = itNone;
			}
			MAP[nItemCY][nItemCX].Type = itNone;
			MAP[nItemCY][nItemCX].Blind = btNone;
			SAFE_DELETE( MAP[nItemCY][nItemCX].pItem );
			SAFE_DELETE( MAP[nItemCY][nItemCX].pBlindage );
		}
	}

	return DoCommands( m_gui->Update( dt ) );
}

void CEdit::Render()
{
	m_sprBg->Render( 0, 0 );
	if( m_sprGroundBtn ) m_sprGroundBtn->Render( GROUND_BTN_X, GROUND_BTN_Y );
	if( m_sprItemBtn ) m_sprItemBtn->Render( ITEM_BTN_X, ITEM_BTN_Y );
	MAP.Render( true );
	if( m_nSelID != -1 )
	{
		m_sprHighlight->Render( m_nItemX, m_nItemY );
	}

	if( strcmp( m_szSaveName, "" ) == 0 )
	{
		m_fntDigit->Render( 140, 7, GSTTEXT_LEFT, "-- Unnamed" );
		//m_fntcn->Render( 140, 7, "-- 未命名我们是中国人！！！" );
	}
	else
	{
		m_fntDigit->printf( 140, 7, GSTTEXT_LEFT, "-- %s", m_szSaveName );
		//m_fntcn->printf( 140, 7, "-- %s", m_szSaveName );
	}

	if( m_bShowGrid ) ShowGrid();

	ShowCoordinate();

	m_gui->Render();	
}

int CEdit::DoCommands(int nID)
{
	switch( nID )
	{
	case BTN_NEW:
		New();
		break;
	case BTN_OPEN:		
		OpenMap();
		break;
	case BTN_SAVE:
		if( strcmp( m_szSaveName, "" ) == 0 )
		{
			SaveAs();
		}
		else
		{
			MAP.SaveMap( m_szSaveName, m_nMapType );
		}
		break;
	case BTN_SAVE_AS:
		SaveAs();
		break;
	case BTN_PAGE_NEXT:
		if( m_nPageNum < FACTORY.GetItemNum() / 12 )
		{
			m_nPageNum++;
			m_nSelID = -1;
			m_gui->EnableCtrl( BTN_PAGE_PREV, true );
			if( m_nPageNum == FACTORY.GetItemNum() / 12 )
			{
				m_gui->EnableCtrl( BTN_PAGE_NEXT, false );

				m_sprItemBtn = g_pResManager->GetSprite( "sprType2ItemBtn2" );/////待改
			}
		}
	//	MessageBox(NULL,"aa","MSG",MB_OK);
		break;
	case BTN_PAGE_PREV:
		if( m_nPageNum > 0 )
		{
			m_nPageNum--;
			m_nSelID = -1;
			m_gui->EnableCtrl( BTN_PAGE_NEXT, true );
			if( m_nPageNum == 0 )
			{
				m_gui->EnableCtrl( BTN_PAGE_PREV, false );

				m_sprItemBtn = g_pResManager->GetSprite( "sprType2ItemBtn1" );////待改
			}
		}
		break;
	case LST_MAPTYPE:
	//	int nOldType = m_nMapType;
		NewMapType( ( (CGUIListbox *)m_gui->GetCtrl( LST_MAPTYPE ) )->GetSelectedItem() + 1 );
		New();
		///////
		break;
	case CHECK_GRID:
		m_bShowGrid = ( (CGUICheckButton *)m_gui->GetCtrl( CHECK_GRID ) )->GetState();
		break;
	}

	return nID;
}

void CEdit::NewMapType(int nNewType)
{
	if( m_nMapType != nNewType )
	{
		m_nMapType = nNewType;
		
		if( m_nMapType == 1 )///////待改
		{
			m_sprGroundBtn = g_pResManager->GetSprite( "sprType1GroundBtn" );
			m_sprItemBtn = g_pResManager->GetSprite( "sprType1ItemBtn" );
		}
		else if( m_nMapType == 2 )
		{
			m_sprGroundBtn = g_pResManager->GetSprite( "sprType2GroundBtn" );
			m_sprItemBtn = g_pResManager->GetSprite( "sprType2ItemBtn1" );
		}

		char szBuffer[55];
		sprintf( szBuffer, "data\\ini\\maptype%d.ini", m_nMapType );
		LoadMapType( szBuffer );		

		m_nPageNum = 0;
		m_gui->EnableCtrl( BTN_PAGE_PREV, false );
		m_gui->EnableCtrl( BTN_PAGE_NEXT, FACTORY.GetItemNum() > 12 );		

		m_nSelID = -1;
	}
}

void CEdit::New()
{
	MAP.RestoreMap();
	for( int i = 0; i <= CELL_X_ALL; i++ )
	{
		MAP[0][i].Type = MAP[CELL_Y_ALL - 1][i].Type = itFirm;
		if( i < CELL_Y_ALL )
		{
			MAP[i][0].Type = MAP[i][CELL_X_ALL - 1].Type = itFirm;
		}
	}

	strcpy( m_szSaveName, "" );
}

void CEdit::LoadMapType(const char *szIniFile)
{
	char buf[MAX_PATH];
	sprintf(buf, "%s\\%s", g_SysPath, szIniFile);
	FACTORY.LoadMapPrototype( buf );
	char szSection[80];
//	m_nItemNum = g_pGST->Ini_GetInt( "Setup", "ItemNum", 0 );
	ZeroMemory( m_nType, sizeof( m_nType ) );
	ZeroMemory( m_nItemType, sizeof( m_nItemType ) );
	for( int i = 1; i <= FACTORY.GetItemNum(); i++ )
	{
		sprintf( szSection, "Item%d", i );
		m_nType[i] = g_pGST->Ini_GetInt( szSection, "MainBlock", 0 );
		m_nItemType[i] = g_pGST->Ini_GetInt( szSection, "Type", 0 );
	}
}

void CEdit::SaveAs()
{
	char filename[MAX_PATH] = "";
	OPENFILENAME ofn;
	ZeroMemory( &ofn, sizeof( OPENFILENAME ) );
	ofn.lStructSize = sizeof( OPENFILENAME );
	ofn.hwndOwner = g_pGST->System_GetState( GST_HWND );
	ofn.lpstrFilter = "*.map\0*.map\0\0";
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrDefExt = "map";
	ofn.lpstrFile = filename;

	if( GetSaveFileName( &ofn ) )
	{
		MAP.SaveMap( filename, m_nMapType );
		strcpy( m_szSaveName, filename );


		char buf[MAX_PATH];
		sprintf( buf, "%s\\data\\ini\\MapList.ini", g_SysPath );
		g_pGST->System_SetState( GST_INIFILE, buf );

		sprintf( buf, "MapType%d", m_nMapType );
		int nMapNum = g_pGST->Ini_GetInt( "MapNum", buf, 0 ) + 1;
		g_pGST->Ini_SetInt( "MapNum", buf, nMapNum );

		char szName[80];
		char szString[80];
		sprintf( szName, "map%03d", nMapNum );
		sprintf( szString, "%s.map", szName );
		g_pGST->Ini_SetString( buf, szName, szString );
	}
}

void CEdit::OpenMap()
{
	char filename[MAX_PATH] = "";
	OPENFILENAME ofn;
	ZeroMemory( &ofn, sizeof( OPENFILENAME ) );
	ofn.lStructSize = sizeof( OPENFILENAME );
	ofn.hwndOwner = g_pGST->System_GetState( GST_HWND );
	ofn.lpstrFilter="*.map\0*.map\0\0";
	ofn.nMaxFile = MAX_PATH;
	ofn.nMaxFileTitle = MAX_PATH;
	ofn.lpstrDefExt = "map";
	ofn.lpstrFile = filename;

	if( GetOpenFileName( &ofn ) )
	{
		int nNewType = MAP.LoadMap( filename );
		NewMapType( nNewType );
		( (CGUIListbox *)m_gui->GetCtrl( LST_MAPTYPE ) )->SetSelectedItem( nNewType - 1 );
		strcpy( m_szSaveName, filename );
	}
}

void CEdit::ShowGrid()
{
	float x, y;
	for( int cy = 0; cy <= CELL_Y; cy++ )
	{
		y = SCENE_TOP + cy * CELL_SIZE;
		g_pGST->Gfx_RenderLine( SCENE_LEFT, y, SCENE_LEFT + CELL_SIZE * CELL_X, y, ARGB( 155, 0, 190, 255 ) );
	}
	
	for( int cx = 0; cx <= CELL_X; cx++ )
	{
		x = SCENE_LEFT + cx * CELL_SIZE;
		g_pGST->Gfx_RenderLine( x, SCENE_TOP, x, SCENE_TOP + CELL_SIZE * CELL_Y, ARGB( 155, 0, 190, 255 ) );
	}
}

void CEdit::ShowCoordinate()
{
	float x, y;
	g_pGST->Input_GetMousePos( &x, &y );
	if( x < SCENE_LEFT || y < SCENE_TOP ) return;

	int nItemCX, nItemCY;
	nItemCY = int( y - SCENE_TOP ) / CELL_SIZE + 1;
	nItemCX = int( x - SCENE_LEFT ) / CELL_SIZE + 1;

	if( nItemCX >= 1 && nItemCX <= CELL_X && nItemCY >=1 && nItemCY <= CELL_Y )
	{
		m_fntDigit->printf( 730, 123, GSTTEXT_LEFT, "%d", nItemCX );
		m_fntDigit->printf( 765, 123, GSTTEXT_LEFT, "%d", nItemCY );

		//m_fntDigit->printf( x, y - 10, GSTTEXT_LEFT, "%d", nItemCX );
		//m_fntDigit->printf( x + 20, y - 10, GSTTEXT_LEFT, "%d", nItemCY );
	}
}