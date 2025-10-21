// BuyRegDialog.cpp: implementation of the CBuyRegDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BuyRegDialog.h"
#include "WindowIDEnum.h"
#include "./Interface\cEditBox.h"
#include "./Interface\cListDialog.h"
#include "./Interface\cButton.h"
#include "./Interface\cStatic.h"
#include "./Input/Mouse.h"
#include "./Interface/cWindowManager.h"
#include "ExchangeItem.h"
#include "MHFile.h"
#include "ItemManager.h"
#include "GameResourceStruct.h"
#include "cIMEex.h"
#include "ChatManager.h"
#include "cMsgBox.h"
#include "GameIn.h"
#include "StreetBuyStall.h"
#include "./Interface/cScriptManager.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBuyRegDialog::CBuyRegDialog()
{
	m_pTypeList		=	NULL;

	for(int i = 0; i < ITEM_TYPE_COUNT; i++)
		m_pItemList[i]	=	NULL;

	m_pClassList	=	NULL;
	m_pVolumeEdit	=	NULL;
	m_pMoneyEdit	=	NULL;
	m_pRegBtn		=	NULL;

	mMoney = 0;
	mColor = 0xffffffff;

	m_nType = 0;
	m_nItem = 0;

}

CBuyRegDialog::~CBuyRegDialog()
{

}

//////////////////////////////////////////////////////////////////////////
//���� ������ ����Ʈ �ε�
void CBuyRegDialog::LoadItemList()
{
	CMHFile fp;
	
	if( !fp.Init( ".\\System\\Resource\\BRTList.bin", "rb" ) )	return;	
	
	// 100121 pdy MAX_LISTITEM_SIZE ��ŭ ���۸� �⵵�� ����
	char buf[MAX_LISTITEM_SIZE] = {0,};

	while(1)
	{
		fp.GetString(buf);

		m_pTypeList->AddItem(buf, RGB_HALF(255, 255, 255));
		
		if(fp.IsEOF())
			break;
	}

	m_pTypeList->ResetGuageBarPos();

	fp.Release();
	
	if( !fp.Init( ".\\System\\Resource\\BRList.bin", "rb" ) )	return;	

	// 090114 LUJ, ��ȯ �Ұ� �������� ǥ������ �ʵ��� ��
	while( ! fp.IsEOF() )
	{
		WORD nType = fp.GetWord();
		fp.GetBool(); // class
		DWORD nItem = fp.GetDword();

		const ITEM_INFO* const itemInfo = ITEMMGR->GetItemInfo( nItem );

		if( ! itemInfo )
		{
			continue;
		}
		else if( ! itemInfo->Trade )
		{
			continue;
		}
		else if( ITEM_TYPE_COUNT < nType )
		{
			continue;
		}

		cListDialog* dialog = m_pItemList[ nType - 1 ];

		if( dialog )
		{
			dialog->AddItem( itemInfo->ItemName, RGB_HALF( 255, 255, 255 ) );
		}
	}

	int i;
	for(i = 0; i < ITEM_TYPE_COUNT; i++)
		m_pItemList[i]->ResetGuageBarPos();

	fp.Release();

	for(i = 0; i < 10; i++)
	{
		wsprintf(buf, "+%d", i);
		m_pClassList->AddItem(buf, RGB_HALF(255, 255, 255));
	}

	m_pClassList->ResetGuageBarPos();
}
//////////////////////////////////////////////////////////////////////////


