#include "stdafx.h"
#include "ReinforceDlg.h"
#include "Item.h"
#include "VirtualItem.h"
#include "INTERFACE\cIconDialog.h"
#include "INTERFACE\cIconGridDialog.h"
#include "WindowIDEnum.h"
#include "ItemManager.h"
#include "ChatManager.h"
#include "Interface/cScriptManager.h"
#include "Interface/cWindowManager.h"
#include "ObjectManager.h"
#include "Interface/cStatic.h"
#include "InventoryExDialog.h"
#include "ProgressDialog.h"
#include "cMsgBox.h"
#include "GameIn.h"
#include "FishingDialog.h"

CReinforceDlg::CReinforceDlg() :
mItemIcon		( new CVirtualItem ),
mBaseDialog		( 0 ),
mIconGridDialog	( 0 ),
mTextQuantity	( 0 ),
mQuantity		( 0 ),
mMaxQuantity	( 100 ),
mIsReinforcing	( FALSE ),
// 080228 LUJ, �Ϲ�/��ȣ ��� ǥ������ ��Ʈ�� �ʱ�ȭ
mTitle( 0 ),
mProtectTitle( 0 ),
mProtectSymbol( 0 )
{
	// ���� ��� ������
	{
		cImage image;

		SCRIPTMGR->GetImage( 0, &image, PFT_HARDPATH );

		mItemIcon->SetToolTip( "", RGB_HALF( 255, 255, 255), &image );
		mItemIcon->SetMovable(FALSE);
	}

	// 080228 LUJ, ��ȣ ������ �ʱ�ȭ
	ZeroMemory( &mProtectionItem, sizeof( mProtectionItem ) );

	// 080228 LUJ, ��ȭ �� ��ġ �ʱ�ȭ
	ZeroMemory( &mProtectedOption, sizeof( mProtectedOption ) );
	// 080929 LUJ, ���� ������ �ʱ�ȭ
	ZeroMemory( &mSupportItem, sizeof( mSupportItem ) );
}


CReinforceDlg::~CReinforceDlg()
{
	Release();

	SAFE_DELETE( mItemIcon );
}


void CReinforceDlg::Linking()
{
	Release();

	mBaseDialog		= ( cIconDialog* )		GetWindowForID( ITR_REINFORCEICONDLG );
	mIconGridDialog = ( cIconGridDialog* )	GetWindowForID( ITR_REINFORCEICONGRIDDLG );

	{
		mTextQuantity	= (cStatic*)GetWindowForID( ITR_REINFORCEGRAVITY );
	
		cImage image;
		SCRIPTMGR->GetImage( 0, &image, PFT_HARDPATH );
		mTextQuantity->SetToolTip( "", RGBA_MAKE(255, 255, 255, 255), &image, TTCLR_DEFAULT );
	}	

	// ������ �Ҵ�
	{
		int size = mIconGridDialog->GetCellNum();

		mMaterialIcon.reserve( size );

		ITEMBASE base;
		ZeroMemory( &base, sizeof( base ) );

		cImage image;

		while( size-- )
		{
			CItem* item = new CItem( &base );

			mMaterialIcon.push_back( item );
		}
	}

	{
		char line[ MAX_PATH ];
		sprintf( line, "%3d/%3d", mQuantity, mMaxQuantity );

		mTextQuantity->SetStaticText( line );
	}

	// 080228 LUJ, �Ϲ�/��ȣ ��� ǥ�� �� �������̽� ��ũ
	{		
		mTitle			= GetWindowForID( ITR_REINFORCETITLE );
		mProtectTitle	= GetWindowForID( ITR_REINFORCE_PROTECT_TITLE );
		mProtectSymbol	= GetWindowForID( ITR_REINFORCE_PROTECT_SYMBOL );
	}
}


