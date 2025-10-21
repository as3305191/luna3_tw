
// SubQuestInfo.cpp: implementation of the CSubQuestInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"																			// ǥ�� �ý��۰�, ������Ʈ�� ������, ���־��� �ش����� ���� �ش������� �ҷ��´�.
#include "SubQuestInfo.h"																	// ���� ����Ʈ ���� �ش��� �ҷ��´�.

#include "QuestLimit.h"																		// ����Ʈ ���� ����� �ҷ��´�.
#include "QuestTrigger.h"																	// ����Ʈ Ʈ���� ����� �ҷ��´�.
#include "QuestNpcScript.h"																	// ����Ʈ npc ��ũ��Ʈ ����� �ҷ��´�.
#include "QuestNpc.h"																		// ����Ʈ npc ����� �ҷ��´�.
#include "QuestExecute.h"
#include "QuestManager.h"

#ifdef _MAPSERVER_
	#include "Quest.h"
	#include "QuestGroup.h"
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSubQuestInfo::CSubQuestInfo( DWORD dwQuestIdx, DWORD dwSubQuestIdx )
{
	m_dwQuestIdx = dwQuestIdx;
	m_dwSubQuestIdx = dwSubQuestIdx;
	m_dwMaxCount = 0;																		// �ִ� ī��Ʈ�� 0���� �����Ѵ�.
}

CSubQuestInfo::~CSubQuestInfo()																// �Ҹ��� �Լ�.
{
	Release();																				// ���� �Լ��� ȣ���Ѵ�.
}

void CSubQuestInfo::Release()																// ���� �Լ�.
{
	CQuestLimit* pLimit = NULL;																// ����Ʈ ���� ������ ���� ������ ������ �����ϰ� null ó���� �Ѵ�.

	PTRLISTPOS pos = m_QuestLimitList.GetHeadPosition();									// ����Ʈ ���� ����Ʈ�� ���� �����ϰ� ��ġ�� �޴´�.

	while( pos )																			// ��ġ ������ ��ȿ�� ���� while���� ������.
	{
		pLimit = (CQuestLimit*)m_QuestLimitList.GetNext( pos );								// ��ġ ������ �ش��ϴ� ����Ʈ ���� ������ �޴´�.

		if( pLimit )																		// ����Ʈ ���� ������ ��ȿ���� üũ�Ѵ�.
		{
			delete pLimit;																	// ����Ʈ ���� ������ �����Ѵ�.
		}
	}

	m_QuestLimitList.RemoveAll();															// ����Ʈ ���� ����Ʈ�� ��� ����.

	CQuestTrigger* pTrigger = NULL;															// ũ���� ������ ���� �����͸� �����ϰ� NULL ó���� �Ѵ�.

	pos = m_QuestTriggerList.GetHeadPosition();												// ����Ʈ Ʈ���� ����Ʈ�� ���� �����ϰ� ��ġ ������ �޴´�.

	while( pos )																			// ��ġ ������ ��ȿ�� ���� while ���� ������.
	{
		pTrigger = (CQuestTrigger*)m_QuestTriggerList.GetNext( pos );						// ��ġ�� �ش��ϴ� Ʈ���� ������ �޴´�.

		if( pTrigger )																		// Ʈ���� ������ ��ȿ���� üũ�Ѵ�.
		{
			delete pTrigger;																// Ʈ���� ������ �����Ѵ�.
		}
	}

	m_QuestTriggerList.RemoveAll();															// Ʈ���� ����Ʈ�� ��� ����.


	for( std::map< DWORD, CQuestNpcScript* >::iterator npcScriptIter = m_QuestNpcScript.begin() ; npcScriptIter != m_QuestNpcScript.end() ; ++npcScriptIter )
	{
		CQuestNpcScript* pNpcScript = npcScriptIter->second;
		SAFE_DELETE( pNpcScript );
	}
	m_QuestNpcScript.clear();

	CQuestNpc* pQuestNpc = NULL;															// ����Ʈ npc ������ ���� �����͸� �����ϰ� null ó���� �Ѵ�.

	pos = m_QuestNpcList.GetHeadPosition();													// ����Ʈ npc ����Ʈ�� ���� �����ϰ� ��ġ ������ �޴´�.

	while( pos )																			// ��ġ ������ ��ȿ�ѵ��� while���� ������.
	{
		pQuestNpc = (CQuestNpc*)m_QuestNpcList.GetNext( pos );								// ����Ʈ npc ������ �޴´�.

		if( pQuestNpc )																		// ����Ʈ npc ������ ��ȿ���� üũ�Ѵ�.
		{
			delete pQuestNpc;																// ����Ʈ npc ������ �����Ѵ�.
		}
	}

	m_QuestNpcList.RemoveAll();																// ����Ʈ npc ����Ʈ�� ��� ����.
}

