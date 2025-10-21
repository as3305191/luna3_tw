// QuestExecute_Item.cpp: implementation of the CQuestExecute_Item class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"																// ǥ�� �ý��۰�, ������Ʈ�� ������, ���־��� �ش����� ���� �ش������� �ҷ��´�.
#include "QuestExecute_Item.h"													// ����Ʈ ���� ������ ����� �ҷ��´�.

#include "QuestScriptLoader.h"													// ����Ʈ ��ũ��Ʈ �δ� ����� �ҷ��´�.

#ifdef _MAPSERVER_																// �� ������ ����Ǿ� �ִٸ�,

#include "Player.h"																// �÷��̾� ����� �ҷ��´�.

#include "QuestGroup.h"															// ����Ʈ �׷� ����� �ҷ��´�.

#include "ItemManager.h"														// ������ �Ŵ��� ����� �ҷ��´�.

#include "Quest.h"																// ����Ʈ ����� �ҷ��´�.

#include "SubQuestInfo.h"
#include "QuestTrigger.h"
#include "QuestManager.h"

#include "..\[CC]Header\GameResourceManager.h"
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
// ������ �Լ�.
CQuestExecute_Item::CQuestExecute_Item( DWORD dwExecuteKind, CStrTokenizer* pTokens, DWORD dwQuestIdx, DWORD dwSubQuestIdx )
: CQuestExecute( dwExecuteKind, pTokens, dwQuestIdx, dwSubQuestIdx )
{
	m_dwItemKind = m_dwItemNum = m_dwItemProbability = m_dwWeaponKind = 0;		// ������ ����, ������ ����, ������ �Ӽ�, ���� Ÿ�� ���� 0���� �����Ѵ�.

	switch( dwExecuteKind )														// ���� ������ Ȯ���Ѵ�.
	{
	case eQuestExecute_GiveItem:												// �������� �ִ� ���� �̰ų�,
	case eQuestExecute_GiveQuestItem:											// ����Ʈ �������� �ִ� �����̶��,
		{
			GetScriptParam( m_dwItemKind, pTokens );							// ������ ������ �޴´�.
			GetScriptParam( m_dwItemNum, pTokens );								// ������ ���� �޴´�.
		}
		break;

	case eQuestExecute_GiveMoney:												// �Ӵϸ� �ִ� �����̰ų�,
	case eQuestExecute_TakeMoney:												// �Ӵϸ� �޴� �����̰ų�,
	case eQuestExecute_TakeExp:													// ����ġ�� �޴� �����̰ų�,
	case eQuestExecute_TakeSExp:												// SP�� �޴� �����̶��,
		{
			GetScriptParam( m_dwItemNum, pTokens );								// ������ ���� �޴´�.
		}
		break;

	case eQuestExecute_TakeQuestItem:											// ����Ʈ �������� �޴� �����̰ų�,
	case eQuestExecute_TakeItem:												// �������� �޴� �����̶��,
		{
			GetScriptParam( m_dwItemKind, pTokens );							// ������ ������ �޴´�.
			GetScriptParam( m_dwItemNum, pTokens );								// ������ ���� �޴´�.
			GetScriptParam( m_dwItemProbability, pTokens );						// ������ �Ӽ��� �޴´�.
		}
		break;

	case eQuestExecute_TakeQuestItemFQW:										// ����Ʈ Ư�� ���⸦ �޴� �����̰ų�,
	case eQuestExecute_TakeQuestItemFW:											// ����Ʈ ���⸦ �޴� �����̸�,
		{
			GetScriptParam( m_dwItemKind, pTokens );							// ������ ������ �޴´�.
			GetScriptParam( m_dwItemNum, pTokens );								// ������ ���� �޴´�.
			GetScriptParam( m_dwItemProbability, pTokens );						// ������ �Ӽ��� �޴´�.
			GetScriptParam( m_dwWeaponKind, pTokens );							// ���� ������ �޴´�.
		}
		break;
		
	case eQuestExecute_TakeMoneyPerCount:										// ī��Ʈ�� �Ӵϸ� �޴� �����̸�,
		{
			GetScriptParam( m_dwItemKind, pTokens );							// ������ ������ �޴´�.
			GetScriptParam( m_dwItemNum, pTokens );								// ������ ���� �޴´�.
		}
		break;
	}
}

