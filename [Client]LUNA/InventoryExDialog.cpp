#include "stdafx.h"
#include "InventoryExDialog.h"
#include "WindowIDEnum.h"
#include "Interface/cWindowManager.h"
#include "Interface/cIconGridDialog.h"
#include "Interface/cStatic.h"
#include "Interface/cPushupButton.h"
#include "ObjectStateManager.h"

#include "GameIn.h"

#include "ItemManager.h"
#include "ObjectManager.h"
#include "cSkillTreeManager.h"
#include "ChatManager.h"

#include "DealItem.h"
#include "cDivideBox.h"
#include "./Audio/MHAudioManager.h"

#include "DealDialog.h"

#include "./Input/UserInput.h"
// 080916 LUJ, ������ �ռ� â
#include "ComposeDialog.h"
#include "PKManager.h"
#include "cMsgBox.h"
#include "QuestManager.h"
#include "QuickSlotDlg.h"
#include "ProgressDialog.h"
#include "StreetStallManager.h"
#include "ShoutDialog.h"

// 080218 LUJ
#include "ApplyOptionDialog.h"

#include "StreetStallManager.h"
#include "ShoutDialog.h"

#include "ShowdownManager.h"
#include "MoveManager.h"
#include "./Interface/cScriptManager.h"

#include "NpcScriptManager.h"
#include "MHMap.h"
// 080228 LUJ, ��ȣ ������ ó��
#include "MixDialog.h"
#include "ReinforceDlg.h"
#include "EnchantDialog.h"
#include "../[CC]Header/GameResourceManager.h"
#include "../hseos/Farm/SHFarmManager.h"
#include "TutorialManager.h"
#include "../[cc]skill/client/manager/skillmanager.h"
#include "cChangeNameDialog.h"
#include "WearedExDialog.h"
#include "../hseos/Family/SHFamilyManager.h"

// 080414 LUJ, �ܾ� ��ȯ â
#include "BodyChangeDialog.h"

#include "FishingDialog.h"
#include "FishingManager.h"

#include "PetManager.h"
#include "PetResurrectionDialog.h"
#include "PetWearedDialog.h"
#include "./interface/cResourceManager.h"

#include "GuildTournamentMgr.h"

#include "cHousingMgr.h"

// 080917 KTH -- SiegeWarfareManager Include
#include "SiegeWarfareMgr.h"

//090423 pdy �Ͽ�¡â�������� 
#include "cHousingStoredIcon.h"

//090526 pdy �Ͽ�¡ ���湮 UI
#include "cHouseSearchDlg.h"

#include "cCookMgr.h"
#include "MiniMapDlg.h"
#include "Questquickviewdialog.h"
#include "QuestDialog.h"

#include "ChangeClassDlg.h"
#include "DissolveDialog.h"

CInventoryExDialog::CInventoryExDialog()
{
	m_type				= WT_INVENTORYDIALOG;
	m_pWearedDlg		= NULL;
	m_pMoneyEdit		= NULL;
	m_BtnPushstartTime		= 0;
	m_BtnPushDelayTime		= 500;
	m_bPushTabWithMouseOver = FALSE;

	ZeroMemory(
		m_pBackground,
		sizeof(m_pBackground));
	// 071204 LYW --- InventoryExDialog : ���� ���� ���� ��� �� ������ ������ �߰�.
	// ���� ���� ������ ����ϰ� ���� ��, �ٽ� �ѹ� ���� ���� ������ ����ϸ�, �޽����� ����.
	// �޽��� ó�� �ο���, Yes�� �����ϸ�, ��� ��û�� �����µ�, �̶� ������ ������ �ʿ��ϴ�.
	// ������, �������� �� ���Կ� ����ϰ� �κ��� ������ �ʰ� ����� ��찡 ����µ�, 
	// �̶� GetCurSelectedItem()���� ����� �� �����͸� �ѱ��� ���ϱ� ������ 
	// �� ���� ��� �� �ӽ� ������ �����͸� ���� �� ����Ѵ�.
	m_pQuickSelectedItem = NULL ;
}

CInventoryExDialog::~CInventoryExDialog()
{}

void CInventoryExDialog::Render()
{
	cDialog::RenderWindow();
	cDialog::RenderComponent();
	cTabDialog::RenderTabComponent();
}
void CInventoryExDialog::Add(cWindow * window)
{
	// 071210 LYW --- InventoryExDialog : �κ��丮 Ȯ�忡 ���� Ȯ��������� Ȱ��ȭ ���� ó��.
	if(window->GetType() == WT_PUSHUPBUTTON)
	{
		BYTE byCurIdx = 0 ;
		byCurIdx = curIdx1 ;

		AddTabBtn(curIdx1++, (cPushupButton * )window);

        if( byCurIdx >= 2 )
		{
			//window->SetActive(FALSE) ;
			((cPushupButton*)window)->SetActive(FALSE) ;
		}
	}
	else if(window->GetType() == WT_ICONGRIDDIALOG)
	{
		BYTE byCurIdx = 0 ;
		byCurIdx = curIdx2 ;

		AddTabSheet(curIdx2++, window);
		((cIconGridDialog*)window)->SetDragOverIconType( WT_ITEM );

		if( byCurIdx >= 2 )
		{
			//window->SetActive(FALSE) ;
			((cIconGridDialog*)window)->SetActive(FALSE) ;
		}
	}
	else 
		cDialog::Add(window);
}


DWORD CInventoryExDialog::ActionEvent(CMouse * mouseInfo)
{
	//������ �巡�����϶�.. �ٸ� ������ �̵��ؾ��Ѵ�.
	if( WINDOWMGR->IsDragWindow() && WINDOWMGR->GetDragDlg() )
	{
		if(WINDOWMGR->GetDragDlg()->GetType() == WT_ITEM)
		{
			DWORD we = WE_NULL;
			if(!m_bActive) return we;
			we = cDialog::ActionEvent(mouseInfo);

			BOOL bMouseOver = FALSE;
			for( int i = 0 ; i < m_bTabNum ; i++ )
			{
				m_ppPushupTabBtn[i]->ActionEvent( mouseInfo );
				
				if( we & WE_MOUSEOVER )
				if( m_ppPushupTabBtn[i]->IsActive() && m_ppPushupTabBtn[i]->PtInWindow( mouseInfo->GetMouseX(), mouseInfo->GetMouseY() ) )
				{
					bMouseOver = TRUE;
					
					if( m_bPushTabWithMouseOver )
					{
						if( i != m_bSelTabNum )
						{
							SelectTab(i);		//������Ű��(����â����..)
							m_bSelTabNum = i;							
						}
					}
					else
					{
						if( m_BtnPushstartTime == 0 )
							m_BtnPushstartTime = gCurTime;
						else if( gCurTime - m_BtnPushstartTime > m_BtnPushDelayTime )
							m_bPushTabWithMouseOver = TRUE;
					}				
				}
			}
			
			if( !bMouseOver )
				m_BtnPushstartTime = 0;
			
			we |= m_ppWindowTabSheet[m_bSelTabNum]->ActionEvent( mouseInfo );
			return we;
		}
	}

	m_BtnPushstartTime		= 0;
	m_bPushTabWithMouseOver = FALSE;
	return cTabDialog::ActionEvent( mouseInfo );	
}


WORD CInventoryExDialog::GetTabIndex( POSTYPE absPos )
{
	return ( absPos - TP_INVENTORY_START ) / TABCELL_INVENTORY_NUM;
}

void CInventoryExDialog::RefreshInvenItem()
{
	if(!HERO)
		return;

	BYTE TabNum = GetTabNum();
	for( BYTE i = 0 ; i < TabNum ; ++i )
	{
		cIconGridDialog* gridDlg = (cIconGridDialog*)GetTabSheet(i);

		for( WORD j = 0 ; j < gridDlg->GetCellNum() ; ++j )
		{
			CItem * pItem = (CItem *)gridDlg->GetIconForIdx(TP_INVENTORY_START+j);
			
			if(pItem)
				ITEMMGR->RefreshItem( pItem );
		}
	}

	// �������� ��� ����
	for( WORD j = eWearedItem_Weapon ; j < eWearedItem_Max ; ++j )
	{
		CItem * pItem = GetItemForPos( TP_WEAR_START + j );
		if(pItem)
			ITEMMGR->RefreshItem( pItem );
	}
}

BOOL CInventoryExDialog::AddItem(ITEMBASE * itemBase)
{
	if( itemBase->dwDBIdx == 0 )
	{
//		DEBUGMSG( 0, "Item DB idx == 0" );
		return FALSE;
	}

	CItem * newItem = ITEMMGR->MakeNewItem( itemBase, "AddItem" );
	if(newItem == NULL)
		return FALSE;

	// 071125 LYW --- InventoryExDialog : �ð� �������� ���, ���� �ð� ����.
	if( itemBase->nSealed == eITEM_TYPE_UNSEAL )
	{
		newItem->SetItemBaseInfo( *itemBase ) ;
	}
	
	return AddItem( newItem );
}

// ���� Dura�� ���� ���� �ش�.���ҽ� ��������.
BOOL CInventoryExDialog::AddItem(CItem* pItem)
{
	if(pItem == NULL)
	{
		ASSERT(pItem);
		return FALSE;
	}

	//�� ��ȯ
	ITEMMGR->RefreshItem( pItem );
	//add �ϴ� �����۸� refresh �ϸ� �ȴ�?

	// 071210 LYW --- InventoryExDialog : �Ʒ� �ҽ����� �������� ������ ��� �޾Ƽ� �����ϴ� ����,
	// �ѹ� �޾Ƽ� ��� ����ϵ��� ������.
	POSTYPE pos = 0 ;
	pos = pItem->GetPosition() ;
	BOOL bRes = FALSE;


	if(TP_INVENTORY_START <= pos && pos < (TP_INVENTORY_END + ( TABCELL_INVENTORY_NUM * HERO->Get_HeroExtendedInvenCount() )))
	{
		
		// �κ��丮 
		int tabIndex = GetTabIndex( pItem->GetPosition() );
		cIconGridDialog * dlg = (cIconGridDialog *)GetTabSheet(tabIndex);

//		WORD relPos = pItem->GetPosition()-INVENTORY_STARTPOSITION-CELLNUMPERPAGE*tabIndex;
		WORD relPos = pos-TP_INVENTORY_START-TABCELL_INVENTORY_NUM*tabIndex;
		CItem * item = (CItem * )dlg->GetIconForIdx(relPos);
		if(item)
		{
			ASSERT(0);
			return FALSE;
		}
		else
		{
			//return dlg->AddIcon(relPos, pItem);
			bRes = dlg->AddIcon(relPos, pItem);
			ITEMMGR->UpdateDlgCoolTime(pItem->GetItemIdx());	// 080326 NYJ --- ��Ÿ�Ӱ���

			// 100422 ShinJS --- �κ��丮 ������ ����� ����Ʈ ������ �����Ѵ�.
			GAMEIN->GetQuestQuickViewDialog()->ResetQuickViewDialog();
			GAMEIN->GetQuestDialog()->RefreshQuestList();
			QUESTMGR->ProcessNpcMark();
			return bRes;
		}
	}
	else if(TP_WEAR_START <= pos && pos < TP_WEAR_END)
	{
		// ����â
		//return m_pWearedDlg->AddItem(pos-TP_WEAR_START, pItem);
		bRes = m_pWearedDlg->AddItem(pos-TP_WEAR_START, pItem);
		ITEMMGR->UpdateDlgCoolTime(pItem->GetItemIdx());	// 080326 NYJ --- ��Ÿ�Ӱ���
		return bRes;
	}

    return FALSE;
}

BOOL CInventoryExDialog::DeleteItem(POSTYPE Pos,CItem** ppItem)
{
	BOOL bRet = FALSE;
//	if(INVENTORY_STARTPOSITION <= Pos && Pos < INVENTORY_ENDPOSITION)
	if(TP_INVENTORY_START <= Pos && Pos < (TP_INVENTORY_END + ( TABCELL_INVENTORY_NUM * HERO->Get_HeroExtendedInvenCount() )))
	{
		int TabIndex = GetTabIndex( Pos );
		cIconGridDialog * Dlg = (cIconGridDialog *)GetTabSheet(TabIndex);
		if(!Dlg->IsAddable(Pos - TP_INVENTORY_START-TABCELL_INVENTORY_NUM*TabIndex))
			bRet = Dlg->DeleteIcon(Pos-TP_INVENTORY_START-TABCELL_INVENTORY_NUM*TabIndex, (cIcon **)ppItem);
	}
//	else if(WEAR_STARTPOSITION <= Pos && Pos < WEAR_ENDPOSITION)
	else if(TP_WEAR_START <= Pos && Pos < TP_WEAR_END)
	{
//		if(!m_pWearedDlg->IsAddable(Pos - WEAR_STARTPOSITION))
//			return m_pWearedDlg->DeleteItem(Pos-WEAR_STARTPOSITION, (cIcon **)ppItem);
		if(!m_pWearedDlg->IsAddable(Pos - TP_WEAR_START))
			bRet = m_pWearedDlg->DeleteItem(Pos-TP_WEAR_START, (cIcon **)ppItem);
	}

	// 100422 ShinJS --- �κ��丮 ������ ����� ����Ʈ ������ �����Ѵ�.
	GAMEIN->GetQuestQuickViewDialog()->ResetQuickViewDialog();
	GAMEIN->GetQuestDialog()->RefreshQuestList();
	QUESTMGR->ProcessNpcMark();
    return bRet;
}

BOOL CInventoryExDialog::UpdateItemDurabilityAdd(POSTYPE absPos, DURTYPE dur)
{
//	if(INVENTORY_STARTPOSITION <= absPos && absPos < INVENTORY_ENDPOSITION)
	if(TP_INVENTORY_START <= absPos && absPos < (TP_INVENTORY_END + ( TABCELL_INVENTORY_NUM * HERO->Get_HeroExtendedInvenCount() )))
	{
		int TabIndex = GetTabIndex( absPos );
		cIconGridDialog * Dlg = (cIconGridDialog *)GetTabSheet(TabIndex);
//		WORD relPos = absPos-INVENTORY_STARTPOSITION-CELLNUMPERPAGE*TabIndex;
		WORD relPos = absPos-TP_INVENTORY_START-TABCELL_INVENTORY_NUM*TabIndex;
		CItem * item = (CItem * )Dlg->GetIconForIdx(relPos);
		item->SetDurability(item->GetDurability()+dur);
		
		// 100422 ShinJS --- �κ��丮 ������ ����� ����Ʈ ������ �����Ѵ�.
		GAMEIN->GetQuestQuickViewDialog()->ResetQuickViewDialog();
		GAMEIN->GetQuestDialog()->RefreshQuestList();
		QUESTMGR->ProcessNpcMark();
		return TRUE;
	}

    return FALSE;
}
CItem * CInventoryExDialog::GetItemForPos(POSTYPE absPos)
{
//	if(INVENTORY_STARTPOSITION <= absPos && absPos < INVENTORY_ENDPOSITION)
	if(TP_INVENTORY_START <= absPos && absPos < (TP_INVENTORY_END + ( TABCELL_INVENTORY_NUM * HERO->Get_HeroExtendedInvenCount() )))
	{
		int TabIndex = GetTabIndex( absPos );
		cIconGridDialog * Dlg = (cIconGridDialog *)GetTabSheet(TabIndex);
		return (CItem *)Dlg->GetIconForIdx(absPos-TP_INVENTORY_START-TABCELL_INVENTORY_NUM*TabIndex);
//		return (CItem *)Dlg->GetIconForIdx(absPos-INVENTORY_STARTPOSITION-CELLNUMPERPAGE*TabIndex);
	}
//	else if(WEAR_STARTPOSITION <= absPos && absPos < WEAR_ENDPOSITION)
	else if(TP_WEAR_START <= absPos && absPos < TP_WEAR_END)
	{
//		return (CItem *)m_pWearedDlg->GetIconForIdx(absPos-WEAR_STARTPOSITION);
		return (CItem *)m_pWearedDlg->GetIconForIdx(absPos-TP_WEAR_START);
	}

	return NULL;
}

