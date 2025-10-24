#include "stdafx.h"
#include "MixDialog.h"
#include "ChatManager.h"
#include "interface/cIconGridDialog.h"
#include "WindowIDEnum.h"
#include "Item.h"
#include "itemShow.h"
#include "ItemManager.h"
#include "InventoryExDialog.h"
#include "cIconDialog.h"
#include "interface/cStatic.h"
#include "Interface/cScriptManager.h"
#include "ObjectManager.h"
#include "Interface/cWindowManager.h"
#include "objectstatemanager.h"
#include "StreetStallManager.h"
#include "interface/cListDialog.h"
#include "cResourceManager.h"
#include "cDivideBox.h"
#include "cMsgBox.h"
#include "ProgressDialog.h"
#include "FishingManager.h"
#include "GameIn.h"
#include "FishingDialog.h"
#include "FishingPointDialog.h"
#include "interface/cSpin.h"
#include "interface/cComboBox.h"
#include "cIMEEX.h"

const DWORD DisableColor( RGBA_MAKE( 255, 100, 100, 0 ) );
const DWORD EnableColor( RGBA_MAKE( 255, 255, 255, 0 ) );


CMixDialog::CMixDialog() :
mDescription	( 0 ),
mResultIconDialog		( 0 ),
mIconGridDialog	( 0 ),
// 080228 LUJ, �Ϲ�/��ȣ ��� ǥ������ ��Ʈ�� �ʱ�ȭ
mTitle( 0 ),
mProtectTitle( 0 ),
mProtectSymbol( 0 )
{
	// 080228 LUJ, ��ȣ ������ ���� �ʱ�ȭ
	ZeroMemory( &mProtectionItem, sizeof( mProtectionItem ) );
	// 080925 LUJ, ���� ������ ���� �ʱ�ȭ
	ZeroMemory( &mSupportItem, sizeof( mSupportItem ) );

	// 090812 ShinJS --- ���� �������̽� ����, Category ���� �ʱ�ȭ
	for( int i=0 ; i<eMixDlgCategoryType_Max ; ++i )
	{
		m_CategoryDlg[ i ] = NULL;
		m_SelectedCategory[ i ] = 1;
		m_bChangedCategory[ i ] = TRUE;
	}

	m_pCntSpin = NULL;								// ���� cSpin �ʱ�ȭ
	m_pInvenMoney = NULL;							// �����ݾ� cStatic �ʱ�ȭ
	m_pMixCost = NULL;								// ���պ�� cStatic �ʱ�ȭ

	mResultIcon = NULL;

	m_pSearchNameEdit = NULL;
	m_pSearchMinLv = NULL;
	m_pSearchMaxLv = NULL;

	m_htCategoryInfo.Initialize( 20 );				// Category List Info Initialize
	m_htEqualSelectedMixResult.Initialize( 10 );	// ���õ� ���� �������� ��ũ��Ʈ Table (�⺻ �������۰� ��� �������� ���� ������ �ٸ� ��ũ��Ʈ�� ����) �ʱ�ȭ
	m_pSelectedMixResult = NULL;					// ���õ� ���� �������� ��ũ��Ʈ �ʱ�ȭ
	m_MixItemCnt = 1;								// ���õ� ���� �������� ���� ���� �ʱ�ȭ
	m_eMixItemSortType = eMixItemSortType_ByName;	// ���� ������ List ���� ��� �ʱ�ȭ
	m_eMixDialogState = eMixDialogState_Normal;		// ���� Dialog ����
	m_pSearchConditionInitBTN	=	NULL;			// ���� �ʱ�ȭ��ư

	m_dwMixResultCnt	=	0;
	m_bIsEnoughMoney	=	FALSE;
}


CMixDialog::~CMixDialog()
{
	// don't call Release() 
	// pre delete item 

	Release();
}


void CMixDialog::SetActiveRecursive(BOOL val)
{
	if( (STREETSTALLMGR->GetStallKind() == eSK_BUY && OBJECTSTATEMGR->GetObjectState(HERO) == eObjectState_StreetStall_Owner) )
	{
		CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg(789) );
		return;
	}
}

DWORD CMixDialog::ActionKeyboardEvent( CKeyboard* keyInfo )
{
	DWORD	we	=	WE_NULL;

	if( !m_bActive )
		return we;

	if( m_pSearchNameEdit->IsFocus() )
	{
		const char*	pSearchText	=	m_pSearchNameEdit->GetEditText();
		if( strlen( pSearchText ) >= LESSSTRINGLEN )
			SearchMixResultItem();
	}
	//����â ���հ��� �Է½� �ٷ� �ʿ�/���� ������ ���� ����
	else if( m_pCntSpin->IsFocus() )
	{
		char	strItemCount[128]	=	{0, };
		strcpy( strItemCount, RemoveComma( m_pCntSpin->GetEditText() ) );
		DWORD	ItemCount	=	(DWORD)atoi( strItemCount );

		if( ItemCount > MAX_YOUNGYAKITEM_DUPNUM )
		{
			ItemCount = MAX_YOUNGYAKITEM_DUPNUM;
			m_pCntSpin->SetValue( ItemCount );
		}
		
		if( ItemCount != m_MixItemCnt )
		{
			m_MixItemCnt = ItemCount;
			UpdateOwnStuff();
			UpdateCostInfo();
		}
	}

	return	we;
}

void CMixDialog::Linking()
{
	Release();

	// 090812 ShinJS --- Category List Info Load
	LoadMixDlgCategory();

	mDescription		= (cListDialog*)GetWindowForID( ITMD_MIXDESCRIPTION );
	mResultIconDialog	= (cIconDialog*)GetWindowForID(ITMD_MIXICONDLG);
	mIconGridDialog		= (cIconGridDialog*)GetWindowForID(ITMD_MIXICONGRIDDLG);

	// 090812 ShinJS --- Category Dialog Link
	m_CategoryDlg[ eMixDlgCategoryType_1 ]	= (cListDialog*)GetWindowForID( ITMD_MIXCATEGORY1 );
	m_CategoryDlg[ eMixDlgCategoryType_2 ]	= (cListDialog*)GetWindowForID( ITMD_MIXCATEGORY2 );
	m_CategoryDlg[ eMixDlgCategoryType_3 ]	= (cListDialog*)GetWindowForID( ITMD_MIXCATEGORY3 );
	m_CategoryDlg[ eMixDlgCategoryType_4 ]	= (cListDialog*)GetWindowForID( ITMD_MIXCATEGORY4 );

	// ���� ���� �Է¿� cSpin
	m_pCntSpin							= (cSpin*)GetWindowForID( ITMD_MIXCNTSPIN );
	if( m_pCntSpin )
	{
		m_pCntSpin->SetUnit( 1 );
		m_pCntSpin->SetValidCheck( VCM_NUMBER );
	}
	m_pInvenMoney						= (cStatic*)GetWindowForID( ITMD_INVEN_MONEY );
	m_pMixCost							= (cStatic*)GetWindowForID( ITMD_MIX_COST );
	m_pSortTypeCombo					= (cComboBox*)GetWindowForID(ITMD_SORT_TYPE_COMBOBOX);
	// �̸� ���ķ� �ʱ�ȭ
	SetSortType( eMixItemSortType_ByLevel );
	m_pSortTypeCombo->SelectComboText( eMixItemSortType_ByLevel );

	// ���� �ʱ�ȭ��ư	
	m_pSearchConditionInitBTN			= (cButton*)GetWindowForID( ITMD_MIXRESULT_SEARCH_RESET_BTN );

	for( int i=0 ; i<eMixDlgCategoryType_Max ; ++i )
	{
		// Category List Item ���� ���� ����
		if( m_CategoryDlg[ i ] )
			m_CategoryDlg[ i ]->SetShowSelect( TRUE );
	}

	// �������� �ʱ�ȭ�Ѵ�
	{
		cImage imgToolTip;
		SCRIPTMGR->GetImage( 0, &imgToolTip, PFT_HARDPATH );

		// ��� ������
		{
			ITEMBASE base;
			ZeroMemory( &base, sizeof( base ) );

			mResultIcon = new CItem( &base );
			mResultIcon->SetToolTip( "", RGB_HALF( 255, 255, 255), &imgToolTip );
			mResultIcon->SetMovable(FALSE);
		}

		// ��� ������
		{
			int size = mIconGridDialog->GetCellNum();

			mStuffIcon.reserve( size );

			while( size-- )
			{
				CItemShow* item = new CItemShow;

				item->SetToolTip( "", RGB_HALF( 255, 255, 255), &imgToolTip );
				item->SetMovable(FALSE);
				item->SetVisibleDurability( FALSE );

				mStuffIcon.push_back( item );
			}
		}
	}

	// 080228 LUJ, �Ϲ�/��ȣ ��� ǥ������ ��Ʈ�� ��ũ
	{
		mTitle			= GetWindowForID( ITMD_MIXICONTITLE );
		mProtectTitle	= GetWindowForID( ITMD_MIX_PROTECT_TITLE );
		mProtectSymbol	= GetWindowForID( ITMD_MIX_PROTECT_SYMBOL );
	}

	// 090812 ShinJS --- ���� �������̽� ����
	// Category ListDlg �ʱ�ȭ
	InitCategory();

	// 091124 ShinJS --- �˻� ��� �߰�
	m_pSearchNameEdit		= (cEditBox*)GetWindowForID( ITMD_MIXRESULT_SEARCH_NAME_EDIT );
	m_pSearchMinLv			= (cEditBox*)GetWindowForID( ITMD_MIXRESULT_SEARCH_LV_MIN_EDIT );
	m_pSearchMaxLv			= (cEditBox*)GetWindowForID( ITMD_MIXRESULT_SEARCH_LV_MAX_EDIT );

	// 091201 ShinJS --- Comma ���� ���� ����
	m_pSearchMinLv->SetValidCheck( VCM_NORMAL_NUMBER );
	m_pSearchMaxLv->SetValidCheck( VCM_NORMAL_NUMBER );
	m_pSearchMinLv->SetCheckMethodBaseStr( "" );
	m_pSearchMaxLv->SetCheckMethodBaseStr( "" );

}

