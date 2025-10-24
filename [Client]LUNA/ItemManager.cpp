#include "stdafx.h"
#include "ItemManager.h"
#include "ItemShow.h"
#include "cQuickItem.h"
#include "DealDialog.h"
#include "ExchangeDialog.h"
#include "MixDialog.h"
#include "ProgressDialog.h"
#include "GameIn.h"
#include "WindowIDEnum.h"
#include "Interface/cWindowManager.h"
#include "Interface/cScriptManager.h"
#include "cResourceManager.h"
#include "cDivideBox.h"
#include "ObjectManager.h"
#include "AppearanceManager.h"
#include "QuickManager.h"
#include "ExchangeItem.h"
#include "ChatManager.h"
#include "mhFile.h"
#include "GuildWarehouseDialog.h"
#include "InventoryExDialog.h"
#include "StorageDialog.h"
#include "EnchantDialog.h"
#include "ReinforceDlg.h"
#include "ApplyOptionDialog.h"
#include "MHMap.h"
#include "DissolveDialog.h"
#include "QuickSlotDlg.h"
#include "cSkillTreeManager.h"
#include "../[cc]skill/client/manager/Skillmanager.h"
#include "ItemShopDialog.h"
#include "ComposeDialog.h"
#include "NpcImageDlg.h"
#include "NpcScriptDialog.h"
#include "../hseos/Farm/SHFarmManageDlg.h"
#include "TutorialManager.h"
#include "cMapMoveDialog.h"
#include "../CharacterDialog.h"
#include "../[cc]skill/client/manager/skillmanager.h"
#include "../[cc]skill/client/info/ActiveSkillInfo.h"
#include "cMsgBox.h"
#include "../ConductInfo.h"
#include "ConductManager.h"
#include "cChangeNameDialog.h"
#include "../[CC]Header/FilteringTable.h"
#include "interface/cEditBox.h"
#include "Interface/cFont.h"
#include "../hseos/Farm/FarmAnimalDlg.h"
#include "BuyItem.h"
#include "ShoutDialog.h"
#include "FishingDialog.h"
#include "FishingManager.h"
#include "CommonCalcFunc.h"
#include "../hseos/Monstermeter/SHMonstermeterDlg.h"
#include "ItemPopupDlg.h"
#include "petweareddialog.h"
#include "petmanager.h"
#include "./GuildManager.h"
#include "./ObjectStateManager.h"
#include "./SiegeWarfareMgr.h"
#include "cCookMgr.h"
#include "cHousingMgr.h"
#include "cHousingStoredIcon.h"
#include "MoveManager.h"
#include "ChangeClassDlg.h"
#include "Questquickviewdialog.h"
#include "QuestDialog.h"
#include "QuestManager.h"
#include "..\[CC]Header\GameResourceManager.h"
#include "ConsignmentDlg.h"
#include "QuickDlg.h"
#include "interface/cListDialog.h"
#include "TransmogDialog.h"

struct TextUnit
{
	TCHAR mText[MAX_PATH];
	DWORD mColor;
	BOOL mIsAttach;
};

CItemManager* CItemManager::GetInstance()
{
	static CItemManager instance;

	return &instance;
}

CItemManager::CItemManager()
{
	m_ItemHash.Initialize(128);
	m_ItemInfoList.Initialize(MAX_ITEM_NUM);
	mEffectList.Initialize( 100 );
	mEventInfoTable.Initialize( 3 );

	m_IconIndexCreator.Init(MAX_ITEMICON_NUM, IG_ITEM_STARTINDEX);
	m_bAddPrice = FALSE;
	//m_RareItemInfoTable.Initialize(64);
	memset(&m_TempDeleteItem, 0, sizeof(ITEMBASE));
	m_nSealItemDelPos = -1;

	m_htMixResultItemInfo.Initialize(MAX_ITEM_NUM);
	m_ENchantNotifyCheckTime = 0;

	LoadMixScript();
	LoadDissolveScript();
	LoadCoolTimeScript();
	LoadChangeItem();
	LoadBodyChangeScript();
	LoadItemList();
	LoadItemToolTipList();
	LoadItemEffectList();
	LoadGiftEvent();
}

CItemManager::~CItemManager()
{
	//m_GroundHash.R
	Release();
	m_IconIndexCreator.Release();

	ITEM_INFO * pInfo = NULL;
	m_ItemInfoList.SetPositionHead();
	while((pInfo = m_ItemInfoList.GetData()) != NULL)
		delete pInfo;
	m_ItemInfoList.RemoveAll();
	
	m_ToolTipTextMap.clear();

	EnchantEffect * pEffect = NULL;
	mEffectList.SetPositionHead();
	while((pEffect = mEffectList.GetData()) != NULL)
		delete pEffect;
	mEffectList.RemoveAll();

	EVENTINFO* pEvent = NULL;
	mEventInfoTable.SetPositionHead();
	while((pEvent = mEventInfoTable.GetData()) != NULL)
	{
		delete[] pEvent->Msg;
		delete pEvent;
	}
	mEventInfoTable.RemoveAll();

	m_GiftEventTime = 0;

	m_htMixResultItemInfo.SetPositionHead();
	while(stMixResultItemInfo* pMixInfo = m_htMixResultItemInfo.GetData())
	{
		SAFE_DELETE( pMixInfo );
	}
	m_htMixResultItemInfo.RemoveAll();
}


cImage * CItemManager::GetIconImage(DWORD ItemIdx, cImage * pImageGet)
{
	// 080916 LUJ, �� ������ üũ �߰�
	const ITEM_INFO* itemInfo = GetItemInfo( ItemIdx );
	SCRIPTMGR->GetImage( itemInfo ? itemInfo->Image2DNum : 0, pImageGet, PFT_ITEMPATH );
	return pImageGet;
}

CItem* CItemManager::MakeNewItem( const ITEMBASE* pBaseInfo, const char* strKind)
{
	ITEM_INFO* pItemInfo = GetItemInfo(pBaseInfo->wIconIdx);
	
	if(pItemInfo == NULL)
	{
		char temp[256];
		sprintf(temp,"NoItemInfo %d,  %d,  %s",gHeroID,pBaseInfo->wIconIdx,strKind);
		ASSERTMSG(pItemInfo, temp);
		return NULL;
	}
	
	if(pBaseInfo->dwDBIdx == 0)
		return NULL;

	CItem* pItem = NULL;
	pItem = GetItem(pBaseInfo->dwDBIdx);

	if(!pItem)
	{
		pItem = new CItem(pBaseInfo);
		m_ItemHash.Add(pItem,pItem->GetDBIdx());						// HASH���� ����AIAU ��O��a
		cImage tmpIconImage;
		pItem->Init(0,0,DEFAULT_ICONSIZE,DEFAULT_ICONSIZE,
					GetIconImage(pItem->GetItemIdx(), &tmpIconImage),
					m_IconIndexCreator.GenerateIndex());
		pItem->SetIconType(eIconType_AllItem);
		pItem->SetData(pItem->GetItemIdx());
		pItem->SetLinkPosition( 0 );

		WINDOWMGR->AddWindow(pItem);
		RefreshItem( pItem );
	}
	else
	{
		// 080923 LUJ, â���� �ι� �̻� ���� ���� �������� ��Ȱ���Ѵ�. �̶� �ܿ��ð��� ���ŵ��� ������
		//				��Ÿ���� ó������ ���� ��ó�� ���δ�. �ð��� �������ֱ� ���� ������ �ٽ� �����Ѵ�
		pItem->SetItemBaseInfo( *pBaseInfo );
	}

	return pItem;
}


void CItemManager::AddToolTip( cWindow* window, DWORD toolTipIndex, DWORD color )
{
	if( ! window )
	{
		return;
	}

	const std::vector< std::string >& cutToolTipList = GetToolTipMsg( toolTipIndex );
	for( std::vector< std::string >::const_iterator cutToolTipListIter = cutToolTipList.begin() ; cutToolTipListIter != cutToolTipList.end() ; ++cutToolTipListIter )
	{
		const std::string& strTooltip = *cutToolTipListIter;
		window->AddToolTipLine( strTooltip.c_str(), color );
	}
}


void CItemManager::AddToolTip( CVirtualItem* icon )
{
	if( ! icon )
	{
		return;
	}

	CBaseItem* linkedItem = icon->GetLinkItem();

	if( ! linkedItem )
	{
		return;
	}

	ITEMBASE item;
	{
		ZeroMemory( &item, sizeof( item ) );

		item.wIconIdx	= linkedItem->GetItemIdx();
		item.dwDBIdx	= linkedItem->GetDBIdx();
	}

	AddNametoolTip( *icon, item );
	AddFurnitureFunctionToolTip( *icon, item );
	AddPetToolTip( *icon, item );
	AddOptionToolTip( *icon, item );
	AddDescriptionToolTip( *icon, item );
	AddSetToolTip( *icon, item );
	AddFunctionToolTip( *icon, item );
	AddMakerToolTip( *icon, item );
	AddFurnitureOptionToolTip( *icon, item );
}


void CItemManager::AddToolTip( cIcon* icon )
{
	if( ! icon )
	{
		return;
	}

	ITEMBASE item;
	{
		ZeroMemory( &item, sizeof( item ) );

		item.wIconIdx	= icon->GetData();
	}

	AddNametoolTip( *icon, item );
	AddFurnitureFunctionToolTip( *icon, item );
	AddPetToolTip( *icon, item );
	AddOptionToolTip( *icon, item );
	AddDescriptionToolTip( *icon, item );
	AddSetToolTip( *icon, item );
	AddFunctionToolTip( *icon, item );
	AddMakerToolTip( *icon, item );
	AddFurnitureOptionToolTip( *icon, item );
}


void CItemManager::AddToolTip( CItemShow* icon )
{
	if( ! icon )
	{
		return;
	}

	ITEMBASE item;
	{
		ZeroMemory( &item, sizeof( item ) );

		item.wIconIdx	= icon->GetItemIdx();
		item.dwDBIdx	= icon->GetDBIdx();
	}

	AddNametoolTip( *icon, item );
	AddFurnitureFunctionToolTip( *icon, item );
	AddPetToolTip( *icon, item );
	AddOptionToolTip( *icon, item );
	AddDescriptionToolTip( *icon, item );
	AddSetToolTip( *icon, item );
	AddFunctionToolTip( *icon, item );
	AddMakerToolTip( *icon, item );
	AddFurnitureOptionToolTip( *icon, item );

}


void CItemManager::AddToolTip( CItem* icon )
{
	if( ! icon )
	{
		return;
	}

	const ITEMBASE& item = icon->GetItemBaseInfo();

	BOOL bFirst = TRUE ;

	if( eITEM_TYPE_UNSEAL == item.nSealed )
	{
		bFirst = FALSE ;
		icon->SetLastCheckTime( gCurTime );
	}
		
	AddNametoolTip( *icon, item );
	AddFurnitureFunctionToolTip( *icon, item );
	AddPetToolTip( *icon, item );
	AddOptionToolTip( *icon, item );
	AddDescriptionToolTip( *icon, item );
	AddSetToolTip( *icon, item );
	AddFunctionToolTip( *icon, item );
	AddMakerToolTip( *icon, item );
	AddFurnitureOptionToolTip( *icon, item );

	// 071115 LYW --- ItemManager : ���� ���ð� ���� �߰�.
	AddRemainTimeToolTip( *icon, item, bFirst );

	if( icon->IsNoDefenceTooltip() )
	{
		icon->AddToolTipLine( "" );
		icon->AddToolTipLine( CHATMGR->GetChatMsg( 1195 ), ICONCLR_DISABLE );
		icon->AddToolTipLine( CHATMGR->GetChatMsg( 1196 ), ICONCLR_DISABLE );
	}
}


void CItemManager::AddToolTip( cQuickItem* icon )
{
	if( !	icon  )
	{
		return;
	}

	if(icon->GetSlotInfo().kind == QuickKind_Skill)
	{
		return ;
	}
	else if(icon->GetSlotInfo().kind == QuickKind_Conduct )
	{
		cConductInfo* pConductInfo = CONDUCTMGR->GetConductInfo(icon->GetSlotInfo().idx ) ;

		if( !pConductInfo ) return ;

		cImage image;
		SCRIPTMGR->GetImage( 0, &image, PFT_HARDPATH );

		icon->SetToolTip( "", RGBA_MAKE(255, 255, 255, 255), &image, TTCLR_ITEM_CANEQUIP );
		icon->AddToolTipLine( pConductInfo->GetConductName(), RGB_HALF( 255, 255, 255 ) );
		return ;
	}

	ITEMBASE item;
	ZeroMemory( &item, sizeof( item ) );
	item.wIconIdx = icon->GetSlotInfo().idx;

	if(CItem* const linkedItem = GetItem(icon->GetSlotInfo().dbIdx))
	{
		item.dwDBIdx = linkedItem->GetDBIdx();
		item.nSealed = ITEM_SEAL_TYPE(linkedItem->IsSeal());
	}

	AddNametoolTip( *icon, item );
	AddFurnitureFunctionToolTip( *icon, item );
	AddPetToolTip( *icon, item );
	AddOptionToolTip( *icon, item );
	AddDescriptionToolTip( *icon, item );
	AddSetToolTip( *icon, item );
	AddFunctionToolTip( *icon, item );
	AddMakerToolTip( *icon, item );
	AddUseableTimeToolTip( *icon, item );
	AddFurnitureOptionToolTip( *icon, item );
}


void CItemManager::AddToolTip( CExchangeItem* icon )
{
	if( ! icon  )
	{
		return;
	}

	ITEMBASE item;
	{
		ZeroMemory( &item, sizeof( item ) );

		if( icon->GetLinkItem() )
		{
			item = icon->GetLinkItem()->GetItemBaseInfo();
		}
		else
		{
			item.wIconIdx	= icon->GetData();
			item.dwDBIdx	= icon->GetDBId();
			item.nSealed	= ITEM_SEAL_TYPE(icon->IsSeal());
		}		
	}

	AddNametoolTip( *icon, item );
	AddFurnitureFunctionToolTip( *icon, item );
	AddPetToolTip( *icon, item );
	AddOptionToolTip( *icon, item );
	AddDescriptionToolTip( *icon, item );
	AddSetToolTip(* icon, item );
	AddFunctionToolTip( *icon, item );
	AddMakerToolTip( *icon, item );
	AddFurnitureOptionToolTip( *icon, item );

	if( item.nSealed )
	{
		AddUseableTimeToolTip( *icon, item ) ;
	}
}


void CItemManager::AddTimeToolTip( cIcon* icon, const ITEM_OPTION& option, BOOL bFirst)
{
	if( ! icon )
	{
		ASSERT( 0 );
		return;
	}
	
	ITEMBASE item;
	{
		ZeroMemory( &item, sizeof( item ) );

		if( WT_ITEM == icon->GetType() )
		{
			item = ( ( CItem* )icon )->GetItemBaseInfo();
		}
		else
		{
			item.wIconIdx	= icon->GetData();
		}		
	}

	const ITEM_INFO* info = GetItemInfo( icon->GetData() );

	if( ! info )
	{
		return;
	}

	AddNametoolTip( *icon, item );
	AddFurnitureFunctionToolTip( *icon, item );			// 090430 pdy �Ͽ�¡������ ���� �߰� 
	AddPetToolTip( *icon, item );
	AddOptionToolTip( *icon, item );
	AddDescriptionToolTip( *icon, item );
	AddSetToolTip( *icon, item );
	AddFunctionToolTip( *icon, item );					
	AddMakerToolTip( *icon, item );
	AddFurnitureOptionToolTip( *icon, item );			// 090430 pdy �Ͽ�¡������ ���� �߰� 

	if( icon->GetType() == WT_HOUSING_STORED_ICON )
	{
		// 090430 pdy �Ͽ�¡������ ���� �߰� 
		AddFurnitureStateToolTip(*icon, item);
	}

	// 071115 LYW --- ItemManager : ���� �ð� ���.
	// 090430 pdy WT_HOUSING_STORED_ICON �߰� 
	if( icon->GetType() == WT_ITEM ||
		icon->GetType() == WT_HOUSING_STORED_ICON )
	{
		AddRemainTimeToolTip( *icon, item, bFirst) ;
	}	
}

void CItemManager::AddToolTip(CBuyItem* icon)
{
	if( ! icon  )
	{
		return;
	}

	ITEMBASE item;
	{
		ZeroMemory( &item, sizeof( item ) );
		item.wIconIdx	= icon->GetItemIdx();
		item.nSealed	= ITEM_SEAL_TYPE(icon->GetSeal());
	}

	AddNametoolTip( *icon, item );
	AddFurnitureFunctionToolTip( *icon, item );
	AddPetToolTip( *icon, item );
	AddOptionToolTip( *icon, item );
	AddDescriptionToolTip( *icon, item );
	AddSetToolTip(* icon, item );
	AddFunctionToolTip( *icon, item );
	AddMakerToolTip( *icon, item );
	AddFurnitureOptionToolTip( *icon, item );

	if( item.nSealed )
	{
		AddUseableTimeToolTip( *icon, item ) ;
	}
}


//---------------------------------------------------------------------------------------------------------------------------
// 071126 LYW --- ItemManager : ���� ���ð� ���� �߰�.
//---------------------------------------------------------------------------------------------------------------------------
void CItemManager::AddUseableTimeToolTip(cIcon& icon, const ITEMBASE& item )
{
	const ITEM_INFO* pItemInfo = GetItemInfo( item.wIconIdx ) ;

	if( !pItemInfo ||
		pItemInfo->wSeal != eITEM_TYPE_SEAL ||
		pItemInfo->SupplyType == ITEM_KIND_PET )
	{
		return ;
	}

	const DWORD dwRemainTime = pItemInfo->dwUseTime ;

	const int nDayValue		= 86400 ;
	const int nHourValue	= 3600 ;

	int nDay = dwRemainTime/(nDayValue) ;

	int nHour = ( dwRemainTime - (nDay * nDayValue) ) / nHourValue ;
	
	int nMinute = ( (dwRemainTime - (nDay * nDayValue)) - (nHour * nHourValue) ) / 60;

	char tempBuf[512] = {0, } ;

	icon.AddToolTipLine(CHATMGR->GetChatMsg( 1184 ), RGB_HALF( 143, 205, 217 )) ;

	if( nDay <= 0 )
	{
		if( nHour <= 0 )
		{
			if( nMinute <= 0 ) nMinute = 1 ;
			sprintf(tempBuf, CHATMGR->GetChatMsg( 1185 ), nMinute) ;
		}
		else
		{
			sprintf(tempBuf, CHATMGR->GetChatMsg( 1186 ), nHour, nMinute) ;
		}
	}
	else
	{
		sprintf(tempBuf, CHATMGR->GetChatMsg( 1187 ), nDay, nHour, nMinute) ;
	}

	icon.AddToolTipLine(tempBuf, RGB_HALF( 143, 205, 217 )) ;
}


//---------------------------------------------------------------------------------------------------------------------------
// 071115 LYW --- ItemManager : ���� ���ð� ���� �߰�.
//---------------------------------------------------------------------------------------------------------------------------
void CItemManager::AddRemainTimeToolTip( cIcon& icon, const ITEMBASE& item, BOOL bFirst)
{
	const ITEM_INFO* pItemInfo = GetItemInfo( item.wIconIdx );

	if( !pItemInfo )
	{
		return;
	}

	if( pItemInfo->SupplyType == ITEM_KIND_PET )
	{
		return;
	}

	DWORD dwRemainTime = 0;
	char tempBuf[512] = { 0 } ;

	switch( item.nSealed )
	{		
	case eITEM_TYPE_UNSEAL:
		{
			icon.AddToolTipLine( "" );

			if( bFirst )
			{
				dwRemainTime = pItemInfo->dwUseTime ;
			}
			else
			{
				dwRemainTime = item.nRemainSecond ;
			}

			int nDayValue = 86400 ;
			int nHourValue = 3600 ;

			int nDay = dwRemainTime/(nDayValue) ;
			int nHour = ( dwRemainTime - (nDay * nDayValue) ) / nHourValue ;		
			int nMinuteValue = (dwRemainTime - (nDay * nDayValue)) - (nHour * nHourValue) ;		
			int nMinute = nMinuteValue / 60;
			int nSecond = nMinuteValue % 60;

			if( nSecond > 0 )
			{
				++nMinute ;
			}

			icon.AddToolTipLine(CHATMGR->GetChatMsg( 1179 ), RGB_HALF( 255, 255, 0 )) ;

			if( nDay <= 0 )
			{
				if( nHour <= 0 )
				{
					if( nMinute <= 0 ) 
					{
						sprintf(tempBuf, CHATMGR->GetChatMsg( 1181 ), 1) ;
					}
					else
					{
						sprintf(tempBuf, CHATMGR->GetChatMsg( 1181 ), nMinute) ;
					}

				}
				else
				{
					sprintf(tempBuf, CHATMGR->GetChatMsg( 1182 ), nHour, nMinute) ;
				}
			}
			else
			{
				sprintf(tempBuf, CHATMGR->GetChatMsg( 1183 ), nDay, nHour, nMinute) ;
			}

			icon.AddToolTipLine(tempBuf, RGB_HALF( 255, 255, 0 )) ;
			break;
		}
	case eITEM_TYPE_SEAL:
		{
			icon.AddToolTipLine( "" );

			dwRemainTime = pItemInfo->dwUseTime ;

			int nDayValue = 86400 ;
			int nHourValue = 3600 ;

			int nDay = dwRemainTime/(nDayValue) ;

			int nHour = ( dwRemainTime - (nDay * nDayValue) ) / nHourValue ;

			int nMinute = ( (dwRemainTime - (nDay * nDayValue)) - (nHour * nHourValue) ) / 60;

			icon.AddToolTipLine(CHATMGR->GetChatMsg( 1184 ), RGB_HALF( 143, 205, 217 )) ;

			if( nDay <= 0 )
			{
				if( nHour <= 0 )
				{
					if( nMinute <= 0 ) nMinute = 1 ;
					sprintf(tempBuf, CHATMGR->GetChatMsg( 1185 ), nMinute) ;
				}
				else
				{
					sprintf(tempBuf, CHATMGR->GetChatMsg( 1186 ), nHour, nMinute) ;
				}
			}
			else
			{
				sprintf(tempBuf, CHATMGR->GetChatMsg( 1187 ), nDay, nHour, nMinute) ;
			}

			icon.AddToolTipLine(tempBuf, RGB_HALF( 143, 205, 217 )) ;
			break;
		}
	}

	//090430 pdy �Ͽ�¡ ������ �ð����� �߰�
	if( pItemInfo->SupplyType == ITEM_KIND_FURNITURE )
	{
		if( pItemInfo->dwUseTime >= 1500000000 )		//090604 pdy �Ͽ�¡ ������ �ð������� 15���̻��̸� ����� �ʴ´�. (��ȹ�� ��û)
			return;

		//�����������ϰ�� ������ �ƴ����� �ð������� �������Ѵ�.
		BOOL	bInstalled = FALSE; 
		dwRemainTime = pItemInfo->dwUseTime ;			//����Ʈ�� �������� �⺻���Ⱓ�� �ִ´�

		if( icon.GetType() == WT_HOUSING_STORED_ICON )
		{
			//���� �������� ���� 
			cHousingStoredIcon* pHS_StoredIcon = (cHousingStoredIcon*) &icon;
			stFurniture* pstFurniture = pHS_StoredIcon->GetLinkFurniture();
			if(! pstFurniture )
				return;

			if( pstFurniture->wState != eHOUSEFURNITURE_STATE_KEEP )
			{
				//���� �������϶� �ѹ��̶� ��ġ�� �Ǿ����� ������ �����ð��� �������� 
				dwRemainTime = pstFurniture->dwRemainTime;
				bInstalled = TRUE;
			}
		}

		//icon.AddToolTipLine( "" );
		int nDayValue = 86400 ;
		int nHourValue = 3600 ;
		int nDay = dwRemainTime/(nDayValue) ;
		int nHour = ( dwRemainTime - (nDay * nDayValue) ) / nHourValue ;		
		int nMinuteValue = (dwRemainTime - (nDay * nDayValue)) - (nHour * nHourValue) ;		
		int nMinute = nMinuteValue / 60;

		if( bInstalled == TRUE )		 
		{
			icon.AddToolTipLine(CHATMGR->GetChatMsg( 1179 ), RGB_HALF( 255, 255, 0 )) ;
			//�������� �ѹ��̶� ��ġ�� ���¸� [�����ð�] ���
			if( nDay > 0)
			{
				sprintf(tempBuf, CHATMGR->GetChatMsg( 1183 ), nDay, nHour, nMinute) ;
			}
			else if( nHour > 0 )
			{
				sprintf(tempBuf, CHATMGR->GetChatMsg( 1182 ), nHour, nMinute) ;
			}
			else if( nMinute > 0 ) 
			{
				sprintf(tempBuf, CHATMGR->GetChatMsg( 1181 ), nMinute) ;
			}
			else
			{
				sprintf(tempBuf, CHATMGR->GetChatMsg( 1181 ), 1) ;
			}
			icon.AddToolTipLine(tempBuf, RGB_HALF( 255, 255, 0 )) ;
		}
		else 
		{
			icon.AddToolTipLine(CHATMGR->GetChatMsg( 1184 ), RGB_HALF( 143, 205, 217 )) ;
			//�������� �ѹ��̶� ��ġ�������� ���¸� [��ġ�Ľð�] ���
			if( nDay > 0)
			{
				sprintf(tempBuf, CHATMGR->GetChatMsg( 1843 ), nDay, nHour, nMinute) ;
			}
			else if( nHour > 0 )
			{
				sprintf(tempBuf, CHATMGR->GetChatMsg( 1842), nHour, nMinute) ;
			}
			else if( nMinute > 0 )
			{
				sprintf(tempBuf, CHATMGR->GetChatMsg( 1843 ), nMinute) ;
			}
			else 
			{
				sprintf(tempBuf, CHATMGR->GetChatMsg( 1843 ), 1) ;
			}
			icon.AddToolTipLine(tempBuf, RGB_HALF( 143, 205, 217 )) ;
		}
	}
}


void CItemManager::AddMakerToolTip( cIcon& icon, const ITEMBASE& item )
{
	const ITEM_INFO* info = GetItemInfo( item.wIconIdx );

	// 080226 LUJ, ���� �Ұ����� �������� ������ ������ ǥ������ �ʴ´�
	if( !	info ||
			eEquipType_None == info->EquipType )
	{
		return;
	}

	const ITEM_OPTION& option = GetOption( item.dwDBIdx );

	const char* reinforceMaker	= option.mReinforce.mMadeBy;
	const char* mixMaker		= option.mMix.mMadeBy;
	const char* enchantMaker	= option.mEnchant.mMadeBy;

	if( ! *reinforceMaker	&&
		! *mixMaker			&&
		! *enchantMaker )
	{
		return;
	}

	char text[ MAX_PATH ];

	icon.AddToolTipLine( "" );

	if( *reinforceMaker )
	{
		sprintf( text, CHATMGR->GetChatMsg( 1170 ), reinforceMaker );
		icon.AddToolTipLine( text, RGB_HALF( 255, 255, 0 ) );
	}

	if( *mixMaker )
	{
		sprintf( text, CHATMGR->GetChatMsg( 1169 ), mixMaker );
		icon.AddToolTipLine( text, RGB_HALF( 255, 255, 0 ) );
	}

	if( *enchantMaker )
	{
		sprintf( text, CHATMGR->GetChatMsg( 1171 ), enchantMaker );
		icon.AddToolTipLine( text, RGB_HALF( 255, 255, 0 ) );
	}
}

void CItemManager::AddFunctionToolTip( cIcon& icon, const ITEMBASE& item )
{
	const ITEM_INFO* info = GetItemInfo( item.wIconIdx );

	if( ! info )
	{
		return;
	}

	// 091012 pdy ��ȹ�� ��û���� FunctionToolTip ���� ��� ���� 
	// ���� = ����� , �Ұ��� = ������ 

	// ��ȭ �Ǹ� ��ȯ ���� 
	icon.AddToolTipLine( "" );

	// �Ǹ� 
	if( info->Sell )
	{
		icon.AddToolTipPartialLine( TRUE, CHATMGR->GetChatMsg( 1958 ) , RGB_HALF( 255,255,0) );
	}
	else
	{
		icon.AddToolTipPartialLine( TRUE, CHATMGR->GetChatMsg( 1958 ) , RGB_HALF( 255,0,0) );
	}

	icon.AddToolTipPartialLine( FALSE ," ");

	// ��ȯ
	if( info->Trade )
	{
		if( item.nSealed != eITEM_TYPE_SEAL_NORMAL && item.nSealed !=  eITEM_TYPE_SEAL )
		{
			// ���� �������� ��� ������ ���� �Ǿ������� ��ȯ �Ұ� 
			icon.AddToolTipPartialLine( FALSE , CHATMGR->GetChatMsg( 1957 ) , RGB_HALF( 255,0,0) );
		}
		else
		{
			icon.AddToolTipPartialLine( FALSE , CHATMGR->GetChatMsg( 1957 ) , RGB_HALF( 255,255,0) );
		}
	}
	else
	{
		// ��ȯ �Ұ�
		icon.AddToolTipPartialLine( FALSE , CHATMGR->GetChatMsg( 1957 ) , RGB_HALF( 255,0,0) );
	}

	icon.AddToolTipPartialLine( FALSE ," ");

	// â������ 
	if( info->Deposit )
	{
		icon.AddToolTipPartialLine( FALSE , CHATMGR->GetChatMsg( 1959 ) , RGB_HALF( 255,255,0) );
	}
	else
	{
		icon.AddToolTipPartialLine( FALSE , CHATMGR->GetChatMsg( 1959 ) , RGB_HALF( 255,0,0) );
	}

	icon.AddToolTipPartialLine( FALSE ," ");

	// ��ȭ 
	if( info->Improvement )
	{
		icon.AddToolTipPartialLine( FALSE ,  CHATMGR->GetChatMsg( 1965 ) , RGB_HALF( 255,255,0) );
	}
	else
	{
		icon.AddToolTipPartialLine( FALSE ,  CHATMGR->GetChatMsg( 1965 ) , RGB_HALF( 255,0,0) );
	}


	// ���� ���� ��æƮ

	// ����
	if( GetMixScript( info->ItemIdx ) )
	{
		icon.AddToolTipPartialLine( TRUE , CHATMGR->GetChatMsg( 1966 ) , RGB_HALF( 255,255,0) );
	}
	else
	{
		icon.AddToolTipPartialLine( TRUE , CHATMGR->GetChatMsg( 1966 ) , RGB_HALF( 255,0,0) );
	}

	icon.AddToolTipPartialLine( FALSE ," ");

	// ����
	if( GetDissolveScript( info->ItemIdx ) )
	{
		icon.AddToolTipPartialLine( FALSE , CHATMGR->GetChatMsg( 1967 ) , RGB_HALF( 255,255,0) );
	}
	else
	{
		icon.AddToolTipPartialLine( FALSE , CHATMGR->GetChatMsg( 1967 ) , RGB_HALF( 255,0,0) );
	}

	icon.AddToolTipPartialLine( FALSE ," ");

	// ��æƮ
	if( info->IsEnchant )
	{
		icon.AddToolTipPartialLine( FALSE , CHATMGR->GetChatMsg( 1968 ) , RGB_HALF( 255,255,0) );
	}
	else
	{
		icon.AddToolTipPartialLine( FALSE , CHATMGR->GetChatMsg( 1968 ) , RGB_HALF( 255,0,0) );
	}
}

void CItemManager::Item_Transmog_Success_Ack( void* pMsg )
{
	const MSG_ITEM_TRANSMOG_ACK* message = ( MSG_ITEM_TRANSMOG_ACK* )pMsg;

	{
		CTransmogDialog* dialog = ( CTransmogDialog* )WINDOWMGR->GetWindowForID( ITTD_TRANSMOGDLG	);
		ASSERT( dialog );

		dialog->Succeed( *message );
	}

	QUICKMGR->RefreshQickItem();
}
void CItemManager::Item_Transmog_Nack( void* pMsg )
{
	const MSG_DWORD* message = ( MSG_DWORD* )pMsg;

	switch( eLogitemmoney( message->dwData ) )
	{
	case eLog_ItemTransmogHacking:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, "Report to arcaneluna.com, please: your apply to server was abnormal" );
			break;
		}
	// 090713 ShinJS --- ÀÎÃ¦Æ® °ñµå¼Òºñ Ãß°¡¿¡ ´ëÇÑ ¼ÒÝö±Ý ºÎÝ· ¿À·ù
	case eLog_ItemTransmogNotEnoughMoney:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 324 ) );
			break;
		}
	default:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1275 ) );
			break;
		}
	}

	{
		CTransmogDialog* dialog = ( CTransmogDialog* )WINDOWMGR->GetWindowForID( ITTD_TRANSMOGDLG	);
		ASSERT( dialog );

		dialog->Restore();
	}
}

void CItemManager::Item_Transmog_Failed_Ack( void* pMsg )
{
	CTransmogDialog* dialog = ( CTransmogDialog* )WINDOWMGR->GetWindowForID( ITTD_TRANSMOGDLG	);
	ASSERT( dialog );

	dialog->Fail();
}