void CInventoryExDialog::UseItem(CItem* pItem)
{
	ASSERT(pItem);

	CHero* hero = OBJECTMGR->GetHero();
	ASSERT( hero );
	
	if( WINDOWMGR->IsDragWindow() && WINDOWMGR->GetDragDlg() )
	if( WINDOWMGR->GetDragDlg()->GetType() == WT_ITEM)
		return;

	// 080822 LUJ, ��Ÿ���� �ú��� ������ ǥ����
	{
		DWORD coolTime = pItem->GetRemainedCoolTime();

		if( coolTime )
		{
			const DWORD oneSecond	= 1000;
			const DWORD oneMinute	= oneSecond * 60;
			const DWORD oneHour		= oneMinute * 60;
			
			char text[ MAX_PATH ]	= { 0 };
			char buffer[ MAX_PATH ] = { 0 };

			// 080820 LUJ, �� ���ϱ�
			{
				const DWORD hour = coolTime / oneHour;

				if( hour )
				{
					sprintf( buffer, CHATMGR->GetChatMsg( 1410 ), hour );
					strcat( text, buffer );

					coolTime = coolTime - hour * oneHour;
				}
			}

			// 080820 LUJ, �� ���ϱ�
			{
				const DWORD minute	= coolTime / oneMinute;

				if( minute )
				{
					sprintf( buffer, CHATMGR->GetChatMsg( 1411 ), minute );
					strcat( text, buffer );

					coolTime = coolTime - minute * oneMinute;
				}
			}

			// 080820 LUJ, �� ���ϱ�
			{
				const float second	= float( coolTime ) / oneSecond;

				if( second )
				{
					sprintf( buffer, "%0.1f%s", second, CHATMGR->GetChatMsg( 795 ) );
					strcat( text, buffer );
				}
			}

			CHATMGR->AddMsg(
				CTC_SYSMSG,
				CHATMGR->GetChatMsg( 1658 ),
				text );
			return;
		}
	}
//KES...
	if( m_bDisable )	return;		//�κ� �𽺿��̺��̸� ������ ��� �Ұ���

	if( pItem->GetItemInfo()->EquipType == eEquipType_Pet )
	{
		if( !HEROPET )
			return;

		GAMEIN->GetPetWearedDlg()->FakeMoveIcon( 0, 0, pItem );
		return;
	}	
//KES EXCHANGE 031002

//	
	if( hero->GetState() == eObjectState_Die )	//���� ���¿����� ������ ��� ���Ѵ�.
		return;

	// 080917 KTH -- ���������� ������ ������ �˻�
   	if( SIEGEWARFAREMGR->IsSiegeWarfareZone() )
   	{
   		// �������� ������ ��ų �Ǵ� ��� ���� ������ �̶�� ��� ���и� �˸���.
   		if( SIEGEWARFAREMGR->IsFobiddenItem(pItem->GetItemIdx()) )
   		{
   			// ���� �������� ����� �� ���� �������Դϴ�. ��� �ý��� �޽����� ��� �Ͽ� �ش�.
   			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1691 ) );
   			return;
   		}
   	}

	if( HOUSINGMGR->IsHousingMap() ) 
	{
		//090527 pdy �Ͽ�¡ �ý��۸޼��� �Ͽ�¡ ���϶� �����ൿ [�����ۻ��]
		if( !HOUSINGMGR->CanUseItemFromHousingMap(pItem) )
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1867 ) );	//1867	"�� ���������� ����Ͻ� �� �����ϴ�."
			return;
		}

		//090527 pdy �Ͽ�¡ �ý��۸޼��� �ٹ̱� ����Ͻ� ���ѻ��� [�����ۻ��]
		if( hero->GetState() == eObjectState_Housing )
		if( !HOUSINGMGR->CanUseItemFromDecorationMode(pItem) )
		{
			CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg( 1874 ) );	//1874	"�ٹ̱� ��忡�� ����Ͻ� �� �����ϴ�."
			return;
		}
	}

	// 080411 LUJ, ������ ��� ������ üũ�ϵ��� �Ѵ�. ����: ��� ������ ���� �������� 0���� �����Ǿ� �ִ�
	{
		const ITEM_INFO* info = pItem->GetItemInfo();

		if( info				&&
			info->LimitLevel	&&
			info->LimitLevel > hero->GetLevel() )
		{
			return;
		}
	}

	// 090422 ShinJS --- Ż�� ����ó���� ���� �߰�
	if( pItem->GetItemInfo()->SupplyType != ITEM_KIND_PET &&
		pItem->GetItemInfo()->SupplyType != ITEM_KIND_SUMMON_VEHICLE &&
		pItem->IsLocked() ) 
		return;

	// desc_hseos_����ý���_01
	// S ����ý��� �߰� added by hseos 2007.09.18
	// ..���� ��� ���� �� ����
	if( g_csFarmManager.CLI_GetIngCharMotion() )
		return;
	// E ����ý��� �߰� added by hseos 2007.09.18

	if( !pItem->GetItemInfo()->Battle && HERO->GetObjectBattleState() == eObjectBattleState_Battle )
		return;

	if( !pItem->GetItemInfo()->Peace && HERO->GetObjectBattleState() == eObjectBattleState_Peace )
		return;

	if( !pItem->GetItemInfo()->Move && HERO->GetBaseMoveInfo()->bMoving )
		return;

	if( !pItem->GetItemInfo()->Stop && !HERO->GetBaseMoveInfo()->bMoving )
		return;

	if( pItem->GetItemInfo()->SupplyType == ITEM_KIND_PET_NORMAL_RESURRECTION ||
		pItem->GetItemInfo()->SupplyType == ITEM_KIND_PET_SPECIAL_RESURRECTION )
	{
		CPetResurrectionDialog* window = GAMEIN->GetPetResurrectionDlg();

		if(		window &&
			!	window->IsActive() )
		{
			window->SetUsedItem( pItem );
			window->SetActive( TRUE );
		}

		return;
	}

    if(g_CGameResourceManager.IsApplyOptionItem(pItem->GetItemIdx()))
	{
		CApplyOptionDialog* window = ( CApplyOptionDialog* )WINDOWMGR->GetWindowForID( APPLY_OPTION_DIALOG );

		if(		window &&
			!	window->IsActive() )
		{
			window->SetSourceItem( *pItem );
			window->SetActive( TRUE );
		}

		return;
	}
	// 080228 LUJ, ���� ��ȣ �������� ��� ���� â ǥ��
	else if( ITEMMGR->IsMixProtectionItem( pItem->GetItemBaseInfo() ) )
	{
		CMixDialog* window = ( CMixDialog* )WINDOWMGR->GetWindowForID( ITMD_MIXDLG );

		if(		window &&
			!	window->IsActive() )
		{
			window->SetActive( TRUE );

			// 081209 LUJ, â�� ���������� ���� ��쿡�� ��ȣ ��带 �����Ѵ�
			if( window->IsActive() )
			{
				window->SetProtectedActive( *pItem );
			}
		}

		return;
	}
	// 080925 LUJ, ���� ���� ���������� ���� ����
	else if( ITEMMGR->GetMixSupportScript( pItem->GetItemIdx() ) )
	{
		CMixDialog* window = ( CMixDialog* )WINDOWMGR->GetWindowForID( ITMD_MIXDLG );
	
		if(		window &&
			!	window->IsActive() )
		{
			window->SetActive( TRUE );

			// 081209 LUJ, â�� ���������� ���� ��쿡�� ��ȣ ��带 �����Ѵ�
			if( window->IsActive() )
			{
				pItem->SetLock( TRUE );
				window->SetSupportItem( pItem->GetItemBaseInfo() );
			}
		}
	}
	else if(g_CGameResourceManager.IsReinforceProtectionItem(pItem->GetItemBaseInfo()))
	{
		CReinforceDlg* window = ( CReinforceDlg* )WINDOWMGR->GetWindowForID( ITR_REINFORCEDLG );

		if(	window )
		{
			window->SetActive( TRUE );

			// 081209 LUJ, â�� ���������� ���� ��쿡�� ��ȣ ��带 �����Ѵ�
			if( window->IsActive() )
			{
				window->SetProtectedActive( *pItem );
			}
		}

		return;
	}
	else if(g_CGameResourceManager.GetReinforceSupportScript(pItem->GetItemIdx()))
	{
		CReinforceDlg* dialog = ( CReinforceDlg* )WINDOWMGR->GetWindowForID( ITR_REINFORCEDLG );

		if(		dialog &&
			!	dialog->IsActive() )
		{
			dialog->SetActive( TRUE );

			// 081209 LUJ, â�� ���������� ���� ��쿡�� ���� ��带 �����Ѵ�
			if( dialog->IsActive() )
			{
				pItem->SetLock( TRUE );
				dialog->SetSupportItem( pItem->GetItemBaseInfo() );
			}
		}

		return;
	}
	else if( GAMERESRCMNGR->IsComposeKeyItem( pItem->GetItemIdx() ) )
	{
		CComposeDialog* dialog = GAMEIN->GetComposeDialog();

		if(		dialog &&
			!	dialog->IsActive() )
		{
			pItem->SetLock( TRUE );

			dialog->SetKeyItem( pItem->GetItemBaseInfo() );
			dialog->SetActive( TRUE );
		}

		return;
	}

	// 080228 LUJ, ��æƮ ��ȣ �������� ��� ��æƮ â ǥ��
	// 080320 LUJ, ��æƮ ���� ��ȣ/��ü ��ȣ ���¿� ���� �ý��� �޽��� �и�
	else
	{
		const EnchantProtection& protection	= g_CGameResourceManager.GetEnchantProtection();
		const EnchantProtection::ItemMap::const_iterator it	= protection.mItemMap.find(
			pItem->GetItemIdx());
		CEnchantDialog* window = (CEnchantDialog*)WINDOWMGR->GetWindowForID(
			ITD_UPGRADEDLG);

		// 080328 LUJ, ��æƮ ��ȣ ������ �� ������ ��
		if(	window &&
			protection.mItemMap.end() != it )
		{
			window->SetActive( TRUE );

			if( window->IsActive() )
			{
				CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1454 ) );

				window->SetProtectedActive( *pItem );
				return;
			}
		}		
	}
      
	// 080414 LUJ, �ܾ� ��ȯ �������� ��� �ش� â ǥ��
	{
		typedef CItemManager::BodyChangeScriptListMap BodyChangeScriptListMap;

		const BodyChangeScriptListMap&			scriptListMap	= ITEMMGR->GetBodyChangeScriptListMap();
		BodyChangeScriptListMap::const_iterator	it				= scriptListMap.find(  pItem->GetItemIdx() );

		CBodyChangeDialog*	window = ( CBodyChangeDialog* )WINDOWMGR->GetWindowForID( BODY_CHANGE_DIALOG );
    
		cDialog* stallShopDialog	= WINDOWMGR->GetWindowForID( SSI_STALLDLG );	//����
 
		if(	window &&
			scriptListMap.end() != it )
		{
			if( !stallShopDialog->IsActive() )		//100525 ���λ����� �����ִٸ� �ܾ� ��ȯ������ ������� �ʵ��� �Ѵ�.
			{
				window->SetActive( TRUE, *pItem );

				SetActive( FALSE );
				ITEMMGR->SetDisableDialog( TRUE, eItemTable_Inventory );
			}
			else
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1078 ) );
			}
			return;
		}
	}

	switch( pItem->GetItemInfo()->SupplyType )
	{
	case ITEM_KIND_PET_SKILL:					
	case ITEM_KIND_PET_COSTUME:				
	case ITEM_KIND_PET_EQUIP:					
	case ITEM_KIND_PET_EGG:					
	case ITEM_KIND_PET_FOOD:				
	case ITEM_KIND_PET_HP_RECOVER:			
	case ITEM_KIND_PET_MP_RECOVER:			
		{
			if( !HEROPET )
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(360));
				return;
			}
		}
		break;
	case ITEM_KIND_PET_EVOLUTION_PYHSIC_TYPE:	
	case ITEM_KIND_PET_EVOLUTION_MAGIC_TYPE:	
	case ITEM_KIND_PET_EVOLUTION_SUPPORT_TYPE:
		{
			if( !HEROPET )
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(360));
				return;
			}
			if( HEROPET->GetLevel() != PETMGR->GetGradeUpLevel( HEROPET->GetGrade() ) )
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(789));
				return;
			}

			if( HEROPET->GetType() != ePetType_Basic &&
				HEROPET->GetType() != pItem->GetItemInfo()->SupplyType - ITEM_KIND_PET_EVOLUTION_PYHSIC_TYPE + 1 )
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1577));
				return;
			}
		}
		break;
	}

	switch( pItem->GetItemInfo()->Category )
	{
	case eItemCategory_Equip:
		{
			if( OBJECTSTATEMGR->CheckAvailableState(hero,eCAS_EQUIPITEM) == FALSE )
			{
				CAction act;
				act.InitAction(eActionKind_EquipItem,(void*)pItem->GetDBIdx(),0,NULL);
				hero->SetNextAction(&act);
				return;
			}

			// ���� ������	
			switch( GetTableIdxPosition( pItem->GetPosition() ) )
			{
			case eItemTable_Inventory:
				{
					ITEM_INFO * pInfo = ITEMMGR->GetItemInfo(pItem->GetItemIdx()) ;
					if(!pInfo) return ;

					BYTE byEquipResult = ITEMMGR->CanEquip(pItem->GetItemIdx()) ;
					if( byEquipResult == eEquip_Able )
					{
						EWEARED_ITEM EquipToPos = pInfo->EquipSlot;
						CItem * pToItem = GetItemForPos(pInfo->EquipSlot + TP_WEAR_START);

						switch(EquipToPos)
						{
						case eWearedItem_Ring1:
							{
								if(0 == GetItemForPos(eWearedItem_Ring2 + TP_WEAR_START))
								{
									pToItem = GetItemForPos(eWearedItem_Ring2 + TP_WEAR_START );
									EquipToPos = eWearedItem_Ring2;
								}

								break;
							}
						case eWearedItem_Earring1:
							{
								if( !GetItemForPos(eWearedItem_Earring2 + TP_WEAR_START))
								{
									pToItem = GetItemForPos(eWearedItem_Earring2 + TP_WEAR_START);
									EquipToPos = eWearedItem_Earring2;
								}
								break;
							}
						case eWearedItem_Shield:
							{
								if( hero->GetWeaponAniType() == eWeaponAnimationType_TwoHand ||
									hero->GetWeaponAniType() == eWeaponAnimationType_Staff ||
									hero->GetWeaponAniType() == eWeaponAnimationType_Bow ||
									hero->GetWeaponAniType() == eWeaponAnimationType_Gun ||
									// 090521 ONS �ű����� ���� Ÿ�� �Ӽ� �߰�
									hero->GetWeaponAniType() == eWeaponAnimationType_BothDagger )
								{
									CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(772) );
									return;
								}
								break;
							}
						}
						
						FakeGeneralItemMove(EquipToPos + TP_WEAR_START, pItem, pToItem);
					}
					else
					{
						// 090114 LYW --- InventoryExDialog : ������ ���� ���� üũ �Լ� ������ ���� ó��.
						switch( byEquipResult )
						{
						case eEquip_RaceFail :			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(823), pInfo->ItemName) ;	break ;
						case eEquip_GenderFail :		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(824), pInfo->ItemName) ;	break ;
						case eEquip_LevelFail :			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(825), pInfo->ItemName) ;	break ;
						case eEquip_FishingLevelFail :	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1530)) ;					break ;
						default :						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(73)) ;						break ;
						}
					}
				}
				break;
			case eItemTable_Weared:
				{
					POSTYPE emptyPos;
					if( GetBlankPositionRestrictRef( emptyPos ) )
					{
						CItem * pToItem = GetItemForPos( emptyPos );
						FakeGeneralItemMove(emptyPos, pItem, pToItem);
					}
				}
				break;
			}
		}
		break;
	case eItemCategory_Expend:
		{
			// desc_hseos_����ý���_01
			// S ����ý��� �߰� added by hseos 2007.05.14	2007.08.21
			// ..���� ������ �Ϻδ� Ÿ���� �����ϰ� ����ؾ� �ϱ� ������ ������ ���� �ٸ���. ���� �߰�
			const ITEM_INFO * pInfo = ITEMMGR->GetItemInfo(pItem->GetItemIdx());

			if( ! pInfo )
			{
				return;
			}

			// 100224 ONS �Ҹ� �����ۿ��� �������� ó�� �߰�.
			if(pInfo->LimitGender)
			{
				if(pInfo->LimitGender != HERO->GetGender()+1)
				{
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(2036), pInfo->ItemName) ;
					return ;
				}
			}

			switch( pInfo->SupplyType )
			{
			case ITEM_KIND_BUFF_ITEMS :
				{
					// �����ʸ�Ʈ�� ������ ���Ұ�.
					if(MAP->GetMapNum()==GTMAPNUM)
					{
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(91)) ;
						return;
					}

					// 071224 LYW --- InventoryExDialog : ��Ƽ���̸�, ��� ���Ѵٴ� ���� �޽��� ����� �Ѵ�.
					if( HERO->GetPartyIdx() != 0 )
					{
						const ITEM_INFO* pGetItemInfo = pItem->GetItemInfo() ;

						if( !pGetItemInfo )
						{
							MessageBox(NULL, "Failed to receive iinfo", "Use item", MB_OK) ;
							return ;
						}

						if( !SKILLMGR->IsUseableBuff_InParty(pGetItemInfo->SupplyValue) )
						{
							WINDOWMGR->MsgBox( MBI_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1252) ) ;
							return ;
						}
					}

					if( HERO->GetState()==eObjectState_Fishing || HERO->GetState()==eObjectState_FishingResult )
					{
						WINDOWMGR->MsgBox( MBI_CANTUSE_MAPMOVESCROLL, MBT_OK, CHATMGR->GetChatMsg(789) ) ;
						return ;
					}

					Set_QuickSelectedItem(pItem) ;

					if( ITEMMGR->IsUsingItem(pItem) )
					{
						WINDOWMGR->MsgBox( MBI_FORCE_USEBUFFITEM_AREYOUSURE, MBT_YESNO, CHATMGR->GetChatMsg(1224) ) ;

						// 080728 LUJ, �κ��丮�� ����� �� ������ ��ٴ�
						SetDisable( TRUE );
						return ;
					}

					ASSERT(pInfo) ;

					char tempBuf[128] = {0, } ;
					char newItemName[128] = {0, } ;

					const char* pPersent = NULL ;

					pPersent = strchr( pInfo->ItemName, '%' ) ;
					if( !pPersent )
					{
						WINDOWMGR->MsgBox( MBI_USEBUFFITEM_AREYOUSURE, MBT_YESNO, CHATMGR->GetChatMsg(1244), pInfo->ItemName ) ;

						// 080728 LUJ, �κ��丮�� ����� �� ������ ��ٴ�
						SetDisable( TRUE );
					}
					else
					{
						strncpy(tempBuf, pInfo->ItemName, strlen(pInfo->ItemName) - strlen(pPersent)) ;

						sprintf(newItemName, "%s%c", tempBuf, '%') ;
						strcat(newItemName, pPersent) ;

						memset(tempBuf, 0, sizeof(tempBuf)) ;
						sprintf(tempBuf, CHATMGR->GetChatMsg(1244), newItemName) ;

						WINDOWMGR->MsgBox( MBI_USEBUFFITEM_AREYOUSURE, MBT_YESNO, tempBuf ) ;

						// 080728 LUJ, �κ��丮�� ����� �� ������ ��ٴ�
						SetDisable( TRUE );
					}
				}
				return ;

			case ITEM_KIND_FOOD:
				{
					Set_QuickSelectedItem(pItem) ;
					ITEMMGR->UseItem_ItemMallItem(pItem, HERO) ;
					return;
				}
				return;

			case ITEM_KIND_GTBUFF_ITEMS:
				{
					CObject* pObject = OBJECTMGR->GetSelectedObject();
					if(!pObject)
					{
						WINDOWMGR->MsgBox( MBI_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1037)) ;
						return;
					}
					
					if(pObject->GetObjectKind() != eObjectKind_Player)
					{
						WINDOWMGR->MsgBox( MBI_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1043)) ;
						return;
					}

					// 071224 LYW --- InventoryExDialog : ��Ƽ���̸�, ��� ���Ѵٴ� ���� �޽��� ����� �Ѵ�.
					if( HERO->GetPartyIdx() != 0 )
					{
						const ITEM_INFO* pGetItemInfo = pItem->GetItemInfo() ;

						if( !pGetItemInfo )
						{
							MessageBox(NULL, "Failed to receive iinfo", "Use item", MB_OK) ;
							return ;
						}

						if( !SKILLMGR->IsUseableBuff_InParty(pGetItemInfo->SupplyValue) )
						{
							WINDOWMGR->MsgBox( MBI_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1252) ) ;
							return ;
						}
					}

					if( HERO->GetState()==eObjectState_Fishing || HERO->GetState()==eObjectState_FishingResult )
					{
						WINDOWMGR->MsgBox( MBI_CANTUSE_MAPMOVESCROLL, MBT_OK, CHATMGR->GetChatMsg(789) ) ;
						return ;
					}

					Set_QuickSelectedItem(pItem) ;
					ITEMMGR->UseItem_ItemMallItem(pItem, OBJECTMGR->GetSelectedObject()) ;
					return;
				}
				return;

			// 100511 ONS ���� �ʱ�ȭ ������ �߰�
			case ITEM_KIND_CHANGE_CLASS:
				{
					if( HERO->GetState() != eObjectState_None )
					{
						WINDOWMGR->MsgBox( MBI_CANTUSE_MAPMOVESCROLL, MBT_OK, CHATMGR->GetChatMsg(789) ) ;
						return ;
					}

					// 080124 KTH -- ������ ����϶����� ����� �� �����ϴ�.
					if( HERO->IsPKMode() )
					{
						WINDOWMGR->MsgBox( MBI_CANTUSE_MAPMOVESCROLL, MBT_OK, CHATMGR->GetChatMsg(1263) ) ;
						return;
					}

					{
						const ITEMBASE* weaponItem = ITEMMGR->GetItemInfoAbsIn( HERO, TP_WEAR_START + eWearedItem_Weapon );
						const ITEMBASE* shieldItem = ITEMMGR->GetItemInfoAbsIn( HERO, TP_WEAR_START + eWearedItem_Shield );
						
						const ITEM_INFO* weaponItemInfo	= ( weaponItem ? ITEMMGR->GetItemInfo( weaponItem->wIconIdx ) : 0 );
						const ITEM_INFO* shieldItemInfo	= ( shieldItem ? ITEMMGR->GetItemInfo( shieldItem->wIconIdx ) : 0 );

						if( weaponItemInfo &&
							shieldItemInfo &&
							weaponItemInfo->WeaponType == shieldItemInfo->WeaponType )
						{
							// 080714 LUJ, ���ҽ��� �߸� ������Ʈ�Ǿ� �޽��� ��ȣ�� ����
							CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1590 ) );
							return;
						}
					}

					pItem->SetLock(TRUE) ;
					Set_QuickSelectedItem(pItem) ;

					CChangeClassDlg* pDlg = GAMEIN->GetChangeClassDlg();
					if(!pDlg) return;

					pDlg->SetDisable( FALSE );
					pDlg->SetActive( TRUE );
				}
				return;

			// 071203 LYW --- InventoryExDialog : ���� �ʱ�ȭ ������ �߰�.
			case ITEM_KIND_RESET_STATS :
				{
					if( HERO->GetState() != eObjectState_None )
					{
						WINDOWMGR->MsgBox( MBI_CANTUSE_MAPMOVESCROLL, MBT_OK, CHATMGR->GetChatMsg(789) ) ;
						return ;
					}

					// 080124 KTH -- ������ ����϶����� ����� �� �����ϴ�.
					if( HERO->IsPKMode() )
					{
						WINDOWMGR->MsgBox( MBI_CANTUSE_MAPMOVESCROLL, MBT_OK, CHATMGR->GetChatMsg(1263) ) ;
						return;
					}

					pItem->SetLock(TRUE) ;
					Set_QuickSelectedItem(pItem) ;
					WINDOWMGR->MsgBox( MBI_RESETSTATS_AREYOUSURE, MBT_YESNO, CHATMGR->GetChatMsg(1223) ) ;
					return ;
				}
				return ;

			// 071203 LYW --- InventoryExDialog : ��ų �ʱ�ȭ ������ �߰�.
			case ITEM_KIND_RESET_SKILL :
				{
					if( HERO->GetState() != eObjectState_None )
					{
						WINDOWMGR->MsgBox( MBI_CANTUSE_MAPMOVESCROLL, MBT_OK, CHATMGR->GetChatMsg(789) ) ;
						return ;
					}
					
					// 080124 KTH -- ������ ����϶����� ����� �� �����ϴ�.
					if( HERO->IsPKMode() )
					{
						WINDOWMGR->MsgBox( MBI_CANTUSE_MAPMOVESCROLL, MBT_OK, CHATMGR->GetChatMsg(1263) ) ;
						return;
					}

					// 080703 LUJ, �̵��� ��� ������ ���·� ��ų �ʱ�ȭ�� �� ���, �ʿ��� ��ų��
					//				���µ��� �̵��� ���°� �ȴ�. �� ������ �̵����� ��쿡�� ��ų �ʱ�ȭ��
					//				�� �� ����
					{
						const ITEMBASE* weaponItem = ITEMMGR->GetItemInfoAbsIn( HERO, TP_WEAR_START + eWearedItem_Weapon );
						const ITEMBASE* shieldItem = ITEMMGR->GetItemInfoAbsIn( HERO, TP_WEAR_START + eWearedItem_Shield );
						
						const ITEM_INFO* weaponItemInfo	= ( weaponItem ? ITEMMGR->GetItemInfo( weaponItem->wIconIdx ) : 0 );
						const ITEM_INFO* shieldItemInfo	= ( shieldItem ? ITEMMGR->GetItemInfo( shieldItem->wIconIdx ) : 0 );

						if( weaponItemInfo &&
							shieldItemInfo &&
							weaponItemInfo->WeaponType == shieldItemInfo->WeaponType )
						{
							// 080714 LUJ, ���ҽ��� �߸� ������Ʈ�Ǿ� �޽��� ��ȣ�� ����
							CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1590 ) );
							return;
						}
					}

					pItem->SetLock(TRUE) ;
					Set_QuickSelectedItem(pItem) ;
					WINDOWMGR->MsgBox( MBI_RESETSKILL_AREYOUSURE, MBT_YESNO, CHATMGR->GetChatMsg(1222) ) ;
					return ;
				}
				return ;

			// 080312 NYJ --- ��ü��ġ��(����Ȯ����) ������
			case ITEM_KIND_WORLDSHOUT:
				{
					ITEMMGR->UseItem_WorldShout(pItem);
				}
				return;

			case ITEM_KIND_FISHING_BAIT:
				{
					CFishingDialog*	window = GAMEIN->GetFishingDlg();

					if(	window && !FISHINGMGR->IsActive())
					{
						//  ���� â�� �������¸� ����â ����
						window->SetActive( TRUE );
						window->ChangeBaitItem(pItem);
					}
					else if( window && FISHINGMGR->IsActive())
					{
						// ���� â�� �������¸� �̳���� or ����
						window->ChangeBaitItem(pItem);
					}
				}
				return;

			case ITEM_KIND_FISHING_FISH:
				{
					CHATMGR->AddMsg( CTC_EXPENDEDITEM, CHATMGR->GetChatMsg(772));
					return;
				}
				return;

			case ITEM_KIND_RESET_REGIDENT_REGIST :
				{
					if( HERO->GetState() != eObjectState_None )
					{
						WINDOWMGR->MsgBox( MBI_CANTUSE_MAPMOVESCROLL, MBT_OK, CHATMGR->GetChatMsg(789) ) ;
						return ;
					}
					
					// 080124 KTH -- ������ ����϶����� ����� �� �����ϴ�.
					if( HERO->IsPKMode() )
					{
						WINDOWMGR->MsgBox( MBI_CANTUSE_MAPMOVESCROLL, MBT_OK, CHATMGR->GetChatMsg(1263) ) ;
						return;
					}

					pItem->SetLock(TRUE) ;
					Set_QuickSelectedItem(pItem) ;
					WINDOWMGR->MsgBox( MBI_RESET_REGIDENTREGIST, MBT_YESNO, CHATMGR->GetChatMsg(1244), pItem->GetItemInfo()->ItemName) ;
					return ;
				}
				return ;

			// 071201 LYW --- InventoryExDialog : �� �̵� �ֹ��� ó�� �߰�.
			case ITEM_KIND_MAP_MOVE_SCROLL :
				{
					if( HERO->GetState() != eObjectState_None || HERO->IsPKMode() )
					{
						WINDOWMGR->MsgBox( MBI_CANTUSE_MAPMOVESCROLL, MBT_OK, CHATMGR->GetChatMsg(789) ) ;
						return ;
					}

					ITEMMGR->UseItem_MapMoveScroll(pItem) ;
				}
				return ;

			// 071206 LYW --- InventoryExDialog : â�� Ȯ�� �ֹ��� ó�� �߰�.
			case ITEM_KIND_EXTEND_STORAGE :
				{
					BYTE byNum = 0 ;
					byNum = HERO->GetStorageNum() ;

					if( 2 > byNum || byNum >= MAX_STORAGELIST_NUM )
					{
						WINDOWMGR->MsgBox( MBI_RESETSKILL_AREYOUSURE, MBT_OK, CHATMGR->GetChatMsg(1243) ) ;
						return ;
					}

					ITEMMGR->UseItem_ExtendStorage(pItem) ;
				}
				return ;

			// 071210 LYW --- InventoryExDialog : �κ��丮 Ȯ�� �ֹ��� ó�� �߰�.
			case ITEM_KIND_EXTEND_INVEN :
				{
					// HERO�� �κ��丮 ������ ���� �´�.
					int nInvenCount = HERO->Get_HeroExtendedInvenCount() ;

					if( HERO->GetState() != eObjectState_None )
					{
						WINDOWMGR->MsgBox( MBI_CANTUSE_MAPMOVESCROLL, MBT_OK, CHATMGR->GetChatMsg(789) ) ;
						return ;
					}

					if( nInvenCount >= 2)
					{
						WINDOWMGR->MsgBox( MBI_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1248) ) ;
						return ;
					}
					else
					{
						pItem->SetLock(TRUE) ;
						Set_QuickSelectedItem(pItem) ;

						WINDOWMGR->MsgBox( MBI_EXTENDINVEN_AREYOUSURE, MBT_YESNO, CHATMGR->GetChatMsg(1247) ) ;
						return ;
					}
				}
				return ;

			// 071228 LYW --- InventoryExDialog : �̸� ���� ������ ��� ó�� �߰�.
			case ITEM_KIND_CHANGE_NAME :
				{
					if( HERO->GetState() != eObjectState_None )
					{
						WINDOWMGR->MsgBox( MBI_NOTICE, MBT_OK, CHATMGR->GetChatMsg(789) ) ;
						return ;
					}

					if( HERO->GetPartyIdx() != 0 )
					{
						WINDOWMGR->MsgBox( MBI_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1258) ) ;
						return ;
					}

					if( HERO->GetGuildIdx() != 0 )
					{
						WINDOWMGR->MsgBox( MBI_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1257) ) ;
						return ;
					}

					CSHFamily* pFamily = NULL ;
					pFamily = HERO->GetFamily() ;

					if( pFamily )
					{
						if( pFamily->Get()->nMasterID != 0 )
						{
							WINDOWMGR->MsgBox( MBI_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1259) ) ;
							return ;
						}
					}

					pItem->SetLock(TRUE) ;
					Set_QuickSelectedItem(pItem) ;

					cChangeNameDialog* pChangeNameDlg = NULL ;
					pChangeNameDlg = GAMEIN->GetChangeNameDlg() ;

					if( !pChangeNameDlg ) return ;

					pChangeNameDlg->Clear_NameBox() ;
					pChangeNameDlg->SetActive(TRUE) ;
				}
				return ;

				// ..���۹� �ɱ�
			case ITEM_KIND_FARM_PLANT:
				g_csFarmManager.CLI_RequestPlantCropInGarden(pItem->GetPosition(), pItem->GetItemIdx());
				return;
			// ..���۹� ��� �ֱ�
			case ITEM_KIND_FARM_MANURE:
				g_csFarmManager.CLI_RequestManureCropInGarden(pItem->GetPosition(), pItem->GetItemIdx());
				return;
			// ..���۹� ��Ȯ
			case ITEM_KIND_FARM_HARVEST:
				g_csFarmManager.CLI_RequestHarvestropInGarden(pItem->GetPosition(), pItem->GetItemIdx());
				return;
			// 080421 KTH 
			case ITEM_KIND_ANIMAL:
				//if( g_csFarmManager.CLI_GetTargetObj()->GetInfo()->nKind == CSHFarmManager::RENDER_BASIC_ANIMAL_CAGE_SLOT_KIND )
				{
					pItem->SetLock(TRUE) ;
					Set_QuickSelectedItem(pItem) ;
					WINDOWMGR->MsgBox( MBI_FARM_ANIMAL_INSTALL, MBT_YESNO, CHATMGR->GetChatMsg( 1571 ) );
					//g_csFarmManager.CLI_RequestInstallAnimalInAnimalCage(pItem->GetPosition(), pItem->GetItemIdx());
				}
				return;

			// 080415 KTH ---
			// ..���� ��� �ֱ�
			case ITEM_KIND_ANIMAL_FOOD:
				g_csFarmManager.CLI_RequestFeedAnimalInCage(pItem->GetPosition(), pItem->GetItemIdx());
				return;
			//080515 KTH
			// ..���� û�� ������
			case ITEM_KIND_ANIMAL_CLEANER:
				g_csFarmManager.CLI_RequestCleaningAnimalInCage(pItem->GetPosition(), pItem->GetItemIdx());
				return;
			case ITEM_KIND_OPENSTORAGE_BYITEM : 
				{
					cDialog* storageDialog = WINDOWMGR->GetWindowForID( PYO_STORAGEDLG );
	
					if( ! storageDialog )
					{
						return;
					}
					else if( storageDialog->IsActive() )
					{
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1552 ) );
						return;
					}
					else if( ! HERO->GetStorageNum() )
					{
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1551 ) );
						return;
					}
					// 090403 ONS ����,����,��ȭ,��æƮ ���̾�αװ� Ȱ��ȭ �Ǿ� �ִ� ���¿��� â����ȯ ������ ��� �Ұ�
					else
					{
						cDialog* pMixDialog				= WINDOWMGR->GetWindowForID( ITMD_MIXDLG );			// ����
						cDialog* pDissolutioniDialog	= WINDOWMGR->GetWindowForID( DIS_DISSOLUTIONDLG );	// ����
						cDialog* pReinforceDialog		= WINDOWMGR->GetWindowForID( ITR_REINFORCEDLG );	// ��ȭ
						cDialog* pUpgradeDialog			= WINDOWMGR->GetWindowForID( ITD_UPGRADEDLG );		//��þƮ

						if( !pMixDialog				|| 
							!pDissolutioniDialog	|| 
							!pReinforceDialog		|| 
							!pUpgradeDialog	) 
						{
								return;
						}

						if(	pMixDialog->IsActive()			|| 
							pDissolutioniDialog->IsActive() || 
							pReinforceDialog->IsActive()	|| 
							pUpgradeDialog->IsActive()		)
						{
							CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1080 ) );
							return;
						}
					}
				}	
				break;
			case ITEM_KIND_GET_SKILLPOINT :
				{
					ITEMMGR->GetSkillPointByItem(pItem) ;
				}
				return ;

				// 080826 LYW --- InventoryExDialog : ���ͽõ� ��� �߰�.
   			case ITEM_KIND_SIEGEWARFARE_WATERSEED :
   				{
   					// ������ ��� ��û�� ������.
   					ITEMMGR->UseItem_WarterSeed(pItem) ;
   					return ;
   				}
   				break ;
			case ITEM_KIND_FURNITURE :
				{
					//090527 pdy �Ͽ�¡ �ý��۸޼��� ITEM_KIND_FURNITURE�� ���Ұ��� [ITEM_KIND_FURNITURE]
					CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1906) );	//1906	"������������ �κ��丮���� ��� �Ұ���"
					return;

				}
				break ;

			//090518 pdy �Ͽ�¡ ���͸��� ��ü 
			case ITEM_KIND_FURNITURE_WALLPAPER :
			case ITEM_KIND_FURNITURE_FLOORPAPER :
			case ITEM_KIND_FURNITURE_CEILINGPAPER :
				{
					//090527 pdy �Ͽ�¡ �ý��۸޼��� �Ͽ�¡ ���� �ƴҶ� �����ൿ [���͸���ü]
					if( HOUSINGMGR->IsHousingMap() == FALSE )
					{
						CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1867) );	//1867	"�� ���������� ����Ͻ� �� �����ϴ�."
						return;
					}

					// 091019 pdy �Ͽ�¡�ʿ��� �ε��߿� ���� �Ͽ�¡ ��� ������ �߰�
					if( HOUSINGMGR->IsHouseInfoLoadOk() == FALSE )
					{
						return;
					}

					//090527 pdy �Ͽ�¡ �ý��۸޼��� �ٹ̱� ��尡 �ƴҽ� ���ѻ��� [���͸���ü]
					if( HERO->GetState() != eObjectState_Housing  )
					{
						CHATMGR->AddMsg(CTC_SYSMSG,  CHATMGR->GetChatMsg(1878) );	//1878	"�ٹ̱� ����϶��� ����Ͻ� �� �ֽ��ϴ�."
						return;
					}
				//090527 pdy �Ͽ�¡ �ý��۸޼��� ž�»��½� ���ѻ��� [���͸���ü]
					if( HERO->GetState() == eObjectState_HouseRiding  )
					{
						CHATMGR->AddMsg(CTC_SYSMSG,  CHATMGR->GetChatMsg(1876) ); //1876	"ž�� ���¸� ������ �� ����ϼž� �մϴ�."
						return;
					}

					ITEM_INFO* pInfo = ITEMMGR->GetItemInfo( pItem->GetItemIdx() );
					if( pInfo && HOUSINGMGR->IsHouseInfoLoadOk() && HOUSINGMGR->IsHouseOwner() ) 
					{
						CInventoryExDialog* pDlg = GAMEIN->GetInventoryDialog();

						if( ! pDlg )
							return;

						pDlg->SetDisable(TRUE);
						pItem->SetLock(TRUE) ;
						Set_QuickSelectedItem(pItem) ;
						HOUSINGMGR->ChangeMaterialFurniture(pInfo->SupplyType , pInfo->SupplyValue, 0 );
						//090527 pdy �Ͽ�¡ �˾�â [���͸��� ����]
						WINDOWMGR->MsgBox( MBI_HOUSE_CHANGEMATERIAL_AREYOUSURE, MBT_YESNO, CHATMGR->GetChatMsg(1896)) ; //1896	"������ �����Ͻðڽ��ϱ�?"
					}
					return;
				}
				break;
			case ITEM_KIND_VISIT_HOUSE:				
				{
					// 091019 pdy �Ͽ�¡�ʿ��� �ε��߿� ���� �Ͽ�¡ ��� ������ �߰�
					if( HOUSINGMGR->IsHousingMap() &&	
						HOUSINGMGR->IsHouseInfoLoadOk() == FALSE )
					{
						return;
					}

					if( HERO->GetState() != eObjectState_None || HERO->IsPKMode() )
					{
						WINDOWMGR->MsgBox( MBI_CANTUSE_MAPMOVESCROLL, MBT_OK, CHATMGR->GetChatMsg(789) ) ;
						return ;
					}

					if(HERO->GetVehicleID())
					{
						CHATMGR->AddMsg(CTC_SYSMSG,  CHATMGR->GetChatMsg(1839) ); //1839	"ž�� ������ �̿� �ÿ��� �ش� ����� ����Ͻ� �� �����ϴ�."
						return;
					}

					CInventoryExDialog* pDlg = GAMEIN->GetInventoryDialog();
					if( ! pDlg )
						return;

					cHouseSearchDlg* pSearchDlg = GAMEIN->GetHouseSearchDlg();
					if( ! pSearchDlg )
						return;

					//�̹� ������̸� �ݴ´� 
					if( pSearchDlg->IsActive() ) 
						pSearchDlg->SetActive(FALSE);
		

					pSearchDlg->SetVisitInfo(eHouseVisitByItem ,pItem->GetItemIdx() , pItem->GetPosition() );
					pSearchDlg->SetActive(TRUE);

					pDlg->SetDisable(TRUE);
					pItem->SetLock(TRUE) ;
					Set_QuickSelectedItem(pItem) ;

					return;
				}
				break;
			case ITEM_KIND_VISIT_HOME:			
				{
					// 091019 pdy �Ͽ�¡�ʿ��� �ε��߿� ���� �Ͽ�¡ ��� ������ �߰�
					if( HOUSINGMGR->IsHousingMap() &&	
						HOUSINGMGR->IsHouseInfoLoadOk() == FALSE )
					{
						return;
					}

					if( HERO->GetState() != eObjectState_None || HERO->IsPKMode() )
					{
						WINDOWMGR->MsgBox( MBI_CANTUSE_MAPMOVESCROLL, MBT_OK, CHATMGR->GetChatMsg(789) ) ;
						return ;
					}

					if(HERO->GetVehicleID())
					{
						CHATMGR->AddMsg(CTC_SYSMSG,  CHATMGR->GetChatMsg(1839) ); //1839	"ž�� ������ �̿� �ÿ��� �ش� ����� ����Ͻ� �� �����ϴ�."
						return;
					}

					CInventoryExDialog* pDlg = GAMEIN->GetInventoryDialog();
					if( ! pDlg )
						return;

					//090527 pdy �Ͽ�¡ �ý��۸޼��� �������Ͻ� ���ѻ��� [�����湮]
					if( HOUSINGMGR->IsHouseOwner() )	
					{
						CHATMGR->AddMsg(CTC_SYSMSG,  CHATMGR->GetChatMsg(1904) ); //1904	"�̹� ������ �����ֽ��ϴ�."
						return;
					}

					pDlg->SetDisable(TRUE);
					pItem->SetLock(TRUE) ;
					Set_QuickSelectedItem(pItem) ;

					//090527 pdy �Ͽ�¡ �˾�â [�ڱ����湮]
					WINDOWMGR->MsgBox( MBI_HOUSE_VISIT_MYHOME_AREYOUSURE,MBT_YESNO, CHATMGR->GetChatMsg(1897)); //1897	"������ �̵��Ͻðڽ��ϱ�?"
					OBJECTSTATEMGR->StartObjectState(HERO, eObjectState_Deal) ;
					return;
				}
				break;
			case ITEM_KIND_EXTEND_HOUSE :
				{
					//090527 pdy �Ͽ�¡ �ý��۸޼��� �Ͽ�¡ ���� �ƴҶ� �����ൿ [�Ͽ�¡ Ȯ��]
					if( HOUSINGMGR->IsHousingMap() == FALSE )
					{
						CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1867) );	//1867	"�� ���������� ����Ͻ� �� �����ϴ�."
						return;
					}

					// 091019 pdy �Ͽ�¡�ʿ��� �ε��߿� ���� �Ͽ�¡ ��� ������ �߰�
					if( HOUSINGMGR->IsHouseInfoLoadOk() == FALSE )
					{
						return;
					}

					//090527 pdy �Ͽ�¡ �ý��۸޼��� �ִ�Ȯ�� �ܰ� ���ѻ��� [�Ͽ�¡ Ȯ��]
					if(! HOUSINGMGR->CanExtendHouse() )
					{
						CHATMGR->AddMsg(CTC_SYSMSG,  CHATMGR->GetChatMsg(1866) );	//1866	"�� �̻� Ȯ�� �Ͻ� �� �����ϴ�."
						return;
					}

					//090527 pdy �Ͽ�¡ �ý��۸޼��� �ٹ̱� ����Ͻ� ���ѻ��� [�Ͽ�¡ Ȯ��]
					if( HERO->GetState() == eObjectState_Housing  )
					{
						CHATMGR->AddMsg( CTC_SYSMSG,  CHATMGR->GetChatMsg(1874) );	//1874	"�ٹ̱� ��忡�� ����Ͻ� �� �����ϴ�."
						return;
					}

					CInventoryExDialog* pDlg = GAMEIN->GetInventoryDialog();

					if( ! pDlg )
						return;

					pDlg->SetDisable(TRUE);
					pItem->SetLock(TRUE) ;
					Set_QuickSelectedItem(pItem) ;

					//090527 pdy �Ͽ�¡ �˾�â [�Ͽ콺Ȯ��]
					WINDOWMGR->MsgBox( MBI_HOUSE_EXTEND_AREYOUSURE, MBT_YESNO,  CHATMGR->GetChatMsg(1898) ); //1898	"���� Ȯ�� �Ͻðڽ��ϱ�?"

					return;
				}
				break;
			case ITEM_KIND_DUNGEON_KEY:
				{
					stDungeonKey* pKey = GAMERESRCMNGR->GetDungeonKey(pInfo->SupplyValue);
					if(!pKey)
						return;

					if(MAP->GetMapNum() == pKey->wMapNum)
					{
						CHATMGR->AddMsg( CTC_SYSMSG,  CHATMGR->GetChatMsg(1977) );
						return;
					}

					MSG_DWORD4 msg;
					msg.Category = MP_DUNGEON;
					msg.Protocol = MP_DUNGEON_ENTRANCE_SYN;
					msg.dwData1 = pKey->dwIndex;
					msg.dwData2 = pKey->wMapNum;
					msg.dwData3 = pItem->GetItemIdx();
					msg.dwData4 = pItem->GetPosition();
					NETWORK->Send(&msg,sizeof(msg));
					return;
				}
				break;
			// 090422 ShinJS --- Ż�� ������ ���
			case ITEM_KIND_SUMMON_VEHICLE :
				{
					// �̹� ��ȯ�Ǿ� �ִ� ��� ���� ó��
					if( pItem->IsLocked() )
					{
						// 090423 ShinJS --- Ż�� ���� ���ɿ��� Ȯ��
						if( !HERO->CanSealVehicle() )
							return;

						StartVehicleSeal( HERO->GetVehicleID() );
						return;
					}

					// 090423 ShinJS --- ��ȯ ���ɻ��� Ȯ��
					if( !HERO->CanSummonVehicle() )
						return;

					WORD wMonsterKind = WORD( pItem->GetItemInfo()->SupplyValue );
					const VehicleScript& script = GAMERESRCMNGR->GetVehicleScript( wMonsterKind );

					// Ż�� ��ũ��Ʈ Ȯ��
					if( script.mMonsterKind == wMonsterKind )
					{
						// Ż�� ��ġ ����
						OBJECTMGR->StartVehicleInstall( wMonsterKind, pItem );
					}
					return;
				}
				break;

			case ITEM_KIND_LEVEL_UP:
				{
					if(HERO->GetLevel() >= MAX_CHARACTER_LEVEL_NUM)
					{
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 2019 ) );
						return;
					}
				}
				break;

			case ITEM_KIND_GUILDPOINT_UP:
				{
					if( HERO->GetGuildIdx() == 0 )
					{
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 2020 ) );
						return ;
					}
				}
				break;

			case ITEM_KIND_FAMILYPOINT_UP:
				{
					if( HERO->GetFamilyIdx() == 0 )
					{
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 2021 ) );
						return ;
					}
				}
				break;
			
			case ITEM_KIND_EXP_UP:
				{
					if(HERO->GetLevel() >= MAX_CHARACTER_LEVEL_NUM)
					{
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 2019 ) );
						return;
					}
				}
				break;

			case ITEM_KIND_PCROOMPOINT_UP:
				{
					CMiniMapDlg* pMiniMapDlg = GAMEIN->GetMiniMapDialog();
					cWindow* pMallBtn = pMiniMapDlg->GetWindowForID( MNM_PCROOM_MALL_BTN );
					if( !pMallBtn )
						break;

					if(! pMallBtn->IsActive())
					{
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 2039 ) );
						return;
					}

					pItem->SetLock(TRUE) ;
					Set_QuickSelectedItem(pItem) ;
				}
				break;

			case ITEM_KIND_FISHPOINT_UP:
				{
					if(FISHINGMGR->GetFishingLevel() >= MAX_FISHING_LEVEL)
					{
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 2040 ) );
						return;
					}
				}
				break;

			case ITEM_KIND_COOKLEVEL_UP:
				{
					if(COOKMGR->GetCookLevel() >= MAX_COOKING_LEVEL)
					{
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 2040 ) );
						return;
					}
				}
				break;

			case ITEM_KIND_DECOUSEPOINT_UP:
				{
					if(! HOUSINGMGR->IsHousingMap())
					{
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 2041 ) );
						return;
					}
				}
				break;

			case ITEM_KIND_PKPOINT_UP:
			case ITEM_KIND_PKPOINT_DN:
				{
					pItem->SetLock(TRUE) ;
					Set_QuickSelectedItem(pItem) ;
				}
				break;
			}

			// E ����ý��� �߰� added by hseos 2007.05.14	2007.08.21

			{
				mMessage.Category = MP_ITEM;
				// 080820 LUJ, ȹ�� �� ��Ÿ���� ��� �������� ��ȯ �� �ٸ� ���������� �����Ѵ�
				mMessage.Protocol = ( ITEM_KIND_COOLTIME == pInfo->SupplyType ? MP_ITEM_USE_CHANGEITEM_SYN : MP_ITEM_USE_SYN );
				mMessage.dwObjectID = gHeroID;
				mMessage.TargetPos = pItem->GetPosition();
				mMessage.wItemIdx = pItem->GetItemIdx();
			
				if( pItem->GetItemInfo()->SupplyType == ITEM_KIND_PET )
				{
					if( HEROPET && pItem->GetDBIdx() != HEROPET->GetItemDBIdx() )
					{
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1598) );
						return;
					}

					if( PETMGR->GetLastSummonTime() != 0 && gCurTime - PETMGR->GetLastSummonTime() < PETMGR->GetSummonPenaltyDelay() )
					{
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1597) );
						return;
					}

					if( HERO->GetObjectBattleState() == eObjectBattleState_Battle )
					{
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1599) );
						return;
					}

					PET_OBJECT_INFO* pObjectInfo = PETMGR->GetPetObjectInfo( pItem->GetDBIdx() );

					if( !pObjectInfo || pObjectInfo->State == ePetState_Die )
					{
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(789) );
						return;
					}
			
					if( !HEROPET && HERO->GetLevel() < PETMGR->GetSummonPlayerLevel( pObjectInfo->Grade ) )
					{
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1600) );
						return;
					}

					SetDisable( TRUE );

					// 071213 KTH -- ������ 5�ʿ��� 2�ʷ� �ٲ�.
					//const DWORD changeSecond = 2000;

					//SetCoolTime( pItem->GetItemIdx(), changeSecond );
					SetCoolTime( pItem->GetItemIdx(), WAITMILISECOND );

					{
						cQuickSlotDlg* dialog = ( cQuickSlotDlg* )WINDOWMGR->GetWindowForID( QI1_QUICKSLOTDLG );
						ASSERT( dialog );

						//dialog->SetCoolTime( pItem->GetItemIdx(), changeSecond );
						dialog->SetCoolTime( pItem->GetItemIdx(), WAITMILISECOND );
					}

					{
						CProgressDialog* dialog = ( CProgressDialog* )WINDOWMGR->GetWindowForID( PROGRESS_DIALOG );
						ASSERT( dialog );

						if( HEROPET )
						{
							dialog->SetText( RESRCMGR->GetMsg( 1007 ) );
						}
						else
						{
							dialog->SetText( RESRCMGR->GetMsg( 1006 ) );
						}
						dialog->Wait( CProgressDialog::eActionPetSummon );
					}
				}
				else if( pItem->GetItemInfo()->SupplyType == ITEM_KIND_CHANGESIZE_UPWEIGHT ||
						pItem->GetItemInfo()->SupplyType == ITEM_KIND_CHANGESIZE_DNWEIGHT ||
						pItem->GetItemInfo()->SupplyType == ITEM_KIND_CHANGESIZE_DEFALUT)
				{
					// ����ʿ��� Ű��ȯ ���� ������ ����� ���Ѵ�
					if( MAP->GetMapNum()==GTMAPNUM )
					{
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 91 ) );
						return;
					}
					// �����ʿ����� Ű��ȯ ���� ������ ����� ���Ѵ�
					else if( SIEGEWARFAREMGR->IsSiegeWarfareKindMap(MAP->GetMapNum()) && 
							 SIEGEWARFAREMGR->Is_CastleMap() == FALSE )
					{
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 91 ) );
						return;
					}

					SetDisable( TRUE );

					SetCoolTime( pItem->GetItemIdx(), WAITMILISECOND );
	
					cQuickSlotDlg* dialog = ( cQuickSlotDlg* )WINDOWMGR->GetWindowForID( QI1_QUICKSLOTDLG );
					ASSERT( dialog );

					//dialog->SetCoolTime( pItem->GetItemIdx(), changeSecond );
					dialog->SetCoolTime( pItem->GetItemIdx(), WAITMILISECOND );

					// 080706 LYW --- InventoryExDialog : Ȯ�� �� ���� �߰� ó��.
					cQuickSlotDlg* dialog2 = NULL ;
					dialog2 = ( cQuickSlotDlg* )WINDOWMGR->GetWindowForID( QI2_QUICKSLOTDLG ) ;
					ASSERT(dialog2) ;

					if(dialog2)
					{
						dialog2->SetCoolTime( pItem->GetItemIdx(), WAITMILISECOND ) ;
					}

					{
						CProgressDialog* dialog = ( CProgressDialog* )WINDOWMGR->GetWindowForID( PROGRESS_DIALOG );
						ASSERT( dialog );

						dialog->Wait( CProgressDialog::eActionChangeSize );
					}
				}
				else if( pItem->GetItemInfo()->SupplyType == ITEM_KIND_SUMMON_MONSTER )
				{
					SetDisable( TRUE );

					SetCoolTime( pItem->GetItemIdx(), WAITMILISECOND );
	
					cQuickSlotDlg* dialog = ( cQuickSlotDlg* )WINDOWMGR->GetWindowForID( QI1_QUICKSLOTDLG );
					ASSERT( dialog );

					//dialog->SetCoolTime( pItem->GetItemIdx(), changeSecond );
					dialog->SetCoolTime( pItem->GetItemIdx(), WAITMILISECOND);

					// 080706 LYW --- InventoryExDialog : Ȯ�� �� ���� �߰� ó��.
					cQuickSlotDlg* dialog2 = NULL ;
					dialog2 = ( cQuickSlotDlg* )WINDOWMGR->GetWindowForID( QI2_QUICKSLOTDLG ) ;
					ASSERT(dialog2) ;

					if(dialog2)
					{
						dialog2->SetCoolTime( pItem->GetItemIdx(), WAITMILISECOND ) ;
					}

					{
						CProgressDialog* dialog = ( CProgressDialog* )WINDOWMGR->GetWindowForID( PROGRESS_DIALOG );
						ASSERT( dialog );

						dialog->Wait( CProgressDialog::eActionSummonMonster );
					}
				}
				else if(pItem->GetItemInfo()->SupplyType == ITEM_KIND_SUMMON_EFFECT)
				{
					// 100125 ShinJS --- ���� �� Progress ������� �ʰ� �����ϵ��� ����
					MOVEMGR->HeroMoveStop();

					SetCoolTime( pItem->GetItemIdx(), WAITMILISECOND );
	
					cQuickSlotDlg* dialog = ( cQuickSlotDlg* )WINDOWMGR->GetWindowForID( QI1_QUICKSLOTDLG );
					ASSERT( dialog );

					dialog->SetCoolTime( pItem->GetItemIdx(), WAITMILISECOND);

					// 080706 LYW --- InventoryExDialog : Ȯ�� �� ���� �߰� ó��.
					cQuickSlotDlg* dialog2 = NULL ;
					dialog2 = ( cQuickSlotDlg* )WINDOWMGR->GetWindowForID( QI2_QUICKSLOTDLG ) ;
					ASSERT(dialog2) ;

					if(dialog2)
					{
						dialog2->SetCoolTime( pItem->GetItemIdx(), WAITMILISECOND ) ;
					}

					NETWORK->Send( &mMessage, sizeof( mMessage ) );
				}

				else if( pItem->GetItemInfo()->SupplyType == ITEM_KIND_PET_EVOLUTION_PYHSIC_TYPE ||
						 pItem->GetItemInfo()->SupplyType == ITEM_KIND_PET_EVOLUTION_MAGIC_TYPE ||
						 pItem->GetItemInfo()->SupplyType == ITEM_KIND_PET_EVOLUTION_SUPPORT_TYPE )
				{
					SetDisable( TRUE );

					WINDOWMGR->MsgBox( MBI_PETGRADEUP, MBT_YESNO, CHATMGR->GetChatMsg(1585) ) ;
				}
				else
				{
					NETWORK->Send( &mMessage, sizeof( mMessage ) );
				}
			}
		}
		break;
	default:
		{
		}
		break;
	}
}