BOOL CMixDialog::FakeMoveIcon( LONG x, LONG y, cIcon* icon )
{
	if( ! icon ||
		WT_ITEM != icon->GetType()	||
		icon->IsLocked() )
	{
		return FALSE;
	}

	CItem* item = ( CItem* )icon;

	// �κ��丮�̿ܿ� ������ FALSE
	if( ! ITEMMGR->IsEqualTableIdxForPos( eItemTable_Inventory, item->GetPosition() ) )
	{
		CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1168 ) );
		return FALSE;
	}

	// ���� ������� ����
	RemoveResult();

	// ���� Script�� Category Update
	m_dwDragItemIdx = item->GetItemIdx();
	UpdateCategoryByItemIdx( m_dwDragItemIdx );

	// ����â�� �ֻ��� Dialog�� �����.
	WINDOWMGR->SetWindowTop( this );

	// Item Drag ���·� ����
	m_eMixDialogState = eMixDialogState_Drag;

	return FALSE;
}

void CMixDialog::PutDescription( const ItemMixResult* result )
{
	// 090911 ShinJS --- ���� Ȯ�� �߰�
	if( !result )
		return;

	const ITEM_INFO* info = ITEMMGR->GetItemInfo( result->mItemIndex );
	ASSERT( info );

	const std::string prefixProduct	( RESRCMGR->GetMsg( 626 ) ); // "����ǰ��: ";
	const std::string prefixLevel	( RESRCMGR->GetMsg( 627 ) ); // "���ѷ���: ";
	const std::string prefixMaterial( RESRCMGR->GetMsg( 628 ) ); // "�ʿ����: ";
	const std::string prefixMoney	( RESRCMGR->GetMsg( 629 ) ); // "���ۺ��: ";
	const std::string prefixSuccess	( CHATMGR->GetChatMsg(1464) );
	const std::string postfixList	( RESRCMGR->GetMsg( 630 ) ); // "��";

	CHero*		hero = OBJECTMGR->GetHero();
	const DWORD textColor( RGBA_MAKE( 255, 255, 255, 0 ) );
	char		line[ MAX_TEXT_SIZE ];

	mDescription->RemoveAll();

	// ���� ǰ�� ǥ��
	{
		sprintf( line, "%s%s", prefixProduct.c_str(), info->ItemName );

		mDescription->AddItemAutoLine( line, ITEMMGR->GetItemNameColor( *info ) );
	}

	// ���� ǥ��
	{
		if( result->mRequiredLevel > hero->GetLevel() )
		{
			sprintf( line, "%s%d", prefixLevel.c_str(), result->mRequiredLevel );
			mDescription->AddItemAutoLine( line, DisableColor );
		}
	}

	// ��� ǥ��
	{
		sprintf( line, "%s%s", prefixMoney.c_str(), AddComma( result->mMoney ) );

		mDescription->AddItemAutoLine( line, ( result->mMoney > hero->GetMoney() ? DisableColor : EnableColor ) );
	}

	// 080925 LUJ, �������� 0~100% ������ ǥ�õǵ��� �Ѵ�
	{
		const MixSupportScript* script				= ITEMMGR->GetMixSupportScript( mSupportItem.wIconIdx );
		const float				bonusSuccessRate	= ( script ? script->mBonusSucessRate : 0 );
		sprintf( line, prefixSuccess.c_str(), min( 100.0f, max( 0, ( bonusSuccessRate + result->mSuccessRate ) * 100.0f ) ) );

		mDescription->AddItemAutoLine( line, textColor );
	}

	typedef std::list< std::string >	Description;
	Description							description;

	// ��� ������
	{
		sprintf( line, "%s", prefixMaterial.c_str() );

		mDescription->AddItemAutoLine( line, textColor );

		std::string tab;

		// ��� ǥ���� ���� ��ŭ�� �������� �Ѵ�
		for( int i = strlen( prefixMaterial.c_str() ) / 2; i--; )
		{
			tab += " ";
		}

		for(ItemMixResult::Material::const_iterator it = result->mMaterial.begin();
			result->mMaterial.end() != it;
			++it )
		{
			const DWORD itemIndex	= it->first;
			const DWORD quantity	= it->second;

			const ITEM_INFO* materialInfo = ITEMMGR->GetItemInfo( itemIndex );

			sprintf( line, "%s%s %d%s", tab.c_str(), materialInfo->ItemName, quantity, postfixList.c_str() );

			mDescription->AddItemAutoLine( line, textColor );
		}
	}

	mDescription->SetPosition( 0 );
}

void CMixDialog::RemoveResult()
{
	mDescription->RemoveAll();

	for(WORD i = 0 ; i < mIconGridDialog->GetCellNum() ; ++i )
	{
		mIconGridDialog->DeleteIcon( i, 0 );
	}

	mResultIconDialog->DeleteIcon( 0, 0 );
}


