#include "StdAfx.h"
#include "RegenManager.h"
#include "AIGroupManager.h"
#include "..\[CC]Header\GameResourceManager.h"
#include "AIGroupManager.h"
#include "TileManager.h"
#include "BossMonster.h"
#include "GridSystem.h"
#include "MHError.h"
#include "UserTable.h"
#include "Player.h"
#include "FieldBossMonster.h"
#include "FieldSubMonster.h"
#include "AISystem.h"
// 080616 LUJ, ������ �� ����� �����ϱ� ���� ����
#include "Trap.h"
// 090316 LUJ, Ż�� ���� ����
#include "Vehicle.h"
#include "MapDBMsgParser.h"

CRegenManager::CRegenManager()
{}

CRegenManager::~CRegenManager()
{}

void CRegenManager::RegenGroup(DWORD groupID, DWORD dwGridID)
{
	CAIGroup* const aiGroup = GROUPMGR->GetGroup(
		groupID,
		dwGridID);

	if(0 == aiGroup)
	{
		return;
	}

	aiGroup->ForceRegen();
}

void CRegenManager::RangePosAtOrig(const VECTOR3& OrigPos, int Range, VECTOR3& OutPos) const
{
	int ran=0;
	ran = rand();
	float dx = float(ran%Range) * (ran%2?1:-1);
	ran = rand();
	float dz = float(ran%Range) * (ran%2?1:-1);
	OutPos.x = OrigPos.x + dx;
	OutPos.z = OrigPos.z + dz;
	
	if( OutPos.x < 0 )				OutPos.x = 0;
	else if( OutPos.x > 51199 )	OutPos.x = 51199;

	if( OutPos.z < 0 )				OutPos.z = 0;
	else if( OutPos.z > 51199 )	OutPos.z = 51199;
}