void CBuyRegDialog::Linking()
{
	m_pTypeList		= (cListDialog*)GetWindowForID(BRS_TYPELIST);
	
	// 091127 ONS ���Ż��� �����۸���Ʈ �߰�/����
	m_pItemList[WEAPON]		= (cListDialog*)GetWindowForID(BRS_ITEMLIST1);
	m_pItemList[CLOTHES]	= (cListDialog*)GetWindowForID(BRS_ITEMLIST2);
	m_pItemList[ACCESSORY]	= (cListDialog*)GetWindowForID(BRS_ITEMLIST3);
	m_pItemList[POTION]		= (cListDialog*)GetWindowForID(BRS_ITEMLIST4);
	m_pItemList[MATERIAL]	= (cListDialog*)GetWindowForID(BRS_ITEMLIST5);
	m_pItemList[PET]		= (cListDialog*)GetWindowForID(BRS_ITEMLIST6);
	m_pItemList[COSTUME]	= (cListDialog*)GetWindowForID(BRS_ITEMLIST7);
	m_pItemList[PRODUCTION]	= (cListDialog*)GetWindowForID(BRS_ITEMLIST8);
	m_pItemList[HOUSING]	= (cListDialog*)GetWindowForID(BRS_ITEMLIST9);
	m_pItemList[ETC]		= (cListDialog*)GetWindowForID(BRS_ITEMLIST10);

	m_pClassList	= (cListDialog*)GetWindowForID(BRS_CLASSLIST);
	m_pClassList->SetShowSelect(TRUE);
	
	m_pVolumeEdit	= (cEditBox*)GetWindowForID(BRS_VOLUMEEDIT);
	m_pVolumeEdit->SetValidCheck( VCM_NUMBER );
	m_pVolumeEdit->SetAlign( TXT_RIGHT );
	m_pVolumeEdit->SetReadOnly( FALSE );
	
	cImage ToolTipImage;
	SCRIPTMGR->GetImage( 0, &ToolTipImage, PFT_HARDPATH );
	m_pVolumeEdit->SetToolTip( CHATMGR->GetChatMsg(1440), RGBA_MAKE(255, 255, 255, 255), &ToolTipImage, TTCLR_DEFAULT );

	m_pMoneyEdit	= (cEditBox*)GetWindowForID(BRS_MONEYEDIT);
	m_pMoneyEdit->SetValidCheck( VCM_NUMBER );
	m_pMoneyEdit->SetAlign( TXT_RIGHT );
	m_pMoneyEdit->SetReadOnly( FALSE );
	
	m_pRegBtn		= (cButton*)GetWindowForID(BRS_REGBTN);
	m_pDltBtn		= (cButton*)GetWindowForID(BRS_DELETEBTN);

	m_pClassText	= (cStatic*)GetWindowForID(BRS_CLASSTEXT);
	m_pClassRect1	= (cStatic*)GetWindowForID(BRS_CLASSRECT1);
	m_pClassRect2	= (cStatic*)GetWindowForID(BRS_CLASSRECT2);
	
	m_pItemBigRect		= (cStatic*)GetWindowForID(BRS_ITEMBIGRECT);
	m_pItemSmallRect1	= (cStatic*)GetWindowForID(BRS_ITEMSMALLRECT1);
	m_pItemSmallRect2	= (cStatic*)GetWindowForID(BRS_ITEMSMALLRECT2);
	
	LoadItemList();
	InitRegInfo();
}

