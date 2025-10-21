#pragma once


// CBaseDlgBar ��ȭ �����Դϴ�.
#include "DlgBarSheet.h"

class CBaseDlgBar : public CDialog
{
	DECLARE_DYNAMIC(CBaseDlgBar)

	CDlgBarSheet* m_pSheet;

public:
	CBaseDlgBar(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CBaseDlgBar();

	CWnd* GetMonsterPage() { return (m_pSheet == NULL ? NULL : m_pSheet->GetMonsterPage()); }

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DLGBAR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
