#include "common.h"
#include "stdio.h"

GST *g_pGST = NULL;
gstResourceManager *g_pResManager = NULL;
char g_SysPath[MAX_PATH];

bool resManagerCreate()
{
	GetCurrentDirectory( MAX_PATH, g_SysPath );
	//sprintf( g_SysPath, "%s\\work", g_SysPath );

	if( g_pResManager ) return true;
	g_pResManager=new gstResourceManager( "data/resource.txt" );
	g_pResManager->CacheBegin();
	return g_pResManager ? true : false;
}