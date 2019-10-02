#include "map.h"
#include <algorithm>

bool Greater(int r1,int r2)
{
	if( ROLE[r1].GetY() != ROLE[r2].GetY() )
	{
		return ROLE[r1].GetY() < ROLE[r2].GetY();
	}
	else
	{
		return ROLE[r1].GetOffsetY() < ROLE[r2].GetOffsetY();
	}
}

CMap::CMap() : m_sprExplosion(NULL)// m_aniExplosion(NULL)
{
	//m_aniExplosion = g_pResManager->GetAnimation( "aniExplosion" );
	m_sprExplosion = g_pResManager->GetSprite( "sprExplosion" );

	for( int i = 0; i < ROLE_NUM; i++ )
	{
		m_vRoleSeq[i] = i;
	}
}

CMap::~CMap()
{
	ReleaseMap();
//	SAFE_DELETE( m_aniExplosion );
//	SAFE_DELETE( m_sprExplosion );
}

CMap &CMap::Instance()
{
	static CMap instance;
	return instance;
}

int CMap::LoadMap(const char *szFileName)
{	
	FILE *pFile;
	//ZeroMemory( m_vCell, sizeof( m_vCell ) );
	if( strstr( szFileName, ":" ) )
	{
		pFile = fopen( szFileName, "rb" );
	}
	else
	{
		char buf[MAX_PATH];
		sprintf( buf, "%s\\%s", g_SysPath, szFileName );

		pFile = fopen( buf, "rb" );
	}

	if( !pFile )
	{
		printf( "找不到该地图文件!\n" );
		return 0;
	}

	int nMapType;
	fread( &nMapType, sizeof( nMapType ), 1, pFile );
	char szBuffer[55];
	sprintf( szBuffer, "data\\ini\\maptype%d.ini", nMapType );
	FACTORY.LoadMapPrototype( szBuffer );	

	Cell *pCell = m_vCell;

	// 释放原地图块内容
	ReleaseMap();

	// 读取地图的每块内容
	

	for (int i = 0; i < CELL_NUM; i++)
	{
		// 读取属性
		fread( pCell, 4 * sizeof( BYTE ), 1, pFile );

		// 加载物体
		if (pCell->Blind >= btBush)
		{
			pCell->pBlindage = (CBlindage *)FACTORY.MakeMapItem( pCell->ID );
			pCell->pBlindage->SetPosition( i / CELL_X_ALL, i % CELL_X_ALL );
		}
		else if (pCell->ID > 0)
		{
			pCell->pItem = FACTORY.MakeMapItem( pCell->ID );
			pCell->pItem->SetPosition( i / CELL_X_ALL, i % CELL_X_ALL );
		}

		// 初始化爆炸效果
		pCell->Explosion.Status = 0;

		// 进入下一格
		pCell++;
	}
	fclose( pFile );

	return nMapType;
}

void CMap::Update(float dt)
{
	for(int i = 0; i < CELL_NUM; i++)
	{
		if( m_vCell[i].pItem != NULL ) m_vCell[i].pItem->Update( dt );
		if( m_vCell[i].pBlindage != NULL ) m_vCell[i].pBlindage->Update( dt );
	}

//	std::sort( m_vRoleSeq, m_vRoleSeq + ROLE_NUM, Greater );

	// 更新爆炸效果
#define EXP m_vCell[i].Explosion

	for( int i = 0; i < CELL_NUM; i++ )
	{
		if( EXP.Status == 1 )
		{
			EXP.Timer--;
			if( EXP.Timer == 0 )
			{
				EXP.Status++;
			}
		}
		else if( EXP.Status > 1 )
		{			
			if( EXP.Dr == drMiddle && EXP.Status >= 12 )
			{
				EXP.Status = 0;
			}
			else if( EXP.Status >= 20 )
			{
				EXP.Status = 0;
				if( EXP.Gift != 0 )
				{
					m_vCell[i].pItem = FACTORY.MakeFixedItem( EXP.Gift );
					m_vCell[i].pItem->SetPosition( i / CELL_X_ALL, i % CELL_X_ALL );
				}
			}
			else
			{
				EXP.Status++;
			}
		}
	}

#undef EXP
}

void CMap::Render(bool bEdit) const
{
	RenderGround();

#define CELL m_vCell[cy * CELL_X_ALL + cx]

	for( int cy = 1; cy <= CELL_Y; cy++ )
	{
		for( int cx = CELL_X; cx>=1; cx-- )
		{
			if( CELL.Type >= itFirm && CELL.Type <= itBox && CELL.Blind == btNone )
			{
				if(CELL.pItem) CELL.pItem->Render();
			}
			else if( CELL.Blind >= btBush )
			{
				CELL.pBlindage->Render();
			}
		}
		
		for( int cx = 1; cx <= CELL_X; cx++ )
		{
			if( CELL.Type == itGift || CELL.Type == itPopo && CELL.Blind == btNone )
			{
				CELL.pItem->Render();
			}
		}

		if( !bEdit )
		{
			for( int cx = 1; cx <= CELL_X; cx++ )
			{
				if( MAP[cy][cx].Explosion.Status > 1 && MAP[cy][cx].Blind != btTent )
				{
					RenderExplosion( cy, cx );
				}
			}

			for( int j = 0; j < ROLE_NUM; j++ )
			{
				//int nX = ROLE[m_vRoleSeq[j]].GetX();
				//int nY = ROLE[m_vRoleSeq[j]].GetY();
				int nX = ROLE[j].GetX();
				int nY = ROLE[j].GetY();
				if (nY == cy && m_vCell[cy * CELL_X_ALL + nX].Blind == btNone)
				{
					//ROLE[m_vRoleSeq[j]].Render();
					ROLE[j].Render();
				}
			}
		}
	}

#undef CELL	
}

