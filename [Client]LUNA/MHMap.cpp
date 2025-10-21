// MHMap.cpp: implementation of the CMHMap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MHMap.h"

#include "GameResourceManager.h"
#include "TileManager.h"
#include "MHFile.h"
//#include "Mp3/BGMLoader.h"
#include "ObjectManager.h"
#include "MainGame.h"
#include "Engine/Engine.h"
#include "GameIn.h"
#include "./Audio/MHAudioManager.h"
#include "MHCamera.h"
//#include "./mp3/BGMLoader.h"



#include "MiniMapDlg.h"
#include "QuestManager.h"
#include "ItemManager.h"

// desc_hseos_����ý���_01
// S ����ý��� �߰� added by hseos 2007.05.02
#include "../hseos/Farm/SHFarmManager.h"
// E ����ý��� �߰� added by hseos 2007.05.02
#include "CWayPointManager.h"
#include "MHTimeManager.h"
#include "BigMapDlg.h"
#include "GMToolManager.h"

// 090907 ShinJS --- ��ã�� ���� Data
DWORD WINAPI PathFind_Thread(LPVOID p_Param);
BOOL PathFind_Simple();								// 1��� ��ã��
BOOL PathFind_Quickly();							// 3/4 ��� ��ã��
VECTOR3 g_PathFindStart;
VECTOR3 g_PathFindEnd;
CTileManager* g_TileManager = NULL;
CWayPointManager* g_WayPointManager = NULL;
BOOL g_SimpleMode = TRUE;
// 091204 ShinJS --- ��ã�� ������ Ȱ�� ���θ� ������ ����
volatile LONG g_PathThreadActivated = 0;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
GLOBALTON(CMHMap)
CMHMap::CMHMap()
{
	sky = NULL;
	m_pTileManager = NULL;
	mapDesc = NULL;
	m_bIsInited = FALSE;
	m_CloudTable.Initialize(64);
	
	m_bVillage = FALSE;

	m_dwQuestNpcId = 1000;

	m_bRenderTileData = FALSE;
}

CMHMap::~CMHMap()
{
	SAFE_DELETE(m_pTileManager);
	SAFE_DELETE(mapDesc);
}

void CMHMap::InitMap(MAPTYPE MapNum)
{
	m_dwQuestNpcId = 1000;
	m_bIsInited = TRUE;
	m_MapNum = MapNum;
	int farmZone = 0;

	// 090921 LUJ, ���� ���� ��쿡 �ε��ϵ��� �Ѵ�	
	if( g_csFarmManager.GetFarmZoneID(
		MapNum,
		&farmZone ) )
	{
		MAINGAME->GetEngine()->GetGraphicsEngine()->LoadPack(
			"data/3dData/farm.pak");
	}

	// 090623 LUJ, �ʿ� ���� �ε��� pak������ �����Ѵ�
	switch( MapNum )
	{
	case AlkerResidence:
		{
			MAINGAME->LoadPack(
				"data/3dData/housing.pak");
			break;
		}
	}

	//LOGFILE("LoadMapDesc(MapNum)");
	LoadMapDesc(MapNum);
	
	DIRECTORYMGR->SetLoadMode(eLM_Map);
	{
		// �ӽ�
		//LOGFILE("map.Init(mapDesc->MapFileName)");
		BOOL rt;
		rt = map.Init(mapDesc->MapFileName);
		
		//LOGFILE("SAFE_DELETE(sky)");
		SAFE_DELETE(sky);
		if(mapDesc->bSky)
		{
			//LOGFILE("sky");
			sky = new CEngineSky;
			sky->CreateSky(mapDesc->SkyMod,mapDesc->SkyAnm, &mapDesc->SkyOffset);
		}
		
		
		//LOGFILE("m_pTileManager");
		DIRECTORYMGR->SetLoadMode(eLM_Resource);

		SAFE_DELETE(m_pTileManager);
		m_pTileManager = new CTileManager;
		if (m_pTileManager->LoadTileInfo(mapDesc->TileFileName) == FALSE)
			SAFE_DELETE(m_pTileManager);
		DIRECTORYMGR->SetLoadMode(eLM_Map);
	}
	DIRECTORYMGR->SetLoadMode(eLM_Root);

	//LOGFILE("ApplyMapDesc()");
	ApplyMapDesc();

	// ���� �̸� �ε�
//	PreLoadData();
	LoadPreData(MapNum);
}

/*
void CMHMap::PreLoadData()
{
	BASE_MONSTER_LIST* pMonsterInfo = GAMERESRCMNGR->GetMonsterListInfo(13);

}
*/