void CInventoryExDialog::SetActive(BOOL val)
{
	// 080221 LYW --- MacroManager : �κ��丮 Ȯ�忡 ���� ó��.
	int nExtendedInven = 0 ;
	nExtendedInven = HERO->Get_HeroExtendedInvenCount() ;

	ShowTab(2,FALSE) ;
	ShowTab(3,FALSE) ;

	// 100305 Ȯ��� �κ��丮 ���� Disible ó���� �ȵǴ� ���� ����
	switch(nExtendedInven)
	{
	case 1 : 
		{
			ShowTab(2,TRUE); 
			SetVisibleTabCount(3);
		}
		break ;
	case 2 :
		{
			ShowTab(2,TRUE) ;
			ShowTab(3,TRUE) ;
			SetVisibleTabCount(4);
		}
		break ;

	default : break ;
	}

	if( !m_bDisable )
	{
		if(val == FALSE)
		{
			CDealDialog * dDlg = GAMEIN->GetDealDialog();
			if(dDlg)
				if(dDlg->IsShow())
					dDlg->SetActive(FALSE);

			cDialog* dialog = WINDOWMGR->GetWindowForID( GDW_WAREHOUSEDLG );
			ASSERT( dialog );

			if( dialog->IsActive() )
			{
				dialog->SetActive( FALSE );
			}
		}

		cTabDialog::SetActive(val);
	}

	CheckBackground();

	// �κ� â�� ������ �Ʒ� â�鵵 ����.
	if( ! val )
	{
		cDialog* enchantDialog		= WINDOWMGR->GetWindowForID( ITD_UPGRADEDLG );
		cDialog* reinforceDialog	= WINDOWMGR->GetWindowForID( ITR_REINFORCEDLG );
		cDialog* dissoloveDialog	= WINDOWMGR->GetWindowForID( DIS_DISSOLUTIONDLG );

		enchantDialog->SetActive( FALSE );
		reinforceDialog->SetActive( FALSE );
		dissoloveDialog->SetActive( FALSE );
	}

	// 071022 LYW --- InventoryExDialog : Check open dialog.
	TUTORIALMGR->Check_OpenDialog(this->GetID(), val) ;

	// 100616 �κ��丮�� ��ġ����
	if( val )
		RevisionDialogPos();
}