void CItemManager::AddNametoolTip( cIcon& icon, const ITEMBASE& item )
{
	const ITEM_INFO* info = GetItemInfo( item.wIconIdx );

	if( ! info )
	{
		return;
	}

	// �ʱ�ȭ
	{
		cImage image;
		SCRIPTMGR->GetImage( 0, &image, PFT_HARDPATH );

		icon.SetToolTip( "", RGBA_MAKE(255, 255, 255, 255), &image, TTCLR_ITEM_CANEQUIP );
	}	

	char line[ MAX_PATH ] = { 0 };

	// �̸� ǥ��
	{
		char isSealing[ MAX_PATH ] = { 0 };

		DWORD color = GetItemNameColor( *info );

		if( //eIconType_AllItem	== icon.GetIconType() &&
			eITEM_TYPE_SEAL		== item.nSealed )
		{
			sprintf( isSealing, "[%s] ", CHATMGR->GetChatMsg( 1393 ) );

			color = RGB_HALF( 255, 0, 0 );
		}

		char isEnchant[ MAX_PATH ] = { 0 };

		// 080601 LUJ, ��ø���� �ʴ� �����۸� ��æƮ ������ ǥ���Ѵ�
		if( ! info->Stack )
		{
			const ITEM_OPTION& option = GetOption( item.dwDBIdx );

			if( option.mEnchant.mLevel )
			{
				sprintf( isEnchant, "+%d ", option.mEnchant.mLevel );
			}

			if( option.mTransmogIdx )
			{
				const ITEM_INFO* info_transmog = GetItemInfo( option.mTransmogIdx );
				if(info_transmog->ItemName)
				{
					sprintf( line, "Model : %s ", info_transmog->ItemName );
					icon.AddToolTipLine( line, color );
				}
			}

		}

		sprintf( line, "%s%s%s", isSealing, isEnchant, info->ItemName );

		icon.AddToolTipLine( line, color );
	}	

	// ��� ����
	switch( eEquipType( info->EquipType ) )
	{
	case eEquipType_Weapon:
		{
			switch( eWeaponType( info->WeaponType ) )
			{
			case eEquipType_Card:
			case eWeaponType_Sword:
				{
					sprintf( line, "%s", CHATMGR->GetChatMsg( 660 ) );
					break;
				}				
			case eWeaponType_Mace:
				{
					sprintf( line, "%s", CHATMGR->GetChatMsg( 662 ) );
					break;
				}				
			case eWeaponType_Axe:
				{
					sprintf( line, "%s", CHATMGR->GetChatMsg( 661 ) );
					break;
				}
			case eWeaponType_Dagger:
				{
					sprintf( line, "%s", CHATMGR->GetChatMsg( 663 ) );
					break;
				}
			case eWeaponType_Staff:
				{
					sprintf( line, "%s", CHATMGR->GetChatMsg( 665 ) );
					break;
				}
			case eWeaponType_Bow:
				{
					sprintf( line, "%s", CHATMGR->GetChatMsg( 664 ) );
					break;
				}
			case eWeaponType_FishingPole:
				{
					sprintf( line, "%s", CHATMGR->GetChatMsg( 1542 ) );
					break;
				}
			case eWeaponType_CookUtil:
				{
					sprintf( line, "%s", CHATMGR->GetChatMsg( 1803 ) );
					break;
				}
				// 080902 LUJ, �� �߰�
			case eWeaponType_Gun:
				{
					sprintf( line, "%s", CHATMGR->GetChatMsg( 1677 ) );
					break;
				}
				// 090528 ONS �ű����� ����Ӽ��߰�
			case eWeaponType_BothDagger:
				{
					sprintf( line, "%s", CHATMGR->GetChatMsg( 1924 ) );
					break;
				}
				// 080902 LUJ, ������ �������� '��Ÿ'�� ǥ��
			default:
				{
					sprintf( line, "%s", CHATMGR->GetChatMsg( 48 ) );
					break;
				}
			}

			switch( eWeaponAnimationType( info->WeaponAnimation ) )
			{
			case eWeaponAnimationType_OneHand:
				{
					sprintf( line, "%s / %s", line, CHATMGR->GetChatMsg( 658 ) );
					break;
				}
			case eWeaponAnimationType_TwoHand:
				{
					sprintf( line, "%s / %s", line, CHATMGR->GetChatMsg( 659 ) );
					break;
				}
				//��� ������ ǥ��
			case eWeaponAnimationType_Staff:
				{
					sprintf( line, "%s / %s", line, CHATMGR->GetChatMsg( 659 ) );
					break;
				}

			}

			icon.AddToolTipLine( line, TTTC_ITEM_NAME );
			break;
		}
	case eEquipType_Armor:
		{
			if(eWeaponType( info->WeaponType ) == eWeaponType_CookUtil)
			{
				// �丮������ �޽��� ����
				sprintf( line, "%s", CHATMGR->GetChatMsg( 1803 ) );
			}
			else
			{
				sprintf( line, "%s", GetName(info->EquipSlot));
			}

			// D : ����
			switch( eArmorType( info->ArmorType ) )
			{
			case eArmorType_Robe:
				{
					sprintf( line, "%s / %s", line, CHATMGR->GetChatMsg( 692 ) );
					break;
				}
				break;
			case eArmorType_Leather:
				{
					sprintf( line, "%s / %s", line, CHATMGR->GetChatMsg( 693 ) );
					break;
				}
			case eArmorType_Metal:
				{
					sprintf( line, "%s / %s", line, CHATMGR->GetChatMsg( 694 ) );
					break;
				}				
			}

			icon.AddToolTipLine( line, TTTC_ITEM_NAME );
			break;
		}
			// 091008 pdy ��ȹ�� ��û �Ǽ��縮 �������� ������ġ�� �Ǵ��Ͽ� ������ ����ش�. 
		case eEquipType_Accessary:
			{
				switch(info->EquipSlot)
				{
				case eWearedItem_Ring1:
				case eWearedItem_Ring2:
				case eWearedItem_Necklace:
				case eWearedItem_Earring1:
				case eWearedItem_Earring2:
					{
						sprintf( line, "%s", CHATMGR->GetChatMsg( 1943 ) );
					}
					break;
				case eWearedItem_Belt:
					{
						sprintf( line, "%s", CHATMGR->GetChatMsg( 1947 ) );
					}
					break;
				case eWearedItem_Band:
					{
						sprintf( line, "%s", CHATMGR->GetChatMsg( 1951 ) );
					}
					break;
				case eWearedItem_Glasses:
					{
						sprintf( line, "%s", CHATMGR->GetChatMsg( 1950 ) );
					}
					break;
				case eWearedItem_Mask:
					{
						sprintf( line, "%s", CHATMGR->GetChatMsg( 1949 ) );
					}
					break;
				case eWearedItem_Wing:
					{
						sprintf( line, "%s", CHATMGR->GetChatMsg( 2624 ) );
					}
					break;
				case eWearedItem_Costume_Wing:
					{
						sprintf( line, "%s", CHATMGR->GetChatMsg( 2625 ) );
					}
					break;
				case eWearedItem_Costume_Head:
				case eWearedItem_Costume_Dress:
				case eWearedItem_Costume_Glove:
				case eWearedItem_Costume_Shoes:
					{
						sprintf( line, "%s", CHATMGR->GetChatMsg( 1952 ) );
					}
					break;

				}

				switch(info->EquipSlot)
				{
				case eWearedItem_Ring1:
				case eWearedItem_Ring2:
					{
						// �Ǽ��縮 / ���� 
						sprintf( line, "%s / %s", line , CHATMGR->GetChatMsg( 1946 ) );
					}
					break;
				case eWearedItem_Necklace:
					{
						// �Ǽ��縮 / �����
						sprintf( line, "%s / %s", line , CHATMGR->GetChatMsg( 1944 ) );
					}
					break;
				case eWearedItem_Earring1:
				case eWearedItem_Earring2:
					{
						// �Ǽ��縮 / �Ͱ���
						sprintf( line, "%s / %s", line , CHATMGR->GetChatMsg( 1945 ) );
					}
					break;
				case eWearedItem_Costume_Head:
					{
						// �ڽ�Ƭ / ���
						sprintf( line, "%s / %s", line , CHATMGR->GetChatMsg( 1953 ) );
					}
					break;
				case eWearedItem_Costume_Dress:
					{
						// �ڽ�Ƭ / ����
						sprintf( line, "%s / %s", line , CHATMGR->GetChatMsg( 1954 ) );
					}
					break;
				case eWearedItem_Costume_Glove:
					{
						// �ڽ�Ƭ / �尩
						sprintf( line, "%s / %s", line , CHATMGR->GetChatMsg( 1955 ) );
					}
					break;
				case eWearedItem_Costume_Shoes:
					{
						// �ڽ�Ƭ / �Ź�
						sprintf( line, "%s / %s", line , CHATMGR->GetChatMsg( 1956 ) );
					}
					break;
				}

				icon.AddToolTipLine( line, TTTC_ITEM_NAME );
			}
			break;

			// 091008 pdy ��ȹ�� ��û �� �������� ������ġ�� �Ǵ��Ͽ� ������ ����ش�. 
			case eEquipType_Pet:
			{
				switch( ePetEquipSlot( info->EquipSlot - PET_EQUIP_SLOT_START ) )
				{
					// �� ���
					case ePetEquipSlot_Helmet:	
					case ePetEquipSlot_Armor:		
					case ePetEquipSlot_Glove:
					case ePetEquipSlot_Accessaries:
					case ePetEquipSlot_Weapon:
						{
							sprintf( line, "%s", CHATMGR->GetChatMsg( 1960 ) );
						}
						break;
					// �� �ڽ�Ƭ
					case ePetEquipSlot_Face:
					case ePetEquipSlot_Head:
					case ePetEquipSlot_Back:	
					case ePetEquipSlot_Tail:	
						{
							sprintf( line, "%s", CHATMGR->GetChatMsg( 1962 ) );
						}
						break;

					case ePetEquipSlot_Skill:
						{
							sprintf( line, "%s", CHATMGR->GetChatMsg( 1960 ) );
						}
						break;
				}

				switch( ePetEquipSlot( info->EquipSlot - PET_EQUIP_SLOT_START ) )
				{
					// �� ���
					case ePetEquipSlot_Helmet:
						{
							// �� ��� / ����
							sprintf( line, "%s / %s", line , CHATMGR->GetChatMsg( 684 ) );
						}
						break;
					case ePetEquipSlot_Armor:	
						{
							// �� ��� / ����
							sprintf( line, "%s / %s", line , CHATMGR->GetChatMsg( 683 ) );
						}
						break;
					case ePetEquipSlot_Glove:
						{
							// �� ��� / �尩
							sprintf( line, "%s / %s", line , CHATMGR->GetChatMsg( 685 ) );
						}
						break;
					case ePetEquipSlot_Accessaries:
						{
							// �� ��� / �Ǽ�����
							sprintf( line, "%s / %s", line , CHATMGR->GetChatMsg( 1943 ) );
						}
						break;
					case ePetEquipSlot_Weapon:
						{
							// �� ��� / ����
							sprintf( line, "%s / %s", line , CHATMGR->GetChatMsg( 1961 ) );
						}
						break;
					// �� �ڽ�Ƭ
					case ePetEquipSlot_Face:
						{
							// �� ��� / �����
							sprintf( line, "%s / %s", line , CHATMGR->GetChatMsg( 1950 ) );
						}
						break;
					case ePetEquipSlot_Head:
						{
							// �� �ڽ�Ƭ / �Ӹ����
							sprintf( line, "%s / %s", line , CHATMGR->GetChatMsg( 1951 ) );
						}
						break;
					case ePetEquipSlot_Back:	
						{
							// �� ��� / ��
							sprintf( line, "%s / %s", line , CHATMGR->GetChatMsg( 1948 ) );
						}
						break;
					case ePetEquipSlot_Tail:	
						{
							// �� ��� / ����
							sprintf( line, "%s / %s", line , CHATMGR->GetChatMsg( 1964 ) );
						}
						break;
					case ePetEquipSlot_Skill:
						{
							// �� ��� / ��ų
							sprintf( line, "%s / %s", line , CHATMGR->GetChatMsg( 1998 ) );
						}
						break;
				}

				icon.AddToolTipLine( line, TTTC_ITEM_NAME );
			}
			break;
	}
}


void CItemManager::AddDescriptionToolTip( cIcon& icon, const ITEMBASE& item, DWORD color )
{
	const ITEM_INFO* info = GetItemInfo( item.wIconIdx );

	if( ! info )
	{
		return;
	}

	const std::vector< std::string >& cutToolTipList = GetToolTipMsg( info->ItemTooltipIdx );

	// 080227 LUJ, �ɼ��� �ְų� ��� �������� ��� �� �� �絵�� �Ѵ�
	if( !cutToolTipList.empty() && 
		( GetOption( item.dwDBIdx ).mItemDbIndex || eEquipType_None != info->EquipType ) )
	{
		icon.AddToolTipLine( "" );
	}

	for( std::vector< std::string >::const_iterator cutToolTipListIter = cutToolTipList.begin() ; cutToolTipListIter != cutToolTipList.end() ; ++cutToolTipListIter )
	{
		const std::string& strTooltip = *cutToolTipListIter;
		icon.AddToolTipLine( strTooltip.c_str(), color );
	}
}

// 090430 pdy �Ͽ�¡������ ���� �߰� 
void CItemManager::AddFurnitureStateToolTip	( cIcon& icon, const ITEMBASE& item)
{
	const ITEM_INFO* pItemInfo = GetItemInfo( item.wIconIdx );

	if( !pItemInfo )
	{
		return;
	}

	if( pItemInfo->SupplyType != ITEM_KIND_FURNITURE )
		return;

	char tempBuf[512] = {0,};

	if( icon.GetType() == WT_HOUSING_STORED_ICON )
	{
		//���� �������� ���� 
		cHousingStoredIcon* pHS_StoredIcon = (cHousingStoredIcon*) &icon;
		if(! pHS_StoredIcon )
			return;

		stFurniture* pstFurniture = pHS_StoredIcon->GetLinkFurniture();
		if(! pstFurniture )
			return;

		switch(pstFurniture->wState)
		{
			case eHOUSEFURNITURE_STATE_KEEP:
				{
					//ȸ������ 
					sprintf(tempBuf,CHATMGR->GetChatMsg( 1846 )) ;
					icon.AddToolTipLine(tempBuf, RGB_HALF( 255, 255, 255 )) ;

					//��ġ���� 
					sprintf(tempBuf,CHATMGR->GetChatMsg( 1844 )) ;
					icon.AddToolTipLine(tempBuf, RGB_HALF( 255, 255, 255 )) ;
				}
				break;
			case eHOUSEFURNITURE_STATE_INSTALL:
				{
					//ȸ���Ұ�
					sprintf(tempBuf,CHATMGR->GetChatMsg( 1847 )) ;
					icon.AddToolTipLine(tempBuf, RGB_HALF( 255, 0, 0 )) ;

					//��ġ��
					sprintf(tempBuf,CHATMGR->GetChatMsg( 1845 )) ;
					icon.AddToolTipLine(tempBuf, RGB_HALF( 255, 0, 0 )) ;
				}
				break;
			case eHOUSEFURNITURE_STATE_UNINSTALL:
				{
					//ȸ���Ұ� 
					sprintf(tempBuf,CHATMGR->GetChatMsg( 1847 )) ;
					icon.AddToolTipLine(tempBuf, RGB_HALF( 255, 0, 0 )) ;

					//��ġ���� 
					sprintf(tempBuf,CHATMGR->GetChatMsg( 1844 )) ;
					icon.AddToolTipLine(tempBuf, RGB_HALF( 255, 255, 255 )) ;
				}
				break;
		}
	}
}


// 090430 pdy �Ͽ�¡������ ���� �߰� 
void CItemManager::AddFurnitureOptionToolTip	( cIcon& icon, const ITEMBASE& item)
{
	const ITEM_INFO* pItemInfo = GetItemInfo( item.wIconIdx );

	if( !pItemInfo )
	{
		return;
	}

	if( pItemInfo->SupplyType != ITEM_KIND_FURNITURE )
		return;

	char tempBuf[512] = {0,};

	stFunitureInfo* pstFurnitureInfo = GAMERESRCMNGR->GetFunitureInfo(pItemInfo->SupplyValue);
	if(! pstFurnitureInfo )
		return;

	icon.AddToolTipLine( "" );
	//�ٹ̱� ����Ʈ 
	sprintf(tempBuf,CHATMGR->GetChatMsg( 1848 ),pstFurnitureInfo->dwDecoPoint) ;
	icon.AddToolTipLine(tempBuf, RGB_HALF( 255, 255, 0 )) ;

	//�ױ� ����
	if( pstFurnitureInfo->bStackable )
	{
		sprintf(tempBuf,CHATMGR->GetChatMsg( 1849 )) ;
		icon.AddToolTipLine(tempBuf, RGB_HALF( 255, 255, 0 )) ;
	}

	//ȸ�� ����
	if( pstFurnitureInfo->bRotatable )
	{
		sprintf(tempBuf,CHATMGR->GetChatMsg( 1850 )) ;
		icon.AddToolTipLine(tempBuf, RGB_HALF( 255, 255, 0 )) ;
	}
	icon.AddToolTipLine( "" );

}

// 090430 pdy �Ͽ�¡������ ���� �߰� 
void CItemManager::AddFurnitureFunctionToolTip	( cIcon& icon, const ITEMBASE& item)
{
	const ITEM_INFO* pItemInfo = GetItemInfo( item.wIconIdx );

	if( !pItemInfo )
	{
		return;
	}

	if( pItemInfo->SupplyType != ITEM_KIND_FURNITURE )
		return;

	char tempBuf[512] = {0,};


	DWORD dwFunction = Get_Function(pItemInfo->SupplyValue);

	switch(dwFunction)
	{
		case eHOUSE_Function_None:
			{
				//�Ϲ� ���ڷ��̼� 
				sprintf(tempBuf,CHATMGR->GetChatMsg( 1851 )) ;
				icon.AddToolTipLine(tempBuf, RGB_HALF( 255, 255, 0 )) ;
			}
			break;
		case eHOUSE_Function_Wall :
			{
				//�����ܺ� 
				sprintf(tempBuf,CHATMGR->GetChatMsg( 1852 )) ;
				icon.AddToolTipLine(tempBuf, RGB_HALF( 255, 255, 0 )) ;
			}
			break;
		case eHOUSE_Function_Floor :
			{
				//�����ٴ�
				sprintf(tempBuf,CHATMGR->GetChatMsg( 1853 )) ;
				icon.AddToolTipLine(tempBuf, RGB_HALF( 255, 255, 0 )) ;
			}
			break;
		case eHOUSE_Function_Door:
			{
				//��
				sprintf(tempBuf,CHATMGR->GetChatMsg( 1854 )) ;
				icon.AddToolTipLine(tempBuf, RGB_HALF( 255, 255, 0 )) ;
			}
			break;
		case eHOUSE_Function_Start:
			{
				//������ġ
				sprintf(tempBuf,CHATMGR->GetChatMsg( 1855 )) ;
				icon.AddToolTipLine(tempBuf, RGB_HALF( 255, 255, 0 )) ;
			}
			break;
		case eHOUSE_Function_Action:
			{
				//��ɼ�
				sprintf(tempBuf,CHATMGR->GetChatMsg( 1856 )) ;
				icon.AddToolTipLine(tempBuf, RGB_HALF( 255, 255, 0 )) ;
			}
			break;
	}
}

void CItemManager::ItemDelete(CItem* pItem)
{
	m_ItemHash.Remove(pItem->GetDBIdx());	
	//ICFreeIndex(&m_IconIndexCreator,pItem->GetID()-START_ITEMTCON_ID);
	m_IconIndexCreator.ReleaseIndex(pItem->GetID());
	
//KES EXCHANGE 031002
	if( pItem->GetLinkItem() )
	{
		GAMEIN->GetExchangeDialog()->DelItem( 0, pItem->GetLinkItem()->GetPosition() );
	}
//

//	WINDOWMGR->DeleteWindowForID(pItem->GetID());	//KES ������������ÿ�. 031128
	WINDOWMGR->AddListDestroyWindow( pItem );
}


void CItemManager::Release()
{
	CItem* pItem;
	m_ItemHash.SetPositionHead();
	while((pItem = m_ItemHash.GetData()) != NULL)
	{
		ItemDelete(pItem);
		//ICFreeIndex(&m_IconIndexCreator,pItem->GetID()-START_ITEMTCON_ID);		
	}
	m_ItemHash.RemoveAll();

	{
		for(
			OptionMap::iterator it = mOptionMap.begin();
			mOptionMap.end() != it;
			++it )
		{
			SAFE_DELETE( it->second );
		}

		mOptionMap.clear();
	}
}


CItem* CItemManager::GetItem(DWORD dwDBIdx)
{
	return m_ItemHash.GetData( dwDBIdx );
}


void CItemManager::CombineItem( MSG_ITEM_COMBINE_ACK * pMsg )
{	
	const eITEMTABLE FromTableIdx = GetTableIdxForAbsPos(pMsg->FromPos);
	const eITEMTABLE ToTableIdx = GetTableIdxForAbsPos(pMsg->ToPos);

	CItem * pFromItem = GetItemofTable(FromTableIdx, pMsg->FromPos);
	CItem * pToItem = GetItemofTable(ToTableIdx, pMsg->ToPos);

	if( !pFromItem || !pToItem )
	{
		ASSERTMSG(0, "Item Combine Failed : pToItem=NULL or pFromItem=NULL");
		return;
	}
	if( ( pFromItem->GetItemIdx() != pToItem->GetItemIdx() ) || ( pFromItem->GetItemIdx() != pMsg->wItemIdx) )
//	if( ( pFromItem->GetItemIdx() != pToItem->GetItemIdx() ) || ( pFromItem->GetItemIdx() != pMsg->CombineItemIdx ) )
	{
		ASSERTMSG(0, "Item Combine Failed : FromItemIdx != ToItemIdx");
		return;
	}

	if(eITEM_TYPE_UNSEAL == pToItem->GetItemBaseInfo().nSealed)
	{
		const DWORD totalRemainSecond = pFromItem->GetItemBaseInfo().nRemainSecond * (pFromItem->GetDurability() - pMsg->FromDur) + pToItem->GetItemBaseInfo().nRemainSecond * pToItem->GetDurability();
		const DWORD combinedRemainSecond = totalRemainSecond / (pFromItem->GetDurability() - pMsg->FromDur + pToItem->GetDurability());

		pToItem->SetRemainTime(
			combinedRemainSecond);
#ifdef _GMTOOL_
		OutputDebug(
			"Items are merged with time");
		OutputDebug(
			"\tfrom: second: %d, count: %d, server count: %d",
			pFromItem->GetItemBaseInfo().nRemainSecond,
			pFromItem->GetDurability(),
			pMsg->FromDur);
		OutputDebug(
			"\tto: second: %d, count: %d, server count: %d",
			pToItem->GetItemBaseInfo().nRemainSecond,
			pToItem->GetDurability(),
			pMsg->ToDur);
		OutputDebug(
			"\tfrom: %d, to: %d. second",
			pFromItem->GetItemBaseInfo().nRemainSecond * (pFromItem->GetDurability() - pMsg->FromDur),
			pToItem->GetItemBaseInfo().nRemainSecond * pToItem->GetDurability());
		OutputDebug(
			"\ttotal: %d, count: %d, result: %d",
			totalRemainSecond,
			pFromItem->GetDurability() - pMsg->FromDur + pToItem->GetDurability(),
			combinedRemainSecond);
#endif
	}

	pFromItem->SetDurability( pMsg->FromDur );
	pToItem->SetDurability( pMsg->ToDur );

	// 100422 ShinJS --- �κ��丮 ������ ����� ����Ʈ ������ �����Ѵ�.
	GAMEIN->GetQuestQuickViewDialog()->ResetQuickViewDialog();
	GAMEIN->GetQuestDialog()->RefreshQuestList();
	QUESTMGR->ProcessNpcMark();
	
	if( pMsg->FromDur == 0 )
	{
		DeleteItemofTable(
			FromTableIdx,
			pFromItem->GetPosition(),
			TRUE);
	}

	if( pMsg->ToDur == 0 )
	{
		DeleteItemofTable(
			FromTableIdx,
			pFromItem->GetPosition(),
			TRUE);
	}
}

void CItemManager::MoveItem( MSG_ITEM_MOVE_ACK * pMsg )
{
	const eITEMTABLE tableIdx = GetTableIdxForAbsPos(pMsg->ToPos);

	if(tableIdx == eItemTable_Storage)
		MoveItemToStorage(pMsg);
	else if(tableIdx == eItemTable_MunpaWarehouse)
		MoveItemToGuild(pMsg);
	else if(tableIdx == eItemTable_Shop)
		MoveItemToShop(pMsg);
	else if(tableIdx == eItemTable_Inventory || tableIdx == eItemTable_Weared)
	{
		CInventoryExDialog * pInven = GAMEIN->GetInventoryDialog();
		CStorageDialog* pStorage = GAMEIN->GetStorageDialog();
		CGuildWarehouseDialog* pGuild = GAMEIN->GetGuildWarehouseDlg();
		CItemShopDialog* pShop = GAMEIN->GetItemShopDialog();
		
		CItem * FromItem = NULL;
		CItem * ToItem = NULL;
		
		pInven->DeleteItem(pMsg->ToPos, &ToItem);

		const eITEMTABLE FromTableIdx = GetTableIdxForAbsPos(pMsg->FromPos);

		if(FromTableIdx == eItemTable_Storage)
		{
			pStorage->DeleteItem( pMsg->FromPos, &FromItem );
		}
		else if(FromTableIdx == eItemTable_MunpaWarehouse)
		{
			pGuild->DeleteItem( pMsg->FromPos, &FromItem );
		}
		else if(FromTableIdx == eItemTable_Shop)
		{
			// �Ŀ� �������� �κ����� ������ �߰�

			pShop->DeleteItem( pMsg->FromPos, &FromItem );
		}
		else 
			pInven->DeleteItem( pMsg->FromPos , &FromItem );

		if(ToItem)
		{
			// 080820 LUJ, ȹ�� �� ��Ÿ���� ���� ������ ������ �����Ѵ�
			UpdateCoolTimeItem( *ToItem );

			ToItem->SetPosition( pMsg->FromPos );

			if(FromTableIdx == eItemTable_Storage)
			{
				if( ToItem->IsQuickLink() )
				{
					ToItem->RemoveQuickLink();
				}
				pStorage->AddItem(ToItem);
			}
			else if(FromTableIdx == eItemTable_MunpaWarehouse)
			{
				if( ToItem->IsQuickLink() )
				{
					ToItem->RemoveQuickLink();
				}
				pGuild->AddItem(ToItem);
				QUICKMGR->RefreshQickItem();
			}
			else
				pInven->AddItem(ToItem);
		}
		
		if(FromItem == 0)
		{
			ASSERT(0);
			return;
		}

		// 080820 LUJ, ȹ�� �� ��Ÿ���� ���� ������ ������ �����Ѵ�
		UpdateCoolTimeItem( *FromItem );

		FromItem->SetPosition(pMsg->ToPos);
		pInven->AddItem(FromItem); 
		QUICKMGR->RefreshQickItem();
	}
}

void CItemManager::PetMoveItem( MSG_ITEM_MOVE_ACK * pMsg )
{
	CPetWearedDialog* pPetDlg = GAMEIN->GetPetWearedDlg();
	CInventoryExDialog * pInven = GAMEIN->GetInventoryDialog();
	CStorageDialog* pStorage = GAMEIN->GetStorageDialog();

	CItem * FromItem = NULL;
	cIcon * buf = NULL;
	CItem * ToItem = NULL;

	pPetDlg->DeleteItem(pMsg->ToPos, &buf);
	ToItem = ( CItem* )buf;

	const eITEMTABLE FromTableIdx = GetTableIdxForAbsPos(pMsg->FromPos);

	if(FromTableIdx == eItemTable_Storage)
	{
		pStorage->DeleteItem( pMsg->FromPos, &FromItem );
	}
	else 
		pInven->DeleteItem( pMsg->FromPos , &FromItem );

	if(ToItem)
	{
		ToItem->SetPosition( pMsg->FromPos );

		ITEM_INFO* pinfo = GetItemInfo( ToItem->GetItemBaseInfo().wIconIdx );
		if( pinfo )
		{
			if(FromTableIdx == eItemTable_Storage)
			{
				if( ToItem->IsQuickLink() )
				{
					ToItem->RemoveQuickLink();
				}
				pStorage->AddItem(ToItem);
			}
			else
				pInven->AddItem(ToItem);
		}
	}	

	if(FromItem)
	{
		FromItem->SetPosition( pMsg->ToPos );
		pPetDlg->AddItem( pMsg->ToPos, (cIcon*)FromItem );
	}

	QUICKMGR->RefreshQickItem();
}

void CItemManager::MoveItemToGuild( MSG_ITEM_MOVE_ACK* pMsg )
{	
	CGuildWarehouseDialog * pGuildDlg	= GAMEIN->GetGuildWarehouseDlg();
	CInventoryExDialog * pInvenDlg = GAMEIN->GetInventoryDialog();
	const eITEMTABLE FromTableIdx = GetTableIdxForAbsPos(pMsg->FromPos);
	CItem * FromItem = NULL;
	CItem * ToItem = NULL;

	pGuildDlg->DeleteItem(pMsg->ToPos , &ToItem);

	if(FromTableIdx == eItemTable_MunpaWarehouse)
	{
		pGuildDlg->DeleteItem( pMsg->FromPos, &FromItem);
	}
	else if(FromTableIdx == eItemTable_Inventory || FromTableIdx == eItemTable_Weared)
	{
		pInvenDlg->DeleteItem( pMsg->FromPos, &FromItem);
	}
	else 
		ASSERT(0);
	
	if(ToItem)
	{
		// 080820 LUJ, ȹ�� �� ��Ÿ�� �������� �̵� �ø��� �����ؾ��Ѵ�
		UpdateCoolTimeItem( *ToItem );

		ToItem->SetPosition( pMsg->FromPos );
		if(FromTableIdx == eItemTable_Inventory)
			pInvenDlg->AddItem(ToItem);
		else
			pGuildDlg->AddItem(ToItem);
	}

	ReLinkQuickPosition(FromItem);
	if( FromItem->IsQuickLink() )
	{
		FromItem->RemoveQuickLink();
	}
	ASSERT(FromItem);

	// 080820 LUJ, ȹ�� �� ��Ÿ�� �������� �̵� �ø��� �����ؾ��Ѵ�
	UpdateCoolTimeItem( *FromItem );

	FromItem->SetPosition(pMsg->ToPos);
	pGuildDlg->AddItem(FromItem);
	
	QUICKMGR->RefreshQickItem();

	SetDisableDialog(FALSE, eItemTable_Inventory);
	SetDisableDialog(FALSE, eItemTable_Storage);
	SetDisableDialog(FALSE, eItemTable_MunpaWarehouse);
}

void CItemManager::MoveItemToStorage(MSG_ITEM_MOVE_ACK* pMsg)
{
	CInventoryExDialog * pInven = GAMEIN->GetInventoryDialog();
	CStorageDialog* pStorage = GAMEIN->GetStorageDialog();
	
	CItem * FromItem = NULL;
	CItem * ToItem = NULL;

	pStorage->DeleteItem(pMsg->ToPos, &ToItem);
	
	const eITEMTABLE FromTableIdx = GetTableIdxForAbsPos(pMsg->FromPos);

	if(FromTableIdx == eItemTable_Inventory || FromTableIdx == eItemTable_Weared)
	{
		pInven->DeleteItem( pMsg->FromPos, &FromItem );				
	}
		
	else if(FromTableIdx == eItemTable_Storage)
	{
		pStorage->DeleteItem( pMsg->FromPos, &FromItem );
	}
	else
	{
		ASSERT(0);
		return;
	}
		
	if(ToItem)
	{
		// 080820 LUJ, ȹ�� �� ��Ÿ�� �������� �̵� �ø��� �����ؾ��Ѵ�
		UpdateCoolTimeItem( *ToItem );

		ToItem->SetPosition( pMsg->FromPos );
		if(FromTableIdx == eItemTable_Inventory || FromTableIdx == eItemTable_Weared)
			pInven->AddItem(ToItem);
		else
			pStorage->AddItem(ToItem);
		if( ToItem->IsQuickLink() )
		{
			ToItem->RemoveQuickLink();
		}
	}

	ReLinkQuickPosition(FromItem);
	
	if( FromItem->IsQuickLink() )
	{
		FromItem->RemoveQuickLink();
	}
	ASSERT(FromItem);

	// 071125 LYW --- ItemManager : �ð� ������ �̵� ó�� �߰�.
	const ITEMBASE& pItemBase = FromItem->GetItemBaseInfo() ;

	if( pItemBase.nSealed == eITEM_TYPE_UNSEAL )
	{
		const ITEM_OPTION& pOption = GetOption( FromItem->GetItemBaseInfo() ) ;

		AddTimeToolTip(((CItem*)FromItem), pOption, FALSE );
	}

	// 080820 LUJ, ȹ�� �� ��Ÿ�� �������� �̵� �ø��� �����ؾ��Ѵ�
	UpdateCoolTimeItem( *FromItem );

	FromItem->SetPosition(pMsg->ToPos);
	pStorage->AddItem(FromItem);
	
	QUICKMGR->RefreshQickItem();
}



void CItemManager::MoveItemToShop(MSG_ITEM_MOVE_ACK* pMsg)
{
	CItemShopDialog* pShop = GAMEIN->GetItemShopDialog();
	
	CItem * FromItem = NULL;
	CItem * ToItem = NULL;

	pShop->DeleteItem(pMsg->ToPos, &ToItem);

	const eITEMTABLE FromTableIdx = GetTableIdxForAbsPos(pMsg->FromPos);

	if( FromTableIdx == eItemTable_Shop )
	{
		pShop->DeleteItem( pMsg->FromPos, &FromItem );				
	}
		
	if(ToItem)
	{
		// 080820 LUJ, ȹ�� �� ��Ÿ�� �������� �̵� �ø��� �����ؾ��Ѵ�
		UpdateCoolTimeItem( *ToItem );

		ToItem->SetPosition( pMsg->FromPos );		

		if( FromTableIdx == eItemTable_Shop )		
			pShop->AddItem(ToItem);	

		if( ToItem->IsQuickLink() )
		{
			ToItem->RemoveQuickLink();
		}
	}

	ReLinkQuickPosition(FromItem);
	if( FromItem->IsQuickLink() )
	{
		FromItem->RemoveQuickLink();
	}
	ASSERT(FromItem);

	// 080820 LUJ, ȹ�� �� ��Ÿ�� �������� �̵� �ø��� �����ؾ��Ѵ�
	UpdateCoolTimeItem( *FromItem );

	FromItem->SetPosition(pMsg->ToPos);
	pShop->AddItem(FromItem);

	QUICKMGR->RefreshQickItem();
}

void CItemManager::DeleteItem( POSTYPE absPos, CItem** ppItemOut )
{
	switch(GetTableIdxForAbsPos(absPos))
	{
	case eItemTable_Inventory:
	case eItemTable_Weared:
		{
			CInventoryExDialog * pInven = GAMEIN->GetInventoryDialog();
			pInven->DeleteItem( absPos, ppItemOut );

		}break;
	case eItemTable_Storage:
		{
			CStorageDialog* pStorage = GAMEIN->GetStorageDialog();
			pStorage->DeleteItem( absPos, ppItemOut );
			
		}break;
	case eItemTable_MunpaWarehouse:
		{
			CGuildWarehouseDialog* pGuild = GAMEIN->GetGuildWarehouseDlg();
			pGuild->DeleteItem( absPos, ppItemOut );
		}break;
	}

	if((*ppItemOut))
	{
		RemoveOption((*ppItemOut)->GetDBIdx());
		
		if( (*ppItemOut)->IsQuickLink() )
		{
			(*ppItemOut)->RemoveQuickLink();
		}

		ItemDelete( (*ppItemOut) );
	}
		
}

void CItemManager::DeleteItemofTable(eITEMTABLE TableIdx, POSTYPE absPos, BOOL bSellDel)
{
	CItem * pItemOut = NULL;
	
	switch(TableIdx)
	{
	case eItemTable_Inventory:
	case eItemTable_Weared:
		{
			CInventoryExDialog * pDlg = GAMEIN->GetInventoryDialog();
			if( !pDlg->DeleteItem( absPos, &pItemOut ) )
			{
				ASSERTMSG(0, "Item Delete Failed : DeleteItem return FALSE");
				return;
			}

			const ITEMBASE& pItemBase = pItemOut->GetItemBaseInfo() ;
			
			if( pItemBase.nSealed == eITEM_TYPE_UNSEAL && !bSellDel )
			{
				if( m_nSealItemDelPos != (int)pItemBase.Position )
				{
					CHATMGR->AddMsg( CTC_CHEAT_1, CHATMGR->GetChatMsg( 1192 ), CHATMGR->GetChatMsg( 1189 ), pItemOut->GetItemInfo()->ItemName );
				}
			}

			// 071127 LYW --- ItemManager : ���� ������ ( ���� ���� ) ��ġ ���� ���� �߰�.
			m_nSealItemDelPos = -1 ;
		}
		break;
	case eItemTable_Storage:
		{
			CStorageDialog * pDlg = GAMEIN->GetStorageDialog();
			if( !pDlg->DeleteItem( absPos, &pItemOut ) )
			{
				ASSERTMSG(0, "Item Delete Failed : DeleteItem return FALSE");
				return;
			}

			// 071124 LYW --- ItemManager : �ð� �Ϸ� ������ ���� �޽��� �߰�.
			const ITEMBASE& pItemBase = pItemOut->GetItemBaseInfo() ;
			
			if( pItemBase.nSealed == eITEM_TYPE_UNSEAL )
			{
				if( m_nSealItemDelPos != (int)pItemBase.Position )
				{
					CHATMGR->AddMsg( CTC_CHEAT_1, CHATMGR->GetChatMsg( 1192 ), CHATMGR->GetChatMsg( 1188 ), pItemOut->GetItemInfo()->ItemName );
				}
			}

			// 071127 LYW --- ItemManager : ���� ������ ( ���� ���� ) ��ġ ���� ���� �߰�.
			m_nSealItemDelPos = -1 ;
		}
		break;
	case eItemTable_MunpaWarehouse:
		{
			CGuildWarehouseDialog * pDlg = GAMEIN->GetGuildWarehouseDlg();
			if( !pDlg->DeleteItem( absPos, &pItemOut ) )
			{
				ASSERTMSG(0, "Item Delete Failed : DeleteItem return FALSE");
				return;
			}			
		}
		break;
	case eItemTable_Shop:
		{
			CItemShopDialog* pDlg = GAMEIN->GetItemShopDialog();
			if( !pDlg->DeleteItem( absPos, &pItemOut ) )
			{
				ASSERTMSG(0, "Item Delete Failed : DeleteItem return FALSE");
				return;
			}
		}
		break;
	}
	if(pItemOut)
	{
		RemoveOption(pItemOut->GetDBIdx() );

		if( pItemOut->IsQuickLink() )
		{
			pItemOut->RemoveQuickLink();
		}
		ItemDelete( pItemOut );
	}
}


void CItemManager::RemoveOption( DWORD dwOptionIdx )
{
	OptionMap::iterator it = mOptionMap.find( dwOptionIdx );

	if( mOptionMap.end() == it )
	{
		return;
	}

	SAFE_DELETE( it->second );

	mOptionMap.erase( it );
}


void CItemManager::AddOption( const ITEM_OPTION& option )
{
	OptionMap::iterator it = mOptionMap.find( option.mItemDbIndex );

	// �̹� �Ҵ� ������ ������ �ش� �޸𸮿� �����ع�����
	if( mOptionMap.end() != it )
	{
		*( it->second ) = option;
		return;
	}

	mOptionMap.insert( std::make_pair( option.mItemDbIndex, new ITEM_OPTION( option ) ) );
}



const ITEM_OPTION& CItemManager::GetOption( const ITEMBASE& item ) const
{
	return GetOption( item.dwDBIdx );
}


const ITEM_OPTION& CItemManager::GetOption( DWORD itemDbIndex ) const
{
	OptionMap::const_iterator it = mOptionMap.find( itemDbIndex );

	if( mOptionMap.end() == it )
	{
		const static ITEM_OPTION emptyOption = {0};

		return emptyOption;
	}

	return *( it->second );
}


