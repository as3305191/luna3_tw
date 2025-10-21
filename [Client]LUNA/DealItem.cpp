// DealItem.cpp: implementation of the CDealItem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DealItem.h"
#include "ItemManager.h"
#include "ChatManager.h"
#include "./Interface/cResourceManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDealItem::CDealItem()
{
	m_type = WT_DEALITEM;
}

CDealItem::~CDealItem()
{

}
DWORDEX CDealItem::GetSellPrice()
{
	ITEM_INFO * pItemInfo = ITEMMGR->GetItemInfo(m_wItemIdx);
	if(pItemInfo)
		return pItemInfo->SellPrice;
	return 0;
}

DWORDEX CDealItem::GetBuyPrice()
{
	ITEM_INFO * pItemInfo = ITEMMGR->GetItemInfo(m_wItemIdx);
	if(pItemInfo)
		return pItemInfo->BuyPrice;
	return 0;
}

DWORD CDealItem::GetBuyFishPoint()
{
	ITEM_INFO * pItemInfo = ITEMMGR->GetItemInfo(m_wItemIdx);
	if(pItemInfo)
		return pItemInfo->dwBuyFishPoint;
	return 0;
}

char* CDealItem::GetItemName()
{
	ITEM_INFO * pItemInfo = ITEMMGR->GetItemInfo(m_wItemIdx);
	if(pItemInfo)
		return pItemInfo->ItemName;
	return NULL;
}

// 100104 ShinJS --- Item Point Type�� ���� ���� �߰�
void CDealItem::AddPointTypeToolTip()
{
	char buf[MAX_PATH] = {0,};

	ITEM_INFO * pItemInfo = ITEMMGR->GetItemInfo(m_wItemIdx);
	if( !pItemInfo )	return;

	switch( pItemInfo->wPointType )
	{
	case eItemPointType_Item:
		{
			// Coin Item�� ������ ���Ѵ�
			DWORD dwCoinItemIdx = pItemInfo->dwPointTypeValue1;
			DWORD dwNeedCoinCnt = pItemInfo->dwPointTypeValue2;
			ITEM_INFO* pCoinItemInfo = ITEMMGR->GetItemInfo( dwCoinItemIdx );
			if( pCoinItemInfo )
			{
				wsprintf( buf, CHATMGR->GetChatMsg(1909), pCoinItemInfo->ItemName, AddComma( dwNeedCoinCnt ) );
				AddToolTipLine( buf, TTTC_BUYPRICE );
			}
		}
		break;

	case eItemPointType_PCRoomPoint:
		{
			DWORD dwNeedPCRoomPoint = pItemInfo->dwPointTypeValue1;
			wsprintf( buf, RESRCMGR->GetMsg( 1219 ), AddComma( dwNeedPCRoomPoint ) ); // "�ʿ� PC�� ����Ʈ : %s"
			AddToolTipLine( buf, TTTC_BUYPRICE );
		}
		break;
	}
	
}