void CInventoryExDialog::OnActionEvnet(LONG lId, void * p, DWORD we)
{
	if( !p ) return ;

	if(we == WE_LBTNDBLCLICK)
	{	
		CItem * pItem = NULL ;

		if(lId == IN_WEAREDDLG)
		{
			if( !m_pWearedDlg ) return ;

			WORD wIconIdx = 0 ;
			wIconIdx = (WORD)m_pWearedDlg->GetCurSelCellPos() ;

			pItem = (CItem*)m_pWearedDlg->GetIconForIdx( wIconIdx ) ;
		}
		else
		{
			cIconGridDialog * gridDlg = NULL ;
			gridDlg = (cIconGridDialog *)GetTabSheet(GetCurTabNum()) ;

			if( !gridDlg ) return ;

			pItem = (CItem*)gridDlg->GetIconForIdx((WORD)gridDlg->GetCurSelCellPos()) ;
		}

		if( !pItem ) return ;

		//if( pItem->GetItemParam() == eITEM_PARAM_SEAL )
		if( pItem->GetItemBaseInfo().nSealed == eITEM_TYPE_SEAL )
		{
			if( pItem->IsLocked() ) return ;

			WINDOWMGR->MsgBox(MBI_SHOPITEM_LOCK, MBT_YESNO, CHATMGR->GetChatMsg( 1178 )) ;

			// 090107 LYW --- InventoryExDialog : ���� ���� �޽����� ���� ��쿡��, 
			// �޽��� ���� �κ��丮 ��� �ȵǵ��� �����Ѵ�.
			SetDisable( TRUE ) ;

			return ;
		}
		else
		{
			UseItem(pItem) ;
		}
	}
	else if(we == WE_RBTNCLICK)
	{
		CItem * pItem = NULL ;

		if(lId == IN_WEAREDDLG)
		{
			if( !m_pWearedDlg ) return ;

			WORD wIconIdx = 0 ;
			wIconIdx = (WORD)m_pWearedDlg->GetCurSelCellPos() ;

			pItem = (CItem*)m_pWearedDlg->GetIconForIdx( wIconIdx ) ;
		}
		else
		{
			cIconGridDialog * gridDlg = NULL ;
			gridDlg = (cIconGridDialog *)GetTabSheet(GetCurTabNum()) ;

			if( !gridDlg ) return ;

			pItem = (CItem*)gridDlg->GetIconForIdx((WORD)gridDlg->GetCurSelCellPos()) ;
		}

		if( !pItem ) return ;
		
		
		CDealDialog* pDealDlg = GAMEIN->GetDealDialog() ;
		if( pDealDlg && pDealDlg->IsActive() ) {
			pDealDlg->SellItem(pItem);
			return;
		}

		CDissolveDialog* pDissolveDlg = GAMEIN->GetDissolveDialog();
		if( pDissolveDlg && pDissolveDlg->IsActive() ) {
			pDissolveDlg->MoveIcon(pItem);
			return;
		}

		if( pItem->GetItemBaseInfo().nSealed == eITEM_TYPE_SEAL )
		{
			if( pItem->IsLocked() ) return ;

			WINDOWMGR->MsgBox(MBI_SHOPITEM_LOCK, MBT_YESNO, CHATMGR->GetChatMsg( 1178 )) ;

			// 090107 LYW --- InventoryExDialog : ���� ���� �޽����� ���� ��쿡��, 
			// �޽��� ���� �κ��丮 ��� �ȵǵ��� �����Ѵ�.
			SetDisable( TRUE ) ;

			return ;
		}
		else
		{
			UseItem(pItem) ;
		}
	}
}