void CItemManager::NetworkMsgParse(BYTE Protocol,void* pMsg)
{
	switch(Protocol)
	{
	case MP_ITEM_TOTALINFO_LOCAL:					Item_TotalInfo_Local( pMsg ) ;					break;
	case MP_ITEM_STORAGE_ITEM_INFO_ACK:				Item_Storage_Item_Info_Ack( pMsg ) ;			break;
	case MP_ITEM_MOVE_ACK:							Item_Move_Ack( pMsg ) ;							break;
	case MP_ITEM_COMBINE_ACK:						Item_Combine_Ack( pMsg ) ;						break;
	case MP_ITEM_DIVIDE_ACK:						Item_Divide_Ack( pMsg ) ;						break;
	case MP_ITEM_MONSTER_OBTAIN_NOTIFY:				Item_Monster_Obtain_Notify( pMsg ) ;			break;
	case MP_ITEM_MOVE_NACK:							Item_Move_Nack( pMsg ) ;						break;
	case MP_ITEM_DIVIDE_NACK:						Item_Divite_Nack( pMsg ) ;						break;
	case MP_ITEM_COMBINE_NACK:						Item_Combine_Nack( pMsg ) ;						break;
	case MP_ITEM_DISCARD_NACK:						Item_Discard_Nack( pMsg ) ;						break;
	case MP_ITEM_DISCARD_ACK:						Item_Discard_Ack( pMsg ) ;						break;
	case MP_ITEM_USE_NOTIFY:						Item_Use_Notify( pMsg ) ;						break;
	case MP_ITEM_USE_ACK:							Item_Use_Ack( pMsg ) ;							break;
	case MP_ITEM_REINFORCE_OPTION_ACK:				Item_Reinforce_Option_Ack( pMsg );				break;
	case MP_ITEM_REINFORCE_NACK:					Item_ReInForce_Nack( pMsg ) ;					break;
	case MP_ITEM_REINFORCE_SUCCEED_ACK:
	case MP_ITEM_REINFORCE_FAILED_ACK:
		{
			Item_ReInForce_Ack( pMsg );
			break;
		}	
	case MP_ITEM_COOK_ACK:
		{
			Item_Cook_Ack( pMsg);
			break;
		}
	case MP_ITEM_HOUSE_RESTORED:
		{
			Item_HouseRestored( pMsg ) ;
			break;
		}
	case MP_ITEM_HOUSE_BONUS:
		{
			Item_HouseBonus( pMsg ) ;
			break;
		}
	case MP_ITEM_MIX_SUCCEED_ACK:
	case MP_ITEM_MIX_FAILED_ACK:
	case MP_ITEM_MIX_BIGFAILED_ACK:
		{
			Item_Mix_Ack( pMsg );
			break;
		}
	case MP_ITEM_MIX_NACK:							Item_Mix_Nack( pMsg ) ;							break;
	    case MP_ITEM_TRANSMOG_SUCCESS_ACK:				Item_Transmog_Success_Ack( pMsg ) ;				break;
	case MP_ITEM_TRANSMOG_NACK:						Item_Transmog_Nack( pMsg ) ;					break;
	case MP_ITEM_TRANSMOG_FAILED_ACK:				Item_Transmog_Failed_Ack( pMsg );				break;
	case MP_ITEM_MIX_GETITEM:						Item_Mix_GetItem( pMsg );						break;
	case MP_ITEM_COOK_GETITEM:						Item_Cook_GetItem( pMsg );						break;
	case MP_ITEM_ENCHANT_SUCCESS_ACK:				Item_Enchant_Success_Ack( pMsg ) ;				break;
	case MP_ITEM_ENCHANT_NACK:						Item_Enchant_Nack( pMsg ) ;						break;
	case MP_ITEM_ENCHANT_FAILED_ACK:				Item_Enchant_Failed_Ack( pMsg );				break;
	case MP_ITEM_WORKING_SUCCESS:					Item_Working_Success( pMsg ) ;					break;
	case MP_ITEM_WORKING_FAIL:						Item_Working_Fail( pMsg ) ;						break;
	case MP_ITEM_WORKING_START:						Item_Working_Start( pMsg ) ;					break;
	case MP_ITEM_WORKING_STARTEX:					Item_Working_StartEx( pMsg ) ;					break;
	case MP_ITEM_WORKING_STOP:						Item_Working_Stop( pMsg ) ;						break;
	case MP_ITEM_USE_NACK:							Item_Use_Nack( pMsg ) ;							break;
	case MP_ITEM_SELL_ACK:							Item_Sell_Ack( pMsg ) ;							break;
	case MP_ITEM_SELL_NACK:							Item_Sell_Nack( pMsg ) ;						break;
	case MP_ITEM_BUY_ACK:							Item_Buy_Ack( pMsg ) ;							break;
	case MP_ITEM_BUY_NACK:							Item_Buy_Nack( pMsg ) ;							break;
	case MP_ITEM_BUY_BY_COIN:						Item_Buy_By_Coin( pMsg ) ;						break;			// 090227 ShinJS --- �ϻ��� �߰��۾� : Coin Item ���Խ� Coin ���� �۾�
	case MP_ITEM_DEALER_ACK:						Item_Dealer_Ack( pMsg ) ;						break;
	case MP_ITEM_APPEARANCE_ADD:					Item_Appearance_Add( pMsg );					break;
	case MP_ITEM_APPEARANCE_REMOVE:					Item_Appearance_Remove( pMsg );					break;
	case MP_ITEM_MONEY:								Item_Money( pMsg ) ;							break;
	case MP_ITEM_MONEY_ERROR:		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(115) );		break;
	case MP_ITEM_OBTAIN_MONEY:						Item_Obtain_Money( pMsg ) ;						break;
	case MP_ITEM_DIVIDE_NEWITEM_NOTIFY:				Item_Divide_NewItem_Notify( pMsg ) ;			break;
	case MP_ITEM_GUILD_MOVE_ACK:					Item_Guild_Move_Ack( pMsg ) ;					break;
	case MP_ITEM_GUILD_MOVE_NACK:					Item_Guild_Move_Nack( pMsg ) ;					break;
	case MP_ITEM_DELETEFROM_GUILDWARE_NOTIFY:		Item_DeleteFrom_GuildWare_Notify( pMsg ) ;		break;
	case MP_ITEM_ADDTO_GUILDWARE_NOTIFY:			Item_AddTo_GuildWare_Notify( pMsg ) ;			break;
	case MP_ITEM_STORAGE_MOVE_ACK:					Item_Storage_Move_Ack( pMsg ) ;					break;
	case MP_ITEM_DISSOLVE_GETITEM:					Item_Dissolve_GetItem( pMsg ) ;					break;
	case MP_ITEM_DISSOLVE_ACK:						Item_Dissolve_Ack( pMsg ) ;						break;
	case MP_ITEM_DISSOLVE_NACK:						Item_Dissolve_Nack( pMsg ) ;					break;	
	case MP_ITEM_ERROR_NACK:						Item_Error_Nack( pMsg ) ;						break;
	case MP_ITEM_CHANGEITEM_NACK:					Item_ChangeItem_Nack( pMsg ) ;					break;
	case MP_ITEM_USE_CHANGEITEM_ACK:				Item_Use_ChangeItem_Ack( pMsg ) ;				break;
	case MP_ITEM_TIMELIMT_ITEM_ONEMINUTE :			Item_TimeLimit_Item_OneMinute( pMsg ) ;			break;
	case MP_ITEM_SHOPITEM_USE_ACK:					Item_ShopItem_Use_Ack( pMsg ) ;					break;
	case MP_ITEM_EVENTITEM_USE:						Item_EventItem_Use( pMsg ) ;					break;
	case MP_ITEM_SHOPITEM_INFO_ACK:					Item_ShopItem_Info_Ack( pMsg ) ;				break;
	case MP_ITEM_SKILL_ACK :						Item_Skill_Ack( pMsg ) ;						break;
	case MP_ITEM_RESET_STATS_ACK :					Item_Reset_Stats_Ack( pMsg ) ;					break;
	case MP_ITEM_RESET_STATS_NACK :					Item_Reset_Stats_Nack( pMsg ) ;					break;
	case MP_ITEM_MAPMOVE_SCROLL_NACK :				Item_MapMoveScroll_Nack( pMsg ) ;				break;
	case MP_ITEM_INCREASE_INVENTORY_ACK :			Item_Increase_Inventory_Ack( pMsg ) ;			break;
	case MP_ITEM_INCREASE_INVENTORY_NACK :			Item_Increase_Inventory_Nack( pMsg ) ;			break;
	case MP_ITEM_CHANGE_CHARACTER_NAME_ACK :		Item_Change_Character_Name_Ack( pMsg ) ;		break;
	case MP_ITEM_CHANGE_CHARACTER_NAME_NACK :		Item_Change_Character_Name_Nack( pMsg ) ;		break;
	case MP_ITEM_GIFT_EVENT_NOTIFY:					Item_Gift_Event_Notify( pMsg );					break;

	// 080124 LUJ, �ɼ� �߰�
	case MP_ITEM_OPTION_ADD_ACK:
		{
			const MSG_ITEM_OPTION* message = ( MSG_ITEM_OPTION* )pMsg;

			for( DWORD i = 0; i < message->mSize; ++i )
			{
				const ITEM_OPTION& option = message->mOption[ i ];

				AddOption( option );
			}
			break;
		}
	// 080218 LUJ, ������ �ɼ� �ο�
	case MP_ITEM_APPLY_OPTION_ACK:
		{
			CApplyOptionDialog* dialog = ( CApplyOptionDialog* )WINDOWMGR->GetWindowForID( APPLY_OPTION_DIALOG );

			if( dialog )
			{
				dialog->Succeed();
			}

			break;
		}
	// 080218 LUJ, ������ �ɼ� �ο�
	case MP_ITEM_APPLY_OPTION_NACK:
		{
			CApplyOptionDialog* dialog = ( CApplyOptionDialog* )WINDOWMGR->GetWindowForID( APPLY_OPTION_DIALOG );

			if( dialog )
			{
				dialog->Fail();
			}

			break;
		}

	// 080317 NYJ --- ��ü��ġ�� ������ ������
	case MP_ITEM_SHOPITEM_SHOUT_NACK:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1461 ) );
			break;
		}

	case MP_ITEM_ENCHANT_EFFECT_NOTIFY:
		{
			MSG_DWORDBYTE* pmsg = ( MSG_DWORDBYTE* )pMsg;

			CObject* pTarget = OBJECTMGR->GetObject( pmsg->dwData );

			if( !pTarget || pTarget->GetObjectKind() != eObjectKind_Player )
				break;

			CPlayer* pPlayer = ( CPlayer* )pTarget;

			pPlayer->GetCharacterTotalInfo()->WeaponEnchant = pmsg->bData;
		}
		break;
	case MP_ITEM_PET_MOVE_ACK:	Item_Pet_Move_Ack( pMsg ) ;	break;

		// 080916 LUJ, ������ �ռ� ����
	case MP_ITEM_COMPOSE_ACK:
		{
			CComposeDialog* dialog = GAMEIN->GetComposeDialog();

			if( dialog )
			{
				const MSG_ITEM_COMPOSE_ACK* message = ( MSG_ITEM_COMPOSE_ACK* )pMsg;
				dialog->Proceed( *message );
			}
		}
		break;
		// 080916 LUJ, ������ �ռ� ����
	case MP_ITEM_COMPOSE_NACK:
		{
			CComposeDialog* dialog = GAMEIN->GetComposeDialog();

			if( dialog )
			{
				const MSG_ITEM_COMPOSE_NACK* message = ( MSG_ITEM_COMPOSE_NACK* )pMsg;
				dialog->Proceed( *message );
			}
		}
		break;

	case MP_ITEM_CHANGESIZE_ACK:
		{
			MSG_DWORD2INT* pmsg = (MSG_DWORD2INT*)pMsg;
			CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject( pmsg->dwData1 );
			if( !pPlayer ) return;

			float fSize = (float)pmsg->dwData2 / 100.f;
			if(fSize < .3f)
				fSize = 0.3f;
			else if(fSize > 3.0f)
				fSize = 3.0f;

			if(pPlayer->GetID() == gHeroID)
			{
				int nDiffSize = pmsg->nData;

				if(nDiffSize < -70)
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1735));
				else if(-70<=nDiffSize && nDiffSize<-40)
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1734));
				else if(-40<=nDiffSize && nDiffSize<-10)
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1733));
				else if(-10<=nDiffSize && nDiffSize<11)
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1736));
				else if(11<=nDiffSize && nDiffSize<41)
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1730));
				else if(41<=nDiffSize && nDiffSize<71)
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1731));
				else if(71 < nDiffSize)
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1732));
			}

			
			pPlayer->SetCharacterSize( fSize );
		}
		break;

	case MP_ITEM_USEEFFECT_ACK:
		{
			MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
			CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject( pmsg->dwData1 );
			if( !pPlayer ) return;

			OBJECTEFFECTDESC desc;
			desc.SetDesc((WORD)pmsg->dwData2);
			pPlayer->RemoveObjectEffect( ITEM_SUMMON_EFFECTID );
			pPlayer->AddObjectEffect( ITEM_SUMMON_EFFECTID, &desc, 1 );
		}
		break;
	// 100517 ONS �����ʱ�ȭ ������ ó�� �߰�
	case MP_ITEM_CHANGE_CLASS_ACK:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 2314 ) );
		}
		break;
	case MP_ITEM_CHANGE_CLASS_NACK:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 2226 ) );
		}
		break;
	case MP_ITEM_ENCHANT_LEVEL_NOTIFY:
		{
			EnchantNotifyAdd( pMsg );
		}
		break;
	}
}

BOOL CItemManager::IsEqualTableIdxForPos(eITEMTABLE TableIdx, POSTYPE absPos )
{
	if(TP_INVENTORY_START <= absPos && absPos < (TP_INVENTORY_END + ( TABCELL_INVENTORY_NUM * HERO->Get_HeroExtendedInvenCount() )))
	{
		return (TableIdx == eItemTable_Inventory);
	}
	else if(TP_WEAR_START <= absPos && absPos < TP_WEAR_END)
	{
		return (TableIdx == eItemTable_Weared);
	}
	else if(TP_GUILDWAREHOUSE_START <= absPos && absPos < TP_GUILDWAREHOUSE_END)
	{
		return (TableIdx == eItemTable_MunpaWarehouse);
	}
	else if(TP_STORAGE_START <= absPos && absPos < TP_STORAGE_END)
	{
		return (TableIdx == eItemTable_Storage);
	}
	
	return FALSE;
}

eITEMTABLE CItemManager::GetTableIdxForAbsPos(POSTYPE absPos)
{
	if(TP_INVENTORY_START <= absPos && absPos < (TP_INVENTORY_END + ( TABCELL_INVENTORY_NUM * HERO->Get_HeroExtendedInvenCount() )))
	{
		return eItemTable_Inventory;
	}
	else if(TP_WEAR_START <= absPos && absPos < TP_WEAR_END)
	{
		return eItemTable_Weared;
	}
	else if(TP_GUILDWAREHOUSE_START <= absPos && absPos < TP_GUILDWAREHOUSE_END)
	{
		return eItemTable_MunpaWarehouse;
	}
	else if(TP_STORAGE_START <= absPos && absPos < TP_STORAGE_END)
	{
		return eItemTable_Storage;
	}
	else if(TP_SHOPITEM_START <= absPos && absPos < TP_SHOPITEM_END)
	{
		return eItemTable_Shop;
	}
	
	return eItemTable_Max;
}


void CItemManager::RefreshAllItem()
{
	CItem* pItem;
	m_ItemHash.SetPositionHead();
	
	while((pItem = m_ItemHash.GetData()) != NULL)
	{
		// 080901 LUJ, ȹ�� �� ��Ÿ�� �������� ���μ��� Ÿ�Ӹ��� ��Ÿ���� ó������ �ʴ´�.
		//				���� ���ΰ�ħ ���� ���������� ������, ������ �� �� ó�� ������ �缳���ȴ�
		UpdateCoolTimeItem( *pItem );

		RefreshItem( pItem );
	}
}

void CItemManager::RefreshItem( CItem* pItem )
{
	if(pItem->GetItemInfo()->Category == eItemCategory_Equip)
	{
		// 090114 LYW --- ItemManager : ������ ���� ���� üũ �Լ� ������ ���� ó��.
		if(CanEquip(pItem->GetItemIdx()) == eEquip_Able)
		{
			pItem->SetUseParam( 0 );
//			pItem->SetToolTipImageRGB( TTCLR_ITEM_CANEQUIP );
		}
		else
		{
			pItem->SetUseParam( 1 );
//			pItem->SetToolTipImageRGB( TTCLR_ITEM_CANNOTEQUIP );
		}
	}
	else if(pItem->GetItemInfo()->Category == eItemCategory_Expend)
	{
		if(CanUse(pItem->GetItemIdx()))
		{
			pItem->SetUseParam( 0 );
		}
		else
		{
			pItem->SetUseParam( 1 );
		}
	}

	AddToolTip( pItem );

	if( m_bAddPrice )
	{
		char buf[128];
		DWORDEX SellPrice = pItem->GetItemInfo()->SellPrice;
		pItem->AddToolTipLine( "" );
		wsprintf( buf, CHATMGR->GetChatMsg(35), AddComma(SellPrice) );
		pItem->AddToolTipLine( buf, TTTC_SELLPRICE );
	}	

	// 080820 LUJ, ���� Ÿ�Կ� ���� ó�� �߰�
	switch( pItem->GetItemInfo()->SupplyType )
	{
		// 080820 LUJ, ȹ�� �ÿ� ��Ÿ���� ���� �ִ� ������
	case ITEM_KIND_COOLTIME:
		{
			const ITEMBASE& item			= pItem->GetItemBaseInfo();
			const DWORD		remainedTick	= item.nRemainSecond * 1000;

			// 080820 LUJ, �и������� ������ �����Ѵ�
			if( remainedTick / 100 <= pItem->GetRemainedCoolTime() / 100 )
			{
				break;
			}

			const DWORD maxTick		= pItem->GetItemInfo()->SupplyValue * 1000;
			const DWORD	currentTick	= GetTickCount();

			// 080820 LUJ, ���� �ð�/�ִ� �ð��� ������� ��Ÿ���� ǥ�����ֱ� ����, ������ ���۵� ��ó�� �����Ѵ�
			pItem->SetCoolTimeAbs(
				currentTick - ( maxTick - remainedTick ),
				currentTick + remainedTick,
				maxTick );
			break;
		}
	}
}

void CItemManager::RefreshItemToolTip( DWORD ItemDBIdx )
{
	CItem* pItem = GetItem(ItemDBIdx);

	ASSERT(pItem);

	AddToolTip( pItem );
}

void CItemManager::SetPriceToItem( BOOL bAddPrice )
{
	CItem* pItem;
	m_ItemHash.SetPositionHead();

	if( bAddPrice )
	{
		char buf[128];

		while((pItem = m_ItemHash.GetData()) != NULL)
		{
			DWORDEX SellPrice = pItem->GetItemInfo()->SellPrice;
			pItem->AddToolTipLine( "" );
			wsprintf( buf, CHATMGR->GetChatMsg(35), AddComma(SellPrice) );
			pItem->AddToolTipLine( buf, TTTC_SELLPRICE );			
		}
	}
	else
	{
		while((pItem = m_ItemHash.GetData()) != NULL)
		{
			AddToolTip( pItem );
		}
	}

	m_bAddPrice = bAddPrice;
}

RESULT_CANEQUIP CItemManager::CanEquip(DWORD wItemIdx)
{
	ITEM_INFO * pInfo = GetItemInfo(wItemIdx);

	if( !pInfo ) return eEquip_Disable ;

	if(pInfo->LimitRace)
	{
		// 090708 ONS �ű����� �߰��� ���� ������ ������ ����ó�� ����
		BYTE byRace = HERO->GetRace();
		if( (pInfo->LimitRace == eLimitRace_HumanElf) && (byRace == RaceType_Devil) ||
			(pInfo->LimitRace == eLimitRace_Devil)	  && (byRace != RaceType_Devil)	)
				return eEquip_RaceFail ;
	}


	// [���] %s �������� ������ ���� �ʾ� ������ �� �����ϴ�.
	if(pInfo->LimitGender)
	{
		if(pInfo->LimitGender != HERO->GetGender()+1) return eEquip_GenderFail ;
	}
	

	// [���] %s �������� ������ ���� ������ �� �����ϴ�.
	if(pInfo->LimitLevel > HERO->GetLevel())
	{
		return eEquip_LevelFail;
	}


	// ���ô�� ���õ���� ����.
	WORD wFishingLevel = FISHINGMGR->GetFishingLevel();
	if(pInfo->WeaponType==eWeaponType_FishingPole && wFishingLevel<pInfo->kind)
		return eEquip_FishingLevelFail ;

	// �丮������ ����� ����.
	WORD wCookingLevel = COOKMGR->GetCookLevel();
	if(pInfo->WeaponType==eWeaponType_CookUtil && wCookingLevel<pInfo->Grade)
		return eEquip_CookingLevelFail ;


	// ���� ���� ����� �����Ѵ�.
	return eEquip_Able ;
}

BOOL CItemManager::CanUse(DWORD wItemIdx)
{
	ITEM_INFO * pInfo = GetItemInfo(wItemIdx);


	// �̳��� ���õ���� ����.
	if(pInfo->SupplyType == ITEM_KIND_FISHING_BAIT)
	{
		WORD wFishingLevel = FISHINGMGR->GetFishingLevel();

		if(wFishingLevel < pInfo->kind)
		{
			return FALSE;
		}
	}
	// �丮�����Ǵ� ����, �丮����� ����.
	else if(pInfo->SupplyType == ITEM_KIND_ADDRECIPE)
	{
		WORD wRecipeLevel = (WORD)COOKMGR->GetCookLevelFromRecipe(pInfo->SupplyValue);

		if(pInfo->LimitLevel>HERO->GetLevel() || wRecipeLevel>COOKMGR->GetCookLevel())
		{
			return FALSE;
		}
	}
	// �丮�� ������ ����
	else if(pInfo->SupplyType == ITEM_KIND_FOOD)
	{
		if(pInfo->LimitLevel>HERO->GetLevel())
		{
			return FALSE;
		}
	}
	// �丮��ų���� ���õ��� ������ ����
	else if(pInfo->SupplyType == ITEM_KIND_FOODSKILLBOOK)
	{
		if(pInfo->LimitLevel>HERO->GetLevel())
		{
			return FALSE;
		}

		if(pInfo->SupplyValue-COOKMGR->GetCookLevel() > 1)
		{
			return FALSE;
		}
	}

	return TRUE;
}

ITEM_INFO * CItemManager::GetItemInfo(DWORD wItemIdx)
{
	return m_ItemInfoList.GetData(wItemIdx);
}

LPCTSTR CItemManager::GetName(EWEARED_ITEM slot) const
{
	switch( slot )
	{
	case eWearedItem_Shield:
		{
			return CHATMGR->GetChatMsg( 695 );
		}
	case eWearedItem_Dress:
		{
			return CHATMGR->GetChatMsg( 683 );
		}
	case eWearedItem_Hat:
		{
			return CHATMGR->GetChatMsg( 684 );
		}
	case eWearedItem_Glove:
		{
			return CHATMGR->GetChatMsg( 685 );
		}
	case eWearedItem_Shoes:
		{
			return CHATMGR->GetChatMsg( 686 );
		}
	case eWearedItem_Ring1:
		{
			return CHATMGR->GetChatMsg( 687 );
		}
	case eWearedItem_Necklace:
		{
			return CHATMGR->GetChatMsg( 688 );
		}
	case eWearedItem_Earring1:
		{
			return CHATMGR->GetChatMsg( 689 );
		}
	case eWearedItem_Belt:
		{
			return CHATMGR->GetChatMsg( 690 );
		}
	case eWearedItem_Band:
		{
			return CHATMGR->GetChatMsg( 691 );
		}
	case eWearedItem_Glasses:
		{
			return CHATMGR->GetChatMsg( 832 );
		}
	case eWearedItem_Mask:
		{
			return CHATMGR->GetChatMsg( 833 );
		}
	case eWearedItem_Wing:
		{
			return CHATMGR->GetChatMsg( 2624 );
		}
	case eWearedItem_Costume_Wing:
		{
			return CHATMGR->GetChatMsg( 2625 );
		}
	case eWearedItem_Costume_Head:
		{
			return CHATMGR->GetChatMsg( 835 );
		}
	case eWearedItem_Costume_Dress:
		{
			return CHATMGR->GetChatMsg( 836 );
		}
	case eWearedItem_Costume_Glove:
		{
			return CHATMGR->GetChatMsg( 837 );
		}
	case eWearedItem_Costume_Shoes:
		{
			return CHATMGR->GetChatMsg( 838 );
		}
	}

	return "?";
}

// 080916 LUJ, �ɼ� �̸��� ��ȯ�޴´�
const char* CItemManager::GetName( ITEM_OPTION::Drop::Key key ) const
{
	switch( key )
	{
	case ITEM_OPTION::Drop::KeyPlusStrength:
		{
			return CHATMGR->GetChatMsg( 157 );
		}
	case ITEM_OPTION::Drop::KeyPlusIntelligence:
		{
			return CHATMGR->GetChatMsg( 160 );
		}
	case ITEM_OPTION::Drop::KeyPlusDexterity:
		{
			return CHATMGR->GetChatMsg( 158 );
		}
	case ITEM_OPTION::Drop::KeyPlusWisdom:
		{
			return CHATMGR->GetChatMsg( 161 );
		}
	case ITEM_OPTION::Drop::KeyPlusVitality:
		{
			return CHATMGR->GetChatMsg( 678 );
		}
	case ITEM_OPTION::Drop::KeyPlusPhysicalAttack:
		{
			return CHATMGR->GetChatMsg( 152 );
		}
	case ITEM_OPTION::Drop::KeyPlusPhysicalDefence:
		{
			return CHATMGR->GetChatMsg( 153 );
		}
	case ITEM_OPTION::Drop::KeyPlusMagicalAttack:
		{
			return CHATMGR->GetChatMsg( 155 );
		}
	case ITEM_OPTION::Drop::KeyPlusMagicalDefence:
		{
			return CHATMGR->GetChatMsg( 156 );
		}
	case ITEM_OPTION::Drop::KeyPlusCriticalRate:
		{
			return CHATMGR->GetChatMsg( 842 );
		}
	case ITEM_OPTION::Drop::KeyPlusCriticalDamage:
		{
			return CHATMGR->GetChatMsg( 843 );
		}
	case ITEM_OPTION::Drop::KeyPlusAccuracy:
		{
			return CHATMGR->GetChatMsg( 162 );
		}
	case ITEM_OPTION::Drop::KeyPlusEvade:
		{
			return CHATMGR->GetChatMsg( 841 );
		}
	case ITEM_OPTION::Drop::KeyPlusMoveSpeed:
		{
			return CHATMGR->GetChatMsg( 840 );
		}
	case ITEM_OPTION::Drop::KeyPlusLife:
		{
			return CHATMGR->GetChatMsg( 164 );
		}
	case ITEM_OPTION::Drop::KeyPlusMana:
		{
			return CHATMGR->GetChatMsg( 165 );
		}
	case ITEM_OPTION::Drop::KeyPlusLifeRecovery:
		{
			return CHATMGR->GetChatMsg( 846 );
		}
	case ITEM_OPTION::Drop::KeyPlusManaRecovery:
		{
			return CHATMGR->GetChatMsg( 847 );
		}
	case ITEM_OPTION::Drop::KeyPercentStrength:
		{
			return CHATMGR->GetChatMsg( 157 );
		}
	case ITEM_OPTION::Drop::KeyPercentIntelligence:
		{
			return CHATMGR->GetChatMsg( 160 );
		}
	case ITEM_OPTION::Drop::KeyPercentDexterity:
		{
			return CHATMGR->GetChatMsg( 158 );
		}
	case ITEM_OPTION::Drop::KeyPercentWisdom:
		{
			return CHATMGR->GetChatMsg( 161 );
		}
	case ITEM_OPTION::Drop::KeyPercentVitality:
		{
			return CHATMGR->GetChatMsg( 678 );
		}
	case ITEM_OPTION::Drop::KeyPercentPhysicalAttack:
		{
			return CHATMGR->GetChatMsg( 152 );
		}
	case ITEM_OPTION::Drop::KeyPercentPhysicalDefence:
		{
			return CHATMGR->GetChatMsg( 153 );
		}
	case ITEM_OPTION::Drop::KeyPercentMagicalAttack:
		{
			return CHATMGR->GetChatMsg( 155 );
		}
	case ITEM_OPTION::Drop::KeyPercentMagicalDefence:
		{
			return CHATMGR->GetChatMsg( 156 );
		}
	case ITEM_OPTION::Drop::KeyPercentCriticalRate:
		{
			return CHATMGR->GetChatMsg( 842 );
		}
	case ITEM_OPTION::Drop::KeyPercentCriticalDamage:
		{
			return CHATMGR->GetChatMsg( 843 );
		}
	case ITEM_OPTION::Drop::KeyPercentAccuracy:
		{
			return CHATMGR->GetChatMsg( 162 );
		}
	case ITEM_OPTION::Drop::KeyPercentEvade:
		{
			return CHATMGR->GetChatMsg( 841 );
		}
	case ITEM_OPTION::Drop::KeyPercentMoveSpeed:
		{
			return CHATMGR->GetChatMsg( 840 );
		}
	case ITEM_OPTION::Drop::KeyPercentLife:
		{
			return CHATMGR->GetChatMsg( 164 );
		}
	case ITEM_OPTION::Drop::KeyPercentMana:
		{
			return CHATMGR->GetChatMsg( 165 );
		}
	case ITEM_OPTION::Drop::KeyPercentLifeRecovery:
		{
			return CHATMGR->GetChatMsg( 846 );
		}
	case ITEM_OPTION::Drop::KeyPercentManaRecovery:
		{
			return CHATMGR->GetChatMsg( 847 );
		}
	}

	return "?";
}



void CItemManager::LoadItemToolTipList()
{
	CMHFile file;
    char filename[256];
    sprintf(filename, "Data/Interface/Windows/%s", GAMERESRCMNGR->GetTranslationPath("ToolTipMsg.bin"));
	if( !file.Init( filename, "rb" ) )
		return;

	char buf[512];	//������ ��������...22		//256���� �����ϰ�..
	DWORD dwIndex;
	while( 1 )
	{
		if( file.IsEOF() ) break;

		SafeStrCpy( buf, file.GetString(), sizeof(buf) );

		if( strcmp( buf, "#Msg" ) == 0 )
		{
			dwIndex = file.GetDword();	//msg index
			SafeStrCpy( buf, file.GetString(), sizeof(buf) );
			if( strcmp( buf, "{" ) == 0 )
			{
				// ToolTipMsg �� ��� ���� �� ���� ��ȯ�Ѵ�.
				std::string strToolTipMsg;
				file.GetLine( buf, sizeof(buf) );
				strToolTipMsg.append( buf );

				file.GetLine( buf, sizeof(buf) );
				while( *buf != '}' )
				{
					strToolTipMsg += "\n";
					strToolTipMsg.append( buf );
					file.GetLine( buf, sizeof(buf) );
				}

				// 100312 ShinJS --- ���� �޼��� ����� ��ȯ
 				SKILLMGR->ConvertToolTipMsgKeyWordFormText( dwIndex, strToolTipMsg );

				m_ToolTipTextMap.insert( std::make_pair( dwIndex, strToolTipMsg ) );
			}
		}		
	}	
}

#define TOOLTIPINFO_EXTENT		144
const std::vector< std::string >& CItemManager::GetToolTipMsg( DWORD dwIndex )
{
	static std::vector< std::string > emptyTooltipList;

	// ������ �߸� ToolTip �˻�
	std::map< DWORD, std::vector< std::string > >::const_iterator cutToolTipIter = m_CutToolTipText.find( dwIndex );

	// �������� �ʴ� ��� ����
	if( cutToolTipIter == m_CutToolTipText.end() )
	{
		const std::string& totalToolTip = GetTotalToolTipMsg( dwIndex );
		if( totalToolTip.empty() )
			return emptyTooltipList;

		// �̸���½��� ���̷� �ڸ���.
		int nToolTipWidth = TOOLTIPINFO_EXTENT;
		cSkillInfo* pSkillInfo = SKILLMGR->GetSkillInfo( dwIndex );
		ITEM_INFO* pItemInfo = GetItemInfo( dwIndex );
		if( pSkillInfo )
		{
			char buf[512]={0,};
			sprintf( buf, CHATMGR->GetChatMsg( 2094 ), pSkillInfo->GetName(), pSkillInfo->GetLevel() );
			int nNameWidth = CFONT_OBJ->GetTextExtentEx( 0, buf, strlen( buf ) );
			nToolTipWidth = ( nNameWidth > TOOLTIPINFO_EXTENT ? nNameWidth : TOOLTIPINFO_EXTENT );
		}
		else if( pItemInfo )
		{
			int nNameWidth = CFONT_OBJ->GetTextExtentEx( 0, pItemInfo->ItemName, strlen( pItemInfo->ItemName ) );
			nToolTipWidth = ( nNameWidth > TOOLTIPINFO_EXTENT ? nNameWidth : TOOLTIPINFO_EXTENT );
		}

		std::vector< std::string >& cutToolTipList = m_CutToolTipText[ dwIndex ];
		CutToolTipMsg( totalToolTip, cutToolTipList, nToolTipWidth );

		return cutToolTipList;
	}

	return cutToolTipIter->second;
}

const std::string& CItemManager::GetTotalToolTipMsg( DWORD dwIndex )
{
	static std::string emptyTooltip;

	std::map< DWORD, std::string >::const_iterator tooltipMapIter = m_ToolTipTextMap.find( dwIndex );
	if( tooltipMapIter == m_ToolTipTextMap.end() )
        return emptyTooltip;

	return tooltipMapIter->second;
}

void CItemManager::CutToolTipMsg( const std::string totalToolTip, std::vector< std::string >& cutToolTipList, const int nCutWidth )
{
	char* str = (char*)totalToolTip.c_str();
	int nLen = totalToolTip.length();
	std::string cutToolTip;

#ifdef _TL_LOCAL_
	BYTE brk[512];
	int nBrkNum = g_TBreak.FindThaiWordBreak( str, nLen, brk, 512, FTWB_SEPARATESYMBOL );
	int nCutPos = 0;

	for( int i = 0 ; i < nBrkNum ; ++i )
	{
		nCutPos += brk[i];
		if( CFONT_OBJ->GetTextExtentEx(0, str, nCutPos ) > nCutWidth )
		{
			nCutPos -= brk[i];
			cutToolTip.clear();
			cutToolTip.append( str, nCutPos );

			--i;
			str += nCutPos;
			nCutPos = 0;

			if( *str == ' ' )
			{
				++str;	//���� ù���ڰ� �����̽��� �ƴϵ���...
				++i;
			}

			cutToolTipList.push_back( cutToolTip );
		}
	}

#else

	int nCutPos = 0;
	int nLenPos = 0;
	while( nLenPos < nLen )
	{
		if( IsDBCSLeadByte( *(str + nCutPos) ) )
		{
			++nCutPos;
			++nLenPos;
		}

		++nCutPos;
		++nLenPos;

		if( CFONT_OBJ->GetTextExtentEx(0, str, nCutPos ) > nCutWidth )
		{
			cutToolTip.clear();
			cutToolTip.append( str, nCutPos );

			str += nCutPos;
			nCutPos = 0;

			if( *str == ' ' )
			{
				++str;	//���� ù���ڰ� �����̽��� �ƴϵ���...
				++nLenPos;
			}

			cutToolTipList.push_back( cutToolTip );
		}
	}
#endif

	if( nCutPos > 0 )
	{
		cutToolTip.clear();
		cutToolTip.append( str );

		cutToolTipList.push_back( cutToolTip );
	}
}