// 080228 LUJ,	��ȣ �������� ����� ���, ��ȭ �Ӽ��� �ϳ��� ���⸸ ��ġ��ų �� �ֵ��� ��.
//				��ȣ �������� ����� ���, �� ������ ������ ��ġ�� �� �ֵ��� ��
BOOL CReinforceDlg::FakeMoveIcon(LONG x, LONG y, cIcon* icon)
{
	if( !	icon						||
			WT_ITEM != icon->GetType() ||
			mIsReinforcing )
	{
		return FALSE;
	}
	
	CItem* item = ( CItem* )icon;

	// �κ��丮�̿ܿ� ������ FALSE
	if( ! ITEMMGR->IsEqualTableIdxForPos( eItemTable_Inventory, item->GetPosition() ) )
	{
		CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 787 ) );
		return FALSE;
	}

	const ITEM_INFO* info = item->GetItemInfo();

	if( ! info )
	{
		return FALSE;
	}

	if( ITEMMGR->IsDupItem( item->GetItemIdx() ) )
	{
		AddMaterial( *item );
	}
	else if( info->Improvement )
	{
		// 080228 LUJ, ��ȣ �������� �� ��� �Ӽ��� �ϳ��� ��ȭ�� ��� �ø� �� �ִ�
		if( mProtectionItem.dwDBIdx )
		{
			ITEM_OPTION option = ITEMMGR->GetOption( item->GetItemBaseInfo() );

			// 080228 LUJ, ��æƮ�� ���� ���� ��� ����� �Ѵ�
			if(const EnchantScript* enchantScript = g_CGameResourceManager.GetEnchantScript(option.mEnchant.mIndex))
			{
				g_CGameResourceManager.AddEnchantValue(
					option,
					enchantScript->mAbility,
					-1 * info->EnchantDeterm * option.mEnchant.mLevel);
			}

			const ITEM_OPTION::Reinforce& protectedOption = option.mReinforce;

			// 080228 LUJ, ��ȣ�� �ɼ��� ���� ��� ��ȭ���� �ʴ´�
			{
				const ITEM_OPTION::Reinforce emptyOption = { 0 };

				if( ! memcmp( &emptyOption, &protectedOption, sizeof( emptyOption ) ) )
				{
					CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1453 ) );
					return FALSE;
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
				CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1457 ) );
				return FALSE;
			}			
		}

		// 080929 LUJ, ���� ������ ��� �� ������ ��� ������ �о�, ����� Ÿ������ �˻��Ѵ�
		{
			const ReinforceSupportScript* supportScript = g_CGameResourceManager.GetReinforceSupportScript( mSupportItem.wIconIdx );
			
			if( supportScript )
			{
				BOOL isValid = FALSE;

				for(	ReinforceSupportScript::MaterialSet::const_iterator it = supportScript->mMaterialSet.begin();
						supportScript->mMaterialSet.end() != it;
						++it )
				{
					const ReinforceScript* const script = g_CGameResourceManager.GetReinforceScript( *it );

					if( ! script )
					{
						continue;
					}

					const BOOL isSameRareType	= ( ITEMMGR->IsRare( info ) == script->mForRare );
					const BOOL isReinforceable	= ( script->mSlot.end() != script->mSlot.find(info->EquipSlot) );
					
					if( ! isSameRareType	||
						! isReinforceable )
					{
						continue;
					}

					isValid = TRUE;
					break;
				}

				if( ! isValid )
				{
					const ITEM_INFO*	supportItemInfo = ITEMMGR->GetItemInfo( mSupportItem.wIconIdx );
					const char*			text			= CHATMGR->GetChatMsg( 1718 );

					WINDOWMGR->MsgBox(
						MBI_NOTICE,
						MBT_OK,
						const_cast< char* >( text ),
						supportItemInfo ? supportItemInfo->ItemName : "?" );
					return FALSE;
				}
			}
		}

		RemoveMaterial();

		// �̹� ��ϵ� ������ ������ ����
		if( ! mBaseDialog->IsAddable( 0 ) )
		{
			CItem* old = ( CItem* )( mItemIcon->GetLinkItem() );

			// 080108 LUJ, �÷��̾� ���¿� ���� �ʱ�ȭ�ؾ��Ѵ�
			ITEMMGR->RefreshItem( old );

			old->SetLock( FALSE );
		}

		mBaseDialog->DeleteIcon( 0, 0 );

		mItemIcon->SetData( item->GetItemIdx()  );
		mItemIcon->SetLinkItem( item );
		
		ITEMMGR->AddToolTip( mItemIcon );

		mBaseDialog->AddIcon( 0, mItemIcon );

		//item->SetUseParam( 1 );
		item->SetLock(TRUE);
		
		ITEMMGR->AddToolTip( mItemIcon );
	}
	else
	{
		CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg(809) );
	}
	
	return FALSE;
}