void CMixDialog::Submit( DURTYPE mixSize, BOOL isCheck )
{
	if( ! mixSize )
	{
		CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1085 ) );
		return;
	}

	UpdateCostInfo();

	// 090817 ShinJS --- ���õ� ���� ��ũ��Ʈ�� ���� ���
	if( !m_pSelectedMixResult )
	{
		// ���� ������ ���� ��û �޼��� ���
		CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 791 ) );
		return ;
	}

	const DWORD sourceItemIndex = m_pSelectedMixResult->mBaseMaterial;
	CItem* sourceItem = GAMEIN->GetInventoryDialog()->GetItemForIdx(
		sourceItemIndex,
		WORD(-1));
	const POSTYPE inventoryStartPosition = TP_INVENTORY_START;
	const POSTYPE inventoryEndPosition = POSTYPE(TP_INVENTORY_END + TABCELL_INVENTORY_NUM * HERO->Get_HeroExtendedInvenCount());

	// �⺻ ������ ����?
	if( ! sourceItem )
	{
		// ��� ���� �޼��� ���
		CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 784 ) );
		return ;
	}

	CHero* hero = OBJECTMGR->GetHero();
	ASSERT( hero );

	if( m_pSelectedMixResult->mMoney * mixSize > HERO->GetMoney() )
	{
		CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 324 ) );
		return ;
	}
	else if( hero->GetLevel() < m_pSelectedMixResult->mRequiredLevel )
	{
		CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 792 ) );
		return;
	}

	ZeroMemory( &mMessage, sizeof( mMessage ) );

	const DWORD					sourceItemDBIndex	= sourceItem->GetDBIdx();
	MSG_ITEM_MIX_SYN::Material*	material			= mMessage.mMaterial;
	ItemMixResult::Material		requiredMaterial;

	// �ʿ� ������ ä����... 
	for(
		ItemMixResult::Material::const_iterator it = m_pSelectedMixResult->mMaterial.begin();
		m_pSelectedMixResult->mMaterial.end() != it;
		++it )
	{
		const DWORD		itemIndex	= it->first;
		const DURTYPE	quantity	= it->second;

		requiredMaterial[ itemIndex ] = quantity * mixSize;

		if( ! ITEMMGR->GetItemInfo( itemIndex ) )
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1271 ) );

			ZeroMemory( &mMessage, sizeof( mMessage ) );
			return;
		}
	}

	CInventoryExDialog* inventory = ( CInventoryExDialog* )WINDOWMGR->GetWindowForID( IN_INVENTORYDLG );
	ASSERT( inventory );

	// ��� ������ �����Ѵ�
	for(
		POSTYPE position = inventoryStartPosition;
		inventoryEndPosition > position;		
		++position )
	{
		if( requiredMaterial.empty() )
		{
			break;
		}

		CItem* item = inventory->GetItemForPos( position );

		if( ! item )
		{
			continue;
		}

		const DWORD	itemIndex	= item->GetItemIdx();
		const BOOL	isDuplicate = ITEMMGR->IsDupItem( itemIndex );
		const DWORD quantity	= ( isDuplicate ? item->GetDurability() : 1 );

		if( 1 > quantity													||
			requiredMaterial.end() == requiredMaterial.find( itemIndex )	||
			item->IsLocked() )
		{
			continue;
		}

		DWORD& remainedQuantity = requiredMaterial[ itemIndex ];

		// ���� �������� ���� ������ ������ ����
		// ���� ������ 0������ ��� ������ ������ ���� ���� ������ �Ѵ�.
		{
			const DWORD previousRemainedQuantity = remainedQuantity;

			remainedQuantity		= max( 0, int( remainedQuantity - quantity ) );

			//material->mPosition		= item->GetPosition();
			material->mPosition		= position;
			material->mItemIndex	= itemIndex;
			material->mQuantity		= ( isDuplicate ? min( previousRemainedQuantity, quantity ) : 1 );

			if( sourceItemDBIndex == item->GetDBIdx() )
			{
				mMessage.mSourceIndex = material - mMessage.mMaterial;

				if( isDuplicate )
				{
					++material->mQuantity;
				}
			}

			++mMessage.mSourceSize;
			++material;
		}

		if( ! remainedQuantity )
		{
			requiredMaterial.erase( itemIndex );
		}
	}

	// ���� ��ġ�� ����Ѵ�.
	{
		typedef MSG_ITEM_MIX_SYN::Material	Material;
		typedef std::map< DWORD, Material > InventoryMap;
		InventoryMap						inventoryMap;

		// �κ��丮 ���� ����
		for(
			POSTYPE position = inventoryStartPosition;
			inventoryEndPosition > position;
			++position )
		{
			CItem* item = inventory->GetItemForPos( position );

			if( item )
			{
				const BOOL isDuplicate = ITEMMGR->IsDupItem( item->GetItemIdx() );

				Material& slot = inventoryMap[ position ];

				slot.mItemIndex	= item->GetItemIdx();
				slot.mPosition	= position;
				slot.mQuantity	= ( isDuplicate ? item->GetDurability() : 1 );
			}
		}

		// ��� ������ ��ȸ�ذ��鼭 �κ��丮 ������ �����Ѵ�(�ҿ�� ��Ḧ �����Ѵ�)
		for( DWORD i = mMessage.mSourceSize; i--; )
		{
			const Material& source		= mMessage.mMaterial[ i ];
			const POSTYPE	position	= source.mPosition;

			if( inventoryMap.end() == inventoryMap.find( position ) )
			{
				continue;
			}

			Material& slot = inventoryMap[ position ];
			ASSERT( slot.mQuantity >= source.mQuantity );

			slot.mQuantity -= source.mQuantity;

			if( slot.mQuantity ||
				slot.mPosition == sourceItem->GetPosition() )
			{
				continue;
			}

			inventoryMap.erase( position );
		}

		MSG_ITEM_MIX_SYN::Material*	resultSlot			= mMessage.mMaterial + mMessage.mSourceSize;
		int							resultSize			= int( mixSize );
		const BOOL					isResultDuplicate	= ITEMMGR->IsDupItem( m_pSelectedMixResult->mItemIndex );
		// 091215 ONS ������Stack�ִ���� �����ۺ��� ������ ������ ó���Ѵ�.
		const WORD wItemStackNum = ITEMMGR->GetItemStackNum( m_pSelectedMixResult->mItemIndex );

		// ��ġ�� �������̸�
		if( isResultDuplicate )
		{
			// �ϴ� ���� ������ �ε����� ã�� ����������ŭ ���� �ִ´�
			for(
				POSTYPE position = inventoryStartPosition;
				inventoryEndPosition > position;
				++position )
			{
				if( ! resultSize )
				{
					break;
				}
				else if( inventoryMap.end() == inventoryMap.find( position ) )
				{
					continue;
				}

				const Material& slot		= inventoryMap[ position ];
				const int		enableSize	= int( wItemStackNum - slot.mQuantity );

				// 071213 LUJ,	��� �������� ��� ����� ������Ʈ�� ������ �������� �ʴ´�
				//				DB ������ ����/������ �ڹٲ�� ����� ��� �����ص� �������� �����ϴ� ��찡 �߻��Ѵ�
				if(	m_pSelectedMixResult->mItemIndex != slot.mItemIndex	||
					0 >=	enableSize						||
					position == sourceItem->GetPosition() )
				{
					continue;
				}

				resultSlot->mItemIndex	= m_pSelectedMixResult->mItemIndex;
				resultSlot->mQuantity	= min( resultSize, enableSize );
				resultSlot->mPosition	= position;

				resultSize	-= resultSlot->mQuantity;

				++resultSlot;
			}
		}

		// ��ġ�� �������� �ƴϰų�, ���� ��ġ�� �������� ���� ��� ä���.
		for(
			POSTYPE position = inventoryStartPosition;
			inventoryEndPosition > position;
			++position )
		{
			if( ! resultSize )
			{
				break;
			}
			else if( inventoryMap.end() != inventoryMap.find( position ) )
			{
				continue;
			}

			resultSlot->mItemIndex	= m_pSelectedMixResult->mItemIndex;
			resultSlot->mQuantity	= ( isResultDuplicate ? min( resultSize, wItemStackNum ) : 1 );
			resultSlot->mPosition	= position;

			resultSize				-= resultSlot->mQuantity;
			++resultSlot;
		}

		if( resultSize )
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 910 ) );

			ZeroMemory( &mMessage, sizeof( mMessage ) );
			return;
		}

		mMessage.mResultSize = resultSlot - ( mMessage.mMaterial + mMessage.mSourceSize );
	}

	if( ! requiredMaterial.empty() )
	{
		CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 784 ) );

		ZeroMemory( &mMessage, sizeof( mMessage ) );
		return;
	}

	{
		// 090812 ShinJS --- MixScript List���� ���� ���õ� MixResult�� ��ġ�� ���Ѵ�
		const DWORD dwMixScriptPos = ITEMMGR->GetMixResultPosInfo( m_pSelectedMixResult->mBaseMaterial, m_pSelectedMixResult );
		if( dwMixScriptPos == (DWORD)-1 )
			return;

		mMessage.Category			= MP_ITEM;
		mMessage.Protocol			= MP_ITEM_MIX_SYN;
		mMessage.dwObjectID			= gHeroID;
		mMessage.mResultIndex		= dwMixScriptPos;
		mMessage.mMixSize			= mixSize;
	}

	// 080228 LUJ, ��ȣ ������ ���� ����
	if( mProtectionItem.wIconIdx )
	{
		ICONBASE& protectionItem = mMessage.mProtectionItem;

		protectionItem.dwDBIdx	= mProtectionItem.dwDBIdx;
		protectionItem.wIconIdx	= mProtectionItem.wIconIdx;
		protectionItem.Position	= mProtectionItem.Position;
	}

	if( isCheck )
	{
		if( 1 < mixSize )
		{
			const ITEM_INFO* itemInfo = ITEMMGR->GetItemInfo( m_pSelectedMixResult->mItemIndex );
			ASSERT( itemInfo );

			WINDOWMGR->MsgBox( MBI_ITEM_MIX_CONFIRM, MBT_YESNO, CHATMGR	->GetChatMsg( 1086 ), itemInfo->ItemName, mixSize );
		}
		// ���� ǥ�ð� ���� �� ������ �۾��� ��û�� ���̴�
		else
		{
			CProgressDialog* dialog = ( CProgressDialog* )WINDOWMGR->GetWindowForID( PROGRESS_DIALOG );
			ASSERT( dialog );

			dialog->Wait( CProgressDialog::eActionMix );
		}

		SetDisable( TRUE );
	}
}

void CMixDialog::OnActionEvent( LONG lId, void * p, DWORD we )
{
	CatagoryScrollEnd();
	switch( lId )
	{
		// ���� �õ�
	case ITMD_MIXOKBTN:
		{
			// 090817 ShinJS --- ���õ� ���� �������� ���� ���
			if( !m_pSelectedMixResult )
			{
				// ���� ������ ���� ��û �޼��� ���
				CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 791 ) );
				return;
			}

			// 080925 LUJ, ���� �������� ���� ��� ����/��/�׼������� ���� �����ϴ�
			if( mSupportItem.wIconIdx )
			{
				const ITEM_INFO* itemInfo = ITEMMGR->GetItemInfo( m_pSelectedMixResult->mItemIndex );

				if( ! itemInfo )
				{
					return;
				}
				else if(	( eEquipType_Accessary	!= itemInfo->EquipType ) &&
							( eEquipType_Weapon		!= itemInfo->EquipType ) &&
							( eEquipType_Card		!= itemInfo->EquipType ) &&
							( eEquipType_Armor		!= itemInfo->EquipType ) )
				{
					WINDOWMGR->MsgBox(
						MBI_NOTICE,
						MBT_OK,
						CHATMGR->GetChatMsg( 1714 ) );
					return;
				}
			}

			// 090817 ShinJS --- ���� ���� ����
			if( m_pCntSpin )
			{
				DURTYPE mixItemCnt = (DURTYPE)m_pCntSpin->GetValue();
				if( mixItemCnt > MAX_YOUNGYAKITEM_DUPNUM )
					mixItemCnt = MAX_YOUNGYAKITEM_DUPNUM;
				m_pCntSpin->SetValue( mixItemCnt );
				m_MixItemCnt = mixItemCnt;
			}
			
			Submit( m_MixItemCnt, TRUE );
		}
		break;
		// 080925 LUJ, �ݱ� ��ư ó�� �߰�
	case ITMD_MIXDLG:
	case ITMD_MIXCANCELBTN:
		{
			//CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 785 ) );

			SetActive( FALSE );
		}
		break;		

		// 090812 ShinJS --- ī�װ��� ����
	case ITMD_MIXCATEGORY1:
	case ITMD_MIXCATEGORY2:
	case ITMD_MIXCATEGORY3:
	case ITMD_MIXCATEGORY4:
		{
			if( we == WE_ROWCLICK )
			{
				ITEM* pItem = GetSelectedItemFromListDlg( lId );
				if( !pItem )
					break;

				UpdateCategory();
			}
		}
		break;

	case ITMD_MIXCNTSPIN:
		{
			// ���� ������ ������ ����� ��� �����Ȳ Update
			DURTYPE mixItemCnt = (DURTYPE)m_pCntSpin->GetValue();
			if( mixItemCnt > MAX_YOUNGYAKITEM_DUPNUM )
			{
				mixItemCnt = MAX_YOUNGYAKITEM_DUPNUM;
				m_pCntSpin->SetValue( mixItemCnt );
			}
			
			if( mixItemCnt != m_MixItemCnt )
			{
				m_MixItemCnt = mixItemCnt;
				UpdateOwnStuff();
				UpdateCostInfo();
			}
		}
		break;

	case ITMD_SORT_TYPE_COMBOBOX:
		{
			if( we != WE_COMBOBOXSELECT )
				break;

			// ���� Type ����			
			SetSortType( eMixItemSortType( m_pSortTypeCombo->GetCurSelectedIdx() ) );
		}
		break;

	case ITMD_MIXRESULT_SEARCH_RESET_BTN:
		{
			// �˻� �ʱ�ȭ
			if( we != WE_BTNCLICK )
				break;
			
			DWORD	dwMixCost	=	0;
			m_pSearchNameEdit->SetEditText( "" );
			m_pSearchMinLv->SetEditText( "" );
			m_pSearchMaxLv->SetEditText( "" );
			m_pCntSpin->SetValue(0);
			m_pMixCost->SetStaticText( AddComma( dwMixCost ) );

			// ��� ����
			RemoveResult();

			// ī�װ��� ����� ���õ� ��ũ��Ʈ �ʱ�ȭ
			ClearSelectedCategoryInfo();

			// ī�װ��� �ʱ�ȭ
			m_CategoryDlg[ eMixDlgCategoryType_1 ]->SetCurSelectedRowIdx( -1 );
			m_CategoryDlg[ eMixDlgCategoryType_2 ]->RemoveAll();
			m_CategoryDlg[ eMixDlgCategoryType_3 ]->RemoveAll();
			m_CategoryDlg[ eMixDlgCategoryType_4 ]->RemoveAll();

			m_SelectedCategory[ eMixDlgCategoryType_1 ] = -1;
			m_SelectedCategory[ eMixDlgCategoryType_2 ] = -1;
			m_SelectedCategory[ eMixDlgCategoryType_3 ] = -1;
			m_SelectedCategory[ eMixDlgCategoryType_4 ] = -1;
		}
		break;

	case ITMD_MIXRESULT_SEARCH_NAME_EDIT:
	case ITMD_MIXRESULT_SEARCH_LV_MIN_EDIT:
	case ITMD_MIXRESULT_SEARCH_LV_MAX_EDIT:
	case ITMD_MIXRESULT_SEARCH_BTN:
		{
			// ������ �˻�
			if( !((we & WE_RETURN) || (we & WE_BTNCLICK)) )
				break;

			// �˻� ���·� ����
			m_eMixDialogState = eMixDialogState_Search;

			SearchMixResultItem();
		}
		break;

	case	ITMD_MIXCATEGORY1_BTN:
		{
			PushCatagory1BTN();
		}
		break;
	case	ITMD_MIXCATEGORY2_BTN:
		{
			PushCatagory2BTN();
		}
		break;
	}
}

