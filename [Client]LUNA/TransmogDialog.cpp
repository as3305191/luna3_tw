#include "stdafx.h"
#include "WindowIDEnum.h"
#include "ChatManager.h"
#include "interface/cIconDialog.h"
#include "Interface/cScriptManager.h"
#include "Interface/cWindowManager.h"
#include "interface/cStatic.h"
#include "InventoryExDialog.h"
#include "Item.h"
#include "VirtualItem.h"
#include "ItemManager.h"
#include "TransmogDialog.h"
#include "ObjectManager.h"
#include "ProgressDialog.h"
#include "FishingManager.h"
#include "GameIn.h"
#include "FishingDialog.h"
#include "FishingPointDialog.h"
#include "input/Mouse.h"
#include "CommonCalcFunc.h"

 
CTransmogDialog::CTransmogDialog() :
mSourceItem( new CVirtualItem ),
mDummyItem( new CVirtualItem ), 
mIconDialog( 0 )
{
	cImage image;
	SCRIPTMGR->GetImage( 0, &image, PFT_HARDPATH );
	

	ITEMBASE nBase;
	ZeroMemory( &nBase, sizeof( nBase ) );

	mMaterialItem = new CItem( &nBase );
	mMaterialItem->SetToolTip( "", RGB_HALF( 255, 255, 255), &image );
	mMaterialItem->SetMovable( FALSE );
	
	mSourceItem->SetToolTip( "", RGB_HALF( 255, 255, 255), &image );
	mSourceItem->SetMovable(FALSE);

	mDummyItem->SetToolTip( "", RGB_HALF( 255, 255, 255), &image );
	mDummyItem->SetMovable(FALSE);

	m_pTransmogCost = NULL;
	transmogItemIdx = 60000017;
}

CTransmogDialog::~CTransmogDialog()
{
	SAFE_DELETE( mSourceItem )
	SAFE_DELETE( mDummyItem );
	SAFE_DELETE( mMaterialItem );
}

void CTransmogDialog::Linking()
{
	mIconDialog		= ( cIconDialog* )	GetWindowForID( ITTD_TRANSMOGICONDLG );
	m_pTransmogCost	= (cStatic*)GetWindowForID( ITTD_TRANSMOGCOST );
}

BOOL CTransmogDialog::isTransmogable(CItem* item)
{
	const DWORD			itemIndex	= item->GetItemIdx();
	const ITEM_INFO*	info		= ITEMMGR->GetItemInfo( itemIndex );

	if(info && (info->EquipSlot == eWearedItem_Costume_Dress 
		|| info->EquipSlot == eWearedItem_Costume_Glove 
		|| info->EquipSlot == eWearedItem_Costume_Head
		|| info->EquipSlot == eWearedItem_Costume_Shoes 
		|| info->EquipSlot == eWearedItem_Shield 
		|| info->EquipSlot == eWearedItem_Weapon 
		|| info->EquipSlot == eWearedItem_Dress
		|| info->EquipSlot == eWearedItem_Hat
		|| info->EquipSlot == eWearedItem_Glove
		|| info->EquipSlot == eWearedItem_Shoes
		|| info->EquipSlot == eWearedItem_Wing 
		|| info->EquipSlot == eWearedItem_Glasses
		|| info->EquipSlot == eWearedItem_Mask
		|| info->EquipSlot == eWearedItem_Costume_Wing
		|| info->EquipSlot == eWearedItem_Band
		))
	{
		
		return TRUE;
	}

	return FALSE;

}

