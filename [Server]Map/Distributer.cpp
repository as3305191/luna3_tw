#include "stdafx.h"
#include "Distributer.h"
#include "UserTable.h"
#include "PartyManager.h"
#include "Party.h"
#include "Monster.h"
#include "BossRewardsManager.h"
#include "FieldBossMonsterManager.h"
#include "ItemManager.h"
#include "Player.h"
#include "ItemDrop.h"
#include "../hseos/Date/SHDateManager.h"
#include "../hseos/Family/SHFamilyManager.h"
#include "petmanager.h"
#include "pet.h"
#include "MapDBMsgParser.h"
#include "GameResourceManager.h"

float gEventRate[eEvent_Max];
float gEventRateFile[eEvent_Max];

CDistributer::CDistributer()																	// ������ �Լ�.
{
	m_DamageObjectTableSolo.Initialize(2);
	m_DamageObjectTableParty.Initialize(2);

	m_1stPartyID = 0;
	m_1stPlayerID = 0;
	m_TotalDamage = 0;
	m_FirstDamage = 0;
	m_nTotalKillerCount = 0;
	m_HighLevel = 0;
	m_pKiller = NULL;
	m_pMob = NULL;
	m_dwDropItemID = 0;
	m_dwDropItemRatio = 0;
}

CDistributer::~CDistributer()																	// �Ҹ��� �Լ�.
{
	Release();																					// ���� �Լ��� ȣ���Ѵ�.
}

void CDistributer::Release()																	// ���� �Լ�.
{
	m_1stPartyID = 0;																			// ù �������� �� ��Ƽ ���̵� �ʱ�ȭ.
	m_1stPlayerID = 0;																			// ù �������� �� �÷��̾� ���̵� �ʱ�ȭ.
	m_TotalDamage = 0;																			// ������ �հ� �ʱ�ȭ.
	m_FirstDamage = 0 ;																			// ù ������ �ʱ�ȭ.
	m_nTotalKillerCount = 0 ;																	// ų�� �� �ʱ�ȭ.
	m_HighLevel = 0 ;																			// ų�� �� �� ���� ������ ���� ų���� ����.

	DAMAGEOBJ *sobj, *pobj = NULL;																// ���� �� ��Ƽ ������ ���̺��� ������.

	m_DamageObjectTableSolo.SetPositionHead();													// ���� ������ ���̺��� ����� �����Ѵ�.

	while((sobj = m_DamageObjectTableSolo.GetData())!= NULL)												// ������ ������Ʈ�� ���� �� ���� while.
	{
		delete sobj;																			// ������ ������Ʈ�� �����Ѵ�.
		sobj = NULL ;																			// ������ ������Ʈ NULLó��.
	}

	m_DamageObjectTableSolo.RemoveAll();														// ������ ������Ʈ�� ����.				

	m_DamageObjectTableParty.SetPositionHead();													// ��Ƽ ������ ���̺��� ����� �����Ѵ�.

	while((pobj = m_DamageObjectTableParty.GetData())!= NULL)											// ������ ������Ʈ�� ���� �� ���� while.		
	{																							
		delete pobj;																			// ������ ������Ʈ�� �����Ѵ�.
		pobj = NULL ;																			// ������ ������Ʈ NULLó��.
	}																							
																								
	m_DamageObjectTableParty.RemoveAll();														// ������ ������Ʈ�� ����.					

	m_pKiller = NULL;
	m_pMob = NULL;
	m_dwDropItemID		= 0 ;																	// ��� ������ ���̵� ��� ���� �ʱ�ȭ.
	m_dwDropItemRatio	= 0 ;																	// ������� ��� ���� �ʱ�ȭ.							

    mPlayerIndexContainer.clear();
}

void CDistributer::AddDamageObject(CPlayer* pPlayer, DWORD damage, DWORD plusdamage)
{
	if( pPlayer ) 																				// �÷��̾� ������ ��ȿ���� üũ�Ѵ�.
	{
		DoAddDamageObj(
			m_DamageObjectTableSolo,
			pPlayer->GetID(),
			damage + plusdamage);

		//if(plusdamage != 0)																		// �߰� �������� �ִ��� üũ�Ѵ�.
		if( m_1stPlayerID == 0 )
		{
			m_1stPlayerID = pPlayer->GetID();													// ù �������� �� �÷��̾� ���̵� �޴´�.
			m_1stPartyID = pPlayer->GetPartyIdx();												// ù �������� �� �÷��̾��� ��Ƽ �ε����� �޴´�.
		}
		
		m_TotalDamage += damage + plusdamage;
	}
}

void CDistributer::DoAddDamageObj(CYHHashTable<DAMAGEOBJ>& pTable, DWORD dwID, DWORD damage)
{
	if(DAMAGEOBJ* pDObj = pTable.GetData(dwID))
	{
		pDObj->dwData += damage;
	}
	else
	{
		pDObj = new DAMAGEOBJ;
		ZeroMemory(
			pDObj,
			sizeof(*pDObj));
		pDObj->dwID = dwID;
		pDObj->dwData = damage;
		
		pTable.Add(
			pDObj,
			dwID);
	}
}


BOOL CDistributer::Chk(CPlayer& pPlayer, DWORD GridID)
{
	if(pPlayer.GetInited())
	{
		if(FALSE == pPlayer.IsShowdown())
		{
			VECTOR3 PlayerPosition = {0};
			pPlayer.GetPosition(
				&PlayerPosition);

			const float fDistance = CalcDistanceXZ(
				&m_pKilledPosition,
				&PlayerPosition);

			if(fDistance <= POINT_VALID_DISTANCE)
			{
				if(pPlayer.GetState() != eObjectState_Die)
				{
					if(pPlayer.GetGridID() == GridID)
					{
						return TRUE;
					}
				}
			}
		}
	}

	return FALSE ;																				// FALSE ����.
}

void CDistributer::ChooseOne(const DAMAGEOBJ& pobj, DWORD& pBigDamage, DWORD& pBigID)
{
	if(pBigDamage < pobj.dwData)
	{
		pBigDamage = pobj.dwData;
		pBigID = pobj.dwID;
	}
	else if(pBigDamage == pobj.dwData)
	{
		if(1 == (rand() % 2))
		{
			pBigID = pobj.dwID;
		}
	}
}

void CDistributer::DistributeItemPerDamage(CMonster& pMob)
{
	CPlayer* TargetPlayerTable[100] = {0};
	WORD TargetPos = 0;
	
	if(FIELDBOSSDROPITEMLIST* const pDropItemList = FIELDBOSSMONMGR->GetFieldBossDropItemList(pMob.GetMonsterKind()))													
	{
		for(WORD index = 0; index < MAX_FIELDBOSS_DROPITEM_GROUP_NUM; index++)
		{
			// ��� ������ ������ �ִ°�? ���ٸ� ����
			if( pDropItemList->m_ItemList[ index ].m_wItemIndex[0] == 0 )
				break;
	        
			// ������ �׷� ���� � �������� ���� ����
			WORD dropitemnum;
			for( dropitemnum = 0; dropitemnum < MAX_FIELDBOSS_DROPITEM_NUM; dropitemnum++)
			{
				if(pDropItemList->m_ItemList[ index ].m_wItemIndex[dropitemnum] == 0)
					break;
			}

			WORD select = WORD( rand() % dropitemnum );

			// ����� Ƚ���� �����´�
			WORD count = pDropItemList->m_ItemList[ index ].m_wCount;

			for(WORD i = 0; i < count; i++)
			{
				// ��� �Ұ��ΰ�?
				if( (rand() % 100) < pDropItemList->m_ItemList[ index ].m_wDropRate )
				{
					// �����ʱ�ȭ
					memset( TargetPlayerTable, 0, sizeof(CPlayer*) * 100 );
					TargetPos = 0;

					// ������ ������� �˻��ؼ� ����ġ �̻� �������� �� ����� ��������
					m_DamageObjectTableSolo.SetPositionHead();

					while(const DAMAGEOBJ* const obj = m_DamageObjectTableSolo.GetData())
					{
						if(obj->dwData == 0)
							continue;

						CPlayer* const pReceivePlayer = (CPlayer *)g_pUserTable->FindUser(obj->dwID);

						if(!pReceivePlayer)	
							continue;
						if(pReceivePlayer->GetInited())
							continue;
						if(pReceivePlayer->GetGridID() != pMob.GetGridID())
							continue;
						if(pReceivePlayer->GetLevel() > pMob.GetSMonsterList().Level + 8 )
							continue;
						
						// ������ ���� ���
						WORD DamageRate = (WORD)(((double)obj->dwData / m_TotalDamage) * 100);

						// ����ġ �̻� �������� �־��ٸ�
						if( DamageRate >= pDropItemList->m_ItemList[ index ].m_wDamageRate )
						{
							// ������ ������ŭ ��� ���̺� ����Ѵ�
							for(DWORD rate = 0; rate < DamageRate; rate++)
							{
								TargetPlayerTable[ TargetPos++ ] = pReceivePlayer;
							}
						}
					}

					// ��� ���̺� ��ϵ� ������ �ִٸ�
					if( TargetPos > 0 )
					{
						// �������� ���̺� ��ġ�� �����Ѵ�
						WORD TargetIndex = WORD( rand() % TargetPos );

						if(CPlayer* const TargetPlayer = TargetPlayerTable[TargetIndex])
						{
							ITEMMGR->MonsterObtainItem(
								TargetPlayer,
								pDropItemList->m_ItemList[index].m_wItemIndex[select],
								pMob.GetMonsterKind(),
								pDropItemList->m_ItemList[index].m_wItemCount[select]);
						}
					}
				}
			}
		}
	}

	
}

