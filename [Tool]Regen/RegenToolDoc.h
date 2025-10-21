// RegenToolDoc.h : interface of the CRegenToolDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_REGENTOOLDOC_H__6AED26BE_0BF4_40B3_A511_45BF436A72E1__INCLUDED_)
#define AFX_REGENTOOLDOC_H__6AED26BE_0BF4_40B3_A511_45BF436A72E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "DefineStruct.h"
#include <list>

using namespace std ;

class CRegenToolDoc : public CDocument
{
protected: // create from serialization only
	CRegenToolDoc();
	DECLARE_DYNCREATE(CRegenToolDoc)

// Attributes
public:
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRegenToolDoc)
	public:
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRegenToolDoc();

protected:
	// 091106 LUJ, Ű: �׷� ��ȣ
	typedef std::map< DWORD, Script > ScriptMap;
	ScriptMap mScriptMap;

public:
	// Regen Script Load
	void LoadRegenScript(LPCTSTR path);
	// Script ������ Data Update, GXO ����/��ġ
	void UpdateDataFromScript();

	// Data�� Script�� ��ȯ
	void DataToScript( CString& text );

	// Script ���� ���
	int GetGroupNum() const { return mScriptMap.size(); }
	Script* GetScriptInfo( DWORD dwGroupIdx );
	Script* GetScriptInfoFromGXOHandle( GXOBJECT_HANDLE hGXO );
	Script::Condition* GetConditionInfo( DWORD dwGroupIdx, int nIndex );
	Script::Monster* GetMonsterInfo( DWORD dwGroupIdx, GXOBJECT_HANDLE hGXO );
	Script::Monster* GetMonsterInfo( GXOBJECT_HANDLE hGXO );

	// Group �߰�/����
	const DWORD GetUsableGroupIndex() const;
	Script* AddGroup();
	BOOL DeleteGroup( DWORD dwGroup );

	// Condition �߰�/����
	const DWORD GetUsableConditionIndex( const DWORD dwGroup ) const;
	Script::Condition* AddCondition( DWORD dwGroup );
	BOOL DeleteCondition( DWORD dwGroup, DWORD dwConditionIdx );

	// Monster �߰�/����
	Script::Monster* AddMonster( DWORD dwGroupIdx );
	BOOL DeleteMonster( GXOBJECT_HANDLE hGXO );


protected:
	//{{AFX_MSG(CRegenToolDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGENTOOLDOC_H__6AED26BE_0BF4_40B3_A511_45BF436A72E1__INCLUDED_)
