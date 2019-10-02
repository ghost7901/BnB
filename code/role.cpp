#include "role.h"
#include "map.h"
#include "sound.h"
#include "game.h"

CRole ROLE[ROLE_NUM];

/////////////////////////////////////////////////////////////////
// CRole
/////////////////////////////////////////////////////////////////
CRole::CRole() : m_pState(NULL), m_nWinCount(0)
{
	New();
}

CRole::~CRole()
{
	SAFE_DELETE( m_pState );
}

void CRole::Load(const char *szIniFile)
{
	g_pGST->System_SetState( GST_INIFILE, szIniFile );
	m_aniRoleUp = g_pResManager->GetAnimation( g_pGST->Ini_GetString( "Role", "Up", "" ) );
	m_aniRoleDown = g_pResManager->GetAnimation( g_pGST->Ini_GetString( "Role", "Down", "" ) );
	m_aniRoleLeft = g_pResManager->GetAnimation( g_pGST->Ini_GetString( "Role", "Left", "" ) );
	m_aniRoleRight = g_pResManager->GetAnimation( g_pGST->Ini_GetString( "Role", "Right", "" ) );
	m_aniRole = g_pResManager->GetAnimation( g_pGST->Ini_GetString( "Role", "Ani", "" ) );
	m_aniRoleWin = g_pResManager->GetAnimation( g_pGST->Ini_GetString( "Role", "Win", "" ) );
	m_aniRoleIdle = g_pResManager->GetAnimation( g_pGST->Ini_GetString( "Role", "Idle", "" ) );
	m_aniRoleDie = g_pResManager->GetAnimation( g_pGST->Ini_GetString( "Role", "Die", "" ) );
	m_aniRoleStart = g_pResManager->GetAnimation( g_pGST->Ini_GetString( "Role", "Start", "" ) );
	m_aniRoleStartFlash = g_pResManager->GetAnimation( g_pGST->Ini_GetString( "Role", "StartFlash", "" ) );
	m_aniRoleRide = g_pResManager->GetAnimation( g_pGST->Ini_GetString( "Role", "Ride", "" ) );
	m_sprShadow = g_pResManager->GetSprite( g_pGST->Ini_GetString( "Role", "Shadow", "" ) );

	m_aniBigPopo = new gstAnimation( *(g_pResManager->GetAnimation( "aniBigPopo" )) );
	m_aniBigPopoBomb = new gstAnimation( *(g_pResManager->GetAnimation( "aniBigPopoBomb" )) );
	m_aniFastTurtle = new gstAnimation( *(g_pResManager->GetAnimation( "aniFastTurtle" )) );
	m_aniSlowTurtle = new gstAnimation( *(g_pResManager->GetAnimation( "aniSlowTurtle" )) );
	m_aniFastUFO = new gstAnimation( *(g_pResManager->GetAnimation( "aniFastUFO" )) );
	m_aniOwl = new gstAnimation( *(g_pResManager->GetAnimation( "aniOwl" )) );

	m_aniFastTurtle->Play();
	m_aniSlowTurtle->Play();
	m_aniFastUFO->Play();
	m_aniOwl->Play();

	m_aniRoleUp->Play();
	m_aniRoleDown->Play();
	m_aniRoleLeft->Play();
	m_aniRoleRight->Play();
	m_aniRole->Play();
	m_aniRoleRide->Play();
}

void CRole::Start()
{
	ChangeState( new CRoleStateStart( this ) );
}

void CRole::New()
{
	m_nCellX = 1;
	m_nCellY = 1;
	m_fOffsetX = 0.0f;
	m_fOffsetY = 0.0f;
	m_nPushTime = 0;

	m_Ability.nUsedPopo = 0;
	m_Ability.nPopoMax = 1;
	m_Ability.nPopoPower = 1;
	m_Ability.nSpeed = 3;
	m_Ability.bKick = false;
	m_Ability.Vehicle = vtNone;

	m_bMoving = false;
	m_bFlying = false;
	m_bDead = false;
	m_Direction = drDown;

	SAFE_DELETE( m_pState );
}