void CDistributer::DistributePerDamage(CMonster& pMob)
{	
	MONEYTYPE MaxMoney = 0;
	DWORDEX MaxExp = 0;

	BOSSREWARDSMGR->GetBossRewardsInfo(
		pMob.GetMonsterKind(),
		MaxExp,
		MaxMoney);

	m_DamageObjectTableSolo.SetPositionHead();

	while(DAMAGEOBJ* const obj = m_DamageObjectTableSolo.GetData())
	{
		if(obj->dwData == 0)
			continue;
		CPlayer* pReceivePlayer = (CPlayer *)g_pUserTable->FindUser(obj->dwID);

		if(!pReceivePlayer)	
			continue;
		if(pReceivePlayer->GetInited())
			continue;
		if(pReceivePlayer->GetGridID() != pMob.GetGridID())
			continue;

		EXPTYPE Exp = (DWORD)(((double)obj->dwData * MaxExp) / m_TotalDamage);
		MONEYTYPE Money = (DWORD)(((double)obj->dwData * MaxMoney) / m_TotalDamage);

		float fBuffExp = pReceivePlayer->GetRateBuffStatus()->GetExp;
		Exp = (DWORDEX)(Exp*gEventRate[eEvent_ExpRate]) ;
		Exp += EXPTYPE(Exp*(fBuffExp/100)) ;

		if(pMob.GetObjectKind() == eObjectKind_FieldBossMonster)
		{
			if(pReceivePlayer->GetLevel() > pMob.GetSMonsterList().Level + 6 )
				continue;

			if(Exp == 0) Exp = 1;
			if(Money == 0) Money = 1;

			pReceivePlayer->AddPlayerExpPoint(Exp);
			AddMoney(
				pReceivePlayer,
				Money,
				pMob.GetMonsterKind());
		}
		else
		{
			if(Exp >= 10000)
				pReceivePlayer->AddPlayerExpPoint(Exp);

			if(Money >= 100000)
				AddMoney(
					pReceivePlayer,
					Money,
					pMob.GetMonsterKind());
		}
	}
}

void CDistributer::DamageInit()
{
	m_1stPartyID = 0;																			// ù �������� �� ��Ƽ ���̵� �ʱ�ȭ.
	m_1stPlayerID = 0;																			// ù �������� �� �÷��̾� ���̵� �ʱ�ȭ.
	m_TotalDamage = 0;																			// ������ �հ� �ʱ�ȭ.
	m_FirstDamage = 0 ;																			// ù ������ �ʱ�ȭ.
	m_nTotalKillerCount = 0 ;																	// ų�� �� �ʱ�ȭ.
	m_HighLevel = 0 ;																			// ų�� �� �� ���� ������ ���� ų���� ����.

	m_DamageObjectTableSolo.RemoveAll();														// ������ ������Ʈ ���̺��� ��� ����.	

	m_pKiller = NULL;
	m_pMob = NULL;
	m_dwDropItemID		= 0 ;																	// ��� ������ ���̵� ��� ���� �ʱ�ȭ.
	m_dwDropItemRatio	= 0 ;																	// ������� ��� ���� �ʱ�ȭ.							
}

void CDistributer::DeleteDamagedPlayer(DWORD CharacterID)
{	
	DAMAGEOBJ* pData = NULL ;																	// ������ ������Ʈ ������ ���� �����͸� ���� �� NULL �ʱ�ȭ.

	pData = m_DamageObjectTableSolo.GetData( CharacterID );										// ���� ���̵�� ������ ���̺��� ������ ������Ʈ �����͸� �޴´�.

	if( pData )																					// ������ ������Ʈ �����Ͱ� ��ȿ���� üũ�Ѵ�.
	{
		if( m_TotalDamage >= pData->dwData )													// �������� �������� ������ �հ躸�� ũ�ų� ������,
		{
			m_TotalDamage -= pData->dwData;														// ������ �հ迡�� �������� ������ ��ŭ ����.
		}
		else																					// �������� �������� ������ �հ躸�� ������
		{
			m_TotalDamage = 0;																	// �հ� �������� 0���� �����Ѵ�.
		}
	} 

	m_DamageObjectTableSolo.Remove(CharacterID);												// ������ ������Ʈ ���̺��� �־��� ���̵��� ������ ������Ʈ�� ����.
}

void CDistributer::SetInfoToDistribute(DWORD dwKillerID, DWORD DropItemID, DWORD DropItemRatio,CMonster& pMob)
{
	CObject* pKiller = g_pUserTable->FindUser(dwKillerID);

	pMob.GetPosition(
		&m_pKilledPosition);

	if( pKiller->GetObjectKind() == eObjectKind_Player )
	{
		m_pKiller = (CPlayer *)pKiller;
	}
	else if( pKiller->GetObjectKind() == eObjectKind_Pet )
	{
		CObject* const object = g_pUserTable->FindUser(
			pKiller->GetOwnerIndex());

		if(object &&
			eObjectKind_Player == object->GetObjectKind())
		{
			m_pKiller = (CPlayer*)object;
		}
	}
	else if(CObject* const owner = g_pUserTable->FindUser(pKiller->GetOwnerIndex()))
	{
		if(eObjectKind_Player == owner->GetObjectKind())
		{
			m_pKiller = (CPlayer*)owner;
		}
	}

	m_pMob = &pMob;
	m_dwDropItemID = DropItemID;
	m_dwDropItemRatio = DropItemRatio;
}		

// 091111 pdy ��Ƽ ������ ���� ����� ���� ����ÿ� �ѹ� ��� �ϵ��� ����
void CDistributer::Distribute()
{
	GetAllPartyDamage();
	GetHighLevelOfKillers();
	GetTotalKillerCount();

	if( m_nTotalKillerCount > 1 )																// ���͸� óġ �� �÷��̾ �ټ����, 
	{
		DistributeToKillers() ;																	// �ټ� ó���� �Ѵ�.
	}
	else																						// ���͸� óġ �� �÷��̾ �Ѹ��̶��, 
	{
		DistributeToKiller() ;																	// ���� ó���� �Ѵ�.
	}

	Release();																					// ������ ���̺� �� ��� ���� ������ �ʱ�ȭ �Ѵ�.
}

void CDistributer::DistributeToKillers()
{
	if(!GetFirstDamange())																		// �������� �������� ���Ѵ�.
	{
		if( m_pMob->GetObjectKind() == eObjectKind_BossMonster )
		{
			SYSTEMTIME time;
			char szFile[256] = {0,};
			GetLocalTime( &time );

			sprintf(szFile, "./Log/BossMonsterAssertLog_%02d_%04d%02d%02d.txt", g_pServerSystem->GetMapNum(), time.wYear, time.wMonth, time.wDay );
			FILE* fp;
			fp = fopen(szFile, "a+");
			if (fp)
			{
				fprintf(fp, "BossMonster Not FirstDamage Return\n");
				fclose(fp);
			}
		}
	}

	int nPenaltyType = GetLevelPenaltyToMonster(m_HighLevel);

	MONEYTYPE money = ITEMDROP_OBJ->DropMoney(
		m_pMob->GetSMonsterList(),
		nPenaltyType);

	if( m_pMob->GetObjectKind() == eObjectKind_BossMonster )
	{
		SYSTEMTIME time;
		char szFile[256] = {0,};
		GetLocalTime( &time );

		sprintf(szFile, "./Log/BossMonsterLog_%02d_%04d%02d%02d_%d.txt", g_pServerSystem->GetMapNum(), time.wYear, time.wMonth, time.wDay, m_pMob->GetMonsterKind() );
		FILE* fp;
		fp = fopen(szFile, "a+");
		if (fp)
		{
			fprintf(fp, "Time : [%2d:%2d], HighLevel : %d, Drop Money : %u, nPenaltyType : %d\n", time.wHour, time.wMinute, m_HighLevel, money, nPenaltyType);

			m_DamageObjectTableSolo.SetPositionHead() ;

			while(DAMAGEOBJ* const obj = m_DamageObjectTableSolo.GetData())
			{
				if(CObject* const object = g_pUserTable->FindUser(obj->dwID))
				{
					if(object->GetGridID() == m_pMob->GetGridID())
					{
						fprintf(
							fp,
							"PlayerID : %u,		Damage : %u,	Level : %u,	CharacterName : %s\n",
							obj->dwID,
							obj->dwData,
							object->GetLevel(),
							object->GetObjectName());
					}
				}
			}

			m_DamageObjectTableParty.SetPositionHead();

			while(DAMAGEOBJ* const obj = m_DamageObjectTableParty.GetData())
			{
				CParty* pParty = PARTYMGR->GetParty(obj->dwID) ;
				if( !pParty ) continue;

				fprintf(fp, "\n[PartyIdx : %u][PartyDamage : %u]\n", obj->dwID, obj->dwData);

				for(int i=0; i<MAX_PARTY_LISTNUM; i++)
				{
					if(CObject* const object = g_pUserTable->FindUser(pParty->GetMemberID(i)))
					{
						if(object->GetGridID() == m_pMob->GetGridID())
						{
							fprintf(fp, "[PartyMemberIdx : %d]\n", pParty->GetMemberID(i));
						}
					}
				}
			}

			fclose(fp);
		}
	}

	DistributerToSolo(money) ;																	// ���� ����ġ�� �Ӵ� ó���� �Ѵ�.

	DistributeToPartys(money) ;																	// ��Ƽ ����ġ�� �Ӵ� ó���� �Ѵ�.

	DistributeItemToKillers() ;																	// ������ ó���� �Ѵ�.
}

