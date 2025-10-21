#pragma once
#include "afxwin.h"


// CIndexInputDialog ��ȭ �����Դϴ�.

class CIndexInputDialog : public CDialog
{
	DECLARE_DYNAMIC(CIndexInputDialog)
public:
	struct Configuration
	{
		bool mIsEnableItemDbIndex;
		bool mIsEnableItemIndex;
		bool mIsEnablePlayerIndex;
		bool mIsEnableKeyword;
		bool mIsEnableUserIndex;
		bool mIsEnableConsignIndex;
	};

public:
	CIndexInputDialog( CclientApp&, const Configuration&, CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CIndexInputDialog();

	virtual void OnFinalRelease();
	virtual void OnOK();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_INDEXINPUTDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

private:
	DWORD	mItemIndex;
	DWORD	mItemDbIndex;
	DWORD	mPlayerIndex;
	CString	mKeyword;
	DWORD mUserIndex;
	DWORD	mConsignIndex;

public:
	inline DWORD GetItemIndex()		const { return mItemIndex; }
	inline DWORD GetItemDbIndex()	const { return mItemDbIndex; }
	inline DWORD GetPlayerIndex()	const { return mPlayerIndex; }
	inline LPCTSTR GetKeyword()		const { return mKeyword; }
	inline DWORD GetUserIndex() const { return mUserIndex; }
	inline DWORD GetConsignIndex() const { return mConsignIndex; }
	CEdit mIndexInputEdit;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedIndexInputItemRadio();

private:
	CclientApp&			mApplication;
	const Configuration	mConfiguration;
	CButton mItemRadioButton;
	CButton mItemDbRadioButton;
	CButton mPlayerRadioButton;
	CButton mKeywordRadioButton;
	CButton mUserIndexButton;
	CButton mConsignIndexButton;
};