void CReinforceDlg::OnActionEvent(LONG lId, void * p, DWORD we)
{
	switch( lId )
	{
	case ITR_REINFORCEOKBTN:
		{
			Submit( TRUE );
			break;
		}
	case ITR_REINFORCECANCELBTN:
		{
			SetActive( FALSE );
			break;
		}
	case ITR_RFGUIDEBTN:
		{
			cDialog* dialog = WINDOWMGR->GetWindowForID( RFGUIDE_DLG );
			ASSERT( dialog );

			dialog->SetActive( ! dialog->IsActive() );
		}
	}
}


void CReinforceDlg::Release()
{
	for(	MaterialIcon::iterator it = mMaterialIcon.begin();
			mMaterialIcon.end() != it;
			++it )
	{
		CItem* item = *it;

		SAFE_DELETE( item );
	}

	mMaterialIcon.clear();
}


// 080228 LUJ, �������͸� ���� ���� ���������� ����
// 080228 LUJ, ��ȣ ������ ��� �� �� ������ �����۸� ����ϰ� ��
void CReinforceDlg::AddMaterial( CItem& item )
{
	// ��ȭ�� �������� ������ ��ϵ��� �ʴ´�
	CItem* targetItem = 0;
	{
		cIcon* icon = mBaseDialog->GetIconForIdx( 0 );

		if( !	icon ||
				mItemIcon != icon )
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg(802) );
			return;
		}

		targetItem = ( CItem* )( mItemIcon->GetLinkItem() );

		if( !	targetItem &&
				WT_ITEM != targetItem->GetType())
		{
			return;
		}
	}

	const ReinforceScript* script = g_CGameResourceManager.GetReinforceScript( item.GetItemIdx() );

	if( ! script )
	{
		CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg(803) );
		return;
	}

	const ReinforceSupportScript* supportScript = g_CGameResourceManager.GetReinforceSupportScript( mSupportItem.wIconIdx );

	// 080929 LUJ, ���� �������� ���� ���, ��ȭ ���� ��ũ��Ʈ���� ��ᰡ ��� �������� üũ�Ѵ�
	if( supportScript )
	{
		if( supportScript->mMaterialSet.end() == supportScript->mMaterialSet.find( item.GetItemIdx() ) )
		{
			const ITEM_INFO*	supportItemInfo = ITEMMGR->GetItemInfo( mSupportItem.wIconIdx );
			const char*			text			= CHATMGR->GetChatMsg( 1718 );

			WINDOWMGR->MsgBox(
				MBI_NOTICE,
				MBT_OK,
				const_cast< char* >( text ),
				supportItemInfo ? supportItemInfo->ItemName : "?" );
			return;
		}
	}
	// �Ϲ� �������� ���� �Ϲ� ��ȭ ��ᰡ, ���� �������� ���� ���� ��ȭ ��ᰡ ��ϵǾ�� �Ѵ�.
	// 080929 LUJ, ���� �������� ������ �ʴ� ��쿡�� ���� ó���Ѵ�
	else
	{
		CItem*		source = ( CItem* )( mItemIcon->GetLinkItem() );
		const BOOL	isRare = ITEMMGR->IsRare( source->GetItemInfo() );

		if( isRare && ! script->mForRare )
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg(804) );
			return;
		}

		if( ! isRare && script->mForRare )
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 911 ) );
			return;
		}
	}

	// �ش� ���� ��ȭ�� �� �ִ� ���������� �˻�
	{
		const ITEM_INFO* info = ITEMMGR->GetItemInfo( mItemIcon->GetSrcItemIdx() );
		
		if( script->mSlot.end() == script->mSlot.find( info->EquipSlot ) )
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg(803) );
			return;
		}
	}

	// 080228 LUJ, ��ȣ �������� ����� ���, �� ������ ��Ḹ �ø� �� �ִ�
	if( mProtectionItem.dwDBIdx )
	{
		cIcon* icon = mIconGridDialog->GetIconForIdx( 0 );

		if( icon &&
			icon->GetData() != item.GetItemIdx() )
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1458 ) );
			return;
		}

		// 080228 LUJ, ��ȣ ������ ��� ������ ��ȭ �ɼ��� ���� ��ġ�� ������ ��ȭ��Ű�� �Ӽ��� ��ġ�ؾ���
		{
			// 080228 LUJ, ��æƮ�� ���� ���� ��� ����� �Ѵ�
			ITEM_OPTION::Reinforce protectedOption = { 0 };
			{
				ITEM_OPTION	option = ITEMMGR->GetOption(
					targetItem->GetItemBaseInfo());
				const EnchantScript* enchantScript = g_CGameResourceManager.GetEnchantScript(
					option.mEnchant.mIndex);
				const ITEM_INFO* info = targetItem->GetItemInfo();

				if( info &&
					enchantScript )
				{
					g_CGameResourceManager.AddEnchantValue(
						option,
						enchantScript->mAbility,
						-1 * info->EnchantDeterm * option.mEnchant.mLevel);
				}

				protectedOption = option.mReinforce;

				// 080228 LUJ, �̸��� �ʱ�ȭ���� ������, ���� �� �׻� �����Ѵ�
				ZeroMemory( &( protectedOption.mMadeBy ), sizeof( protectedOption.mMadeBy ) );
			}

			// 080228 LUJ, �ش� ������ ��ȭ�ϴ� �Ӽ��� �ɼ� ������ ���, �� �Ӽ��� ��ȭ�Ǿ����� Ȯ���Ѵ�
			{
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
					CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1459 ) );
					return;
				}
			}
		}
	}

	for( WORD i = 0; i < mMaterialIcon.size(); ++i )
	{
		if( ! mIconGridDialog->IsAddable( i ) )
		{
			continue;
		}

		// ��� ���� ����
		{
			if( mMaxQuantity < mQuantity + int( item.GetDurability() ) )
				mQuantity = mMaxQuantity;
			else
				mQuantity += int( item.GetDurability() );

			char line[ MAX_PATH ] = { 0 };
			sprintf( line, "%3d/%d", mQuantity, mMaxQuantity );

			mTextQuantity->SetStaticText( line );
		}

		// ��� ������ ���		
		{
			CItem* material = mMaterialIcon[ i ];
			cImage image;

			material->Init(
				0,
				0,
				DEFAULT_ICONSIZE,
				DEFAULT_ICONSIZE,
				ITEMMGR->GetIconImage( item.GetItemIdx(), &image ),
				0 );

			material->SetIconType( eIconType_AllItem );
			material->SetData( item.GetItemIdx() );
			material->SetItemBaseInfo( item.GetItemBaseInfo() );
			material->SetDurability( mQuantity );

			// 080228 LUJ, ������ �̵��� ������Ų��
			material->SetMovable( FALSE );

			ITEMMGR->AddToolTip( material );
			mIconGridDialog->AddIcon( i, material );
		}

		// ���� �����ۿ��� ��ϵǾ����� �˸��� ���� ������ 0���� �ϰ� ��� ǥ���Ѵ�
		{
			if( ITEMMGR->IsDupItem( item.GetItemIdx() ) )
			{
				item.SetDurability( item.GetDurability() - mQuantity );
			}

			//item->SetUseParam( 1 );
			item.SetLock( TRUE );
		}

		return;
	}

	CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg(806) );
}