void CSubQuestInfo::AddQuestLimit( CQuestLimit* pQuestLimit )								// ����Ʈ ���� ������ �߰��ϴ� �Լ�.
{
	if( !pQuestLimit ) return;																// ����Ʈ ���� ������ ��ȿ���� ������ ���� ó���� �Ѵ�.

	m_QuestLimitList.AddTail( pQuestLimit );												// ����Ʈ ���� ����Ʈ�� �߰��Ѵ�.
}

void CSubQuestInfo::AddQuestTrigger( CQuestTrigger* pQuestTrigger )							// ����Ʈ Ʈ���Ÿ� �߰��ϴ� �Լ�.
{
	if( !pQuestTrigger ) return;															// ����Ʈ Ʈ���� ������ ��ȿ���� ������ ���� ó���� �Ѵ�.

	m_QuestTriggerList.AddTail( pQuestTrigger );											// ����Ʈ Ʈ���� ����Ʈ�� �߰��Ѵ�.
}

void CSubQuestInfo::AddQuestNpcScipt( CQuestNpcScript* pQuestNpcScript )					// ����Ʈ npc ��ũ��Ʈ�� �߰��ϴ� �Լ�.
{
	if( !pQuestNpcScript ) return;															// ����Ʈ npc ��ũ��Ʈ ������ ��ȿ���� ������ ���� ó���� �Ѵ�.

	m_QuestNpcScript.insert( std::make_pair( pQuestNpcScript->GetKind(), pQuestNpcScript ) );	// ����Ʈ npc ��ũ��Ʈ ����Ʈ�� �߰��Ѵ�.
}

CQuestNpcScript* CSubQuestInfo::GetQuestNpcScript()
{
	// 100517 ShinJS --- ����Ʈ �䱸������ �Ϸ��ߴ��� �˻�(�Ϲ� ������ ��)
	const DWORD dwNpcScriptState = ( QUESTMGR->IsCompletedDemandQuestExecute( m_dwQuestIdx ) == TRUE ? eQuest_NpcScript_Normal : eQuest_NpcScript_Lack );
	std::map< DWORD, CQuestNpcScript* >::const_iterator npcScriptIter = m_QuestNpcScript.find( dwNpcScriptState );
	if( npcScriptIter == m_QuestNpcScript.end() )
	{
		if( dwNpcScriptState != eQuest_NpcScript_Normal )
		{
			npcScriptIter = m_QuestNpcScript.find( eQuest_NpcScript_Normal );
			if( npcScriptIter == m_QuestNpcScript.end() )
				return NULL;
		}
		else
		{
			return NULL;
		}
	}

	return npcScriptIter->second;
}

void CSubQuestInfo::AddQuestNpc( CQuestNpc* pQuestNpc )										// ����Ʈ npc �� �߰��ϴ� �Լ�.
{
	if( !pQuestNpc ) return;																// ����Ʈ npc ������ ��ȿ ���� ������ ���� ó���� �Ѵ�.

	m_QuestNpcList.AddTail(  pQuestNpc );													// ����Ʈ npc ����Ʈ�� �߰��Ѵ�.
}