template <Direction dr>
CRole::WalkProperty CRole::GetWalkProperty()
{
	int x = m_nCellX;
	int y = m_nCellY;

	switch(dr)
	{
	case drUp: y--; break;
	case drDown: y++; break;
	case drLeft: x--; break;
	case drRight: x++; break;
	case drUpLeft: x--; y--; break;
	case drUpRight: x++; y--; break;
	case drDownLeft: x--; y++; break;
	case drDownRight: x++; y++; break;
	}

	if( MAP[y][x].Type == itFirm || MAP[y][x].Type == itPopo )
	{
		return wpStop;
	}
	else if( MAP[y][x].Type == itSolid || MAP[y][x].Type == itBox )
	{
		return m_bFlying ? wpNone : wpStop;
	}
	else if( MAP[y][x].Type == itNone && MAP[y][x].Blind == btTent )
	{
		return wpTent;
	}
	else
	{
		return wpNone;
	}
}

template <Direction dr>
void CRole::Push()
{
	int x = m_nCellX;
	int y = m_nCellY;

	switch( dr )
	{
	case drUp: y--; break;
	case drDown: y++; break;
	case drLeft: x--; break;
	case drRight: x++; break;
	case drUpLeft: x--; y--; break;
	case drUpRight: x++; y--; break;
	case drDownLeft: x--; y++; break;
	case drDownRight: x++; y++; break;
	}

	if( MAP[y][x].Type == itBox )
	{
		m_nPushTime++;
		if( m_nPushTime >= 20 )
		{
			m_nPushTime = 0;
			( (CBox *)MAP[y][x].pItem )->Move( dr );
		}
	}
	else if( MAP[y][x].Type == itPopo && m_Ability.bKick && !m_bFlying )
	{
		( (CPopo *)MAP[y][x].pItem )->Move( dr );
	}
}

void CRole::UpdateMoveUp(float fSpeed)
{
	WalkProperty up, upleft, upright, left, right;

	up = GetWalkProperty <drUp> ();
	upleft = GetWalkProperty <drUpLeft> ();
	upright = GetWalkProperty <drUpRight> ();
	left = GetWalkProperty <drLeft> ();
	right = GetWalkProperty <drRight> ();

	if( m_fOffsetY > 0 )
	{
		m_fOffsetY -= fSpeed;
		if( m_fOffsetY < 0 && up != wpNone )
		{
			m_fOffsetY = 0;
		}
	}
	else
	{
		switch( up )
		{
		case wpStop:
			if( m_fOffsetX < -10 && left == wpNone && upleft <= wpTent )
			{
				m_fOffsetX -= fSpeed;
			}
			else if( m_fOffsetX > 10 && right == wpNone && upright <= wpTent )
			{
				m_fOffsetX += fSpeed;
			}
			else
			{
				Push<drUp>();
			}
			break;

		case wpTent:
			if( m_fOffsetX == 0 )
			{
				m_fOffsetY -= fSpeed;
			}
			else if( m_fOffsetX < 0 )
			{
				m_fOffsetX = min( m_fOffsetX + fSpeed, 0 );
			}
			else if( m_fOffsetX > 0 )
			{
				m_fOffsetX = max( m_fOffsetX - fSpeed, 0 );
			}
			break;

		case wpNone:
			if( m_fOffsetX < 0 && upleft == wpNone ||
				m_fOffsetX > 0 && upright == wpNone ||
				m_fOffsetX == 0 )
			{
				m_fOffsetY -= fSpeed;
			}
			else if( m_fOffsetX < 0 && upleft != wpNone )
			{
				m_fOffsetX = min( m_fOffsetX + fSpeed, 0 );
			}
			else if( m_fOffsetX > 0 && upright != wpNone )
			{
				m_fOffsetX = max( m_fOffsetX - fSpeed, 0 );
			}
			break;
		}
	}

	m_aniRoleUp->SetSpeed( fSpeed / 7.0f * 18.0f );
	m_aniRoleUp->Update(g_pGST->Timer_GetDelta());

	AdjustCell();
}

