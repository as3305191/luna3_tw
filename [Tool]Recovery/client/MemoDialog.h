#pragma once
#include "afxcmn.h"


// CMemoDialog ��ȭ �����Դϴ�.

class CMemoDialog : public CDialog
{
	DECLARE_DYNAMIC(CMemoDialog)

public:
	CMemoDialog(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CMemoDialog();
	void SetText(LPCTSTR);

	virtual void OnFinalRelease();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_MEMODIALOG };

private:
    CString mText;
	CRichEditCtrl mMemoEditCtrl;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};