void CMHMap::LoadPreData(MAPTYPE MapNum)
{	
	sPRELOAD_INFO* pPreLoadInfo = GAMERESRCMNGR->GetPreDataInfo(MapNum);
	if(!pPreLoadInfo)		return;
	
	//
	BASE_MONSTER_LIST* pBMonInfo = NULL;

	//----------------------------------------------------------------
	// Monster
	//----------------------------------------------------------------
	DIRECTORYMGR->SetLoadMode(eLM_Monster);

	for(int i=0; i<pPreLoadInfo->Count[ePreLoad_Monster]; i++)
	{
		pBMonInfo = GAMERESRCMNGR->GetMonsterListInfo(pPreLoadInfo->Kind[ePreLoad_Monster][i]);
		if(pBMonInfo)
			CEngineObject::PreLoadObject(pBMonInfo->ChxName);
	}

	//----------------------------------------------------------------
	// Item
	//----------------------------------------------------------------
	DIRECTORYMGR->SetLoadMode(eLM_Character);

	MOD_LIST* pModList_Elf_Man = &GAMERESRCMNGR->m_ModFileList[RaceType_Elf][GENDER_MALE];
	MOD_LIST* pModList_Elf_Woman = &GAMERESRCMNGR->m_ModFileList[RaceType_Elf][GENDER_FEMALE];
	StaticString* pString = NULL;
	int PartModelNum = 0;
	int i;
	for(i=0; i<pPreLoadInfo->Count[ePreLoad_Item]; i++)
	{		
		PartModelNum = ITEMMGR->GetItemInfo(pPreLoadInfo->Kind[ePreLoad_Item][i])->Part3DModelNum;

		// Man
		pString = &pModList_Elf_Man->ModFile[PartModelNum];
		if(pString == 0)
		{
			char buf[128];
			sprintf(buf, "No Name ModList! Plz Check again.ItemNum : %d", pPreLoadInfo->Kind[ePreLoad_Item][i]);
			ASSERTMSG(0, buf);
		}			
		if(pString->Str)
			CEngineObject::PreLoadObject(pString->Str);

		// Woman
		pString = &pModList_Elf_Woman->ModFile[PartModelNum];
		if(pString->Str)
			CEngineObject::PreLoadObject(pString->Str);
	}
	
	//----------------------------------------------------------------
	// Mod
	//----------------------------------------------------------------
	PRELOAD* pMod = NULL;
	cPtrList* pList = GAMERESRCMNGR->GetPreLoadModlist();

	PTRLISTPOS pos = pList->GetHeadPosition();
	while(pos)
	{
		pMod = (PRELOAD*)pList->GetNext(pos);
		if(pMod)
		{
			CEngineObject::PreLoadObject(pMod->FileName.Str);
		}
	}

	//----------------------------------------------------------------
	// Effect
	//----------------------------------------------------------------
	DIRECTORYMGR->SetLoadMode(eLM_Effect);

	PRELOAD* pEff = NULL;
	pList = GAMERESRCMNGR->GetPreLoadEfflist();

	pos = pList->GetHeadPosition();
	while(pos)
	{
		pEff = (PRELOAD*)pList->GetNext(pos);
		if(pEff)
		{
			g_pExecutive->PreLoadGXObject(pEff->FileName.Str);			
		}
	}

	///////////////////////////////////////////////////////////////////////////////
	// 06. 04. PreLoadData �߰���� - �̿���
	// ������ PreLoad
	WORD* ItemList = GAMERESRCMNGR->GetPreLoadItemlist();

	if(ItemList)
	{
		WORD count = ItemList[0];

		for(WORD cnt = 1; cnt < count + 1; cnt++)
		{
			PartModelNum = ITEMMGR->GetItemInfo(ItemList[cnt])->Part3DModelNum;

			// Man
			pString = &pModList_Elf_Man->ModFile[PartModelNum];
			if(pString == 0)
			{
				char buf[128];
				sprintf(buf, "No Name ModList! Plz Check again.ItemNum : %d", pPreLoadInfo->Kind[ePreLoad_Item][i]);
				ASSERTMSG(0, buf);
			}			
			if(pString->Str)
				CEngineObject::PreLoadObject(pString->Str);

			// Woman
			pString = &pModList_Elf_Woman->ModFile[PartModelNum];
			if(pString->Str)
				CEngineObject::PreLoadObject(pString->Str);
		}
	}
	///////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////
	// 06. 05. PreLoadData �߰���� - �̿���
	// ����Ʈ PreLoad
	sPRELOAD_EFFECT_INFO* pPreLoadEffectInfo = GAMERESRCMNGR->GetPreEffectInfo(MapNum);
	if(!pPreLoadEffectInfo)		return;

	PRELOAD* pEffect = NULL;
	pList = &(pPreLoadEffectInfo->Effect);

	pos = pList->GetHeadPosition();
	while(pos)
	{
		pEffect = (PRELOAD*)pList->GetNext(pos);
		if(pEffect)
		{
			CEngineObject::PreLoadObject(pEffect->FileName.Str);
		}
	}
	///////////////////////////////////////////////////////////////////////////////

	DIRECTORYMGR->SetLoadMode(eLM_Root);
}

void CMHMap::ApplyMapDesc()
{	
	MAINGAME->GetEngine()->GetGraphicsEngine()->SetBackGroundColor(mapDesc->backColor);
	MAINGAME->GetEngine()->GetGraphicsEngine()->SetFixHeight(mapDesc->bFixHeight,mapDesc->FixHeight);
	
	bRenderSky = mapDesc->bSky;

	g_pExecutive->GetGeometry()->SetAmbientColor(0,mapDesc->Ambient);

	if(mapDesc->fogdesc.bEnable)
	{
		g_pExecutive->GetGeometry()->EnableFog(0);
		g_pExecutive->GetGeometry()->SetFog(&mapDesc->fogdesc,0);
	}
	else
	{
		g_pExecutive->GetGeometry()->DisableFog(0);
	}

	VECTOR3 pos = {0};
	g_pExecutive->GetGeometry()->ResetCamera(&pos,CAMERA_NEAR,mapDesc->DefaultSight,DEGTORAD(mapDesc->Fov),0);
	CAMERA->SetMaxSightDistance( mapDesc->DefaultSight );
	CAMERA->SetCameraFov( DEGTORAD(mapDesc->Fov) );
	AUDIOMGR->PlayBGM( mapDesc->BGMSoundNum );

	g_pExecutive->GetGeometry()->EnableDirectionalLight(0);
	
	DIRECTIONAL_LIGHT_DESC LightDesc = {0};
	LightDesc.dwDiffuse = 0xffffffff;
	LightDesc.dwSpecular = 0xffffffff;
	LightDesc.v3Dir = mapDesc->SunPos * -1.f;
	LightDesc.bEnable = TRUE;

	g_pExecutive->GetGeometry()->SetDirectionalLight(&LightDesc,0);
	
	if(GAMERESRCMNGR->m_GameDesc.bShadow == MHSHADOW_DETAIL)
	{
		VECTOR3 to = {0};
		float len = VECTOR3Length(&m_SunPosForShadow);
		m_Shadowlight.InitShadowLight(0,&pos,&to,gPHI/16, len + 1000);
	}
	
	if (mapDesc->bHField == FALSE)
	{
		g_pExecutive->SetRenderMode(g_pExecutive->GetRenderMode()&(~RENDER_MODE_HFIELD));
	}
	else
	{
		g_pExecutive->SetRenderMode(g_pExecutive->GetRenderMode()|RENDER_MODE_HFIELD);
	}
	
	if(mapDesc->bSun)
	{
		DIRECTORYMGR->SetLoadMode(eLM_Map);
		m_SunObject.Init(mapDesc->SunObject,NULL,eEngineObjectType_Effect);
		DIRECTORYMGR->SetLoadMode(eLM_Root);
	}
	
	if(mapDesc->CloudNum)
	{
		CEngineCloud::LoadCloudList(mapDesc->CloudListFile);
		CEngineCloud::SetCloudHeight(mapDesc->CloudHMin,mapDesc->CloudHMax);
		VECTOR3 vel;
		vel.x = 20;
		vel.y = 0;
		vel.z = 10;
		CEngineCloud::SetCloudVelocity(&vel);
		ASSERT(CEngineCloud::GetMaxCloudKindNum() != 0);
		DWORD numperside = (DWORD)sqrt((double)mapDesc->CloudNum);
		DWORD n=0;
		float sizeperside = 51200.f / numperside;
		for(DWORD x=0;x<numperside;++x)
		{
			for(DWORD z=0;z<numperside;++z)
			{
				CEngineCloud* pCloud = new CEngineCloud;
				float fx = x*sizeperside + rand()%(DWORD(sizeperside));
				float fz = z*sizeperside + rand()%(DWORD(sizeperside));
				pCloud->Init(rand()%pCloud->GetMaxCloudKindNum(),fx,fz);
				m_CloudTable.Add(pCloud,n++);
			}
		}
	}
}