void CRole::UpdateMoveDown(float fSpeed)
{
	WalkProperty down, downleft, downright, left, right;
	down = GetWalkProperty <drDown> ();
	downleft = GetWalkProperty <drDownLeft> ();
	downright = GetWalkProperty <drDownRight> ();
	left = GetWalkProperty <drLeft> ();
	right = GetWalkProperty <drRight> ();

	if( m_fOffsetY < 0 )
	{
		m_fOffsetY += fSpeed;
		if( m_fOffsetY > 0 && down != wpNone )
		{
			m_fOffsetY = 0;
		}
	}
	else
	{
		switch( down )
		{
		case wpStop:
			if( m_fOffsetX < -10 && left == wpNone && downleft <= wpTent )
			{
				m_fOffsetX -= fSpeed;
			}
			else if( m_fOffsetX > 10 && right == wpNone && downright <= wpTent )
			{
				m_fOffsetX += fSpeed;
			}
			else
			{
				Push <drDown> ();
			}
			break;
			
		case wpTent:
			if( m_fOffsetX == 0 )
			{
				m_fOffsetY += fSpeed;
			}
			else if( m_fOffsetX < 0 )
			{
				m_fOffsetX = min( m_fOffsetX + fSpeed, 0 );
			}
			else if( m_fOffsetX > 0 )
			{
				m_fOffsetX = max( m_fOffsetX - fSpeed, 0 );
			}
			break;

		case wpNone:
			if( m_fOffsetX < 0 && downleft == wpNone ||
				m_fOffsetX > 0 && downright == wpNone ||
				m_fOffsetX == 0 )
			{
				m_fOffsetY += fSpeed;
			}
			else if( m_fOffsetX < 0 && downleft != wpNone )
			{
				m_fOffsetX = min( m_fOffsetX + fSpeed, 0 );
			}
			else if ( m_fOffsetX > 0 && downright != wpNone )
			{
				m_fOffsetX = max( m_fOffsetX - fSpeed, 0 );
			}
			break;
		}
	}

	m_aniRoleDown->SetSpeed( fSpeed / 7.0f * 18.0f );
	m_aniRoleDown->Update(g_pGST->Timer_GetDelta());

	AdjustCell();
}

void CRole::UpdateMoveLeft(float fSpeed)
{
	WalkProperty left, upleft, downleft, up, down;
	left = GetWalkProperty <drLeft> ();
	upleft = GetWalkProperty <drUpLeft> ();
	downleft = GetWalkProperty <drDownLeft> ();
	up = GetWalkProperty <drUp> ();
	down = GetWalkProperty <drDown> ();

	if( m_fOffsetX > 0 )
	{
		m_fOffsetX -= fSpeed;
		if( m_fOffsetX < 0 && left != wpNone )
		{
			m_fOffsetX = 0;
		}
	}
	else if( MAP[m_nCellY][m_nCellX].Blind != btTent )
	{
		if( left != wpNone )
		{
			if( m_fOffsetY < -10 && up == wpNone && upleft <= wpTent )
			{
				m_fOffsetY -= fSpeed;
			}
			else if( m_fOffsetY > 10 && down == wpNone && downleft <= wpTent )
			{
				m_fOffsetY += fSpeed;
			}
			else
			{
				Push <drLeft> ();
			}
		}
		else
		{			
			if( m_fOffsetY < 0 && ( upleft != wpNone || up == wpTent ) )
			{
				m_fOffsetY = min( m_fOffsetY + fSpeed, 0 );
			}
			else if( m_fOffsetY > 0 && ( downleft != wpNone || down == wpTent ) )
			{
				m_fOffsetY = max( m_fOffsetY - fSpeed, 0 );
			}
			else if( m_fOffsetY < 0 && upleft == wpNone ||
				m_fOffsetY > 0 && downleft == wpNone ||
				m_fOffsetY == 0 )
			{
				m_fOffsetX -= fSpeed;
			}
		}
	}

	m_aniRoleLeft->SetSpeed( fSpeed / 7.0f * 18.0f );
	m_aniRoleLeft->Update(g_pGST->Timer_GetDelta());

	AdjustCell();
}

