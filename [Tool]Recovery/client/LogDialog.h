/*
080401 LUJ, �α� â. Ŀ�ǵ� ������ ����Ͽ� ���� �������̽��� ������ ���� ������ �� �� �ְ� �Ѵ�
*/
#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "afxdtctl.h"


class CCommand;


class CLogDialog : public CDialog
{
	DECLARE_DYNAMIC(CLogDialog)

public:
	CLogDialog( CclientApp&, CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CLogDialog();

	virtual void OnFinalRelease();

	void Parse( DWORD serverIndex, const MSGROOT* );

	
// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_LOGDIALOG };


private:
	DWORD		mServerIndex;
	CCommand*	mCommand;
	CclientApp& mApplication;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual INT_PTR DoModal( DWORD serverIndex, CCommand& );

	afx_msg void OnBnClickedLogClipboardButton();
	afx_msg void OnBnClickedLogExcelButton();
	afx_msg void OnBnClickedLogFindButton();
	afx_msg void OnBnClickedLogStopButton();
	CListCtrl mResultListCtrl;
	CStatic mResultStatic;
	CDateTimeCtrl mBeginDateCtrl;
	CDateTimeCtrl mBeginTimeCtrl;
	CDateTimeCtrl mEndDateCtrl;
	CDateTimeCtrl mEndTimeCtrl;
	CButton mFindButton;
	CButton mStopButton;
	CProgressCtrl mProgressCtrl;
	CComboBox mServerCombo;
	CStatic mMaxLogStatic;
};
