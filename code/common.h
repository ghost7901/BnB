#ifndef _COMMON_H
#define _COMMON_H

#pragma once

#include <gstresource.h>

#define BTN_RETURN 1

#define SAFE_DELETE(p) { if(p) { delete (p); (p)=NULL; } }

extern GST *g_pGST;
extern gstResourceManager *g_pResManager;
extern char g_SysPath[MAX_PATH];

bool resManagerCreate();

#endif