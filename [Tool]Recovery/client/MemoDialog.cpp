// MemoDialog.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "client.h"
#include "MemoDialog.h"
#include ".\memodialog.h"


// CMemoDialog ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CMemoDialog, CDialog)
CMemoDialog::CMemoDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CMemoDialog::IDD, pParent)
{
	EnableAutomation();

}

CMemoDialog::~CMemoDialog()
{
}

void CMemoDialog::OnFinalRelease()
{
	// �ڵ�ȭ ��ü�� ���� ������ ������ �����Ǹ� OnFinalRelease��
	// ȣ��˴ϴ�. �⺻ Ŭ�������� �ڵ����� ��ü�� �����մϴ�.
	// �⺻ Ŭ������ ȣ���ϱ� ���� ��ü�� �ʿ��� �߰� ���� �۾���
	// �߰��Ͻʽÿ�.

	CDialog::OnFinalRelease();
}

void CMemoDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MEMO_EDIT_CTRL, mMemoEditCtrl);
}

void CMemoDialog::SetText(LPCTSTR text)
{
	mMemoEditCtrl.SetWindowText(
		text);
}


BEGIN_MESSAGE_MAP(CMemoDialog, CDialog)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CMemoDialog, CDialog)
END_DISPATCH_MAP()

// ����: IID_IMemoDialog�� ���� ������ �߰��Ͽ�
// VBA���� ���� �������� ���ε��� �����մϴ�. 
// �� IID�� .IDL ���Ͽ� �ִ� dispinterface�� GUID�� ��ġ�ؾ� �մϴ�.

// {060057AD-D423-423D-A75C-469ED798C544}
static const IID IID_IMemoDialog =
{ 0x60057AD, 0xD423, 0x423D, { 0xA7, 0x5C, 0x46, 0x9E, 0xD7, 0x98, 0xC5, 0x44 } };

BEGIN_INTERFACE_MAP(CMemoDialog, CDialog)
	INTERFACE_PART(CMemoDialog, IID_IMemoDialog, Dispatch)
END_INTERFACE_MAP()