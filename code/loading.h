#ifndef _LOADING_H
#define _LOADING_H

#include <gst.h>

class gstSprite;

class CLoading
{
public:
	CLoading();
	~CLoading();

	int Update();
	void Render();

private:
	HTEXTURE m_hTex1;
	HTEXTURE m_hTex2;
	gstSprite *m_sprBg;
	gstSprite *m_sprLoading;
	gstSprite *m_sprHead;
	gstSprite *m_sprProcess;

	int m_nProcess;
};

#endif