void CDistributer::DistributerToSolo(MONEYTYPE money)
{
	BYTE byPet_ExpPenaltyLevel = PETMGR->GetExpPenaltyLevel() ;

	m_DamageObjectTableSolo.SetPositionHead() ;													// ������ ���̺��� �ص�� �����Ѵ�.

	while(const DAMAGEOBJ* const obj = m_DamageObjectTableSolo.GetData())
	{
		if(obj->dwData > 0)																	// �������� �ִ��� ���θ� üũ�Ѵ�.
		{
			if(CPlayer* const pPlayer = (CPlayer *)g_pUserTable->FindUser(obj->dwID))
			{
				if(pPlayer->GetGridID() != m_pMob->GetGridID())
				{
					continue;
				}
				else if(0 == PARTYMGR->GetParty(pPlayer->GetPartyIdx()))
				{
					if( pPlayer->GetID() == m_1stPlayerID )									// ù �������� �� �÷��̾���, 
					{
						DistributeToFirstKiller(
							*pPlayer,
							obj->dwData);
					}
					else
					{
						DistributeToOtherKiller(
							*pPlayer,
							obj->dwData);
					}

					if(CPet* const pPet = PETMGR->GetPet(pPlayer->GetPetItemDbIndex()))
					{
						if( pPlayer->GetLevel() < m_pMob->GetLevel() + byPet_ExpPenaltyLevel )
						{
							pPet->AddExp() ;
						}
					}

					float fDamageRate = (float)obj->dwData / (float)(m_TotalDamage+(m_FirstDamage*0.3f))  ;		// �� ���������� �÷��̾��� ������ ������� ���Ѵ�.

					float fMoney = money * fDamageRate ;									// ���Ͱ� ����� �ӴϿ��� ������ ������ �Ӵϸ� �޴´�.

					MONEYTYPE dwMoney = (DWORD)fMoney ;										// �Ҽ� �������� �Ӵ� ����.
					SendMoneyToPerson(pPlayer, dwMoney) ;									// �Ӵϸ� �����Ѵ�.
				}
			}
		}
	}
}

void CDistributer::DistributeToFirstKiller(CPlayer& pPlayer, DWORD dwMyDamage)
{
	if(pPlayer.GetState() == eObjectState_Die)
	{
		return;
	}

	DWORD dwMonsterExp = CalcObtainExp(
		pPlayer.GetLevel(),
		1);

	if( m_TotalDamage < 1 )																	// ��Ż �������� 1���� ������,
	{
		m_TotalDamage = 1 ;																	// ��Ż �������� 1�� �����Ѵ�.
	}

	float fExpVal_A = (float)(dwMyDamage * 0.3f) ;											// �ڽ��� ������ * 0.3
	float fExpVal_B = (float)(dwMyDamage + fExpVal_A) ;										// �ڽ��� ������ + fExpVal_A
	float fExpVal_C = (float)(fExpVal_B / (m_TotalDamage+fExpVal_A)) ;						// fExpVal_B / ��� ������ ������ ����
	float fExpVal_D = (float)(fExpVal_C * dwMonsterExp) ;									// fExpVal_C * ���� ����ġ

	// 080102 LYW --- Distributer : �ַ� �÷��̾� ����ġ �߰� ó��.
	DWORD dwCurExp = (DWORD)floor(fExpVal_D) ;
	DWORD dwExp = dwCurExp + (DWORD)(dwCurExp*0.15f) ;												// �ݿø� �� ����ġ�� �޴´�.

	SendToPersonalExp(
		&pPlayer,
		dwExp);

	if( pPlayer.GetLevel() < m_pMob->GetLevel() + PETMGR->GetExpPenaltyLevel() )
	{
		CPet* const petObject = PETMGR->GetPet(
			pPlayer.GetPetItemDbIndex());

		if(petObject)
		{
			petObject->AddExp();
		}
	}

	g_csFamilyManager.SRV_ProcessHonorPoint(
		&pPlayer,
		dwExp);
}

void CDistributer::DistributeToOtherKiller(CPlayer& pPlayer, DWORD dwMyDamage)
{
	DWORD dwMonsterExp = CalcObtainExp(pPlayer.GetLevel(), 1) ;
	int nKillerCount = 1;

	if(m_nTotalKillerCount > 1)
	{
		nKillerCount = m_nTotalKillerCount-1;
	}

	if( m_TotalDamage < 1 )
	{
		m_TotalDamage = 1;
	}

	float fExpVal_A = (float)(m_FirstDamage * 0.3f) ;										// �������� ������ * 0.3
	float fExpVal_B = (float)(fExpVal_A / (nKillerCount)) ;									// fExpVal_A / (������ �Ѽ� - 1)
	if( fExpVal_B <= 1 ) fExpVal_B = 1 ;
	float fExpVal_C = (float)(dwMyDamage - fExpVal_B) ;										// �ڽ��� ������ - fExpVal_B
	if( fExpVal_C <= 1 ) fExpVal_C = 1 ;
	float fExpVal_D = (float)(fExpVal_C / (m_TotalDamage+fExpVal_A)) ;									// fExpVal_C / ��� ������ ������ ����
	float fExpVal_E = (float)(fExpVal_D * dwMonsterExp) ;									// fExpVal_D * ���� ����ġ.

	DWORD dwCurExp = (DWORD)floor(fExpVal_E);
	DWORD dwExp = dwCurExp + (DWORD)(dwCurExp*0.15f);

	SendToPersonalExp(
		&pPlayer,
		dwExp);

	if( pPlayer.GetLevel() < m_pMob->GetLevel() + PETMGR->GetExpPenaltyLevel() )
	{
		if(CPet* const petObject = PETMGR->GetPet(pPlayer.GetPetItemDbIndex()))
		{
			petObject->AddExp();
		}
	}

	g_csFamilyManager.SRV_ProcessHonorPoint(
		&pPlayer,
		dwExp);
}

void CDistributer::DistributeToPartys(MONEYTYPE money)
{
	m_DamageObjectTableParty.SetPositionHead();

	while(DAMAGEOBJ* const obj = m_DamageObjectTableParty.GetData())
	{
		if(obj->dwData > 0)																	// �������� �ִ��� ���θ� üũ�Ѵ�.
		{
			if(CParty* const pParty = PARTYMGR->GetParty(obj->dwID))
			{
				if( pParty->GetPartyIdx() == m_1stPartyID )									// ù �������� �� ��Ƽ���, 
				{
					DistributeToFirstParty(
						*pParty,
						obj->dwData);
				}
				else
				{
					DistributeToOtherParty(
						*pParty,
						obj->dwData);
				}

				float fDamageRate = (float)obj->dwData / (float)(m_TotalDamage+(m_FirstDamage*0.3f))  ;			// ��ü �������� ���� ��Ƽ �������� ������� ���Ѵ�.

				float fMoney = money * fDamageRate ;										// ���Ͱ� ����� �ӴϿ��� ������ ������ŭ �Ӵϸ� ���Ѵ�.

				SendMoneyToPartys(fMoney) ;													// ��Ƽ�� �Ӵϸ� �����Ѵ�.
			}
		}
	}
}