// �Ҹ��� �Լ�.
CQuestExecute_Item::~CQuestExecute_Item()										
{
}

// ���� �Լ�.
BOOL CQuestExecute_Item::Execute( PLAYERTYPE* pPlayer, CQuestGroup* pQuestGroup, CQuest* pQuest )
{
#ifdef _MAPSERVER_																// �� ������ ����Ǿ� �ִٸ�,

	switch( m_dwExecuteKind )													// ���� ������ Ȯ���Ѵ�.
	{
	case eQuestExecute_TakeQuestItem:											// ����Ʈ �������� �޴� �����̸�,
		{
			pQuestGroup->TakeQuestItem( pPlayer, m_dwQuestIdx, m_dwSubQuestIdx, // ����Ʈ �׷��� ����Ʈ �������� �޴� �Լ��� ȣ���Ѵ�.
				m_dwItemKind, m_dwItemNum, m_dwItemProbability );
		}
		break;

	case eQuestExecute_GiveQuestItem:											// ����Ʈ �������� �ִ� �����̸�,
		{
			//pQuestGroup->GiveQuestItem( pPlayer, m_dwQuestIdx, m_dwItemKind,	// ����Ʈ �׷��� ����Ʈ �������� �ִ� �Լ��� ȣ���Ѵ�.
			//	m_dwItemNum );

			if( !pQuestGroup->GiveQuestItem( pPlayer, m_dwQuestIdx, m_dwItemKind, m_dwItemNum )) return FALSE ;
		}
		break;

	case eQuestExecute_GiveItem:												// �������� �ִ� �����̸�,
		{
			//if( !pQuestGroup->GiveItem( pPlayer, m_dwItemKind, m_dwItemNum ) )	// ������ �ݳ��� �����ϸ�,
			//{
			//	return FALSE ;													// false�� �����Ѵ�.
			//}

			if( !pQuestGroup->GiveItem( pPlayer, m_dwItemKind, m_dwItemNum, pQuest->GetQuestIdx()) )
			{
				return FALSE ;
			}
		}
		break;

	case eQuestExecute_GiveMoney:												// �Ӵϸ� �ִ� �����̸�,
		{
			pQuestGroup->GiveMoney( pPlayer, m_dwItemNum );						// ����Ʈ �׷��� �Ӵϸ� �ִ� �Լ��� ȣ���Ѵ�.
		}
		break;

	case eQuestExecute_TakeItem:												// �������� �޴� �����̸�,
		{
			return pQuestGroup->TakeItem( pPlayer, m_dwQuestIdx, m_dwSubQuestIdx, m_dwItemKind,			// ����Ʈ �׷��� �������� �޴� �Լ��� ȣ���Ͽ� ����� �����Ѵ�.
				m_dwItemNum, m_dwItemProbability );
		}
		break;

	case eQuestExecute_TakeMoney:												// �Ӵϸ� �޴� �����̸�,
		{
			pQuestGroup->TakeMoney( pPlayer, m_dwItemNum );						// ����Ʈ �׷��� �Ӵϸ� �޴� �Լ��� ȣ���Ѵ�.
		}
		break;

	case eQuestExecute_TakeExp:													// ����ġ�� �޴� �����̸�,
		{
			pQuestGroup->TakeExp( pPlayer, m_dwItemNum );						// ����Ʈ �׷��� ����ġ�� �޴� �Լ��� ȣ���Ѵ�.
		}
		break;

	case eQuestExecute_TakeSExp:												// SP�� �޴� �����̸�,
		{
			pQuestGroup->TakeSExp( pPlayer, m_dwItemNum );						// ����Ʈ �׷��� SP�� �޴� �Լ��� ȣ���Ѵ�.
		}
		break;

	case eQuestExecute_TakeQuestItemFQW:										// ����Ʈ Ư�� ���⸦ �޴� �����̸�,
		{
			pQuestGroup->TakeQuestItemFromQWeapon( pPlayer, m_dwQuestIdx,		// Ư�� ����� ���� ����Ʈ �������� �޴� �Լ��� ȣ���Ѵ�.
				m_dwSubQuestIdx, m_dwItemKind, m_dwItemNum, m_dwItemProbability, 
				m_dwWeaponKind );
		}
		break;
	case eQuestExecute_TakeMoneyPerCount:										// ī��Ʈ�� �Ӵϸ� �޴� �����̸�,
		{
			pQuestGroup->TakeMoneyPerCount( pPlayer, m_dwQuestIdx, m_dwSubQuestIdx, m_dwItemKind, m_dwItemNum );	// ī��Ʈ �� �Ӵϸ� �޴� �Լ��� ȣ���Ѵ�.
		}
		break;
	}
#endif
	return TRUE;
}





