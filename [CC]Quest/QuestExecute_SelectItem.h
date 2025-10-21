// QuestExecute_SelectItem.h: interface for the CQuestExecute_SelectItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUESTEXECUTE_SELECTITEM_H__E5FAF1F8_D9F2_4B21_9792_D7F488E99EF6__INCLUDED_)
#define AFX_QUESTEXECUTE_SELECTITEM_H__E5FAF1F8_D9F2_4B21_9792_D7F488E99EF6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "QuestExecute.h"														// ����Ʈ ���� ��� ������ �ҷ��´�.

struct SELECTITEM																// ���� ������ ������ ��� ����ü.
{
	DWORD	dwItemIdx;															// ������ �ε����� ��� ����.
	DWORD	dwItemNum;															// ������ ���� ��� ����.
};

class CQuestExecute_SelectItem : public CQuestExecute							// ����Ʈ ���� Ŭ������ ��ӹ��� ����Ʈ ���� ���� ������ Ŭ����.
{
protected:
	DWORD			m_dwMaxItemCount;												// ������ �ִ� ī��Ʈ.
	SELECTITEM*		m_pSelectItem;
	
public:
	CQuestExecute_SelectItem( DWORD dwExecuteKind, CStrTokenizer* pTokens, DWORD dwQuestIdx, DWORD dwSubQuestIdx );	// ������ �Լ�.
	virtual ~CQuestExecute_SelectItem();																			// �Ҹ��� �Լ�.

	virtual BOOL	Execute( PLAYERTYPE* pPlayer, CQuestGroup* pQuestGroup, CQuest* pQuest );						// ���� �Լ�.

	virtual int		CheckCondition( PLAYERTYPE* pPlayer, CQuestGroup* pQuestGroup, CQuest* pQuest );
	
	DWORD			GetMaxItemCount()			{ return m_dwMaxItemCount; }
	DWORD			GetItemKind( int nIndex )	{ return m_pSelectItem[nIndex].dwItemIdx; }
	DWORD			GetItemNum( int nIndex )	{ return m_pSelectItem[nIndex].dwItemNum; }

};

#endif // !defined(AFX_QUESTEXECUTE_RANDOMITEM_H__E5FAF1F8_D9F2_4B21_9792_D7F488E99EF6__INCLUDED_)