void CMixDialog::Release()
{
	{
		for(std::vector<CItemShow*>::iterator it = mStuffIcon.begin();
			mStuffIcon.end() != it;
			++it )
		{
			CItemShow* item = *it;

			SAFE_DELETE( item );
		}

		mStuffIcon.clear();
	}

	SAFE_DELETE( mResultIcon );

	m_htCategoryInfo.SetPositionHead();

	for(stCategoryInfo* pInfo = m_htCategoryInfo.GetData();
		0 < pInfo;
		pInfo = m_htCategoryInfo.GetData())
	{
		SAFE_DELETE( pInfo );
	}
	m_htCategoryInfo.RemoveAll();

	m_htEqualSelectedMixResult.RemoveAll();
}

// 080228 LUJ, ��ȣ �������� ����� ��� �Ҹ��Ѵ�
void CMixDialog::Succeed( const MSG_ITEM_MIX_ACK& message )
{
	Restore();

	// 080228 LUJ, ��ȣ �������� ����� ��� â�� �ݴ´�
	if( mMessage.mProtectionItem.dwDBIdx )
	{
		CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1452 ) );

		CItem* item = ITEMMGR->GetItem( mMessage.mProtectionItem.dwDBIdx );

		if( item )
		{
			SetActive( FALSE );
		}
	}

	// 080925 LUJ, ���� �������� ����� ��� â�� �ݴ´�
	if( mSupportItem.wIconIdx )
	{
		SetActive( FALSE );
	}
}


// 080228 LUJ, ��ȣ ��� ó�� �߰�
void CMixDialog::Fail( const MSG_ITEM_MIX_ACK& message )
{
	Restore();

	// 080228 LUJ, ��ȣ �������� ����� ��� â�� �ݴ´�
	if( mMessage.mProtectionItem.dwDBIdx )
	{
		CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1449 ) );

		CItem* item = ITEMMGR->GetItem( mMessage.mProtectionItem.dwDBIdx );

		if( item )
		{
			SetActive( FALSE );
		}
	}
	else
	{
		CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 812 ) );
	}

	// 080925 LUJ, ���� �������� ����� ��� â�� �ݴ´�
	if( mSupportItem.wIconIdx )
	{
		SetActive( FALSE );
	}
}


void CMixDialog::SetActive( BOOL isActive )
{
	// �ٸ� �۾��� ó�� ���� ��쿡�� â�� ��� �� ����
	{
		cDialog* dialog = WINDOWMGR->GetWindowForID( PROGRESS_DIALOG );
		ASSERT( dialog );

		if( dialog->IsDisable() &&
			dialog->IsActive() )
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1082 ) );
			return;
		}
	}

	if( isActive )
	{
		// NPC ������ �� ���¿����� �Ұ�. 
		// �Ǹ� ������ �� ���¿��� �� �� ����: �������� ��������, �������� ��� ó���� Ŭ���̾�Ʈ�� ������� �����Ƿ� ��õ������ �����ۿ� ����
		// ���� ���� �õ��� �����ؾ� �Ѵ�.
		// �ŷ� ��/â�� ����� ���� �Ұ���. ���� �������� ����
		{
			cDialog* npcScriptDialog			= WINDOWMGR->GetWindowForID( NSI_SCRIPTDLG );
			cDialog* npcShopDialog				= WINDOWMGR->GetWindowForID( DE_DEALERDLG );
			cDialog* stallShopDialog			= WINDOWMGR->GetWindowForID( SSI_STALLDLG );
			cDialog* exchangeDialog				= WINDOWMGR->GetWindowForID( XCI_DIALOG );
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
			// 071211 LUJ â���� ǥ�õ� ��� ��� �Ұ�
			else if(	privateWarehouseDialog->IsActive()	||
						guildWarehouseDialog->IsActive()	||
						itemMallWarehouseDialog->IsActive() )
			{
				CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1082 ) );
				return;
			}
			// 080429 NYJ --- ������ ���Ұ�
			else if(GAMEIN->GetFishingDlg()->IsPushedStartBtn())
			{
				CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1082 ) );
				return;
			}
			// 080429 NYJ --- ��������Ʈ��ȯ�� ���Ұ�
			else if(GAMEIN->GetFishingPointDlg()->IsActive() )
			{
				CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1082 ) );
				return;
			}
		}

		// ��ȭ/��æƮ/����/����/�����Ǹ� â�� ���ÿ� �ߴ� ���� ���´�
		{
			cDialog* DissoloveDialog	= WINDOWMGR->GetWindowForID( DIS_DISSOLUTIONDLG );
			cDialog* enchantDialog		= WINDOWMGR->GetWindowForID( ITD_UPGRADEDLG );
			cDialog* reinforceDialog	= WINDOWMGR->GetWindowForID( ITR_REINFORCEDLG );

			ASSERT( DissoloveDialog && enchantDialog && reinforceDialog );

			DissoloveDialog->SetActive( FALSE );
			enchantDialog->SetActive( FALSE );
			reinforceDialog->SetActive( FALSE );
		}

		mIconGridDialog->SetCurSelCellPos( -1 );

		// �κ��丮�� �Բ� �߾ӿ� ǥ�ý�Ų��
		{
			cDialog* inventory = WINDOWMGR->GetWindowForID( IN_INVENTORYDLG );
			ASSERT( inventory );

			const DISPLAY_INFO& screen	= GAMERESRCMNGR->m_GameDesc.dispInfo;
			const int			space	=  screen.dwWidth > DWORD(m_width + inventory->GetWidth() + 140) ? 140 :screen.dwWidth - m_width - inventory->GetWidth();
			const DWORD			x		= ( screen.dwWidth - m_width - inventory->GetWidth() - space ) / 2;
			const DWORD			y		= ( screen.dwHeight - max( inventory->GetHeight(), m_height ) ) / 2;

			SetAbsXY( x + space + inventory->GetWidth(), y );

			inventory->SetAbsXY( x, y );
			inventory->SetActive( TRUE );
		}

		// 080228 LUJ, �Ϲ� ��� ǥ��
		{
			if( mTitle )
			{
				mTitle->SetActive( TRUE );
			}

			if( mProtectTitle )
			{
				mProtectTitle->SetActive( FALSE );
			}

			if( mProtectSymbol )
			{
				mProtectSymbol->SetActive( FALSE );
			}
		}

		// 090817 ShinJS --- ���õ� ���� �������� ���� ���� �ʱ�ȭ
		m_MixItemCnt = 1;
		m_pCntSpin->SetValue( 1 );

		InitCategory();
		UpdateCostInfo();
	}
	// 080925 LUJ, â�� ���� �� ó��
	else
	{
		// 080925 LUJ, ���� ������ ����
		{
			CItem* item = ITEMMGR->GetItem( mSupportItem.dwDBIdx );

			if( item )
			{
				item->SetLock( FALSE );
			}

			ZeroMemory( &mSupportItem, sizeof( mSupportItem ) );
		}

		RemoveResult();

		mResultIconDialog->DeleteIcon( 0, 0 );

		// 080228 LUJ, ��ȣ �������� �ʱ�ȭ�Ѵ�
		{
			CItem* item = ITEMMGR->GetItem( mProtectionItem.dwDBIdx );

			if( item )
			{
				item->SetLock( FALSE );

				ZeroMemory( &mProtectionItem, sizeof( mProtectionItem ) );
			}
		}
	}
	
	m_pSearchNameEdit->SetEditText( "" );

	cDialog::SetActive( isActive );
}

void CMixDialog::Restore()
{
	//mIsMixing	 = FALSE;

	ITEMMGR->SetDisableDialog( FALSE, eItemTable_Inventory );

	if( HERO->GetState() == eObjectState_Die )
	{
		SetActive( FALSE );
	}

	SetDisable( FALSE );
}

void CMixDialog::Send()
{
	Submit( mMessage.mMixSize, FALSE );

	static const MSG_ITEM_MIX_SYN emptyMessage;

	if( ! memcmp( &emptyMessage, &mMessage, sizeof( emptyMessage ) ) )
	{
		return;
	}

	MSG_ITEM_MIX_SYN2 message;
	{
		ZeroMemory( &message, sizeof( message ) );

		if( !m_pSelectedMixResult )
			return;

		const DWORD dwSoruceItemIdx = m_pSelectedMixResult->mBaseMaterial;
		CItem* sourceItem = GAMEIN->GetInventoryDialog()->GetItemForIdx(
			dwSoruceItemIdx,
			WORD(-1));

		if( ! sourceItem )
		{
			// ��� ���� �޼��� ���
			CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 784 ) );
			return ;
		}

		message.Category			= MP_ITEM;
		message.Protocol			= MP_ITEM_MIX_SYN;
		message.dwObjectID			= HEROID;
		message.mSourceItemPosition	= sourceItem->GetPosition();
		message.mSourceItemDbIndex	= sourceItem->GetDBIdx();
		message.mMixSize			= mMessage.mMixSize;
		message.mResultIndex		= mMessage.mResultIndex;
		message.mProtectionItem		= mMessage.mProtectionItem;
		// 080925 LUJ, ���� ������ ���� ����
		message.mSupportItem		= mSupportItem;
	}

	NETWORK->Send( &message, sizeof( message ) );	

	{
		SetDisable( FALSE );
	}	
}

