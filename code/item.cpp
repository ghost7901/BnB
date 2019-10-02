#include "item.h"
#include "map.h"
#include "sound.h"

void MoveAhead(const int nOldX,const int nOldY,int *pNewX,int *pNewY,Direction dir)
{
	switch( dir )
	{
	case drUp:
		*pNewY = nOldY - 1;
		*pNewX = nOldX;
		break;
	case drDown:
		*pNewY = nOldY + 1;
		*pNewX = nOldX;
		break;
	case drLeft:
		*pNewX = nOldX - 1;
		*pNewY = nOldY;
		break;
	case drRight:
		*pNewX = nOldX + 1;
		*pNewY = nOldY;
		break;
	}
}

bool MiJi(int *nID,CPopo *popo)
{
	if( popo->GetOwner() != &ROLE[0] ) return false;
	if( g_pGST->Input_GetKeyState( GSTK_Q ) )
	{
		*nID = 10;
		return true;
	}
	else if( g_pGST->Input_GetKeyState( GSTK_E ) )
	{
		*nID = 5;
		return true;
	}
	else if( g_pGST->Input_GetKeyState( GSTK_Z ) )
	{
		*nID = 4;
		return true;
	}
	else if( g_pGST->Input_GetKeyState( GSTK_X ) )
	{
		*nID = 1;
		return true;
	}
	return false;
}

Cell *CItem::GetCell(Direction dr)
{
	int x = m_nCellX;
	int y = m_nCellY;

	switch( dr )
	{
	case drUp: y--; break;
	case drDown: y++; break;
	case drLeft: x--; break;
	case drRight: x++; break;
	default: break;
	}

	return &MAP[y][x];
}

void CAniItem::Update(float dt)
{
	if( m_aniShadow ) m_aniShadow->Update( dt );
	if( m_aniMain ) m_aniMain->Update( dt );
}

void CAniItem::Render() const
{
	int x = SCENE_LEFT + (m_nCellX - 1) * CELL_SIZE;
	int y = SCENE_TOP + m_nCellY * CELL_SIZE;

	if( m_aniShadow ) m_aniShadow->Render( x, y );
	if( m_aniMain ) m_aniMain->Render( x, y );
}

void CMedium::Render(void) const
{
	int x = SCENE_LEFT + (m_nCellX - 1) * CELL_SIZE;
	int y = SCENE_TOP + m_nCellY * CELL_SIZE;

	if( m_sprShadow ) m_sprShadow->Render( x, y );
	if( m_sprMain ) m_sprMain->Render( x, y );
}

void CLarge::Update(float dt)
{
	static const float INTERVAL = 4.0f;
	static const float PLAYTIME = 0.8f;

	m_fTime += dt;
	if( m_bPlay )
	{
		if( m_aniMain ) m_aniMain->Update( dt );
		if( m_fTime > PLAYTIME )
		{
			m_fTime = 0.0f;
			m_bPlay = false;
		}
	}
	else if( m_fTime > INTERVAL )
	{
		m_fTime = 0.0f;
		m_bPlay = true;
	}
}

void CLarge::Render(void) const
{
	int x = SCENE_LEFT + (m_nCellX - 1) * CELL_SIZE;
	int y = SCENE_TOP + m_nCellY * CELL_SIZE;

	if( m_sprShadow ) m_sprShadow->Render( x, y );
	if( m_sprMain ) m_sprMain->Render( x, y );
	if( m_aniMain ) m_aniMain->Render( x, y );
}

void CGift::Update(float dt)
{
	static const float DELTA = 0.15f;

	if( m_bStart )
	{
		m_nScale++;
		if( m_nScale > 20 )
		{
			m_bStart = false;
		}
	}
	else
	{
		m_fTime += dt;
		if( m_fTime > DELTA )
		{
			m_fTime -= DELTA;
			m_nOffsetY += m_nDir;
			if( m_nOffsetY >=5 || m_nOffsetY <= 0 )
			{
				m_nDir = -m_nDir;
			}
		}

		m_aniMain->Update( dt );
	}
}

void CGift::Render() const
{
	int x = SCENE_LEFT + (m_nCellX - 1) * CELL_SIZE;
	int y = SCENE_TOP + m_nCellY * CELL_SIZE;

	if( m_bStart )
	{
		m_aniMain->RenderEx( x, y + ( 20 - m_nScale ) * 2, 0, 0.05f * m_nScale, 0.05f * m_nScale );
	}
	else
	{
		// 画阴影
		if( m_aniShadow != NULL )
		{
			if( m_nOffsetY <= 2 )
			{
				m_aniShadow->SetFrame( 1 );
			}
			else
			{
				m_aniShadow->SetFrame( 0 );
			}
			m_aniShadow->Render( x, y );
		}

		// 画宝物
		if( m_aniMain != NULL )
		{			
			m_aniMain->Render( x, y - m_nOffsetY );
		}
	}
}