CItem * CInventoryExDialog::GetCurSelectedItem(eITEMTABLE type)
{
	switch(type)
	{
	case eItemTable_Inventory:
		{
			// INVENTORY
			cIconGridDialog * gridDlg = (cIconGridDialog *)GetTabSheet(GetCurTabNum());
			if(gridDlg->GetCurSelCellPos() == -1) 
				return NULL;
			return (CItem *)gridDlg->GetIconForIdx((WORD)gridDlg->GetCurSelCellPos());
		}
		break;
	case eItemTable_Weared:
		{
			// WEARED
			if(m_pWearedDlg->GetCurSelCellPos() == -1) 
				return NULL;
			return (CItem *)m_pWearedDlg->GetIconForIdx((WORD)m_pWearedDlg->GetCurSelCellPos());
		}
		break;
	}
	return NULL;
}

BOOL CInventoryExDialog::GetBlankPositionRestrictRef(WORD & absPos)
{
	for(int i = 0 ; i < GetTabNum() ; ++i)
	{
		cIconGridDialog * gridDlg = (cIconGridDialog *)GetTabSheet(i);
		for(int j = 0 ; j < gridDlg->GetCellNum() ; ++j)
		{
			if(gridDlg->IsAddable(j))
			{
//				absPos = INVENTORY_STARTPOSITION+i*CELLNUMPERPAGE+j;
				absPos = TP_INVENTORY_START+i*TABCELL_INVENTORY_NUM+j;
				return TRUE;
			}
		}
		
	}
	return FALSE;
}

