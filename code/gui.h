#ifndef _GUI_H
#define _GUI_H

#pragma once

#include "common.h"
#include "gstgui.h"

/*
** CGUIButton
*/
class CGUIButton : public gstGUIObject
{
public:
	CGUIButton(int _id,float _x,float _y,float _w,float _h,
		gstSprite *sprOver=0,gstSprite *sprDown=0,gstSprite *sprDisable=0);
//	virtual ~CGUIButton();

	virtual void Render();
	virtual void MouseOver(bool bOver) { m_bOver=bOver; } 
	virtual bool MouseLButton(bool bDown) { m_bClick=bDown; return !bDown; }
	
protected:
	gstSprite *m_sprOver, *m_sprDown, *m_sprDisable;
	bool m_bClick, m_bOver;
};

/*
** CGUIListbox
*/
struct GUIListboxItem
{
	char				text[64];
	GUIListboxItem	*next;
};

class CGUIListbox : public gstGUIObject
{
public:
	CGUIListbox(int id, float x, float y, float w, float h, gstFont *fnt, DWORD tColor, DWORD thColor, DWORD hColor);
	virtual			~CGUIListbox();

	int				AddItem(char *item);
	void			DeleteItem(int n);
	int				GetSelectedItem() { return nSelectedItem; }
	void			SetSelectedItem(int n) { nSelectedItem=n; }
	int				GetTopItem() { return nTopItem; }
	void			SetTopItem(int n) { if(n>=0 && n<=GetNumItems()-GetNumRows()) nTopItem=n; }

	void			SetRowHigh(float rowhigh) { m_fRowHigh = rowhigh; sprHighlight->SetTextureRect(0,0,sprHighlight->GetWidth(),rowhigh); }
	void			SetRowSpace(float rowspace) { m_fRowSpace = rowspace; }

	char			*GetItemText(int n);
	int				GetNumItems() { return nItems; }
	int				GetNumRows() { return int((rect.y2-rect.y1)/( m_fRowHigh + m_fRowSpace )); }
	void			Clear();

	virtual void	Render();
	virtual bool	MouseMove(float x, float y) { mx=x; my=y; return false; }
	virtual bool	MouseLButton(bool bDown);
	virtual bool	MouseWheel(int nNotches);
	virtual bool	KeyClick(int key, int chr);

private:
	gstSprite		*sprHighlight;
	gstFont			*font;
	DWORD			textColor, texthilColor;

	float			m_fRowHigh;
	float			m_fRowSpace;

	int					nItems, nSelectedItem, nTopItem;
	float				mx, my;
	GUIListboxItem	*pItems;
};

/*
** CGUICheckButton
*/
class CGUICheckButton : public gstGUIObject
{
public:
	CGUICheckButton(int _id,float x,float y,float w,float h,gstSprite *sprDown,gstSprite *sprChecked);
	virtual ~CGUICheckButton();

	virtual void Render();
	virtual bool MouseLButton(bool bDown);

	void SetState(bool bCheck) { m_bChecked=bCheck; }
	bool GetState() { return m_bChecked; }

protected:
	bool m_bChecked;
	bool m_bDown;
	gstSprite *m_sprDown;
	gstSprite *m_sprChecked;
};

#endif