void CDistributer::DistributeToFirstParty(CParty& pParty, DWORD dwPartyDamage)
{
	LEVELTYPE highLevel = 0;
	LEVELTYPE totalLevel = 0;

	SaveCandidacy(
		pParty);
	GetPartyLevels(
		pParty,
		highLevel,
		totalLevel);

	DWORD dwMonsterExp = CalcObtainExp(highLevel, mPlayerIndexContainer.size()) ;

	if( m_TotalDamage < 1 )
	{
		m_TotalDamage = 1 ;
	}

	float fExpVal_A = (float)(dwPartyDamage * 0.3f) ;										// �ڽ��� ��Ƽ������ * 0.3
	float fExpVal_B = (float)(dwPartyDamage + fExpVal_A) ;									// �ڽ��� ��Ƽ������ + fExpVal_A
	float fExpVal_C = (float)(fExpVal_B / (m_TotalDamage+fExpVal_A)) ;						// fExpVal_B / ��� ��Ƽ�� ������ ����
	float fExpVal_D = (float)(fExpVal_C * dwMonsterExp) ;									// fExpVal_C * ���� ����ġ.			

	if(mPlayerIndexContainer.empty())
	{
		return;
	}
	else if(1 == mPlayerIndexContainer.size())
	{
		if(CPlayer* const pMember = (CPlayer*)g_pUserTable->FindUser(*mPlayerIndexContainer.begin()))
		{
			if(pMember->GetGridID() != m_pMob->GetGridID())
			{
				return;
			}

			DWORD dwExp = (DWORD)fExpVal_D;
			g_csFamilyManager.SRV_ProcessHonorPoint(pMember, dwExp);

			DWORD dwCurExp = dwExp + (DWORD)(dwExp*0.15f) ;
			SendToPersonalExp(
				pMember,
				dwCurExp);

			if( pMember->GetLevel() < m_pMob->GetLevel() + PETMGR->GetExpPenaltyLevel() )
			{
				CPet* const petObject = PETMGR->GetPet(
					pMember->GetPetItemDbIndex());

				if(petObject)
				{
					petObject->AddExp();
				}
			}
		}
	}
	// ������ ���� �ٸ� �÷��̾ ������
	else
	{
		BOOL bLevelPenalty = FALSE ;
		LEVELTYPE memberLevel ;
		LEVELTYPE lowLevel = 150 ;
		float fTotalLvWeight = 0 ;

		GetPartyLevels(
			pParty,
			highLevel,
			totalLevel) ;

		for(IndexContainer::const_iterator iterator = mPlayerIndexContainer.begin();
			mPlayerIndexContainer.end() != iterator;
			++iterator)
		{
			if(CPlayer* pMember = (CPlayer*)g_pUserTable->FindUser(*iterator))
			{
				memberLevel = pMember->GetLevel();
				// ��������	= ( ��� - ( �ְ��� - �ڱⷹ�� ) ) / ���
				fTotalLvWeight += abs( (float)( MAX_CHARACTER_LEVEL_NUM - ( highLevel - memberLevel ) ) / (float)MAX_CHARACTER_LEVEL_NUM );
			}
		}

		if( (highLevel - lowLevel) >= 21 ) bLevelPenalty = TRUE ;

		float applyRate = 0.0f ;

		switch(mPlayerIndexContainer.size())
		{
		case 2 : applyRate = 0.50f ;	break ;
		case 3 : applyRate = 0.86f ;	break ;
		case 4 : applyRate = 1.21f ;	break ;
		case 5 : applyRate = 1.58f ;	break ;
		case 6 : applyRate = 1.98f ;	break ;
		case 7 : applyRate = 2.41f ;	break ;
		}

		if( bLevelPenalty )
		{
			applyRate = 0.0f ;
		}

		for(IndexContainer::const_iterator iterator = mPlayerIndexContainer.begin();
			mPlayerIndexContainer.end() != iterator;
			++iterator)
		{
			if(CPlayer* const pMember = (CPlayer*)g_pUserTable->FindUser(*iterator))
			{
				if(pMember->GetGridID() != m_pMob->GetGridID())
				{
					continue;
				}

				memberLevel = pMember->GetLevel();

				// ��������	= ( ( ��� - ( �ְ��� - �ڱⷹ�� ) ) / ��� ) / �����Ƽ���� �������� ���� )
				float fExpA = ( abs( (float)( MAX_CHARACTER_LEVEL_NUM - ( highLevel - memberLevel ) ) / (float)MAX_CHARACTER_LEVEL_NUM ) ) / fTotalLvWeight;
				float fExpB = (fExpVal_D * ( 1.f + applyRate ));
				float fExp = fExpA * fExpB;

				if(fExp > 0)
				{
					DWORD dwExp = (DWORD)fExp;

					g_csFamilyManager.SRV_ProcessHonorPoint(pMember, dwExp);

					if( (highLevel - memberLevel) >= 21 )
					{
						SendToPersonalExp(pMember,1);
					}
					else
					{
						SendToPersonalExp(pMember,dwExp);
					}

					if(CPet* const pPet = PETMGR->GetPet(pMember->GetPetItemDbIndex()))
					{
						if( pMember->GetLevel() < m_pMob->GetLevel() + PETMGR->GetExpPenaltyLevel() )
						{
							pPet->AddExp() ;
						}
					}
				}
			}
		}
	}
}

void CDistributer::DistributeToOtherParty(CParty& pParty, DWORD dwPartyDamage)
{
	LEVELTYPE highLevel = 0;
	LEVELTYPE totalLevel = 0;

	SaveCandidacy(
		pParty);
	GetPartyLevels(
		pParty,
		highLevel,
		totalLevel);

	DWORD dwMonsterExp = CalcObtainExp(highLevel, mPlayerIndexContainer.size());
	int nKillerCount = 1;

	if( m_nTotalKillerCount > 1 )
	{
		nKillerCount = m_TotalDamage -1;
	}

	if( m_TotalDamage < 1 )
	{
		m_TotalDamage = 1;
	}

	float fExpVal_A = (float)(m_FirstDamage * 0.3f) ;										// ������ ��Ƽ ������ * 0.3
	float fExpVal_B = (float)(fExpVal_A / (nKillerCount)) ;									// fExpVal_A / (��Ƽ �� �� - 1)

	if( fExpVal_B <= 1 ) fExpVal_B = 1 ;													// ���� üũ�� �Ѵ�.

	float fExpVal_C = (float)(dwPartyDamage - fExpVal_B) ;									// �ڽ��� ��Ƽ ������ - fExpVal_B

	if( fExpVal_C <= 1 ) fExpVal_C = 1 ;													// ���� üũ���Ѵ�.

	float fExpVal_D = (float)(fExpVal_C / (m_TotalDamage+fExpVal_A)) ;						// fExpVal_C / ��� ��Ƽ�� ������ ����
	float fExpVal_E = (float)(fExpVal_D * dwMonsterExp) ;

	if(mPlayerIndexContainer.empty())
	{
		return;
	}

	if(1 == mPlayerIndexContainer.size())
	{
		if(CPlayer* pMember = (CPlayer*)g_pUserTable->FindUser(*mPlayerIndexContainer.begin()))
		{
			if(pMember->GetGridID() != m_pMob->GetGridID())
			{
				return;
			}

			DWORD dwExp = (DWORD)fExpVal_E ;
			g_csFamilyManager.SRV_ProcessHonorPoint(
				pMember,
				dwExp);
			SendToPersonalExp(
				pMember,
				dwExp);

			if( pMember->GetLevel() < m_pMob->GetLevel() + PETMGR->GetExpPenaltyLevel() )
			{
				if(CPet* const petObject = PETMGR->GetPet(pMember->GetPetItemDbIndex()))
				{
					petObject->AddExp();
				}
			}
		}
	}
	// ������ ���� �ٸ� �÷��̾ ������
	else
	{
		BOOL bLevelPenalty = FALSE;
		LEVELTYPE memberLevel;
		LEVELTYPE lowLevel = 150;
		float fTotalLvWeight = 0;

		for(IndexContainer::const_iterator iterator = mPlayerIndexContainer.begin();
			mPlayerIndexContainer.end() != iterator;
			++iterator)
		{
			if(CPlayer* const pMember = (CPlayer*)g_pUserTable->FindUser(*iterator))														// ��� ������ ��ȿ���� üũ�Ѵ�.
			{
				memberLevel = pMember->GetLevel();
				// ��������	= ( ��� - ( �ְ��� - �ڱⷹ�� ) ) / ���
				fTotalLvWeight += abs( (float)( MAX_CHARACTER_LEVEL_NUM - ( highLevel - memberLevel ) ) / (float)MAX_CHARACTER_LEVEL_NUM );
			}
		}

		if( (highLevel - lowLevel) >= 21 ) bLevelPenalty = TRUE ;

		float applyRate = 0.0f;

		switch(mPlayerIndexContainer.size())
		{
		case 2 : applyRate = 0.50f ;	break ;
		case 3 : applyRate = 0.86f ;	break ;
		case 4 : applyRate = 1.21f ;	break ;
		case 5 : applyRate = 1.58f ;	break ;
		case 6 : applyRate = 1.98f ;	break ;
		case 7 : applyRate = 2.41f ;	break ;
		}

		if( bLevelPenalty )
		{
			applyRate = 0.0f ;
		}

		for(IndexContainer::const_iterator iterator = mPlayerIndexContainer.begin();
			mPlayerIndexContainer.end() != iterator;
			++iterator)
		{
			if(CPlayer* const pMember = (CPlayer*)g_pUserTable->FindUser(*iterator))
			{
				if(pMember->GetGridID() != m_pMob->GetGridID())
				{
					continue;
				}

				memberLevel = pMember->GetLevel() ;

				// ��������	= ( ( ��� - ( �ְ��� - �ڱⷹ�� ) ) / ��� ) / �����Ƽ���� �������� ���� )
				float fExpA = ( abs( (float)( MAX_CHARACTER_LEVEL_NUM - ( highLevel - memberLevel ) ) / (float)MAX_CHARACTER_LEVEL_NUM ) ) / fTotalLvWeight;
				float fExpB = (fExpVal_E * ( 1.f + applyRate ));
				float fExp = fExpA * fExpB ;

				if(fExp > 0)
				{
					DWORD dwExp = (DWORD)fExp ;
					g_csFamilyManager.SRV_ProcessHonorPoint(pMember, dwExp);

					if( (highLevel - memberLevel) >= 21 )
					{
						SendToPersonalExp(pMember,1);
					}
					else
					{
						SendToPersonalExp(pMember,dwExp);
					}

					if(CPet* const pPet = PETMGR->GetPet(pMember->GetPetItemDbIndex()))
					{
						if( pMember->GetLevel() < m_pMob->GetLevel() + PETMGR->GetExpPenaltyLevel() )
						{
							pPet->AddExp() ;
						}
					}
				}
			}
		}
	}
}

void CDistributer::SendMoneyToPartys(float ChangeValue)
{
	if(mPlayerIndexContainer.empty())
	{
		return;
	}

	float fMoney = ChangeValue / float(mPlayerIndexContainer.size());
	MONEYTYPE dwMoney = (DWORD)fMoney ;

	if(0 == m_pMob)
	{
		return;
	}

	for(IndexContainer::const_iterator iterator = mPlayerIndexContainer.begin();
		mPlayerIndexContainer.end() != iterator;
		++iterator)
	{
		CPlayer* pMember = (CPlayer*)g_pUserTable->FindUser(*iterator) ;						// ��� ���̵�� ��� ������ �޴´�.

		if( pMember )																	// ��� ������ ��ȿ���� üũ�Ѵ�.
		{
			if(pMember->GetGridID() != m_pMob->GetGridID())
			{
				continue;
			}

			if( pMember->SetMoney(dwMoney, MONEY_ADDITION, MF_OBTAIN, eItemTable_Inventory,	// �� ������� �Ӵϸ� �����Ѵ�.
				eMoneyLog_GetMonster, m_pMob->GetMonsterKind()) != dwMoney )
			{
				// error msg ������. ���ѷ� �ʰ�
				MSGBASE msg;
				msg.Category = MP_ITEM;
				msg.Protocol = MP_ITEM_MONEY_ERROR;
				msg.dwObjectID = pMember->GetID();

				pMember->SendMsg(&msg, sizeof(msg));
			}
		}
	}
}





