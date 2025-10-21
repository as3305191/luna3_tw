#include "StdAfx.h"
#include ".\dungeonprocessorbrokenshipwrecked.h"

#include "..\[CC]Header\GameResourceManager.h"
#include "..\Monster.h"
#include "..\RegenManager.h"
#include "..\AISystem.h"
#include "DungeonMgr.h"
#include "..\Object.h"
#include "..\UserTable.h"
#include "..\Trigger\Manager.h"

CDungeonProcessorBrokenShipwrecked::CDungeonProcessorBrokenShipwrecked(void)
{
}

CDungeonProcessorBrokenShipwrecked::~CDungeonProcessorBrokenShipwrecked(void)
{
}

void CDungeonProcessorBrokenShipwrecked::Create(stDungeon* pDungeon)
{
}

void CDungeonProcessorBrokenShipwrecked::Process()
{
}

void CDungeonProcessorBrokenShipwrecked::SetSwitch(DWORD dwChannelID, WORD num, BOOL val)
{
	if(val && num == Switch_for_FallingRock)
	{
		// ������ ������ ����Ʈ. Ʈ������ AddEffect �׼��� ����Ͽ� ������ ������.
		g_pUserTable->SetPositionHead();
		CObject* pObject;
		while((pObject = g_pUserTable->GetData()) != NULL)
		{
			if(pObject->GetObjectKind() == eObjectKind_Player &&
				pObject->GetGridID() == dwChannelID)
			{
				MSG_DWORD5	msg;
				msg.Category = MP_TRIGGER;
				msg.Protocol = MP_TRIGGER_ADDEFFECT_ACK;
				msg.dwObjectID = pObject->GetID();
				msg.dwData1 = EffectDescNum_FallingRock;
				msg.dwData2 = 1;		// ������Ʈ
				msg.dwData3 = 43336;	//GetValue(eProperty_PosX);
				msg.dwData4 = 33;		//GetValue(eProperty_PosY);
				msg.dwData5 = 16135;	//GetValue(eProperty_PosZ);
				pObject->SendMsg(&msg, sizeof(msg));

				// ������
				MSG_DWORD3	msg2;
				msg2.Category = MP_DUNGEON;
				msg2.Protocol = MP_DUNGEON_ZONEBLOCK;
				msg2.dwObjectID = pObject->GetID();
				msg2.dwData1 = Range_X_FallenRock;	//GetValue(eProperty_RangeX);
				msg2.dwData2 = Range_Z_FallenRock;	//GetValue(eProperty_RangeZ);
				msg2.dwData3 = Range_FallenRock;	//GetValue(eProperty_Range);
				pObject->SendMsg(&msg2, sizeof(msg2));
			}
		}
	}
	else if(val && num == Switch_for_FallenRock)
	{
		// ������ ������ ����Ʈ. Ʈ������ AddEffect �׼��� ����Ͽ� ������ ������.
		g_pUserTable->SetPositionHead();
		CObject* pObject;
		while((pObject = g_pUserTable->GetData()) != NULL)
		{
			if(pObject->GetObjectKind() == eObjectKind_Player &&
				pObject->GetGridID() == dwChannelID)
			{
				MSG_DWORD2	msg;
				msg.Category = MP_TRIGGER;
				msg.Protocol = MP_TRIGGER_REMOVEEFFECT_ACK;
				msg.dwObjectID = pObject->GetID();
				msg.dwData1 = EffectDescNum_FallingRock;
				msg.dwData2 = 1;	// ������Ʈ
				pObject->SendMsg(&msg, sizeof(msg));

				MSG_DWORD5	msg2;
				msg2.Category = MP_TRIGGER;
				msg2.Protocol = MP_TRIGGER_ADDEFFECT_ACK;
				msg2.dwObjectID = pObject->GetID();
				msg2.dwData1 = EffectDescNum_FallenRock;
				msg2.dwData2 = 1;		// ������Ʈ
				msg2.dwData3 = 43336;	//GetValue(eProperty_PosX);
				msg2.dwData4 = 33;		//GetValue(eProperty_PosY);
				msg2.dwData5 = 16135;	//GetValue(eProperty_PosZ);
				pObject->SendMsg(&msg2, sizeof(msg2));
			}
		}
		
		DungeonMGR->SetSwitch(dwChannelID, Switch_for_FallingRock, FALSE);
	}
}

