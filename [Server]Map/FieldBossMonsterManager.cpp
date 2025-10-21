#include "stdafx.h"
#include "FieldBossMonsterManager.h"
#include "FieldBossMonster.h"
#include "FieldSubMonster.h"
#include "AIGroupManager.h"
#include "AISystem.h"
#include "RegenManager.h"
#include "MHFile.h"
#include "..\[cc]BattleSystem\BattleSystem_Server.h"
#include "UserTable.h"
#include "ObjectFactory.h"
#include "AttackManager.h"
#include "PackedData.h"
#include "CharMove.h"
#include "MHError.h"
#include "GridSystem.h"
#include "GridTable.h"
#include "TileManager.h"

GLOBALTON(CFieldBossMonsterManager);

CFieldBossMonsterManager::CFieldBossMonsterManager(void)
{
	m_FieldGroupTable.Initialize(10);
	m_FieldMonsterInfoTable.Initialize(10);
	m_FieldBossDropItemTable.Initialize(10);
}

CFieldBossMonsterManager::~CFieldBossMonsterManager(void)
{
	m_FieldGroupTable.SetPositionHead();

	while(FIELDGROUP* group = m_FieldGroupTable.GetData())
	{
		SAFE_DELETE(
			group);
	}

	m_FieldMonsterInfoTable.SetPositionHead();

	while(FIELDMONSTERINFO* info = m_FieldMonsterInfoTable.GetData())
	{
		SAFE_DELETE(
			info);
	}

	m_FieldBossDropItemTable.SetPositionHead();

	while(FIELDBOSSDROPITEMLIST* list = m_FieldBossDropItemTable.GetData())
	{
		SAFE_DELETE(
			list);
	}
}

// �ʵ庸�� ���� ���� �������� �о���δ�
void CFieldBossMonsterManager::Init()
{
	CMHFile file;

	// �ʵ� ���� �� ���� �߰� ���� ����Ʈ
	if(!file.Init("System/Resource/FieldBossList.bin","rb"))
		return ;

	while(FALSE == file.IsEOF())
	{
		FIELDMONSTERINFO* const pMonsterInfo = new FIELDMONSTERINFO;
		pMonsterInfo->m_MonsterKind	= file.GetWord();
		pMonsterInfo->m_ObjectKind	= file.GetWord();
		pMonsterInfo->m_SubKind		= file.GetWord();
		pMonsterInfo->m_SubCount	= file.GetWord();

		pMonsterInfo->m_dwRegenTimeMin	= file.GetDword();
		pMonsterInfo->m_dwRegenTimeMax	= file.GetDword();
		pMonsterInfo->m_dwDistructTime	= file.GetDword() * 60000;

		pMonsterInfo->m_dwRecoverStartTime	= file.GetDword() * 1000;
		pMonsterInfo->m_dwRecoverDelayTime	= file.GetDword() * 1000;
		pMonsterInfo->m_fLifeRate	= file.GetFloat() / 100;

		m_FieldMonsterInfoTable.Add(pMonsterInfo, pMonsterInfo->m_MonsterKind);
	}

	file.Release();
	// �ʵ� ���� ��� ������ ���� ����Ʈ
	if(!file.Init("System/Resource/FieldBossDropItemList.bin","rb"))
		return ;

	while(FALSE == file.IsEOF())
	{
		FIELDBOSSDROPITEMLIST* const pItemList = new FIELDBOSSDROPITEMLIST;
		pItemList->m_wMonsterKind = file.GetWord();

		for(WORD group = 0; group < MAX_FIELDBOSS_DROPITEM_GROUP_NUM; group++)
		{
			file.GetWord();
			pItemList->m_ItemList[ group ].m_wCount = file.GetWord();
			pItemList->m_ItemList[ group ].m_wDropRate = file.GetWord();
			pItemList->m_ItemList[ group ].m_wDamageRate = file.GetWord();

			for(WORD count = 0; count < MAX_FIELDBOSS_DROPITEM_NUM; count++)
			{
				pItemList->m_ItemList[ group ].m_wItemIndex[ count ] = file.GetDword();
				pItemList->m_ItemList[ group ].m_wItemCount[ count ] = file.GetWord();
			}
		}

		m_FieldBossDropItemTable.Add(pItemList, pItemList->m_wMonsterKind);
	}
}