void CMHMap::Release()
{
	AUDIOMGR->StopBGM();

	m_CloudTable.SetPositionHead();
	while(CEngineCloud* pCloud = m_CloudTable.GetData())
	{
		delete pCloud;
	}
	m_CloudTable.RemoveAll();
	CEngineCloud::ReleaseCloudList();

	m_bIsInited = FALSE;
	ProcessGarbageObject();
	m_Shadowlight.Release();
	SAFE_DELETE(mapDesc);
	SAFE_DELETE(sky);
	m_SunObject.Release();

	map.Release();
	
	SAFE_DELETE(m_pTileManager);

}

void CMHMap::Process(DWORD CurTime)
{	
	if( !m_bIsInited ) return;
	
	static DWORD SumTick = 0;
	static DWORD LastTime = 0;
	static DWORD Tick = 0;

	if(LastTime)
	{
		Tick = CurTime - LastTime;
		LastTime = CurTime;
	}
	else
	{
		LastTime = CurTime;
		return;
	}


	if(mapDesc->bSky && sky)
	{
		SumTick += Tick;
		if(SumTick > 50)
		{
			sky->IncreaseSkyAniFrame(1);
			SumTick -= 50;
		}
	
		sky->RenderSky();
	}
/*
	if(GAMERESRCMNGR->m_GameDesc.bShadow == MHSHADOW_DETAIL)	//
	{
		VECTOR3 pos;
		pos = m_ShadowPivotPos + mapDesc->SunPos;
		m_Shadowlight.SetPosition(&pos);
		LIGHT_DESC desc;
		m_Shadowlight.GetLightDesc(&desc);
		desc.v3Point = pos;
		desc.v3To = m_ShadowPivotPos;
		m_Shadowlight.SetLightDesc(&desc);
	}
*/	

	if(mapDesc->bSun)
	{
		VECTOR3 pos;
		Normalize(&pos,&mapDesc->SunPos);
		pos = pos * mapDesc->SunDistance;
		pos = m_ShadowPivotPos;// + pos;
		pos.x += 10000;
		pos.y -= 2000;
		m_SunObject.SetEngObjPosition(&pos);
	}
	
	
}

DWORD CMHMap::GetMultipleObjectWithRay(PICK_GXOBJECT_DESC* pPickDesc, DWORD dwMaxDescNum, VECTOR3* pv3Pos, VECTOR3* pv3Dir, DWORD dwFlag)
{
	ASSERT(pPickDesc);

	return map.GetMultipleObjectWithRay(pPickDesc, dwMaxDescNum, pv3Pos, pv3Dir, dwFlag);
}

DWORD CMHMap::GetTileWidth()
{
	if(m_pTileManager)
		return m_pTileManager->GetTileWidth();
	return 100;
}
BOOL CMHMap::CollisionLine(VECTOR3* pStart,VECTOR3* pEnd, VECTOR3* Target, MAPTYPE MapNum,CObject* pObject)
{
	if(m_pTileManager == NULL)
		return FALSE;

	return m_pTileManager->CollisionLine(pStart, pEnd, Target, MapNum,pObject);
}

BOOL CMHMap::NonCollisionLine(VECTOR3* pStart, VECTOR3* pEnd, VECTOR3* Target, MAPTYPE MapNum, CObject* pObject)
{
	if(m_pTileManager == NULL)
		return FALSE;

	return m_pTileManager->NonCollisionLine(pStart, pEnd, Target, MapNum, pObject);
}

BOOL CMHMap::IsInTile(int cellX, int cellY, MAPTYPE MapNum,CObject* pObject)
{
	if(m_pTileManager == NULL)
		return FALSE;

	if(m_pTileManager->IsInTile(cellX, cellY, MapNum, pObject))
	{
		return TRUE;
	}
	return FALSE;
}
BOOL CMHMap::CollisionTileCell(int x, int y, MAPTYPE MapNum,CObject* pObject)
{
	if(m_pTileManager == NULL)
		return FALSE;

	if(m_pTileManager->CollisionTile(x, y, MapNum,pObject))
	{
		return FALSE;
	}
	return TRUE;
}
BOOL CMHMap::CollisionTilePos(int x, int y, MAPTYPE MapNum,CObject* pObject)
{
	if(m_pTileManager == NULL)
		return FALSE;

	if(m_pTileManager->CollisionTile(int(x/ TILECOLLISON_DETAIL), int(y/ TILECOLLISON_DETAIL), MapNum, pObject))
	{
		return TRUE;
	}
	return FALSE;
}
BOOL CMHMap::CollisonCheck(VECTOR3* pStart,VECTOR3* pEnd,VECTOR3* pRtCollisonPos,CObject* pObj)
{
	if(m_pTileManager == NULL)
		return FALSE;
	if(m_pTileManager->CollisonCheck(pStart,pEnd,pRtCollisonPos,pObj) == TRUE)
		return TRUE;


	return FALSE;
}

void CMHMap::SetShadowPivotPos(VECTOR3* pPivotPos)
{
	m_ShadowPivotPos = *pPivotPos;
		
	VECTOR3 pos;
//	pos = m_ShadowPivotPos + mapDesc->SunPos;
	pos = m_ShadowPivotPos + m_SunPosForShadow;
	
	m_Shadowlight.SetPosition(&pos);
	LIGHT_DESC desc;
	m_Shadowlight.GetLightDesc(&desc);
	desc.v3Point = pos;
	desc.v3To = m_ShadowPivotPos;
	m_Shadowlight.SetLightDesc(&desc);
}