void CMixDialog::Refresh()
{
	if( !IsActive() )
		return;

	// ������� ������/���� ������ ����ɶ����� ������ �ʿ��ϴ�
	// ������ ������ ������ ����, ��� ��Ȳ�� �������� �ʵ��� �Ѵ�.
	PutDescription( m_pSelectedMixResult );

	UpdateCostInfo();
}

// 080227 LUJ, ���� ����� �����Ѵ�
void CMixDialog::RefreshResult( const ITEMBASE& itemBase )
{
	if( mResultIcon )
	{
		mResultIcon->SetItemBaseInfo( itemBase );

		ITEMMGR->AddToolTip( mResultIcon );

		mResultIconDialog->AddIcon( 0, mResultIcon );
	}
}

// 080228 LUJ, ��ȣ ������ ����
void CMixDialog::SetProtectedActive( CItem& item )
{
	// 080228 LUJ, ��ȣ ��� ǥ��
	{
		if( mTitle )
		{
			mTitle->SetActive( FALSE );
		}

		if( mProtectTitle )
		{
			mProtectTitle->SetActive( TRUE );
		}

		if( mProtectSymbol )
		{
			mProtectSymbol->SetActive( TRUE );
		}
	}

	mProtectionItem = item.GetItemBaseInfo();
	item.SetLock( TRUE );

	CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1456 ) );
}

// 080925 LUJ, ���� ���� ������ ����
void CMixDialog::SetSupportItem( const ITEMBASE& itemBase )
{
	mSupportItem.wIconIdx	= itemBase.wIconIdx;
	mSupportItem.dwDBIdx	= itemBase.dwDBIdx;
	mSupportItem.Position	= itemBase.Position;
}

// 090812 ShinJS --- ���� �������̽� ����, ���� ������ Category ����Ʈ Load, Init, Update
void CMixDialog::LoadMixDlgCategory()
{
	CMHFile file;
    char filename[256];
    sprintf(filename, "Data/Interface/Windows/%s", GAMERESRCMNGR->GetTranslationPath("MixDialogCategory.bin"));
    
	file.Init(filename, "rb");
    


	eMixDlgCategoryType eCurCategoryType = eMixDlgCategoryType_Max;

	while( !file.IsEOF() )
	{
		char buf[ MAX_PATH ] = {0,};

		file.GetString( buf );

		if( buf[0] == '/' && buf[1] == '/' )
		{
			file.GetLineX( buf, MAX_PATH );
			continue;
		}
		else if( buf[0] == '}' )
		{
			eCurCategoryType = eMixDlgCategoryType_Max;
		}
		else if( strcmp( buf, "$Category1" ) == 0 )
		{
			eCurCategoryType = eMixDlgCategoryType_1;
		}
		else if( strcmp( buf, "$Category2" ) == 0 )
		{
			eCurCategoryType = eMixDlgCategoryType_2;
		}
		else if( strcmp( buf, "#AddList" ) == 0 )
		{
			stCategoryInfo* pInfo = new stCategoryInfo;

			switch( eCurCategoryType )
			{
			case eMixDlgCategoryType_1:
				{
					pInfo->categoryType				= eMixDlgCategoryType_1;
					pInfo->wID						= file.GetWord();						// ID
					pInfo->wHighCategoryID			= WORD( -1 );
					file.GetString( pInfo->szName );										// Name	
					
					// Item Type
					char txt[MAX_PATH]={0,};
					file.GetLine( txt, MAX_PATH );
					char* delimit = " ,\t";
					char* token = strtok( txt, delimit );
                    while( token != NULL )
					{
						pInfo->setItemType.insert( DWORD(atoi( token )) );
						token = strtok( 0, delimit );
					}
				}
				break;
			case eMixDlgCategoryType_2:
				{
					pInfo->categoryType				= eMixDlgCategoryType_2;
					pInfo->wID						= WORD( m_htCategoryInfo.GetDataNum() );// ID
					pInfo->wHighCategoryID			= file.GetWord();						// ����Type
					file.GetString( pInfo->szName );										// Name
					
					// Item TypeDetail
					char txt[MAX_PATH]={0,};
					file.GetLine( txt, MAX_PATH );
					char* delimit = " ,\t";
					char* token = strtok( txt, delimit );
                    while( token != NULL )
					{
						pInfo->setItemTypeDetail.insert( DWORD(atoi( token )) );
						token = strtok( 0, delimit );
					}
				}
				break;
			}

			m_htCategoryInfo.Add( pInfo, pInfo->wHighCategoryID );
		}
	}
}

ITEM* CMixDialog::GetSelectedItemFromListDlg( eMixDlgCategoryType eCategory, BOOL bCancelOpt )
{
	if( !m_CategoryDlg[ eCategory ] )
		return NULL;

	int nSelectedRow = m_CategoryDlg[ eCategory ]->GetClickedRowIdx();
	if( nSelectedRow < 0 )
		return NULL;

	PTRLISTPOS pos = m_CategoryDlg[ eCategory ]->GetListItem()->FindIndex( nSelectedRow );
	if( !pos )
		return NULL;

	ITEM* pItem = (ITEM*)m_CategoryDlg[ eCategory ]->GetListItem()->GetAt(pos);
	if( !pItem )
		return NULL;

	if( nSelectedRow != m_SelectedCategory[ eCategory ] )
	{
		m_bChangedCategory[ eCategory ] = TRUE;

		// ���� ��ġ ����
		m_SelectedCategory[ eCategory ] = nSelectedRow;
	}
	return pItem;
}

ITEM* CMixDialog::GetSelectedItemFromListDlg( LONG lId )
{
	eMixDlgCategoryType categoryType;
	switch( lId )
	{
	case ITMD_MIXCATEGORY1:
		categoryType = eMixDlgCategoryType_1;
		break;
	case ITMD_MIXCATEGORY2:
		categoryType = eMixDlgCategoryType_2;
		break;
	case ITMD_MIXCATEGORY3:
		categoryType = eMixDlgCategoryType_3;
		break;
	case ITMD_MIXCATEGORY4:
		categoryType = eMixDlgCategoryType_4;
		break;
	default:
		return NULL;
	}

	return GetSelectedItemFromListDlg( categoryType, TRUE );
}

stCategoryInfo* CMixDialog::GetSelectedCategoryInfo( eMixDlgCategoryType eCategory )
{
	if( !m_CategoryDlg[ eCategory ] ||
		m_SelectedCategory[ eCategory ] < 0 ||
		eCategory > eMixDlgCategoryType_2 )
		return NULL;

	PTRLISTPOS pos = m_CategoryDlg[ eCategory ]->GetListItem()->FindIndex( m_SelectedCategory[ eCategory ] );
	if( !pos )
		return NULL;

	ITEM* pItem = (ITEM*)m_CategoryDlg[ eCategory ]->GetListItem()->GetAt(pos);
	if( !pItem )
		return NULL;

	DWORD dwMultiDataKey = (eCategory > eMixDlgCategoryType_1 ? m_SelectedCategory[ eCategory - 1 ] : (WORD)-1);
	
	m_htCategoryInfo.StartGetMultiData( dwMultiDataKey );

	for(stCategoryInfo* pInfo = m_htCategoryInfo.GetMultiData();
		0 < pInfo;
		pInfo = m_htCategoryInfo.GetMultiData())
	{
		if( strcmp( pItem->string, pInfo->szName ) == 0 )
			return pInfo;
	}

	return NULL;
}

void CMixDialog::InitCategory()
{
	for( int i=0 ; i<eMixDlgCategoryType_Max ; ++i )
		if( !m_CategoryDlg[ i ] )
			return;

	// Category1 List Setting
	m_CategoryDlg[ eMixDlgCategoryType_1 ]->RemoveAll();

	m_htCategoryInfo.SetPositionHead();

	for(stCategoryInfo* pInfo = m_htCategoryInfo.GetData();
		0 < pInfo;
		pInfo = m_htCategoryInfo.GetData())
	{
		if( pInfo->categoryType == eMixDlgCategoryType_1 )
		{
			m_CategoryDlg[ eMixDlgCategoryType_1 ]->AddItem( pInfo->szName, RGB_HALF( 255, 255, 255 ) );
		}
	}

	// �ʱ� ����
	m_SelectedCategory[ eMixDlgCategoryType_1 ] = 0;
	m_SelectedCategory[ eMixDlgCategoryType_2 ] = 0;
	m_SelectedCategory[ eMixDlgCategoryType_3 ] = -1;
	m_SelectedCategory[ eMixDlgCategoryType_4 ] = -1;

	m_CategoryDlg[ eMixDlgCategoryType_1 ]->SetCurSelectedRowIdx( 0 );

	m_bChangedCategory[ eMixDlgCategoryType_1 ] = TRUE;

	m_dwDragItemIdx = 0;
	m_eMixDialogState = eMixDialogState_Normal;

	UpdateCategory();
}