//=========================================================================
//	NAME : DistributeItemToKillers
//	DESC : The function to distribute item to killers. 080114 LYW
//=========================================================================
void CDistributer::DistributeItemToKillers() 
{
	BOOL bNoPlayer = FALSE ;
	BOOL bNoParty  = FALSE ;
	DWORD dwEventRewardItem = 0;
	if(m_pMob->GetObjectKind()==eObjectKind_BossMonster)
		dwEventRewardItem = BOSSREWARDSMGR->GetBossEventRewardItem(m_pMob->GetMonsterKind());
	else
		dwEventRewardItem = GAMERESRCMNGR->GetMonsterEventRewardItem(m_pMob->GetMonsterKind());

	// ���� �÷��̾� �� ������ ȹ�� �ĺ��� �̴´�.

	DWORD dwBigPlayerDamage = 0 ;																// ���� ū ���� �������� ���� ����.
	DWORD dwBigPlayerID = 0 ;																	// ���� ū �������� �� �÷��̾� ���̵� ���� ����.

	m_DamageObjectTableSolo.SetPositionHead() ;													// ������ ���̺��� �ص�� �����Ѵ�.

	while(const DAMAGEOBJ* const obj = m_DamageObjectTableSolo.GetData())
	{
		CPlayer* const pPlayer = (CPlayer*)g_pUserTable->FindUser(obj->dwID);

		if( !pPlayer ) continue ;																// �÷��̾� ������ ��ȿ���� üũ�Ѵ�.

		if( pPlayer->GetState() == eObjectState_Die ) continue ;								// �÷��̾ �׾��ٸ�, ��󿡼� ���� ��Ų��.

		// 090702 - NYJ ����������� �ִ°�� ��ο��� �ش�.
		if(dwEventRewardItem)
			ITEMMGR->ObtainGeneralItem(pPlayer, dwEventRewardItem, 1, eLog_ItemObtainMonster, MP_ITEM_MONSTER_OBTAIN_NOTIFY);

		if(0 == PARTYMGR->GetParty(pPlayer->GetPartyIdx()))
		{
			ChooseOne(
				*obj,
				dwBigPlayerDamage,
				dwBigPlayerID);
		}
	}

	if( dwBigPlayerDamage == 0 ) bNoPlayer = TRUE ;												// ������ ��Ư �ĺ��� ���� �÷��̾ ����.

	// ��Ƽ �� ������ ȹ�� ��Ƽ�� �̴´�.

	DWORD dwBigPartyDamage = 0 ;																// ���� ū ��Ƽ �������� ���� ����.
	DWORD dwBigPartyID = 0 ;																	// ���� ū ��Ƽ �׹����� �� ��Ÿ ���̵� ���� ����.

	m_DamageObjectTableParty.SetPositionHead() ;												// ������ ���̺��� �ص�� �����Ѵ�.

	while(const DAMAGEOBJ* const obj = m_DamageObjectTableParty.GetData())
	{
		ChooseOne(
			*obj,
			dwBigPartyDamage,
			dwBigPartyID);
	}

	if( dwBigPartyDamage == 0 ) bNoParty = TRUE ;

	if( bNoPlayer && bNoParty ) return;

	if( !bNoPlayer && !bNoParty )
	{
		// ��Ƽ�� ������ �������� ���Ѵ�.
		if( dwBigPlayerDamage == dwBigPartyDamage )													// ������ �������� ��Ƽ�� �������� ���ٸ�,
		{
			int nResult = random(0, 100) ;															// ���� ���� ���Ѵ�.

			if( nResult < 50 )																		// ���� ���� 50�̸��̸�, 
			{
				dwBigPlayerDamage = 0 ;																// ��Ƽ���� �������� ������.
				dwBigPartyDamage  = 1 ;
			}
			else																					// ���� ���� 50�� �ʰ��ϸ�,
			{
				dwBigPlayerDamage = 1 ;																// ���ο��� �������� ������.
				dwBigPartyDamage  = 0 ;
			}
		}

		if( dwBigPlayerDamage > dwBigPartyDamage )													// ������ �������� ��Ƽ�� ������ ���� ũ��,
		{
			if(CPlayer* const pPlayer = (CPlayer*)g_pUserTable->FindUser(dwBigPlayerID))
			{
				SendItemToPerson(pPlayer) ;															// ���� ū �������� �� �÷��̾�� �������� �����Ѵ�.
			}
		}
		else if( dwBigPlayerDamage < dwBigPartyDamage )												// ��Ƽ �������� ������ ������ ���� ũ��,
		{
			if(CParty* const pParty = PARTYMGR->GetParty(dwBigPartyID))
			{
				SaveCandidacy(
					*pParty);

				switch(pParty->GetOption())															// ��Ƽ�� ������ �й� ����� �޾ƿ´�.
				{
				case ePartyOpt_Sequence : DistribuSequence(*pParty); break;
				case ePartyOpt_Damage :	  DistributeDamage(); break;
				}
			}
		}
	}
	else
	{
		if( bNoPlayer )
		{
			if(CParty* const pParty = PARTYMGR->GetParty(dwBigPartyID))
			{
				SaveCandidacy(
					*pParty);

				switch(pParty->GetOption())															// ��Ƽ�� ������ �й� ����� �޾ƿ´�.
				{
				case ePartyOpt_Sequence : DistribuSequence(*pParty); break;
				case ePartyOpt_Damage :	  DistributeDamage(); break;
				}
			}
		}
		else if( bNoParty )
		{
			if(CPlayer* const pPlayer = (CPlayer*)g_pUserTable->FindUser(dwBigPlayerID))
			{
				SendItemToPerson(pPlayer) ;															// ���� ū �������� �� �÷��̾�� �������� �����Ѵ�.
			}
		}
	}	
}

void CDistributer::DistributeToKiller()
{
	if( m_pKiller )																				// ų�� ������ ��ȿ���� üũ�Ѵ�.
	{
		if(0 == PARTYMGR->GetParty(m_pKiller->GetPartyIdx()))
		{
			DistributeToPerson(
				*m_pKiller);
		}
		else																					// ų���� ��Ƽ�� �����ִٸ�,
		{
			DistributeToParty(
				*m_pKiller);
		}
	}
}

void CDistributer::DistributeToPerson(CPlayer& pPlayer)
{
	if(m_pMob)
	{
		if(pPlayer.GetGridID() != m_pMob->GetGridID())
		{
			return;
		}
		else if(pPlayer.GetState() == eObjectState_Die)
		{
			return;
		}

		DWORD dwCur = CalcObtainExp(pPlayer.GetLevel(), 1);
		EXPTYPE dwExp = dwCur + (EXPTYPE)(dwCur * 0.15f);
		int nPenaltyType = GetLevelPenaltyToMonster(m_HighLevel);
		MONEYTYPE money = ITEMDROP_OBJ->DropMoney(
			m_pMob->GetSMonsterList(),
			nPenaltyType);

		g_csFamilyManager.SRV_ProcessHonorPoint(
			&pPlayer,
			DWORD(dwExp));
		SendToPersonalExp(
			&pPlayer,
			dwExp);

		if( pPlayer.GetLevel() < m_pMob->GetLevel() + PETMGR->GetExpPenaltyLevel() )
		{
			if(CPet* const petObject = PETMGR->GetPet(pPlayer.GetPetItemDbIndex()))
			{
				petObject->AddExp();
			}
		}

		SendMoneyToPerson(
			&pPlayer,
			money);
		SendItemToPerson(
			&pPlayer);

		if(DWORD dwEventRewardItem = GAMERESRCMNGR->GetMonsterEventRewardItem(m_pMob->GetMonsterKind()))
		{
			ITEMMGR->ObtainGeneralItem(
				&pPlayer,
				dwEventRewardItem,
				1,
				eLog_ItemObtainMonster,
				MP_ITEM_MONSTER_OBTAIN_NOTIFY);
		}

		// 081027 LUJ, Ư�� ������ ��� ȹ���� ����ġ�� �α׿� �����
		switch( m_pMob->GetObjectKind() )
		{
		case eObjectKind_BossMonster:
		case eObjectKind_FieldBossMonster:
		case eObjectKind_FieldSubMonster:
			{
				InsertLogExp(
					eExpLog_Get,
					pPlayer.GetID(),
					pPlayer.GetLevel(),
					dwExp,
					pPlayer.GetPlayerExpPoint(),
					m_pMob->GetMonsterKind(),
					m_pMob->GetObjectKind(),
					0 );
				break;
			}
		}
	}
}