BOOL CTransmogDialog::comparePart(const ITEM_INFO* firstInfo , const ITEM_INFO* secondInfo)
{
	if(firstInfo->EquipSlot != secondInfo->EquipSlot)
	{
		CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 2484 ) );
		return FALSE;
	}
	if(firstInfo->EquipSlot == eWearedItem_Weapon && secondInfo->EquipSlot == eWearedItem_Weapon)
	{
		if(firstInfo->WeaponType != secondInfo->WeaponType)
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 2484 ) );
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CTransmogDialog::FakeMoveIcon(LONG x, LONG y, cIcon* icon)
{
	if( WT_ITEM != icon->GetType() ||
		icon->IsLocked()			||
		m_bDisable )
	{
		return FALSE;
	}

	CItem* item = (CItem*)icon;

	if(!ITEMMGR->IsEqualTableIdxForPos(eItemTable_Inventory, item->GetPosition() ) )
	{
		CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 787 ) ); 
		return FALSE;
	}
	
	const DWORD				itemIndex	= item->GetItemIdx();
	const ITEM_INFO*		info		= ITEMMGR->GetItemInfo( itemIndex );


	if(!info) 
	{ 
		return FALSE;
	}
	
	
	CVirtualItem* firstVItem = ( CVirtualItem* ) mIconDialog->GetIconForIdx( 0 ); 
	CVirtualItem* secondVItem = ( CVirtualItem* ) mIconDialog->GetIconForIdx( 1 ); 

	
	if(itemIndex == transmogItemIdx || itemIndex == 60000808 || itemIndex == 60000809 )
	{
		if( ! mIconDialog->IsAddable( 2 ) )
		{
			CItem* original = ITEMMGR->GetItem( mMaterialItem->GetDBIdx() );

			if( original )
			{
				ITEMMGR->RefreshItem( original );

				original->SetLock( FALSE );
			}

			mIconDialog->DeleteIcon( 2, 0 );
		}

		item->SetLock( TRUE );

		if( ITEMMGR->IsDupItem( itemIndex ) )
		{
			item->SetDurability(
				item->GetDurability());
			item->SetVisibleDurability(
				TRUE);
		}
		else
		{
			item->SetVisibleDurability( FALSE );
		}

		{
				ITEMBASE itemBase;
				ZeroMemory( &itemBase, sizeof( itemBase ) );
				itemBase.wIconIdx	= itemIndex;
				itemBase.Position	= item->GetPosition();
				itemBase.Durability	= (ITEMMGR->IsDupItem( itemIndex ) ? item->GetDurability() : 0);
				itemBase.dwDBIdx	= item->GetDBIdx();

				cImage image;

				mMaterialItem->Init(
					0,
					0,
					DEFAULT_ICONSIZE,
					DEFAULT_ICONSIZE,
					ITEMMGR->GetIconImage( itemIndex, &image ),
					0 );

				mMaterialItem->SetIconType( eIconType_AllItem );
				mMaterialItem->SetData( itemIndex );
				mMaterialItem->SetItemBaseInfo( itemBase );
				mMaterialItem->SetVisibleDurability( ITEMMGR->IsDupItem( itemIndex ) );
				
				ITEMMGR->AddToolTip( mMaterialItem );
				mIconDialog->AddIcon( 2, mMaterialItem );
		}

	}


	if (((!secondVItem && !firstVItem) || (secondVItem && !firstVItem)) && isTransmogable(item))
	{
		
		if((secondVItem && !firstVItem))
		{
			const ITEM_INFO*	secondVItemInfo		= ITEMMGR->GetItemInfo( mDummyItem->GetSrcItemIdx() );
			if(!comparePart(secondVItemInfo,info))
			{
				return FALSE;
			}
		}
		

		if( ! mIconDialog->IsAddable( 0 ) )
		{
			CItem* old = ( CItem* )( mSourceItem->GetLinkItem() );

			if( ! old )
			{
				return FALSE;
			}

			ITEMMGR->RefreshItem( old );

			old->SetLock( FALSE );

			mIconDialog->DeleteIcon( 0, 0 );
		}

		{
			mSourceItem->SetData( item->GetItemIdx() );
			mSourceItem->SetLinkItem( item );

			ITEMMGR->AddToolTip( mSourceItem );

			mIconDialog->AddIcon( 0, mSourceItem );
		}	

		item->SetLock( TRUE );
	}
	
	

	if (((!secondVItem && firstVItem) || (secondVItem && firstVItem)) && isTransmogable(item))  
	{ 
		 
		
		const ITEM_INFO*		firstVItemInfo		= ITEMMGR->GetItemInfo( mSourceItem->GetSrcItemIdx() ); 
		if(!comparePart(firstVItemInfo,info))
		{
			return FALSE;
		}
	

		if( ! mIconDialog->IsAddable( 1 ) )
		{
			CItem* old = ( CItem* )( mDummyItem->GetLinkItem() );

			if( ! old )
			{
				return FALSE;
			}

			ITEMMGR->RefreshItem( old );

			old->SetLock( FALSE );

			mIconDialog->DeleteIcon( 1, 0 );
		}

		{
			mDummyItem->SetData( item->GetItemIdx() );
			mDummyItem->SetLinkItem( item );

			ITEMMGR->AddToolTip( mDummyItem );

			mIconDialog->AddIcon( 1, mDummyItem );
		}	

		item->SetLock( TRUE );
	}

	if(!mIconDialog->IsAddable(0) && !mIconDialog->IsAddable(1) && !mIconDialog->IsAddable(2))
	{
		const ITEM_INFO* const itemTempInfo = ITEMMGR->GetItemInfo(
				mSourceItem->GetSrcItemIdx());
		m_pTransmogCost->SetStaticText( AddComma(g_CGameResourceManager.GetTransmogCost(*itemTempInfo)) );
	}
	else {
		m_pTransmogCost->SetStaticText( "" );
	}
	

	return FALSE;

}