BOOL CSubQuestInfo::OnQuestEvent( PLAYERTYPE* pPlayer, CQuestGroup* pQuestGroup, CQuest* pQuest, CQuestEvent* pQuestEvent )	// ����Ʈ �̺�Ʈ�� ó���ϴ� �Լ�.
{
	// 100405 ShinJS --- QuestEvent ���� ���� �Ǵ�
	if( !CanDoOnQuestEvent( pPlayer, pQuestGroup, pQuest ) )
		return FALSE;

	BOOL bIsExcuted = TRUE;
	
	// trigger
	PTRLISTPOS pos = m_QuestTriggerList.GetHeadPosition();									// Ʈ���� ����Ʈ�� ���� �����ϰ� ��ġ ������ �޴´�.

	CQuestTrigger* pTrigger = NULL;															// Ʈ���� ������ ���� �����͸� �����ϰ� null ó���� �Ѵ�.

	while( pos )																			// ��ġ ������ �ִµ��� while���� ������.
	{
		pTrigger = (CQuestTrigger*)m_QuestTriggerList.GetNext( pos );						// ��ġ�� �ش��ϴ� Ʈ���� ������ �޴´�.

		if( pTrigger )																		// Ʈ���� ������ ��ȿ���� üũ�Ѵ�.
		{
			if( pTrigger->OnQuestEvent( pPlayer, pQuestGroup, pQuest, pQuestEvent ) == FALSE )
				bIsExcuted = FALSE;
		}
	}

	return bIsExcuted;
}

BOOL CSubQuestInfo::IsNpcRelationQuest( DWORD dwNpcIdx )									// npc ���� ����Ʈ���� üũ�ϴ� �Լ�.
{
	DWORD dwQuestIdx, dwSubQuestIdx;														// ����Ʈ �ε����� ���� ����Ʈ �ε����� ���� ������ �����Ѵ�.

	dwQuestIdx = dwSubQuestIdx = 0;															// ����Ʈ �ε����� ���� ����Ʈ �ε����� 0���� �����Ѵ�.

	// 100517 ShinJS --- ����Ʈ �䱸������ �Ϸ��ߴ��� �˻�(�Ϲ� ������ ��)
	const DWORD dwNpcScriptState = ( QUESTMGR->IsCompletedDemandQuestExecute( m_dwQuestIdx ) == TRUE ? eQuest_NpcScript_Normal : eQuest_NpcScript_Lack );
	std::map< DWORD, CQuestNpcScript* >::const_iterator npcScriptIter = m_QuestNpcScript.find( dwNpcScriptState );
	if( npcScriptIter == m_QuestNpcScript.end() )
	{
		if( dwNpcScriptState != eQuest_NpcScript_Normal )
		{
			npcScriptIter = m_QuestNpcScript.find( eQuest_NpcScript_Normal );
			if( npcScriptIter == m_QuestNpcScript.end() )
				return FALSE;
		}
		else
		{
			return FALSE;
		}
	}

	CQuestNpcScript* pNpcScript = npcScriptIter->second;
	if( !pNpcScript )
		return FALSE;

	if( pNpcScript->IsSameNpc( dwNpcIdx, &dwQuestIdx, &dwSubQuestIdx ) == 0 )
		return FALSE;

	// 100405 ShinJS --- QuestEvent ���� ���� �Ǵ�
	if( !CanDoOnQuestEvent( dwQuestIdx, dwSubQuestIdx ) )
		return FALSE;

	return TRUE;																			// TRUE ���� ó���� �Ѵ�.
}

DWORD CSubQuestInfo::GetNpcScriptPage( DWORD dwNpcIdx )										// NPC ��ũ��Ʈ �������� �����ϴ� �Լ�.
{
	// 100517 ShinJS --- ����Ʈ �䱸������ �Ϸ��ߴ��� �˻�(�Ϲ� ������ ��)
	const DWORD dwNpcScriptState = ( QUESTMGR->IsCompletedDemandQuestExecute( m_dwQuestIdx ) == TRUE ? eQuest_NpcScript_Normal : eQuest_NpcScript_Lack );
	std::map< DWORD, CQuestNpcScript* >::const_iterator npcScriptIter = m_QuestNpcScript.find( dwNpcScriptState );
	if( npcScriptIter == m_QuestNpcScript.end() )
	{
		if( dwNpcScriptState != eQuest_NpcScript_Normal )
		{
			npcScriptIter = m_QuestNpcScript.find( eQuest_NpcScript_Normal );
			if( npcScriptIter == m_QuestNpcScript.end() )
				return 0;
		}
		else
		{
			return 0;
		}
	}

	CQuestNpcScript* pNpcScript = npcScriptIter->second;
	if( !pNpcScript )
		return 0;

	DWORD dwQuestIdx = 0, dwSubQuestIdx = 0;
	return pNpcScript->IsSameNpc( dwNpcIdx, &dwQuestIdx, &dwSubQuestIdx );
}

