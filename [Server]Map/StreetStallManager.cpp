#include "stdafx.h"
#include "StreetStall.h"
#include "StreetStallManager.h"
#include "usertable.h"
#include "PackedData.h"
#include "ObjectStateManager.h"
#include "mapdbmsgparser.h"
#include "Player.h"
#include "itemmanager.h"
#include "CheckRoutine.h"
// 090114 LUJ, ���� �������� �´��� �����ϱ� ���� ����
#include "..\[CC]Header\GameResourceManager.h"
#include "Battle.h"

#include "HousingMgr.h"

GLOBALTON(cStreetStallManager)

#define ITEM_PARAM_SEAL 1

cStreetStallManager::cStreetStallManager()
{
	m_mpStreetStall = new CPool<cStreetStall>;
	m_mpStreetStall->Init( 100, 50, "StreetStallPool" );

	m_StallTable.Initialize(128);
}

cStreetStallManager::~cStreetStallManager()
{
	cStreetStall* pStall;
	m_StallTable.SetPositionHead();
	while( ( pStall = m_StallTable.GetData() ) != NULL)
	{
		m_mpStreetStall->Free(pStall);
	}
	m_StallTable.RemoveAll();

	SAFE_DELETE(m_mpStreetStall);
}


cStreetStall* cStreetStallManager::CreateStreetStall( CPlayer* pOwner, WORD StallKind, char* title )
{
	// ���� �÷��̾ ���� ������ �����ϴ��� Ȯ���ؾ��ϳ� ??
	ASSERT( !m_StallTable.GetData(pOwner->GetID()) );

	cStreetStall* pStall = m_mpStreetStall->Alloc();
	pStall->Init();

	ASSERT( pStall );
	if( pStall == NULL )
		return NULL;

	pStall->SetOwner( pOwner );
	pStall->SetStallKind( StallKind );
	pOwner->SetStreetStallTitle( title );
	
	m_StallTable.Add(pStall,pOwner->GetID());

	return pStall;
}

void cStreetStallManager::DeleteStreetStall( CPlayer* pOwner )
{
	ASSERT(m_StallTable.GetData(pOwner->GetID()));
	cStreetStall* pStall = m_StallTable.GetData(pOwner->GetID());
	
	if( pStall )
	{
		pStall->EmptyCellAll();
		pStall->DeleteGuestAll();
		m_mpStreetStall->Free( pStall );
		m_StallTable.Remove(pOwner->GetID());
	}
}


BOOL cStreetStallManager::IsExist( cStreetStall* pStall )
{
	if( m_StallTable.GetData(pStall->GetOwner()->GetID()) == NULL ) 
		return FALSE;

	return TRUE;
}
/*
BOOL cStreetStallManager::GuestIn( CPlayer* pOwner, CPlayer* pGuest )
{
	cStreetStall* pStall = m_StallTable.GetData(pOwner->GetID());
	if( pStall )
	{
		pStall->AddGuest( pGuest );
		return TRUE;
	}

	return FALSE;
}

void cStreetStallManager::GuestOut( DWORD OwnerId, CPlayer* pGuest )
{
	CPlayer* pOwner = (CPlayer*)g_pUserTable->FindUser( OwnerId ); // ���ʰ� �ִ��� ���� ����.
	if( pOwner == NULL ) return;

	cStreetStall* pStall = m_StallTable.GetData(OwnerId);
	if( pStall )
		pStall->DeleteGuest( pGuest );
}
*/
cStreetStall* cStreetStallManager::FindStreetStall( CPlayer* pOwner )
{
	return m_StallTable.GetData(pOwner->GetID());
}


void cStreetStallManager::StreetStallMode( CPlayer* pPlayer, WORD StallKind, char* title, BOOL bMode )
{
	MSG_STREETSTALL_TITLE msg;
	msg.Category = MP_STREETSTALL;
	msg.dwObjectID = pPlayer->GetID();

	if(bMode ==  TRUE)
	{
		OBJECTSTATEMGR_OBJ->StartObjectState(pPlayer, eObjectState_StreetStall_Owner, 0);
		msg.Protocol = MP_STREETSTALL_START;
		//strcpy( msg.Title, title );
		msg.StallKind = StallKind;
		SafeStrCpy( msg.Title, title, MAX_STREETSTALL_TITLELEN+1 );
	}
	else
	{
		OBJECTSTATEMGR_OBJ->EndObjectState(pPlayer, eObjectState_StreetStall_Owner, 0);
		msg.Protocol = MP_STREETSTALL_END;
		msg.StallKind = StallKind;
	}

	PACKEDDATA_OBJ->QuickSendExceptObjectSelf(pPlayer,&msg,sizeof(msg));
}

void cStreetStallManager::UserLogOut( CPlayer* pPlayer )
{
	cStreetStall* pGestStall = pPlayer->GetGuestStall();
	cStreetStall* pOwnerStall = FindStreetStall( pPlayer );

//find
//m_StallTable.GetData(pOwner->GetID());
//m_StallTable.GetData(pStall->GetOwner()->GetID()

	if( pGestStall ) // ������ �մ����� ������ ���...
	{
		if( IsExist( pGestStall ) )
			pGestStall->DeleteGuest( pPlayer ); // �մ����� �ִ� �������� ���´�.

		MSGBASE msg;
		msg.Category = MP_STREETSTALL;
		msg.Protocol = MP_STREETSTALL_CLOSE;
		msg.dwObjectID = pPlayer->GetID();
		pPlayer->SendMsg(&msg, sizeof(msg));
		return;
	}
	
	if( pOwnerStall ) // ����� ������ �ִ� ���...
	{
		// ����������Ḧ �˸���.(�ڱ��ڽ��� ���º�ȭ)
		MSG_STREETSTALL_TITLE msg;
		msg.Category = MP_STREETSTALL;
		msg.Protocol = MP_STREETSTALL_END;
		msg.StallKind = pOwnerStall->GetStallKind();
		msg.dwObjectID = pPlayer->GetID();
		pPlayer->SendMsg(&msg, sizeof(msg));

		// �մԵ鿡�� ������ ����Ǿ����� �˸���.
		MSGBASE CloseSyn;
		CloseSyn.Category = MP_STREETSTALL;
		CloseSyn.Protocol = MP_STREETSTALL_CLOSE;
		CloseSyn.dwObjectID = pPlayer->GetID();
		pOwnerStall->SendMsgGuestAll( &CloseSyn, sizeof(MSGBASE), TRUE ); 
		StreetStallMode( pPlayer, pOwnerStall->GetStallKind(), NULL, FALSE );

		DeleteStreetStall(pPlayer);

		return;
	}
}

BOOL cStreetStallManager::BuyItem( CPlayer* pOwner, cStreetStall* pStall, CPlayer* pGuest, STREETSTALL_BUYINFO* pBuyInfo )
{
	POSTYPE pos = pBuyInfo->StallPos;		// ���������� ��ġ
	sCELLINFO* pItemInfo = pStall->GetCellInfo( pos );
	ITEMBASE ItemBase = pItemInfo->sItemBase;
	POSTYPE RealPos = ItemBase.Position;	// ���� ������ ������
	MONEYTYPE money = pItemInfo->dwMoney;

	// RaMa - ShopItemInven
	CItemSlot* pGuestInventory;
	CItemSlot* pOwnerInventory;

	ITEM_INFO* pItemKindInfo = ITEMMGR->GetItemInfo( pItemInfo->sItemBase.wIconIdx );
	if( !pItemKindInfo )	return FALSE;

	eITEMTABLE Table = eItemTable_Inventory;

	pGuestInventory	= (CInventorySlot*)pGuest->GetSlot( eItemTable_Inventory );
	pOwnerInventory = (CInventorySlot*)pOwner->GetSlot( eItemTable_Inventory );
	
	MSG_WORD		msgNack; // ���� �޼���
	MSG_LINKITEM	DelSyn; // �����󿡼��� ���� �޼���
	ZeroMemory( &DelSyn, sizeof( DelSyn ) );

	MSG_SELLITEM	sellMsg; // ���ο��� ������ �޼���
	ZeroMemory( &sellMsg, sizeof( sellMsg ) );

	MSG_ITEMEX		buyMsg; // �մԿ��� ������ �޼���
	ZeroMemory( &buyMsg, sizeof( buyMsg ) );

	MSG_STALLMSG	stallMsg;
	
	POSTYPE absPosOut;
	WORD EmptyCellPos[1];
	const ITEMBASE* pInvenItem;

	ITEMBASE temp;

	if(CanBuyItem(pOwner, pGuest, pItemInfo, EmptyCellPos, pBuyInfo, &ItemBase, 1) == FALSE)
		goto ITEMBUY_FAILED;

	absPosOut = EmptyCellPos[0]; // ���� ä�� ���� ��

	pInvenItem = pOwnerInventory->GetItemInfoAbs(pItemInfo->sItemBase.Position);
	
	if(!CHKRT->ItemOf(pOwner, pInvenItem->Position, pInvenItem->wIconIdx,0,0,CB_EXIST|CB_ICONIDX))
	{
		goto ITEMBUY_FAILED;
	}

	if(!CHKRT->ItemOf(pGuest, absPosOut,0,0,0,CB_ICONIDX))
	{
		goto ITEMBUY_FAILED;
	}	

	if(pOwnerInventory->DeleteItemAbs(pOwner, RealPos, &temp, SS_LOCKOMIT) != EI_TRUE)
		goto ITEMBUY_FAILED;
	
	temp.Position = absPosOut;
	if(pGuestInventory->InsertItemAbs(pGuest, absPosOut, &temp, SS_LOCKOMIT) !=  EI_TRUE)
	{	
		//rollback
		temp.Position = RealPos;
		if(pOwnerInventory->InsertItemAbs(pOwner, RealPos, &temp) != EI_TRUE)
		{
			ASSERT(0);
			goto ITEMBUY_FAILED;
		}
		goto ITEMBUY_FAILED;
	}

	// RaMa - ShopItem
	pOwner->SetMoney( pItemInfo->dwMoney, MONEY_ADDITION, 0, eItemTable_Inventory, eMoneyLog_GetStreetStall, pGuest->GetID());
	pGuest->SetMoney( pItemInfo->dwMoney, MONEY_SUBTRACTION, 0, eItemTable_Inventory, eMoneyLog_LoseStreetStall, pOwner->GetID());

	ItemUpdateToDB( pGuest->GetID(), ItemBase.dwDBIdx ,ItemBase.wIconIdx, ItemBase.Durability, absPosOut, 0 );

	// Log ����
	LogItemMoney(pOwner->GetID(), pOwner->GetObjectName(), pGuest->GetID(), pGuest->GetObjectName(),
		eLog_StreetStallBuyAll, pOwner->GetMoney(), pGuest->GetMoney(), pItemInfo->dwMoney,
		temp.wIconIdx, temp.dwDBIdx, 0, temp.Position, temp.Durability, pOwner->GetPlayerExpPoint());

	// �����󿡼� �����Ѵ�.
	pStall->EmptyCell( &ItemBase, Table );
	
	DelSyn.Category = MP_STREETSTALL;
	DelSyn.Protocol = MP_STREETSTALL_DELETEITEM;
	DelSyn.dwObjectID = pBuyInfo->dwObjectID;
	DelSyn.ItemInfo = ItemBase;
	DelSyn.wAbsPosition = pos;

	pStall->SendMsgGuestAll( &DelSyn, sizeof(DelSyn) );

	//////////////////////////////////////////////////////
	// ���ο��Լ� �������� ���� ���� �־� �ش�./////////////////
	sellMsg.Category = MP_STREETSTALL;
	sellMsg.Protocol = MP_STREETSTALL_SELLITEM;
	sellMsg.dwObjectID = pBuyInfo->StallOwnerID;
	sellMsg.ItemInfo = ItemBase;
	sellMsg.count = 0;
	sellMsg.dwData = pBuyInfo->dwObjectID;

	pOwner->SendMsg( &sellMsg, sizeof(sellMsg) );
	/////////////////////////////////////////////////////////////

	ItemBase.Position = absPosOut;
	
	// �մԿ��� �������� ÷���ϰ� ���� ����.////////////////////
	buyMsg.Category = MP_STREETSTALL;
	buyMsg.Protocol = MP_STREETSTALL_BUYITEM_ACK;
	buyMsg.dwObjectID = pBuyInfo->dwObjectID;
	buyMsg.ItemInfo = ItemBase;
	//buyMsg.count = 0;

	buyMsg.sOptionInfo = ITEMMGR->GetOption( ItemBase );
	
	pGuest->SendMsg( &buyMsg, sizeof( buyMsg ) );
	/////////////////////////////////////////////////////////////

	stallMsg.Category = MP_STREETSTALL;
	stallMsg.Protocol = MP_STREETSTALL_MESSAGE;
	stallMsg.dwObjectID = pOwner->GetID();
	stallMsg.SellerID = pOwner->GetID();
	stallMsg.BuyerID = pGuest->GetID();
	stallMsg.ItemIdx = ItemBase.wIconIdx;
	stallMsg.count = 1;
	stallMsg.money = money;
	
	pOwner->SendMsg( &stallMsg, sizeof(MSG_STALLMSG) );
	pGuest->SendMsg( &stallMsg, sizeof(MSG_STALLMSG) );
	
	return TRUE;

ITEMBUY_FAILED:
	msgNack.Category	= MP_STREETSTALL;
	msgNack.Protocol	= MP_STREETSTALL_BUYITEM_NACK;
	msgNack.dwObjectID	= pBuyInfo->dwObjectID;
	msgNack.wData		= 0;
	pGuest->SendMsg( &msgNack, sizeof(msgNack) );

	return FALSE;
}