void CFieldBossMonsterManager::Process()
{
	FIELDGROUP* pGroup = NULL;
	
	m_FieldGroupTable.SetPositionHead();

	// ������ ���� �׷� ������ �����´�
	while( (pGroup = m_FieldGroupTable.GetData() )!= NULL)
	{
		BOOL bAllDead = TRUE;

		// ������ �ִ°�?
		if( pGroup->m_pBoss )
		{
			bAllDead = FALSE;

			// �������ΰ�?
			if( !pGroup->m_pBoss->IsBattle() )
			{
				// �Ҹ�ð��� �Ǿ��°�?
				if( pGroup->m_pBoss->IsDistruct() )
				{
					// ���ϰ� �����ִٸ� ���� �����Ѵ�
					if( pGroup->m_SubList.GetCount() )
					{
						PTRLISTPOS pos = pGroup->m_SubList.GetHeadPosition();
						CFieldSubMonster* pSub;

						while(pos)
						{
							pSub = (CFieldSubMonster*)pGroup->m_SubList.GetNext(pos);

							if( pSub )
							{
								// �������� �༮�� �н�...
								if( pSub->IsBattle() )
									continue;

								// ���� ����Ʈ���� ����
								pGroup->m_SubList.Remove((void*)pSub);

								// �������� ����
								g_pServerSystem->RemoveMonster( pSub->GetID() );
							}
						}
					}

					
					//g_pServerSystem->RemoveMonster( pGroup->m_pBoss->GetID() );

					// �ʵ庸�� �Ҹ� Ȯ�� �αױ��
					g_Console.LOG(4, "FieldBossMonster Delete : GROUPID: %d, ID: %d, NAME: %s, Time: %d",
						pGroup->m_pBoss->GetMonsterGroupNum(), 
						pGroup->m_pBoss->GetGridID(), 
						pGroup->m_pBoss->GetObjectName(),
						gCurTime / 1000);
					// Ŭ���̾�Ʈ�� ������� �޼��� �����ش�
					CCharMove::ReleaseMove(pGroup->m_pBoss);

					// ���� ����
					BATTLESYSTEM->DeleteObjectFromBattle(pGroup->m_pBoss);
					//g_pUserTable->RemoveUser(pGroup->m_pBoss->GetID());
					g_pUserTable->RemoveUser(pGroup->m_pBoss->GetID());	//NYJ
					//g_pObjectFactory->ReleaseObject(pGroup->m_pBoss);

					pGroup->m_pBoss = NULL;
				}
				else
					pGroup->m_pBoss->Recover();
			}
			
		}
		
		// ���ϰ� �ִ°�?
		if( pGroup->m_SubList.GetCount() )
		{
			bAllDead = FALSE;

			PTRLISTPOS pos = pGroup->m_SubList.GetHeadPosition();
			CFieldSubMonster* pSub;

			while(pos)
			{
				pSub = (CFieldSubMonster*)pGroup->m_SubList.GetNext(pos);

				if( pSub )
				{
					// ������ �ִ°�?
					if( pGroup->m_pBoss )
					{
						// �Ҹ�ð� ����
						pSub->ResetDistructTime();
					}

					// �������ΰ�?
					if( !pSub->IsBattle() )
					{
						// �Ҹ�ð��� �Ǿ��°�?
						if( pSub->IsDistruct() )
						{
							// ���� ����Ʈ���� ����
							pGroup->m_SubList.Remove((void*)pSub);

							// �������� ����
							g_pAISystem.ReleaseMonsterID( pSub->GetID() );
							g_pServerSystem->RemoveMonster( pSub->GetID() );
						}
						else
							pSub->Recover();
					}
				}
			}
		}

		// ��� �׾��°�?
		if( bAllDead )
		{
			CAIGroup* pAIGroup = GROUPMGR->GetGroup(pGroup->m_GroupID, pGroup->m_GridID);

			if(pAIGroup)
			{
				// ���� �����̸� ������ش�
				DWORD delaytime = pGroup->m_dwRegenTimeMax - pGroup->m_dwRegenTimeMin;

				if( delaytime )
					delaytime = rand() % (delaytime) * 60000;

				delaytime += pGroup->m_dwRegenTimeMin * 60000;
				
				pAIGroup->SetRegenDelayTime(delaytime);
				g_pAISystem.RemoveObject(pGroup->m_BossObjectID);
				//pAIGroup->Die(pGroup->m_BossObjectID);
				//pAIGroup->RegenCheck();

				g_Console.LOG(4, "FieldBossMonster Regen Count Start : GROUPID: %d, ID: %d, CurTime: %d, RegenTime: %d",
								pGroup->m_GroupID, 
								pGroup->m_BossObjectID,
								gCurTime / 1000,
								delaytime / 60000);

				//MHERROR->OutputFile("Debug.txt", MHERROR->GetStringArg("FieldBoss Regen Count Start = gCurTime: %d", gCurTime));
			

				m_FieldGroupTable.Remove(pGroup->m_BossObjectID);
				SAFE_DELETE(
					pGroup);
			}
			else
			{
				g_pAISystem.RemoveObject(pGroup->m_BossObjectID);

				m_FieldGroupTable.Remove(pGroup->m_BossObjectID);
				SAFE_DELETE(
					pGroup);
			}
		}
	}
}