void CRole::UpdateMoveRight(float fSpeed)
{
	WalkProperty right, downright, upright, down, up;

	right = GetWalkProperty <drRight> ();
	downright = GetWalkProperty <drDownRight> ();
	upright = GetWalkProperty <drUpRight> ();
	down = GetWalkProperty <drDown> ();
	up = GetWalkProperty <drUp> ();

	if( m_fOffsetX < 0 )
	{
		m_fOffsetX += fSpeed;
		if( m_fOffsetX > 0 && right != wpNone )
		{
			m_fOffsetX = 0;
		}
	}
	else if( MAP[m_nCellY][m_nCellX].Blind != btTent )
	{
		if( right != wpNone )
		{
			if( m_fOffsetY < -10 && up == wpNone && upright <= wpTent )
			{
				m_fOffsetY -= fSpeed;
			}
			else if( m_fOffsetY > 10 && down == wpNone && downright <= wpTent )
			{
				m_fOffsetY += fSpeed;
			}
			else
			{
				Push <drRight> ();
			}
		}
		else
		{			
			if( m_fOffsetY < 0 && ( upright != wpNone || up == wpTent ) )
			{
				m_fOffsetY = min( m_fOffsetY + fSpeed, 0 );
			}
			else if( m_fOffsetY > 0 && ( downright != wpNone || down == wpTent ) )
			{
				m_fOffsetY = max( m_fOffsetY - fSpeed, 0 );
			}
			else if( m_fOffsetY < 0 && upright == wpNone ||
				m_fOffsetY > 0 && downright == wpNone ||
				m_fOffsetY == 0 )
			{
				m_fOffsetX += fSpeed;
			}
		}
	}

	m_aniRoleRight->SetSpeed( fSpeed / 7.0f * 18.0f );
	m_aniRoleRight->Update( g_pGST->Timer_GetDelta() );

	AdjustCell();
}

void CRole::Update(float dt)
{
	m_pState->Update( dt );
}

void CRole::Render()
{
	float x = SCENE_LEFT + ( m_nCellX - 1 ) * CELL_SIZE + m_fOffsetX;
	float y = SCENE_TOP + m_nCellY * CELL_SIZE + m_fOffsetY;

	m_pState->Render( x, y );
}

void CRole::ChangeState(CRoleState *pState)
{
	SAFE_DELETE( m_pState );
	m_pState = pState;
}

void CRole::Move(Direction dr)
{
	m_bMoving = true;
	if( m_Direction != dr )
	{
		m_Direction = dr;
	}
}

void CRole::AdjustCell()
{
	if( m_fOffsetX < -CELL_SIZE / 2 )
	{
		m_nCellX--;
		m_fOffsetX += CELL_SIZE;
	}
	else if( m_fOffsetX > CELL_SIZE /2 )
	{
		m_nCellX++;
		m_fOffsetX -= CELL_SIZE;
	}
	else if( m_fOffsetY < -CELL_SIZE / 2 )
	{
		m_nCellY--;
		m_fOffsetY += CELL_SIZE;
	}
	else if( m_fOffsetY > CELL_SIZE / 2 )
	{
		m_nCellY++;
		m_fOffsetY -= CELL_SIZE;
	}
	else
	{
		return;
	}

	EnterNewCell();
}

void CRole::EnterNewCell()
{
	if( MAP[m_nCellY][m_nCellX].Blind >= btBush )
	{
		MAP[m_nCellY][m_nCellX].pBlindage->Waggle();
	}
}

void CRole::LayPopo()
{
	RoleState rs = m_pState->Type();
	if( ( rs == rsNormal || rs == rsIdle ) && m_Ability.nUsedPopo < m_Ability.nPopoMax &&
		( MAP[m_nCellY][m_nCellX].Type == itNone || MAP[m_nCellY][m_nCellX].Type == itGift ) )
	{
		m_Ability.nUsedPopo++;

		CPopo *pPopo = (CPopo *)FACTORY.MakeFixedItem( 0 );
		pPopo->SetPosition( m_nCellY, m_nCellX );
		pPopo->SetPower( m_Ability.nPopoPower );
		pPopo->SetOwner( this );
		SAFE_DELETE( MAP[m_nCellY][m_nCellX].pItem );
		MAP[m_nCellY][m_nCellX].pItem = pPopo;
		MAP[m_nCellY][m_nCellX].Type = itPopo;

		SOUND.PlaySnd( sndLay );
	}
}

void CRole::Win()
{
	ChangeState( new CRoleStateWin( this ) );
	m_nWinCount++;
}

void CRole::Normal()
{
	RoleState rs = m_pState->Type();
	if( rs == rsNormal && m_Ability.Vehicle != vtNone || rs == rsOnRide )
	{
		ChangeState( new CRoleStateOnDismount( this ) );
	}
	else if( rs == rsTraped || rs == rsOnTrap )
	{
	//	ChangeState(new CRoleStateOnSave(this));
	}
}