void CItemManager::LoadItemList()
{

	{
		CMHFile file;

		if( ! file.Init("System/Resource/ItemList.bin", "rb" ) )
		{
			return;
		}

		while( ! file.IsEOF() )
		{
			const DWORD dwItemIndex = file.GetDword();
			if( dwItemIndex == 0 )
			{
				char buf[MAX_PATH]={0,};
				file.GetLine( buf, sizeof(buf) );
				continue;
			}

			ITEM_INFO* pInfo = new ITEM_INFO;
			if( !pInfo )
				continue;

			pInfo->ItemIdx = dwItemIndex;

#ifdef _GMTOOL_
			if (m_ItemInfoList.GetData(pInfo->ItemIdx))
			{
				char szText[256];
				sprintf(szText, "�̹� �����ϴ� ������ID: %d", pInfo->ItemIdx);
				char	szCaption[512];														// �Ʒ� ���� ��¦ �߰�. -_-; 2007.05.28 ������.
				sprintf( szCaption, "%s, %s() %d", __FILE__, __FUNCTION__, __LINE__); 
				MessageBox(NULL, szText, szCaption, NULL);
			}
#endif
			// E ASSERT���� �߰� added by hseos 2007.05.22
			 
			SafeStrCpy(pInfo->ItemName,GAMERESRCMNGR->GetTranslation(CGameResourceManager::eLanguageBin::ITEMLIST, dwItemIndex, file.GetString()), MAX_ITEMNAME_LENGTH+1 );

			pInfo->ItemTooltipIdx = file.GetDword();
			pInfo->Image2DNum = file.GetWord();	
			pInfo->Part3DType = file.GetWord();
			pInfo->Part3DModelNum = file.GetWord();	

			pInfo->Shop = file.GetWord(); // �����۸� ����
			pInfo->Category = eItemCategory(file.GetWord());
			pInfo->Grade = file.GetWord();
			pInfo->LimitRace = LIMIT_RACE(file.GetWord());
			pInfo->LimitGender = file.GetWord();		
			pInfo->LimitLevel = (WORD)file.GetLevel();	

			pInfo->Stack = file.GetWord();
			pInfo->Trade = file.GetWord();
			pInfo->Deposit = file.GetWord();
			pInfo->Sell = file.GetWord();
			pInfo->Decompose = file.GetWord();

			pInfo->DecomposeIdx = file.GetDword();
			pInfo->Time = file.GetDword();

			pInfo->BuyPrice = file.GetDword();			
			pInfo->SellPrice = file.GetDword();	

#ifdef _GMTOOL_
			if( pInfo->BuyPrice < pInfo->SellPrice )
			{
				char szText[256];
				sprintf(szText, "�Ǹűݾ��� ���űݾ׺��� ���� ������ID: %d", pInfo->ItemIdx);
				char	szCaption[512];														// �Ʒ� ���� ��¦ �߰�. -_-; 2007.05.28 ������.
				sprintf( szCaption, "%s, %s() %d", __FILE__, __FUNCTION__, __LINE__); 
				MessageBox(NULL, szText, szCaption, NULL);
			}
#endif
			pInfo->EquipType = eEquipType(file.GetWord());
			pInfo->EquipSlot = EWEARED_ITEM(file.GetWord() - 1);
			pInfo->Durability = file.GetDword();
			file.GetDword();

			pInfo->Repair = file.GetWord();
			pInfo->WeaponAnimation = eWeaponAnimationType(file.GetWord());
			pInfo->WeaponType = eWeaponType(file.GetWord());
			pInfo->PhysicAttack = file.GetInt();
			pInfo->MagicAttack = file.GetInt();
			pInfo->ArmorType = eArmorType(file.GetWord());
			pInfo->AccessaryType = eAccessaryType(file.GetWord());
			pInfo->PhysicDefense = file.GetInt();
			pInfo->MagicDefense = file.GetInt();

			pInfo->IsEnchant = file.GetBool();
			pInfo->Improvement = file.GetWord();

			pInfo->EnchantValue		= file.GetDword();
			pInfo->EnchantDeterm	= file.GetDword();

			pInfo->ImprovementStr = file.GetInt();			
			pInfo->ImprovementDex = file.GetInt();			
			pInfo->ImprovementVit = file.GetInt();		//10
			pInfo->ImprovementInt = file.GetInt();
			pInfo->ImprovementWis = file.GetInt();			
			pInfo->ImprovementLife = file.GetInt();			
			pInfo->ImprovementMana = file.GetInt();			

			pInfo->ImprovementSkill = file.GetDword();
			pInfo->Skill		= file.GetDword();
			pInfo->SupplyType	= ITEM_KIND(file.GetWord());
			pInfo->SupplyValue	= file.GetDword();
			pInfo->kind			= ITEM_INFO::eKind( file.GetDword() );

			pInfo->RequiredBuff = file.GetDword();
			pInfo->Battle = file.GetBool();
			pInfo->Peace = file.GetBool();
			pInfo->Move = file.GetBool();
			pInfo->Stop = file.GetBool();
			pInfo->Rest = file.GetBool();
			pInfo->wSeal = ITEM_SEAL_TYPE(file.GetWord());
			pInfo->nTimeKind = ITEM_TIMEKIND(file.GetInt());
			pInfo->dwUseTime = file.GetDword();
			pInfo->dwBuyFishPoint = file.GetDword();
			pInfo->wPointType = eItemPointType(file.GetWord());
			pInfo->dwPointTypeValue1 = file.GetDword();
			pInfo->dwPointTypeValue2 = file.GetDword();
			pInfo->dwType =  file.GetDword(); // Item ����
			pInfo->dwTypeDetail =  file.GetDword();	// Item ��������

			switch(pInfo->SupplyType)
			{
			case ITEM_KIND_FARM_PLANT:
				{
					CSHFarmManageGardenDlg::SetCropImgInfo( ITEM_KIND( pInfo->SupplyValue ), pInfo->Image2DNum );
					break;
				}
			case ITEM_KIND_ANIMAL:
				{
					GAMEIN->GetAnimalDialog()->SetAnimalImgInfo( ITEM_KIND( pInfo->SupplyValue ), pInfo->Image2DNum );
					break;
				}
			}

			m_ItemInfoList.Add(pInfo, pInfo->ItemIdx);
		}
	}
}

BOOL CItemManager::IsDupItem( DWORD wItemIdx )
{
	return 0 < GetItemStackNum(
		wItemIdx);
}

CItem* CItemManager::GetItemofTable(eITEMTABLE TableIDX, POSTYPE absPos)
{
	CItem * pItem = NULL;
	switch(TableIDX)
	{
	case eItemTable_Weared:
	case eItemTable_Inventory:
		{
			CInventoryExDialog * pDlg = GAMEIN->GetInventoryDialog();
			pItem = pDlg->GetItemForPos( absPos );			
		}
		break;
	case eItemTable_Storage:
		{
			CStorageDialog * pDlg = GAMEIN->GetStorageDialog();
			pItem = pDlg->GetItemForPos( absPos );			
		}
		break;
	case eItemTable_MunpaWarehouse:
		{
			CGuildWarehouseDialog * pDlg = GAMEIN->GetGuildWarehouseDlg();
			pItem = pDlg->GetItemForPos( absPos );			
		}
		break;
	case eItemTable_Shop:
		{
			CItemShopDialog* pDlg = GAMEIN->GetItemShopDialog();
			pItem = pDlg->GetItemForPos( absPos );
		}
		break;
	}
	return pItem;
}

// yunho StatsCalcManager ������ ����
const ITEMBASE* CItemManager::GetItemInfoAbsIn( CPlayer*, POSTYPE absPos)
{
	const eITEMTABLE TableIdx = GetTableIdxForAbsPos(absPos);

	if(eItemTable_Max == TableIdx)
	{
		return NULL;
	}

	CItem* pItem = GetItemofTable(
		TableIdx,
		absPos);
	
	return pItem ? &( pItem->GetItemBaseInfo() ) : 0;
}

void CItemManager::SetDisableDialog(BOOL val, BYTE TableIdx)
{
	switch(TableIdx)
	{
	case eItemTable_Weared:
	case eItemTable_Inventory:
		{
			CInventoryExDialog * pDlg = GAMEIN->GetInventoryDialog();
			if( pDlg )
				pDlg->SetDisable( val );
		}
		break;
	case eItemTable_Storage:
		{
			CStorageDialog * pDlg = GAMEIN->GetStorageDialog();
			if( pDlg )
				pDlg->SetDisable( val );			
		}
		break;
	case eItemTable_MunpaWarehouse:
		{
			CGuildWarehouseDialog * pDlg = GAMEIN->GetGuildWarehouseDlg();
			if( pDlg )
				pDlg->SetDisable( val );
		}
		break;
	case eItemTable_Deal:
		{
			CDealDialog * pDlg = GAMEIN->GetDealDialog();
			if( pDlg )
				pDlg->SetDisable( val );
		}
		break;
	case eItemTable_Shop:
		{
			CItemShopDialog* pDlg = GAMEIN->GetItemShopDialog(); 
			if( pDlg )
				pDlg->SetDisable( val );
		}
		break;
	default:
		{
			ASSERTMSG(0, "���̺� �ε����� �������.DisableDlg()-Taiyo. Invalid Table Index.");
			break;
		}
	}
}

void CItemManager::DivideItem(ITEMOBTAINARRAY * pMsg )
{
	ITEMOBTAINARRAY * pmsg = (ITEMOBTAINARRAY *)pMsg;
	
	CItem* pItem = NULL;
	CItem* pPreItem = NULL;
	for(WORD i = 0 ; i < pmsg->ItemNum ; ++i)
	{
		DeleteItem(pmsg->GetItem(i)->Position, &pPreItem);

		pItem = MakeNewItem(pmsg->GetItem(i),"DivideItem");	
	//	BOOL rt = GAMEIN->GetInventoryDialog()->AddItem(pItem);
		if(!AddItem(pItem))
		{
			ASSERT(0);
		}
	}
}

BOOL CItemManager::AddItem(CItem* pItem)
{
	ASSERT( pItem );

	switch( GetTableIdxForAbsPos( pItem->GetPosition() ) )
	{
	case eItemTable_Inventory:
	case eItemTable_Weared:
	case eItemTable_Storage:
		return GAMEIN->GetStorageDialog()->AddItem(pItem);
	case eItemTable_MunpaWarehouse:
		return GAMEIN->GetGuildWarehouseDlg()->AddItem(pItem);
	}
	
	return FALSE;
}

ITEM_INFO* CItemManager::FindItemInfoForName( char* strName )
{
	m_ItemInfoList.SetPositionHead();
	ITEM_INFO* pInfo = NULL;
	while( (pInfo = m_ItemInfoList.GetData()) != NULL)
	{
		if( strcmp( strName, pInfo->ItemName ) == 0 )
		{
			return pInfo;
		}
	}

	return NULL;
}

void CItemManager::ReLinkQuickPosition(CItem* pItem)
{
	if(IsDupItem(pItem->GetItemIdx()))
	{
		QUICKMGR->RefreshQickItem();
	}
}

void CItemManager::SetPreItemData(sPRELOAD_INFO* pPreLoadInfo, int* Level, int Count)
{
	ITEM_INFO* pItemInfo = NULL;
	int ItemIndex = 0;

	m_ItemInfoList.SetPositionHead();
	while( (pItemInfo = m_ItemInfoList.GetData()) != NULL)
	{
		for(int i=0; i<Count; i++)
		{
			if(pItemInfo->LimitLevel == Level[i])
			{
				// �������� ������ �⺻���⿡ +�� ���� �������̴�. �׷��� �н�~
				if( pItemInfo->ItemIdx%10 )			continue;
				
				ItemIndex = pPreLoadInfo->Count[ePreLoad_Item];
				pPreLoadInfo->Kind[ePreLoad_Item][ItemIndex] = pItemInfo->ItemIdx;
				++pPreLoadInfo->Count[ePreLoad_Item];
				
				// Max�� �Ѿ�� �׸���~~
				if(pPreLoadInfo->Count[ePreLoad_Item] >= MAX_KIND_PERMAP)		return;
			}
		}
	}
}

void CItemManager::ItemDropEffect( DWORD wItemIdx )
{
	const ITEM_INFO* const itemInfo = GetItemInfo(
		wItemIdx);

	if(0 == itemInfo)
	{
		return;
	}

	switch(itemInfo->kind)
	{
	case ITEM_INFO::eKindRare:
		{
			EFFECTMGR->StartHeroEffectProcess(eEffect_GetItem_Rare);
		}
		break;
	case ITEM_INFO::eKindUnique:
		{
			EFFECTMGR->StartHeroEffectProcess(eEffect_GetItem_Unique);
		}
		break;
	}

	if(itemInfo->Category == eItemCategory_Equip )
	{
		switch(GetItemInfo(wItemIdx)->EquipType)
		{
		case eEquipType_Card:
		case eEquipType_Weapon:
			{
				EFFECTMGR->StartHeroEffectProcess(eEffect_GetItem_Weapon);
			}
			break;
		case eEquipType_Armor:
			{
				EFFECTMGR->StartHeroEffectProcess(eEffect_GetItem_Dress);
			}
			break;
		}
	}
	else
	{
		EFFECTMGR->StartHeroEffectProcess(eEffect_GetItem_Youngyak);
	}
}

void CItemManager::Process()
{
	ProcessCoolTime();

	// 100609 ONS 5�ʰ������� üũ�Ͽ� ť�� �����Ͱ� �����ϸ� ������ ����.
	if(false == m_EnchantNotifyQueue.empty())
	{
		if(m_ENchantNotifyCheckTime > gCurTime)
		{
			return;
		}

		const EnchantNotifyInfo Info = m_EnchantNotifyQueue.front();
		m_EnchantNotifyQueue.pop();

		const ITEM_INFO* pInfo = GetItemInfo( Info.ItemIndex );
		if( NULL == pInfo )
		{
			return;
		}

        char buf[128] = {0};
		sprintf( buf, Info.IsSuccessed ? CHATMGR->GetChatMsg(2285) : CHATMGR->GetChatMsg(2321), 
						Info.Name, 
						pInfo->ItemName, 
						Info.EnchantLevel );
		GAMEIN->GetItemPopupDlg()->SetText( buf );
		GAMEIN->GetItemPopupDlg()->SetActive( TRUE );

		m_ENchantNotifyCheckTime = gCurTime + 5000;
	}
}

void CItemManager::FakeDeleteItem(CItem* pItem)
{
	m_TempDeleteItem.Durability = pItem->GetDurability();
	m_TempDeleteItem.Position = pItem->GetPosition();
	m_TempDeleteItem.wIconIdx = pItem->GetItemIdx();
}

void CItemManager::SendDeleteItem()
{
	if( m_TempDeleteItem.wIconIdx == 0 ) return;

	CItem* pItem = GetItemofTable(GetTableIdxForAbsPos(m_TempDeleteItem.Position), m_TempDeleteItem.Position);
	if((pItem == 0) || (pItem->GetItemIdx() != m_TempDeleteItem.wIconIdx))
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(153) );
		GAMEIN->GetInventoryDialog()->SetDisable(FALSE);
		return;
	}

	// 071127 LYW --- ItemManager : ���� ������ ( ���� ���� ) ��ġ ���� ���� �߰�.
	m_nSealItemDelPos = m_TempDeleteItem.Position ;
			
	MSG_ITEM_DISCARD_SYN msg;
	msg.Category = MP_ITEM;
	msg.Protocol = MP_ITEM_DISCARD_SYN;
	msg.dwObjectID = gHeroID;
	msg.TargetPos =m_TempDeleteItem.Position;
	msg.wItemIdx = m_TempDeleteItem.wIconIdx;
	msg.ItemNum = m_TempDeleteItem.Durability;

	NETWORK->Send(&msg,sizeof(msg));
	memset(&m_TempDeleteItem, 0, sizeof(ITEMBASE));
}

void CItemManager::Item_TotalInfo_Local( void* pMsg )
{
	if( !pMsg ) return ;

	ITEM_TOTALINFO * msg = (ITEM_TOTALINFO *)pMsg;

	for(int i = 0 ; i < (TP_INVENTORY_END + ( TABCELL_INVENTORY_NUM * HERO->Get_HeroExtendedInvenCount() )) ; ++i)
	{
		GAMEIN->GetInventoryDialog()->AddItem(&msg->Inventory[i]);
	}

	for(DWORD i = 0 ; i < eWearedItem_Max ; ++i)
	{
		GAMEIN->GetInventoryDialog()->AddItem(&msg->WearedItem[i]);
	}

	QUICKMGR->RefreshQickItem();
}


void CItemManager::Item_Storage_Item_Info_Ack( void* pMsg )
{
	SEND_STORAGE_ITEM_INFO * pmsg = (SEND_STORAGE_ITEM_INFO*)pMsg;

	CAddableInfoIterator iter(&pmsg->AddableInfo);
	BYTE AddInfoKind;
	while((AddInfoKind = iter.GetInfoKind()) != CAddableInfoList::None)
	{
		switch(AddInfoKind)
		{
		case(CAddableInfoList::ItemOption):
			{
				ITEM_OPTION OptionInfo[SLOT_STORAGE_NUM];
				iter.GetInfoData(&OptionInfo);
				
				for( WORD i = 0; i < pmsg->wOptionCount; ++i )
				{
					ITEM_OPTION& option = OptionInfo[ i ];

					AddOption( option );
				}
			}
			break;
		}
		iter.ShiftToNextData();
	}

	for(int i=0; i<SLOT_STORAGE_NUM; ++i)
	{
		if(pmsg->StorageItem[i].dwDBIdx != 0)
			GAMEIN->GetStorageDialog()->AddItem(&pmsg->StorageItem[i]);
	}
	GAMEIN->GetStorageDialog()->SetStorageMoney(pmsg->money);		
	
	GAMEIN->GetStorageDialog()->SetItemInit(TRUE);
	GAMEIN->GetStorageDialog()->ShowStorageDlg(TRUE) ;

	if(HERO->GetStorageNum())
	{
		GAMEIN->GetStorageDialog()->ShowStorageMode(eStorageMode_StorageWare1);
		GAMEIN->GetStorageDialog()->AddStorageMode(eStorageMode_StorageWare1);
		GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);							// ��ũ��Ʈ ���̾�α׸� ��Ȱ��ȭ �Ѵ�.
	}
}

void CItemManager::Item_Reinforce_Option_Ack( void* pMsg )
{
	const MSG_ITEM_REINFORCE_OPTION_ACK* message = ( MSG_ITEM_REINFORCE_OPTION_ACK* )pMsg;
	
	AddOption( message->mOption );

	{
		CReinforceDlg* dialog = GAMEIN->GetReinforceDialog();
		ASSERT( dialog );

		dialog->Succeed( *message );
	}

	QUICKMGR->RefreshQickItem();
}


void CItemManager::Item_ReInForce_Ack( void* pMsg )
{
	const MSG_ITEM_REINFORCE_ACK*	message		= ( MSG_ITEM_REINFORCE_ACK* )pMsg;
	
	if( MP_ITEM_REINFORCE_FAILED_ACK == message->Protocol )
	{
		CReinforceDlg* dialog = GAMEIN->GetReinforceDialog();
		ASSERT( dialog );

		dialog->Fail();
	}
	// 071226 LUJ, ��ȭ �ɼ��� ���� �������� �޾Ƴ���...
	else
	{
		CItem* item = GetItem( message->mSourceItemDbIndex );

		if( item )
		{
			// 080108 LUJ, ���� ������ �ö����� �ٽ� ��ȭ�� �� ���� �Ѵ�
			item->SetLock( TRUE );
		}
	}

	CItem*				deletedItem		= 0;
	CInventoryExDialog*	inventoryDialog = GAMEIN->GetInventoryDialog();
	ASSERT( inventoryDialog );

	// ������ ������Ʈ
	for( DWORD i = 0; i < message->mSize; ++i )
	{
		const ITEMBASE& updatedItem = message->mItem[ i ];

		CItem* item	= GetItem( updatedItem.dwDBIdx );

		if( item )
		{
			if( updatedItem.wIconIdx )
			{
				cImage image;

				item->SetBasicImage( GetIconImage( updatedItem.wIconIdx, &image ) );
				item->SetItemBaseInfo( updatedItem );
				item->SetVisibleDurability( IsDupItem( updatedItem.wIconIdx ) );
				item->SetData( updatedItem.wIconIdx );

				AddToolTip( item );
			}
			else
			{
				DeleteItem( item->GetPosition(), &deletedItem );

				//inventoryDialog->DeleteItem( updatedItem.Position, &deletedItem );
			}

			item->SetLock( FALSE );
		}
		// 071218 LUJ,	Ŭ���̾�Ʈ�� ���� �������̸� �ӽÿ� ������ ����, DB �ε����� �������� �ö����� ��ų��´�
		//				DB������ ���� �������� ������ٰ� �����ϰų�, �ش� ������ ����ϴ� ���� ���� �����̴�
		else
		{
			inventoryDialog->AddItem(
				MakeNewItem(&updatedItem, ""));
		}
	}
}


void CItemManager::Item_ReInForce_Nack( void* pMsg )
{
	{
		CReinforceDlg* dialog = GAMEIN->GetReinforceDialog();
		ASSERT( dialog );

		dialog->Restore();
	}

	const MSG_ITEM_ERROR* message = ( MSG_ITEM_ERROR* )pMsg;

	switch( message->ECode )
	{
	case eLog_ItemReinforceDataHaveTooManyMaterials:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "Report to LUNA homepage, please: Data is too big" );
			break;
		}
	case eLog_ItemReinforceMaterialIsInvalid:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "Report to LUNA homepage, please: Material data is invalid" );
			break;
		}
	case eLog_ItemReinforceWhichMaterialHasZero:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "Report to LUNA homepage, please: Some material has zero" );
			break;
		}
	case eLog_ItemReinfoceYouCannotOver100ToReinforce:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "Report to LUNA homepage, please: You cannot over 100 materials to reinforce" );
			break;
		}
	case eLog_ItemReinforceSourceItemNotExist:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "Report to LUNA homepage, please: Source item not exist" );
			break;
		}
	case eLog_ItemRerinfoceServerHasNoReinforceScript:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "Report to LUNA homepage, please: Server has no reinforce script" );
			break;
		}
	case eLog_ItemReinforceMaterialSourceItemCannnotReinforce:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "Report to LUNA homepage, please: Source item cannot reinforce" );
			break;
		}
	case eLog_ItemReinforceSourceItemCanDuplicate:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "Report to LUNA homepage, please: Source item can duplicate" );
			break;
		}
	case eLog_ItemReinforceRemovingMaterialIsFailed:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "Report to LUNA homepage, please: Removing material is failed" );
			break;
		}
	case eLog_ItemReinforceUpdatingItemIsFailed:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "Report to LUNA homepage, please: Update item is failed" );
			break;
		}
		// 080929 LUJ, ���� �������� �߸���
	case eLog_ItemReinforceInvalidSupportItem:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "Support item is invalid" );
			break;
		}
		// 080929 LUJ, ���� ��ũ��Ʈ�� ����
	case eLog_ItemReinforceNoSupportScript:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "There is no support script" );
			break;
		}
		// 080929 LUJ, ���� �����ۿ� ������ ��ᰡ �ƴ�
	case eLog_ItemReinforceInvalidMaterialForSupportItem:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "Some Material is invalid for support item" );
			break;
		}
	default:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "Report to LUNA homepage, please: Undefined result detected" );
			break;
		}
	}
}


void CItemManager::Item_Move_Ack( void* pMsg )
{
	SetDisableDialog(FALSE, eItemTable_Inventory);
	SetDisableDialog(FALSE, eItemTable_Storage);
	SetDisableDialog(FALSE, eItemTable_MunpaWarehouse);
	MoveItem( (MSG_ITEM_MOVE_ACK *)pMsg );			
	
	WINDOWMGR->DragWindowNull();
}

void CItemManager::Item_Pet_Move_Ack( void* pMsg )
{
	SetDisableDialog(FALSE, eItemTable_Inventory);
	SetDisableDialog(FALSE, eItemTable_Storage);
	PetMoveItem( (MSG_ITEM_MOVE_ACK *)pMsg );			
	
	WINDOWMGR->DragWindowNull();
}

void CItemManager::Item_Combine_Ack( void* pMsg )
{
	CombineItem( (MSG_ITEM_COMBINE_ACK *)pMsg );	

	SetDisableDialog(FALSE, eItemTable_Inventory);
	SetDisableDialog(FALSE, eItemTable_Storage);
	SetDisableDialog(FALSE, eItemTable_MunpaWarehouse);
	
	WINDOWMGR->BackDragWindow();
}


void CItemManager::Item_Divide_Ack( void* pMsg )
{
	CInventoryExDialog * pInven = GAMEIN->GetInventoryDialog();
	SetDisableDialog(FALSE, eItemTable_Inventory); 
	SetDisableDialog(FALSE, eItemTable_Storage);
	SetDisableDialog(FALSE, eItemTable_MunpaWarehouse);

	ITEMOBTAINARRAY * pmsg = (ITEMOBTAINARRAY *)pMsg;

	for(WORD i = 0 ; i < pmsg->ItemNum ; ++i)
	{
		ITEMBASE* const dividedItem = pmsg->GetItem(i);

		if(const CItem* const originalItem = GetItem(dividedItem->ItemParam))
		{
			dividedItem->nRemainSecond = originalItem->GetItemBaseInfo().nRemainSecond;
		}

		pInven->AddItem(
			MakeNewItem(dividedItem, "MP_ITEM_DIVIDE_ACK"));
	}
			
	//---KES ItemDivide Fix 071020
	GAMEIN->GetInventoryDialog()->UpdateItemDurabilityAdd( GetDivideItemInfoPos(), -GetDivideItemInfoDur() );
	//----------------------------

	// QuickDur�ٽ� ���
	QUICKMGR->RefreshQickItem();

	WINDOWMGR->DragWindowNull();
}


void CItemManager::Item_Monster_Obtain_Notify( void* pMsg )
{
	ITEMOBTAINARRAY* const pmsg	= (ITEMOBTAINARRAY*)pMsg;
	CItem* pItem = NULL;
	
	for(int i = 0 ; i < pmsg->ItemNum ; ++i)
	{
		ITEMBASE& itemBase = pmsg->ItemInfo[ i ];
		const ITEM_INFO* const pInfo = GetItemInfo(
			itemBase.wIconIdx);

		if(NULL == pInfo)
		{
			continue;
		}

		pItem = GAMEIN->GetInventoryDialog()->GetItemForPos( itemBase.Position );
		
		ITEMBASE prevItemBase;
		ZeroMemory( &prevItemBase, sizeof( prevItemBase ) );

		if( pItem )
		{
			if( pItem->GetDBIdx() == itemBase.dwDBIdx )
			{
				prevItemBase = pItem->GetItemBaseInfo();
			}
			
			pItem->SetItemBaseInfo( itemBase ) ;
		}
		else
		{
			pItem = MakeNewItem( &itemBase, "MP_ITEM_MONSTER_OBTAIN_NOTIFY" ) ;

			// �������� ���۵� ������ ������ ���� �������� ���� ��찡 �ִ�. ITEMOBTAINARRAY�� �ʱ�ȭ�Ǿ����� ���� ����� �����ϰ� ����
			if(0 == pItem)
			{
				continue;
			}
			else if( pInfo->wSeal == eITEM_TYPE_SEAL )
			{
				itemBase.nSealed = eITEM_TYPE_SEAL ;
				itemBase.nRemainSecond = pInfo->dwUseTime ;

				pItem->SetItemBaseInfo( itemBase ) ;
			}

			GAMEIN->GetInventoryDialog()->AddItem(pItem);
		}

		// 071220 LUJ, ������ ������ ������ ���� ����
		if( IsDupItem( itemBase.wIconIdx ) )
		{
			const DURTYPE quantity = abs( int( prevItemBase.Durability ) - int( itemBase.Durability ) );

			// 080526 LUJ, ȹ�� ������ 0�� ��� ǥ������ �ʵ��� ��
			if( quantity )
			{
				CHATMGR->AddMsg(
					CTC_GETITEM,
					CHATMGR->GetChatMsg(1112),
					pInfo->ItemName,
					quantity);
			}
		}
		else
		{
			CHATMGR->AddMsg(
				CTC_GETITEM,
				CHATMGR->GetChatMsg(101),
				pInfo->ItemName);
		}
	}

	if( pItem )
	{
		ItemDropEffect( pItem->GetItemIdx() );
	}

	//������â�� ������ �����.
	cDialog* pDlg = WINDOWMGR->GetWindowForID( DBOX_DIVIDE_INV );
	if( pDlg )
	{
		((cDivideBox*)pDlg)->ExcuteDBFunc( 0 );	//��ҹ�ư ������
	}

	QUICKMGR->RefreshQickItem();

	//071022 LYW --- ItemManager : Check item tutorial.
	TUTORIALMGR->Check_GetItem(pItem) ;
}

void CItemManager::Item_HouseRestored( void* pMsg )
{
	//�Ͽ�¡ ���������� �������� �����Ǿ� �´� �׿����� ó�� 
	ITEMOBTAINARRAY *	pmsg	= (ITEMOBTAINARRAY *)pMsg;
	CItem*				pItem	= NULL;
		
	for(int i = 0 ; i < pmsg->ItemNum ; ++i)
	{
		ITEMBASE& itemBase = pmsg->ItemInfo[ i ];
		
		ITEMBASE prevItemBase;
		ZeroMemory( &prevItemBase, sizeof( prevItemBase ) );

		pItem = MakeNewItem( &itemBase, "MP_ITEM_HOUSE_RESTORED" ) ;

		// 071127 LYW --- ItemManager : ���� ������ ó��.
		const ITEM_INFO* pInfo = GetItemInfo( pItem->GetItemIdx() ) ;

		if( !pInfo ) continue;

		if( !GAMEIN->GetInventoryDialog()->AddItem(pItem) )
		{
			ASSERT(0) ;
		}

		//090527 pdy �Ͽ�¡ �ý��۸޼��� [��������]
		CHATMGR->AddMsg( CTC_GETITEM, CHATMGR->GetChatMsg( 1864 ), pInfo->ItemName );	//1864 "%s�� �κ��丮�� �̵� �Ͽ����ϴ�."
	}

	if( pItem )
	{
		ItemDropEffect( pItem->GetItemIdx() );
	}

	QUICKMGR->RefreshQickItem();
}

//090618 pdy �Ͽ�¡ �ٹ̱� ���ʽ� �����ۺ��� �߰� 
void CItemManager::Item_HouseBonus( void* pMsg ) 
{
	//�Ͽ�¡ �ٹ̱� ���ʽ� ���� �����ۺ��� Ÿ���ϰ�� �������� ����´�. 
	ITEMOBTAINARRAY *	pmsg	= (ITEMOBTAINARRAY *)pMsg;
	CItem*				pItem	= NULL;
		
	for(int i = 0 ; i < pmsg->ItemNum ; ++i)
	{
		ITEMBASE& itemBase = pmsg->ItemInfo[ i ];
		
		ITEMBASE prevItemBase;
		ZeroMemory( &prevItemBase, sizeof( prevItemBase ) );

		pItem = MakeNewItem( &itemBase, "MP_ITEM_HOUSE_BONUS" ) ;

		// 071127 LYW --- ItemManager : ���� ������ ó��.
		const ITEM_INFO* pInfo = GetItemInfo( pItem->GetItemIdx() ) ;

		if( !pInfo ) continue;

		if( !GAMEIN->GetInventoryDialog()->AddItem(pItem) )
		{
			ASSERT(0) ;
		}

		CHATMGR->AddMsg( CTC_GETITEM, CHATMGR->GetChatMsg( 101 ), pInfo->ItemName );	
	}

	if( pItem )
	{
		ItemDropEffect( pItem->GetItemIdx() );
	}

	QUICKMGR->RefreshQickItem();
}

void CItemManager::Item_Move_Nack( void* pMsg )
{
	MSG_WORD* pmsg = (MSG_WORD*)pMsg;
	
	switch(pmsg->wData)
	{
	case 7:
		break;
	default:
		ASSERT(pmsg->wData == 3);	
	}
	SetDisableDialog(FALSE, eItemTable_Inventory);
	SetDisableDialog(FALSE, eItemTable_Storage);
	SetDisableDialog(FALSE, eItemTable_MunpaWarehouse);
	
	WINDOWMGR->BackDragWindow();
}


void CItemManager::Item_Divite_Nack( void* pMsg )
{
	SetDisableDialog(FALSE, eItemTable_Inventory);
	SetDisableDialog(FALSE, eItemTable_Storage);
	SetDisableDialog(FALSE, eItemTable_MunpaWarehouse);
	WINDOWMGR->BackDragWindow();
}


void CItemManager::Item_Combine_Nack( void* pMsg )
{
	SetDisableDialog(FALSE, eItemTable_Inventory);
	SetDisableDialog(FALSE, eItemTable_Storage);
	SetDisableDialog(FALSE, eItemTable_MunpaWarehouse);
	
	WINDOWMGR->BackDragWindow();
}


void CItemManager::Item_Discard_Nack( void* pMsg )
{
	SetDisableDialog(FALSE, eItemTable_Inventory);
	SetDisableDialog(FALSE, eItemTable_Storage);
	SetDisableDialog(FALSE, eItemTable_MunpaWarehouse);

	GAMEIN->GetInventoryDialog()->SetDisable( FALSE );
	MSG_ITEM_ERROR * pmsg = (MSG_ITEM_ERROR*)pMsg;
	if(pmsg->ECode != 4)
		WINDOWMGR->BackDragWindow();
	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(799));
}


void CItemManager::Item_Discard_Ack( void* pMsg )
{
	MSG_ITEM_DISCARD_SYN* const pmsg = (MSG_ITEM_DISCARD_SYN*)pMsg;
	const ITEMBASE* pItemBase = GetItemInfoAbsIn(
		HERO,
		pmsg->TargetPos);

	if(0 == pItemBase)
	{
		return;
	}

	QUICKMGR->DeleteLinkdedQuickItem(
		pItemBase->dwDBIdx);
	DeleteItemofTable(
		GetTableIdxForAbsPos(pmsg->TargetPos),
		pmsg->TargetPos,
		FALSE);
	SetDisableDialog(
		FALSE,
		eItemTable_Inventory);
	SetDisableDialog(
		FALSE,
		eItemTable_Storage);
	SetDisableDialog(
		FALSE,
		eItemTable_MunpaWarehouse);
	QUICKMGR->RefreshQickItem();
}


void CItemManager::Item_Use_Notify( void* pMsg )
{
	MSG_DWORDBYTE* pmsg = (MSG_DWORDBYTE*)pMsg;
	CObject* pObject = OBJECTMGR->GetObject( pmsg->dwData );
	if( pObject == NULL ) return;
	if( pObject->GetObjectKind() != eObjectKind_Player ) return;

	CPlayer* pPlayer = (CPlayer*)pObject;

	switch( pmsg->bData )
	{
	case 1:	//������ ȸ�� ����Ʈ
		{
			OBJECTEFFECTDESC desc((WORD)FindEffectNum("ItemUSE_PotionHP_little.beff"));
			pPlayer->AddObjectEffect( LIFE_RECOVER_EFFECTID, &desc, 1 );
		}
		break;
	case 2:	//���� ȸ�� ����Ʈ
		{
			OBJECTEFFECTDESC desc((WORD)FindEffectNum("ItemUSE_PotionMP_little.beff"));
			pPlayer->AddObjectEffect( MANA_RECOVER_EFFECTID, &desc, 1 );
		}
		break;
	case 3:	//�� ȸ�� ����Ʈ
		{
			//---KES ���߿� ����Ʈ �ϳ��� ���� �װɷ� �ѷ�����!
			OBJECTEFFECTDESC desc1((WORD)FindEffectNum("ItemUSE_PotionHP_little.beff"));
			pPlayer->AddObjectEffect( LIFE_RECOVER_EFFECTID, &desc1, 1 );
			OBJECTEFFECTDESC desc2((WORD)FindEffectNum("ItemUSE_PotionMP_little.beff"));
			pPlayer->AddObjectEffect( MANA_RECOVER_EFFECTID, &desc2, 1 );
//			OBJECTEFFECTDESC desc(FindEffectNum("ItemUSE_PotionHP_little.beff"));
//			pPlayer->AddObjectEffect( ALL_RECOVER_EFFECTID, &desc, 1 );
		}
		break;
	}
}


void CItemManager::Item_Use_Ack( void* pMsg )
{
	MSG_ITEM_USE_ACK* const message = (MSG_ITEM_USE_ACK*)pMsg;
	const POSTYPE itemPosition = message->TargetPos;
	const eItemUse_Err itemResult = message->eResult;
	
	CItem* const item = GAMEIN->GetInventoryDialog()->GetItemForPos(
		itemPosition);
	
	if( !item )
	{
		return;
	}

	if( item->IsLocked() ) 
	{
		// 090811 ShinJS --- ����Dialog�� ��ϵ� �̳� �������� ��� Lock�� �������� �ʴ´�
		CFishingDialog* pFishingDlg = GAMEIN->GetFishingDlg();
		ASSERT( pFishingDlg )
		if( pFishingDlg->GetBaitItemIdx() != item->GetItemIdx() )
			item->SetLock( FALSE ) ;
	}

	if(eItemUseSuccess == itemResult)
	{
		if(AddCoolTime(gHeroID, item->GetItemIdx()))
		{
			CoolTimeScript::ItemData::const_iterator item_it = mCoolTimeScript.mItemData.find( item->GetItemIdx() );

			if( mCoolTimeScript.mItemData.end() == item_it )
			{
				return;
			}

			const CoolTimeScript::Unit& unit = item_it->second;
			const DWORD	itemIndex = item->GetItemIdx();

			CoolTimeScript::GroupData::const_iterator group_it = mCoolTimeScript.mGroupData.find( GetCoolTimeGroupIndex( itemIndex ) );

			if( mCoolTimeScript.mGroupData.end() == group_it )
			{
				return;
			}

			GAMEIN->GetInventoryDialog()->SetCoolTime(
				itemIndex,
				unit.mTime);
			GAMEIN->GetQuickDlg()->GetSlotDlg(0)->SetCoolTime(
				itemIndex,
				unit.mTime);
			GAMEIN->GetQuickDlg()->GetSlotDlg(1)->SetCoolTime(
				itemIndex,
				unit.mTime);
		}
	}

	const ITEM_INFO* info = item->GetItemInfo();

	if(0 == info)
	{
		return;
	}
	else if( eItemCategory_Expend == info->Category )
	{
		//090527 pdy �Ͽ�¡ �������� ������ ���޼����� ����.
		if( info->SupplyType != ITEM_KIND_FURNITURE )
			CHATMGR->AddMsg( CTC_EXPENDEDITEM, CHATMGR->GetChatMsg( 1111 ), info->ItemName );
	}
	
	switch(GetTableIdxForAbsPos(itemPosition))
	{
	case eItemTable_Inventory:
	case eItemTable_Weared:
		{
			BOOL bDelItem = FALSE;

			switch(info->Category)
			{
			// LUJ, ��Ḧ ��ų �ߵ� �� �Ҹ��Ѵ�. �̷��� �ؾ� �Һ� ó���� �ȴ�
			case eItemCategory_Material:
			case eItemCategory_Expend:
				{
					if( info->SupplyType == ITEM_KIND_PET )
					{
						break;
					}
					else if(eItemUseUnsealed == itemResult)
					{
						break;
					}
					else if(info->Stack)
					{
						item->SetDurability(
							item->GetDurability() - 1);

						if(0 == item->GetDurability())
						{
							CItem* pOutItem = 0;
							DeleteItem(
								itemPosition,
								&pOutItem);

							if( pOutItem )
							{
								bDelItem = TRUE;
								ReLinkQuickPosition( pOutItem );
							}
						}
					}
					else
					{
						const ITEMBASE& pItemBase = item->GetItemBaseInfo() ;
						
						if( pItemBase.nSealed == eITEM_TYPE_UNSEAL )
						{
							QUICKMGR->RefreshQickItem();
							return ;
						}

						CItem* pOutItem = 0;
						DeleteItem(
							itemPosition,
							&pOutItem);

						if( pOutItem )
						{
							bDelItem = TRUE;
							ReLinkQuickPosition( pOutItem );
						}
					}

					switch(info->SupplyType)
					{
					case ITEM_KIND_HP_RECOVER:
					case ITEM_KIND_HP_RECOVER_INSTANT:
						{
							EFFECTMGR->StartHeroEffectProcess(
								FindEffectNum("ItemUSE_PotionHP_little.beff"));
							break;
						}
					case ITEM_KIND_MP_RECOVER:
					case ITEM_KIND_MP_RECOVER_INSTANT:
						{
							EFFECTMGR->StartHeroEffectProcess(
								FindEffectNum("ItemUSE_PotionMP_little.beff"));
							break;
						}
					case ITEM_KIND_HPMP_RECOVER:
					case ITEM_KIND_HPMP_RECOVER_INSTANT:
						{
							EFFECTMGR->StartHeroEffectProcess(
								FindEffectNum("ItemUSE_PotionHP_little.beff"));
							EFFECTMGR->StartHeroEffectProcess(
								FindEffectNum("ItemUSE_PotionMP_little.beff"));
							break;
						}
					case ITEM_KIND_FISHING_BAIT:
						{
							if(bDelItem)
							{
								if(CItem* pItem = GAMEIN->GetInventoryDialog()->GetItemForIdx(item->GetItemIdx()))
									GAMEIN->GetFishingDlg()->UpdateBaitItem();
								else
									GAMEIN->GetFishingDlg()->SetBaitItem(NULL);
							}
							else
							{
								GAMEIN->GetFishingDlg()->UpdateBaitItem();
							}

							break;
						}
					case ITEM_KIND_SUMMON_EFFECT:
						{
							EFFECTMGR->StartHeroEffectProcess(
								info->SupplyValue);
							break;
						}
					}
				}
				break;
			default:
				{	
					if( info->SupplyType != ITEM_KIND_COOLTIME )
					{	
						break;
					}	
	
					CItem* deletedItem = 0;
					DeleteItem( item->GetPosition(), &deletedItem );
				}	
				break;
			}	
		}
		break;
	}
	QUICKMGR->RefreshQickItem();
}