BOOL cStreetStallManager::BuyDupItem( CPlayer* pOwner, cStreetStall* pStall, CPlayer* pGuest, STREETSTALL_BUYINFO* pBuyInfo )
{
	POSTYPE pos = pBuyInfo->StallPos;	// ����â������ ��ġ
	sCELLINFO* pItemInfo = pStall->GetCellInfo( pos );
	ITEMBASE ItemBase = pItemInfo->sItemBase;
	POSTYPE RealPos = ItemBase.Position; // ���� ������ ������
	DURTYPE wBuyNum = pBuyInfo->ItemInfo.Durability;
	MONEYTYPE money = pItemInfo->dwMoney*wBuyNum;
	
	// RaMa - ShopItemInven
	CItemSlot* pGuestInventory;
	CItemSlot* pOwnerInventory;

	ITEM_INFO* pItemKindInfo = ITEMMGR->GetItemInfo( pItemInfo->sItemBase.wIconIdx );
	if( !pItemKindInfo )	return FALSE;

	// 081202 LUJ, ������ ������ �������� ��쿡�� �ŷ��ϵ��� �Ѵ�
	// 091215 ONS ������Stack�ִ���� �����ۺ��� ������ ������ ó���Ѵ�.
	if( !	pBuyInfo->ItemInfo.Durability ||
			pBuyInfo->ItemInfo.Durability > pItemKindInfo->Stack )
	{
		return FALSE;
	}

	eITEMTABLE Table = eItemTable_Inventory;
	pGuestInventory	= (CInventorySlot*)pGuest->GetSlot( eItemTable_Inventory );
	pOwnerInventory = (CInventorySlot*)pOwner->GetSlot( eItemTable_Inventory );
	
	WORD rt = NOT_ERROR;
	POSTYPE absPosOut;
	WORD EmptyCellPos[1];
	MSG_WORD msgNack; // ���� �޼���
	MSG_LINKITEM DelSyn; // �����󿡼��� ���� �޼���
	MSG_SELLITEM sellMsg; // ���ο��� ������ �޼���
	MSG_ITEMEX buyMsg; // �մԿ��� ������ �޼���
	MSG_SELLITEM updateMsg; // ��ġ�� �������� ��� ��쿡 ���� ó�� 
	MSG_STALLMSG stallMsg;
	const ITEMBASE* pInvenItem;

	if( ItemBase.Durability < wBuyNum )
	{
		rt = NOT_EXIST;
		goto ITEMBUY_FAILED;
	}
	if(CanBuyItem(pOwner, pGuest, pItemInfo, EmptyCellPos, pBuyInfo, &ItemBase, wBuyNum) == FALSE)
		goto ITEMBUY_FAILED;

	absPosOut =  EmptyCellPos[0];

	pInvenItem = pOwnerInventory->GetItemInfoAbs(pItemInfo->sItemBase.Position);
	
	if(!CHKRT->ItemOf(pOwner, pInvenItem->Position, pInvenItem->wIconIdx,0,0,CB_EXIST|CB_ICONIDX))
	{
		goto ITEMBUY_FAILED;
	}
	if(!CHKRT->ItemOf(pGuest, absPosOut,0,0,0,CB_ICONIDX))
	{
		goto ITEMBUY_FAILED;
	}
	
	if( ItemBase.Durability == wBuyNum )
	{
		ITEMBASE temp;
		if(pOwnerInventory->DeleteItemAbs(pOwner, RealPos, &temp, SS_LOCKOMIT) != EI_TRUE)
			goto ITEMBUY_FAILED;
		temp.Position = absPosOut;
		if(pGuestInventory->InsertItemAbs(pGuest, absPosOut, &temp, SS_LOCKOMIT) !=  EI_TRUE)
		{
			//rollback
			temp.Position = RealPos;
			if(pOwnerInventory->InsertItemAbs(pOwner, RealPos, &temp) != EI_TRUE)
			{
				ASSERT(0);
				goto ITEMBUY_FAILED;
			}
			goto ITEMBUY_FAILED;
		}

		ItemUpdateToDB( pGuest->GetID(), temp.dwDBIdx ,temp.wIconIdx, temp.Durability, absPosOut, 0 );
		
		LogItemMoney(pOwner->GetID(), pOwner->GetObjectName(), pGuest->GetID(), pGuest->GetObjectName(),
			eLog_StreetStallBuyAll, pOwner->GetMoney(), pGuest->GetMoney(), money,
			temp.wIconIdx, temp.dwDBIdx, temp.Position, absPosOut, temp.Durability, pOwner->GetPlayerExpPoint());
		// �����󿡼� �����Ѵ�.
		pStall->EmptyCell( &ItemBase, Table );
		
		DelSyn.Category = MP_STREETSTALL;
		DelSyn.Protocol = MP_STREETSTALL_DELETEITEM;
		DelSyn.dwObjectID = pBuyInfo->dwObjectID;
		DelSyn.ItemInfo = ItemBase;
		DelSyn.wAbsPosition = pos;

		pStall->SendMsgGuestAll( &DelSyn, sizeof(DelSyn) );
		////////////////////////////////////////////////////// 
		// ���ο��Լ� �������� ���� ���� �־� �ش�./////////////////
		sellMsg.Category = MP_STREETSTALL;
		sellMsg.Protocol = MP_STREETSTALL_SELLITEM;
		sellMsg.dwObjectID = pBuyInfo->StallOwnerID;
		sellMsg.ItemInfo = ItemBase;
		sellMsg.count = 0;
		sellMsg.dwData = pBuyInfo->dwObjectID;

		pOwner->SendMsg( &sellMsg, sizeof(sellMsg) );
		/////////////////////////////////////////////////////////////
		
		ItemBase.Position = absPosOut;
		
		// �մԿ��� �������� ÷���ϰ� ���� ����.////////////////////
		buyMsg.Category = MP_STREETSTALL;
		buyMsg.Protocol = MP_STREETSTALL_BUYITEM_ACK;
		buyMsg.dwObjectID = pBuyInfo->StallOwnerID;
		buyMsg.ItemInfo = ItemBase;

		//---KES StreetStall Fix 071020
		//---�� �ּ�ó���� �Ǿ� ������. ���߿� Ȯ��.
		//buyMsg.count = 0;
		//-----------------------------

		pGuest->SendMsg( &buyMsg, sizeof(buyMsg) );
	}
	else
	{
		// ��ġ�� �������� ������ ���δ�. (����)
		DURTYPE LeftDurability = ItemBase.Durability - wBuyNum;

		//---KES StreetStall Fix 071020
		//---�������� ��� ����
		// 091215 ONS ������Stack�ִ���� �����ۺ��� ������ ������ ó���Ѵ�.
		const ITEM_INFO* pInfo = ITEMMGR->GetItemInfo( ItemBase.wIconIdx );
		if( !pInfo )	goto ITEMBUY_FAILED;
		if(LeftDurability > pInfo->Stack ) goto ITEMBUY_FAILED;

		//---������Ʈ�� �̸��غ���. (DBIDX���� üũ)
		if( EI_TRUE != pOwnerInventory->UpdateItemAbs(pOwner, RealPos, ItemBase.dwDBIdx, ItemBase.wIconIdx, 0, 0, LeftDurability, UB_DURA, SS_LOCKOMIT|SS_CHKDBIDX ) )
			goto ITEMBUY_FAILED;
		//-----------------------------

		pStall->UpdateCell( pos,  LeftDurability);
		
		updateMsg.Category = MP_STREETSTALL;
		updateMsg.Protocol = MP_STREETSTALL_UPDATEITEM;
		updateMsg.dwObjectID = pBuyInfo->StallOwnerID;
		updateMsg.dwData = pos;
		updateMsg.ItemInfo = ItemBase;
		updateMsg.count = LeftDurability;
		
		pOwner->SendMsg( &updateMsg, sizeof(updateMsg) );
		
		pStall->SendMsgGuestAll( &updateMsg, sizeof(updateMsg) );
		
		//////////////////////////////////////////////////////
		// ���ο��Լ� �������� ���� ���� �־� �ش�./////////////////
		sellMsg.Category = MP_STREETSTALL;
		sellMsg.Protocol = MP_STREETSTALL_SELLITEM;
		sellMsg.dwObjectID = pBuyInfo->StallOwnerID;
		sellMsg.ItemInfo = ItemBase;
		sellMsg.count = LeftDurability;
		sellMsg.dwData = pBuyInfo->dwObjectID;

		pOwner->SendMsg( &sellMsg, sizeof(sellMsg) );

		//---KES StreetStall Fix 071020
		//---�̸� üũ�ϱ� ���� ���� �ڸ� �ű�
		//pOwnerInventory->UpdateItemAbs(pOwner, RealPos, ItemBase.dwDBIdx, ItemBase.wIconIdx, 0, 0, LeftDurability, UB_DURA, SS_LOCKOMIT );
		//-----------------------------

		pOwnerInventory->SetLock(RealPos, TRUE);
	
		ItemUpdateToDB( pOwner->GetID(), ItemBase.dwDBIdx ,ItemBase.wIconIdx, LeftDurability, RealPos, 0 );
		
		LogItemMoney(pOwner->GetID(), pOwner->GetObjectName(), pGuest->GetID(), pGuest->GetObjectName(),
			eLog_StreetStallSellDivide, pOwner->GetMoney(), pGuest->GetMoney(), money,
			ItemBase.wIconIdx, ItemBase.dwDBIdx, ItemBase.Position, absPosOut, LeftDurability, pOwner->GetPlayerExpPoint());

		ITEMBASE NewItemBase;
		ZeroMemory(
			&NewItemBase,
			sizeof(NewItemBase));
		NewItemBase.Position = absPosOut;
		pGuestInventory->InsertItemAbs(
			pGuest,
			absPosOut,
			&NewItemBase,
			SS_PREINSERT);
		SSItemInsert(
			pGuest->GetID(),
			ItemBase.wIconIdx,
			wBuyNum,
			absPosOut,
			pOwner->GetID(),
			ItemBase.nSealed,
			ItemBase.nRemainSecond);
	}
	
	pOwner->SetMoney( money, MONEY_ADDITION, 0, eItemTable_Inventory, eMoneyLog_GetStreetStall, pGuest->GetID());
	pGuest->SetMoney( money, MONEY_SUBTRACTION, 0, eItemTable_Inventory, eMoneyLog_LoseStreetStall, pOwner->GetID());

	stallMsg.Category = MP_STREETSTALL;
	stallMsg.Protocol = MP_STREETSTALL_MESSAGE;
	stallMsg.dwObjectID = pOwner->GetID();
	stallMsg.SellerID = pOwner->GetID();
	stallMsg.BuyerID = pGuest->GetID();
	stallMsg.ItemIdx = ItemBase.wIconIdx;
	stallMsg.count = (WORD)wBuyNum;
	stallMsg.money = money;
	
	pOwner->SendMsg( &stallMsg, sizeof(MSG_STALLMSG) );
	pGuest->SendMsg( &stallMsg, sizeof(MSG_STALLMSG) );

	return TRUE;
	
ITEMBUY_FAILED:
	msgNack.Category	= MP_STREETSTALL;
	msgNack.Protocol	= MP_STREETSTALL_BUYITEM_NACK;
	msgNack.dwObjectID	= pBuyInfo->dwObjectID;
	msgNack.wData		= rt;
	pGuest->SendMsg( &msgNack, sizeof(MSG_WORD) );
	
	return FALSE;
}