/////////////////////////////////////////////////////////////////
// CRoleState
/////////////////////////////////////////////////////////////////
void CRoleState::CheckChange()
{
	Cell *pCell = &MAP[m_pRole->m_nCellY][m_pRole->m_nCellX];

	if( pCell->Type == itGift && !m_pRole->m_bFlying )
	{
		SOUND.PlaySnd(sndGet);

		const Ability ab = ( ( CGift * ) pCell->pItem )->GetAbility();
		SAFE_DELETE( pCell->pItem );
		pCell->Type = itNone;
		
		Ability &abRole = m_pRole->m_Ability;
		abRole.nPopoMax += ab.nPopoMax;
		abRole.nPopoPower = min( abRole.nPopoPower + ab.nPopoPower, MAX_POWER );
		abRole.nSpeed = min( abRole.nSpeed + ab.nSpeed, MAX_SPEED );
		abRole.bKick = abRole.bKick || ab.bKick;
		if( abRole.Vehicle == vtNone && ab.Vehicle != vtNone )
		{
			abRole.Vehicle = ab.Vehicle;
			m_pRole->m_fOffsetX = 0;
			m_pRole->m_fOffsetY = 0;
			if( ab.Vehicle == vtFastUFO || ab.Vehicle == vtSlowUFO )
			{
				m_pRole->m_bFlying = true;
			}
			ChangeState( new CRoleStateOnRide( m_pRole ) );
			return;
		}
	}

	if( pCell->Explosion.Status != 0 )
	{
		if( m_pRole->m_Ability.Vehicle == vtNone )
		{
			ChangeState( new CRoleStateOnTrap( m_pRole ) );
		}
		else
		{
			ChangeState( new CRoleStateOnDismount( m_pRole ) );
		}
	}
}

void CRoleState::RenderShadow(float x,float y) const
{
	m_pRole->m_sprShadow->Render( x, y );
}

/////////////////////////////////////////////////////////////////
// CRoleStateStart
/////////////////////////////////////////////////////////////////
void CRoleStateStart::Update(float dt)
{	
	if( !m_bFlash )
	{
		m_pRole->m_aniRoleStart->Update( dt );
		if( !m_pRole->m_aniRoleStart->IsPlaying() )
		{			
			m_bFlash = true;
		}		
	}
	else
	{
		m_pRole->m_aniRoleStartFlash->Update( dt );
		m_fTime += dt;
		if( m_fTime > 1.0f )
		{
			ChangeState( new CRoleStateNormal( m_pRole ) );
		}
	}
}

void CRoleStateStart::Render(float x,float y) const
{
	RenderShadow( x, y );

	if( !m_bFlash )
	{
		m_pRole->m_aniRoleStart->Render( x, y );
	}
	else
	{
		m_pRole->m_aniRoleStartFlash->Render( x, y );
	}
}