void CItemManager::Item_Cook_Ack( void* pMsg )
{
	const MSG_COOK_ACK* message		= ( MSG_COOK_ACK *)pMsg;
	
	CItem*				deletedItem		= 0;
	CInventoryExDialog*	inventoryDialog = GAMEIN->GetInventoryDialog();

	for(size_t i = 0; i < message->mUpdateResult.mSize; ++i)
	{
		const ITEMBASE& updatedItem = message->mUpdateResult.mItem[i];
		CItem* item	= GetItem( updatedItem.dwDBIdx );
		
		if( item )
		{
			const BOOL isDuplicated = IsDupItem( updatedItem.wIconIdx );

			if( updatedItem.wIconIdx )
			{
				cImage image;

				item->SetBasicImage( GetIconImage( updatedItem.wIconIdx, &image ) );
				item->SetItemBaseInfo( updatedItem );
				item->SetVisibleDurability( isDuplicated );
				item->SetData( updatedItem.wIconIdx );

				RefreshItem( item );
			}
			else
			{
				DeleteItem( item->GetPosition(), &deletedItem );
			}
		}
		// 071218 LUJ,	Ŭ���̾�Ʈ�� ���� �������̸� �ӽÿ� ������ ����, DB �ε����� �������� �ö����� ��ų��´�
		//				DB������ ���� �������� ������ٰ� �����ϰų�, �ش� ������ ����ϴ� ���� ���� �����̴�
		else
		{
			inventoryDialog->AddItem(
				MakeNewItem(&updatedItem, ""));
		}
	}
}

void CItemManager::Item_Cook_GetItem( void* p )
{
	ITEMOBTAINARRAY*	message = ( ITEMOBTAINARRAY* )p;

	// �κ��丮 ����
	{
		CInventoryExDialog* dialog	= GAMEIN->GetInventoryDialog();
		ASSERT( dialog );

		for( WORD i = 0; i < message->ItemNum ; ++i )
		{
			const ITEMBASE* itemBase = message->GetItem( i );

			{
				CItem* item = dialog->GetItemForPos( itemBase->Position );

				if( item )
				{
					CItem*	deletedItem	= 0;
					DeleteItem( item->GetPosition(), &deletedItem );
				}
			}

			{
				CItem* item = MakeNewItem( itemBase, "" );

				if( ! dialog->AddItem( item ) )
				{
					ASSERT( 0 );
				}

				item->SetLock( FALSE );
			}
		}	
	}
}

void CItemManager::Item_Mix_Ack( void* pMsg ) 
{
	const MSG_ITEM_MIX_ACK* message		= ( MSG_ITEM_MIX_ACK *)pMsg;
	const BOOL				isFailed	= ( message->Protocol != MP_ITEM_MIX_SUCCEED_ACK );
	CMixDialog*				mixDialog	= GAMEIN->GetMixDialog();

	ASSERT( mixDialog );

	// ��� �˸�
	if( isFailed )
	{
		mixDialog->Fail( *message );

		QUICKMGR->RefreshQickItem();
	}
	else
	{
		CItem* item	= GetItem( message->mSourceItemDbIndex );

		if( ! item )
		{
			ASSERT( 0 );
			return;
		}

		const ItemMixResult*	mixResult	= GetMixResult( item->GetItemIdx(), message->mResultIndex );
		const ITEM_INFO*		info		= GetItemInfo( mixResult->mItemIndex );
		ASSERT( info );

		if( info )
		{
			CHATMGR->AddMsg(
				CTC_OPERATEITEM,
				CHATMGR->GetChatMsg( 814 ),
				info->ItemName,
				message->mMixSize );

			QUICKMGR->RefreshQickItem();
		}

		mixDialog->Succeed( *message );
	}
	
	CItem* deletedItem = 0;
	
	for(DWORD i = 0; i < message->mUpdateResult.mSize; ++i)
	{
		const ITEMBASE& updatedItem = message->mUpdateResult.mItem[i];
		CItem* item	= GetItem( updatedItem.dwDBIdx );
		
		if( item )
		{
			const BOOL isDuplicated = IsDupItem( updatedItem.wIconIdx );

			if( updatedItem.wIconIdx )
			{
				cImage image;

				item->SetBasicImage( GetIconImage( updatedItem.wIconIdx, &image ) );
				item->SetItemBaseInfo( updatedItem );
				item->SetVisibleDurability( isDuplicated );
				item->SetData( updatedItem.wIconIdx );

				RefreshItem( item );

				// 100422 ShinJS --- �κ��丮 ������ ����� ����Ʈ ������ �����Ѵ�.
				GAMEIN->GetQuestQuickViewDialog()->ResetQuickViewDialog();
				GAMEIN->GetQuestDialog()->RefreshQuestList();
				QUESTMGR->ProcessNpcMark();
			}
			else
			{
				DeleteItem( item->GetPosition(), &deletedItem );
			}
		}
		// 071218 LUJ,	Ŭ���̾�Ʈ�� ���� �������̸� �ӽÿ� ������ ����, DB �ε����� �������� �ö����� ��ų��´�
		//				DB������ ���� �������� ������ٰ� �����ϰų�, �ش� ������ ����ϴ� ���� ���� �����̴�
		else
		{
			GAMEIN->GetInventoryDialog()->AddItem(
				MakeNewItem(&updatedItem, ""));
		}
	}

	// 090818 ShinJS --- â�� �����ִ� ��� ������ �����Ѵ�
	if( mixDialog->IsActive() )
	{
		mixDialog->UpdateCategory( eMixDlgCategoryType_4 );
	}
}


void CItemManager::Item_Mix_GetItem( void* p )
{
	ITEMOBTAINARRAY*	message = ( ITEMOBTAINARRAY* )p;

	// �κ��丮 ����
	{
		CInventoryExDialog* dialog	= GAMEIN->GetInventoryDialog();
		ASSERT( dialog );

		for( WORD i = 0; i < message->ItemNum ; ++i )
		{
			const ITEMBASE* itemBase = message->GetItem( i );

			{
				CItem* item = dialog->GetItemForPos( itemBase->Position );

				if( item )
				{
					CItem*	deletedItem	= 0;

					//dialog->DeleteItem( itemBase->Position, 0 );
					DeleteItem( item->GetPosition(), &deletedItem );
				}
			}

			{
				CItem* item = MakeNewItem( itemBase, "" );

				if( ! dialog->AddItem( item ) )
				{
					ASSERT( 0 );
				}

				item->SetLock( FALSE );
			}
		}	
	}

	// 080227 LUJ, ����â ����. ����� �� ������ ó���� �� �ֵ��� ����Ǿ� ������ ��������
	{
		CMixDialog* dialog = ( CMixDialog* )WINDOWMGR->GetWindowForID( ITMD_MIXDLG );

		if( dialog )
		{
			const ITEMBASE* itemBase = message->GetItem( 0 );

			if( itemBase )
			{
				dialog->RefreshResult( *itemBase );
			}
		}
	}
}

void CItemManager::Item_Mix_Nack( void* pMsg )
{
	const MSG_DWORD*	message = ( MSG_DWORD* )	pMsg;		

	switch( eLogitemmoney( message->dwData ) )
	{
	case eLog_ItemMixInsufficentSpace:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 910 ) );
			break;
		}
	case eLog_ItemMixSystemFault:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1275 ) );
			break;
		}
	case eLog_ItemMixNotFound:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 784 ) );
			break;
		}
	case eLog_itemMixBadRequest:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1276 ) );
			break;
		}
	case eLog_ItemMixBadCondition:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1277 ) );
			break;
		}
	case eLog_ItemMixBadQuantity:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1278 ) );
			break;
		}
	default:
		{
			ASSERT( 0 );
			CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1275 ) );
			break;
		}
	}
}


void CItemManager::Item_Working_Success( void* pMsg ) 
{
	const MSG_DWORD*	message		= ( MSG_DWORD* )pMsg;
	const DWORD			playerIndex = message->dwData;
	CObject*			object		= OBJECTMGR->GetObject( playerIndex );

	// 090911 ShinJS --- �̵����� ��� ����Ʈ, ��� ������� �ʵ��� ����
	if( object && !MOVEMGR->IsMoving( object ) && !HERO->IsGetOnVehicle() )
	{
		OBJECTEFFECTDESC desc( 28 );

		object->RemoveObjectEffect( ITEM_WORKING_EFFECT );
		object->RemoveObjectEffect( ITEM_WORKING_SUCCESS_EFFECT );
		object->AddObjectEffect( ITEM_WORKING_SUCCESS_EFFECT, &desc, 1 );
	}
}

void CItemManager::Item_Working_Fail( void* pMsg )
{
	const MSG_DWORD*	message		= ( MSG_DWORD* )pMsg;
	const DWORD			playerIndex = message->dwData;
	CObject*			object		= OBJECTMGR->GetObject( playerIndex );

	// 090911 ShinJS --- �̵����� ��� ����Ʈ, ��� ������� �ʵ��� ����
	if( object && !MOVEMGR->IsMoving( object ) && !HERO->IsGetOnVehicle() )
	{
		OBJECTEFFECTDESC desc( 29 );

		object->RemoveObjectEffect( ITEM_WORKING_EFFECT );
		object->RemoveObjectEffect( ITEM_WORKING_FAIL_EFFECT );
		object->AddObjectEffect( ITEM_WORKING_FAIL_EFFECT, &desc, 1 );
	}
}

void CItemManager::Item_Working_Start( void* pMsg )
{
	const MSG_DWORD*	message		= ( MSG_DWORD* )pMsg;
	const DWORD			playerIndex = message->dwData;
	CObject*			object		= OBJECTMGR->GetObject( playerIndex );

	// 090911 ShinJS --- �̵����� ��� ����Ʈ, ��� ������� �ʵ��� ����
	if( object && !MOVEMGR->IsMoving( object ) && !HERO->IsGetOnVehicle() )
	{
		OBJECTEFFECTDESC desc( 30 );

		object->RemoveObjectEffect( ITEM_WORKING_EFFECT );
		object->AddObjectEffect( ITEM_WORKING_EFFECT, &desc, 1 );
	}
}

void CItemManager::Item_Working_StartEx( void* pMsg )
{
	const MSG_DWORD2*	message		= ( MSG_DWORD2* )pMsg;
	const DWORD			playerIndex = message->dwData1;
	CObject*			object		= OBJECTMGR->GetObject( playerIndex );

	// 090911 ShinJS --- �̵����� ��� ����Ʈ, ��� ������� �ʵ��� ����
	if( object && !MOVEMGR->IsMoving( object ) && !HERO->IsGetOnVehicle() )
	{
		OBJECTEFFECTDESC desc;

		switch(message->dwData2)
		{
		case CProgressDialog::eActionExSummonEffect:
				desc.SetDesc(/*FindEffectNum("E_W_stable_01.beff")*/ 35);
				object->RemoveObjectEffect( ITEM_SUMMON_EFFECTACTIONID );
				object->AddObjectEffect( ITEM_SUMMON_EFFECTACTIONID, &desc, 1 );
			break;

		case CProgressDialog::eActionExCooking:
				desc.SetDesc(/*FindEffectNum("E_M_cook_01.beff")*/ 37);
				object->RemoveObjectEffect( COOKING_EFFECT );
				object->AddObjectEffect( COOKING_EFFECT, &desc, 1 );

				OBJECTSTATEMGR->StartObjectState(object, eObjectState_Cooking);
			break;

		// 091106 ShinJS --- Ż�� ��ȯ/������ �ٸ� ����Ʈ ����
		case CProgressDialog::eActionVehicleSummon:
		case CProgressDialog::eActionVehicleSeal:
				desc.SetDesc( 9454 );
				object->RemoveObjectEffect( ITEM_WORKING_EFFECT );
				object->AddObjectEffect( ITEM_WORKING_EFFECT, &desc, 1 );
			break;
		default:
				desc.SetDesc(30);
				object->RemoveObjectEffect( ITEM_WORKING_EFFECT );
				object->AddObjectEffect( ITEM_WORKING_EFFECT, &desc, 1 );
			break;
		}
	}
}

void CItemManager::Item_Working_Stop( void* pMsg )
{
	const MSG_DWORD*	message		= ( MSG_DWORD* )pMsg;
	const DWORD			playerIndex = message->dwData;
	CObject*			object		= OBJECTMGR->GetObject( playerIndex );

	if( object )
	{
		object->RemoveObjectEffect( ITEM_WORKING_EFFECT );
	}
}

void CItemManager::Item_TimeLimit_Item_OneMinute( void* pMsg )
{
	if( !pMsg ) return ;
	
	MSG_DWORD2* pmsg = NULL ;
	pmsg = (MSG_DWORD2*)pMsg ;

	if( !pmsg ) return ;

	ITEM_INFO* pItemInfo = NULL ;
	pItemInfo = GetItemInfo( pmsg->dwData1 ) ;

	if( !pItemInfo ) return ;

	POSTYPE pos = (POSTYPE)pmsg->dwData2 ;

	const ITEMBASE* pItemBase = GetItemInfoAbsIn( HERO, pos ) ;

	if( pItemBase )
	{
		CItem* pItem = NULL ;
		pItem = GetItem( pItemBase->dwDBIdx ) ;

		if( pItem )
		{
			pItem->SetItemBaseInfo( *pItemBase ) ;
			pItem->SetLastCheckTime(gCurTime) ;
			pItem->SetRemainTime( 50 ) ;
		}
	}

	const eITEMTABLE wTableIdx = GetTableIdxForAbsPos( pos ) ;

	if( wTableIdx == eItemTable_Inventory )
	{
		CHATMGR->AddMsg( CTC_CHEAT_1, CHATMGR->GetChatMsg( 1191 ), CHATMGR->GetChatMsg( 1189 ), pItemInfo->ItemName ) ;
	}
	else if( wTableIdx == eItemTable_Storage )
	{
		CHATMGR->AddMsg( CTC_CHEAT_1, CHATMGR->GetChatMsg( 1191 ), CHATMGR->GetChatMsg( 1188 ), pItemInfo->ItemName ) ;
	}
	else if( wTableIdx == eItemTable_Weared )
	{
		CHATMGR->AddMsg( CTC_CHEAT_1, CHATMGR->GetChatMsg( 1191 ), CHATMGR->GetChatMsg( 1190 ), pItemInfo->ItemName ) ;
	}
}

void CItemManager::Item_ShopItem_Use_Ack( void* pMsg )
{
	MSG_ITEM* pmsg = (MSG_ITEM*)pMsg;
	CItem* const pItem = GAMEIN->GetInventoryDialog()->GetItemForPos(
		pmsg->ItemInfo.Position);

	if(0 == pItem)
	{
		return;
	}

	pItem->SetItemBaseInfo(
		pmsg->ItemInfo);
	pItem->SetLastCheckTime(
		gCurTime);

	AddToolTip(
		pItem);
	QUICKMGR->UnsealQuickItem(
		pmsg->ItemInfo.dwDBIdx);
	QUICKMGR->RefreshQickItem();

	CHATMGR->AddMsg(
		CTC_CHEAT_1,
		CHATMGR->GetChatMsg(366));
	CHATMGR->AddMsg(
		CTC_CHEAT_1, 
		CHATMGR->GetChatMsg(1176),
		pItem->GetItemInfo()->ItemName);
}

void CItemManager::Item_Enchant_Success_Ack( void* pMsg )
{
	const MSG_ITEM_ENCHANT_ACK* message = ( MSG_ITEM_ENCHANT_ACK* )pMsg;

	{
		CEnchantDialog* dialog = ( CEnchantDialog* )WINDOWMGR->GetWindowForID( ITD_UPGRADEDLG	);
		ASSERT( dialog );

		dialog->Succeed( *message );
	}

	QUICKMGR->RefreshQickItem();
}


void CItemManager::Item_Enchant_Nack( void* pMsg )
{
	const MSG_DWORD* message = ( MSG_DWORD* )pMsg;

	switch( eLogitemmoney( message->dwData ) )
	{
	case eLog_ItemEnchantItemInvalid:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, "Report to LUNA homepage, please: Enchanting item is invalid" );
			break;
		}
	case eLog_ItemEnchantServerError:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1275 ) );
			break;
		}
	case eLog_ItemEnchantHacking:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, "Report to LUNA homepage, please: your apply to server was abnormal" );
			break;
		}
	case eLog_ItemEnchantItemUpdateFailed:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, "Report to LUNA homepage, please: After enchanting update data is failed" );
			break;
		}
	// 090713 ShinJS --- ��æƮ ���Һ� �߰��� ���� ������ ���� ����
	case eLog_ItemEnchantNotEnoughMoney:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 324 ) );
			break;
		}
	default:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1275 ) );
			break;
		}
	}

	{
		CEnchantDialog* dialog = ( CEnchantDialog* )WINDOWMGR->GetWindowForID( ITD_UPGRADEDLG	);
		ASSERT( dialog );

		dialog->Restore();
	}
}


void CItemManager::Item_Enchant_Failed_Ack( void* pMsg )
{
	CEnchantDialog* dialog = ( CEnchantDialog* )WINDOWMGR->GetWindowForID( ITD_UPGRADEDLG	);
	ASSERT( dialog );

	dialog->Fail();
}


void CItemManager::Item_Use_Nack( void* pMsg )
{
	SetDisableDialog(FALSE, eItemTable_Inventory);
	SetDisableDialog(FALSE, eItemTable_Storage);
	SetDisableDialog(FALSE, eItemTable_MunpaWarehouse);

	const MSG_ITEM_ERROR* const pmsg = (MSG_ITEM_ERROR*)pMsg;

	switch(pmsg->ECode)
	{
	case eItemUseErr_PreInsert :
		{
			CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1279));
		}
		break ;

	case eItemUseErr_FullInven : 
		{
			// 091214 ONS Full�κ����� ���� �޼�����ȣ ����
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1975) ) ;
		}
		break ;

		// 080904 LYW --- ItemManager : ������ ����� �� ���ٴ� ���� ó��.
   	case eItemUseErr_SiegeWarfareForbiddenItem :
   		{
   			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(91)) ;
   		}
   		break ;
	case eItemUseErr_ForbiddenSummon:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1737)) ;
		}
		break;
	case eItemUseErr_InvalidCookGrade:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1794)) ;
		}
		break;
	case eItemUseErr_LowerLevelForEat:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1795)) ;
		}
		break;
	case eItemUseErr_AlreadyLearnRecipe:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1796)) ;
		}
		break;
	case eItemUseErr_FullLearnRecipe:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1797)) ;
		}
		break;
	// 100218 ONS �ֹε�� �ʱ�ȭ ������ ��� ���� ���� �߰�
	case eItemUseErr_NotRegist:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1859)) ;
		}
		break;
	// 100607 �÷��̾� ���� �����϶� ������ ���� �����߰�
	case eltemUseErr_Stun:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(2264) );
		}
		break;
	case eItemUseErr_PlayerIsGod:
		{
			CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1350));
		}
		break;
	case eItemUseErr_SkillCanNotStart:
		{
			CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1351));
		}
		break;
	default :
		{
			CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(772));
#ifdef _GMTOOL_
			CHATMGR->AddMsg(CTC_SYSMSG, "Item use error. rt:%d", pmsg->ECode);
#endif
		}
		break ;
	}
}


void CItemManager::Item_Sell_Ack( void* pMsg )
{
	const MSG_ITEM_SELL_ACK* const pmsg = (MSG_ITEM_SELL_ACK*)pMsg;
	CItem* pItem = NULL;
	
	switch(GetTableIdxForAbsPos( pmsg->TargetPos ))
	{
	case eItemTable_Inventory :
		{
			// �κ��丮 ������ �޴´�.
			CInventoryExDialog* pInvenDlg = GAMEIN->GetInventoryDialog() ;

			// �κ��丮 ��ȿ�� üũ.
			if( !pInvenDlg )
			{
				#ifdef _GMTOOL_
				MessageBox( NULL, "Invalid inventory dialog!!", __FUNCTION__, MB_OK ) ;
				#endif //_GMTOOL_
				return ;
			}

			// ������ ������ �޴´�.
			pItem = pInvenDlg->GetItemForPos( pmsg->TargetPos ) ;
		}
		break ;

	case eItemTable_Storage :
		{
			// â�� ������ �޴´�.
			CStorageDialog* pStorageDlg = GAMEIN->GetStorageDialog() ;

			// â�� ��ȿ�� üũ.
			if( !pStorageDlg )
			{
				#ifdef _GMTOOL_
				MessageBox( NULL, "Invalid storage dialog!!", __FUNCTION__, MB_OK ) ;
				#endif //_GMTOOL_
				return ;
			}

			// ������ ������ �޴´�.
			pItem = pStorageDlg->GetItemForPos( pmsg->TargetPos ) ;
		}
		break ;

	default : return ;
	}


	// �������� ��ȿ���� Ȯ���Ѵ�.
	if( !pItem )
	{
		#ifdef _GMTOOL_
		MessageBox( NULL, "Invalid item position!!", __FUNCTION__, MB_OK ) ;
		#endif //_GMTOOL_
		return ;
	}


	// ������ ������ �޴´�.
	const ITEM_INFO* pSellItemInfo = pItem->GetItemInfo() ;

	if( !pSellItemInfo )
	{
		#ifdef _GMTOOL_
		MessageBox( NULL, "Invalid item info!!", __FUNCTION__, MB_OK ) ;
		#endif //_GMTOOL_
		return ;
	}


	// ���� ����������, ���� ���������� Ȯ���Ѵ�.
	if( IsDupItem( pmsg->wSellItemIdx ) && ( pItem->GetDurability() - pmsg->SellItemNum ) > 0 )
	{
		// 1. ������ ���� ������Ʈ�Ѵ�.
		pItem->SetDurability( pItem->GetDurability() - pmsg->SellItemNum ) ;
		DWORDEX SellPrice = pSellItemInfo->SellPrice;

		CHATMGR->AddMsg( CTC_SELLITEM, CHATMGR->GetChatMsg( 39 ), pSellItemInfo->ItemName, pmsg->SellItemNum, AddComma( SellPrice*pmsg->SellItemNum ) ) ;

		// 4. �������� ���ΰ�ħ�Ѵ�.
		QUICKMGR->RefreshQickItem() ;
	}
	else
	{
		// 1. �Ǹ� ������ �޴´�.
		DWORDEX SellPrice = pSellItemInfo->SellPrice;

		// 2. �Ǹ� �޽����� ����Ѵ�.
		if( pmsg->SellItemNum == 0 || ! IsDupItem( pItem->GetItemIdx() ) )
		{
			CHATMGR->AddMsg( CTC_SELLITEM, CHATMGR->GetChatMsg( 38 ), pSellItemInfo->ItemName, AddComma( SellPrice ) );
		}
		else
		{
			CHATMGR->AddMsg( CTC_SELLITEM, CHATMGR->GetChatMsg( 39 ), pSellItemInfo->ItemName, pmsg->SellItemNum, AddComma( SellPrice*pmsg->SellItemNum ) );
		}

		// 3. ������ ���� ó���� �Ѵ�.
		// 071202 LYW --- ItemManager : ���� �������� ���� �� ��, ���� ������ ���� ������ �� ������ 
		// ���� �� �Լ��� ����ϱ� ������, �Լ� ���� ���ڷ� �ȶ����� �������� ���θ� üũ�ϵ��� ������.
		DeleteItemofTable( GetTableIdxForAbsPos( pmsg->TargetPos ), pmsg->TargetPos, TRUE ) ;
		QUICKMGR->DeleteLinkdedQuickItem( pItem->GetDBIdx() ) ;
	}


	// ���� Disable ���״� ���̾�α׵��� �����Ѵ�.
	
	// 1. ���� ���̾�α׸� üũ.
	CDealDialog* pDealDlg = GAMEIN->GetDealDialog() ;

	if( !pDealDlg )
	{
		#ifdef _GMTOOL_
		MessageBox( NULL, "Invalid deal dialog!!", __FUNCTION__, MB_OK ) ;
		#endif //_GMTOOL_
		return ;
	}

	if( pDealDlg->IsActive() ) pDealDlg->SetDisable( FALSE ) ;

	
	// 2. �κ��丮 ���̾�α� üũ.
	CInventoryExDialog* pInvenDlg = GAMEIN->GetInventoryDialog() ;

	if( !pInvenDlg )
	{
		#ifdef _GMTOOL_
		MessageBox( NULL, "Invalid inventory dialog!!", __FUNCTION__, MB_OK ) ;
		#endif //_GMTOOL_
		return ;
	}

	if( pInvenDlg->IsActive() ) pInvenDlg->SetDisable( FALSE ) ;

	// 3. â�� ���̾�α� üũ.
	CStorageDialog* pStorageDlg = GAMEIN->GetStorageDialog() ;

	// â�� ��ȿ�� üũ.
	if( !pStorageDlg )
	{
		#ifdef _GMTOOL_
		MessageBox( NULL, "Invalid storage dialog!!", __FUNCTION__, MB_OK ) ;
		#endif //_GMTOOL_
		return ;
	}

	if( pStorageDlg->IsActive() ) pStorageDlg->SetDisable( FALSE ) ;


	// �� ������ ���ΰ�ħ�Ѵ�.
	QUICKMGR->RefreshQickItem() ;
}


void CItemManager::Item_Sell_Nack( void* pMsg )
{
	// 090107 LYW --- ItemManager : ������ �Ǹ� ���� ó�� �޽��� �߰�.


	// �Լ� ���� Ȯ��.
	if( !pMsg ) return ;
    

	// ���� �޽��� ��ȯ.
	MSG_ITEM_ERROR* pmsg = (MSG_ITEM_ERROR*)pMsg ;
	if( !pmsg ) return ;


	// ���� �޽��� Ȯ��
	switch( pmsg->ECode )
	{
	case ERR_NOTSELLITEM :
	case ERR_INVALSLOT :
	case ERR_INVALITEMINFO :
	case ERR_INVALPLAYER :
	case ERR_LOOTPLAYER :
	case ERR_INVALNPC :
	case ERR_INVALIDENTITY :
	case ERR_OVERSTACKCOUNT :
	case ERR_FAILDISCARD :
		break ;

	case ERR_MAXMONEY :
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(459) ) ;
		break ;

	case ERR_NOTINVENITEM :
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1809 ) ) ;
		break ;
	}


	// Ȱ��ȭ ���� â���� Disable�Ǿ� ������ �����Ѵ�.

	// �κ��丮 ���̾�α� ó��.
	CInventoryExDialog* pInven = GAMEIN->GetInventoryDialog() ;
	if( !pInven )
	{
#ifdef _GMTOOL_
		MessageBox( NULL, "Invalid inventory dialog!!", __FUNCTION__, MB_OK ) ;
#endif //_GMTOOL_
	}
	else
	{
		if( pInven->IsActive() ) pInven->SetDisable( FALSE ) ;
	}

	// ���� ���̾�α� ó��.
	CDealDialog* pDealDlg = GAMEIN->GetDealDialog() ;
	if( !pDealDlg )
	{
#ifdef _GMTOOL_
		MessageBox( NULL, "Invalid deal dialog!!", __FUNCTION__, MB_OK ) ;
#endif //_GMTOOL_
	}
	else
	{
		if( pDealDlg->IsActive() ) pDealDlg->SetDisable( FALSE ) ;
	}

	// â�� ��ȯ ó�� ���� �߰�.
	CStorageDialog* pStorageDlg = GAMEIN->GetStorageDialog() ;
	if( !pStorageDlg )
	{
#ifdef _GMTOOL_
		MessageBox( NULL, "Invalid storage dialog!!", __FUNCTION__, MB_OK ) ;
#endif //_GMTOOL_
	}
	else
	{
		if( pStorageDlg->IsActive() ) pStorageDlg->SetDisable( FALSE ) ;
	}

}


void CItemManager::Item_Buy_Ack( void* pMsg )
{
	SetDisableDialog(FALSE, eItemTable_Deal);
	SetDisableDialog(FALSE, eItemTable_Inventory);
	SetDisableDialog(FALSE, eItemTable_Storage);
	SetDisableDialog(FALSE, eItemTable_MunpaWarehouse);

	ITEMOBTAINARRAY * pmsg = (ITEMOBTAINARRAY *)pMsg;

	CItem* pItem = NULL;
	CItem* pPreItem = NULL;
	for(WORD i = 0 ; i < pmsg->ItemNum ; ++i)
	{
		if( (pPreItem = GAMEIN->GetInventoryDialog()->GetItemForPos( pmsg->GetItem(i)->Position )) != NULL)
		{
			DeleteItem(pmsg->GetItem(i)->Position, &pPreItem);
			ASSERT(pPreItem);
		}
		pItem = MakeNewItem(pmsg->GetItem(i),"MP_ITEM_BUY_ACK");	
		BOOL rt = GAMEIN->GetInventoryDialog()->AddItem(pItem);
		if(!rt)
		{
			ASSERT(0);					
		}

		// 090811 ShinJS --- ���� �̳� �������� ��� ����Dialog�� ��ϵǾ� �ִ��� �Ǵ��Ͽ� Update ���ش�
		CFishingDialog* pFishingDlg = GAMEIN->GetFishingDlg();
		ASSERT( pFishingDlg )
		if( pFishingDlg->GetBaitItemIdx() == pItem->GetItemIdx() )
		{
			pFishingDlg->RefreshBaitItem();
		}
		
	}
	MONEYTYPE BuyPrice;
	BuyPrice = pItem->GetItemInfo()->BuyPrice;

	if( pmsg->wObtainCount > 1 )
	{
		CHATMGR->AddMsg( CTC_BUYITEM, CHATMGR->GetChatMsg(37), pItem->GetItemInfo()->ItemName, pmsg->wObtainCount,
						AddComma( BuyPrice * pmsg->wObtainCount ) );	//confirm
	}
	else
	{
		CHATMGR->AddMsg( CTC_BUYITEM, CHATMGR->GetChatMsg(36), pItem->GetItemInfo()->ItemName, AddComma( BuyPrice ) );
	}

	QUICKMGR->RefreshQickItem();
}

// 070329 LYW --- ItemManager : Add function to make string.
void CItemManager::CompositString( char* inputStr, char* str, ... )
{
	if( inputStr == NULL ) return ;
	if( str == NULL ) return;

	va_list argList;

	va_start(argList, str);
	vsprintf(inputStr,str,argList);
	va_end(argList);
}


void CItemManager::Item_Buy_Nack( void* pMsg )
{
	MSG_ITEM_ERROR* pmsg = (MSG_ITEM_ERROR*)pMsg;

	//most of all, inventory full process : taiyo
	SetDisableDialog(FALSE, eItemTable_Inventory);
	SetDisableDialog(FALSE, eItemTable_Storage);
	SetDisableDialog(FALSE, eItemTable_MunpaWarehouse);
	SetDisableDialog(FALSE, eItemTable_Deal);
	//GAMEIN->GetDealDialog()->SetDisable( FALSE );

	switch(pmsg->ECode)
	{
	case NOT_MONEY:	
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(324) );
		break;
	case NOT_SPACE:
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(474) );
		break;
	case NOT_EXIST:
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(476) );
		break;
	case NOT_PLAYER:
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(477) );
		break;
	case NOT_FISHINGPOINT:
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1533 ) );
		break;
	// 090227 ShinJS --- Coin �������� ���� ���� ����
	case NOT_COIN:
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1919 ) );

	// 100104 ShinJS --- PC�� ����Ʈ �������� ���� ���� ����
	case NOT_PCROOMPOINT:
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 2011 ) );
		break;
	// 100105 ShinJS --- PC�� ��Ͼ��Ұ� �ƴ� ���
	case NOT_PCROOM:
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 2013 ) );
		break;
	default:
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(478) );
		break;
	}

	//ASSERT(0);
}

// 090227 ShinJS --- �ϻ��� �߰��۾� : Coin Item ���Խ� Coin ���� �۾�
void CItemManager::Item_Buy_By_Coin( void* pMsg )
{
	if( !pMsg )		return;

	MSG_ITEM_BUY_BY_COIN* pmsg	= (MSG_ITEM_BUY_BY_COIN*)pMsg;
	CItem* deletedItem			= 0;

	// ������ ������Ʈ
	for( DWORD i = 0; i < pmsg->mSize; ++i )
	{
		const ITEMBASE& updatedItem = pmsg->mItem[ i ];

		CItem* item	= GetItem( updatedItem.dwDBIdx );
		
		if( item )
		{
			const BOOL isDuplicated = IsDupItem( updatedItem.wIconIdx );

			// Coin Item ���� ��ȭ
			if( updatedItem.wIconIdx )
			{
				cImage image;

				item->SetBasicImage( GetIconImage( updatedItem.wIconIdx, &image ) );
				item->SetItemBaseInfo( updatedItem );
				item->SetVisibleDurability( isDuplicated );
				item->SetData( updatedItem.wIconIdx );

				RefreshItem( item );
			}
			// Coin Item ����
			else
			{
				DeleteItem( item->GetPosition(), &deletedItem );
			}
		}
	}
}

void CItemManager::Item_Dealer_Ack( void* pMsg )
{
	MSG_WORD* pmsg = (MSG_WORD*)pMsg; 
	if( HERO->GetState() != eObjectState_Die)
		GAMEIN->GetDealDialog()->ShowDealer(pmsg->wData);

	// 070329 LYW --- ItemManager : Setting position deal dialog and inventory dialog.
	GAMEIN->GetDealDialog()->ShowDealDialog(TRUE) ;
}

void CItemManager::Item_Appearance_Add( void* p )
{
	const MSG_APPERANCE_ADD*	message		= ( MSG_APPERANCE_ADD* )p;
	const DWORD					playerIndex = message->dwObjectID;

	CPlayer* player = ( CPlayer* )OBJECTMGR->GetObject( playerIndex );

	if( player &&
		player->GetObjectKind() == eObjectKind_Player )
	{
		player->SetWearedItemIdx( message->mSlotIndex, message->mItemIndex );
		player->SetWearedItemTransmog( message->mSlotIndex, message->mItemTransmog );
		APPEARANCEMGR->AddCharacterPartChange( playerIndex );
	}
	else
	{
		ASSERT( 0 );
	}
}


void CItemManager::Item_Appearance_Remove( void* p )
{
	const MSG_APPERANCE_REMOVE*	message		= ( MSG_APPERANCE_REMOVE* )p;
	const DWORD					playerIndex = message->dwObjectID;

	CPlayer* player = ( CPlayer* )OBJECTMGR->GetObject( playerIndex );

	if( player &&
		player->GetObjectKind() == eObjectKind_Player )
	{
		player->SetWearedItemIdx( message->mSlotIndex, 0 );
		 player->SetWearedItemTransmog( message->mSlotIndex, 0 );
		APPEARANCEMGR->AddCharacterPartChange( playerIndex );
	}
}