void CFieldBossMonsterManager::AddRegenPosition(float x, float z)
{
	if( m_RegnePosCount >= MAX_REGENPOS_NUM )
		return;
	
	m_RegenPos[ m_RegnePosCount ].x = x;
	m_RegenPos[ m_RegnePosCount ].z = z;

	m_RegnePosCount++;
}

VECTOR3 CFieldBossMonsterManager::GetRegenPosition(int index)
{
	if(index < 0)
	{
		if( m_RegnePosCount != 0)
			index = rand() % m_RegnePosCount;
		else
		{
			VECTOR3 temp = {0.0f, 0.0f, 0.0f};
			return temp;
		}
	}

	return m_RegenPos[ index ];
}

void CFieldBossMonsterManager::BossDead(CFieldBossMonster* pBoss)
{
	FIELDGROUP* pGroup = m_FieldGroupTable.GetData(pBoss->GetID());

	if(pGroup)
	{
		// ���� ������ ��ġ�ϸ� �����Ѵ�
		if( pGroup->m_pBoss == pBoss )
		{
			// �ʵ庸�� ���� Ȯ�� �αױ��
			g_Console.LOG(4, "FieldBossMonster Dead : GROUPID: %d, ID: %d, NAME: %s, Time: %d",
						  pGroup->m_pBoss->GetMonsterGroupNum(), 
						  pGroup->m_pBoss->GetGridID(), 
						  pGroup->m_pBoss->GetObjectName(),
						  gCurTime / 1000);
			// �׸���� �ʿ��� ����
			CGridTable* pGridTable = g_pServerSystem->GetGridSystem()->GetGridTable(pGroup->m_pBoss);
			pGridTable->RemoveObject(pGroup->m_pBoss);

			BATTLESYSTEM->DeleteObjectFromBattle(pGroup->m_pBoss);
			
			pGroup->m_pBoss = NULL;


			// 080113 NYJ - ��ȹ����û: �ʵ庸�� �Ҹ�� ���ϸ��鵵 �Ҹ��Ų��.
			if( pGroup->m_SubList.GetCount() )
			{
				PTRLISTPOS pos = pGroup->m_SubList.GetHeadPosition();
				CFieldSubMonster* pSub;

				while(pos)
				{
					pSub = (CFieldSubMonster*)pGroup->m_SubList.GetNext(pos);

					if( pSub )
					{
						pSub->SetDieTime(gCurTime + 20000);
					}
				}
			}
		}
	}
}