void CDistributer::DistributeToParty(CPlayer& pPlayer)
{	
	if( !m_pMob ) return ;

	if(CParty* pParty = PARTYMGR->GetParty(pPlayer.GetPartyIdx()))
	{
		SaveCandidacy(
			*pParty);

		if(mPlayerIndexContainer.empty())
		{
			return;
		}
		else if(1 == mPlayerIndexContainer.size())
		{
			DistributeToPerson(
				pPlayer);
			return;
		}
		// ������ ���� �ٸ� �÷��̾ ������
		else
		{
			DWORD dwEventRewardItem = GAMERESRCMNGR->GetMonsterEventRewardItem(m_pMob->GetMonsterKind());
			BOOL bLevelPenalty = FALSE;
			LEVELTYPE highLevel = 0;
			LEVELTYPE totalLevel = 0;
			LEVELTYPE lowLevel = 150;
			LEVELTYPE memberLevel = 0;
			float fTotalLvWeight = 0;

			GetPartyLevels(
				*pParty,
				highLevel,
				totalLevel);

			for(IndexContainer::const_iterator iterator = mPlayerIndexContainer.begin();
				mPlayerIndexContainer.end() != iterator;
				++iterator)
			{
				const DWORD playerIndex = *iterator;

				if(CPlayer* const pMember = (CPlayer*)g_pUserTable->FindUser(playerIndex))
				{
					// 090702 - NYJ ����������� �ִ°�� �������� �� �ĺ��� �ش�.
					if(dwEventRewardItem)
					{
						if(DAMAGEOBJ* const obj = m_DamageObjectTableSolo.GetData(playerIndex))
						{
							ITEMMGR->ObtainGeneralItem(pMember, dwEventRewardItem, 1, eLog_ItemObtainMonster, MP_ITEM_MONSTER_OBTAIN_NOTIFY);
						}
					}

					memberLevel = pMember->GetLevel() ;
					// ��������	= ( ��� - ( �ְ��� - �ڱⷹ�� ) ) / ���
					fTotalLvWeight += abs( (float)( MAX_CHARACTER_LEVEL_NUM - ( highLevel - memberLevel ) ) / (float)MAX_CHARACTER_LEVEL_NUM );
				}
			}

			// 091123 ONS ç������ ����ġ �й� ó�� �߰� : ç�������ϰ�� �г�Ƽ�� ���� �ʴ´�.
			BOOL bIsChallengeZone = FALSE;

			if(g_csDateManager.IsChallengeZoneHere())
			{
				bIsChallengeZone = TRUE;
			}

			if( ((highLevel - lowLevel) >= 21) && !bIsChallengeZone ) bLevelPenalty = TRUE ;

			DWORD partyexp = CalcObtainExp(highLevel, mPlayerIndexContainer.size());
			float applyRate = 0.0f;

			switch(mPlayerIndexContainer.size())
			{
			case 2 : applyRate = 0.50f ;	break ;
			case 3 : applyRate = 0.86f ;	break ;
			case 4 : applyRate = 1.21f ;	break ;
			case 5 : applyRate = 1.58f ;	break ;
			case 6 : applyRate = 1.98f ;	break ;
			case 7 : applyRate = 2.41f ;	break ;
			}

			if( bLevelPenalty )
			{
				applyRate = 0.0f ;
			}

			const int nPenaltyType = GetLevelPenaltyToMonster(m_HighLevel);
			const MONEYTYPE money = ITEMDROP_OBJ->DropMoney(
				m_pMob->GetSMonsterList(),
				nPenaltyType);
			const MONEYTYPE ObtainMoney = money / mPlayerIndexContainer.size();

			for(IndexContainer::const_iterator iterator = mPlayerIndexContainer.begin();
				mPlayerIndexContainer.end() != iterator;
				++iterator)
			{
				if(CPlayer* const pMember = (CPlayer*)g_pUserTable->FindUser(*iterator))
				{
					if(pMember->GetGridID() != m_pMob->GetGridID())
					{
						continue;
					}

					float fDistributedExp = 0.0f;

					if( bIsChallengeZone )
					{
						float fExpRate = ((float)pMember->GetChallengeZoneExpRate()) * 0.01f;
						fDistributedExp = (partyexp * ( 1.f + applyRate )) * fExpRate;
					}
					else
					{
						int nPersonPenaltyType = GetLevelPenaltyToMonster(pMember->GetLevel());
						if(nPersonPenaltyType == 3)
						{
							SendToPersonalExp(pMember,1);
							continue;
						}

						memberLevel = pMember->GetLevel();

						// ��������	= ( ( ��� - ( �ְ��� - �ڱⷹ�� ) ) / ��� ) / �����Ƽ���� �������� ���� )
						float fExpA = ( abs( (float)( MAX_CHARACTER_LEVEL_NUM - ( highLevel - memberLevel ) ) / (float)MAX_CHARACTER_LEVEL_NUM ) ) / fTotalLvWeight;
						float fExpB = (partyexp * ( 1.f + applyRate )) ;				// ����ġ ����ƮB�� ���Ѵ�.
						fDistributedExp = fExpA * fExpB ;
					}

					if(fDistributedExp >= 1.0f)
					{
						DWORD dwExp = (DWORD)fDistributedExp;

						if( ((highLevel - memberLevel) >= 21) && !bIsChallengeZone )
						{
							SendToPersonalExp(pMember,1);
						}
						else
						{
							SendToPersonalExp(pMember,dwExp);
						}


						if(CPet* const pPet = PETMGR->GetPet(pMember->GetPetItemDbIndex()))
						{
							if( pMember->GetLevel() < m_pMob->GetLevel() + PETMGR->GetExpPenaltyLevel() )
							{
								pPet->AddExp() ;
							}
						}

						g_csFamilyManager.SRV_ProcessHonorPoint(
							&pPlayer,
							dwExp);
						SendMoneyToPerson(
							pMember,
							ObtainMoney);
					}
				}
			}

			SaveCandidacy(
				*pParty);

			switch(pParty->GetOption())
			{
			case ePartyOpt_Sequence : DistribuSequence(*pParty); break;
			case ePartyOpt_Damage :	  DistributeDamage(); break;
			}
		}
	}
}

void CDistributer::SendMoneyToPerson(CPlayer* pPlayer, MONEYTYPE ChangeValue)
{
	if( pPlayer && m_pMob )																		// �÷��̾�� ������ ������ ��ȿ���� üũ�Ѵ�.
	{
		if(pPlayer->GetGridID() != m_pMob->GetGridID())
		{
			return;
		}

		MONEYTYPE eventValue = (MONEYTYPE)(gEventRate[eEvent_MoneyRate] * ChangeValue);

		// �Ӵϸ� �÷��̾�� �����Ѵ�.
		if( pPlayer->SetMoney(eventValue, MONEY_ADDITION, MF_OBTAIN, eItemTable_Inventory, eMoneyLog_GetMonster, m_pMob->GetMonsterKind()) != eventValue )
		{
			// error msg ������. ���ѷ� �ʰ�
			MSGBASE msg;
			msg.Category = MP_ITEM;
			msg.Protocol = MP_ITEM_MONEY_ERROR;
			msg.dwObjectID = pPlayer->GetID();
			
			pPlayer->SendMsg(&msg, sizeof(msg));
		}
	}
}

void CDistributer::SendItemToPerson(CPlayer* pPlayer)
{
	if( !pPlayer || !m_pMob ) return ;															// �÷��̾�� ���� ������ ��ȿ���� üũ�Ѵ�.

	if(pPlayer->GetGridID() != m_pMob->GetGridID())
	{
		return;
	}

	int nMaxItemCount = eDROPITEMKIND_MAX-1;
	const BASE_MONSTER_LIST& baseMonsterList = m_pMob->GetSMonsterList();
	BOOL bObtainItem = FALSE;

	if( m_HighLevel <= 0 )																		// Ȥ�ö� �ְ����� ���� ������ �ִٸ�,
	{
		m_HighLevel = pPlayer->GetMaxLevel() ;													// �ְ� ������ �÷��̾��� MAX������ �����Ѵ�.
	}

	int nPenaltyType = GetLevelPenaltyToMonster(m_HighLevel) ;									// ���Ϳ� �������� ���� �г�Ƽ ���� �޴´�.

	// 090818 ShinJS --- GM-Tool Monster Regen Item Drop ������
	if( m_dwDropItemID )
	{
		// ������ Ȯ�� �˻�
		if( m_dwDropItemRatio > DWORD( rand() % 100 ) )
		{
			ITEMMGR->MonsterObtainItem(
				pPlayer,
				m_dwDropItemID,
				m_pMob->GetMonsterKind(),
				1);
		}

		m_dwDropItemID = 0;
		m_dwDropItemRatio = 0;
	}

	if( nPenaltyType == 3 ) return ;															// �г�Ƽ ���� 3, 21���� �̻� ���̳��� ������ ����� ����.

	if( m_pMob->GetObjectKind() == eObjectKind_FieldBossMonster )
	{
		return;
	}

	for( int count = 0 ;  count < nMaxItemCount ; ++count )
	{
		if(ITEMDROP_OBJ->DropItem(
			m_pMob->GetMonsterKind(),
			baseMonsterList.dropItem[count],
			pPlayer,
			nPenaltyType))
		{
			bObtainItem = TRUE;
		}
	}

	if( bObtainItem )																			// ������ �Ҵ��� �̷�� ���ٸ�, 
	{
		CParty* pParty = PARTYMGR->GetParty(pPlayer->GetPartyIdx() ) ;							// ��Ƽ ������ �޴´�.

		if( pParty )																			// ��Ƽ ������ ��ȿ���� üũ�Ѵ�.
		{
			BYTE winnerIdx = pParty->GetWinnerIdx() ;											// ��÷�� ��ȣ�� �޴´�.
			
			if(winnerIdx == MAX_PARTY_LISTNUM-1 )												// ��÷ ��ȣ ������ ���������� üũ�Ѵ�.
			{
				pParty->SetWinnerIdx(0) ;														// ��÷ ��ȣ�� ó������ �����Ѵ�.
			}
			else																				// ������ �������� �ƴϸ�, 
			{
				pParty->SetWinnerIdx(++winnerIdx) ;												// ��÷ ��ȣ�� �����Ѵ�.						
			}
		}
	}
}