void CMixDialog::UpdateCategory()
{
	// ��� ������ ��� �����Ѵ�.
	RemoveResult();

	// Category1 �� ������ ����� ��� Category2 List ����
	if( m_bChangedCategory[ eMixDlgCategoryType_1 ] )
	{
		m_bChangedCategory[ eMixDlgCategoryType_1 ] = FALSE;
		
		// ī�װ��� ����� ���õ� ��ũ��Ʈ �ʱ�ȭ
		ClearSelectedCategoryInfo();

		// �⺻ ���·� ����
		m_eMixDialogState = eMixDialogState_Normal;

		UpdateCategory( eMixDlgCategoryType_1 );

		// ���� ī�װ��� ����
		m_bChangedCategory[ eMixDlgCategoryType_2 ] = TRUE;
	}

	// Category2 �� ������ ����� ��� Category3 List ����
	if( m_bChangedCategory[ eMixDlgCategoryType_2 ] )
	{
		m_bChangedCategory[ eMixDlgCategoryType_2 ] = FALSE;

		// ī�װ��� ����� ���õ� ��ũ��Ʈ �ʱ�ȭ
		ClearSelectedCategoryInfo();

		// �⺻ ���·� ����
		m_eMixDialogState = eMixDialogState_Normal;

		UpdateCategory( eMixDlgCategoryType_2 );

		// ���� ī�װ��� ����
		m_bChangedCategory[ eMixDlgCategoryType_3 ] = TRUE;
	}

	// Category3 �� ������ ����� ��� ���ս� ���� ����
	if( m_bChangedCategory[ eMixDlgCategoryType_3 ] )
	{
		m_bChangedCategory[ eMixDlgCategoryType_3 ] = FALSE;

		// ī�װ��� ����� ���õ� ��ũ��Ʈ �ʱ�ȭ
		ClearSelectedCategoryInfo();

		UpdateCategory( eMixDlgCategoryType_3 );

		// ���� ī�װ��� ����
		m_bChangedCategory[ eMixDlgCategoryType_4 ] = TRUE;
	}

	// Category4 �� ������ ����� ��� ���� ��� ���� ����
	if( m_bChangedCategory[ eMixDlgCategoryType_4 ] )
	{
		m_bChangedCategory[ eMixDlgCategoryType_4 ] = FALSE;

		UpdateCategory( eMixDlgCategoryType_4 );
	}
}

void CMixDialog::UpdateCategoryByItemIdx( DWORD dwItemIdx )
{
	const MixScript* pMixScript = ITEMMGR->GetMixScript( dwItemIdx );
	if( !pMixScript )
	{
		CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 782 ) );
		return;
	}

	if( pMixScript->empty() )
		return;

	// ī�װ��� �ʱ�ȭ
	m_CategoryDlg[ eMixDlgCategoryType_1 ]->SetCurSelectedRowIdx( -1 );
	m_CategoryDlg[ eMixDlgCategoryType_2 ]->RemoveAll();
	m_CategoryDlg[ eMixDlgCategoryType_3 ]->RemoveAll();
	m_CategoryDlg[ eMixDlgCategoryType_4 ]->RemoveAll();

	m_SelectedCategory[ eMixDlgCategoryType_1 ] = -1;
	m_SelectedCategory[ eMixDlgCategoryType_2 ] = -1;
	m_SelectedCategory[ eMixDlgCategoryType_3 ] = -1;
	m_SelectedCategory[ eMixDlgCategoryType_4 ] = -1;

	ClearSelectedCategoryInfo();

	// ��� �������� �����Ѵ�.
	std::set<DWORD> addedResultItemIdx;
	for( MixScript::const_iterator it = pMixScript->begin() ; pMixScript->end() != it ; ++it )
	{
		const ItemMixResult&	result		= *it;
		const DWORD				itemIndex	= result.mItemIndex;

		// �̹� �߰��� Item�� ��� �߰����� �ʴ´�.
		if( addedResultItemIdx.find( itemIndex ) != addedResultItemIdx.end() )
			continue;

		ITEM_INFO* pIteminfo = ITEMMGR->GetItemInfo( itemIndex );
		if( !pIteminfo )
			continue;

		switch( m_eMixItemSortType )
		{
		case eMixItemSortType_ByName:
			{
				m_CategoryDlg[ eMixDlgCategoryType_3 ]->AddItem( pIteminfo->ItemName, RGB_HALF( 255, 255, 255 ), -1, pIteminfo->ItemIdx );
			}
			break;
		case eMixItemSortType_ByLevel:
			{
				if( pIteminfo->LimitLevel )
				{
					char txt[ MAX_LISTITEM_SIZE ] = {0,};
					sprintf( txt, RESRCMGR->GetMsg( 1156 ), pIteminfo->ItemName, pIteminfo->LimitLevel );
					m_CategoryDlg[ eMixDlgCategoryType_3 ]->AddItem( txt, RGB_HALF( 255, 255, 255 ), -1, pIteminfo->ItemIdx, pIteminfo->LimitLevel );
				}
				else
					m_CategoryDlg[ eMixDlgCategoryType_3 ]->AddItem( pIteminfo->ItemName, RGB_HALF( 255, 255, 255 ), -1, pIteminfo->ItemIdx, pIteminfo->LimitLevel );
			}
			break;
		}

		addedResultItemIdx.insert( itemIndex );
	}

	ITEM_INFO* pSourceItemInfo	=	ITEMMGR->GetItemInfo( dwItemIdx );

	m_pSearchNameEdit->SetEditText( pSourceItemInfo->ItemName );
		
	m_CategoryDlg[ eMixDlgCategoryType_3 ]->SetTopListItemIdx( 0 );
}

void CMixDialog::UpdateCategory( eMixDlgCategoryType eCategory )
{
	switch( eCategory )
	{
	case eMixDlgCategoryType_1:
		{
			// eMixDlgCategoryType_2 List ����
			// ���� ī�װ��� List ����
			m_CategoryDlg[ eMixDlgCategoryType_2 ]->RemoveAll();
			m_SelectedCategory[ eMixDlgCategoryType_2 ] = -1;

			// Category2 List Setting
			stCategoryInfo* pInfo = NULL;
			m_htCategoryInfo.StartGetMultiData( m_SelectedCategory[ eMixDlgCategoryType_1 ] );
			while( (pInfo = m_htCategoryInfo.GetMultiData()) != NULL )
			{
				m_CategoryDlg[ eMixDlgCategoryType_2 ]->AddItem( pInfo->szName, RGB_HALF( 255, 255, 255 ) );
			}

			m_CategoryDlg[ eMixDlgCategoryType_2 ]->SetTopListItemIdx( 0 );
		}
		break;
	case eMixDlgCategoryType_2:
		{
			// eMixDlgCategoryType_3 List ����

			// ���� Dialog ���¿� ���� List ��������� �޸� �Ѵ�
			switch( m_eMixDialogState )
			{
			case eMixDialogState_Normal:
				{
					// ���� ī�װ��� List ����
					m_CategoryDlg[ eMixDlgCategoryType_3 ]->RemoveAll();
					m_SelectedCategory[ eMixDlgCategoryType_3 ] = -1;

					stCategoryInfo* pCategoryInfo = GetSelectedCategoryInfo( eMixDlgCategoryType_2 );
					if( pCategoryInfo )
					{
						CYHHashTable<stMixResultItemInfo>& htMixInfoForScript = ITEMMGR->GetMixInfoForScriptTable();

						htMixInfoForScript.SetPositionHead();

						for(stMixResultItemInfo* pMixInfo = htMixInfoForScript.GetData();
							0 < pMixInfo;
							pMixInfo = htMixInfoForScript.GetData())
						{
							// 091012 ShinJS --- �ߺ��� ������� ��� ������� �ʴ´�.
							if( pMixInfo->bOverlappedResult )
								continue;

							ITEM_INFO* pIteminfo = ITEMMGR->GetItemInfo( pMixInfo->dwResultItemIdx );
							if( !pIteminfo )
								continue;

							// ��� �������� Type�� Category�� ���� �ʴ� ���
							if( pCategoryInfo->setItemTypeDetail.find( pIteminfo->dwTypeDetail ) == pCategoryInfo->setItemTypeDetail.end() )
								continue;

							// ���� ����� ���� �߰��� ������ �ٸ��� �ִ´�.
							switch( m_eMixItemSortType )
							{
							case eMixItemSortType_ByName:
								{
									m_CategoryDlg[ eMixDlgCategoryType_3 ]->AddItem( pIteminfo->ItemName, RGB_HALF( 255, 255, 255 ), -1, pIteminfo->ItemIdx );
								}
								break;
							case eMixItemSortType_ByLevel:
								{
									if( pIteminfo->LimitLevel )
									{
										char txt[ MAX_LISTITEM_SIZE ] = {0,};
										sprintf( txt, RESRCMGR->GetMsg( 1156 ), pIteminfo->ItemName, pIteminfo->LimitLevel );
										m_CategoryDlg[ eMixDlgCategoryType_3 ]->AddItem( txt, RGB_HALF( 255, 255, 255 ), -1, pIteminfo->ItemIdx, pIteminfo->LimitLevel );
									}
									else
										m_CategoryDlg[ eMixDlgCategoryType_3 ]->AddItem( pIteminfo->ItemName, RGB_HALF( 255, 255, 255 ), -1, pIteminfo->ItemIdx, pIteminfo->LimitLevel );
								}
								break;
							}
						}
					}

					m_CategoryDlg[ eMixDlgCategoryType_3 ]->SetTopListItemIdx( 0 );
				}
				break;
			case eMixDialogState_Drag:
				{
					UpdateCategoryByItemIdx( m_dwDragItemIdx );
				}
				break;
			case eMixDialogState_Search:
				{
					SearchMixResultItem();
				}
				break;
			}
		}
		break;
	case eMixDlgCategoryType_3:
		{
			// ���� ī�װ��� List ����
			m_CategoryDlg[ eMixDlgCategoryType_4 ]->RemoveAll();
			m_SelectedCategory[ eMixDlgCategoryType_4 ] = -1;

			// ���ս� ���� Update
			UpdateMixInfo();
		}
		break;
	case eMixDlgCategoryType_4:
		{
			// ���ս� ���ÿ� ���� ���� ���� ����
			UpdateMixScript();
		}
		break;
	}
}