//////////////////////////////////////////////////////////////////////////
//������ ���� ����
void CBuyRegDialog::UpdateType()
{
	if(m_nType != m_pTypeList->GetCurSelectedRowIdx())
	{
		m_pItemList[m_nType]->SetActive(FALSE);					//���õǾ� �ִ� ������ ����Ʈ�� ��Ȱ��ȭ

		m_nType = WORD(m_pTypeList->GetCurSelectedRowIdx());
		m_nItem = 0;
		m_pItemList[m_nType]->SetCurSelectedRowIdx(m_nItem);

		m_pItemList[m_nType]->SetActive(TRUE);					//���õ� ������ ����Ʈ Ȱ��ȭ

		cPtrList* pList = m_pTypeList->GetListItem();
		PTRLISTPOS pos = pList->GetHeadPosition();
		
		while(pos)
		{
			ITEM* pItem = (ITEM*)pList->GetNext(pos);

			if( pItem == NULL )
				continue;

			if( pItem == m_pTypeList->GetItem(m_nType) )
			{
				m_pTypeList->GetItem(m_nType)->rgb = RGB_HALF(255, 255, 0);
				continue;
			}
			pItem->rgb = RGB_HALF(255, 255, 255);
		}

		pList = m_pItemList[m_nType]->GetListItem();
		pos = pList->GetHeadPosition();

		while(pos)
		{
			ITEM* pItem = (ITEM*)pList->GetNext(pos);

			if( pItem == NULL )
				continue;

			if( pItem == m_pItemList[m_nType]->GetItem(m_nItem) )
			{
				m_pItemList[m_nType]->GetItem(m_nItem)->rgb = RGB_HALF(255, 255, 0);
				continue;
			}
			pItem->rgb = RGB_HALF(255, 255, 255);
		}

		m_pClassList->SetActive(FALSE);
		m_nClass = 0;

		m_pClassText->SetActive(FALSE);
		m_pClassRect1->SetActive(FALSE);
		m_pClassRect2->SetActive(FALSE);
		
		m_pItemBigRect->SetActive(TRUE);
		m_pItemSmallRect1->SetActive(FALSE);
		m_pItemSmallRect2->SetActive(FALSE);

		m_nVolume = WORD(atoi( DEFAULT_VOLUME_TEXT ));
		m_pVolumeEdit->SetEditText( DEFAULT_VOLUME_TEXT );
		m_pVolumeEdit->SetDisable(FALSE);
		
		m_nMoney = 0;
		m_pMoneyEdit->SetEditText( DEFAULT_MONEY_TEXT );
	}
}
//////////////////////////////////////////////////////////////////////////

void CBuyRegDialog::UpdateItem()
{
	//������ ���õǾ��� �����۰� �ٸ� �������� ���� �Ǿ��ٸ�
	//���ð��� �ٲٰ� ��� ����Ʈ�� ���� �Ѵ�
	if(m_nItem != m_pItemList[m_nType]->GetCurSelectedRowIdx())
	{
		m_nItem = WORD(m_pItemList[m_nType]->GetCurSelectedRowIdx());
		m_nVolume = WORD(atoi( DEFAULT_VOLUME_TEXT ));
		
		for(int i = 0; i < ITEM_TYPE_COUNT; i++)
		{
			if(i == m_nType)
			{
				m_pItemList[i]->SetActive(TRUE);
				m_pItemList[i]->SetCurSelectedRowIdx(m_nItem);

				cPtrList* pList = m_pItemList[i]->GetListItem();
				PTRLISTPOS pos = pList->GetHeadPosition();
				
				while(pos)
				{
					ITEM* pItem = (ITEM*)pList->GetNext(pos);

					if( pItem == NULL )
						continue;

					if( pItem == m_pItemList[i]->GetItem(m_nItem) )
					{
						m_pItemList[i]->GetItem(m_nItem)->rgb = RGB_HALF(255, 255, 0);
						continue;
					}
					pItem->rgb = RGB_HALF(255, 255, 255);
				}
			}
			else
			{
				m_pItemList[i]->SetActive(FALSE);
			}
		}
		
		if(m_nType == ETC)
			m_nVolume = 1;
	
		m_nMoney = 0;
		m_pMoneyEdit->SetEditText( DEFAULT_MONEY_TEXT );
		m_pVolumeEdit->SetEditText( AddComma(m_nVolume) );
	}
}

void CBuyRegDialog::UpdateClass()
{
	if( m_nClass != m_pClassList->GetCurSelectedRowIdx() )
	{
		m_nClass = WORD(m_pClassList->GetCurSelectedRowIdx());
		m_nMoney = 0;
		m_pMoneyEdit->SetEditText( DEFAULT_MONEY_TEXT );
	}
}


