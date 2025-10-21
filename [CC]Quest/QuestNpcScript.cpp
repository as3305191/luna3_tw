// QuestNpcScript.cpp: implementation of the CQuestNpcScript class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"															// ǥ�� �ý��۰�, ������Ʈ�� ������, ���־��� �ش����� ���� �ش������� �ҷ��´�.
#include "QuestDefines.h"
#include "QuestNpcScript.h"
#include "QuestScriptLoader.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQuestNpcScript::CQuestNpcScript( CStrTokenizer* pTokens, DWORD dwQuestIdx, DWORD dwSubQuestIdx )	// ������ �Լ�.
{
	m_dwQuestIdx = dwQuestIdx;														// ����Ʈ �ε����� �����Ѵ�.
	m_dwSubQuestIdx = dwSubQuestIdx;												// ���� ����Ʈ �ε����� �����Ѵ�.

	const char* token = pTokens->GetNextTokenUpper();
	if( !token )
	{
		m_dwKind = 0;
	}
	else if( stricmp( token, "@NPC" ) == 0 )
	{
		m_dwKind = eQuest_NpcScript_Normal;
	}
	else if( stricmp( token, "@DENIAL" ) == 0 )
	{
		m_dwKind = eQuest_NpcScript_Lack;
	}

	GetScriptParam( m_dwNpcIdx, pTokens );						// ��ũ��Ʈ�� ���� npc �ε����� �޴´�.
	GetScriptParam( m_dwScriptPage, pTokens );					// ��ũ��Ʈ�� ���� ��ũ��Ʈ �������� �޴´�.
	GetScriptParam( m_dwNpcMarkType, pTokens );					// ��ũ��Ʈ�� ���� npc ��ũ Ÿ���� �޴´�.
	GetScriptParam( m_dwDialogType, pTokens ) ;					// ���̾�α� ���� ��� ���θ� �����Ѵ�.
}

CQuestNpcScript::~CQuestNpcScript()													// �Ҹ��� �Լ�.
{
}

DWORD CQuestNpcScript::IsSameNpc( DWORD dwNpcIdx, DWORD* pQuestIdx, DWORD* pSubQuestIdx )			// npc ��ũ��Ʈ �������� �����ϴ� �Լ�.
{
	*pQuestIdx = m_dwQuestIdx;														// ����Ʈ �ε����� �����Ѵ�.
	*pSubQuestIdx = m_dwSubQuestIdx;												// ���� ����Ʈ �ε����� �����Ѵ�.

	return (dwNpcIdx == m_dwNpcIdx ? m_dwScriptPage : 0);
}

DWORD CQuestNpcScript::IsSameNpcMark( DWORD dwNpcIdx, DWORD* pQuestIdx, DWORD* pSubQuestIdx )	// npc ��ũ�� �����ϴ� �Լ�.
{
	*pQuestIdx = m_dwQuestIdx;														// ����Ʈ �ε����� �����Ѵ�.
	*pSubQuestIdx = m_dwSubQuestIdx;												// ���� ����Ʈ�� �����Ѵ�.

	return (dwNpcIdx == m_dwNpcIdx ? m_dwNpcMarkType : 0);
}