void CTransmogDialog::OnActionEvent(LONG lId, void * p, DWORD we)
{

	switch( lId )
	{
	case ITTD_TRANSMOGBTN:
		{
			

			if( m_bDisable )
			{
				CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 992 ) );
				break;
			}
			else if( mIconDialog->IsAddable( 0 ) )
			{
				CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 2532 ) );
				break;
			}
			else if( mIconDialog->IsAddable( 1 ) )
			{
				CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 2533 ) );
				break;
			}
			else if( mIconDialog->IsAddable( 2 ) )
			{
				CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 2534 ) );
				break;
			}

			const ITEM_INFO* const sourceItemInfo = ITEMMGR->GetItemInfo(
				mSourceItem->GetSrcItemIdx());

			const ITEM_INFO* const dummyItemInfo = ITEMMGR->GetItemInfo(
				mDummyItem->GetSrcItemIdx());

			if(0 == sourceItemInfo)
			{
				break;
			}
			else if( 0 == dummyItemInfo)
			{
				break;
			}
			else if(HERO->GetMoney() < g_CGameResourceManager.GetTransmogCost(*sourceItemInfo))
			{
				CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 324 ) );
				break;
			}		

			// 090917 ShinJS --- ÀÎÃ¦Æ® ½Ãµµ Àü ItemOption Ý¤º¸ ÀúÀå
			CItem* pSourceItem = ( CItem* )( mSourceItem->GetLinkItem() );
			if( !pSourceItem )
				break;

			CItem* pDummyItem = ( CItem* )( mDummyItem->GetLinkItem() );
			if( !pDummyItem )
				break;

			const ITEM_OPTION& option = ITEMMGR->GetOption( pSourceItem->GetItemBaseInfo() );
			m_PrevOption = option;


			// 080228 LUJ, ÃÊ±âÈ­
			ZeroMemory( &mMessage, sizeof( mMessage ) );
			
			mMessage.Category			= MP_ITEM;
			mMessage.Protocol			= MP_ITEM_TRANSMOG_SYN;
			mMessage.dwObjectID			= HEROID;
			mMessage.ItemPos			= mSourceItem->GetSrcPosition();
			mMessage.wItemDBIdx			= mSourceItem->GetLinkItem()->GetDBIdx();
			mMessage.wItemIdx		= mSourceItem->GetSrcItemIdx();
			mMessage.SacrificeItemPos = mDummyItem->GetSrcPosition();
			mMessage.wSacrificeItemDBIdx = mDummyItem->GetLinkItem()->GetDBIdx();
			mMessage.wSacrificeItemIdx = mDummyItem->GetSrcItemIdx();
			mMessage.MaterialItemPos	= mMaterialItem->GetPosition();
			mMessage.wMaterialItemDBIdx = mMaterialItem->GetDBIdx();
			mMessage.wMaterialItemIdx	= mMaterialItem->GetItemIdx();
			

			mMessage.PartTypeItem = sourceItemInfo->Part3DType;
			mMessage.PartTypeSacrifice = dummyItemInfo->Part3DType;
			mMessage.EquipTypeItem = sourceItemInfo->EquipType;
			mMessage.EquipTypeSacrifice = dummyItemInfo->EquipType;
			mMessage.EquipSlotItem = sourceItemInfo->EquipSlot;
			mMessage.EquipSlotSacrifice = dummyItemInfo->EquipSlot;


			ITEMMGR->SetDisableDialog( TRUE, eItemTable_Inventory );
			SetDisable( TRUE );

			// ÝøÇà Ç¥½Ã°¡ ³¡³­ ÈÄ ¼­¹ö¿¡ ÀÛ¾÷À» ¿äÃ»ÇÒ °ÝÀÌ´Ù
			{
				CProgressDialog* dialog = ( CProgressDialog* )WINDOWMGR->GetWindowForID( PROGRESS_DIALOG );
				ASSERT( dialog );

				dialog->Wait( CProgressDialog::eActionTransmog );
			}

			break;
		}
	}
}

