#include "gameframe.h"
#include "game.h"
//#include "GUI/GameEngine.h"
#include "resource.h"

CGameFrame *g_pGameFrame;

bool InitGame();
bool GameFrameFunc();
bool GameRenderFunc();

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	//-------------------------------------------------------------
	#ifndef  NDEBUG
     AllocConsole();                  //创建Console窗口
     freopen("CONOUT$", "a", stdout); //使标准输出导向Console窗口
	#endif
	//-------------------------------------------------------------

	printf( "Start!\n\n" );


	if( InitGame() )
	{
		g_pGameFrame->StartGameFrame();
	}

	SAFE_DELETE( g_pGameFrame );
	g_pGST->System_Shutdown();
	g_pGST->Release();

	return 0;
}

bool InitGame()
{
	g_pGameFrame=new CGameFrame();
	if(!g_pGameFrame) return false;

	if(!g_pGameFrame->InitGameFrame(GameFrameFunc,GameRenderFunc)) return false;

	return true;
}

bool GameFrameFunc()
{
	return g_pGameFrame->GameFrameUpdate();
}

bool GameRenderFunc()
{
	return g_pGameFrame->GameFrameRender();
}