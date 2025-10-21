// LogDialog.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "client.h"
#include "LogDialog.h"
#include "ItemLogCommand.h"


// CLogDialog ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CLogDialog, CDialog)
CLogDialog::CLogDialog( CclientApp& application, CWnd* pParent /*=NULL*/) :
CDialog(CLogDialog::IDD, pParent),
mApplication( application ),
mCommand( 0 ),
mServerIndex( 0 )
{
	EnableAutomation();
}

CLogDialog::~CLogDialog()
{
}

void CLogDialog::OnFinalRelease()
{
	// �ڵ�ȭ ��ü�� ���� ������ ������ �����Ǹ� OnFinalRelease��
	// ȣ��˴ϴ�. �⺻ Ŭ�������� �ڵ����� ��ü�� �����մϴ�.
	// �⺻ Ŭ������ ȣ���ϱ� ���� ��ü�� �ʿ��� �߰� ���� �۾���
	// �߰��Ͻʽÿ�.

	CDialog::OnFinalRelease();
}

void CLogDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOG_RESULT_LIST, mResultListCtrl);
	DDX_Control(pDX, IDC_GUILD_LOG_RESULT_STATIC, mResultStatic);
	DDX_Control(pDX, IDC_LOG_BEGIN_DATE_CTRL, mBeginDateCtrl);
	DDX_Control(pDX, IDC_LOG_BEGIN_TIME_CTRL, mBeginTimeCtrl);
	DDX_Control(pDX, IDC_LOG_END_DATE_CTRL, mEndDateCtrl);
	DDX_Control(pDX, IDC_LOG_END_TIME_CTRL, mEndTimeCtrl);
	DDX_Control(pDX, IDC_LOG_FIND_BUTTON, mFindButton);
	DDX_Control(pDX, IDC_LOG_STOP_BUTTON, mStopButton);
	DDX_Control(pDX, IDC_LOG_RESULT_PROGRESS, mProgressCtrl);
	DDX_Control(pDX, IDC_FARM_LOG_SERVER_COMBO, mServerCombo);
	DDX_Control(pDX, IDC_LOG_MAX_SIZE_STATIC, mMaxLogStatic);
}


BEGIN_MESSAGE_MAP(CLogDialog, CDialog)
	ON_BN_CLICKED(IDC_LOG_CLIPBOARD_BUTTON, OnBnClickedLogClipboardButton)
	ON_BN_CLICKED(IDC_LOG_EXCEL_BUTTON, OnBnClickedLogExcelButton)
	ON_BN_CLICKED(IDC_LOG_FIND_BUTTON, OnBnClickedLogFindButton)
	ON_BN_CLICKED(IDC_LOG_STOP_BUTTON, OnBnClickedLogStopButton)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CLogDialog, CDialog)
END_DISPATCH_MAP()

// ����: IID_ILogDialog�� ���� ������ �߰��Ͽ�
// VBA���� ���� �������� ���ε��� �����մϴ�. 
// �� IID�� .IDL ���Ͽ� �ִ� dispinterface�� GUID�� ��ġ�ؾ� �մϴ�.

// {6DF333F1-26A6-4BFB-AD8B-5510144C9A72}
static const IID IID_ILogDialog =
{ 0x6DF333F1, 0x26A6, 0x4BFB, { 0xAD, 0x8B, 0x55, 0x10, 0x14, 0x4C, 0x9A, 0x72 } };

BEGIN_INTERFACE_MAP(CLogDialog, CDialog)
	INTERFACE_PART(CLogDialog, IID_ILogDialog, Dispatch)
END_INTERFACE_MAP()

void CLogDialog::OnBnClickedLogClipboardButton()
{
	// 090223 LUJ, ���õ� ���ڵ尡 ������ ��ü ���縦 �Ѵ�
	const bool isWholeCopy = ( ! mResultListCtrl.GetSelectedCount() );

	mApplication.SaveToClipBoard(
		mResultListCtrl,
		isWholeCopy );

	CString textSucceed;
	textSucceed.LoadString( IDS_STRING439 );
	CString textMessage;
	textMessage.Format(
		textSucceed,
		isWholeCopy ? mResultListCtrl.GetItemCount() : mResultListCtrl.GetSelectedCount() );	
	MessageBox( textMessage, _T( "" ), MB_OK );
}

void CLogDialog::OnBnClickedLogExcelButton()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( mCommand )
	{
		mCommand->SaveToExcel( mServerIndex, mResultListCtrl );
	}
}