void CBox::Move(Direction dr)
{
	Cell *pCell;
	int nNewX, nNewY;

	MoveAhead( m_nCellX, m_nCellY, &nNewX, &nNewY, dr );

	pCell = &MAP[nNewY][nNewX];
	
	for( int i = 0; i < ROLE_NUM; i++ )
	{
		if( ROLE[i].GetX() == nNewX && ROLE[i].GetY() == nNewY &&
			!ROLE[i].IsFlying() )
		{
			return;
		}
	}

	if( !m_bMoving &&
		( pCell->Type == itGift || pCell->Type == itNone ) &&
		( pCell->Blind != btTent || dr != drLeft && dr != drRight ) )
	{
		m_bMoving = true;
		m_Direction = dr;
	}
}

void CBox::Update(float dt)
{
	if( !m_bMoving ) return;

	static const int STEP = 4;
	Cell *pCell = NULL;

	switch( m_Direction )
	{
	case drUp:
		m_fOffsetY -= STEP;
		if( m_fOffsetY < -CELL_SIZE / 2 )
		{
			pCell = GetCell( drMiddle );
			pCell->Type = itNone;
			pCell->pItem = NULL;
			pCell = GetCell( drUp );
			pCell->Type = itBox;
			SAFE_DELETE( pCell->pItem );
			pCell->pItem = this;

			m_fOffsetY += CELL_SIZE;
			m_nCellY--;
		}
		if( m_fOffsetY == 0 )
		{
			m_bMoving = false;
		}
		break;

	case drDown:
		m_fOffsetY += STEP;
		if( m_fOffsetY > CELL_SIZE / 2 )
		{
			pCell = GetCell( drMiddle );
			pCell->Type = itNone;
			pCell->pItem = NULL;
			pCell = GetCell( drDown );
			pCell->Type = itBox;
			SAFE_DELETE( pCell->pItem );
			pCell->pItem = this;

			m_fOffsetY -= CELL_SIZE;
			m_nCellY++;
		}
		if( m_fOffsetY == 0 )
		{
			m_bMoving = false;
		}
		break;

	case drLeft:
		m_fOffsetX -= STEP;
		if( m_fOffsetX < -CELL_SIZE / 2 )
		{
			pCell = GetCell( drMiddle );
			pCell->Type = itNone;
			pCell->pItem = NULL;
			pCell = GetCell( drLeft );
			pCell->Type = itBox;
			SAFE_DELETE( pCell->pItem );
			pCell->pItem = this;

			m_fOffsetX += CELL_SIZE;
			m_nCellX--;
		}
		if( m_fOffsetX == 0 )
		{
			m_bMoving = false;
		}				
		break;

	case drRight:
		m_fOffsetX += STEP;
		if( m_fOffsetX > CELL_SIZE / 2 )
		{
			pCell = GetCell( drMiddle );
			pCell->Type = itNone;
			pCell->pItem = NULL;
			pCell = GetCell( drRight );
			pCell->Type = itBox;
			SAFE_DELETE( pCell->pItem );
			pCell->pItem = this;

			m_fOffsetX -= CELL_SIZE;
			m_nCellX++;
		}
		if( m_fOffsetX == 0 )
		{
			m_bMoving = false;
		}
		break;
	}
}

void CBox::Render() const
{
	int x = ( m_nCellX - 1 ) * CELL_SIZE + SCENE_LEFT + m_fOffsetX;
	int y = m_nCellY * CELL_SIZE + SCENE_TOP + m_fOffsetY;

	if( m_sprShadow != NULL)
	{
		m_sprShadow->Render( x, y );
	}
	if( m_sprMain != NULL )
	{
		m_sprMain->Render( x, y );
	}
}