void CBuyRegDialog::Show()
{
	SetActive(TRUE);
	GAMEIN->GetStreetBuyStallDialog()->SetDisable( TRUE );
	
	m_pTypeList->SetCurSelectedRowIdx(m_nType);

	m_pClassList->SetActive(FALSE);
	m_nClass = 0;

	m_pClassText->SetActive(FALSE);
	m_pClassRect1->SetActive(FALSE);
	m_pClassRect2->SetActive(FALSE);
	
	m_pItemBigRect->SetActive(TRUE);
	m_pItemSmallRect1->SetActive(FALSE);
	m_pItemSmallRect2->SetActive(FALSE);
	
	m_pVolumeEdit->SetDisable(FALSE);

	//���õ� ������ ����Ʈ�� Ȱ��ȭ
	for(int i = 0; i < ITEM_TYPE_COUNT; i++)
	{
		if(i == m_nType)
		{
			m_pItemList[i]->SetActive(TRUE);
		}
		else
		{
			m_pItemList[i]->SetActive(FALSE);
		}
	}
}

void CBuyRegDialog::InitRegInfo()
{
	// 091127 ONS ���� ����Ʈ ���û��¸� �ʱ�ȭ�Ѵ�.
	cPtrList* pList = m_pTypeList->GetListItem();
	PTRLISTPOS pos = pList->GetHeadPosition();
	while(pos)
	{
		ITEM* pItem = (ITEM*)pList->GetNext(pos);

		if( pItem == NULL )
			continue;

			if( pItem == m_pTypeList->GetItem(0) )
			{
				m_pTypeList->GetItem(0)->rgb = RGB_HALF(255, 255, 0);
				continue;
			}
			pItem->rgb = RGB_HALF(255, 255, 255);
	}

	// ������ ����Ʈ ���û��¸� �ʱ�ȭ�Ѵ�.
	pList = m_pItemList[m_nType]->GetListItem();
	pos = pList->GetHeadPosition();
	while(pos)
	{
		ITEM* pItem = (ITEM*)pList->GetNext(pos);

		if( pItem == NULL )
			continue;

		if( pItem == m_pItemList[m_nType]->GetItem(0) )
		{
			m_pItemList[m_nType]->GetItem(0)->rgb = RGB_HALF(255, 255, 0);
			continue;
		}
		pItem->rgb = RGB_HALF(255, 255, 255);
	}
	m_pItemList[m_nType]->SetCurSelectedRowIdx(0);


	m_nType = 0;
	m_nItem = 0;
	m_nClass = 0;
	m_nMoney = 0;
	m_nVolume = WORD(atoi( DEFAULT_VOLUME_TEXT ));

	m_pVolumeEdit->SetEditText( DEFAULT_VOLUME_TEXT );
	m_pMoneyEdit->SetEditText( DEFAULT_MONEY_TEXT );
}

void CBuyRegDialog::SetRegInfo(BUY_REG_INFO& RegInfo)
{
	m_nType = RegInfo.Type;
	m_nItem = RegInfo.Item;
	m_nClass = RegInfo.Class;
	m_nVolume = RegInfo.Volume;
	m_nMoney = RegInfo.Money;

	m_pVolumeEdit->SetEditText( AddComma( m_nVolume ) );
	m_pMoneyEdit->SetEditText( AddComma( m_nMoney ) );
}

void CBuyRegDialog::Close()
{
	InitRegInfo();

	SetActive(FALSE);
	GAMEIN->GetStreetBuyStallDialog()->SetDisable( FALSE );
}

void CBuyRegDialog::OnActionEvnet(LONG lId, void * p, DWORD we)
{
	if( we & WE_BTNCLICK && lId == BRS_REGBTN )
	{
		Close();
	}
	else if( we & WE_BTNCLICK && lId == BRS_CLOSEBTN )
	{
		Close();
	}
}