int CQuestExecute_Item::CheckCondition( PLAYERTYPE* pPlayer, 
							CQuestGroup* pQuestGroup, CQuest* pQuest )			// ����Ʈ �̺�Ʈ�� �����ϱ� ���� ������ �����ϴ��� üũ�ϴ� �Լ�.
{
	int nErrorCode = e_EXC_ERROR_NO_ERROR ;										// ���� �ڵ带 ���� ������ �����ϰ� e_EXE_ERROR_NO_ERROR�� �����Ѵ�.

	if( !pPlayer )																// �÷��̾� ������ ��ȿ���� ������, 
	{
		nErrorCode = e_EXC_ERROR_NO_PLAYERINFO ;								// �÷��̾� ������ ���ٴ� ���� �ڵ带 �����Ѵ�.
		return nErrorCode ;														// ���� �ڵ带 return ó���� �Ѵ�.
	}

	if( !pQuestGroup )															// ����Ʈ �׷� ������ ��ȿ���� ������,
	{
		nErrorCode = e_EXC_ERROR_NO_QUESTGROUP ;								// ����Ʈ �׷� ������ ���ٴ� ���� �ڵ带 �����Ѵ�.
		return nErrorCode ;														// ���� �ڵ带 return ó���� �Ѵ�.
	}

	if( !pQuest )																// ����Ʈ ������ ��ȿ���� �ʴٸ�,
	{
		nErrorCode = e_EXC_ERROR_NO_QUEST ;										// ����Ʈ ������ ���ٴ� ���� �ڵ带 �����Ѵ�.
		return nErrorCode ;														// ���� �ڵ带 return ó���� �Ѵ�.
	}

#ifdef _MAPSERVER_																// �� ������ ����Ǿ� �ִٸ�,

	switch(m_dwExecuteKind)														// ���� ������ Ȯ���Ѵ�.
	{
	case eQuestExecute_TakeQuestItem :											// ����Ʈ �������� ��� ���๮�̸�,
		{
			return nErrorCode ;													// ���� �ڵ带 return ó���� �Ѵ�.

			// ���� �÷��̾��� ����Ʈ ������ ������ ������ �޾ƿ��� �κ��� 
			// �����Ƿ�, �켱�� ���� �������� ó���� ����.

			//CItemSlot* pSlot = NULL ;											// ������ ���� ������ ���� �����͸� �����ϰ� null ó���� �Ѵ�.
			//pSlot = pPlayer->GetSlot(eItemTable_Inventory) ;					// �÷��̾��� �κ��丮 ���� ������ �޴´�.

			//if( !pSlot )														// �κ��丮 ������ ��ȿ���� ������,
			//{
			//	nErrorCode = e_EXC_ERROR_NO_INVENTORYINFO ;						// �κ��丮 ������ ���ٴ� �����ڵ带 �����Ѵ�.
			//	return nErrorCode ;												// ���� �ڵ带 return ó���� �Ѵ�.
			//}

			//WORD EmptyCellPos[255];											// �� ���� ��ġ�� ���� ������ �����Ѵ�.
			//WORD EmptyCellNum;												// �� ���� ������ ���� ������ �����Ѵ�.

			//WORD wResult = 0 ;												// �� ������ üũ ��� ���� ���� ������ �����ϰ� 0���� �����Ѵ�.

			//wResult = ITEMMGR->GetCanBuyNumInSpace(pPlayer, pSlot,			// �������� ���� ������ ������� ��� ���� �޴´�.
			//	m_dwItemKind, m_dwItemNum, EmptyCellPos, EmptyCellNum) ;

			//if( wResult == 0 )												// ��� ���� 0�� ������,
			//{
			//	nErrorCode = e_EXC_ERROR_NO_EXTRASLOT ;							// ���� �ڵ带 �κ��丮 ������ ���ٴ� ���� �ڵ带 �����Ѵ�.
			//	return nErrorCode ;												// ���� �ڵ带 return ó���� �Ѵ�.
			//}
		}
		break ;

	case eQuestExecute_GiveQuestItem:											// ����Ʈ �������� �ݳ� �����̸�,
		{
			QUESTITEM* pQuestItem = NULL ;										// ����Ʈ ������ ������ ���� �����͸� �����ϰ� null ó���� �Ѵ�.
			pQuestItem = pQuestGroup->GetQuestItem(m_dwItemKind) ;				// ����Ʈ ������ �ε����� ����Ʈ ������ ������ �޴´�.

			if( !pQuestItem )													// ����Ʈ ������ ������ ��ȿ���� �ʴٸ�,
			{
				nErrorCode = e_EXC_ERROR_NO_QUESTITEM ;							// ����Ʈ �������� ���ٴ� ���� �ڵ带 �����Ѵ�.
				return nErrorCode ;												// ���� �ڵ带 return ó���� �Ѵ�.
			}

			if( pQuestItem->dwItemNum < m_dwItemNum )							// �������� ������, ��ǥ �������� ������,
			{
				nErrorCode = e_EXC_ERROR_NOT_ENOUGH_QUESTITEM ;					// ����Ʈ �������� �����ϴٴ� ���� �ڵ带 �����Ѵ�.
				return nErrorCode ;												// ���� �ڵ带 return ó���� �Ѵ�.
			}
		}
		break;

	case eQuestExecute_GiveItem:												// �������� �ݳ��ϴ� �����̸�,
		{
			int count = 0;
			DWORD nGiveItemCount = 0;

			ITEM_TOTALINFO ItemInfo;
			ZeroMemory(
				&ItemInfo,
				sizeof(ItemInfo));
			pPlayer->GetItemtotalInfo(
				ItemInfo,
				GETITEM_FLAG_INVENTORY | GETITEM_FLAG_WEAR );

			int nTotalInvenCount = (int)(SLOT_INVENTORY_NUM + pPlayer->GetInventoryExpansionSize());
			BOOL bStackItem = ITEMMGR->IsDupItem( m_dwItemKind ) ;

			for( count = 0 ; count < nTotalInvenCount ; ++count )				
			{
				if( ItemInfo.Inventory[count].wIconIdx == m_dwItemKind )		// ���ڷ� �Ѿ�� ������ �ε����� ���� �������� �ִٸ�,
				{
					if( bStackItem )											// ���þ������̸�, 
					{
						nGiveItemCount += ItemInfo.Inventory[count].Durability ;// �ݳ� ������ ������ ���� �߰��Ѵ�.
					}
					else														// �Ϲݾ������̸�, 
					{
						++nGiveItemCount ;										// �ݳ� ������ ���� �����Ѵ�.
					}
					
				}
			}

			for( count = 0 ; count < SLOT_WEAR_NUM ; ++count )					// ��� â�� ���� �� ��ŭ for���� ������.
			{
				if( ItemInfo.WearedItem[count].wIconIdx == m_dwItemKind )		// ���ڷ� �Ѿ�� ������ �ε����� ���� �������� �ִٸ�,
				{
					if( bStackItem )											// ���þ������̸�, 
					{
						nGiveItemCount += ItemInfo.Inventory[count].Durability ;// �ݳ� ������ ������ ���� �߰��Ѵ�.
					}
					else														// �Ϲݾ������̸�, 
					{
						++nGiveItemCount ;										// �ݳ� ������ ���� �����Ѵ�.
					}
				}
			}

			if( nGiveItemCount == 0 )											// �ݳ� ������ ������ ���� 0����,
			{
				nErrorCode = e_EXC_ERROR_NO_GIVEITEM ;							// �ݳ��� �������� ���ٴ� ������ �����Ѵ�.
				return nErrorCode ;												// ���� �ڵ带 return ó���� �Ѵ�.
			}

			if( nGiveItemCount < m_dwItemNum )
			{
				nErrorCode = e_EXC_ERROR_NOT_ENOUGH_GIVEITEM ;					// �ݳ��� �������� �����ϴٴ� ������ �����Ѵ�.
				return nErrorCode ;												// ���� �ڵ带 return ó���� �Ѵ�.
			}
		}
		break;

	case eQuestExecute_GiveMoney:												// �Ӵϸ� �ݳ��ϴ� �����̸�,
		{
			MONEYTYPE curMoney = pPlayer->GetMoney() ;							// �÷��̾��� ���� �Ӵϸ� �޴´�.

			if( curMoney < m_dwItemNum )										// ���� �÷��̾��� �Ӵϰ� �ݳ� �� �ӴϺ��� �����ϸ�,
			{
				nErrorCode = e_EXC_ERROR_NOT_ENOUGH_GIVEMONEY ;					// �Ӵϰ� �����ϴٴ� ������ �����Ѵ�.
				return nErrorCode ;												// ���� �ڵ带 return ó���� �Ѵ�.
			}
		}
		break;

	case eQuestExecute_TakeItem :
		{
			// 080515 LYW --- QuestExecute_Item : ������ ȹ�� ���� ����.
			// ���� üũ �����, Ʈ���ſ� �����ִ� ���� ���� �ϳ��� ���� ���� ������ üũ �߱� ������, 
			// Ʈ���� ������, �������� �������� �ٶ�, (A, B, C, D)�������� �ϳ��� �����Ѵٰ� �ϸ�, 
			// �󽽷� �ϳ��� �������, ���� ���� üũ�� �����Ǿ���.
			// ������ Ʈ���ſ� ���� ���� ������ ��� üũ�ϵ��� ����.
			// ���� �ڵ� ����.
			nErrorCode = e_EXC_ERROR_NO_EXTRASLOT ;



			// ����Ʈ�� �޴´�.
			CQuest* pQuest = NULL ;
			pQuest = pQuestGroup->GetQuest(m_dwQuestIdx) ;

			if(!pQuest) return nErrorCode ;



			// ���� ����Ʈ ������ �޴´�.
			CSubQuestInfo* pSubQuestInfo = NULL ;
			pSubQuestInfo = pQuest->GetSubQuestInfo(m_dwSubQuestIdx) ;

			if(!pSubQuestInfo) return nErrorCode ;

			cPtrList& pTriggerList = pSubQuestInfo->GetTriggerList() ;

			// ����Ʈ Ʈ���� ������ �޴´�.
			CQuestTrigger* pTrigger ;
			cPtrList* pExeList = NULL ;

			PTRLISTPOS triPos = pTriggerList.GetHeadPosition() ;
			while(triPos)
			{
				pTrigger = (CQuestTrigger*)pTriggerList.GetNext(triPos) ;

				if(!pTrigger) continue ;

				pExeList = pTrigger->GetExeList() ;
				break ;
			}

			if(!pExeList) return nErrorCode ;



			// ������ ���� ������ �޴´�.
			CItemSlot* pSlot = NULL ;
			pSlot = pPlayer->GetSlot(eItemTable_Inventory) ;

			if( !pSlot )
			{
				nErrorCode = e_EXC_ERROR_NO_INVENTORYINFO ;
				return nErrorCode ;
			}



			// ����ִ� ������ cell ī��Ʈ�� �޴´�.
			POSTYPE startPos = pSlot->GetStartPos() ;
			POSTYPE EndPos = startPos + pSlot->GetSlotNum() ;

			BYTE byEmptyCellCount = 0 ;
			for( WORD count = startPos ; count < EndPos ; ++count )
			{
				if(!pSlot->IsEmpty(count)) continue ;

				++byEmptyCellCount ;
			}



			// ����Ʈ ���� ������ �޴´�.
			CQuestExecute* pExecute ;
			PTRLISTPOS exePos = pExeList->GetHeadPosition() ;

			BYTE byDifferrentItemCount = 0 ;
			while(exePos)
			{
				pExecute = NULL ;
				pExecute = (CQuestExecute*)pExeList->GetNext(exePos) ;

				if(!pExecute) continue ;

				if(pExecute->GetQuestExecuteKind() != eQuestExecute_TakeItem) continue ;

				// ������ ������ �޴´�.
				DWORD dwItemIdx		= ((CQuestExecute_Item*)pExecute)->GetItemKind() ;
				DURTYPE dwItemNum	= ((CQuestExecute_Item*)pExecute)->GetItemNum() ;

				ITEM_INFO* ItemInfo = NULL ;
				ItemInfo = ITEMMGR->GetItemInfo( dwItemIdx ) ;

				if(!ItemInfo)
				{
					// ������ ������ ��ȿ���� ������, �α� �����.
					char tempMsg[128] = {0,} ;
					sprintf(tempMsg, "------ %s ----- TakeItem Failed!!", __FUNCTION__) ;
					QUESTMGR->WriteLog(tempMsg) ;

					memset(tempMsg, 0, strlen(tempMsg)) ;
					sprintf(tempMsg, "QuestID:%d, SubQuestID:%d, ItemID:%d PlayerID:%d", m_dwQuestIdx, m_dwSubQuestIdx, dwItemIdx, pPlayer->GetID()) ;
					QUESTMGR->WriteLog(tempMsg) ;

					return nErrorCode ;
				}

				// ������ ����/���� ������ ���� üũ�Ѵ�.
				if(ITEMMGR->IsDupItem(dwItemIdx))
				{
					// 091215 ONS ������Stack�ִ���� �����ۺ��� ������ ������ ó���Ѵ�.
					const WORD wItemStackNum = ITEMMGR->GetItemStackNum( dwItemIdx );
					for( WORD count = startPos ; count < EndPos ; ++count )
					{
						if( dwItemNum == 0 ) break ;

						const ITEMBASE * pItemBase = pSlot->GetItemInfoAbs(count) ;

						if( !pSlot->IsEmpty(count) && !pSlot->IsLock(count) && pItemBase->wIconIdx == dwItemIdx && pItemBase->Durability < wItemStackNum )
						{
							if( pItemBase->Durability + dwItemNum > wItemStackNum )
							{
								dwItemNum = dwItemNum + pItemBase->Durability - wItemStackNum;
							}
							else
							{
								dwItemNum = 0 ;
							}
						}
					}

					for( WORD count = startPos ; count < EndPos ; ++count )
					{
						if( dwItemNum == 0 ) break;

						if(pSlot->IsEmpty(count))
						{
							if( dwItemNum > wItemStackNum )
							{
								dwItemNum -= wItemStackNum;
							}
						}
					}

					if(dwItemNum != 0) ++byDifferrentItemCount ;
				}
				else ++byDifferrentItemCount ;
			}



			// ������ �й谡 �������� üũ�Ѵ�.
			if(byDifferrentItemCount > byEmptyCellCount) return nErrorCode ;
			else return e_EXC_ERROR_NO_ERROR;
		}
		break ;

	case eQuestExecute_TakeQuestItemFQW:										// Ư�� ���⸦ üũ�ؼ� ����Ʈ �������� �޴� �����̸�,
		{
			if( pPlayer->GetWearedWeapon() != m_dwWeaponKind )					// �÷��̾ �����ϰ� �ִ� ����� �񱳴�� ���Ⱑ ���� ������,
			{
				nErrorCode = e_EXC_ERROR_NOT_SAME_WEAPONKIND ;					// ���� ������ �޶� ����Ʈ �������� ���� �� ���ٴ� ������ �����Ѵ�.
				return nErrorCode ;												// ���� �ڵ带 return ó���� �Ѵ�.
			}
		}
		break;
	case eQuestExecute_TakeMoney:												// �Ӵϸ� ���޹޴� �����̸�,
	case eQuestExecute_TakeMoneyPerCount:										// ī��Ʈ�� �Ӵϸ� �޴� �����̸�,
		{
			MONEYTYPE curMoney = pPlayer->GetMoney() ;							// �÷��̾��� ���� �Ӵϸ� �޴´�.

			if( curMoney >= MAX_INVENTORY_MONEY )
			{
				nErrorCode = e_EXC_ERROR_CANT_RECEIVE_MONEY ;					// �÷��̾�� ���̻� �Ӵϸ� ���� �� ���ٴ� ���� �ڵ带 �����Ѵ�.
				return nErrorCode ;													// ���� �ڵ带 return ó���� �Ѵ�.
			}
		}
	}

#endif //_MAPSERVER_

	return nErrorCode ;															// �⺻ ���� �����Ѵ�.
}