int CInventoryExDialog::GetBlankNum()
{
	int nBlankNum = 0;
	BYTE TabNum = GetTabNum();
	for( BYTE i = 0 ; i < TabNum ; ++i )
	{
		cIconGridDialog* gridDlg = (cIconGridDialog*)GetTabSheet(i);

		for( WORD j = 0 ; j < gridDlg->GetCellNum() ; ++j )
		{
			if( gridDlg->IsAddable(j) )
				++nBlankNum;
		}
	}

	return nBlankNum;
}

BOOL CInventoryExDialog::FakeMoveIcon(LONG x, LONG y, cIcon * icon)
{
	ASSERT(icon);
	if( m_bDisable )	return FALSE;

	if( PKMGR->IsPKLooted() )	return FALSE;	//�׾����� �ٸ�����.. //KES 040801

	if( icon->GetType() == WT_STALLITEM || icon->GetType() == WT_EXCHANGEITEM ) return FALSE;
	
	if(icon->GetType() == WT_ITEM)
	{
		return FakeMoveItem(x, y, (CItem *)icon);
	}
	else if(icon->GetType() == WT_DEALITEM)
	{
		FakeBuyItem(x, y, (CDealItem *)icon);
		AUDIOMGR->Play(
			58,
			gHeroID);
	}
	else if( icon->GetType() == WT_HOUSING_STORED_ICON )
	{
		if (FALSE == FakeMoveHousingStoredIcon(x, y,(cHousingStoredIcon*)icon) )
		{
			//090527 pdy �Ͽ�¡ �ý��۸޼��� [��������]
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1873 ) ); //1873	"�ش� �������� �̵� �� �� �����ϴ�."
		}
	}
	return FALSE;
}

void CInventoryExDialog::FakeBuyItem( LONG mouseX, LONG mouseY, CDealItem * pFromItem )
{
//	if( pFromItem )
//		GAMEIN->GetDealDialog()->FakeBuyItem( pFromItem->GetAbsX(), pFromItem->GetAbsY(), pFromItem->GetItemIdx() );
	if( pFromItem )
		GAMEIN->GetDealDialog()->FakeBuyItem( (LONG)pFromItem->GetAbsX(), (LONG)pFromItem->GetAbsY(), pFromItem );
}

// ����â ���� �̵�
void CInventoryExDialog::MoveItem(POSTYPE FromPos, POSTYPE ToPos )
{
	CItem * FromItem = NULL;
	CItem * ToItem = NULL;

	DeleteItem(FromPos, &FromItem);
	DeleteItem(ToPos, &ToItem);

	if(FromItem)
	{
		FromItem->SetPosition(ToPos);
		if(!AddItem(FromItem))
			assert(0);
	}

	if(ToItem)
	{
		ToItem->SetPosition(FromPos);
		if(!AddItem(ToItem))
			assert(0);
	}
}

BOOL CInventoryExDialog::FakeMoveItem( LONG mouseX, LONG mouseY, CItem * pFromItem )
{
	WORD ToPos=0;
	
	if( !GetPositionForXYRef( 0, mouseX, mouseY, ToPos ) )	// ������ġ �Ѿ� ��
		return FALSE;

	CItem * pToItem = GetItemForPos( ToPos );
	
	//sound
	if(ITEMMGR->GetTableIdxForAbsPos(ToPos)==eItemTable_Weared)
	{
		AUDIOMGR->Play(
			59,
			gHeroID);
	}
	else
	{
		AUDIOMGR->Play(
			58,
			gHeroID);
	}

//KES EXCHANGE 031002
	if( pToItem )
	if( pToItem->IsLocked() ) return FALSE;
	
	if( pFromItem->IsLocked() ) return FALSE;

	if( pFromItem->GetParent() == ( cObject* )GAMEIN->GetPetWearedDlg() )
	{
		if( pToItem )
		{
			RECT rt;
			GAMEIN->GetPetWearedDlg()->GetIconCellRect( pFromItem->GetPosition(), rt );
			GAMEIN->GetPetWearedDlg()->FakeMoveIcon( rt.left, rt.top, pToItem );
		}
		else
		{
			MSG_PET_ITEM_MOVE_SYN msg;
			msg.Category = MP_PET;
			msg.Protocol = MP_PET_ITEM_MOVE_PLAYER_SYN;
			msg.dwObjectID = HEROID;
			msg.PlayerPos = ToPos;
			msg.PlayerItemIdx = 0;
			msg.PetPos = pFromItem->GetPosition();
			msg.PetItemIdx = pFromItem->GetItemIdx();

			NETWORK->Send( &msg, sizeof( msg ) );
		}
	
		return FALSE;
	}
	
	//---KES Item Combine Fix 071021
	if(TP_STORAGE_START <= pFromItem->GetPosition() && pFromItem->GetPosition() < TP_STORAGE_END)
	{
		if( ITEMMGR->IsDupItem( pFromItem->GetItemIdx() ) )
		{
			if( pToItem )
			if( pToItem->GetItemIdx() == pFromItem->GetItemIdx() )
			{
				return FakeItemCombine( ToPos, pFromItem, pToItem );
			}
		}

		if( ToPos == ( TP_WEAR_START + eWearedItem_Shield) )
		{
			switch( HERO->GetWeaponAniType() )
			{
			case eWeaponAnimationType_TwoHand:
			case eWeaponAnimationType_Staff:
			case eWeaponAnimationType_Bow:
			case eWeaponAnimationType_Gun:
			case eWeaponAnimationType_TwoBlade:
			// 090521 ONS �ű����� ���� Ÿ�� �Ӽ� �߰�
			case eWeaponAnimationType_BothDagger:
				{
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(772) );
					return FALSE;
				}
				break;
			}
		}

		return FakeGeneralItemMove( ToPos, pFromItem, pToItem );
	}
	else if(TP_SHOPITEM_START <= pFromItem->GetPosition() && pFromItem->GetPosition() < TP_SHOPITEM_END)
	{
		return FakeGeneralItemMove( ToPos, pFromItem, pToItem );
	}

	if( ITEMMGR->IsDupItem( pFromItem->GetItemIdx() ) )
	{
		if( IsDivideFunctionKey() && !( pToItem ) )
		{
			return FakeItemDivide( ToPos, pFromItem, pToItem );
		}
		else if( ( pToItem && pFromItem->GetItemIdx() == pToItem->GetItemIdx() ) )
		{
			return FakeItemCombine( ToPos, pFromItem, pToItem );
		}
	}

	if( ToPos == ( TP_WEAR_START + eWearedItem_Shield) )
	{
		switch( HERO->GetWeaponAniType() )
		{
		case eWeaponAnimationType_TwoHand:
		case eWeaponAnimationType_Staff:
		case eWeaponAnimationType_Bow:
		case eWeaponAnimationType_Gun:
		// 090521 ONS �ű����� ���� Ÿ�� �Ӽ� �߰�
		case eWeaponAnimationType_BothDagger:
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(772) );
				return FALSE;
			}
			break;
		}
	}
		
	return FakeGeneralItemMove( ToPos, pFromItem, pToItem );
}

BOOL CInventoryExDialog::IsDivideFunctionKey()
{
	return KEYBOARD->GetKeyPressed(KEY_MENU);
}

BOOL CInventoryExDialog::FakeGeneralItemMove( POSTYPE ToPos, CItem * pFromItem, CItem * pToItem )
{
	if( CanBeMoved( pFromItem, ToPos ) == FALSE )
		return FALSE;

	BYTE FromIdx = ITEMMGR->GetTableIdxForAbsPos( pFromItem->GetPosition() );
	if(FromIdx >= eItemTable_Max) return FALSE;
	if( pToItem && CanBeMoved( pToItem, pFromItem->GetPosition() ) == FALSE )
	{
		return FALSE;
	}
	if(pToItem && (FromIdx == eItemTable_MunpaWarehouse))
	{
		if(FromIdx == eItemTable_MunpaWarehouse)
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(365) );
		return FALSE;
	}

	BYTE ToIdx = ITEMMGR->GetTableIdxForAbsPos( ToPos );
	if(ToIdx >= eItemTable_Max) return FALSE;


	MSG_ITEM_MOVE_SYN msg;
	msg.Category			= MP_ITEM;
	msg.Protocol			= MP_ITEM_MOVE_SYN;
	msg.dwObjectID			= HEROID;

	msg.FromPos				= pFromItem->GetPosition();
	msg.wFromItemIdx		= pFromItem->GetItemIdx();
	msg.ToPos				= ToPos;
	msg.wToItemIdx			= (pToItem?pToItem->GetItemIdx():0);

	msg.PartTypeA = pFromItem->GetItemInfo()->Part3DType;
	msg.PartTypeB = (pToItem?pToItem->GetItemInfo()->Part3DType:0);

	msg.EquipTypeA = pFromItem->GetItemInfo()->EquipType;
	msg.EquipTypeB = (pToItem ? pToItem->GetItemInfo()->EquipType : eEquipType_None);

	msg.EquipSlotA = pFromItem->GetItemInfo()->EquipSlot;
	msg.EquipSlotB = (pToItem ? pToItem->GetItemInfo()->EquipSlot : eWearedItem_None);

 	if( msg.FromPos == msg.ToPos )
		return FALSE;

	ITEMMGR->SetDisableDialog(TRUE, eItemTable_Inventory);
	ITEMMGR->SetDisableDialog(TRUE, eItemTable_Storage);
	ITEMMGR->SetDisableDialog(TRUE, eItemTable_MunpaWarehouse);

	NETWORK->Send( &msg, sizeof(msg) );
	// 080715 LUJ, ������ �̵� üũ�� �������� �Ϸ�� �� �̵�ǵ��?FALSE�� ��ȯ�ؾ��Ѵ�
	return FALSE;
}

BOOL CInventoryExDialog::FakeItemDivide( POSTYPE ToPos, CItem * pFromItem, CItem * pToItem )
{
	if( !CanBeMoved( pFromItem, ToPos ) )
		return FALSE;
	if( pFromItem->GetPosition() == ToPos )
		return FALSE;
	if( pFromItem->IsLocked() )
		return FALSE;

	BYTE FromIdx = ITEMMGR->GetTableIdxForAbsPos( pFromItem->GetPosition() );
	if(FromIdx >= eItemTable_Max) return FALSE;

	BYTE ToIdx = ITEMMGR->GetTableIdxForAbsPos( ToPos );
	if(ToIdx >= eItemTable_Max) return FALSE;
	
	if(pFromItem->GetDurability() == 1)
	{
		return FakeGeneralItemMove(ToPos, pFromItem, pToItem);
	}

	m_divideMsg.Category			= MP_ITEM;
	m_divideMsg.Protocol			= MP_ITEM_DIVIDE_SYN;
	m_divideMsg.dwObjectID			= HEROID;

	m_divideMsg.wItemIdx = pFromItem->GetItemIdx();
	m_divideMsg.FromPos = pFromItem->GetPosition();
	m_divideMsg.FromDur = pFromItem->GetDurability();
	m_divideMsg.ToPos = ToPos;
	m_divideMsg.ToDur = 0;

	ITEMMGR->SetDisableDialog(TRUE, eItemTable_Inventory);
	ITEMMGR->SetDisableDialog(TRUE, eItemTable_Storage);
	ITEMMGR->SetDisableDialog(TRUE, eItemTable_MunpaWarehouse);

	cDivideBox * pDivideBox = WINDOWMGR->DivideBox( DBOX_DIVIDE_INV, (LONG)pFromItem->GetAbsX(), (LONG)pFromItem->GetAbsY(), OnFakeItemDivideOk, OnFakeItemDivideCancel, this, pFromItem, CHATMGR->GetChatMsg(185) );
	
	// 091215 ONS ������Stack�ִ���� �����ۺ��� ������ ������ ó���Ѵ�.
	const WORD wItemStackNum = ITEMMGR->GetItemStackNum( pFromItem->GetItemIdx() );	
	pDivideBox->SetMaxValue( wItemStackNum );
	pDivideBox->SetValue( 0 );
	//pDivideBox->SetValue( m_divideMsg.FromDur );
	
	return FALSE;
		
}

void CInventoryExDialog::OnFakeItemDivideCancel( LONG iId, void* p, DWORDEX param1, void * vData1, void * vData2 )
{
	ITEMMGR->SetDisableDialog(FALSE, eItemTable_Inventory);
	ITEMMGR->SetDisableDialog(FALSE, eItemTable_Storage);
	ITEMMGR->SetDisableDialog(FALSE, eItemTable_MunpaWarehouse);
}

void CInventoryExDialog::OnFakeItemDivideOk( LONG iId, void* p, DWORDEX param1, void * vData1, void * vData2 )
{
	CInventoryExDialog * tDlg = ( CInventoryExDialog * )vData1;
	if( param1 == 0 )
	{
		OnFakeItemDivideCancel(0,NULL,0,NULL,NULL);
		return;
	}

	const eITEMTABLE TargetTableIdx = ITEMMGR->GetTableIdxForAbsPos(tDlg->m_divideMsg.FromPos);
	CItem* pTargetItem = ITEMMGR->GetItemofTable(TargetTableIdx, tDlg->m_divideMsg.FromPos);

	if(!pTargetItem)
	{
		OnFakeItemDivideCancel(0,NULL,0,NULL,NULL);
		return ;
	}
	if(pTargetItem->IsLocked())
	{
		OnFakeItemDivideCancel(0,NULL,0,NULL,NULL);
		return ;
	}

	if(tDlg->m_divideMsg.wItemIdx != pTargetItem->GetItemIdx())
	{
		OnFakeItemDivideCancel(0,NULL,0,NULL,NULL);
		return ;
	}
	if(tDlg->m_divideMsg.FromDur != pTargetItem->GetDurability())
	{
		OnFakeItemDivideCancel(0,NULL,0,NULL,NULL);
		return ;
	}
	CItem * pToItem = ITEMMGR->GetItemofTable(ITEMMGR->GetTableIdxForAbsPos(tDlg->m_divideMsg.ToPos), tDlg->m_divideMsg.ToPos);
	if(pToItem)
	{
		OnFakeItemDivideCancel(0,NULL,0,NULL,NULL);
		return ;
	}

	if( tDlg->m_divideMsg.FromDur > param1 )
	{
		tDlg->m_divideMsg.ToDur			= param1;
		tDlg->m_divideMsg.FromDur		= tDlg->m_divideMsg.FromDur - param1;

		//---KES ItemDivide Fix 071020
		ITEMMGR->SetDivideItemInfo( tDlg->m_divideMsg.FromPos, tDlg->m_divideMsg.ToDur );
		//----------------------------

		NETWORK->Send( &tDlg->m_divideMsg, sizeof(tDlg->m_divideMsg) );
	}
	else
	{
		//������ �̵�
		tDlg->FakeGeneralItemMove(tDlg->m_divideMsg.ToPos, (CItem *)vData2, NULL);
		ITEMMGR->SetDisableDialog(FALSE, eItemTable_Inventory);
		ITEMMGR->SetDisableDialog(FALSE, eItemTable_Storage);
		ITEMMGR->SetDisableDialog(FALSE, eItemTable_MunpaWarehouse);
	}
}