BOOL cStreetStallManager::SellItem( CPlayer* pOwner, cStreetStall* pStall, CPlayer* pGuest, STREETSTALL_BUYINFO* pSellInfo )
{
	POSTYPE pos = pSellInfo->StallPos;					// ����â������ ��ġ
	sCELLINFO* pItemInfo = pStall->GetCellInfo( pos );
	
	POSTYPE RealPos = pSellInfo->ItemInfo.Position;		// ���� ������ ������
	
	ITEMBASE ItemBase = pSellInfo->ItemInfo;			//�Ǹ��� �������� ����
	ItemBase.Durability = 0;
	DURTYPE wSellNum = pSellInfo->ItemInfo.Durability;
	MONEYTYPE money = pItemInfo->dwMoney*wSellNum;
	
	ITEM_INFO* pItemKindInfo = ITEMMGR->GetItemInfo( pItemInfo->sItemBase.wIconIdx );
	if( !pItemKindInfo )	return FALSE;

	CItemSlot* pGuestInventory	= (CInventorySlot*)pGuest->GetSlot( eItemTable_Inventory );
	CItemSlot* pOwnerInventory = (CInventorySlot*)pOwner->GetSlot( eItemTable_Inventory );

	WORD	rt = NOT_ERROR;
	MSG_WORD2		msgNack;	// ���� �޼���
	MSG_LINKITEM	DelSyn;		// �����󿡼��� ���� �޼���
	ZeroMemory( &DelSyn, sizeof( DelSyn ) );

	MSG_SELLITEM	sellMsg;	// �մԿ��� ������ �޼���
	ZeroMemory( &sellMsg, sizeof( sellMsg ) );

	MSG_BUYITEM		buyMsg;		// ���ο��� ������ �޼���
	ZeroMemory( &buyMsg, sizeof( buyMsg ) );

	MSG_SELLITEM	updateMsg;	// �������� ������ �����ɶ�
	MSG_STALLMSG	stallMsg;	// �ŷ� ���� �޼���
	
	POSTYPE absPosOut;
	WORD EmptyCellPos[1];
	const ITEMBASE* pInvenItem;

	ITEMBASE temp;

	if(CanSellItem(pOwner, pGuest, pItemInfo, EmptyCellPos, pSellInfo, &ItemBase, 1, rt) == FALSE)
		goto ITEMSELL_FAILED;

	absPosOut = EmptyCellPos[0]; // ���� ä�� ���� ��

	pInvenItem = pGuestInventory->GetItemInfoAbs(RealPos);
	if( !pInvenItem ) goto ITEMSELL_FAILED;

	if( pInvenItem->nSealed == eITEM_TYPE_UNSEAL )
	{
		pItemInfo->bLock = FALSE;
		goto ITEMSELL_FAILED;
	}

	memcpy(&buyMsg.ItemInfo, pInvenItem, sizeof(ITEMBASE));
	
	if(!pInvenItem)
	{
		pItemInfo->bLock = FALSE;
		goto ITEMSELL_FAILED;
	}

	if(!CHKRT->ItemOf(pGuest, pInvenItem->Position, pInvenItem->wIconIdx,0,0,CB_EXIST|CB_ICONIDX))
	{
		pItemInfo->bLock = FALSE;
		goto ITEMSELL_FAILED;
	}

	if(!CHKRT->ItemOf(pOwner, absPosOut,0,0,0,CB_ICONIDX))
	{
		pItemInfo->bLock = FALSE;
		goto ITEMSELL_FAILED;
	}	
	
	if(pGuestInventory->DeleteItemAbs(pGuest, RealPos, &temp, SS_LOCKOMIT) != EI_TRUE)
	{
		pItemInfo->bLock = FALSE;
		goto ITEMSELL_FAILED;
	}
	
	temp.Position = absPosOut;
	if(pOwnerInventory->InsertItemAbs(pOwner, absPosOut, &temp, SS_LOCKOMIT) !=  EI_TRUE)
	{	
		//rollback
		temp.Position = RealPos;
		if(pGuestInventory->InsertItemAbs(pGuest, RealPos, &temp) != EI_TRUE)
		{
			ASSERT(0);
			pItemInfo->bLock = FALSE;
			goto ITEMSELL_FAILED;
		}
		pItemInfo->bLock = FALSE;
		goto ITEMSELL_FAILED;
	}
	
	// RaMa - ShopItem
	pGuest->SetMoney( pItemInfo->dwMoney, MONEY_ADDITION, 0, eItemTable_Inventory, eMoneyLog_GetStreetStall, pOwner->GetID());
	pOwner->SetMoney( pItemInfo->dwMoney, MONEY_SUBTRACTION, 0, eItemTable_Inventory, eMoneyLog_LoseStreetStall, pGuest->GetID());
	
	ItemUpdateToDB( pOwner->GetID(), ItemBase.dwDBIdx ,ItemBase.wIconIdx, ItemBase.Durability, absPosOut, 0 );
	
	// Log ����
	LogItemMoney(pGuest->GetID(), pGuest->GetObjectName(), pOwner->GetID(), pOwner->GetObjectName(),
		eLog_StreetStallBuyAll, pGuest->GetMoney(), pOwner->GetMoney(), pItemInfo->dwMoney,
		temp.wIconIdx, temp.dwDBIdx, 0, temp.Position, temp.Durability, pGuest->GetPlayerExpPoint());
	
	if(pItemInfo->wVolume == 1)
	{
		// �����󿡼� �����Ѵ�.
		pStall->EmptyCell( pos );
		
		DelSyn.Category = MP_STREETSTALL;
		DelSyn.Protocol = MP_STREETSTALL_DELETEITEM;
		DelSyn.dwObjectID = pSellInfo->dwObjectID;
		DelSyn.ItemInfo = ItemBase;
		DelSyn.wAbsPosition = pos;
		
		pStall->SendMsgGuestAll( &DelSyn, sizeof(DelSyn) );
		
		DelSyn.Protocol = MP_STREETSTALL_DELETEITEM_ACK;
		
		pOwner->SendMsg( &DelSyn, sizeof(DelSyn) );
	}
	else
	{	// �����󿡼� ������ ���δ�.
		DURTYPE LeftDurability = pItemInfo->wVolume - wSellNum;
		pStall->UpdateCell( pos,  LeftDurability);
		pStall->GetCellInfo(pos)->bLock = FALSE;
		
		updateMsg.Category = MP_STREETSTALL;
		updateMsg.Protocol = MP_STREETSTALL_UPDATEITEM;
		updateMsg.dwObjectID = pSellInfo->StallOwnerID;
		updateMsg.dwData = pos;
		updateMsg.ItemInfo = ItemBase;
		updateMsg.count = LeftDurability;
		
		pOwner->SendMsg( &updateMsg, sizeof(updateMsg) );
		
		pStall->SendMsgGuestAll( &updateMsg, sizeof(updateMsg) );
	}
	
	//////////////////////////////////////////////////////
	// �մԿ��Լ� �������� ���� ���� �־� �ش�./////////////////
	sellMsg.Category = MP_STREETSTALL;
	sellMsg.Protocol = MP_STREETSTALL_SELLITEM_ACK;
	sellMsg.dwObjectID = pOwner->GetID();
	sellMsg.ItemInfo = ItemBase;
	sellMsg.count = 0;
	sellMsg.dwData = pOwner->GetID();
	
	pGuest->SendMsg( &sellMsg, sizeof(sellMsg) );
	/////////////////////////////////////////////////////////////
	
	ItemBase.Position = absPosOut;
	
	// ���ο��� �������� ÷���ϰ� ���� ����.////////////////////
	buyMsg.Category = MP_STREETSTALL;
	buyMsg.Protocol = MP_STREETSTALL_BUYITEM;
	buyMsg.dwObjectID = pGuest->GetID();
	buyMsg.ItemInfo.Position = absPosOut;
	//memcpy(&buyMsg.ItemInfo, &ItemBase, sizeof(ITEMBASE));
	//buyMsg.count = 0;
	buyMsg.dwData = pGuest->GetID();
	buyMsg.sOptionInfo = ITEMMGR->GetOption( ItemBase );
	
	pOwner->SendMsg( &buyMsg, sizeof(buyMsg) );
	/////////////////////////////////////////////////////////////

	stallMsg.Category = MP_STREETSTALL;
	stallMsg.Protocol = MP_STREETSTALL_MESSAGE;
	stallMsg.dwObjectID = pOwner->GetID();
	stallMsg.SellerID = pGuest->GetID();
	stallMsg.BuyerID = pOwner->GetID();
	stallMsg.ItemIdx = ItemBase.wIconIdx;
	stallMsg.count = (WORD)wSellNum;
	stallMsg.money = money;

	pOwner->SendMsg( &stallMsg, sizeof(MSG_STALLMSG) );
	pGuest->SendMsg( &stallMsg, sizeof(MSG_STALLMSG) );

	return TRUE;
	
ITEMSELL_FAILED:
	msgNack.Category	= MP_STREETSTALL;
	msgNack.Protocol	= MP_STREETSTALL_SELLITEM_NACK;
	msgNack.dwObjectID	= pSellInfo->dwObjectID;
	msgNack.wData1		= rt;
	msgNack.wData2		= RealPos;
	pGuest->SendMsg( &msgNack, sizeof(MSG_WORD2) );

	return FALSE;
}