DWORD CDistributer::CalcObtainExp(LEVELTYPE KillerLevel, int nMemberCount)
{
	if(NULL == m_pMob)
	{
		return 0;
	}

	DWORD dwExp = m_pMob->GetSMonsterList().ExpPoint;

	if(m_pMob->GetObjectKind() == eObjectKind_BossMonster)
	{
		dwExp = (DWORD)BOSSREWARDSMGR->GetExpOfBossMonster(m_pMob->GetMonsterKind());
	}

	const LEVELTYPE MonsterLevel = m_pMob->GetSMonsterList().Level;
	const LEVELTYPE levelGap = (MonsterLevel > KillerLevel ? MonsterLevel - KillerLevel : KillerLevel - MonsterLevel);																// ���Ϳ� �÷��̾��� ���� ����.

	if( MonsterLevel > KillerLevel + 15 )
	{
		if( m_pMob->GetObjectKind() == eObjectKind_BossMonster )
		{
			//if(MonsterLevel > KillerLevel+16)
			if(MonsterLevel > KillerLevel+20)													// ���� ������ �÷��̾� ���� ���� 21���� �̻��� �� ����ġ 1 ����
				return 1;
			else
				return dwExp ;
		}

		double dwLevel = KillerLevel ;												// ���� ����Ʈ ����.
		// 080116 LYW --- Distributer : HP�� ������ ���͸� ����� ��, ȹ�� ����ġ ���� ó��.
		double dwPenalty = 1.95f ;																// ���Ƽ ����Ʈ ����.
		double result = pow(dwLevel, dwPenalty) ;												// ���� ����Ʈ ^ ���Ƽ ����Ʈ�� ���Ѵ�.

		float fExp = (float)(result + 15) ;														// �÷�Ʈ���� ũ�� ������ ����ġ�� ���Ѵ�.

		if( fExp > dwExp )
		{
			return dwExp ;
		}
		else
		{
			return (DWORD)floor(fExp) ;															// ����ġ�� �ݿø� �Ͽ� DWORD������ ��ȯ.
		}
	}
	else if( MonsterLevel < KillerLevel )														// �÷��̾� ������ ���� �������� ū ���.
	{
		float fPenalty = 1.0f ;																	// ���Ƽ ����Ʈ.
		float fExp     = 0.0f ;																	// �÷�Ʈ�� ����ġ.

		// 080116 LYW --- Distributer : ����Ʈ ��Ī ���� ��쿡�� �г�Ƽ�� ���� �ʴ´�.
		if (!g_csDateManager.IsChallengeZoneHere())
		{
			// 091023 pdy ���� ���� �г�Ƽ ���� , ��Ƽ ���ʽ� ����ġ ��ȹ ����
			if( levelGap >= 8 && levelGap <= 15 )												// �÷��̾� ������ +8~+15�� ��.
			{
				fPenalty = 0.5f ;																// ���Ƽ ����Ʈ 50%�� ����
			}
			else if( levelGap >= 16 && levelGap <= 20 )											// �÷��̾� ������ +15~+20�� ��.
			{
				fPenalty = 0.25f ;																// ���Ƽ ����Ʈ 25%�� ����.
			}
			else if( levelGap >= 21 )															// �÷��̾� ������ +21 �̻��� ��.
			{
				return 1 ;																		// ����ġ�� ������ 1�� ��ȯ�Ѵ�.
			}
		}

		fExp = (float)(dwExp * fPenalty) ;														// ����ġ�� �÷�Ʈ������ ���Ѵ�.

		return (DWORD)floor(fExp) ;																// DWORD������ ���Ƽ ����ġ ��ȯ.
	}	
	else																						// ���Ϳ� �÷��̾� ������ ���� ��.
	{
		return dwExp ;																			// �÷��̾�� ������ ������ ������� ���� ����ġ ��ȯ.			
	}
}

BOOL CDistributer::GetFirstDamange()
{
	m_FirstDamage = 0 ;																			// ù ������ ������ 0���� �����Ѵ�.
	
	CPlayer* pPlayer = (CPlayer *)g_pUserTable->FindUser(m_1stPlayerID) ;						// ù �������� �� �÷��̾� ������ �޴´�.

	if( pPlayer )																				// �÷��̾� ������ ��ȿ���� üũ�Ѵ�.
	{
		DAMAGEOBJ* pDObj = NULL ;																// ������ ������ ���� ������ ������ �����Ѵ�.

		CParty* pParty = PARTYMGR->GetParty(pPlayer->GetPartyIdx()) ;							// �÷��̾��� ��Ƽ������ �޴´�.

		if( pParty )																			// ��Ƽ�� �����ִٸ�,
		{
			pDObj = m_DamageObjectTableParty.GetData(pParty->GetPartyIdx()) ;					// ��Ƽ�� ������ ������ �޴´�.

			if( pDObj )																			// ������ ������ ��ȿ���� üũ�Ѵ�.
			{
				m_FirstDamage = pDObj->dwData ;													// ù �������� �����Ѵ�.

				return TRUE ;																	// ���� TRUE.
			}
		}
		else																					// ��Ƽ�� �������� �ʴٸ�,
		{
			pDObj = m_DamageObjectTableSolo.GetData(pPlayer->GetID()) ;							// �÷��̾��� ������ ������ �޴´�.

			if( pDObj )																			// ������ ������ ��ȿ���� üũ�Ѵ�.
			{
				m_FirstDamage = pDObj->dwData ;													// ù �������� �����Ѵ�.

				return TRUE ;																	// ���� TRUE.
			}
		}
	}

	return FALSE ;																				// ���� FALSE.
}

void CDistributer::GetTotalKillerCount()
{
	m_nTotalKillerCount = 0;

	m_DamageObjectTableSolo.SetPositionHead() ;													// ���� ������ ���̺� üũ.

	while(const DAMAGEOBJ* const obj = m_DamageObjectTableSolo.GetData())
	{
		if(obj->dwData > 0)																	// �������� �ִ��� ���θ� üũ�Ѵ�.
		{
			if(CPlayer* const pPlayer = (CPlayer *)g_pUserTable->FindUser(obj->dwID))
			{
				if(0 == PARTYMGR->GetParty(pPlayer->GetPartyIdx()))
				{
					++m_nTotalKillerCount ;													// �ַ� �÷��̾� ���� �ø���.
				}
			}
			// 091111 pdy ��Ƽ ������ ���� ����� ���� ����ÿ� �ѹ� ��� �ϵ��� ����
			else
			{
				++m_nTotalKillerCount ;														// ������ ���� �÷��̾� ��Ƽ����� �ַ� �÷��̾�� �����ϰ� ���� �ø���.
			}
		}
	}

	m_DamageObjectTableParty.SetPositionHead() ;												// ��Ƽ ���̺� üũ.

	while(const DAMAGEOBJ* const obj = m_DamageObjectTableParty.GetData())
	{
		if(obj->dwData > 0)																	// �������� �ִ��� ���θ� üũ�Ѵ�.
		{
			if(PARTYMGR->GetParty(obj->dwID))
			{
				++m_nTotalKillerCount ;														// �ַ� �÷��̾� ���� �ø���.
			}
		}
	}
}

void CDistributer::GetHighLevelOfKillers()
{
	m_nTotalKillerCount = 0;

	m_DamageObjectTableSolo.SetPositionHead() ;													// ���� ������ ���̺� üũ.

	while(const DAMAGEOBJ* const obj = m_DamageObjectTableSolo.GetData())
	{
		if(obj->dwData > 0)																	// �������� �ִ��� ���θ� üũ�Ѵ�.
		{
			if(CPlayer* const pPlayer = (CPlayer *)g_pUserTable->FindUser(obj->dwID))
			{
				if(0 == PARTYMGR->GetParty(pPlayer->GetPartyIdx()))
				{
					//090402 pdy ���� ������� ����üũ ���� 
					//LEVELTYPE playerLevel = pPlayer->GetLevel() ;							// �÷��̾��� ������ �޴´�.
					LEVELTYPE playerLevel = pPlayer->GetMaxLevel() ;

					if( playerLevel > m_HighLevel )											// �÷��̾��� ������ �ְ� �������� ũ�ٸ�,
					{
						m_HighLevel = playerLevel ;											// �ְ� ������ �÷��̾� ������ �����Ѵ�.
					}
				}
			}
		}
	}

	m_DamageObjectTableParty.SetPositionHead() ;												// ��Ƽ ������ ���̺� üũ.

	while(DAMAGEOBJ* const obj = m_DamageObjectTableParty.GetData())
	{
		if(obj->dwData > 0)																	// �������� �ִ��� ���θ� üũ�Ѵ�.
		{
			if(CParty* const pParty = PARTYMGR->GetParty(obj->dwID))
			{
				LEVELTYPE highLevel = 0 ;													// 
				LEVELTYPE totalLevel = 0 ;

				GetPartyLevels(
					*pParty,
					highLevel,
					totalLevel,
					eGET_MAXLEVEL);

				if( highLevel > m_HighLevel )												// �÷��̾��� ������ �ְ� �������� ũ�ٸ�,
				{
					m_HighLevel = highLevel ;												// �ְ� ������ �÷��̾� ������ �����Ѵ�.
				}
			}
		}
	}
}

