// QuestExecute_RandomItem.cpp: implementation of the CQuestExecute_RandomItem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"																		// ǥ�� �ý��۰�, ������Ʈ�� ������, ���־��� �ش����� ���� �ش������� �ҷ��´�.
#include "QuestExecute_SelectItem.h"													// ����Ʈ ���� ���� ������ ����� �ҷ��´�.

#include "QuestScriptLoader.h"															// ����Ʈ ��ũ��Ʈ �δ� ����� �ҷ��´�.

#ifdef _MAPSERVER_																		// �� ������ ����Ǿ� �ִٸ�,

#include "QuestGroup.h"																	// ����Ʈ �׷� ����� �ҷ��´�.

#include "ItemSlot.h"

#include "Player.h"

#include "ItemManager.h"

#include "Quest.h"																// ����Ʈ ����� �ҷ��´�.

#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
// ������ �Լ�.
CQuestExecute_SelectItem::CQuestExecute_SelectItem( DWORD dwExecuteKind, CStrTokenizer* pTokens, DWORD dwQuestIdx, DWORD dwSubQuestIdx )
: CQuestExecute( dwExecuteKind, pTokens, dwQuestIdx, dwSubQuestIdx )
{
	GetScriptParam( m_dwMaxItemCount, pTokens );										// ������ �ִ� ���� �޴´�.

	m_pSelectItem = new SELECTITEM[m_dwMaxItemCount];

	m_dwQuestIdx = dwQuestIdx;
	m_dwSubQuestIdx = dwSubQuestIdx;

	for( DWORD i = 0; i < m_dwMaxItemCount; ++i )										// �ִ� ������ �� ��ŭ for���� ������.
	{
		GetScriptParam( m_pSelectItem[i].dwItemIdx, pTokens );							// ������ ������ �޴´�.
		GetScriptParam( m_pSelectItem[i].dwItemNum, pTokens );							// ������ ���� �޴´�.
	}
}

CQuestExecute_SelectItem::~CQuestExecute_SelectItem()									// �Ҹ��� �Լ�.
{
	SAFE_DELETE_ARRAY(
		m_pSelectItem);
}

// ���� �Լ�.
BOOL CQuestExecute_SelectItem::Execute( PLAYERTYPE* pPlayer, CQuestGroup* pQuestGroup, CQuest* pQuest )
{
#ifdef _MAPSERVER_																		// �� ������ ����Ǿ� �ִٸ�,

	if( eQuestExecute_TakeSelectItem == m_dwExecuteKind )															// ���� ������ Ȯ���Ѵ�.
	{
		return pQuestGroup->TakeSelectItem( pPlayer, pQuestGroup, m_dwQuestIdx, m_dwSubQuestIdx );
	}
	
#endif	

	return TRUE;																		// TRUE�� �����Ѵ�.
}

int CQuestExecute_SelectItem::CheckCondition( PLAYERTYPE* pPlayer,				
							CQuestGroup* pQuestGroup, CQuest* pQuest )					
{
	int nErrorCode = e_EXC_ERROR_NO_ERROR ;												// ���� �ڵ带 ���� ������ �����ϰ� e_EXE_ERROR_NO_ERROR�� �����Ѵ�.

	if( !pPlayer )																		// �÷��̾� ������ ��ȿ���� ������, 
	{
		nErrorCode = e_EXC_ERROR_NO_PLAYERINFO ;										// �÷��̾� ������ ���ٴ� ���� �ڵ带 �����Ѵ�.
		return nErrorCode ;																// ���� �ڵ带 return ó���� �Ѵ�.
	}

	if( !pQuestGroup )																	// ����Ʈ �׷� ������ ��ȿ���� ������,
	{
		nErrorCode = e_EXC_ERROR_NO_QUESTGROUP ;										// ����Ʈ �׷� ������ ���ٴ� ���� �ڵ带 �����Ѵ�.
		return nErrorCode ;																// ���� �ڵ带 return ó���� �Ѵ�.
	}

	if( !pQuest )																		// ����Ʈ ������ ��ȿ���� �ʴٸ�,
	{
		nErrorCode = e_EXC_ERROR_NO_QUEST ;												// ����Ʈ ������ ���ٴ� ���� �ڵ带 �����Ѵ�.
		return nErrorCode ;																// ���� �ڵ带 return ó���� �Ѵ�.
	}

#ifdef _MAPSERVER_																		// �� ������ ����Ǿ� �ִٸ�,

	switch( m_dwExecuteKind )															// ���� ������ Ȯ���Ѵ�.
	{
	case eQuestExecute_TakeSelectItem:													// ���� �������� �޴� �����̸�,
		{
			// 1. ����Ʈ �׷��������� ��������� ������ 
			const REQUITALINFO* pInfo = pQuestGroup->FindRequitalIndex( pQuest->GetQuestIdx() );
			if( !pInfo )
			{
				nErrorCode = e_EXC_ERROR_NOT_EXIST_REQUITAL;
				return nErrorCode;
			}

			nErrorCode = e_EXC_ERROR_NOT_EXIST_REQUITAL;
			for( DWORD n = 0; n < m_dwMaxItemCount; n++)
			{
				if( m_pSelectItem[n].dwItemIdx != pInfo->dwRequitalIndex )
					continue;
				if( m_pSelectItem[n].dwItemNum != pInfo->dwRequitalCount )
					continue;
	
				nErrorCode = e_EXC_ERROR_NO_ERROR;
			}
			if( nErrorCode == e_EXC_ERROR_NOT_EXIST_REQUITAL )
			{
				return nErrorCode;
			}

			// ������ ���� ������ �޴´�.
			CItemSlot* pSlot = NULL ;
			pSlot = pPlayer->GetSlot(eItemTable_Inventory) ;

			if( !pSlot )
			{
				nErrorCode = e_EXC_ERROR_NO_INVENTORYINFO ;
				return nErrorCode ;
			}

			WORD EmptyCellPos[255];											
			WORD EmptyCellNum;												
			WORD wResult = 0 ;												
			wResult = ITEMMGR->CheckExtraSlot(pPlayer, pSlot, pInfo->dwRequitalIndex, pInfo->dwRequitalCount, EmptyCellPos, EmptyCellNum);
			if( wResult == 0 )								
			{
				nErrorCode = e_EXC_ERROR_NO_INVENTORYINFO ;	
				return nErrorCode ;							
			}
		}
		break;
	}

#endif //_MAPSERVER_

	return nErrorCode ;																	// �⺻ ���� �����Ѵ�.
}