void CMixDialog::UpdateHighCategory( DWORD dwResultItemIdx )
{
	// ��������ۿ� �ش��ϴ� CategorInfo�� ���Ѵ�.
	const ITEM_INFO* pResultItemInfo = ITEMMGR->GetItemInfo( dwResultItemIdx );
	stCategoryInfo* pCategoryInfo = NULL;
	stCategoryInfo* pResultCategory1Info = NULL;
	stCategoryInfo* pResultCategory2Info = NULL;
	
	// eMixDlgCategoryType_2 �� ������ ���Ѵ�.
	m_htCategoryInfo.SetPositionHead();
	while( (pCategoryInfo = m_htCategoryInfo.GetData()) != NULL )
	{
		if( pCategoryInfo->setItemTypeDetail.find( pResultItemInfo->dwTypeDetail ) != pCategoryInfo->setItemTypeDetail.end() )
		{
			pResultCategory2Info = pCategoryInfo;
			break;
		}
	}

	if( !pResultCategory2Info )
		return;

	// eMixDlgCategoryType_1 �� ������ ���Ѵ�.
	m_htCategoryInfo.StartGetMultiData( (WORD)-1 );
	while( (pCategoryInfo = m_htCategoryInfo.GetMultiData()) != NULL )
	{
		if( pCategoryInfo->wID == pResultCategory2Info->wHighCategoryID )
		{
			pResultCategory1Info = pCategoryInfo;
			break;
		}
	}

	if( !pResultCategory1Info )
		return;

	// eMixDlgCategoryType_1 ����
	int nCategoryIndex = GetCategoryIndexFromName( eMixDlgCategoryType_1, pResultCategory1Info->szName );
	m_CategoryDlg[ eMixDlgCategoryType_1 ]->SetCurSelectedRowIdx( nCategoryIndex );
	m_SelectedCategory[ eMixDlgCategoryType_1 ] = nCategoryIndex;

	// eMixDlgCategoryType_2�� List ����
	UpdateCategory( eMixDlgCategoryType_1 );


	// eMixDlgCategoryType_2 ����
	nCategoryIndex = GetCategoryIndexFromName( eMixDlgCategoryType_2, pResultCategory2Info->szName );
	m_CategoryDlg[ eMixDlgCategoryType_2 ]->SetCurSelectedRowIdx( nCategoryIndex );
	m_SelectedCategory[ eMixDlgCategoryType_2 ] = nCategoryIndex;
}

void CMixDialog::UpdateMixInfo()
{
	ITEM* pCategoryItem = GetSelectedItemFromListDlg( eMixDlgCategoryType_3 );
	if( !pCategoryItem )
		return;

	// ��� Item���� MixScript ��� ���ϱ�
	const DWORD dwItemIdx = pCategoryItem->dwData;
	m_htEqualSelectedMixResult.RemoveAll();
	ITEMMGR->GetMixResultFromResultItemIdx( dwItemIdx, &m_htEqualSelectedMixResult );
	if( m_htEqualSelectedMixResult.GetDataNum() <= 0 )
		return;

	// ��� �������� ���õ� ��� ���� ���ս� Category �ʱ�ȭ/Update
	if( m_SelectedCategory[ eMixDlgCategoryType_3 ] >= 0 )
	{
		// ���� ī�װ��� List ����
		DWORD dwMixResultCnt = 1;

		m_htEqualSelectedMixResult.SetPositionHead();

		for(ItemMixResult* pMixResult = m_htEqualSelectedMixResult.GetData();
			0 < pMixResult;
			pMixResult = m_htEqualSelectedMixResult.GetData())
		{
			char szItemName[ 256 ] = {0,};
			sprintf( szItemName, RESRCMGR->GetMsg( 1153 ), dwMixResultCnt++ );
			m_CategoryDlg[ eMixDlgCategoryType_4 ]->AddItem( szItemName, RGB_HALF( 255, 255, 255 ) );
		}

		m_CategoryDlg[ eMixDlgCategoryType_4 ]->SetCurSelectedRowIdx( 0 );
		m_SelectedCategory[ eMixDlgCategoryType_4 ] = 0;
	}
	else
	{
		m_CategoryDlg[ eMixDlgCategoryType_4 ]->SetCurSelectedRowIdx( -1 );
		m_SelectedCategory[ eMixDlgCategoryType_4 ] = -1;
	}

	m_CategoryDlg[ eMixDlgCategoryType_4 ]->SetTopListItemIdx( 0 );

	// ���� ī�װ��� ������ �ʿ��� ���(�˻�, Drag ������ ���)
	if( m_eMixDialogState != eMixDialogState_Normal )
	{
		UpdateHighCategory( dwItemIdx );
	}
}

void CMixDialog::UpdateMixScript()
{
	// ���õ� ���� ���ų� ���ս��� ���� ���
	if( m_SelectedCategory[ eMixDlgCategoryType_4 ] < 0 ||
		m_htEqualSelectedMixResult.GetDataNum() <= 0 )
		return;

	// ���õ� ���� �������� ��ũ��Ʈ ����
	m_pSelectedMixResult = m_htEqualSelectedMixResult.GetData( m_SelectedCategory[ eMixDlgCategoryType_4 ] );
	// ���ս� ������ �߸��� ���
	if( !m_pSelectedMixResult )
		return;

	// ��� ������ Icon Update
	ITEM_INFO* pItemInfo = ITEMMGR->GetItemInfo( m_pSelectedMixResult->mItemIndex );
	if( !pItemInfo )
		return;	
	// ��� ������ ��ġ ����
	LONG	absX	=	(LONG)mResultIcon->GetAbsX();
	LONG	absY	=	(LONG)mResultIcon->GetAbsY();

	cImage image;
	mResultIcon->Init( absX, absY, DEFAULT_ICONSIZE, DEFAULT_ICONSIZE, ITEMMGR->GetIconImage( pItemInfo->ItemIdx, &image ), 0 );
	ITEMBASE itembase;
	memset( &itembase, 0, sizeof(ITEMBASE) );
	itembase.wIconIdx = pItemInfo->ItemIdx;

	mResultIcon->SetItemBaseInfo( itembase );
	mResultIcon->SetIconType( eIconType_AllItem );
	mResultIcon->SetData( pItemInfo->ItemIdx );
	mResultIcon->SetVisibleDurability( FALSE );

	ITEMMGR->AddToolTip( mResultIcon );
	mResultIconDialog->AddIcon( 0, mResultIcon );


	// ���� ���� Update
	PutDescription( m_pSelectedMixResult );

	// ���� �������� ��ġ�� �ʴ°�� spin btn/edit�� ������� �ʵ��� �Ѵ�
	if( ITEMMGR->IsDupItem( m_pSelectedMixResult->mItemIndex ) )
	{
		m_pCntSpin->SetUnit( 1 );
		m_pCntSpin->SetReadOnly( FALSE );
	}
	else
	{
		m_pCntSpin->SetUnit( 0 );
		m_pCntSpin->SetReadOnly( TRUE );
	}
	m_pCntSpin->SetValue( 1 );
	m_MixItemCnt = 1;

	// ���� ��� Update
	UpdateOwnStuff();

	m_pCntSpin->SetValue( m_dwMixResultCnt );

	// �����ݰ� ���� ��� Update
	UpdateCostInfo();
}

void CMixDialog::UpdateOwnStuff()
{
	if( !m_pSelectedMixResult )
		return;

	CInventoryExDialog* pInvenDlg = (CInventoryExDialog*)WINDOWMGR->GetWindowForID( IN_INVENTORYDLG );
	ASSERT( pInvenDlg );

	DWORD index	= 0;

	int		nMinResultCnt	=	0;
	int		nCurrentMinCnt	=	0;

	//�ʱⰪ ����
	ItemMixResult::Material::const_iterator firstIter = m_pSelectedMixResult->mMaterial.begin();
	const DWORD dwStuffItemIdx = firstIter->first;
	const DURTYPE stuffItemNeedCntFirst = firstIter->second * m_MixItemCnt;
	const DURTYPE curItemCntFirst = pInvenDlg->GetTotalItemDurability( dwStuffItemIdx );

	if( stuffItemNeedCntFirst )
		nMinResultCnt	=	curItemCntFirst	/	stuffItemNeedCntFirst;


	for( ItemMixResult::Material::const_iterator iter = m_pSelectedMixResult->mMaterial.begin() ; iter != m_pSelectedMixResult->mMaterial.end() ; ++iter, ++index )
	{
		if( mStuffIcon.size() <= index )
		{
			ASSERT( 0 && "��� ������ �׸��� �� �����ܺ��� ����" );
			break;
		}

		CItemShow*				itemShow	= mStuffIcon[ index ];
		const DWORD dwStuffItemIdx = iter->first;
		const DURTYPE stuffItemNeedCnt = iter->second * m_MixItemCnt;
		const DURTYPE curItemCnt = pInvenDlg->GetTotalItemDurability( dwStuffItemIdx );
		//�κ��丮�� ������ ������ ����
		const DURTYPE ownItemCount	=	GAMEIN->GetInventoryDialog()->GetTotalItemDurability( dwStuffItemIdx );

		itemShow->SetData( dwStuffItemIdx );
		itemShow->Init( dwStuffItemIdx, curItemCnt );
		itemShow->SetVisibleDurability( TRUE );

		// ��� ������ ���� Lock ó��
		if( curItemCnt >= stuffItemNeedCnt && curItemCnt != 0 ) //|| stuffItemNeedCnt <= 0 )
			itemShow->SetLock( false );
		else 
			itemShow->SetLock( true );
		

		ITEMMGR->AddToolTip( itemShow );

		mIconGridDialog->AddIcon( WORD( index ), itemShow );
		if( ownItemCount != 0 && stuffItemNeedCnt != 0 )
			nCurrentMinCnt	=	ownItemCount / stuffItemNeedCnt;
		else
			nCurrentMinCnt	=	0;

		
		if( nMinResultCnt > nCurrentMinCnt )
		{
			nMinResultCnt = nCurrentMinCnt;
		}

		// ��� �ʿ� ���� ToolTip �߰�	//	�ʿ䰳�� / ���������� ����	�����Ұ�� �����

		char szToolTip[ MAX_PATH ]	=	{ 0, };
		DWORD	dwFontColor;
		
		sprintf( szToolTip, RESRCMGR->GetMsg( 1152 ), ownItemCount, iter->second * m_MixItemCnt );
			if(	stuffItemNeedCnt <= ownItemCount && ownItemCount != 0 )	//������ �������� �ʿ�� �������
			dwFontColor	=	RGBA_MAKE( 255, 255, 0, 255 );
		else
			dwFontColor =	RGBA_MAKE( 255, 0, 0, 255 );
		itemShow->AddToolTipLine( szToolTip, dwFontColor );
	}
	// �������� �ʴ� �������� ��� ���հ����� �ִ�� 1�� ����
	if( !(ITEMMGR->IsDupItem( m_pSelectedMixResult->mItemIndex )) && nMinResultCnt >= 1 )
		m_dwMixResultCnt	=	1;
	else
		m_dwMixResultCnt	=	nMinResultCnt;
}