/////////////////////////////////////////////////////////////////
// CRoleStateNormal
/////////////////////////////////////////////////////////////////
void CRoleStateNormal::Update(float dt)
{
	if( m_pRole->m_bMoving )
	{
		int nSpeed;
		if( m_pRole->m_Ability.Vehicle != vtNone )
		{
			nSpeed = VEHICLE_SPEED [ m_pRole->m_Ability.Vehicle ];
			switch( m_pRole->m_Ability.Vehicle )
			{
			case vtFastTurtle:
				m_pRole->m_aniFastTurtle->SetTextureRect( 204, 184 + 32 * m_pRole->m_Direction, 48, 32 );
				m_pRole->m_aniFastTurtle->Update( dt );
				break;
			case vtSlowTurtle:
				m_pRole->m_aniSlowTurtle->SetTextureRect( 300, 184 + 32 * m_pRole->m_Direction, 48, 32 );
				m_pRole->m_aniSlowTurtle->Update( dt );
				break;
			case vtFastUFO:
				m_pRole->m_aniFastUFO->SetTextureRect( 100, 184 + 31 * m_pRole->m_Direction, 52, 31 );
				m_pRole->m_aniFastUFO->Update( dt );
				break;
			case vtSlowUFO:
				//待加入
				break;
			case vtOwl:
				m_pRole->m_aniOwl->SetTextureRect( 20, 184 + 40 * m_pRole->m_Direction, 40, 40 );
				m_pRole->m_aniOwl->Update( dt );
				break;
			}
			m_pRole->m_aniRoleRide->SetFrame( m_pRole->m_Direction );
		}
		else
		{
			nSpeed = m_pRole->m_Ability.nSpeed;
		}

		switch( m_pRole->m_Direction )
		{
		case drUp:
			m_pRole->UpdateMoveUp( nSpeed );
			break;
		case drDown:
			m_pRole->UpdateMoveDown( nSpeed );
			break;
		case drLeft:
			m_pRole->UpdateMoveLeft( nSpeed );
			break;
		case drRight:
			m_pRole->UpdateMoveRight( nSpeed );
			break;
		}

		m_fTime = 0.0f;
		m_pRole->m_bMoving = false;
	}
	else
	{
		switch( m_pRole->m_Direction )
		{
		case drUp:
			m_pRole->m_aniRoleUp->SetFrame( 0 );
			break;
		case drDown:
			m_pRole->m_aniRoleDown->SetFrame( 0 );
			break;
		case drLeft:
			m_pRole->m_aniRoleLeft->SetFrame( 0 );
			break;
		case drRight:
			m_pRole->m_aniRoleRight->SetFrame( 0 );
			break;
		}
		
		//m_pRole->m_nPushTime = 0;
		if( m_pRole->m_Ability.Vehicle == vtNone )
		{
			m_fTime += dt;
			if( m_fTime > 3.5f )
			{
			//	m_fTime = 0;
				ChangeState( new CRoleStateIdle( m_pRole ) );
				return;
			}
		}
	}

	CheckChange();
}

void CRoleStateNormal::Render(float x,float y) const
{
	RenderShadow( x, y );

	if( m_pRole->m_Ability.Vehicle == vtNone )
	{
		switch( m_pRole->m_Direction )
		{
		case drUp:
			m_pRole->m_aniRoleUp->Render( x, y );
			break;
		case drDown:
			m_pRole->m_aniRoleDown->Render( x, y );
			break;
		case drLeft:
			m_pRole->m_aniRoleLeft->Render( x, y );
			break;
		case drRight:
			m_pRole->m_aniRoleRight->Render( x, y );
			break;
		}
	}
	else
	{
		switch (m_pRole->m_Ability.Vehicle)
		{
		case vtFastTurtle:		
			m_pRole->m_aniFastTurtle->Render( x, y );
			break;
		case vtSlowTurtle:		
			m_pRole->m_aniSlowTurtle->Render( x, y );
			break;
		case vtFastUFO:
			m_pRole->m_aniFastUFO->Render( x, y );
			break;
		case vtOwl:
			m_pRole->m_aniOwl->Render( x, y );
			break;		
		}
		m_pRole->m_aniRoleRide->Render( x, y );
	}


}

/////////////////////////////////////////////////////////////////
// CRoleStateIdle
/////////////////////////////////////////////////////////////////
void CRoleStateIdle::Update(float dt)
{
	m_pRole->m_aniRoleIdle->Update( dt );

	m_fTime += dt;
	if( m_fTime > 1.0f || m_pRole->m_bMoving )
	{
		m_pRole->m_Direction = drDown;
		ChangeState( new CRoleStateNormal( m_pRole ) );
		
		return;
	}

	CheckChange();
}

void CRoleStateIdle::Render(float x,float y) const
{
	RenderShadow( x, y );
	m_pRole->m_aniRoleIdle->Render( x, y );
}

/////////////////////////////////////////////////////////////////
// CRoleStateOnTrap
/////////////////////////////////////////////////////////////////
void CRoleStateOnTrap::Update(float dt)
{
	m_pRole->m_aniRole->Update( dt );
	m_pRole->m_aniBigPopo->Update( dt );
	if( !m_pRole->m_aniBigPopo->IsPlaying() ) ChangeState( new CRoleStateTraped( m_pRole ) );
}

void CRoleStateOnTrap::Render(float x,float y) const
{
	RenderShadow( x, y );

	m_pRole->m_aniRole->Render( x, y );
	m_pRole->m_aniBigPopo->Render( x, y );

}