void CItemManager::Item_Money( void* pMsg )
{
	MSG_MONEY* pmsg = (MSG_MONEY*)pMsg;
			
	MONEYTYPE OriMoney = HERO->GetMoney();
	MONEYTYPE PrintMoney = 0;

	switch(pmsg->bFlag)
	{
	case 64:
		{
			PrintMoney = pmsg->dwTotalMoney - OriMoney;
			if( PrintMoney )
			{
				// 070126 LYW --- GETMONEY : Modified message number.
				//CHATMGR->AddMsg( CTC_GETMONEY, CHATMGR->GetChatMsg( 120 ), AddComma( PrintMoney ) );
				CHATMGR->AddMsg( CTC_GETMONEY, CHATMGR->GetChatMsg( 301 ), AddComma( PrintMoney ) );
				EFFECTMGR->StartHeroEffectProcess(eEffect_GetMoney);
			}
		}
		break;
	case MF_NOMAL:
		{
			break;
		}
	case MF_OBTAIN: // Obtain Money;
		{
			PrintMoney = pmsg->dwTotalMoney - OriMoney;
			if( PrintMoney )
			{
				// 070126 LYW --- GETMONEY : Modified message number.
				//CHATMGR->AddMsg( CTC_GETMONEY, CHATMGR->GetChatMsg( 120 ), AddComma( PrintMoney ) );
				CHATMGR->AddMsg( CTC_GETMONEY, CHATMGR->GetChatMsg( 301 ), AddComma( PrintMoney ) );
				EFFECTMGR->StartHeroEffectProcess(eEffect_GetMoney);
			}
		}
		break;
	case MF_LOST:
		{
			PrintMoney = OriMoney - pmsg->dwTotalMoney;
			if( PrintMoney )
			{
				// 070126 LYW --- GETMONEY : Modified message number.
				//CHATMGR->AddMsg( CTC_GETMONEY, CHATMGR->GetChatMsg( 118 ), AddComma( PrintMoney) );
				CHATMGR->AddMsg( CTC_GETMONEY, CHATMGR->GetChatMsg( 378 ), AddComma( PrintMoney) );
			}
		}
		break;
	case MF_FEE:
		{
			PrintMoney = OriMoney - pmsg->dwTotalMoney;
			if( PrintMoney )
			{
				CHATMGR->AddMsg( CTC_GETMONEY, CHATMGR->GetChatMsg(1601), AddComma( PrintMoney) );
			}
		}
		break;
	default:
		break;
	}

	HERO->SetMoney(pmsg->dwTotalMoney);

	// ������ ������� ������/������ ����޴µ�, ���� ���¿� ����ؼ� ǥ�����ִ� �׸��� �����Ƿ� ������ ����ɶ����� ������ �ʿ��ϴ�
	{
		CMixDialog* dialog = ( CMixDialog* )WINDOWMGR->GetWindowForID( ITMD_MIXDLG );
		ASSERT( dialog );

		dialog->Refresh();
	}
}


void CItemManager::Item_Obtain_Money( void* pMsg )
{
	MSG_DWORDEX* pmsg = (MSG_DWORDEX*)pMsg;

	DWORDEX OriMoney = HERO->GetMoney();
	DWORDEX PrintMoney = pmsg->dweData - OriMoney;

	if( PrintMoney )
	{
		CHATMGR->AddMsg( CTC_GETMONEY, CHATMGR->GetChatMsg( 301 ), PrintMoney );
		EFFECTMGR->StartHeroEffectProcess(eEffect_GetMoney);
	}
	
	HERO->SetMoney(pmsg->dweData);
}


void CItemManager::Item_Divide_NewItem_Notify( void* pMsg )
{
	MSG_ITEM* pmsg = (MSG_ITEM*)pMsg;
	CItem* pItem = MakeNewItem(&pmsg->ItemInfo,"MP_ITEM_DIVIDE_NEWITEM_NOTIFY");
	int tbidx = GetTableIdxForAbsPos(pItem->GetItemBaseInfo().Position);

	if(tbidx == eItemTable_Inventory)
		GAMEIN->GetInventoryDialog()->AddItem(pItem);
	else if(tbidx == eItemTable_Storage)
		GAMEIN->GetStorageDialog()->AddItem(pItem);
	else if(tbidx == eItemTable_MunpaWarehouse)
		GAMEIN->GetGuildWarehouseDlg()->AddItem(pItem);

	CHATMGR->AddMsg( CTC_GETITEM, CHATMGR->GetChatMsg( 123 ));
}


void CItemManager::Item_Guild_Move_Ack( void* pMsg )
{
	MoveItemToGuild((MSG_ITEM_MOVE_ACK*)pMsg);
	WINDOWMGR->DragWindowNull();
}


void CItemManager::Item_Guild_Move_Nack( void* pMsg )
{
	SetDisableDialog(FALSE, eItemTable_Inventory);
	SetDisableDialog(FALSE, eItemTable_Storage);
	SetDisableDialog(FALSE, eItemTable_MunpaWarehouse);
	WINDOWMGR->BackDragWindow();
}


void CItemManager::Item_DeleteFrom_GuildWare_Notify( void* pMsg )
{
	MSG_ITEM * pItembase = (MSG_ITEM*)pMsg;
	CItem* pItem = NULL;
	if(GAMEIN->GetGuildWarehouseDlg()->DeleteItem(pItembase->ItemInfo.Position, &pItem) == TRUE)
		ItemDelete(pItem);
	else
	{
		ASSERT(0);
	}
}


void CItemManager::Item_AddTo_GuildWare_Notify( void* pMsg )
{
	const MSG_ITEM_WITH_OPTION * pItem = (MSG_ITEM_WITH_OPTION*)pMsg;

	AddOption( pItem->OptionInfo );
	
	GAMEIN->GetGuildWarehouseDlg()->AddItem(&pItem->ItemInfo);
}


void CItemManager::Item_Storage_Move_Ack( void* pMsg )
{
	MoveItemToStorage( (MSG_ITEM_MOVE_ACK*)pMsg );
	WINDOWMGR->DragWindowNull();
}


void CItemManager::Item_Dissolve_GetItem( void* pMsg )
{
	ITEMOBTAINARRAY* const message = (ITEMOBTAINARRAY*)pMsg;
	
	for(WORD i = 0 ; i < message->ItemNum ; ++i)
	{
		const ITEMBASE* const itemBase = message->GetItem(i);

		CItem* deletedItem = 0;
		DeleteItem(
			itemBase->Position,
			&deletedItem);

		GAMEIN->GetInventoryDialog()->AddItem(
			MakeNewItem(itemBase, ""));
	}
}


void CItemManager::Item_Dissolve_Ack( void* pMsg )
{
	const MSG_ITEM_DISSOLVE_ACK* message = ( MSG_ITEM_DISSOLVE_ACK *)pMsg;
	GAMEIN->GetDissolveDialog()->Succeed( *message );
	
	for(DWORD i = 0; i < message->mUpdateResult.mSize; ++i)
	{
		const ITEMBASE& itemBase = message->mUpdateResult.mItem[i];

		RemoveOption(
			itemBase.dwDBIdx);

		CItem* const inventoryItem = GetItem(
			itemBase.dwDBIdx);

		if(0 == inventoryItem)
		{
			CItem* const item = MakeNewItem(
				&itemBase,
				"");

			if(0 == item)
			{
				continue;
			}

			item->SetLock(
				TRUE);

			GAMEIN->GetInventoryDialog()->AddItem(
				item);
			continue;
		}
		else if(0 == itemBase.wIconIdx)
		{
			CItem* deletedItem = 0;
			DeleteItem(
				inventoryItem->GetPosition(),
				&deletedItem);
			continue;
		}

		cImage image;
		inventoryItem->SetBasicImage(
			GetIconImage(itemBase.wIconIdx, &image));
		inventoryItem->SetItemBaseInfo(
			itemBase);
		inventoryItem->SetVisibleDurability(
			IsDupItem(itemBase.wIconIdx));
		inventoryItem->SetDurability(
			itemBase.Durability);
		inventoryItem->SetData(
			itemBase.wIconIdx);
		inventoryItem->SetLock(
			FALSE);

		AddToolTip(
			inventoryItem);
	}
}


void CItemManager::Item_Dissolve_Nack( void* pMsg )
{
	const MSG_DWORD* message = ( MSG_DWORD* )pMsg;

	switch( eLogitemmoney( message->dwData ) )
	{
	case eLog_ItemDissolvePlayerNotExist:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "Report to LUNA homepage, please: Server have no such player data" );
			break;
		}
	case eLog_ItemDissolveItemDataNotMatched:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "Report to LUNA homepage, please: Server item data do not matched with yours" );
			break;
		}
	case eLog_ItemDissolveServerHaveNoDissolvingData:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "Report to LUNA homepage, please: Server has no data about it's dissolution" );
			break;
		}
	case eLog_ItemDissolveItNeedMoreLevel:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "Report to LUNA homepage, please: You need more level to dissolve it" );
			break;
		}
	case eLog_ItemDissolveRemovingItemIsFailed:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "Report to LUNA homepage, please: Removing item is failed" );
			break;
		}
	case eLog_ItemDissolveAddingItemIsFailed:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "Report to LUNA homepage, please: Adding item is failed" );
			break;
		}
	case eLog_ItemDissolveInsufficentSpace:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 839 ) );
			break;
		}
	case eLog_ItemDissolveNoResult:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "Report to LUNA homepage, please: No result remained" );
			break;
		}
	case eLog_ItemDissolvePlayserHasNoInventory:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "Report to LUNA homepage, please: You have no inventory" );
			break;
		}
	case eLog_ItemDissolveScriptInvalid:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "Report to LUNA homepage, please: dissolving script is invalid" );
			break;
		}
	case eLog_ItemDissolveServerError:
	case eLog_ItemDissolveSourceItemNotExist:
	default:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1275 ) );
			break;
		}
	}
}


void CItemManager::Item_Error_Nack( void* p )
{
	SetDisableDialog(FALSE, eItemTable_Inventory);
	SetDisableDialog(FALSE, eItemTable_Storage);
	SetDisableDialog(FALSE, eItemTable_MunpaWarehouse);

	WINDOWMGR->BackDragWindow();

	const MSG_ITEM_ERROR* message = ( MSG_ITEM_ERROR* )p;

	switch( message->ECode )
	{
	case eItemUseErr_PreInsert:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1279 ) );
			break;
		}
	case eItemUseErr_Discard:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1280 ) );
			break;
		}
	case eItemUseErr_AlreadyUse:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1281 ) );
			break;
		}
#ifdef _GMTOOL_
	default:
		{
			CHATMGR->AddMsg(
				CTC_SYSMSG,
				"Item error. rt: %d",
				message->ECode);
			break;
		}
#endif
	}
}


void CItemManager::Item_ChangeItem_Nack( void* pMsg )
{
	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(354) );
}


void CItemManager::Item_Use_ChangeItem_Ack( void* pMsg )
{
	ITEMOBTAINARRAY* pmsg = (ITEMOBTAINARRAY *)pMsg;
		
	CItem* pItem = NULL;

	for(WORD i = 0 ; i < pmsg->ItemNum ; ++i)
	{
		pItem								= GAMEIN->GetInventoryDialog()->GetItemForPos( pmsg->GetItem(i)->Position );
		const DURTYPE	previousQuantity	= ( pItem ? pItem->GetDurability() : 0 );

		// 071127 LYW --- ItemManager : ������ ���� ó���� ���� ���� ó�� �߰�.
		ITEMBASE* pItemBase = pmsg->GetItem(i) ;

		if( !pItemBase ) return ;

		if( pItem )
		{
			pItem->SetItemBaseInfo(*pItemBase);
		}
		else
		{
			pItem = MakeNewItem(pItemBase,"MP_ITEM_MONSTER_OBTAIN_NOTIFY");	

			ITEM_INFO* pInfo = GetItemInfo( pItem->GetItemIdx() ) ;

			if( !pInfo ) continue ;

			if( pInfo->wSeal == eITEM_TYPE_SEAL )
			{
				pItemBase->nSealed = eITEM_TYPE_SEAL ;
				pItemBase->nRemainSecond = pInfo->dwUseTime ;

				pItem->SetItemBaseInfo( *pItemBase ) ;
			}
			
			if(! GAMEIN->GetInventoryDialog()->AddItem(pItem) )
			{
				ASSERT(0);
			}
		}

		if( IsDupItem( pItem->GetItemIdx() ) )
		{
			CHATMGR->AddMsg( CTC_GETITEM, CHATMGR->GetChatMsg( 1112 ), pItem->GetItemInfo()->ItemName, pItem->GetDurability() - previousQuantity );
		}
		else
		{
			CHATMGR->AddMsg( CTC_GETITEM, CHATMGR->GetChatMsg( 101 ), pItem->GetItemInfo()->ItemName );
		}
	}

	if( pItem )
	{
		ItemDropEffect( pItem->GetItemIdx() );
	}
	
	//�������ϡ�aA��AI AOA����e Ao��i��U.
	cDialog* pDlg = WINDOWMGR->GetWindowForID( DBOX_DIVIDE_INV );
	if( pDlg )
	{
		((cDivideBox*)pDlg)->ExcuteDBFunc( 0 );	//Ae��O��o���� ���Ϣ��̡�a
	}

	QUICKMGR->RefreshQickItem();
}


void CItemManager::Item_EventItem_Use( void* pMsg )
{
#ifdef TAIWAN_LOCAL
	if( pmsg->dwData2 == 53074 )
	{
		EFFECTMGR->StartPlayerEffectProcess( pPlayer, FindEffectNum("m_ba_079.beff") );
	}

	// 06. 03. ȭ��Ʈ���� �̺�Ʈ
	if( pmsg->dwData2 = 53151 )
	{
		EFFECTMGR->StartPlayerEffectProcess( pPlayer, FindEffectNum("m_ba_082.beff") );
	}
#else
	MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
	CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject(pmsg->dwData1);

	// 06. 03. ȭ��Ʈ���� �̺�Ʈ
	if( pmsg->dwData2 == 53151 )
	{
		EFFECTMGR->StartPlayerEffectProcess( pPlayer, FindEffectNum("m_ba_082.beff") );
	}
#endif
}


// 071128 LYW --- ItemManager : �����۸� â�� �߰� ó��.
void CItemManager::Item_ShopItem_Info_Ack( void* pMsg ) 
{
	SEND_SHOPITEM_INFO* pmsg = (SEND_SHOPITEM_INFO*)pMsg;

	if( pmsg->ItemCount )
	{
		GAMEIN->GetItemShopDialog()->SetItemInfo(pmsg);
	}
	else
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1193) );
	}

	GAMEIN->GetItemShopDialog()->SetActive(TRUE);
	GAMEIN->GetInventoryDialog()->SetActive( TRUE );
}





// 071203 LYW --- ItemManager
void CItemManager::Item_Skill_Ack( void* pMsg )
{
	if( !pMsg ) return ;

	Item_Use_Ack(pMsg);
}

// 071204 LYW --- ItemManager :
void CItemManager::Item_Reset_Stats_Ack( void* pMsg )
{
	if( !pMsg ) return ;
			
	SEND_DEFAULT_STATS* pmsg = NULL ;
	pmsg = (SEND_DEFAULT_STATS*)pMsg ;

	if( !pmsg ) return ;

	if( !HERO ) return ;

	HERO->SetStrength((DWORD)pmsg->nStr) ;
	HERO->SetDexterity((DWORD)pmsg->nDex) ;
	HERO->SetVitality((DWORD)pmsg->nVit) ;
	HERO->SetIntelligence((DWORD)pmsg->nInt) ;
	HERO->SetWisdom((DWORD)pmsg->nWis) ;
	GAMEIN->GetCharacterDialog()->SetPointLeveling(TRUE, (WORD)pmsg->nPoint);
	CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg( 1283 ) );
}


// 071208 LYW --- ItemManager : ���� �ʱ�ȭ ���� ó��.
void CItemManager::Item_Reset_Stats_Nack( void* pMsg ) 
{
	WINDOWMGR->MsgBox( MBI_NOTICE, MBT_OK, CHATMGR->GetChatMsg( 1282 ) );	
}


// 071208 LYW --- ItemManager : �� �̵� ���� ó��.
void CItemManager::Item_MapMoveScroll_Nack( void* pMsg )
{
	WINDOWMGR->MsgBox(
		MBI_MAPMOVE_NOTICE,
		MBT_OK,
		CHATMGR->GetChatMsg(56));
}


// 080312 NYJ --- ItemManager : ��ü��ġ�� �����ۻ��(Ŭ��) ó��.
void CItemManager::UseItem_WorldShout(CItem* pItem)
{
	if( !pItem ) return ;

	pItem->SetLock(TRUE) ;

	CShoutDialog* pDlg = NULL ;
	pDlg = GAMEIN->GetShoutDlg();

	if( !pDlg ) return ;
	pDlg->SetItem(pItem);
	pDlg->SetActive(TRUE) ;
}

// 080319 LUJ, �߰� ��æƮ ��ġ�� ǥ����
// 080320 LUJ, �߰� ��æƮ ��ġ�� �Լ��� ������
void CItemManager::AddOptionToolTip( cIcon& icon, const ITEMBASE& item )
{
	const ITEM_INFO* info = GetItemInfo( item.wIconIdx );

	if( !	info ||
			eEquipType_None == info->EquipType )
	{
		return;
	}

	// ��������
	switch( info->LimitGender )
	{
	case 1:
		{
			icon.AddToolTipLine( CHATMGR->GetChatMsg( 666 ), TTTC_ITEM_NAME );
			break;
		}
	case 2:
		{
			icon.AddToolTipLine( CHATMGR->GetChatMsg( 667 ), TTTC_ITEM_NAME );
			break;
		}
	}

	// 090708 ONS �ű����� �߰��� ���� ������ �������� ó�� ����
	// ��������
	switch( info->LimitRace )
	{
	case 0:
		{
			// ���� ������
			icon.AddToolTipLine( CHATMGR->GetChatMsg( 1927 ), TTTC_ITEM_NAME );
			break;
		}
	case 1:
		{
			// �޸�/���� ����
			icon.AddToolTipLine( CHATMGR->GetChatMsg( 668 ), TTTC_ITEM_NAME );
			break;
		}
	case 2:
		{
			// ���� ����
			icon.AddToolTipLine( CHATMGR->GetChatMsg( 669 ), TTTC_ITEM_NAME );
			break;
		}
	}

	char line[ MAX_PATH ];

	// ��������
	if( info->LimitLevel )
	{
		sprintf( line, "%s%d", CHATMGR->GetChatMsg( 670 ), info->LimitLevel );
		icon.AddToolTipLine( line, TTTC_ITEM_NAME );
	}

	// 081231 LUJ, �ۼ�Ʈ ������ ǥ���ϱ� ���� ����ü ITEM_OPTION -> PlayerStat���� ����
	PlayerStat base;
	PlayerStat enchantStat;
	ITEM_OPTION option	= GetOption( item.dwDBIdx );

	// ��æƮ�� ��ġ�� ��ȣ�ȿ� ǥ�õ��� �ʴ´�. ���� ����� ���� �޸� ���� �� ��� �ӵ� ����� ���� �ɼ� ������ ���� ����ȴ�.
	// ���� �ϴ� ��ġ�� ������ �����Ѵ�. �׸���, ��æƮ�Ǿ����� �� ��ŭ�� �⺻ ��ġ�� �����ְ�, �ɼ� ��ġ������ ���ش�.
	{
		SetValue( base.mPhysicAttack,	info->PhysicAttack,										SetValueTypeAdd );
		SetValue( base.mPhysicDefense,	info->PhysicDefense,									SetValueTypeAdd );
		SetValue( base.mMagicAttack,	info->MagicAttack,										SetValueTypeAdd );
		SetValue( base.mMagicDefense,	info->MagicDefense,										SetValueTypeAdd );
		SetValue( base.mStrength,		info->ImprovementStr	+ option.mMix.mStrength,		SetValueTypeAdd );
		SetValue( base.mDexterity,		info->ImprovementDex	+ option.mMix.mDexterity,		SetValueTypeAdd );
		SetValue( base.mVitality,		info->ImprovementVit	+ option.mMix.mVitality,		SetValueTypeAdd );
		SetValue( base.mIntelligence,	info->ImprovementInt	+ option.mMix.mIntelligence,	SetValueTypeAdd );
		SetValue( base.mWisdom,			info->ImprovementWis	+ option.mMix.mWisdom,			SetValueTypeAdd );
		SetValue( base.mLife,			info->ImprovementLife,									SetValueTypeAdd );
		SetValue( base.mMana,			info->ImprovementMana,									SetValueTypeAdd );

		if(const EnchantScript* const script = g_CGameResourceManager.GetEnchantScript(option.mEnchant.mIndex))
		{
			const stEnchantLvBonusRate& pEnchantLvBonusRate = g_CGameResourceManager.GetEnchantLvBonusInfo(
				option.mEnchant.mLevel);
			const int value = info->EnchantDeterm * option.mEnchant.mLevel;

			g_CGameResourceManager.AddEnchantValue(
				enchantStat,
				script->mAbility,
				value);
			g_CGameResourceManager.AddEnchantValue(
				option,
				script->mAbility,
				-value);

			for(EnchantScript::Ability::const_iterator it = script->mAbility.begin();
				script->mAbility.end() != it;
				++it )
			{
				switch( EnchantScript::eType( *it ) )
				{
				case ReinforceScript::eTypeStrength:
					{
						SetValue(
							enchantStat.mStrength,
							GetBonusEnchantValue( *info, option, info->ImprovementStr ),
							SetValueTypeAdd );
						break;
					}
				case ReinforceScript::eTypeDexterity:
					{
						SetValue(
							enchantStat.mDexterity,
							GetBonusEnchantValue( *info, option, info->ImprovementDex ),
							SetValueTypeAdd );
						break;
					}
				case ReinforceScript::eTypeVitality:
					{
						SetValue(
							enchantStat.mVitality,
							GetBonusEnchantValue( *info, option, info->ImprovementVit ),
							SetValueTypeAdd );
						break;
					}
				case ReinforceScript::eTypeWisdom:
					{
						SetValue(
							enchantStat.mWisdom,
							GetBonusEnchantValue( *info, option, info->ImprovementWis ),
							SetValueTypeAdd );
						break;
					}
				case ReinforceScript::eTypeIntelligence:
					{
						SetValue(
							enchantStat.mIntelligence,
							GetBonusEnchantValue( *info, option, info->ImprovementInt ),
							SetValueTypeAdd );
						break;
					}
				case ReinforceScript::eTypeLife:
					{
						SetValue(
							enchantStat.mLife,
							GetBonusEnchantValue( *info, option, info->ImprovementLife ),
							SetValueTypeAdd );
						break;
					}
				case ReinforceScript::eTypeMana:
					{
						SetValue(
							enchantStat.mMana,
							GetBonusEnchantValue( *info, option, info->ImprovementMana ),
							SetValueTypeAdd );
						break;
					}
				case ReinforceScript::eTypePhysicAttack:
					{
						SetValue(
							enchantStat.mPhysicAttack,
							GetBonusEnchantValue( *info, option, info->PhysicAttack ),
							SetValueTypeAdd );
						break;
					}
				case ReinforceScript::eTypePhysicDefence:
					{
						SetValue(
							enchantStat.mPhysicDefense,
							GetBonusEnchantValue( *info, option, info->PhysicDefense ),
							SetValueTypeAdd );
						break;
					}
				case ReinforceScript::eTypeMagicAttack:
					{
						SetValue(
							enchantStat.mMagicAttack,
							GetBonusEnchantValue( *info, option, info->MagicAttack ),
							SetValueTypeAdd );
						break;
					}
				case ReinforceScript::eTypeMagicDefence:
					{
						SetValue(
							enchantStat.mMagicDefense,
							GetBonusEnchantValue( *info, option, info->MagicDefense ),
							SetValueTypeAdd );
						break;
					}
				}
			}
		}
		else if(const EnchantScript* const script = g_CGameResourceManager.GetEnchantScript(info->EquipSlot))
		{
			int value = 0;

			if(const BOOL isEnchantedOnDrop = (UINT_MAX == option.mEnchant.mIndex))
			{
				value = info->EnchantDeterm * option.mEnchant.mLevel;
			}

			g_CGameResourceManager.AddEnchantValue(
				enchantStat,
				script->mAbility,
				value);
			g_CGameResourceManager.AddEnchantValue(
				option,
				script->mAbility,
				-value);
		}		
	}

	typedef std::list< TextUnit > TextList;
	TextList firstTextList;
	
	// 081231 LUJ, ���Ŀ� �°� ������ ǥ���Ѵ�
	class ToolTip
	{
	public:
		void Format(TextList& list, const char* title, const PlayerStat::Value& baseValue, int optionValue, DWORD baseColor, LPCTSTR baseValueFormat = _T(" %+0.f"), LPCTSTR optionValueFormat = _T(" %+d"))
		{
			Format(
				list,
				title,
				baseValue,
				optionValue,
				baseColor,
				baseColor,
				baseValueFormat,
				optionValueFormat);
		}

		void Format(TextList& list, const char* title, const PlayerStat::Value& baseValue, int optionValue, DWORD baseColor, DWORD optionColor, LPCTSTR baseValueFormat = _T(" %+0.f"), LPCTSTR optionValueFormat = _T(" %+d"))
		{
			TCHAR textOptionValue[ MAX_PATH ] = { 0 };

			// 081231 LUJ, ǥ���� �ɼ� ���� ���ڿ� ���ۿ� �����Ѵ�
			if( optionValue )
			{
				_stprintf(
					textOptionValue,
					_T( " %+d" ),
					optionValue );
			}

			TCHAR textBaseValue[ MAX_PATH ]	= { 0 };

			// 081231 LUJ, ǥ���� �⺻ ���� ���ڿ� ���ۿ� �����Ѵ�
			if( baseValue.mPlus ||
				baseValue.mPercent )
			{
				TCHAR textPlus[ MAX_PATH ]		= { 0 };
				TCHAR textPercent[ MAX_PATH ]	= { 0 };
				_stprintf(
					textPlus,
					baseValue.mPlus ? baseValueFormat : _T( "" ),
					baseValue.mPlus );
				_stprintf(
					textPercent,
					baseValue.mPercent ? _T( " %+0.f%%" ) : _T( "" ),
					baseValue.mPercent * 100.0f );
				// 081231 LUJ, ���� �� �� ���� ��쿡 ��ǥ�� ����Ѵ�
				_stprintf(
					textBaseValue,
					_T( "%s%s%s" ),
					textPlus,
					( baseValue.mPlus && baseValue.mPercent ) ? _T( "," ) : _T( "" ),
					textPercent );
			}

			const BOOL hasValue = ( _tcslen( textOptionValue ) || _tcslen( textBaseValue ) );

			if( hasValue )
			{
				TextUnit textUnit = {0};
				textUnit.mColor = baseColor;
				_stprintf(
					textUnit.mText,
					_T( "%s%s" ),
					title,
					textBaseValue);
				list.push_back(
					textUnit);

				textUnit.mColor = optionColor;
				textUnit.mIsAttach = TRUE;
				_stprintf(
					textUnit.mText,
					optionValue ? optionValueFormat : "",
					optionValue);
				list.push_back(
					textUnit);
			}
		}
	}
	toolTip;

	// 081231 LUJ, ����/���� ����/������ �⺻ ��ġ�� ��ȣ ���� ǥ�õȴ�
	toolTip.Format(
		firstTextList,
		CHATMGR->GetChatMsg(671),
		base.mPhysicAttack,
		int(enchantStat.mPhysicAttack.mPlus),
		TTTC_ITEM_NAME,
		NAMECOLOR_WANTED,
		_T(" %0.f"),
		_T(" (%+d)"));
	toolTip.Format(
		firstTextList,
		CHATMGR->GetChatMsg(673),
		base.mPhysicDefense,
		int(enchantStat.mPhysicDefense.mPlus),
		TTTC_ITEM_NAME,
		NAMECOLOR_WANTED,
		_T(" %0.f"),
		_T(" (%+d)"));
	toolTip.Format(
		firstTextList,
		CHATMGR->GetChatMsg(672),
		base.mMagicAttack,
		int(enchantStat.mMagicAttack.mPlus),
		TTTC_ITEM_NAME,
		NAMECOLOR_WANTED,
		_T(" %0.f"),
		_T(" (%+d)"));
	toolTip.Format(
		firstTextList,
		CHATMGR->GetChatMsg(674),
		base.mMagicDefense,
		int(enchantStat.mMagicDefense.mPlus),
		TTTC_ITEM_NAME,
		NAMECOLOR_WANTED,
		_T(" %0.f"),
		_T(" (%+d)"));

	// ��� �ɼ��� �׸��� �и��ؼ� ǥ��
	{
		const ITEM_OPTION empytOption = { 0 };

		if( memcmp( &empytOption.mDrop, &option.mDrop, sizeof( empytOption.mDrop ) ) )
		{
			TextUnit textUnit = {0};
			textUnit.mColor = TTTC_DEFAULT;

			firstTextList.push_back(
				textUnit);
		}

		const DWORD size = sizeof( option.mDrop.mValue ) / sizeof( *option.mDrop.mValue );

		for( DWORD i = 0; i < size; ++i )
		{
			const ITEM_OPTION::Drop::Value& value = option.mDrop.mValue[ i ];

			switch( value.mKey )
			{
			case ITEM_OPTION::Drop::KeyPercentStrength:	
			case ITEM_OPTION::Drop::KeyPercentIntelligence:	
			case ITEM_OPTION::Drop::KeyPercentDexterity:	
			case ITEM_OPTION::Drop::KeyPercentWisdom:	
			case ITEM_OPTION::Drop::KeyPercentVitality:	
			case ITEM_OPTION::Drop::KeyPercentPhysicalAttack:	
			case ITEM_OPTION::Drop::KeyPercentPhysicalDefence:	
			case ITEM_OPTION::Drop::KeyPercentMagicalAttack:	
			case ITEM_OPTION::Drop::KeyPercentMagicalDefence:	
			case ITEM_OPTION::Drop::KeyPercentCriticalRate:	
			case ITEM_OPTION::Drop::KeyPercentCriticalDamage:	
			case ITEM_OPTION::Drop::KeyPercentAccuracy:	
			case ITEM_OPTION::Drop::KeyPercentEvade:	
			case ITEM_OPTION::Drop::KeyPercentMoveSpeed:	
			case ITEM_OPTION::Drop::KeyPercentLife:	
			case ITEM_OPTION::Drop::KeyPercentMana:	
			case ITEM_OPTION::Drop::KeyPercentLifeRecovery:	
			case ITEM_OPTION::Drop::KeyPercentManaRecovery:	
				{
					TextUnit textUnit = {0};
					textUnit.mColor = RGB_HALF(57, 187, 225);
					sprintf(
						textUnit.mText,
						"%s %+.1f%%",
						GetName(value.mKey),
						value.mValue * 100.0f);

					firstTextList.push_back(
						textUnit);
					break;	
				}	
			case ITEM_OPTION::Drop::KeyPlusStrength:	
			case ITEM_OPTION::Drop::KeyPlusIntelligence:	
			case ITEM_OPTION::Drop::KeyPlusDexterity:	
			case ITEM_OPTION::Drop::KeyPlusWisdom:	
			case ITEM_OPTION::Drop::KeyPlusVitality:	
			case ITEM_OPTION::Drop::KeyPlusPhysicalAttack:	
			case ITEM_OPTION::Drop::KeyPlusPhysicalDefence:	
			case ITEM_OPTION::Drop::KeyPlusMagicalAttack:	
			case ITEM_OPTION::Drop::KeyPlusMagicalDefence:	
			case ITEM_OPTION::Drop::KeyPlusCriticalRate:	
			case ITEM_OPTION::Drop::KeyPlusCriticalDamage:	
			case ITEM_OPTION::Drop::KeyPlusAccuracy:	
			case ITEM_OPTION::Drop::KeyPlusEvade:	
			case ITEM_OPTION::Drop::KeyPlusMoveSpeed:	
			case ITEM_OPTION::Drop::KeyPlusLife:	
			case ITEM_OPTION::Drop::KeyPlusMana:	
			case ITEM_OPTION::Drop::KeyPlusLifeRecovery:	
			case ITEM_OPTION::Drop::KeyPlusManaRecovery:	
				{
					TextUnit textUnit = {0};
					textUnit.mColor = RGB_HALF(57, 187, 225);
					sprintf(
						textUnit.mText,
						"%s %+.1f",
						GetName(value.mKey),
						value.mValue);

					firstTextList.push_back(
						textUnit);
					break;	
				}
			}
		}
	}

	TextList secondTextList;
	toolTip.Format(
		secondTextList,
		CHATMGR->GetChatMsg(671),
		PlayerStat::Value(),
		option.mReinforce.mPhysicAttack,
		NAMECOLOR_WANTED);
	toolTip.Format(
		secondTextList,
		CHATMGR->GetChatMsg(673),
		PlayerStat::Value(),
		option.mReinforce.mPhysicDefence,
		NAMECOLOR_WANTED);
	toolTip.Format(
		secondTextList,
		CHATMGR->GetChatMsg(672),
		PlayerStat::Value(),
		option.mReinforce.mMagicAttack,
		NAMECOLOR_WANTED);
	toolTip.Format(
		secondTextList,
		CHATMGR->GetChatMsg(674),
		PlayerStat::Value(),
		option.mReinforce.mMagicDefence,
		NAMECOLOR_WANTED);
	toolTip.Format(
		secondTextList,
		CHATMGR->GetChatMsg(676),
		base.mStrength,
		option.mReinforce.mStrength,
		option.mReinforce.mStrength ? NAMECOLOR_WANTED : (option.mMix.mStrength ? RGB_HALF(255, 255, 0) : TTTC_ITEM_VALUE));
	toolTip.Format(
		secondTextList,
		CHATMGR->GetChatMsg(677),
		base.mDexterity,
		option.mReinforce.mDexterity,
		option.mReinforce.mDexterity ? NAMECOLOR_WANTED : (option.mMix.mDexterity ? RGB_HALF(255, 255, 0) : TTTC_ITEM_VALUE));
	toolTip.Format(
		secondTextList,
		CHATMGR->GetChatMsg(678),
		base.mVitality,
		option.mReinforce.mVitality,
		option.mReinforce.mVitality ? NAMECOLOR_WANTED : (option.mMix.mVitality ? RGB_HALF(255, 255, 0) : TTTC_ITEM_VALUE));
	toolTip.Format(
		secondTextList,
		CHATMGR->GetChatMsg(679),
		base.mIntelligence,
		option.mReinforce.mIntelligence,
		option.mReinforce.mIntelligence ? NAMECOLOR_WANTED : (option.mMix.mIntelligence ? RGB_HALF(255, 255, 0) : TTTC_ITEM_VALUE));
	toolTip.Format(
		secondTextList,
		CHATMGR->GetChatMsg(680),
		base.mWisdom,
		option.mReinforce.mWisdom,
		option.mReinforce.mWisdom ? NAMECOLOR_WANTED : (option.mMix.mWisdom ? RGB_HALF(255, 255, 0) : TTTC_ITEM_VALUE));
	toolTip.Format(
		secondTextList,
		CHATMGR->GetChatMsg(681),
		base.mLife,
		option.mReinforce.mLife,
		option.mReinforce.mLife ? NAMECOLOR_WANTED : TTTC_ITEM_VALUE);
	toolTip.Format(
		secondTextList,
		CHATMGR->GetChatMsg(682),
		base.mMana,
		option.mReinforce.mMana,
		option.mReinforce.mMana ? NAMECOLOR_WANTED : TTTC_ITEM_VALUE);
	toolTip.Format(
		secondTextList,
		CHATMGR->GetChatMsg(846),
		base.mRecoveryLife,
		option.mReinforce.mLifeRecovery,
		option.mReinforce.mLifeRecovery ? NAMECOLOR_WANTED : TTTC_ITEM_VALUE);
	toolTip.Format(
		secondTextList,
		CHATMGR->GetChatMsg(847),
		base.mRecoveryMana,
		option.mReinforce.mManaRecovery,
		option.mReinforce.mManaRecovery ? NAMECOLOR_WANTED : TTTC_ITEM_VALUE);
	toolTip.Format(
		secondTextList, 
		CHATMGR->GetChatMsg(840),
		base.mMoveSpeed,
		option.mReinforce.mMoveSpeed,
		option.mReinforce.mMoveSpeed ? NAMECOLOR_WANTED : TTTC_ITEM_VALUE);
	toolTip.Format(
		secondTextList,
		CHATMGR->GetChatMsg(841),
		base.mEvade,
		option.mReinforce.mEvade,
		option.mReinforce.mEvade ? NAMECOLOR_WANTED : TTTC_ITEM_VALUE);
	toolTip.Format(
		secondTextList,
		CHATMGR->GetChatMsg(162),
		base.mAccuracy,	
		option.mReinforce.mAccuracy,
		option.mReinforce.mAccuracy ? NAMECOLOR_WANTED : TTTC_ITEM_VALUE);
	toolTip.Format(
		secondTextList,
		CHATMGR->GetChatMsg(842),
		base.mCriticalRate,	
		option.mReinforce.mCriticalRate,
		option.mReinforce.mCriticalRate ? NAMECOLOR_WANTED : TTTC_ITEM_VALUE);
	toolTip.Format(
		secondTextList,
		CHATMGR->GetChatMsg(843),
		base.mCriticalDamage,
		option.mReinforce.mCriticalDamage,
		option.mReinforce.mCriticalDamage ? NAMECOLOR_WANTED : TTTC_ITEM_VALUE);

	// 081231 LUJ, ������ �߰� �ɷ��� ������ ��� ǥ���ϱ� ���� �����ش�
	{
		const SetScript::Element&	itemElement = GAMERESRCMNGR->GetItemElement( info->ItemIdx );
		const PlayerStat&			itemStat	= itemElement.mStat;

		toolTip.Format(
			secondTextList,
			CHATMGR->GetChatMsg( 671 ),
			itemStat.mPhysicAttack,
			0,
			TTTC_ITEM_VALUE,
			TTTC_ITEM_VALUE);
		toolTip.Format(
			secondTextList,
			CHATMGR->GetChatMsg( 673 ),
			itemStat.mPhysicDefense,
			0,
			TTTC_ITEM_VALUE,
			TTTC_ITEM_VALUE);
		toolTip.Format(
			secondTextList,
			CHATMGR->GetChatMsg( 672 ),
			itemStat.mMagicAttack,
			0,
			TTTC_ITEM_VALUE,
			TTTC_ITEM_VALUE);
		toolTip.Format(
			secondTextList,
			CHATMGR->GetChatMsg( 674 ),
			itemStat.mMagicDefense,
			0,
			TTTC_ITEM_VALUE,
			TTTC_ITEM_VALUE);
		toolTip.Format(
			secondTextList,
			CHATMGR->GetChatMsg( 676 ),
			itemStat.mStrength,
			0,
			TTTC_ITEM_VALUE,
			TTTC_ITEM_VALUE);
		toolTip.Format(
			secondTextList,
			CHATMGR->GetChatMsg( 677 ),
			itemStat.mDexterity,
			0,
			TTTC_ITEM_VALUE,
			TTTC_ITEM_VALUE);
		toolTip.Format(
			secondTextList,
			CHATMGR->GetChatMsg( 678 ),
			itemStat.mVitality,
			0,
			TTTC_ITEM_VALUE,
			TTTC_ITEM_VALUE);
		toolTip.Format(
			secondTextList,
			CHATMGR->GetChatMsg( 679 ),
			itemStat.mIntelligence,
			0,
			TTTC_ITEM_VALUE,
			TTTC_ITEM_VALUE);
		toolTip.Format(
			secondTextList,
			CHATMGR->GetChatMsg( 680 ),
			itemStat.mWisdom,
			0,
			TTTC_ITEM_VALUE,
			TTTC_ITEM_VALUE);
		toolTip.Format(
			secondTextList,
			CHATMGR->GetChatMsg( 681 ),
			itemStat.mLife,
			0,
			TTTC_ITEM_VALUE,
			TTTC_ITEM_VALUE);
		toolTip.Format(
			secondTextList,
			CHATMGR->GetChatMsg( 682 ),
			itemStat.mMana,
			0,
			TTTC_ITEM_VALUE,
			TTTC_ITEM_VALUE);
		toolTip.Format(
			secondTextList,
			CHATMGR->GetChatMsg( 846 ),
			itemStat.mRecoveryLife,
			0,
			TTTC_ITEM_VALUE,
			TTTC_ITEM_VALUE);
		toolTip.Format(
			secondTextList,
			CHATMGR->GetChatMsg( 847 ),
			itemStat.mRecoveryMana,
			0,
			TTTC_ITEM_VALUE,
			TTTC_ITEM_VALUE);
		toolTip.Format(
			secondTextList, 
			CHATMGR->GetChatMsg( 840 ),
			itemStat.mMoveSpeed,
			0,
			TTTC_ITEM_VALUE,
			TTTC_ITEM_VALUE);
		toolTip.Format(
			secondTextList,
			CHATMGR->GetChatMsg( 841 ),
			itemStat.mEvade,
			0,
			TTTC_ITEM_VALUE,
			TTTC_ITEM_VALUE);
		toolTip.Format(
			secondTextList,
			CHATMGR->GetChatMsg( 162 ),
			itemStat.mAccuracy,	
			0,
			TTTC_ITEM_VALUE,
			TTTC_ITEM_VALUE);
		toolTip.Format(
			secondTextList,
			CHATMGR->GetChatMsg( 842 ),
			itemStat.mCriticalRate,	
			0,
			TTTC_ITEM_VALUE,
			TTTC_ITEM_VALUE);
		toolTip.Format(
			secondTextList,
			CHATMGR->GetChatMsg( 843 ),
			itemStat.mCriticalDamage,
			0,
			TTTC_ITEM_VALUE,
			TTTC_ITEM_VALUE);

		// 081231 LUJ, ��ų�� ǥ���Ѵ�
		const SetScript::Element::Skill& skillMap = itemElement.mSkill;

		for(	SetScript::Element::Skill::const_iterator skill_it = skillMap.begin();
				skillMap.end() != skill_it;
				++skill_it )
		{
			const BYTE			skillLevel	= skill_it->second;
			const DWORD			skillIndex	= skill_it->first / 100 * 100 + skillLevel;	
			cSkillInfo* const	skillInfo	= SKILLMGR->GetSkillInfo( skillIndex );
			
			if( ! skillInfo )
			{
				continue;
			}
			else if( skillInfo->GetLevel() )
			{
				TextUnit textUnit = {0};
				textUnit.mColor = TTTC_ITEM_VALUE;
				sprintf(
					textUnit.mText,
					"[%s] %s +%d ",
					skillInfo->GetName(),
					RESRCMGR->GetMsg( 636 ),
					skillInfo->GetLevel() );

				secondTextList.push_back(
					textUnit);
			}
			else
			{
				TextUnit textUnit = {0};
				textUnit.mColor = TTTC_ITEM_VALUE;
				sprintf(
					textUnit.mText,
					"[%s] ",
					skillInfo->GetName());

				secondTextList.push_back(
					textUnit);
			}
		}
	}

	// �����ܿ� ���� ǥ��
	{
		for(TextList::const_iterator it = firstTextList.begin();
			firstTextList.end() != it;
			++it )
		{
			const TextUnit& textUnit = *it;

			if(textUnit.mIsAttach)
			{
				icon.AddToolTipPartialLine(
					FALSE,
					textUnit.mText,
					textUnit.mColor);
			}
			else
			{
				icon.AddToolTipLine(
					textUnit.mText,
					textUnit.mColor);
			}
		}

		if( ! secondTextList.empty() )
		{
			icon.AddToolTipLine( "" );
		}

		for(TextList::const_iterator it = secondTextList.begin();
			secondTextList.end() != it;
			++it )
		{
			const TextUnit& textUnit = *it;

			if(textUnit.mIsAttach)
			{
				icon.AddToolTipPartialLine(
					FALSE,
					textUnit.mText,
					textUnit.mColor);
			}
			else
			{
				icon.AddToolTipLine(
					textUnit.mText,
					textUnit.mColor);
			}
		}
	}	
}

