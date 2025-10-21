/*
080822 LUJ, ���� �ݱ� ���� Ȯ���� �޴� â
*/
#pragma once


// CConfirmDialog ��ȭ �����Դϴ�.

class CConfirmDialog : public CDialog
{
	DECLARE_DYNAMIC(CConfirmDialog)

public:
	CConfirmDialog(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CConfirmDialog();

	virtual void OnFinalRelease();
	virtual void OnOK();

	// 080822 LUJ, �Էµ� ���ڿ��� ��ġ�ϴ��� ���Ѵ�
public:
	inline BOOL IsConfirm() const
	{
		return mInputText == mConfirmText;
	}
private:
	CString mInputText;
	CString mConfirmText;

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_CONFIRMDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
public:
	virtual BOOL OnInitDialog();

private:
	CEdit	mConfirmTextEdit;
	CStatic	mConfirmTextStatic;
};