BOOL CInventoryExDialog::FakeItemCombine( POSTYPE ToPos, CItem * pFromItem, CItem * pToItem )
{
	if( pFromItem->GetPosition() == pToItem->GetPosition() )
		return FALSE;
	if( pFromItem->GetItemIdx() != pToItem->GetItemIdx() )
		return FALSE;

	if( CanBeMoved( pFromItem, ToPos ) == FALSE )
		return FALSE;

	if( pToItem && CanBeMoved( pToItem, pFromItem->GetPosition() ) == FALSE )
		return FALSE;

	if( pFromItem->IsLocked() || pToItem->IsLocked() )
		return FALSE;

	BYTE FromIdx = ITEMMGR->GetTableIdxForAbsPos( pFromItem->GetPosition() );
	if(FromIdx >= eItemTable_Max) return FALSE;

	BYTE ToIdx = ITEMMGR->GetTableIdxForAbsPos( ToPos );
	if(ToIdx >= eItemTable_Max) return FALSE;

	MSG_ITEM_COMBINE_SYN msg;
	msg.Category			= MP_ITEM;
	msg.Protocol			= MP_ITEM_COMBINE_SYN;
	msg.dwObjectID			= HEROID;

	msg.FromPos				= pFromItem->GetPosition();
	msg.ToPos				= pToItem->GetPosition();
	msg.wItemIdx			= pFromItem->GetItemIdx();
	msg.FromDur				= pFromItem->GetDurability();
	msg.ToDur				= pToItem->GetDurability();

	ITEMMGR->SetDisableDialog(TRUE, eItemTable_Inventory);
	ITEMMGR->SetDisableDialog(TRUE, eItemTable_Storage);
	ITEMMGR->SetDisableDialog(TRUE, eItemTable_MunpaWarehouse);

	NETWORK->Send( &msg, sizeof(msg) );

	// 080715 LUJ, ������ �̵� üũ�� �������� �Ϸ�� �� �̵��ǵ��� FALSE�� ��ȯ�ؾ��Ѵ�
	return FALSE;
}

//090423 �Ͽ�¡â�� ����������->�κ��丮 ȸ��
BOOL	CInventoryExDialog::FakeMoveHousingStoredIcon(LONG mouseX, LONG mouseY, cHousingStoredIcon* pFromStoredIcon)
{
	if( pFromStoredIcon->IsLocked() ) return FALSE;

	WORD ToPos=0;
	
	if( !GetPositionForXYRef( 0, mouseX, mouseY, ToPos ) )	// ������ġ �Ѿ� ��
		return FALSE;

	//sound
	if(ITEMMGR->GetTableIdxForAbsPos(ToPos)!=eItemTable_Inventory)
		return FALSE;
	else
	{
		AUDIOMGR->Play(
			58,
			gHeroID);
	}

	CItem * pToItem = GetItemForPos( ToPos );

	if( pToItem )		//ToPos�� �������� ������ ����?
		return FALSE;

	//�������� ������ �� 

	stFurniture* pstFurniture = pFromStoredIcon->GetLinkFurniture();
	if ( pstFurniture->wState != eHOUSEFURNITURE_STATE_KEEP )
		return FALSE;


	if(! GetBlankPositionRestrictRef( ToPos ) )
		return FALSE;

	return HOUSINGMGR->RequestRestoredICon(ToPos,pFromStoredIcon);
}

void CInventoryExDialog::Linking()
{
	m_pWearedDlg = (CWearedExDialog *)GetWindowForID(IN_WEAREDDLG);
	m_pMoneyEdit = (cStatic *)GetWindowForID(IN_MONEYEDIT);

	for(int count = 0 ; count < MAX_BACKGROUND ; ++count)
	{
		m_pBackground[count] = GetWindowForID(IN_BACKGROUND1 + count);
		m_pBackground[count]->SetActive( FALSE ) ;
	}
}

// 090824 ShinJS --- Seal Option �� �����Ͽ� �ش� Opt�� ������ Item�� ã���� �ֵ��� ����
// 091106 LUJ, ������ �����ϴ� �������� ��ȯ�� �� �ֵ��� ����
CItem * CInventoryExDialog::GetItemForIdx(DWORD wItemIdx, WORD wExceptSealOpt, DURTYPE quantity, BOOL bExceptItemOpt)
{
	const ITEM_INFO* const pItemInfo = ITEMMGR->GetItemInfo(wItemIdx);

	if(0 == pItemInfo)
	{
		return 0;
	}

	// 091106 LUJ, �������� �����۸� ������ ��ȿ�ϴ�
	quantity = (pItemInfo->Stack ? quantity : 0);

	for(BYTE i = 0; i < GetTabNum(); ++i)
	{
		cIconGridDialog* gridDlg = (cIconGridDialog*)GetTabSheet(i);

		for( WORD j = 0 ; j < gridDlg->GetCellNum() ; ++j )
		{
			if(!gridDlg->IsAddable(j) )
			{
				CItem* const pItem = (CItem *)gridDlg->GetIconForIdx(TP_INVENTORY_START + j);

				if(pItem->GetItemIdx() != wItemIdx)
				{
					continue;
				}
				else if(pItem->GetItemBaseInfo().nSealed == wExceptSealOpt)
				{
					continue;
				}
				else if(0 < quantity)
				{
					if(pItem->GetDurability() < quantity)
					{
						continue;
					}
				}
				else if( bExceptItemOpt )
				{
					// 091127 ONS ��ȭ, ����, ��æƮ �������� ����
					const ITEM_OPTION& option = ITEMMGR->GetOption( pItem->GetDBIdx() );
					if( option.mItemDbIndex )
					{
						continue;
					}
				}

				return pItem;
			}
		}
	}
	return NULL;
}

DURTYPE	CInventoryExDialog::GetTotalItemDurability(DWORD wItemIdx)
{
	DURTYPE DurSum = 0;
	BYTE TabNum = GetTabNum();
	for( BYTE i = 0 ; i < TabNum ; ++i )
	{
		cIconGridDialog* gridDlg = (cIconGridDialog*)GetTabSheet(i);

		for( WORD j = 0 ; j < gridDlg->GetCellNum() ; ++j )
		{
			if(!gridDlg->IsAddable(j) )
			{
				CItem * pItem = (CItem *)gridDlg->GetIconForIdx(TP_INVENTORY_START+j/*+TABCELL_INVENTORY_NUM*i*/);

				// 090824 ShinJS --- ���̴� ������ �Ǵ��Ͽ� ���� �ľ��� �� �ֵ��� ����
				if(pItem->GetItemIdx() == wItemIdx)
				{
					DURTYPE dur = ITEMMGR->IsDupItem( pItem->GetItemIdx() ) ? pItem->GetDurability() : 1;
					DurSum += dur;
				}
			}
		}
	}

	return DurSum;
}
// 0(all),1(inv),2
BOOL CInventoryExDialog::GetPositionForXYRef(BYTE param, LONG x, LONG y, POSTYPE& pos)
{
	WORD position=0;										// ��� ��ġ

	if(param != 2)
	{
		cIconGridDialog * gridDlg = (cIconGridDialog *)GetTabSheet(GetCurTabNum());
		if(gridDlg->GetPositionForXYRef(x, y, position))
		{
			pos = position+TP_INVENTORY_START+TABCELL_INVENTORY_NUM*GetCurTabNum();
			return TRUE;
		}
	}

	if(param != 1)
	if(m_pWearedDlg->GetPositionForXYRef(x, y, position))
	{
//		pos = position+WEAR_STARTPOSITION;					// -> ���� ��ġ�� �ٲ�
		pos = position+TP_WEAR_START;					// -> ���� ��ġ�� �ٲ�
		return TRUE;
	}
	
	return FALSE;
}

void CInventoryExDialog::SetMoney(DWORDEX Money)
{
	m_pMoneyEdit->SetStaticText(AddComma(Money));
}

BOOL CInventoryExDialog::IsExist(POSTYPE abs_pos)
{
	BOOL bExist = FALSE;
	if(TP_INVENTORY_START <= abs_pos && abs_pos < (TP_INVENTORY_END + ( TABCELL_INVENTORY_NUM * HERO->Get_HeroExtendedInvenCount() )))
	{
		int tabIndex = GetTabIndex( abs_pos );		
		cIconGridDialog * dlg = (cIconGridDialog *)GetTabSheet(tabIndex);
		if(dlg->GetIconForIdx(abs_pos-TABCELL_INVENTORY_NUM*tabIndex-TP_INVENTORY_START))
			bExist = TRUE;
	}
	else if(TP_WEAR_START <= abs_pos && abs_pos < TP_WEAR_END)
	{
		if( m_pWearedDlg->GetIconForIdx(abs_pos - TP_WEAR_START) )
			bExist = TRUE;
	}

	return bExist;
}

// 080703 LUJ, �̵����� ������. �ߺ� �ڵ带 �����ϰ� ������.
BOOL CInventoryExDialog::CanBeMoved( CItem* pItem, POSTYPE toPos )
{
	if( ! pItem )
	{
		return FALSE;
	}

	const POSTYPE		fromPos				= pItem->GetPosition();
	const ITEM_INFO*	movingItemInfo		= pItem->GetItemInfo();

	if( ! movingItemInfo )
	{
		return FALSE;
	}	
	else if( TP_WEAR_START <= toPos && toPos < TP_WEAR_END )
	{
		// 080410 LUJ, â�� -> ��� �̵��� ���´�
		switch( ITEMMGR->GetTableIdxForAbsPos( fromPos ) )
		{
		case eItemTable_Storage:
		case eItemTable_Shop:
		case eItemTable_MunpaWarehouse:
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 76 ) );
				return FALSE;
			}
		}

		if( pItem->GetItemBaseInfo().nSealed == eITEM_TYPE_SEAL ) 
		{
			WINDOWMGR->MsgBox( MBI_SEALITEM_CANTEQUIP, MBT_OK, CHATMGR->GetChatMsg( 1177 ) );
			return FALSE ;
		}
		// 080703 LUJ, ���� ���Կ� �������� �ִ� ��� ������ ��ġ�ϴ��� �˻��Ѵ�
		else if( TP_WEAR_START == toPos )
		{
			// 080703 LUJ, ���⸸ ���� ����
			if( ePartType_OneHand	!= movingItemInfo->Part3DType	&&
				ePartType_TwoHand	!= movingItemInfo->Part3DType   &&
				ePartType_TwoBlade	!= movingItemInfo->Part3DType   )
			{
				return FALSE;
			}

			CItem*				shieldItem		= GetItemForPos( TP_WEAR_START + eWearedItem_Shield );
			const ITEM_INFO*	shieldItemInfo	= ( shieldItem ? shieldItem->GetItemInfo() : 0 );

			// 080703 LUJ, ���� ���Կ� �������� ����
			if( shieldItemInfo )
			{
				// 080703 LUJ, ��� ������ ��� �翬 ����
				if( ePartType_TwoHand == movingItemInfo->Part3DType ||
					ePartType_TwoBlade == movingItemInfo->Part3DType )
				{
					// 090729 ShinJS --- ���� ���н� �����޼��� ���
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(772) );
					return FALSE;
				}
				// 080703 LUJ, ���а� �ƴϰ� ���Ⱑ ���� ��� ������ ��ġ�ؾ� ���� ����
				else if(	shieldItemInfo->Part3DType	!= ePartType_Shield		&&
							shieldItemInfo->EquipType	!= movingItemInfo->EquipType )
				{
					return FALSE;
				}
			}
		}
		// 080703 LUJ, ���� ���Կ� ���⸦ ������ �� �ֵ��� üũ�Ѵ�
		else if( TP_WEAR_START + eWearedItem_Shield == toPos )
		{
			CItem*				weaponItem			= GetItemForPos( TP_WEAR_START + eWearedItem_Weapon );
			const ITEM_INFO*	weaponItemInfo		= ( weaponItem ? weaponItem->GetItemInfo() : 0 );
			const BOOL			isEnableTwoBlade	= HERO->GetHeroTotalInfo()->bUsingTwoBlade;

			// 080703 LUJ, ���� ���Կ� �������� �ִ� ���, ���� ������ ������ �� �ִ�
			if( weaponItemInfo )
			{
				// 080703 LUJ, ��� ����� ���� �Ұ�
				if( ePartType_TwoHand == weaponItemInfo->Part3DType ||
					ePartType_TwoHand == movingItemInfo->Part3DType )
				{
					return FALSE;
				}				
				// 080703 LUJ, �̵��� �����Ͱ� ���µ� ���⸦ �����Ϸ� �� ���
				else if( ePartType_Shield != movingItemInfo->Part3DType )
				{
					if( ! isEnableTwoBlade )
					{
						// 080714 LUJ, ���ҽ��� �߸� ������Ʈ�Ǿ� �޽��� ��ȣ�� ����
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1591 ) );
						return FALSE;
					}
					else if( movingItemInfo->WeaponType	!= weaponItemInfo->WeaponType )
					{
						// 080714 LUJ, ���ҽ��� �߸� ������Ʈ�Ǿ� �޽��� ��ȣ�� ����
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1592 ) );
						return FALSE;
					}
					else if( movingItemInfo->WeaponType == eWeaponType_Staff )
					{
						// 080714 LUJ, ���ҽ��� �߸� ������Ʈ�Ǿ� �޽��� ��ȣ�� ����
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1593 ) );
						return FALSE;
					}
					// 090910 pdy ���н��� ���������� äũ �߰� 
					// ���⽽��->���н��� �̵��ǰ�� ���н����� ���Ⱑ �������Ⱑ �ƴϸ� �Ұ�
					else if( TP_WEAR_START + eWearedItem_Weapon == fromPos)
					{
						CItem*				ShieldItem			= GetItemForPos( TP_WEAR_START + eWearedItem_Shield );
						const ITEM_INFO*	ShieldItemInfo		= ( ShieldItem ? ShieldItem->GetItemInfo() : 0 );

						if( ! ShieldItemInfo )
						{
							CHATMGR->AddMsg(
							CTC_SYSMSG,
							CHATMGR->GetChatMsg( 1594 ) );

							return FALSE;
						}
						else if( movingItemInfo->WeaponType !=  ShieldItemInfo->WeaponType )
						{
							return FALSE;
						}
					}
				}
			}
			// 080703 LUJ, ���� ������ �� ��� ���и� ������ �� �ִ�
			else if( ePartType_Shield != movingItemInfo->Part3DType )
			{
				CHATMGR->AddMsg(
					CTC_SYSMSG,
					// 080714 LUJ, ���ҽ��� �߸� ������Ʈ�Ǿ� �޽��� ��ȣ�� ����
					isEnableTwoBlade ? CHATMGR->GetChatMsg( 1594 ) : CHATMGR->GetChatMsg( 1591 ) );
				
				return FALSE;
			}
		}
		else
                {
                        const EWEARED_ITEM targetSlot = static_cast<EWEARED_ITEM>( toPos - TP_WEAR_START );

                        if( movingItemInfo->EquipSlot == eWearedItem_Earring1 )
                        {
                                if( targetSlot != eWearedItem_Earring1 && targetSlot != eWearedItem_Earring2 )
                                {
                                        return FALSE;
                                }
                        }
                        else if( movingItemInfo->EquipSlot == eWearedItem_Ring1 )
                        {
                                if( targetSlot != eWearedItem_Ring1 && targetSlot != eWearedItem_Ring2 )
                                {
                                        return FALSE;
                                }
                        }
                        else if( eWearedItem_Card_Weapon1 <= movingItemInfo->EquipSlot &&
                                movingItemInfo->EquipSlot <= eWearedItem_Card_Shoes2 )
                        {
                                switch( movingItemInfo->EquipSlot )
                                {
                                case eWearedItem_Card_Weapon1:
                                case eWearedItem_Card_Weapon2:
                                        if( targetSlot != eWearedItem_Card_Weapon1 && targetSlot != eWearedItem_Card_Weapon2 )
                                        {
                                                return FALSE;
                                        }
                                        break;
                                case eWearedItem_Card_Glove1:
                                case eWearedItem_Card_Glove2:
                                        if( targetSlot != eWearedItem_Card_Glove1 && targetSlot != eWearedItem_Card_Glove2 )
                                        {
                                                return FALSE;
                                        }
                                        break;
                                case eWearedItem_Card_Shoes1:
                                case eWearedItem_Card_Shoes2:
                                        if( targetSlot != eWearedItem_Card_Shoes1 && targetSlot != eWearedItem_Card_Shoes2 )
                                        {
                                                return FALSE;
                                        }
                                        break;
                                default:
                                        if( targetSlot != movingItemInfo->EquipSlot )
                                        {
                                                return FALSE;
                                        }
                                        break;
                                }
                        }
                        else if( movingItemInfo->EquipSlot != targetSlot )
                        {
                                return FALSE;
                        }
                }

		// 090114 LYW --- InvantoryExDialog : ������ ���� ���� üũ �Լ� ������ ���� ó��.
		const BYTE byEnableEquip = ITEMMGR->CanEquip(pItem->GetItemIdx() ) ;
		if( byEnableEquip != eEquip_Able )
		{
			switch( byEnableEquip )
			{
				case eEquip_RaceFail :			
					{
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(823), movingItemInfo->ItemName) ;
					}
					break ;

				case eEquip_GenderFail :		
					{
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(824), movingItemInfo->ItemName) ;
					}
					break ;

				case eEquip_LevelFail :			
					{
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(825), movingItemInfo->ItemName) ;	
					}
					break ;

				case eEquip_FishingLevelFail :	
					{
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1530)) ;
					}
					break ;
				case eEquip_CookingLevelFail :
					{
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1661 ) );
					}
					break;

				case eEquip_Disable : 
				default :				
					{
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(73)) ;								
					}
					break ;
			}

			return FALSE;
		}
	}
	// 080703 LUJ, ���� ���Կ��� �������� ���� ��, �̵��� ������ ��� �� �� ����
	//				���� ���Կ��� ���Ⱑ ���� ��� �ִϸ��̼��� �ſ� ����ϴ�
	else if( fromPos == TP_WEAR_START + eWearedItem_Weapon )
	{
		CItem* item = GetItemForPos( TP_WEAR_START + eWearedItem_Shield );

		if( item &&
			item->GetItemInfo()->WeaponType == movingItemInfo->WeaponType )
		{
			if(movingItemInfo->WeaponType == eWeaponType_CookUtil)
			{
				// NYJ - �丮������ ������ �޼����� ����Ѵ�.
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1811 ) );
				return FALSE;
			}
			else
			{
				// 080714 LUJ, ���ҽ��� �߸� ������Ʈ�Ǿ� �޽��� ��ȣ�� ����
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1595 ) );
				return FALSE;
			}
		}
	}
	// 071210 LYW --- InventoryExDialog : �κ��丮 Ȯ�忡 ���� ó���� �Ѵ�.
	else if( TP_INVENTORY_START<= toPos && toPos < ( TP_INVENTORY_END + ( TABCELL_INVENTORY_NUM * HERO->Get_HeroExtendedInvenCount() ) ) )
	{
		return TRUE;
	}	
	else if( TP_STORAGE_START <= toPos && toPos < TP_STORAGE_END )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