/////////////////////////////////////////////////////////////////
// CRoleStateTraped
/////////////////////////////////////////////////////////////////
void CRoleStateTraped::Update(float dt)
{
	static const float DELTA_SHAKE = 0.1f;
	
	m_fTime += dt;
	if( m_fTime > DELTA_SHAKE )
	{
		m_fTime -= DELTA_SHAKE;
		m_nShake += m_nShakeDr;
		if( m_nShake >= 12 || m_nShake <= 0 )
		{
			m_nShakeDr = -m_nShakeDr;
		}
	}

	m_fDeadTime -= dt;
	if( m_fDeadTime <= 0.0f )
	{
		g_pGame->RoleOnDie( m_pRole );
		ChangeState( new CRoleStateOnDie( m_pRole ) );
		return;
	}

	if( m_pRole->m_bMoving )
	{
		switch( m_pRole->m_Direction )
		{
		case drUp:
			m_pRole->UpdateMoveUp( 0.125f );
			break;
		case drDown:
			m_pRole->UpdateMoveDown( 0.125f );
			break;
		case drLeft:
			m_pRole->UpdateMoveLeft( 0.125f );
			break;
		case drRight:
			m_pRole->UpdateMoveRight( 0.125f );
			break;
		}
	}
	/// m_pRole->m_nPushTime = 0;
	m_pRole->m_bMoving = false;

	for( int i = 0; i< ROLE_NUM; i++ )
	{
		if( &ROLE[i] != m_pRole && 
			ROLE[i].GetX() == m_pRole->m_nCellX && 
			ROLE[i].GetY() == m_pRole->m_nCellY )
		{
			ChangeState( new CRoleStateOnDie( m_pRole ) );
			return;
		}
	}

	m_pRole->m_aniRole->Update( dt );
}

void CRoleStateTraped::Render(float x,float y) const
{
	RenderShadow( x, y );

	m_pRole->m_aniRole->Render( x, y + m_nShake / 2 );
	if( m_fDeadTime <= 1.0f )
	{
		m_pRole->m_aniBigPopo->SetColor( ARGB( 255 - int( m_fDeadTime * 55 ), 255, 255, 255 ) );
	}
	m_pRole->m_aniBigPopo->Render( x, y + m_nShake / 2 );
}

/////////////////////////////////////////////////////////////////
// CRoleStateOnDie
/////////////////////////////////////////////////////////////////
CRoleStateOnDie::CRoleStateOnDie(CRole *pRole) : CRoleState(pRole), m_fTime(0.0f), m_bShow(true)
{
	m_pRole->m_bDead = true;
	m_pRole->m_aniBigPopoBomb->Play();
	m_pRole->m_aniRoleDie->Play();
	g_pGame->RoleDead( m_pRole );
	SOUND.PlaySnd( sndDie );
}

void CRoleStateOnDie::Update(float dt)
{
	static const float DELTA_ONDIE = 0.03f;


	m_pRole->m_aniBigPopoBomb->Update( dt );
	if( !m_pRole->m_aniBigPopoBomb->IsPlaying() )
	{
		m_pRole->m_aniRoleDie->Update( dt );

		m_fTime += dt;
		if( m_fTime > DELTA_ONDIE )
		{
			m_fTime -= DELTA_ONDIE;
			m_bShow = !m_bShow;
		}
	}
	else
	{
		m_pRole->m_aniRole->Update( dt );
	}
	
	if( !m_pRole->m_aniRoleDie->IsPlaying() )
	{
		ChangeState( new CRoleStateDead( m_pRole ) );		
	}
}

void CRoleStateOnDie::Render(float x,float y) const
{	
	if( m_pRole->m_aniBigPopoBomb->IsPlaying() )
	{
		m_pRole->m_aniRole->Render( x, y );
		m_pRole->m_aniBigPopoBomb->Render( x, y );
	}
	else if( m_bShow )
	{
		RenderShadow( x, y );
		m_pRole->m_aniRoleDie->Render( x, y );		
	}
}

/////////////////////////////////////////////////////////////////
// CRoleStateWin
/////////////////////////////////////////////////////////////////
void CRoleStateWin::Update(float dt)
{
	m_pRole->m_aniRoleWin->Update( dt );
}

void CRoleStateWin::Render(float x,float y) const
{
	RenderShadow( x, y );
	m_pRole->m_aniRoleWin->Render( x, y );
}

