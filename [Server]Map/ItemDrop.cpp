// ItemDrop.cpp: implementation of the CItemDrop class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ItemDrop.h"
#include "Player.h"
#include "time.h"
#include "ItemManager.h"
#include "Monster.h"
#include "..\[CC]Header\GameResourceManager.h"
#include "MHFile.h"
#include "PartyManager.h"
#include "Party.h"
#include "UserTable.h"

// 080212 LYW --- ItemDrop : Include Boss rewards manager.
#include "BossRewardsManager.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CItemDrop::CItemDrop()
{}

CItemDrop::~CItemDrop()
{}

MONEYTYPE CItemDrop::DropMoney(const BASE_MONSTER_LIST& baseMonsterList, int nPenaltyType)
{
	MONEYTYPE ResultMoney	= 0;											// ȹ�� �Ӵ�.
	MONEYTYPE MaxMoney		= 0 ; 

	if( baseMonsterList.ObjectKind == eObjectKind_BossMonster )				// ���� �����̸�, 
	{
		WORD wMonsterKind = 0 ;
		wMonsterKind = baseMonsterList.MonsterKind ;

		MaxMoney = BOSSREWARDSMGR->GetMoneyOfBossMonster(wMonsterKind) ;	// ���� ���� �Ӵϸ� �޴´�.
	}
	else																	// �Ϲ� �����̸�,
	{
		MaxMoney = baseMonsterList.dropMoney ;								// ���Ͱ� ����� �� �ִ� �ִ� �Ӵϸ� �޴´�.
	}

	DWORD dwDropRateValue = baseMonsterList.dwMoneyDropRate ;				// ������ �Ӵ� ������� �޴´�.

	WORD wDropRate1 = WORD( random(0, 10000) ) ;									// �̱� Ȯ��1�� ���Ѵ�.
	WORD wDropRate2 = WORD( random(0, 10000) ) ;									// �̱� Ȯ��2�� ���Ѵ�.

	DWORD dwDropRate = (wDropRate1 * 10000) + wDropRate2 ;					// ����� �̱� Ȯ���� ���Ѵ�.

	float pickupPercent = random(50.0f, 100.0f) ;							// ȹ���� �� �ִ� Ȯ���� ���Ѵ�.
	float fObtainMoney = 0.0f ;												// ȹ���� �Ӵϸ� �ӽ÷� ���� ����.

	if( dwDropRateValue >= dwDropRate)										// ���� ��� Ȯ���� �̱� Ȯ�� ��.
	{
		fObtainMoney = MaxMoney * (pickupPercent/100) ;						// ȹ�� �Ӵϸ� �����Ѵ�.
	}

	ResultMoney = (DWORD)fObtainMoney ;

	// �г�Ƽ 3�����ؼ��� ó���ϵ��� �Ѵ�.
	if( nPenaltyType == 3 )
	{
		ResultMoney = 1 ;
	}

	return ResultMoney;
}

BOOL CItemDrop::DropItem(WORD MonsterKind, const MONSTER_DROPITEM& dropItem, CPlayer* pPlayer, int nPenaltyType)
{
	if(0 == pPlayer)
	{
		return FALSE;
	}

	if( dropItem.wItemIdx == 0 ) return FALSE ;								// ��� �������� �ε����� üũ�Ѵ�.

	if( gEventRate[eEvent_ItemRate] < 0 )										// Ȥ�ö� ������ ������, 
	{
		gEventRate[eEvent_ItemRate] = 1 ;										// �Ʒ� Ȯ���� ū�ϳ���.
	}

	DWORD dwDropRateValue = 0 ;
	dwDropRateValue = (DWORD)(gEventRate[eEvent_ItemRate] * dropItem.dwDropPercent) ;	// ����� ����.

	// 071204 -- KTH ����� ��� Buff ����
	const float fIncreaseDropRate = (pPlayer->GetRateBuffStatus()->IncreaseDropRate + pPlayer->GetRatePassiveStatus()->IncreaseDropRate) / 100.f;
	dwDropRateValue += DWORD( dwDropRateValue * fIncreaseDropRate );

	// stdlib�� rand �Լ��� �ִ� ���� 32767���� ������ ������, random �Լ��� ����� �Ѵ�.

	DWORD dwDropRate1 = random(0, 99) ;										// ���ڸ� �̱� Ȯ���� ���Ѵ�.
	DWORD dwDropRate2 = random(0, 99) ;										// ���ڸ� �̱� Ȯ���� ���Ѵ�.
	DWORD dwDropRate3 = random(0, 99) ;										// ���ڸ� �̱� Ȯ���� ���Ѵ�.
	DWORD dwDropRate4 = random(0, 99) ;										// ���ڸ� �̱� Ȯ���� ���Ѵ�.

	DWORD dwDropRate = (dwDropRate1 * 1000000) + (dwDropRate2 * 10000) + (dwDropRate3 * 100) + dwDropRate4 ;					// ����� �̱� Ȯ���� ���Ѵ�.

	if( dwDropRateValue > dwDropRate)											// ��� Ȯ���� �̱� Ȯ���� ���Ѵ�.
	{
		ITEM_INFO * pItemInfo	= ITEMMGR->GetItemInfo( dropItem.wItemIdx );
		if(pItemInfo)
		{
			// ��޾������� �����й�~
			CParty* pParty = PARTYMGR->GetParty(pPlayer->GetPartyIdx());
			if( pParty && pItemInfo->kind > pParty->GetDiceGradeOpt() )
			{
				DWORD dwItemGetPlayerID = PARTYMGR->GetRandomMember(pPlayer->GetPartyIdx());
				if(dwItemGetPlayerID)
				{
					CPlayer* pItemGetPlayer = (CPlayer*)g_pUserTable->FindUser(dwItemGetPlayerID);
					if(pItemGetPlayer)
					{
						ITEMMGR->MonsterObtainItem(pItemGetPlayer, dropItem.wItemIdx, MonsterKind, (WORD)dropItem.byCount);
						PARTYMGR->SendObtainItemByDiceMsg(pItemGetPlayer, dropItem.wItemIdx, dropItem.byCount);
					}
				}
				return TRUE;
			}
		}

		ITEMMGR->MonsterObtainItem(pPlayer, dropItem.wItemIdx, MonsterKind, (WORD)dropItem.byCount);

		return TRUE ;
	}

	return FALSE ;
}