BOOL CMHMap::LoadMapDesc(MAPTYPE MapNum)
{	
	char mapdescfile[256];
	CMHFile file;
	sprintf(mapdescfile,"data\\script\\mapset\\MAP%d.bmhm",MapNum);
	if(file.Init(mapdescfile,"rb") == FALSE)
		return FALSE;
	
	char value[64] = {0,};
	SAFE_DELETE(mapDesc);
	mapDesc = new MAPDESC;

	while(1)
	{
		if(file.IsEOF())
			break;

		strcpy(value,strupr(file.GetString()));

		if(strcmp(value,"*SIGHT") == 0)
		{
			mapDesc->DefaultSight = file.GetFloat();
		}
		else if(strcmp(value,"*FOV") == 0)
		{
			mapDesc->Fov = file.GetFloat();
		}
		else if(strcmp(value,"*FOG") == 0)
		{
			mapDesc->fogdesc.bEnable = file.GetBool();
		}
		else if(strcmp(value,"*BRIGHT") == 0)
		{
			DWORD dd;
			dd = file.GetDword();
			mapDesc->Ambient = RGBA_MAKE(dd,dd,dd,dd);
		}
		else if(strcmp(value,"*FOGCOLOR") == 0)
		{
			DWORD r,g,b,a;
			r = file.GetDword();
			g = file.GetDword();
			b = file.GetDword();
			a = file.GetDword();
			mapDesc->fogdesc.dwColor = RGBA_MAKE(r,g,b,a);
		}
		else if(strcmp(value,"*FOGDENSITY") == 0)
		{
			mapDesc->fogdesc.fDensity = file.GetFloat();
		}
		else if(strcmp(value,"*FOGSTART") == 0)
		{
			mapDesc->fogdesc.fStart = file.GetFloat();
		}
		else if(strcmp(value,"*FOGEND") == 0)
		{
			mapDesc->fogdesc.fEnd = file.GetFloat();
		}
		else if(strcmp(value,"*MAP") == 0)
		{
			file.GetString(mapDesc->MapFileName);
		}
		else if(strcmp(value,"*TILE") == 0)
		{
			char buf[64];
			file.GetString(buf);
			sprintf( mapDesc->TileFileName, "%s%s", "../map/", buf );
		}
		else if(strcmp(value,"*SKYMOD") == 0)
		{
			file.GetString(mapDesc->SkyMod);
		}
		else if(strcmp(value,"*SKYANM") == 0)
		{
			file.GetString(mapDesc->SkyAnm);
		}
		else if(strcmp(value,"*SKYBOX") == 0)
		{
			mapDesc->bSky = file.GetBool();
		}
		else if(strcmp(value,"*BGM") == 0)
		{
			mapDesc->BGMSoundNum = file.GetWord();
		}
		else if(strcmp(value,"*COLOR") == 0)
		{
			BYTE r = file.GetByte();
			BYTE g = file.GetByte();
			BYTE b = file.GetByte();
			mapDesc->Ambient = RGBA_MAKE(r,g,b,255);
		}
		else if(strcmp(value,"*SUNPOS") == 0)
		{
			mapDesc->SunPos.x = file.GetFloat();
			mapDesc->SunPos.y = file.GetFloat();
			mapDesc->SunPos.z = file.GetFloat();

			float dist = VECTOR3Length( &mapDesc->SunPos );
			m_SunPosForShadow = mapDesc->SunPos / dist * 2000;	//�Ÿ��� õ? //�ݰ溸�� ª�ƾ�?
			m_SunPosForShadow.y += 1000;
			
		}
		else if(strcmp(value,"*SUNOBJECT") == 0)
		{
			file.GetString(mapDesc->SunObject);
		}
		else if(strcmp(value,"*SUN") == 0)
		{
			mapDesc->bSun = file.GetBool();
		}
		else if(strcmp(value,"*SUNDISTANCE") == 0)
		{
			mapDesc->SunDistance = file.GetFloat();
		}
		else if(strcmp(value,"*BACKCOLOR") == 0)
		{
			DWORD r,g,b,a;
			r = file.GetDword();
			g = file.GetDword();
			b = file.GetDword();
			a = file.GetDword();
			mapDesc->backColor = RGBA_MAKE(r,g,b,a);
		}
		else if(strcmp(value,"*FIXHEIGHT") == 0)
		{
			mapDesc->bFixHeight = TRUE;
			mapDesc->FixHeight = file.GetFloat();			
		}
		else if(strcmp(value,"*CLOUD") == 0)
		{
			mapDesc->CloudNum = file.GetDword();			
		}
		else if(strcmp(value,"*CLOUDLIST") == 0)
		{
			file.GetString(mapDesc->CloudListFile);
		}
		else if(strcmp(value,"*CLOUDHEIGHT") == 0)
		{
			mapDesc->CloudHMin = file.GetInt();
			mapDesc->CloudHMax = file.GetInt();
		}	
		//trustpak 2005/04/15
		else if(strcmp(value, "*SKYOFFSET") == 0)
		{
			mapDesc->SkyOffset.x = file.GetFloat();
			mapDesc->SkyOffset.y = file.GetFloat();
			mapDesc->SkyOffset.z = file.GetFloat();
		}
		else if(strcmp(value, "*HFIELD") == 0)
		{
			mapDesc->bHField = file.GetInt();
		}
		///
	}
	
	file.Release();

	return TRUE;
}

void CMHMap::LoadNpcForQuestDialog()
{
	CMHFile file ;

	char filename[100];


    sprintf(filename, "System/Resource/%s", GAMERESRCMNGR->GetTranslationPath("StaticNpc.bin"));

	file.Init(filename, "rb") ;

	if( file.IsInited() == FALSE )
	{
		ASSERTMSG(0, "����ƽ ������ �ε��ϴµ� �����ߴ�~!!") ;
		return ;
	}

	WORD nCount = 0 ;

	stNPCIDNAME NpcIdName ;

	while(TRUE)
	{
		if( file.IsEOF() )
		{
			break ;
		}

		memset(&NpcIdName, 0, sizeof(&NpcIdName)) ;

		file.GetInt() ;

		file.GetInt() ;

		SafeStrCpy(NpcIdName.name, file.GetString(), MAX_NPC_NAME_LENGTH+1) ;

		NpcIdName.wId = file.GetWord() ;

		QUESTMGR->SetNpcIDName(nCount, &NpcIdName) ;

		for( int count = 0 ; count < 3 ; ++count )
		{
			file.GetInt() ;
		}

		file.GetByte();

		++nCount ;
	}
}