/////////////////////////////////////////////////////////////////
// CRoleStateOnRide
/////////////////////////////////////////////////////////////////
CRoleStateOnRide::CRoleStateOnRide(CRole *pRole) : CRoleState(pRole), m_fTime(0.0f), m_nAniSeq(0)
{
	switch( m_pRole->m_Ability.Vehicle )
	{
	case vtFastTurtle:
		m_pRole->m_aniFastTurtle->SetTextureRect( 204, 184 + 32 * m_pRole->m_Direction, 48, 32 );
		break;
	case vtSlowTurtle:
		m_pRole->m_aniSlowTurtle->SetTextureRect( 300, 184 + 32 * m_pRole->m_Direction, 48, 32 );
		break;
	case vtFastUFO:
		m_pRole->m_aniFastUFO->SetTextureRect( 100, 184 + 31 * m_pRole->m_Direction, 52, 31 );
		break;
	case vtOwl:
		m_pRole->m_aniOwl->SetTextureRect( 20, 184 + 40 * m_pRole->m_Direction, 40, 40 );
		break;
	}

	m_pRole->m_aniRoleRide->SetFrame( m_pRole->m_Direction );
}

void CRoleStateOnRide::Update(float dt)
{
	static const float DELTA_ONRIDE = 0.025f;

	m_fTime += dt;
	if( m_fTime > DELTA_ONRIDE )
	{
		m_fTime -= DELTA_ONRIDE;
		m_nAniSeq++;
		if( m_nAniSeq >	17 )
		{
			ChangeState( new CRoleStateNormal( m_pRole ) );
			return;
		}
	}
}

void CRoleStateOnRide::Render(float x,float y) const
{
	RenderShadow( x, y );

	switch( m_pRole->m_Ability.Vehicle )
	{
	case vtFastTurtle:		
		m_pRole->m_aniFastTurtle->Render( x, y );
		break;
	case vtSlowTurtle:		
		m_pRole->m_aniSlowTurtle->Render( x, y );
		break;
	case vtOwl:
		m_pRole->m_aniOwl->Render( x, y );
		break;
	}

	if( m_nAniSeq < 11 )
	{
		m_pRole->m_aniRoleRide->Render( x, y - ( m_nAniSeq - 4 ) * 3 );
	}
	else
	{
		m_pRole->m_aniRoleRide->Render( x, y - ( 17 - m_nAniSeq ) * 3 );
	}

	switch( m_pRole->m_Ability.Vehicle )
	{
	case vtFastUFO:
		m_pRole->m_aniFastUFO->Render( x, y );
		break;
	case vtSlowUFO:
		//待加入
		break;
	}
}

/////////////////////////////////////////////////////////////////
// CRoleStateOnDismount
/////////////////////////////////////////////////////////////////
void CRoleStateOnDismount::Update(float dt)
{
	static const float DELTA_ONDISMOUNT = 0.025f;

	m_fTime += dt;
	if( m_fTime > DELTA_ONDISMOUNT )
	{
		m_fTime -= DELTA_ONDISMOUNT;
		m_nAniSeq--;
		if( m_nAniSeq <	1 )
		{
			m_pRole->m_Ability.Vehicle = vtNone;
			m_pRole->m_bFlying = false;
			ChangeState( new CRoleStateNormal( m_pRole ) );
			return;
		}
	}
}

void CRoleStateOnDismount::Render(float x,float y) const
{
	RenderShadow( x, y );

	if( m_nAniSeq % 4 > 1 )
	{
		switch( m_pRole->m_Ability.Vehicle )
		{
		case vtFastTurtle:		
			m_pRole->m_aniFastTurtle->Render( x, y );
			break;
		case vtSlowTurtle:		
			m_pRole->m_aniSlowTurtle->Render( x, y );
			break;
		case vtOwl:
			m_pRole->m_aniOwl->Render( x, y );
			break;
		}
	}

	if( m_nAniSeq > 11 )
	{
		m_pRole->m_aniRoleRide->Render( x, y - ( 18 - m_nAniSeq ) * 3 );
	}
	else
	{
		m_pRole->m_aniRoleRide->Render( x, y - ( m_nAniSeq - 4 ) * 3 );
	}

	if( m_nAniSeq % 4 > 1 )
	{
		switch( m_pRole->m_Ability.Vehicle )
		{
		case vtFastUFO:
			m_pRole->m_aniFastUFO->Render( x, y );
			break;
		case vtSlowUFO:
			//待加入
			break;
		}
	}
}