void CReinforceDlg::Submit( BOOL isCheck )
{
	ZeroMemory( &mMessage, sizeof( mMessage ) );

	if( mIsReinforcing			||
		mBaseDialog->IsAddable( 0 ) )
	{
		CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 802 ) );
		return;
	}
	
	{
		CBaseItem* item = mItemIcon->GetLinkItem();

		if( ! item )
		{
			return;
		}

		mMessage.Category				= MP_ITEM;
		mMessage.Protocol				= MP_ITEM_REINFORCE_SYN;
		mMessage.dwObjectID				= HEROID;
		mMessage.mSourceItemIndex		= item->GetItemIdx();
		mMessage.mSourceItemPosition	= item->GetPosition();
		mMessage.mSourceItemDbIndex		= item->GetDBIdx();
		mMessage.mSize					= 0;
		mMessage.mSupportItem			= mSupportItem;
	}	

	// ���� ���Ƽ� �޽����� �����Ѵ�(��ġ�� ����)
	for( WORD i = 0; i < mMaterialIcon.size(); ++i )
	{
		if( mIconGridDialog->IsAddable( i ) )
		{
			break;
		}

		// ������ �߰�
		{
			CItem* material = mMaterialIcon[ i ];

			mMessage.mItem[ mMessage.mSize ] = material->GetItemBaseInfo();

			CItem*	original = ITEMMGR->GetItem( material->GetDBIdx() );

			mMessage.mItem[ mMessage.mSize++ ].Durability = original->GetDurability() + material->GetDurability();
		}
	}

	if( ! mMessage.mSize )
	{
		CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 912 ) );

		ZeroMemory( &mMessage, sizeof( mMessage ) );
		return;
	}

	ITEMMGR->SetDisableDialog( TRUE, eItemTable_Inventory );
	SetDisable( TRUE );

	// �̹� ��ȭ �ɼ��� �ִ� ��� �޽���â�� ���� ����� ǥ���ϵ��� �Ѵ�.
	{
		const ITEMBASE* item = ITEMMGR->GetItemInfoAbsIn( HERO, mItemIcon->GetSrcPosition() );

        if(		item									&&
			!	ITEMMGR->IsDupItem( item->wIconIdx )	&&
				item->Durability )
		{
			const ITEM_OPTION& option = ITEMMGR->GetOption(
				*item);
			const EnchantScript* enchantScript = g_CGameResourceManager.GetEnchantScript(
				option.mEnchant.mIndex);
			ITEM_OPTION	dummyOption	= option;

			if( enchantScript )
			{
				const ITEM_INFO* info = ITEMMGR->GetItemInfo( item->wIconIdx );

				if( ! info )
				{
					return;
				}
				
				g_CGameResourceManager.AddEnchantValue(
					dummyOption,
					enchantScript->mAbility,
					-1 * info->EnchantDeterm * option.mEnchant.mLevel);
			}

			if( isCheck )
			{
				const ITEM_OPTION::Reinforce emptyReinforce	= { 0 };

				if( memcmp( &emptyReinforce, &dummyOption.mReinforce, sizeof( emptyReinforce ) ) )
				{
					WINDOWMGR->MsgBox( MBI_ITEM_REINFORCE_CONFIRM, MBT_YESNO, CHATMGR->GetChatMsg( 1118 ) );
					return;
				}
			}				
		}
	}

	// 080228 LUJ, ��ȣ ������ ���� ����
	if( mProtectionItem.dwDBIdx )
	{
		ICONBASE& protectionItem = mMessage.mProtectionItem;

		protectionItem.dwDBIdx	= mProtectionItem.dwDBIdx;
		protectionItem.wIconIdx	= mProtectionItem.wIconIdx;
		protectionItem.Position	= mProtectionItem.Position;
	}
	
	// ���� ǥ�ð� ���� �� ������ �۾��� ��û�� ���̴�
	if( isCheck )
	{
		CProgressDialog* dialog = ( CProgressDialog* )WINDOWMGR->GetWindowForID( PROGRESS_DIALOG );
		ASSERT( dialog );

		dialog->Wait( CProgressDialog::eActionReinforce );
	}
}


