#pragma once


// CScriptViewDlg ��ȭ �����Դϴ�.

class CScriptViewDlg : public CDialog
{
	DECLARE_DYNAMIC(CScriptViewDlg)

public:
	CScriptViewDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CScriptViewDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_SCRIPT_VIEW_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strScriptEdit;
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnPaint();
protected:
	virtual void OnOK();
};
