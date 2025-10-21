#pragma once
#include "afxwin.h"
#include "afxcmn.h"


class CclientApp;


// CLoginDialog ��ȭ �����Դϴ�.

class CLoginDialog : public CDialog
{
	DECLARE_DYNAMIC(CLoginDialog)

public:
	CLoginDialog( CclientApp&, CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CLoginDialog();

	virtual void OnFinalRelease();
	void Parse( DWORD serverIndex, const MSGROOT* );

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_LOGINDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
public:
//	afx_msg void OnOK();
	

private:
	CclientApp&	mApplication;
	CEdit			mIdEdit;
	CEdit			mPasswordEdit;
	CListCtrl		mServerListCtrl;
	CStatic			mVersionStatic;
	CButton			mSubmitButton;
	CButton			mCancelButton;
	CProgressCtrl	mLoadingProgressCtrl;
	CBitmap			mTitleImage;

public:
	virtual BOOL	OnInitDialog();
	afx_msg void OnBnClickedLoginConnectButton();
	inline void SetProgressMax( int value )	{ mLoadingProgressCtrl.SetRange32( 0, value ); }
	// 080630 LUJ, �ε� ���α׷��� ��Ʈ���� ���� �����Ѵ�
	void SetProgressValue( int value, const TCHAR* message = 0 );
protected:
	virtual void OnCancel();
public:
	CStatic mTitleImageStatic;
};