CMonster* CRegenManager::RegenObject(DWORD dwID, DWORD dwSubID, DWORD dwGridID, EObjectKind wObjectKind, WORD wMonsterKind, const VECTOR3* vPos, DWORD dwGroupID, DWORD DropItemID /* = 0 */, DWORD dwDropRatio /* = 100  */,BOOL bRandomPos /* = TRUE */, BOOL bEventRegen /* = FALSE  */, DWORD ownedObjectIndex)
{
	/// 06. 08. �̿���
	/// ���� ���̵� ����ϴ� ��ü�� �������
	/// ������ ��Ű�� �ʰ� NULL���� ��ȯ����!
	if( g_pUserTable->FindUser( dwID ) )
	{
		ASSERT(0);
		return NULL;
	}

	VECTOR3 vOutPos( *vPos );

	// �ʵ庸���� ���
	if( wObjectKind == eObjectKind_FieldBossMonster )
	{
		// �ʵ庸�� �Ŵ����� ���� ��ġ�� 1�� �̻� ��ϵǾ� �ִٸ�
		if( FIELDBOSSMONMGR->GetRegenPositionCount() > 0 )
		{
			// ��ϵ� ���� ��ġ���� �������� �����Ѵ�
			VECTOR3 pos = FIELDBOSSMONMGR->GetRegenPosition();
			vOutPos.x = pos.x;
			vOutPos.z = pos.z;
		}
	}

	const MAPTYPE MapNum = g_pServerSystem->GetMapNum();

	if( bRandomPos )
	{
		int maxCount = 5;
	
		CAIGroup * pGroup = GROUPMGR->GetGroup(dwGroupID, dwGridID);
		DWORD range = pGroup->GetRegenRange();

		// 090611 LUJ, ���� ��ġ�� �浹���� �ʵ��� �ݺ��ؼ� ã�´�
		do
		{
			if( 0 > --maxCount )
			{
				vOutPos = *vPos;
				break;
			}

			RangePosAtOrig(
				*vPos,
				range,
				vOutPos);
		}
		while( g_pServerSystem->GetMap()->CollisionTilePos( vOutPos.x, vOutPos.z, MapNum ) );
	}

	switch( wObjectKind )
	{
	case eObjectKind_ToghterPlayMonster:
	case eObjectKind_SpecialMonster:
	case eObjectKind_Monster:
	// 080616 LUJ, ���� �߰�
	case eObjectKind_Trap:
	// 090316 LUJ, Ż�� �߰�
	case eObjectKind_Vehicle:
		{
			MONSTER_TOTALINFO MonsterInfo = {0};
			BASEOBJECT_INFO Baseinfo;
			ZeroMemory(
				&Baseinfo,
				sizeof(Baseinfo));
			BASE_MONSTER_LIST* pMonsterListInfo = GAMERESRCMNGR->GetMonsterListInfo(wMonsterKind);

			if(pMonsterListInfo == NULL)
				return NULL;

			Baseinfo.dwObjectID = dwID;
			SafeStrCpy(Baseinfo.ObjectName, pMonsterListInfo->Name, MAX_NAME_LENGTH+1);
			Baseinfo.BattleID = dwGridID;
	//		g_Console.LOG(4, "Regen Succeeded : CHANNEL:%d, GROUPID: %d, ID: %d, NAME: %s, (x,z):(%f,%f)", dwGridID, dwGroupID, dwID, pMonsterListInfo->Name, vOutPos.x, vOutPos.z);

			MonsterInfo.Life = pMonsterListInfo->Life;
			MonsterInfo.MonsterKind = wMonsterKind;
			MonsterInfo.Group = (WORD)dwGroupID;
			// 090316 LUJ, ������ ������Ʈ ��ȣ ����
			MonsterInfo.OwnedObjectIndex = ownedObjectIndex;
			
			CMonster* pMonster = g_pServerSystem->AddMonster(dwSubID, &Baseinfo, &MonsterInfo, &vOutPos,wObjectKind);
			if(!pMonster)
				return NULL;

			pMonster->SetEventMob(bEventRegen);
			if(bEventRegen && dwGroupID==ITEMRECALL_GROUP_ID)	// �����ۼ�ȯ���� �񼱰����� �����Ѵ�.
				pMonster->SetForeAttack(FALSE);

			pMonster->SetDropItemID(DropItemID, dwDropRatio);

			return pMonster;

		}
		break;

	// �ʵ庸�� - 05.12 �̿���
	// �ʵ庸�� ����
	case eObjectKind_FieldBossMonster:
		{
			MONSTER_TOTALINFO MonsterInfo = {0};
			BASEOBJECT_INFO Baseinfo;
			ZeroMemory(
				&MonsterInfo,
				sizeof(MonsterInfo));
			BASE_MONSTER_LIST * pMonsterListInfo = GAMERESRCMNGR->GetMonsterListInfo(wMonsterKind);
			
			if(FIELDBOSSMONMGR->IsRemainGroup(dwID))
			{
				g_Console.LOG(4, "FieldBossMonster Regen Failed(Remain Group) : GridID: %d, GROUPID: %d, ID: %d, NAME: %s", dwGridID, dwGroupID, dwID, pMonsterListInfo->Name);
				return NULL;
			}

			Baseinfo.dwObjectID = dwID;
			Baseinfo.BattleID = dwGridID;
			SafeStrCpy(Baseinfo.ObjectName, pMonsterListInfo->Name, MAX_NAME_LENGTH+1);

			// �ʵ庸�� ���� Ȯ�� �αױ��
			g_Console.LOG(4, "FieldBossMonster Regen Succeeded : GridID: %d, GROUPID: %d, ID: %d, NAME: %s, (x,z):(%f,%f), Time: %d", dwGridID, dwGroupID, dwID, pMonsterListInfo->Name, vOutPos.x, vOutPos.z, gCurTime / 1000);
			//MHERROR->OutputFile("Debug.txt", MHERROR->GetStringArg("FieldBoss Regen = GridID: %d, gCurTime: %d", dwGridID, gCurTime));

			MonsterInfo.Life = pMonsterListInfo->Life;
			MonsterInfo.MonsterKind = wMonsterKind;
			MonsterInfo.Group = (WORD)dwGroupID;

			CFieldBossMonster * pMob = g_pServerSystem->AddFieldBossMonster(dwSubID, &Baseinfo, &MonsterInfo, &vOutPos);
			
			// �ʵ� ���� �Ŵ����� ���
			FIELDBOSSMONMGR->AddFieldBossMonster(pMob);
			pMob->SetEventMob(bEventRegen);
			if(bEventRegen && dwGroupID==ITEMRECALL_GROUP_ID)	// �����ۼ�ȯ���� �񼱰����� �����Ѵ�.
				pMob->SetForeAttack(FALSE);

			return pMob;
		}
		break;
	// �ʵ庸������ ����
	case eObjectKind_FieldSubMonster:
		{
			MONSTER_TOTALINFO MonsterInfo = {0};
			BASEOBJECT_INFO Baseinfo;
			ZeroMemory(
				&MonsterInfo,
				sizeof(MonsterInfo));
			BASE_MONSTER_LIST * pMonsterListInfo = GAMERESRCMNGR->GetMonsterListInfo(wMonsterKind);

			Baseinfo.dwObjectID = dwID;
			Baseinfo.BattleID = dwGridID;
			SafeStrCpy(Baseinfo.ObjectName, pMonsterListInfo->Name, MAX_NAME_LENGTH+1);

			// �ʵ庸������ ���� Ȯ�� �αױ��
			g_Console.LOG(4, "FieldSubMonster Regen Succeeded : GROUPID: %d, ID: %d, NAME: %s, (x,z):(%f,%f)", dwGroupID, dwID, pMonsterListInfo->Name, vOutPos.x, vOutPos.z);
			//MHERROR->OutputFile("Debug.txt", MHERROR->GetStringArg("FieldSub Regen = GridID: %d, gCurTime: %d", dwGridID, gCurTime));

			MonsterInfo.Life = pMonsterListInfo->Life;
			MonsterInfo.MonsterKind = wMonsterKind;
			MonsterInfo.Group = (WORD)dwGroupID;

			CFieldSubMonster * pMob = g_pServerSystem->AddFieldSubMonster(dwSubID, &Baseinfo, &MonsterInfo, &vOutPos);
			pMob->SetEventMob(bEventRegen);
			if(bEventRegen && dwGroupID==ITEMRECALL_GROUP_ID)	// �����ۼ�ȯ���� �񼱰����� �����Ѵ�.
				pMob->SetForeAttack(FALSE);

			return pMob;
		}
		break;

	case eObjectKind_BossMonster:
		{
			MONSTER_TOTALINFO MonsterInfo = {0};
			BASEOBJECT_INFO Baseinfo;
			ZeroMemory(
				&MonsterInfo,
				sizeof(MonsterInfo));
			BASE_MONSTER_LIST * pMonsterListInfo = GAMERESRCMNGR->GetMonsterListInfo(wMonsterKind);

			Baseinfo.dwObjectID = dwID;
			Baseinfo.BattleID = dwGridID;
			SafeStrCpy(Baseinfo.ObjectName, pMonsterListInfo->Name, MAX_NAME_LENGTH+1);
			g_Console.LOG(4, "BossMonster Regen Succeeded : GROUPID: %d, ID: %d, NAME: %s, (x,z):(%f,%f)", dwGroupID, dwID, pMonsterListInfo->Name, vOutPos.x, vOutPos.z);
			MHERROR->OutputFile("Debug.txt", MHERROR->GetStringArg("Boss Regen = GridID: %d, gCurTime: %d", dwGridID, gCurTime));

			MonsterInfo.Life = pMonsterListInfo->Life;
			MonsterInfo.MonsterKind = wMonsterKind;
			MonsterInfo.Group = (WORD)dwGroupID;

			CBossMonster * pMob = g_pServerSystem->AddBossMonster(dwSubID, &Baseinfo, &MonsterInfo, &vOutPos);
			pMob->SetEventMob(bEventRegen);
			if(bEventRegen && dwGroupID==ITEMRECALL_GROUP_ID)	// �����ۼ�ȯ���� �񼱰����� �����Ѵ�.
				pMob->SetForeAttack(FALSE);
			return pMob;
			
		}
		break;
	case eObjectKind_Npc:
		{			
			BASEOBJECT_INFO Baseinfo;
			NPC_TOTALINFO NpcTotalInfo;
			STATIC_NPCINFO* pNpcInfo = GAMERESRCMNGR->GetStaticNpcInfo(wMonsterKind);
			if(pNpcInfo == 0)
			{
				return 0;
			}
			NPC_LIST* pNpcList = GAMERESRCMNGR->GetNpcList(pNpcInfo->wNpcJob);
			if(pNpcList == 0)
			{
				return 0;
			}
			
			Baseinfo.dwObjectID = dwID;
			Baseinfo.BattleID = dwGridID;
			
			SafeStrCpy(Baseinfo.ObjectName, pNpcInfo->NpcName, MAX_NAME_LENGTH+1);
			NpcTotalInfo.Group = (WORD)dwGroupID;
			NpcTotalInfo.MapNum = MapNum;
			NpcTotalInfo.NpcJob = pNpcInfo->wNpcJob;
			NpcTotalInfo.NpcKind = pNpcList->NpcKind;
			NpcTotalInfo.NpcUniqueIdx = pNpcInfo->wNpcUniqueIdx;
		
			g_pServerSystem->AddNpc(&Baseinfo, &NpcTotalInfo, &vOutPos);
			return 0;
		}
		break;
	}

	return 0;
}