void CMap::RenderGround() const
{
#define CELL m_vCell[cy * CELL_X_ALL + cx]

	for( int cy = 1; cy <= CELL_Y; cy++ )
	{
		for( int cx = 1; cx <= CELL_X; cx++ )
		{
			if( !CELL.Ground ) continue;
		//	m_aniGround->SetFrame( CELL.Ground - 1 );
		//	m_aniGround->Render( SCENE_LEFT + ( cx - 1 ) * CELL_SIZE, SCENE_TOP + ( cy - 1 ) * CELL_SIZE );
			FACTORY.MakeMapGround( CELL.Ground - 1 )->Render( SCENE_LEFT + ( cx - 1 ) * CELL_SIZE, SCENE_TOP + ( cy - 1 ) * CELL_SIZE );
		}
	}

#undef CELL
}

void CMap::RenderExplosion(int nCellY,int nCellX) const
{
#define EXP m_vCell[nCellY * CELL_X_ALL + nCellX].Explosion

	int col;
	int x = ( nCellX - 1 ) * CELL_SIZE + SCENE_LEFT;
	int y = ( nCellY - 1 ) * CELL_SIZE + SCENE_TOP;

	if( EXP.Dr == drMiddle )
	{
		col = EXP.Status % 4;
	}
	else
	{
		if( EXP.IsHead == 0 )
		{
			if( EXP.Status == 2 )
			{
				col = 0;
			}
			else if( EXP.Status < EXP.Dead )	// Dead min = 12
			{
				col = ( EXP.Status - 3 ) / 3 % 2 + 3;
			}
			else
			{
				col = EXP.Status - 7;
			}
		}
		else
		{
			if( EXP.Status <= 11 )
			{
				col = ( EXP.Status - 2 ) / 3;
			}
			else
			{
				col = EXP.Status - 7;
			}
		}
	}

	//m_aniExplosion->SetFrame( EXP.Dr * 14 + col );
	//m_aniExplosion->Render( x, y );
	m_sprExplosion->SetTextureRect( 20 + col * 40, 344 + EXP.Dr * 40, 40, 40 );
	m_sprExplosion->Render( x, y );

#undef EXP
}

void CMap::ReleaseMap()
{
	for(int i = 0; i < CELL_NUM; i++)
	{
		SAFE_DELETE( m_vCell[i].pItem );
		SAFE_DELETE( m_vCell[i].pBlindage );
	}
}

void CMap::SaveMap(const char *szFileName,int nMapType)
{
//	ZeroMemory( m_vCell, sizeof( m_vCell ) );
//
//	for( int i = 0; i <= CELL_X_ALL; i++ )
//	{
//		MAP[0][i].Type = MAP[CELL_Y_ALL - 1][i].Type = itFirm;
//		if( i < CELL_Y_ALL )
//		{
//			MAP[i][0].Type = MAP[i][CELL_X_ALL - 1].Type = itFirm;
//		}
//	}
//
//#define CELL m_vCell[cy * CELL_X_ALL + cx]
//
//	for(int cy = 1; cy <= CELL_Y; cy++)
//	{
//		for(int cx = 1; cx <= CELL_X; cx++)
//		{
//			if((cx % 2 != 0 && cy % 2 != 0) || (cx % 2 ==0 && cy %2 ==0)) CELL.Ground = 1; else CELL.Ground = 2;
//			CELL.Type = itNone;////
//		}
//	}
//
//#undef CELL

	FILE *pFile;
	if( strstr( szFileName, ":" ) )
	{
		pFile = fopen( szFileName, "wb" );
	}
	else
	{
		char buf[MAX_PATH];
		sprintf( buf, "%s\\%s", g_SysPath, szFileName );

		pFile = fopen( buf, "wb" );
	}

	if( !pFile )
	{
		printf( "无法保存地图文件!\n" );
		return;
	}
	
	fwrite( &nMapType, sizeof(nMapType), 1, pFile );

	Cell *pCell = m_vCell;
	for (int i = 0; i < CELL_NUM; i++)
	{
		// 读取属性
		fwrite( pCell, 4 * sizeof( BYTE ), 1, pFile );

		// 进入下一格
		pCell++;
	}
	fclose( pFile );
}

void CMap::RestoreMap()
{
	ReleaseMap();
	ZeroMemory( m_vCell, sizeof( m_vCell ) );	
}