void CMHMap::LoadStaticNpc(MAPTYPE MapNum)											// ����ƽ npc�� �ε��ϴ� �Լ�.
{
	MAPTYPE npcMapnum = 0 ;															// npc �� ��ȣ�� ���� ������ �����Ѵ�.

	char value[256] = {0,};															// �ӽ� ���۸� �����ϰ� �ʱ�ȭ �Ѵ�.

	CMHFile file;																	// ���������� �����Ѵ�.
    	char filename[100];
	sprintf(filename, "System/Resource/%s", GAMERESRCMNGR->GetTranslationPath("StaticNpc.bin"));

	file.Init(filename, "rb") ;						// ����ƽ npc ������ �д´�.

	if(file.IsInited() == FALSE)													// �б⿡ �����ϸ�,
		return;																		// ����ó���� �Ѵ�.

	NPC_TOTALINFO tinfo;															// npc ��Ż ������ ���� ����ü�� �����Ѵ�.
	BASEOBJECT_INFO binfo;															// �⺻ ������Ʈ ������ ���� ����ü�� �����Ѵ�.
	BASEMOVE_INFO minfo;															// �⺻ �̵� ������ ���� ����ü�� �����Ѵ�.

#ifdef _GMTOOL_
	GMTOOLMGR->InitNpcData();
#endif

	DWORD n=0;																		// ī��Ʈ�� ������ �����ϰ� 0���� �����Ѵ�.
	while(FALSE == file.IsEOF())
	{
		npcMapnum = file.GetWord();													// ���Ͽ��� �� ��ȣ�� �о���δ�.

		if(npcMapnum != MapNum)														// npc �ʰ� ���� �� ��ȣ�� ���� ������,
		{
			file.GetLine(value,256);												// ������ �о���δ�.
			continue;																// ��Ƽ���� �Ѵ�.
		}

		binfo.dwObjectID = STATICNPC_ID_START + n++;
		binfo.ObjectState = eObjectState_None;
		tinfo.MapNum = MapNum;
		minfo.bMoving = FALSE;
		minfo.KyungGongIdx = 0;
		minfo.MoveMode = eMoveMode_Run;

		tinfo.NpcKind = file.GetWord();												// ��Ż ������ npc ������ �����Ѵ�.
		// 080218 LYW --- MHMap : ������Ʈ �̸��� ���� Ȯ��.
		SafeStrCpy( binfo.ObjectName, file.GetString(), MAX_NPC_NAME_LENGTH+1 );						// �⺻ ������ npc �̸��� �����Ѵ�.
		//SafeStrCpy( binfo.ObjectName, file.GetString(), MAX_NAME_LENGTH+1 );		// �⺻ ������ npc �̸��� �����Ѵ�.
		tinfo.NpcUniqueIdx = file.GetWord();										// ��Ż ������ npc ����ũ �ε����� �����Ѵ�.

		NPC_LIST* pInfo = GAMERESRCMNGR->GetNpcInfo(tinfo.NpcKind);					// npc ������ �����Ͽ� npc ����Ʈ ������ �޴´�.

		if(pInfo)																	// npc ����Ʈ ������ ��ȿ�ϸ�,
		{
			tinfo.NpcJob = pInfo->JobKind;											// ��Ż ������ npc ������ �����Ѵ�.
		}
		else																		// npc ����Ʈ ������ ��ȿ���� ������,
		{
			tinfo.NpcJob = 0;														// ������ 0���� �����Ѵ�.
		}
		
		//jop�� 0�̸� �̸��� ������.
		if( tinfo.NpcJob == 0 )														// ������ 0�̸�,
		{
			binfo.ObjectName[0] = 0;												// �̸��� �����.
		}

		minfo.CurPosition.x = file.GetFloat();										// �̵������� ���Ϸ� ���� ���� ��ġ�� ��ġ�� �����Ѵ�.
		minfo.CurPosition.y = 0;
		minfo.CurPosition.z = file.GetFloat();
		
		CNpc* pNpc = OBJECTMGR->AddNpc(&binfo,&minfo,&tinfo);

		//����.....
		float fDir = file.GetFloat();												// ������ �о���δ�.
		pNpc->SetAngle( DEGTORAD( fDir ) );											// ������ �����Ѵ�.

		// 090925 ShinJS --- NpcNotice Dialog �� ��� ����
		BOOL bUseNoticeDlg = file.GetBool();
		pNpc->SetUseNoticeDlg( bUseNoticeDlg );										// NPC Notice Dialog ��뿩�θ� �����Ѵ�.
		
		GAMEIN->GetMiniMapDialog()->AddStaticNpcIcon(pNpc);							// �̴ϸʿ� npc �������� �߰��Ѵ�.
		
		// Npc Add
		QUESTMGR->SetNpcData( pNpc );												// ����Ʈ �Ŵ����� npc �����͸� �����Ѵ�.

		// 100427 ONS StaticNpc������ GM���� �����Ѵ�.
#ifdef _GMTOOL_
		GMTOOLMGR->SetNpcData( tinfo.NpcUniqueIdx, binfo.ObjectName, minfo.CurPosition );
#endif
	}
	
	file.Release();																	// ������ �����Ѵ�.
}

void CMHMap::AddQuestNpc( QUESTNPCINFO* pInfo )
{
	if( pInfo->wMapNum != GetMapNum() )
		return;
	CObject* pObject = OBJECTMGR->GetObject( pInfo->dwObjIdx );
	if( pObject )
		return;

	NPC_TOTALINFO tinfo;
	BASEOBJECT_INFO binfo;
	BASEMOVE_INFO minfo;

	binfo.dwObjectID = STATICNPC_ID_START + m_dwQuestNpcId++;
	pInfo->dwObjIdx = binfo.dwObjectID;
	binfo.ObjectState = eObjectState_None;
	tinfo.MapNum = pInfo->wMapNum;
	minfo.bMoving = FALSE;
	minfo.KyungGongIdx = 0;
	minfo.MoveMode = eMoveMode_Run;

	tinfo.NpcKind = pInfo->wNpcKind;
//	strcpy( binfo.ObjectName, pInfo->sName );
	SafeStrCpy( binfo.ObjectName, pInfo->sName, MAX_NAME_LENGTH+1 );
	tinfo.NpcUniqueIdx = pInfo->wNpcIdx;
	NPC_LIST* pList = GAMERESRCMNGR->GetNpcInfo(tinfo.NpcKind);
	if(pInfo)
	{
		tinfo.NpcJob = pList->JobKind;
	}
	else
	{
		tinfo.NpcJob = 0;
	}

	minfo.CurPosition.x = pInfo->vPos.x;
	minfo.CurPosition.y = 0;
	minfo.CurPosition.z = pInfo->vPos.z;

	CNpc* pNpc = OBJECTMGR->AddNpc(&binfo,&minfo,&tinfo);

	//����.....
	float fDir = pInfo->fDir;

	pNpc->SetAngle( DEGTORAD( fDir ) );
	
	GAMEIN->GetMiniMapDialog()->AddStaticNpcIcon(pNpc);

	// Npc Add
	QUESTMGR->SetNpcData( pNpc );


	TARGETSET set;
	set.pTarget = pNpc;
	//EFFECTMGR->StartEffectProcess(eEffect_MonRegen,pNpc,&set,0,pNpc->GetID());
	EFFECTMGR->StartEffectProcess(eEffect_NewCharacter,pNpc,&set,0,pNpc->GetID());
}


///--------------------------------------------------
/// ��ã��, �浹 ����
///--------------------------------------------------

// Ÿ�� �������� ������ �׾ �ѹ��� �� �� �ִ°� ���°��� üũ
BOOL CMHMap::CollisionCheck_OneLine_New( VECTOR3* pStart,VECTOR3* pEnd )
{
	if(m_pTileManager == NULL)
	{
		ASSERT(!"�н��Ŵ����� �����ϴ�");
		return FALSE;
	}

	return m_pTileManager->CollisionCheck_OneLine_New(pStart,pEnd);
}