void CReinforceDlg::SetActive( BOOL isActive )
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
			// 080429 NYJ  ��������Ʈ��ȯ�� ���Ұ�
			// 080929 LUJ, ��ü Ŭ������ �������� �ʵ��� ����
			{
				cDialog* dialog = WINDOWMGR->GetWindowForID( FISHINGPOINTDLG );

				if( dialog &&
					dialog->IsActive() )
				{
					CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1082 ) );
					return;
				}
			}
		}

		// ��ȭ/��æƮ/����/����/�����Ǹ� â�� ���ÿ� �ߴ� ���� ���´�
		{
			cDialog* mixDialog			= WINDOWMGR->GetWindowForID( ITMD_MIXDLG );
			cDialog* DissoloveDialog	= WINDOWMGR->GetWindowForID( DIS_DISSOLUTIONDLG );
			cDialog* enchantDialog		= WINDOWMGR->GetWindowForID( ITD_UPGRADEDLG );
			
			ASSERT( mixDialog && DissoloveDialog && enchantDialog );

			mixDialog->SetActive( FALSE );
			DissoloveDialog->SetActive( FALSE );
			enchantDialog->SetActive( FALSE );
		}

		// �κ��丮�� �Բ� �߾ӿ� ǥ�ý�Ų��
		{
			cDialog* inventory = WINDOWMGR->GetWindowForID( IN_INVENTORYDLG );
			ASSERT( inventory );

			const DISPLAY_INFO& screen	= GAMERESRCMNGR->m_GameDesc.dispInfo;
			const DWORD			space	= 140;
			const DWORD			x		= ( screen.dwWidth - m_width - inventory->GetWidth() - space ) / 2;
			const DWORD			y		= ( screen.dwHeight - max( inventory->GetHeight(), m_height ) ) / 2;

			SetAbsXY( x + space + inventory->GetWidth(), y );

			inventory->SetAbsXY( x, y );
			inventory->SetActive( TRUE );
		}

		// 080228 LUJ, ��ȣ ��� ǥ��
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
	}
	// 080929 LUJ, â�� ���� �� ���� ó��
	else	
	{
		// ���� ������ ����
		if( ! mBaseDialog->IsAddable( 0 ) )
		{
			mBaseDialog->DeleteIcon( 0, 0 );

			CItem* item = ( CItem* )( mItemIcon->GetLinkItem() );
			
			if( item )
			{
				item->SetLock( FALSE );
			}
		}

		// ��ϵ� ��Ḧ ���� ������Ų��
		RemoveMaterial();
		
		// ����â ����
		{
			cDialog* dialog = WINDOWMGR->GetWindowForID( RFGUIDE_DLG );
			ASSERT( dialog );

			dialog->SetActive( FALSE );
		}

		// 080228 LUJ, ��ȣ �������� �ʱ�ȭ�Ѵ�
		{
			CItem* item = ITEMMGR->GetItem( mProtectionItem.dwDBIdx );

			if( item )
			{
				item->SetLock( FALSE );

				ZeroMemory( &mProtectionItem, sizeof( mProtectionItem ) );
			}
		}

		// 080925 LUJ, ���� ������ ����
		{
			CItem* item = ITEMMGR->GetItem( mSupportItem.dwDBIdx );

			if( item )
			{
				item->SetLock( FALSE );
			}

			ZeroMemory( &mSupportItem, sizeof( mSupportItem ) );
		}
	}

	cDialog::SetActive( isActive );
}


