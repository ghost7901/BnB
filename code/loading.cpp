#include "loading.h"
#include "common.h"

CLoading::CLoading() : m_nProcess( 0 )
{
	m_hTex1 = g_pGST->Texture_Load( ".\\loading.png" );
	m_sprBg = new gstSprite( m_hTex1, 0, 0, 364, 71 );
	m_sprLoading = new gstSprite( m_hTex1, 0, 72, 106, 14 );
	m_sprHead = new gstSprite( m_hTex1, 0, 87, 51, 24 );

	m_hTex2 = g_pGST->Texture_Load( ".\\process.png" );
	m_sprProcess = new gstSprite( m_hTex2, 0, 0, 2, 6 );

	g_pResManager->CacheBegin();
}

CLoading::~CLoading()
{
	SAFE_DELETE( m_sprBg );
	SAFE_DELETE( m_sprLoading );
	SAFE_DELETE( m_sprHead );
	SAFE_DELETE( m_sprProcess );

	g_pGST->Texture_Free( m_hTex1 );
	g_pGST->Texture_Free( m_hTex2 );
}

int CLoading::Update()
{
	m_nProcess = g_pResManager->CacheProcess();
	return m_nProcess;
}

void CLoading::Render()
{
	m_sprBg->Render( 218, 450 );

	m_sprProcess->RenderStretch( 238, 487, 238 + 321 * m_nProcess / 100.0f, 500 );
	
	m_sprHead->SetFlip( false, false );
	m_sprHead->Render( 193, 481 );
	m_sprHead->SetFlip( true, false );
	m_sprHead->Render( 553, 481 );

	m_sprLoading->Render( 347, 520 );
}