BOOL cStreetStallManager::SellDupItem( CPlayer* pOwner, cStreetStall* pStall, CPlayer* pGuest, STREETSTALL_BUYINFO* pSellInfo )
{
	POSTYPE pos = pSellInfo->StallPos;					// ����â������ ��ġ
	sCELLINFO* pItemInfo = pStall->GetCellInfo( pos );	// ��������

	POSTYPE RealPos = pSellInfo->ItemInfo.Position;		// ���� ������ ������

	ITEMBASE ItemBase = pSellInfo->ItemInfo;			// �Ǹ��� �������� ����
	DURTYPE wSellNum = pSellInfo->ItemInfo.Durability;	// �Ǹ� ����
	MONEYTYPE money = pItemInfo->dwMoney;
	// 091215 ONS ������Stack�ִ���� �����ۺ��� ������ ������ ó���Ѵ�.	
	ITEM_INFO* pItemKindInfo = ITEMMGR->GetItemInfo( pItemInfo->sItemBase.wIconIdx );
	if( !pItemKindInfo )	return FALSE;

	// 081202 LUJ, ������ ������ �������� ��쿡�� �ŷ��ϵ��� �Ѵ�
	if( !	pSellInfo->ItemInfo.Durability ||
			pSellInfo->ItemInfo.Durability > pItemKindInfo->Stack )
	{
		return FALSE;
	}

	CItemSlot* pGuestInventory	= (CInventorySlot*)pGuest->GetSlot( eItemTable_Inventory );
	CItemSlot* pOwnerInventory = (CInventorySlot*)pOwner->GetSlot( eItemTable_Inventory );
	
	WORD rt = NOT_ERROR;
	POSTYPE absPosOut;
	WORD EmptyCellPos[1];
	MSG_WORD2 msgNack;			// ���� �޼���

	MSG_LINKITEM DelSyn;		// �������� �����ɶ�
	MSG_SELLITEM updateMsg;		// �������� ������ �����ɶ�
	
	MSG_SELLITEM sellMsg;		// �Ǹ���(�Խ�Ʈ)���� ������ �޼���
	MSG_BUYITEM buyMsg;			// ������(����)���� ������ �޼���

	MSG_STALLMSG stallMsg;
	
	const ITEMBASE* pInvenItem;	// �Ǹ���(�Խ�Ʈ)�� �κ��� �ִ� ���� ������

	if(pItemInfo->wVolume == 0)	// �������� �̹� ������ �������̸� ����
	{
		rt = NOT_EXIST;
		goto ITEMSELL_FAILED;
	}

	if(CanSellItem(pOwner, pGuest, pItemInfo, EmptyCellPos, pSellInfo, &ItemBase, wSellNum, rt) == FALSE)
		goto ITEMSELL_FAILED;

	absPosOut =  EmptyCellPos[0];

	pInvenItem = pGuestInventory->GetItemInfoAbs(RealPos);

	// �ȷ��� ������ �κ��� �ִ� �������� ũ�ٸ�
	// �κ��� �ִ� �������� ����
	if(pInvenItem->Durability < wSellNum)
		wSellNum = pInvenItem->Durability;


	if(!pInvenItem)
	{
		pItemInfo->bLock = FALSE;
		goto ITEMSELL_FAILED;
	}

	if (pInvenItem->wIconIdx != pItemInfo->sItemBase.wIconIdx)
	{
		goto ITEMSELL_FAILED;
	}

	if(!CHKRT->ItemOf(pGuest, pInvenItem->Position, pInvenItem->wIconIdx,0,0,CB_EXIST|CB_ICONIDX))
	{
		pItemInfo->bLock = FALSE;
		goto ITEMSELL_FAILED;
	}
	if(!CHKRT->ItemOf(pOwner, absPosOut,0,0,0,CB_ICONIDX))
	{
		pItemInfo->bLock = FALSE;
		goto ITEMSELL_FAILED;
	}
	
	if( pItemInfo->wVolume == wSellNum ) 
	{	//������ ���Ե�ϵ� ������ �ȷ��� ������ ������ ��ġ�ϸ�
		//�������� ���� �Ѵ�
		pStall->EmptyCell( pos );
		
		DelSyn.Category = MP_STREETSTALL;
		DelSyn.Protocol = MP_STREETSTALL_DELETEITEM;
		DelSyn.dwObjectID = pSellInfo->dwObjectID;
		DelSyn.ItemInfo = ItemBase;
		DelSyn.wAbsPosition = pos;
		
		pStall->SendMsgGuestAll( &DelSyn, sizeof(DelSyn) );
		
		DelSyn.Protocol = MP_STREETSTALL_DELETEITEM_ACK;
		
		pOwner->SendMsg( &DelSyn, sizeof(DelSyn) );
		
	}
	else if(pItemInfo->wVolume > wSellNum)
	{	//������ ���Ե�ϵ� �������� �ȷ��� ������ ������ ������
		//�������� ������ ���δ�
		DURTYPE LeftDurability = pItemInfo->wVolume - wSellNum;
		pStall->UpdateCell( pos,  LeftDurability);
		pStall->GetCellInfo(pos)->bLock = FALSE;

		updateMsg.Category = MP_STREETSTALL;
		updateMsg.Protocol = MP_STREETSTALL_UPDATEITEM;
		updateMsg.dwObjectID = pSellInfo->StallOwnerID;
		updateMsg.dwData = pos;
		updateMsg.ItemInfo = ItemBase;
		updateMsg.count = LeftDurability;
		
		pOwner->SendMsg( &updateMsg, sizeof(updateMsg) );
		
		pStall->SendMsgGuestAll( &updateMsg, sizeof(updateMsg) );
	}
	else
	{	//������ ���� ��ϵ� �������� �ȷ��� ������ ������
		//�������� ���� �ϰ� �ȷ��� ������ �����Ѵ�
		wSellNum = pItemInfo->wVolume;
		
		pStall->EmptyCell( pos );
		
		DelSyn.Category = MP_STREETSTALL;
		DelSyn.Protocol = MP_STREETSTALL_DELETEITEM;
		DelSyn.dwObjectID = pSellInfo->dwObjectID;
		DelSyn.ItemInfo = ItemBase;
		DelSyn.wAbsPosition = pos;
		
		pStall->SendMsgGuestAll( &DelSyn, sizeof(DelSyn) );
		
		DelSyn.Protocol = MP_STREETSTALL_DELETEITEM_ACK;
		
		pOwner->SendMsg( &DelSyn, sizeof(DelSyn) );
	}

	money = money * wSellNum;		// �Ǹ� ����
	
	if( wSellNum == pInvenItem->Durability)
	{	//�ȷ��� ������ �κ��� ������ ��ġ�ϸ�
		//�Խ�Ʈ �κ����� �����ϰ� ������ �κ��� �ִ´�
		ITEMBASE temp;
		if(pGuestInventory->DeleteItemAbs(pGuest, RealPos, &temp, SS_LOCKOMIT) != EI_TRUE)
		{
			pItemInfo->bLock = FALSE;
			goto ITEMSELL_FAILED;
		}
	
		temp.Position = absPosOut;
		if(pOwnerInventory->InsertItemAbs(pGuest, absPosOut, &temp, SS_LOCKOMIT) !=  EI_TRUE)
		{
			//rollback
			temp.Position = RealPos;
			if(pGuestInventory->InsertItemAbs(pOwner, RealPos, &temp) != EI_TRUE)
			{
				ASSERT(0);
				pItemInfo->bLock = FALSE;
				goto ITEMSELL_FAILED;
			}
			pItemInfo->bLock = FALSE;
			goto ITEMSELL_FAILED;
		}
		
		ItemUpdateToDB( pOwner->GetID(), temp.dwDBIdx ,temp.wIconIdx, temp.Durability, absPosOut, 0 );
		
		// 080605 LUJ, ���� �÷��̾� ��ȣ�� �մ� �÷��̾� ��ȣ�� �߸� �����ϴ� ���� ����
		LogItemMoney(pOwner->GetID(), pOwner->GetObjectName(), pGuest->GetID(), pGuest->GetObjectName(),
			eLog_StreetStallBuyAll, pOwner->GetMoney(), pGuest->GetMoney(), money,
			temp.wIconIdx, temp.dwDBIdx, temp.Position, absPosOut, temp.Durability, pOwner->GetPlayerExpPoint());
		
		//////////////////////////////////////////////////////
		// �մԿ��Լ� �������� ���� ���� �־� �ش�./////////////////
		sellMsg.Category = MP_STREETSTALL;
		sellMsg.Protocol = MP_STREETSTALL_SELLITEM_ACK;
		sellMsg.dwObjectID = pOwner->GetID();
		sellMsg.ItemInfo = ItemBase;
		sellMsg.count = 0;
		sellMsg.dwData = pOwner->GetID();
		
		pGuest->SendMsg( &sellMsg, sizeof(sellMsg) );
		/////////////////////////////////////////////////////////////
		
		ItemBase.Position = absPosOut;
		
		// ���ο��� �������� ÷���ϰ� ���� ����.////////////////////
		buyMsg.Category = MP_STREETSTALL;
		buyMsg.Protocol = MP_STREETSTALL_BUYITEM;
		buyMsg.dwObjectID = pGuest->GetID();
		buyMsg.ItemInfo = ItemBase;
		//buyMsg.count = 0;
		buyMsg.dwData = pGuest->GetID();
		
		pOwner->SendMsg( &buyMsg, sizeof( buyMsg ) );
		/////////////////////////////////////////////////////////////
	}
	else
	{	//�ȷ��� ������ �κ��� �������� ������
		//�Խ�Ʈ �κ����� ������ ������ ���̰� ������ �κ��� �ִ´�
		DURTYPE LeftDurability = pInvenItem->Durability - wSellNum;

		//---KES StreetStall Fix 071020
		//---�������� ��� ����
		// 091215 ONS ������Stack�ִ���� �����ۺ��� ������ ������ ó���Ѵ�.
		if(LeftDurability > pItemKindInfo->Stack ) goto ITEMSELL_FAILED;

		//---������Ʈ�� �̸��غ���. (DBIDX���� üũ)
		if( EI_TRUE != pGuestInventory->UpdateItemAbs(pGuest, RealPos, ItemBase.dwDBIdx, ItemBase.wIconIdx, 0, 0, LeftDurability, UB_DURA, SS_LOCKOMIT|SS_CHKDBIDX ) )
			goto ITEMSELL_FAILED;

		ITEMBASE NewItemBase;
		ZeroMemory(
			&NewItemBase,
			sizeof(NewItemBase));
		NewItemBase.Position = absPosOut;
		pOwnerInventory->InsertItemAbs(
			pGuest,
			absPosOut,
			&NewItemBase,
			SS_PREINSERT);
		SSItemInsert(
			pOwner->GetID(),
			ItemBase.wIconIdx,
			wSellNum,
			absPosOut,
			pGuest->GetID(),
			ItemBase.nSealed,
			ItemBase.nRemainSecond);

		ItemBase.Position = RealPos;

		/////////////////////////////////////////////////////////////
		// �մԿ��Լ� �������� ���� ���� �־� �ش�.////////////////////
		sellMsg.Category = MP_STREETSTALL;
		sellMsg.Protocol = MP_STREETSTALL_SELLITEM_ACK;
		sellMsg.dwObjectID = pOwner->GetID();
		sellMsg.ItemInfo = ItemBase;
		sellMsg.count = LeftDurability;
		sellMsg.dwData = pOwner->GetID();
		
		pGuest->SendMsg( &sellMsg, sizeof(sellMsg) );
		
		//---KES StreetStall Fix 071020
		//---�̸� üũ�ϱ� ���� ���� �ڸ� �ű�
		//pGuestInventory->UpdateItemAbs(pGuest, RealPos, ItemBase.dwDBIdx, ItemBase.wIconIdx, 0, 0, LeftDurability, UB_DURA, SS_LOCKOMIT );
		//-----------------------------

		pGuestInventory->SetLock(RealPos, FALSE);
		
		ItemUpdateToDB( pGuest->GetID(), ItemBase.dwDBIdx ,ItemBase.wIconIdx, LeftDurability, RealPos, 0 );
		
		LogItemMoney(pGuest->GetID(), pGuest->GetObjectName(), pOwner->GetID(), pOwner->GetObjectName(),
			eLog_StreetStallSellDivide, pGuest->GetMoney(), pOwner->GetMoney(), money,
			ItemBase.wIconIdx, ItemBase.dwDBIdx, ItemBase.Position, absPosOut, LeftDurability, pGuest->GetPlayerExpPoint());
		
		/////////////////////////////////////////////////////////////
	}
	
	pGuest->SetMoney( money, MONEY_ADDITION, 0, eItemTable_Inventory, eMoneyLog_GetStreetStall, pOwner->GetID());
	pOwner->SetMoney( money, MONEY_SUBTRACTION, 0, eItemTable_Inventory, eMoneyLog_LoseStreetStall, pGuest->GetID());

	stallMsg.Category = MP_STREETSTALL;
	stallMsg.Protocol = MP_STREETSTALL_MESSAGE;
	stallMsg.dwObjectID = pOwner->GetID();
	stallMsg.SellerID = pGuest->GetID();
	stallMsg.BuyerID = pOwner->GetID();
	stallMsg.ItemIdx = ItemBase.wIconIdx;
	stallMsg.count = (WORD)wSellNum;
	stallMsg.money = money;
	
	pOwner->SendMsg( &stallMsg, sizeof(MSG_STALLMSG) );
	pGuest->SendMsg( &stallMsg, sizeof(MSG_STALLMSG) );

	return TRUE;
	
ITEMSELL_FAILED:
	msgNack.Category	= MP_STREETSTALL;
	msgNack.Protocol	= MP_STREETSTALL_SELLITEM_NACK;
	msgNack.dwObjectID	= pSellInfo->dwObjectID;
	msgNack.wData1		= rt;
	msgNack.wData2		= RealPos;
	pGuest->SendMsg( &msgNack, sizeof(MSG_WORD2) );
	
	return FALSE;
}

void cStreetStallManager::CreateDupItem( DWORD dwObjectId, ITEMBASE* pItemBase, DWORD FromChrID )
{
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser( dwObjectId );
	
	if( pPlayer == NULL )
	{
		ASSERT(0);
		return;
	}

	cStreetStall* pStall = FindStreetStall( pPlayer );
	
	if(!pStall)	// ���� �մ��� ��� �Ǹų����̴� 
	{
		MSG_ITEMEX message;
		sizeof( &message, sizeof( message ) );

		message.Category = MP_STREETSTALL;
		message.Protocol = MP_STREETSTALL_BUYITEM_ACK;
		message.dwObjectID = dwObjectId;
		message.ItemInfo = *pItemBase;
		//message.count = 0;
		pPlayer->SendMsg( &message, sizeof( message ) );
	}
	else
	{
		MSG_BUYITEM message;
		ZeroMemory( &message, sizeof( message ) );

		message.Category = MP_STREETSTALL;
		message.Protocol = MP_STREETSTALL_BUYITEM;
		message.dwObjectID = dwObjectId;
		message.ItemInfo = *pItemBase;
		//message.count = 0;
		message.dwData = FromChrID;
		
		pPlayer->SendMsg( &message, sizeof( message ) );
	}
	
	ITEM_INFO* pItemKindInfo = ITEMMGR->GetItemInfo( pItemBase->wIconIdx );
	if( !pItemKindInfo )	return;
	
	CItemSlot* pInventory	= (CInventorySlot*)pPlayer->GetSlot( eItemTable_Inventory );

	// 071213 LUJ, ������ ����� Ǯ���ִ� �޼ҵ带 ����ؾ� �Ѵ�
	pInventory->InsertItemAbsFromDb( pPlayer, *pItemBase );
	
	char FromName[MAX_NAME_LENGTH+1] = {0,};
	MONEYTYPE FromMoney = 0;
	EXPTYPE FromExpPoint = 0;
	CPlayer* pFromPlayer = (CPlayer*)g_pUserTable->FindUser( FromChrID );
	if(pFromPlayer)
	{
		SafeStrCpy(FromName, pFromPlayer->GetObjectName(), MAX_NAME_LENGTH+1);
		FromMoney = pFromPlayer->GetMoney();
		FromExpPoint = pFromPlayer->GetPlayerExpPoint();
	}
	LogItemMoney(FromChrID, FromName, dwObjectId, pPlayer->GetObjectName(),
		eLog_StreetStallBuyDivide, FromMoney, pPlayer->GetMoney(), 0,
		pItemBase->wIconIdx, pItemBase->dwDBIdx, 0, pItemBase->Position, pItemBase->Durability, 
		FromExpPoint);
}