void CItemManager::AddPetToolTip( cIcon& icon, const ITEMBASE& item )
{
	PET_OBJECT_INFO* pPetObjectInfo = PETMGR->GetPetObjectInfo( item.dwDBIdx );

	if( !pPetObjectInfo )
		return;

	icon.AddToolTipLine( "" );

	char buf[256] = {0,};

	if( pPetObjectInfo->State == ePetState_Die )
	{
		cImage dieimage;
		SCRIPTMGR->GetImage( 141, &dieimage );
		icon.SetStateImage( &dieimage );
	}
	else
	{
		icon.ClearStateImage();
	}

	sprintf( buf, CHATMGR->GetChatMsg( 1586 ), pPetObjectInfo->Level );
	icon.AddToolTipLine( buf );

	if( pPetObjectInfo->Type == ePetType_Basic )
	{
		sprintf( buf, CHATMGR->GetChatMsg( 1587 ), RESRCMGR->GetMsg( 1005 ) );
		icon.AddToolTipLine( buf );
	}
	else
	{
		sprintf( buf, CHATMGR->GetChatMsg( 1587 ), RESRCMGR->GetMsg( pPetObjectInfo->Type + 996 ) );
		icon.AddToolTipLine( buf );
	}

	sprintf( buf, CHATMGR->GetChatMsg( 1588 ), RESRCMGR->GetMsg( pPetObjectInfo->Grade + 1000 ) );			
	icon.AddToolTipLine( buf );

	sprintf( buf, CHATMGR->GetChatMsg( 1589 ), pPetObjectInfo->SkillSlot );
	icon.AddToolTipLine( buf );
}