void CPopo::Update(float dt)
{
	static const int STEP = 8;
	Cell *pCell = NULL;

	m_fRemainTime -= dt;
	if( m_fRemainTime <= 0.0f )
	{
		Explode();
		return;
	}
	
	if( m_aniShadow ) m_aniShadow->Update( dt );
	if( m_aniMain ) m_aniMain->Update( dt );
	
	if( m_bMoving )
	{
		switch( m_Direction )
		{
		case drUp:
			if( m_fOffsetY != 0 )
			{
				m_fOffsetY -= STEP;
			}
			else
			{
				pCell = GetCell( drUp );
				if( pCell->Type == itGift || pCell->Type == itNone )
				{
					m_fOffsetY -= STEP;
				}
				else
				{
					m_bMoving = false;
				}
			}

			if( m_fOffsetY < -CELL_SIZE / 2 )
			{
				pCell = GetCell( drMiddle );
				pCell->Type = itNone;
				pCell->pItem = NULL;
				pCell = GetCell( drUp );
				pCell->Type = itPopo;
				SAFE_DELETE( pCell->pItem );
				pCell->pItem = this;

				m_fOffsetY += CELL_SIZE;
				m_nCellY--;
			}
			break;
		case drDown:
			if( m_fOffsetY != 0 )
			{
				m_fOffsetY += STEP;
			}
			else
			{
				pCell = GetCell( drDown );
				if( pCell->Type == itGift || pCell->Type == itNone )
				{
					m_fOffsetY += STEP;
				}
				else
				{
					m_bMoving = false;
				}
			}

			if( m_fOffsetY > CELL_SIZE / 2 )
			{
				pCell = GetCell( drMiddle );
				pCell->Type = itNone;
				pCell->pItem = NULL;
				pCell = GetCell( drDown );
				pCell->Type = itPopo;
				SAFE_DELETE( pCell->pItem );
				pCell->pItem = this;

				m_fOffsetY -= CELL_SIZE;
				m_nCellY++;
			}
			break;
		case drLeft:
			if( m_fOffsetX != 0 )
			{
				m_fOffsetX -= STEP;
			}
			else
			{
				pCell = GetCell( drLeft );
				if( pCell->Type == itGift || pCell->Type == itNone )
				{
					m_fOffsetX -= STEP;
				}
				else
				{
					m_bMoving = false;
				}
			}
			
			if( m_fOffsetX < -CELL_SIZE / 2 )
			{
				pCell = GetCell( drMiddle );
				pCell->Type = itNone;
				pCell->pItem = NULL;
				pCell = GetCell( drLeft );
				pCell->Type = itPopo;
				SAFE_DELETE( pCell->pItem );
				pCell->pItem = this;

				m_fOffsetX += CELL_SIZE;
				m_nCellX--;
			}
			break;
		case drRight:
			if( m_fOffsetX != 0 )
			{
				m_fOffsetX += STEP;
			}
			else
			{
				pCell = GetCell( drRight );
				if( pCell->Type == itGift || pCell->Type == itNone )
				{
					m_fOffsetX += STEP;
				}
				else
				{
					m_bMoving = false;
				}
			}
			
			if( m_fOffsetX > CELL_SIZE / 2 )
			{
				pCell = GetCell( drMiddle );
				pCell->Type = itNone;
				pCell->pItem = NULL;
				pCell = GetCell( drRight );
				pCell->Type = itPopo;
				SAFE_DELETE( pCell->pItem );
				pCell->pItem = this;

				m_fOffsetX -= CELL_SIZE;
				m_nCellX++;
			}
			break;
		}
	}
}

void CPopo::Render() const
{
	int x = SCENE_LEFT + ( m_nCellX - 1 ) * CELL_SIZE + m_fOffsetX;
	int y = SCENE_TOP + m_nCellY * CELL_SIZE + m_fOffsetY;

	if( m_aniShadow ) m_aniShadow->Render( x, y );
	if( m_aniMain ) m_aniMain->Render( x, y );
}