void cStreetStallManager::NetworkMsgParse( BYTE Protocol, void* pMsg )
{
	switch( Protocol )
	{
	case MP_STREETSTALL_OPEN_SYN:
		{
			MSG_STREETSTALL_TITLE* msg = (MSG_STREETSTALL_TITLE*)pMsg;
		// 1. ���� ���
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser( msg->dwObjectID );
			if( !pPlayer ) 
			{
				ASSERT(0);
				break;
			}

			/* State�� ���� Check�� �Ѵ�. */
			//KES SHOWDOWN 031103 �߰� 
			if( pPlayer->IsShowdown() || pPlayer->GetExchangeContainer()->pRoom != NULL ||
				pPlayer->GetBattle()->GetBattleKind() == eBATTLE_KIND_GTOURNAMENT ||
				pPlayer->GetMountedVehicle() )
			{
				MSGBASE CloseSyn;
				CloseSyn.Category = MP_STREETSTALL;
				CloseSyn.Protocol = MP_STREETSTALL_OPEN_NACK;
				pPlayer->SendMsg(&CloseSyn, sizeof(CloseSyn));
				return;
			}
			//
			
			// �Ͽ�¡�ʿ����� �Ұ�
			if(HOUSINGMAPNUM == g_pServerSystem->GetMapNum())
			{
				MSGBASE CloseSyn;
				CloseSyn.Category = MP_STREETSTALL;
				CloseSyn.Protocol = MP_STREETSTALL_OPEN_NACK;
				pPlayer->SendMsg(&CloseSyn, sizeof(CloseSyn));
				return;
			}

	
			// ���� ������ �����ϸ� ���ش�.
			cStreetStall* pStall = FindStreetStall( pPlayer );
			if( pStall )
			{
//				ASSERT(0);
				DeleteStreetStall( pPlayer );

				MSGBASE CloseSyn;
				CloseSyn.Category = MP_STREETSTALL;
				CloseSyn.Protocol = MP_STREETSTALL_OPEN_NACK;
				pPlayer->SendMsg(&CloseSyn, sizeof(CloseSyn));

				return;
			}
/*
			if(msg->StallKind == eSK_BUY)
			{
				MSGBASE CloseSyn;
				CloseSyn.Category = MP_STREETSTALL;
				CloseSyn.Protocol = MP_STREETSTALL_OPEN_NACK;
				pPlayer->SendMsg(&CloseSyn, sizeof(CloseSyn));
				
				return;
			}
*/
			pStall = CreateStreetStall( pPlayer, msg->StallKind, msg->Title );
/*		
			//////////////////////////////////////////////////////////////////////////
			// ���� ������ ��� �߰� �̿� �κ����� �����ͼ� �Ҵ����ش�
			if(msg->StallKind == eSK_BUY)
			{
				pStall->SetDefaultUsable();

				SHOPITEMOPTION*	pShopItemOption = pPlayer->GetShopItemStats();
				pStall->SetExtraUsable( pShopItemOption->BuyStallInven );

				// ���� ������ ��� �κ����� �ִ� �κ����� ������
				// �ִ� �κ����� �Ҵ��Ѵ�
				if(pStall->GetUsable() >  MAX_STREETSTALL_CELLNUM)
					pStall->SetMaxUsable();
			}
			//////////////////////////////////////////////////////////////////////////
*/			
		// 2. ���� Message�� ������.
			MSG_STREETSTALL_TITLE OpenSyn;
			OpenSyn = *msg;
			OpenSyn.StallKind = msg->StallKind;
			OpenSyn.Protocol = MP_STREETSTALL_OPEN_ACK;
		
			pPlayer->SendMsg( &OpenSyn, sizeof(MSG_STREETSTALL_TITLE) );

		// 3. ���� �ο����� ���¸� �����ϵ��� �����Ѵ�.
			StreetStallMode( pPlayer, msg->StallKind, msg->Title, TRUE );

			return;
		}
		break;
	case MP_STREETSTALL_CLOSE_SYN:
		{
			MSGBASE* msg = (MSGBASE*)pMsg;
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser( msg->dwObjectID );
			if( pPlayer == NULL ) break;

			cStreetStall* pStall = FindStreetStall( pPlayer );
			if( pStall == NULL ) break;

			// 1. ���� �ο����� ���¸� �����ϵ��� �����Ѵ�.
			StreetStallMode( pPlayer, pStall->GetStallKind(), NULL, FALSE ); // Ani ����

			// 2. �մԵ鿡�� �˸���. (�մԵ��� ���¸� �����Ͽ��� �Ѵ�.)
			MSGBASE CloseSyn;
			CloseSyn.Category = MP_STREETSTALL;
			CloseSyn.Protocol = MP_STREETSTALL_CLOSE;
			CloseSyn.dwObjectID = msg->dwObjectID;

			pStall->SendMsgGuestAll( &CloseSyn, sizeof(MSGBASE), TRUE );

			// 3. ������ ���� �Ѵ�.
			DeleteStreetStall( pPlayer );

			// 4. ���� Message�� ������.
			CloseSyn.Protocol = MP_STREETSTALL_CLOSE_ACK;

			//���� StreetStallMode���� �̹� ȣ���Ѵ�.
			pPlayer->SendMsg( &CloseSyn, sizeof(MSGBASE) );
		}
		break;
	case MP_STREETSTALL_LOCKITEM_SYN:
		{
			MSG_STREETSTALL_ITEMSTATUS* msg = (MSG_STREETSTALL_ITEMSTATUS*)pMsg;

			CPlayer* pPlayer = NULL;
			cStreetStall* pStall = NULL;
			sCELLINFO* pInfo = NULL;
			// 081016 LUJ, ������ goto �� ���� ��ġ���Ѿ���. goto ������ �� ������ ���� �� ���Կ� ����ϴµ�,
			//			���� �ʱ�ȭ ���� ����ϴ� ��찡 ���� �������� ���ܰ� �߻�����
			DWORD erroflag = 0;

			MSGBASE Ftemp;
			MSG_STREETSTALL_ITEMSTATUS Stemp;

			// 1. ������ ã�Ƽ� �������� Lock �Ѵ�.
			pPlayer = (CPlayer*)g_pUserTable->FindUser( msg->dwObjectID );
			if( !pPlayer )
			{
				ASSERT(0);
				return;
			}

			if( msg->wAbsPosition >= MAX_STREETSTALL_CELLNUM ) goto ITEMLOCKFAIL;

			pStall = FindStreetStall( pPlayer );
			if( pStall == NULL ) goto ITEMLOCKFAIL;

			pInfo = pStall->GetCellInfo( msg->wAbsPosition );
			if( pInfo->sItemBase.dwDBIdx != msg->ItemInfo.dwDBIdx ) goto ITEMLOCKFAIL;

			// 081013 KTH -- ���� �ݾ��� 40���� ���� ���ϰ� �Ѵ�.
			unsigned long long nTotalMoney = msg->dwMoney;
			
			//if( msg->ItemInfo.Durability != 0 )
			if( ITEMMGR->GetItemInfo(msg->ItemInfo.wIconIdx)->Stack )
				nTotalMoney = nTotalMoney * msg->ItemInfo.Durability;

			nTotalMoney += pPlayer->GetMoney();
			
			for( POSTYPE i = 0; i < MAX_STREETSTALL_CELLNUM; ++i )
			{
				WORD wDurability = 0;
				pInfo = pStall->GetCellInfo(i);
				
				if( pInfo == NULL )
					continue;

				
				//if( !ITEMMGR->GetItemInfo(pInfo->sItemBase.wIconIdx)->Stack )
				if( ITEMMGR->GetItemInfo(pInfo->sItemBase.wIconIdx) )
				{
					if( !ITEMMGR->GetItemInfo(pInfo->sItemBase.wIconIdx)->Stack )
						wDurability = 1;
					else
						wDurability = WORD( pInfo->sItemBase.Durability );
				}

				nTotalMoney += pInfo->dwMoney * wDurability;

				if( nTotalMoney > MAX_INVENTORY_MONEY )
				{
					erroflag = 2;
					goto ITEMLOCKFAIL;
				}
			}

			pStall->SetMoney( msg->wAbsPosition, msg->dwMoney );
			pStall->SetVolume( msg->wAbsPosition, msg->wVolume );
			pStall->ChangeCellState( msg->wAbsPosition, TRUE );

			Stemp = *msg;
			Stemp.Protocol = MP_STREETSTALL_LOCKITEM_ACK;
			pPlayer->SendMsg( &Stemp, sizeof(MSG_STREETSTALL_ITEMSTATUS) );

			// �մ��� ���¸� �����Ѵ�.
			Stemp.Protocol = MP_STREETSTALL_LOCKITEM;
			pStall->SendMsgGuestAll( &Stemp, sizeof(MSG_STREETSTALL_ITEMSTATUS) );

			return;

ITEMLOCKFAIL:
			/*Ftemp.Category = MP_STREETSTALL;
			Ftemp.Protocol = MP_STREETSTALL_LOCKITEM_NACK;
			Ftemp.dwObjectID = msg->dwObjectID;
			pPlayer->SendMsg( &Ftemp, sizeof(MSGBASE) );*/

			// 081013 KTH -- �������� ��� ���и� �˷��ش�.
			MSG_DWORD2 RegistFail;

			RegistFail.Category = MP_STREETSTALL;
			RegistFail.Protocol = MP_STREETSTALL_FAKEREGISTITEM_NACK;
			RegistFail.dwObjectID = msg->dwObjectID;
			RegistFail.dwData1 = msg->ItemInfo.Position;
			RegistFail.dwData2 = erroflag;
			
			pPlayer->SendMsg( &RegistFail, sizeof(MSG_DWORD2) );

			ITEM_INFO* pItemKindInfo = ITEMMGR->GetItemInfo( msg->ItemInfo.wIconIdx );
			if( !pItemKindInfo )	return;

			eITEMTABLE Table = eItemTable_Inventory;	

			// 081016 LUJ, ������ ���� ���¿��� goto������ �� ��찡 �ִ�
			if( ! pStall )
			{
				break;
			}

			if(pStall->GetStallKind() == eSK_SELL)
				pStall->EmptyCell( &msg->ItemInfo, Table );
			else if(pStall->GetStallKind() == eSK_BUY)
				pStall->EmptyCell( msg->wAbsPosition );


			// �޼��� ������.
			MSG_LINKITEM DelSyn;
			DelSyn.Category = MP_STREETSTALL;
			DelSyn.Protocol = MP_STREETSTALL_DELETEITEM_ACK;
			DelSyn.dwObjectID = msg->dwObjectID;
			DelSyn.ItemInfo = msg->ItemInfo;
			DelSyn.wAbsPosition = msg->wAbsPosition;
			pPlayer->SendMsg( &DelSyn, sizeof(MSG_LINKITEM) );

			// �մԿ��Լ� ��ǰ�� ���� �Ѵ�.
			DelSyn.Protocol = MP_STREETSTALL_DELETEITEM;
			pStall->SendMsgGuestAll( &DelSyn, sizeof(MSG_LINKITEM) );
		}
		break;
	case MP_STREETSTALL_UNLOCKITEM_SYN:
		{
			MSG_STREETSTALL_ITEMSTATUS* msg = (MSG_STREETSTALL_ITEMSTATUS*)pMsg;

			CPlayer* pPlayer = NULL;
			cStreetStall* pStall = NULL;
			sCELLINFO* pInfo = NULL;

			MSGBASE Ftemp;
			MSG_STREETSTALL_ITEMSTATUS Stemp;

			// 1. ������ ã�Ƽ� �������� UnLock �Ѵ�.
			pPlayer = (CPlayer*)g_pUserTable->FindUser( msg->dwObjectID );
			if( !pPlayer ) 
			{
				ASSERT(0);
				return;
			}

			if( msg->wAbsPosition >= MAX_STREETSTALL_CELLNUM ) goto ITEMUNLOCKFAIL;

			pStall = FindStreetStall( pPlayer );
			if( pStall == NULL )  goto ITEMUNLOCKFAIL;

			pInfo = pStall->GetCellInfo( msg->wAbsPosition );
			if( pInfo->sItemBase.dwDBIdx != msg->ItemInfo.dwDBIdx) goto ITEMUNLOCKFAIL;

			pStall->SetMoney( msg->wAbsPosition, 0 );
			pStall->SetVolume( msg->wAbsPosition, 0 );
			pStall->ChangeCellState( msg->wAbsPosition, FALSE );
				
			Stemp = *msg;
			Stemp.Protocol = MP_STREETSTALL_UNLOCKITEM_ACK;
			pPlayer->SendMsg( &Stemp, sizeof(MSG_STREETSTALL_ITEMSTATUS) );
				
			// �մ��� ���¸� �����Ѵ�.
			Stemp.Protocol = MP_STREETSTALL_UNLOCKITEM;
			pStall->SendMsgGuestAll( &Stemp, sizeof(MSG_STREETSTALL_ITEMSTATUS) );

			return;

ITEMUNLOCKFAIL:
			Ftemp.Category = MP_STREETSTALL;
			Ftemp.Protocol = MP_STREETSTALL_UNLOCKITEM_NACK;
			Ftemp.dwObjectID = msg->dwObjectID;
			pPlayer->SendMsg( &Ftemp, sizeof(MSGBASE) );
		}
		break;

	case MP_STREETSTALL_FAKEREGISTITEM_SYN:
		{
			MSG_REGISTITEMEX* msg = (MSG_REGISTITEMEX*)pMsg;
			cStreetStall* pStall = NULL;
			CPlayer* pPlayer = NULL;
			CItemSlot* pSlot = NULL;
			ITEMBASE pBase;
			DWORD erroflag = 0;			
			// RaMa
			eITEMTABLE Table = eItemTable_Inventory;
			ITEM_INFO* pItemKindInfo = NULL;
			
			MSG_DWORD2 RegistFail;
			MSG_LINKITEMEX RegistSyn;
			SEND_LINKITEM_TOTALINFO RegistGuest;
			ZeroMemory( &RegistGuest, sizeof( RegistGuest ) );

			// 1. ������ ã�Ƽ� �������� ÷���Ѵ�.
			pPlayer = (CPlayer*)g_pUserTable->FindUser( msg->dwObjectID );
			if( !pPlayer )
			{
				ASSERT(0);
				return;
			}
			
			if( pPlayer->GetGuestStall() ) goto ITEMREGISTFAILEX; // Guest�� ������ ��쿡 ���� ���� ó��
			
			pStall = FindStreetStall( pPlayer );
			if( pStall == NULL ) goto ITEMREGISTFAILEX;
			if( pStall->IsFull() ) goto ITEMREGISTFAILEX; // ������ �������..

			if(pStall->GetStallKind() == eSK_BUY)	goto ITEMREGISTFAILEX;
			
			// RaMa - ShopItemInven
			pItemKindInfo = ITEMMGR->GetItemInfo( msg->ItemInfo.wIconIdx );
			if( !pItemKindInfo )	goto ITEMREGISTFAILEX;

			pSlot = (CInventorySlot*)pPlayer->GetSlot( eItemTable_Inventory );
			
			if( !pSlot ) goto ITEMREGISTFAILEX;
			if( Table==eItemTable_Inventory && pSlot->IsLock((POSTYPE)msg->ItemInfo.Position) ) goto ITEMREGISTFAILEX;
			
			if( !CHKRT->ItemOf( pPlayer, msg->ItemInfo.Position, msg->ItemInfo.wIconIdx,
				msg->ItemInfo.Durability, msg->ItemInfo.QuickPosition ) )
				goto ITEMREGISTFAILEX;

			if( pSlot->GetItemInfoAbs( msg->ItemInfo.Position ) == NULL )
				goto ITEMREGISTFAILEX;

			if( pSlot->GetItemInfoAbs( msg->ItemInfo.Position )->QuickPosition != 0 )
				goto ITEMREGISTFAILEX;
			
			pBase =  *pSlot->GetItemInfoAbs((POSTYPE)msg->ItemInfo.Position);

			// 071125 StreetStallManager - �������� ���� Ǯ������ ������ ����Ҽ� ����.
			if( pBase.nSealed == eITEM_TYPE_UNSEAL )
			{
				goto ITEMREGISTFAILEX;
			}

			// 080204 KTH -- ��ȯ�� �������� ���� ������ ó��.
			if( !pItemKindInfo->Trade )
			{
				goto ITEMREGISTFAILEX;
			}

			if( !pStall->CheckItemDBIdx(pBase.dwDBIdx) ) 
			{
				erroflag = 1;
				goto ITEMREGISTFAILEX; // �̹� ��ϵǾ� �ִ� ����������...
			}
			
			//			if( !pStall->FillCell( &pBase, msg->dwMoney, TRUE ) ) goto ITEMREGISTFAILEX;
			if( !pStall->FillCell( &pBase, msg->dwMoney, FALSE ) ) goto ITEMREGISTFAILEX;

			RegistSyn.Category = MP_STREETSTALL;
			RegistSyn.Protocol = MP_STREETSTALL_FAKEREGISTITEM_ACK;
			RegistSyn.dwObjectID = msg->dwObjectID;
			RegistSyn.ItemInfo = pBase;
			RegistSyn.dwMoney = msg->dwMoney;
			
			pPlayer->SendMsg( &RegistSyn, sizeof(MSG_LINKITEMEX) );
			
			// �մԿ��� ��ǰ�� ������ش�.

			RegistGuest.Category = MP_STREETSTALL;
			RegistGuest.Protocol = MP_STREETSTALL_FAKEREGISTITEM;
			RegistGuest.dwObjectID = msg->dwObjectID;
			RegistGuest.ItemInfo = pBase;
			RegistGuest.dwMoney = msg->dwMoney;
			RegistGuest.PetInfoCount = 0;
			RegistGuest.sOptionInfo = ITEMMGR->GetOption( pBase );
			
			pStall->SendMsgGuestAll( &RegistGuest, sizeof( RegistGuest ) );
			
			return;
			
ITEMREGISTFAILEX:
			RegistFail.Category = MP_STREETSTALL;
			RegistFail.Protocol = MP_STREETSTALL_FAKEREGISTITEM_NACK;
			RegistFail.dwObjectID = msg->dwObjectID;
			RegistFail.dwData1 = msg->ItemInfo.Position;
			RegistFail.dwData2 = erroflag;
			
			pPlayer->SendMsg( &RegistFail, sizeof(MSG_DWORD2) );
			
			return;
		}
		break;
	case MP_STREETSTALL_FAKEREGISTBUYITEM_SYN:
		{
			MSG_REGISTBUYITEMEX* msg = (MSG_REGISTBUYITEMEX*)pMsg;
			cStreetStall* pStall = NULL;
			CPlayer* pPlayer = NULL;
			CItemSlot* pSlot = NULL;
			ITEMBASE pBase;
			sCELLINFO* pInfo = NULL;
			DWORD erroflag = 0;			
			ITEM_INFO* pItemKindInfo = NULL;
			//DWORD TotalMoney = 0;
			//090818 pdy ���Ż��� ��ǰ��Ͻ� ���հ谡 40���� ������ ��Ͼȵǰ� ó�� 
			unsigned long long TotalMoney;
			
			MSG_DWORD2 RegistFail;
			MSG_LINKBUYITEMEX RegistSyn;
			SEND_LINKBUYITEM_TOTALINFO RegistGuest;
			
			// 1. ������ ã�Ƽ� �������� ÷���Ѵ�.
			pPlayer = (CPlayer*)g_pUserTable->FindUser( msg->dwObjectID );
			if( !pPlayer )
			{
				ASSERT(0);
				return;
			}
			
			if( pPlayer->GetGuestStall() ) goto BUYITEMREGISTFAILEX; // Guest�� ������ ��쿡 ���� ���� ó��
			
			pStall = FindStreetStall( pPlayer );
			if( pStall == NULL ) goto BUYITEMREGISTFAILEX;
			if( pStall->IsFull() ) goto BUYITEMREGISTFAILEX; // ������ �������..
			
			if(pStall->GetStallKind() == eSK_SELL) goto BUYITEMREGISTFAILEX;

			//////////////////////////////////////////////////////////////////////////
			//����ϰ��� �ϴ� ���� ���� �κ��� ��� �Ұ����� ���϶�
//			if(msg->wAbsPosition >= pStall->GetUsable()) goto BUYITEMREGISTFAILEX;
			//////////////////////////////////////////////////////////////////////////
			
			//���԰����� �������� �ʰ��Ҷ�

			pInfo = pStall->GetCellInfo(msg->wAbsPosition);

			if(!pInfo) goto BUYITEMREGISTFAILEX;

			TotalMoney = pStall->GetTotalMoney();

			if(pInfo->bFill)	// ���� �������� ��� �Ǿ��ִٸ� ���� ��ϵ� ������ ����
				TotalMoney -= (pInfo->dwMoney * pInfo->wVolume);

			//090818 pdy ���Ż��� ��ǰ��Ͻ� ���հ谡 40���� ������ ��Ͼȵǰ� ó�� 
			unsigned long long MsgMoney = unsigned long long( msg->dwMoney) * unsigned long long(msg->wVolume);
			//if(pPlayer->GetMoney() < TotalMoney + (msg->dwMoney * msg->wVolume)) 

			if( TotalMoney + MsgMoney > MAX_INVENTORY_MONEY )
			{
				erroflag = 1;
				pInfo->bLock = FALSE;
				goto BUYITEMREGISTFAILEX;
			}

			if(pPlayer->GetMoney() < TotalMoney + MsgMoney )
			{
				erroflag = 1;
				pInfo->bLock = FALSE;
				goto BUYITEMREGISTFAILEX;
			}
			
			//////////////////////////////////////////////////////////////////////////
			// ���ŵ���� �����ۿ� �ش��ϴ� �κ��� ������ �ִ��� �Ǵ�
			WORD EmptyCellPos;

			pItemKindInfo = ITEMMGR->GetItemInfo( msg->ItemInfo.wIconIdx );
			
			if( !pItemKindInfo )	goto BUYITEMREGISTFAILEX;

			pSlot = (CInventorySlot*)pPlayer->GetSlot( eItemTable_Inventory );
			
			if( pSlot->GetEmptyCell(&EmptyCellPos) == 0 ) 
			{
				erroflag = 2;
				pInfo->bLock = FALSE;
				goto BUYITEMREGISTFAILEX;
			}
			//////////////////////////////////////////////////////////////////////////

			
			pBase = msg->ItemInfo;
			
			if( !pStall->FillCell( &pBase, msg->dwMoney, FALSE, msg->wVolume, msg->wAbsPosition ) )  goto BUYITEMREGISTFAILEX;

			RegistSyn.Category = MP_STREETSTALL;	
			RegistSyn.Protocol = MP_STREETSTALL_FAKEREGISTBUYITEM_ACK;
			RegistSyn.dwObjectID = msg->dwObjectID;
			RegistSyn.ItemInfo = pBase;
			RegistSyn.dwMoney = msg->dwMoney;
			RegistSyn.wVolume = msg->wVolume;
			RegistSyn.wAbsPosition = msg->wAbsPosition;
			
			pPlayer->SendMsg( &RegistSyn, sizeof(MSG_LINKBUYITEMEX) );
			
			// �մԿ��� ��ǰ�� ������ش�.
			
			RegistGuest.Category = MP_STREETSTALL;
			RegistGuest.Protocol = MP_STREETSTALL_FAKEREGISTBUYITEM;
			RegistGuest.dwObjectID = msg->dwObjectID;
			RegistGuest.ItemInfo = pBase;
			RegistGuest.dwMoney = msg->dwMoney;
			//RegistGuest.count = 0;;
			//RegistGuest.RareCount = 0;
			RegistGuest.wVolume = msg->wVolume;
			RegistGuest.wAbsPosition = msg->wAbsPosition;
			RegistGuest.sOptionInfo = ITEMMGR->GetOption( pBase );
			
			pStall->SendMsgGuestAll( &RegistGuest, sizeof(SEND_LINKBUYITEM_TOTALINFO) );

			return;
BUYITEMREGISTFAILEX:
			RegistFail.Category = MP_STREETSTALL;
			RegistFail.Protocol = MP_STREETSTALL_FAKEREGISTBUYITEM_NACK;
			RegistFail.dwObjectID = msg->dwObjectID;
			RegistFail.dwData1 = msg->ItemInfo.Position;
			RegistFail.dwData2 = erroflag;
			
			pPlayer->SendMsg( &RegistFail, sizeof(MSG_DWORD2) );

			//090819 pdy ���ž����� ������Ʈ�� �����ϸ� �ٸ�Ŭ���̾�Ʈ ��Ͽ��� �������� �ʴ� ���� ���� 
			if( pInfo->bFill )
			{
				// ���� �������� ��� �Ǿ��ִ� ���¿��� ���ж�� �ٸ� Ŭ���̾�Ʈ�� MP_STREETSTALL_UPDATEEND�� �����ش�.
				MSGBASE success;
				success.Category = MP_STREETSTALL;
				success.dwObjectID = pPlayer->GetID();
				success.Protocol = MP_STREETSTALL_UPDATEEND;
				pStall->SendMsgGuestAll( &success, sizeof(MSGBASE) );
			}
			
			return;
			
		}
		break;
	case MP_STREETSTALL_DELETEITEM_SYN:
		{
			MSG_LINKITEM* msg = (MSG_LINKITEM*)pMsg;

			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser( msg->dwObjectID );
			if( !pPlayer ) break;

			cStreetStall* pStall = FindStreetStall( pPlayer );
			if( pStall == NULL ) break;

			// RaMa - ShopItemInven
			ITEM_INFO* pItemKindInfo = ITEMMGR->GetItemInfo( msg->ItemInfo.wIconIdx );
			if( !pItemKindInfo )	return;

			eITEMTABLE Table = eItemTable_Inventory;	

			if(pStall->GetStallKind() == eSK_SELL)
				pStall->EmptyCell( &msg->ItemInfo, Table );
			else if(pStall->GetStallKind() == eSK_BUY)
				pStall->EmptyCell( msg->wAbsPosition );


			// �޼��� ������.
			MSG_LINKITEM DelSyn;
			DelSyn.Category = MP_STREETSTALL;
			DelSyn.Protocol = MP_STREETSTALL_DELETEITEM_ACK;
			DelSyn.dwObjectID = msg->dwObjectID;
			DelSyn.ItemInfo = msg->ItemInfo;
			DelSyn.wAbsPosition = msg->wAbsPosition;
			pPlayer->SendMsg( &DelSyn, sizeof(MSG_LINKITEM) );

			// �մԿ��Լ� ��ǰ�� ���� �Ѵ�.
			DelSyn.Protocol = MP_STREETSTALL_DELETEITEM;
			pStall->SendMsgGuestAll( &DelSyn, sizeof(MSG_LINKITEM) );
		}
		break;
	case MP_STREETSTALL_BUYITEM_SYN:
		{
			STREETSTALL_BUYINFO* msg = (STREETSTALL_BUYINFO*)pMsg;
			CPlayer* pOwner = (CPlayer *)g_pUserTable->FindUser(msg->StallOwnerID);
			if(pOwner == NULL) return;

			CPlayer* pGuest = (CPlayer *)g_pUserTable->FindUser(msg->dwObjectID);
			if(pGuest == NULL) return;

			//---KES StreetStall Fix 071020
			//---�մ��� ���������� �� ����.
			if( FindStreetStall(pGuest) != NULL )
				goto BUYITEMFAILEX;
			//----------------------------

			cStreetStall* pStall = FindStreetStall( pOwner );
			if( pStall == NULL ) return;
			if( pGuest->GetGuestStall() == NULL ) 
			{
				char buf[128];
				sprintf(buf, "������ ���µ� �������� ����� �Ѵ�. %d", pGuest->GetID());
				ASSERTMSG(0, buf);

				goto BUYITEMFAILEX;
			}

			if(pGuest->GetGuestStall()->GetOwner()->GetID() != msg->StallOwnerID)
			{
				char buf[128];
				sprintf(buf, "��ϵ� �������� �ٸ� ���������������� ����� �Ѵ�: %d", pGuest->GetID());
				ASSERTMSG(0, buf);

				goto BUYITEMFAILEX;
			}
			
			if( pStall->CheckItemDBIdx(msg->ItemInfo.dwDBIdx) ) // ��Ͼ��������� üũ�Ѵ�.
			{
				MSG_WORD msgNack;
				msgNack.Category	= MP_STREETSTALL;
				msgNack.Protocol	= MP_STREETSTALL_BUYITEM_NACK;
				msgNack.dwObjectID	= msg->dwObjectID;
				msgNack.wData		= NOT_EXIST;
				pGuest->SendMsg( &msgNack, sizeof(msgNack) );

				return;
			}

			if( ITEMMGR->IsDupItem( msg->ItemInfo.wIconIdx ) )
			{
				BuyDupItem( pOwner, pStall, pGuest, msg );
			}
			else
			{
				BuyItem( pOwner, pStall, pGuest, msg );
			}

//---KES StreetStall Fix 071020
//---�ڵ� ����
			return;

BUYITEMFAILEX:
		MSG_WORD msgNack;
		msgNack.Category	= MP_STREETSTALL;
		msgNack.Protocol	= MP_STREETSTALL_BUYITEM_NACK;
		msgNack.dwObjectID	= msg->dwObjectID;
		msgNack.wData		= NOT_EXIST;
		pGuest->SendMsg( &msgNack, sizeof(msgNack) );
		return;
//-----------------------------
		}
		break;

	case MP_STREETSTALL_SELLITEM_SYN:
		{
			STREETSTALL_BUYINFO* msg = (STREETSTALL_BUYINFO*)pMsg;
			CPlayer* pOwner = NULL;
			CPlayer* pGuest = NULL;
			cStreetStall* pStall = NULL;

			pOwner = (CPlayer *)g_pUserTable->FindUser(msg->StallOwnerID);
			pGuest = (CPlayer *)g_pUserTable->FindUser(msg->dwObjectID);
			
			if( !pOwner || !pGuest ) goto SELLITEMFAILEX;

			pStall = FindStreetStall( pOwner );		
			
			if( !pStall ) goto SELLITEMFAILEX;
			
			if( !pGuest->GetGuestStall() ) 	goto SELLITEMFAILEX;

			if(pGuest->GetGuestStall()->GetOwner()->GetID() != msg->StallOwnerID)
				goto SELLITEMFAILEX;

			// 090114 LUJ, ��Ͼȵ� �������� ���� �õ���
            if( ! GAMERESRCMNGR->IsBuyItem( msg->ItemInfo.wIconIdx ) )
			{
				char text[ MAX_PATH ] = { 0 };
				sprintf(
					text,
					"item:%d",
					msg->ItemInfo.wIconIdx );
				LogScriptHack(
					*pOwner,
					"BRList.bin",
					text );
				goto SELLITEMFAILEX;
			}

			// 090114 LUJ, ��ȯ �Ұ� �������� ���� �������� ����� �� ����
			{
				const ITEM_INFO* const buyItemInfo = ITEMMGR->GetItemInfo( msg->ItemInfo.wIconIdx );
				
				if( ! buyItemInfo	||
					! buyItemInfo->Trade )
				{
					char text[ MAX_PATH ] = { 0 };
					sprintf(
						text,
						"item:%d",
						msg->ItemInfo.wIconIdx );
					LogScriptHack(
						*pOwner,
						"BRList.bin",
						text );
					goto SELLITEMFAILEX;
				}
			}

			if( !(pStall->GetCellInfo(msg->StallPos)->bFill)			// ������ ����� �ȵ� ������
				|| ( (pStall->GetCellInfo(msg->StallPos)->sItemBase.wIconIdx) != msg->ItemInfo.wIconIdx ) 
					// ������ �ε��� ����ġ
				|| (pStall->GetCellInfo(msg->StallPos)->wVolume < 1)	// ������ ��ϵ� ������ 0�� ����
				|| (pStall->GetCellInfo(msg->StallPos)->bLock) )		// ����ִ� ������
				goto SELLITEMFAILEX;

			if( ITEMMGR->IsDupItem( msg->ItemInfo.wIconIdx ) )
				SellDupItem( pOwner, pStall, pGuest, msg );
			else
				if( !SellItem( pOwner, pStall, pGuest, msg ) )
					goto SELLITEMFAILEX;

			return;

SELLITEMFAILEX:
			MSG_WORD2 msgNack;
			msgNack.Category	= MP_STREETSTALL;
			msgNack.Protocol	= MP_STREETSTALL_SELLITEM_NACK;
			msgNack.dwObjectID	= msg->dwObjectID;
			msgNack.wData1		= NOT_EXIST;
			msgNack.wData2		= msg->ItemInfo.Position;
			pGuest->SendMsg( &msgNack, sizeof(MSG_WORD2) );
			
			return;
		}
		break;

	case MP_STREETSTALL_GUESTIN_SYN:
		{
			MSG_DWORD* msg = (MSG_DWORD*)pMsg;
			BOOL bVal = TRUE;
			CPlayer* pOwner = (CPlayer*)g_pUserTable->FindUser( msg->dwData );

			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser( msg->dwObjectID );
			if( !pPlayer ) break;
			
			/* State�� ���� Check�� �Ѵ�. */
			//KES SHOWDOWN 031103 �߰�
			if( pPlayer->IsShowdown() ) bVal = FALSE;	//����
			if( pPlayer->GetExchangeContainer()->pRoom != NULL ) bVal = FALSE;	//��ȯ��
			if( (pPlayer->GetState() == eObjectState_StreetStall_Guest) || (pPlayer->GetState() == eObjectState_StreetStall_Owner))
			{
				char buf[128];
				sprintf(buf, "���������� �̹� ���� ����: %d", pPlayer->GetID());
				ASSERTMSG(0, buf);
				SendNackMsg(pPlayer, MP_STREETSTALL_GUESTIN_NACK);
				return;
			}
				
			cStreetStall* pStall = NULL;

			if( !pOwner )
			{
				bVal = FALSE;
			}
			else if( bVal )
			{
				pStall = FindStreetStall( pOwner );
				if( !pStall ) 
				{
					bVal = FALSE;
				}
				else
				{
					ASSERT( pPlayer->GetGuestStall() == NULL );	// null�̾���Ѵ�.

					if( pPlayer->GetGuestStall() )
					{
						pPlayer->GetGuestStall()->DeleteGuest( pPlayer );
					}
					

					pStall->AddGuest( pPlayer );
				}
			}

			if( bVal )
			{
				STREETSTALL_INFO sInfo;
				memset(&sInfo, 0, sizeof(STREETSTALL_INFO));
				pStall->GetStreetStallInfo( sInfo );
				sInfo.Category = MP_STREETSTALL;
				sInfo.Protocol = MP_STREETSTALL_GUESTIN_ACK;
				sInfo.StallKind = pStall->GetStallKind();
				sInfo.dwObjectID = msg->dwObjectID;
				sInfo.StallOwnerID = msg->dwData;

				OBJECTSTATEMGR_OBJ->StartObjectState(pPlayer, eObjectState_StreetStall_Guest, 0);

				pPlayer->SendMsg( &sInfo, sInfo.GetSize() );
			}
			else
			{
				SendNackMsg(pPlayer, MP_STREETSTALL_GUESTIN_NACK);
			}
		}
		break;
	case MP_STREETSTALL_GUESTOUT_SYN:
		{
			MSG_DWORD* msg = (MSG_DWORD*)pMsg;
			CPlayer* pOwner = (CPlayer*)g_pUserTable->FindUser( msg->dwData );
			if( !pOwner ) break;

			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser( msg->dwObjectID );
			if( !pPlayer ) break;
			cStreetStall * pPlayerStall = pPlayer->GetGuestStall();
			if(!pPlayerStall)
			{
				char buf[128];
				sprintf(buf, "�÷��̾ gueststall�� ���µ� ������ �޽��� ��: %d", pPlayer->GetID());
				ASSERTMSG(0, buf);
				SendNackMsg(pPlayer, MP_STREETSTALL_GUESTOUT_NACK);
				if(pPlayer->GetState() == eObjectState_StreetStall_Guest)
				{				
					char buf[128];
					sprintf(buf, "�÷��̾ gueststall�� ���µ� ���´� ���� ����: %d", pPlayer->GetID());
					ASSERTMSG(0, buf);
				}
				return;
			}
			if(pPlayerStall->GetOwner()->GetID() != pOwner->GetID())
			{
				char buf[128];
				sprintf(buf, "������ �ִ� �������� �ٸ� �������� �����ٰ� �޽��� ��: %d", pPlayer->GetID());
				ASSERTMSG(0, buf);
				return;
			}
			if( pPlayer->GetState() != eObjectState_StreetStall_Guest)
			{
				char buf[128];
				sprintf(buf, "�������� �մԻ��°� �ƴѵ� ������ �޽��� ��: %d", pPlayer->GetID());
				ASSERTMSG(0, buf);
				SendNackMsg(pPlayer, MP_STREETSTALL_GUESTOUT_NACK);
				return;
			}

			OBJECTSTATEMGR_OBJ->EndObjectState(pPlayer, eObjectState_StreetStall_Guest);
			cStreetStall* pStall = FindStreetStall( pOwner );
			if( pStall )
				pStall->DeleteGuest( pPlayer );

			MSGBASE OutSyn;
			OutSyn.Category = MP_STREETSTALL;
			OutSyn.Protocol = MP_STREETSTALL_GUESTOUT_ACK;
			OutSyn.dwObjectID = msg->dwObjectID;

			pPlayer->SendMsg( &OutSyn, sizeof(MSGBASE) );
		}
		break;
	case MP_STREETSTALL_EDITTITLE_SYN:
		{
			MSG_STREETSTALL_TITLE* msg = (MSG_STREETSTALL_TITLE*)pMsg;

			// 1. ������ ã�Ƽ� Ÿ��Ʋ�� �����Ѵ�.
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser( msg->dwObjectID );
			if( !pPlayer ) break;

			cStreetStall* pStall = FindStreetStall( pPlayer );

			if( pStall == NULL )
			{
				MSGBASE temp;
				temp.Category = MP_STREETSTALL;
				temp.dwObjectID = msg->dwObjectID;
				temp.Protocol = MP_STREETSTALL_EDITTITLE_NACK;

				pPlayer->SendMsg( &temp, sizeof(MSGBASE) );
			}
			else
			{
				pPlayer->SetStreetStallTitle( msg->Title );

				MSG_STREETSTALL_TITLE temp;
				temp.Category = MP_STREETSTALL;
				temp.dwObjectID = msg->dwObjectID;
				temp.StallKind = msg->StallKind;
				SafeStrCpy( temp.Title, msg->Title, MAX_STREETSTALL_TITLELEN+1 );
				temp.Protocol = MP_STREETSTALL_EDITTITLE_ACK;
				pPlayer->SendMsg( &temp, sizeof(MSG_STREETSTALL_TITLE) );

				// �մԿ��� Ÿ��Ʋ�� �˸���.
				temp.Protocol = MP_STREETSTALL_EDITTITLE;
				PACKEDDATA_OBJ->QuickSendExceptObjectSelf(pPlayer,&temp,sizeof(MSG_STREETSTALL_TITLE));
			}
		}
		break;
	case MP_STREETSTALL_UPDATE_SYN:
		{
			MSG_WORD* msg = (MSG_WORD*)pMsg;
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser( msg->dwObjectID );
			if( !pPlayer ) break;
			
			cStreetStall* pStall = FindStreetStall( pPlayer );
			if( !pStall ) break;
			
			
			sCELLINFO* pCellInfo = pStall->GetCellInfo( msg->wData );
			
			if(( pCellInfo ) && ( pCellInfo->bFill ) && ( !pCellInfo->bLock ))
			{
				pCellInfo->bLock = TRUE;

				MSG_WORD success;

				success.Category = MP_STREETSTALL;
				success.Protocol = MP_STREETSTALL_UPDATE_ACK;
				success.dwObjectID = pPlayer->GetID();
				success.wData = msg->wData;
								
				pPlayer->SendMsg( &success, sizeof(MSG_WORD) );

				success.Protocol = MP_STREETSTALL_UPDATE;

				pStall->SendMsgGuestAll( &success, sizeof(MSG_WORD) );
			}
			else
			{
				MSG_WORD fail;

				fail.Category = MP_STREETSTALL;
				fail.Protocol = MP_STREETSTALL_UPDATE_NACK;
				fail.dwObjectID = pPlayer->GetID();
				fail.wData = msg->wData;

				pPlayer->SendMsg( &fail, sizeof(MSG_WORD) );
			}
		}
		break;
	case MP_STREETSTALL_UPDATEEND_SYN:
		{
			MSGBASE* msg = (MSGBASE*)pMsg;
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser( msg->dwObjectID );
			if( !pPlayer ) break;
			
			cStreetStall* pStall = FindStreetStall( pPlayer );
			if( !pStall ) break;
			
			for(POSTYPE i = 0; i < SLOT_STREETBUYSTALL_NUM; i++)
			{
				sCELLINFO* pCellInfo = pStall->GetCellInfo(i);
				
				if(pCellInfo)
					pCellInfo->bLock = FALSE;
			}
			
			MSGBASE success;

			success.Category = MP_STREETSTALL;
			success.dwObjectID = pPlayer->GetID();
			
			success.Protocol = MP_STREETSTALL_UPDATEEND;

			pStall->SendMsgGuestAll( &success, sizeof(MSGBASE) );

			success.Protocol = MP_STREETSTALL_UPDATEEND_ACK;

			pPlayer->SendMsg( &success, sizeof(MSGBASE) );
		}
		break;
	}
}