void CTransmogDialog::Send()
{
	// 080228 LUJ, º¸È£ ¾ÆÀÌÅÛ »ç¿ë Ý¤º¸°¡ ¼­¹ö¿¡¼­ Àü¼ÛÇÝÝö ¾ÊÀ¸¹Ç·Î Å¬¶óÀÌ¾ðÆ® Ý¤º¸¸¦ È°¿ëÇÝµµ·Ý º¹»çÇØ¼­ ¾´´Ù
	MSG_ITEM_TRANSMOG_SYN message( mMessage );

	NETWORK->Send( &mMessage, sizeof( mMessage ) );
}

void CTransmogDialog::SetActive( BOOL isActive )
{
	// ´Ù¸¥ ÀÛ¾÷À» Ã³¸® ÝßÀÎ °æ¿ì¿¡´Â Ã¢À» ¶ç¿ï ¼ö ¾ø´Ù
	//{
	//	cDialog* dialog = WINDOWMGR->GetWindowForID( PROGRESS_DIALOG );
	//	ASSERT( dialog );

	//	if( dialog->IsDisable() &&
	//		dialog->IsActive() )
	//	{
	//		CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1082 ) );
	//		return;
	//	}
	//}

	if( isActive )
	{
		{
			cDialog* npcScriptDialog	= WINDOWMGR->GetWindowForID( NSI_SCRIPTDLG );
			cDialog* npcShopDialog		= WINDOWMGR->GetWindowForID( DE_DEALERDLG );
			cDialog* stallShopDialog	= WINDOWMGR->GetWindowForID( SSI_STALLDLG );
			cDialog* exchangeDialog		= WINDOWMGR->GetWindowForID( XCI_DIALOG );
			cDialog* privateWarehouseDialog		= WINDOWMGR->GetWindowForID( PYO_STORAGEDLG );
			cDialog* guildWarehouseDialog		= WINDOWMGR->GetWindowForID( GDW_WAREHOUSEDLG );
			cDialog* itemMallWarehouseDialog	= WINDOWMGR->GetWindowForID( ITMALL_BASEDLG );

			ASSERT( npcScriptDialog && npcShopDialog && stallShopDialog && exchangeDialog );
			ASSERT( privateWarehouseDialog && guildWarehouseDialog && itemMallWarehouseDialog );

			if( npcScriptDialog->IsActive() )
			{
				return;
			}
			else if( npcShopDialog->IsActive() )
			{
				CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1077 ) );
				return;
			}
			else if( stallShopDialog->IsActive() )
			{
				CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1078 ) );
				return;
			}
			else if( exchangeDialog->IsActive() )
			{
				CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1079 ) );
				return;
			}
			// 071211 LUJ Ã¢°í°¡ Ç¥½ÃµÈ °æ¿ì »ç¿ë ºÒ°¡
			else if(	privateWarehouseDialog->IsActive()	||
				guildWarehouseDialog->IsActive()	||
				itemMallWarehouseDialog->IsActive() )
			{
				CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1082 ) );
				return;
			}
			// 080429 NYJ --- ³¬½ÃÝß »ç¿ëºÒ°¡
			else if(GAMEIN->GetFishingDlg()->IsPushedStartBtn())
			{
				CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1082 ) );
				return;
			}
			// 080429 NYJ --- ³¬½ÃÆ÷ÀÎÆ®±³È¯Ýß »ç¿ëºÒ°¡
			else if(GAMEIN->GetFishingPointDlg()->IsActive() )
			{
				CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1082 ) );
				return;
			}
		}

		// °­È­/ÀÎÃ¦Æ®/Ý¶ÇÕ/ºÝÇØ/³ëÝ¡ÆÇ¸Å Ã¢ÀÌ µ¿½Ã¿¡ ¶ß´Â °ÝÀ» ¸·´Â´Ù
		{
			cDialog* mixDialog			= WINDOWMGR->GetWindowForID( ITMD_MIXDLG );
			cDialog* DissoloveDialog	= WINDOWMGR->GetWindowForID( DIS_DISSOLUTIONDLG );
			cDialog* reinforceDialog	= WINDOWMGR->GetWindowForID( ITR_REINFORCEDLG );
			cDialog* enchantDialog		= WINDOWMGR->GetWindowForID( ITD_UPGRADEDLG );
			
			ASSERT( mixDialog && DissoloveDialog && reinforceDialog && enchantDialog );

			mixDialog->SetActive( FALSE );
			DissoloveDialog->SetActive( FALSE );
			reinforceDialog->SetActive( FALSE );
			enchantDialog->SetActive( FALSE );
		}

		// ÀÎº¥Åä¸®¿Ý ÇÔ²² Ç¥½Ã½ÃÅ²´Ù
		// 091223 ShinJS --- À§Ä¡¸¦ °íÝ¤ÇÝÝö ¾Êµµ·Ý ¼öÝ¤.
		{
			cDialog* inventory = WINDOWMGR->GetWindowForID( IN_INVENTORYDLG );
			ASSERT( inventory );
			inventory->SetActive( TRUE );
		}

		m_pTransmogCost->SetStaticText( "" );
	}
	
	{
		if( ! mIconDialog->IsAddable( 0 ) )
		{
			CItem* item = ( CItem* )( mSourceItem->GetLinkItem() );
			ASSERT( item );

			if( ! item )
			{
				return;
			}

			item->SetLock( FALSE );
			
			mIconDialog->DeleteIcon( 0, 0 );
		}

		if( ! mIconDialog->IsAddable( 1 ) )
		{
			CItem* item = ( CItem* )( mDummyItem->GetLinkItem() );
			ASSERT( item );

			if( ! item )
			{
				return;
			}

			item->SetLock( FALSE );
			
			mIconDialog->DeleteIcon( 1, 0 );
		}

		// Àç·á ¾ÆÀÌÅÛ º¹±¸
		if( ! mIconDialog->IsAddable( 2 ) )
		{
			if(CItem* CONST original = ITEMMGR->GetItem(mMaterialItem->GetDBIdx()))
			{
				original->SetLock( FALSE );
			}

			mIconDialog	->DeleteIcon( 2, 0 );
		}
	}

	cDialog::SetActive( isActive );
}