void CPopo::Explode()
{
	m_pOwner->GetAbility().nUsedPopo--;
	SOUND.PlaySnd( sndExplode );

	// 处理爆炸中心
	MAP[m_nCellY][m_nCellX].Explosion.Status = 1;
	MAP[m_nCellY][m_nCellX].Explosion.Dr = drMiddle;
	MAP[m_nCellY][m_nCellX].Explosion.Timer = 1;

	MAP[m_nCellY][m_nCellX].Type = itNone;
	MAP[m_nCellY][m_nCellX].pItem = NULL;
	if( MAP[m_nCellY][m_nCellX].Blind == btBush )
	{
		MAP[m_nCellY][m_nCellX].Blind = btNone;
		SAFE_DELETE( MAP[m_nCellY][m_nCellX].pBlindage );
	}

	// 处理爆炸冲击波
	for( int dr = drUp; dr <= drRight; dr++ )
	{
		int i = 0;
		int nCurrentX = m_nCellX, nCurrentY = m_nCellY;

		while( i++ < m_nPower )
		{
			if( MAP[nCurrentY][nCurrentX].Blind == btTent && ( dr == drLeft || dr == drRight ) )
			{
				break;
			}

			MoveAhead( nCurrentX, nCurrentY, &nCurrentX, &nCurrentY, (Direction)dr );
			ExpProperty ep = GetExpProperty( nCurrentY, nCurrentX, (Direction)dr );

			if( ep != epFirm )
			{
				MAP[nCurrentY][nCurrentX].Explosion.Status = 1;
				MAP[nCurrentY][nCurrentX].Explosion.Dr = dr;
				MAP[nCurrentY][nCurrentX].Explosion.IsHead = ( i == m_nPower ? 1 : 0 );
				MAP[nCurrentY][nCurrentX].Explosion.Timer = i;
				MAP[nCurrentY][nCurrentX].Explosion.Dead = 12 + m_nPower - i;

				if( ep == epSolid )
				{
					SAFE_DELETE( MAP[nCurrentY][nCurrentX].pItem );
					MAP[nCurrentY][nCurrentX].Type = itNone;

					int nID = 0;
					MiJi( &nID, this );
					if( nID || g_pGST->Random_Int( 0, 1000 ) % 4 == 0 )
					{
						// 随机产生宝物
						if( !nID )
						{
							nID = g_pGST->Random_Int( 1, FACTORY.GetGiftNum() + 8 );
							if( nID >= FACTORY.GetGiftNum() + 4 )
							{
								nID = 1;
							}
							else if( nID > FACTORY.GetGiftNum() )
							{
								nID = 3;
							}
						}

						MAP[nCurrentY][nCurrentX].pItem = FACTORY.MakeFixedItem( nID );
						MAP[nCurrentY][nCurrentX].pItem->SetPosition( nCurrentY, nCurrentX );
						MAP[nCurrentY][nCurrentX].Type = itGift;

						SOUND.PlaySnd( sndAppear );
					}
					break;
				}
				else if( ep == epPopo )
				{
					( ( CPopo * ) MAP[nCurrentY][nCurrentX].pItem )->Explode();
				}
				else
				{
					SAFE_DELETE( MAP[nCurrentY][nCurrentX].pItem );
					MAP[nCurrentY][nCurrentX].Type = itNone;
					if( MAP[nCurrentY][nCurrentX].Blind == btBush )
					{
						SAFE_DELETE( MAP[nCurrentY][nCurrentX].pBlindage );
						MAP[nCurrentY][nCurrentX].Blind = btNone;
					}
				}
			}
			else
			{
				break;
			}
		}
	}
	delete this;
}

CPopo::ExpProperty CPopo::GetExpProperty(int nCellY,int nCellX,Direction dr)
{
	if( MAP[nCellY][nCellX].Type == itFirm ||
		MAP[nCellY][nCellX].Blind == btTent && ( dr == drLeft || dr == drRight ) )
	{
		return epFirm;
	}
	else if( MAP[nCellY][nCellX].Type == itSolid ||
		MAP[nCellY][nCellX].Type == itBox )
	{
		return epSolid;
	}
	else if( MAP[nCellY][nCellX].Type == itPopo )
	{
		return epPopo;
	}
	else
	{
		return epNone;
	}
}

void CBush::Update(float dt)
{
	static const float DELTA = 0.08f;

	if( m_bWaggle )
	{
		m_fTime += dt;
		if( m_fTime > DELTA )
		{
			m_fTime -= DELTA;
			m_nStatus++;
			if( m_nStatus == 3 )
			{
				m_fTime = 0;
				m_nStatus = 0;
				m_bWaggle = false;
			}
		}
	}
}

void CBush::Render() const
{
	int x = ( m_nCellX - 1 ) * CELL_SIZE + SCENE_LEFT;
	int y = m_nCellY * CELL_SIZE + SCENE_TOP;

	if( m_sprShadow )
	{
		m_sprShadow->Render( x, y );
	}

	if( m_nStatus == 1 )
	{
		y -= 3;
	}
	else if( m_nStatus == 2 )
	{
		y += 3;
	}

	if( m_sprMain )
	{
		m_sprMain->Render( x, y );
	}
}

void CTent::Update(float dt)
{
	if( m_bWaggle )
	{
		m_aniMain->Update( dt );
		if( !m_aniMain->IsPlaying() )
		{
			m_aniMain->SetFrame( 0 );
			m_bWaggle = false;
		}
	}
}

void CTent::Render() const
{
	int x = ( m_nCellX - 1 ) * CELL_SIZE + SCENE_LEFT;
	int y = m_nCellY * CELL_SIZE + SCENE_TOP;

	if( m_sprShadow ) m_sprShadow->Render( x, y );
	if( m_aniMain ) m_aniMain->Render( x, y );
}