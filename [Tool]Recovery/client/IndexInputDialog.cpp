// IndexInputDialog.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "client.h"
#include "IndexInputDialog.h"
#include "ItemAddDialog.h"


// CIndexInputDialog ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CIndexInputDialog, CDialog)
CIndexInputDialog::CIndexInputDialog( CclientApp& application, const Configuration& configuration, CWnd* pParent /*=NULL*/)
	: CDialog(CIndexInputDialog::IDD, pParent)
	, mApplication( application )
	, mItemIndex( 0 )
	, mItemDbIndex( 0 )
	, mPlayerIndex( 0 )
	, mUserIndex(0)
	, mConsignIndex(0)
	, mConfiguration( configuration )
{
	EnableAutomation();
}

CIndexInputDialog::~CIndexInputDialog()
{
}

void CIndexInputDialog::OnFinalRelease()
{
	// �ڵ�ȭ ��ü�� ���� ������ ������ �����Ǹ� OnFinalRelease��
	// ȣ��˴ϴ�. �⺻ Ŭ�������� �ڵ����� ��ü�� �����մϴ�.
	// �⺻ Ŭ������ ȣ���ϱ� ���� ��ü�� �ʿ��� �߰� ���� �۾���
	// �߰��Ͻʽÿ�.

	CDialog::OnFinalRelease();
}

void CIndexInputDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control( pDX, IDC_INDEX_INPUT_ITEM_RADIO, mItemRadioButton );
	DDX_Control( pDX, IDC_INDEX_INPUT_ITEM_DB_RADIO, mItemDbRadioButton );
	DDX_Control( pDX, IDC_INDEX_INPUT_PLAYER_RADIO, mPlayerRadioButton );
	DDX_Control( pDX, IDC_INDEX_INPUT_KEYWORD_RADIO, mKeywordRadioButton );
	DDX_Control( pDX, IDC_INDEX_INPUT_EDIT, mIndexInputEdit);
	DDX_Control(pDX, IDC_INDEX_INPUT_USER_INDEX_RADIO, mUserIndexButton);
	DDX_Control(pDX, IDC_INDEX_INPUT_CONSIGN_INDEX_RADIO, mConsignIndexButton);
}


BEGIN_MESSAGE_MAP(CIndexInputDialog, CDialog)
	ON_BN_CLICKED(IDC_INDEX_INPUT_ITEM_RADIO, OnBnClickedIndexInputItemRadio)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CIndexInputDialog, CDialog)
END_DISPATCH_MAP()

// ����: IID_IIndexInputDialog�� ���� ������ �߰��Ͽ�
// VBA���� ���� �������� ���ε��� �����մϴ�. 
// �� IID�� .IDL ���Ͽ� �ִ� dispinterface�� GUID�� ��ġ�ؾ� �մϴ�.

// {5BE2F299-6851-48E7-86C8-3E17249855C7}
static const IID IID_IIndexInputDialog =
{ 0x5BE2F299, 0x6851, 0x48E7, { 0x86, 0xC8, 0x3E, 0x17, 0x24, 0x98, 0x55, 0xC7 } };

BEGIN_INTERFACE_MAP(CIndexInputDialog, CDialog)
	INTERFACE_PART(CIndexInputDialog, IID_IIndexInputDialog, Dispatch)
END_INTERFACE_MAP()


// CIndexInputDialog �޽��� ó�����Դϴ�.

void CIndexInputDialog::OnOK()
{
	CString index;
	mIndexInputEdit.GetWindowText( index );

	if( mItemRadioButton.GetCheck() )
	{
		mItemIndex = _ttoi( index );
	}
	else if( mItemDbRadioButton.GetCheck() )
	{
		mItemDbIndex = _ttoi( index );
	}
	else if( mPlayerRadioButton.GetCheck() )
	{
		mPlayerIndex = _ttoi( index );
	}
	else if( mKeywordRadioButton.GetCheck() )
	{
		mKeyword = index;
	}
	else if(mUserIndexButton.GetCheck())
	{
		mUserIndex = _ttoi(index);
	}
	else if(mConsignIndexButton.GetCheck())
	{
		mConsignIndex = _ttoi(index);
	}

	CDialog::OnOK();
}


BOOL CIndexInputDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	mItemDbRadioButton.EnableWindow( mConfiguration.mIsEnableItemDbIndex );
	mItemRadioButton.EnableWindow( mConfiguration.mIsEnableItemIndex );
	mPlayerRadioButton.EnableWindow( mConfiguration.mIsEnablePlayerIndex );
	mKeywordRadioButton.EnableWindow( mConfiguration.mIsEnableKeyword );
	mUserIndexButton.EnableWindow(mConfiguration.mIsEnableUserIndex);
	mConsignIndexButton.EnableWindow(mConfiguration.mIsEnableConsignIndex);

	// 080403 LUJ, Ȱ���� ���� ��ư�� üũ ǥ���Ѵ�. �ϳ��� ������ ����Ʈ ���ڸ� ��Ȱ��ȭ�Ѵ�
	if( mConfiguration.mIsEnableItemDbIndex )
	{
		mItemDbRadioButton.SetCheck( TRUE );
	}
	else if( mConfiguration.mIsEnableItemIndex )
	{
		mItemRadioButton.SetCheck( TRUE );
	}
	else if( mConfiguration.mIsEnablePlayerIndex )
	{
		mPlayerRadioButton.SetCheck( TRUE );
	}
	else if( mConfiguration.mIsEnableKeyword )
	{
		mKeywordRadioButton.SetCheck( TRUE );
	}
	else if(mConfiguration.mIsEnableUserIndex)
	{
		mUserIndexButton.SetCheck(TRUE);
	}
	else if(mConfiguration.mIsEnableConsignIndex)
	{
		mConsignIndexButton.SetCheck(TRUE);
	}
	else
	{
		mIndexInputEdit.EnableWindow( FALSE );
	}
	
	mIndexInputEdit.SetFocus();

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


void CIndexInputDialog::OnBnClickedIndexInputItemRadio()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CItemAddDialog dialog( mApplication );

	if( IDCANCEL == dialog.DoModal() )
	{
		return;
	}

	CString text;
	text.Format( _T( "%d" ), dialog.GetSelectedItemIndex() );
	
	mIndexInputEdit.SetWindowText( text );
}