int CDistributer::GetLevelPenaltyToMonster(LEVELTYPE level) 
{

	if (g_csDateManager.IsChallengeZoneHere())
	{
		return 0;
	}

	LEVELTYPE MonsterLevel = m_pMob->GetSMonsterList().Level;
	LEVELTYPE levelGap = 0;
	
	if( MonsterLevel <= level )																	// ���� ������ �÷��̾� ���� ū ���.
	{
		levelGap = level - MonsterLevel ;														// ���� ���̸� ���Ѵ�.
	}

	// 091023 pdy ���� ���� �г�Ƽ ���� , ��Ƽ ���ʽ� ����ġ ��ȹ ����
	if( levelGap > 7 && levelGap < 16 )															// �������̰� 8~15�̸�, 
	{
		return 1;
	}
	else if( levelGap > 15 && levelGap < 21 )													// �������̰� 16~20�̸�, 
	{
		return 2;
	}
	else if( levelGap > 20 )																	// ���� ���̰� 20�̻��̸�,
	{
		return 3;
	}

	return 0;
}

void CDistributer::GetPartyLevels(CParty& pParty, LEVELTYPE& pHighLevel, LEVELTYPE& pTotalLevel , eGET_LVTYPE eGetLvType )
{
	LEVELTYPE totallevel = 0;
	LEVELTYPE highlevel = 0;
	LEVELTYPE curlevel = 0;
	
	for(int count = 0 ; count < MAX_PARTY_LISTNUM ; ++count)									// �α��� ���� ��Ƽ�� ���� �� �� �ְ� ������ ���Ѵ�.
	{
		if(CPlayer* const pMember = (CPlayer*)g_pUserTable->FindUser(pParty.GetMemberID(count)))																			// ��� ������ ��ȿ���� üũ.
		{
			if(Chk(*pMember, m_pMob->GetGridID()))
			{

				if( eGetLvType == eGET_MAXLEVEL )
					curlevel = pMember->GetMaxLevel();
				else
					curlevel = pMember->GetLevel();

				if(highlevel < curlevel)
				{
					highlevel = curlevel;
				}

				totallevel = LEVELTYPE( totallevel + curlevel);
			}
		}
	}

	pHighLevel = highlevel;
	pTotalLevel = totallevel;
}

// 091111 pdy ��Ƽ ������ ���� ����� ���� ����ÿ� �ѹ� ��� �ϵ��� ����
void CDistributer::GetAllPartyDamage()
{
	m_DamageObjectTableSolo.SetPositionHead();

	while(const DAMAGEOBJ* const obj = m_DamageObjectTableSolo.GetData())
	{
		if(obj->dwData > 0)																	// �������� �ִ��� ���θ� üũ�Ѵ�.
		{
			if(CPlayer* const pPlayer = (CPlayer *)g_pUserTable->FindUser(obj->dwID))
			{
				if(pPlayer->GetGridID() != m_pMob->GetGridID())
				{
					continue;
				}
				else if(PARTYMGR->GetParty(pPlayer->GetPartyIdx()))
				{
					DoAddDamageObj(
						m_DamageObjectTableParty,
						pPlayer->GetPartyIdx(),
						obj->dwData);
				}
			}
		}
	}
}

void CDistributer::SaveCandidacy(CParty& pParty)
{
	if(NULL == m_pMob)
	{
		return;
	}

	mPlayerIndexContainer.clear();

	for(int count = 0 ; count < MAX_PARTY_LISTNUM ; ++count)									// �ִ� ��Ƽ ��� �� ��ŭ ����.
	{
		if(CPlayer* const pMember = (CPlayer*)g_pUserTable->FindUser(pParty.GetMemberID(count)))
		{
			// 071215 LYW --- Distributer : ��Ƽ ����� ���� ���¿�����, �������� �߰����� �ʴ´�.
			if( pMember->GetState() == eObjectState_Die ) continue ;

			if(Chk(*pMember, m_pMob->GetGridID()))
			{
				mPlayerIndexContainer.push_back(
					pMember->GetID());
			}
		}
	}
}

void CDistributer::DistribuSequence(CParty& pParty)
{
	if(mPlayerIndexContainer.empty())
	{
		return;
	}

	const size_t nWinnerIdx = size_t(pParty.GetWinnerIdx());
	DWORD playerIndex = 0;

	if( nWinnerIdx >= mPlayerIndexContainer.size())
	{
		pParty.SetWinnerIdx(0);
		playerIndex = *mPlayerIndexContainer.begin();
	}
	else if(nWinnerIdx < mPlayerIndexContainer.size())
	{
		IndexContainer::const_iterator iterator = mPlayerIndexContainer.begin();
		std::advance(
			iterator,
			nWinnerIdx);

		playerIndex = *iterator;
	}

	CPlayer* pMember = (CPlayer*)g_pUserTable->FindUser(
		playerIndex);

	if( pMember )																	// ��� ������ ��ȿ���� üũ�Ѵ�.
	{
		if(pMember->GetGridID() != m_pMob->GetGridID())
		{
			return;
		}

		SendItemToPerson( pMember ) ;												// �������� ������� �����Ѵ�.
	}
}

void CDistributer::DistributeDamage()
{
	DWORD bigDamage = 0 ;																		// ���� ū ������.
	DWORD bigID = 0 ;																			// ���� ū �������� �� ���̵�.

	for(IndexContainer::const_iterator iterator = mPlayerIndexContainer.begin();
		mPlayerIndexContainer.end() != iterator;
		++iterator)
	{
		DAMAGEOBJ* obj = m_DamageObjectTableSolo.GetData(*iterator) ;								// ������ ���̺��� ���̵�� ������ ������ �޴´�.

		if( obj )																		// ������ ������ ��ȿ���� üũ�Ѵ�.
		{
			if(bigDamage < obj->dwData)													// �ִ� �������� ����� ������ ��.
			{
				bigDamage = obj->dwData ;												// �ִ� �������� ����� �������� ����.
				bigID = obj->dwID ;														// �ִ� �������� �� ����� ���̵� ����.				
			}
		}
	}

	if( bigID == 0 ) return ;																	// ���� ū �������� ��� ���̵� üũ�Ѵ�.

	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(bigID);									// ��� ������ �޴´�.

	if( pPlayer )																				// ��� ������ ��ȿ���� üũ.
	{
		if(pPlayer->GetGridID() != m_pMob->GetGridID())
		{
			return;
		}

		SendItemToPerson( pPlayer ) ;															// �������� �����Ѵ�.							
	}
}

void CDistributer::SendToPersonalExp(CPlayer* pReceivePlayer, EXPTYPE Exp)
{
	Exp = (DWORD)(Exp*gEventRate[eEvent_ExpRate]);
	//// 071221 KTH -- ����ġ ��� ���� ����
	Exp += EXPTYPE(Exp * (pReceivePlayer->GetRateBuffStatus()->GetExp/100));

	LEVELTYPE PlayerLv = pReceivePlayer->GetLevel();
	DWORD LimitExp = 0;

	if( m_pMob->GetObjectKind() == eObjectKind_BossMonster )
	{
		LEVELTYPE MonsterLv = m_pMob->GetLevel();

		if( abs( int(MonsterLv) - int(PlayerLv) ) >= 21 )	// ���������� ��� 21����(high/low ���) �̻� ���̳��� äũ�Ѵ�
		{
			double dwLevel = PlayerLv ;																								
			double result = pow(dwLevel, 1.95) ;												
			float fExp = (float)( (result + 15) * 20) ;														
			LimitExp = (DWORD)floor(fExp) ;			

//			if( LimitExp < Exp )							// ���� �޴� exp�� ���Ѽ� ���� ���ٸ� ���Ѽ� ����ġ�� ����
//			{
//				Exp = LimitExp ;
//			}
		}
	}
	else 
	{
		if(g_csDateManager.IsChallengeZoneHere())
		{
			double dwLevel = PlayerLv ;						// ����Ʈ���ϰ�� ���Ѽ� = ( (����^1.95) + 15 ) * 10																			
			double result = pow(dwLevel, 1.95) ;												
			float fExp = (float)( (result + 15) * 5) ;														
			LimitExp = (DWORD)floor(fExp) ;			
		}
		else
		{
			double dwLevel = PlayerLv ;						// �Ϲ��� ��� ���Ѽ� = ( (����^1.95) + 15 ) * 20																										
			double result = pow(dwLevel, 1.95) ;												
			float fExp = (float)( (result + 15) * 20) ;														
			LimitExp = (DWORD)floor(fExp) ;					
		}

//		if( LimitExp < Exp )							// ���� �޴� exp�� ���Ѽ� ���� ���ٸ� ���Ѽ� ����ġ�� ����
//		{
//			Exp = LimitExp ;
//		}
	}

	pReceivePlayer->AddPlayerExpPoint(Exp);
}

void CDistributer::AddMoney(CPlayer* pPlayer, MONEYTYPE Money, WORD MonsterKind)
{
	if( pPlayer->SetMoney(Money, MONEY_ADDITION, MF_OBTAIN, eItemTable_Inventory, eMoneyLog_GetMonster, MonsterKind) != Money )
	{
		MSGBASE msg;
		msg.Category = MP_ITEM;
		msg.Protocol = MP_ITEM_MONEY_ERROR;

		pPlayer->SendMsg(&msg, sizeof(msg));
	}
}