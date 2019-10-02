#include "factory.h"

CFactory& CFactory::Instance()
{
	static CFactory instance;
	return instance;
}

void CFactory::LoadFixedPrototype(const char *szIniFile)
{
	gstAnimation *ani;
	CPopo *pPopo;
	CGift *pGift;
	char szSection[80];
	int nShare;
	Ability ab;

	g_pGST->System_SetState( GST_INIFILE, szIniFile );
	
	// 加载泡泡		
	pPopo = new CPopo();

	// 加载泡泡主图
	ani = g_pResManager->GetAnimation( g_pGST->Ini_GetString("Popo", "Main", "") );
	ani->Play();
//	m_vFixedAnimation.push_back(ani);
	pPopo->LoadMainAni(ani);

	// 加载泡泡阴影
	ani = g_pResManager->GetAnimation( g_pGST->Ini_GetString("Popo", "Shadow", "") );
	ani->Play();
//    m_vFixedAnimation.push_back(ani);
	pPopo->LoadShadowAni(ani);

	// 将已加载的泡泡放入原型库
	m_vFixedPrototype.push_back(pPopo);
	

	// 加载宝物	
	m_nGiftNum = g_pGST->Ini_GetInt( "Gift", "GiftNum", 0 );

	for( int i = 1; i <= m_nGiftNum; i++ )
	{
		sprintf(szSection, "Gift%d", i);		

		pGift = new CGift();

		// 加载宝物主图
		ani = g_pResManager->GetAnimation( g_pGST->Ini_GetString(szSection, "Main", "") );
		ani->Play();
//		m_vFixedAnimation.push_back(ani);
		pGift->LoadMainAni(ani);

		// 加载宝物阴影
		nShare = g_pGST->Ini_GetInt( szSection, "ShareShadow", 0 );
		if( nShare > 0 && nShare < (int)m_vFixedPrototype.size() &&
			m_vFixedPrototype[nShare] != NULL )
		{
			pGift->LoadShadowAni( ( (CAniItem *)m_vFixedPrototype[nShare] )->GetShadow() );
		}
		else
		{
			ani = g_pResManager->GetAnimation( g_pGST->Ini_GetString( szSection, "Shadow", "" ) );
			ani->Play();
//			m_vFixedAnimation.push_back( ani );
			pGift->LoadShadowAni( ani );
		}

		// 加载宝物属性
		ab.nPopoMax = g_pGST->Ini_GetInt( szSection, "PopoMax", 0 );
		ab.nPopoPower = g_pGST->Ini_GetInt( szSection, "PopoPower", 0 );
		ab.nSpeed = g_pGST->Ini_GetInt( szSection, "Speed", 0 );
		ab.bKick = g_pGST->Ini_GetInt( szSection, "Kick", 0 ) == 0 ? false : true;
		ab.Vehicle = (VehicleType)g_pGST->Ini_GetInt( szSection, "Vehicle", 0 );
		pGift->SetAbility( ab );

		// 将已加载的宝物放入原型库
		m_vFixedPrototype.push_back( pGift );
	}
}

void CFactory::LoadMapPrototype(const char *szIniFile)
{
	enum Type { Medium = 1, MediumAni, Large, Box, Bush, Tent };

	int nShare;
	char szSection[80];
	gstSprite *spr;
	CItem *pItem;
	Type type;
//	int nItemNum;

	g_pGST->System_SetState( GST_INIFILE, szIniFile );

	// 加载地面
	m_aniGround = g_pResManager->GetAnimation( g_pGST->Ini_GetString( "Setup", "Ground", "" ) );

	// 初始化地图物体原型库
	ReleaseMapPrototype();
	m_vMapPrototype.push_back(NULL);

	// 加载地图物体原型	
	m_nItemNum = g_pGST->Ini_GetInt( "Setup", "ItemNum", 0 );

	for (int i = 1; i <= m_nItemNum; i++)
	{
		sprintf(szSection, "Item%d", i);		
		type = (Type)g_pGST->Ini_GetInt( szSection, "Type", 0 );

		// 创建对象
		switch (type)
		{
		case Medium:
			pItem = new CMedium();
			break;
		case MediumAni:
			pItem = new CMediumAni();
			break;
		case Large:
			pItem = new CLarge();
			break;
		case Box:
			pItem = new CBox();
			break;
		case Bush:
			pItem = new CBush();
			break;
		case Tent:
			pItem = new CTent();
			break;
		default:
			continue;
		}

		if( type == Medium || type == Box || type == Large || type == Bush || type == Tent )
		{
			CMedium *pMedium = (CMedium *)pItem;
			// 加载主图
			spr = g_pResManager->GetSprite( g_pGST->Ini_GetString(szSection, "Main", "") );
//			m_vMapSprite.push_back( spr );
			pMedium->LoadMainSprite( spr );

			// 加载阴影
			nShare = g_pGST->Ini_GetInt( szSection, "ShareShadow", 0 );
			if (nShare != 0 && m_vMapPrototype[nShare] != NULL)
			{
				pMedium->LoadShadowSprite( ((CMedium *)m_vMapPrototype[nShare])->GetShadow() );
			}
			else
			{
				if ( g_pGST->Ini_GetString( szSection, "Shadow", "" ) != "NULL" )
				{
					spr = g_pResManager->GetSprite( g_pGST->Ini_GetString(szSection, "Shadow", "" ) );
//					m_vMapSprite.push_back( spr );
					pMedium->LoadShadowSprite( spr );
				}
			}
		}
		
		if( type == MediumAni || type == Large || type == Tent )
		{
			CAniItem *pAni = (CAniItem*)pItem;

			if( type == Large )
			{
				gstAnimation *ani = g_pResManager->GetAnimation( g_pGST->Ini_GetString(szSection, "Ani", "") );
			//	m_vMapSprite.push_back(spr);
				((CLarge *)pAni)->LoadMainAni(ani);
			}
			else if( type == Tent )
			{
				gstAnimation *ani = g_pResManager->GetAnimation( g_pGST->Ini_GetString(szSection, "Main", "") );
				((CTent *)pAni)->LoadMainAni(ani);
			}
		}

		// 将已创建的对象加入地图原型库
		m_vMapPrototype.push_back(pItem);
	}
}

void CFactory::ReleaseFixedPrototype()
{
	for(size_t i = 0; i < m_vFixedPrototype.size(); i++)
	{
		SAFE_DELETE( m_vFixedPrototype[i] );
	}
	m_vFixedPrototype.clear();

	//for(size_t i = 0; i < m_vFixedAnimation.size(); i++)
	//{
	//	SAFE_DELETE( m_vFixedAnimation[i] );
	//}
//	m_vFixedAnimation.clear();
}

void CFactory::ReleaseMapPrototype()
{
	for(size_t i = 0; i < m_vMapPrototype.size(); i++)
	{
		SAFE_DELETE( m_vMapPrototype[i] );
	}
	m_vMapPrototype.clear();

	//SAFE_DELETE( m_aniGround );
	//for(size_t i = 0; i<m_vMapSprite.size(); i++)
	//{
	//	SAFE_DELETE( m_vMapSprite[i] );
	//}
	//m_vMapSprite.clear();
}