BOOL CBuyRegDialog::GetBuyRegInfo(BUY_REG_INFO& BuyInfo)
{
	m_nVolume = (WORD)atoi(RemoveComma(m_pVolumeEdit->GetEditText()));
	m_nMoney = (DWORDEX)_atoi64(RemoveComma(m_pMoneyEdit->GetEditText()));

	//090819 pdy ���ų����� ����� ������ ������ 40���̳��� ���ϰ� ó��
	unsigned long long NewMoney = _atoi64( RemoveComma( m_pMoneyEdit->GetEditText() ) )  * m_nVolume ;

	if( NewMoney  > MAX_INVENTORY_MONEY )
	{
		WINDOWMGR->MsgBox(MBI_BUYREG, MBT_OK, CHATMGR->GetChatMsg(1433));
		m_nMoney = 0;
		m_pMoneyEdit->SetEditText( DEFAULT_MONEY_TEXT );
		return FALSE;
	}

	if(m_nVolume < 1)	// ������ 1�� �̸��϶� ���� �޼���
	{
		WINDOWMGR->MsgBox(MBI_BUYREG, MBT_OK, CHATMGR->GetChatMsg(1436));
		return FALSE;
	}
	else if(m_nVolume > 2000)	// ������ 100�� �̻��϶� ���� �޼���
	{
		WINDOWMGR->MsgBox(MBI_BUYREG, MBT_OK, CHATMGR->GetChatMsg(1437));
		m_nVolume = 2000;
		m_pVolumeEdit->SetEditText( MAX_VOLUME_TEXT );
		return FALSE;
	}
	else if( m_nMoney < 1)	// �ݾ��� 0�� �����϶� ���� �޼���
	{
		WINDOWMGR->MsgBox(MBI_BUYREG, MBT_OK, CHATMGR->GetChatMsg(1432));
		m_nMoney = 0;
		m_pMoneyEdit->SetEditText( DEFAULT_MONEY_TEXT );
		return FALSE;
	}

	BuyInfo.Type = m_nType;
	BuyInfo.Item = m_nItem;
	BuyInfo.Class = m_nClass;
	BuyInfo.Volume = m_nVolume;
	BuyInfo.Money = m_nMoney;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// ���õ� ������� ������ ������ ã�� �Լ�
ITEM_INFO* CBuyRegDialog::GetSelectItemInfo()
{
	PTRLISTPOS pos = m_pItemList[m_nType]->GetListItem()->FindIndex(m_nItem);
	if(!pos)
		return NULL;
	ITEM* pItem = (ITEM*)(m_pItemList[m_nType]->GetListItem()->GetAt(pos));

	if(!pItem)
		return NULL;

	//����� ���� �������� ����� 0���� �ʱ�ȭ
	//Ȥ�ó� �ؼ� �ѹ���!
	// 080221 KTH -- �ּ�
/*	if(m_nType >= ACCESSORY)
		m_nClass = 0;*/

	if(m_nClass > 0 && m_nClass < 10)
	{
		char ItemName[128] = {0,};
		wsprintf(ItemName, "%s+%d", pItem->string, m_nClass);
		
		return ITEMMGR->FindItemInfoForName(ItemName);
	}
	else
		return ITEMMGR->FindItemInfoForName(pItem->string);
}
//////////////////////////////////////////////////////////////////////////

void CBuyRegDialog::Render()
{
	if( m_bActive )
	{
		if( mMoney != (DWORDEX)_atoi64(RemoveComma(m_pMoneyEdit->GetEditText())) )
		{
			mMoney = (DWORDEX)_atoi64(RemoveComma(m_pMoneyEdit->GetEditText()));;

			// 091112 ONS �ݾ�ǥ�� ���� ����
			mColor = GetMoneyColor(mMoney);
		}
	}
	
	m_pMoneyEdit->SetNonactiveTextColor( mColor );
	m_pMoneyEdit->SetActiveTextColor( mColor );
	cDialog::Render();
}