DWORD CSubQuestInfo::GetNpcMarkType( DWORD dwNpcIdx )										// npc ��ũ Ÿ���� ��ȯ�ϴ� �Լ�.
{
	// 100517 ShinJS --- ����Ʈ �䱸������ �Ϸ��ߴ��� �˻�(�Ϲ� ������ ��)
	const DWORD dwNpcScriptState = ( QUESTMGR->IsCompletedDemandQuestExecute( m_dwQuestIdx ) == TRUE ? eQuest_NpcScript_Normal : eQuest_NpcScript_Lack );
	std::map< DWORD, CQuestNpcScript* >::const_iterator npcScriptIter = m_QuestNpcScript.find( dwNpcScriptState );
	if( npcScriptIter == m_QuestNpcScript.end() )
	{
		if( dwNpcScriptState != eQuest_NpcScript_Normal )
		{
			npcScriptIter = m_QuestNpcScript.find( eQuest_NpcScript_Normal );
			if( npcScriptIter == m_QuestNpcScript.end() )
				return 0;
		}
		else
		{
			return 0;
		}
	}

	CQuestNpcScript* pNpcScript = npcScriptIter->second;
	if( !pNpcScript )
		return 0;

	// 100405 ShinJS --- QuestEvent ���� ���� �Ǵ�
	if( !CanDoOnQuestEvent( m_dwQuestIdx, m_dwSubQuestIdx ) )
		return 0;

	DWORD dwQuestIdx = 0, dwSubQuestIdx = 0;
	return pNpcScript->IsSameNpcMark( dwNpcIdx, &dwQuestIdx, &dwSubQuestIdx );
}
	
eQuestState CSubQuestInfo::IsQuestState( DWORD dwNpcIdx )											// ����Ʈ ���¸� �����ϴ� �Լ�.
{
	// 100517 ShinJS --- ����Ʈ �䱸������ �Ϸ��ߴ��� �˻�(�Ϲ� ������ ��)
	const DWORD dwNpcScriptState = ( QUESTMGR->IsCompletedDemandQuestExecute( m_dwQuestIdx ) == TRUE ? eQuest_NpcScript_Normal : eQuest_NpcScript_Lack );
	std::map< DWORD, CQuestNpcScript* >::const_iterator npcScriptIter = m_QuestNpcScript.find( dwNpcScriptState );
	if( npcScriptIter == m_QuestNpcScript.end() )
	{
		if( dwNpcScriptState != eQuest_NpcScript_Normal )
		{
			npcScriptIter = m_QuestNpcScript.find( eQuest_NpcScript_Normal );
			if( npcScriptIter == m_QuestNpcScript.end() )
				return eQuestState_ImPossible;
		}
		else
		{
			return eQuestState_ImPossible;
		}
	}

	CQuestNpcScript* pNpcScript = npcScriptIter->second;
	if( !pNpcScript )
		return eQuestState_ImPossible;

	DWORD dwQuestIdx = 0, dwSubQuestIdx = 0;
	DWORD dwPage = pNpcScript->IsSameNpc( dwNpcIdx, &dwQuestIdx, &dwSubQuestIdx );
	if( dwPage == 0 )																		// �������� 0�� ���ٸ�,
	{
		return eQuestState_ImPossible;
	}

	// 100405 ShinJS --- QuestEvent ���� ���� �Ǵ�
	if( !CanDoOnQuestEvent( dwQuestIdx, dwSubQuestIdx ) )
		return eQuestState_ImPossible;

	if( dwSubQuestIdx == 0 )	return eQuestState_Possible;			// ���డ���� ����						// ���� ����Ʈ �ε����� 0�� ������, ���డ���� ����.
	else						return eQuestState_Executed;			// �������� ����						// ���� ����Ʈ �ε����� 0�� ���� ������, �������� ����.

}