void cStreetStallManager::SendNackMsg(CPlayer* pPlayer, BYTE Protocol)
{
	MSGBASE nmsg;
	nmsg.Category = MP_STREETSTALL;
	nmsg.Protocol = Protocol;
	pPlayer->SendMsg(&nmsg, sizeof(nmsg));
}


BOOL cStreetStallManager::CanBuyItem(CPlayer* pOwner, CPlayer *pGuest, sCELLINFO* pItemInfo, WORD* EmptyCellPos, STREETSTALL_BUYINFO* pBuyInfo, ITEMBASE* pStallItem, DWORD dwBuyNum )
{
	//KES: �������� ���� ��ȭ
	if( pItemInfo->dwMoney != pBuyInfo->dwBuyPrice )
	{
		AssertBuyErr(NOT_SAMEPRICE);
		return FALSE;
	}

	// �������� ���� üũ
	if( !pItemInfo || !pItemInfo->bFill || !pItemInfo->bLock || pStallItem->dwDBIdx != pBuyInfo->ItemInfo.dwDBIdx && pStallItem->wIconIdx != pBuyInfo->ItemInfo.wIconIdx )
	{
		AssertBuyErr(NOT_EXIST);
		return FALSE;
	}

	// HAVE MONEY ?
	if( pGuest->GetMoney() < pItemInfo->dwMoney * dwBuyNum )
	{
		AssertBuyErr(NOT_MONEY);
		return FALSE;
	}

	ITEM_INFO* pItemKindInfo = ITEMMGR->GetItemInfo( pItemInfo->sItemBase.wIconIdx );
	if( !pItemKindInfo )	return FALSE;

	CItemSlot* pOwnerInventory	= pOwner->GetSlot( eItemTable_Inventory );
	CItemSlot* pGuestInventory	= pGuest->GetSlot( eItemTable_Inventory );

	const ITEMBASE* pRealItem = pOwnerInventory->GetItemInfoAbs( pStallItem->Position );
	if( pRealItem == NULL )
	{
		AssertBuyErr(NOT_EXIST);
		return FALSE;
	}
	if( pRealItem->Durability	!= pStallItem->Durability ||
		pRealItem->dwDBIdx		!= pStallItem->dwDBIdx ||
		pRealItem->wIconIdx		!= pStallItem->wIconIdx )
	{
		AssertBuyErr(NOT_EXIST);
		return FALSE;
	}
	//-------------------------------------


	// �κ��丮���� üũ
	if( pGuestInventory->GetEmptyCell(EmptyCellPos) == 0 ) 
	{
		AssertBuyErr(NOT_SPACE);
		return FALSE;
	}
	if( !pOwner->IsEnoughAdditionMoney(pItemInfo->dwMoney) )
	{
		AssertBuyErr(NOT_OWNERMONEY);
		
		MSG_WORD msg;
		msg.Category = MP_STREETSTALL;
		msg.Protocol = MP_STREETSTALL_SELLITEM_ERROR;
		msg.wData = pBuyInfo->StallPos;
		pOwner->SendMsg(&msg, sizeof(msg));

		return FALSE;
	}

	return TRUE;
}