void CMixDialog::UpdateCostInfo()
{
	if( !m_pInvenMoney || !m_pMixCost )
		return;

	DURTYPE mixItemCnt = (DURTYPE)m_pCntSpin->GetValue();

	// ���� �ݾ� Update
	m_pInvenMoney->SetStaticText( AddComma( HERO->GetMoney() ) );

	if( !m_pSelectedMixResult )
		return;

	DWORDEX	dwMixCost	=	m_pSelectedMixResult->mMoney * mixItemCnt;


	// ���� ��� Update
	m_pMixCost->SetStaticText( AddComma( dwMixCost ) );

	if( dwMixCost < HERO->GetMoney() )
		m_bIsEnoughMoney	=	TRUE;
	else
		m_bIsEnoughMoney	=	FALSE;

}

void CMixDialog::SetSortType( eMixItemSortType eSortType )
{
	switch( eSortType )
	{
	case eMixItemSortType_ByName:
		{
			m_CategoryDlg[ eMixDlgCategoryType_3 ]->SetSortType( eListItemSortType_StringGreater );
		}
		break;
	case eMixItemSortType_ByLevel:
		{
			m_CategoryDlg[ eMixDlgCategoryType_3 ]->SetSortType( eListItemSortType_SortDataGreater );
		}
		break;
	}

	// ���� ����� ����� ��� List �ٽ� ����
	if( m_eMixItemSortType != eSortType )
	{
		// ���� Type ����
		m_eMixItemSortType = eSortType;

		// ���� ���� �� Update
		UpdateCategory( eMixDlgCategoryType_2 );
	}
}

void CMixDialog::ClearSelectedCategoryInfo()
{
	m_pSelectedMixResult = NULL;

	// ���õ� ���� �������� ��ũ��Ʈ Table (�⺻ �������۰� ��� �������� ���� ������ �ٸ� ��ũ��Ʈ�� ����) ����	
	m_htEqualSelectedMixResult.RemoveAll();
}

int CMixDialog::GetCategoryIndexFromName( eMixDlgCategoryType eCategory, const char* szItemName )
{
	int nCategoryCnt = m_CategoryDlg[ eCategory ]->GetListItem()->GetCount();
	for( int index = 0 ; index < nCategoryCnt ; ++index )
	{
		PTRLISTPOS pos = m_CategoryDlg[ eCategory ]->GetListItem()->FindIndex( index );
		if( !pos )
			continue;

		ITEM* pItem = (ITEM*)m_CategoryDlg[ eCategory ]->GetListItem()->GetAt( pos );
		if( !pItem )
			continue;

		if( strcmp( pItem->string, szItemName ) == 0 )
			return index;
	}

	return -1;
}

int CMixDialog::GetCategoryIndexFromItemIdx( eMixDlgCategoryType eCategory, DWORD dwItemIdx )
{
	int nCategoryCnt = m_CategoryDlg[ eCategory ]->GetListItem()->GetCount();
	for( int index = 0 ; index < nCategoryCnt ; ++index )
	{
		PTRLISTPOS pos = m_CategoryDlg[ eCategory ]->GetListItem()->FindIndex( index );
		if( !pos )
			continue;

		ITEM* pItem = (ITEM*)m_CategoryDlg[ eCategory ]->GetListItem()->GetAt( pos );
		if( !pItem )
			continue;

		if( pItem->dwData == dwItemIdx )
			return index;
	}

	return -1;
}

void CMixDialog::SearchMixResultItem()
{
	char	tempSearchName[MAX_PATH]	=	{ 0, };
	char	tempItemName[MAX_PATH]		=	{ 0, };
	// ī�װ��� �ʱ�ȭ
	m_CategoryDlg[ eMixDlgCategoryType_3 ]->RemoveAll();
	m_CategoryDlg[ eMixDlgCategoryType_4 ]->RemoveAll();

	m_SelectedCategory[ eMixDlgCategoryType_3 ] = -1;
	m_SelectedCategory[ eMixDlgCategoryType_4 ] = -1;

	ClearSelectedCategoryInfo();

	// �˻� ����� ���� ����
	stCategoryInfo* pCategoryInfo = NULL;
	stCategoryInfo* pInfo = GetSelectedCategoryInfo( eMixDlgCategoryType_1 );
	if( pInfo )
		pCategoryInfo = pInfo;

	pInfo = GetSelectedCategoryInfo( eMixDlgCategoryType_2 );
	if( pInfo && pCategoryInfo->wID != 0 )	// 1�� ī�װ����� ��ü �ϰ�� 2��ī�װ����� �������� ����
		pCategoryInfo = pInfo;


	// �˻� ���� ����
	WORD wMinLv = WORD( atoi(m_pSearchMinLv->GetEditText()) );
	WORD wMaxLv = WORD( atoi(m_pSearchMaxLv->GetEditText()) );

	// Lv Text ����
	if( wMinLv < 1 )
		m_pSearchMinLv->SetEditText( "" );
	if( wMaxLv < 1 )
		m_pSearchMaxLv->SetEditText( "" );

	wMaxLv = (wMaxLv > 0 ? wMaxLv : WORD(-1));

	if( wMinLv > wMaxLv )
	{
		// �˻� ������ ��Ȯ�ϰ� �Է����ּ���
		CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1989 ) );
		return;
	}

	// �˻��� ����
	const char* szSearchName = m_pSearchNameEdit->GetEditText();

	CYHHashTable<stMixResultItemInfo>& htMixInfoForScript = ITEMMGR->GetMixInfoForScriptTable();

	htMixInfoForScript.SetPositionHead();

	for(stMixResultItemInfo* pMixInfo = htMixInfoForScript.GetData();
		0 < pMixInfo;
		pMixInfo = htMixInfoForScript.GetData())
	{
		// 091012 ShinJS --- �ߺ��� ������� ��� ������� �ʴ´�.
		if( pMixInfo->bOverlappedResult )
			continue;

		ITEM_INFO* pIteminfo = ITEMMGR->GetItemInfo( pMixInfo->dwResultItemIdx );
		if( !pIteminfo )
			continue;

		// ���� ���� Ȯ��
		if( wMinLv > pIteminfo->LimitLevel || pIteminfo->LimitLevel > wMaxLv )
			continue;

		// ī�װ��� ���� Ȯ��
		if( pCategoryInfo )
		{
			switch( pCategoryInfo->categoryType )
			{
			case eMixDlgCategoryType_1:
				{
					if( pCategoryInfo->setItemType.find( pIteminfo->dwType ) == pCategoryInfo->setItemType.end() )
						continue;
				}
				break;
			case eMixDlgCategoryType_2:
				{
					if( pCategoryInfo->setItemTypeDetail.find( pIteminfo->dwTypeDetail ) == pCategoryInfo->setItemTypeDetail.end() )
						continue;
				}
				break;
			}
		}

		strcpy( tempSearchName, szSearchName );
		strcpy( tempItemName, pIteminfo->ItemName );

		//�빮�ڷ� ��ȯ
		strupr( tempSearchName );
		strupr( tempItemName );


		// �˻��� Ȯ��
		if( strstr( tempItemName, tempSearchName ) == NULL )
			continue;

		// �˻� ������ �߰�, ���� ����� ���� �߰��� ������ �ٸ��� �ִ´�.
		switch( m_eMixItemSortType )
		{
		case eMixItemSortType_ByName:
			{
				m_CategoryDlg[ eMixDlgCategoryType_3 ]->AddItem( pIteminfo->ItemName, RGB_HALF( 255, 255, 255 ), -1, pIteminfo->ItemIdx );
			}
			break;
		case eMixItemSortType_ByLevel:
			{
				if( pIteminfo->LimitLevel )
				{
					char txt[ MAX_LISTITEM_SIZE ] = {0,};
					sprintf( txt, RESRCMGR->GetMsg( 1156 ), pIteminfo->ItemName, pIteminfo->LimitLevel );
					m_CategoryDlg[ eMixDlgCategoryType_3 ]->AddItem( txt, RGB_HALF( 255, 255, 255 ), -1, pIteminfo->ItemIdx, pIteminfo->LimitLevel );
				}
				else
					m_CategoryDlg[ eMixDlgCategoryType_3 ]->AddItem( pIteminfo->ItemName, RGB_HALF( 255, 255, 255 ), -1, pIteminfo->ItemIdx, pIteminfo->LimitLevel );
			}
			break;
		}
	}
	
	m_CategoryDlg[ eMixDlgCategoryType_3 ]->SetTopListItemIdx( 0 );

	char txt[ MAX_LISTITEM_SIZE ] = {0,};
	sprintf( txt, CHATMGR->GetChatMsg( 1988 ), m_CategoryDlg[ eMixDlgCategoryType_3 ]->GetItemCount() ); // �˻���� %d ��
	m_CategoryDlg[ eMixDlgCategoryType_4 ]->AddItem( txt, RGB_HALF( 255, 255, 255 ), -1 );
}

void CMixDialog::PushCatagory1BTN()
{
	m_CategoryDlg[ eMixDlgCategoryType_2 ]->SetActive( FALSE );
	m_pSortTypeCombo->SetActive( FALSE );
	m_pSearchConditionInitBTN->SetActive( FALSE );
}

void CMixDialog::PushCatagory2BTN()
{
	m_pSortTypeCombo->SetActive( FALSE );
	m_pSearchConditionInitBTN->SetActive( FALSE );
}

void CMixDialog::CatagoryScrollEnd()
{
	if( m_pSortTypeCombo &&m_pSearchConditionInitBTN )
	{
		m_CategoryDlg[ eMixDlgCategoryType_1 ]->SetActive( TRUE );
		m_CategoryDlg[ eMixDlgCategoryType_2 ]->SetActive( TRUE );

		m_CategoryDlg[ eMixDlgCategoryType_1 ]->SetShowScroll( FALSE );
		m_CategoryDlg[ eMixDlgCategoryType_2 ]->SetShowScroll( FALSE );

		m_pSortTypeCombo->SetActive( TRUE );
		m_pSearchConditionInitBTN->SetActive( TRUE );
	}
}