CVehicle* CRegenManager::RegenVehicle( CPlayer* player, DWORD monsterKind, VECTOR3& vehiclePosition)
{
	const BASE_MONSTER_LIST* baseMonsterList = GAMERESRCMNGR->GetMonsterListInfo( monsterKind );

	if( ! baseMonsterList ||
		! player )
	{
		return 0;
	}

	CVehicle* const vehicle = ( CVehicle* )RegenObject(
		g_pAISystem.GeneraterMonsterID(),
		0,
		player->GetGridID(),
		baseMonsterList->ObjectKind,
		baseMonsterList->MonsterKind,
		&vehiclePosition,
		(DWORD)VEHICLE_RECALL_GROUP_ID,
		0,
		0,
		FALSE,
		FALSE,
		player->GetID() );

	if( ! vehicle )
	{
		return 0;
	}
	else if( vehicle->GetObjectKind() != eObjectKind_Vehicle )
	{
		vehicle->Die( 0 );
		return 0;
	}

	// 091212 ShinJS --- �ȱ�/�ٱ� �ӵ� �з�
	vehicle->SetMoveSpeed( baseMonsterList->RunMove, baseMonsterList->WalkMove );
	return vehicle;
}

CVehicle* CRegenManager::RegenVehicle(DWORD gridIndex, DWORD monsterKind, VECTOR3& position)
{
	const BASE_MONSTER_LIST* const baseMonsterList = GAMERESRCMNGR->GetMonsterListInfo( monsterKind );

	if(0 == baseMonsterList)
	{
		return 0;
	}

	CVehicle* const vehicle = ( CVehicle* )RegenObject(
		g_pAISystem.GeneraterMonsterID(),
		0,
		gridIndex,
		baseMonsterList->ObjectKind,
		baseMonsterList->MonsterKind,
		&position,
		(DWORD)EVENTRECALL_GROUP_ID,
		0,
		0,
		FALSE,
		FALSE,
		UINT_MAX);

	if(0 == vehicle)
	{
		return 0;
	}
	else if(eObjectKind_Vehicle != vehicle->GetObjectKind())
	{
		vehicle->Die(
			0);
		return 0;
	}

	return vehicle;
}