BOOL cStreetStallManager::CanSellItem(CPlayer* pOwner, CPlayer *pGuest, sCELLINFO* pItemInfo, WORD* EmptyCellPos, STREETSTALL_BUYINFO* pSellInfo, ITEMBASE* pSellItem, DWORD dwSellNum, WORD& result )
{
	//KES: �������� ���� ��ȭ
	if( pItemInfo->dwMoney != pSellInfo->dwBuyPrice )
	{
		AssertSellErr(NOT_SAMEPRICE);
		return FALSE;
	}
	
	// �������� ���� üũ
	if( !pItemInfo || !pItemInfo->bFill || pItemInfo->bLock || pSellItem->wIconIdx != pItemInfo->sItemBase.wIconIdx || pItemInfo->wVolume < 1 )
	{
		AssertSellErr(NOT_EXIST);
		result = NOT_EXIST;

		return FALSE;
	}
	
	// HAVE MONEY ?
	if( pOwner->GetMoney() < pItemInfo->dwMoney * dwSellNum )
	{
		AssertSellErr(NOT_MONEY);
		result = NOT_MONEY;

		MSG_WORD msg;
		msg.Category = MP_STREETSTALL;
		msg.Protocol = MP_STREETSTALL_BUYITEM_ERROR;
		msg.wData = NOT_MONEY;
		pOwner->SendMsg(&msg, sizeof(msg));
			
		return FALSE;
	}

	DWORDEX dwGuestMoney = (DWORDEX)pGuest->GetMoney();
	DWORDEX ItemMoney = (DWORDEX)(pItemInfo->dwMoney * dwSellNum);
	DWORDEX dwCheckMoney = dwGuestMoney + ItemMoney;

	if( dwCheckMoney > MAX_INVENTORY_MONEY)
	{
		result = OVER_INVENMONEY;
		return FALSE;
	}

	ITEM_INFO* pItemKindInfo = ITEMMGR->GetItemInfo( pItemInfo->sItemBase.wIconIdx );
	if( !pItemKindInfo )	return FALSE;
	
	CItemSlot* pOwnerInventory	= pOwner->GetSlot( eItemTable_Inventory );
	CItemSlot* pGuestInventory	= pGuest->GetSlot( eItemTable_Inventory );

	const ITEMBASE* pRealItem = pGuestInventory->GetItemInfoAbs( pSellItem->Position );
	if( pRealItem == NULL )
	{
		AssertBuyErr(NOT_EXIST);
		return FALSE;
	}
	// �̰� ����? ���� �ִ� �����ϰ� �ٸ��� FALSE???
	if( pRealItem->Durability	!= pSellItem->Durability ||
		pRealItem->dwDBIdx		!= pSellItem->dwDBIdx ||
		pRealItem->wIconIdx		!= pSellItem->wIconIdx )
	{
		AssertBuyErr(NOT_EXIST);
		return FALSE;
	}
	//-------------------------------------

	// �κ��丮���� üũ
	if( pOwnerInventory->GetEmptyCell(EmptyCellPos) == 0 ) 
	{
		AssertSellErr(NOT_SPACE);
		result = NOT_SPACE;

		MSG_WORD msg;
		msg.Category = MP_STREETSTALL;
		msg.Protocol = MP_STREETSTALL_BUYITEM_ERROR;
		msg.wData = NOT_SPACE;
		pOwner->SendMsg(&msg, sizeof(msg));
		
		return FALSE;
	}

	pItemInfo->bLock = TRUE;
	
	return TRUE;
}

void cStreetStallManager::AssertBuyErr(WORD Err)
{
#ifdef _DEBUG
	OutputDebug(
		"���� ��� ����: %d",
		Err);
#endif
}

void cStreetStallManager::AssertSellErr(WORD Err)
{
#ifdef _DEBUG
	OutputDebug(
		"���� �ȱ� ����: %d",
		Err);
#endif
}