// Ÿ�� ������ Ÿ���� �շ��ִ°��� üũ
BOOL CMHMap::CollisionCheck_OneTile( VECTOR3* pDestPos )
{
	if(m_pTileManager == NULL)
	{
		ASSERT(!"�н��Ŵ����� �����ϴ�");
		return FALSE;
	}

	CTile* l_Node = m_pTileManager->GetNode1x(pDestPos->x, pDestPos->z);

	if (l_Node)
	{
		if ( l_Node->IsCollisonTile() == FALSE )
		{
			// Ÿ���� �ְ� �� �� �ִ� ��� TRUE
			return TRUE;
		}
	}

	return FALSE;
}

//
// �����带 �̿��� ��ã��
// �ִ� 50�� Loop, 5~6�� ����
DWORD WINAPI PathFind_Thread( LPVOID p_Param )
{
	HTR_S( eRT_AStarTotal );

	g_WayPointManager->SetWMStatus(eWMStatus_Active);

	/// �����ϰ� ã���� �����ϰ� ã������ �ɼǿ� ���� �ٸ��� ���� ã�´�
	if ( g_SimpleMode )
	{
		// 1��� �����ϰ� ã�� ���
		if( PathFind_Simple() )
		{
			// ������ ����ȭ�� ����
			g_WayPointManager->SetWMStatus(eWMStatus_Success);
			InterlockedExchange(&g_PathThreadActivated, 0);

			return TRUE;
		}

		HTRACE("PathFind_Thread( LPVOID p_Param ) ����!!! \n");
		g_WayPointManager->SetWMStatus(eWMStatus_Fail);
		InterlockedExchange(&g_PathThreadActivated, 0);

		return FALSE;
	}

	// 3��� Ÿ���� �̿��� ������ ������ ��ģ ���� �ӵ��� ��ã��
	if( PathFind_Quickly() )
	{
		// ������ ����ȭ�� ����
		g_WayPointManager->SetWMStatus(eWMStatus_Success);
		InterlockedExchange(&g_PathThreadActivated, 0);

		return TRUE;
	}

	// 091207 ShinJS --- x��� ��ã�� ���н� 1��� ��ã�� �õ�
	if( PathFind_Simple() )
	{
		// ������ ����ȭ�� ����
		g_WayPointManager->SetWMStatus(eWMStatus_Success);
		InterlockedExchange(&g_PathThreadActivated, 0);

		return TRUE;
	}

	HTRACE("PathFind_Thread( LPVOID p_Param ) ����!!! \n");
	g_WayPointManager->SetWMStatus(eWMStatus_Fail);
	InterlockedExchange(&g_PathThreadActivated, 0);

	return FALSE;
}

// 1��� ��ã��
BOOL PathFind_Simple()
{
	g_TileManager->SetPathFindMode(ePathFindLayer_1x);
	g_TileManager->AStarDataReset();

	HTR_S( eRT_InputPoint );
	BOOL t_InputPoint = g_TileManager->InputPoint( g_PathFindStart, g_PathFindEnd ); 
	HTR_E( eRT_InputPoint );

	if ( t_InputPoint )
	{
		HTR_S( eRT_PathFind_Astar );
		BOOL t_PathFind_AStar = g_TileManager->PathFind_AStar( 500000 );
		HTR_E( eRT_PathFind_Astar );

		if ( t_PathFind_AStar == TRUE)
		{
			HTR_S( eRT_MakeAStarPath );
			g_TileManager->MakeAStarPath();
			HTR_E( eRT_MakeAStarPath );

			// ����ȭ ������ �߿��ϴ�
			HTR_S( eRT_Optimize );
			g_TileManager->Optimize();
			HTR_E( eRT_Optimize );

			// ��������Ʈ�� �޾ƿ´�
			g_WayPointManager->GetVec_WayPoint() = g_TileManager->GetPathResult_Optimize(eDefaultTileRatio);

			// ����׿� ���� �Է�
			MAP->SetPath_Debug( g_TileManager->GetPathResult(eDefaultTileRatio) );
			MAP->SetPath_Optimize_Debug( g_TileManager->GetPathResult_Optimize_Debug(eDefaultTileRatio) );

			// ��������Ʈ�� TRACE�غ���
			g_WayPointManager->Debug_TRACEWayPoint();

			HTR_E( eRT_AStarTotal );

			return TRUE;
		}
	}

	return FALSE;
}

// 3/4 ��� ��ã��
BOOL PathFind_Quickly()
{
	/// 1. ����� - �� ��ġ���� �� �� �ִ� ���� ����� 3xŸ���� ��ġ�� ã�´�
	/// 2. ������ - Ŭ���� �������� ���� ����� 3xŸ���� ��ġ�� �ٷ� ����

	/// 3. �� ��ġ���� ����� 3x Ÿ�ϱ����� 1x��ã��
	/// 4. 3x����� -> 3x������ Ÿ�� ��ã��
	/// 5. 4�� ����� 1xŸ����ġ�� ȯ��
	/// 6. 3�� ��� + 5�� ����� ����(1xŸ�� ���)
	/// 7. 6������ ���� 1xŸ�� ������ 1x�������� ����ȭ
	/// �ϼ�!!!

	g_TileManager->SetPathFindMode(ePathFindLayer_3x);
	g_TileManager->AStarDataReset();

	// ���� ���(3x)
	IPathNode* l_StartNode3x = NULL;
	g_TileManager->FindClosestTile(g_PathFindStart, l_StartNode3x);

	// ���� ���(3x)
	IPathNode* l_DestNode3x = NULL;
	g_TileManager->FindClosestTile(g_PathFindEnd, l_DestNode3x);

	HTR_S( eRT_InputPoint );
	BOOL t_InputPoint = g_TileManager->InputPoint( l_StartNode3x, l_DestNode3x ); 
	HTR_E( eRT_InputPoint );

	if ( t_InputPoint )
	{
		HTR_S( eRT_PathFind_Astar );
		BOOL t_PathFind_AStar = g_TileManager->PathFind_AStar( 500000 );
		HTR_E( eRT_PathFind_Astar );

		if ( t_PathFind_AStar == TRUE)
		{
			HTR_S( eRT_MakeAStarPath );
			g_TileManager->MakeAStarPath();
			HTR_E( eRT_MakeAStarPath );

			// ����ȭ ������ �߿��ϴ�
			HTR_S( eRT_Optimize );
			g_TileManager->Optimize();
			HTR_E( eRT_Optimize );

			float fNodeRatio = g_TileManager->GetNodeRatio();

			// ��������Ʈ�� �޾ƿ´�
			g_WayPointManager->GetVec_WayPoint() = g_TileManager->GetPathResult_Optimize( fNodeRatio );

			// ����׿� ���� �Է�
			MAP->SetPath_Debug( g_TileManager->GetPathResult( fNodeRatio ) );
			MAP->SetPath_Optimize_Debug( g_TileManager->GetPathResult_Optimize_Debug( fNodeRatio ) );

			// ��������Ʈ�� TRACE�غ���
			g_WayPointManager->Debug_TRACEWayPoint();

			// ��� ���󺹱�
			g_TileManager->SetPathFindMode(ePathFindLayer_1x);

			HTR_E( eRT_AStarTotal );

			return TRUE;
		}
	}

	return FALSE;
}