void CTransmogDialog::Restore()
{
	SetDisable( FALSE );

	ITEMMGR->SetDisableDialog( FALSE, eItemTable_Inventory );
}


DWORD CTransmogDialog::ActionEvent(CMouse* mouse)
{
	const DWORD windowEvent = cDialog::ActionEvent(
		mouse);

	if(FALSE == (WE_LBTNCLICK & windowEvent))
	{
		return windowEvent;
	}

	const POINT point = {mouse->GetMouseX(), mouse->GetMouseY()};

	for(POSTYPE position = 0; mIconDialog->GetCellNum() > position; ++position)
	{
		cIcon* const icon = mIconDialog->GetIconForIdx(
			position);

		if(0 == icon)
		{
			continue;
		}

		RECT rect = {0};
		rect.left = LONG(icon->GetAbsX());
		rect.top = LONG(icon->GetAbsY());
		rect.right = LONG(icon->GetAbsX()) + icon->GetWidth();
		rect.bottom = LONG(icon->GetAbsY()) + icon->GetHeight();

		if(FALSE == PtInRect(&rect, point))
		{
			continue;
		}
		else if(mSourceItem == icon)
		{
			mSourceItem->GetLinkItem()->SetLock(
				FALSE);
		}
		else if(mDummyItem == icon)
		{
			mDummyItem->GetLinkItem()->SetLock(
				FALSE);
		}
		else if(mMaterialItem == icon)
		{
			if(CItem* const item = ITEMMGR->GetItem(mMaterialItem->GetDBIdx()))
			{
				item->SetLock(
					FALSE);
			}
		}

		mIconDialog->DeleteIcon(
			position,
			0);
		break;
	}

	return windowEvent;
}

