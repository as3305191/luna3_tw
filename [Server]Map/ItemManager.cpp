#include "stdafx.h"
#include "ItemManager.h"
#include "ItemSlot.h"
#include "Player.h"
#include "Purse.h"
#include "ItemContainer.h"
#include "MHFile.h"
#include "MapDBMsgParser.h"
#include "UserTable.h"
#include "SkillTreeManager.h"
#include "CharacterCalcManager.h"
#include "GuildManager.h"
#include "Guild.h"
#include "CheckRoutine.h"
#include "MHError.h"
#include "PartyManager.h"
#include "LootingManager.h"
#include "QuestManager.h"
#include "BattleSystem_Server.h"
#include "..\[CC]Header\GameResourceStruct.h"
#include "ObjectStateManager.h"
#include "MHMap.h"
// 080923 LUJ, â�� ��ȯ�� ���� ����
#include "StorageManager.h"
#include "..\[CC]Header\GameResourceManager.h"
#include "PackedData.h"
#include "Network.h"
#include "MapNetworkMsgParser.h"
#include "GuildTournamentMgr.h"

#include "ChannelSystem.h"
#include "Vehicle.h"
#include "AISystem.h"
#include "Npc.h"
#include "RecallManager.h"
#include "VehicleManager.h"
#include "../[cc]skill/server/manager/skillmanager.h"
#include "RegenManager.h"
#include "../[CC]Skill/Server/Tree/SkillTree.h"
#include "../hseos/SHMain.h"
#include "../hseos/Common/SHMath.h"
#include "../hseos/Date/SHDateManager.h"
#include "../hseos/Residentregist/SHResidentRegistManager.h"
#include "MHFile.h"
#include "PetManager.h"
#include "Pet.h"
#include "SiegeWarfareMgr.h"
#include "SiegeRecallMgr.h"
#include "cCookManager.h"
#include "HousingMgr.h"
#include "PCRoomManager.h"
#include "../hseos/Family/SHFamilyManager.h"

CItemManager::CItemManager()
{
	m_ItemInfoList.Initialize(MAX_ITEM_NUM);
	m_ItemArrayPool.Init(500, 100, "ItemArrayPool" );
	m_ArrayIndexCreator.Init(MAX_ITEM_NUM*10, 1);
	m_DealerTable.Initialize(32);
	m_stChangeItemInfo = NULL;
	m_nChangeItemInfoNum = 0;
	mOptionPool.Init(500, 500, "Pool:ItemOption");

	LoadMixScript();
	LoadDissolveScript();
	LoadMixSetting();
	LoadCoolTimeScript();
	LoadBodyChangeScript();
}


CItemManager::~CItemManager()
{
	ITEM_INFO * pInfo = NULL;
	m_ItemInfoList.SetPositionHead();
	while( (pInfo = m_ItemInfoList.GetData()) != NULL)
		delete pInfo;
	m_ItemInfoList.RemoveAll();

	DealerData* pDealer = NULL;
	m_DealerTable.SetPositionHead();
	while((pDealer = m_DealerTable.GetData())!= NULL)
	{
		POS pos = pDealer->m_DealItemList.GetFirstPos();
		while( pos )
		{
			DealerItem* pDI = pDealer->m_DealItemList.GetNextPos( pos );
			if( pDI ) delete pDI;
		}

		pDealer->m_DealItemList.DeleteAll();
		delete pDealer;
	}
	m_DealerTable.RemoveAll();


	for(
		OptionMap::iterator it = mOptionMap.begin();
		mOptionMap.end() != it;
		++it )
	{
		SAFE_DELETE( it->second );
	}
	
	m_ItemArrayPool.Release();
	m_ArrayIndexCreator.Release();

	// desc_hseos_ü����������01
	// S ü���������� �߰� added by hseos 2007.07.29
	if (m_stChangeItemInfo)
	{
		for(int i=0; i<m_nChangeItemInfoNum; i++)
		{
			for(int j=0; j<m_stChangeItemInfo[i].nResultNum; j++)
			{
				SAFE_DELETE_ARRAY(m_stChangeItemInfo[i].stResult[j].stResultBase);
			}
			SAFE_DELETE_ARRAY(m_stChangeItemInfo[i].stResult);
		}

		SAFE_DELETE_ARRAY(m_stChangeItemInfo);
	}
	// E ü���������� �߰� added by hseos 2007.07.29


	std::vector<stMonSummonGroupInfo*>::iterator iter;
	for(iter=m_vecMonSummonGroupInfo.begin(); iter!=m_vecMonSummonGroupInfo.end(); iter++)
	{
		if((*iter))
			SAFE_DELETE((*iter));
	}

	std::map<DWORD, stNpcSummonInfo*>::iterator itr;
	for(itr=m_mapNpcSummonInfo.begin(); itr!=m_mapNpcSummonInfo.end(); itr++)
	{
		if((*itr).second)
			SAFE_DELETE((*itr).second);
	}
}

ITEMOBTAINARRAYINFO* CItemManager::Alloc(CPlayer * pPlayer, MP_CATEGORY category, BYTE protocol, DWORD dwObjectID, DWORD dwFurnisherIdx, eLogitemmoney wType, WORD ObtainNum, DBResult CallBackFunc)
{
	ITEMOBTAINARRAYINFO* const pArrayInfo = m_ItemArrayPool.Alloc();
	ZeroMemory(
		pArrayInfo,
		sizeof(*pArrayInfo));
	pArrayInfo->ItemArray.Init(
		category,
		protocol,
		dwObjectID);
	pArrayInfo->ItemArray.wObtainCount = ObtainNum;
	pArrayInfo->wObtainArrayID = m_ArrayIndexCreator.GenerateIndex();
	pArrayInfo->CallBack = CallBackFunc;
	pArrayInfo->dwFurnisherIdx = dwFurnisherIdx;
	pArrayInfo->wType = wType;
	pPlayer->AddArray(pArrayInfo);
	return pArrayInfo;
}
void CItemManager::Free(CPlayer * pPlayer, ITEMOBTAINARRAYINFO * info)
{
	info->ItemArray.Clear();
	pPlayer->RemoveArray(info->wObtainArrayID);
	m_ArrayIndexCreator.ReleaseIndex(info->wObtainArrayID);
	m_ItemArrayPool.Free( info );
}

BOOL CItemManager::IsDupItem( DWORD wItemIdx )
{
	const ITEM_INFO* info = GetItemInfo( wItemIdx );

	return info && info->Stack;
}

void CItemManager::SendErrorMessage(CObject* object, BYTE Protocol, int errorCode)
{
	MSG_ITEM_ERROR msg;
	msg.Category = MP_ITEM;
	msg.Protocol = Protocol;
	msg.ECode = errorCode;
	object->SendMsg(&msg, sizeof(msg));
}

int CItemManager::MoveItem( CPlayer * pPlayer, DWORD FromItemIdx, POSTYPE FromPos, DWORD ToItemIdx, POSTYPE& ToPos)
{
//	CItemSlot * pFromSlotD	= pPlayer->GetSlot(FromPos);
//	MHERROR->OutputFile("itemtable.txt", pFromSlotD->_ToStringSlot());

	CItemSlot * pFromSlot	= pPlayer->GetSlot(FromPos);
	CItemSlot * pToSlot		= pPlayer->GetSlot(ToPos);

	// check hacking
	if( !CheckHackItemMove( pPlayer, pFromSlot, pToSlot ) )	return eItemUseErr_Move;

	if( LOOTINGMGR->IsLootedPlayer( pPlayer->GetID() ) )
	{
		return 10;
	}

	if(FromPos == ToPos)
		return 1;

	if(!CHKRT->ItemOf(pPlayer, FromPos, FromItemIdx,0,0,CB_EXIST|CB_ICONIDX))
		return 2;

	if(!CHKRT->ItemOf(pPlayer, ToPos, ToItemIdx,0,0,CB_ICONIDX))
		return 3;
	
	const ITEMBASE * pItem = pFromSlot->GetItemInfoAbs(FromPos);
	const ITEMBASE * pToItem = pToSlot->GetItemInfoAbs(ToPos);
	// �Ⱓ�� �ֹ����� ������϶� �̵��Ұ�
	// if( FromItemIdx == eIncantation_TownMove15 || FromItemIdx == eIncantation_MemoryMove15 )
	ITEM_INFO* pInfo = GetItemInfo( FromItemIdx );
	ITEM_INFO* pToInfo = GetItemInfo( ToItemIdx );

	// 071125 KTH --- Storage Move Item Check "â���� �������� ������ ��� �α��� Ÿ�� üũ ����� �������� ������ Ǯ�� �ִ� ���� ������ ����."
	if( pToSlot == pPlayer->GetSlot(eItemTable_Storage)  )
	{
		if( (pItem->nSealed == eITEM_TYPE_UNSEAL && pInfo->nTimeKind == eKIND_PLAYTIME) || pInfo->Deposit == eITEM_TYPE_DEPOSIT_NONE )
			return eItemUseErr_Move;
	}

	// ����â���� ������ ������ ���� â������ �������̸� ���â���� ������ ����.
	if( pToSlot == pPlayer->GetSlot(eItemTable_MunpaWarehouse) )
	{
		if( (pItem->nSealed == eITEM_TYPE_UNSEAL && pInfo->nTimeKind == eKIND_PLAYTIME) || 
			pInfo->Deposit == eITEM_TYPE_DEPOSIT_NONE || 
			pInfo->Deposit == eITEM_TYPE_DEPOSIT_ONLY_STORAGE )
			return eItemUseErr_Move;
	}

	// 071126 KTH --- ShopStorage(����)���δ� �������� �ȱ�� ����.
	if( pToSlot == pPlayer->GetSlot(eItemTable_Shop) )
	{
		return eItemUseErr_Move;
	}
	// 071126 KTH --- Seal ������ �������� ������ �� ����.
	if( pToSlot == pPlayer->GetSlot(eItemTable_Weared) )
	{
		if( pItem->nSealed == eITEM_TYPE_SEAL )
			return eItemUseErr_Move;
		if( pToItem->nSealed == eITEM_TYPE_SEAL )
			return eItemUseErr_Move;
	}
	if( pFromSlot == pPlayer->GetSlot(eItemTable_Weared) )
	{
		if( pItem->nSealed == eITEM_TYPE_SEAL )
			return eItemUseErr_Move;
		if( pToItem->nSealed == eITEM_TYPE_SEAL )
			return eItemUseErr_Move;
	}
	else if( !pFromSlot->IsEmpty(FromPos) )
	{
		if( pFromSlot == pPlayer->GetSlot(eItemTable_Storage) && 
			pToItem->nSealed == eITEM_TYPE_UNSEAL && 
			pToInfo->nTimeKind == eKIND_PLAYTIME )
		{
			return eItemUseErr_Move;
		}
		// 081015 KTH --- �̵��� �����ϴ°��� â���̰� �̵� �Ϸ��� ���� �������� �̵� �Ұ� �������̶�� â���� �ü� ����.
		if( pFromSlot == pPlayer->GetSlot(eItemTable_Storage) &&
			pToInfo != NULL &&
			pToInfo->Deposit == eITEM_TYPE_DEPOSIT_NONE)
			return eItemUseErr_Move;
	}

	if(!CanbeMoved(pItem->wIconIdx,ToPos,pPlayer))
	{
		ASSERT(0); //A������OoAI��u�ϡ̡�����c��?�ˢ硧u���� A�ϩ��ˡ���c C�ϡ��ˡ�A��i��I.
		return 9;
	}
	if(!pToSlot->IsEmpty(ToPos))
	{
		pItem = pToSlot->GetItemInfoAbs(ToPos);
		if(pItem)
		{
			// Shop�������۰� �Ϲ����������� ��ȯ�� ���� üũ�Ѵ�.
			// CanbeMoved���� �Ϸ��� �����.
			ITEM_INFO* pInfo = GetItemInfo( FromItemIdx );
			ITEM_INFO* pToInfo = GetItemInfo( ToItemIdx );
			if( !pInfo || !pToInfo )			return 10;

			if(CanbeMoved(pItem->wIconIdx, FromPos, pPlayer) == FALSE)
			{
				ASSERT(0); //A������OoAI��u�ϡ̡�����c��?�ˢ硧u���� A�ϩ��ˡ���c C�ϡ��ˡ�A��i��I.
				return 10;
			}
		}
	}
	// From A����AI����i��?�ˢ硧u���� ��u����AIAU Ao��?o
	// 1.
	ITEMBASE DelFromItem;
	ZeroMemory( &DelFromItem, sizeof( DelFromItem ) );
	
	if(pFromSlot->DeleteItemAbs(pPlayer, FromPos, &DelFromItem) != EI_TRUE)
	{
		// Ao��?i����tA ��u����AIAU(FromPosition)AI ��u�Ϩ��ˡ�U 
		return 5;
	}

	ClearQuickPos(&DelFromItem, ToPos);
	// ToTable��?�ˢ硧u���� ��u����AIAU Ao��?o
	// 2.`
	ITEMBASE DelToItem;
	ZeroMemory( &DelToItem, sizeof( DelToItem ) );

	DelToItem.dwDBIdx = 0;
	if(pToSlot->DeleteItemAbs(pPlayer, ToPos, &DelToItem) != EI_TRUE)
	{
		// 0->1
		DelFromItem.Position = ToPos;
		// 3.
		if(pToSlot->InsertItemAbs(pPlayer, ToPos, &DelFromItem) != EI_TRUE)
		{
			DelFromItem.Position = FromPos;
			if(pFromSlot->InsertItemAbs(pPlayer, FromPos, &DelFromItem) != EI_TRUE)
				ASSERT(0);
			return 6;
		}
	}
	else
	{
		SWAPVALUE(DelFromItem.Position, DelToItem.Position);
		ClearQuickPos(&DelToItem, FromPos);

		if(pToSlot->InsertItemAbs(pPlayer, ToPos, &DelFromItem) != EI_TRUE)
		{
			SWAPVALUE(DelFromItem.Position, DelToItem.Position);

			if(pFromSlot->InsertItemAbs(pPlayer, FromPos, &DelFromItem) != EI_TRUE)
				ASSERT(0);
			
			if(pToSlot->InsertItemAbs(pPlayer, ToPos, &DelToItem) != EI_TRUE)
				ASSERT(0);
			return 7;
		}

		if(pFromSlot->InsertItemAbs(pPlayer, FromPos, &DelToItem) != EI_TRUE)
		{
			SWAPVALUE(DelFromItem.Position, DelToItem.Position);

			if(pFromSlot->InsertItemAbs(pPlayer, FromPos, &DelFromItem) != EI_TRUE)
				ASSERT(0);
			ClearQuickPos(&DelToItem, ToPos);
			if(pToSlot->InsertItemAbs(pPlayer, ToPos, &DelToItem) != EI_TRUE)
				ASSERT(0);
			return 8;
		}
	}

	if( ItemMoveUpdateToDBbyTable(
			pPlayer,
			DelFromItem.dwDBIdx,
			DelFromItem.wIconIdx,
			DelFromItem.Position,
			DelToItem.dwDBIdx,
			DelToItem.Position ) )
	{
		// Log
		ItemMoveLog(FromPos, ToPos, pPlayer, &DelFromItem);
		if(DelToItem.dwDBIdx != 0)
			ItemMoveLog(ToPos, FromPos, pPlayer, &DelToItem);
	}

	if( FromPos == TP_WEAR_START + eWearedItem_Weapon ||
		ToPos == TP_WEAR_START + eWearedItem_Weapon )
	{
		const ITEMBASE* pWeaponItemBase = pPlayer->GetSlot( eItemTable_Weared )->GetItemInfoAbs( TP_WEAR_START + eWearedItem_Weapon );

		if( pWeaponItemBase && pWeaponItemBase->dwDBIdx )
		{
			const ITEM_OPTION& option = GetOption( *pWeaponItemBase );

			pPlayer->SetWeaponEnchant( option.mEnchant.mLevel );
            
			if( option.mItemDbIndex )
			{
				MSG_DWORDBYTE msg;

				msg.Category = MP_ITEM;
				msg.Protocol = MP_ITEM_ENCHANT_EFFECT_NOTIFY;
				msg.dwData  = pPlayer->GetID();
				msg.bData = option.mEnchant.mLevel;

				PACKEDDATA_OBJ->QuickSend( pPlayer, &msg, sizeof( MSG_DWORDBYTE ) );
			}
			// 080519 LYW --- ItemManager : ����Ʈ�� �ʿ� ���� �������̶�� �޽����� Ŭ���̾�Ʈ�� �����Ѵ�.
			else
			{
				MSG_DWORDBYTE msg;

				msg.Category = MP_ITEM;
				msg.Protocol = MP_ITEM_ENCHANT_EFFECT_NOTIFY;
				msg.dwData	 = pPlayer->GetID();
				msg.bData	 = 0;

				PACKEDDATA_OBJ->QuickSend( pPlayer, &msg, sizeof( MSG_DWORDBYTE ) );
			}
		}
		else
		{
			pPlayer->SetWeaponEnchant( 0 );
		}
	}
	return 0;
}

int CItemManager::MovePetItem( CPlayer * pPlayer, DWORD FromItemIdx, POSTYPE FromPos, DWORD ToItemIdx, POSTYPE& ToPos)
{
	CItemSlot * pFromSlot	= pPlayer->GetSlot(FromPos);
	CPet* pPet = PETMGR->GetPet(
		pPlayer->GetPetItemDbIndex());;

	if( !pPet )
		return eItemUseErr_Err;

	const ITEMBASE * pFromItem = pFromSlot->GetItemInfoAbs(FromPos);
	const ITEMBASE * pToItem = pPet->GetWearedItem( ToPos );


	ITEMBASE FromItem;
	ITEMBASE ToItem;
	memset( &FromItem, 0, sizeof( ITEMBASE ) );
	memset( &ToItem, 0, sizeof( ITEMBASE ) );

	pFromSlot->DeleteItemAbs(pPlayer, FromPos, &FromItem);
	if( pToItem )
	{
		ToItem = *pToItem;
		pPet->SetWearedItem( ToPos, NULL );
	}

	ITEM_INFO* pFromInfo = GetItemInfo( FromItemIdx );
	ITEM_INFO* pToInfo = GetItemInfo( ToItemIdx );

	if( pFromSlot == pPlayer->GetSlot(eItemTable_Weared) )
	{
		return eItemUseErr_Move;
	}

	DWORD FromITemDBIdx = 0;
	DWORD ToItemDBIdx  = 0;

	if( FromItem.dwDBIdx )
	{
		FromITemDBIdx = FromItem.dwDBIdx;
		pPet->SetWearedItem( ToPos, &FromItem );

		// 080716 LUJ, �� ������ �̵� �α׸� �����Ѵ�
		LogItemMoney(
			pPlayer->GetID(),
			pPlayer->GetObjectName(),
			pPet->GetID(),
			pPet->GetObjectName(),
			eLog_ItemMoveFromPlayerToPet,
			pPlayer->GetMoney(),
			0,
			0,
			FromItemIdx,
			FromITemDBIdx,
			FromPos,
			ToPos,
			pFromInfo->Stack ? ( pFromItem ? pFromItem->Durability : 0 ) : 0,
			0 );
	}
	if( ToItem.dwDBIdx )
	{
		ToItemDBIdx = ToItem.dwDBIdx;
		pFromSlot->InsertItemAbs(pPlayer, FromPos, &ToItem);

		// 080716 LUJ, �� ������ �̵� �α׸� �����Ѵ�
		LogItemMoney(
			pPet->GetID(),
			pPet->GetObjectName(),
			pPlayer->GetID(),
			pPlayer->GetObjectName(),
			eLog_ItemMoveFromPetToPlayer,
			0,
			0,
			0,
			ToItemIdx,
			ToItemDBIdx,
			ToPos,
			FromPos,
			pToInfo->Stack ? ( pToItem ? pToItem->Durability : 0 ) : 0,
			0 );
	}

	ItemMoveUpdatePetInven( pPlayer->GetID(), pPlayer->GetUserID(), pPet->GetID(), FromITemDBIdx, FromPos, ToItemDBIdx, ToPos, 0 );

	return 0;
}

int CItemManager::DivideItem( CPlayer* pPlayer, DWORD ItemIdx,  POSTYPE FromPos, POSTYPE ToPos, DURTYPE FromDur, DURTYPE ToDur)
{
	if(FromPos == ToPos)
		return 1;

//---KES ItemDivide Fix 071020
//---������ ����� �κ��丮��(�������)
	//if(TP_INVENTORY_START <= FromPos && FromPos < TP_INVENTORY_END && TP_INVENTORY_START <= ToPos && ToPos < TP_INVENTORY_END)
	// 071210 KTH -- Ȯ���κ��丮���� �˻�.
	if(TP_INVENTORY_START <= FromPos && FromPos < (TP_INVENTORY_END + pPlayer->GetInventoryExpansionSize() ) && TP_INVENTORY_START <= ToPos && ToPos < (TP_INVENTORY_END + (pPlayer->GetInventoryExpansion() * TABCELL_INVENTORY_NUM ) ) )
	{
	}
	else
	{
		return 1;
	}
//-----
	
	if(!CHKRT->ItemOf(pPlayer, FromPos, ItemIdx, FromDur+ToDur, 0, CB_EXIST|CB_ICONIDX|CB_DURA))
		return 2;

	if(CHKRT->ItemOf(pPlayer, ToPos,0,0,0,CB_EXIST))
		return 3;

	if( LOOTINGMGR->IsLootedPlayer( pPlayer->GetID() ) )
	{
		return 10;
	}

	CItemSlot * pFromSlot = pPlayer->GetSlot(FromPos);

	const ITEMBASE * FromItemBase = pFromSlot->GetItemInfoAbs( FromPos );

	//---KES ItemDivide Fix 071020
	//---���� ������ �������ε����� �˻�
	if(!IsDupItem(FromItemBase->wIconIdx))
	{
		return 4;
	}
	//----------------------------
	
	//////////////////////////////////////////////////////////////////////////
	//itemmaxcheck
	// 091215 ONS ������Stack�ִ���� �����ۺ��� ������ ������ ó���Ѵ�.
	const WORD wItemStackNum = GetItemStackNum( FromItemBase->wIconIdx );
	//---KES ItemDivide Fix 071020
	//---�ߺ��˻�� ������ ����. ��ģ������ 0�϶��� ���� �� ����.
	if( FromItemBase->Durability > wItemStackNum )
		return 1;
	if( FromDur + ToDur > wItemStackNum )
		return 1;
	if( FromDur > wItemStackNum )
		return 1;
	if( ToDur > wItemStackNum )
		return 1;
	if( FromDur == 0 ) return 1;
	if( ToDur == 0 ) return 1;
	//---------------------------
	//////////////////////////////////////////////////////////////////////////

	//---KES ItemDivide Fix 071020
	//---���� �ٿ��� ������Ʈ. ������ DB ���.
	if( pFromSlot->UpdateItemAbs(pPlayer, FromPos, FromItemBase->dwDBIdx, 0, 0, 0, FromDur, UB_DURA, SS_CHKDBIDX ) != EI_TRUE )
	{
		return 7;
	}

	ItemUpdatebyTable(pPlayer, FromItemBase->dwDBIdx, FromItemBase->wIconIdx, FromDur, FromPos, FromItemBase->QuickPosition );

	POSTYPE position[] = {ToPos};

	return ObtainItemEx(
		pPlayer,
		Alloc(pPlayer, MP_ITEM, MP_ITEM_DIVIDE_ACK, pPlayer->GetID(), FromItemBase->Position, eLog_ItemObtainDivide, 2, DBResult(DivideItemDBResult)),
		FromItemBase->wIconIdx,
		(WORD)ToDur,
		position,
		_countof(position),
		_countof(position),
		FromItemBase->nSealed);
}

void CItemManager::DivideItemDBResult(CPlayer* pPlayer, WORD ArrayID)
{
	ObtainItemDBResult(pPlayer, ArrayID);
}

int CItemManager::CombineItem( CPlayer* pPlayer, DWORD ItemIdx, POSTYPE FromPos, POSTYPE ToPos, DURTYPE & FromDur, DURTYPE & ToDur)
{
	if(FromPos == ToPos)
		return 1;

	if(!CHKRT->ItemOf(pPlayer, FromPos, ItemIdx,FromDur,0,CB_EXIST|CB_ICONIDX|CB_DURA))
		return 2;

	if(!CHKRT->ItemOf(pPlayer, ToPos, ItemIdx,ToDur,0,CB_EXIST|CB_ICONIDX|CB_DURA))
		return 3;

	if( LOOTINGMGR->IsLootedPlayer( pPlayer->GetID() ) )
	{
		return 10;
	}

	CItemSlot * pFromSlot = pPlayer->GetSlot(FromPos);
	CItemSlot * pToSlot = pPlayer->GetSlot(ToPos);

	const ITEMBASE * FromItemBase = pFromSlot->GetItemInfoAbs( FromPos );
	ITEMBASE * ToItemBase = pToSlot->GetItemInfoAbs( ToPos );

	//---KES Item Combine Fix 071021
	//---���� ������ �����ͷ� �˻�
	if( !IsDupItem(FromItemBase->wIconIdx) || FromItemBase->wIconIdx != ToItemBase->wIconIdx )
	{
		return 4;
	}
	else if(FromItemBase->nSealed != ToItemBase->nSealed)
	{
		return 4;
	}
	
	// 091215 ONS ������Stack�ִ���� �����ۺ��� ������ ������ ó���Ѵ�.
	const WORD wItemStackNum = GetItemStackNum( FromItemBase->wIconIdx );
	//////////////////////////////////////////////////////////////////////////
	//---�߸��� ������ üũ
	if( FromItemBase->Durability > wItemStackNum )
		return 1;
	if( ToItemBase->Durability > wItemStackNum )
		return 1;
	//////////////////////////////////////////////////////////////////////////

	
	DURTYPE SumDur = FromItemBase->Durability + ToItemBase->Durability;
	if( SumDur > wItemStackNum )
	{
		FromDur	= (SumDur - wItemStackNum);
		ToDur = wItemStackNum;

		const DURTYPE previousFromItemQuantity = FromItemBase->Durability - FromDur;
		const DURTYPE previousToItemQuantity = ToItemBase->Durability;

		if(pFromSlot->UpdateItemAbs(pPlayer, FromPos, 0, 0, 0, 0, FromDur, UB_DURA ) != EI_TRUE )
		{
			return 5;
		}

		// 080605 LUJ, �α�
		LogItemMoney(
			pPlayer->GetID(),
			pPlayer->GetObjectName(),
			0,
			"*combine",
			eLog_ItemMoveInvenToInven,
			pPlayer->GetMoney(),
			0,
			0,
			FromItemBase->wIconIdx,
			FromItemBase->dwDBIdx,
			FromItemBase->Position,
			ToPos,
			FromItemBase->Durability,
			0 );

		if(pToSlot->UpdateItemAbs(pPlayer, ToPos, 0, 0, 0, 0, wItemStackNum, UB_DURA ) != EI_TRUE)
		{
			return 6;
		}

		// 080605 LUJ, �α�
		LogItemMoney(
			pPlayer->GetID(),
			pPlayer->GetObjectName(),
			0,
			"*combine",
			eLog_ItemObtainMerge,
			pPlayer->GetMoney(),
			0,
			0,
			ToItemBase->wIconIdx,
			ToItemBase->dwDBIdx,
			ToItemBase->Position,
			FromPos,
			ToItemBase->Durability,
			0 );

		//DB
		// update2
		ItemCombineUpdateToDB(FromItemBase->dwDBIdx, FromItemBase->Durability, ToItemBase->dwDBIdx, ToItemBase->Durability, pPlayer->GetID());

		if(eITEM_TYPE_UNSEAL == ToItemBase->nSealed)
		{
			const DWORD totalRemainTime = FromItemBase->nRemainSecond * previousFromItemQuantity + ToItemBase->nRemainSecond * previousToItemQuantity;
			const DWORD combinedRemainTime = totalRemainTime / (previousFromItemQuantity + previousToItemQuantity);

			ToItemBase->nRemainSecond = combinedRemainTime;

			UpdateRemainTime(
				pPlayer->GetID(),
				ToItemBase->dwDBIdx,
				ToItemBase->nSealed,
				combinedRemainTime);
		}
	}
	else
	{
		FromDur	= 0;
		ToDur = SumDur;

		const DURTYPE previousFromItemQuantity = FromItemBase->Durability;
		const DURTYPE previousToItemQuantity = ToItemBase->Durability;
		const DWORD previousFromItemRemainSeconds = FromItemBase->nRemainSecond;

		ITEMBASE fromItemOut;
		if(pFromSlot->DeleteItemAbs(pPlayer,  FromPos, &fromItemOut ) != EI_TRUE)
		{
			return 7;
		}

		// 080605 LUJ, �α�
		LogItemMoney(
			pPlayer->GetID(),
			pPlayer->GetObjectName(),
			0,
			"*combine",
			eLog_ItemMoveInvenToInven,
			pPlayer->GetMoney(),
			0,
			0,
			fromItemOut.wIconIdx,
			fromItemOut.dwDBIdx,
			fromItemOut.Position,
			ToPos,
			fromItemOut.Durability,
			0 );
		
		if(pToSlot->UpdateItemAbs(pPlayer, ToPos, 0, 0, 0, 0, SumDur, UB_DURA ) != EI_TRUE)
		{
			return 8;
		}

		// 080605 LUJ, �α�
		LogItemMoney(
			pPlayer->GetID(),
			pPlayer->GetObjectName(),
			0,
			"*combine",
			eLog_ItemObtainMerge,
			pPlayer->GetMoney(),
			0,
			0,
			ToItemBase->wIconIdx,
			ToItemBase->dwDBIdx,
			ToItemBase->Position,
			FromPos,
			ToItemBase->Durability,
			0 );

		ItemCombineUpdateToDB(fromItemOut.dwDBIdx, 0, ToItemBase->dwDBIdx, ToItemBase->Durability, pPlayer->GetID());
		
		if(eITEM_TYPE_UNSEAL == ToItemBase->nSealed)
		{
			const DWORD totalRemainTime = previousFromItemRemainSeconds * previousFromItemQuantity + ToItemBase->nRemainSecond * previousToItemQuantity;
			const DWORD combinedRemainTime = totalRemainTime / (previousFromItemQuantity + previousToItemQuantity);

			ToItemBase->nRemainSecond = combinedRemainTime;

			UpdateRemainTime(
				pPlayer->GetID(),
				ToItemBase->dwDBIdx,
				ToItemBase->nSealed,
				combinedRemainTime);
		}
	}
	return 0;
}

// description by hseos S
// ������ ������
// description by hseos E
int CItemManager::DiscardItem(CPlayer* pPlayer, POSTYPE whatPos, DWORD whatItemIdx, DURTYPE whatItemNum)
{
	CItemSlot * pSlot = pPlayer->GetSlot(whatPos);
	if( !pSlot )	return 1;

	const ITEMBASE * pItemBase = pSlot->GetItemInfoAbs(whatPos);
	
	if(pItemBase->wIconIdx != whatItemIdx) // discard just after move item... 
		return 4;
	
	// 081202 LUJ, ������ �������� ������ 0�� ��� ó���� �����ϵ��� �Ѵ�
	if(	IsDupItem(pItemBase->wIconIdx) )
	{
		// 091215 ONS ������Stack�ִ���� �����ۺ��� ������ ������ ó���Ѵ�.
		const WORD wItemStackNum = GetItemStackNum( pItemBase->wIconIdx );
		const BOOL invalidQuantity = ( ( pItemBase->Durability > wItemStackNum )  || ( ! pItemBase->Durability ) );

		if( invalidQuantity )
		{
			return 1;
		}
	}

	if(IsDupItem(pItemBase->wIconIdx) && (pItemBase->Durability > whatItemNum))
	{
		if( pSlot->UpdateItemAbs(pPlayer, whatPos, 0, 0, 0, 0, pItemBase->Durability - whatItemNum, UB_DURA ) != EI_TRUE )
		{
			return 2;
		}

		ItemUpdateToDB(pPlayer->GetID(), pItemBase->dwDBIdx, pItemBase->wIconIdx, 
				pItemBase->Durability, pItemBase->Position, pItemBase->QuickPosition );
	}
	else
	{
		// ItemShop
		ITEM_INFO* pItemInfo = GetItemInfo( pItemBase->wIconIdx );
		if( !pItemInfo )	return eItemUseErr_Discard;

		ITEMBASE DelItemOut;
		
		if( pSlot->DeleteItemAbs(pPlayer, whatPos, &DelItemOut, SS_LOCKOMIT ) != EI_TRUE )
			return 3;		

		//db
		// 071231 KTH -- GetID Add
		ItemDeleteToDB(DelItemOut.dwDBIdx);
	}

	return 0;
}





// 071027 LYW --- ItemManager : Add function to discard item from quest group.
// ����Ʈ �׷쿡�� ���� ������ �ݳ� ó���� �ϱ� ���� �Լ�.
// ���� �ִ� ���� �� �ִ� ������ ���� 20�� �̱� ������, DURTYPE�� ������ int ������ ��ȯ�Ͽ� ����Ѵ�.
// DURTUPE���� ��� �� ��, ��� �Ŀ� ���� ������ ��û�� ���� �߸��� ��ġ�� �� �� �ֱ� ����.
// ���� ����Ʈ �׷쿡�� �ݳ� �� �� �ִ� �������� ������ int ���� ǥ�� ���� ������ �����Ѵ�.
// ���� �� ������ ������ �Ǿ� ���� �ʱ� ������, �� �����ۿ� ���� ó���� ó�� ���� �ʴ´�.
int CItemManager::QG_Discard_StackItem(CPlayer* pPlayer, POSTYPE whatPos, DWORD whatItemIdx, int* nGiveNum)
{
	// --------------------------------------------------------------------  ó�� �������� ���Ἲ ����.

	CItemSlot * pSlot = NULL ;												// ���� ������ ���� �����͸� �����Ѵ�.
	pSlot = pPlayer->GetSlot(whatPos) ;										// ��ġ�� �ش��ϴ� ���� ������ �޴´�.

	if( !pSlot ) return err_Invalid_CItemSlot ;								// ���� ������ ��ȿ���� ������, ���� �� return.

	const ITEMBASE * pItemBase = NULL ;										// ������ �⺻ ������ ���� �����͸� �����ϰ� null.
	pItemBase = pSlot->GetItemInfoAbs(whatPos) ;							// ��ġ�� �ش��ϴ� ������ �⺻������ �޴´�.

	if( !pItemBase ) return err_Invalid_ITEMBASE ;							// �⺻ ������ ������ ��ȿ���� ������, ���� �� return.
	
	if(pItemBase->wIconIdx != whatItemIdx)	return err_Not_Same_ItemIdx ;	// �ε����� ��ġ���� ������, ���� �� return.

	if( !IsDupItem(pItemBase->wIconIdx) ) return err_Not_StackItem ;		// ���� �������� �ƴϸ�, ���� �� return.
	
	// 091215 ONS ������Stack�ִ���� �����ۺ��� ������ ������ ó���Ѵ�.
	const WORD wItemStackNum = GetItemStackNum( pItemBase->wIconIdx );

	int nCount = 0 ;														// ������ ���� ���� ������ �����ϰ� 0���� ����.
	nCount = (int)pItemBase->Durability ;									// ������ ������ ��´�.

	if(nCount > wItemStackNum) return err_Over_ItemCount ;					// �׿� �ִ� ������ ���� �ִ���� ������, ���� �� return.

	// --------------------------------------------------------------------  �������� ����ϸ�,

	if(nCount > *nGiveNum)												
	{
		int nRemainCount = 0 ;												// ������ �ݳ� �� ���� ī��Ʈ ������ �����ϰ� 0���� �����Ѵ�.
		nRemainCount = nCount - *nGiveNum ;									// ���� ī��Ʈ�� �޴´�.

		if( nRemainCount <= 0 )												// ���� ���� 0���ϸ� �ȵȴ�!
		{
			return err_Invalid_RemainCount ;								// ���� �� return.
		}

		ERROR_ITEM eResult = EI_TRUE ;										// ���� ������Ʈ ����� ���� ������ �����ϰ� �ʱ�ȭ �Ѵ�.
		eResult = pSlot->UpdateItemAbs(pPlayer, whatPos, 0, 0, 0, 0, 
												nRemainCount, UB_DURA ) ;	// ��� ���� �޴´�.

		if( eResult != EI_TRUE )											// ������Ʈ�� ���� �ߴٸ�,
		{
			return eResult ;												// ���� �� return.
		}

		ItemUpdateToDB(pPlayer->GetID(), pItemBase->dwDBIdx, 
			pItemBase->wIconIdx, pItemBase->Durability, pItemBase->Position, 
			pItemBase->QuickPosition );									// DB�� ������ ������Ʈ ó���� �Ѵ�.

		*nGiveNum = 0 ;														// �ݳ� �� ������ ���� 0���� �����Ѵ�.

		MSG_EXCHANGE_DECREASEITEM Msg;										// ������ ���� ��ġ�� �����ϴ� �޽��� ����ü�� �����Ѵ�.
			
		Msg.Category = MP_QUEST;											// ī�װ����� ����Ʈ�� �����Ѵ�.
		Msg.Protocol = MP_QUEST_GIVEITEMS_ACK;								// ���������� ������ �ݳ� �������� �����Ѵ�.

		Msg.wAbsPosition = whatPos ;										// ������ �޽��� ����ü�� �������� �����Ѵ�.
		Msg.nItemCount = nRemainCount ;										// ���� ������ ���� �����Ѵ�.
		pPlayer->SendMsg( &Msg, sizeof(Msg) );								// �÷��̾�� �޽����� �����Ѵ�.

		return err_No_Error ;												// ���� ������ return.
	}

	// --------------------------------------------------------------------  ������ ���� �ݳ��� ���� ���ų� ������,

	else																	
	{
		ITEMBASE DelItemOut ;												// ���� ������ ����ü�� �����Ѵ�.

		ERROR_ITEM eResult = EI_TRUE ;										// ���� ������Ʈ ����� ���� ������ �����ϰ� �ʱ�ȭ �Ѵ�.
		eResult = pSlot->DeleteItemAbs(pPlayer, whatPos, 
										&DelItemOut, SS_LOCKOMIT ) ;		// �������� �����ϰ� ��� ���� �޴´�.

		if( eResult != EI_TRUE )											// ������ ���� �ߴٸ�,
		{
			return eResult ;												// ���� �� return.
		}	

		// 071231 KTH -- GetID add
		ItemDeleteToDB(DelItemOut.dwDBIdx) ;								// DB�� ������ ���� ó���� �Ѵ�.

		int nRemainCount = 0 ;												// ���� ������ ���� ������ �����ϰ� 0���� ����.
		nRemainCount = (*nGiveNum - (int)DelItemOut.Durability) ;			// ���� ������ �޴´�.

		if( nRemainCount < 0 )												// ���� ���� 0���ϸ� �ȵȴ�!
		{
			return err_Invalid_RemainCount ;								// ���� �� return.
		}

		*nGiveNum = nRemainCount ;											// ���� ������ �����Ѵ�.

		MSG_EXCHANGE_REMOVEITEM Msg;										// ������ ���� ��ġ�� �����ϴ� �޽��� ����ü�� �����Ѵ�.
			
		Msg.Category = MP_QUEST;											// ī�װ����� ����Ʈ�� �����Ѵ�.
		Msg.Protocol = MP_QUEST_GIVEITEM_ACK;								// ���������� ������ �ݳ� �������� �����Ѵ�.

		Msg.wAbsPosition = whatPos ;										// ������ �޽��� ����ü�� �������� �����Ѵ�.
		pPlayer->SendMsg( &Msg, sizeof(Msg) );								// �÷��̾�� �޽����� �����Ѵ�.

		return err_No_Error ;												// ���� ������ return.
	}
}





// 071027 LYW --- ItemManager : Add function to discard normal item from quest group.
// ����Ʈ �׷쿡�� �Ϲ�(������ �ʴ�) ������ �ݳ� ó���� �ϱ� ���� �Լ�.
// ���� �ִ� ���� �� �ִ� ������ ���� 20�� �̱� ������, DURTYPE�� ������ int ������ ��ȯ�Ͽ� ����Ѵ�.
// DURTUPE���� ��� �� ��, ��� �Ŀ� ���� ������ ��û�� ���� �߸��� ��ġ�� �� �� �ֱ� ����.
// ���� ����Ʈ �׷쿡�� �ݳ� �� �� �ִ� �������� ������ int ���� ǥ�� ���� ������ �����Ѵ�.
// ���� �� ������ ������ �Ǿ� ���� �ʱ� ������, �� �����ۿ� ���� ó���� ó�� ���� �ʴ´�.
int CItemManager::QG_Discard_NormalItem(CPlayer* pPlayer, POSTYPE whatPos, DWORD whatItemIdx, int* nGiveNum)
{
	// --------------------------------------------------------------------  ó�� �������� ���Ἲ ����.

	CItemSlot * pSlot = NULL ;												// ���� ������ ���� �����͸� �����Ѵ�.
	pSlot = pPlayer->GetSlot(whatPos) ;										// ��ġ�� �ش��ϴ� ���� ������ �޴´�.

	if( !pSlot ) return err_Invalid_CItemSlot ;								// ���� ������ ��ȿ���� ������, ���� �� return.

	const ITEMBASE * pItemBase = NULL ;										// ������ �⺻ ������ ���� �����͸� �����ϰ� null.
	pItemBase = pSlot->GetItemInfoAbs(whatPos) ;							// ��ġ�� �ش��ϴ� ������ �⺻������ �޴´�.

	if( !pItemBase ) return err_Invalid_ITEMBASE ;							// �⺻ ������ ������ ��ȿ���� ������, ���� �� return.
	
	if(pItemBase->wIconIdx != whatItemIdx)	return err_Not_Same_ItemIdx ;	// �ε����� ��ġ���� ������, ���� �� return.

	if( IsDupItem(pItemBase->wIconIdx) ) return err_Not_NormalItem ;		// �Ϲ� �������� �ƴϸ�, ���� �� return.

	// --------------------------------------------------------------------  �������� ����ϸ�,

	ITEMBASE DelItemOut ;													// ���� ������ ����ü�� �����Ѵ�.

	ERROR_ITEM eResult = EI_TRUE ;											// ���� ������Ʈ ����� ���� ������ �����ϰ� �ʱ�ȭ �Ѵ�.
	eResult = pSlot->DeleteItemAbs(pPlayer, whatPos, 
									&DelItemOut, SS_LOCKOMIT ) ;			// �������� �����ϰ� ��� ���� �޴´�.

	if( eResult != EI_TRUE )												// ������ ���� �ߴٸ�,
	{
		return eResult ;													// ���� �� return.
	}	

	// 071231 KTH -- GetID Add
	ItemDeleteToDB(DelItemOut.dwDBIdx) ;									// DB�� ������ ���� ó���� �Ѵ�.

	--*nGiveNum ;															// ������ ���� ���δ�.

	if( *nGiveNum < 0 )														// ���� ���� 0���ϸ� �ȵȴ�!
	{
		*nGiveNum = 0 ;														// ���� ī��Ʈ�� 0���� �����Ѵ�.

		return err_Invalid_RemainCount ;									// ���� �� return.
	}

	MSG_EXCHANGE_REMOVEITEM Msg;											// ������ ���� ��ġ�� �����ϴ� �޽��� ����ü�� �����Ѵ�.
		
	Msg.Category = MP_QUEST;												// ī�װ����� ����Ʈ�� �����Ѵ�.
	Msg.Protocol = MP_QUEST_GIVEITEM_ACK;									// ���������� ������ �ݳ� �������� �����Ѵ�.

	Msg.wAbsPosition = whatPos ;											// ������ �޽��� ����ü�� �������� �����Ѵ�.
	pPlayer->SendMsg( &Msg, sizeof(Msg) );									// �÷��̾�� �޽����� �����Ѵ�.

	return err_No_Error ;													// ���� ������ return.
}





int CItemManager::SellItem( CPlayer* pPlayer, POSTYPE whatPos, DWORD wSellItemIdx, DURTYPE sellItemDur, DWORD DealerIdx )
{
	if(0 == pPlayer)
	{
		return ERR_INVALPLAYER;
	}

	CItemSlot* const pSlot = pPlayer->GetSlot(
		whatPos);

	if(0 == pSlot)
	{
		return ERR_INVALSLOT;
	}

	const ITEM_INFO* const pItemInfo = GetItemInfo(
		wSellItemIdx);

	if(0 == pItemInfo)
	{
		return ERR_INVALITEMINFO;
	}
	else if(FALSE == pItemInfo->Sell)
	{
		return ERR_NOTSELLITEM;
	}

    ITEMBASE Item = *pSlot->GetItemInfoAbs(
		whatPos);

	if(eItemTable_Inventory != GetTableIdxPosition(whatPos))
	{
		return ERR_NOTINVENITEM;
	}
	else if(FALSE == CHKRT->ItemOf( pPlayer, whatPos, wSellItemIdx, sellItemDur, 0, CB_EXIST|CB_ICONIDX|CB_ENOUGHDURA ) )
	{
		return ERR_INVALIDENTITY;
	}

	const MONEYTYPE money = pItemInfo->SellPrice * (pItemInfo->Stack ? sellItemDur : 1);

	if(FALSE == pPlayer->IsEnoughAdditionMoney(money))
	{
		return ERR_MAXMONEY;
	}
	else if(LOOTINGMGR->IsLootedPlayer(pPlayer->GetID()))
	{
		return ERR_LOOTPLAYER;
	}
	else if(CPet* const pet = PETMGR->GetPet(Item.dwDBIdx))
	{
		if(ePetState_None == pet->GetObjectInfo().State)
		{
			return ERR_NOTSELLITEM;
		}
	}
	else if(CVehicle* const vehicle = (CVehicle*)g_pUserTable->FindUser(pPlayer->GetSummonedVehicle()))
	{
		if(eObjectKind_Vehicle == vehicle->GetObjectKind() &&
			vehicle->GetUseItem().dwDBIdx == Item.dwDBIdx)
		{
			return ERR_NOTSELLITEM;
		}
	}

	const ITEM_OPTION optionForLog = GetOption(Item);

	// ������ ���� ó���� �Ѵ�.
	if( EI_TRUE != DiscardItem( pPlayer, whatPos, wSellItemIdx, sellItemDur ) ) return ERR_FAILDISCARD ;
	
	pPlayer->SetMoney(
		money,
		MONEY_ADDITION,
		0,
		eItemTable_Inventory,
		eMoneyLog_GetStall,
		DealerIdx);
	LogItemMoney(
		pPlayer->GetID(),
		pPlayer->GetObjectName(),
		DealerIdx,
		"",
		eLog_ItemSell,
		pPlayer->GetMoney(),
		0,
		money,
		Item.wIconIdx,
		Item.dwDBIdx,
		Item.Position,
		0,
		Item.Durability,
		0);
	LogItemDropOption(
		optionForLog,
		eLog_ItemSell);
	LogItemOption(
		pPlayer->GetID(),
		Item.dwDBIdx,
		optionForLog,
		eLog_ItemSell);
	return ERR_NONE ;
}

int CItemManager::BuyItem( CPlayer* pPlayer, DWORD buyItemIdx, WORD buyItemNum, WORD DealerIdx )
{
//	if(!CHKRT->StateOf(pPlayer, eObjectState_Deal))
//		return 1;

	//DealerIdx��?�ˢ硧u���� ����A�ˡ�A ��u����AIAUAI�����ˢ�??
	if(!CHKRT->DearlerItemOf(DealerIdx, buyItemIdx))
		return 2;

	if( buyItemNum == 0 ) return EI_TRUE;	//0AI�ˡ�e���������ϩ�E~����a

	if( buyItemNum > MAX_ITEMBUY_NUM )
		buyItemNum = MAX_ITEMBUY_NUM;

	ITEM_INFO * pItemInfo	= GetItemInfo( buyItemIdx );
	CItemSlot * pSlot = pPlayer->GetSlot(eItemTable_Inventory);
	WORD obtainItemNum = 0;
	MONEYTYPE ItemPrice = pItemInfo->BuyPrice;

	obtainItemNum = GetCanBuyNumInMoney(pPlayer, buyItemNum, ItemPrice);
	if(obtainItemNum == 0)
		return NOT_MONEY; //��i����AI �ˡ��ϡ�AU�ˡ���I�ˡ�U.

	MONEYTYPE ItemFishPoint = pItemInfo->dwBuyFishPoint;
	if(0 < ItemFishPoint)
	{
		WORD wCanBuyNum = GetCanBuyNumInFishingPoint(pPlayer, buyItemNum, ItemFishPoint);
		if(wCanBuyNum < buyItemNum)
			return NOT_FISHINGPOINT;
	}

	WORD EmptyCellPos[255];
	WORD EmptyCellNum;
	obtainItemNum = GetCanBuyNumInSpace(pPlayer, pSlot, buyItemIdx, obtainItemNum, EmptyCellPos, EmptyCellNum);
	if(obtainItemNum == 0)
		return NOT_SPACE;	//AU�ˡ���c�����ˢ� ��u�Ϩ��ˡ�U.

	
	// 100104 ShinJS --- Item Point Type ����
	switch( pItemInfo->wPointType )
	{
		// Coin ����
	case eItemPointType_Item:
		{
			// 090227 ShinJS --- �ϻ��� �߰��۾� : Coin���� �����ϴ� �������� ��� �ʿ�Coin ���� Ȯ��
			const POSTYPE	inventoryStartPosition	= POSTYPE( TP_INVENTORY_START );
			const POSTYPE	inventoryEndPosition	= POSTYPE( TP_INVENTORY_END + pPlayer->GetInventoryExpansionSize() );
			DWORD dwCoinCnt = 0;
			DWORD dwCoinTypeIdx = pItemInfo->dwPointTypeValue1;
			DWORD dwNeedCoinCnt = pItemInfo->dwPointTypeValue2;

			// �κ��丮�� Coin ���� Ȯ��
			for( POSTYPE position = inventoryStartPosition ; inventoryEndPosition > position ; ++position )
			{
				const ITEMBASE* item = pSlot->GetItemInfoAbs( position );
				if( !item )		continue;

				// �ʿ� Coin�� ���
				if( item->wIconIdx	== dwCoinTypeIdx )
				{
					const BOOL bIsDupIicate = IsDupItem( item->wIconIdx );
					const DWORD quantity = ( bIsDupIicate ? item->Durability : 1 );

					dwCoinCnt += quantity;		// ���� ����

					// ����Coin ���� �˳��� ��� ���� ����
					if( dwCoinCnt >= dwNeedCoinCnt * obtainItemNum )						
						break;
				}
			}

			// ����Coin ���� ������ ���
			if( dwCoinCnt < dwNeedCoinCnt * obtainItemNum )		
				return NOT_COIN;
		}
		break;
		// PC�� ����Ʈ ����
	case eItemPointType_PCRoomPoint:
		{
			if(0 == PCROOMMGR->GetGameRoomIndex(pPlayer->GetID()))
				return NOT_PCROOM;

			DWORD dwNeedPCRoomPoint = pItemInfo->dwPointTypeValue1 * buyItemNum;
			DWORD dwPlayerPCRoomPoint = PCROOMMGR->UpdatePoint( pPlayer->GetID() );

			// ������ ���
			if( dwNeedPCRoomPoint > dwPlayerPCRoomPoint )
				return NOT_PCROOMPOINT;
		}
		break;
	}

	int rt = 0;
	if( EI_TRUE == (rt = ObtainItemEx(pPlayer, Alloc(pPlayer, MP_ITEM, MP_ITEM_BUY_ACK, pPlayer->GetID(), DealerIdx, eLog_ItemObtainBuy, obtainItemNum, (DBResult)(BuyItemDBResult)), buyItemIdx, obtainItemNum, EmptyCellPos, EmptyCellNum, EmptyCellNum, pItemInfo->wSeal)) )
	{
		pPlayer->SetMoney( ItemPrice*obtainItemNum, MONEY_SUBTRACTION, 0, eItemTable_Inventory, eMoneyLog_LoseStall, DealerIdx );

		if(0 < ItemFishPoint)
		{
			//090212 pdy ���� ����Ʈ ���� ���� 
			//���� ����Ʈ ������ obtainItemNum(��������)�� ���������ʾ� �ټ��� �������� ���� �����߻� 
			DWORDEX dwFishPoint = pPlayer->GetFishPoint() - (ItemFishPoint * obtainItemNum);		// * obtainItemNum�� ������Ѵ�. 
			pPlayer->SetFishPoint(dwFishPoint);

			MSG_DWORDEX msgFishPoint;
			msgFishPoint.Category = MP_FISHING;
			msgFishPoint.Protocol = MP_FISHING_POINT_ACK;
			msgFishPoint.dweData   = dwFishPoint;
			pPlayer->SendMsg( &msgFishPoint, sizeof(msgFishPoint) );

			FishingData_Update(pPlayer->GetID(), pPlayer->GetFishingLevel(), pPlayer->GetFishingExp(), pPlayer->GetFishPoint());

			// 080808 LUJ, ���� ����ġ/���� �߰�
			Log_Fishing(
				pPlayer->GetID(),
				eFishingLog_UsePointForBuyItem,
				(ItemFishPoint * obtainItemNum),		
				pPlayer->GetFishPoint(),
				buyItemIdx,
				0,
				pPlayer->GetFishingExp(),
				pPlayer->GetFishingLevel() );
		}
	}

	return rt;
}

void CItemManager::BuyItemDBResult(CPlayer * pPlayer, WORD ArrayID )
{
	ObtainItemDBResult(pPlayer, ArrayID);
}

// 090929 LUJ, ���� ���ͷκ��� �������� ȹ���� ��� �ٸ� �÷��̾�� �˸����� ��. ���� �ڵ带 ������
void CItemManager::MonsterObtainItem(CPlayer* player, DWORD itemIndex, DWORD monsterKind, WORD itemQuantity)
{
	CItemSlot* const playSlot = player->GetSlot(eItemTable_Inventory);

	if(0 == playSlot)
	{
		return;
	}

	const ITEM_INFO* const itemInfo = GetItemInfo(
		itemIndex);

	if(0 == itemInfo)
	{
		return;
	}

	WORD cellPoition[128] = {0};
	WORD cellSize = 0;
	const WORD emptyCellSize = GetCanBuyNumInSpace(
		player,
		playSlot,
		itemIndex,
		random(1, itemQuantity),
		cellPoition,
		cellSize);

	if(0 == emptyCellSize)
	{
		SendErrorMessage(
			player,
			MP_ITEM_BUY_NACK,
			NOT_SPACE);
		return;
	}

	ITEMOBTAINARRAYINFO* const obtainArrayInfo = Alloc(
		player,
		MP_ITEM,
		MP_ITEM_MONSTER_OBTAIN_NOTIFY,
		player->GetID(),
		monsterKind,
		eLog_ItemObtainMonster,
		emptyCellSize,
		DBResult(ObtainItemDBResult));
	const int isFailed = ObtainItemEx(
		player,
		obtainArrayInfo,
		itemIndex,
		itemQuantity,
		cellPoition,
		cellSize,
		cellSize,
		itemInfo->wSeal);

	if(isFailed)
	{
		return;
	}

	PARTYMGR->SendObtainItemMsg(
		player,
		itemIndex);

	const BASE_MONSTER_LIST* const monsterList = GAMERESRCMNGR->GetMonsterListInfo(monsterKind);
	const BOOL isBossMonster = (monsterList && monsterList->ObjectKind == eObjectKind_BossMonster);

	if(isBossMonster)
	{
		MSG_USER_ADD_ITEM message;
		ZeroMemory(&message, sizeof(message));
		message.Category = MP_SIGNAL;
		message.Protocol = MP_SIGNAL_USER_ADD_ITEM;
		SafeStrCpy(
			message.CharacterName,
			player->GetObjectName(),
			sizeof(message.CharacterName) / sizeof(*message.CharacterName));
		message.dwIconIdx = itemIndex;

		PACKEDDATA_OBJ->QuickSend(
			player,
			&message,
			sizeof(message));
	}
}

int CItemManager::CheatObtainItem( CPlayer* pPlayer, DWORD obtainItemIdx, WORD ItemNum )
{
	WORD EmptyCellPos[255];
	WORD EmptyCellNum;
	CItemSlot * pSlot = pPlayer->GetSlot(eItemTable_Inventory);
	WORD obtainItemNum = GetCanBuyNumInSpace( pPlayer, pSlot, obtainItemIdx, ItemNum, EmptyCellPos, EmptyCellNum );
	if(obtainItemNum == 0)
	{
		SendErrorMessage(
			pPlayer,
			MP_ITEM_BUY_NACK,
			NOT_SPACE);
		return 1;
	}

	const ITEM_INFO* const itemInfo = GetItemInfo(
		obtainItemIdx);

	return ObtainItemEx(
		pPlayer,
		Alloc(pPlayer, MP_CHEAT, MP_CHEAT_ITEM_ACK, pPlayer->GetID(), 0, eLog_ItemObtainCheat, obtainItemNum, DBResult(ObtainItemDBResult)),
		obtainItemIdx,
		obtainItemNum,
		EmptyCellPos,
		EmptyCellNum,
		EmptyCellNum,
		itemInfo ? itemInfo->wSeal : eITEM_TYPE_SEAL_NORMAL);
}

// desc_hseos_���͹���01
// S ���͹��� �߰� added by hseos 2007.07.08
int CItemManager::ObtainMonstermeterItem( CPlayer* pPlayer, DWORD obtainItemIdx, WORD ItemNum )
{
	WORD EmptyCellPos[255];
	WORD EmptyCellNum;
	CItemSlot * pSlot = pPlayer->GetSlot(eItemTable_Inventory);
	WORD obtainItemNum = GetCanBuyNumInSpace( pPlayer, pSlot, obtainItemIdx, ItemNum, EmptyCellPos, EmptyCellNum );
	if(obtainItemNum == 0)
	{
		SendErrorMessage(
			pPlayer,
			MP_ITEM_BUY_NACK,
			NOT_SPACE);
		return 1;
	}

    const ITEM_INFO* const itemInfo = GetItemInfo(
		obtainItemIdx);

	if(0 == itemInfo)
	{
		return 1;
	}

	return ObtainItemEx(
		pPlayer,
		Alloc(pPlayer, MP_ITEM, MP_ITEM_MONSTER_OBTAIN_NOTIFY, pPlayer->GetID(), 0, eLog_ItemObtainMonstermeter, obtainItemNum, DBResult(ObtainItemDBResult)),
		obtainItemIdx,
		obtainItemNum,
		EmptyCellPos,
		EmptyCellNum,
		EmptyCellNum,
		itemInfo->wSeal);
}

int CItemManager::ObtainGeneralItem(CPlayer* pPlayer, DWORD obtainItemIdx, WORD ItemNum, eLogitemmoney nObtainKind, MP_PROTOCOL_ITEM nProtocal)
{
	WORD EmptyCellPos[255];
	WORD EmptyCellNum;
	CItemSlot * pSlot = pPlayer->GetSlot(eItemTable_Inventory);
	WORD obtainItemNum = GetCanBuyNumInSpace( pPlayer, pSlot, obtainItemIdx, ItemNum, EmptyCellPos, EmptyCellNum );
	if(obtainItemNum == 0)
	{
		SendErrorMessage(
			pPlayer,
			MP_ITEM_BUY_NACK,
			NOT_SPACE);
		return 1;
	}

	const ITEM_INFO* const itemInfo = GetItemInfo(
		obtainItemIdx);

	if(0 == itemInfo)
	{
		return 1;
	}

	return ObtainItemEx(
		pPlayer,
		Alloc(pPlayer, MP_ITEM, BYTE(nProtocal), pPlayer->GetID(), 0, nObtainKind, obtainItemNum, DBResult(ObtainItemDBResult)),
		obtainItemIdx,
		obtainItemNum,
		EmptyCellPos,
		EmptyCellNum,
		EmptyCellNum,
		itemInfo->wSeal);
}

int CItemManager::ObtainItemEx(CPlayer* pPlayer, ITEMOBTAINARRAYINFO* pArrayInfo, DWORD obtainItemIdx, WORD whatItemNum, WORD * EmptyCellPos, WORD EmptyCellNum, WORD ArrayInfoUnitNum, ITEM_SEAL_TYPE wSeal)
{
	WORD obtainItemNum = whatItemNum;
	ITEMBASE NewItemBase;
	memset( &NewItemBase, 0, sizeof(ITEMBASE) );

	const ITEM_INFO* const pItemInfo = GetItemInfo(obtainItemIdx);

	if(0 == pItemInfo)
	{
		return 3;
	}

	BOOL bDBReturn = FALSE;

	if(pItemInfo->Stack)
	{	
		const WORD wItemStackNum = GetItemStackNum(obtainItemIdx);

		for(POSTYPE i = 0 ; i < EmptyCellNum ; ++i )
		{
			CItemSlot * pSlot = pPlayer->GetSlot(EmptyCellPos[i]);
			const ITEMBASE * pItemBase = pSlot->GetItemInfoAbs(EmptyCellPos[i]);
			BOOL bIsEmpty = pSlot->IsEmpty(EmptyCellPos[i]);
			if( bIsEmpty )
			{
				bDBReturn = TRUE;
				if( obtainItemNum > wItemStackNum )
				{
					NewItemBase.Position		= EmptyCellPos[i];
					if(pSlot->InsertItemAbs(pPlayer, EmptyCellPos[i], &NewItemBase, SS_PREINSERT) != EI_TRUE)
					{
						return 3;
						ASSERT(0);
					}
					ItemInsertToDB(pPlayer->GetID(), obtainItemIdx,  wItemStackNum , EmptyCellPos[i], MAKEDWORD(ArrayInfoUnitNum, pArrayInfo->wObtainArrayID), wSeal);
					obtainItemNum = obtainItemNum - wItemStackNum;
				}
				else
				{
					NewItemBase.Position		= EmptyCellPos[i]; 
					if(pSlot->InsertItemAbs(pPlayer, EmptyCellPos[i], &NewItemBase, SS_PREINSERT) != EI_TRUE)
					{
						return 4;
						ASSERT(0);
					}
					
					ItemInsertToDB(
						pPlayer->GetID(),
						obtainItemIdx,
						obtainItemNum,
						EmptyCellPos[i],
						MAKEDWORD(ArrayInfoUnitNum, pArrayInfo->wObtainArrayID),
						wSeal);
					obtainItemNum = 0;
				}
			}
			else if( !bIsEmpty &&
				pItemBase->wIconIdx == obtainItemIdx && 
				pItemBase->Durability <  wItemStackNum  )	//$ �ߺ� ������ ä���
			{				
				// 090206 LUJ, �α׸� ���� �纻�� �����صд�
				const DURTYPE previousItemCount = pItemBase->Durability;

				if( pItemBase->Durability + obtainItemNum >  wItemStackNum  )
				{
					obtainItemNum = obtainItemNum + (WORD)pItemBase->Durability -  wItemStackNum ;
					if( pSlot->UpdateItemAbs(pPlayer, pItemBase->Position, 0, 0, 0, 0,  wItemStackNum , UB_DURA, SS_PREINSERT)  != EI_TRUE )
					{
						///SendErrorMsg();
						return 1;
					}
					
					// 090206 LUJ, �α��� ������ ������ �߰��� �з���ŭ ǥ�õǵ��� �ϱ� ����
					//			���� ������ ������ �ߺ� �����ۿ����� ������� �ʴ� ItemParam�� �����Ѵ�
					pArrayInfo->ItemArray.AddItem( pItemBase->dwDBIdx, 
						pItemBase->wIconIdx, 
						pItemBase->Durability, 
						pItemBase->Position, 
						pItemBase->QuickPosition,
						previousItemCount,
						0,
						pItemBase->nSealed);
					
					// to apply DB
					ItemUpdateToDB(pPlayer->GetID(), pItemBase->dwDBIdx, pItemBase->wIconIdx, 
								pItemBase->Durability, pItemBase->Position, pItemBase->QuickPosition );
					
					// '��?�ˢ硧u���� UpdateCI�ˡ�A ��u�Ϩ�������I��?�ˢ� ��������AI ��oU�ϡ�i����i 
					// ��oU�ϡ�i ��u����AIAUAC �ˡ������ˡ��ϡ̡ˡ���cAO��uO�ˡ�| ��uo��ui ��?I��u���� 
					// ��u���������ˢ�AC ��oo���������ˡ�| ��uc��ieC�ϡ�= 
				}
				else
				{
					if( pSlot->UpdateItemAbs(pPlayer, pItemBase->Position, 0, 0, 0, 0, pItemBase->Durability + obtainItemNum, UB_DURA, SS_PREINSERT )  != EI_TRUE )
					{
						//SendErrorMsg();
						return 2;
					}
					
					// 090206 LUJ, �α��� ������ ������ �߰��� �з���ŭ ǥ�õǵ��� �ϱ� ����
					//			���� ������ ������ �ߺ� �����ۿ����� ������� �ʴ� ItemParam�� �����Ѵ�
					pArrayInfo->ItemArray.AddItem( pItemBase->dwDBIdx, 
						pItemBase->wIconIdx, 
						pItemBase->Durability, 
						pItemBase->Position, 
						pItemBase->QuickPosition,
						previousItemCount,
						0,
						pItemBase->nSealed);
					
					// to apply DB
					ItemUpdateToDB(pPlayer->GetID(), pItemBase->dwDBIdx, pItemBase->wIconIdx, 
								pItemBase->Durability, pItemBase->Position, pItemBase->QuickPosition );
					
					obtainItemNum = 0;
				}
			}
			else
				ASSERT(0);
		}
		
		if( bDBReturn == FALSE )
		{
			pArrayInfo->CallBack( pPlayer, (WORD)pArrayInfo->wObtainArrayID );
			// 090227 ShinJS --- Coin Item�� ��� Coin�� �����ϱ� ���� �ϴ��� �۾��� ���� return ����
//			return 0;
		}
	}
	else	//$ �ߺ��Ұ� ������ ó��
	{
		for(POSTYPE i = 0 ; i < EmptyCellNum ; ++i)
		{
			CItemSlot * pSlot = pPlayer->GetSlot(EmptyCellPos[i]);
			if( pSlot->IsEmpty(EmptyCellPos[i]) )
			{
				NewItemBase.Position		= EmptyCellPos[i]; 
				if(pSlot->InsertItemAbs(pPlayer, EmptyCellPos[i], &NewItemBase, SS_PREINSERT) != EI_TRUE)
				{
					return 5;
					ASSERT(0);
				}
				--obtainItemNum;

				ItemInsertToDB(pPlayer->GetID(), obtainItemIdx, 0, EmptyCellPos[i], MAKEDWORD(ArrayInfoUnitNum, pArrayInfo->wObtainArrayID), pItemInfo->wSeal);
			}
		}
	}

	if(obtainItemNum > 0)
	{
		ASSERT(0);
		return 6;
	}

	// 100104 ShinJS --- Item Point Type ����
	switch( pItemInfo->wPointType )
	{
		// Coin ����
	case eItemPointType_Item:
		{
			// 090227 ShinJS --- ������ Item�� Coin���� Item�� ��� �ʿ�Coin ����
			CItemSlot* pSlot = pPlayer->GetSlot( NewItemBase.Position );
			if( !pSlot )		return -1;

			MSG_ITEM_BUY_BY_COIN buybyCoinMessage;
			ZeroMemory( &buybyCoinMessage, sizeof(MSG_ITEM_BUY_BY_COIN) );

			const POSTYPE	inventoryStartPosition	= POSTYPE( TP_INVENTORY_START );
			const POSTYPE	inventoryEndPosition	= POSTYPE( TP_INVENTORY_END + pPlayer->GetInventoryExpansionSize() );
			DWORD dwCoinTypeIdx = pItemInfo->dwPointTypeValue1;															// ���Ž� �ŷ��� Coin�� Item Index
			int nNeedCoinCnt = pItemInfo->dwPointTypeValue2 * whatItemNum;												// ���Ž� �ʿ�Coin�� �� ����

			// �κ��丮�� �ʿ�Coin ����
			for( POSTYPE position = inventoryStartPosition ; inventoryEndPosition > position ; ++position )
			{
				// �ʿ�Coin�� ���� ��� �Ϸ�
				if( nNeedCoinCnt <= 0 )	break;

				const ITEMBASE* itemInven = pSlot->GetItemInfoAbs( position );
				const ITEM_INFO* pInfoInven = ITEMMGR->GetItemInfo( itemInven->wIconIdx );
				if( !itemInven || !pInfoInven )		continue;

				// �ʿ�Coin�� ���
				if( itemInven->wIconIdx	== dwCoinTypeIdx )
				{
					const BOOL bIsDupIicate = BOOL(pInfoInven->Stack);
					const DWORD quantity = ( bIsDupIicate ? itemInven->Durability : 1 );

					// �ʿ�Coin ������ ���� ��� ������ �� ����(Update)
					if( quantity > (DWORD)nNeedCoinCnt  )
					{
						if( EI_TRUE != pSlot->UpdateItemAbs( pPlayer,
							itemInven->Position,
							itemInven->dwDBIdx,
							itemInven->wIconIdx,
							itemInven->Position,
							itemInven->QuickPosition,
							quantity - nNeedCoinCnt,
							UB_DURA,
							SS_CHKDBIDX ) )
							continue;

						ItemUpdateToDB( pPlayer->GetID(), itemInven->dwDBIdx, itemInven->wIconIdx, itemInven->Durability, itemInven->Position, itemInven->QuickPosition );

						LogItemMoney(
							pPlayer->GetID(),
							pPlayer->GetObjectName(),
							0,
							"",
							eLog_CoinDecreaseByItemBuy,
							pPlayer->GetMoney(),
							0,
							0,
							itemInven->wIconIdx,
							itemInven->dwDBIdx,
							itemInven->Position,
							0,
							itemInven->Durability,
							0 );

						buybyCoinMessage.mItem[ buybyCoinMessage.mSize++ ] = *itemInven;
					}
					// �ʿ�Coin ������ ���ų� ���� ��� ������ ����(Delete)
					else
					{
						ITEMBASE DelItemOut;

						if( pSlot->DeleteItemAbs(pPlayer, itemInven->Position, &DelItemOut) != EI_TRUE )
							continue;

						ItemDeleteToDB(DelItemOut.dwDBIdx);

						LogItemMoney(
							pPlayer->GetID(),
							pPlayer->GetObjectName(),
							0,
							"",
							eLog_CoinDestroyByItemBuy,
							pPlayer->GetMoney(),
							0,
							0,
							itemInven->wIconIdx,
							itemInven->dwDBIdx,
							itemInven->Position,
							0,
							itemInven->Durability,
							0 );

						buybyCoinMessage.mItem[ buybyCoinMessage.mSize++ ].dwDBIdx = DelItemOut.dwDBIdx;
					}

					nNeedCoinCnt -= quantity;		// �ʿ�Coin �� ����
				}
			}

			// ������ ��ȭ�� �ִ°�� ��Ŷ ����
			if( buybyCoinMessage.mSize > 0 )
			{
				buybyCoinMessage.Category = MP_ITEM;
				buybyCoinMessage.Protocol = MP_ITEM_BUY_BY_COIN;

				pPlayer->SendMsg( &buybyCoinMessage, buybyCoinMessage.GetSize() );
			}
		}
		break;

		// PC�� ����Ʈ ����
	case eItemPointType_PCRoomPoint:
		{
			DWORD dwNeedPCRoomPoint = pItemInfo->dwPointTypeValue1 * whatItemNum;
			// ����Ʈ ����
			DWORD dwPlayerPCRoomPoint = PCROOMMGR->UpdatePoint( pPlayer->GetID() );
			dwPlayerPCRoomPoint = (dwPlayerPCRoomPoint > dwNeedPCRoomPoint ? dwPlayerPCRoomPoint - dwNeedPCRoomPoint : 0);
			PCROOMMGR->SetPoint( pPlayer->GetID(), dwPlayerPCRoomPoint );

			g_DB.FreeMiddleQuery(
				0,
				pPlayer->GetID(),
				"EXEC dbo.MP_GAMEROOM_UPDATE_POINT %d, %d",
				pPlayer->GetID(),
				dwPlayerPCRoomPoint);
			g_DB.LogMiddleQuery(
				0,
				0,
				"EXEC DBO.TP_GAMEROOM_POINT_LOG_INSERT %d, %d, %d",
				eLog_ItemUse,
				pPlayer->GetUserID(),
				dwPlayerPCRoomPoint);

			// ����� ����Ʈ ����
			MSG_DWORD2 msg;
			ZeroMemory( &msg, sizeof(msg) );
			msg.Category = MP_PCROOM;
			msg.Protocol = MP_PCROOM_CHANGE_POINT;
			msg.dwData1 = pPlayer->GetID();
			msg.dwData2 = dwPlayerPCRoomPoint;

			pPlayer->SendMsg( &msg, sizeof(msg) );
		}
		break;
	}

	return 0;
}

// 080327 LUJ, ��� �ɼ� �α� �߰�
void CItemManager::ObtainItemDBResult(CPlayer * pPlayer, WORD ArrayID)
{
	ITEMOBTAINARRAYINFO * pArrayInfo = pPlayer->GetArray(ArrayID);

	for( WORD i = 0 ; i < pArrayInfo->ItemArray.ItemNum ; ++i )
	{
		ITEMBASE * item = pArrayInfo->ItemArray.GetItem(  i  );
		CItemSlot * pSlot = pPlayer->GetSlot(item->Position);

		//if(pSlot->InsertItemAbs(pPlayer, item->Position, item) != EI_TRUE)

		// 071218 LUJ, ���ԵǴ� �������� ����� Ǫ�� �޼ҵ带 ȣ���ؾ� �Ѵ�
		if( EI_TRUE != pSlot->InsertItemAbsFromDb( pPlayer, *item ) )
		{
			continue;
		}

		MONEYTYPE			ChangeMoney	= 0;
		const ITEM_INFO*	pInfo = ITEMMGR->GetItemInfo( item->wIconIdx );

		if(pArrayInfo->wType == eLog_ItemObtainBuy)
		{
			ChangeMoney = pInfo->BuyPrice;
		}

		switch( pArrayInfo->ItemArray.Protocol )
		{
		case MP_ITEM_MIX_GETITEM:
			{
				// 080925 LUJ, ���� ���� ������ ��� ���θ� �̻������ furnisher index�� ����Ͽ� ó���Ѵ�
				ICONBASE supportItem = { 0 };
				supportItem.wIconIdx = pArrayInfo->dwFurnisherIdx;

				ITEMMGR->AddMixOption( *pPlayer, *item, supportItem );
				break;
			}
		case MP_ITEM_MONSTER_OBTAIN_NOTIFY:
			{
				MSG_ITEM_OPTION optionMessage;
				{
					ZeroMemory( &optionMessage, sizeof( optionMessage ) );

					optionMessage.Category	= MP_ITEM;
					optionMessage.Protocol	= MP_ITEM_OPTION_ADD_ACK;
					optionMessage.mSize		= 1;
				}

				// 080526 LUJ, �������� �������� 0���� ȹ���� ��� �α׸� �����ϵ��� ��
				if(		pInfo->Stack &&
					!	item->Durability )
				{
					SYSTEMTIME time = { 0 };
					GetLocalTime( &time );

					char buffer[ MAX_PATH ] = { 0 };
					sprintf(
						buffer,
						"log/Zero.Qty.Item.%04d%02d%02d.log",
						time.wYear,
						time.wMonth,
						time.wDay );

					FILE* file = fopen( buffer, "r" );

					if( ! file )
					{
						file = fopen( buffer, "a" );

						if( file )
						{
							const char* title = "Time\tPlayer\tItem\tDBIdx\tMap\n";

							fwrite(
								title,
								sizeof( *title ),
								strlen( title ),
								file );
						}
					}

					fclose( file );
					file = fopen( buffer, "a" );

					if( file )
					{
						sprintf(
							buffer,
							"%02d:%02d:%02d\t%u\t%u\t%u\t%d\n",
							time.wHour,
							time.wMinute,
							time.wSecond,
							pPlayer->GetID(),
							item->wIconIdx,
							item->dwDBIdx,
							g_pServerSystem->GetMapNum() );

						fwrite(
							buffer,
							sizeof( *buffer ),
							strlen( buffer ),
							file );

						fclose( file );
					}
				}

				ITEM_OPTION& option = optionMessage.mOption[ 0 ];

				ITEMMGR->AddDropOption( *item, option );

				const ITEM_OPTION emptyOption = { 0 };

				if( ! memcmp( &emptyOption, &option, sizeof( emptyOption ) ) )
				{
					break;
				}

				option.mItemDbIndex	= item->dwDBIdx;

				if( memcmp( &emptyOption.mEnchant, &option.mEnchant, sizeof( emptyOption.mEnchant ) ) )
				{
					// 081118 LUJ, ĳ���� ��ȣ�� ������ ������ DB �����带 ������ Ű������ ���ǵ��� ��
					ItemOptionInsert( option, pPlayer->GetID() );
					LogItemOption(
						pPlayer->GetID(),
						item->dwDBIdx,
						option,
						eLog_ItemObtainMonster );
				}

				if( memcmp( &emptyOption.mDrop, &option.mDrop, sizeof( emptyOption.mDrop ) ) )
				{
					// 081118 LUJ, ĳ���� ��ȣ�� ������ ������ DB �����带 ������ Ű������ ���ǵ��� ��
					ItemDropOptionInsert( option, pPlayer->GetID() );
					// 080327 LUJ, �α�
					LogItemDropOption(
						option,
						eLog_ItemObtainMonster );
				}

				ITEMMGR->AddOption( option );
				
				pPlayer->SendMsg( &optionMessage, optionMessage.GetSize() );
				break;
			}
			case MP_ITEM_HOUSE_RESTORED:
			{
				// 080526 LUJ, �������� �������� 0���� ȹ���� ��� �α׸� �����ϵ��� ��
				if(		pInfo->Stack &&
					!	item->Durability )
				{
					SYSTEMTIME time = { 0 };
					GetLocalTime( &time );

					char buffer[ MAX_PATH ] = { 0 };
					sprintf(
						buffer,
						"log/Zero.Qty.Item.%04d%02d%02d.log",
						time.wYear,
						time.wMonth,
						time.wDay );

					FILE* file = fopen( buffer, "r" );

					if( ! file )
					{
						file = fopen( buffer, "a" );

						if( file )
						{
							const char* title = "Time\tPlayer\tItem\tDBIdx\tMap\n";

							fwrite(
								title,
								sizeof( *title ),
								strlen( title ),
								file );
						}
					}

					fclose( file );
					file = fopen( buffer, "a" );

					if( file )
					{
						sprintf(
							buffer,
							"%02d:%02d:%02d\t%u\t%u\t%u\t%d\n",
							time.wHour,
							time.wMinute,
							time.wSecond,
							pPlayer->GetID(),
							item->wIconIdx,
							item->dwDBIdx,
							g_pServerSystem->GetMapNum() );

						fwrite(
							buffer,
							sizeof( *buffer ),
							strlen( buffer ),
							file );

						fclose( file );
					}
				}
				break;
			}
			case MP_ITEM_DIVIDE_ACK:
			{
				ITEMBASE* const originalItem = pSlot->GetItemInfoAbs(
					POSTYPE(pArrayInfo->dwFurnisherIdx));

				if(0 == originalItem)
				{
					break;
				}

				if(ITEMBASE* const dividedItem = pSlot->GetItemInfoAbs(item->Position))
				{
					dividedItem->nRemainSecond = originalItem->nRemainSecond;
					item->ItemParam = originalItem->dwDBIdx;

					g_DB.FreeMiddleQuery(
						0,
						0,
						"EXEC DBO.MP_ITEM_DIVIDE_UNSEALED %d, %d",
						originalItem->dwDBIdx,
						dividedItem->dwDBIdx);
				}

				break;
			}
		}

		// 090206 LUJ, DB�� UPDATE ������ �ϴ� ��쿡�� ���� ����� Durability �׸�
		//			����ȴ�. �� ������ UPDATE�� �� ��쿡�� �� �������� ���� ������
		//			�� �� ����. �ùٸ� �α� �ۼ��� ���� �ߺ� �������� ��� ItemParam
		//			�ʵ忡 ���� ������ �����صд�.

		//090624 pdy ��Ÿ�� �������� ItemParam�� ���λ���Ͽ�, LogItemMoney�� addedItemCount�� ���������̴� ���� ����
		const DURTYPE addedItemCount = (ITEM_KIND_COOLTIME == pInfo->SupplyType)? item->Durability : item->Durability - item->ItemParam;

		if(pArrayInfo->wType == eLog_ItemObtainHouseRestore)
		{
			LogItemMoney(
				pArrayInfo->dwFurnisherIdx,
				"*HS_RESTORED",
				pPlayer->GetID(),
				pPlayer->GetObjectName(),
				eLogitemmoney( pArrayInfo->wType ),
				0,
				pPlayer->GetMoney(eItemTable_Inventory),
				ChangeMoney * addedItemCount,
				item->wIconIdx,
				item->dwDBIdx,
				0,
				item->Position,
				addedItemCount,
				0 );

			// ����â�� ������������ �α�
			Log_Housing(pPlayer->GetUserID(), pPlayer->GetObjectName(), eHouseLog_UnKepp, item->dwDBIdx, item->wIconIdx, 0,
				0, 0, "", 0, 0);
		}
		else
		{
			LogItemMoney(
				pArrayInfo->dwFurnisherIdx,
				"",
				pPlayer->GetID(),
				pPlayer->GetObjectName(),
				eLogitemmoney( pArrayInfo->wType ),
				0,
				pPlayer->GetMoney(eItemTable_Inventory),
				ChangeMoney * addedItemCount,
				item->wIconIdx,
				item->dwDBIdx,
				0,
				item->Position,
				addedItemCount,
				0 );
		}
	}

	pPlayer->SendMsg( &pArrayInfo->ItemArray, pArrayInfo->ItemArray.GetSize() );

	ITEMMGR->Free(pPlayer, pArrayInfo);
}


// 071211 KTH -- CPlayer �߰�
unsigned int CItemManager::GetTotalEmptySlotNum(CItemSlot* pSlot, CPlayer* pPlayer)
{
	POSTYPE startPos = pSlot->GetStartPos();
	POSTYPE EndPos = startPos +  pSlot->GetSlotNum();//(pSlot->GetSlotNum() - wInventoryExpansion);
	POSTYPE i = 0;

	unsigned int totalemptyNum = 0;
	for( i = startPos ; i < EndPos ; ++i )
	{
		if(pSlot->IsEmpty(i)) totalemptyNum++;
	}

	return totalemptyNum;
}

MP_PROTOCOL_ITEM  CItemManager::Transmog(CPlayer *player, const MSG_ITEM_TRANSMOG_SYN *message , eLogitemmoney& log)
{
	const DWORD		sourceIndex			= message->wItemIdx;
	const DWORD		materialIndex		= message->wMaterialItemIdx;
	const DWORD		sacrificeIndex		= message->wSacrificeItemIdx;
	const POSTYPE	sourcePosition		= message->ItemPos;
	const POSTYPE	materialPosition	= message->MaterialItemPos;
	const POSTYPE	sacrificePosition	= message->SacrificeItemPos;
	const DWORD		sourceDBIdx = message->wItemDBIdx;
	const DWORD		materialDBIdx = message->wMaterialItemDBIdx;
	const DWORD		sacrificeDBIdx = message->wSacrificeItemDBIdx;
	const DWORDEX moneyBefore = player->GetMoney();
	if( !	player																				||
		!	CHKRT->ItemOf(player, sourcePosition, sourceIndex, 0,0, CB_EXIST|CB_ICONIDX)		||
		!	CHKRT->ItemOf(player, materialPosition, materialIndex, 0,0, CB_EXIST|CB_ICONIDX)	||
		!	CHKRT->ItemOf(player, sacrificePosition, sacrificeIndex, 0,0, CB_EXIST|CB_ICONIDX)	||
		!	player->IsInventoryPosition( sourcePosition ) )
	{
		log = eLog_ItemTransmogHacking;
		return MP_ITEM_TRANSMOG_NACK;
	}
	
	const ITEM_INFO* sourceInfo	= GetItemInfo( sourceIndex );
	const ITEM_INFO* materialInfo = GetItemInfo( materialIndex) ;
	const ITEM_INFO* sacrificeInfo	= GetItemInfo( sacrificeIndex );

	if( !	sourceInfo					||
		!	materialInfo				||
		!	sacrificeInfo				||
			sourceInfo->Stack			||
			sacrificeInfo->Stack		||
		!	IsDupItem( materialIndex ))
	{
		log = eLog_ItemTransmogHacking;
		return MP_ITEM_TRANSMOG_NACK;
	}

	CItemSlot* inventory = player->GetSlot( eItemTable_Inventory );

	if(!inventory)
	{
		log = eLog_ItemTransmogHacking;
		return MP_ITEM_TRANSMOG_NACK;
	}
	
	const ITEMBASE*	source	= inventory->GetItemInfoAbs( sourcePosition );

	if(!source || source->dwDBIdx != sourceDBIdx || source->wIconIdx != sourceIndex)
	{
		log = eLog_ItemTransmogHacking;
		return MP_ITEM_TRANSMOG_NACK; 
	}
	
	const ITEMBASE* material = inventory->GetItemInfoAbs( materialPosition );

	if(!material || material->dwDBIdx != materialDBIdx || material->wIconIdx != materialIndex)
	{
		log = eLog_ItemTransmogHacking;
		return MP_ITEM_TRANSMOG_NACK; 
	}

	if(material->wIconIdx == 60000808 || material->wIconIdx == 60000809)
	{
		const MONEYTYPE transmogCost = g_CGameResourceManager.GetTransmogCost(
			*sourceInfo);

		if( player->SetMoney( transmogCost, MONEY_SUBTRACTION, MF_NOMAL, eItemTable_Inventory, eMoneyLog_Transmog ) != transmogCost )
		{
			log = eLog_ItemTransmogNotEnoughMoney;
			return MP_ITEM_TRANSMOG_NACK;
		}

		



		const ITEMBASE* sourceItem = inventory->GetItemInfoAbs( sourcePosition );

		ITEM_OPTION option = { 0 };
		const ITEM_OPTION& previousOption = GetOption( *sourceItem );
			DWORD ItemIdxBackup;

		if(previousOption.mTransmogIdx)
		{
			ItemIdxBackup = previousOption.mTransmogIdx;
		}
		else {
			return MP_ITEM_TRANSMOG_NACK; 
		}


		option.mReinforce = previousOption.mReinforce;
		option.mMix    = previousOption.mMix;
		option.mDrop    = previousOption.mDrop;
		option.mEnchant = previousOption.mEnchant;
		option.mItemDbIndex = previousOption.mItemDbIndex;

	
		RemoveOption(previousOption.mItemDbIndex);
		AddOption(option);

	

		ITEM_INFO* itemBackup	= GetItemInfo( ItemIdxBackup );
		if(!itemBackup)
		{
			return MP_ITEM_TRANSMOG_NACK; 
		}

		
		ItemTransmogReverse(option);
		InsertReversedTransmogItem(player->GetUserID(),ItemIdxBackup);
		
		if(materialInfo->Stack && material->Durability > 1)
		{
			const DURTYPE durablity = material->Durability - 1;
			if( EI_TRUE != inventory->UpdateItemAbs(
				player,
				material->Position,
				material->dwDBIdx,
				material->wIconIdx,
				material->Position,
				material->QuickPosition,
				durablity ) )
			{
				log = eLog_ItemTransmogHacking;
				return MP_ITEM_TRANSMOG_NACK; 
			}

			ItemUpdateToDB(
				player->GetID(),
				material->dwDBIdx,
				material->wIconIdx,
				durablity,
				material->Position,
				material->QuickPosition );
		}
		else 
		{
			if( EI_TRUE != inventory->DeleteItemAbs(
				player,
				material->Position,
				0 ) )
			{
				log = eLog_ItemTransmogHacking;
				return MP_ITEM_TRANSMOG_NACK; 
			}
			
			ItemDeleteToDB(materialDBIdx);
		}
		
		MSG_ITEM_TRANSMOG_ACK m;
		m.Category		= MP_ITEM;
		m.Protocol		= MP_ITEM_TRANSMOG_SUCCESS_ACK;
		m.TargetPos		= sourcePosition;
		m.OptionInfo	= option;

		player->SendMsg( &m, sizeof( m ) );
		
		
		

		LogItemMoney(
					player->GetID(),
					player->GetObjectName(),
					0,
					"*transmog",
					eLog_ItemTransmogMaterialRemoved, 
					moneyBefore,
					player->GetMoney(),
					0,
					material->wIconIdx,
					material->dwDBIdx,
					materialPosition,
					0,
					1,
					0 );

		return MP_ITEM_TRANSMOG_SUCCESS_ACK;

	}
	else 
	{

		const ITEMBASE*	sacrifice	= inventory->GetItemInfoAbs( sacrificePosition );

		if(!sacrifice || sacrifice->dwDBIdx != sacrificeDBIdx || sacrifice->wIconIdx != sacrificeIndex)
		{
			log = eLog_ItemTransmogHacking;
			return MP_ITEM_TRANSMOG_NACK; 
		}


		if((sacrificeInfo->dwUseTime > 0 || sacrificeInfo->wSeal == 1) || (sourceInfo->dwUseTime > 0 || sourceInfo->wSeal == 1))
		{
			log = eLog_ItemTransmogItemInvalid;
			return MP_ITEM_TRANSMOG_NACK; 
		}
		if(sacrificeInfo->LimitGender > 0)
		{
			log = eLog_ItemTransmogItemInvalid;
			return MP_ITEM_TRANSMOG_NACK; 
		}


		if(material->wIconIdx != 60000017)
		{
			log = eLog_ItemTransmogHacking;
			return MP_ITEM_TRANSMOG_NACK; 
		}

		if (sourceInfo->Part3DType != message->PartTypeItem || sacrificeInfo->Part3DType != message->PartTypeSacrifice
			|| sourceInfo->EquipType != message->EquipTypeItem || sacrificeInfo->EquipType != message->EquipTypeSacrifice
			|| sourceInfo->EquipSlot != message->EquipSlotItem || sacrificeInfo->EquipSlot != message->EquipSlotSacrifice)
		{
			log = eLog_ItemTransmogHacking;
			return MP_ITEM_TRANSMOG_NACK; 
		}
		
		ITEM_OPTION option = GetOption( *source );
		option.mItemDbIndex	= source->dwDBIdx;
		option.mTransmogIdx = sacrificeIndex;
		 
		ItemTransmogInsert(option,sacrifice->dwDBIdx,player->GetID());
		AddOption(option);

		const MONEYTYPE transmogCost = g_CGameResourceManager.GetTransmogCost(
			*sourceInfo);

		if( player->SetMoney( transmogCost, MONEY_SUBTRACTION, MF_NOMAL, eItemTable_Inventory, eMoneyLog_Transmog ) != transmogCost )
		{
			log = eLog_ItemTransmogNotEnoughMoney;
			return MP_ITEM_TRANSMOG_NACK;
		}

	

		if( EI_TRUE != inventory->DeleteItemAbs(
			player,
			sacrifice->Position,
			0 ) )
		{
			log = eLog_ItemTransmogHacking;
			return MP_ITEM_TRANSMOG_NACK; 
		}
		
		ItemDeleteToDB(sacrificeDBIdx);
		ItemOptionDelete(sacrificeDBIdx);
		RemoveOption(sacrificeDBIdx);
		
		if(materialInfo->Stack && material->Durability > 1)
		{
			const DURTYPE durablity = material->Durability - 1;
			if( EI_TRUE != inventory->UpdateItemAbs(
				player,
				material->Position,
				material->dwDBIdx,
				material->wIconIdx,
				material->Position,
				material->QuickPosition,
				durablity ) )
			{
				log = eLog_ItemTransmogHacking;
				return MP_ITEM_TRANSMOG_NACK; 
			}

			ItemUpdateToDB(
				player->GetID(),
				material->dwDBIdx,
				material->wIconIdx,
				durablity,
				material->Position,
				material->QuickPosition);
		}
		else 
		{
			if( EI_TRUE != inventory->DeleteItemAbs(
				player,
				material->Position,
				0 ) )
			{
				log = eLog_ItemTransmogHacking;
				return MP_ITEM_TRANSMOG_NACK; 
			}
			
			ItemDeleteToDB(materialDBIdx);
		}

		MSG_ITEM_TRANSMOG_ACK m;
		m.Category		= MP_ITEM;
		m.Protocol		= MP_ITEM_TRANSMOG_SUCCESS_ACK;
		m.TargetPos		= sourcePosition;
		m.OptionInfo	= option;

		player->SendMsg( &m, sizeof( m ) );
		
		
		LogItemMoney(
					player->GetID(),
					player->GetObjectName(),
					0,
					"*transmog",
					eLog_ItemTransmogDummyRemoved, 
					moneyBefore,
					player->GetMoney(),
					0,
					sacrificeInfo->ItemIdx,
					sacrificeDBIdx,
					sacrificePosition,
					0,
					1,
					0 );

		LogItemMoney(
					player->GetID(),
					player->GetObjectName(),
					0,
					"*transmog",
					eLog_ItemTransmogMaterialRemoved, 
					moneyBefore,
					player->GetMoney(),
					0,
					material->wIconIdx,
					material->dwDBIdx,
					materialPosition,
					0,
					1,
					0 );

		return MP_ITEM_TRANSMOG_SUCCESS_ACK;
	}
}



WORD CItemManager::CheckExtraSlot(CPlayer * pPlayer, CItemSlot * pSlot, DWORD whatItemIdx, DURTYPE whatItemNum, WORD * EmptyCellPos, WORD & EmptyCellNum)
{
	DURTYPE EstItemNum = whatItemNum;
	POSTYPE startPos = pSlot->GetStartPos();
	//POSTYPE EndPos = startPos +  pSlot->GetSlotNum();
	// 071212 KTH -- 
	POSTYPE EndPos = startPos + pSlot->GetSlotNum();//(pSlot->GetSlotNum() - wInventoryExpansion);
	POSTYPE i = 0;
	EmptyCellNum = 0;

	if(!IsDupItem(whatItemIdx))
	{
		for( i = startPos ; i < EndPos ; ++i )
		{
			if( EstItemNum == 0 ) break;
			if(pSlot->IsEmpty(i))
			{
				--EstItemNum;
				EmptyCellPos[EmptyCellNum++] = i;
			}
		}
	}
	else
	{
		// 091215 ONS ������Stack�ִ���� �����ۺ��� ������ ������ ó���Ѵ�.
		const WORD wItemStackNum = GetItemStackNum( whatItemIdx );
		for( i = startPos ; i < EndPos ; ++i )
		{
			if( EstItemNum == 0 ) break;
			const ITEMBASE * pItemBase = pSlot->GetItemInfoAbs(i);
			if( !pSlot->IsEmpty(i) && !pSlot->IsLock(i) &&
				pItemBase->wIconIdx == whatItemIdx &&
				pItemBase->Durability < wItemStackNum )
			{
				if( pItemBase->Durability + EstItemNum > wItemStackNum )
					EstItemNum = EstItemNum + pItemBase->Durability - wItemStackNum;
				else
					EstItemNum = 0;
				EmptyCellPos[EmptyCellNum++] = i;
			}
		}

		for( i = startPos ; i < EndPos ; ++i )
		{
			if( EstItemNum == 0 ) break;
			if(pSlot->IsEmpty(i))
			{
				if( EstItemNum > wItemStackNum )
					EstItemNum -= wItemStackNum;
				else
					EstItemNum = 0;
				EmptyCellPos[EmptyCellNum++] = i;
			}
		}
	}

	return (WORD)(whatItemNum - EstItemNum);
}

// rt : ����o�����Ϩ�������I8����I ��ii ��uo AO�ˡ�A �����ϩ���uo
WORD CItemManager::GetCanBuyNumInSpace(CPlayer * pPlayer, CItemSlot * pSlot, DWORD whatItemIdx, DURTYPE whatItemNum, WORD * EmptyCellPos, WORD & EmptyCellNum)
{
	DURTYPE EstItemNum = whatItemNum;
	POSTYPE startPos = pSlot->GetStartPos();
	POSTYPE EndPos = startPos + pSlot->GetSlotNum();//(pSlot->GetSlotNum() - wInventoryExpansion);
	POSTYPE i = 0;
	EmptyCellNum = 0;

	if(!IsDupItem(whatItemIdx))
	{
		for( i = startPos ; i < EndPos ; ++i )
		{
			if( EstItemNum == 0 ) break;
			if(pSlot->IsEmpty(i))
			{
				--EstItemNum;
				EmptyCellPos[EmptyCellNum++] = i;
			}
		}
	}
	else
	{
		// 091215 ONS ������Stack�ִ���� �����ۺ��� ������ ������ ó���Ѵ�.
		const WORD wItemStackNum = GetItemStackNum( whatItemIdx );
		for( i = startPos ; i < EndPos ; ++i )
		{
			if( EstItemNum == 0 ) break;
			const ITEMBASE * pItemBase = pSlot->GetItemInfoAbs(i);
			if( !pSlot->IsEmpty(i) && !pSlot->IsLock(i) &&
				pItemBase->wIconIdx == whatItemIdx &&
				pItemBase->Durability < wItemStackNum &&
				eITEM_TYPE_UNSEAL != pItemBase->nSealed)
			{
				if( pItemBase->Durability + EstItemNum > wItemStackNum )
					EstItemNum = EstItemNum + pItemBase->Durability - wItemStackNum;
				else
					EstItemNum = 0;
				EmptyCellPos[EmptyCellNum++] = i;
			}
		}

		for( i = startPos ; i < EndPos ; ++i )
		{
			if( EstItemNum == 0 ) break;
			if(pSlot->IsEmpty(i))
			{
				if( EstItemNum > wItemStackNum )
					EstItemNum -= wItemStackNum;
				else
					EstItemNum = 0;
				EmptyCellPos[EmptyCellNum++] = i;
			}
		}

		// 091215 ONS �ټ��� ������ ����Ұ�� ���� ���Լ��� üũ�Ѵ�.
		DWORD dwUseSlotCount = 0;
		if( whatItemNum%wItemStackNum )
		{
			dwUseSlotCount  = whatItemNum/wItemStackNum + 1;
		}
		else
		{
			dwUseSlotCount  = whatItemNum/wItemStackNum;
		}

		if( dwUseSlotCount > EmptyCellNum )
		{
			return 0;
		}
	}

	return (WORD)(whatItemNum - EstItemNum);
}
WORD CItemManager::GetCanBuyNumInMoney(CPlayer * pPlayer, WORD buyNum, MONEYTYPE Price)
{
	MONEYTYPE money = pPlayer->GetMoney();

	if( money > (MONEYTYPE)(buyNum * Price) )
	{
		return buyNum;
	}
	else
	{
		// 080411 LUJ, 0 ������ ���ܸ� ���� ���� ������ ���� üũ
		return Price ? WORD( money / Price ) : buyNum;
	}	
}

WORD CItemManager::GetCanBuyNumInFishingPoint(CPlayer* pPlayer, WORD buyNum, MONEYTYPE Point)
{
	MONEYTYPE PlayerPoint = (MONEYTYPE)pPlayer->GetFishPoint();

	if(PlayerPoint > (MONEYTYPE)(buyNum * Point))
	{
		return buyNum;
	}
	else
	{
		return Point ? WORD(PlayerPoint / Point) : buyNum;
	}
}

// desc_hseos_����ý���_01
// S ����ý��� �߰� added by hseos 2007.09.11
ITEM_INFO* CItemManager::GetUseItemInfo(CPlayer* pPlayer, WORD TargetPos, DWORD wItemIdx)
{
	if(pPlayer->GetState() == eObjectState_Die)
		return NULL;
	if(!CHKRT->ItemOf(pPlayer, TargetPos, wItemIdx,0,0,CB_EXIST|CB_ICONIDX))
		return NULL;

	if(pPlayer->GetState() == eObjectState_Immortal )
		OBJECTSTATEMGR_OBJ->EndObjectState( pPlayer, eObjectState_Immortal );

	CItemSlot * pSlot = pPlayer->GetSlot(TargetPos);
	if(pSlot == NULL)
	{
		return NULL;
	}

	const ITEMBASE * pItemBase = pSlot->GetItemInfoAbs(TargetPos);
	if (pItemBase == NULL)
	{
		return NULL;
	}

	ITEM_INFO * pItemInfo = GetItemInfo(wItemIdx);
	if (pItemInfo == NULL)
	{
		return NULL;
	}

	if (pItemBase->wIconIdx != pItemInfo->ItemIdx)
	{
		return NULL;
	}

	return pItemInfo;
}

eItemUse_Err CItemManager::UseItem(CPlayer* pPlayer, POSTYPE TargetPos, DWORD wItemIdx)
{
	if(pPlayer->GetState() == eObjectState_Die)
		return eItemUseErr_Err;

	CItemSlot* const pSlot = pPlayer->GetSlot(
		eItemTable_Inventory);

	if(NULL == pSlot)
	{
		return eItemUseErr_Err;
	}

	const ITEMBASE* const pItem = pSlot->GetItemInfoAbs(
		TargetPos);

	if(NULL == pItem)
	{
		return eItemUseErr_Err;
	}

	const ITEM_INFO* const pItemInfo = GetItemInfo(
		pItem->wIconIdx);

	if(NULL == pItemInfo)
	{
		return eItemUseErr_Err;
	}
	else if(pItemInfo->ItemIdx != wItemIdx)
	{
		return eItemUseErr_Err;
	}

	const AbnormalStatus* const pAbnormalStatus = pPlayer->GetAbnormalStatus();
 
	if( pAbnormalStatus->IsSlip ||
		pAbnormalStatus->IsStone ||
		pAbnormalStatus->IsParalysis || 
		pAbnormalStatus->IsFreezing	||
		pAbnormalStatus->IsUnableUseItem ||
		pAbnormalStatus->IsStun)
		return eltemUseErr_Stun;

	if(pPlayer->GetState() == eObjectState_Immortal )
		OBJECTSTATEMGR_OBJ->EndObjectState( pPlayer, eObjectState_Immortal );

	// LUJ, �������� �Һ��� ��� pItem�� ������ �ʱ�ȭ�ǹǷ�, �̸� �����صξ�� �α� � �� �� �ִ�
	ITEMBASE usedItem = *pItem;

	//�����ʸ�Ʈ �ʿ����� ���빰���� ������ �Ҹ𼺾����� ���Ұ�.
	if(g_pServerSystem->GetMapNum()==GTMAPNUM)
	{
		if(pItemInfo->Category==eItemCategory_Expend)
		{
			if(pItemInfo->SupplyType!=ITEM_KIND_GTHP_RECOVER_INSTANT &&
				pItemInfo->SupplyType!=ITEM_KIND_GTMP_RECOVER_INSTANT &&
				pItemInfo->SupplyType!=ITEM_KIND_GTBUFF_ITEMS)
				return eItemUseErr_Err;
		}
	}
	else if(g_pServerSystem->GetMapNum()!=HOUSINGMAPNUM)
	{
		if(pItemInfo->SupplyType==ITEM_KIND_FURNITURE ||
			pItemInfo->SupplyType==ITEM_KIND_FURNITURE_WALLPAPER ||
			pItemInfo->SupplyType==ITEM_KIND_FURNITURE_FLOORPAPER ||
			pItemInfo->SupplyType==ITEM_KIND_FURNITURE_CEILINGPAPER ||
			pItemInfo->SupplyType==ITEM_KIND_EXTEND_HOUSE)
		{
			// �����뺸
			MSG_DWORD4 msg;
			msg.Category = MP_HOUSE;
			msg.Protocol = MP_HOUSE_USEITEM_NACK;
			msg.dwObjectID = pPlayer->GetID();
			msg.dwData1 = wItemIdx;
			msg.dwData2 = TargetPos;
			msg.dwData3 = pItemInfo->SupplyType;
			msg.dwData4 = eHOUSEERR_DO_HOUSE;
			pPlayer->SendMsg( &msg, sizeof(msg) ) ;

			return eItemUseErr_Err;
		}
	}
	else
	{
		// �Ͽ�¡�ʿ��� �������
		if(pItemInfo->SupplyType==ITEM_KIND_SUMMON_MONSTER ||
			pItemInfo->SupplyType==ITEM_KIND_CHANGESIZE_UPWEIGHT ||
			pItemInfo->SupplyType==ITEM_KIND_CHANGESIZE_DNWEIGHT ||
			pItemInfo->SupplyType==ITEM_KIND_CHANGESIZE_DEFALUT)
		{
			// �����뺸
			MSG_DWORD4 msg;
			msg.Category = MP_HOUSE;
			msg.Protocol = MP_HOUSE_USEITEM_NACK;
			msg.dwObjectID = pPlayer->GetID();
			msg.dwData1 = wItemIdx;
			msg.dwData2 = TargetPos;
			msg.dwData3 = pItemInfo->SupplyType;
			msg.dwData4 = eHOUSEERR_DONOT_HOUSE;
			pPlayer->SendMsg( &msg, sizeof(msg) ) ;

			return eItemUseErr_Err;
		}

		// ���� �ܿ��� ���Ұ�
		if(pItemInfo->SupplyType==ITEM_KIND_FURNITURE ||
			pItemInfo->SupplyType==ITEM_KIND_FURNITURE_WALLPAPER ||
			pItemInfo->SupplyType==ITEM_KIND_FURNITURE_FLOORPAPER ||
			pItemInfo->SupplyType==ITEM_KIND_FURNITURE_CEILINGPAPER ||
			pItemInfo->SupplyType==ITEM_KIND_EXTEND_HOUSE)
		{
			stHouse* pHouse = HOUSINGMGR->GetHouseByChannelID(pPlayer->GetChannelID());
			if(!pHouse || pHouse->HouseInfo.dwOwnerUserIndex!=pPlayer->GetUserID())
			{
				// �����뺸
				MSG_DWORD4 msg;
				msg.Category = MP_HOUSE;
				msg.Protocol = MP_HOUSE_USEITEM_NACK;
				msg.dwObjectID = pPlayer->GetID();
				msg.dwData1 = wItemIdx;
				msg.dwData2 = TargetPos;
				msg.dwData3 = pItemInfo->SupplyType;
				msg.dwData4 = eHOUSEERR_NOTOWNER;
				pPlayer->SendMsg( &msg, sizeof(msg) ) ;

				return eItemUseErr_Err;
			}

			if(pItemInfo->SupplyType==ITEM_KIND_EXTEND_HOUSE)
			{
				int rt = HOUSINGMGR->CheckExtendHouse(pPlayer->GetID());
				if(rt != eHOUSEERR_NONE)
				{
					// �����뺸
					MSG_DWORD4 msg;
					msg.Category = MP_HOUSE;
					msg.Protocol = MP_HOUSE_USEITEM_NACK;
					msg.dwObjectID = pPlayer->GetID();
					msg.dwData1 = wItemIdx;
					msg.dwData2 = TargetPos;
					msg.dwData3 = pItemInfo->SupplyType;
					msg.dwData4 = rt;
					pPlayer->SendMsg( &msg, sizeof(msg) ) ;

					return eItemUseErr_Err;
				}
			}
			else
			{
				if(!HOUSINGMGR->IsDecoMode(pHouse->HouseInfo.dwOwnerUserIndex, pPlayer->GetID()))
				{
					// �����뺸
					MSG_DWORD4 msg;
					msg.Category = MP_HOUSE;
					msg.Protocol = MP_HOUSE_USEITEM_NACK;
					msg.dwObjectID = pPlayer->GetID();
					msg.dwData1 = wItemIdx;
					msg.dwData2 = TargetPos;
					msg.dwData3 = pItemInfo->SupplyType;
					msg.dwData4 = eHOUSEERR_DECOMODE;
					pPlayer->SendMsg( &msg, sizeof(msg) ) ;

					return eItemUseErr_Err;
				}
			}
		}
	}

	// 080917 -- KTH : ItemManager ���������� ������ �������� ����� �� ����.
  	if( SIEGEWARFAREMGR->Is_CastleMap() == FALSE &&
		SIEGEWARFAREMGR->IsSiegeWarfareZone() &&
		SIEGEWARFAREMGR->GetState() > eSiegeWarfare_State_Before )
  	{
  		if( SIEGEWARFAREMGR->IsFobiddenItem(wItemIdx) )
  		{
  			return eItemUseErr_SiegeWarfareForbiddenItem;
  		}
  	}
	else if(pItemInfo->LimitLevel &&
			pItemInfo->LimitLevel > pPlayer->GetLevel())
	{
		return eItemUseErr_Err;
	}

	if(eITEM_TYPE_SEAL == usedItem.nSealed)
	{
		if( pItemInfo->SupplyType == ITEM_KIND_PET )
		{
			LogItemMoney(
				pPlayer->GetID(),
				pPlayer->GetObjectName(),
				0,
				"",
				eLog_ShopItemUnseal,
				pPlayer->GetMoney(),
				0,
				0,
				usedItem.wIconIdx,
				usedItem.dwDBIdx,
				usedItem.Position,
				0,
				usedItem.Durability,
				0);

			pSlot->ForcedUnseal(
				TargetPos);
			PETMGR->HatchPet(
				pPlayer,
				&usedItem);
			return eItemUseUnsealed;
		}

		ITEMBASE* const pItemBase = pSlot->GetItemInfoAbs(
			usedItem.Position);
		pItemBase->nRemainSecond = pItemInfo->dwUseTime;
		pItemBase->nSealed = eITEM_TYPE_UNSEAL;
		
		// LUJ, usedItem�� �� �����̹Ƿ�, ���� �����͸� �ٲ� �Ŀ��� �ٽ� �����ؾ��Ѵ�.
		usedItem = *pItemBase;

		if(eKIND_REALTIME == pItemInfo->nTimeKind)
		{
			UnSealItemByRealTime(
				pPlayer->GetID(),
				usedItem.dwDBIdx,
				pItemInfo->dwUseTime);
		}
		else
		{
			UnSealItemByGameTime(
				pPlayer->GetID(),
				usedItem.dwDBIdx,
				pItemInfo->dwUseTime);
		}

		MSG_ITEM msg;
		msg.Category = MP_ITEM;
		msg.Protocol = MP_ITEM_SHOPITEM_USE_ACK;
		msg.ItemInfo = *pItemBase;
		pPlayer->SendMsg(
			&msg,
			sizeof(msg));

		LogItemMoney(
			pPlayer->GetID(),
			pPlayer->GetObjectName(),
			0,
			"",
			eLog_ShopItemUnseal,
			pPlayer->GetMoney(),
			0,
			0,
			usedItem.wIconIdx,
			usedItem.dwDBIdx,
			usedItem.Position,
			0,
			usedItem.Durability,
			0 );

		return eItemUseUnsealed;
	}


	// ��Ÿ�� ������ �ִ� �������̸� ���� �������� üũ�Ѵ�.
	{
		CoolTimeScript::ItemData::const_iterator it = mCoolTimeScript.mItemData.find( wItemIdx );

		if( mCoolTimeScript.mItemData.end() != it )
		{
			const CoolTimeScript::Unit& unit = it->second;

			if( ! pPlayer->AddCoolTime( unit.mIndex, unit.mTime ) )
			{
				return eItemUseErr_Err;
			}
		}
	}

	switch( pItemInfo->SupplyType )
	{
	case ITEM_KIND_PET_SKILL:					
	case ITEM_KIND_PET_COSTUME:				
	case ITEM_KIND_PET_EQUIP:					
	case ITEM_KIND_PET_EGG:					
	case ITEM_KIND_PET_FOOD:				
	case ITEM_KIND_PET_HP_RECOVER:			
	case ITEM_KIND_PET_MP_RECOVER:			
		{
			if(0 == PETMGR->GetPet(
				pPlayer->GetPetItemDbIndex()))
			{
				return eItemUseErr_Err;
			}
		}
		break;
	case ITEM_KIND_PET_EVOLUTION_PYHSIC_TYPE:	
	case ITEM_KIND_PET_EVOLUTION_MAGIC_TYPE:	
	case ITEM_KIND_PET_EVOLUTION_SUPPORT_TYPE:
		{
			CPet* const petObject = PETMGR->GetPet(
				pPlayer->GetPetItemDbIndex());

			if(0 == petObject)
			{
				return eItemUseErr_Err;
			}
			else if( petObject->GetObjectInfo().Level != PETMGR->GetGradeUpLevel( petObject->GetObjectInfo().Grade ) )
			{
				return eItemUseErr_Err;
			}
			else if(petObject->GetObjectInfo().Type != ePetType_Basic &&
				petObject->GetObjectInfo().Type != pItemInfo->SupplyType - ITEM_KIND_PET_EVOLUTION_PYHSIC_TYPE + 1 )
			{
				return eItemUseErr_Err;
			}
		}
		break;

		// 080826 LYW --- ItemManager : ���������� ����� ���ͽõ� ó���� �߰��Ѵ�.
  	case ITEM_KIND_SIEGEWARFARE_WATERSEED :
  		{
			// 081226 LYW --- ItemManager : ĳ����Ʈ ��ȯ ������ �޴´�.
			SIEGERECALLMGR->RequestKillCountToDB() ;
			// 081009 LYW --- ItemManager : ���� �ҽ� ����.
			WORD wResult = SIEGEWARFAREMGR->MC_InstallWaterSeed(pPlayer);

  			if( !wResult )
  			{
				MSG_DWORD2 msg ;

				msg.dwData1 = g_pServerSystem->GetMapNum() ;
				msg.dwData2	= pPlayer->GetID() ;

				SetProtocol( &msg, MP_SIEGEWARFARE, MP_SIEGEWARFARE_USE_WATERSEED_ACK) ;
				g_Network.Broadcast2AgentServer((char*)&msg, sizeof(MSG_DWORD2)) ;
  			}
  			else
  			{
  				MSG_WORD msg;
  				SetProtocol( &msg, MP_SIEGEWARFARE, MP_SIEGEWARFARE_USE_WATERSEED_NACK);
				msg.wData = wResult;
  				g_Network.Broadcast2AgentServer((char*)&msg, sizeof(MSG_WORD));
  
  				// ���ͽõ� ����ڿ��� ���ͽõ� ��� Failed �޽����� �����Ѵ�.
  				pPlayer->SendMsg( &msg, sizeof(MSG_WORD) ) ;

				// 081106 LYW --- Player : ������ ����� �����ϸ�, �������� ��� �� ��Ÿ�� ������ �����Ѵ�.
				CoolTimeScript::ItemData::const_iterator it = mCoolTimeScript.mItemData.find( wItemIdx );

				if( mCoolTimeScript.mItemData.end() != it )
				{
					const CoolTimeScript::Unit& unit = it->second;
				
					pPlayer->RemoveCoolTime(unit.mIndex);
				}
  
  				return eItemUseErr_WaterseedErr ;
  			}
  		}
  		break ;

	case ITEM_KIND_LEVEL_UP:
		{
			if(pPlayer->GetLevel() >= MAX_CHARACTER_LEVEL_NUM)
				return eItemUseErr_Err;

			EXPTYPE pointForLevel = 0;
			LEVELTYPE diff = (LEVELTYPE)pItemInfo->SupplyValue;

			if(diff==0 || diff >= MAX_CHARACTER_LEVEL_NUM)
				return eItemUseErr_Err;

			for(LEVELTYPE i=0; i<diff; ++i)
			{
				pointForLevel = pointForLevel + GAMERESRCMNGR->GetMaxExpPoint(pPlayer->GetLevel()+i);
			}
			pPlayer->SetPlayerExpPoint(pointForLevel);
			pPlayer->SetPlayerExpPoint(1);					
			
			// YH
			CharacterHeroInfoUpdate(pPlayer);
			CharacterTotalInfoUpdate(pPlayer);		
		}
		break;

	case ITEM_KIND_GUILDPOINT_UP:
		{
			DWORD dwGuildIdx = pPlayer->GetGuildIdx();
			if(dwGuildIdx == 0)
				return eItemUseErr_Err;

			// �������Ʈ ����
			GuildAddScore(dwGuildIdx, (float)pItemInfo->SupplyValue);
			InsertLogGuildScore(dwGuildIdx, (float)pItemInfo->SupplyValue, eGuildLog_ScoreAddByItem );

			MSGBASE message;
			ZeroMemory( &message, sizeof( message ) );
			message.Category	= MP_GUILD;
			message.Protocol	= MP_GUILD_SCORE_UPDATE_TO_MAP;
			message.dwObjectID	= dwGuildIdx;

			g_Network.Broadcast2AgentServer( ( char* )&message, sizeof( message ) );
		}
		break;

	case ITEM_KIND_FAMILYPOINT_UP:
		{
			if(pPlayer->GetFamilyIdx() == 0)
				return eItemUseErr_Err;

			// �йи�����Ʈ ����
			g_csFamilyManager.SRV_ProcessHonorPointAdd(pPlayer, pItemInfo->SupplyValue);
		};
		break;

	case ITEM_KIND_EXP_UP:
		{
			if(pPlayer->GetLevel() >= MAX_CHARACTER_LEVEL_NUM)
				return eItemUseErr_Err;

			EXPTYPE point = pItemInfo->SupplyValue;

			pPlayer->SetPlayerExpPoint(pPlayer->GetPlayerExpPoint() + point);
			InsertLogExp( eExpLog_GetbyItem, pPlayer->GetID(), pPlayer->GetLevel(), point, pPlayer->GetPlayerExpPoint(),
				0, 0, 0 );
			
			// YH
			CharacterHeroInfoUpdate(pPlayer);
			CharacterTotalInfoUpdate(pPlayer);		
		}
		break;

	case ITEM_KIND_PCROOMPOINT_UP:
		{
			if(0 == PCROOMMGR->GetGameRoomIndex(pPlayer->GetID()))
				return eItemUseErr_Err;

			DWORD point = pItemInfo->SupplyValue;
			PCROOMMGR->SetPoint(pPlayer->GetID(), PCROOMMGR->GetPoint(pPlayer->GetID()) + point);

			// ����� ����Ʈ ����
			MSG_DWORD2 msg;
			ZeroMemory( &msg, sizeof(msg) );
			msg.Category = MP_PCROOM;
			msg.Protocol = MP_PCROOM_CHANGE_POINT;
			msg.dwData1 = pPlayer->GetID();
			msg.dwData2 = PCROOMMGR->GetPoint(pPlayer->GetID());

			pPlayer->SendMsg( &msg, sizeof(msg) );
		}
		break;

	case ITEM_KIND_FISHINGEXP_UP:
		{
			EXPTYPE point = pItemInfo->SupplyValue;
			pPlayer->SetFishingExp(pPlayer->GetFishingExp() + point);

			MSG_DWORDEX2 msgFishingExp;
			msgFishingExp.Category = MP_FISHING;
			msgFishingExp.Protocol = MP_FISHING_EXP_ACK;
			msgFishingExp.dwObjectID = pPlayer->GetID();
			msgFishingExp.dweData1    = pPlayer->GetFishingLevel();
			msgFishingExp.dweData2   = pPlayer->GetFishingExp();
			pPlayer->SendMsg( &msgFishingExp, sizeof( msgFishingExp ) );

			Log_Fishing(
				pPlayer->GetID(),
				eFishingLog_GetExpFromItem,
				0,
				0,
				0,
				0,
				pPlayer->GetFishingExp(),
				pPlayer->GetFishingLevel() );
		}
		break;

	case ITEM_KIND_FISHPOINT_UP:
		{
			DWORD point = pItemInfo->SupplyValue;
			pPlayer->SetFishPoint(pPlayer->GetFishPoint() + point);

			MSG_DWORDEX msgFishPoint;
			msgFishPoint.Category = MP_FISHING;
			msgFishPoint.Protocol = MP_FISHING_POINT_ACK;
			msgFishPoint.dweData	= pPlayer->GetFishPoint();
			pPlayer->SendMsg(&msgFishPoint, sizeof(msgFishPoint));

			// 080808 LUJ, ���� ����ġ/���� �߰�
			FishingData_Update(pPlayer->GetID(), pPlayer->GetFishingLevel(), pPlayer->GetFishingExp(), pPlayer->GetFishPoint());
			Log_Fishing(
				pPlayer->GetID(),
				eFishingLog_GetPointFromItem,
				point,
				pPlayer->GetFishPoint(),
				0,
				0,
				pPlayer->GetFishingExp(),
				pPlayer->GetFishingLevel() );
				}
				break;

	case ITEM_KIND_COOKLEVEL_UP:
		{
			WORD wCookLevel = pPlayer->GetCookLevel();

			if(wCookLevel >= MAX_COOKING_LEVEL)
				return eItemUseErr_Err;
			
			pPlayer->SetCookLevel(wCookLevel + 1);
			pPlayer->SetCookCount(0);
			pPlayer->SetEatCount(0);
			pPlayer->SetFireCount(0);

			MSG_DWORD4 msg;
			msg.Category = MP_COOK;
			msg.Protocol = MP_COOK_STATE;
			msg.dwObjectID = pPlayer->GetID();
			msg.dwData1 = pPlayer->GetCookLevel();
			msg.dwData2 = pPlayer->GetCookCount();
			msg.dwData3 = pPlayer->GetEatCount();
			msg.dwData4 = pPlayer->GetFireCount();
			pPlayer->SendMsg(&msg, sizeof(msg));
		}
		break;

	case ITEM_KIND_DECOUSEPOINT_UP:
		{
			if(HOUSINGMAPNUM != g_pServerSystem->GetMapNum())
				return eItemUseErr_Err;

			stHouse* pHouse = HOUSINGMGR->GetHouseByChannelID(pPlayer->GetChannelID());
			if(! pHouse)
				return eItemUseErr_Err;

			pHouse->HouseInfo.dwDecoUsePoint += pItemInfo->SupplyValue;
			HouseUpdate(&pHouse->HouseInfo);

			MSG_DWORD2 msg;
			msg.Category = MP_HOUSE;
			msg.Protocol = MP_HOUSE_BONUS_ACK;
			msg.dwObjectID = pPlayer->GetID();
			msg.dwData1 = 0;
			msg.dwData2 = pHouse->HouseInfo.dwDecoUsePoint;
			PACKEDDATA_OBJ->QuickSend( pPlayer, &msg, sizeof( msg ) );			
		}
		break;

	case ITEM_KIND_PKPOINT_UP:
		{
			FAMETYPE addBadFame = pItemInfo->SupplyValue;
			pPlayer->SetBadFame( pPlayer->GetBadFame() + addBadFame );

			BadFameCharacterUpdate( pPlayer->GetID(), pPlayer->GetBadFame() );

			MSG_FAME msg;
			msg.Category = MP_CHAR;
			msg.Protocol = MP_CHAR_BADFAME_NOTIFY;
			msg.dwObjectID = pPlayer->GetID();
			msg.Fame = pPlayer->GetBadFame();

			PACKEDDATA_OBJ->QuickSend(
				pPlayer,
				&msg, sizeof(msg));
		}
		break;

	case ITEM_KIND_PKPOINT_DN:
		{
			FAMETYPE reduceBadFame = pItemInfo->SupplyValue;
			
			if(pPlayer->GetBadFame() < reduceBadFame)
				pPlayer->SetBadFame(0);
			else
				pPlayer->SetBadFame( pPlayer->GetBadFame() - reduceBadFame );

			BadFameCharacterUpdate( pPlayer->GetID(), pPlayer->GetBadFame() );
			
			MSG_FAME msg;
			msg.Category = MP_CHAR;
			msg.Protocol = MP_CHAR_BADFAME_NOTIFY;
			msg.dwObjectID = pPlayer->GetID();
			msg.Fame = pPlayer->GetBadFame();

			PACKEDDATA_OBJ->QuickSend(
				pPlayer,
				&msg, sizeof(msg));
		}
		break;
	case ITEM_KIND_PET:
		{
			PETMGR->UsePetItem(
				pPlayer,
				usedItem.dwDBIdx);
			return eItemUseSuccess;
		}
		break;
	case ITEM_KIND_RESET_REGIDENT_REGIST:
		{
			if(FALSE == pPlayer->GetResidentRegistInfo()->bIsValid ||
				pPlayer->IsPKMode())
			{
				return eItemUseErr_NotRegist;
			}
		}
		break;
	case ITEM_KIND_RETURN_SCROLL:
		{
			if (g_csDateManager.IsChallengeZoneHere())
			{
				return eItemUseErr_Err;
			}
		}
		break;
	case ITEM_KIND_SUMMON_MONSTER:
		{
			if(m_vecMonSummonGroupInfo.empty())
			{
				return eItemUseErr_ForbiddenSummon;
			}
			else if(stMonSummonGroupInfo* pSummonGroup = m_vecMonSummonGroupInfo.at(pItemInfo->SupplyValue))
			{
				if(true == pSummonGroup->bNotForbidden)
				{
					break;
				}
				else if(m_vecMonSummonForbiddenMap.end() != m_vecMonSummonForbiddenMap.find(g_pServerSystem->GetMapNum()))
				{
					return eItemUseErr_ForbiddenSummon;
				}
			}
		}
		break;
	case ITEM_KIND_SUMMON_NPC:
		{
			if(m_mapNpcSummonInfo.empty())
			{
				return eItemUseErr_ForbiddenSummon;
			}
			else if(m_vecNpcSummonForbiddenMap.end() != m_vecNpcSummonForbiddenMap.find(g_pServerSystem->GetMapNum()))
			{
				return eItemUseErr_ForbiddenSummon;
			}
		}
		break;
	case ITEM_KIND_ADDRECIPE:
		{
			const int nRecipeLevel = COOKMGR->GetCookLevelFromRecipe(pItemInfo->SupplyValue);

			if(pPlayer->GetCookLevel()<eCOOKLEVEL4 || pPlayer->GetCookLevel() < nRecipeLevel)
			{
				return eItemUseErr_InvalidCookGrade;
			}

			const int nCheck = pPlayer->CanAddRecipe(pItemInfo->SupplyValue);

			if(ERR_FULLLEANRN_RECIPE == nCheck)
			{
				return eItemUseErr_AlreadyLearnRecipe;
			}
			else if(ERR_ALREADYLEARN_RECIPE == nCheck)
			{
				return eItemUseErr_FullLearnRecipe;
			}
		}
		break;
	case ITEM_KIND_FOODSKILLBOOK:
		{
			const int nValue = pItemInfo->SupplyValue;

			if(eCOOKLEVEL2 > nValue ||
				eCOOKLEVEL4 < nValue)
			{
				return eItemUseErr_Err;
			}
			else if(FALSE == COOKMGR->CanLevelUp( pPlayer, WORD( nValue ) ) )
			{
				return eItemUseErr_InvalidCookGrade;
			}
		}
		break;
	case ITEM_KIND_FOOD:
		{
			if(eCOOKLEVEL4 != pItemInfo->SupplyValue)
			{
				break;
			}
			else if(pPlayer->GetLevel() >= LV4FOOD_EAT_LEVEL)
			{
				break;
			}
			
			return eItemUseErr_LowerLevelForEat;
		}
		break;
	case ITEM_KIND_FISHING_FISH:
		{
			return eItemUseErr_Err;
		}
		break;
	case ITEM_KIND_RESET_SKILL:
		{
			const ITEMBASE* weaponItem = GetItemInfoAbsIn( pPlayer, TP_WEAR_START + eWearedItem_Weapon );
			const ITEMBASE* shieldItem = GetItemInfoAbsIn( pPlayer, TP_WEAR_START + eWearedItem_Shield );

			const ITEM_INFO* weaponItemInfo	= ( weaponItem ? ITEMMGR->GetItemInfo( weaponItem->wIconIdx ) : 0 );
			const ITEM_INFO* shieldItemInfo	= ( shieldItem ? ITEMMGR->GetItemInfo( shieldItem->wIconIdx ) : 0 );

			if( weaponItemInfo &&
				shieldItemInfo &&
				weaponItemInfo->WeaponType == shieldItemInfo->WeaponType )
			{
				return eItemUseErr_Err;
			}
		}
		break;
	case ITEM_KIND_EXTEND_STORAGE:
		{
			const WORD storagenum = pPlayer->GetStorageNum();

			if( TAB_BASE_STORAGE_NUM > storagenum || storagenum >= TAB_STORAGE_NUM )
			{
				return eItemUseErr_Err;
			}
		}
		break;
	}

	if(0 == usedItem.Durability && 0 == pItemInfo->dwUseTime)
	{
		return eItemUseErr_Err;
	}

	// desc_hseos_ü����������01
	// S ü���������� �߰� added by hseos 2007.08.03
	// ..ü���� �������� ��쿡 �� ������ üũ�ؼ� �� ������ �ϳ��� ������
	// ..����� ���� �ʰ� �Ѵ�. �ϳ��� ������ �����ϰ� ������ ü����. ���� �������� ������ �����ϴ�
	// ..���������� ��ȯ�Ǵ� Ȯ���� �߻��ϴ���, �� ������ �ϳ��ۿ�(Ȥ�� ��ȯ ������ ���� �̸�) ���� ������ 
	// ..�ϳ��� ���޵ȴ�.
	BOOL bChangeItem = FALSE;
	if (IsChangeItem(pItemInfo->ItemIdx))
	{
		CItemSlot* pSlot = pPlayer->GetSlot(eItemTable_Inventory);
		if(NULL == pSlot) ASSERTMSG(0,"�ùٸ� ������ ������ ���� �� �����ϴ�.");

		// 071211 KTH -- pPlayer �߰�
		DWORD nNum = GetTotalEmptySlotNum(pSlot, pPlayer);
		// 071129 KTH -- ChangeItem ��Ͽ��� ��� �����Ұ��� ������ ������ ���� ���� �󽽷��� ���ٸ� return eItemUseErr_Err
		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		DWORD ResultItemNum = 0;

		for(int i=0; i<m_nChangeItemInfoNum; i++)
		{
			if (pItemInfo->ItemIdx == (DWORD)m_stChangeItemInfo[i].nChangeItemID)
			{
				ResultItemNum = m_stChangeItemInfo[i].nResultNum;
				break;
			}
		}

		if(nNum == 0 || ResultItemNum > nNum )
			/////////////////////////////////////////////////////////////////////////////////////////////////////////
		{
			MSG_DWORD m;
			m.Category	= MP_ITEM;
			m.Protocol	= MP_ITEM_WORKING_FAIL;
			m.dwData	= pPlayer->GetID();

			PACKEDDATA_OBJ->QuickSend( pPlayer, &m, sizeof( m ) );

			// 091214 ONS Full�κ����� ���� �޼�����ȣ ����
			return eItemUseErr_FullInven;
		}

		bChangeItem = TRUE;
	}
	

	if( pItemInfo->Stack > 0 )
	{
		if(EI_TRUE != DiscardItem(pPlayer, TargetPos, wItemIdx, 1))
		{
			return eItemUseErr_Err;
		}
	}
	// 080326 NYJ --- �Ⱓ�� �������� �������� �ȵǹǷ� DiscardItem()�� �н��Ѵ�.
	else if(!pItemInfo->nTimeKind && !pItemInfo->dwUseTime && pItemInfo->SupplyType != ITEM_KIND_PET )
	{
		if(EI_TRUE != DiscardItem(pPlayer, TargetPos, wItemIdx, 1))
		{
			return eItemUseErr_Err;
		}
	}

	BYTE bEffectKind = 0;	//�񹫽� �ٸ�ĳ���Ϳ��Ե� ����Ʈ �����ֱ� ����
	eItemUse_Err errorCode = eItemUseSuccess;

	switch( pItemInfo->SupplyType )
	{
		// 071204 LYW --- ItemManager : ���� �ʱ�ȭ ó�� �߰�.
	case ITEM_KIND_RESET_STATS :
		{
			UseItem_ResetStats(pPlayer);
		}
		break ;

		// 071204 LYW --- ItemManager : ��ų �ʱ�ȭ ó�� �߰�.
	case ITEM_KIND_RESET_SKILL :
		{
			UseItem_ResetSkill(pPlayer);
		}
		break ;

	case ITEM_KIND_HP_RECOVER:
		{
			CCharacterCalcManager::StartUpdateLife( pPlayer, pItemInfo->SupplyValue );

			bEffectKind = 1;//������ ȸ�� ����Ʈ
			break;
		}
	case ITEM_KIND_MP_RECOVER:
		{
			CCharacterCalcManager::StartUpdateMana( pPlayer, pItemInfo->SupplyValue );

			bEffectKind = 2;//���� ȸ�� ����Ʈ
			break;
		}
	case ITEM_KIND_HP_RECOVER_INSTANT:
	case ITEM_KIND_GTHP_RECOVER_INSTANT:
		{
			CCharacterCalcManager::StartUpdateLife( pPlayer, pItemInfo->SupplyValue, 1, 0 );

			bEffectKind = 1;//������ ȸ�� ����Ʈ
			break;
		}
	case ITEM_KIND_MP_RECOVER_INSTANT:
	case ITEM_KIND_GTMP_RECOVER_INSTANT:
		{
			CCharacterCalcManager::StartUpdateMana( pPlayer, pItemInfo->SupplyValue, 1, 0 );

			bEffectKind = 2;//���� ȸ�� ����Ʈ
			break;
		}
	case ITEM_KIND_HPMP_RECOVER:		//����ȸ����
		{
			CCharacterCalcManager::StartUpdateLife( pPlayer, pItemInfo->SupplyValue );
			CCharacterCalcManager::StartUpdateMana( pPlayer, pItemInfo->SupplyValue );
				bEffectKind = 3;
			break;
		}
	case ITEM_KIND_HPMP_RECOVER_INSTANT: //�������ȸ����
		{
			CCharacterCalcManager::StartUpdateLife( pPlayer, pItemInfo->SupplyValue, 1, 0 );
			CCharacterCalcManager::StartUpdateMana( pPlayer, pItemInfo->SupplyValue, 1, 0 );
				bEffectKind = 3;
			break;
		}
	case ITEM_KIND_RETURN_SCROLL:
		{
			//---KES PK Mode 080104
			//---KES AUTONOTE
			if( pPlayer->IsPKMode() || pPlayer->GetAutoNoteIdx() )
			{
				return eItemUseErr_Err;
			}
			//---------------------

			MSG_WORD message;
			message.dwObjectID	= pPlayer->GetID();
			message.Category	= MP_USERCONN;
			message.Protocol	= MP_USERCONN_RETURN_SYN;
			message.wData		= ( pItemInfo->SupplyValue ? WORD( pItemInfo->SupplyValue ) : pPlayer->GetPlayerLoginPoint() );

			UserConn_Return_Syn( pPlayer->GetAgentNum(), ( char* )&message );

			break;
		}
	// desc_hseos_����Ʈ ��_01
	// S ����Ʈ �� �߰� added by hseos 2007.12.01
	case ITEM_KIND_CHALLENGEZONE_ENTER_FREQ:
		{
			g_csDateManager.SRV_SetChallengeZoneEnterBonusFreq(pPlayer, pItemInfo->SupplyValue);
			break;
		}
		// 071206 KTH -- â�� Ȯ�� ������ ���
	case ITEM_KIND_EXTEND_STORAGE:
		{
			pPlayer->SetStorageNum(
				pPlayer->GetStorageNum() + 1);
			pPlayer->SetMaxPurseMoney(
				eItemTable_Storage,
				STORAGEMGR->GetStorageInfo(pPlayer->GetStorageNum())->MaxMoney);
			StorageBuyStorage(
				pPlayer->GetID());

			MSG_BYTE message;
			ZeroMemory(
				&message,
				sizeof(message));
			message.Category = MP_STORAGE;
			message.Protocol = MP_STORAGE_BUY_ACK;
			message.bData = pPlayer->GetStorageNum();
			pPlayer->SendMsg(
				&message,
				sizeof(message));
			break;
		}

	case ITEM_KIND_JOIN_EVENT:
		{
			UpdateEventFromGame( pPlayer->GetUserID(), pPlayer->GetObjectName() );
		}
		break;
	// 080607 LYW, â�� ��ȯ ������ ó�� �߰�.
	// 080923 LUJ, ������ â�� ���� �ڵ带 ����ϵ��� ��
	case ITEM_KIND_OPENSTORAGE_BYITEM :
		{
			if( ! pPlayer->GetStorageNum() )
			{	
				// 080923 LUJ, Ŭ���̾�Ʈ���� üũ�ϹǷ� ���� ó���� �ʿ����
				break;
			}	
			else if( ! pPlayer->IsGotWarehouseItems() )
			{	
				CharacterStorageItemInfo( pPlayer->GetID(), pPlayer->GetUserID(), 0 ) ;
				pPlayer->SetGotWarehouseItems( TRUE );
			}	
			else
			{	
				// 080923 LUJ, �ܿ� �ð��� �����ؾ� �Ѵ�. �׷��� ������ Ŭ���̾�Ʈ ������ ǥ�� ������ �߻��Ѵ�
				pPlayer->ProcessTimeCheckItem( TRUE );
				STORAGEMGR->SendStorageItemInfo( pPlayer );
			}	
		}
		break ;

	// 080611 LYW --- ItemManager : ��ų ����Ʈ ȹ�� �ֹ��� ó�� �߰�.
	case ITEM_KIND_GET_SKILLPOINT :
		{
			pPlayer->SetSkillPoint(
				pItemInfo->SupplyValue,
				FALSE);
		}
		break ;
	case ITEM_KIND_PET_FOOD:	
		{
			CPet* const petObject = PETMGR->GetPet(
				pPlayer->GetPetItemDbIndex());

			if(0 == petObject)
			{
				break;
			}

			petObject->FriendlyUp(
				BYTE(pItemInfo->SupplyValue));
		}
		break;
	case ITEM_KIND_PET_HP_RECOVER:			
		{
			CPet* const petObject = PETMGR->GetPet(
				pPlayer->GetPetItemDbIndex());

			if(0 == petObject)
			{
				break;
			}

			petObject->SetLife(
				petObject->GetLife() + pItemInfo->SupplyValue);
		}
		break;
	case ITEM_KIND_PET_MP_RECOVER:
		{
			CPet* const petObject = PETMGR->GetPet(
				pPlayer->GetPetItemDbIndex());

			if(0 == petObject)
			{
				break;
			}

			petObject->SetMana(
				petObject->GetMana() + pItemInfo->SupplyValue);
		}
		break;
	case ITEM_KIND_PET_EVOLUTION_PYHSIC_TYPE:
		{
			CPet* const petObject = PETMGR->GetPet(
				pPlayer->GetPetItemDbIndex());

			if(0 == petObject)
			{
				break;
			}

			if( petObject->GetObjectInfo().Type == ePetType_Basic )
			{
				petObject->SetType( ePetType_Physic );
				petObject->GradeUp();
			}
			else if( petObject->GetObjectInfo().Type == ePetType_Physic )
			{
				petObject->GradeUp();
			}
			else
				return eItemUseErr_Err;
		}
		break;
	case ITEM_KIND_PET_EVOLUTION_MAGIC_TYPE:
		{
			CPet* const petObject = PETMGR->GetPet(
				pPlayer->GetPetItemDbIndex());

			if(0 == petObject)
			{
				break;
			}

			if( petObject->GetObjectInfo().Type == ePetType_Basic )
			{
				petObject->SetType( ePetType_Magic );
				petObject->GradeUp();
			}
			else if( petObject->GetObjectInfo().Type == ePetType_Magic )
			{
				petObject->GradeUp();
			}
			else
				return eItemUseErr_Err;
		}
		break;
	case ITEM_KIND_PET_EVOLUTION_SUPPORT_TYPE:
		{
			CPet* const petObject = PETMGR->GetPet(
				pPlayer->GetPetItemDbIndex());

			if(0 == petObject)
			{
				break;
			}

			if( petObject->GetObjectInfo().Type == ePetType_Basic )
			{
				petObject->SetType( ePetType_Support );
				petObject->GradeUp();
			}
			else if( petObject->GetObjectInfo().Type == ePetType_Support )
			{
				petObject->GradeUp();
			}
			else
				return eItemUseErr_Err;
		}
		break;

	case ITEM_KIND_SUMMON_NPC:
		{
			std::map<DWORD, stNpcSummonInfo*>::iterator iter;
			iter = m_mapNpcSummonInfo.find(pItemInfo->SupplyValue);

			if(iter != m_mapNpcSummonInfo.end())
			{
				stNpcSummonInfo* pSummonInfo = iter->second;
				if(pSummonInfo)
				{
					VECTOR3 vOutPos;
					pPlayer->GetPosition(&vOutPos);

					DWORD dwGridID = pPlayer->GetChannelID();
					MAPTYPE MapNum = g_pServerSystem->GetMapNum();
					vOutPos.x = ((DWORD)(vOutPos.x/50.0f)*50.0f) + 100.0f;
					vOutPos.z = ((DWORD)(vOutPos.z/50.0f)*50.0f) + 100.0f;

					BASEOBJECT_INFO Baseinfo;
					NPC_TOTALINFO NpcTotalInfo;
					NPC_LIST* pNpcList = GAMERESRCMNGR->GetNpcInfo(WORD( pSummonInfo->NpcIdx ));
					if(pNpcList == 0)
						return eItemUseErr_NoSummonInfo;

					Baseinfo.dwObjectID =  g_pAISystem.GeneraterMonsterID();
					Baseinfo.BattleID = dwGridID;
					if(pNpcList->JobKind == HOUSING_PORTAL_ROLE)
					{
						size_t len = strlen(pPlayer->GetHouseName());
						if(len < 2)
							return eItemUseErr_HaveNoHouse;

						SafeStrCpy(Baseinfo.ObjectName, pPlayer->GetObjectName(), MAX_NAME_LENGTH+1);
					}
					else
						SafeStrCpy(Baseinfo.ObjectName, pNpcList->Name, MAX_NAME_LENGTH+1);

					NpcTotalInfo.Group = 0;
					NpcTotalInfo.MapNum = MapNum;
					NpcTotalInfo.NpcJob = pNpcList->JobKind;
					NpcTotalInfo.NpcKind = pNpcList->NpcKind;
					NpcTotalInfo.NpcUniqueIdx = 0;
					NpcTotalInfo.dwSummonerUserIndex = pPlayer->GetUserID();
					SafeStrCpy(NpcTotalInfo.szSummonerName, pPlayer->GetObjectName(), MAX_NAME_LENGTH+1);

					CNpc* pNpc = g_pServerSystem->AddNpc(&Baseinfo, &NpcTotalInfo, &vOutPos);
					if(pNpc)
						pNpc->SetDieTime(gCurTime + (pSummonInfo->LifeTime * 1000));

					// �丮�α�
					if(COOKMGR->IsFireNpc(pSummonInfo->NpcIdx))
					{
						DWORD dwFireCount = pPlayer->GetFireCount();
						DWORD dwDiffCount = 0;
						if(dwFireCount < COOKMGR->GetMaxFireCount(pPlayer->GetCookLevel()))
						{
							dwDiffCount = 1;
							pPlayer->SetFireCount(WORD( dwFireCount + dwDiffCount ));
						}
						else
							pPlayer->SetFireCount(COOKMGR->GetMaxFireCount(pPlayer->GetCookLevel()));

						Cooking_Update(pPlayer);
						Cooking_Log(pPlayer->GetID(), eCookingLog_FireCount, pItemInfo->ItemIdx, dwFireCount, dwDiffCount, pPlayer->GetFireCount());


						MSG_DWORD4 msg;
						msg.Category = MP_COOK;
						msg.Protocol = MP_COOK_STATE;
						msg.dwObjectID = pPlayer->GetID();
						msg.dwData1 = pPlayer->GetCookLevel();
						msg.dwData2 = pPlayer->GetCookCount();
						msg.dwData3 = pPlayer->GetEatCount();
						msg.dwData4 = pPlayer->GetFireCount();
						pPlayer->SendMsg(&msg, sizeof(msg));
					}
				}
			}
		}
		break;
	
	//081027 NYJ - ���ͼ�ȯ������
	case ITEM_KIND_SUMMON_MONSTER:
		{
			stMonSummonGroupInfo* pInfo = m_vecMonSummonGroupInfo.at(pItemInfo->SupplyValue);
			if(pInfo)
			{
				DWORD dwRand1 = rand() % 100;
				DWORD dwRand2 = rand() % 10000;
				DWORD dwResRand = dwRand1*10000 + dwRand2;
				DWORD dwDieTime = pInfo->dwDieTime;

				DWORD dwMonsterIdx = pInfo->MonSummon(dwResRand);

				if(dwMonsterIdx)
				{
					VECTOR3 vOutPos;
					pPlayer->GetPosition(&vOutPos);

					DWORD dwGridID = pPlayer->GetChannelID();
					vOutPos.x = ((DWORD)(vOutPos.x/50.0f)*50.0f) + 200.0f;
					vOutPos.z = ((DWORD)(vOutPos.z/50.0f)*50.0f) + 200.0f;

					int nRad = 10;
					RECALLMGR->EventRecall( pPlayer, WORD( dwMonsterIdx ),  1 , BYTE( dwGridID ), &vOutPos,WORD( nRad ), 0,0, ITEMRECALL_GROUP_ID, dwDieTime);
				}
			}			
		}
		break;

	case ITEM_KIND_SUMMON_EFFECT:
		{
			MSG_DWORD2 msgEff;
			msgEff.Category		= MP_ITEM;
			msgEff.Protocol		= MP_ITEM_USEEFFECT_ACK;
			msgEff.dwData1		= pPlayer->GetID();
			msgEff.dwData2		= pItemInfo->SupplyValue;

			PACKEDDATA_OBJ->QuickSend( pPlayer, &msgEff, sizeof(msgEff) );
		}
		break;
	case ITEM_KIND_ADDRECIPE:
		{
			DWORD dwValue = pItemInfo->SupplyValue;
			stRecipeInfo* pRecipeInfo = COOKMGR->GetRecipeInfo(dwValue);

			int nCheck = pPlayer->CanAddRecipe(pItemInfo->SupplyValue);
			if(0<=nCheck && nCheck<MAX_RECIPE_LV4_LIST)
			{
				pPlayer->SetMasterRecipe(POSTYPE( nCheck ), pRecipeInfo->dwRecipeIdx, pRecipeInfo->dwRemainTime);
				Cooking_Recipe_Update(pPlayer->GetID(), eCOOKRECIPE_ADD, pRecipeInfo->dwRecipeIdx, POSTYPE( nCheck ), pRecipeInfo->dwRemainTime);
				CookRecipe_Log(pPlayer->GetID(), pRecipeInfo->dwRecipeIdx, POSTYPE( nCheck ), eCOOKRECIPE_ADD, pRecipeInfo->dwRemainTime);

				MSG_DWORD4 msg;
				msg.Category	= MP_COOK;
				msg.Protocol	= MP_COOK_UPDATERECIPE;
				msg.dwObjectID	= pPlayer->GetID();
				msg.dwData1		= eCOOKRECIPE_ADD;
				msg.dwData2		= pRecipeInfo->dwRecipeIdx;
				msg.dwData3		= nCheck;
				msg.dwData4		= pRecipeInfo->dwRemainTime;
				pPlayer->SendMsg(&msg, sizeof(msg));
			}
		}
		break;

	case ITEM_KIND_FOODSKILLBOOK:
		{
			int nValue = pItemInfo->SupplyValue;
			if(COOKMGR->CanLevelUp(pPlayer, WORD( nValue )))
			{
				WORD wCookLevel = pPlayer->GetCookLevel();
				pPlayer->SetCookLevel(wCookLevel + 1);
				pPlayer->SetCookCount(0);
				pPlayer->SetEatCount(0);
				pPlayer->SetFireCount(0);

				// 100607 NYJ ������� �������� ����������
				DWORD dwRewardItem = COOKMGR->GetLevelUpReward(wCookLevel + 1);
				if(dwRewardItem)
				{
					ITEM_INFO* pInfo = ITEMMGR->GetItemInfo(dwRewardItem);
					if(pInfo)
					{
						// 2289, 2290, "2291" �� SystemMsg.bin�� �ε���
						ItemInsertToNote(pPlayer->GetID(), dwRewardItem, 1, pInfo->wSeal, 0, eNoteParsing_CookLevelUp, 2289, 2290, "2291");
					}
				}

				MSG_DWORD4 msg;
				msg.Category = MP_COOK;
				msg.Protocol = MP_COOK_LEVELUP_ACK;
				msg.dwObjectID = pPlayer->GetID();
				msg.dwData1 = wCookLevel + 1;
				msg.dwData2 = 0;
				msg.dwData3 = 0;
				msg.dwData4 = 0;
				pPlayer->SendMsg(&msg, sizeof(msg));
			}
		}
		break;

	case ITEM_KIND_FOOD:
		{
			if(pPlayer->GetCookLevel() == pItemInfo->Grade)
			{
				DWORD dwEatCount = pPlayer->GetEatCount();
				DWORD dwDiffCount = 0;
				if(dwEatCount < COOKMGR->GetMaxEatCount(pPlayer->GetCookLevel()))
				{
					dwDiffCount = 1;
					pPlayer->SetEatCount(WORD( dwEatCount + dwDiffCount ) );
				}
				else
					pPlayer->SetEatCount(COOKMGR->GetMaxEatCount(pPlayer->GetCookLevel()));

				Cooking_Update(pPlayer);
				Cooking_Log(pPlayer->GetID(), eCookingLog_EatCount, pItemInfo->ItemIdx, dwEatCount, dwDiffCount, pPlayer->GetEatCount());

				MSG_DWORD4 msg;
				msg.Category = MP_COOK;
				msg.Protocol = MP_COOK_STATE;
				msg.dwObjectID = pPlayer->GetID();
				msg.dwData1 = pPlayer->GetCookLevel();
				msg.dwData2 = pPlayer->GetCookCount();
				msg.dwData3 = pPlayer->GetEatCount();
				msg.dwData4 = pPlayer->GetFireCount();
				pPlayer->SendMsg(&msg, sizeof(msg));
			}
		}
		break;

	case ITEM_KIND_CHANGESIZE_UPWEIGHT:
		{
			// ����ʿ��� Ű��ȯ ���� ������ ����� ���Ѵ�
			if( g_pServerSystem->GetMapNum()==GTMAPNUM )
			{
				return eItemUseErr_Err;
			}
			// �����ʿ����� Ű��ȯ ���� ������ ����� ���Ѵ�.
			else if(  SIEGEWARFAREMGR->Is_CastleMap() == FALSE && SIEGEWARFAREMGR->IsSiegeWarfareZone() )
			{
				return eItemUseErr_Err;
			}

			float fSize = 1.0f;
			int nRand, nBound = 0;
			float fDiffValue = 0.0f;
			if(rand()%10 < 5) // Ŀ��Ȯ�� 50%
			{
				// Ŀ�� ����ġ�� 1��
				nBound = pItemInfo->SupplyValue;
				nRand = rand()%nBound;
				fSize = (float)nRand / 100.0f;
				fDiffValue = (float)nRand / (float)nBound;
			}
			else // �۾���Ȯ�� 50%
			{
				// �۾��� ����ġ�� 1/2��
				nBound = pItemInfo->SupplyValue/2;
				nRand = rand()%nBound;
				fSize = (float)nRand / 100.0f;
				fSize *= -1.0f;

				fDiffValue = (float)nRand / (float)nBound;
				fDiffValue *= -1.0f;
			}

			float fNewSize = pPlayer->GetCharacterSize() + fSize;
			if(MAX_CHAR_CHANGESIZE < fNewSize)
				fNewSize = MAX_CHAR_CHANGESIZE;

			pPlayer->SetCharacterSize( fNewSize );
			CharSize_Update(pPlayer);

			MSG_DWORD2INT msg;
			msg.Category	= MP_ITEM;
			msg.Protocol	= MP_ITEM_CHANGESIZE_ACK;
			msg.dwData1		= pPlayer->GetID();
			msg.dwData2		= (DWORD)(fNewSize * 100.0f);
			msg.nData		= (int)(fDiffValue * 100.0f);
			PACKEDDATA_OBJ->QuickSend( pPlayer, &msg, sizeof( msg ) );
		}
		break;

	case ITEM_KIND_CHANGESIZE_DNWEIGHT:
		{
			// ����ʿ��� Ű��ȯ ���� ������ ����� ���Ѵ�
			if( g_pServerSystem->GetMapNum()==GTMAPNUM )
			{
				return eItemUseErr_Err;
			}
			// �����ʿ����� Ű��ȯ ���� ������ ����� ���Ѵ�.
			else if(  SIEGEWARFAREMGR->Is_CastleMap() == FALSE && SIEGEWARFAREMGR->IsSiegeWarfareZone() )
			{
				return eItemUseErr_Err;
			}

			float fSize = 1.0f;
			int nRand, nBound = 0;
			float fDiffValue = 0.0f;
			if(rand()%10 < 5) // Ŀ��Ȯ�� 50%
			{
				// Ŀ�� ����ġ�� 1/2��
				nBound = pItemInfo->SupplyValue/2;
				nRand = rand()%nBound;
				fSize = (float)nRand / 100.0f;
				fDiffValue = (float)nRand / (float)nBound;
				
			}
			else // �۾���Ȯ�� 50%
			{
				// �۾��� ����ġ�� 1��
				nBound = pItemInfo->SupplyValue;
				nRand = rand()%nBound;
				fSize = (float)(rand()%nBound) / 100.0f;
				fSize *= -1.0f;

				fDiffValue = (float)nRand / (float)nBound;
				fDiffValue *= -1.0f;
			}

			float fNewSize = pPlayer->GetCharacterSize() + fSize;
			if(fNewSize < MIN_CHAR_CHANGESIZE)
				fNewSize = MIN_CHAR_CHANGESIZE;

			pPlayer->SetCharacterSize( fNewSize );
			CharSize_Update(pPlayer);

			MSG_DWORD2INT msg;
			msg.Category	= MP_ITEM;
			msg.Protocol	= MP_ITEM_CHANGESIZE_ACK;
			msg.dwData1		= pPlayer->GetID();
			msg.dwData2		= (DWORD)(fNewSize * 100.0f);
			msg.nData		= (int)(fDiffValue * 100.0f);
			PACKEDDATA_OBJ->QuickSend( pPlayer, &msg, sizeof( msg ) );
		}
		break;

	case ITEM_KIND_CHANGESIZE_DEFALUT:
		{
			// ����ʿ��� Ű��ȯ ���� ������ ����� ���Ѵ�
			if( g_pServerSystem->GetMapNum()==GTMAPNUM )
			{
				return eItemUseErr_Err;
			}
			// �����ʿ����� Ű��ȯ ���� ������ ����� ���Ѵ�.
			else if(  SIEGEWARFAREMGR->Is_CastleMap() == FALSE && SIEGEWARFAREMGR->IsSiegeWarfareZone() )
			{
				return eItemUseErr_Err;
			}

			const float fNewSize = 1.0f;
			float fDiffValue = 1.0f - pPlayer->GetCharacterSize();

			pPlayer->SetCharacterSize( fNewSize );
			CharSize_Update(pPlayer);

			MSG_DWORD2INT msg;
			msg.Category	= MP_ITEM;
			msg.Protocol	= MP_ITEM_CHANGESIZE_ACK;
			msg.dwData1		= pPlayer->GetID();
			msg.dwData2		= (DWORD)(fNewSize * 100.0f);
			msg.nData		= (int)(fDiffValue * 100.0f);
			PACKEDDATA_OBJ->QuickSend( pPlayer, &msg, sizeof( msg ) );
		}
		break;

	case ITEM_KIND_FURNITURE_WALLPAPER:
	case ITEM_KIND_FURNITURE_FLOORPAPER:
	case ITEM_KIND_FURNITURE_CEILINGPAPER:
		{
			HOUSINGMGR->ChangeMaterial(
				pPlayer->GetID(),
				usedItem.dwDBIdx,
				usedItem.wIconIdx,
				pItemInfo);
		}
		break;

	case ITEM_KIND_EXTEND_HOUSE:
		{
			HOUSINGMGR->ExtendHouse(
				pPlayer->GetID(),
				usedItem.dwDBIdx,
				usedItem.wIconIdx,
				pItemInfo);
		}
		break;
	case ITEM_KIND_RESET_REGIDENT_REGIST:
		{
			errorCode = eItemUseSuccess_ResetRegidentCard;
		}
		break;
	}

	// desc_hseos_ü����������01
	// S ü���������� �߰� added by hseos 2007.08.03
	if (bChangeItem)
	{
		BOOL bResult = ProcChangeItem(pPlayer, pItemInfo->ItemIdx);
		if (bResult == FALSE)
		{
			return eItemUseErr_Err;
		}
	}
	// E ü���������� �߰� added by hseos 2007.08.03

	if( bEffectKind != 0 )
	{
		if( pPlayer->GetBattle()->GetBattleKind() == eBATTLE_KIND_SHOWDOWN )	//����
		{
			MSG_DWORDBYTE msg;
			msg.Category	= MP_ITEM;
			msg.Protocol	= MP_ITEM_USE_NOTIFY;
			msg.dwData		= pPlayer->GetID();
			msg.bData		= bEffectKind;

			PACKEDDATA_OBJ->QuickSend( pPlayer, &msg, sizeof(msg) );
		}
	}

	// 080627 LUJ, ���� �������� ��� ��� �α׸� ���⵵�� �Ѵ�
	// 100813 NUJ, ��ų���������� �α׸� ������ ���⵵�� �Ѵ�.
	if( pItemInfo->Shop &&
		pItemInfo->SupplyType!=ITEM_KIND_GTBUFF_ITEMS)
	{
		LogItemMoney(
			pPlayer->GetID(),
			pPlayer->GetObjectName(),
			0,
			"",
			IsChangeItem( pItemInfo->ItemIdx ) ? eLog_ItemDestroybyChangeItem : eLog_ItemUse,
			pPlayer->GetMoney(),
			0,
			0,
			usedItem.wIconIdx,
			usedItem.dwDBIdx,
			usedItem.Position,
			0,
			pItemInfo->Stack ? usedItem.Durability - 1 : 0,
			0 );
	}

	return errorCode;
}

eLogitemmoney CItemManager::Mix( CPlayer* player, const MSG_ITEM_MIX_SYN2& input, const CBackupSlot* backupSlot )
{
	// �߰��� ������:	�� ���Կ� ���� DB �׼����� ����ȭ���� ���� ���, DBó���� ���������� �����Ƿ�
	//					�ſ� �����ϴ�.
	//
	// �ذ�å:			���� ������ �����صΰ� �޸𸮿� ���� ������Ʈ�Ѵ�. ������ ������ ��� ������
	//					������ �������� �ٷ� �޸𸮸� �����Ѵ�. ���������� ������ ���� ������ �����
	//					������ ���Ͽ� �� ���Կ� ���� �ϳ��� ���۸� ��û�Ѵ�
	//					���� DB �ε����� ��Ȱ���� �� �־� �ӵ��� ���� ���ڵ� ������ �ּ�ȭ�ȴ�.

	const DWORD		resultIndex				= input.mResultIndex;
	DWORD			mixSize					= input.mMixSize;
	const POSTYPE	inventoryStartPosition	= POSTYPE( TP_INVENTORY_START );
	const POSTYPE	inventoryEndPosition	= POSTYPE( TP_INVENTORY_END + player->GetInventoryExpansionSize() );

	CItemSlot* playerSlot = player->GetSlot( eItemTable_Inventory );

	if( ! backupSlot	||
		! playerSlot	||
		! mixSize )
	{
		return eLog_itemMixBadRequest;
	}

	ITEMBASE sourceItem;
	{
		const ITEMBASE* item = playerSlot->GetItemInfoAbs( input.mSourceItemPosition );

		if( !	item	||
			item->dwDBIdx != input.mSourceItemDbIndex )
		{
			return eLog_itemMixBadRequest;
		}

		sourceItem = *item;
	}

	const MixScript* script	= GetMixScript( sourceItem.wIconIdx );

	if( ! script )
	{
		return eLog_itemMixBadRequest;
	}

	const ItemMixResult* mixResult = 0;
	{
		MixScript::const_iterator it	= script->begin();

		if( script->size() <= resultIndex )
		{
			return eLog_ItemMixBadCondition;
		}

		std::advance( it, resultIndex );
		mixResult = &( *it );
	}

	if(	player->GetLevel() < mixResult->mRequiredLevel	||
		player->GetMoney() < mixResult->mMoney * mixSize )
	{
		return eLog_ItemMixBadCondition;
	}

	// 080228 LUJ, ��ȣ ��� �÷���
	BOOL isProtection = FALSE;

	// 080228 LUJ, ��ȣ �������� ���� ��� �Ҹ��Ѵ�
	if( input.mProtectionItem.wIconIdx )
	{
		const ITEMBASE* item = playerSlot->GetItemInfoAbs( input.mProtectionItem.Position );

		if( !	item											||
				item->dwDBIdx != input.mProtectionItem.dwDBIdx	||
			!	IsMixProtectionItem( *item ) )
		{
			return eLog_ItemMixBadCondition;
		}

		const ITEM_INFO* info = GetItemInfo( item->wIconIdx );

		if( ! info )
		{
			return eLog_ItemMixBadCondition;
		}

		if( info->Stack &&
			1 < item->Durability )
		{
			if( EI_TRUE != playerSlot->UpdateItemAbs(
				player,
				item->Position,
				item->dwDBIdx,
				item->wIconIdx,
				item->Position,
				item->QuickPosition,
				item->Durability - 1 ) )
			{
				return eLog_ItemMixBadCondition;
			}
		}
		else
		{
			if( EI_TRUE != playerSlot->DeleteItemAbs(
				player,
				item->Position,
				0 ) )
			{
				return eLog_ItemMixBadCondition;
			}
		}

		isProtection = TRUE;
	}

	// 080925 LUJ, ���� �������� ���� ��� �Ҹ��Ѵ�
	if( input.mSupportItem.wIconIdx )
	{
		// 080925 LUJ, ���� ������ ��� �� ����� ����/���/�׼������� ����
		{
			const ITEM_INFO* itemInfo = GetItemInfo( mixResult->mItemIndex );

			if( ! itemInfo )
			{
				return eLog_ItemMixBadCondition;
			}
			else if(	( itemInfo->EquipType	!= eEquipType_Accessary ) &&
						( itemInfo->EquipType	!= eEquipType_Weapon	) &&
						( itemInfo->EquipType	!= eEquipType_Armor		) )
			{
				return eLog_ItemMixBadCondition;
			}
		}

		const ITEMBASE* itemBase = playerSlot->GetItemInfoAbs( input.mSupportItem.Position );

		if( !	itemBase	||
				itemBase->dwDBIdx	!= input.mSupportItem.dwDBIdx||
				itemBase->wIconIdx	!= input.mSupportItem.wIconIdx )
		{
			return eLog_ItemMixBadCondition;
		}
		else if(	IsDupItem( itemBase->wIconIdx )	&&
					1 < itemBase->Durability )
		{
			if( EI_TRUE != playerSlot->UpdateItemAbs(
				player,
				itemBase->Position,
				itemBase->dwDBIdx,
				itemBase->wIconIdx,
				itemBase->Position,
				itemBase->QuickPosition,
				itemBase->Durability - 1 ) )
			{
				return eLog_ItemMixBadCondition;
			}
		}
		else
		{
			if( EI_TRUE != playerSlot->DeleteItemAbs(
				player,
				itemBase->Position,
				0 ) )
			{
				return eLog_ItemMixBadCondition;
			}
		}
	}

	// 080925 LUJ, ���� �������� ���� ��� ���� Ȯ�� ���ʽ� ���� ���´�
	float bonusSuccessRate = 0;
	{
		const MixSupportScript* script = GetMixSupportScript( input.mSupportItem.wIconIdx );

		bonusSuccessRate = ( script ? script->mBonusSucessRate : 0 );
	}

	// 080227 LUJ, ����и� ��� �Ӽ����� �߰�
	enum
	{
		TypeSuccess,
		TypeFailure,
		TypeBigFailure,		
	}
	// 080925 LUJ, ���� Ȯ�� ���ʽ� ����
	resultType = ( ( mixResult->mSuccessRate + bonusSuccessRate ) < mRandom.GetFloat() ? TypeFailure : TypeSuccess );
	
	// 080228 LUJ, ��ȣ ����� ��� �����ϴ��� ��Ḧ ���� �ʵ��� �Ѵ�
	if(	TypeSuccess == resultType ||
		TypeFailure == resultType && ! isProtection )
	{
		// ��� ����� ��Ȯ�� ��ġ�ϴ��� Ȯ���Ѵ�. ������ �������� ������ �ε����� 0���� �����Ͽ� �޽����� ���� ���̴�
		for(
			ItemMixResult::Material::const_iterator material_it = mixResult->mMaterial.begin();
			mixResult->mMaterial.end() != material_it;
			++material_it )
		{
			const DWORD itemIndex = material_it->first;
			DWORD needQuantity = material_it->second * mixSize;

			for(POSTYPE position = inventoryStartPosition;
				inventoryEndPosition > position;
				++position)
			{
				if( ! needQuantity )
				{
					break;
				}

				const ITEMBASE* item = playerSlot->GetItemInfoAbs( position );

				if(0 ==	item ||
					0 == item->dwDBIdx ||
					item->wIconIdx != itemIndex)
				{
					continue;
				}

				if( IsDupItem( item->wIconIdx ) )
				{
					const DWORD quantity = item->Durability;

					if( item->Durability > needQuantity )
					{
						if( EI_TRUE != playerSlot->UpdateItemAbs(
							player,
							item->Position,
							item->dwDBIdx,
							item->wIconIdx,
							item->Position,
							item->QuickPosition,
							item->Durability - needQuantity ) )
						{
							//return eLog_ItemMixSystemFault;
							continue;
						}
					}
					else
					{
						if( EI_TRUE != playerSlot->DeleteItemAbs(
							player,
							item->Position,
							0 ) )
						{
							//return eLog_ItemMixSystemFault;
							continue;
						}
					}

					needQuantity -= min( quantity, needQuantity );
				}
				else
				{
					if( EI_TRUE != playerSlot->DeleteItemAbs( player, item->Position, 0 ) )
					{
						//return eLog_ItemMixSystemFault;
						continue;
					}

					--needQuantity;
				}
			}

			if( needQuantity )
			{
				return eLog_ItemMixBadCondition;
			}
		}
	}

	if( TypeFailure == resultType )
	{
		// ������ Ȯ���� �� ��Ḧ ���� �� �ִ�
		// 080314 LUJ, ����� �� 0�� ��ġ�� �������� �սǵǴ� �������� �־�, ����и� �߻����� �ʵ��� �Ѵ�
		const float bigFailedRate	= 0.0f;
		//const float bigFailedRate	= 2.0f;

		// 080228 LUJ, ���� ��ȣ�� ��� Ű �������� ���� �ʵ��� ��
		if(	!	isProtection &&
				mRandom.GetFloat() < bigFailedRate )
		{
			resultType = TypeBigFailure;
		}
		 
		// 080131 LUJ
		// 080314 LUJ, ���� ���� �αװ� �� ���� ǥ�õǵ��� ��
		LogItemMoney(
			player->GetID(),
			player->GetObjectName(),
			0,
			isProtection ? "*protect" : "",
			TypeBigFailure == resultType ? eLog_ItemMixBigFail : eLog_ItemMixFail,
			player->GetMoney(),
			0,
			0,
			sourceItem.wIconIdx,
			sourceItem.dwDBIdx,
			sourceItem.Position,
			0,
			0,
			0 );

		if(	TypeBigFailure == resultType )
		{			
			const ITEMBASE* item = playerSlot->GetItemInfoAbs( input.mSourceItemPosition );

			if( ! item )
			{
				return eLog_ItemMixSystemFault;
			}

			if( IsDupItem( item->wIconIdx ) )
			{
				if( 1 == item->Durability )
				{
					if( EI_TRUE != playerSlot->DeleteItemAbs( player, item->Position, 0 ) )
					{
						return eLog_ItemMixSystemFault;
					}
				}
				else if( EI_TRUE != playerSlot->UpdateItemAbs(
					player,
					item->Position,
					item->dwDBIdx,
					item->wIconIdx,
					item->Position,
					item->QuickPosition,
					item->Durability - 1 ) )
				{
					return eLog_ItemMixSystemFault;
				}
			}
			else 
			{
				if( EI_TRUE != playerSlot->DeleteItemAbs( player, item->Position, 0 ) )
				{
					return eLog_ItemMixSystemFault;
				}
			}
		}
	}
	else
	{
		// �ɼ��� ���� �Ϲ� ������ ����
		// �ϴ� �ִ��� ��ġ��
		if( IsDupItem( mixResult->mItemIndex ) )
		{
			const ITEM_INFO* const itemInfo = GetItemInfo(
				mixResult->mItemIndex);

			for(POSTYPE position = inventoryStartPosition;
				inventoryEndPosition > position;
				++position )
			{
				if( ! mixSize )
				{
					break;
				}

				const ITEMBASE* item = playerSlot->GetItemInfoAbs( position );

				if( !	item									||
					!	item->dwDBIdx							||
						item->wIconIdx != mixResult->mItemIndex	||
					!	IsDupItem( item->wIconIdx )				||
						itemInfo->Stack <= item->Durability		||
						itemInfo->wSeal != item->nSealed)
				{
					continue;
				}

				const DURTYPE enableSize = itemInfo->Stack - item->Durability;

				if( EI_TRUE != playerSlot->UpdateItemAbs(
					player,
					item->Position,
					item->dwDBIdx,
					item->wIconIdx,
					item->Position,
					item->QuickPosition,
					item->Durability + min( enableSize, mixSize ),
					UB_DURA,
					SS_CHKDBIDX ) )
				{
					//return eLog_ItemMixSystemFault;
					continue;
				}

				mixSize -= min( enableSize, mixSize );
			}
		}

		// ���� ������ �� ������ ����
		if( mixSize )
		{
			WORD	emptyPosition[ SLOT_MAX_INVENTORY_NUM ] = { 0 };
			WORD	emptyCount;
			{
				if( IsDupItem( mixResult->mItemIndex ) )
				{	
					// 091215 ONS ������Stack�ִ���� �����ۺ��� ������ ������ ó���Ѵ�.
					const WORD wItemStackNum = GetItemStackNum( mixResult->mItemIndex );
					emptyCount = max( 1, WORD( mixSize ) / wItemStackNum );
				}
				else
				{
					emptyCount = WORD( mixSize );
				}
			}

			playerSlot->GetEmptyCell( emptyPosition, emptyCount );

			while( emptyCount-- )
			{
				const POSTYPE position = emptyPosition[ emptyCount ];

				ITEMBASE item;
				ZeroMemory( &item, sizeof( item ) );

				item.wIconIdx	= mixResult->mItemIndex;
				item.Position	= position;

				if( IsDupItem( mixResult->mItemIndex ) )
				{
					const WORD wItemStackNum = GetItemStackNum( mixResult->mItemIndex );
					const DURTYPE enableSize = min( mixSize, wItemStackNum );

					mixSize			-=	enableSize;
					item.Durability	=	enableSize;
				}
				else
				{
					--mixSize;
					item.Durability	= 0;
				}

				// 080901 LUJ, ȹ�� �� ��Ÿ���� ���� �������� ���� �ð��� �������ش�
				if(const ITEM_INFO* info = GetItemInfo(mixResult->mItemIndex))
				{
					item.nSealed = info->wSeal;
					item.nRemainSecond = info->dwUseTime;

					if(info->SupplyType == ITEM_KIND_COOLTIME)
					{
						item.nRemainSecond = info->SupplyValue;
					}
				}

				const ITEMBASE* backupItem = backupSlot->GetItem( position );
				
				// 071218 LUJ,	�� �����̶� ������ �̷� ������ �ѹ� �� ��ȸ����. DB �ε����� ��Ȱ�������ν�
				//				���� insert�� ����� ���ϰ�, update�� ���� �� �ִ�
				if( backupItem )
				{
					item.dwDBIdx = backupItem->dwDBIdx;

					// 071218 LUJ,	UpdateItemAbs�� DB �ε����� ������Ʈ���� �ʴ´�.
					//				���� InsertItemAbs�� ���� ����� �ɸ��� �ʵ��� �����Ѵ�
					if( EI_TRUE != playerSlot->InsertItemAbs(
						player,
						position,
						&item ) )
					{
						//return eLog_ItemMixInsufficentSpace;

						continue;
					}

				}
				else
				{
					// 071218 LUJ, DB �ε����� 0�� ��� ���� ���������� �ν��ϹǷ�, �ϴ� �������� �־�����
					item.dwDBIdx = UINT_MAX;

					if( EI_TRUE != playerSlot->InsertItemAbs(
						player,
						position,
						&item,
						SS_PREINSERT ) )
					{
						//return eLog_ItemMixInsufficentSpace;

						continue;
					}
				}
			}
		}

		if( mixSize )
		{
			return eLog_ItemMixInsufficentSpace;
		}
	}

	player->SetMoney( mixResult->mMoney * input.mMixSize, MONEY_SUBTRACTION );

	MSG_ITEM_MIX_ACK message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category = MP_ITEM;
	message.mSourceItemDbIndex = input.mSourceItemDbIndex;
	message.mSourceItemPosition	= input.mSourceItemPosition;
	message.mMixSize = input.mMixSize;
	message.mResultIndex = input.mResultIndex;

	switch( resultType )
	{
	case TypeSuccess:
		{
			message.Protocol = MP_ITEM_MIX_SUCCEED_ACK;
			break;
		}
	case TypeFailure:
		{
			message.Protocol = MP_ITEM_MIX_FAILED_ACK;
			break;
		}
	case TypeBigFailure:
		{
			message.Protocol = MP_ITEM_MIX_BIGFAILED_ACK;
			break;
		}
	default:
		{
			return eLog_ItemMixServerError;
		}
	}

	UpdateResult(
		*player,
		*playerSlot,
		*backupSlot,
		message.mUpdateResult,
		MP_ITEM_MIX_GETITEM,
		eLog_ItemMixUse,
		eLog_ItemMixSuccess,
		input.mSupportItem);
	player->SendMsg(
		&message,
		message.GetSize());

	return resultType == TypeSuccess ? eLog_ItemMixSuccess : eLog_ItemMixFail;
}

BOOL CItemManager::AddMixOption(CPlayer& player, const ITEMBASE& item, const ICONBASE& supportItem)
{
	const ITEM_INFO* info = GetItemInfo( item.wIconIdx );

	if( !	info ||
			eEquipType_None == info->EquipType )
	{
		return FALSE;
	}
	
	eStatusKind equippmentType = eStatusKind_None;

	switch( info->WeaponType )
	{
	case eWeaponType_Sword:
		{
			equippmentType = eStatusKind_Sword;
			break;
		}
	case eWeaponType_Mace:
		{
			equippmentType = eStatusKind_Mace;
			break;
		}
	case eWeaponType_Axe:
		{
			equippmentType = eStatusKind_Axe;
			break;
		}
	case eWeaponType_Dagger:
		{
			equippmentType = eStatusKind_Dagger;
			break;
		}
	case eWeaponType_Staff:
		{
			equippmentType = eStatusKind_Staff;
			break;
		}
	case eWeaponType_Bow:
		{
			equippmentType = eStatusKind_Bow;
			break;
		}
	case eWeaponType_BothDagger:
		{
			equippmentType = eStatusKind_BothDagger;
			break;
		}
	case eWeaponType_Gun:
		{
			equippmentType = eStatusKind_Gun;
			break;
		}
	case eWeaponType_FishingPole:
		{
			equippmentType = eStatusKind_FishingPole;
			break;
		}
	case eWeaponType_CookUtil:
		{
			equippmentType = eStatusKind_CookUtil;
			break;
		}
	}

	switch( info->ArmorType )
	{
	case eArmorType_Robe:
		{
			equippmentType = eStatusKind_RobeArmor;
			break;
		}
	case eArmorType_Leather:
		{
			equippmentType = eStatusKind_LightArmor;
			break;
		}
	case eArmorType_Metal:
		{
			equippmentType = eStatusKind_HeavyArmor;
			break;
		}
	}

	MSG_ITEM_OPTION message;
	{
		ZeroMemory( &message, sizeof( message ) );

		message.Category = MP_ITEM;
		message.Protocol = MP_ITEM_OPTION_ADD_ACK;
		message.mSize = 1;
	}

	ITEM_OPTION& option = message.mOption[ 0 ];

	// 080601 LUJ, ���ǿ� ������ ��� �ɼ��� �ο��Ѵ�
	{
		MixSetting::Equippment::const_iterator it = mMixSetting.mEquippment.find( equippmentType );

		if( mMixSetting.mEquippment.end() != it )
		{
			// 080925 LUJ, ���� ������ ��� �� �ٸ� Ȯ�� ������ ����ϵ��� ��
			const MixSupportScript*	script = GetMixSupportScript( supportItem.wIconIdx );
			const MixSetting::Ability& ability = ( script && ( ! script->mAbiltyMap.empty() ) ? script->mAbiltyMap : it->second );
	
			// 080925 LUJ, 0~1 ������ �Ǽ� Ȯ���� ����ϵ��� ��
			MixSetting::Ability::const_iterator	ability_it = ability.lower_bound( mRandom.GetFloat() );
			MixSetting::RangeMap::const_iterator range_it = mMixSetting.mRangeMap.lower_bound( info->LimitLevel );

			if( ability.end()				!= ability_it &&
				mMixSetting.mRangeMap.end()	!= range_it )
			{
				const MixSetting::Range&	range		= range_it->second;
				const MixSetting::eAbility	abilityType = ability_it->second;

				// ��ġ ���� �ȿ��� ���� ���� ���Ѵ�
				const DWORD			different	= range.mMax - range.mMin;
				const DWORD			randomValue	= ( range.mMin + rand() % ( different + 1 ) );
				ITEM_OPTION::Mix&	mix			= option.mMix;

				switch( abilityType )
				{
				case MixSetting::eAbilityStrength:
					{
						mix.mStrength		= randomValue;
						break;
					}
				case MixSetting::eAbilityDexterity:
					{
						mix.mDexterity		= randomValue;
						break;
					}
				case MixSetting::eAbilityIntelligence:
					{
						mix.mIntelligence	= randomValue;
						break;
					}
				case MixSetting::eAbilityVitality:
					{
						mix.mVitality		= randomValue;
						break;
					}
				case MixSetting::eAbilityWisdom:
					{
						mix.mWisdom			= randomValue;
						break;
					}
				}
			}
		}
	}

	option.mItemDbIndex	= item.dwDBIdx;
	SafeStrCpy( option.mMix.mMadeBy, player.GetObjectName(), sizeof( option.mMix.mMadeBy ) );

	// 081118 LUJ, ĳ���� ��ȣ�� ������ ������ DB �����带 ������ Ű������ ���ǵ��� ��
	ItemOptionInsert( option, player.GetID() );
	// 080314 LUJ, ��� �ɼ��� �������� �� �����Ƿ� �� ���� ����Ἥ �������� �ʱ�ȭ�Ѵ�
	// 081118 LUJ, ĳ���� ��ȣ�� ������ ������ DB �����带 ������ Ű������ ���ǵ��� ��
	ItemDropOptionInsert( option, player.GetID() );
	
	AddOption( option );

	LogItemOption(
		player.GetID(),
		item.dwDBIdx,
		option,
		eLog_ItemMixSuccess );

	player.SendMsg( &message, message.GetSize() );
	return TRUE;
}


const ITEMBASE * CItemManager::GetItemInfoAbsIn(CPlayer* pPlayer, POSTYPE Pos)
{
	CItemSlot * pSlot = pPlayer->GetSlot(Pos);

	return pSlot ? pSlot->GetItemInfoAbs(Pos) : 0;
}


// 080228 LUJ, ��ȣ ��� ����
eLogitemmoney CItemManager::Reinforce( CPlayer* player, const MSG_ITEM_REINFORCE_SYN& input, const CBackupSlot* backupSlot )
{
	CItemSlot* playerSlot = player->GetSlot( eItemTable_Inventory );

	if( ! playerSlot	||
		! backupSlot )
	{
		return eLog_ItemReinforceSourceInvalid;
	}

	const ITEMBASE* sourceItem = playerSlot->GetItemInfoAbs( input.mSourceItemPosition );

	if( !	sourceItem										||
			sourceItem->dwDBIdx != input.mSourceItemDbIndex	||
			sourceItem->wIconIdx != input.mSourceItemIndex ||
			IsDupItem( sourceItem->wIconIdx ) )
	{
		return eLog_ItemReinforceSourceInvalid;
	}

	const ITEM_INFO* sourceItemInfo	= GetItemInfo( sourceItem->wIconIdx );

	if( ! sourceItemInfo )
	{
		return eLog_ItemReinforceMaterialSourceItemCannnotReinforce;
	}

	const POSTYPE	inventoryStartPosition	= POSTYPE( TP_INVENTORY_START );
	const POSTYPE	inventoryEndPosition	= POSTYPE( TP_INVENTORY_END + player->GetInventoryExpansionSize() );
	
	// Ű: ������ �ε���, ��: ����
	typedef std::map< DWORD, DWORD >	MaterialMap;
	MaterialMap							materialMap;

	// ��� �Ұ�
	{
		DURTYPE	quantity = 0;

		for( DWORD i = 0; i < input.mSize; ++i )
		{
			const ITEMBASE& clientItem	= input.mItem[ i ];
			const ITEMBASE* serverItem	= playerSlot->GetItemInfoAbs( clientItem.Position );

			if( !	serverItem											||
				!	IsDupItem( serverItem->wIconIdx )					||
					serverItem->wIconIdx	!=	clientItem.wIconIdx		||
					serverItem->dwDBIdx		!=	clientItem.dwDBIdx		||
					serverItem->Durability	!=	clientItem.Durability )
			{
				return eLog_ItemReinforceMaterialIsInvalid;
			}
			else if(	IsDupItem( serverItem->wIconIdx  ) &&
					100 < serverItem->Durability )
			{
				if( EI_TRUE != playerSlot->UpdateItemAbs(
						player,
						serverItem->Position,
						serverItem->dwDBIdx,
						serverItem->wIconIdx,
						serverItem->Position,
						serverItem->QuickPosition,
						serverItem->Durability - 100 ) )
				{
					return eLog_ItemReinforceMaterialIsInvalid;
				}
			}
			else if( serverItem->Durability <= 100 && EI_TRUE != playerSlot->DeleteItemAbs(
						player,
						serverItem->Position,
						0 ) )
			{
				return eLog_ItemReinforceMaterialIsInvalid;				
			}
			
			if( clientItem.Durability > 100)
			{
				materialMap[ clientItem.wIconIdx  ] = 100;
				quantity = 100;
			}
			else
			{
				materialMap[ clientItem.wIconIdx  ] += clientItem.Durability;
				quantity += clientItem.Durability;
			}
		}

		const DURTYPE maxQuantity = 100;

		if( maxQuantity < quantity )
		{
			return eLog_ItemReinforceDataHaveTooManyMaterials;
		}
	}

	// 080228 LUJ, ��ȣ�� �ɼ�
	ITEM_OPTION::Reinforce protectedOption = { 0 };

	// 080228 LUJ, ������ ��� ��ȣ ��带 �����Ѵ�
	if( input.mProtectionItem.wIconIdx )
	{
		// 080228 LUJ, ��ȣ ������ ���
		{
			const ITEMBASE* item = playerSlot->GetItemInfoAbs( input.mProtectionItem.Position );

			if(0 ==	item ||
				FALSE == g_CGameResourceManager.IsReinforceProtectionItem(*item))
			{
				return eLog_ItemReinforceMaterialIsInvalid;
			}

			const ITEM_INFO* info = GetItemInfo( item->wIconIdx );

			if( ! info )
			{
				return eLog_ItemReinforceMaterialIsInvalid;
			}

			if( info->Stack	&&
				1 < item->Durability )
			{
				if( EI_TRUE != playerSlot->UpdateItemAbs( player, item->Position, item->dwDBIdx, item->wIconIdx, item->Position, item->QuickPosition, item->Durability - 1 ) )
				{
					return eLog_ItemReinforceUpdatingItemIsFailed;
				}
			}
			else
			{
				if( EI_TRUE != playerSlot->DeleteItemAbs( player, item->Position, 0 ) )
				{
					return eLog_ItemReinforceUpdatingItemIsFailed;
				}
			}
		}		

		// 080228 LUJ, ��ȣ ������ ��� ���� �˻�: �ɼ��� ���� ��� �� �Ӽ��� �־����
		{
			ITEM_OPTION option = GetOption( *sourceItem );

			if(const EnchantScript* const enchantScript = g_CGameResourceManager.GetEnchantScript(option.mEnchant.mIndex))
			{
				g_CGameResourceManager.AddEnchantValue( option, enchantScript->mAbility, -1 * sourceItemInfo->EnchantDeterm * option.mEnchant.mLevel );
			}

			protectedOption = option.mReinforce;

			// 080228 LUJ, ��ȣ�� �ɼ��� ���� ��� ��ȭ���� �ʴ´�
			{
				const ITEM_OPTION::Reinforce emptyOption = { 0 };

				if( ! memcmp( &emptyOption, &protectedOption, sizeof( emptyOption ) ) )
				{
					return eLog_ItemReinforceSourceInvalid;
				}
			}

			int count = 0;

			count = ( protectedOption.mStrength			? ++count : count );
			count = ( protectedOption.mDexterity		? ++count : count );
			count = ( protectedOption.mVitality			? ++count : count );
			count = ( protectedOption.mIntelligence		? ++count : count );
			count = ( protectedOption.mWisdom			? ++count : count );
			count = ( protectedOption.mLife				? ++count : count );
			count = ( protectedOption.mMana				? ++count : count );
			count = ( protectedOption.mManaRecovery		? ++count : count );
			count = ( protectedOption.mLifeRecovery		? ++count : count );
			count = ( protectedOption.mPhysicAttack		? ++count : count );
			count = ( protectedOption.mPhysicDefence	? ++count : count );
			count = ( protectedOption.mMagicAttack		? ++count : count );
			count = ( protectedOption.mMagicDefence		? ++count : count );	
			count = ( protectedOption.mCriticalRate		? ++count : count );
			count = ( protectedOption.mCriticalDamage	? ++count : count );
			count = ( protectedOption.mMoveSpeed		? ++count : count );
			count = ( protectedOption.mEvade			? ++count : count );
			count = ( protectedOption.mAccuracy			? ++count : count );

			if( 1 < count )
			{
				return eLog_ItemReinforceMaterialSourceItemCannnotReinforce;
			}
		}

		// 080228 LUJ, ��ȣ ������ ��� ������ ��ȭ �ɼ��� ���� ��ġ�� ������ ��ȭ��Ű�� �Ӽ��� ��ġ�ؾ���
		{
			// 080228 LUJ, ��ȣ ������ ��� ���� �˻�: �� ������ ������ ���Ǿ�� ��. ����־ �ȵ�
			if( materialMap.empty()	||
				1 < materialMap.size() )
			{
				return eLog_ItemReinforceMaterialSourceItemCannnotReinforce;
			}

			MaterialMap::const_iterator it = materialMap.begin();
			const ReinforceScript* script = g_CGameResourceManager.GetReinforceScript( it->first );

			if( ! script )
			{
				return eLog_ItemReinforceMaterialSourceItemCannnotReinforce;
			}

			// 080228 LUJ, �̸��� �ʱ�ȭ���� ������, ���� �� �׻� �����Ѵ�
			ZeroMemory(
				protectedOption.mMadeBy,
				sizeof(protectedOption.mMadeBy));
			
			ITEM_OPTION::Reinforce option = { 0 };

			switch( script->mType )
			{
			case ReinforceScript::eTypeStrength:
				{
					option.mStrength = protectedOption.mStrength;
					break;
				}
			case ReinforceScript::eTypeDexterity:
				{
					option.mDexterity = protectedOption.mDexterity;
					break;
				}
			case ReinforceScript::eTypeVitality:
				{
					option.mVitality = protectedOption.mVitality;
					break;
				}
			case ReinforceScript::eTypeWisdom:
				{
					option.mWisdom = protectedOption.mWisdom;
					break;
				}
			case ReinforceScript::eTypeIntelligence:
				{
					option.mIntelligence = protectedOption.mIntelligence;
					break;
				}
			case ReinforceScript::eTypeLife:
				{
					option.mLife = protectedOption.mLife;
					break;
				}
			case ReinforceScript::eTypeLifeRecovery:
				{
					option.mLifeRecovery = protectedOption.mLifeRecovery;
					break;
				}
			case ReinforceScript::eTypeMana:
				{
					option.mMana = protectedOption.mMana;
					break;
				}
			case ReinforceScript::eTypeManaRecovery:
				{
					option.mManaRecovery = protectedOption.mManaRecovery;
					break;
				}
			case ReinforceScript::eTypePhysicAttack:
				{
					option.mPhysicAttack = protectedOption.mPhysicAttack;
					break;
				}
			case ReinforceScript::eTypePhysicDefence:
				{
					option.mPhysicDefence = protectedOption.mPhysicDefence;
					break;
				}
			case ReinforceScript::eTypeMagicAttack:
				{
					option.mMagicAttack = protectedOption.mMagicAttack;
					break;
				}
			case ReinforceScript::eTypeMagicDefence:
				{
					option.mMagicDefence = protectedOption.mMagicDefence;
					break;
				}
			case ReinforceScript::eTypeMoveSpeed:
				{
					option.mMoveSpeed = protectedOption.mMoveSpeed;
					break;
				}
			case ReinforceScript::eTypeEvade:
				{
					option.mEvade = protectedOption.mEvade;
					break;
				}
			case ReinforceScript::eTypeAccuracy:
				{
					option.mAccuracy = protectedOption.mAccuracy;
					break;
				}
			case ReinforceScript::eTypeCriticalRate:
				{
					option.mCriticalRate = protectedOption.mCriticalRate;
					break;
				}
			case ReinforceScript::eTypeCriticalDamage:
				{
					option.mCriticalDamage = protectedOption.mCriticalDamage;
					break;
				}
			}

			if( memcmp( &option, &protectedOption, sizeof( option ) ) )
			{
				return eLog_ItemReinforceMaterialSourceItemCannnotReinforce;
			}
		}
	}

	// 080929 LUJ, ���� �������� �ִ� ��� �˻�/�Ҹ��Ѵ�
	if( input.mSupportItem.wIconIdx )
	{
		const ITEMBASE* item = playerSlot->GetItemInfoAbs( input.mSupportItem.Position );

		if( ! item )
		{
			return eLog_ItemReinforceInvalidSupportItem;
		}
		else if(	IsDupItem( item->wIconIdx  ) &&
					1 < item->Durability )
		{
			if( EI_TRUE != playerSlot->UpdateItemAbs(
					player,
					item->Position,
					item->dwDBIdx,
					item->wIconIdx,
					item->Position,
					item->QuickPosition,
					item->Durability - 1 ) )
			{
				return eLog_ItemReinforceInvalidSupportItem;
			}
		}
		else
		{
			if( EI_TRUE != playerSlot->DeleteItemAbs(
					player,
					item->Position,
					0 ) )
			{
				return eLog_ItemReinforceInvalidSupportItem;
			}
		}

		const ReinforceSupportScript* script = g_CGameResourceManager.GetReinforceSupportScript( input.mSupportItem.wIconIdx );

		if( ! script )
		{
			return eLog_ItemReinforceNoSupportScript;
		}

		// 080929 LUJ, ���� �����۰� ��� ������ �������� Ȯ���Ѵ�
		for( DWORD i = 0; i < input.mSize; ++i )
		{
			const ITEMBASE& materialItem = input.mItem[ i ];

			if( script->mMaterialSet.end() == script->mMaterialSet.find( materialItem.wIconIdx ) )
			{
				return eLog_ItemReinforceInvalidMaterialForSupportItem;
			}
		}
	}

	ITEM_OPTION		option		= { 0 };
	const BOOL		isRareItem	= IsRare( *sourceItemInfo );

	for(
		MaterialMap::const_iterator it = materialMap.begin();
		materialMap.end() != it;
		++it )
	{
		const DWORD				itemIndex	= it->first;
		const DWORD				quantity	= it->second;
		const ReinforceScript*	script		= g_CGameResourceManager.GetReinforceScript( itemIndex );

		// 080707 LUJ, �� ������ ���� ���� �˻�
		if( ! script )
		{
			return eLog_ItemReinforceMaterialSourceItemCannnotReinforce;
		}
		// 090115 LUJ, ������ ���� ���ο� ���� �´� ��Ḧ ������ �˻�
		// 090205 LUJ, ���� �ڵ� �߰�
		else if( isRareItem != script->mForRare )
		{
			LogScriptHack(
				*player,
				"ItemReinforce.bin",
				"wrong rare" );
			return eLog_ItemReinforceFail;
		}

		if(script->mSlot.end() == script->mSlot.find(sourceItemInfo->EquipSlot))
		{
			LogScriptHack(
				*player,
				"ItemReinforce.bin",
				"wrong slot" );
			return eLog_ItemReinforceFail;
		}

		DWORD value	= 0;
		const float	random = float( rand() % 100 + 1 );
	
		if(const ReinforceSupportScript* supportScript	= g_CGameResourceManager.GetReinforceSupportScript( input.mSupportItem.wIconIdx ))
		{	
			const float bonus = supportScript->mBonusRange.mMin + abs( supportScript->mBonusRange.mMax - supportScript->mBonusRange.mMin ) * mRandom.GetFloat();
	
			value = DWORD( floor( 0.5f + bonus + supportScript->mBias * random * float( quantity ) / 100.0f ) );
			value = max( int( supportScript->mValueRange.mMin ), int( value ) );
			value = min( int( supportScript->mValueRange.mMax ), int( value ) );
		}
		else if( 0 < ( rand() % 101 - sourceItemInfo->LimitLevel / 2 ) )
		{	
			value = DWORD( floor( 0.5f + script->mBias * random * float( quantity ) / 100.0f ) );
			value = min( script->mMax, value );
		}

		ITEM_OPTION::Reinforce&	reinforce	= option.mReinforce;

		switch( script->mType )
		{
		case ReinforceScript::eTypeStrength:
			{
				reinforce.mStrength			= max( protectedOption.mStrength, value );
				break;
			}
		case ReinforceScript::eTypeDexterity:
			{
				reinforce.mDexterity		= max( protectedOption.mDexterity, value );
				break;
			}
		case ReinforceScript::eTypeVitality:
			{
				reinforce.mVitality			= max( protectedOption.mVitality, value );
				break;
			}
		case ReinforceScript::eTypeWisdom:
			{
				reinforce.mWisdom			= max( protectedOption.mWisdom, value );
				break;
			}
		case ReinforceScript::eTypeIntelligence:
			{
				reinforce.mIntelligence		= max( protectedOption.mIntelligence, value );
				break;
			}
		case ReinforceScript::eTypeLife:
			{
				reinforce.mLife				= max( protectedOption.mLife, value );
				break;
			}
		case ReinforceScript::eTypeLifeRecovery:
			{
				reinforce.mLifeRecovery		= max( protectedOption.mLifeRecovery, value );
				break;
			}
		case ReinforceScript::eTypeMana:
			{
				reinforce.mMana				= max( protectedOption.mMana, value );
				break;
			}
		case ReinforceScript::eTypeManaRecovery:
			{
				reinforce.mManaRecovery		= max( protectedOption.mManaRecovery, value );
				break;
			}
		case ReinforceScript::eTypePhysicAttack:
			{
				reinforce.mPhysicAttack		= max( protectedOption.mPhysicAttack, value );
				break;
			}
		case ReinforceScript::eTypePhysicDefence:
			{
				reinforce.mPhysicDefence	= max( protectedOption.mPhysicDefence, value );
				break;
			}
		case ReinforceScript::eTypeMagicAttack:
			{
				reinforce.mMagicAttack		= max( protectedOption.mMagicAttack, value );
				break;
			}
		case ReinforceScript::eTypeMagicDefence:
			{
				reinforce.mMagicDefence		= max( protectedOption.mMagicDefence, value );
				break;
			}
		case ReinforceScript::eTypeMoveSpeed:
			{
				reinforce.mMoveSpeed		= max( protectedOption.mMoveSpeed, value );
				break;
			}
		case ReinforceScript::eTypeEvade:
			{
				reinforce.mEvade			= max( protectedOption.mEvade, value );
				break;
			}
		case ReinforceScript::eTypeAccuracy:
			{
				reinforce.mAccuracy			= max( protectedOption.mAccuracy, value );
				break;
			}
		case ReinforceScript::eTypeCriticalRate:
			{
				reinforce.mCriticalRate		= max( protectedOption.mCriticalRate, value );
				break;
			}
		case ReinforceScript::eTypeCriticalDamage:
			{
				reinforce.mCriticalDamage	= max( protectedOption.mCriticalDamage, value );
				break;
			}
		default:
			{
				ASSERT( 0 && "invaild script. check it now" );
				break;
			}
		}
	}

	BOOL isFailed = FALSE;

	// ��ȭ�� �ɼǰ��� �ִ��� üũ�ϰ� ������ DB�� ��������
	{
		const ITEM_OPTION emptyOption = { 0 };

		ASSERT( sizeof( emptyOption ) == sizeof( option ) );

		// �޸� ���� ��ȭ ���� ��ġ��. �� ��ȭ ����.
		if( ! memcmp( &option, &emptyOption, sizeof( option ) ) )
		{
			LogItemMoney(
				player->GetID(),
				player->GetObjectName(),
				0,
				"",
				eLog_ItemReinforceFail, 
				player->GetMoney(),
				0,
				0,
				sourceItem->wIconIdx,
				sourceItem->dwDBIdx,
				sourceItem->Position,
				sourceItem->Position,
				sourceItem->Durability,
				0 );

			isFailed = TRUE;
		}
		// ���� ������ �ɼ� ó��. ��æƮ�� �������� ���� ��ġ�� ��ȭ �Ŀ��� �����ؾ� �Ѵ�.
		else
		{
			const ITEM_OPTION& previousOption = GetOption( *sourceItem );

			option.mItemDbIndex	= sourceItem->dwDBIdx;

			if( previousOption.mItemDbIndex )
			{
				// ��æƮ�� �������� ���� �߰��� �����־�� �Ѵ�
				option.mEnchant = previousOption.mEnchant;

				if(const EnchantScript* script	= g_CGameResourceManager.GetEnchantScript( option.mEnchant.mIndex ))
				{
					g_CGameResourceManager.AddEnchantValue(
						option,
						script->mAbility,
						sourceItemInfo->EnchantDeterm * option.mEnchant.mLevel);
				}

				option.mMix	= previousOption.mMix;
				option.mDrop = previousOption.mDrop;
			}

			SafeStrCpy(
				option.mReinforce.mMadeBy,
				player->GetObjectName(),
				sizeof( option.mReinforce.mMadeBy ) );

			option = LimiterItemOption(option, sourceItemInfo, player, isRareItem);

			// 081118 LUJ, ĳ���� ��ȣ�� ������ ������ DB �����带 ������ Ű������ ���ǵ��� ��
			ItemOptionInsert( option, player->GetID() );
			AddOption( option );

			// 080428 LUJ, �α�
			LogItemOption(
				player->GetID(),
				option.mItemDbIndex,
				option,
				eLog_ItemReinforceSuccess );
			// 081202 LUJ, �븸�� ��û���� ��ȭ ������ �������� �α׸� �ۼ��Ѵ�
			LogItemMoney(
				player->GetID(),
				player->GetObjectName(),
				0,
				"*reinforce",
				eLog_ItemReinforceSuccess, 
				player->GetMoney(),
				0,
				0,
				sourceItem->wIconIdx,
				sourceItem->dwDBIdx,
				sourceItem->Position,
				sourceItem->Position,
				sourceItem->Durability,
				0 );
		}
	}

	MSG_ITEM_REINFORCE_ACK message;
	{
		ZeroMemory( &message, sizeof( message ) );

		message.Category			= MP_ITEM;
		message.Protocol			= (BYTE)( isFailed ? MP_ITEM_REINFORCE_FAILED_ACK : MP_ITEM_REINFORCE_SUCCEED_ACK );
		message.mSourceItemDbIndex	= input.mSourceItemDbIndex;
	}

	// ���Ե� �޽����� �����Ѵ�. �ٸ� DB �ε����� ���������� ��� ���̴�
	// ���� ������ �������� ������ �ε����� ����. ������Ʈ�� �������� �����۸� �ش�ȴ�
	for( POSTYPE position = inventoryStartPosition; inventoryEndPosition > position; ++position )
	{
		const ITEMBASE*	item;
		{
			item = playerSlot->GetItemInfoAbs( position );

			// DB �ε����� ������ �� ����
			if( ! item ||
				! item->dwDBIdx )
			{
				item = 0;
			}
		}

		const ITEMBASE*	backupItem = backupSlot->GetItem( position );

		if( ! item )
		{
			// �������� ���µ�, �̷� ���Կ��� ������ ��� ������ ��Ȳ
			// �Ѵ� ���� ��� ������Ʈ�� �ʿ� ����
			if( backupItem )
			{
				// 071231 KTH -- GetID add
				ItemDeleteToDB( backupItem->dwDBIdx );

				message.mItem[ message.mSize++ ].dwDBIdx = backupItem->dwDBIdx;

				// 071220 LUJ
				LogItemMoney(
					player->GetID(),
					player->GetObjectName(),
					0,
					"",
					eLog_ItemDestroyReinforce,
					player->GetMoney(),
					0,
					0,
					backupItem->wIconIdx,
					backupItem->dwDBIdx,
					backupItem->Position,
					0,
					backupItem->Durability,
					0 );
			}

			continue;
		}

		// �������� �ִµ�, �̷� ���԰� ������ ������ �ٸ��� ������Ʈ�� ��Ȳ
		if( item->wIconIdx		!= backupItem->wIconIdx ||
			item->Durability	!= backupItem->Durability )
		{
			ItemUpdateToDB(
				player->GetID(),
				item->dwDBIdx,
				item->wIconIdx,
				item->Durability,
				item->Position,
				item->QuickPosition );

			message.mItem[ message.mSize++ ] = *item;

			// 071220, LUJ. ������ �ٸ��� mirrorItem�� �Һ�Ǿ�, item�� ��� �����
			if( item->wIconIdx != backupItem->wIconIdx )
			{
				LogItemMoney(
					player->GetID(),
					player->GetObjectName(),
					0,
					"",
					eLog_ItemDestroyReinforce,
					player->GetMoney(),
					0,
					0,
					backupItem->wIconIdx,
					backupItem->dwDBIdx,
					backupItem->Position,
					0,
					backupItem->Durability,
					0 );

				LogItemMoney(
					player->GetID(),
					player->GetObjectName(),
					0,
					"",
					eLog_ItemReinforceSuccess,
					player->GetMoney(),
					0,
					0,
					item->wIconIdx,
					item->dwDBIdx,
					item->Position,
					0,
					item->Durability,
					0 );
			}
			else if( IsDupItem( item->wIconIdx ) )
			{
				const BOOL isUsed = ( backupItem->Durability > item->Durability );

				LogItemMoney(
					player->GetID(),
					player->GetObjectName(),
					0,
					"",
					isUsed ? eLog_ItemDestroyReinforce : eLog_ItemReinforceSuccess,
					player->GetMoney(),
					0,
					0,
					item->wIconIdx,
					item->dwDBIdx,
					item->Position,
					0,
					item->Durability,
					0 );
			}
			else
			{
				ASSERT( 0 && "update error" );
			}
		}
	}
	
	player->SendMsg( &message, message.GetSize() );

	// ��� ����
	if( isFailed )
	{
		return eLog_ItemReinforceFail;
	}
	else
	{
		MSG_ITEM_REINFORCE_OPTION_ACK m;
		m.Category		= MP_ITEM;
		m.Protocol		= MP_ITEM_REINFORCE_OPTION_ACK;
		m.mItemDbIndex	= sourceItem->dwDBIdx;
		m.mOption		= GetOption( *sourceItem );

		player->SendMsg( &m, sizeof( m ) );
		
		return eLog_ItemReinforceSuccess;
	}	
}


// 080228 LUJ, ��ȣ ������ ��� �� ��æƮ ���� �ÿ��� �Ҹ���� �ʵ��� ��
// 080320 LUJ, ��æƮ ���� ��ȣ ��� �߰�
MP_PROTOCOL_ITEM CItemManager::Enchant(CPlayer* player, const MSG_ITEM_ENCHANT_SYN* message, eLogitemmoney& log )
{
	const DWORD		sourceIndex			= message->wItemIdx;
	const DWORD		materialIndex		= message->wMaterialItemIdx;
	const POSTYPE	sourcePosition		= message->ItemPos;
	const POSTYPE	materialPosition	= message->MaterialItemPos;

	if( !	player																				||
		!	CHKRT->ItemOf(player, sourcePosition, sourceIndex, 0,0, CB_EXIST|CB_ICONIDX)		||
		!	CHKRT->ItemOf(player, materialPosition, materialIndex, 0,0, CB_EXIST|CB_ICONIDX)	||
		!	player->IsInventoryPosition( sourcePosition ) )
	{
		log = eLog_ItemEnchantHacking;

		return MP_ITEM_ENCHANT_NACK;
	}

	const ITEM_INFO* sourceInfo	= GetItemInfo( sourceIndex );

	if( !	sourceInfo					||
		!	sourceInfo->IsEnchant		||
			sourceInfo->Stack			||
		!	IsDupItem( materialIndex ) )
	{
		log = eLog_ItemEnchantHacking;

		return MP_ITEM_ENCHANT_NACK;
	}

	const EnchantScript* const script = g_CGameResourceManager.GetEnchantScript(
		materialIndex);

	if( !	script												||
			script->mItemLevel.mMin > sourceInfo->LimitLevel	||
			script->mItemLevel.mMax < sourceInfo->LimitLevel	)
	{
		log = eLog_ItemEnchantHacking;

		return MP_ITEM_ENCHANT_NACK;
	}

	CItemSlot* inventory = player->GetSlot( eItemTable_Inventory );

	// 080228 LUJ, ��ȣ ������ ������
	const EnchantProtection::Data* protectionData = 0;
	
	// 080228 LUJ, ��ȣ �������� ���� ��� ���
	if( message->mProtectionItem.wIconIdx )
	{
		const ITEMBASE* sourceItem = inventory->GetItemInfoAbs( sourcePosition );

		if( ! sourceItem )
		{
			return MP_ITEM_ENCHANT_NACK;
		}
		
		const EnchantProtection::ItemMap::const_iterator it = g_CGameResourceManager.GetEnchantProtection().mItemMap.find(
			message->mProtectionItem.wIconIdx);

		// 080320 LUJ, ��æƮ ��ȣ ���¶�� ���� ������ �´��� üũ
		if(g_CGameResourceManager.GetEnchantProtection().mItemMap.end() != it)
		{
			const ITEM_OPTION& option = GetOption( *sourceItem );

			protectionData = &( it->second );

			if( option.mEnchant.mLevel < protectionData->mEnchantLevelRange.mMin ||
				option.mEnchant.mLevel > protectionData->mEnchantLevelRange.mMax )
			{
				return MP_ITEM_ENCHANT_NACK;
			}
			// 080328 LUJ, ������ ���� ���� üũ
			else if(	sourceInfo->LimitLevel < protectionData->mItemLimitLevelRange.mMin	||
						sourceInfo->LimitLevel > protectionData->mItemLimitLevelRange.mMax )
			{
				return MP_ITEM_ENCHANT_NACK;
			}
		}

		const ITEMBASE* item = inventory->GetItemInfoAbs( message->mProtectionItem.Position );

		if( !	item											||
				item->dwDBIdx != message->mProtectionItem.dwDBIdx ||
				item->wIconIdx != message->mProtectionItem.wIconIdx )
		{
			return MP_ITEM_ENCHANT_NACK;
		}
		else if( EI_TRUE != DiscardItem(
			player,
			item->Position,
			item->wIconIdx, 1 ) )
		{
			return MP_ITEM_ENCHANT_NACK;
		}

		// 080428 LUJ, ��ȣ ������ ��� �α�
		LogItemMoney(
			player->GetID(),
			player->GetObjectName(),
			0,
			"*protect",
			eLog_ItemUse,
			player->GetMoney(),
			0,
			0,
			message->mProtectionItem.wIconIdx,
			message->mProtectionItem.dwDBIdx,
			message->mProtectionItem.Position,
			0,
			1,
			0 );
	}

	ITEMBASE materialForLog;
	ZeroMemory( &materialForLog, sizeof( materialForLog ) );
	{
		const ITEMBASE* material = inventory->GetItemInfoAbs( materialPosition );

		if( material )
		{
			materialForLog = *material;
		}
	}
	
	// ��� �ı�
	if( EI_TRUE != DiscardItem( player, materialPosition, materialIndex, 1 ) )
	{
		log = eLog_ItemEnchantServerError;

		return MP_ITEM_ENCHANT_NACK;
	}
	else if(script->mSlot.end() == script->mSlot.find(sourceInfo->EquipSlot))
	{
		log = eLog_ItemEnchantHacking;

		return MP_ITEM_ENCHANT_NACK;
	}

	// ��æƮ�� �ҿ�� ������ �α�
	LogItemMoney(
		player->GetID(),
		player->GetObjectName(),
		0,
		"",
		eLog_ItemUse, 
		player->GetMoney(),
		0,
		0,
		materialForLog.wIconIdx,
		materialForLog.dwDBIdx,
		materialForLog.Position,
		0, 
		1,
		0 );

	const ITEMBASE*	source	= inventory->GetItemInfoAbs( sourcePosition );
	
	ITEMBASE sourceForLog;
	{
		if( source )
		{
			sourceForLog = *source;
		}
		else
		{
			ZeroMemory( &sourceForLog, sizeof( sourceForLog ) );
		}
	}

	ITEM_OPTION	option	= GetOption( sourceForLog );

	if( script->mEnchantMaxLevel == option.mEnchant.mLevel )
	{
		return MP_ITEM_ENCHANT_FAILED_ACK;
	}

	enum
	{
		eEnchantResultNotify_Fail = 0,
		eEnchantResultNotify_Success,
	};

	// ���� ���� ����
	{
		float basis = g_CGameResourceManager.GetEnchantSuccessRate(
			option.mEnchant.mLevel);

		if(0 == basis)
		{
			basis = 0.85f - ( (option.mEnchant.mLevel + 1) * 0.005f );
			basis = 1000.0f * pow( basis, option.mEnchant.mLevel );
		}

		const int randomValue = rand() % 1000;
		int value = int( basis ) + ( protectionData ? protectionData->mAddedPercent : 0 );

		if( randomValue > value )
		{
			// 080328 LUJ, ��ȣ ������ �� ������ ���, ���̳ʽ� ���ʽ��� �����Ѵ�
			if( protectionData )
			{
				// 080328 LUJ, std::map::lower_bound()�� ����ϸ� �������� ���� ���� �� �ִ�. �ڼ��� ������ STL Ʃ�丮�� ����
				EnchantProtection::Data::BonusMap::const_iterator bonus_it = protectionData->mMinusBonusMap.lower_bound( mRandom.GetFloat() );

				if( protectionData->mMinusBonusMap.end() != bonus_it )
				{
					const BYTE previousEnchantLevel = option.mEnchant.mLevel;

					// 080328 LUJ,	��æƮ ���� ����.
					//				����! ���� �� ���� ������ ���� ��æƮ �������� ���� �� �ִ�. �̶��� ���� ������ �����ؼ��� �ȵȴ�
					{
						const int bonusLevel	= bonus_it->second;
                        const int enchantLevel	= BYTE( max( int( option.mEnchant.mLevel ) + bonusLevel, int( protectionData->mMinimumEnchantLevel ) ) );

						option.mEnchant.mLevel	= BYTE( min( enchantLevel, previousEnchantLevel ) );
					}

					// 080328 LUJ, ���ҵ� ��ŭ ��æƮ ������ ����
					g_CGameResourceManager.AddEnchantValue( option, script->mAbility, -1 * ( previousEnchantLevel - option.mEnchant.mLevel ) * sourceInfo->EnchantDeterm );

					// 080328 LUJ, ��æƮ ������ ���ٸ� ��æƮ ������ �ʱ�ȭ����
					if( ! option.mEnchant.mLevel )
					{
						ZeroMemory( &option.mEnchant, sizeof( option.mEnchant ) );
					}

					const BOOL isRareItem	= IsRare( *sourceInfo );
					option = LimiterItemOption(option, sourceInfo, player, isRareItem);
					
					// 080320 LUJ, �޸�/DB ������Ʈ
					AddOption( option );
					// 081118 LUJ, ĳ���� ��ȣ�� ������ ������ DB �����带 ������ Ű������ ���ǵ��� ��
					ItemOptionInsert( option, player->GetID() );
					// 080428 LUJ, �α�
					LogItemOption(
						player->GetID(),
						option.mItemDbIndex,
						option,
						eLog_ItemEnchantDestroy );

					MSG_ITEM_OPTION message;
					ZeroMemory( &message, sizeof( message ) );
					message.Category					= MP_ITEM;
					message.Protocol					= MP_ITEM_OPTION_ADD_ACK;
					message.mOption[ message.mSize++ ]	= option;

					player->SendMsg( &message, message.GetSize() );
				}

				LogItemMoney(
					player->GetID(),
					player->GetObjectName(),
					0,
					"",
					eLog_ItemEnchantProtected,
					player->GetMoney(),
					0,
					0,
					sourceForLog.wIconIdx,
					sourceForLog.dwDBIdx,
					sourceForLog.Position,
					0,
					1,
					0);

				return MP_ITEM_ENCHANT_FAILED_ACK;
			}
			// 090917 ShinJS --- ��ȣ �������� ���� ���
            else
			{
				const stEnchantFailSetting* const pFailSetting = g_CGameResourceManager.GetEnchantFailSetting(
					sourceInfo->kind);

				if( !pFailSetting )
				{
					// ��� ������ �ı�
					if( EI_TRUE != DiscardItem( player, sourcePosition, sourceIndex, 1 ) )
					{
						log = eLog_ItemEnchantServerError;
						return MP_ITEM_ENCHANT_NACK;
					}

					// ��� ������ �ı� �α�
					LogItemMoney(
						player->GetID(),
						player->GetObjectName(),
						0,
						"",
						eLog_ItemEnchantDestroy, 
						player->GetMoney(),
						0,
						0,
						sourceForLog.wIconIdx,
						sourceForLog.dwDBIdx,
						sourceForLog.Position,
						0, 
						1,
						0 );
					// 091102 LUJ, ��� ��û���� ��æƮ ���� �� �ɼ� �α� �߰�
					LogItemOption(
						player->GetID(),
						option.mItemDbIndex,
						option,
						eLog_ItemEnchantDestroy);
					return MP_ITEM_ENCHANT_FAILED_ACK;
				}

				BYTE byCurEnchantLv = option.mEnchant.mLevel;
				BOOL bChangeLv = FALSE;

				// ��æƮ ���� ���� ���� �ľ�
				if( pFailSetting->dwManusMinLv <= byCurEnchantLv && byCurEnchantLv <= pFailSetting->dwManusMaxLv )
				{
					// ��æƮ ���� ����
					option.mEnchant.mLevel = (byCurEnchantLv > 0 ? byCurEnchantLv - 1 : 0);
					bChangeLv = TRUE;
				}

				// ��æƮ ���� 0 ���� ���� �ľ�
				if( pFailSetting->dwZeroMinLv <= byCurEnchantLv && byCurEnchantLv <= pFailSetting->dwZeroMaxLv )
				{
					option.mEnchant.mLevel = 0;
					bChangeLv = TRUE;
				}

				// ��æƮ ������ ����� ��� �ɼ� ����
				if( bChangeLv )
				{
					// ���ҵ� ��ŭ ��æƮ ��ġ ����
					g_CGameResourceManager.AddEnchantValue( option, script->mAbility, -1 * ( byCurEnchantLv - option.mEnchant.mLevel ) * sourceInfo->EnchantDeterm );

					// ��æƮ ������ ���ٸ� ��æƮ ������ �ʱ�ȭ
					if( ! option.mEnchant.mLevel )
						ZeroMemory( &option.mEnchant, sizeof( option.mEnchant ) );

					const BOOL isRareItem	= IsRare( *sourceInfo );
					option = LimiterItemOption(option, sourceInfo, player, isRareItem);
					
					// �޸�/DB ������Ʈ
					AddOption( option );

					// ĳ���� ��ȣ�� ������ ������ DB �����带 ������ Ű������ ���ǵ��� ��
					ItemOptionInsert( option, player->GetID() );

					// �α�
					LogItemOption(
						player->GetID(),
						option.mItemDbIndex,
						option,
						eLog_ItemEnchantLevelChanged);

					TCHAR text[MAX_NAME_LENGTH] = {0};
					_sntprintf(
						text,
						_countof(text),
						"level:%d -> %d",
						byCurEnchantLv,
						option.mEnchant.mLevel);
					LogItemMoney(
						player->GetID(),
						player->GetObjectName(),
						0,
						text,
						eLog_ItemEnchantLevelChanged,
						player->GetMoney(),
						0,
						0,
						sourceForLog.wIconIdx,
						sourceForLog.dwDBIdx,
						sourceForLog.Position,
						0,
						1,
						0);

					MSG_ITEM_OPTION message;
					ZeroMemory( &message, sizeof( message ) );
					message.Category					= MP_ITEM;
					message.Protocol					= MP_ITEM_OPTION_ADD_ACK;
					message.mOption[ message.mSize++ ]	= option;

					player->SendMsg( &message, message.GetSize() );
				}

				// ������ ���� ���� �ľ�
				if( pFailSetting->dwDeleteItemMinLv <= byCurEnchantLv && byCurEnchantLv <= pFailSetting->dwDeleteItemMaxLv )
				{
					// ��� ������ �ı�
					if( EI_TRUE != DiscardItem( player, sourcePosition, sourceIndex, 1 ) )
					{
						log = eLog_ItemEnchantServerError;
						return MP_ITEM_ENCHANT_NACK;
					}

					// ��� ������ �ı� �α�
					LogItemMoney(
						player->GetID(),
						player->GetObjectName(),
						0,
						"",
						eLog_ItemEnchantDestroy, 
						player->GetMoney(),
						0,
						0,
						sourceForLog.wIconIdx,
						sourceForLog.dwDBIdx,
						sourceForLog.Position,
						0, 
						1,
						0 );

					if( option.mEnchant.mLevel >= 10 )
					{
						MSG_NAME_DWORD3 msg;
						ZeroMemory( &msg, sizeof(msg) );
						msg.Category		= MP_ITEM;
						msg.Protocol		= MP_ITEM_ENCHANT_LEVEL_NOTIFY;
						msg.dwObjectID		= player->GetID();
						msg.dwData1			= sourceIndex;
						msg.dwData2			= option.mEnchant.mLevel + 1;
						msg.dwData3			= eEnchantResultNotify_Fail;
						SafeStrCpy( msg.Name, player->GetObjectName(), MAX_NAME_LENGTH+1 );
						player->SendMsg(&msg, sizeof(msg));
					}
				}
			}

			return MP_ITEM_ENCHANT_FAILED_ACK;
		}
	}

	const MONEYTYPE enchantCost = g_CGameResourceManager.GetEnchantCost(
		*sourceInfo);

	if( player->SetMoney( enchantCost, MONEY_SUBTRACTION, MF_NOMAL, eItemTable_Inventory, eMoneyLog_Enchant ) != enchantCost )
	{
		log = eLog_ItemEnchantNotEnoughMoney;
		return MP_ITEM_ENCHANT_NACK;
	}

	// 080328 LUJ, ��æƮ ������ 1 ������Ű��, ��ȣ ������ �� ���ʽ� ������ �߰��� ������Ų��
	{
		const int previousEnchantLevel = option.mEnchant.mLevel;

		++option.mEnchant.mLevel;
		option.mEnchant.mIndex	= materialIndex;

		// 080328 LUJ, ��æƮ ��ȣ �����϶� ������ ��� �߰� ���ʽ��� �ο��Ѵ�
		if( protectionData )
		{
			// 080328 LUJ, std::map::lower_bound()�� ����ϸ� �������� ���� ���� �� �ִ�. �ڼ��� ������ STL Ʃ�丮�� ����
			EnchantProtection::Data::BonusMap::const_iterator it = protectionData->mPlusBonusMap.lower_bound( mRandom.GetFloat() );

			const int bonusLevel	= ( protectionData->mPlusBonusMap.end() == it ? 0 : it->second );
			option.mEnchant.mLevel	= BYTE( min( int( option.mEnchant.mLevel ) + bonusLevel, int( script->mEnchantMaxLevel ) ) );
		}	

		g_CGameResourceManager.AddEnchantValue(
			option,
			script->mAbility,
			(option.mEnchant.mLevel - previousEnchantLevel) * sourceInfo->EnchantDeterm);
	}

	CItemSlot* sourceSlot = player->GetSlot( sourcePosition );

	if( EI_TRUE != sourceSlot->UpdateItemAbs( player, sourcePosition, 0, 0, 0, 0, 0, UB_DURA ) )
	{
		log = eLog_ItemEnchantItemUpdateFailed;

		return MP_ITEM_ENCHANT_NACK;
	}

	SafeStrCpy( option.mEnchant.mMadeBy, player->GetObjectName(), sizeof( option.mEnchant.mMadeBy ) );

	option.mItemDbIndex	= sourceForLog.dwDBIdx;

	const BOOL isRareItem	= IsRare( *sourceInfo );
	option = LimiterItemOption(option, sourceInfo, player, isRareItem);

	// 081118 LUJ, ĳ���� ��ȣ�� ������ ������ DB �����带 ������ Ű������ ���ǵ��� ��
	ItemOptionInsert( option, player->GetID() );
	AddOption( option );

	// 080428 LUJ, �α�
	LogItemOption(
		player->GetID(),
		option.mItemDbIndex,
		option,
		eLog_ItemEnchantSuccess );

	// �޽��� ����
	{
		MSG_ITEM_ENCHANT_ACK m;
		m.Category		= MP_ITEM;
		m.Protocol		= MP_ITEM_ENCHANT_SUCCESS_ACK;
		m.TargetPos		= sourcePosition;
		m.OptionInfo	= option;

		player->SendMsg( &m, sizeof( m ) );
	}

	{
		char text[ MAX_NAME_LENGTH + 1 ] = { 0 };

		sprintf( text, "enchant:%d", option.mEnchant.mLevel );

		// ��æƮ ���� �α�
		LogItemMoney(
			player->GetID(),
			player->GetObjectName(),
			0,
			text,
			eLog_ItemEnchantSuccess, 
			player->GetMoney(),
			0,
			0,
			sourceForLog.wIconIdx,
			sourceForLog.dwDBIdx,
			sourceForLog.Position,
			0, 
			1,
			0 );
	}

	// 100609 ONS ��äƮ ������ +10�̻��� ��� ������ ����.
	if( option.mEnchant.mLevel >= 10 )
	{
		MSG_NAME_DWORD3 msg;
		ZeroMemory( &msg, sizeof(msg) );
		msg.Category		= MP_ITEM;
		msg.Protocol		= MP_ITEM_ENCHANT_LEVEL_NOTIFY;
		msg.dwObjectID		= player->GetID();
		msg.dwData1			= sourceIndex;
		msg.dwData2			= option.mEnchant.mLevel;
		msg.dwData3			= eEnchantResultNotify_Success;
		SafeStrCpy( msg.Name, player->GetObjectName(), MAX_NAME_LENGTH+1 );
		player->SendMsg(&msg, sizeof(msg));
	}

	return MP_ITEM_ENCHANT_SUCCESS_ACK;
}


ITEM_INFO * CItemManager::GetItemInfo(DWORD wItemIdx)
{
	return m_ItemInfoList.GetData(wItemIdx);
}


void CItemManager::LoadItemList()
{
	ITEM_INFO * pInfo = NULL;
	CMHFile file;
	file.Init(
		"System/Resource/ItemList.bin",
		"rb");

	while(FALSE == file.IsEOF())
	{
		const DWORD ItemIdx = file.GetDword();

		if(0 == ItemIdx)
		{
			continue;
		}
		else if(m_ItemInfoList.GetData(ItemIdx) == NULL)
		{
			pInfo = new ITEM_INFO;
			SetItemInfo(ItemIdx,pInfo,&file);
			m_ItemInfoList.Add(pInfo, pInfo->ItemIdx);
		}
	}
}

// desc_hseos_ü����������01
// S ü���������� �߰� added by hseos 2007.07.29
// -------------------------------------------------------------------------------------------------------------------------------------
//  LoadScriptFileDataChangeItem Method													 		   ü���� ������ ���� ��ũ��Ʈ ���� �ε�
//
BOOL CItemManager::LoadScriptFileDataChangeItem()
{
	char szFile[MAX_PATH] = "./System/Resource/ChangeItem.bin";
	char szLine[MAX_PATH], szTxt[MAX_PATH];
	int	 nKind = 0, nResult = 0, nResultBase = 0;
	CMHFile fp;

	// ü���� ������ �ִ� ���� ���
	fp.Init(szFile, "rb");
	if(!fp.IsInited())
	{
		return FALSE;
	}

	while(TRUE)
	{
		if (fp.IsEOF()) break;
		fp.GetLine(szLine, sizeof(szLine));
		if (strstr(szLine, "//") ||
			IsEmptyLine(szLine))
		{
			continue;			
		}
		else if (strstr(szLine, "END_KIND")) 
		{
			m_nChangeItemInfoNum++;
			continue;
		}
	}

	fp.Release();

	m_stChangeItemInfo = new stCHANGEITEM[m_nChangeItemInfoNum];
	ZeroMemory(m_stChangeItemInfo, sizeof(stCHANGEITEM)*m_nChangeItemInfoNum);

	// ü���� ������ ��� �ִ� ���� ���
	fp.Init(szFile, "rb");
	if(!fp.IsInited())
	{
		return FALSE;
	}
	while(TRUE)
	{
		if (fp.IsEOF()) break;
		fp.GetLine(szLine, sizeof(szLine));
		if (strstr(szLine, "//") ||
			IsEmptyLine(szLine))
		{
			continue;			
		}
		else if (strncmp(szLine, "-", 1) == 0)
		{
			m_stChangeItemInfo[nKind].nResultNum++;
			continue;
		}
		else if (strstr(szLine, "END_KIND"))
		{
			m_stChangeItemInfo[nKind].nResultNum++;
			m_stChangeItemInfo[nKind].stResult = new stCHANGEITEM_RESULT[m_stChangeItemInfo[nKind].nResultNum];
			ZeroMemory(m_stChangeItemInfo[nKind].stResult, sizeof(*m_stChangeItemInfo[nKind].stResult)*m_stChangeItemInfo[nKind].nResultNum);
			nKind++;
			continue;
		}
	}

	fp.Release();

	// ü���� ������ ����� ������ �ִ� ���� ���
	nKind = 0; 
	nResult = 0;
	fp.Init(szFile, "rb");
	if(!fp.IsInited())
	{
		return FALSE;
	}
	while(TRUE)
	{
		if (fp.IsEOF()) break;
		fp.GetLine(szLine, sizeof(szLine));
		if (strstr(szLine, "//") ||
			IsEmptyLine(szLine))
		{
			continue;			
		}
		else if (strncmp(szLine, "-", 1) == 0)
		{
			if (nResult == 0) m_stChangeItemInfo[nKind].stResult[nResult].nResultBaseNum--;
			m_stChangeItemInfo[nKind].stResult[nResult].stResultBase = new stCHANGEITEM_RESULT_BASE[m_stChangeItemInfo[nKind].stResult[nResult].nResultBaseNum];
			ZeroMemory(m_stChangeItemInfo[nKind].stResult[nResult].stResultBase, sizeof(*m_stChangeItemInfo[nKind].stResult[nResult].stResultBase)*m_stChangeItemInfo[nKind].stResult[nResult].nResultBaseNum);
			nResult++;
			continue;			
		}
		else if (strstr(szLine, "END_KIND"))
		{
			if (nResult == 0) m_stChangeItemInfo[nKind].stResult[nResult].nResultBaseNum--;
			m_stChangeItemInfo[nKind].stResult[nResult].stResultBase = new stCHANGEITEM_RESULT_BASE[m_stChangeItemInfo[nKind].stResult[nResult].nResultBaseNum];
			ZeroMemory(m_stChangeItemInfo[nKind].stResult[nResult].stResultBase, sizeof(*m_stChangeItemInfo[nKind].stResult[nResult].stResultBase)*m_stChangeItemInfo[nKind].stResult[nResult].nResultBaseNum);
			nResult = 0;
			nKind++;
			continue;
		}

		m_stChangeItemInfo[nKind].stResult[nResult].nResultBaseNum++;
	}

	fp.Release();


	// ������ �б�
	nKind = 0;
	nResult = 0;
	nResultBase = -1;
	fp.Init(szFile, "rb");
	if(!fp.IsInited())
	{
		return FALSE;
	}
	while(TRUE)
	{
		if (fp.IsEOF()) break;
		fp.GetLine(szLine, sizeof(szLine));
		if (strstr(szLine, "//") ||
			IsEmptyLine(szLine))
		{
			continue;			
		}
		else if (strncmp(szLine, "-", 1) == 0)
		{
			nResult++;
			nResultBase = 0;
			continue;
		}
		else if (strstr(szLine, "END_KIND"))
		{
			nKind++;
			nResult = 0;
			nResultBase = -1;
			continue;
		}

		if (nResultBase == -1)
		{
			// ������ �б�
			sscanf(szLine, "%u", &m_stChangeItemInfo[nKind].nChangeItemID);
		}
		else
		{
			// ������ �б�
			sscanf(szLine, "%s %u %u %u %u", szTxt, 
				&m_stChangeItemInfo[nKind].stResult[nResult].stResultBase[nResultBase].nValue01, 
				&m_stChangeItemInfo[nKind].stResult[nResult].stResultBase[nResultBase].nValue02, 
				&m_stChangeItemInfo[nKind].stResult[nResult].stResultBase[nResultBase].nValue03, 
				&m_stChangeItemInfo[nKind].stResult[nResult].stResultBase[nResultBase].nProbaValue);

			if (nResultBase > 0)
			{
				m_stChangeItemInfo[nKind].stResult[nResult].stResultBase[nResultBase].nProbaValue += 
					m_stChangeItemInfo[nKind].stResult[nResult].stResultBase[nResultBase-1].nProbaValue;
			}

			// ��� ����
			if		(stricmp(szTxt, "ITEM") == 0)	m_stChangeItemInfo[nKind].stResult[nResult].stResultBase[nResultBase].eResultKind = TRK_ITEM;
			else if (stricmp(szTxt, "MONEY") == 0)	m_stChangeItemInfo[nKind].stResult[nResult].stResultBase[nResultBase].eResultKind = TRK_MONEY;
		}
		nResultBase++;
	}

	fp.Release();
	return TRUE;
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  IsChangeItem Method																								 ü���� ������ �ΰ�?
//
BOOL CItemManager::IsChangeItem(DWORD nItemID)
{
	if (m_stChangeItemInfo)
	{
		for(int i=0; i<m_nChangeItemInfoNum; i++)
		{
			if (nItemID == (DWORD)m_stChangeItemInfo[i].nChangeItemID)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  ProcChangeItem Method																							  ü���� ������ ó��
//
BOOL CItemManager::ProcChangeItem(CPlayer* pPlayer, DWORD nItemID)
{
	if (m_stChangeItemInfo)
	{
		for(int i=0; i<m_nChangeItemInfoNum; i++)
		{
			if (nItemID == (DWORD)m_stChangeItemInfo[i].nChangeItemID)
			{
				// ��ȿ �������ΰ�?
				// ..��ũ��Ʈ ������ �����̳� ��Ÿ����� ������ ��ȿ���� ���� �������̶�� ó������ �ʴ´�.
				ITEM_INFO* pstItem = GetItemInfo(nItemID);
				if (pstItem == NULL)
				{
					continue;
				}

				// ��� ����
				for(int j=0; j<m_stChangeItemInfo[i].nResultNum; j++)
				{
					//DWORD nProba = CSHMath::GetRandomNum(0, 9999);
					// 080924 KTH -- ChangeItem Random Ratio Fix
					DWORD nProba;
					nProba = CSHMath::GetRandomNum(0, 9999);
					nProba += CSHMath::GetRandomNum(0, 99) * 10000;
					
					for(int k=0; k<m_stChangeItemInfo[i].stResult[j].nResultBaseNum; k++)
					{
						switch(m_stChangeItemInfo[i].stResult[j].stResultBase[k].eResultKind)
						{
							// ������
						case TRK_ITEM:
							{
								CItemSlot* pSlot = pPlayer->GetSlot(eItemTable_Inventory);
								if(NULL == pSlot)
								{
									return FALSE;
								}

								// 071211 KTH -- pPlayer�߰�
								DWORD nNum = GetTotalEmptySlotNum(pSlot, pPlayer);
								if(nNum == 0)
								{
									return FALSE;
								}

								if (nProba < m_stChangeItemInfo[i].stResult[j].stResultBase[k].nProbaValue ||
									k == m_stChangeItemInfo[i].stResult[j].nResultBaseNum-1)
								{
									int nItemID  = m_stChangeItemInfo[i].stResult[j].stResultBase[k].nValue01;
									int nItemNum1 = m_stChangeItemInfo[i].stResult[j].stResultBase[k].nValue02;
									int nItemNum2 = m_stChangeItemInfo[i].stResult[j].stResultBase[k].nValue03;

									if (m_stChangeItemInfo[i].stResult[j].stResultBase[k].nValue03 == 0)
									{
										nItemNum1 = 1;
										nItemNum2 = m_stChangeItemInfo[i].stResult[j].stResultBase[k].nValue02;
									}

									int nItemNum = CSHMath::GetRandomNum(nItemNum1, nItemNum2);
									//ObtainMonstermeterItem(pPlayer, nItemID, nItemNum);
									ObtainItemFromChangeItem( pPlayer, nItemID, WORD( nItemNum ) );

									goto NEXT_RESULT;
								}
							}
							break;
							// ��
						case TRK_MONEY:
							{
								if (nProba < m_stChangeItemInfo[i].stResult[j].stResultBase[k].nProbaValue ||
									k == m_stChangeItemInfo[i].stResult[j].nResultBaseNum-1)
								{
									int nMoney = m_stChangeItemInfo[i].stResult[j].stResultBase[k].nValue01;
									if( pPlayer->SetMoney(nMoney, MONEY_ADDITION, MF_OBTAIN, eItemTable_Inventory, eMoneyLog_GetMonstermeter) != (MONEYTYPE)nMoney )
									{
										MSGBASE msg;
										msg.Category = MP_ITEM;
										msg.Protocol = MP_ITEM_MONEY_ERROR;
										msg.dwObjectID = pPlayer->GetID();

										pPlayer->SendMsg(&msg, sizeof(msg));
										return FALSE;
									}
									goto NEXT_RESULT;
								}
							}
							break;
						}
					}
NEXT_RESULT:;
				}
			}
		}
	}

	return TRUE;
}
// E ü���������� �߰� added by hseos 2007.07.29


void CItemManager::ReloadItemList()
{
	CMHFile file;

	if(!file.Init("System/Resource/ItemList.bin", "rb"))
	{
		return;
	}

	ITEM_INFO* pInfo;

	while((pInfo = m_ItemInfoList.GetData())!= NULL)
	{
		delete pInfo;
	}

	m_ItemInfoList.RemoveAll();

	while( ! file.IsEOF() )
	{
		DWORD ItemIdx = file.GetDword();
		if( ItemIdx == 0 )
		{
			char buf[MAX_PATH]={0,};
			file.GetLine( buf, sizeof(buf) );
			continue;
		}

		pInfo = m_ItemInfoList.GetData(ItemIdx);

		if(pInfo)
		{
			SetItemInfo(ItemIdx,pInfo,&file);
		}
		else
		{
			pInfo = new ITEM_INFO;
			SetItemInfo(ItemIdx,pInfo,&file);
			m_ItemInfoList.Add(pInfo, pInfo->ItemIdx);
		}
	}
}


void CItemManager::SetItemInfo(DWORD ItemIdx,ITEM_INFO* pInfo,CMHFile* pFile)
{
	pInfo->ItemIdx = ItemIdx;
	SafeStrCpy( pInfo->ItemName, pFile->GetString(), MAX_ITEMNAME_LENGTH+1 );

	pInfo->ItemTooltipIdx = pFile->GetWord();
	pInfo->Image2DNum = pFile->GetWord();	
	pInfo->Part3DType = pFile->GetWord();
	pInfo->Part3DModelNum = pFile->GetWord();	

	pInfo->Shop = pFile->GetWord(); // �����۸� ����
	pInfo->Category = eItemCategory(pFile->GetWord());
	pInfo->Grade = pFile->GetWord();
	pInfo->LimitRace = LIMIT_RACE(pFile->GetWord());
	pInfo->LimitGender = pFile->GetWord();		
	pInfo->LimitLevel = WORD( pFile->GetLevel() );	

	pInfo->Stack = pFile->GetWord();
	pInfo->Trade = pFile->GetWord();
	pInfo->Deposit = pFile->GetWord();
	pInfo->Sell = pFile->GetWord();
	pInfo->Decompose = pFile->GetWord();

	pInfo->DecomposeIdx = pFile->GetDword();
	pInfo->Time = pFile->GetDword();

	pInfo->BuyPrice = pFile->GetDword();			
	pInfo->SellPrice = pFile->GetDword();
	pInfo->EquipType = eEquipType(pFile->GetWord());
	pInfo->EquipSlot = EWEARED_ITEM(pFile->GetWord() - 1);
	pInfo->Durability = pFile->GetDword();
	pFile->GetDword();

	pInfo->Repair = pFile->GetWord();
	pInfo->WeaponAnimation = eWeaponAnimationType(pFile->GetWord());
	pInfo->WeaponType = eWeaponType(pFile->GetWord());
	pInfo->PhysicAttack = pFile->GetInt();
	pInfo->MagicAttack = pFile->GetInt();
	pInfo->ArmorType = eArmorType(pFile->GetWord());
	pInfo->AccessaryType = eAccessaryType(pFile->GetWord());
	pInfo->PhysicDefense = pFile->GetInt();
	pInfo->MagicDefense = pFile->GetInt();
	pInfo->IsEnchant		= pFile->GetBool();
	pInfo->Improvement		= pFile->GetWord();

	pInfo->EnchantValue		= pFile->GetDword();
	pInfo->EnchantDeterm	= pFile->GetDword();

	pInfo->ImprovementStr = pFile->GetInt();			
	pInfo->ImprovementDex = pFile->GetInt();			
	pInfo->ImprovementVit = pFile->GetInt();		//10
	pInfo->ImprovementInt = pFile->GetInt();
	pInfo->ImprovementWis = pFile->GetInt();			
	pInfo->ImprovementLife = pFile->GetInt();			
	pInfo->ImprovementMana = pFile->GetInt();			

	pInfo->ImprovementSkill = pFile->GetDword();
	pInfo->Skill = pFile->GetDword();
	pInfo->SupplyType = ITEM_KIND(pFile->GetWord());
	pInfo->SupplyValue = pFile->GetDword();
	pInfo->kind	= ITEM_INFO::eKind(pFile->GetDword());
	pInfo->RequiredBuff = pFile->GetDword();
	pInfo->Battle = pFile->GetBool();
	pInfo->Peace = pFile->GetBool();
	pInfo->Move = pFile->GetBool();
	pInfo->Stop = pFile->GetBool();
	pInfo->Rest = pFile->GetBool();
	pInfo->wSeal = ITEM_SEAL_TYPE(pFile->GetWord());
	pInfo->nTimeKind = ITEM_TIMEKIND(pFile->GetInt());
	pInfo->dwUseTime = pFile->GetDword();
	pInfo->dwBuyFishPoint = pFile->GetDword();
	pInfo->wPointType = eItemPointType(pFile->GetWord());
	pInfo->dwPointTypeValue1 = pFile->GetDword();
	pInfo->dwPointTypeValue2 = pFile->GetDword();
	// 091123 ShinJS --- Item Type �߰�
	pInfo->dwType =  pFile->GetDword();				// Item ����
	pInfo->dwTypeDetail =  pFile->GetDword();		// Item ��������
}

void CItemManager::NetworkMsgParse( DWORD dwConnectionIndex, BYTE Protocol, void* pMsg )
{
	MSGBASE * pmsg = (MSGBASE *)pMsg;

	CObject* pObject = g_pUserTable->FindUser( pmsg->dwObjectID );
	if( !pObject )
		return;

	// 100531 ShinJS --- �α׾ƿ����� ��� �����۰��� ó���� ���� �ʵ��� �Ѵ�.
	if( pObject->GetState() == eObjectState_Exit )
		return;

	switch(Protocol)
	{
	case MP_ITEM_PET_MOVE_SYN:
		{
			MSG_ITEM_MOVE_SYN*	pmsg	= (MSG_ITEM_MOVE_SYN *)pMsg;
			CPlayer*			pPlayer = ( CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);

			if( ! pPlayer )
			{
				return;
			}

			if( EI_TRUE == MovePetItem(pPlayer, pmsg->wFromItemIdx, pmsg->FromPos, pmsg->wToItemIdx, pmsg->ToPos) )
			{
				MSG_ITEM_MOVE_ACK message;
				memcpy(&message, pMsg, sizeof(MSG_ITEM_MOVE_SYN));
				message.Protocol = MP_ITEM_PET_MOVE_ACK;

				pPlayer->SendMsg( &message, sizeof( message ) );
			}
			else
			{
				MSG_ITEM_ERROR message;
				message.Category	= MP_ITEM;
				message.Protocol	= MP_ITEM_ERROR_NACK;
				message.dwObjectID	= pPlayer->GetID();
				message.ECode		= eItemUseErr_Move;

				pPlayer->SendMsg( &message, sizeof( message ) );
			}
		}
		break;
	case MP_ITEM_MOVE_SYN:
		{
			MSG_ITEM_MOVE_SYN*	pmsg	= (MSG_ITEM_MOVE_SYN *)pMsg;
			CPlayer*			pPlayer = ( CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			int					rt		= 0;
			const ITEM_INFO*	pInfo	= GetItemInfo( pmsg->wFromItemIdx );
			const ITEM_INFO*	pToInfo = GetItemInfo( pmsg->wToItemIdx );

			if( ! pPlayer )
			{
				return;
			}
			else if( ! pInfo )
			{
				MSG_ITEM_ERROR message;
				message.Category	= MP_ITEM;
				message.Protocol	= MP_ITEM_ERROR_NACK;
				message.dwObjectID	= pPlayer->GetID();
				message.ECode		= eItemUseErr_Move;

				pPlayer->SendMsg( &message, sizeof( message ) );
				return;
			}

			if (pmsg->ToPos >= TP_WEAR_START && pmsg->ToPos < TP_WEAR_END)
			{
				WORD tempPart;
				eEquipType tempType;
				EWEARED_ITEM tempSlot;
				if (!pToInfo)
				{
					tempPart = 0;
					tempType = eEquipType_None;
					tempSlot = eWearedItem_None;
				}
				else
				{
					tempPart = pToInfo->Part3DType;
					tempType = pToInfo->EquipType;
					tempSlot = pToInfo->EquipSlot;
				}
				if (pInfo->Part3DType != pmsg->PartTypeA || tempPart != pmsg->PartTypeB
					|| pInfo->EquipType != pmsg->EquipTypeA || tempType != pmsg->EquipTypeB
					|| pInfo->EquipSlot != pmsg->EquipSlotA || tempSlot != pmsg->EquipSlotB)
				{
					MSG_ITEM_ERROR message;
					message.Category = MP_ITEM;
					message.Protocol = MP_ITEM_ERROR_NACK;
					message.dwObjectID = pPlayer->GetID();
					message.ECode = eItemUseErr_Move;

					pPlayer->SendMsg(&message, sizeof(message));
					return;
				}
			}
			// 080703 LUJ, �̵����� �߰��Ǿ� ����������. �̵��� �����͸� ��ų�� ������ ������ ���� �� ���� �� ���� �� �� �ִ�
			//
			//				- ����� �Ѽհ�/�ܰ˸� �����ϴ�
			//				- ����/���� ���Կ� ���� ������ ���⸦ ����
			//				- ���� ���Կ� �������� �ִ� ��쿡�� ���� ���Կ� ���⸦ ������ �� �ִ�
			switch( pmsg->ToPos )
			{
				// 080703 LUJ, ���� �������� ������ �̵� �� ó��
			case TP_WEAR_START + eWearedItem_Weapon:
				{
					const ITEMBASE*		rightItem		= GetItemInfoAbsIn( pPlayer, TP_WEAR_START + eWearedItem_Shield );
					const ITEM_INFO*	rightItemInfo	= GetItemInfo( rightItem ? rightItem->wIconIdx : 0 );

					// 080703 LUJ, ���⸸ ������ �� �ִ�
					if( ePartType_OneHand != pInfo->Part3DType	&&
						ePartType_TwoHand != pInfo->Part3DType  &&
						// 090528 ONS ������� ������ ��� üũ.
						ePartType_TwoBlade!= pInfo->Part3DType  )
					{
						rt = MP_ITEM_ERROR_NACK;
						break;
					}
					// 080703 LUJ, ���� ĭ�� ��������� ��� �������� ���� ����
					else if( ! rightItemInfo )
					{
						break;
					}
					// 080703 LUJ, ��� ������ ��� �����տ� �������� ������ ���� �Ұ�
					else if( IsTwoHand( pmsg->wFromItemIdx ) )
					{
						rt = MP_ITEM_ERROR_NACK;
						break;
					}
					// 080703 LUJ, ���а� ������ ��� �Ѽ� ���⸸ ������ �� �ִ�
					else if( ePartType_Shield == rightItemInfo->Part3DType )
					{
						if( ePartType_OneHand != pInfo->Part3DType  )
						{
							rt = MP_ITEM_ERROR_NACK;
						}

						break;
					}
					// 080703 LUJ, �����տ� ���Ⱑ ������ ��� ���� ������ �����ϴ�
					else if( ePartType_OneHand == rightItemInfo->Part3DType )
					{
						if( rightItemInfo->WeaponType != pInfo->WeaponType )
						{
							rt = MP_ITEM_ERROR_NACK;
						}

						break;
					}

					break;
				}
				// 080703 LUJ, ���� �������� �̵� �� ó��
			case TP_WEAR_START + eWearedItem_Shield:
				{
					const ITEMBASE*		leftItem			= GetItemInfoAbsIn( pPlayer, TP_WEAR_START + eWearedItem_Weapon );
					const ITEM_INFO*	leftItemInfo		= GetItemInfo( leftItem ? leftItem->wIconIdx : 0 );
					
					// 080703 LUJ, ���� ������ �� ���
					if( ! leftItemInfo )
					{
						// 080703 LUJ, ���а� �ƴ� ��� ���� �Ұ�
						if( ePartType_Shield != pInfo->Part3DType )
						{
							rt = MP_ITEM_ERROR_NACK;
							break;
						}
					}
					// 080703 LUJ, �μչ���� ������ �� ����
					else if(	ePartType_TwoHand == leftItemInfo->Part3DType ||
								ePartType_TwoHand == pInfo->Part3DType ||
								// 090521 ONS �ű����� ���� Ÿ�� �Ӽ� �߰�
								ePartType_TwoBlade == pInfo->Part3DType
								)
					{
						rt = MP_ITEM_ERROR_NACK;
						break;
					}
					// 080703 LUJ, ��� ���Ⱑ ���ٸ� ���д� �׻� ���� ����
					else if( ePartType_Shield == pInfo->Part3DType )
					{
						break;
					}
					// 080703 LUJ, �̵��� ���°� �ƴϸ� ����
					else if( ! pPlayer->GetHeroTotalInfo()->bUsingTwoBlade )
					{
						rt = MP_ITEM_ERROR_NACK;
						break;
					}
					// 080703 LUJ, ���⸦ ������ ��� ������ ���� �͸� ���Ǿ�� �Ѵ�
					//				���� �̵��� ����� �������� ������ �ʴ´�
					else if(	pInfo->WeaponType	!= leftItemInfo->WeaponType ||
								eWeaponType_Staff	== leftItemInfo->WeaponType )
					{
						rt = MP_ITEM_ERROR_NACK;
						break;
					}
					// 090910 pdy ���н��� ���������� äũ �߰� 
					// ���⽽��->���н��� �̵��ǰ�� äũ�� ���ؾ� �Ѵ�.
					else if( pmsg->FromPos == TP_WEAR_START + eWearedItem_Weapon )
					{
						const ITEMBASE*		rightItem		= GetItemInfoAbsIn( pPlayer, TP_WEAR_START + eWearedItem_Shield );
						const ITEM_INFO*	rightItemInfo	= GetItemInfo( rightItem ? rightItem->wIconIdx : 0 );

						//���н����� ����ְų� ���н����� ���Ⱑ �������Ⱑ �ƴϸ� �Ұ�
						if( ! rightItemInfo )
						{
							rt = MP_ITEM_ERROR_NACK;
							break;
						}
						else if( leftItemInfo->WeaponType !=  rightItemInfo->WeaponType )
						{
							rt = MP_ITEM_ERROR_NACK;
							break;
						}
					}

					break;
				}
			default:
				{
					// 080703 LUJ, ���� ���Կ��� �������� ���� ��, �̵��� ������ ��� �� �� ����
					//				���� ���Կ��� ���Ⱑ ���� ��� �ִϸ��̼��� �ſ� ����ϴ�
					if( TP_WEAR_START + eWearedItem_Weapon == pmsg->FromPos )
					{
						const ITEMBASE*		item		= GetItemInfoAbsIn( pPlayer, TP_WEAR_START + eWearedItem_Shield );
						const ITEM_INFO*	itemInfo	= ( item ? GetItemInfo( item->wIconIdx ) : 0 );
						WORD tempPart;
						eEquipType tempType;
						EWEARED_ITEM tempSlot;
						const ITEM_INFO*	pToInfo = GetItemInfo( pmsg->wToItemIdx );
						if (!pToInfo)
						{
							tempPart = 0;
							tempType = eEquipType_None;
							tempSlot = eWearedItem_None;
						}
						else
						{
							tempPart = pToInfo->Part3DType;
							tempType = pToInfo->EquipType;
							tempSlot = pToInfo->EquipSlot;
						}
						if( itemInfo &&
							itemInfo->WeaponType == pInfo->WeaponType )
						{
							rt = MP_ITEM_ERROR_NACK;
						}

						if(tempSlot != eWearedItem_Weapon && tempSlot != eWearedItem_None)
						{
							rt = MP_ITEM_ERROR_NACK;
						}
					}

					break;
				}
			}

			if( MP_ITEM_ERROR_NACK == rt ||
				EI_TRUE != MoveItem(pPlayer, pmsg->wFromItemIdx, pmsg->FromPos, pmsg->wToItemIdx, pmsg->ToPos )  )
			{
				MSG_ITEM_ERROR message;
				message.Category	= MP_ITEM;
				message.Protocol	= MP_ITEM_ERROR_NACK;
				message.dwObjectID	= pPlayer->GetID();
				message.ECode		= eItemUseErr_Move;

				pPlayer->SendMsg( &message, sizeof( message ) );
			}
			else
			{
				MSG_ITEM_MOVE_ACK message;
				ZeroMemory( &message, sizeof( message ) );
				message.Category		= MP_ITEM;
				message.Protocol		= MP_ITEM_MOVE_ACK;
				message.FromPos			= pmsg->FromPos;
				message.ToPos			= pmsg->ToPos;
				message.wFromItemIdx	= pmsg->wFromItemIdx;
				message.wToItemIdx		= pmsg->wToItemIdx;

				pPlayer->SendMsg( &message, sizeof( message ) );
			}
		}
		break;
	case MP_ITEM_COMBINE_SYN:
		{			
			MSG_ITEM_COMBINE_SYN * pmsg = (MSG_ITEM_COMBINE_SYN *)pMsg;
			CPlayer * pPlayer = (CPlayer *)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(pPlayer == NULL) return;
			int rt = 0;
			if(EI_TRUE == (rt = CombineItem(pPlayer, pmsg->wItemIdx, pmsg->FromPos, pmsg->ToPos, pmsg->FromDur, pmsg->ToDur)))
			{
				MSG_ITEM_COMBINE_ACK msg;
				memcpy(&msg, pmsg, sizeof(MSG_ITEM_COMBINE_SYN));
				msg.Protocol = MP_ITEM_COMBINE_ACK;
				pPlayer->SendMsg(
					&msg,
					sizeof(msg));
			}
			else
			{
				SendErrorMessage(
					pPlayer,
					MP_ITEM_ERROR_NACK,
					eItemUseErr_Combine);
			}
		}
		break;
	case MP_ITEM_DIVIDE_SYN:
		{

			MSG_ITEM_DIVIDE_SYN * pmsg = (MSG_ITEM_DIVIDE_SYN *)pMsg;
			CPlayer * pPlayer = (CPlayer *)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(pPlayer == NULL) return;
			int rt = 0;
			if(EI_TRUE == (rt = DivideItem(pPlayer, pmsg->wItemIdx, pmsg->FromPos, pmsg->ToPos, pmsg->FromDur, pmsg->ToDur)))
			{

			}
			else
			{
				SendErrorMessage(
					pPlayer,
					MP_ITEM_ERROR_NACK,
					eItemUseErr_Divide);
			}

		}
		break;
	case MP_ITEM_DISCARD_SYN:
		{
			MSG_ITEM_DISCARD_SYN * pmsg = (MSG_ITEM_DISCARD_SYN *)pMsg;
			CPlayer * pPlayer = (CPlayer *)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(pPlayer == NULL) return;

			CItemSlot * pSlot = pPlayer->GetSlot(pmsg->TargetPos);
			if(!pSlot) return;
			ITEMBASE Item = *pSlot->GetItemInfoAbs(pmsg->TargetPos);

			int rt = 0;
			if( LOOTINGMGR->IsLootedPlayer( pPlayer->GetID() ) )
			{
				SendErrorMessage(
					pPlayer,
					MP_ITEM_ERROR_NACK,
					eItemUseErr_Discard);
				break;
			}

			const ITEM_OPTION optionForLog = GetOption(Item);

			if(EI_TRUE == (rt = DiscardItem(pPlayer, pmsg->TargetPos, pmsg->wItemIdx, pmsg->ItemNum)))
			{
				MSG_ITEM_DISCARD_SYN message(*pmsg);
				message.Protocol = MP_ITEM_DISCARD_ACK;
				pPlayer->SendMsg(
					&message,
					sizeof(message));
				LogItemDropOption(
					optionForLog,
					eLog_ItemDiscard);
				LogItemOption(
					pmsg->dwObjectID,
					Item.dwDBIdx,
					optionForLog,
					eLog_ItemDiscard);
				LogItemMoney(
					pmsg->dwObjectID,
					pPlayer->GetObjectName(),
					0,
					"",
					eLog_ItemDiscard,
					pPlayer->GetMoney(),
					0,
					0,
					Item.wIconIdx,
					Item.dwDBIdx,
					Item.Position,
					0,
					Item.Durability,
					0);
			}
			else
			{
				SendErrorMessage(
					pPlayer,
					MP_ITEM_DISCARD_NACK,
					rt);
			}
		}
		break;
	case MP_ITEM_BUY_SYN:
		{
			MSG_ITEM_BUY_SYN * pmsg = (MSG_ITEM_BUY_SYN *)pMsg;
			CPlayer * pPlayer = (CPlayer *)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(pPlayer == NULL) return;

			// checkhacknpc
			if( CheckHackNpc( pPlayer, pmsg->wDealerIdx ) == FALSE )
			{
				SendErrorMessage(
					pPlayer,
					MP_ITEM_BUY_NACK,
					NOT_EXIST);
				return;
			}

			int rt = 0;
			if(EI_TRUE == (rt = BuyItem(pPlayer, pmsg->wBuyItemIdx, pmsg->BuyItemNum, pmsg->wDealerIdx)))
			{
			}
			else
			{
				SendErrorMessage(
					pPlayer,
					MP_ITEM_BUY_NACK,
					rt);
			}
		}
		break;
	case MP_ITEM_SELL_SYN:
		{
			MSG_ITEM_SELL_SYN* pmsg = (MSG_ITEM_SELL_SYN*)pMsg;
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser( pmsg->dwObjectID ) ;
			if( !pPlayer ) return ;


			// NPC ��ȿ�� üũ.
			if( CheckHackNpc( pPlayer, pmsg->wDealerIdx ) == FALSE )
			{
				SendErrorMessage(
					pPlayer,
					MP_ITEM_SELL_NACK,
					ERR_INVALNPC);
				return;
			}


			// ������ �Ǹ� ó���� �Ѵ�.
			int rt = SellItem(pPlayer, pmsg->TargetPos, pmsg->wSellItemIdx, pmsg->SellItemNum, pmsg->wDealerIdx) ;


			// �Ǹ� ��� ó���� �����Ѵ�.
			if( rt == ERR_NONE )
			{
				MSG_ITEM_SELL_ACK msg ;

				memcpy(&msg, pmsg, sizeof(MSG_ITEM_SELL_SYN)) ;
				msg.Protocol = MP_ITEM_SELL_ACK ; 

				pPlayer->SendMsg(
					&msg,
					sizeof(msg));
			}
			else
			{
				SendErrorMessage(
					pPlayer,
					MP_ITEM_SELL_NACK,
					rt);
			}
		}
		break;
	case MP_ITEM_USE_SYN:
		{
			MSG_ITEM_USE_SYN * pmsg = (MSG_ITEM_USE_SYN *)pMsg;
			CPlayer * pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);

			if(pPlayer == NULL) return;

			const eItemUse_Err rt = UseItem(pPlayer, pmsg->TargetPos, pmsg->wItemIdx);

			if(rt == eItemUseSuccess || rt == eItemUseUnsealed )
			{
				MSG_ITEM_USE_ACK message;
				ZeroMemory(
					&message,
					sizeof(message));
				message.Category = MP_ITEM;
				message.Protocol = MP_ITEM_USE_ACK;
				message.TargetPos = pmsg->TargetPos;
				message.dwItemIdx = pmsg->wItemIdx;
				message.eResult = rt;

				pPlayer->SendMsg(
					&message,
					sizeof(message));
			}
			else if(eItemUseSuccess_ResetRegidentCard == rt)
			{
				// �Լ� ó�� �� CPlayer�� ���ŵȴ�. �ٽ� �����ؼ��� �ȵȴ�
				ResidentRegist_Reset(
					pPlayer->GetID());
				break;
			}
			else
			{
				SendErrorMessage(
					pPlayer,
					MP_ITEM_USE_NACK,
					rt);
			}
		}
		break;
	case MP_ITEM_MIX_SYN:
		{			
			MSG_ITEM_MIX_SYN2*	message = ( MSG_ITEM_MIX_SYN2* )pMsg;
			CPlayer*			player	= ( CPlayer* )			g_pUserTable->FindUser(message->dwObjectID);

			if( ! player )
			{
				return;
			}

			CItemSlot* slot = player->GetSlot( eItemTable_Inventory );

			if( ! slot )
			{
				return;
			}

			CBackupSlot backupSlot( *slot );

			const eLogitemmoney log = Mix( player, *message, &backupSlot );

			switch( log )
			{
			case eLog_ItemMixSuccess:
				{
					MSG_DWORD message;
					message.Category	= MP_ITEM;
					message.Protocol	= MP_ITEM_WORKING_SUCCESS;
					message.dwData		= player->GetID();

					PACKEDDATA_OBJ->QuickSend( player, &message, sizeof( message ) );
					break;
				}
			case eLog_ItemMixFail:
				{
					MSG_DWORD message;
					message.Category	= MP_ITEM;
					message.Protocol	= MP_ITEM_WORKING_FAIL;
					message.dwData		= player->GetID();

					PACKEDDATA_OBJ->QuickSend( player, &message, sizeof( message ) );
					break;
				}
			default:
				{
					backupSlot.Restore();

					{
						MSG_DWORD message;
						message.Category	= MP_ITEM;
						message.Protocol	= MP_ITEM_MIX_NACK;
						message.dwData		= log;

						player->SendMsg( &message, sizeof( message ) );
					}

					// Ÿ�ο��� �ִϸ��̼��� ǥ��
					{
						MSG_DWORD message;
						message.Category	= MP_ITEM;
						message.Protocol	= MP_ITEM_WORKING_FAIL;
						message.dwData	= player->GetID();

						PACKEDDATA_OBJ->QuickSend( player, &message, sizeof( message ) );
					}

					break;
				}
			}
		}
		break;
	 case MP_ITEM_TRANSMOG_SYN:
		{
			const MSG_ITEM_TRANSMOG_SYN* m		= ( MSG_ITEM_TRANSMOG_SYN* )pMsg;
			CPlayer*					player	= ( CPlayer* )g_pUserTable->FindUser( m->dwObjectID );

			if( ! player )
			{
				return;
			}

			eLogitemmoney log = eLog_Max;

			switch( Transmog( player, m, log ) )
			{
			case MP_ITEM_TRANSMOG_SUCCESS_ACK:
				{
					// È¿ï¿½ï¿½ï¿½ï¿½ Ç¥ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½Öµï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
					{
						MSG_DWORD message;
						message.Category	= MP_ITEM;
						message.Protocol	= MP_ITEM_WORKING_SUCCESS;
						message.dwData		= player->GetID();

						PACKEDDATA_OBJ->QuickSend( player, &message, sizeof( message ) );
					}

					break;
				}
			case MP_ITEM_TRANSMOG_FAILED_ACK:
				{
					{
						MSGBASE message;
						message.Category	= MP_ITEM;
						message.Protocol	= MP_ITEM_TRANSMOG_FAILED_ACK;

						player->SendMsg( &message, sizeof( message ) );
					}					

					// È¿ï¿½ï¿½ï¿½ï¿½ Ç¥ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½Öµï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
					{
						MSG_DWORD message;
						message.Category	= MP_ITEM;
						message.Protocol	= MP_ITEM_WORKING_FAIL;
						message.dwData	= player->GetID();

						PACKEDDATA_OBJ->QuickSend( player, &message, sizeof( message ) );
					}

					break;
				}
			case MP_ITEM_TRANSMOG_NACK:
				{
					MSG_DWORD message;
					message.Category	= MP_ITEM;
					message.Protocol	= MP_ITEM_TRANSMOG_NACK;
					message.dwData		= log;

					player->SendMsg( &message, sizeof( message ) );
					break;
				}
			}
		}
		break;
	case MP_ITEM_ENCHANT_SYN:
		{
			const MSG_ITEM_ENCHANT_SYN* m		= ( MSG_ITEM_ENCHANT_SYN* )pMsg;
			CPlayer*					player	= ( CPlayer* )g_pUserTable->FindUser( m->dwObjectID );

			if( ! player )
			{
				return;
			}

			eLogitemmoney log = eLog_Max;

			switch( Enchant( player, m, log ) )
			{
			case MP_ITEM_ENCHANT_SUCCESS_ACK:
				{
					// ȿ���� ǥ���� �� �ֵ��� ����
					{
						MSG_DWORD message;
						message.Category	= MP_ITEM;
						message.Protocol	= MP_ITEM_WORKING_SUCCESS;
						message.dwData		= player->GetID();

						PACKEDDATA_OBJ->QuickSend( player, &message, sizeof( message ) );
					}

					break;
				}
			case MP_ITEM_ENCHANT_FAILED_ACK:
				{
					{
						MSGBASE message;
						message.Category	= MP_ITEM;
						message.Protocol	= MP_ITEM_ENCHANT_FAILED_ACK;

						player->SendMsg( &message, sizeof( message ) );
					}					

					// ȿ���� ǥ���� �� �ֵ��� ����
					{
						MSG_DWORD message;
						message.Category	= MP_ITEM;
						message.Protocol	= MP_ITEM_WORKING_FAIL;
						message.dwData	= player->GetID();

						PACKEDDATA_OBJ->QuickSend( player, &message, sizeof( message ) );
					}

					break;
				}
			case MP_ITEM_ENCHANT_NACK:
				{
					MSG_DWORD message;
					message.Category	= MP_ITEM;
					message.Protocol	= MP_ITEM_ENCHANT_NACK;
					message.dwData		= log;

					player->SendMsg( &message, sizeof( message ) );
					break;
				}
			}
		}
		break;
	case MP_ITEM_REINFORCE_SYN:
		{
			const MSG_ITEM_REINFORCE_SYN* m = ( MSG_ITEM_REINFORCE_SYN* )pMsg;

			CPlayer* player = (CPlayer *)g_pUserTable->FindUser( m->dwObjectID );

			if( ! player )
			{
				return;
			}

			CItemSlot* slot = player->GetSlot( eItemTable_Inventory );

			if( ! slot )
			{
				return;
			}

			CBackupSlot backupSlot( *slot );

			eLogitemmoney log = Reinforce( player, *m, &backupSlot );

			switch( log )
			{
			case eLog_ItemReinforceSuccess:
				{
					MSG_DWORD message;
					message.Category	= MP_ITEM;
					message.Protocol	= MP_ITEM_WORKING_SUCCESS;
					message.dwData		= player->GetID();

					PACKEDDATA_OBJ->QuickSend( player, &message, sizeof( message ) );
					break;
				}
			case eLog_ItemReinforceFail:
				{
					MSG_DWORD message;
					message.Category	= MP_ITEM;
					message.Protocol	= MP_ITEM_WORKING_FAIL;
					message.dwData		= player->GetID();

					PACKEDDATA_OBJ->QuickSend( player, &message, sizeof( message ) );
					break;
				}
			default:
				{
					backupSlot.Restore();

					MSG_ITEM_ERROR message;
					message.Category	= MP_ITEM;
					message.Protocol	= MP_ITEM_REINFORCE_NACK;
					message.dwObjectID	= player->GetID();
					message.ECode		= log;

					player->SendMsg( &message, sizeof( message ) );
					break;
				}
			}			
		}
		break;

	case MP_ITEM_DEALER_SYN:
		{
			MSG_WORD* pmsg = (MSG_WORD*)pMsg;
			CPlayer* pPlayer = (CPlayer *)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(pPlayer == NULL)
				return;

			// checkhacknpc
			if( CheckHackNpc( pPlayer, pmsg->wData ) == FALSE )
				return;

			// AO��oA
			MSG_WORD msg;
			msg.Category = MP_ITEM;
			msg.Protocol = MP_ITEM_DEALER_ACK;
			msg.dwObjectID = pmsg->dwObjectID;
			msg.wData = pmsg->wData;
			pPlayer->SendMsg(&msg,sizeof(msg));

		}
		break;
	case MP_ITEM_GUILD_MOVE_SYN:
		{
			MSG_ITEM_MOVE_SYN * pmsg = (MSG_ITEM_MOVE_SYN *)pMsg;
			CPlayer * pPlayer = (CPlayer *)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(pPlayer == NULL) return;

			if(!CanMovetoGuildWare(pmsg->FromPos, pmsg->ToPos, pPlayer))
			{				
				SendErrorMessage(
					pPlayer,
					MP_ITEM_GUILD_MOVE_NACK,
					4);
				return;
			}
			int rt = 0;
			if(EI_TRUE == (rt = MoveItem(pPlayer, pmsg->wFromItemIdx, pmsg->FromPos, pmsg->wToItemIdx, pmsg->ToPos)))
			{
				//	AddGuildItemOption(pmsg->FromPos, pmsg->ToPos, pPlayer);
				MSG_ITEM_MOVE_ACK msg;
				memcpy(&msg, pmsg, sizeof(MSG_ITEM_MOVE_SYN));
				msg.Protocol = MP_ITEM_GUILD_MOVE_ACK;
				pPlayer->SendMsg(
					&msg,
					sizeof(msg));
			}
			else
			{
				SendErrorMessage(
					pPlayer,
					MP_ITEM_GUILD_MOVE_NACK,
					rt);
			}
		}
		break;
	case MP_ITEM_STORAGE_ITEM_INFO_SYN:
		{
			MSG_WORD2* pmsg = (MSG_WORD2*)pMsg;
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);

			if( !pPlayer )	return;

			if( CheckHackNpc( pPlayer, pmsg->wData1, pmsg->wData2 ) == FALSE )
				return;
			//SW ��ȭ/���� ���� �ް� ���̽� ���� �޴´�. ������ ������?

			if( pPlayer->IsGotWarehouseItems() == TRUE )	//�������� �޴����̰ų� �̹� �޾Ҵ�.
				return;

			pPlayer->SetGotWarehouseItems( TRUE );
			
			CharacterStorageItemInfo( pPlayer->GetID(), pPlayer->GetUserID(), 0 );
		}
		break;
	case MP_ITEM_USE_FOR_QUESTSTART_SYN:
		{
			MSG_ITEM_USE_SYN * pmsg = (MSG_ITEM_USE_SYN *)pMsg;
			CPlayer * pPlayer = (CPlayer *)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(pPlayer == NULL) return;

			int rt = UseItem(pPlayer, pmsg->TargetPos, pmsg->wItemIdx);
			if(rt == eItemUseSuccess)
			{	
				MSG_ITEM_USE_ACK msg;
				msg.Category	= MP_ITEM;
				msg.Protocol	= MP_ITEM_USE_ACK;
				msg.dwObjectID	= pmsg->dwObjectID;
				msg.TargetPos	= pmsg->TargetPos;
				msg.dwItemIdx	= pmsg->wItemIdx;
				msg.eResult		= eItemUseSuccess;
				pPlayer->SendMsg(
					&msg,
					sizeof(msg));
			}
			else
			{
				SendErrorMessage(
					pPlayer,
					MP_ITEM_USE_NACK,
					eItemUseErr_Quest);
			}
		}
		break;

	case MP_ITEM_DISSOLVE_SYN:
		{
			const MSG_ITEM_DISSOLVE_SYN* const receivedMessage = (MSG_ITEM_DISSOLVE_SYN*)pMsg;
			CPlayer* const player = (CPlayer*)g_pUserTable->FindUser(
				receivedMessage->dwObjectID);

			if(0 == player)
			{
				break;
			}
			else if(eObjectKind_Player != player->GetObjectKind())
			{
				break;
			}

			CItemSlot* const slot = player->GetSlot(
				eItemTable_Inventory);

			if(0 == slot)
			{
				break;
			}

			CBackupSlot backupSlot(
				*slot);
			const eLogitemmoney log = Dissolve(
				*player,
				*receivedMessage,
				backupSlot);

			if(eLog_ItemDissolveSuccess == log)
			{
				break;
			}

			backupSlot.Restore();

			MSG_DWORD message;
			ZeroMemory(
				&message,
				sizeof(message));
			message.Category = MP_ITEM;
			message.Protocol = MP_ITEM_DISSOLVE_NACK;
			message.dwData = log;

			player->SendMsg(
				&message,
				sizeof(message));
		}
		break;
		// 080820 LUJ, ������ ��ȯ
	case MP_ITEM_USE_CHANGEITEM_SYN:
		{	
			CPlayer* player = ( CPlayer* )g_pUserTable->FindUser( pmsg->dwObjectID );
	
			if( !	player ||
					player->GetObjectKind() != eObjectKind_Player )
			{ 
				break;
			}
	
			const MSG_ITEM_USE_SYN*	message			= ( MSG_ITEM_USE_SYN* )pmsg;
			const DWORD				itemIndex		=			message->wItemIdx;
			const POSTYPE			itemPosition	= POSTYPE(	message->TargetPos );
	
			CItemSlot* slot = player->GetSlot( eItemTable_Inventory );
	
			if( ! slot )
			{	
				break;
			}
	
			const ITEMBASE* itemBase = slot->GetItemInfoAbs( itemPosition );
	
			if( !	itemBase	||
					itemBase->wIconIdx	!= itemIndex )
			{	
				break;
			}
	
			const ITEM_INFO* itemInfo = GetItemInfo( itemIndex );
	
			if( ! itemInfo ||
				! IsChangeItem( itemIndex ) )
			{	
				break;
			}
			// 080820 LUJ, ȹ�� �� ��Ÿ�� �������� ��Ÿ���� ���Ƽ��� ��� �ȵ�
			else if( ITEM_KIND_COOLTIME == itemInfo->SupplyType )
			{	
				const DWORD lastCheckedTick = itemBase->ItemParam;
				const BOOL	isCoolTime		= ( int( gCurTime - lastCheckedTick ) < itemBase->nRemainSecond * 1000 );
	
				if( isCoolTime )
				{
					break;
				}
			}
	
			// 080820 LUJ, ������ ��ȯ
			if( ! ProcChangeItem( player, itemIndex ) )
			{	
				break;
			}
	
			LogItemMoney(
				player->GetID(),
				player->GetObjectName(),
				0,
				"",
				eLog_ItemDestroybyChangeItem,
				player->GetMoney(),
				0,
				0,
				itemIndex,
				itemBase->dwDBIdx,
				itemPosition,
				0,
				0,
				0 );
	
			DiscardItem( player, itemPosition, itemIndex, 1 );
	
			// 080820 LUJ, ��� ����
			{	
				MSG_ITEM_USE_ACK message;
				message.Category	= MP_ITEM;
				message.Protocol	= MP_ITEM_USE_ACK;
				message.TargetPos	= itemPosition;
				message.dwItemIdx	= itemIndex;
				message.eResult		= eItemUseSuccess;
	
				player->SendMsg( &message, sizeof( message ) );
			}
		}
		break;
	case MP_ITEM_WORKING_START:
		{
			MSG_DWORD* message = ( MSG_DWORD* )pMsg;

			CObject* object = g_pUserTable->FindUser( message->dwObjectID );

			if( object )
			{
				PACKEDDATA_OBJ->QuickSend( object, message, sizeof( *message ) );
			}
		}
		break;

	case MP_ITEM_WORKING_STARTEX:
		{
			MSG_DWORD2* message = ( MSG_DWORD2* )pMsg;

			CObject* object = g_pUserTable->FindUser( message->dwObjectID );

			if( object )
			{
				PACKEDDATA_OBJ->QuickSend( object, message, sizeof( *message ) );
			}
		}
		break;
	case MP_ITEM_SHOPITEM_INFO_SYN:
		{
			MSGBASE* pmsg = (MSGBASE*)pMsg;
			CPlayer * pPlayer = (CPlayer *)g_pUserTable->FindUser(pmsg->dwObjectID);

			if( !pPlayer )
				return;

			CharacterShopItemInfo(
				pPlayer->GetID(),
				0);
		}
		break;

	// 100517 ONS �����ʱ�ȭ ������ ó�� �߰�
	case MP_ITEM_CHANGE_CLASS_SYN:
		{
			MSG_ITEM_CHANGE_CLASS* pmsg = (MSG_ITEM_CHANGE_CLASS*)pMsg;
			POSTYPE pos		= pmsg->TargetPos;
			DWORD	itemIdx = pmsg->wItemIdx;
			const BYTE*	ClassStage = pmsg->ClassStage;

			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if( pPlayer == NULL )
			{
				return;
			}

			CItemSlot * pSlot = pPlayer->GetSlot(pos);
			if( ! pSlot )
			{
				return;
			}

			if( pPlayer->IsPKMode() || pPlayer->GetAutoNoteIdx() )
			{
				MSGBASE msg;
				msg.Category	= MP_ITEM;
				msg.Protocol	= MP_ITEM_CHANGE_CLASS_NACK;
				pPlayer->SendMsg( &msg, sizeof( msg ) );
				return;
			}

			const ITEMBASE * pItemBase = pSlot->GetItemInfoAbs(pos);
			ITEM_INFO* pItemInfo = GetItemInfo( pItemBase->wIconIdx );
			if( !pItemInfo || pItemInfo->SupplyType != ITEM_KIND_CHANGE_CLASS)
			{
				return;
			}

			if( ! pItemBase || 
				!CHKRT->ItemOf(pPlayer, pos, pItemBase->wIconIdx,0,0,CB_EXIST|CB_ICONIDX) || 
				pItemBase->wIconIdx != itemIdx )
			{
				return;
			}

			if( UseItem(pPlayer, pos, itemIdx) != eItemUseSuccess )
			{
				MSGBASE msg;
				ZeroMemory(&msg, sizeof(msg));
				msg.Category	= MP_ITEM;
				msg.Protocol	= MP_ITEM_CHANGE_CLASS_NACK;
				pPlayer->SendMsg( &msg, sizeof( msg ) );
				return;
			}

			// �������� ����
			CHARACTER_TOTALINFO info = pPlayer->GetCharacterTotalInfo();
			CharacterJobUpdate(pPlayer->GetID(), info.JobGrade, 
								info.Job[0], 
								ClassStage[0],
								ClassStage[1],
								ClassStage[2],
								ClassStage[3],
								ClassStage[4]	);

			// 100805 ONS Ŭ���� ���� �α�ó�� �߰�
			for( int nGrade = 0; nGrade < info.JobGrade - 1; nGrade++ )
			{
				if( info.Job[nGrade + 1] == ClassStage[nGrade] )
				{
					continue;
				}

				info.Job[nGrade + 1] = ClassStage[nGrade];

				// ����� ������ �α׿� �����.
				InsertLogJob( pPlayer, info.Job[0], nGrade + 2, ClassStage[nGrade], "Use Change Item" );
			}

			// 100809 ONS Ŭ�����ʱ�ȭ ó���� ����Ʈ��Ī, ���, �йи��� ���� ������ ����ó�� �߰�
			WORD idx = 1;
			if( info.JobGrade > 1 )
			{
				idx = info.Job[info.JobGrade - 1];
			}
			DWORD dwClass = ( info.Job[0] * 1000 ) + ( ( info.Race + 1 ) * 100 ) + ( info.JobGrade * 10 ) + idx;

			MSG_DWORD Packet;
			ZeroMemory(&Packet, sizeof(Packet));
			Packet.Category	= MP_RESIDENTREGIST;
			Packet.Protocol	= MP_RESIDENTREGIST_REGIST_CHANGE;
			Packet.dwObjectID = pPlayer->GetID();
			Packet.dwData = dwClass;
			g_Network.Broadcast2AgentServer( ( char* )&Packet, sizeof( Packet ) );
			

			CGuild* guild = GUILDMGR->GetGuild( pPlayer->GetGuildIdx() );
			if( guild )
			{
				GUILDMEMBERINFO* member = guild->GetMemberInfo( pPlayer->GetID() );
				if( member )
				{
					member->mJobGrade	= info.JobGrade;
					member->mRace		= info.Race;
					memcpy( member->mJob, info.Job, sizeof( member->mJob ) );

					SEND_GUILD_MEMBER_INFO message;
					message.Category	= MP_GUILD;
					message.Protocol	= MP_GUILD_SET_MEMBER_TO_MAP;
					message.GuildIdx	= pPlayer->GetGuildIdx();
					message.MemberInfo	= *member;

					g_Network.Send2AgentServer( ( char* )&message, sizeof( message ) );
					GUILDMGR->NetworkMsgParse( message.Protocol, &message );
				}
			}


			if( pPlayer->GetFamilyIdx() != 0 )
			{
				MSG_DWORD3 message;
				ZeroMemory(&message, sizeof(message));
				message.Category	= MP_FAMILY;
				message.Protocol	= MP_FAMILY_CHANGE_CLASS;
				message.dwObjectID	= pPlayer->GetID();
				message.dwData1		= pPlayer->GetFamilyIdx();
				message.dwData2		= info.Job[info.JobGrade - 1];
				message.dwData3		= info.JobGrade;
				g_Network.Broadcast2AgentServer( ( char* )&message, sizeof( message ) );
			}

			// ��ų �ʱ�ȭ ó��
			UseItem_ResetSkill( pPlayer );

			MSGBASE msg;
			ZeroMemory(&msg, sizeof(msg));
			msg.Category	= MP_ITEM;
			msg.Protocol	= MP_ITEM_CHANGE_CLASS_ACK;
			pPlayer->SendMsg( &msg, sizeof( msg ) );

			LogItemMoney(
				pPlayer->GetID(),
				pPlayer->GetObjectName(),
				0,
				"",
				eLog_ItemChangeClass,
				pPlayer->GetMoney(),
				0,
				0,
				itemIdx,
				pItemBase->dwDBIdx,
				pos,
				0,
				0,
				0 );
		}
		break;

	// 071201 KTH - ���̵� ��ũ�� ���
	case MP_ITEM_MAPMOVE_SCROLL_SYN:
		{
			MSG_DWORD3* pmsg = (MSG_DWORD3*)pMsg;

			MAPTYPE mapType = (MAPTYPE)pmsg->dwData1;
			POSTYPE pos = (POSTYPE)pmsg->dwData2;
			DWORD itemIdx = pmsg->dwData3;

			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			
			if( pPlayer == NULL )
				return;

			CItemSlot * pSlot = pPlayer->GetSlot(pos);

			if( ! pSlot )
			{
				return;
			}

			const ITEMBASE* const pItemBase = pSlot->GetItemInfoAbs(pos);
			const ITEM_INFO* const pItemInfo = GetItemInfo(pItemBase->wIconIdx);

			if(pItemInfo->SupplyType != ITEM_KIND_MAP_MOVE_SCROLL)
				return;

			// 090817 ONS �����ʸ�Ʈ �ʿ����� ���빰���� ������ �Ҹ𼺾����� ���Ұ�.
			if(g_pServerSystem->GetMapNum()==GTMAPNUM)
			{
				if(pItemInfo->Category==MP_POWERUP)
				{
					if(pItemInfo->SupplyType!=ITEM_KIND_GTHP_RECOVER_INSTANT &&
						pItemInfo->SupplyType!=ITEM_KIND_GTMP_RECOVER_INSTANT)
					{
						MSG_ITEM_ERROR msg;
						msg.Category = MP_ITEM;
						msg.Protocol = MP_ITEM_USE_NACK;
						msg.dwObjectID = pPlayer->GetID();
						msg.ECode = eItemUseErr_Err;
						pPlayer->SendMsg( &msg, sizeof( msg ) );
						return;
					}
				}
			}

			//---KES PK Mode 080104
			//---KES AUTONOTE
			if( pPlayer->IsPKMode() || pPlayer->GetAutoNoteIdx() )
			{
				MSG_DWORD msg;
				msg.Category	= MP_ITEM;
				msg.Protocol	= MP_ITEM_MAPMOVE_SCROLL_NACK;
				msg.dwData		= pPlayer->GetGridID() - 1;
				pPlayer->SendMsg( &msg, sizeof( msg ) );
				return;
			}
			//--------------------

			if( ! pItemBase || !CHKRT->ItemOf(pPlayer, pos, pItemBase->wIconIdx,0,0,CB_EXIST|CB_ICONIDX) || pItemBase->wIconIdx != itemIdx )
			{
				return;
			}

			const MAPCHANGE_INFO* ChangeInfo = GAMERESRCMNGR->GetMapChangeInfoForDestMapNum( mapType );

			if( CHANNELSYSTEM->GetChannelID( pPlayer->GetChannelID() - 1 ) == 0 && !CHANNELSYSTEM->IsQuestMap() || !ChangeInfo )
			{
				MSG_DWORD msg;
				msg.Category	= MP_ITEM;
				msg.Protocol	= MP_ITEM_MAPMOVE_SCROLL_NACK;
				msg.dwData		= pPlayer->GetGridID() - 1;
				pPlayer->SendMsg( &msg, sizeof( msg ) );
				return;
			}
			// 090115 LUJ, �� �̵� ��ũ�ѷ� �̵� ������ ������ �˻��ϰ� �α׸� ����
			// 090205 LUJ, ���� �ڵ� �߰�
			else if( ! GAMERESRCMNGR->IsMovableMapWithScroll( mapType ) )
			{
				char memo[ MAX_PATH ] = { 0 };
				sprintf(
					memo,
					"%d > %d",
					g_pServerSystem->GetMapNum(),
					mapType );
				LogScriptHack(
					*pPlayer,
					"MapMoveScroll.bin",
					memo );
				break;
			}

			const ITEMBASE	previousItem		= *pItemBase;

			if( pItemBase->nSealed != eITEM_TYPE_UNSEAL )
			{
				if( pItemBase->nSealed == eITEM_TYPE_SEAL || UseItem(pPlayer, pos, itemIdx) != eItemUseSuccess )
				{
					MSG_DWORD msg;
					msg.Category	= MP_ITEM;
					msg.Protocol	= MP_ITEM_MAPMOVE_SCROLL_NACK;
					msg.dwData		= pPlayer->GetGridID() - 1;
					pPlayer->SendMsg( &msg, sizeof( msg ) );
					return;
				}
			}

			const BOOL vehicleFailedToTeleport = ( FALSE == VEHICLEMGR->Teleport( pPlayer->GetID(), mapType ) );

			if( vehicleFailedToTeleport )
			{
				VEHICLEMGR->Unsummon( pPlayer->GetID(), FALSE );
				Move_ChangeMap( pPlayer, mapType );
			}
		}
		break;
	// 071203 KTH -- ���������� ��ų �߰�
	case MP_ITEM_SKILL_SYN:
		{
			MSG_ITEM_SKILL_START_SYN* pmsg = (MSG_ITEM_SKILL_START_SYN*)pMsg;
			MSG_SKILL_START_SYN SkillStartSyn = pmsg->SkillStartSyn;
			POSTYPE				pos = pmsg->Pos;
			DWORD				itemIdx = pmsg->Idx;
			ITEM_INFO* pItemInfo = GetItemInfo( itemIdx );
			CPlayer* pPlayer;
			pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);

			if( pPlayer == NULL )
				return;

			if( pItemInfo == NULL )
				return;

			if(SkillStartSyn.SkillIdx != pItemInfo->SupplyValue )
				return;

			ITEMBASE previousItem;
			ZeroMemory(
				&previousItem,
				sizeof(previousItem));
			const ITEMBASE* item = GetItemInfoAbsIn( pPlayer, pos );

			if( item )
			{
				previousItem = *item;
			}

			if( !CHKRT->ItemOf(pPlayer, pos, previousItem.wIconIdx,0,0,CB_EXIST|CB_ICONIDX) )
			{
				return;
			}
			// LUJ, ���� ������ �� �������� �Ἥ ��ų�� ������ �Ұ� �ǹǷ� ���´�
			else if(pPlayer->GetAbnormalStatus()->IsGod)
			{
				SendErrorMessage(
					pPlayer,
					MP_ITEM_USE_NACK,
					eItemUseErr_PlayerIsGod);
				return;
			}
			// 081209 LUJ, ��ų�� ���� �ߵ���Ų��. ������ ���� �ߵ����� ���� �����̹Ƿ� ��� �����ϴ�
			else if( SKILL_SUCCESS != SKILLMGR->OnSkillStartSyn(&SkillStartSyn, &previousItem) )
			{
				SendErrorMessage(
					pPlayer,
					MP_ITEM_USE_NACK,
					eItemUseErr_SkillCanNotStart);
				return;
			}

			const eItemUse_Err itemUseError = UseItem(
				pPlayer,
				pos,
				itemIdx);

			if(eItemUseSuccess != itemUseError)
			{
				// 081209 LUJ, �ߵ� ���� ��ų�� ����Ѵ�. ���� ��ų�� ���� ���μ����� ����Ǳ� ������ ������ ��ũ���̴�.
				MSG_DWORD cancelMessage;
				ZeroMemory( &cancelMessage, sizeof( cancelMessage ) );
				cancelMessage.Category		= MP_SKILL;
				cancelMessage.Protocol		= MP_SKILL_CANCEL_SYN;
				cancelMessage.dwObjectID	= pPlayer->GetID();
				cancelMessage.dwData		= pPlayer->CurCastingSkillID;
				SKILLMGR->OnSkillCancel( &cancelMessage );

				SendErrorMessage(
					pPlayer,
					MP_ITEM_USE_NACK,
					itemUseError);
				return;
			}

			{
				MSG_ITEM_USE_ACK msg;
				msg.Category	= MP_ITEM;
				msg.Protocol	= MP_ITEM_SKILL_ACK;
				msg.TargetPos	= pos;
				msg.dwItemIdx	= itemIdx;
				msg.eResult		= eItemUseSuccess;
				pPlayer->SendMsg(
					&msg,
					sizeof(msg));

				CPlayer* pTarget = (CPlayer*)g_pUserTable->FindUser(SkillStartSyn.MainTarget.dwMainTargetID);
				// ��ų���� ����� �α׳�����.
				LogItemMoney(
					pPlayer->GetID(),
					pPlayer->GetObjectName(),
					SkillStartSyn.MainTarget.dwMainTargetID,
					pTarget ? pTarget->GetObjectName() : "",
					eLog_ItemUse_GTBUFFITEM,
					0,
					0,
					0,
					previousItem.wIconIdx,
					previousItem.dwDBIdx,
					previousItem.Position,
					0,
					pItemInfo->Stack ? previousItem.Durability - 1 : 0,
					0);
			}
		}
		break;
	// �κ� �丮 Ȯ��
	case MP_ITEM_INCREASE_INVENTORY_SYN:
		{
			MSG_ITEM_USE_SYN* pmsg = (MSG_ITEM_USE_SYN*)pMsg;
			POSTYPE				pos = pmsg->TargetPos;
			DWORD				itemIdx = pmsg->wItemIdx;
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);

			if( pPlayer == NULL )
				return;

			ITEMBASE previousItem;
			ZeroMemory(
				&previousItem,
				sizeof(previousItem));
			const ITEMBASE* item = GetItemInfoAbsIn( pPlayer, pos );

			if( item )
			{
				previousItem = *item;
			}		

			if( !CHKRT->ItemOf(pPlayer, pos, previousItem.wIconIdx,0,0,CB_EXIST|CB_ICONIDX) )
			{
				return;
			}

			if( ! pPlayer->IncreaseInventoryExpansion() )
			{
				MSG_DWORD msg;
				msg.Category = MP_ITEM;
				msg.Protocol = MP_ITEM_INCREASE_INVENTORY_NACK;
				msg.dwData = 1; // 1�� ���̻� �ø� �������� ���� �ÿ� ������.
				pPlayer->SendMsg(&msg, sizeof(msg));
				return;
			}
			else if( UseItem(pPlayer, pos, itemIdx) != eItemUseSuccess)
			{
				MSG_DWORD msg;
				msg.Category = MP_ITEM;
				msg.Protocol = MP_ITEM_INCREASE_INVENTORY_NACK;
				msg.dwData = 0; // 0�� ������ ����� ���� �Ͽ����� ������.
				pPlayer->SendMsg(&msg, sizeof(msg));
				return;
			}
			else
			{
				MSG_ITEM_USE_ACK msg;
				msg.Category	= MP_ITEM;
				msg.Protocol	= MP_ITEM_SKILL_ACK;
				msg.TargetPos	= pos;
				msg.dwItemIdx	= itemIdx;
				msg.eResult		= eItemUseSuccess;
				pPlayer->SendMsg(
					&msg,
					sizeof(msg));
			}
		}
		break;
	case MP_ITEM_CHANGE_CHARACTER_NAME_SYN:
		{
			MSG_CHANGE_CHARACTER_NAME_SYN* pmsg = (MSG_CHANGE_CHARACTER_NAME_SYN*)pMsg;
			POSTYPE				pos = pmsg->TargetPos;
			DWORD				itemIdx = pmsg->wItemIdx;
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);

			if( pPlayer == NULL )
				return;

			const ITEMBASE* item = GetItemInfoAbsIn( pPlayer, pos );

			// 080124 KTH -- PKMode�϶����� ��� �Ұ�
			if( ! item || !CHKRT->ItemOf(pPlayer, pos, item->wIconIdx,0,0,CB_EXIST|CB_ICONIDX) || pPlayer->IsPKMode() )
			{
				return;
			}
			
			if( pPlayer->GetPartyIdx() != 0 )
			{
				MSG_DWORD msg;
				msg.Category = MP_ITEM;
				msg.Protocol = MP_ITEM_CHANGE_CHARACTER_NAME_NACK;
				msg.dwData = 3; // 3�� ��Ƽ�� ������ �����ִ�.
				pPlayer->SendMsg(&msg, sizeof(msg));
				return;
			}
			else if( pPlayer->GetFamilyIdx() != 0 )
			{
				MSG_DWORD msg;
				msg.Category = MP_ITEM;
				msg.Protocol = MP_ITEM_CHANGE_CHARACTER_NAME_NACK;
				msg.dwData = 2; // 2�� �йи��� ������ �Ǿ��ִ�.
				pPlayer->SendMsg(&msg, sizeof(msg));
				return;
			}
			else if( pPlayer->GetGuildIdx() != 0 )
			{
				MSG_DWORD msg;
				msg.Category = MP_ITEM;
				msg.Protocol = MP_ITEM_CHANGE_CHARACTER_NAME_NACK;
				msg.dwData = 4; // 4�� ��� ������ �Ǿ��ִ�.
				pPlayer->SendMsg(&msg, sizeof(msg));
				return;
			}

			ChangeCharacterName(pmsg->changeName, pmsg->dwObjectID, itemIdx, pos);

		}
		break;
		// 080215 LUJ, ���� �ɼ� ����
	case MP_ITEM_APPLY_OPTION_SYN:
		{
			const MSG_DWORD2* message = ( MSG_DWORD2* )pMsg;

			CPlayer* player = ( CPlayer* )g_pUserTable->FindUser( message->dwObjectID );

			if( ! player )
			{
				return;
			}

			CItemSlot* slot = player->GetSlot( eItemTable_Inventory );

			if( ! slot )
			{
				return;
			}

            const ITEMBASE* sourceItem	= slot->GetItemInfoAbs( POSTYPE( message->dwData1 ) );
			const ITEMBASE* targetItem	= slot->GetItemInfoAbs( POSTYPE( message->dwData2 ) );

			if( !	sourceItem ||
				!	targetItem  )
			{
				return;
			}

			const ITEMBASE savedSourceItem = *sourceItem;

			ITEM_OPTION option = GetOption( *targetItem );
			{
				option.mItemDbIndex = targetItem->dwDBIdx;

				ZeroMemory( &option.mDrop, sizeof( option.mDrop ) );

				ApplyOption( *sourceItem, *targetItem, option );
			}

			{
				const ITEM_OPTION emptyOption = { 0 };

				if( ! memcmp( &option.mDrop, &emptyOption.mDrop, sizeof( option.mDrop ) ) )
				{
					// ����Ʈ ����
					{
						MSG_DWORD message;
						message.Category	= MP_ITEM;
						message.Protocol	= MP_ITEM_WORKING_FAIL;
						message.dwData		= player->GetID();

						PACKEDDATA_OBJ->QuickSend( player, &message, sizeof( message ) );
					}

					{
						MSGBASE message;
						message.Category	= MP_ITEM;
						message.Protocol	= MP_ITEM_APPLY_OPTION_NACK;

						player->SendMsg( &message, sizeof( message ) );
					}
					
					return;
				}
			}

			if( EI_TRUE != DiscardItem( player, sourceItem->Position, sourceItem->wIconIdx, 1 ) )
			{
				MSGBASE message;
				message.Category	= MP_ITEM;
				message.Protocol	= MP_ITEM_APPLY_OPTION_NACK;

				player->SendMsg( &message, sizeof( message ) );
				return;
			}

			AddOption( option );
			// 081118 LUJ, ĳ���� ��ȣ�� ������ ������ DB �����带 ������ Ű������ ���ǵ��� ��
			ItemDropOptionInsert( option, player->GetID() );
			// 080328 LUJ, ��� �ɼ� �α�
			LogItemDropOption(
				option,
				eLog_ShopItemUse );

			// 080627 LUJ, ����� ������ �α�
			{
				const ITEM_INFO* info = GetItemInfo( savedSourceItem.wIconIdx );

				LogItemMoney(
					player->GetID(),
					player->GetObjectName(),
					0,
					"*applyOption",
					eLog_ItemUse,
					player->GetMoney(),
					0,
					0,
					savedSourceItem.wIconIdx,
					savedSourceItem.dwDBIdx,
					savedSourceItem.Position,
					0,
					info->Stack ? savedSourceItem.Durability - 1 : 0,
					0 );

				// 081027 LUJ, � �ɼǿ� �����ߴ��� ǥ���Ѵ�
				LogItemMoney(
					player->GetID(),
					player->GetObjectName(),
					0,
					"*applyOption",
					eLog_ItemDropOptionAdd,
					player->GetMoney(),
					0,
					0,
					targetItem->wIconIdx,
					targetItem->dwDBIdx,
					targetItem->Position,
					0,
					targetItem->Durability,
					0 );
			}

			{
				MSG_ITEM_OPTION message;
				message.Category		= MP_ITEM;
				message.Protocol		= MP_ITEM_OPTION_ADD_ACK;
				message.mSize			= 1;
				message.mOption[ 0 ]	= option;

				player->SendMsg( &message, message.GetSize() );
			}

			{
				MSG_DWORD message;
				message.Category	= MP_ITEM;
				message.Protocol	= MP_ITEM_APPLY_OPTION_ACK;
				message.dwData		= targetItem->dwDBIdx;

				player->SendMsg( &message, sizeof( message ) );
			}

			// ����Ʈ ����
			{
				MSG_DWORD message;
				message.Category	= MP_ITEM;
				message.Protocol	= MP_ITEM_WORKING_SUCCESS;
				message.dwData		= player->GetID();

				PACKEDDATA_OBJ->QuickSend( player, &message, sizeof( message ) );
			}

			break;
		}
		case MP_ITEM_SHOPITEM_SHOUT_SYN:
		{
			SEND_SHOUTBASE_ITEMINFO* pmsg = (SEND_SHOUTBASE_ITEMINFO*)pMsg;

			POSTYPE itempos = (POSTYPE)pmsg->dwItemPos;

			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			
			char coppyname[MAX_NAME_LENGTH+1];
			SafeStrCpy(coppyname, pmsg->Name, sizeof(pmsg->Name) / sizeof(*pmsg->Name));

			CPlayer* pTarget = (CPlayer*)g_pUserTable->FindUserForName( coppyname );	
			
			if (pPlayer->GetObjectName() != pTarget->GetObjectName())
			{
				MSG_DWORD msg;
				msg.Category	= MP_ITEM;
				msg.Protocol	= MP_ITEM_SHOPITEM_SHOUT_NACK;
				msg.dwData		= pPlayer->GetGridID() - 1;
				pPlayer->SendMsg( &msg, sizeof( msg ) );
				return;
			}

			if(pPlayer == NULL)
				return;

			CItemSlot * pSlot = pPlayer->GetSlot( eItemTable_Inventory );

			if( ! pSlot )
				return;

			if(g_pServerSystem->GetMapNum() == AlkerResidence)
			{
				MSG_DWORD msg;
				msg.Category	= MP_ITEM;
				msg.Protocol	= MP_ITEM_SHOPITEM_SHOUT_NACK;
				msg.dwData		= pPlayer->GetGridID() - 1;
				pPlayer->SendMsg( &msg, sizeof( msg ) );
				return;
			}

			const ITEMBASE * pItemBase = pSlot->GetItemInfoAbs(itempos);

			// ������ ��ȿ�� üũ
			if( ! pItemBase || pItemBase->nSealed == eITEM_TYPE_SEAL || eItemUseSuccess != UseItem(pPlayer, (WORD)pmsg->dwItemPos, pmsg->dwItemIdx))
			{
				MSG_DWORD msg;
				msg.Category	= MP_ITEM;
				msg.Protocol	= MP_ITEM_SHOPITEM_SHOUT_NACK;
				msg.dwData		= pPlayer->GetGridID() - 1;
				pPlayer->SendMsg( &msg, sizeof( msg ) );
				return;
			}

			ITEM_INFO* pItemInfo = GetItemInfo( pmsg->dwItemIdx );

			if( pItemInfo->SupplyType != ITEM_KIND_WORLDSHOUT ) return;

			// ��Ŷ���� �� ��ȯ
			SEND_SHOUTBASE_ITEMINFO msgforServer;
			memcpy(&msgforServer, pmsg, sizeof(SEND_SHOUTBASE_ITEMINFO));
			msgforServer.Category = MP_CHAT;
			msgforServer.Protocol = MP_CHAT_SHOUT_ACK;
			msgforServer.dwItemIdx = (DWORD)-1;	//Item�����ʵ带 ��ȿȭ
			msgforServer.dwItemPos = (DWORD)-1;	//Item�����ʵ带 ��ȿȭ
			msgforServer.wItemSupplyValue = (WORD)pItemInfo->SupplyValue;
			// ��� Agent������ ����
			g_Network.Broadcast2AgentServer((char*)&msgforServer, msgforServer.GetMsgLength());

			// �������� ����뺸
			MSG_ITEM_USE_ACK msgforUser;
			msgforUser.Category	= MP_ITEM;
			msgforUser.Protocol	= MP_ITEM_USE_ACK;
			msgforUser.dwObjectID = pmsg->dwObjectID;
			msgforUser.TargetPos = (POSTYPE)pmsg->dwItemPos;
			msgforUser.dwItemIdx = pmsg->dwItemIdx;
			msgforUser.eResult	 = eItemUseSuccess;
			pPlayer->SendMsg(
				&msgforUser,
				sizeof(msgforUser));
		}
		break;
		// 080414 LUJ, �ܾ� ��ȯ
	case MP_ITEM_SHOPITEM_CHARCHANGE_SYN:
		{
			const MSG_CHARACTER_CHANGE* message = ( MSG_CHARACTER_CHANGE* )pMsg;

			// �÷��̾� ���� �˻�
			CPlayer* player = ( CPlayer* )g_pUserTable->FindUser( message->dwObjectID );

			if( ! player )
			{
				break;
			}

			CItemSlot* slot = player->GetSlot( eItemTable_Inventory );

			if( ! slot )
			{
				break;
			}

			const ITEMBASE* item = slot->GetItemInfoAbs( message->mItem.Position );

			// 080414 LUJ, ������ ��ȿ�� �˻�. Ŭ���̾�Ʈ���� ������ ������ ��ġ�ؾ��Ѵ�
			{
				if( !	item ||
						memcmp( item, &message->mItem, sizeof( *item ) ) ||
						mBodyChangeScriptListMap.end() == mBodyChangeScriptListMap.find( item->wIconIdx ) )
				{
					break;
				}
				
				const CHARACTERCHANGE_INFO& info		= message->mInfo;
				const BodyChangeScriptList& scriptList	= mBodyChangeScriptListMap[ item->wIconIdx ];

				BOOL isValidHair = FALSE;
				BOOL isValidFace = FALSE;

				// 080414 LUJ, ����Ʈ�� ��ȯ�ؼ� �÷��̾��� ���� �Ͽ� ���� ���� ������ TRUE�� �����س��´�
				for(
					BodyChangeScriptList::const_iterator it = scriptList.begin();
					scriptList.end() != it;
					++it )
				{
					const BodyChangeScript& script = *it;

					if( script.mGender	!= (DWORD)player->GetGender()	||
						script.mRace	!= player->GetRacial() )
					{
						continue;
					}

					switch( script.mPart )
					{
					case ePartType_Face:
						{
							// 080414 LUJ, �˻縦 ������� ���� ��츸 ��ȿ�� üũ
							if( ! isValidFace )
							{
								isValidFace = ( script.mValue == info.FaceType );
							}

							break;
						}
					case ePartType_Hair:
						{
							// 080414 LUJ, �˻縦 ������� ���� ��츸 ��ȿ�� üũ
							if( ! isValidHair )
							{
                                // 080523 LUJ, ���� �߸� ���ϴ� ���� �Ӹ��� ����
								isValidHair = ( script.mValue == info.HairType );
							}
							
							break;
						}
					}
				}

				// 080414 LUJ, Ŭ���̾�Ʈ���� ������ ���� �������� ����
				if( ! isValidFace ||
					! isValidHair )
				{
					break;
				}
			}

			// 080414 LUJ, DB�� �����Ѵ�. �÷��̾ �����ӵǹǷ� �޸𸮴� �ٽ� �ε��� ���̴�
			if( IsDupItem( item->wIconIdx ) &&
				1 < item->Durability )
			{
				ItemUpdateToDB(
					player->GetID(),
					item->dwDBIdx,
					item->wIconIdx,
					item->Durability - 1,
					item->Position,
					item->QuickPosition );				
			}
			else
			{
				ItemDeleteToDB( item->dwDBIdx );
			}

			// 080414 LUJ, �α�
			LogItemMoney(
				player->GetID(),
				player->GetObjectName(),
				0,
				"*change",
				eLog_ItemUse,
				player->GetMoney(),
				0,
				0,
				item->wIconIdx,
				item->dwDBIdx,
				item->Position,
				0,
				item->Durability,
				0 );

			// 080414 LUJ, ����� �� �÷��̾� ������ ����ǹǷ� �޸𸮸� ������Ʈ���ָ� �ȴ�
			{
				CHARACTER_TOTALINFO info = player->GetCharacterTotalInfo();

				info.HairType	= message->mInfo.HairType;
				info.FaceType	= message->mInfo.FaceType;

				player->InitCharacterTotalInfo( &info );
			}

			// 080414 LUJ, Ŭ���̾�Ʈ�� ����� ������ �ʴ´�. �����ӵ� ���̱� ����
		}
		break;
	case MP_ITEM_GIFT_EVENT_NOTIFY:
		{
			MSG_DWORD4* pmsg = ( MSG_DWORD4* )pMsg;

			DWORD characteridx = pmsg->dwData1;
			DWORD eventidx = pmsg->dwData2;
			DWORD itemidx = pmsg->dwData3;
			DWORD itemcount = pmsg->dwData4;

			CPlayer* pPlayer = ( CPlayer* )g_pUserTable->FindUser( characteridx );

			if( !pPlayer )
			{
				FILE* pFile = fopen( "./log/GiftEventLog.txt", "a+" );
				fprintf( pFile, "%u\tInvalid User(CItemManager::NetworkMsgParse) - character_idx : %u\n", gCurTime, characteridx );
				fclose( pFile );
				return;
			}
			if( !GetItemInfo( itemidx ) )
			{
				FILE* pFile = fopen( "./log/GiftEventLog.txt", "a+" );
				fprintf( pFile, "%u\tInvalid Item(CItemManager::NetworkMsgParse) - item_idx : %u\n", gCurTime, itemidx );
				fclose( pFile );
				return;
			}
			DWORD useridx = pPlayer->GetUserID();
			GiftItemInsert( characteridx, useridx, eventidx, itemidx, itemcount, IsDupItem( itemidx ) );

			// 080509 LUJ, �α� ����.	DB�ε����� �����ϱ� ��ư� �Ǿ��ִ�. �׷��� �������� ���� ������ â���� ����
			//							�̸� ���� �� DB �ε����� �α׿� �����Ƿ� ������ �� �ִ�.
			LogItemMoney(
				pPlayer->GetID(),
				pPlayer->GetObjectName(),
				0,
				"*gift",
				eLog_ItemObtainGift,
				pPlayer->GetMoney(),
				0,
				0,
				itemidx,
				0,
				0,
				0,
				itemcount,
				0 );
		}
		break;
	case MP_ITEM_COMPOSE_SYN:
		{
			const MSG_ITEM_COMPOSE_SYN* message = ( MSG_ITEM_COMPOSE_SYN* )pMsg;
	
			Compose( *message );
		}
		break;
	}
}


BOOL CItemManager::ItemMoveUpdateToDBbyTable(CPlayer* pPlayer, DWORD dwfromDBIdx, DWORD dwfromIconIdx, POSTYPE frompos, DWORD dwtoDBIdx, POSTYPE topos)
{
	if(TP_INVENTORY_START <= frompos && frompos < TP_WEAR_END)
	{
		ItemMoveUpdateToDB(pPlayer->GetID(), dwfromDBIdx, frompos, dwtoDBIdx, topos, pPlayer->GetUserID(), pPlayer->GetGuildIdx());
	}
	else if(TP_GUILDWAREHOUSE_START <= frompos && frompos < TP_GUILDWAREHOUSE_END)
	{
		ItemMoveGuildUpdateToDB(pPlayer->GetID(), pPlayer->GetGuildIdx(), dwfromDBIdx, frompos, dwtoDBIdx, topos);
	}
	else if(TP_STORAGE_START <= frompos && frompos < TP_STORAGE_END)
	{
		ItemMoveStorageUpdateToDB(pPlayer->GetID(), pPlayer->GetUserID(), dwfromDBIdx, frompos, dwtoDBIdx, topos);
	}
	else
	{
		ASSERT(0);
		return FALSE;
	}
	return TRUE;
}

void CItemManager::ItemUpdatebyTable(CPlayer* pPlayer, DWORD dwDBIdx, DWORD wIconIdx, DURTYPE FromDur, POSTYPE Position, POSTYPE QuickPosition)
{
	if(TP_INVENTORY_START <= Position && Position < TP_WEAR_END)
	{
		ItemUpdateToDB( pPlayer->GetID(), dwDBIdx, wIconIdx, FromDur, Position, QuickPosition );
	}
}

BOOL CItemManager::CanMovetoGuildWare(POSTYPE FromPos, POSTYPE ToPos, CPlayer* pPlayer)
{
	CGuild*	guild = GUILDMGR->GetGuild(
		pPlayer->GetGuildIdx());

	if(0 == guild ||
		guild->GetPlayerInWarehouse() != pPlayer->GetID())
	{
		return FALSE;
	}
	else if(const CGuildManager::LevelSetting* setting = GUILDMGR->GetLevelSetting(guild->GetLevel()))
	{
		const DWORD size = TABCELL_GUILDWAREHOUSE_NUM * setting->mWarehouseSize;

		if( TP_GUILDWAREHOUSE_START <= ToPos && ToPos < TP_GUILDWAREHOUSE_START + size )
		{
			return pPlayer->GetSlot(ToPos)->IsEmpty(ToPos);
		}
	}

	return FALSE;
}

void CItemManager::ClearQuickPos(ITEMBASE* pItem, POSTYPE ToPos)
{
	if((TP_GUILDWAREHOUSE_START <= ToPos && ToPos < TP_GUILDWAREHOUSE_END) 
		|| (TP_STORAGE_START <= ToPos && ToPos < TP_STORAGE_END))
	{
		pItem->QuickPosition = 0;
		return;
	}
}
DealerData * CItemManager::GetDealer(WORD wDealerIdx)
{
	return m_DealerTable.GetData(wDealerIdx);
}
void CItemManager::LoadDealerItem()
{
	char buff[2048] = {0};
	char mapname[256] = {0};
	char npcname[256] = {0};
	DWORD ItemIdx = 0;
	DealerItem* pItem = 0;
	DealerData* pCurDealer = NULL;
	LPCTSTR seps = "\t\n";
	char* token = 0;
	int count = 0;

	CMHFile fp;
	fp.Init("./System/Resource/DealItem.bin","rb");

	while(FALSE == fp.IsEOF())
	{
		count++;
		fp.GetWord();
		fp.GetString(mapname);
		fp.GetWord();
		fp.GetString(npcname);
		WORD npc_index = fp.GetWord();
		fp.GetWord();
		fp.GetWord();
		fp.GetWord();
		BYTE tabnum = fp.GetByte();
		BYTE Pos = 0;
		
		fp.GetLine(buff,2048);

		token = strtok( buff, seps );
		if(token == NULL)
			continue;
		token = strtok( NULL, seps );
		ItemIdx = atoi(token);

		pCurDealer = m_DealerTable.GetData(npc_index);
		if(pCurDealer == NULL)
		{
			pCurDealer = new DealerData;
			m_DealerTable.Add(pCurDealer,npc_index);
		}
		
		if( ItemIdx != 0 )
		{
			pItem = new DealerItem;
			ASSERT(tabnum);
			pItem->Tab = tabnum-1;
			pItem->Pos = Pos++;
			pItem->ItemIdx = ItemIdx;
			pCurDealer->m_DealItemList.AddTail(pItem);
		}
		else
		{
			Pos++;
		}

		while( 1 )
		{
			token = strtok( NULL, seps );
			if(token == NULL)
				break;
			token = strtok( NULL, seps );
			ItemIdx = atoi(token);
			
			if( ItemIdx != 0 )
			{
				pItem = new DealerItem;
				pItem->Tab = tabnum-1;
				pItem->Pos = Pos++;
				pItem->ItemIdx = ItemIdx;
				pCurDealer->m_DealItemList.AddTail(pItem);
			}
			else
			{
				Pos++;
			}
		}
	}
	fp.Release();
}

int CItemManager::ObtainItemFromQuest( CPlayer* pPlayer, DWORD wItemKind, DWORD dwItemNum )
{
	WORD EmptyCellPos[255];
	WORD EmptyCellNum;
	CItemSlot* pSlot = pPlayer->GetSlot(eItemTable_Inventory);
	WORD obtainItemNum = GetCanBuyNumInSpace( pPlayer, pSlot, wItemKind, dwItemNum, EmptyCellPos, EmptyCellNum );
	if( obtainItemNum == 0 )
	{
		SendErrorMessage(
			pPlayer,
			MP_ITEM_BUY_NACK,
			NOT_SPACE);
		return 1;
	}

	const ITEM_INFO* const itemInfo = GetItemInfo(
		wItemKind);

	if(0 == itemInfo)
	{
		return 1;
	}

	return ObtainItemEx(
		pPlayer,
		Alloc(pPlayer, MP_QUEST, MP_QUEST_TAKEITEM_ACK, pPlayer->GetID(), 0, eLog_ItemObtainQuest, obtainItemNum, DBResult(ObtainItemDBResult)),
		wItemKind,
		obtainItemNum,
		EmptyCellPos,
		EmptyCellNum,
		EmptyCellNum,
		itemInfo->wSeal);
}

int CItemManager::ObtainItemFromChangeItem( CPlayer* pPlayer, DWORD wItemKind, WORD wItemNum )
{
	WORD EmptyCellPos[255];
	WORD EmptyCellNum;

	CItemSlot* pSlot = pPlayer->GetSlot(eItemTable_Inventory);

	ITEM_INFO* pItemInfo = GetItemInfo( wItemKind );
	if( !pItemInfo )		return 2;
	
	WORD obtainItemNum = GetCanBuyNumInSpace( pPlayer, pSlot, wItemKind, wItemNum, EmptyCellPos, EmptyCellNum );
	if( obtainItemNum == 0 )
	{
		SendErrorMessage(
			pPlayer,
			MP_ITEM_BUY_NACK,
			NOT_SPACE);
		return 1;
	}

	const ITEM_INFO* const itemInfo = GetItemInfo(
		wItemKind);

	if(0 == itemInfo)
	{
		return 1;
	}

	return ObtainItemEx(
		pPlayer,
		Alloc(pPlayer, MP_ITEM, MP_ITEM_USE_CHANGEITEM_ACK, pPlayer->GetID(), 0, eLog_ItemObtainFromChangeItem, obtainItemNum, (DBResult)(ObtainItemDBResult)),
		wItemKind,
		obtainItemNum,
		EmptyCellPos,
		EmptyCellNum,
		EmptyCellNum,
		itemInfo->wSeal);
}

BOOL CItemManager::CanbeMoved(DWORD wIconIdx,POSTYPE pos, CPlayer* pPlayer)
{
	//if(TP_INVENTORY_START<= pos && pos < TP_INVENTORY_END)
	//	071210 KTH -- Ȯ���κ��丮���� �˻�
	if(TP_INVENTORY_START<= pos && pos < (TP_INVENTORY_END + pPlayer->GetInventoryExpansionSize() ))
	{
		ITEM_INFO * pInfo = GetItemInfo(wIconIdx);
		if(!pInfo) return FALSE;
/*		if( pInfo->ItemKind & eSHOP_ITEM )
			return FALSE;
*/
		return TRUE;
	}
	else if(TP_WEAR_START <= pos && pos < TP_WEAR_END)
	{
		ITEM_INFO * pInfo = GetItemInfo(wIconIdx);
		if(!pInfo) return FALSE;

		if(pInfo->EquipSlot == eWearedItem_Earring1)
		{	
			if(pos-TP_WEAR_START != eWearedItem_Earring1 && pos-TP_WEAR_START != eWearedItem_Earring2)
				return FALSE;
		}
		else if(pInfo->EquipSlot == eWearedItem_Ring1)
		{	
			if(pos-TP_WEAR_START != eWearedItem_Ring1 && pos-TP_WEAR_START != eWearedItem_Ring2)
				return FALSE;
		}
		// 080703 LUJ, �̵��� ���°� �ƴ� ��� ���� �Ұ�
		else if( pos-TP_WEAR_START != pInfo->EquipSlot )
		{
			if( ! pPlayer->GetHeroTotalInfo()->bUsingTwoBlade )
				return FALSE;
		}
		
		return CanEquip(pInfo, pPlayer);
	}
	// 080513 KTH -- â�� Ȯ�� ũ�⸦ ������ �ȱ�� �ִ��� üũ �Ѵ�.
	//else if(TP_STORAGE_START <= pos && pos < TP_STORAGE_END)
	else if( TP_STORAGE_START <= pos && pos < TP_STORAGE_START + (pPlayer->GetStorageNum()*TABCELL_STORAGE_NUM) )//TP_STORAGE_END)
	{
		return TRUE;
	}
	else if(TP_GUILDWAREHOUSE_START <= pos && pos < TP_GUILDWAREHOUSE_END)
		return TRUE;
	else if(TP_SHOPITEM_START <= pos && pos < TP_SHOPITEM_END)
	{
		ITEM_INFO * pInfo = GetItemInfo(wIconIdx);
		if(!pInfo) return FALSE;
/*		if( !(pInfo->ItemKind & eSHOP_ITEM) )
			return FALSE;
*/		return TRUE;
	}
	else
		return FALSE;
}

BOOL CItemManager::CanEquip(ITEM_INFO* pInfo, CPlayer* pPlayer)
{
	if(pInfo->LimitLevel > pPlayer->GetLevel())
	{
			return FALSE;
	}

	return TRUE;	
}

void CItemManager::ItemMoveLog(POSTYPE FromPos, POSTYPE ToPos, CPlayer* pPlayer, ITEMBASE* pItem)
{
	DWORD FromID = 0;
	DWORD ToID = 0;
	eLogitemmoney Logtype = eLog_ItemNone;
	MONEYTYPE FromTotalMoney =0;
	MONEYTYPE ToTotalMoney = 0;
	char FromName[MAX_NAME_LENGTH+1];
	char ToName[MAX_NAME_LENGTH+1];

	if(TP_INVENTORY_START<= FromPos && FromPos < TP_WEAR_END)
	{
		FromID = pPlayer->GetID();
		FromTotalMoney = pPlayer->GetMoney();
		
		if(TP_STORAGE_START <= ToPos && ToPos < TP_STORAGE_END)
		{
			ToID = pPlayer->GetUserID();
			Logtype = eLog_ItemMoveInvenToStorage;
			ToTotalMoney = pPlayer->GetMoney(eItemTable_Storage);
			SafeStrCpy(FromName, pPlayer->GetObjectName(), MAX_NAME_LENGTH+1);
			SafeStrCpy(ToName, "#Storage", MAX_NAME_LENGTH+1);
		}
		else if(TP_GUILDWAREHOUSE_START <= ToPos && ToPos < TP_GUILDWAREHOUSE_END)
		{
			ToID = pPlayer->GetGuildIdx();
			Logtype = eLog_ItemMoveInvenToGuild;
			SafeStrCpy(FromName, pPlayer->GetObjectName(), MAX_NAME_LENGTH+1);
			SafeStrCpy(ToName, "#Munpa", MAX_NAME_LENGTH+1);
		}
		else
		{
			return;
		}
	}
	else if(TP_STORAGE_START <= FromPos && FromPos < TP_STORAGE_END)
	{
		if(TP_INVENTORY_START<= ToPos && ToPos < TP_WEAR_END)
		{
			FromID = pPlayer->GetUserID();
			FromTotalMoney = pPlayer->GetMoney(eItemTable_Storage);
			ToTotalMoney = pPlayer->GetMoney();
			ToID = pPlayer->GetID();
			Logtype = eLog_ItemMoveStorageToInven;
			
			SafeStrCpy(FromName, "#Storage", MAX_NAME_LENGTH+1);
			SafeStrCpy(ToName, pPlayer->GetObjectName(), MAX_NAME_LENGTH+1);
		}
		else
		{
			ASSERT(0); return;			
		}

	}
	else if(TP_GUILDWAREHOUSE_START <= FromPos && FromPos < TP_GUILDWAREHOUSE_END)
	{
		FromID			= pPlayer->GetGuildIdx();
		ToID			= pPlayer->GetID();
		ToTotalMoney	= pPlayer->GetMoney();
		SafeStrCpy(FromName, "#Munpa", MAX_NAME_LENGTH+1);
		SafeStrCpy(ToName, pPlayer->GetObjectName(), MAX_NAME_LENGTH+1);

		if(TP_INVENTORY_START<= ToPos && ToPos < TP_WEAR_END)
		{			
			Logtype = eLog_ItemMoveGuildToInven;
		}
		else
		{
			ASSERT(0);
			return;
		}
	}
	else if(TP_SHOPITEM_START <= FromPos && FromPos < TP_SHOPITEM_END)
	{
		if(TP_INVENTORY_START<= ToPos && ToPos < TP_WEAR_END)
		{
			FromID			= pPlayer->GetID();
			ToID			= pPlayer->GetID();
			ToTotalMoney	= 0;
			Logtype			= eLog_ShopItemMoveToInven;
			SafeStrCpy( FromName,	"#ShopInven",				sizeof( FromName ) );
			SafeStrCpy( ToName,		pPlayer->GetObjectName(),	sizeof( ToName ) );
		}
		else
		{
			return;
		}		
	}
	else 
		return; 

	// ��� â�� �α״� ���� ��쿡 �����
	if(	eLog_ItemMoveInvenToGuild	== Logtype	||
		eLog_ItemMoveGuildToInven	== Logtype	||
		eLog_ItemMoveGuild			== Logtype)
	{
		// 081201 LUJ, ��� â�� �α� Ÿ���� �˱� ������ ����
		InsertLogGuildWarehouse( pPlayer, FromPos, pItem, 0, Logtype );
	}

	LogItemMoney(FromID, FromName, ToID, ToName,
		Logtype, FromTotalMoney, ToTotalMoney, 0,
			pItem->wIconIdx, pItem->dwDBIdx,  FromPos, ToPos, pItem->Durability, 0 );
}


eWeaponType CItemManager::GetWeaponKind(DWORD wWeapon)
{
	const ITEM_INFO* const pItemInfo = m_ItemInfoList.GetData( wWeapon );

	return pItemInfo ? pItemInfo->WeaponType : eWeaponType_None;
}

BOOL CItemManager::CheckHackNpc( CPlayer* pPlayer, WORD wNpcIdx, WORD wParam )
{
	if( wParam == eNpcParam_ShowpStorage )
		return TRUE;

	// 100104 ShinJS --- PC�� Mall �� ���
	if( PCROOMMGR->GetMallNPCIdx() == wNpcIdx )
		return TRUE;

	STATIC_NPCINFO* pInfo = GAMERESRCMNGR->GetStaticNpcInfo( wNpcIdx );
	if( pInfo == NULL )	return FALSE;

	VECTOR3 vPos;
	pPlayer->GetPosition( &vPos );
	float dist = CalcDistanceXZ( &vPos, &pInfo->vPos );
	if( dist > 3000.0f )	return FALSE;

	return TRUE;
}


BOOL CItemManager::CheckHackItemMove( CPlayer* pPlayer, CItemSlot* pFromSlot, CItemSlot* pToSlot )
{
	if( ! pPlayer	||
		! pFromSlot ||
		! pToSlot )
	{
		return FALSE;
	}

	// 090223 LUJ, ��� â���� ����� �̵��� ��� �˻�
	{
		const CItemSlot* const	guildSlot		= pPlayer->GetSlot( eItemTable_MunpaWarehouse );
		const BOOL				isMoveWithGuild	= ( guildSlot == pFromSlot || guildSlot == pToSlot );

		if( isMoveWithGuild )
		{
			const CGuild* const guild = GUILDMGR->GetGuild(
				pPlayer->GetGuildIdx());

			if(0 == guild)
			{
				return FALSE;
			}
			else if(guild->GetPlayerInWarehouse() != pPlayer->GetID())
			{
				return FALSE;
			}
		}
	}

	// 070808 ����, ������϶��� üũ���� �ʵ��� �Ѵ�. ���߿� ������
#ifndef _DEBUG
	VECTOR3 vPos;
	pPlayer->GetPosition( &vPos );

	CItemSlot* pGuildSlot = pPlayer->GetSlot( eItemTable_MunpaWarehouse );
	CItemSlot* pWareSlot = pPlayer->GetSlot( eItemTable_Storage );

	if( pFromSlot == pGuildSlot || pToSlot == pGuildSlot || pFromSlot == pWareSlot || pToSlot == pWareSlot )
	{
		if( !	g_pServerSystem->GetMap()->IsVillage() ||
			!	GAMERESRCMNGR->IsInWareNpcPosition( &vPos ) )
		{
			return FALSE;
		}
	}
#endif

	return TRUE;
}


int CItemManager::GetSetItemSize( CPlayer* player, const SetScript* script )
{
	if( ! script || ! player )
	{
		ASSERT( 0 );
		return 0;
	}

	int size = 0;

	for( POSTYPE part = TP_WEAR_START; part < TP_WEAR_END; ++part )
	{
		const ITEMBASE * itemBase = GetItemInfoAbsIn(player, part );

		if( itemBase &&
			itemBase->dwDBIdx )
		{
			const ITEM_INFO * itemInfo = GetItemInfo( itemBase->wIconIdx );

			if( itemInfo &&
				script->mItem.end() != script->mItem.find( itemInfo->ItemIdx ) )
			{
				++size;
			}
		}
	}

	return size;
}

void CItemManager::LoadMixSetting()
{
	CMHFile file;

	if( ! file.Init( "System/Resource/mix_setting.bin", "rb" ) )
	{
		ASSERT( 0 );
		return;
	}

	std::list< std::string >	text;

	int	openCount = 0;

	enum eTag
	{
		eTagNone,
		eTagRate,
		eTagValue,
	}
	mTag = eTagNone;

	for(	TCHAR line[ MAX_PATH ];
			! file.IsEOF();
			ZeroMemory( line, sizeof( line ) ) )
	{
		file.GetLine( line, sizeof( line ) / sizeof( TCHAR ) );

		const int length = _tcslen( line );

		if( ! length )
		{
			continue;
		}

		// �߰��� �ּ� ����
		{
			for( int i = 0; i < length - 1; ++i )
			{
				const char a = line[ i ];
				const char b = line[ i + 1 ];

				if( '/' == a &&
					'/' == b )
				{
					line[ i ] = 0;
					break;
				}
			}
		}

		text.push_back( line );

		const TCHAR* separator	= _T( " ~\n\t=,+\"()%" );
		const TCHAR* token		= _tcstok( line, separator );
		const TCHAR* markBegin	= _T( "{" );
		const TCHAR* markEnd	= _T( "}" );

		if( 0 == token )
		{
			text.pop_back();
		}		
		else if( 0 == _tcsicmp( token, "rate" ) )
		{
			mTag = eTagRate;

			text.pop_back();
		}
		else if( 0 == _tcsicmp( token, "value" ) )
		{
			mTag = eTagValue;

			text.pop_back();
		}
		else if( 0 == _tcsnicmp( markBegin, token, strlen( markBegin ) ) )
		{
			++openCount;
		}
		else if( 0 == _tcsnicmp( markEnd, token, strlen( markEnd ) ) )
		{
			if( 0 < --openCount )
			{
				continue;
			}

			switch( mTag )
			{
			case eTagRate:
				{
					float					step	= 0;
					MixSetting::Ability*	ability = 0;

					for( std::list< std::string >::const_iterator it = text.begin(); text.end() != it; ++it )
					{
						strcpy( line, it->c_str() );

						token = _tcstok( line, separator );

						// ����
						if( ! strcmpi( token, "sword" ) )
						{
							ability = &( mMixSetting.mEquippment[eStatusKind_Sword] );

							step = 0;
						}
						else if( ! strcmpi( token, "axe" ) )
						{
							ability = &( mMixSetting.mEquippment[eStatusKind_Axe] );

							step = 0;
						}
						else if( ! strcmpi( token, "mace" ) )
						{
							ability = &( mMixSetting.mEquippment[eStatusKind_Mace] );

							step = 0;
						}
						else if( ! strcmpi( token, "dagger" ) )
						{
							ability = &( mMixSetting.mEquippment[eStatusKind_Dagger] );

							step = 0;
						}
						else if( ! strcmpi( token, "bow" ) )
						{
							ability = &( mMixSetting.mEquippment[eStatusKind_Bow] );

							step = 0;
						}
						// 080910 LUJ, �� �߰�
						else if( ! strcmpi( token, "gun" ) )
						{
							ability = &( mMixSetting.mEquippment[eStatusKind_Gun] );

							step = 0;
						}
						else if( ! strcmpi( token, "dual_weapon" ) )
						{
							ability = &( mMixSetting.mEquippment[eStatusKind_BothDagger] );

							step = 0;
						}
						else if( ! strcmpi( token, "fishingPole" ) )
						{
							ability = &( mMixSetting.mEquippment[eStatusKind_FishingPole] );

							step = 0;
						}
						else if( ! strcmpi( token, "cookUtil" ) )
						{
							ability = &( mMixSetting.mEquippment[eStatusKind_CookUtil] );

							step = 0;
						}
						else if( ! strcmpi( token, "staff" ) )
						{
							ability = &( mMixSetting.mEquippment[eStatusKind_Staff] );

							step = 0;
						}
						else if( ! strcmpi( token, "robe" ) )
						{
							ability = &( mMixSetting.mEquippment[eStatusKind_RobeArmor] );

							step = 0;
						}
						else if( ! strcmpi( token, "light_armor" ) )
						{
							ability = &( mMixSetting.mEquippment[eStatusKind_LightArmor] );

							step = 0;
						}
						else if( ! strcmpi( token, "heavy_armor" ) )
						{
							ability = &( mMixSetting.mEquippment[eStatusKind_HeavyArmor] );

							step = 0;
						}

						// �ɷ�ġ
						if( ! strcmpi( token, "strength" ) )
						{
							// 080925, ���� 0 ~ 1 ������ �Ǽ��� ������
							step += float( atof( _tcstok( 0, separator ) ) / 100.0f );

							( *ability )[ step ] = MixSetting::eAbilityStrength;
						}
						else if( ! strcmpi( token, "intelligence" ) )
						{
							// 080925, ���� 0 ~ 1 ������ �Ǽ��� ������
							step += float( atof( _tcstok( 0, separator ) ) / 100.0f );

							( *ability )[ step ] = MixSetting::eAbilityIntelligence;
						}
						else if( ! strcmpi( token, "dexterity" ) )
						{
							// 080925, ���� 0 ~ 1 ������ �Ǽ��� ������
							step += float( atof( _tcstok( 0, separator ) ) / 100.0f );

							( *ability )[ step ] = MixSetting::eAbilityDexterity;
						}
						else if( ! strcmpi( token, "vitality" ) )
						{
							// 080925, ���� 0 ~ 1 ������ �Ǽ��� ������
							step += float( atof( _tcstok( 0, separator ) ) / 100.0f );

							( *ability )[ step ] = MixSetting::eAbilityVitality;
						}
						else if( ! strcmpi( token, "wisdom" ) )
						{
							// 080925, ���� 0 ~ 1 ������ �Ǽ��� ������
							step += float( atof( _tcstok( 0, separator ) ) / 100.0f );

							( *ability )[ step ] = MixSetting::eAbilityWisdom;
						}
					}

					break;
				}
			case eTagValue:
				{
					MixSetting::Range* range = 0;

					for( std::list< std::string >::const_iterator it = text.begin(); text.end() != it; ++it )
					{
						strcpy( line, it->c_str() );

						token = _tcstok( line, separator );

						if( ! strcmpi( "level", token ) )
						{
							_tcstok( 0, separator );
							const DWORD maxLevel = atoi( _tcstok( 0, separator ) );

							range = &( mMixSetting.mRangeMap[ maxLevel ] );
						}
						else if( ! strcmpi( "range", token ) )
						{
							const DWORD minRange = atoi( _tcstok( 0, separator ) );
							const DWORD maxRange = atoi( _tcstok( 0, separator ) );

							range->mMin	= minRange;
                            range->mMax	= maxRange;
						}
					}

					break;
				}
			case eTagNone:
				{
					ASSERT( 0 );
					break;
				}
			}

			text.clear();
		}
	}

	if( openCount )
	{
		ASSERT( 0 && "MixSetting.bin�� {} ������ �ùٷ� ������ �ʾҽ��ϴ�" );
	}
}


// 080228 LUJ, ��ȣ �������� ���� ��ũ��Ʈ �Ľ� ����
// 080925 LUJ, ���� ���� ������ ������ �о����
void CItemManager::LoadMixScript()
{
	CMHFile file;

	if( ! file.Init( "System/Resource/ItemMix.bin", "rb" ) )
	{
		ASSERT( 0 );
		return;
	}

	char buffer[ MAX_PATH * 5 ];

	enum Type
	{
		TypeNone,
		TypeProtection,
		TypeResult,
		// 080925 LUJ, ���� ����
		TypeSupport,
	}
	type = TypeNone;

	while( ! file.IsEOF() )
	{
		file.GetLine( buffer, sizeof( buffer ) );

		const int length = _tcslen( buffer );

		if( ! length )
		{
			continue;
		}

		// �߰��� �ּ� ����
		for( int i = 0; i < length - 1; ++i )
		{
			const char a = buffer[ i ];
			const char b = buffer[ i + 1 ];

			if( '/' == a &&
				'/' == b )
			{
				buffer[ i ] = 0;
				break;
			}
		}

		const char*		separator	= "~\n\t(), ";
		const char*		token		= strtok( buffer, separator );

		if( ! token )
		{
			continue;
		}
		else if( ! stricmp( "#protection", token ) )
		{
			type = TypeProtection;
			continue;
		}
		else if( ! stricmp( "#result", token ) )
		{
			type = TypeResult;
			continue;
		}
		// 080925 LUJ, ���� ����
		else if( ! stricmp( "#support", token ) )
		{
			type = TypeSupport;
			continue;
		}

		switch( type )
		{
		case TypeProtection:
			{
				const DWORD itemIndex = atoi( token );

				mMixProtectionItemSet.insert( itemIndex );
				break;
			}
		case TypeResult:
			{
				const DWORD		itemIndex	= atoi( token );
				MixScript&		data		= mMixScriptMap[ itemIndex ];

				data.push_back( ItemMixResult() );

				ItemMixResult& result = data.back();

				result.mMaterial[ itemIndex ] = 1;

				{
					result.mItemIndex		= strtoul( strtok( 0, separator ), 0, 10 );
					result.mRequiredLevel	= WORD( atoi( strtok( 0, separator ) ) );
					result.mMoney			= strtoul( strtok( 0, separator ), 0, 10 );

					const float rate		= float( atof( strtok( 0, separator ) ) / 10000 );
					result.mSuccessRate		= min( 1.0f, rate );
				}

				while( true )
				{
					const char* token1 = strtok( 0, separator );
					const char* token2 = strtok( 0, separator );

					if( ! token1 || ! token2 )
					{
						break;
					}

					const DWORD index		= strtoul( token1, 0, 10 );
					const DWORD	quantity	= strtoul( token2, 0, 10 );

					if( 0 < quantity )
					{
						result.mMaterial[ index ] += quantity;
					}
				}

				break;
			}
			// 080925 LUJ, ���� ����
		case TypeSupport:
			{
				MixSupportScript& script = mMixSupportScriptMap[ atoi( token ) ];

				while( true )
				{
					const char* key		= strtok( 0, separator );
					const char* rate	= strtok( 0, separator );

					if(	! key ||
						! rate )
					{
						break;
					}

					// 080925 LUJ, Ȯ���� �����Ͽ� ������ �����ϴ� ���� Ŭ����
					class
					{
					public:
						void operator()( MixSetting::Ability& abilityMap, const char* rate, MixSetting::eAbility ability ) const
						{
							const float lastRate = ( abilityMap.empty() ? 0 : ( *( abilityMap.rbegin() ) ).first );

							abilityMap[ lastRate + float( atof( rate ) / 100.0f ) ] = ability;
						}
					}
					Accumulate;

					if( ! stricmp( key, "success" ) )
					{
						script.mBonusSucessRate	= float( atof( rate ) / 100.0f );
					}
					else if( ! stricmp( key, "strength" ) )
					{
						Accumulate( script.mAbiltyMap, rate, MixSetting::eAbilityStrength );
					}
					else if( ! stricmp( key, "dexterity" ) )
					{
						Accumulate( script.mAbiltyMap, rate, MixSetting::eAbilityDexterity );
					}
					else if( ! stricmp( key, "vitality" ) )
					{
						Accumulate( script.mAbiltyMap, rate, MixSetting::eAbilityVitality );
					}
					else if( ! stricmp( key, "intelligence" ) )
					{
						Accumulate( script.mAbiltyMap, rate, MixSetting::eAbilityIntelligence );
					}
					else if( ! stricmp( key, "wisdom" ) )
					{
						Accumulate( script.mAbiltyMap, rate, MixSetting::eAbilityWisdom );
					}
				}
			}
		}
	}
}


const MixScript* CItemManager::GetMixScript( DWORD itemIndex ) const
{
	MixScriptMap::const_iterator it = mMixScriptMap.find( itemIndex );

	return mMixScriptMap.end() == it ? 0 : &( it->second );
};


CItemManager* CItemManager::GetInstance()
{
	static CItemManager instance;

	return &instance;
}


const DissolveScript* CItemManager::GetDissolveScript( DWORD itemIndex ) const
{
	DissolveScriptMap::const_iterator it = mDissolveScriptMap.find( itemIndex );

	return mDissolveScriptMap.end() == it ? 0 : &( it->second );
}


void CItemManager::LoadDissolveScript()
{
	CMHFile file;

	if( ! file.Init( "System/Resource/itemDissolve.bin", "rb" ) )
	{
		ASSERTMSG( 0, "���ؽ�ũ��Ʈ(itemDissolve.bin) �� �����ϴ�" );
		return;
	}

	char buffer[ 1024 ];

	while( ! file.IsEOF() )
	{
		file.GetLine( buffer, sizeof( buffer ) );

		const char* separator	= "\t\n ,";
		const char* token		= strtok( buffer, separator );
		const DWORD itemIndex	= strtoul( token, 0, 10 );

		DissolveScript& script = mDissolveScriptMap[ itemIndex ];

		script.mLevel			= strtoul( strtok( 0, separator ), 0, 10 ) ;

		const DWORD staticResultSize = 5;

		for( DWORD i = 0; i < staticResultSize; ++i )
		{
			const char* token1		= strtok( 0, separator );
			const char* token2		= strtok( 0, separator );

			ASSERT( token1 && token2 );

			const DWORD index		= strtoul( token1, 0, 10 );
			const DWORD quantity	= strtoul( token2, 0, 10 );

			if( index && quantity )
			{
				script.mStaticResult.push_back( DissolveScript::Result() );

				DissolveScript::Result& result = script.mStaticResult.back();

				result.mItemIndex	= index;
				result.mQuantity	= quantity;
			}
		}

		// Ȯ���� ���� �������� �����ϴ� ���� ����� ó���ϱ� ���� ��ٷӴ�.
		// ������ ���� ������ ���̴�. ���� �Ľ� ����� �����Ѵ�. �� ���� �������� 
		// ������ ���鼭 �ʿ� ������Ų ������ �����Ѵ�(���� 10000�� ���� �ʵ��� ����).
		// ���� ��� 10%, 30%, 60% Ȯ���� �ִٸ�, �ʿ� 1000, 4000, 10000���� �����Ѵ�.
		// �׸��� ����� ���� ���� �������� ���� �� ���� lower_bound()�� �ش�Ǵ� ���� ã�Ƴ���.
		{
			const DWORD maxRate = 10000;

			for( DWORD rate = 0; rate < maxRate; )
			{
				const char* token1	= strtok( 0, separator );
				const char* token2	= strtok( 0, separator );
				const char* token3	= strtok( 0, separator );

				if( ! token1 || ! token2 || ! token3 )
				{
					break;
				}

				const DWORD itemIndex	= strtoul( token1, 0, 10 );
				const DWORD quantity	= strtoul( token2, 0, 10 );
				const DWORD percent		= strtoul( token3, 0, 10 );

				if( ! itemIndex || ! quantity || ! percent )
				{
					break;
				}

				rate = min( maxRate, rate + percent );

				DissolveScript::Result& result = script.mDynamicResult[ rate ];

				result.mItemIndex	= itemIndex;
				result.mQuantity	= quantity;
			}

			if( !	script.mDynamicResult.empty()	&&
					script.mDynamicResult.end() == script.mDynamicResult.find( maxRate ) )
			{
				ASSERT( 0 && "���� ��ũ��Ʈ�� Ȯ�� ���� ���� 10000�� ���� �ʽ��ϴ�. �ùٸ� ����� ������ �� �����ϴ�" );
			}
		}
	}
}

eLogitemmoney CItemManager::Dissolve(CPlayer& player, const MSG_ITEM_DISSOLVE_SYN& receivedMessage, const CBackupSlot& backupSlot)
{
	CInventorySlot*	const inventorySlot = (CInventorySlot*)player.GetSlot(
		eItemTable_Inventory);

	if(0 == inventorySlot)
	{
		return eLog_ItemDissolvePlayserHasNoInventory;
	}

	// ��� �Һ�
	for(size_t i = 0; i < receivedMessage.mSourceSize; ++i)
	{
		const MSG_ITEM_DISSOLVE_SYN::Item& sourceItem = receivedMessage.mItem[i];
		const ITEMBASE* const itemBase = inventorySlot->GetItemInfoAbs(
			sourceItem.mPosition);

		if(0 == itemBase)
		{
			return eLog_ItemDissolveSourceItemNotExist;
		}
		else if(IsDupItem(itemBase->wIconIdx) &&
			itemBase->Durability != sourceItem.mQuantity)
		{
			return eLog_ItemDissolveItemDataNotMatched;
		}
		else if(itemBase->wIconIdx != sourceItem.mIndex)
		{
			return eLog_ItemDissolveItemDataNotMatched;
		}
		else if(itemBase->dwDBIdx != sourceItem.mDbIndex)
		{
			return eLog_ItemDissolveItemDataNotMatched;
		}
		else if(EI_TRUE != inventorySlot->DeleteItemAbs(
			&player,
			sourceItem.mPosition,
			0))
		{
			return eLog_ItemDissolveServerError;
		}
	}

	typedef DWORD ItemIndex;
	typedef int ItemQuantity;
	typedef std::map< ItemIndex, ItemQuantity > ResultContainer;
	ResultContainer resultContainer;

	// ����� �����Ѵ�
	for(size_t i = 0; i < receivedMessage.mSourceSize; ++i)
	{
		const MSG_ITEM_DISSOLVE_SYN::Item& sourceItem = receivedMessage.mItem[i];
		const ITEM_INFO* const itemInfo = GetItemInfo(
			sourceItem.mIndex);

		if(0 == itemInfo)
		{
			continue;
		}

		const ItemQuantity itemQuantity = (itemInfo->Stack ? sourceItem.mQuantity : 1);
		const DissolveScript* const script = GetDissolveScript(
			sourceItem.mIndex);

		if(0 == script)
		{
			return eLog_ItemDissolveScriptInvalid;
		}

		for(DissolveScript::StaticResult::const_iterator iterator = script->mStaticResult.begin();
			script->mStaticResult.end() != iterator;
			++iterator)
		{
			const DissolveScript::Result& result = *iterator;

			resultContainer[result.mItemIndex] += result.mQuantity * itemQuantity;
		}

		if(false == script->mDynamicResult.empty())
		{
			const DWORD maxRate = 10000;
			const DissolveScript::DynamicResult::const_iterator iterator = script->mDynamicResult.lower_bound(
				rand() % (maxRate + 1));
			
			if(script->mDynamicResult.end() == iterator)
			{
				continue;
			}

			const DissolveScript::Result& result = iterator->second;
			resultContainer[result.mItemIndex] += result.mQuantity * itemQuantity;
		}
	}

	// ����� �����Ѵ�. Ŭ���̾�Ʈ���� ����� ����� ����Ѵ�
	for(size_t i = receivedMessage.mSourceSize; i < receivedMessage.mSize; ++i)
	{
		const MSG_ITEM_DISSOLVE_SYN::Item& resultItem = receivedMessage.mItem[i];
		const ResultContainer::const_iterator iterator = resultContainer.find(
			resultItem.mIndex);

		if(resultContainer.end() == iterator)
		{
			continue;
		}

		const ItemIndex itemIndex = iterator->first;
		const ItemQuantity itemQuantity = iterator->second;
		
		if(0 >= itemQuantity)
		{
			continue;
		}

		const ITEM_INFO* const itemInfo = GetItemInfo(
			itemIndex);

		if(0 == itemInfo)
		{
			continue;
		}

		ITEMBASE* const inventoryItemBase = inventorySlot->GetItemInfoAbs(
			resultItem.mPosition);

		if(0 == inventoryItemBase)
		{
			continue;
		}
		else if(0 == inventoryItemBase->wIconIdx)
		{
			ITEMBASE itemBase;
			ZeroMemory(
				&itemBase,
				sizeof(itemBase));
			itemBase.Position = resultItem.mPosition;
			itemBase.wIconIdx = itemIndex;
			itemBase.Durability = DURTYPE(min(
				itemInfo->Stack ? itemInfo->Stack : 1,
				itemQuantity));
			itemBase.nSealed = itemInfo->wSeal;
			itemBase.nRemainSecond = itemInfo->dwUseTime;

			if(const ITEMBASE* const backupedItemBase = backupSlot.GetItem(resultItem.mPosition))
			{
				itemBase.dwDBIdx = backupedItemBase->dwDBIdx;

				if(EI_TRUE != inventorySlot->InsertItemAbs(
					&player,
					itemBase.Position,
					&itemBase))
				{
					return eLog_ItemDissolveInsufficentSpace;
				}
			}
			else
			{
				itemBase.dwDBIdx = UINT_MAX;

				if(EI_TRUE != inventorySlot->InsertItemAbs(
					&player,
					itemBase.Position,
					&itemBase,
					SS_PREINSERT))
				{
					return eLog_ItemDissolveInsufficentSpace;
				}
			}

			resultContainer[resultItem.mIndex] -= itemBase.Durability;
			continue;
		}
		else if(inventoryItemBase->wIconIdx == itemIndex)
		{
			if(0 == itemInfo->Stack)
			{
				continue;
			}
			else if(itemInfo->wSeal != inventoryItemBase->nSealed)
			{
				continue;
			}

			const ItemQuantity maxItemQuantity = min(
				ItemQuantity(itemInfo->Stack - inventoryItemBase->Durability),
				itemQuantity);

            if(EI_TRUE != inventorySlot->UpdateItemAbs(
				&player,
				inventoryItemBase->Position,
				inventoryItemBase->dwDBIdx,
				inventoryItemBase->wIconIdx,
				inventoryItemBase->Position,
				inventoryItemBase->QuickPosition,
				inventoryItemBase->Durability + maxItemQuantity,
				UB_DURA,
				SS_CHKDBIDX))
			{
				continue;
			}

			resultContainer[itemIndex] -= maxItemQuantity;
			continue;
		}
	}

	for(ResultContainer::const_iterator iterator = resultContainer.begin();
		resultContainer.end() != iterator;
		++iterator)
	{
		const ItemQuantity quantity = iterator->second;

		if(0 < quantity)
		{
			return eLog_ItemDissolveInsufficentSpace;
		}
	}

	MSG_ITEM_DISSOLVE_ACK message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category = MP_ITEM;
	message.Protocol = MP_ITEM_DISSOLVE_ACK;
	ICONBASE emptyIconBase = {0};
	UpdateResult(
		player,
		*inventorySlot,
		backupSlot,
		message.mUpdateResult,
		MP_ITEM_DISSOLVE_GETITEM,
		eLog_ItemDestroyDissolution,
		eLog_ItemObtainDissolve,
		emptyIconBase);
	player.SendMsg(
		&message,
		message.GetSize());

	// ȿ���� ǥ���� �� �ֵ��� ����
	{
		MSG_DWORD message;
		ZeroMemory(
			&message,
			sizeof(message));
		message.Category = MP_ITEM;
		message.Protocol = MP_ITEM_WORKING_SUCCESS;
		message.dwData = player.GetID();
		PACKEDDATA_OBJ->QuickSend(
			&player,
			&message,
			sizeof(message));
	}

	return eLog_ItemDissolveSuccess;
}

BOOL CItemManager::IsRare( const ITEM_INFO& info ) const
{
	switch( info.kind )
	{
	case ITEM_INFO::eKindRare:
	case ITEM_INFO::eKindUnique:
	case ITEM_INFO::eKindLegend:
		{
			return TRUE;
		}
	}

	return FALSE;
}

void CItemManager::LoadCoolTimeScript()
{
	CMHFile file;
	file.Init( "system/resource/itemCoolTime.bin", "rb" );

	int	openCount = 0;
	DWORD groupIndex = 1;

	std::list< std::string >	text;

	for(TCHAR line[ MAX_PATH ];
		! file.IsEOF();
		ZeroMemory( line, sizeof( line ) ) )
	{
		file.GetLine( line, sizeof( line ) / sizeof( TCHAR ) );

		const int length = _tcslen( line );

		if( ! length )
		{
			continue;
		}

		// �߰��� �ּ� ����
		for( int i = 0; i < length - 1; ++i )
		{
			const char a = line[ i ];
			const char b = line[ i + 1 ];

			if( '/' == a &&
				'/' == b )
			{
				line[ i ] = 0;
				break;
			}
		}

		text.push_back( line );

		const TCHAR* separator	= _T( " ~\n\t=,+\"()%" );
		const TCHAR* token		= _tcstok( line, separator );
		const TCHAR* markBegin	= _T( "{" );
		const TCHAR* markEnd	= _T( "}" );

		if( 0 == token ||
			0 == _tcsicmp( "group", token ) )
		{
			text.pop_back();
		}
		else if( 0 == _tcsnicmp( markBegin, token, strlen( markBegin ) ) )
		{
			text.pop_back();

			++openCount;
		}
		else if( 0 == _tcsnicmp( markEnd, token, strlen( markEnd ) ) )
		{
			text.pop_back();

			if( ! --openCount )
			{
				for(std::list< std::string >::const_iterator it = text.begin();
					text.end() != it;
					++it )
				{
					char buffer[ MAX_PATH ] = {0};

					_tcscpy( buffer, it->c_str() );

					const DWORD itemIndex	= strtoul( _tcstok( buffer, separator ), 0, 10 );
					const DWORD coolTime	= atoi( _tcstok( 0, separator ) ) * 1000;

					CoolTimeScript::Unit& unit = mCoolTimeScript.mItemData[ itemIndex ];

					unit.mIndex	= groupIndex;
					unit.mTime	= coolTime;

					{
						CoolTimeScript::ItemIndex& indexSet =  mCoolTimeScript.mGroupData[ groupIndex ];

						indexSet.insert( itemIndex );
					}
				}

				text.clear();
				++groupIndex;
			}
			else
			{
				ASSERT( 0 );
				return;
			}
		}
	}
}

DWORD CItemManager::GetCoolTimeGroupIndex( DWORD itemIndex ) const
{
	CoolTimeScript::ItemData::const_iterator it = mCoolTimeScript.mItemData.find( itemIndex );

	if( mCoolTimeScript.mItemData.end() == it )
	{
		return 0;
	}

	const CoolTimeScript::Unit& unit = it->second;

	return unit.mIndex;
}


void CItemManager::RemoveCoolTime( DWORD playerIndex )
{
	mUsingCoolTimeGroup.erase( playerIndex );
}

// 071201 KTH - �� �̵� �ֹ��� ���� ó�� �ϴºκ�.
BOOL CItemManager::Move_ChangeMap(CPlayer* pPlayer, MAPTYPE mapType) 
{
	if(pPlayer == NULL)
		return FALSE;
	// 090701 LUJ, �� �̵� �� �÷��̾� ������ DB�� ���µ�, �ʱ�ȭ�� ������ �ʾ��� ��� �����Ⱚ��
	//		����� �� �ִ�. �ʱ�ȭ�� ���� ��ü�� ó���Ѵ�
	else if( FALSE == pPlayer->GetInited() )
	{
		return FALSE;
	}

	VECTOR3 vector3 = *GAMERESRCMNGR->GetRevivePoint( mapType );
	MAPCHANGE_INFO* ChangeInfo = GAMERESRCMNGR->GetMapChangeInfoForDestMapNum( mapType );

	if( CHANNELSYSTEM->GetChannelID( pPlayer->GetChannelID() - 1 ) == 0 && !CHANNELSYSTEM->IsQuestMap() || !ChangeInfo )
	{
		MSG_DWORD msg;
		msg.Category	= MP_ITEM;
		msg.Protocol	= MP_ITEM_MAPMOVE_SCROLL_NACK;
		msg.dwData		= pPlayer->GetGridID() - 1;
		pPlayer->SendMsg( &msg, sizeof( msg ) ); 
		return FALSE;
	}

	SaveMapChangePointUpdate(
		pPlayer->GetID(),
		ChangeInfo->Kind);
	pPlayer->UpdateLogoutToDB(
		FALSE);

	VECTOR3 pos;

	int temp;
	temp = rand() % 500 - 250;
	pos.x = vector3.x + temp;
	temp = rand() % 500 - 250;
	pos.z = vector3.z + temp;
	pos.y = 0;

	pPlayer->SetMapMoveInfo( mapType, (DWORD)pos.x, (DWORD)pos.z);

	if(g_pServerSystem->GetMapNum() == HOUSINGMAPNUM ||
		g_csDateManager.IsChallengeZone(g_pServerSystem->GetMapNum()))
	{
		CharacterLogoutPointUpdate(
			pPlayer->GetID(),
			mapType,
			pos.x,
			pos.z);
	}

	// 081218 LUJ, ���� ���� �÷��̾� ������ �����س��´�
	const DWORD			playerIndex		= pPlayer->GetID();
	const DWORD			userIndex		= pPlayer->GetUserID();
	const MONEYTYPE		inventoryMoney	= pPlayer->GetMoney( eItemTable_Inventory );
	const MONEYTYPE		storageMoney	= pPlayer->GetMoney( eItemTable_Storage );

	g_pServerSystem->RemovePlayer( playerIndex );

	// 081218 LUJ, ������Ʈ���� �� �̵��� ���� ����� ��� ������Ʈ���� ���� ������ ���Եȴ�.
	//			�̸� ���� ���� ���ν��� ó�� �Ϸ� �� �����Ѵ�
	UpdatePlayerOnMapOut(
		playerIndex,
		userIndex,
		inventoryMoney,
		storageMoney,
		mapType,
		// 081219 LUJ, WORD���� ����ȣ���̹Ƿ� ĳ�����Ѵ�
		WORD( -1 ) );
	return TRUE;
}


BOOL CItemManager::IsTwoHand( DWORD itemIndex )
{
	const ITEM_INFO* info = GetItemInfo( itemIndex );

	return info	&& (
		info->WeaponAnimation == eWeaponAnimationType_TwoHand	||
		info->WeaponAnimation == eWeaponAnimationType_Staff		||
		info->WeaponAnimation == eWeaponAnimationType_Bow		||
		info->WeaponAnimation == eWeaponAnimationType_Gun );
}

BOOL CItemManager::UseItem_ResetStats(CPlayer* pPlayer)
{
	// ������ ���� ó���� �� �Լ� ���� ��ƾ���� ó�� �ϱ� ������, �� �Լ������� ó�� ���� �ʴ´�.

	if( !pPlayer ) return FALSE;

	// 080124 KTH -- PKMode ���϶����� ��� �Ұ�
	if( pPlayer->IsPKMode() )
		return FALSE;

	// 071206 KTH -- ���� �ʱ�ȭ �߿� ��ȯ�� �ȵǵ��� Lock�� �ɾ��ش�.
	pPlayer->SetResetStat(true);

	// ����, ���� �� �� ������ �⺻ ���� �迭�� �����Ѵ�.
	int StrArray[2][3] = { {16, 12,  8}, { 13, 11,  7} } ;					// �� ��ġ.
	int DexArray[2][3] = { {10, 14,  7}, { 12, 15,  8} } ;					// ��ø ��ġ.
	int VitArray[2][3] = { {14, 12, 11}, { 13, 11,  9} } ;					// �ǰ� ��ġ.
	int IntArray[2][3] = { { 6,  7, 13}, {  7,  8, 14} } ;					// ���� ��ġ.
	int WisArray[2][3] = { { 7,  8, 14}, {  8,  8, 15} } ;					// ���� ��ġ.

	int nRacial = pPlayer->GetRacial() ;									// ���� ��ġ�� �޴´�.
	int nJopNum = pPlayer->GetJop(0) ;										// ���� ��ȣ�� �޴´�.

	if( nJopNum != 0 )
	{
		--nJopNum ;
	}


	int Str;
	int Dex;
	int Vit;
	int Int;
	int Wis;
	// 091102 ONS ���� ����ó��.
	if( RaceType_Devil == nRacial )
	{
		Str = 60;
		Dex = 40;
		Vit = 45;
		Int = 60;
		Wis = 60;
	}
	else
	{
		Str = StrArray[nRacial][nJopNum] * 5 ;								// �⺻ �� ��ġ�� �޴´�.
		Dex = DexArray[nRacial][nJopNum] * 5 ;								// �⺻ ��ø ��ġ�� �޴´�.
		Vit = VitArray[nRacial][nJopNum] * 5 ;								// �⺻ �ǰ� ��ġ�� �޴´�.
		Int = IntArray[nRacial][nJopNum] * 5 ;								// �⺻ ���� ��ġ�� �޴´�.
		Wis = WisArray[nRacial][nJopNum] * 5 ;								// �⺻ ���� ��ġ�� �޴´�.
	}
	

	int nDefaultPoint = 0 ;													// �⺻ ���� ����Ʈ�� ���� ������ �����ϰ� 0����.
	nDefaultPoint = (Str + Dex + Vit + Int + Wis) ;							// �⺻ ���� �� ����Ʈ�� ���Ѵ�.

	LEVELTYPE maxLevel = 0 ;
	maxLevel = pPlayer->GetMaxLevel() ;

	int nNewPoint = 0 ;

	if( maxLevel > 1 )
	{
		nNewPoint = ( maxLevel - 1 ) * 5;						// �ʱ�ȭ ������ �ܿ� ����Ʈ�� ���Ѵ�.
	}
	else
	{
		nNewPoint = 0 ;
	}

	HERO_TOTALINFO* const pTotalInfo = pPlayer->GetHeroTotalInfo();
	pTotalInfo->Str = Str;
	pTotalInfo->Dex = Dex;
	pTotalInfo->Vit = Vit;
	pTotalInfo->Int = Int;
	pTotalInfo->Wis = Wis;
	pTotalInfo->LevelUpPoint = LEVELTYPE(nNewPoint);

	DB_UpdateToDefaultStats( pPlayer->GetID(), Str, Dex, Vit, Int, Wis, nNewPoint ) ;

	// 071206 KTH -- ���� �ʱ�ȭ�� ���� �Ŀ� ������ ������ �� �ֵ��� Lock�� Ǯ���ش�.
	pPlayer->SetResetStat(false);

	return TRUE;
}

BOOL CItemManager::UseItem_ResetSkill(CPlayer* pPlayer)
{
	// ������ ���� ó���� �� �Լ� ���� ��ƾ���� ó�� �ϱ� ������, �� �Լ������� ó�� ���� �ʴ´�.

	if( !pPlayer )
	{
		return FALSE;
	}
	
	// 080124 KTH -- PKMode ���϶����� ��� �Ұ�
	if( pPlayer->IsPKMode() )
		return FALSE;

	// 071206 KTH -- ��ų�� ������Ʈ ���� �ʵ��� ��� �ش�.
	pPlayer->SetResetSkill(true);

	cSkillTree& pSkillTree = pPlayer->GetSkillTree();
	DWORD dwTotalSkillMoney = 0;

	pSkillTree.SetPositionHead();
	SKILL_BASE* pSkill = NULL;
	while( (pSkill = pSkillTree.GetData() ) != NULL)
	{
		dwTotalSkillMoney += GAMERESRCMNGR->GetSkillMoney(
			pSkill->wSkillIdx + pSkill->mLearnedLevel - 1);
	}

	pPlayer->SetRetrunSkillMoney( dwTotalSkillMoney );
	//------------------------------------------

	DB_ResetToDefaultSkill( pPlayer->GetID() ) ;

	return TRUE;
}



const ITEM_OPTION& CItemManager::GetOption( const ITEMBASE& item ) const
{
	OptionMap::const_iterator it = mOptionMap.find( item.dwDBIdx );

	if( mOptionMap.end() == it )
	{
		const static ITEM_OPTION emptyOption = { 0 };

		return emptyOption;
	}

	return *( it->second );
}


void CItemManager::AddOption( const ITEM_OPTION& option )
{
	OptionMap::iterator it = mOptionMap.find( option.mItemDbIndex );

	if( mOptionMap.end() != it )
	{
		*( it->second ) = option;
		return;
	}

	ITEM_OPTION* memory = mOptionPool.Alloc();
	*memory = option;

	mOptionMap.insert( std::make_pair( option.mItemDbIndex, memory ) );
}


void CItemManager::RemoveOption( const ITEMBASE& item )
{
	RemoveOption( item.dwDBIdx );
}


void CItemManager::RemoveOption( DWORD itemDbIndex )
{
	OptionMap::iterator it = mOptionMap.find( itemDbIndex );

	if( mOptionMap.end() == it )
	{
		return;
	}

	mOptionPool.Free( it->second );
	mOptionMap.erase( it );
}


void CItemManager::AddDropOption( const ITEMBASE& item, ITEM_OPTION& option )
{
	const ITEM_INFO* info = GetItemInfo( item.wIconIdx );

	if( !	info ||
			eEquipType_None == info->EquipType )
	{
		return;
	}

	const DropOptionScript& script = g_CGameResourceManager.GetDropOptionScript(
		*info);

	// ��æƮ ����
	{
		DWORD level = 0;
		{
			DropOptionScript::EnchantLevelMap::const_iterator it = script.mEnchantLevelMap.lower_bound( mRandom.GetFloat() );

			if( script.mEnchantLevelMap.end() != it )
			{
				level = it->second;
			}
		}

		const EnchantScript* enchantScript = g_CGameResourceManager.GetEnchantScript(info->EquipSlot);

		if( level && enchantScript )
		{
			option.mEnchant.mIndex = UINT_MAX;
			option.mEnchant.mLevel = BYTE(level);

			g_CGameResourceManager.AddEnchantValue( option, enchantScript->mAbility, info->EnchantDeterm * level );
		}
	}

	// ��� �ɼ� ����
	{
		DropOptionScript::OptionSizeMap::const_iterator size_it = script.mOptionSizeMap.lower_bound( mRandom.GetFloat() );

		if( script.mOptionSizeMap.end() != size_it )
		{
			DWORD size = min( size_it->second, sizeof( option.mDrop.mValue ) / sizeof( *( option.mDrop.mValue ) ) );

			while( size-- )
			{
				DropOptionScript::LevelMap::const_iterator level_it = script.mLevelMap.lower_bound( info->LimitLevel );

				if( script.mLevelMap.end() == level_it )
				{
					continue;
				}

				const DropOptionScript::OptionMap& optionMap = level_it->second;

				DropOptionScript::OptionMap::const_iterator option_it = optionMap.lower_bound( mRandom.GetFloat() );

				if( optionMap.end() == option_it )
				{
					continue;
				}

				const DropOptionScript::Option& scriptOption	= option_it->second;
				const float						gap				= abs( scriptOption.mBeginValue - scriptOption.mEndValue );

				if( ITEM_OPTION::Drop::KeyNone != scriptOption.mKey &&
					0 < gap )
				{
					ITEM_OPTION::Drop::Value& value = option.mDrop.mValue[ size ];

					value.mValue	= Round( scriptOption.mBeginValue + gap * mRandom.GetFloat(), 3 );
					value.mKey		= scriptOption.mKey;
				}
			}
		}
	}
}

void CItemManager::ApplyOption( const ITEMBASE& sourceItem, const ITEMBASE& targetItem, ITEM_OPTION& option )
{
	const ApplyOptionScript& script = g_CGameResourceManager.GetApplyOptionScript(
		sourceItem.wIconIdx);
	const ITEM_INFO* info = GetItemInfo( targetItem.wIconIdx );

	if( !	info ||
			eEquipType_None == info->EquipType )
	{
		return;
	}

	// ���� ������ ���� �����ΰ�
	if( script.mLevel.mMin > info->LimitLevel ||
		script.mLevel.mMax < info->LimitLevel )
	{
		return;
	}

	// ���� ������ Ÿ������ üũ�Ѵ�
	{
		const LONGLONG key = g_CGameResourceManager.GetItemKey(
			info->EquipSlot,
			info->WeaponType,
			info->ArmorType,
			info->WeaponAnimation);

		if( script.mTargetTypeSet.end() == script.mTargetTypeSet.find( key ) )
		{
			return;
		}
	}
	
	int size = 1;

	// �ɼ� ������ ���´�. �ּ� 1��
	{
		ApplyOptionScript::OptionSizeMap::const_iterator it = script.mOptionSizeMap.lower_bound( mRandom.GetFloat() );

		if( script.mOptionSizeMap.end() != it )
		{
			size = max( 1, it->second );
		}
	}
	
	// �ɼ��� �����ϰ� �ο��Ѵ�
	while( size-- )
	{
		const ApplyOptionScript::Option* scriptOption = 0;

		// ��� �ɼ� ����
		do 
		{
			ApplyOptionScript::OptionTypeMap::const_iterator it = script.mOptionTypeMap.lower_bound( mRandom.GetFloat() );

            if( script.mOptionTypeMap.end() != it )
			{
				scriptOption = &( it->second );
			}
		}
		while( !	scriptOption ||
					scriptOption->mKey == ITEM_OPTION::Drop::KeyNone );

		ITEM_OPTION::Drop::Value& value = option.mDrop.mValue[ size ];

		const float gap = abs( scriptOption->mBeginValue - scriptOption->mEndValue );

		// 080407 LUJ, Ư�� �Ҽ��� �Ʒ��� �����Ͽ� �Ǽ� ������ ���Ѵ�
		value.mValue	= Round( scriptOption->mBeginValue + gap * mRandom.GetFloat(), 3 );
		value.mKey		= scriptOption->mKey;
	}	
}


// 080228 LUJ, ���� ��ȣ ������ ����
BOOL CItemManager::IsMixProtectionItem( const ITEMBASE& item ) const
{
	return mMixProtectionItemSet.end() != mMixProtectionItemSet.find( item.wIconIdx );
}

void CItemManager::LoadBodyChangeScript()
{
	CMHFile file;

	if( ! file.Init( "system/resource/ItemBodyChange.bin", "rb" ) )
	{
		return;
	}

	char line[ MAX_PATH * 5 ] = { 0 };

	enum ScriptState
	{
		StateNone,
		StateItem,
		StateRace,
		StatePart,
		StateGender,
	}
	scriptState = StateNone;

	int						openCount	= 0;
	BodyChangeScriptList*	scriptList	= 0;
	BodyChangeScript		script		= { 0 };

	typedef std::list< std::string >	TextList;
	TextList							textList;

	while( ! file.IsEOF() )
	{
		file.GetLine( line, sizeof( line ) );

		const int length = strlen( line );

		// �߰��� �ּ� ����
		for( int i = 0; i < length - 1; ++i )
		{
			const char a = line[ i ];
			const char b = line[ i + 1 ];

			if( '/' == a &&
				'/' == b )
			{
				line[ i ] = 0;
				break;
			}
		}

		textList.push_back( line );

		const char* separator	= " \n\t=,+\"~()";
		const char* token		= strtok( line, separator );
		const char* markBegin	= "{";
		const char* markEnd		= "}";

		if( ! token )
		{
			textList.pop_back();
		}
		// ���ο� Ŭ������ ������ ��带 �Ľ��Ѵ�
		else if( 0 == strnicmp( markBegin, token, strlen( markBegin ) ) )
		{
			textList.pop_back();

			switch( scriptState )
			{
			case StateNone:
				{
					++openCount;

					scriptState = StateItem;

					for(
						TextList::const_iterator it = textList.begin();
						textList.end() != it;
					++it )
					{
						SafeStrCpy( line, it->c_str(), sizeof( line ) );

						token = strtok( line, separator );

						if( ! stricmp( token, "item" ) )
						{
							const char* itemIndex = strtok( 0, separator );

							scriptList = &( mBodyChangeScriptListMap[ atoi( itemIndex ) ] );
						}
					}

					break;
				}
			case StateItem:
				{
					for(
						TextList::const_iterator it = textList.begin();
						textList.end() != it;
					++it )
					{
						SafeStrCpy( line, it->c_str(), sizeof( line ) );

						token = strtok( line, separator );

						if( ! stricmp( token, "elf" ) )
						{
							scriptState		= StateRace;
							script.mRace	= RaceType_Elf;
						}
						else if( ! stricmp( token, "human" ) )
						{
							scriptState		= StateRace;
							script.mRace	= RaceType_Human;
						}
						// 090520 ONS �ű����� ���� ó�� �߰�
						else if( ! stricmp( token, "devil" ) )
						{
							scriptState		= StateRace;
							script.mRace	= RaceType_Devil;
						}
					}

					break;
				}
			case StateRace:
				{
					for(
						TextList::const_iterator it = textList.begin();
						textList.end() != it;
					++it )
					{
						SafeStrCpy( line, it->c_str(), sizeof( line ) );

						token = strtok( line, separator );

						if( ! stricmp( token, "hair" ) )
						{
							scriptState		= StatePart;							
							script.mPart	= ePartType_Hair;
						}
						else if( ! stricmp( token, "face" ) )
						{
							scriptState		= StatePart;							
							script.mPart	= ePartType_Face;
						}
					}

					break;
				}
			case StatePart:
				{
					for(
						TextList::const_iterator it = textList.begin();
						textList.end() != it;
					++it )
					{
						SafeStrCpy( line, it->c_str(), sizeof( line ) );

						token = strtok( line, separator );

						if( ! stricmp( token, "female" ) )
						{
							scriptState		= StateGender;							
							script.mGender	= GENDER_FEMALE;
						}
						else if( ! stricmp( token, "male" ) )
						{
							scriptState		= StateGender;							
							script.mGender	= GENDER_MALE;
						}
					}

					break;
				}				
			}

			// 080511 LUJ, ��ũ��Ʈ�� �ùٷ� �Ľ̵ǵ��� ����
			textList.clear();
		}
		else if( 0 == strnicmp( markEnd, token, strlen( markEnd ) ) )
		{
			textList.pop_back();

			--openCount;

			switch( scriptState )
			{
			case StateItem:
				{
					ZeroMemory( &script, sizeof( script ) );
					scriptList	= 0;

					scriptState	= StateNone;					
					break;
				}
			case StateRace:
				{
					scriptState	= StateItem;
					break;
				}
			case StatePart:
				{
					scriptState	= StateRace;
					break;
				}
			case StateGender:
				{
					scriptState	= StatePart;

					for(
						TextList::const_iterator it = textList.begin();
						textList.end() != it;
					++it )
					{
						SafeStrCpy( line, it->c_str(), sizeof( line ) );

						for(
							const char* key = strtok( line, separator );
							key;
						key = strtok( 0, separator ) )
						{
							const char* value = strtok( 0, separator );

							if( ! value )
							{
								continue;
							}

							if( ! stricmp( "value", key ) )
							{
								script.mValue	= atoi( value );
							}
							else if( ! stricmp( "text", key ) )
							{
								script.mText	= atoi( value );
							}
						}

						if( scriptList )
						{
							scriptList->push_back( script );
						}						
					}

					break;
				}
			}

			textList.clear();
		}
	}

	if( openCount )
	{
		ASSERT( 0 );
	}
}

//081027 NYJ - ���ͼ�ȯ������
void CItemManager::LoadMonSummonItemInfo()
{
	CMHFile file;
	if(!file.Init("system/Resource/MonSummonItem.bin", "rb"))
		return;

	char string[256] = {0,};
	WORD wGroupIdx = 0;
	// 081211 LYW --- ItemManager : ggomgrak��û���� dwAccumRate�� ���������� 
	// WORD->DWORD�� �����ϰ� ��������wAccumRate->dwAccumRate�� ������.
    DWORD dwAccumRate = 0;
	bool bBrace = false;
	stMonSummonGroupInfo* pAddGroup = NULL;

	while(!file.IsEOF())
	{
		file.GetString(string);
		if(string[0] == '@')
		{
			file.GetLineX(string, 256);
			continue;
		}
		else if(0==strcmp(string, "#FORBIDDENMAP"))
		{
			const MAPTYPE mapnum = file.GetWord();
			m_vecMonSummonForbiddenMap.insert(mapnum);
		}
		else if(0==strcmp(string, "#GROUP"))
		{
			pAddGroup = new stMonSummonGroupInfo;
			if(pAddGroup)
				wGroupIdx = file.GetWord();
		}
		else if(0==strcmp(string, "{"))
		{
			dwAccumRate = 0;
			bBrace = true;
		}
		else if(0==strcmp(string, "}"))
		{
			if(bBrace)
			{
				pAddGroup->GroupIdx = wGroupIdx;
				m_vecMonSummonGroupInfo.push_back(pAddGroup);
				pAddGroup = NULL;
				bBrace = false;
			}
		}
		else if(0==strcmp(string, "#NOTFORBIDDENMAP"))
		{
			DWORD value = file.GetDword();
			if(pAddGroup && value)
				pAddGroup->bNotForbidden = true;
		}
		else if(0==strcmp(string, "#DIETIME"))
		{
			DWORD value = file.GetDword();
			if(pAddGroup && value)
				pAddGroup->dwDieTime = value;
		}
		else if(0==strcmp(string, "#BOSS"))
		{
			stMonSummonInfo* pAddData = NULL;
			pAddData = new stMonSummonInfo;
			pAddData->MonsterIdx = file.GetDword();
			pAddData->minRate = dwAccumRate;
			dwAccumRate += file.GetDword();
			pAddData->maxRate = dwAccumRate;

			pAddGroup->vecBossMosSummonGroup.push_back(pAddData);
		}
		else if(0==strcmp(string, "#NORMAL"))
		{
			stMonSummonInfo* pAddData = new stMonSummonInfo;
			pAddData->MonsterIdx = file.GetDword();
			pAddData->minRate = 0;
			pAddData->maxRate = 0;

			pAddGroup->vecMonSummonGroup.push_back(pAddData);
		}
	}
	file.Release();
}

//081027 NYJ - NPC��ȯ������
void CItemManager::LoadNpcSummonItemInfo()
{
	CMHFile file;
	if(!file.Init("system/Resource/NpcSummonItem.bin", "rb"))
		return;

	char string[256] = {0,};

	while(!file.IsEOF())
	{
		file.GetString(string);
		if(string[0] == '@')
		{
			file.GetLineX(string, 256);
			continue;
		}
		else if(0==strcmp(string, "#FORBIDDENMAP"))
		{
			const MAPTYPE mapnum = file.GetWord();
			m_vecNpcSummonForbiddenMap.insert(mapnum);
		}
		else if(0==strcmp(string, "#NPC"))
		{
			stNpcSummonInfo* pAddData = new stNpcSummonInfo;
			if(pAddData)
			{
				pAddData->SummonIdx = file.GetDword();
				pAddData->NpcIdx = file.GetDword();
				pAddData->LifeTime = file.GetDword();

				m_mapNpcSummonInfo.insert(std::make_pair(pAddData->SummonIdx, pAddData));
			}
		}
	}
	file.Release();
}

void CItemManager::Compose( const MSG_ITEM_COMPOSE_SYN& inputMessage )
{
	MSG_ITEM_COMPOSE_NACK failureMessage;
	ZeroMemory( &failureMessage, sizeof( failureMessage ) );
	failureMessage.Category = MP_ITEM;
	failureMessage.Protocol	= MP_ITEM_COMPOSE_NACK;
	
	CPlayer*				player	= 0;
	CItemSlot*				slot	= 0;
	const ComposeScript*	script	= 0;
	
	// 080916 LUJ, �޽����� ��ȿ���� �����Ѵ�
	{
		player = ( CPlayer* )g_pUserTable->FindUser( inputMessage.dwObjectID );
	
		if( !	player ||
				player->GetObjectKind() != eObjectKind_Player )
		{
			return ;
		}
	
		slot = player->GetSlot( eItemTable_Inventory );
	
		if( ! slot )
		{
			// 080916 LUJ, �̷� ���� ���� �� ������ üũ����
			return;
		}
	
		// 080916 LUJ, ��� ������ ��ȿ�� �˻�
		for( DWORD i = 0; i < inputMessage.mUsedItemSize; ++i )
		{
			const ICONBASE& sourceItem	= inputMessage.mUsedItem[ i ];
			const ITEMBASE* item		= slot->GetItemInfoAbs( sourceItem.Position );
	
			// 080916 LUJ, ��ᰡ �޸𸮿� ���ų� ��ġ���� ����
			if( !	item									||
					item->dwDBIdx	!= sourceItem.dwDBIdx	||
					item->wIconIdx	!= sourceItem.wIconIdx )
			{
				failureMessage.mType = MSG_ITEM_COMPOSE_NACK::TypeInvalidUsedItem;
				player->SendMsg( &failureMessage, sizeof( failureMessage ) );

				// 090207 LUJ, �α�
				LogItemMoney(
					player->GetID(),
					player->GetObjectName(),
					0,
					"*compose",
					eLog_ItemComposeFailureByMemoryMismatch,
					player->GetMoney(),
					0,
					0,
					0,
					0,
					0,
					0,
					0,
					player->GetPlayerExpPoint() );
				return;
			}
			// 080916 LUJ, ������ ��ũ��Ʈ�� ������ �������ְ� ���� ��Ḧ �����ؾ��Ѵ�.
			else if( ! script )
			{
				script = GAMERESRCMNGR->GetComposeScript( item->wIconIdx );
			}
			// 080916 LUJ, ������ ��ũ��Ʈ�� ���� ���, ��� ��� �������� ���� ��ũ��Ʈ�� �����ؾ��Ѵ�
			else if( script != GAMERESRCMNGR->GetComposeScript( item->wIconIdx ) )
			{
				failureMessage.mType = MSG_ITEM_COMPOSE_NACK::TypeNotMatchedScript;
				player->SendMsg( &failureMessage, sizeof( failureMessage ) );

				// 090207 LUJ, �α�
				LogItemMoney(
					player->GetID(),
					player->GetObjectName(),
					0,
					"*compose",
					eLog_ItemComposeFailureByWrongScript,
					player->GetMoney(),
					0,
					0,
					item->wIconIdx,
					item->dwDBIdx,
					item->Position,
					0,
					0,
					player->GetPlayerExpPoint() );
				return;
			}
		}
	
		// 080916 LUJ, ��ũ��Ʈ�� ���� ����
		if( ! script )
		{
			failureMessage.mType = MSG_ITEM_COMPOSE_NACK::TypeNotExistedScript;
			player->SendMsg( &failureMessage, sizeof( failureMessage ) );

			// 090207 LUJ, �α�
			LogItemMoney(
				player->GetID(),
				player->GetObjectName(),
				0,
				"*compose",
				eLog_ItemComposeFailureByWrongScript,
				player->GetMoney(),
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				player->GetPlayerExpPoint() );
			return;
		}
		// 080916 LUJ, ��� ������ ��ġ�ϴ��� �˻�
		else if( script->mSourceSize != inputMessage.mUsedItemSize )
		{
			failureMessage.mType = MSG_ITEM_COMPOSE_NACK::TypeWrongUsedItemSize;
			player->SendMsg( &failureMessage, sizeof( failureMessage ) );

			// 090207 LUJ, �α�
			LogItemMoney(
				player->GetID(),
				player->GetObjectName(),
				0,
				"*compose",
				eLog_ItemComposeFailureByWrongUseItemSize,
				player->GetMoney(),
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				player->GetPlayerExpPoint() );
			return;
		}
		// 080916 LUJ, �ռ� ����� ���� ������ ���, ������ �������� ��ȸ�ؾ��Ѵ�
		else if( ! inputMessage.mIsRandomResult )
		{
			const ComposeScript::ResultSet& resultSet = script->mResultSet;
	
			if( resultSet.end() == resultSet.find( inputMessage.mResultItem.wIconIdx ) )
			{
				failureMessage.mType = MSG_ITEM_COMPOSE_NACK::TypeInvalidResult;
				player->SendMsg( &failureMessage, sizeof( failureMessage ) );

				// 090207 LUJ, �α�
				LogItemMoney(
					player->GetID(),
					player->GetObjectName(),
					0,
					"*compose",
					eLog_ItemComposeFailureByInvalidResult,
					player->GetMoney(),
					0,
					0,
					inputMessage.mResultItem.wIconIdx,
					0,
					0,
					0,
					0,
					player->GetPlayerExpPoint() );
				return;
			}
		}
	
		// 080916 LUJ, Ű ������ �˻�
		{
			const ICONBASE&		keyItem		= inputMessage.mKeyItem;
			const ITEMBASE*		item		= slot->GetItemInfoAbs( keyItem.Position );
			const ITEM_INFO*	itemInfo	= GetItemInfo( keyItem.wIconIdx );
			
			if( !	item											||
				!	itemInfo										||
					( itemInfo->Stack && ( 0 == item->Durability ) )||
					item->wIconIdx	!= keyItem.wIconIdx				||
					item->dwDBIdx	!= keyItem.dwDBIdx				||
					script->mKeyItemIndex != keyItem.wIconIdx )
			{
				failureMessage.mType = MSG_ITEM_COMPOSE_NACK::TypeInvalidKeyItem;
				player->SendMsg( &failureMessage, sizeof( failureMessage ) );

				// 090207 LUJ, �α�
				LogItemMoney(
					player->GetID(),
					player->GetObjectName(),
					0,
					"*compose",
					eLog_ItemComposeFailureByInvalidKeyItem,
					player->GetMoney(),
					0,
					0,
					keyItem.wIconIdx,
					keyItem.dwDBIdx,
					keyItem.Position,
					0,
					0,
					player->GetPlayerExpPoint() );
				return;
			}
		}
	
		// 080916 LUJ, ���� ������ ��� �ɼ��ΰ�
		{
			const ComposeScript::OptionMap& optionMap = script->mOptionMap;
	
			if( optionMap.end() == optionMap.find( inputMessage.mOptionKey ) )
			{
				failureMessage.mType = MSG_ITEM_COMPOSE_NACK::TypeNotExistedOption;
				player->SendMsg( &failureMessage, sizeof( failureMessage ) );

				// 090207 LUJ, �α�
				LogItemMoney(
					player->GetID(),
					player->GetObjectName(),
					inputMessage.mOptionKey,
					"*compose",
					eLog_ItemComposeFailureByNoExistedOption,
					player->GetMoney(),
					0,
					0,
					0,
					0,
					0,
					0,
					0,
					player->GetPlayerExpPoint() );
				return;
			}
		}
	}
	
	// 080916 LUJ, �޸�/DB�� ������Ʈ�Ѵ�
	{
		DWORD resultItemIndex = inputMessage.mResultItem.wIconIdx;
	
		// 080916 LUJ, ��� ���� �ϳ��� ����� �ٲ۴�
		{
			// 080916 LUJ, ����� �������� ������ ��� ��ǻ�Ͱ� �������ش�
			if( inputMessage.mIsRandomResult )
			{
				ComposeScript::ResultMap::const_iterator result_it = script->mResultMap.lower_bound( mRandom.GetFloat() );

				// 090129 LUJ, ��ũ��Ʈ���� Ȯ�� ���� 100% �̻��� �ȵ� ���, �������ε� ������
				//			���� ���ϴ� ��찡 �߻��Ѵ�. �� ������ ���� �������� �������ش�.
				//			�ռ��� ���а� �����Ƿ� �̷��� ó���Ѵ�
				if( script->mResultMap.end() == result_it )
				{
					const float		resultSize	= float( max( 0, ( script->mResultMap.size() - 1 ) ) );
					const size_t	resultIndex = size_t( min( resultSize, Round( resultSize * mRandom.GetFloat(), 1 ) ) );

					result_it = script->mResultMap.begin();
					std::advance( result_it, resultIndex );
				}
	
				resultItemIndex = ( script->mResultMap.end() == result_it ? 0 : result_it->second.mItemIndex );
			}
	
			const ITEM_INFO* const resultItemInfo = GetItemInfo( resultItemIndex );

			// 080916 LUJ, ������ ������ ������ ����� �� ����
			// 090129 LUJ, ����� ������ Ŭ���̾�Ʈ�� �˸���
			if( ! resultItemInfo )
			{
				failureMessage.mType = MSG_ITEM_COMPOSE_NACK::TypeInvalidResult;
				player->SendMsg( &failureMessage, sizeof( failureMessage ) );

				// 090207 LUJ, �α�
				LogItemMoney(
					player->GetID(),
					player->GetObjectName(),
					0,
					"*compose",
					eLog_ItemComposeFailureByNoResultItemInfo,
					player->GetMoney(),
					0,
					0,
					resultItemIndex,
					0,
					0,
					0,
					0,
					player->GetPlayerExpPoint() );
				return;
			}
	
			const ICONBASE& sourceItem	= inputMessage.mUsedItem[ 0 ];
			const ITEMBASE* item		= slot->GetItemInfoAbs( sourceItem.Position );
	
			// 080916 LUJ, �̹� �˻� �κп��� �����ߴ�
			if( ! item )
			{
				return;
			}
			else if( EI_TRUE != slot->UpdateItemAbs(
				player,
				item->Position,
				item->dwDBIdx,
				resultItemIndex,
				item->Position,
				item->QuickPosition,
				item->Durability ) )
			{
				return;
			}

			// 090122 LUJ, ������ ���� �������� ����/���� ��Ų��
			{
				switch( resultItemInfo->wSeal )
				{
				case eITEM_TYPE_SEAL_NORMAL:
					{
						slot->ForcedUnseal( item->Position );
						break;
					}
				case eITEM_TYPE_SEAL:
					{
						slot->ForcedSeal( item->Position );
						break;
					}
				}

				g_DB.FreeMiddleQuery(
					0,
					player->GetID(),
					"EXEC dbo.MP_ITEM_SEAL %d, %d",
					item->dwDBIdx,
					resultItemInfo->wSeal );
			}

			ItemUpdateToDB(
				player->GetID(),
				item->dwDBIdx,
				resultItemIndex,
				0,
				item->Position,
				item->QuickPosition );
			// 090207 LUJ, �α�
			LogItemMoney(
				player->GetID(),
				player->GetObjectName(),
				0,
				"*compose",
				eLog_ItemComposeSuccess,
				player->GetMoney(),
				0,
				0,
				resultItemIndex,
				item->dwDBIdx,
				item->Position,
				0,
				0,
				player->GetPlayerExpPoint() );
		}
	
		// 080916 LUJ, �ռ� ����� ���� �ɼ��� �����ϰ� ������Ʈ�Ѵ�
		{
			const ComposeScript::OptionMap::const_iterator composeOption_it = script->mOptionMap.find( inputMessage.mOptionKey );
	
			if( script->mOptionMap.end() != composeOption_it )
			{
				const ICONBASE&					sourceItem		= inputMessage.mUsedItem[ 0 ];
				const ComposeScript::Option&	composeOption	= composeOption_it->second;
	
				// 080916 LUJ, Ŭ���̾�Ʈ�� �����ϱ� ���� �ʱ�ȭ
				MSG_ITEM_OPTION optionMessage;
				ZeroMemory( &optionMessage, sizeof( optionMessage ) );
				optionMessage.Category	= MP_ITEM;
				optionMessage.Protocol	= MP_ITEM_OPTION_ADD_ACK;
				optionMessage.mSize		= 1;
	
				ITEM_OPTION& itemOption	= optionMessage.mOption[ 0 ];
				itemOption.mItemDbIndex	= sourceItem.dwDBIdx;
	
				ITEM_OPTION::Drop::Value& itemOptionValue = itemOption.mDrop.mValue[ 0 ];
				itemOptionValue.mKey	= inputMessage.mOptionKey;
				itemOptionValue.mValue	= composeOption.mValue;
	
				// 080916 LUJ, �˻�� �����̳ʿ� ����� ���� ���� ������ �������̶� ��. �߰��� ���ʽ��� �ش�
				if( script->mResultSet.end() == script->mResultSet.find( resultItemIndex ) )
				{
					if( itemOptionValue.mKey == composeOption.mHiddenBonus.mKey )
					{
						itemOptionValue.mValue += composeOption.mHiddenBonus.mValue;
					}
					else
					{
						itemOption.mDrop.mValue[ 1 ] = composeOption.mHiddenBonus;
					}
				}
	
				AddOption( itemOption );
				// 081118 LUJ, ĳ���� ��ȣ�� ������ ������ DB �����带 ������ Ű������ ���ǵ��� ��
				ItemOptionInsert( itemOption, player->GetID() );
				// 090207 LUJ, �α�
				LogItemOption(
					player->GetID(),
					itemOption.mItemDbIndex,
					itemOption,
					eLog_ItemComposeSuccess );
				ItemDropOptionInsert( itemOption, player->GetID() );
				// 090207 LUJ, �α�
				LogItemDropOption(
					itemOption,
					eLog_ItemComposeSuccess );
	
				player->SendMsg( &optionMessage, optionMessage.GetSize() );
			}
		}
		
		// 080916 LUJ, 0���� ������ �ٸ� ���� �����Ѵ�. 0���� ����� �����ϴµ� ����ؼ� �������� �ʴ´�
		for( DWORD i = 1; i < inputMessage.mUsedItemSize; ++i )
		{
			const ICONBASE& item = inputMessage.mUsedItem[ i ];
	
			if( EI_TRUE != slot->DeleteItemAbs( player, item.Position, 0 ) )
			{
				failureMessage.mType = MSG_ITEM_COMPOSE_NACK::TypeWrongUsedItemSize;
				player->SendMsg( &failureMessage, sizeof( failureMessage ) );
				return;
			}
	
			ItemDeleteToDB( item.dwDBIdx );
			// 090207 LUJ, �α�
			LogItemMoney(
				player->GetID(),
				player->GetObjectName(),
				0,
				"*compose",
				eLog_ItemComposeSuccessWithConsume,
				player->GetMoney(),
				0,
				0,
				item.wIconIdx,
				item.dwDBIdx,
				item.Position,
				0,
				0,
				player->GetPlayerExpPoint() );
		}
	}
	
	// 080916 LUJ, Ŭ���̾�Ʈ�� ������ �˸���.
	{
		MSG_ITEM_COMPOSE_ACK ouptutMessage;
		ZeroMemory( &ouptutMessage, sizeof( ouptutMessage ) );
		ouptutMessage.Category		= MP_ITEM;
		ouptutMessage.Protocol		= MP_ITEM_COMPOSE_ACK;
		
		// 080916 LUJ, ù��° ��ᰡ �ִ� ��ġ�� �ռ� ����� ����Ǿ���.
		const ITEMBASE* resultItem = slot->GetItemInfoAbs( inputMessage.mUsedItem[ 0 ].Position );
	
		if( resultItem )
		{
			// 090122 LUJ, ������ Ÿ������ ����
			ouptutMessage.mResultItem = *resultItem;
		}
		// 080916 LUJ, ����� ����. ó���� ����� �ȵǾ����� �ǹ�
		else
		{
			return;
		}
	
		// 080916 LUJ, ù��° ��Ḧ ������ �ٸ� ���� �Ҹ�� ����̴�.
		for( DWORD i = 1; i < inputMessage.mUsedItemSize; ++i )
		{
			const ICONBASE& sourceItem	= inputMessage.mUsedItem[ i ];
			ICONBASE&		item		= ouptutMessage.mUsedItem[ i - 1 ];
	
			item = sourceItem;
			++( ouptutMessage.mUsedItemSize );
		}
	
		// 080916 LUJ, Ű �������� �Ҹ��Ѵ�
		{
			const ITEMBASE*		keyItem		= slot->GetItemInfoAbs( inputMessage.mKeyItem.Position );
			const ITEM_INFO*	keyItemInfo	= GetItemInfo( inputMessage.mKeyItem.wIconIdx );
	
			if( ! keyItem ||
				! keyItemInfo )
			{
				// 080916 LUJ, �̹� �˻縦 �����Ƿ� �߻��� �� ���� �����̳� üũ����
				return;
			}
	
			ICONBASE& usedKeyItem = ouptutMessage.mUsedKeyItem;
			usedKeyItem.wIconIdx	= keyItem->wIconIdx;
			usedKeyItem.Position	= keyItem->Position;
			usedKeyItem.dwDBIdx		= keyItem->dwDBIdx;
	
			// 080916 LUJ, �ߺ��ǰ� ������ 2�� �̻� ���� ���, ������ �����Ѵ�
			if( keyItemInfo->Stack &&
				keyItem->Durability > 1 )
			{
				const DURTYPE durablity = keyItem->Durability - 1;
	
				if( EI_TRUE != slot->UpdateItemAbs(
					player,
					keyItem->Position,
					keyItem->dwDBIdx,
					keyItem->wIconIdx,
					keyItem->Position,
					keyItem->QuickPosition,
					durablity ) )
				{
					failureMessage.mType = MSG_ITEM_COMPOSE_NACK::TypeWrongUsedItemSize;
					player->SendMsg( &failureMessage, sizeof( failureMessage ) );
					return;
				}
	
				ItemUpdateToDB(
					player->GetID(),
					keyItem->dwDBIdx,
					keyItem->wIconIdx,
					durablity,
					keyItem->Position,
					keyItem->QuickPosition );
				// 090207 LUJ, �α�
				LogItemMoney(
					player->GetID(),
					player->GetObjectName(),
					0,
					"*compose",
					eLog_ItemComposeSuccessWithConsume,
					player->GetMoney(),
					0,
					0,
					keyItem->wIconIdx,
					keyItem->dwDBIdx,
					keyItem->Position,
					0,
					1,
					player->GetPlayerExpPoint() );
			}
			else
			{
				if( EI_TRUE != slot->DeleteItemAbs( player, usedKeyItem.Position, 0 ) )
				{
					failureMessage.mType = MSG_ITEM_COMPOSE_NACK::TypeWrongUsedItemSize;
					player->SendMsg( &failureMessage, sizeof( failureMessage ) );
					return;
				}
	
				ItemDeleteToDB( usedKeyItem.dwDBIdx );
				// 090207 LUJ, �α�
				LogItemMoney(
					player->GetID(),
					player->GetObjectName(),
					0,
					"*compose",
					eLog_ItemComposeSuccessWithConsume,
					player->GetMoney(),
					0,
					0,
					usedKeyItem.wIconIdx,
					usedKeyItem.dwDBIdx,
					usedKeyItem.Position,
					0,
					0,
					player->GetPlayerExpPoint() );
			}			
		}
	
		player->SendMsg( &ouptutMessage, ouptutMessage.GetSize() );
	}
	
	// 080916 LUJ, ���� �ִϸ��̼��� �����Ѵ�. �ռ��� ���а� �߻����� �ʴ´�!
	{
		MSG_DWORD message;
		message.Category	= MP_ITEM;
		message.Protocol	= MP_ITEM_WORKING_SUCCESS;
		message.dwData		= player->GetID();
	
		PACKEDDATA_OBJ->QuickSend( player, &message, sizeof( message ) );
	}	
}

// 080925 LUJ, ���� ���� ������ �����´�
const MixSupportScript* CItemManager::GetMixSupportScript( DWORD itemIndex ) const
{
	const MixSupportScriptMap::const_iterator it = mMixSupportScriptMap.find( itemIndex );

	return mMixSupportScriptMap.end() == it ? 0 : &( it->second );
}

WORD CItemManager::GetItemStackNum( DWORD dwItemIndex )
{
	const ITEM_INFO* pInfo = ITEMMGR->GetItemInfo( dwItemIndex );
	if( NULL == pInfo )
		return 0;

	return pInfo->Stack;
}

void CItemManager::UpdateResult(CPlayer& player, CItemSlot& itemSlot, const CBackupSlot& backupSlot, ITEM_UPDATE_RESULT& itemUpdateResult, MP_PROTOCOL_ITEM protocolForInsert, eLogitemmoney logForRemove, eLogitemmoney logForInsert, const ICONBASE& itemForMixSupport)
{
	const POSTYPE inventoryStartPosition = POSTYPE(TP_INVENTORY_START);
	const POSTYPE inventoryEndPosition = POSTYPE(TP_INVENTORY_END + player.GetInventoryExpansionSize());

	for(POSTYPE position = inventoryStartPosition; inventoryEndPosition > position; ++position)
	{
		ITEMBASE* item = itemSlot.GetItemInfoAbs(
			position);

		// DB �ε����� ������ �� ����
		if(0 == item ||
			0 == item->dwDBIdx)
		{
			item = 0;
		}

		const ITEMBASE*	backupItem = backupSlot.GetItem(position);

		if(0 == item)
		{
			// �������� ���µ�, �̷� ���Կ��� ������ ��� ������ ��Ȳ
			// �Ѵ� ���� ��� ������Ʈ�� �ʿ� ����
			if(backupItem)
			{
				// 071231 KTH -- GetID Add
				ItemDeleteToDB(backupItem->dwDBIdx);

				itemUpdateResult.mItem[itemUpdateResult.mSize++].dwDBIdx = backupItem->dwDBIdx;

				// 071220 LUJ
				LogItemMoney(
					player.GetID(),
					player.GetObjectName(),
					0,
					"",
					logForRemove,
					player.GetMoney(),
					0,
					0,
					backupItem->wIconIdx,
					backupItem->dwDBIdx,
					backupItem->Position,
					0,
					backupItem->Durability,
					0);
			}

			continue;
		}

		// �������� �ִµ�, �̷� ���Կ� ���� ��� ���ԵǴ� ��Ȳ
		if(0 == backupItem)
		{
			const ITEMOBTAINARRAYINFO* itemObtainArray = Alloc(
				&player,
				MP_ITEM,
				BYTE(protocolForInsert),
				player.GetID(),
				// 080925 LUJ, ������� ���� �μ��� ���� ���� ������ ���� �ѱ�� �뵵�� �����
				itemForMixSupport.wIconIdx,
				logForInsert,
				1,
				DBResult(ObtainItemDBResult));
			ItemInsertToDB(
				player.GetID(),
				item->wIconIdx,
				item->Durability,
				item->Position,
				MAKEDWORD(1, itemObtainArray->wObtainArrayID),
				item->nSealed);

			// �������� �����س��� ���� ����
			itemUpdateResult.mItem[itemUpdateResult.mSize++] = *item;

			// 071220 LUJ, DB������ ���� ���� �����ǰų� ��ҵ� �� �����Ƿ� ��û ������ �α׸� ������
			LogItemMoney(
				player.GetID(),
				player.GetObjectName(),
				0,
				"",
				logForInsert,
				player.GetMoney(),
				0,
				0,
				item->wIconIdx,
				item->dwDBIdx,
				item->Position,
				0,
				item->Durability,
				0);

			continue;
		}

		// �������� �ִµ�, �̷� ���԰� ������ ������ �ٸ��� ������Ʈ�� ��Ȳ
		if(item->wIconIdx != backupItem->wIconIdx ||
			item->Durability != backupItem->Durability)
		{
			ItemUpdateToDB(
				player.GetID(),
				item->dwDBIdx,
				item->wIconIdx,
				item->Durability,
				item->Position,
				item->QuickPosition);

			if(item->nSealed == eITEM_TYPE_GET_UNSEAL)
			{
				if(const ITEM_INFO* const itemInfo = GetItemInfo(item->wIconIdx))
				{
					if(itemInfo->nTimeKind == eKIND_REALTIME)
					{
						UnSealItemByRealTime(
							player.GetID(),
							item->dwDBIdx,
							item->nRemainSecond);
					}
					else if(itemInfo->nTimeKind == eKIND_PLAYTIME)
					{
						UnSealItemByGameTime(
							player.GetID(),
							item->dwDBIdx,
							item->nRemainSecond);
					}
				}

				item->nSealed = eITEM_TYPE_UNSEAL;
			}

			UpdateRemainTime(
				player.GetID(),
				item->dwDBIdx,
				item->nSealed,
				item->nRemainSecond);
			itemUpdateResult.mItem[itemUpdateResult.mSize++] = *item;

			// 071220, LUJ. ������ �ٸ��� mirrorItem�� �Һ�Ǿ�, item�� ��� �����
			if(item->wIconIdx != backupItem->wIconIdx)
			{
				LogItemMoney(
					player.GetID(),
					player.GetObjectName(),
					0,
					"",
					logForRemove,
					player.GetMoney(),
					0,
					0,
					backupItem->wIconIdx,
					backupItem->dwDBIdx,
					backupItem->Position,
					0,
					backupItem->Durability,
					0);
				LogItemMoney(
					player.GetID(),
					player.GetObjectName(),
					0,
					"",
					logForInsert,
					player.GetMoney(),
					0,
					0,
					item->wIconIdx,
					item->dwDBIdx,
					item->Position,
					0,
					item->Durability,
					0);

				if(MP_ITEM_MIX_GETITEM != protocolForInsert ||
					FALSE == AddMixOption(player, *item, itemForMixSupport))
				{
					ITEM_OPTION emptyOption	= {0};
					emptyOption.mItemDbIndex = item->dwDBIdx;
					ItemOptionInsert(
						emptyOption,
						player.GetID());
					ItemDropOptionInsert(
						emptyOption,
						player.GetID());
					ItemOptionDelete(
						item->dwDBIdx);
					RemoveOption(
						item->dwDBIdx);
				}
			}
			else if(IsDupItem(item->wIconIdx))
			{
				const BOOL isUsed = (backupItem->Durability > item->Durability);

				LogItemMoney(
					player.GetID(),
					player.GetObjectName(),
					0,
					"",
					isUsed ? logForRemove : logForInsert,
					player.GetMoney(),
					0,
					0,
					item->wIconIdx,
					item->dwDBIdx,
					item->Position,
					0,
					abs(int(backupItem->Durability) - int(item->Durability)),
					0);
			}
		}
	}
}

ITEM_OPTION CItemManager::LimiterItemOption(ITEM_OPTION option, const ITEM_INFO *info, CPlayer *player, BOOL isRare)
{
	DWORD enchantbonus = info->EnchantDeterm *option.mEnchant.mLevel;
	DWORD maxreinstr = 0;
	DWORD maxreindex = 0;
	DWORD maxreinvit = 0;
	DWORD maxreinint = 0;
	DWORD maxreinwis = 0;
	DWORD maxreinlife = 0;
	DWORD maxreinmana = 0;
	DWORD maxreinliferec = 0;
	DWORD maxreinmanarec = 0;
	DWORD maxreinpatk = 0;
	DWORD maxreinmatk = 0;
	DWORD maxreinpdef = 0;
	DWORD maxreinmdef = 0;
	DWORD maxreinmspeed = 0;
	DWORD maxreineva = 0;
	DWORD maxreinacc = 0;
	DWORD maxreincrit = 0;

	if (isRare)
	{
		maxreinstr = 35;
		maxreindex = 35;
		maxreinvit = 35;
		maxreinint = 35;
		maxreinwis = 35;
		maxreinlife = 700;
		maxreinmana = 700;
		maxreinliferec = 20;
		maxreinmanarec = 20;
		maxreinpatk = 30;
		maxreinmatk = 40;
		maxreinpdef = 35;
		maxreinmdef = 35;
		maxreinmspeed = 150;
		maxreineva = 18;
		maxreinacc = 18;
		maxreincrit = 120;
	}
	else
	{
		maxreinstr = 25;
		maxreindex = 25;
		maxreinvit = 25;
		maxreinint = 25;
		maxreinwis = 25;
		maxreinlife = 500;
		maxreinmana = 500;
		maxreinliferec = 15;
		maxreinmanarec = 15;
		maxreinpatk = 20;
		maxreinmatk = 25;
		maxreinpdef = 25;
		maxreinmdef = 25;
		maxreinmspeed = 100;
		maxreineva = 12;
		maxreinacc = 12;
		maxreincrit = 100;
	}

	BOOL m_logged = FALSE;

	ITEM_OPTION optionbefore = option;

	switch (info->EquipType)
	{
		case eEquipType_Weapon:
			{
				maxreinpatk += enchantbonus;
				maxreinmatk += enchantbonus;

				if (option.mReinforce.mStrength > maxreinstr)
				{
					option.mReinforce.mStrength = maxreinstr;
					m_logged = TRUE;
				}

				if (option.mReinforce.mDexterity > maxreindex)
				{
					option.mReinforce.mDexterity = maxreindex;
					m_logged = TRUE;
				}

				if (option.mReinforce.mVitality > maxreinvit)
				{
					option.mReinforce.mVitality = maxreinvit;
					m_logged = TRUE;
				}

				if (option.mReinforce.mIntelligence > maxreinint)
				{
					option.mReinforce.mIntelligence = maxreinint;
					m_logged = TRUE;
				}

				if (option.mReinforce.mWisdom > maxreinwis)
				{
					option.mReinforce.mWisdom = maxreinwis;
					m_logged = TRUE;
				}

				if (option.mReinforce.mLife > maxreinlife)
				{
					option.mReinforce.mLife = maxreinlife;
					m_logged = TRUE;
				}

				if (option.mReinforce.mMana > maxreinmana)
				{
					option.mReinforce.mMana = maxreinmana;
					m_logged = TRUE;
				}

				if (option.mReinforce.mLifeRecovery > maxreinliferec)
				{
					option.mReinforce.mLifeRecovery = maxreinliferec;
					m_logged = TRUE;
				}

				if (option.mReinforce.mManaRecovery > maxreinmanarec)
				{
					option.mReinforce.mManaRecovery = maxreinmanarec;
					m_logged = TRUE;
				}

				if (option.mReinforce.mPhysicAttack > maxreinpatk)
				{
					option.mReinforce.mPhysicAttack = maxreinpatk;
					m_logged = TRUE;
				}

				if (option.mReinforce.mMagicAttack > maxreinmatk)
				{
					option.mReinforce.mMagicAttack = maxreinmatk;
					m_logged = TRUE;
				}

				if (option.mReinforce.mPhysicDefence > maxreinpdef)
				{
					option.mReinforce.mPhysicDefence = maxreinpdef;
					m_logged = TRUE;
				}

				if (option.mReinforce.mMagicDefence > maxreinmdef)
				{
					option.mReinforce.mMagicDefence = maxreinmdef;
					m_logged = TRUE;
				}

				if (option.mReinforce.mMoveSpeed > maxreinmspeed)
				{
					option.mReinforce.mMoveSpeed = maxreinmspeed;
					m_logged = TRUE;
				}

				if (option.mReinforce.mEvade > maxreineva)
				{
					option.mReinforce.mEvade = maxreineva;
					m_logged = TRUE;
				}

				if (option.mReinforce.mAccuracy > maxreinacc)
				{
					option.mReinforce.mAccuracy = maxreinacc;
					m_logged = TRUE;
				}

				if (option.mReinforce.mCriticalRate > maxreincrit)
				{
					option.mReinforce.mCriticalRate = maxreincrit;
					m_logged = TRUE;
				}

				//minus fix

				if (option.mReinforce.mPhysicAttack < enchantbonus)
				{
					option.mReinforce.mPhysicAttack = enchantbonus;
					m_logged = TRUE;
				}

				if (option.mReinforce.mMagicAttack < enchantbonus)
				{
					option.mReinforce.mMagicAttack = enchantbonus;
					m_logged = TRUE;
				}
			}

			break;
		case eEquipType_Armor:
			{
				maxreinpdef += enchantbonus;
				maxreinmdef += enchantbonus;

				if (option.mReinforce.mStrength > maxreinstr)
				{
					option.mReinforce.mStrength = maxreinstr;
					m_logged = TRUE;
				}

				if (option.mReinforce.mDexterity > maxreindex)
				{
					option.mReinforce.mDexterity = maxreindex;
					m_logged = TRUE;
				}

				if (option.mReinforce.mVitality > maxreinvit)
				{
					option.mReinforce.mVitality = maxreinvit;
					m_logged = TRUE;
				}

				if (option.mReinforce.mIntelligence > maxreinint)
				{
					option.mReinforce.mIntelligence = maxreinint;
					m_logged = TRUE;
				}

				if (option.mReinforce.mWisdom > maxreinwis)
				{
					option.mReinforce.mWisdom = maxreinwis;
					m_logged = TRUE;
				}

				if (option.mReinforce.mLife > maxreinlife)
				{
					option.mReinforce.mLife = maxreinlife;
					m_logged = TRUE;
				}

				if (option.mReinforce.mMana > maxreinmana)
				{
					option.mReinforce.mMana = maxreinmana;
					m_logged = TRUE;
				}

				if (option.mReinforce.mLifeRecovery > maxreinliferec)
				{
					option.mReinforce.mLifeRecovery = maxreinliferec;
					m_logged = TRUE;
				}

				if (option.mReinforce.mManaRecovery > maxreinmanarec)
				{
					option.mReinforce.mManaRecovery = maxreinmanarec;
					m_logged = TRUE;
				}

				if (option.mReinforce.mPhysicAttack > maxreinpatk)
				{
					option.mReinforce.mPhysicAttack = maxreinpatk;
					m_logged = TRUE;
				}

				if (option.mReinforce.mMagicAttack > maxreinmatk)
				{
					option.mReinforce.mMagicAttack = maxreinmatk;
					m_logged = TRUE;
				}

				if (option.mReinforce.mPhysicDefence > maxreinpdef)
				{
					option.mReinforce.mPhysicDefence = maxreinpdef;
					m_logged = TRUE;
				}

				if (option.mReinforce.mMagicDefence > maxreinmdef)
				{
					option.mReinforce.mMagicDefence = maxreinmdef;
					m_logged = TRUE;
				}

				if (option.mReinforce.mMoveSpeed > maxreinmspeed)
				{
					option.mReinforce.mMoveSpeed = maxreinmspeed;
					m_logged = TRUE;
				}

				if (option.mReinforce.mEvade > maxreineva)
				{
					option.mReinforce.mEvade = maxreineva;
					m_logged = TRUE;
				}

				if (option.mReinforce.mAccuracy > maxreinacc)
				{
					option.mReinforce.mAccuracy = maxreinacc;
					m_logged = TRUE;
				}

				if (option.mReinforce.mCriticalRate > maxreincrit)
				{
					option.mReinforce.mCriticalRate = maxreincrit;
					m_logged = TRUE;
				}

				//minus fix
				if (option.mReinforce.mPhysicDefence < enchantbonus)
				{
					option.mReinforce.mPhysicDefence = enchantbonus;
					m_logged = TRUE;
				}
			}

			break;
		case eEquipType_Accessary:
			{
				maxreinpdef += enchantbonus;
				maxreinmdef += enchantbonus;

				if (option.mReinforce.mStrength > maxreinstr)
				{
					option.mReinforce.mStrength = maxreinstr;
					m_logged = TRUE;
				}

				if (option.mReinforce.mDexterity > maxreindex)
				{
					option.mReinforce.mDexterity = maxreindex;
					m_logged = TRUE;
				}

				if (option.mReinforce.mVitality > maxreinvit)
				{
					option.mReinforce.mVitality = maxreinvit;
					m_logged = TRUE;
				}

				if (option.mReinforce.mIntelligence > maxreinint)
				{
					option.mReinforce.mIntelligence = maxreinint;
					m_logged = TRUE;
				}

				if (option.mReinforce.mWisdom > maxreinwis)
				{
					option.mReinforce.mWisdom = maxreinwis;
					m_logged = TRUE;
				}

				if (option.mReinforce.mLife > maxreinlife)
				{
					option.mReinforce.mLife = maxreinlife;
					m_logged = TRUE;
				}

				if (option.mReinforce.mMana > maxreinmana)
				{
					option.mReinforce.mMana = maxreinmana;
					m_logged = TRUE;
				}

				if (option.mReinforce.mLifeRecovery > maxreinliferec)
				{
					option.mReinforce.mLifeRecovery = maxreinliferec;
					m_logged = TRUE;
				}

				if (option.mReinforce.mManaRecovery > maxreinmanarec)
				{
					option.mReinforce.mManaRecovery = maxreinmanarec;
					m_logged = TRUE;
				}

				if (option.mReinforce.mPhysicAttack > maxreinpatk)
				{
					option.mReinforce.mPhysicAttack = maxreinpatk;
					m_logged = TRUE;
				}

				if (option.mReinforce.mMagicAttack > maxreinmatk)
				{
					option.mReinforce.mMagicAttack = maxreinmatk;
					m_logged = TRUE;
				}

				if (option.mReinforce.mPhysicDefence > maxreinpdef)
				{
					option.mReinforce.mPhysicDefence = maxreinpdef;
					m_logged = TRUE;
				}

				if (option.mReinforce.mMagicDefence > maxreinmdef)
				{
					option.mReinforce.mMagicDefence = maxreinmdef;
					m_logged = TRUE;
				}

				if (option.mReinforce.mMoveSpeed > maxreinmspeed)
				{
					option.mReinforce.mMoveSpeed = maxreinmspeed;
					m_logged = TRUE;
				}

				if (option.mReinforce.mEvade > maxreineva)
				{
					option.mReinforce.mEvade = maxreineva;
					m_logged = TRUE;
				}

				if (option.mReinforce.mAccuracy > maxreinacc)
				{
					option.mReinforce.mAccuracy = maxreinacc;
					m_logged = TRUE;
				}

				if (option.mReinforce.mCriticalRate > maxreincrit)
				{
					option.mReinforce.mCriticalRate = maxreincrit;
					m_logged = TRUE;
				}

				//minus fix
				if (option.mReinforce.mMagicDefence < enchantbonus)
				{
					option.mReinforce.mMagicDefence = enchantbonus;
					m_logged = TRUE;
				}
			}

			break;
	}

	if (m_logged)
	{
		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);
		FILE *fpLog = NULL;
		char fname[256];
		sprintf(fname, "./Log/LogLimiter%02d-%02d.txt", sysTime.wMonth, sysTime.wDay);
		fpLog = fopen(fname, "a+");
		if (fpLog)
		{
			fprintf(fpLog, "[%02d:%02d][%s][%d - %s]: before[%d - %d - %d - %d - %d - %d - %d - %d - %d - %d - %d - %d - %d - %d - %d - %d - %d] after[%d - %d - %d - %d - %d - %d - %d - %d - %d - %d - %d - %d - %d - %d - %d - %d - %d]\n", sysTime.wHour, sysTime.wMinute, player->GetObjectName(), option.mItemDbIndex, info->ItemName,
				optionbefore.mReinforce.mStrength,
				optionbefore.mReinforce.mDexterity,
				optionbefore.mReinforce.mVitality,
				optionbefore.mReinforce.mIntelligence,
				optionbefore.mReinforce.mWisdom,
				optionbefore.mReinforce.mLife,
				optionbefore.mReinforce.mMana,
				optionbefore.mReinforce.mLifeRecovery,
				optionbefore.mReinforce.mManaRecovery,
				optionbefore.mReinforce.mPhysicAttack,
				optionbefore.mReinforce.mMagicAttack,
				optionbefore.mReinforce.mPhysicDefence,
				optionbefore.mReinforce.mMagicDefence,
				optionbefore.mReinforce.mMoveSpeed,
				optionbefore.mReinforce.mEvade,
				optionbefore.mReinforce.mAccuracy,
				optionbefore.mReinforce.mCriticalRate,
				option.mReinforce.mStrength,
				option.mReinforce.mDexterity,
				option.mReinforce.mVitality,
				option.mReinforce.mIntelligence,
				option.mReinforce.mWisdom,
				option.mReinforce.mLife,
				option.mReinforce.mMana,
				option.mReinforce.mLifeRecovery,
				option.mReinforce.mManaRecovery,
				option.mReinforce.mPhysicAttack,
				option.mReinforce.mMagicAttack,
				option.mReinforce.mPhysicDefence,
				option.mReinforce.mMagicDefence,
				option.mReinforce.mMoveSpeed,
				option.mReinforce.mEvade,
				option.mReinforce.mAccuracy,
				option.mReinforce.mCriticalRate);
			fclose(fpLog);
		}
	}

	return option;
}

ITEM_OPTION CItemManager::LoadLimiterItemOption(ITEM_OPTION option, const ITEM_INFO *info, CPlayer *player, BOOL isRare)
{
	DWORD enchantbonus = info->EnchantDeterm *option.mEnchant.mLevel;
	DWORD maxreinstr = 0;
	DWORD maxreindex = 0;
	DWORD maxreinvit = 0;
	DWORD maxreinint = 0;
	DWORD maxreinwis = 0;
	DWORD maxreinlife = 0;
	DWORD maxreinmana = 0;
	DWORD maxreinliferec = 0;
	DWORD maxreinmanarec = 0;
	DWORD maxreinpatk = 0;
	DWORD maxreinmatk = 0;
	DWORD maxreinpdef = 0;
	DWORD maxreinmdef = 0;
	DWORD maxreinmspeed = 0;
	DWORD maxreineva = 0;
	DWORD maxreinacc = 0;
	DWORD maxreincrit = 0;

	if (isRare)
	{
		maxreinstr = 35;
		maxreindex = 35;
		maxreinvit = 35;
		maxreinint = 35;
		maxreinwis = 35;
		maxreinlife = 700;
		maxreinmana = 700;
		maxreinliferec = 20;
		maxreinmanarec = 20;
		maxreinpatk = 30;
		maxreinmatk = 40;
		maxreinpdef = 35;
		maxreinmdef = 35;
		maxreinmspeed = 150;
		maxreineva = 18;
		maxreinacc = 18;
		maxreincrit = 120;
	}
	else
	{
		maxreinstr = 25;
		maxreindex = 25;
		maxreinvit = 25;
		maxreinint = 25;
		maxreinwis = 25;
		maxreinlife = 500;
		maxreinmana = 500;
		maxreinliferec = 15;
		maxreinmanarec = 15;
		maxreinpatk = 20;
		maxreinmatk = 25;
		maxreinpdef = 25;
		maxreinmdef = 25;
		maxreinmspeed = 100;
		maxreineva = 12;
		maxreinacc = 12;
		maxreincrit = 100;
	}

	BOOL m_logged = FALSE;

	ITEM_OPTION optionbefore = option;

	switch (info->EquipType)
	{
		case eEquipType_Weapon:
			{
				maxreinpatk += enchantbonus;
				maxreinmatk += enchantbonus;

				if (option.mReinforce.mStrength > maxreinstr)
				{
					option.mReinforce.mStrength = maxreinstr;
					m_logged = TRUE;
				}

				if (option.mReinforce.mDexterity > maxreindex)
				{
					option.mReinforce.mDexterity = maxreindex;
					m_logged = TRUE;
				}

				if (option.mReinforce.mVitality > maxreinvit)
				{
					option.mReinforce.mVitality = maxreinvit;
					m_logged = TRUE;
				}

				if (option.mReinforce.mIntelligence > maxreinint)
				{
					option.mReinforce.mIntelligence = maxreinint;
					m_logged = TRUE;
				}

				if (option.mReinforce.mWisdom > maxreinwis)
				{
					option.mReinforce.mWisdom = maxreinwis;
					m_logged = TRUE;
				}

				if (option.mReinforce.mLife > maxreinlife)
				{
					option.mReinforce.mLife = maxreinlife;
					m_logged = TRUE;
				}

				if (option.mReinforce.mMana > maxreinmana)
				{
					option.mReinforce.mMana = maxreinmana;
					m_logged = TRUE;
				}

				if (option.mReinforce.mLifeRecovery > maxreinliferec)
				{
					option.mReinforce.mLifeRecovery = maxreinliferec;
					m_logged = TRUE;
				}

				if (option.mReinforce.mManaRecovery > maxreinmanarec)
				{
					option.mReinforce.mManaRecovery = maxreinmanarec;
					m_logged = TRUE;
				}

				if (option.mReinforce.mPhysicAttack > maxreinpatk)
				{
					option.mReinforce.mPhysicAttack = maxreinpatk;
					m_logged = TRUE;
				}

				if (option.mReinforce.mMagicAttack > maxreinmatk)
				{
					option.mReinforce.mMagicAttack = maxreinmatk;
					m_logged = TRUE;
				}

				if (option.mReinforce.mPhysicDefence > maxreinpdef)
				{
					option.mReinforce.mPhysicDefence = maxreinpdef;
					m_logged = TRUE;
				}

				if (option.mReinforce.mMagicDefence > maxreinmdef)
				{
					option.mReinforce.mMagicDefence = maxreinmdef;
					m_logged = TRUE;
				}

				if (option.mReinforce.mMoveSpeed > maxreinmspeed)
				{
					option.mReinforce.mMoveSpeed = maxreinmspeed;
					m_logged = TRUE;
				}

				if (option.mReinforce.mEvade > maxreineva)
				{
					option.mReinforce.mEvade = maxreineva;
					m_logged = TRUE;
				}

				if (option.mReinforce.mAccuracy > maxreinacc)
				{
					option.mReinforce.mAccuracy = maxreinacc;
					m_logged = TRUE;
				}

				if (option.mReinforce.mCriticalRate > maxreincrit)
				{
					option.mReinforce.mCriticalRate = maxreincrit;
					m_logged = TRUE;
				}

				//minus fix
				if (option.mReinforce.mPhysicAttack < enchantbonus)
				{
					option.mReinforce.mPhysicAttack = enchantbonus;
					m_logged = TRUE;
				}

				if (option.mReinforce.mMagicAttack < enchantbonus)
				{
					option.mReinforce.mMagicAttack = enchantbonus;
					m_logged = TRUE;
				}
			}

			break;
		case eEquipType_Armor:
			{
				maxreinpdef += enchantbonus;
				maxreinmdef += enchantbonus;

				if (option.mReinforce.mStrength > maxreinstr)
				{
					option.mReinforce.mStrength = maxreinstr;
					m_logged = TRUE;
				}

				if (option.mReinforce.mDexterity > maxreindex)
				{
					option.mReinforce.mDexterity = maxreindex;
					m_logged = TRUE;
				}

				if (option.mReinforce.mVitality > maxreinvit)
				{
					option.mReinforce.mVitality = maxreinvit;
					m_logged = TRUE;
				}

				if (option.mReinforce.mIntelligence > maxreinint)
				{
					option.mReinforce.mIntelligence = maxreinint;
					m_logged = TRUE;
				}

				if (option.mReinforce.mWisdom > maxreinwis)
				{
					option.mReinforce.mWisdom = maxreinwis;
					m_logged = TRUE;
				}

				if (option.mReinforce.mLife > maxreinlife)
				{
					option.mReinforce.mLife = maxreinlife;
					m_logged = TRUE;
				}

				if (option.mReinforce.mMana > maxreinmana)
				{
					option.mReinforce.mMana = maxreinmana;
					m_logged = TRUE;
				}

				if (option.mReinforce.mLifeRecovery > maxreinliferec)
				{
					option.mReinforce.mLifeRecovery = maxreinliferec;
					m_logged = TRUE;
				}

				if (option.mReinforce.mManaRecovery > maxreinmanarec)
				{
					option.mReinforce.mManaRecovery = maxreinmanarec;
					m_logged = TRUE;
				}

				if (option.mReinforce.mPhysicAttack > maxreinpatk)
				{
					option.mReinforce.mPhysicAttack = maxreinpatk;
					m_logged = TRUE;
				}

				if (option.mReinforce.mMagicAttack > maxreinmatk)
				{
					option.mReinforce.mMagicAttack = maxreinmatk;
					m_logged = TRUE;
				}

				if (option.mReinforce.mPhysicDefence > maxreinpdef)
				{
					option.mReinforce.mPhysicDefence = maxreinpdef;
					m_logged = TRUE;
				}

				if (option.mReinforce.mMagicDefence > maxreinmdef)
				{
					option.mReinforce.mMagicDefence = maxreinmdef;
					m_logged = TRUE;
				}

				if (option.mReinforce.mMoveSpeed > maxreinmspeed)
				{
					option.mReinforce.mMoveSpeed = maxreinmspeed;
					m_logged = TRUE;
				}

				if (option.mReinforce.mEvade > maxreineva)
				{
					option.mReinforce.mEvade = maxreineva;
					m_logged = TRUE;
				}

				if (option.mReinforce.mAccuracy > maxreinacc)
				{
					option.mReinforce.mAccuracy = maxreinacc;
					m_logged = TRUE;
				}

				if (option.mReinforce.mCriticalRate > maxreincrit)
				{
					option.mReinforce.mCriticalRate = maxreincrit;
					m_logged = TRUE;
				}

				//minus fix
				if (option.mReinforce.mPhysicDefence < enchantbonus)
				{
					option.mReinforce.mPhysicDefence = enchantbonus;
					m_logged = TRUE;
				}
			}

			break;
		case eEquipType_Accessary:
			{
				maxreinpdef += enchantbonus;
				maxreinmdef += enchantbonus;

				if (option.mReinforce.mStrength > maxreinstr)
				{
					option.mReinforce.mStrength = maxreinstr;
					m_logged = TRUE;
				}

				if (option.mReinforce.mDexterity > maxreindex)
				{
					option.mReinforce.mDexterity = maxreindex;
					m_logged = TRUE;
				}

				if (option.mReinforce.mVitality > maxreinvit)
				{
					option.mReinforce.mVitality = maxreinvit;
					m_logged = TRUE;
				}

				if (option.mReinforce.mIntelligence > maxreinint)
				{
					option.mReinforce.mIntelligence = maxreinint;
					m_logged = TRUE;
				}

				if (option.mReinforce.mWisdom > maxreinwis)
				{
					option.mReinforce.mWisdom = maxreinwis;
					m_logged = TRUE;
				}

				if (option.mReinforce.mLife > maxreinlife)
				{
					option.mReinforce.mLife = maxreinlife;
					m_logged = TRUE;
				}

				if (option.mReinforce.mMana > maxreinmana)
				{
					option.mReinforce.mMana = maxreinmana;
					m_logged = TRUE;
				}

				if (option.mReinforce.mLifeRecovery > maxreinliferec)
				{
					option.mReinforce.mLifeRecovery = maxreinliferec;
					m_logged = TRUE;
				}

				if (option.mReinforce.mManaRecovery > maxreinmanarec)
				{
					option.mReinforce.mManaRecovery = maxreinmanarec;
					m_logged = TRUE;
				}

				if (option.mReinforce.mPhysicAttack > maxreinpatk)
				{
					option.mReinforce.mPhysicAttack = maxreinpatk;
					m_logged = TRUE;
				}

				if (option.mReinforce.mMagicAttack > maxreinmatk)
				{
					option.mReinforce.mMagicAttack = maxreinmatk;
					m_logged = TRUE;
				}

				if (option.mReinforce.mPhysicDefence > maxreinpdef)
				{
					option.mReinforce.mPhysicDefence = maxreinpdef;
					m_logged = TRUE;
				}

				if (option.mReinforce.mMagicDefence > maxreinmdef)
				{
					option.mReinforce.mMagicDefence = maxreinmdef;
					m_logged = TRUE;
				}

				if (option.mReinforce.mMoveSpeed > maxreinmspeed)
				{
					option.mReinforce.mMoveSpeed = maxreinmspeed;
					m_logged = TRUE;
				}

				if (option.mReinforce.mEvade > maxreineva)
				{
					option.mReinforce.mEvade = maxreineva;
					m_logged = TRUE;
				}

				if (option.mReinforce.mAccuracy > maxreinacc)
				{
					option.mReinforce.mAccuracy = maxreinacc;
					m_logged = TRUE;
				}

				if (option.mReinforce.mCriticalRate > maxreincrit)
				{
					option.mReinforce.mCriticalRate = maxreincrit;
					m_logged = TRUE;
				}

				//minus fix
				if (option.mReinforce.mMagicDefence < enchantbonus)
				{
					option.mReinforce.mMagicDefence = enchantbonus;
					m_logged = TRUE;
				}
			}

			break;
	}

	if (m_logged)
	{
		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);
		FILE *fpLog = NULL;
		char fname[256];
		sprintf(fname, "./Log/LogLimiter%02d-%02d.txt", sysTime.wMonth, sysTime.wDay);
		fpLog = fopen(fname, "a+");
		if (fpLog)
		{
			fprintf(fpLog, "[%02d:%02d][%s][%d - %s]: before[%d - %d - %d - %d - %d - %d - %d - %d - %d - %d - %d - %d - %d - %d - %d - %d - %d] after[%d - %d - %d - %d - %d - %d - %d - %d - %d - %d - %d - %d - %d - %d - %d - %d - %d]\n", sysTime.wHour, sysTime.wMinute, player->GetObjectName(), option.mItemDbIndex, info->ItemName,
				optionbefore.mReinforce.mStrength,
				optionbefore.mReinforce.mDexterity,
				optionbefore.mReinforce.mVitality,
				optionbefore.mReinforce.mIntelligence,
				optionbefore.mReinforce.mWisdom,
				optionbefore.mReinforce.mLife,
				optionbefore.mReinforce.mMana,
				optionbefore.mReinforce.mLifeRecovery,
				optionbefore.mReinforce.mManaRecovery,
				optionbefore.mReinforce.mPhysicAttack,
				optionbefore.mReinforce.mMagicAttack,
				optionbefore.mReinforce.mPhysicDefence,
				optionbefore.mReinforce.mMagicDefence,
				optionbefore.mReinforce.mMoveSpeed,
				optionbefore.mReinforce.mEvade,
				optionbefore.mReinforce.mAccuracy,
				optionbefore.mReinforce.mCriticalRate,
				option.mReinforce.mStrength,
				option.mReinforce.mDexterity,
				option.mReinforce.mVitality,
				option.mReinforce.mIntelligence,
				option.mReinforce.mWisdom,
				option.mReinforce.mLife,
				option.mReinforce.mMana,
				option.mReinforce.mLifeRecovery,
				option.mReinforce.mManaRecovery,
				option.mReinforce.mPhysicAttack,
				option.mReinforce.mMagicAttack,
				option.mReinforce.mPhysicDefence,
				option.mReinforce.mMagicDefence,
				option.mReinforce.mMoveSpeed,
				option.mReinforce.mEvade,
				option.mReinforce.mAccuracy,
				option.mReinforce.mCriticalRate);
			fclose(fpLog);
		}

		ItemOptionInsert(option, player->GetID());
	}

	return option;
}