void CInventoryExDialog::ItemUseForQuestStart()
{
	if( m_pTempQuestItem )
	{
		// quest �������� ���
		MSG_ITEM_USE_SYN msg;
		msg.Category = MP_ITEM;
		msg.Protocol = MP_ITEM_USE_FOR_QUESTSTART_SYN;
		msg.dwObjectID = HEROID;
		msg.TargetPos = m_pTempQuestItem->GetPosition();
		msg.wItemIdx = m_pTempQuestItem->GetItemIdx();

		NETWORK->Send(&msg,sizeof(msg));
	}
}





//---------------------------------------------------------------------------------------------------------------------------
//	071114 LYW --- IventoryExDialog : ������ ���� ���� ��û.
//---------------------------------------------------------------------------------------------------------------------------
void CInventoryExDialog::ItemUnSealingSync()
{
	CItem* pItem = NULL ;
	pItem = GetCurSelectedItem(eItemTable_Inventory) ;

	if( !pItem ) return ;

	const ITEMBASE& pItemBase = pItem->GetItemBaseInfo() ;

	if( pItemBase.nSealed != eITEM_TYPE_SEAL ) return ;	
	
	if( pItem->IsLocked() ) return ;

	MSG_ITEM_USE_SYN msg;
	msg.Category = MP_ITEM ;
	msg.Protocol = MP_ITEM_USE_SYN ;
	msg.dwObjectID = HEROID ;
	msg.wItemIdx = pItem->GetItemIdx() ;
	msg.TargetPos = pItem->GetPosition() ;

	NETWORK->Send(&msg,sizeof(msg)) ;
}

void CInventoryExDialog::ItemUnSealing(POSTYPE absPos)
{
	CItem* const pItem = ITEMMGR->GetItemofTable(
		eItemTable_Inventory,
		absPos);

	if( !pItem ) return ;

	pItem->SetItemParam( eITEM_PARAM_UNSEAL ) ;

	// ������ �ٽ� �����ش�.
	ITEM_INFO* pItemInfo = NULL ;
	pItemInfo = ITEMMGR->GetItemInfo( pItem->GetItemIdx() ) ;

	if( !pItemInfo ) return ;

	ITEMMGR->AddToolTip( pItem ) ;

	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(366) ) ;
}

void CInventoryExDialog::CheckBackground() 
{
	CHero* pHero = HERO ;

	if(pHero == NULL ) return ;

	CHARACTER_TOTALINFO* pTotalInfo = pHero->GetCharacterTotalInfo() ;
	
	if(pTotalInfo)
	{
		BYTE byRacial = pTotalInfo->Race ;
		BYTE byGender = pTotalInfo->Gender ;

		for( BYTE count = 0 ; count < MAX_BACKGROUND ; ++count )
		{
			m_pBackground[count]->SetActive( FALSE ) ;
		}

		// 090521 ONS �ű����� �߰��� ���� backgroundó������
		if( byRacial == RaceType_Human )
		{
			if( !byGender )
			{
				m_pBackground[0]->SetActive( TRUE ) ;
			}
			else
			{
				m_pBackground[1]->SetActive( TRUE ) ;
			}
		}
		else if( byRacial == RaceType_Elf )
		{
			if( !byGender )
			{
				m_pBackground[2]->SetActive( TRUE ) ;
			}
			else
			{
				m_pBackground[3]->SetActive( TRUE ) ;
			}
		}
		else
		{
			if( !byGender )
			{
				m_pBackground[4]->SetActive( TRUE ) ;
			}
			else
			{
				m_pBackground[5]->SetActive( TRUE ) ;
			}
		}
	}
}


void CInventoryExDialog::SetCoolTime( DWORD itemIndex, DWORD miliSecond )
{
	const DWORD groupIndex = ITEMMGR->GetCoolTimeGroupIndex( itemIndex );

	for( BYTE tabIndex = 0; tabIndex < GetTabNum(); ++tabIndex )
	{
		cIconGridDialog* tab = ( cIconGridDialog* )GetTabSheet( tabIndex );
		
		if( ! tab )
		{
			continue;
		}

		for( WORD iconIndex = 0; iconIndex < tab->GetCellNum(); ++iconIndex )
		{
			cIcon* icon = tab->GetIconForIdx( iconIndex );

			if(		icon &&
				(	icon->GetData() == itemIndex ||
					groupIndex && ITEMMGR->GetCoolTimeGroupIndex( icon->GetData() ) == groupIndex ) )
			{
				icon->SetCoolTime( miliSecond );
			}
		}
	}
}

void CInventoryExDialog::SetCoolTimeAbs( DWORD itemIndex, DWORD dwBegin, DWORD dwEnd, DWORD miliSecond )
{
	const DWORD groupIndex = ITEMMGR->GetCoolTimeGroupIndex( itemIndex );

	for( BYTE tabIndex = 0; tabIndex < GetTabNum(); ++tabIndex )
	{
		cIconGridDialog* tab = ( cIconGridDialog* )GetTabSheet( tabIndex );
		
		if( ! tab )
		{
			continue;
		}

		for( WORD iconIndex = 0; iconIndex < tab->GetCellNum(); ++iconIndex )
		{
			cIcon* icon = tab->GetIconForIdx( iconIndex );

			if(		icon &&
				(	icon->GetData() == itemIndex ||
					groupIndex && ITEMMGR->GetCoolTimeGroupIndex( icon->GetData() ) == groupIndex ) )
			{
				icon->SetCoolTimeAbs( dwBegin, dwEnd, miliSecond );
			}
		}
	}
}


void CInventoryExDialog::Send()
{
	MSG_ITEM_USE_SYN message( mMessage );

	NETWORK->Send( &message, sizeof( message ) );
}


void CInventoryExDialog::Restore()
{
	SetDisable( FALSE );

	CItem* item = GetItemForPos( mMessage.TargetPos );

	if( item )
	{
		item->SetCoolTime( 0 );

		cQuickSlotDlg* dialog = ( cQuickSlotDlg* )WINDOWMGR->GetWindowForID( QI1_QUICKSLOTDLG );
		ASSERT( dialog );

		dialog->SetCoolTime( item->GetItemIdx(), 0 );


		// 080706 LYW --- InventoryExDialog : Ȯ�� �� ���� �߰� ó��.
		cQuickSlotDlg* dialog2 = NULL ;
		dialog2 = ( cQuickSlotDlg* )WINDOWMGR->GetWindowForID( QI2_QUICKSLOTDLG ) ;
		ASSERT(dialog2) ;

		if(dialog2)
		{
			dialog2->SetCoolTime( item->GetItemIdx(), 0 ) ;
		}
	}
}






// 071210 LYW --- CInventoryExDialog : Ȯ�� �κ��丮�� Ȱ��ȭ ���θ� �����ϴ� �Լ� �߰�.
void CInventoryExDialog::InitExtended_Inventory( CHARACTER_TOTALINFO* pInfo )
{
	if( !pInfo ) return ;

	if( pInfo->wInventoryExpansion == 0 ) return ;

	if( pInfo->wInventoryExpansion == 1 )
	{
		ShowTab(2,TRUE) ;

		HERO->Set_HeroExtendedInvenCount(1) ;
	}
	else if( pInfo->wInventoryExpansion == 2 )
	{
		ShowTab(2,TRUE) ;
		ShowTab(3,TRUE) ;

		HERO->Set_HeroExtendedInvenCount(2) ;
	}
}
// 080213 KTH -- ClearInventory
BOOL CInventoryExDialog::ClearInventory()
{
	for( int i = TP_INVENTORY_START; i < (TP_INVENTORY_END + (TABCELL_INVENTORY_NUM * HERO->Get_HeroExtendedInvenCount())); i++ )
	{
			ITEMMGR->DeleteItemofTable( eItemTable_Inventory, i, false );
	}

	return TRUE;
}

void CInventoryExDialog::SetMultipleItemLock(DWORD dwItemIdx, BOOL bVal)
{
	for( BYTE tabIndex = 0; tabIndex < GetTabNum(); ++tabIndex )
	{
		cIconGridDialog* tab = ( cIconGridDialog* )GetTabSheet( tabIndex );
		
		if( ! tab )
		{
			continue;
		}

		for( WORD iconIndex = 0; iconIndex < tab->GetCellNum(); ++iconIndex )
		{
			cIcon* icon = tab->GetIconForIdx( iconIndex );

			if(icon && icon->GetData()==dwItemIdx)
			{
				icon->SetLock(bVal);
			}
		}
	}
}

void CInventoryExDialog::PetGradeUp()
{
	SetCoolTime( mMessage.wItemIdx, WAITMILISECOND );

	{
		cQuickSlotDlg* dialog = ( cQuickSlotDlg* )WINDOWMGR->GetWindowForID( QI1_QUICKSLOTDLG );
		ASSERT( dialog );

		dialog->SetCoolTime( mMessage.wItemIdx, WAITMILISECOND );
	}

	{
		CProgressDialog* dialog = ( CProgressDialog* )WINDOWMGR->GetWindowForID( PROGRESS_DIALOG );
		ASSERT( dialog );

		dialog->SetText( RESRCMGR->GetMsg( 1008 ) );
		dialog->Wait( CProgressDialog::eActionPetGradeUp );
	}
}

void CInventoryExDialog::StartVehicleSummon( const VECTOR3& vecPos )
{
	CItem* pItem = Get_QuickSelectedItem();
	if( !pItem )		return;

	// ��Ÿ�� ����
	SetCoolTime( pItem->GetItemIdx(), WAITMILISECOND );

	cQuickSlotDlg* pQuickSlotDlg = ( cQuickSlotDlg* )WINDOWMGR->GetWindowForID( QI1_QUICKSLOTDLG );
	ASSERT( pQuickSlotDlg );
	if( pQuickSlotDlg )
		pQuickSlotDlg->SetCoolTime( pItem->GetItemIdx(), WAITMILISECOND );

	// Progress ����
	CProgressDialog* pProgressDlg = ( CProgressDialog* )WINDOWMGR->GetWindowForID( PROGRESS_DIALOG );
	ASSERT( pProgressDlg );
	if( pProgressDlg )
	{
		pProgressDlg->SetVehicleSummonPos( vecPos );						// ��ġ ��ġ����
		pProgressDlg->SetText(  RESRCMGR->GetMsg( 1006 ) );					// InterfaceMsg : ��ȯ�մϴ�
		pProgressDlg->Wait( CProgressDialog::eActionVehicleSummon );		// Ż�ͼ�ȯProgress ����		
	}
}

void CInventoryExDialog::StartVehicleSeal( DWORD dwVehicleID )
{
	// Progress ����
	CProgressDialog* pProgressDlg = ( CProgressDialog* )WINDOWMGR->GetWindowForID( PROGRESS_DIALOG );
	ASSERT( pProgressDlg );
	if( pProgressDlg )
	{
		pProgressDlg->SetText(  RESRCMGR->GetMsg( 1007 ) );					// InterfaceMsg : �����մϴ�
		pProgressDlg->Wait( CProgressDialog::eActionVehicleSeal );			// Ż�ͺ���Progress ����
	}
}

void CInventoryExDialog::RefreshArmorItem()
{
	 if(m_pWearedDlg)
	 {
		 m_pWearedDlg->RefreshArmorItem();
	 }
}