void CTransmogDialog::Succeed( const MSG_ITEM_TRANSMOG_ACK& message )
{
Restore();

	ITEMMGR->AddOption( message.OptionInfo );


	ITEMMGR->AddToolTip(
		mSourceItem);
	ITEMMGR->AddToolTip(
		(CItem*)mSourceItem->GetLinkItem());

	const ITEM_INFO* const info = ITEMMGR->GetItemInfo(
		mSourceItem->GetLinkItem()->GetItemIdx());

	CHATMGR->AddMsg(
		CTC_OPERATEITEM,
		CHATMGR->GetChatMsg(2485),
		info->ItemName);
	
	CItem* const item = ITEMMGR->GetItem(mMaterialItem->GetDBIdx());
	CItem* deletedItem = 0;

	if(item)
	{
		if(FALSE == item->GetItemInfo()->Stack ||
			1 ==  item->GetDurability())
		{
			ITEMMGR->DeleteItem(
				item->GetPosition(),
				&deletedItem);
		}
		else {
			item->SetDurability(
			item->GetDurability() - 1);
			item->SetLock(
					FALSE);
		}
	}
	
	if(mMaterialItem->GetItemIdx() == transmogItemIdx)
	{
		ITEMMGR->RemoveOption(mDummyItem->GetLinkItem()->GetDBIdx());
		ITEMMGR->DeleteItem(
					mDummyItem->GetLinkItem()->GetPosition(),
					&deletedItem);

		mDummyItem->GetLinkItem()->SetLock(FALSE);
	}
	else {
		mDummyItem->GetLinkItem()->SetLock(FALSE);
	}
	
	mSourceItem->GetLinkItem()->SetLock(FALSE);

	SetActive(FALSE);

	ITEMBASE nBase;
	ZeroMemory( &nBase, sizeof( nBase ) );
	mMaterialItem = new CItem( &nBase );
	mSourceItem = new CVirtualItem;
	mDummyItem = new CVirtualItem;
	
	
}


// 080228 LUJ, º¸È£ ¾ÆÀÌÅÛÀÌ ÀÖÀ» °æ¿ìÀÇ ½ÇÆÝ Ã³¸® Ãß°¡
// 080320 LUJ, ÀÎÃ¦Æ® Ý¦¿Ü º¸È£ ¸ðµå(ÀÎÃ¦Æ® ¼öÄ¡°¡ ÃÊ±âÈ­µÊ) Ã³¸®
void CTransmogDialog::Fail()
{
	Restore();

}


