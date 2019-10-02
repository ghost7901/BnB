#include "gui.h"

/***********************************************
*    CGUIButton
***********************************************/
CGUIButton::CGUIButton(int _id,float _x,float _y,float _w,float _h,gstSprite *sprOver,gstSprite *sprDown,gstSprite *sprDisable)
{
	id=_id;
	bStatic=false;
	bVisible=true;
	bEnabled=true;

	m_bOver=false;
	m_bClick=false;

	m_sprOver = sprOver;
	m_sprDisable = sprDisable;
	m_sprDown = sprDown;

	rect.Set(_x,_y,_x+_w,_y+_h);
}

void CGUIButton::Render()
{
	if( bEnabled )
	{
		if( m_bClick )
		{
			if( m_sprDown ) m_sprDown->Render( rect.x1, rect.y1 );
		}
		else if( m_bOver )
		{
			if( m_sprOver ) m_sprOver->Render( rect.x1,rect.y1 );
		}
	}
	else if( m_sprDisable )
	{
		m_sprDisable->Render( rect.x1, rect.y1 );
	}
}

/***********************************************
*    CGUIListbox
***********************************************/
CGUIListbox::CGUIListbox(int _id, float x, float y, float w, float h, gstFont *fnt, DWORD tColor, DWORD thColor, DWORD hColor)
{
	id=_id;
	bStatic=false;
	bVisible=true;
	bEnabled=true;
	rect.Set(x, y, x+w, y+h);
	font=fnt;
	m_fRowHigh = fnt ? fnt->GetHeight() : 0;
	m_fRowSpace = 0;
	sprHighlight=new gstSprite(0, 0, 0, w, m_fRowHigh);
	sprHighlight->SetColor(hColor);
	textColor=tColor;
	texthilColor=thColor;
	pItems=0;
	nItems=0;

	nSelectedItem=0;
	nTopItem=0;
	mx=0; my=0;
}

CGUIListbox::~CGUIListbox()
{
	Clear();
	if(sprHighlight) delete sprHighlight;
}


int CGUIListbox::AddItem(char *item)
{
	GUIListboxItem *pItem=pItems, *pPrev=0, *pNew;

	pNew = new GUIListboxItem;
	memcpy(pNew->text, item, min(sizeof(pNew->text), strlen(item)+1));
	pNew->text[sizeof(pNew->text)-1]='\0';
	pNew->next=0;

	while(pItem) { pPrev=pItem;	pItem=pItem->next; }

	if(pPrev) pPrev->next=pNew;
	else pItems=pNew;
	nItems++;

	return nItems-1;
}

void CGUIListbox::DeleteItem(int n)
{
	GUIListboxItem *pItem=pItems, *pPrev=0;

	if(n<0 || n>=GetNumItems()) return;

	int i=0;
	while(pItem && i++ != n) { pPrev=pItem;	pItem=pItem->next; }

	if(pPrev) pPrev->next=pItem->next;
	else pItems=pItem->next;
	delete pItem;
	nItems--;
}

char *CGUIListbox::GetItemText(int n)
{
	int i;
	GUIListboxItem *pItem=pItems;

	if(n<0 || n>=GetNumItems()) return 0;

	for(i=0;i<n;i++) pItem=pItem->next;

	return pItem->text;
}

void CGUIListbox::Clear()
{
	GUIListboxItem *pItem=pItems, *pNext;

	while(pItem)
	{
		pNext=pItem->next;
		delete pItem;
		pItem=pNext;
	}
	
	pItems=0;
	nItems=0;
}

void CGUIListbox::Render()
{
	int i;
	GUIListboxItem *pItem=pItems;

	for(i=0;i<nTopItem;i++) pItem=pItem->next;
	for(i=0;i<GetNumRows();i++)
	{
		if(i>=nItems) return;

		if(nTopItem+i == nSelectedItem)
		{
			sprHighlight->Render(rect.x1,rect.y1+i * ( m_fRowHigh + m_fRowSpace ) );
			if( font ) font->SetColor(texthilColor);
		}
		else
			if( font ) font->SetColor(textColor);

		if( font ) font->Render(rect.x1+3, rect.y1+i * ( m_fRowHigh + m_fRowSpace ), GSTTEXT_LEFT, pItem->text);
		pItem=pItem->next;
	}
}

bool CGUIListbox::MouseLButton(bool bDown)
{
	int nItem;

	if(bDown)
	{
		nItem=nTopItem+int(my)/int( m_fRowHigh + m_fRowSpace );
		if(nItem<nItems)
		{
			nSelectedItem=nItem;
			return true;
		}
	}
	return false;
}


bool CGUIListbox::MouseWheel(int nNotches)
{
	nTopItem-=nNotches;
	if(nTopItem<0) nTopItem=0;
	if(nTopItem>GetNumItems()-GetNumRows()) nTopItem=GetNumItems()-GetNumRows();

	return true;
}

bool CGUIListbox::KeyClick(int key, int chr)
{
	switch(key)
	{
		case GSTK_DOWN:
			if(nSelectedItem < nItems-1)
			{
				nSelectedItem++;
				if(nSelectedItem > nTopItem+GetNumRows()-1) nTopItem=nSelectedItem-GetNumRows()+1;
				return true;
			}
			break;

		case GSTK_UP:
			if(nSelectedItem > 0)
			{
				nSelectedItem--;
				if(nSelectedItem < nTopItem) nTopItem=nSelectedItem;
				return true;
			}
			break;
	}
	return false;
}

/***********************************************
*    CGUICheckButton
***********************************************/
CGUICheckButton::CGUICheckButton(int _id,float x,float y,float w,float h,gstSprite *sprDown,gstSprite *sprChecked)
{
	id = _id;
	m_bChecked = true;
	m_bDown = false;

	bStatic = false;
	bVisible = true;
	bEnabled = true;

	rect.Set( x, y, x + w, y + h );

	m_sprDown = sprDown;
	m_sprChecked = sprChecked;
}

CGUICheckButton::~CGUICheckButton()
{
}

bool CGUICheckButton::MouseLButton(bool bDown)
{
	 if( !bDown )
	 {
		 m_bChecked = !m_bChecked;
	 }
	 m_bDown = bDown;
	 
	 return !bDown;
}

void CGUICheckButton::Render()
{
	if( m_bDown )
		m_sprDown->Render( rect.x1, rect.y1 );
	else if( m_bChecked )
		m_sprChecked->Render( rect.x1, rect.y1 );
}