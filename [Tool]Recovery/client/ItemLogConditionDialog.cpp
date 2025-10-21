// ItemLogConditionDialog.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "client.h"
#include "ItemLogConditionDialog.h"
#include "ItemAddDialog.h"


// CItemLogConditionDialog ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CItemLogConditionDialog, CDialog)
CItemLogConditionDialog::CItemLogConditionDialog( CclientApp& application, CWnd* pParent /*=NULL*/) :
CDialog(CItemLogConditionDialog::IDD, pParent),
mApplication( application )
{
	ZeroMemory( &mInputData, sizeof( mInputData ) );

	EnableAutomation();
}

CItemLogConditionDialog::~CItemLogConditionDialog()
{
}

void CItemLogConditionDialog::OnFinalRelease()
{
	// �ڵ�ȭ ��ü�� ���� ������ ������ �����Ǹ� OnFinalRelease��
	// ȣ��˴ϴ�. �⺻ Ŭ�������� �ڵ����� ��ü�� �����մϴ�.
	// �⺻ Ŭ������ ȣ���ϱ� ���� ��ü�� �ʿ��� �߰� ���� �۾���
	// �߰��Ͻʽÿ�.

	CDialog::OnFinalRelease();
}

void CItemLogConditionDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ITEM_CONDITION_INDEX_EDIT, mItemIndexEdit);
	DDX_Control(pDX, IDC_ITEM_CONDITION_DB_IDX_EDIT, mItemDbIndexEdit);
	DDX_Control(pDX, IDC_ITEM_CONDITION_PLAYER_IDX_EDIT, mPlayerIndexEdit);
	DDX_Control(pDX, IDC_ITEM_CONDITION_LOG_TYPE_COMBO, mLogTypeCombo);
}


BEGIN_MESSAGE_MAP(CItemLogConditionDialog, CDialog)
	ON_BN_CLICKED(IDC_ITEM_LOG_CONDITION_ITEM_IDX_BUTTON, OnBnClickedItemLogConditionItemIdxButton)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CItemLogConditionDialog, CDialog)
END_DISPATCH_MAP()

// ����: IID_IItemLogConditionDialog�� ���� ������ �߰��Ͽ�
// VBA���� ���� �������� ���ε��� �����մϴ�. 
// �� IID�� .IDL ���Ͽ� �ִ� dispinterface�� GUID�� ��ġ�ؾ� �մϴ�.

// {2406325B-DCF7-4930-8408-D7296E73CAFA}
static const IID IID_IItemLogConditionDialog =
{ 0x2406325B, 0xDCF7, 0x4930, { 0x84, 0x8, 0xD7, 0x29, 0x6E, 0x73, 0xCA, 0xFA } };

BEGIN_INTERFACE_MAP(CItemLogConditionDialog, CDialog)
	INTERFACE_PART(CItemLogConditionDialog, IID_IItemLogConditionDialog, Dispatch)
END_INTERFACE_MAP()


// CItemLogConditionDialog �޽��� ó�����Դϴ�.

BOOL CItemLogConditionDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	mPlayerIndexEdit.SetWindowText( _T( "0" ) );
	mItemDbIndexEdit.SetWindowText( _T( "0" ) );
	mItemIndexEdit.SetWindowText( _T( "0" ) );
	
	// 080716 LUJ, �޺� �ڽ� ����
	{
		CString text;

		CString textEmpty;
		textEmpty.LoadString( IDS_STRING203 );
		text.Format( _T( "%s" ), textEmpty );
		mLogTypeCombo.InsertString( mLogTypeCombo.GetCount(), text );

		for( int logType = 0; logType < eLog_Max; ++logType )
		{
			const CString logText = mApplication.GetText( eLogitemmoney( logType ) );

			int i = 0;
			const CString token = logText.Tokenize( _T( "()" ), i );

			if( _T( "?" ) == token )
			{
				continue;
			}

			text.Format( _T( "%s" ), logText );
			mLogTypeCombo.InsertString(
				mLogTypeCombo.GetCount(),
				text );

			mLogDataList.push_back(
				std::make_pair( text, eLogitemmoney( logType ) ) );
		}

		CRect rect;
		mLogTypeCombo.GetWindowRect( rect );
		mLogTypeCombo.SetWindowPos( 0, 0, 0, rect.Width(), 200, SWP_NOMOVE | SWP_NOZORDER );
		mLogTypeCombo.SetCurSel( 0 );
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
}


void CItemLogConditionDialog::OnOK()
{
	CString text;

	mItemDbIndexEdit.GetWindowText( text );
	mInputData.mItemDbIndex	= _ttoi( text );

	mItemIndexEdit.GetWindowText( text );
	mInputData.mItemIndex = _ttoi( text );

	mPlayerIndexEdit.GetWindowText( text );
	mInputData.mPlayerIndex = _ttoi( text );

	mLogTypeCombo.GetLBText( mLogTypeCombo.GetCurSel(), text );

	for(
		LogDataList::const_iterator it = mLogDataList.begin();
		mLogDataList.end() != it;
		++it )
	{
		const CString& logText = it->first;

		if( logText != text )
		{
			continue;
		}

		mInputData.mLogType	= eLogitemmoney( it->second );
	}
	
	CDialog::OnOK();
}


void CItemLogConditionDialog::OnBnClickedItemLogConditionItemIdxButton()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	CItemAddDialog dialog( mApplication );

	if( IDCANCEL == dialog.DoModal() )
	{
		return;
	}

	CString text;
	text.Format( _T( "%d" ), dialog.GetSelectedItemIndex() );
	mItemIndexEdit.SetWindowText( text );
}