// 080228 LUJ, ��ȣ �������� ����� ��� �Ҹ��Ѵ�
void CReinforceDlg::Succeed( const MSG_ITEM_REINFORCE_OPTION_ACK& message )
{
	Restore();

	// 080228 LUJ, �ɼ� ��ȣ ���θ� ��Ÿ��
	BOOL isProtected = FALSE;

	// 080228 LUJ, ��ȣ ������ ��� �� ó��
	if( mMessage.mProtectionItem.dwDBIdx )
	{
		// 080228 LUJ, ��ȣ �������� ����� ��� â�� �ݴ´�
		{
			CItem* item = ITEMMGR->GetItem( mMessage.mProtectionItem.dwDBIdx );

			if( item )
			{
				SetActive( FALSE );
			}
		}

		// 080228 LUJ, �ɼ��� ��ȣ�� ������� �����Ѵ�
		{
			CItem* item = ITEMMGR->GetItem( mMessage.mSourceItemDbIndex );

			if( item )
			{
				ITEM_OPTION	option = ITEMMGR->GetOption(
					item->GetItemBaseInfo());
				const ITEM_INFO* const info = item->GetItemInfo();
				const EnchantScript* const enchantScript = g_CGameResourceManager.GetEnchantScript(
					option.mEnchant.mIndex);

				// 080228 LUJ, ��æƮ�� ���� ���� ��� ����� �Ѵ�
				if( info &&
					enchantScript )
				{
					g_CGameResourceManager.AddEnchantValue(
						option,
						enchantScript->mAbility,
						-1 * info->EnchantDeterm * option.mEnchant.mLevel);
				}

				if( ! memcmp( &( option.mReinforce ), &( mProtectedOption.mReinforce ), sizeof( option.mReinforce ) ) )
				{
					isProtected = TRUE;
				}
			}
		}
	}
	
	// 080228 LUJ, ��ȣ�� ���� �ƴ� ��쿡 �ٸ� �޽����� ǥ���Ѵ�
	CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( isProtected ? 1451 : 807 ) );

	// �������� �ɼ��� �������ش�
	{
		ASSERT( ! mBaseDialog->IsAddable( 0 ) );

		CInventoryExDialog* inventory	= ( CInventoryExDialog* )( WINDOWMGR->GetWindowForID( IN_INVENTORYDLG ) ) ;
		CItem*				item		= inventory->GetItemForPos( mItemIcon->GetSrcPosition() );
		ASSERT( item );
		
		item->SetDBIdx( message.mOption.mItemDbIndex );
		
		ITEMMGR->AddToolTip( item );
		ITEMMGR->AddToolTip( mItemIcon );
	}
	
	{
		char line[ MAX_PATH ] = { 0 };
		sprintf( line, "%3d/%3d", mQuantity = 0, mMaxQuantity );

		mTextQuantity->SetStaticText( line );
	}

	// 080929 LUJ, ���� �������� �� ��� â�� �ݴ´�
	if( mSupportItem.wIconIdx )
	{
		SetActive( FALSE );
	}
}


