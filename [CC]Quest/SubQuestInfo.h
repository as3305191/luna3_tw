// SubQuestInfo.h: interface for the CSubQuestInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SUBQUESTINFO_H__AC05E747_C207_4E04_8D35_09F423BA4A7E__INCLUDED_)
#define AFX_SUBQUESTINFO_H__AC05E747_C207_4E04_8D35_09F423BA4A7E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "QuestDefines.h"															// ����Ʈ ���� ���� ��������� �ҷ��´�.

class CQuestLimit;																	// ����Ʈ ���� Ŭ������ �����Ѵ�.
class CQuestTrigger;																// ����Ʈ Ʈ���� Ŭ������ �����Ѵ�.
class CQuestNpcScript;																// ����Ʈ npc ��ũ��Ʈ Ŭ������ �����Ѵ�.
class CQuestNpc;																	// ����Ʈ npc Ŭ������ �����Ѵ�.

class CQuestGroup;																	// ����Ʈ �׷� Ŭ������ �����Ѵ�.
class CQuest;																		// ����Ʈ Ŭ������ �����Ѵ�.
class CQuestEvent;																	// ����Ʈ �̺�Ʈ Ŭ������ �����Ѵ�.
class CQuestExecute;

class CSubQuestInfo																	// ���� ����Ʈ ���� Ŭ����.
{
protected:
	DWORD		m_dwQuestIdx;
	DWORD		m_dwSubQuestIdx;
	cPtrList	m_QuestLimitList;													// ����Ʈ ���� ������ ��� ����Ʈ.
	cPtrList	m_QuestTriggerList;													// ����Ʈ Ʈ���� ������ ��� ����Ʈ.
	std::map< DWORD, CQuestNpcScript* > m_QuestNpcScript;							// ����Ʈ npc ��ũ��Ʈ ����Ʈ.
	cPtrList	m_QuestNpcList;														// ����Ʈ npc ����Ʈ.

	DWORD		m_dwMaxCount;														// �ִ� ī��Ʈ.

	std::vector< CQuestExecute* > m_FoundQuestExecute;
	std::vector< CQuestExecute* >::const_iterator m_FoundQuestExecuteIter;

	// 100405 ShinJS --- QuestEvent ���� ���� ����
    BOOL	CanDoOnQuestEvent( PLAYERTYPE* pPlayer, CQuestGroup* pQuestGroup, CQuest* pQuest );
	BOOL	CanDoOnQuestEvent( DWORD dwQuestIdx, DWORD dwSubQuestIdx );
public:
	CSubQuestInfo( DWORD dwQuestIdx, DWORD dwSubQuestIdx );
	virtual ~CSubQuestInfo();														// �Ҹ��� �Լ�.
	
	void	Release();																// ���� �Լ�.
	// script
	void	AddQuestLimit( CQuestLimit* pQuestLimit );								// ����Ʈ ������ �߰��ϴ� �Լ�.
	void	AddQuestTrigger( CQuestTrigger* pQuestTrigger );						// ����Ʈ Ʈ���Ÿ� �߰��ϴ� �Լ�.

	void	AddQuestNpcScipt( CQuestNpcScript* pQuestNpcScript );					// ����Ʈ npc ��ũ��Ʈ�� �߰��ϴ� �Լ�.
	CQuestNpcScript* GetQuestNpcScript();

	void	AddQuestNpc( CQuestNpc* pQuestNpc );									// ����Ʈ npc�� �߰��ϴ� �Լ�.

	// event
	BOOL	OnQuestEvent( PLAYERTYPE* pPlayer, CQuestGroup* pQuestGroup, CQuest* pQuest, CQuestEvent* pQuestEvent);	// ����Ʈ �̺�Ʈ�� �����ϴ� �Լ�.
	// npc
	BOOL	IsNpcRelationQuest( DWORD dwNpcIdx );									// npc�� ���õ� ����Ʈ ���� ���θ� ��ȯ�ϴ� �Լ�.
	DWORD	GetNpcScriptPage( DWORD dwNpcIdx );										// npc  ��ũ��Ʈ �������� ��ȯ�ϴ� �Լ�.
	DWORD	GetNpcMarkType( DWORD dwNpcIdx );										// npc ��ũ Ÿ���� ��ȯ�ϴ� �Լ�.
	eQuestState IsQuestState( DWORD dwNpcIdx );										// ����Ʈ ���¸� Ȯ���ϴ� �Լ�.
	cPtrList& GetQuestNpcList() { return m_QuestNpcList; }
	BOOL	CheckLimitCondition( DWORD dwQuestIdx );
	DWORD	GetMaxCount() const { return m_dwMaxCount; }
	void	SetMaxCount( DWORD dwMaxCount )	{ m_dwMaxCount = dwMaxCount; }
	cPtrList& GetLimitList() { return m_QuestLimitList ; }
	cPtrList& GetTriggerList() { return m_QuestTriggerList ; }
	cPtrList& GetNpcList() { return m_QuestNpcList ; }
	// 100422 ShinJS --- eQuestExecute ������ ���� CQuestExecute�� ã�´�.
	void	StartLoopByQuestExecuteKind( DWORD dwExecuteKind );
	// 100422 ShinJS --- StartLoopByQuestExecuteKind()�� ã�� eQuestExecute ������ ���� CQuestExecute�� ��ȸ�Ѵ�.
	CQuestExecute* GetNextQuestExecute();
	const std::map< DWORD, CQuestNpcScript* >& GetQuestNpcScriptList() const { return m_QuestNpcScript; }
};

#endif // !defined(AFX_SUBQUESTINFO_H__AC05E747_C207_4E04_8D35_09F423BA4A7E__INCLUDED_)