void CItemManager::AddSetToolTip( cIcon& icon, const ITEMBASE& item )
{
	const ITEM_INFO* info	= GetItemInfo( item.wIconIdx );
	const SetScript* script = GAMERESRCMNGR->GetSetScript( item.wIconIdx );

	if( ! info ||
		! script )
	{
		return;
	}

	icon.AddToolTipLine( "" );

	CHero* hero = OBJECTMGR->GetHero();

	// ������ �������� ���⿡ �ִ´�
	typedef	std::set< DWORD >	ItemIndexSet;
	ItemIndexSet				itemIndexSet;

	// ���� ������ ������ �ε����� �¿� �ִ´�.
	{
		BOOL isWearItem = FALSE;
		
		for( POSTYPE position = TP_WEAR_START; position < TP_WEAR_END; ++position )
		{		
			const ITEMBASE* equippedItem = GetItemInfoAbsIn( hero, position );

			if( equippedItem  )
			{
				itemIndexSet.insert( equippedItem->wIconIdx );
				
				if( position == item.Position )
				{
					isWearItem = TRUE;
				}
			}
		}

		// �������� ���� �������� ��Ʈ ������ ��ϸ� �����ֵ��� ��� ���� Ŭ�����Ѵ�.
		if( ! isWearItem )
		{
			itemIndexSet.clear();
		}
	}

	// ������ ��Ʈ ������ ����
	int currentSize = 0;
	// 081231 LUJ, �������� ��Ʈ �ɷ� ���θ� ��Ÿ���� �÷���
	const BOOL itemHaveSetAbility = ( 0 < script->mAbility.size() );

	// �ϼ��� ��Ʈ ������ ��������, ��� ������ ���� ǥ���Ѵ�.
	{
		typedef std::list< TextUnit >			TextList;
		TextList								textList;

		// 080916 LUJ, ���� ������ ǥ�� ���
		if( script->mSlotNameMap.empty() )
		{	
			for(	SetScript::Item::const_iterator it = script->mItem.begin();
					script->mItem.end() != it;
					++it )
			{	
				const ITEM_INFO* itemInfo = GetItemInfo( *it );
	
				if( ! itemInfo )
				{
					continue;
				}
	
				const std::string name( std::string( "  " ) + itemInfo->ItemName );
	
				if( itemIndexSet.end() == itemIndexSet.find( itemInfo->ItemIdx ) )
				{
					TextUnit textUnit = {0};
					textUnit.mColor = TTTC_ITEM_LIMIT;
					SafeStrCpy(
						textUnit.mText,
						name.c_str(),
						_countof(textUnit.mText));

					textList.push_back(
						textUnit);
				}
				else
				{
					TextUnit textUnit = {0};
					textUnit.mColor = RGB_HALF(0, 255, 0);
					SafeStrCpy(
						textUnit.mText,
						name.c_str(),
						_countof(textUnit.mText));
	
					textList.push_back(
						textUnit);

					++currentSize;
				}
			}	
		}
		// 080916 LUJ, ���� ��Ī ������ ���
		else
		{	
			typedef std::set< EWEARED_ITEM > EquipSlotSet;
			EquipSlotSet equipSlotSet;

			// 080912 LUJ, ������ ��Ʈ�� �ʿ��� ���Ե��� ����.
			for(	SetScript::Item::const_iterator it = script->mItem.begin();
					script->mItem.end() != it;
					++it )
			{	
				const ITEM_INFO* itemInfo = GetItemInfo( *it );
	
				if(itemInfo)
				{
					equipSlotSet.insert(
						itemInfo->EquipSlot);
				}
			}	
	
			EquipSlotSet lackedEquipSlotSet( equipSlotSet );
	
			// 080912 LUJ, ���� ������ �� �������� �ʴ� ������ ���ܵд�
			for(	ItemIndexSet::const_iterator it = itemIndexSet.begin();
					itemIndexSet.end() != it;
					++it )
			{	
				const ITEM_INFO* itemInfo = GetItemInfo( *it );
	
				if( ! itemInfo )
				{
					continue;
				}

				// 090129 LUJ, ��Ʈ �������� �ƴ� ���� ǥ������ �ʴ´�
				{
					const SetScript::Item::const_iterator item_it = script->mItem.find( itemInfo->ItemIdx );

					if( script->mItem.end() == item_it )
					{
						continue;
					}
				}
	
				lackedEquipSlotSet.erase(itemInfo->EquipSlot);
			}	
	
			// 080912 LUJ, ������ ������ �ε����� ��Ʈ�� �����Ͽ� ��ġ ���ο� ���� �ٸ� ������ ǥ���Ѵ�
			for(	EquipSlotSet::const_iterator it = equipSlotSet.begin();
					equipSlotSet.end() != it;
					++it )
			{	
				const EWEARED_ITEM equipSlot = *it;	
				const SetScript::SlotNameMap::const_iterator name_it = script->mSlotNameMap.find( equipSlot );
				// 090129 LUJ, ���� ������ ���� ���� ���� ��� �Ϲ� ��Ī�� ǥ���Ѵ�
				const std::string slotName( script->mSlotNameMap.end() == name_it ? "" : name_it->second );
				const std::string name( std::string( "  " ) + ( slotName.empty() ? GetName( equipSlot ) : slotName ) );
	
				if( lackedEquipSlotSet.end() != lackedEquipSlotSet.find( equipSlot ) )
				{
					TextUnit textUnit = {0};
					textUnit.mColor = TTTC_ITEM_LIMIT;
					SafeStrCpy(
						textUnit.mText,
						name.c_str(),
						_countof(textUnit.mText));
	
					textList.push_back(
						textUnit);
				}
				else
				{
					TextUnit textUnit = {0};
					textUnit.mColor = RGB_HALF(0, 255, 0);
					SafeStrCpy(
						textUnit.mText,
						name.c_str(),
						_countof(textUnit.mText));

					textList.push_back(
						textUnit);

					++currentSize;
				}
			}
		}

		// ��Ʈ ���� ������ �����Ѵ�. 
		// 081231 LUJ, ��Ʈ �ɷ��� ���� ���� ǥ���Ѵ�
		// 090129 LUJ, �׷� ��Ʈ�� ��� ������ �׷� ������ ǥ���Ѵ�
		if( itemHaveSetAbility )
		{
			const DWORD setItemSize = ( script->mSlotNameMap.empty() ? script->mItem.size() : script->mSlotNameMap.size() );

			{
				TextUnit textUnit = {0};
				textUnit.mColor = RGB_HALF(255, 255, 0);
				sprintf(
					textUnit.mText,
					"%s(%d/%d)",
					script->mName.c_str(),
					currentSize,
					setItemSize);

				textList.push_front(
					textUnit);
			}

			for(TextList::const_iterator it = textList.begin();
				textList.end() != it; 
				++it )
			{
				const TextUnit& textUnit = *it;

				if(textUnit.mIsAttach)
				{
					icon.AddToolTipPartialLine(
						FALSE,
						textUnit.mText,
						textUnit.mColor);
				}
				else
				{
					icon.AddToolTipLine(
						textUnit.mText,
						textUnit.mColor);
				}
			}
		}
	}

	// �ɷ��� ǥ���Ѵ�.
	// 081231 LUJ, ��Ʈ �ɷ��� ���� ���� ǥ���Ѵ�
	if( itemHaveSetAbility )
	{
		icon.AddToolTipLine( "" );

		char buffer	[ MAX_TEXT_SIZE ];
		char line	[ MAX_TEXT_SIZE ] = { 0, };
		
		/// ��Ʈȿ�� ���� ��ŭ ������ �� �ֵ��� ���� ���ڿ��� �����
		std::string tabString;

		// �޽��� [0]��Ʈ ȿ��
		for(
			size_t tabSize = strlen( RESRCMGR->GetMsg( 632 ) );
			tabSize--;
			)
		{
			tabString += " ";
		}

		for(
			SetScript::Ability::const_iterator it = script->mAbility.begin();
			script->mAbility.end() != it;
			++it )
		{
			const int					size	= it->first;
			const SetScript::Element&	element = it->second;
			const DWORD					color	= size > currentSize ? TTTC_ITEM_LIMIT : RGB_HALF( 0, 255, 0 );
			
			//sprintf( buffer, "[%d]��Ʈȿ��: ", size );
			sprintf( buffer, RESRCMGR->GetMsg( 633 ), size );
			const char* prefix = buffer;

			// ���� ����
			{
				const PlayerStat::Value& value = element.mStat.mPhysicAttack;

				if( value.mPercent )
				{
					sprintf( line, "%s%s %+.0f%% ", prefix, CHATMGR->GetChatMsg( 671 ), value.mPercent * 100 );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
				else if( value.mPlus )
				{
					sprintf( line, "%s%s %+.0f%", prefix, CHATMGR->GetChatMsg( 671 ), value.mPlus );
					strcat( line, " " );	// �Ľ� �����ϱ�? �������� ���� sprintf���� ���� ������ ���ð� �ȴ�...
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
			}
			
			// ���� ����
			{
				const PlayerStat::Value& value = element.mStat.mMagicAttack;

				if( value.mPercent )
				{
					sprintf( line, "%s%s %+.0f%% ", prefix, CHATMGR->GetChatMsg( 672 ), value.mPercent * 100 );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
				else if( value.mPlus )
				{
					sprintf( line, "%s%s %+.0f%", prefix, CHATMGR->GetChatMsg( 672 ), value.mPlus );
					strcat( line, " " );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
			}

			// ���� ���
			{
				const PlayerStat::Value& value = element.mStat.mPhysicDefense;

				if( value.mPercent )
				{
					sprintf( line, "%s%s %+.0f%% ", prefix, CHATMGR->GetChatMsg( 673 ), value.mPercent * 100 );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
				else if( value.mPlus )
				{
					sprintf( line, "%s%s %+.0f ", prefix, CHATMGR->GetChatMsg( 673 ), value.mPlus );
					strcat( line, " " );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
			}

			// ���� ���
			{
				const PlayerStat::Value& value = element.mStat.mMagicDefense;

				if( value.mPercent )
				{
					sprintf( line, "%s%s %+.0f%% ", prefix, CHATMGR->GetChatMsg( 674 ), value.mPercent * 100 );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
				else if( value.mPlus )
				{
					sprintf( line, "%s%s %+.0f%", prefix, CHATMGR->GetChatMsg( 674 ), value.mPlus );
					strcat( line, " " );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
			}

			// ��
			{
				const PlayerStat::Value& value = element.mStat.mStrength;

				if( value.mPercent )
				{
					sprintf( line, "%s%s %+.0f%% ", prefix, CHATMGR->GetChatMsg( 676 ), value.mPercent * 100 );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
				else if( value.mPlus )
				{
					sprintf( line, "%s%s %+.0f%", prefix, CHATMGR->GetChatMsg( 676 ), value.mPlus );
					strcat( line, " " );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
			}

			// ��ø
			{
				const PlayerStat::Value& value = element.mStat.mDexterity;

				if( value.mPercent )
				{
					sprintf( line, "%s%s %+.0f%% ", prefix, CHATMGR->GetChatMsg( 677 ), value.mPercent * 100 );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
				else if( value.mPlus )
				{
					sprintf( line, "%s%s %+.0f%", prefix, CHATMGR->GetChatMsg( 677 ), value.mPlus );
					strcat( line, " " );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
			}

			// ����
			{
				const PlayerStat::Value& value = element.mStat.mVitality;

				if( value.mPercent )
				{
					sprintf( line, "%s%s %+.0f%% ", prefix, CHATMGR->GetChatMsg( 678 ), value.mPercent * 100 );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
				else if( value.mPlus )
				{
					sprintf( line, "%s%s %+.0f%", prefix, CHATMGR->GetChatMsg( 678 ), value.mPlus );
					strcat( line, " " );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
			}

			// ����
			{
				const PlayerStat::Value& value = element.mStat.mIntelligence;

				if( value.mPercent )
				{
					sprintf( line, "%s%s %+.0f%% ", prefix, CHATMGR->GetChatMsg( 679 ), value.mPercent * 100 );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
				else if( value.mPlus )
				{
					sprintf( line, "%s%s %+.0f%", prefix, CHATMGR->GetChatMsg( 679 ), value.mPlus );
					strcat( line, " " );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
			}

			// ����
			{
				const PlayerStat::Value& value = element.mStat.mWisdom;

				if( value.mPercent )
				{
					sprintf( line, "%s%s %+.0f%% ", prefix, CHATMGR->GetChatMsg( 680 ), value.mPercent * 100 );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
				else if( value.mPlus )
				{
					sprintf( line, "%s%s %+.0f%", prefix, CHATMGR->GetChatMsg( 680 ), value.mPlus );
					strcat( line, " " );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
			}

			// ����
			{
				const PlayerStat::Value& value = element.mStat.mLife;

				if( value.mPercent )
				{
					sprintf( line, "%s%s %+.0f%% ", prefix, CHATMGR->GetChatMsg( 681 ), value.mPercent * 100 );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
				else if( value.mPlus )
				{
					sprintf( line, "%s%s %+.0f%", prefix, CHATMGR->GetChatMsg( 681 ), value.mPlus );
					strcat( line, " " );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
			}

			// ����
			{
				const PlayerStat::Value& value = element.mStat.mMana;

				if( value.mPercent )
				{
					sprintf( line, "%s%s %+.0f%% ", prefix, CHATMGR->GetChatMsg( 682 ), value.mPercent * 100 );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
				else if( value.mPlus )
				{
					sprintf( line, "%s%s %+.0f%", prefix, CHATMGR->GetChatMsg( 682 ), value.mPlus );
					strcat( line, " " );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
			}

			// ���� ȸ��
			{
				const PlayerStat::Value& value = element.mStat.mRecoveryLife;

				if( value.mPercent )
				{
					sprintf( line, "%s%s %+.0f%% ", prefix, RESRCMGR->GetMsg( 634 ), value.mPercent * 100 );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
				else if( value.mPlus )
				{
					sprintf( line, "%s%s %+.0f%", prefix, RESRCMGR->GetMsg( 634 ), value.mPlus );
					strcat( line, " " );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
			}

			// ���� ȸ��
			{
				const PlayerStat::Value& value = element.mStat.mRecoveryMana;

				if( value.mPercent )
				{
					sprintf( line, "%s%s %+.0f%% ", prefix, RESRCMGR->GetMsg( 635 ), value.mPercent * 100 );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
				else if( value.mPlus )
				{
					sprintf( line, "%s%s %+.0f%", prefix, RESRCMGR->GetMsg( 635 ), value.mPlus );
					strcat( line, " " );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
			}

			// ����
			{
				const PlayerStat::Value& value = element.mStat.mAccuracy;

				if( value.mPercent )
				{
					sprintf( line, "%s%s %+.0f%% ", prefix, RESRCMGR->GetMsg( 282 ), value.mPercent * 100 );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
				else if( value.mPlus )
				{
					sprintf( line, "%s%s %+.0f%", prefix, RESRCMGR->GetMsg( 282 ), value.mPlus );
					strcat( line, " " );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
			}

			// ȸ��
			{
				const PlayerStat::Value& value = element.mStat.mEvade;

				if( value.mPercent )
				{
					sprintf( line, "%s%s %+.0f%% ", prefix, RESRCMGR->GetMsg( 281 ), value.mPercent * 100 );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
				else if( value.mPlus )
				{
					sprintf( line, "%s%s %+.0f%", prefix, RESRCMGR->GetMsg( 281 ), value.mPlus );
					strcat( line, " " );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
			}

			// ũ��Ƽ�� Ȯ��
			{
				const PlayerStat::Value& value = element.mStat.mCriticalRate;

				if( value.mPercent )
				{
					sprintf( line, "%s%s %+.0f%% ", prefix, CHATMGR->GetChatMsg( 842 ), value.mPercent * 100 );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
				else if( value.mPlus )
				{
					sprintf( line, "%s%s %+.0f%", prefix, CHATMGR->GetChatMsg( 842 ), value.mPlus );
					strcat( line, " " );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
			}

			// ũ��Ƽ�� ����
			{
				const PlayerStat::Value& value = element.mStat.mCriticalDamage;

				if( value.mPercent )
				{
					sprintf( line, "%s%s %+.0f%% ", prefix, CHATMGR->GetChatMsg( 843 ), value.mPercent * 100 );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
				else if( value.mPlus )
				{
					sprintf( line, "%s%s %+.0f%", prefix, CHATMGR->GetChatMsg( 843 ), value.mPlus );
					strcat( line, " " );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
			}

			// 080226 LUJ, �̵� �ӵ�
			{
				const PlayerStat::Value& value = element.mStat.mMoveSpeed;

				if( value.mPercent )
				{
					sprintf( line, "%s%s %+.0f%% ", prefix, CHATMGR->GetChatMsg( 840 ), value.mPercent * 100 );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
				else if( value.mPlus )
				{
					sprintf( line, "%s%s %+.0f%", prefix, CHATMGR->GetChatMsg( 840 ), value.mPlus );
					strcat( line, " " );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
			}

			for(
				SetScript::Element::Skill::const_iterator it = element.mSkill.begin();
				element.mSkill.end() != it;
				++it )
			{
				const short	level	= it->second;
				const DWORD	index	= it->first / 100 * 100 + level;	
				cSkillInfo* info	=  SKILLMGR->GetSkillInfo( index );
				
				if( ! info )
				{
					continue;
				}
				else if( info->GetLevel() )
				{
					sprintf( line, "%s[%s] %s +%d ", prefix, info->GetName(), RESRCMGR->GetMsg( 636 ), info->GetLevel() );
				}
				else
				{
					sprintf( line, "%s[%s] ", prefix, info->GetName() );
				}

				icon.AddToolTipLine( line, color );
				
				prefix = tabString.c_str();
			}
		}
	}
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
					// 091012 ShinJS --- �⺻ ��� ������ Index ����
					result.mBaseMaterial	= itemIndex;
					result.mItemIndex		= strtoul( strtok( 0, separator ), 0, 10 );
					result.mRequiredLevel	= WORD( atoi( strtok( 0, separator ) ) );
					result.mMoney			= DWORDEX( _atoi64( strtok( 0, separator ) ) );

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

				// 090812 ShinJS --- ����: ��� Item���� script�� ���ϱ� ���� ���� �߰�
				// 091012 ShinJS --- ��� �������� ������ ���������� �ٸ���� ���� �߰�
				BOOL bExist = FALSE;
				BOOL bOverlapResult = FALSE;
				stMixResultItemInfo* pMixInfo = NULL;
				if( m_htMixResultItemInfo.GetData( result.mItemIndex ) )
				{
					bOverlapResult = TRUE;
					m_htMixResultItemInfo.StartGetMultiData( result.mItemIndex );
					while( (pMixInfo = m_htMixResultItemInfo.GetMultiData()) != NULL )
					{
						if( pMixInfo->dwBaseItemIdx == itemIndex )
						{
							bExist = TRUE;
							break;
						}
					}
				}

				// ���� ������ �����ϴ� ��� �߰����� �ʴ´�.
				if( bExist )
					break;

				pMixInfo = new stMixResultItemInfo;
				pMixInfo->dwResultItemIdx		= result.mItemIndex;
				pMixInfo->dwBaseItemIdx			= itemIndex;
				pMixInfo->bOverlappedResult		= bOverlapResult;

				m_htMixResultItemInfo.Add( pMixInfo, result.mItemIndex );

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

							abilityMap[ lastRate + float( atof( rate ) ) ] = ability;
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


const ItemMixResult* CItemManager::GetMixResult( DWORD itemIndex, DWORD level ) const
{
	MixScriptMap::const_iterator it = mMixScriptMap.find( itemIndex );

	if( mMixScriptMap.end() != it )
	{
		const MixScript&			mixScript	= it->second;
		MixScript::const_iterator	inner		= mixScript.begin();

		if( mixScript.size() > level )
		{
			std::advance( inner, level );

			const ItemMixResult& mixResult = *inner;

			return &mixResult;
		}
	}

	return 0;
}

void CItemManager::GetMixResultFromResultItemIdx( DWORD resultItemIdx, CYHHashTable<ItemMixResult>* pEqualMixScript )
{
	if( !pEqualMixScript )
		return;

	// ��� �����ۿ� �ش��ϴ� ��� ��ũ��Ʈ�� �����Ѵ�.	
	stMixResultItemInfo* pMixResultItemInfo = NULL;
	m_htMixResultItemInfo.StartGetMultiData( resultItemIdx );
	while( (pMixResultItemInfo = m_htMixResultItemInfo.GetMultiData()) != NULL )
	{
		MixScriptMap::const_iterator it = mMixScriptMap.find( pMixResultItemInfo->dwBaseItemIdx );
		if( mMixScriptMap.end() == it )
			return;
		
		const MixScript& mixScript = it->second;
		MixScript::const_iterator iterList;
		for( iterList = mixScript.begin(); iterList != mixScript.end() ; ++iterList )
		{
			const ItemMixResult& mixResult = *iterList;
			if( mixResult.mItemIndex == resultItemIdx )
				pEqualMixScript->Add( const_cast<ItemMixResult*>( &mixResult ), pEqualMixScript->GetDataNum() );
		}
	}
}

const DWORD CItemManager::GetMixResultPosInfo( DWORD BaseItemIdx, ItemMixResult* pMixResult )
{
	MixScriptMap::const_iterator it = mMixScriptMap.find( BaseItemIdx );
	if( mMixScriptMap.end() == it )
		return (DWORD)-1;

	const MixScript& mixScript = it->second;
	MixScript::const_iterator iterList;
	DWORD dwPos = 0;
	for( iterList = mixScript.begin() ; iterList != mixScript.end() ; ++iterList, ++dwPos )
	{
		if( &(*iterList) == pMixResult )
		{
			return dwPos;
		}
	}

	return (DWORD)-1;
}

DWORD CItemManager::GetItemNameColor( const ITEM_INFO& info ) const
{
	switch( info.kind )
	{
	case ITEM_INFO::eKindNormal:
		{
			return RGB( 254, 254, 254 );
		}
	case ITEM_INFO::eKindHighClass:
		{
			return RGB( 0, 255, 0 );
		}
	case ITEM_INFO::eKindUnique:
		{
			return RGB( 50, 135, 245 );
		}
	case ITEM_INFO::eKindRare:
		{
			return RGB( 254, 150, 20 );
		}
	case ITEM_INFO::eKindLegend:
		{
			return RGB( 230, 50, 230 );
		}
	default:
		{
			ASSERTMSG( 0, "�������� ���� ������ �ֽ��ϴ�. �ϴ� ���� ������ ������� ǥ���մϴ�" );
			return RGB( 254, 254, 254 );
		}
	}
}


const DissolveScript* CItemManager::GetDissolveScript( DWORD itemIndex ) const
{
	DissolveScriptMap::const_iterator it = mDissolveScriptMap.find( itemIndex );

	return mDissolveScriptMap.end() == it ? 0 : &( it->second );
}


void CItemManager::LoadDissolveScript()
{
	CMHFile file;
	file.Init( "System/Resource/itemDissolve.bin", "rb" );

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

BOOL CItemManager::IsRare( const ITEM_INFO* info ) const
{
	if( info )
	{
		switch( info->kind )
		{
		case ITEM_INFO::eKindRare:
		case ITEM_INFO::eKindUnique:
		case ITEM_INFO::eKindLegend:
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

void CItemManager::LoadCoolTimeScript()
{
	CMHFile file;
	file.Init( "system/resource/itemCoolTime.bin", "rb" );

	int		openCount	= 0;
	DWORD	groupIndex	= 1;

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


BOOL CItemManager::IsCoolTime( DWORD playerIndex, DWORD itemIndex ) const
{
	UsingCoolTimeGroup::const_iterator it =  mUsingCoolTimeGroup.find( playerIndex );

	if( mUsingCoolTimeGroup.end() == it )
	{
		return FALSE;
	}

	const CoolTimeGroup& group = it->second;

	return group.end() != group.find( GetCoolTimeGroupIndex( itemIndex ) );
}


BOOL CItemManager::AddCoolTime( DWORD playerIndex, DWORD itemIndex )
{
	const DWORD groupIndex = GetCoolTimeGroupIndex( itemIndex );

	if( ! groupIndex )
	{
		return FALSE;
	}

	// �ش� ��Ÿ�� �׷��� ��������� ó���Ѵ�.
	{
		CoolTimeGroup& group = mUsingCoolTimeGroup[ playerIndex ];

		group.insert( groupIndex );
	}

	// ť�� �־� ��� ���� ������ �ڵ����� �����ǵ��� �Ѵ�.
	{
		CoolTimeScript::ItemData::const_iterator it = mCoolTimeScript.mItemData.find( itemIndex );

		if( mCoolTimeScript.mItemData.end() == it )
		{
			ASSERT( 0 );
			return FALSE;
		}

		const CoolTimeScript::Unit& unit = it->second;

		mCoolTimeQueue.push( CoolTimeSort::Tick() );

		CoolTimeSort::Tick& tick		= mCoolTimeQueue.top();
		const DWORD			tickCount	= GetTickCount();

		tick.mCoolTimeGroup	= groupIndex;
		tick.mBegin			= tickCount;
		tick.mEnd			= tickCount + unit.mTime;
		tick.mPlayerIndex	= playerIndex;


		std::pair<DWORD, CoolTimeSort::Tick> pairData;
		pairData.first = groupIndex;
		pairData.second = tick;
		m_mapCoolTime.insert(pairData);
	}

	return TRUE;
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


void CItemManager::ProcessCoolTime()
{
	// ��Ÿ�� üũ. ���� �ð� ���� ���ĵǾ� �����Ƿ� ���� ������ �ִ� �͸� üũ�Ѵ�.
	if( mCoolTimeQueue.empty() )
	{
		return;
	}

	const DWORD					tickCount	= GetTickCount();
	const CoolTimeSort::Tick&	tick		= mCoolTimeQueue.top();

	// ���� �ð� ���� ���ĵ� �켱���� ť�� ���� ������ �ִ� ������ �ð��� ����Ǿ����� üũ����
	if( ( tick.mBegin > tick.mEnd && tickCount < tick.mBegin && tickCount > tick.mEnd ) ||	// �����÷ε� ��Ȳ
		( tick.mBegin < tick.mEnd && tick.mEnd < tickCount ) )								// �Ϲ����� ���
	{
		UsingCoolTimeGroup::iterator it = mUsingCoolTimeGroup.find( tick.mPlayerIndex );

		if( mUsingCoolTimeGroup.end() != it )
		{
			CoolTimeGroup& group = it->second;

			group.erase( tick.mCoolTimeGroup );

			if( group.empty() )
			{
				mUsingCoolTimeGroup.erase( it );
			}
		}

		if(m_mapCoolTime.end() != m_mapCoolTime.find(tick.mCoolTimeGroup))
		{
			m_mapCoolTime.erase(tick.mCoolTimeGroup);
		}

		mCoolTimeQueue.pop();
	}
}


void CItemManager::RemoveCoolTime( DWORD playerIndex )
{
	mUsingCoolTimeGroup.erase( playerIndex );
}

void CItemManager::UpdateDlgCoolTime(DWORD itemIndex)
{
	if(IsCoolTime(gHeroID, itemIndex))
	{
		CoolTimeScript::ItemData::const_iterator item_it = mCoolTimeScript.mItemData.find( itemIndex );

		if( mCoolTimeScript.mItemData.end() == item_it )
		{
			ASSERT( 0 );
			return;
		}

		const CoolTimeScript::Unit& unit		= item_it->second;

		CoolTimeScript::GroupData::const_iterator group_it = mCoolTimeScript.mGroupData.find( GetCoolTimeGroupIndex( itemIndex ) );

		if( mCoolTimeScript.mGroupData.end() == group_it )
		{
			ASSERT( 0 );
			return;
		}

		//const CoolTimeScript::ItemIndex& indexSet = group_it->second;

		std::map<DWORD, CoolTimeSort::Tick>::iterator iter;
		iter = m_mapCoolTime.find(GetCoolTimeGroupIndex( itemIndex ));

		if(iter != m_mapCoolTime.end())
		{
			// �κ��丮 ������ ��Ÿ�� ó��
			{
				CInventoryExDialog* dialog = ( CInventoryExDialog* )WINDOWMGR->GetWindowForID( IN_INVENTORYDLG );
				ASSERT( dialog );
				dialog->SetCoolTimeAbs( itemIndex, iter->second.mBegin, iter->second.mEnd, unit.mTime);
			}

			// ����â ������ ��Ÿ�� ó��
			{
				cQuickSlotDlg* dialog = ( cQuickSlotDlg* )WINDOWMGR->GetWindowForID( QI1_QUICKSLOTDLG );
				ASSERT( dialog );
				dialog->SetCoolTimeAbs( itemIndex, iter->second.mBegin, iter->second.mEnd, unit.mTime);

				// 080706 LYW --- ItemManager : Ȯ�� �� ���� �߰� ó��.
				cQuickSlotDlg* dialog2 = NULL ;
				dialog2 = ( cQuickSlotDlg* )WINDOWMGR->GetWindowForID( QI2_QUICKSLOTDLG ) ;
				ASSERT( dialog2 ) ;

				if(dialog2)
				{
					dialog2->SetCoolTimeAbs( itemIndex, iter->second.mBegin, iter->second.mEnd, unit.mTime) ;
				}
			}
		}
	}
}


void CItemManager::LoadChangeItem()
{
	CMHFile file;

	if( ! file.Init( "system/resource/changeitem.bin", "rb") )
	{
		ASSERT( 0 );
		return;
	}

	char line[ MAX_PATH ];

	while( ! file.IsEOF() )
	{
		file.GetLine( line, sizeof( line ) );

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

		const char* separator	= " \t";
		const char* token		= strtok( line, separator );

		if (	token						&&
				stricmp( token, "money" )	&&
				stricmp( token, "item" )	&&
				stricmp( token, "end_kind" ) )
		{
			mChangeItemSet.insert( atoi( token ) );
		}
	}
}


BOOL CItemManager::IsChangeItem( DWORD itemIndex ) const
{
	return mChangeItemSet.end() != mChangeItemSet.find( itemIndex );
}


//=========================================================================
//	NAME : UseItem_ResetStats
//	DESC : 071203 LYW --- ���� �ʱ�ȭ ������ ó��.
//=========================================================================
void CItemManager::UseItem_ResetStats(CItem* pItem)
{
	if( !pItem ) return ;																// �������� ���Ἲ�� üũ�Ѵ�.

	MSG_ITEM_USE_SYN msg ;																// �޽��� ����ü�� �����ϰ� �ʱ�ȭ �Ѵ�.
	memset(&msg, 0, sizeof(MSG_ITEM_USE_SYN)) ;

	msg.Category	= MP_ITEM ;															// ī�װ���, ��������, ������Ʈ ���̵� ����.
	msg.Protocol	= MP_ITEM_USE_SYN ;
	msg.dwObjectID	= gHeroID ;

	msg.wItemIdx	= pItem->GetItemIdx() ;												// ������ �ε����� �������� �����Ѵ�.
	msg.TargetPos	= pItem->GetItemBaseInfo().Position ;

	NETWORK->Send( &msg, sizeof(MSG_ITEM_USE_SYN) ) ;									// �޽��� ����.
}





//=========================================================================
//	NAME : UseItem_ResetSkill
//	DESC : 071203 LYW --- ��ų �ʱ�ȭ ������ ó��.
//=========================================================================
void CItemManager::UseItem_ResetSkill(CItem* pItem)
{
	if( !pItem ) return ;																// �������� ���Ἲ üũ.

	MSG_ITEM_USE_SYN msg ;																// �޽��� ����ü�� �����ϰ� �ʱ�ȭ �Ѵ�.
	memset(&msg, 0, sizeof(MSG_ITEM_USE_SYN)) ;

	msg.Category	= MP_ITEM ;															// ī�װ���, ��������, ������Ʈ ���̵� �����Ѵ�.
	msg.Protocol	= MP_ITEM_USE_SYN ;
	msg.dwObjectID	= gHeroID ;

	msg.wItemIdx	= pItem->GetItemIdx() ;												// �������� �ε����� �������� �����Ѵ�.
	msg.TargetPos	= pItem->GetItemBaseInfo().Position ;

	NETWORK->Send( &msg, sizeof(MSG_ITEM_USE_SYN) ) ;									// �޽��� ����.
}

// 080218 KTH -- Reset RegidentRegist
void CItemManager::UseItem_ResetRegistResident(CItem* pItem)
{
	if( !pItem ) return;

	MSG_ITEM_USE_SYN msg;

	memset(&msg, 0, sizeof(MSG_ITEM_USE_SYN));

	msg.Category	= MP_ITEM;
	msg.Protocol	= MP_ITEM_USE_SYN;
	msg.dwObjectID	= gHeroID;
	msg.wItemIdx	= pItem->GetItemIdx();
	msg.TargetPos	= pItem->GetItemBaseInfo().Position;

	NETWORK->Send( &msg, sizeof(MSG_ITEM_USE_SYN) );
}


//=========================================================================
//	NAME : UseItem_MapMoveScroll
//	DESC : 071203 LYW --- �� �̵� ��ũ�� ó��.
//=========================================================================
void CItemManager::UseItem_MapMoveScroll(CItem* pItem)
{
	if( !pItem ) return ;																// �������� ���Ἲ üũ.

	cMapMoveDialog* pDlg = NULL ;														// �� �̵� â�� �޴´�.
	pDlg = GAMEIN->GetMapMoveDlg() ;

	if( !pDlg ) return ;																// �� �̵� â�� ���Ἲ üũ.

	CItem* pDeleteItem = NULL ;
	pDeleteItem = pDlg->GetItemInfo() ;													// ��� ���̴� ������ ������ �޴´�.

	if( pDeleteItem )																	// ��� ���̴� �������� �ִٸ�,
	{
		pDeleteItem->SetLock(FALSE) ;													// ��� ���̴� �������� ����� �����Ѵ�.
	}

	pItem->SetLock(TRUE) ;																// ����� �������� ��ٴ�.
    
	pDlg->SetItemInfo(pItem) ;															// �� �̵�â�� ������ ������ �����Ѵ�.

	// 080919 LYW --- ItemManager : ��� �뵵�� �����Ѵ�.
	pDlg->SetUseCase(eNormalStyle) ;

	pDlg->SetActive(TRUE) ;																// �� �̵�â�� Ȱ��ȭ�Ѵ�.

	pDlg->GetMapListDlg()->ResetGuageBarPos() ;											// �� ����Ʈ�� �ٽ� �����Ѵ�.
}





//=========================================================================
//	NAME : UseItem_ExtendStorage
//	DESC : 071206 LYW --- â�� Ȯ�� ������ ó��.
//=========================================================================
void CItemManager::UseItem_ExtendStorage(CItem* pItem)
{
	if( !pItem ) return ;																// �������� ���Ἲ üũ.

	CInventoryExDialog* pDlg = NULL ;													// �κ��丮�� ���´�.
	pDlg = GAMEIN->GetInventoryDialog() ;

	if( !pDlg ) return ;																// �κ��丮 ���Ἲ üũ.

	MSG_ITEM_USE_SYN msg ;																// �޽��� ����ü�� �����ϰ� �ʱ�ȭ �Ѵ�.
	memset(&msg, 0, sizeof(MSG_ITEM_USE_SYN)) ;

	msg.Category = MP_ITEM;																// ī�װ��� ��, ��Ÿ �޽��� ����ü�� �����Ѵ�.
	msg.Protocol = MP_ITEM_USE_SYN;
	msg.dwObjectID = gHeroID;
	msg.TargetPos = pItem->GetPosition();
	msg.wItemIdx = pItem->GetItemIdx();

	if( IsChangeItem( pItem->GetItemIdx() ) )											// ��ȯ �������̸�,
	{
		pDlg->SetDisable( TRUE );														// �κ��丮�� ������Ų��.

		const DWORD changeSecond = WAITMILISECOND;										// ��ȯ �ð��� �����Ѵ�.

		pDlg->SetCoolTime( pItem->GetItemIdx(), changeSecond );							// �����ۿ� ��Ÿ���� �����Ѵ�.

		{
			cQuickSlotDlg* dialog = NULL ;												// �� ���� ���̾�α׸� �޴´�.
			dialog = ( cQuickSlotDlg* )WINDOWMGR->GetWindowForID( QI1_QUICKSLOTDLG ) ;

			ASSERT( dialog ) ;															// �� ���� ���̾�α��� ���Ἲ üũ.

			dialog->SetCoolTime( pItem->GetItemIdx(), changeSecond ) ;					// �� ���Կ� ��Ÿ���� �����Ѵ�.

			// 080706 LYW --- ItemManager : Ȯ�� �� ���� �߰� ó��.
			cQuickSlotDlg* dialog2 = NULL ;
			dialog2 = ( cQuickSlotDlg* )WINDOWMGR->GetWindowForID( QI2_QUICKSLOTDLG ) ;
			ASSERT( dialog2 ) ;

			if(dialog2)
			{
				dialog2->SetCoolTime( pItem->GetItemIdx(), changeSecond ) ;
			}
		}
	}
	else																				// ��ȯ�������� �ƴϸ�, 
	{
		NETWORK->Send( &msg, sizeof( msg ) ) ;											// �޽����� �����Ѵ�.
	}
}

//=========================================================================
//	NAME : GetSkillPointByItem
//	DESC : 080611 LYW --- ��ų ����Ʈ �߰� �ֹ��� ó��.
//=========================================================================
void CItemManager::GetSkillPointByItem(CItem* pItem)
{
	ASSERT(pItem) ;
	if( !pItem ) return ;																// �������� ���Ἲ üũ.

	MSG_ITEM_USE_SYN msg ;																// �޽��� ����ü�� �����ϰ� �ʱ�ȭ �Ѵ�.
	memset(&msg, 0, sizeof(MSG_ITEM_USE_SYN)) ;

	msg.Category	= MP_ITEM ;															// ī�װ���, ��������, ������Ʈ ���̵� �����Ѵ�.
	msg.Protocol	= MP_ITEM_USE_SYN ;
	msg.dwObjectID	= gHeroID ;

	msg.wItemIdx	= pItem->GetItemIdx() ;												// �������� �ε����� �������� �����Ѵ�.
	msg.TargetPos	= pItem->GetItemBaseInfo().Position ;

	NETWORK->Send( &msg, sizeof(MSG_ITEM_USE_SYN) ) ;									// �޽��� ����.
}





//=========================================================================
//	NAME : IsUsingItem
//	DESC : 071206 LYW --- �̹� ����Ͽ� ������ Ȱ��ȭ ������ üũ�ϴ� �Լ� �߰�.
//=========================================================================
BOOL CItemManager::IsUsingItem(CItem* pItem)
{
	const ITEM_INFO* const pInfo = GetItemInfo(
		pItem->GetItemIdx());

	if(NULL == pInfo)
	{
		return FALSE;
	}

	cActiveSkillInfo* const pActiveSkillInfo = (cActiveSkillInfo*)SKILLMGR->GetSkillInfo(
		pInfo->SupplyValue);

	if(NULL == pActiveSkillInfo)
	{
		return FALSE;
	}

	return STATUSICONDLG->IsHasBuff(
		pActiveSkillInfo->GetSkillInfo()->Buff[0]);
}





//=========================================================================
//	NAME : UseItem_ItemMallItem
//	DESC : 071206 LYW --- �����۸� ������ ��� �Լ� �߰�.
//=========================================================================
void CItemManager::UseItem_ItemMallItem(CItem* pItem, CObject* pTarget)
{
	if( !pItem ) return ;																// �������� ���Ἲ üũ.
	if(!pTarget) return;

	MAINTARGET MainTarget;																// �� Ÿ���� �����Ѵ�.

	CActionTarget Target ;																// �׼�Ÿ���� HERO�� �ϰ�, �� Ÿ������ ó���Ѵ�.
	Target.InitActionTarget(/*HERO*/pTarget, NULL) ;		
	Target.GetMainTarget( &MainTarget ) ;

	DWORD dwSupplyValue = 0 ;
	dwSupplyValue = pItem->GetItemInfo()->SupplyValue ;									// �������� �����ϴ� ��ų �ε����� �޴´�.

	ROTATEINFO* pRotateInfo = HERO->GetBaseRotateInfo();
	if( !pRotateInfo )	return;
	const float fOperatorAngle = (pRotateInfo->bRotating == TRUE ? pRotateInfo->Angle.GetTargetAngleRad() : HERO->GetAngle());
	const VECTOR3 directionVector = GetRadToVector( fOperatorAngle );

	MSG_ITEM_SKILL_START_SYN msg ;														
	memset(&msg, 0, sizeof(MSG_ITEM_SKILL_START_SYN)) ;									// �޽��� ����ü�� �����ϰ� �ʱ�ȭ �Ѵ�.

	msg.Category	= MP_ITEM ;
	msg.Protocol	= MP_ITEM_SKILL_SYN ;												// ī�׵��� ��, �� �޽��� ������ �����Ѵ�.
	msg.dwObjectID	= gHeroID ;
	msg.Idx			= pItem->GetItemIdx() ;
	msg.Pos			= pItem->GetItemBaseInfo().Position ;

	msg.SkillStartSyn.InitMsg( dwSupplyValue, &MainTarget, directionVector, gHeroID ) ;		// ��ų ���� ��û �޽����� �ʱ�ȭ�Ѵ�.

	NETWORK->Send(&msg,sizeof(MSG_ITEM_SKILL_START_SYN)) ;								// �޽����� �����Ѵ�.
}





//=========================================================================
//	NAME : UseItem_ExtendInventory
//	DESC : 071210 LYW --- �κ��丮 Ȯ�� ������ ó��.
//=========================================================================
void CItemManager::UseItem_ExtendInventory(CItem* pItem)
{
	if( !pItem ) return ;																// �������� ���Ἲ�� üũ�Ѵ�.

	CInventoryExDialog* pDlg = NULL ;
	pDlg = GAMEIN->GetInventoryDialog() ;												// �κ��丮�� �ް�, �κ��丮�� ���Ἲ�� üũ�Ѵ�.

	if( !pDlg ) return ;

	MSG_ITEM_USE_SYN msg ;																// �޽��� ����ü�� �����ϰ� �ʱ�ȭ �Ѵ�.
	memset(&msg, 0, sizeof(MSG_ITEM_USE_SYN)) ;

	msg.Category = MP_ITEM;																// ī�װ��� ��, �� �޽����� �����Ѵ�.
	msg.Protocol = MP_ITEM_INCREASE_INVENTORY_SYN ;
	msg.dwObjectID = gHeroID;
	msg.TargetPos = pItem->GetPosition();
	msg.wItemIdx = pItem->GetItemIdx();

	pDlg->SetDisable( TRUE );															// �κ��丮�� ������Ų��.

	if( IsChangeItem( pItem->GetItemIdx() ) )											// ��ȯ ���� �������̸�,
	{
		const DWORD changeSecond = WAITMILISECOND;										// ��ȯ �ð��� �����Ѵ�.

		pDlg->SetCoolTime( pItem->GetItemIdx(), changeSecond );							// �κ��丮�� ��Ÿ���� �����Ѵ�.

		{
			cQuickSlotDlg* dialog = NULL ;
			dialog = (cQuickSlotDlg*)WINDOWMGR->GetWindowForID(QI1_QUICKSLOTDLG);		// �� ���� ���̾�α׸� �޴´�.
			ASSERT( dialog );

			dialog->SetCoolTime( pItem->GetItemIdx(), changeSecond );					// �� ���� ���̾�α׿� ��Ÿ���� �����Ѵ�.

			// 080706 LYW --- ItemManager : Ȯ�� �� ���� �߰� ó��.
			cQuickSlotDlg* dialog2 = NULL ;
			dialog2 = ( cQuickSlotDlg* )WINDOWMGR->GetWindowForID( QI2_QUICKSLOTDLG ) ;
			ASSERT( dialog2 ) ;

			if(dialog2)
			{
				dialog2->SetCoolTime( pItem->GetItemIdx(), changeSecond ) ;
			}
		}
	}
	else																				// ��ȯ �Ұ��� �������̸�,
	{
		NETWORK->Send( &msg, sizeof( msg ) ) ;											// �޽����� �����Ѵ�.
	}
}





//=========================================================================
//	NAME : Item_Increase_Inventory_Ack
//	DESC : 071210 LYW --- �κ��丮 Ȯ�� ���� �Լ� �߰�.
//=========================================================================
void CItemManager::Item_Increase_Inventory_Ack( void* pMsg )
{
	if( !pMsg ) return ;																// �޽����� ���Ἲ�� üũ�Ѵ�.

	MSG_DWORD* pmsg = NULL ;															// �޽����� �޴´�.
	pmsg = (MSG_DWORD*)pMsg ;

	if( !pmsg ) return ;																// �޽����� ���Ἲ üũ.

	if( pmsg->dwObjectID != gHeroID ) return ;											// HERO�� �������� üũ.

	CInventoryExDialog* pDlg = NULL ;													
	pDlg = GAMEIN->GetInventoryDialog() ;												// �κ��丮�� �ް�, ���Ἲ üũ�� �Ѵ�.

	if( !pDlg ) return ;

	switch(pmsg->dwData)																// �޽��� Ÿ���� 1�̸�,
	{
	case 1 : 
		{
			pDlg->ShowTab(2,TRUE) ;														// �κ��丮 ��ĭ Ȯ�� ó���� �Ѵ�.
			HERO->Set_HeroExtendedInvenCount(1) ;
		}
		break ;
	case 2 :																			// �޽��� Ÿ���� 2�̸�,
		{
			pDlg->ShowTab(2,TRUE) ; 
			pDlg->ShowTab(3,TRUE) ; 
																						// �κ��丮 ��ĭ Ȯ�� ó���� �Ѵ�.
			HERO->Set_HeroExtendedInvenCount(2) ;
		}
		break ;
	default : break ;
	}

	WINDOWMGR->MsgBox( MBI_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1249) ) ;						// �κ��丮 Ȯ���� �˸���.

	// 071212 LYW --- ItemManager : �κ��丮 Ȯ�� ����ڵ�.
	pDlg->SetDisable(FALSE) ;
}





//=========================================================================
//	NAME : Item_Increase_Inventory_Nack
//	DESC : 071210 LYW --- �κ��丮 Ȯ�� ���� �Լ� �߰�.
//=========================================================================
void CItemManager::Item_Increase_Inventory_Nack( void* pMsg )
{
	if( !pMsg ) return ;																// �޽����� ���Ἲ üũ.

	MSG_DWORD* pmsg = NULL ;
	pmsg = (MSG_DWORD*)pMsg ;															// �޽����� �޴´�.

	if( !pmsg ) return ;

	switch(pmsg->dwData)
	{
	case 0 :																			// 0�� ������ ����� ���� �Ͽ����� ������.
		{
			WINDOWMGR->MsgBox( MBI_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1250) ) ;
		}
		break ;

	case 1 :																			// 1�� ���̻� �ø� �������� ���� �ÿ� ������.
		{
			WINDOWMGR->MsgBox( MBI_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1248) ) ;
		}
		break ;
	}

	// 071212 LYW --- ItemManager : �κ��丮 Ȯ�� ����ڵ�.
	CInventoryExDialog* pDlg = NULL ;													// �κ��丮�� �޴´�.
	pDlg = GAMEIN->GetInventoryDialog() ;

	if( !pDlg ) return ;

	pDlg->SetDisable(FALSE) ;
}


//=========================================================================
//	NAME : UseItem_ChangeName
//	DESC : �̸� ���� ������ ����� ó���ϴ� �Լ�. 071226 LYW
//=========================================================================
void CItemManager::UseItem_ChangeName(char* pName)
{
	if( !pName ) return ;																// �̸��� ���Ἲ üũ.

	int nNameLength = 0 ;																// �̸��� ���̸� �޴´�.
	nNameLength = strlen(pName) ;

	// 080124 KTH -- ������ ��� �ÿ��� ����Ҽ� ����.
	if(HERO->IsPKMode())
	{
		WINDOWMGR->MsgBox( MBI_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1263) ) ;					// ���� �޽����� ����Ѵ�.
		return;
	}

	// 080610 LYW --- ItemManager : ĳ���� ������ 8���ڰ� �ȵȴٰ� �ϴ� ���� ����.
	//if( nNameLength < 4 || nNameLength >= MAX_NAME_LENGTH )								// �̸� ������ ��ȿ���� üũ.
	if( nNameLength < 4 || nNameLength >= MAX_NAME_LENGTH + 1 )								// �̸� ������ ��ȿ���� üũ.
	{
		cEditBox * pNameBox = NULL ;
		pNameBox = (cEditBox *)WINDOWMGR->GetWindowForIDEx(NAME_BOX) ;					// �̸� �Է� �ڽ��� �޴´�.

		if( pNameBox )
		{
			pNameBox->SetEditText("") ;													// �������� �����Ѵ�.
		}

		WINDOWMGR->MsgBox( MBI_NOTICE, MBT_OK, CHATMGR->GetChatMsg(19) ) ;					// ���� �޽����� ����Ѵ�.
		return ;
	}

	if( FILTERTABLE->IsInvalidCharInclude((unsigned char*)pName) )						// �߸� �� �̸����� ���͸� üũ�� �Ѵ�.
	{
		cEditBox * pNameBox = NULL ;
		pNameBox = (cEditBox *)WINDOWMGR->GetWindowForIDEx(NAME_BOX) ;					

		if( pNameBox )
		{
			pNameBox->SetEditText("") ;													// �������� �����Ѵ�.
		}

		WINDOWMGR->MsgBox( MBI_NOTICE, MBT_OK, CHATMGR->GetChatMsg(14) ) ;					// ���� �޽����� ����Ѵ�.
		return ;
	}

	if( !FILTERTABLE->IsUsableName(pName) )												// ��� ������ �̸����� üũ�Ѵ�.
	{
		cEditBox * pNameBox = NULL ;
		pNameBox = (cEditBox *)WINDOWMGR->GetWindowForIDEx(NAME_BOX) ;

		if( pNameBox )																	// �̸� �Է¶��� �������� �����Ѵ�.
		{
			pNameBox->SetEditText("") ;
		}

		WINDOWMGR->MsgBox( MBI_NOTICE, MBT_OK, CHATMGR->GetChatMsg(14) ) ;					// ���� �޽����� ����Ѵ�.
		return ;
	}

	CInventoryExDialog* pDlg = NULL ;
	pDlg = GAMEIN->GetInventoryDialog() ;												// �κ��丮�� �޴´�.

	if( !pDlg ) return ;																// �κ��丮�� ���Ἲ üũ.

	CItem* pItem = NULL ;
	pItem = pDlg->Get_QuickSelectedItem() ;												// �������� �޴´�.

	if( !pItem ) return ;																// �������� ���Ἲ üũ.

	MSG_CHANGE_CHARACTER_NAME_SYN msg ;
	memset(&msg, 0, sizeof(MSG_CHANGE_CHARACTER_NAME_SYN)) ;							// �޽��� ����ü�� �����ϰ� �ʱ�ȭ �Ѵ�.

	msg.Category = MP_ITEM ;
	msg.Protocol = MP_ITEM_CHANGE_CHARACTER_NAME_SYN ;
	msg.dwObjectID = gHeroID ;															// ī�װ���, ���� ������ �����Ѵ�.

	strcpy(msg.changeName, pName) ;														// �̸��� �����Ѵ�.

	msg.TargetPos = pItem->GetPosition() ;												// ������ ������ �����Ѵ�.
	msg.wItemIdx  = pItem->GetItemIdx() ;

	pItem->SetLock(FALSE) ;
	pDlg->Set_QuickSelectedItem(NULL) ;													// �������� ����� �����Ѵ�.

	NETWORK->Send(&msg, sizeof(msg)) ;													// �޽����� �����Ѵ�.

	cChangeNameDialog* pNameBox = NULL ;
	pNameBox = GAMEIN->GetChangeNameDlg() ;												// �̸� ����â�� �޴´�.

	if( !pNameBox ) return ;															// �̸� �������� ���Ἲ üũ.

	pNameBox->SetActive(FALSE) ;														// �̸� ����â�� ��Ȱ��ȭ �Ѵ�.
}

//=========================================================================
//	NAME : Item_Change_Character_Name_Ack
//	DESC : �̸� ���� ������ ��� ������ ó���ϴ� �Լ�. 071226 LYW
//=========================================================================
void CItemManager::Item_Change_Character_Name_Ack( void* pMsg )
{
	WINDOWMGR->MsgBox( MBI_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1254) ) ;				// �̸� ������ ���� �α׾ƿ��� �˸���.
}


//=========================================================================
//	NAME : Item_Change_Character_Name_Nack
//	DESC : �̸� ���� ������ ��� ���и� ó���ϴ� �Լ�. 071226 LYW
//=========================================================================
void CItemManager::Item_Change_Character_Name_Nack( void* pMsg ) 
{
	if( !pMsg ) return ;																// �޽��� ���Ἲ üũ.

	MSG_DWORD* pmsg = NULL ;
	pmsg = (MSG_DWORD*)pMsg ;															// �޽����� �޴´�.

	if( !pmsg ) return ;																// �޽����� ���Ἲ üũ.

	int nMsgNum = 0 ;																	// �޽��� ��ȣ�� ���� ���� ����.

	switch( pmsg->dwData )																// �޽����� Ÿ�Կ� ����, 
	{
	case 0 :nMsgNum = 1255 ;	break ;													// �̸� ���� ����,
	case 2 :nMsgNum = 1259 ;	break ;													// (�йи�) ���� ����
	case 3 :nMsgNum = 1258 ;	break ;													// (��Ƽ) ���� ����
	case 4 :nMsgNum = 1257 ;	break ;													// (���) ���� ����
	case 5 :nMsgNum = 1256 ;	break ;													// (�ߺ�) ���� ����
	case 6 :nMsgNum = 2329 ;	break ;													// �ǸŴ���
	case 7 :nMsgNum = 2330 ;	break ;													// ��������
	default : return ;
	}

	WINDOWMGR->MsgBox( MBI_NOTICE, MBT_OK, CHATMGR->GetChatMsg( nMsgNum ) ) ;					// ���� �޽����� ����.
}


// 080228 LUJ, ���� ��ȣ ������ ����
BOOL CItemManager::IsMixProtectionItem( const ITEMBASE& item ) const
{
	return mMixProtectionItemSet.end() != mMixProtectionItemSet.find( item.wIconIdx );
}

void CItemManager::LoadItemEffectList()
{
	CMHFile file;

	if( ! file.Init("Data/Script/Direct/EnchantSetting.bin", "rb" ) )
	{
		return;
	}

	while( ! file.IsEOF() )
	{
		EnchantEffect* pEffect = new EnchantEffect;

		pEffect->ItemIdx = file.GetDword();
		
		if ( mEffectList.GetData( pEffect->ItemIdx ) )
		{
			continue;
		}

		for( int i =0; i < 13; i++ )
		{
			for( int j = 0; j < 3; j++ )
			{
				pEffect->Effect[ i ][ j ] = file.GetWord();
			}
		}

		mEffectList.Add( pEffect, pEffect->ItemIdx );
	}
}

EnchantEffect* CItemManager::GetItemEffect( DWORD ItemIdx )
{
	return (EnchantEffect*)mEffectList.GetData( ItemIdx );
}

// 080414 LUJ, �ܾ� ��ȯ ��ũ��Ʈ�� �д´�
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

// 080414 LUJ, �����ۿ� �ش��ϴ� �ܾ� ��ȯ ��ũ��Ʈ�� ��ȯ�Ѵ�.
const CItemManager::BodyChangeScriptListMap& CItemManager::GetBodyChangeScriptListMap() const
{
	return mBodyChangeScriptListMap;
}

void CItemManager::LoadGiftEvent()
{
	CMHFile file;
	if(!file.Init("System/Resource/Event_get_list.bin", "rb"))
		return;

	char Token[256];
	char* buf;
	int len;

	while(1)
	{
		if(file.IsEOF())
			break;

		file.GetString( Token );
		if( strcmp(Token,"#EventInfo") == 0 )
		{
			while(1)
			{
				if(file.IsEOF())
					break;

				EVENTINFO* pInfo = new EVENTINFO;

				pInfo->Index = file.GetDword();
				pInfo->Delay = file.GetDword();
				pInfo->Next  = file.GetDword();
				pInfo->Item  = file.GetDword();
				pInfo->Count = file.GetDword();

				file.GetString();

				buf = file.GetStringInQuotation();
				len = strlen( buf );
				pInfo->Msg = new char[ len + 1];
				strcpy( pInfo->Msg, buf );
				pInfo->Msg[ len ] = 0;

				mEventInfoTable.Add( pInfo, pInfo->Index );
			}
		}
	}
}

void CItemManager::Item_Gift_Event_Notify( void* pMsg )
{
	MSG_DWORD2* pmsg = ( MSG_DWORD2* )pMsg;

	DWORD eventidx = pmsg->dwData1;
	DWORD state = pmsg->dwData2;

	EVENTINFO* pInfo = mEventInfoTable.GetData( eventidx );

	if( !pInfo )
		return;

	switch( state )
	{
	case 1:	// �̺�Ʈ ����
		{
			if(CSHMonstermeterDlg* const dialog = GAMEIN->GetMonstermeterDlg())
			{
				m_GiftEventTime = pInfo->Delay * 1000;

				dialog->SetGiftEvent(
					TRUE);
				dialog->SetGiftEventTime(
					m_GiftEventTime);
			}
		}
		break;
	case 2: // �̺�Ʈ �Ϸ� ������ ����
		{
			//CHATMGR->AddMsg(CTC_SYSMSG, pInfo->Msg) ;

			GAMEIN->GetItemPopupDlg()->SetText( pInfo->Msg );
			GAMEIN->GetItemPopupDlg()->SetActive( TRUE );
		}
		break;
	}
}

// 080820 LUJ, ȹ�� �� ��Ÿ���� ���� �������� �̵� �ÿ� �ð��� ������Ʈ������Ѵ�. ������ �˾Ƽ� �����Ѵ�
void CItemManager::UpdateCoolTimeItem( CItem& item )
{
	const ITEM_INFO* info = GetItemInfo( item.GetItemIdx() );

	if( !	info ||
			info->SupplyType != ITEM_KIND_COOLTIME )
	{
		return;
	}

	item.SetRemainTime( item.GetRemainedCoolTime() / 1000 );
}

const MixSupportScript* CItemManager::GetMixSupportScript( DWORD itemIndex ) const
{
	const MixSupportScriptMap::const_iterator it = mMixSupportScriptMap.find( itemIndex );

	return mMixSupportScriptMap.end() == it ? 0 : &( it->second );
}

void CItemManager::UseItem_WarterSeed(CItem* pItem)
{
	if(!pItem)
	{
#ifdef _GMTOOL_
   		MessageBox( NULL, "Invalid item!!", __FUNCTION__, MB_OK ) ;
#endif //_GMTOOL_
   		return ;
	}
   	else if( GUILDMGR->GetLevel() <  SIEGEWARFAREMGR->Get_WaterSeedLimitGuildLevel() )
   	{
   		CHATMGR->AddMsg( CTC_CHEAT_1, CHATMGR->GetChatMsg( 1693 ) ) ;
   		return ;
   	}
   
   	CObject* pTarget = OBJECTMGR->GetSelectedObject() ;
   
   	if( !pTarget )
   	{
   		CHATMGR->AddMsg( CTC_CHEAT_1, CHATMGR->GetChatMsg(1689) ) ;
   		return ;
   	}
   
   	WORD wNpcJob = ((CNpc*)pTarget)->GetNpcJob() ;
   
   	if( pTarget->GetObjectKind() != eObjectKind_Npc || wNpcJob != SIEGEWELL_ROLE )
   	{
   		CHATMGR->AddMsg( CTC_CHEAT_1, CHATMGR->GetChatMsg(1043) ) ;
   		return ;
   	}
   
   
	// 081017 LYW --- ItemManager : ���ͽõ� ��� ��, �칰���� �Ÿ��� üũ�ϴ� �κ� �߰�.
	// HERO�� ��ġ�� �޴´�.
	VECTOR3 HeroPos = HERO->GetCurPosition() ;


	// ���ͽõ��� ��ġ�� �޴´�.
	VECTOR3 TargetPos = pTarget->GetCurPosition() ;


	// �Ÿ��� üũ�Ѵ�.
	float fDistance = 0.0f ;
	fDistance = CalcDistanceXZ( &HeroPos, &TargetPos ) ;

	if( fDistance > SIEGEWARFAREMGR->Get_CravingDistance() )
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1720 ) ) ;
		return ;
	}

	// 090706 pdy Ŭ���̾�Ʈ ���ͽõ� �ð� ���� ������ ���� 
   	MSG_SIEGEWARFARE_USE_WATERSEED_SYN msg ;
   	memset(&msg, 0, sizeof(MSG_SIEGEWARFARE_USE_WATERSEED_SYN)) ;
   
   	msg.Category	= MP_SIEGEWARFARE ;
   	msg.Protocol	= MP_SIEGEWARFARE_USE_WATERSEED_SYN ;
   
   	msg.dwObjectID	= gHeroID ;
   
   	msg.wItemIdx	= pItem->GetItemIdx() ;
   	msg.TargetPos	= pItem->GetItemBaseInfo().Position ;

	//Ŭ���̾�Ʈ�� ���ͽõ� �ð��� �Բ� ���ٴ�.
	msg.dwWaterSeedUsingSecond = SIEGEWARFAREMGR->Get_WaterSeedUsingSecond();
   
   	NETWORK->Send( &msg, sizeof(MSG_SIEGEWARFARE_USE_WATERSEED_SYN) ) ;
   
   
   	// ĳ������ ���¸� Deal ���·� �����Ѵ�.
   	OBJECTSTATEMGR->StartObjectState( HERO, eObjectState_Deal ) ;
}

// 091215 ONS ������Stack�ִ���� �����ۺ��� ������ ������ ó���ϱ����� 
// �ش� �������� stack���� ��ȯ�Ѵ�.
WORD CItemManager::GetItemStackNum( DWORD dwItemIndex )
{
	const ITEM_INFO* pInfo = GetItemInfo( dwItemIndex );

	return 0 == pInfo ? 0 : pInfo->Stack;
}

// 100517 ONS �����ʱ�ȭ ������ ó�� �߰�
void CItemManager::UseItem_ChangeClass( CItem* pItem )
{
	if( !pItem )
	{
		return;
	}

	CChangeClassDlg* pDlg = GAMEIN->GetChangeClassDlg();
	if( !pDlg )
	{
		return;
	}

	MSG_ITEM_CHANGE_CLASS msg;														
	ZeroMemory(&msg, sizeof(msg));

	msg.Category	= MP_ITEM;														
	msg.Protocol	= MP_ITEM_CHANGE_CLASS_SYN;
	msg.dwObjectID	= gHeroID;

	msg.wItemIdx	= pItem->GetItemIdx();											
	msg.TargetPos	= pItem->GetItemBaseInfo().Position;

	CHARACTER_TOTALINFO ChaInfo;
	OBJECTMGR->GetHero()->GetCharacterTotalInfo(&ChaInfo);
	for( WORD Stage = 0; Stage < ChaInfo.JobGrade - 1; Stage++)
	{
		int nIndex = pDlg->GetCurSelectedIdx(Stage);
		if( nIndex > -1 )
		{
			const ITEM* pSelectItem = pDlg->GetItem(Stage, nIndex);
			if( !pSelectItem ) break;

			msg.ClassStage[Stage] = (BYTE)pSelectItem->line + 1;
		}
		else
		{
			msg.ClassStage[Stage] = ChaInfo.Job[Stage+1];
		}
	}

	NETWORK->Send( &msg, sizeof(MSG_ITEM_CHANGE_CLASS) );									
}

// 100609 ONS ��æƮ +10�̻� ������ ť�� �����Ѵ�.
void CItemManager::EnchantNotifyAdd( void* pMsg )
{
	const MSG_NAME_DWORD3* pmsg = (MSG_NAME_DWORD3*)pMsg;
	const DWORD dwItemIdx		= pmsg->dwData1;
	const DWORD dwEnchantLevel	= pmsg->dwData2;
	const DWORD  bIsSuccessed	= pmsg->dwData3;
	const char* const pName		= pmsg->Name;

	if(  0 == dwItemIdx		||
		10 > dwEnchantLevel	||
		NULL == pName		)
	{
		return;
	}

	EnchantNotifyInfo Info;
	ZeroMemory(&Info, sizeof(EnchantNotifyInfo));

	Info.ItemIndex		= dwItemIdx;
	Info.EnchantLevel	= dwEnchantLevel;
	Info.IsSuccessed	= (BYTE)bIsSuccessed;
	SafeStrCpy( Info.Name, pName, MAX_NAME_LENGTH + 1 );

	m_EnchantNotifyQueue.push( Info );
}

WORD CItemManager::GetItemList(const char* pKeyWord, const char* pCategory1, const char* pCategory2, int nRareLevel, WORD wMinLevel, WORD wMaxLevel, WORD wNum, DWORD* pItemList)
{
	if(0==wNum || 0==pItemList)
		return 0;

	if(strlen(pKeyWord) < 4)
	{
		return 0;
	}

	CConsignmentDlg* pDlg = GAMEIN->GetConsignmentDlg();
	if(! pDlg)
		return 0;

	ITEM_INFO* pInfo;
	m_ItemInfoList.SetPositionHead();

	WORD wCount = 0;
	DWORD dwCategory1 = 0;
	DWORD dwCategory2 = 0;
	const WORD wMaxListDialogCount = 99;

	DWORD dwItemCategory1 = 0;
	DWORD dwItemCategory2 = 0;
	BOOL bOverCount = FALSE;
	while(NULL != (pInfo = m_ItemInfoList.GetData()))
	{
		if(wMaxListDialogCount < wCount)
		{
			return wMaxListDialogCount;
		}
		else if(wNum <= wCount)
		{
			bOverCount = TRUE;
		}

		if(FALSE==pInfo->Trade || FALSE==pInfo->Sell)
			continue;

		dwCategory1 = dwCategory2 = 0;
		dwItemCategory1 = dwItemCategory2 = 0;

		if(NULL != pCategory1)
			dwCategory1 = pDlg->GetCategory1Value(pCategory1);

		if(NULL != pCategory2)
			dwCategory2 = pDlg->GetCategory2Value(pCategory2);

		pDlg->GetCategoryValueByItemIndex(pInfo->ItemIdx, dwItemCategory1, dwItemCategory2);
		if(0!=dwCategory1 && dwItemCategory1!=dwCategory1)
			continue;

		if(0!=dwCategory2 && dwItemCategory2!=dwCategory2)
			continue;

		if(-1!=nRareLevel && nRareLevel!=pInfo->kind)
			continue;

		if(pInfo->LimitLevel<wMinLevel || wMaxLevel<pInfo->LimitLevel)
			continue;

		char keyword[256] = {0,};
		SafeStrCpy(keyword, pKeyWord, 256-1);
		if(NULL!=pKeyWord && NULL==strstr(_strupr(pInfo->ItemName), _strupr(keyword)))
			continue;

		if(FALSE == bOverCount)
			pItemList[wCount] = pInfo->ItemIdx;

		wCount++;

		DWORD dwColor = GetItemNameColor( *pInfo );
		pDlg->AddItemToKeywordList(pInfo->ItemName, dwColor);
	}

	return wCount;
}