// 080228 LUJ, ��ȣ ��� ó�� �߰�
void CReinforceDlg::Fail()
{
	Restore();

	// 080228 LUJ, ��ȣ �������� ����� ��� â�� �ݴ´�
	if( mMessage.mProtectionItem.dwDBIdx )
	{
		CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1451 ) );

		CItem* item = ITEMMGR->GetItem( mMessage.mProtectionItem.dwDBIdx );

		if( item )
		{
			SetActive( FALSE );
		}
	}
	else
	{
		char line[ MAX_PATH ];
		sprintf( line, "%3d/%3d", mQuantity = 0, mMaxQuantity );

		mTextQuantity->SetStaticText( line );

		CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 808 ) );
	}

	// 080929 LUJ, ���� �������� �� ��� â�� �ݴ´�
	if( mSupportItem.wIconIdx )
	{
		SetActive( FALSE );
	}
}


void CReinforceDlg::RemoveMaterial()
{
	// ��� ����
	for( WORD i = 0; i < mMaterialIcon.size(); ++i )
	{
		if( mIconGridDialog->IsAddable( i ) )
		{
			break;
		}

		CItem*	material = mMaterialIcon[ i ];
		CItem*	original = ITEMMGR->GetItem( material->GetDBIdx() );

		if( original )
		{
			if( ITEMMGR->IsDupItem( material->GetItemIdx() ) )
			{
				original->SetDurability( material->GetDurability() + original->GetDurability() );
			}
			
			//original->SetUseParam( 0 );
			original->SetLock( FALSE );
		}

		mIconGridDialog->DeleteIcon( i, 0 );
	}

	{
		char line[ MAX_PATH ];
		sprintf( line, "%3d/%3d", mQuantity = 0, mMaxQuantity );

		mTextQuantity->SetStaticText( line );
	}
}


void CReinforceDlg::Restore()
{
	mIsReinforcing	= FALSE;

	ITEMMGR->SetDisableDialog( FALSE, eItemTable_Inventory );

	if( HERO->GetState() == eObjectState_Die )
	{
		SetActive( FALSE );
	}

	SetDisable( FALSE );
}


void CReinforceDlg::Send()
{
	Submit( FALSE );

	static const MSG_ITEM_REINFORCE_SYN emptyMessage;

	if( ! memcmp( &emptyMessage, &mMessage, sizeof( emptyMessage ) ) )
	{
		return;
	}

	// 080228 LUJ, ��ȣ�Ǵ� ��� �ش� ������ �޽��� ����� ���� �ɼ��� �����صд�
	{
		mProtectedOption = ITEMMGR->GetOption( mMessage.mSourceItemDbIndex );

		const EnchantScript* enchantScript = g_CGameResourceManager.GetEnchantScript(
			mProtectedOption.mEnchant.mIndex);
		const ITEM_INFO* info = ITEMMGR->GetItemInfo(
			mMessage.mSourceItemIndex);

		if( info &&
			enchantScript )
		{
			g_CGameResourceManager.AddEnchantValue(
				mProtectedOption,
				enchantScript->mAbility,
				-1 * info->EnchantDeterm * mProtectedOption.mEnchant.mLevel);
		}
	}

	MSG_ITEM_REINFORCE_SYN message( mMessage );

	NETWORK->Send( &message, message.GetSize() );

	SetDisable( FALSE );

	RemoveMaterial();
}


// 080228 LUJ, ��ȣ ������ ����
void CReinforceDlg::SetProtectedActive( CItem& item )
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

	CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1455 ) );
}

// 080929 LUJ, ���� �������� ����Ѵ�
void CReinforceDlg::SetSupportItem( const ITEMBASE& item )
{
	mSupportItem.wIconIdx	= item.wIconIdx;
	mSupportItem.dwDBIdx	= item.dwDBIdx;
	mSupportItem.Position	= item.Position;
}