void CLogDialog::OnBnClickedLogFindButton()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	if( ! mCommand )
	{
		return;
	}

	CTime beginDate;
	mBeginDateCtrl.GetTime( beginDate );

	CTime beginTime;
	mBeginTimeCtrl.GetTime( beginTime );

	CTime endDate;
	mEndDateCtrl.GetTime( endDate );

	CTime endTime;	
	mEndTimeCtrl.GetTime( endTime );

	// 080425 LUJ, ���� ��¥���� ���� �ð��� ū ��� ����� ����
	if( beginDate	> CTime::GetCurrentTime() &&
		beginTime	> CTime::GetCurrentTime() )
	{
		CString textThereIsNoResult;
		textThereIsNoResult.LoadString( IDS_STRING1 );

		MessageBox( textThereIsNoResult, _T( "" ), MB_OK | MB_ICONERROR );
		return;
	}
	// 080425 LUJ, ���� �ð����� �� �ð��� �׻� Ŀ����(���Ƶ� �ǹ� ����)
	else if( beginDate > endDate )
	{
		CString textEndTimeMustBeSmallerThanBegin;
		textEndTimeMustBeSmallerThanBegin.LoadString( IDS_STRING237 );

		MessageBox( textEndTimeMustBeSmallerThanBegin, _T( "" ), MB_OK | MB_ICONERROR );
		return;
	}

	// 080403 LUJ,	������ �α״� �˻� ���̺��� �Ϻ��� �и��Ǿ� �־� �ٸ� �α׿ʹ� �޸� Ư���ϴ�. 
	//				RTTI�� �̿��Ͽ� ������ �α� ����� ��� ������ ó���� �Ѵ�
	{
		CCommand* command = dynamic_cast< CItemLogCommand* >( mCommand );

		if( command )
		{
			endDate = beginDate;

			// 080425 LUJ, ���� ��¥���� 90�� �����̶�� �˻��� �� ����. �� �Ⱓ�� �����Ϸ��� �������� ������ �ʿ�
			{
				const __time64_t dayOfDuration		= 90;
				const __time64_t secondOfday		= 86400;
				const __time64_t validDuration		= secondOfday * dayOfDuration;
				const __time64_t todayTime			= CTime::GetCurrentTime().GetTime();
				const __time64_t beginSearchTime	= beginDate.GetTime();
				
				if( todayTime > beginSearchTime &&
					validDuration <= ( todayTime - beginSearchTime ) )
				{
					CString textValidDurationOfLogIs;
					textValidDurationOfLogIs.LoadString( IDS_STRING370 );

					CString text;
					text.Format(  textValidDurationOfLogIs, dayOfDuration );

					MessageBox( text, _T( "" ), MB_OK | MB_ICONERROR );
					return;
				}
			}			
		}
	}

	if( CTime() == beginTime )
	{
		beginTime = CTime( beginDate.GetYear(), beginDate.GetMonth(), beginDate.GetDay(), 0, 0, 0 );
	}

	// 080403 LUJ, �� �ð��� 0�̸� �⺻������ �����Ѵ�
	if( CTime() == endTime )
	{
		endTime = CTime( endDate.GetYear(), endDate.GetMonth(), endDate.GetDay(), 23, 59, 59 );
	}

	CString serverName;
	mServerCombo.GetLBText(
		mServerCombo.GetCurSel(),
		serverName );

	mCommand->Find(
		mApplication.GetServerIndex( serverName ),
		beginDate.Format( _T( "%y%m%d" ) )	+ _T( " " ) + beginTime.Format( _T( "%H:%M:%S" ) ),
		endDate.Format( _T( "%y%m%d" ) )	+ _T( " " ) + endTime.Format( _T( "%H:%M:%S" ) ) );
}

void CLogDialog::OnBnClickedLogStopButton()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( mCommand )
	{
		mCommand->Stop( mServerIndex );
	}
}


BOOL CLogDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	if( mCommand )
	{
		mCommand->Initialize( mResultListCtrl );

		SetWindowText( mCommand->GetTitle() );
	}

	// ����Ʈ ��Ʈ�� �ʱ�ȭ
	mResultListCtrl.SetExtendedStyle(
		mResultListCtrl.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT );

	// ��ư �ʱ�ȭ
	mStopButton.EnableWindow( FALSE );

	// �޺� �ڽ� �ʱ�ȭ
	{
		mApplication.SetServerComboBox( mServerCombo );
		
		{
			CRect rect;
			mServerCombo.GetWindowRect( rect );
			mServerCombo.SetWindowPos( 0, 0, 0, rect.Width(), 200, SWP_NOMOVE | SWP_NOZORDER );

			const BOOL isEnableSelectionServer( UINT_MAX == mServerIndex );

			if( isEnableSelectionServer )
			{
				mServerCombo.EnableWindow( TRUE );
			}
			else
			{
				mServerCombo.EnableWindow( FALSE );

				const TCHAR* name = mApplication.GetServerName( mServerIndex );

				mServerCombo.SetCurSel(
					mServerCombo.FindString( 0, name ) );
			}
		}
	}

	// �ִ� �α� ���� ����
	{
		CString format;
		mMaxLogStatic.GetWindowText( format );

		CString text;
		text.Format( format, MAX_LOG_SIZE );

		mMaxLogStatic.SetWindowText( text );
	}

	// 080403 LUJ,	������ �α״� �˻� ���̺��� �Ϻ��� �и��Ǿ� �־� �ٸ� �α׿ʹ� �޸� Ư���ϴ�. 
	//				RTTI�� �̿��Ͽ� ������ �α� ����� ��� ������ ó���� �Ѵ�
	{
		CCommand* command = dynamic_cast< CItemLogCommand* >( mCommand );

		if( command )
		{
			mEndDateCtrl.ShowWindow( SW_HIDE );
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


void CLogDialog::Parse( DWORD serverIndex, const MSGROOT* message )
{
	mServerIndex = serverIndex;

	if( mCommand )
	{
		mCommand->Parse(
			message,
			mResultListCtrl,
			mProgressCtrl,
			mResultStatic,
			mFindButton,
			mStopButton );
	}
}


INT_PTR CLogDialog::DoModal( DWORD serverIndex, CCommand& command )
{
	mServerIndex	= serverIndex;
	mCommand		= &command;

	return CDialog::DoModal();
}