BOOL CSubQuestInfo::CheckLimitCondition( DWORD dwQuestIdx )									// ����Ʈ ���� ���¸� Ȯ���ϴ� �Լ�.
{
	// 100405 ShinJS --- QuestEvent ���� ���� �Ǵ�
	if( !CanDoOnQuestEvent( dwQuestIdx, 0 ) )
		return FALSE;

	return TRUE;
}

BOOL CSubQuestInfo::CanDoOnQuestEvent( PLAYERTYPE* pPlayer, CQuestGroup* pQuestGroup, CQuest* pQuest )
{
#ifdef _MAPSERVER_
	// 100402 ShinJS --- ����/�Ϸ� ���� �ð����� �˻�
	if( !pQuestGroup->IsValidDateTime( pQuest->GetQuestIdx() ) )
		return FALSE;
#endif

	// ����Ʈ �������� �˻�
	PTRLISTPOS pos = m_QuestLimitList.GetHeadPosition();
	CQuestLimit* pLimit = NULL;
	while( pos )
	{
		pLimit = (CQuestLimit*)m_QuestLimitList.GetNext( pos );
		if( pLimit && !pLimit->CheckLimit( pPlayer, pQuestGroup, pQuest ) )
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CSubQuestInfo::CanDoOnQuestEvent( DWORD dwQuestIdx, DWORD dwSubQuestIdx )
{
#ifdef _CLIENT_
	// 100402 ShinJS --- ����/�Ϸ� ���� �ð����� �˻�
	if( !QUESTMGR->IsValidDateTime( dwQuestIdx ) )
		return FALSE;
#endif

	// ����Ʈ �������� �˻�
	PTRLISTPOS pos = m_QuestLimitList.GetHeadPosition();
	CQuestLimit* pLimit = NULL;
	while( pos )
	{
		pLimit = (CQuestLimit*)m_QuestLimitList.GetNext( pos );
		if( pLimit && !pLimit->CheckLimit( dwQuestIdx, dwSubQuestIdx ) )
		{
			return FALSE;
		}
	}

	return TRUE;
}

void CSubQuestInfo::StartLoopByQuestExecuteKind( DWORD dwExecuteKind )
{
	m_FoundQuestExecute.clear();

	// ���� ����Ʈ���� ������ ã�´�.
	PTRLISTPOS pos = m_QuestTriggerList.GetHeadPosition();
	CQuestTrigger* pTrigger = NULL;
	while( pos )
	{
		pTrigger = (CQuestTrigger*)m_QuestTriggerList.GetNext( pos );
		if( !pTrigger )
			continue;

		cPtrList* pQuestExeList = pTrigger->GetExeList();
		if( !pQuestExeList )
			continue;

		PTRLISTPOS exePos = pQuestExeList->GetHeadPosition();
		while( exePos )
		{
			CQuestExecute* pQuestExecute = (CQuestExecute*)pQuestExeList->GetNext( exePos );
			if( !pQuestExecute )
				continue;

			if( pQuestExecute->GetQuestExecuteKind() == dwExecuteKind )
			{
				m_FoundQuestExecute.push_back( pQuestExecute );
			}
		}
	}

	m_FoundQuestExecuteIter = m_FoundQuestExecute.begin();
}

CQuestExecute* CSubQuestInfo::GetNextQuestExecute()
{
	if( m_FoundQuestExecute.empty() ||
		m_FoundQuestExecuteIter == m_FoundQuestExecute.end() )
		return NULL;

	CQuestExecute* pQuestExecute = *m_FoundQuestExecuteIter;
	++m_FoundQuestExecuteIter;

	return pQuestExecute;
}