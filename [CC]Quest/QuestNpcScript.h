// QuestNpcScript.h: interface for the CQuestNpcScript class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUESTNPCSCRIPT_H__D5E07727_465F_40AF_93D5_889FF980954F__INCLUDED_)
#define AFX_QUESTNPCSCRIPT_H__D5E07727_465F_40AF_93D5_889FF980954F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CStrTokenizer;																			// ��Ʈ�� ��ũ������ Ŭ������ �����Ѵ�.

class CQuestNpcScript																			// NPC ��ũ��Ʈ Ŭ����.
{
protected:
	DWORD		m_dwQuestIdx;																	// ����Ʈ �ε����� ��� ����.
	DWORD		m_dwSubQuestIdx;																// ���� ����Ʈ �ε����� ��� ����.

	DWORD		m_dwKind;																		// ����Ʈ ���¿� ���� ����
	DWORD		m_dwNpcIdx;																		// npc �ε����� ��� ����.
	DWORD		m_dwScriptPage;																	// ��ũ��Ʈ �������� ��� ����.
	DWORD		m_dwNpcMarkType;																// npc ��ũ Ÿ���� ��� ����.
	DWORD		m_dwDialogType ;																// ���̾�α� ��� Ÿ��.

public:
	CQuestNpcScript( CStrTokenizer* pTokens, DWORD dwQuestIdx, DWORD dwSubQuestIdx );
	virtual ~CQuestNpcScript();																	// �Ҹ��� �Լ�.

	DWORD	IsSameNpc( DWORD dwNpcIdx, DWORD* pQuestIdx, DWORD* pSubQuestIdx );					// npc ��ũ��Ʈ �������� �����ϴ� �Լ�.
	DWORD	IsSameNpcMark( DWORD dwNpcIdx, DWORD* pQuestIdx, DWORD* pSubQuestIdx );				// npc ��ũ�� �����ϴ� �Լ�.

	DWORD	GetKind() const				{ return m_dwKind; }
	DWORD	GetNpcIdx() const			{ return m_dwNpcIdx ; }
	DWORD	GetScriptPage() const		{ return m_dwScriptPage ; }
	DWORD	GetNpcMarkType() const		{ return m_dwNpcMarkType ; }
	DWORD	GetDialogType() const		{ return m_dwDialogType ; }
	DWORD	GetNpcScriptPage() const		{ return m_dwScriptPage; }							// npc ��ũ��Ʈ �������� �����ϴ� �Լ�.
};

#endif // !defined(AFX_QUESTNPCSCRIPT_H__D5E07727_465F_40AF_93D5_889FF980954F__INCLUDED_)