void CFieldBossMonsterManager::SubDead(CFieldSubMonster* pSub)	
{
	FIELDGROUP* pGroup = NULL;
	
	DWORD ID = pSub->GetBossID();

	pGroup = m_FieldGroupTable.GetData(ID);

	if(pGroup)
	{
		// ���� ������ �����
		pGroup->m_SubList.Remove((void*)pSub);
		g_pAISystem.ReleaseMonsterID( pSub->GetID() );
	}
}

void CFieldBossMonsterManager::AddFieldBossMonster( CFieldBossMonster* pBoss )
{
	if(!pBoss)
		return;

	// ���� ������ �߰� ������ �����´�
	FIELDMONSTERINFO* pBossInfo = m_FieldMonsterInfoTable.GetData( pBoss->GetMonsterKind() );
	if(!pBossInfo)
		return;

	FIELDMONSTERINFO* pSubInfo = m_FieldMonsterInfoTable.GetData( pBossInfo->m_SubKind );
	
	pBossInfo->m_dwCountTime = pBossInfo->m_dwDistructTime;
	
	// �߰� ���� ����
	pBoss->SetInfo( pBossInfo );

	if(FIELDGROUP* const pGroup = new FIELDGROUP)
	{
		// �׷� ������ ID�� ���� ������ ID�� ����Ѵ�
		pGroup->m_GroupID = pBoss->GetMonsterGroupNum();
		pGroup->m_GridID = pBoss->GetGridID();
		pGroup->m_pBoss = pBoss;
		pGroup->m_BossObjectID = pBoss->GetID();

		pGroup->m_dwRegenTimeMin = pBossInfo->m_dwRegenTimeMin;
		pGroup->m_dwRegenTimeMax = pBossInfo->m_dwRegenTimeMax;

		if(pSubInfo)
		{
			pSubInfo->m_dwCountTime = pSubInfo->m_dwDistructTime;

			for(WORD count = 0; count < pBossInfo->m_SubCount; ++count)
			{
				VECTOR3 BossPos = {0};
				VECTOR3 TargetPos = {0};

				pBoss->GetPosition(&BossPos);

				// ���� ��ġ�κ��� +- 10 ���� �������� ����
				TargetPos.x = BossPos.x + ( (rand() % 2) ? -1 : 1 ) * (rand() % 10) * 100;
				TargetPos.z = BossPos.z + ( (rand() % 2) ? -1 : 1 ) * (rand() % 10) * 100;

				// ���� ���� ����
				CFieldSubMonster* pSub = (CFieldSubMonster*)REGENMGR->RegenObject(
					g_pAISystem.GeneraterMonsterID(),
					0,
					pBoss->GetGridID(),
					eObjectKind_FieldSubMonster,
					pBossInfo->m_SubKind,
					&TargetPos,
					EVENTRECALL_GROUP_ID == pGroup->m_GroupID ? EVENTRECALL_GROUP_ID : RECALL_GROUP_ID,
					0,
					100,
					FALSE,
					FALSE);

				if( pSub )
				{
					// �߰� ���� ����
					pSub->SetInfo( pSubInfo );
					pSub->SetBossID( pGroup->m_BossObjectID );

					// �׷� ������ ���� ���� �߰�
					pGroup->m_SubList.AddTail( pSub );
				}
			}
		}

		m_FieldGroupTable.Add( pGroup, pGroup->m_BossObjectID );
	}
}

FIELDBOSSDROPITEMLIST* CFieldBossMonsterManager::GetFieldBossDropItemList( WORD MonsterKind )
{
	FIELDBOSSDROPITEMLIST* pItemInfo = m_FieldBossDropItemTable.GetData(MonsterKind);

	return pItemInfo;
}

BOOL CFieldBossMonsterManager::IsRemainGroup(DWORD dwBossIndex)
{
	FIELDGROUP* pCheckGroupAlive = m_FieldGroupTable.GetData(dwBossIndex);
	if(pCheckGroupAlive)
		return TRUE;

	return FALSE;
}