BOOL CMHMap::PathFind( VECTOR3* pStart,VECTOR3* pEnd, CWayPointManager* p_pWayPointManager, BOOL p_SimpleMode /*= TRUE*/ )
{
	/// ���� üũ
	if (m_pTileManager == NULL || p_pWayPointManager == NULL)
	{
		ASSERT(!"�н��Ŵ��� �Ǵ� ��������Ʈ �Ŵ����� �����ϴ�");
		return FALSE;
	}

	// �����尡 Ȱ��ȭ�� ������ �������� �ʴ´�
	if(InterlockedCompareExchange(&g_PathThreadActivated, 1, 0) == 1)
	{
		return FALSE;
	}

	/// ��ã�⸦ ������ �����带 �����
	// �����带 ������ �������� �ɷ����� ��ã�Ҵٸ�
	// �� ����� CWayPointManager�ʿ��� Ȯ���Ѵ�
	DWORD l_ThreadID;
	HANDLE l_ThreadHandle;

	g_PathFindStart		= *pStart;
	g_PathFindEnd		= *pEnd;
	g_TileManager		= m_pTileManager;
	g_WayPointManager	= p_pWayPointManager;
	g_SimpleMode		= p_SimpleMode;

	l_ThreadHandle = CreateThread(NULL, 0, PathFind_Thread, NULL, 0, &l_ThreadID);
	if(l_ThreadHandle == NULL)
	{
		InterlockedExchange(&g_PathThreadActivated, 0);
		return FALSE;
	}
	CloseHandle(l_ThreadHandle);


	return TRUE;
}

//BOOL CMHMap::PathFind_Line(VECTOR3* pStart,VECTOR3* pEnd,VECTOR3* pRtCollisonPos,CObject* pObj)
//{
//	if (m_pTileManager == NULL)
//	{
//		ASSERT(!"�н��Ŵ����� �����ϴ�");
//		return FALSE;
//	}
//
//	return m_pTileManager->CollisionCheck_OneLine(pStart,pEnd,pRtCollisonPos);
//}

void CMHMap::RenderTileData_Debug()
{
#ifdef _GMTOOL_

	if( !m_bRenderTileData ) return;
	if (!HERO) return;
	if (!m_pTileManager) return;
	if ( g_WayPointManager && g_WayPointManager->GetWMStatus() == eWMStatus_Active ) return;

	CBigMapDlg* pBigMapDlg = GAMEIN->GetBigMapDialog();
	if( !pBigMapDlg ) return;

	/// ���� 1xŸ�� ����
	CTile* l_Node = NULL;
	DWORD l_TileWidth = m_pTileManager->GetWidth1x();
	DWORD l_TileHeight = m_pTileManager->GetHeight1x();

	VECTOR3 l_TilePos;

	// ���ΰ� ��ó�� �׸���
	VECTOR3 heroPos;
	HERO->GetPosition( &heroPos );
	int l_HeroX = (int)heroPos.x / eDefaultTileRatio;
	int l_HeroZ = (int)heroPos.z / eDefaultTileRatio;

	int l_StartX = max( l_HeroX - 20, 0 );
	int l_StartZ = max( l_HeroZ - 20, 0 );
	int l_EndX = min( l_HeroX + 20, (int)l_TileWidth );
	int l_EndZ = min( l_HeroZ + 20, (int)l_TileHeight );


	for (DWORD j=l_StartZ; j<(DWORD)l_EndZ; j++)
	{
		for (DWORD i=l_StartX; i<(DWORD)l_EndX; i++ )
		{
			l_Node = m_pTileManager->GetNode1x(i,j);
			if (l_Node->IsCollisonTile())
			{
				l_TilePos.x = (float)i * eDefaultTileRatio;
				l_TilePos.y = heroPos.y + 5;
				l_TilePos.z = (float)j * eDefaultTileRatio;
				RenderOneTile( l_TilePos, 0xFFFF0000 );
			}
			else
			{
				l_TilePos.x = (float)i * eDefaultTileRatio;
				l_TilePos.y = heroPos.y + 5;
				l_TilePos.z = (float)j * eDefaultTileRatio;
				///RenderOneTile( l_TilePos, 0xFF000000 );	// �ӵ������� ����Ÿ���� �׸��� ����
			}
		}
	}


	if( !pBigMapDlg->IsActive() ) return;

	/// ��ã�� ����

	// ���� A-Star
	for (UINT i=0; i<m_Path_Debug.size(); i++)
	{
		VECTOR3 l_Vector = m_Path_Debug[i];
		l_Vector.y = heroPos.y + 10;
		RenderOneTile( l_Vector, 0xFF00CC00 );
	}

	// ����ȭ�� Path->WayPoint���� ��ȯ�� �� (Ÿ���� �������� ���� �ȴ�)
	for (UINT i=0; i<m_Path_Optimize_Debug.size(); i++)
	{
		VECTOR3 l_Vector = m_Path_Optimize_Debug[i];
		l_Vector.y = heroPos.y + 10;
		RenderOneWayPoint( l_Vector, 0xFFFFFFFF );
	}

	// ����ȭ ������ ������� �� ����
	if ( m_Path_Optimize_Debug.size() > 2)
	{
		for (UINT i=0; i<m_Path_Optimize_Debug.size() - 1; i++)
		{
			VECTOR3 l_Vector1 = m_Path_Optimize_Debug[i];
			VECTOR3 l_Vector2 = m_Path_Optimize_Debug[i+1];

			// ��ʰ� �����ϰ� ����ǵ� �ʸŴ����󿡼� Ȯ������� �����´�.
			// ������ ��ȯ �Լ��� �ʸŴ��� �Ǵ� ��ʴ��̾�α׿� �ְ�, �װ��� ����ϸ� ���� �� ����
			VECTOR2 l_2DPos1, l_2DPos2;
			
			l_2DPos1.x = pBigMapDlg->GetAbsX() + (l_Vector1.x / eDefaultTileRatio / 2);
			l_2DPos1.y = pBigMapDlg->GetAbsY() + 512 - (l_Vector1.z / eDefaultTileRatio / 2);
			l_2DPos2.x = pBigMapDlg->GetAbsX() + (l_Vector2.x / eDefaultTileRatio / 2);
			l_2DPos2.y = pBigMapDlg->GetAbsY() + 512 - (l_Vector2.z / eDefaultTileRatio / 2);

			g_pExecutive->GetRenderer()->RenderLine( &l_2DPos1, &l_2DPos2, 0xFF00FF00 );
		}
	}

#endif	// _GMTOOL_
}