void CDungeonProcessorBrokenShipwrecked::Info_Syn(CObject* pObject)
{
	if(! pObject)
		return;

	// ����ġ�� ���¿� ���� ���������� ������ �����ش�.
	if(DungeonMGR->GetSwitch(pObject->GetGridID(), Switch_for_FallingRock))
	{
		// ���� �������� ��.
		MSG_DWORD5	msg;
		msg.Category = MP_TRIGGER;
		msg.Protocol = MP_TRIGGER_ADDEFFECT_ACK;
		msg.dwObjectID = pObject->GetID();
		msg.dwData1 = EffectDescNum_FallingRock;
		msg.dwData2 = 1;		// ������Ʈ
		msg.dwData3 = 43336;	//GetValue(eProperty_PosX);
		msg.dwData4 = 33;		//GetValue(eProperty_PosY);
		msg.dwData5 = 16135;	//GetValue(eProperty_PosZ);
		pObject->SendMsg(&msg, sizeof(msg));

		// ������
		MSG_DWORD3	msg2;
		msg2.Category = MP_DUNGEON;
		msg2.Protocol = MP_DUNGEON_ZONEBLOCK;
		msg2.dwObjectID = pObject->GetID();
		msg2.dwData1 = Range_X_FallenRock;	//GetValue(eProperty_RangeX);
		msg2.dwData2 = Range_Z_FallenRock;	//GetValue(eProperty_RangeZ);
		msg2.dwData3 = Range_FallenRock;	//GetValue(eProperty_Range);
		pObject->SendMsg(&msg2, sizeof(msg2));
	}
	else if(DungeonMGR->GetSwitch(pObject->GetGridID(), Switch_for_FallenRock))
	{
		// ���� ��������.
		MSG_DWORD2	msg;
		msg.Category = MP_TRIGGER;
		msg.Protocol = MP_TRIGGER_REMOVEEFFECT_ACK;
		msg.dwObjectID = pObject->GetID();
		msg.dwData1 = EffectDescNum_FallingRock;
		msg.dwData2 = 1;	// ������Ʈ
		pObject->SendMsg(&msg, sizeof(msg));

		MSG_DWORD5	msg2;
		msg2.Category = MP_TRIGGER;
		msg2.Protocol = MP_TRIGGER_ADDEFFECT_ACK;
		msg2.dwObjectID = pObject->GetID();
		msg2.dwData1 = EffectDescNum_FallenRock;
		msg2.dwData2 = 1;		// ������Ʈ
		msg2.dwData3 = 43336;	//GetValue(eProperty_PosX);
		msg2.dwData4 = 33;		//GetValue(eProperty_PosY);
		msg2.dwData5 = 16135;	//GetValue(eProperty_PosZ);
		pObject->SendMsg(&msg2, sizeof(msg2));

		// ������
		MSG_DWORD3	msg3;
		msg3.Category = MP_DUNGEON;
		msg3.Protocol = MP_DUNGEON_ZONEBLOCK;
		msg3.dwObjectID = pObject->GetID();
		msg3.dwData1 = Range_X_FallenRock;	//GetValue(eProperty_RangeX);
		msg3.dwData2 = Range_Z_FallenRock;	//GetValue(eProperty_RangeZ);
		msg3.dwData3 = Range_FallenRock;	//GetValue(eProperty_Range);
		pObject->SendMsg(&msg3, sizeof(msg3));
	}
}

void CDungeonProcessorBrokenShipwrecked::SetBossMonster(stDungeon* pDungeon)
{
	if(! pDungeon)
		return;

	WORD wBossMonsterKind = 0;
	switch(pDungeon->difficulty)
	{
	case eDIFFICULTY_EASY:
		{
			if(0<=pDungeon->dwPoint && pDungeon->dwPoint<101)
				wBossMonsterKind = 696;
			else if(101<=pDungeon->dwPoint && pDungeon->dwPoint<201)
				wBossMonsterKind = 697;
			else if(201<=pDungeon->dwPoint && pDungeon->dwPoint<301)
				wBossMonsterKind = 698;
			else
				wBossMonsterKind = 699;
		}
		break;
	case eDIFFICULTY_NORMAL:
		{
			if(0<=pDungeon->dwPoint && pDungeon->dwPoint<101)
				wBossMonsterKind = 700;
			else if(101<=pDungeon->dwPoint && pDungeon->dwPoint<201)
				wBossMonsterKind = 701;
			else if(201<=pDungeon->dwPoint && pDungeon->dwPoint<301)
				wBossMonsterKind = 702;
			else
				wBossMonsterKind = 703;
		}
		break;
	case eDIFFICULTY_HARD:
		{
			if(0<=pDungeon->dwPoint && pDungeon->dwPoint<101)
				wBossMonsterKind = 704;
			else if(101<=pDungeon->dwPoint && pDungeon->dwPoint<201)
				wBossMonsterKind = 705;
			else if(201<=pDungeon->dwPoint && pDungeon->dwPoint<301)
				wBossMonsterKind = 706;
			else
				wBossMonsterKind = 707;
		}
		break;
	}

	if(! wBossMonsterKind)
		return;

	// ������������~
	BASE_MONSTER_LIST* pMonsterList = GAMERESRCMNGR->GetMonsterListInfo(wBossMonsterKind);
	if(! pMonsterList)
		return;

	VECTOR3 vPos = {12300, 0, 42400};
	// 100104 LUJ, ���� �ε����� ���� ��ũ��Ʈ(Monster_%02d.bin)�� ���� ��
	//			�����Ǵ� ���� ��ȣ�̴�. ���� ���� ��ũ��Ʈ���� ��������
	//			�ʴ� ���ʹ� 0�̾ �������
	CMonster* pAddMonster = REGENMGR->RegenObject(
		g_pAISystem.GeneraterMonsterID(),
		0,
		pDungeon->dwChannelID,
		pMonsterList->ObjectKind,
		wBossMonsterKind,
		&vPos,
		(DWORD)DUNGEON_MONSTER_GROUP_ID,
		0,
		0,
		FALSE);

	DWORD alias = 0;
	DWORD count = 1;
	DWORD groupAlias = 0;
	// ��Ī�� ���������� ����Ѵ�.
	char buffer[MAX_PATH] = {0};
	sprintf(
		buffer,
		"[%s%d].%d.%d.%d",
		"����", count,
		Trigger::eOwner_Server,
		pDungeon->dwChannelID,
		pDungeon->dwChannelID);
	alias = TRIGGERMGR->GetHashCode(buffer);
	pAddMonster->SetAlias(alias);

	// �׷캰Ī�� ���������� ����Ѵ�.
	//char buffer[MAX_PATH] = {0};
	sprintf(
		buffer,
		"[%s].%d.%d.%d",
		"�����׷�",
		Trigger::eOwner_Server,
		pDungeon->dwChannelID,
		pDungeon->dwChannelID);
	groupAlias = TRIGGERMGR->GetHashCode(buffer);
	pAddMonster->SetGroupAlias(groupAlias);
}