// ��� ��ġ�� ���� �� �������� �׸� �ϳ��� �׷��ش�
void CMHMap::RenderOneTile( const VECTOR3& p_TileOriPos, DWORD p_Color )
{
	VECTOR3 inLefttop, inRightTop, inLeftBottom, inRightBottom;
	VECTOR3 outLefttop, outRightTop, outLeftBottom, outRightBottom;
	VECTOR2 Lefttop, RightTop, LeftBottom, RightBottom;

	inLefttop.x = p_TileOriPos.x + 5;
	inLefttop.y = p_TileOriPos.y;
	inLefttop.z = p_TileOriPos.z + 5;

	inRightTop.x = p_TileOriPos.x + 45;
	inRightTop.y = p_TileOriPos.y;
	inRightTop.z = p_TileOriPos.z + 5;

	inLeftBottom.x = p_TileOriPos.x + 5;
	inLeftBottom.y = p_TileOriPos.y;
	inLeftBottom.z = p_TileOriPos.z + 45;

	inRightBottom.x = p_TileOriPos.x + 45;
	inRightBottom.y = p_TileOriPos.y;
	inRightBottom.z = p_TileOriPos.z + 45;

	GetScreenXYFromXYZ(g_pExecutive->GetGeometry(), 0, &inLefttop, &outLefttop);
	GetScreenXYFromXYZ(g_pExecutive->GetGeometry(), 0, &inRightTop, &outRightTop);
	GetScreenXYFromXYZ(g_pExecutive->GetGeometry(), 0, &inLeftBottom, &outLeftBottom);
	GetScreenXYFromXYZ(g_pExecutive->GetGeometry(), 0, &inRightBottom, &outRightBottom);

	DWORD dwWidth = GAMERESRCMNGR->m_GameDesc.dispInfo.dwWidth;
	DWORD dwHeight = GAMERESRCMNGR->m_GameDesc.dispInfo.dwHeight;

	Lefttop.x = outLefttop.x * dwWidth;
	Lefttop.y = outLefttop.y * dwHeight;

	RightTop.x = outRightTop.x * dwWidth;
	RightTop.y = outRightTop.y * dwHeight;

	LeftBottom.x = outLeftBottom.x * dwWidth;
	LeftBottom.y = outLeftBottom.y * dwHeight;

	RightBottom.x = outRightBottom.x * dwWidth;
	RightBottom.y = outRightBottom.y * dwHeight;


	g_pExecutive->GetRenderer()->RenderLine( &Lefttop, &RightTop, p_Color );
	g_pExecutive->GetRenderer()->RenderLine( &RightTop, &RightBottom, p_Color );
	g_pExecutive->GetRenderer()->RenderLine( &RightBottom, &LeftBottom, p_Color );
	g_pExecutive->GetRenderer()->RenderLine( &LeftBottom, &Lefttop, p_Color );
}
// ��� ��ġ �ٷ� �� ������ ��������Ʈ �׸�
void CMHMap::RenderOneWayPoint( const VECTOR3& p_WayPointPos, DWORD p_Color )
{
	VECTOR3 l_L1, l_L2, l_L3, l_L4;
	VECTOR3 outl_L1, outl_L2, outl_L3, outl_L4;
	VECTOR2 v2_L1, v2_L2, v2_L3, v2_L4;

	l_L1.x = p_WayPointPos.x;
	l_L1.y = p_WayPointPos.y;
	l_L1.z = p_WayPointPos.z - 20;

	l_L2.x = p_WayPointPos.x;
	l_L2.y = p_WayPointPos.y;
	l_L2.z = p_WayPointPos.z + 20;

	l_L3.x = p_WayPointPos.x - 20;
	l_L3.y = p_WayPointPos.y;
	l_L3.z = p_WayPointPos.z;

	l_L4.x = p_WayPointPos.x + 20;
	l_L4.y = p_WayPointPos.y;
	l_L4.z = p_WayPointPos.z;

	GetScreenXYFromXYZ(g_pExecutive->GetGeometry(), 0, &l_L1, &outl_L1);
	GetScreenXYFromXYZ(g_pExecutive->GetGeometry(), 0, &l_L2, &outl_L2);
	GetScreenXYFromXYZ(g_pExecutive->GetGeometry(), 0, &l_L3, &outl_L3);
	GetScreenXYFromXYZ(g_pExecutive->GetGeometry(), 0, &l_L4, &outl_L4);

	DWORD dwWidth = GAMERESRCMNGR->m_GameDesc.dispInfo.dwWidth;
	DWORD dwHeight = GAMERESRCMNGR->m_GameDesc.dispInfo.dwHeight;

	v2_L1.x = outl_L1.x * dwWidth;
	v2_L1.y = outl_L1.y * dwHeight;

	v2_L2.x = outl_L2.x * dwWidth;
	v2_L2.y = outl_L2.y * dwHeight;

	v2_L3.x = outl_L3.x * dwWidth;
	v2_L3.y = outl_L3.y * dwHeight;

	v2_L4.x = outl_L4.x * dwWidth;
	v2_L4.y = outl_L4.y * dwHeight;

	g_pExecutive->GetRenderer()->RenderLine( &v2_L1, &v2_L2, p_Color );
	g_pExecutive->GetRenderer()->RenderLine( &v2_L3, &v2_L4, p_Color );
}

VECTOR3 CMHMap::GetSafePosition( VECTOR3* p_NowPosition )
{
	VECTOR3 l_SafePosition;
	l_SafePosition.x = 0; l_SafePosition.y = 0; l_SafePosition.z = 0;
	
	if (m_pTileManager && p_NowPosition)
	{
		DWORD l_NowPosX = (DWORD)(p_NowPosition->x/eDefaultTileRatio);
		DWORD l_NowPosZ = (DWORD)(p_NowPosition->z/eDefaultTileRatio);

		BOOL l_Result = FALSE;
		l_Result = m_pTileManager->FindClosest1xTile(l_SafePosition, l_NowPosX, l_NowPosZ);

		if (l_Result)
		{
			return l_SafePosition;
		}
	}

	return l_SafePosition;
}
