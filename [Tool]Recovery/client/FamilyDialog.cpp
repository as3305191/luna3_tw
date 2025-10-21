// FamilyDialog.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "client.h"
#include "FamilyDialog.h"
#include "PlayerDialog.h"
// 080403 LUJ, �α� ó��
#include "LogDialog.h"
#include "FamilyPointLogCommand.h"
#include "FarmCropLogCommand.h"
// 080630 LUJ, ���� �α�
#include "LivestockLogCommand.h"
// 081205 LUJ, �йи� �α�
#include "FamilyLogCommand.h"

IMPLEMENT_DYNAMIC(CFamilyDialog, CDialog)
CFamilyDialog::CFamilyDialog( CclientApp& application, CWnd* pParent /*=NULL*/)
	: CDialog(CFamilyDialog::IDD, pParent)
	, mApplication( application )
{
	EnableAutomation();

}

CFamilyDialog::~CFamilyDialog()
{
}

void CFamilyDialog::OnFinalRelease()
{
	// �ڵ�ȭ ��ü�� ���� ������ ������ �����Ǹ� OnFinalRelease��
	// ȣ��˴ϴ�. �⺻ Ŭ�������� �ڵ����� ��ü�� �����մϴ�.
	// �⺻ Ŭ������ ȣ���ϱ� ���� ��ü�� �ʿ��� �߰� ���� �۾���
	// �߰��Ͻʽÿ�.

	CDialog::OnFinalRelease();
}

void CFamilyDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//DDX_Control( pDX, IDC_FAMILY_MEMBER_LIST, mMemberListCtrl );
	DDX_Control(pDX, IDC_FAMILY_FARM_STATE_EDIT, mFarmStateEdit);
	DDX_Control(pDX, IDC_FAMILY_GARDEN_EDIT, mGardenGradeEdit);
	DDX_Control(pDX, IDC_FAMILY_HOUSE_EDIT, mHouseGradeEdit);
	DDX_Control(pDX, IDC_FAMILY_WAREHOUSE_EDIT, mWarehouseGradeEdit);
	DDX_Control(pDX, IDC_FAMILY_CAGE_EDIT, mCageGradeEdit);
	DDX_Control(pDX, IDC_FAMILY_FENCE_EDIT, mFenceGradeEdit);
	DDX_Control(pDX, IDC_FAMILY_CROP_LIST, mCropListCtrl);
	DDX_Control(pDX, IDC_FAMILY_NICK_EDIT, mNameEdit);
	DDX_Control(pDX, IDC_FAMILY_HONOR_EDIT, mHonorEdit);
	DDX_Control(pDX, IDC_FAMILY_ENABLE_NICK_EDIT, mEnableNickEdit);
	DDX_Control(pDX, IDC_FAMILY_BUILD_DATE_EDIT, mBuildDateEdit);
	DDX_Control(pDX, IDC_FAMILY_SERVER_COMBO, mServerCombo);
	DDX_Control(pDX, IDC_FAMILY_KEYWORD_EDIT, mKeywordEdit);
	DDX_Control(pDX, IDC_FAMILY_FIND_BUTTON, mFindButton);
	DDX_Control(pDX, IDC_FAMILY_RESULT_COMBO, mResultCombo);
	DDX_Control(pDX, IDC_FAMILY_RESULT_SIZE_STATIC, mResultSizeStatic);
	DDX_Control(pDX, IDC_FAMILY_DELETE_GUILD_CHECK, mDeletedCheck);
	DDX_Control(pDX, IDC_FAMILY_MEMBER_LIST2, mMemberListCtrl);
	DDX_Control(pDX, IDC_FAMILY_LIVESTOCK_LIST, mLivestockListCtrl);
	DDX_Control(pDX, IDC_FAMILY_FARM_LOG_BUTTON, mCropLogButton);
	DDX_Control(pDX, IDC_FAMILY_LIVESTOCK_LOG_BUTTON, mLivestockLogButton);
}


BEGIN_MESSAGE_MAP(CFamilyDialog, CDialog)
	ON_BN_CLICKED(IDC_FAMILY_MEMBER_BUTTON, OnBnClickedFamilyMemberButton)
	ON_BN_CLICKED(IDC_FAMILY_FIND_BUTTON, OnBnClickedFamilyFindButton)
	ON_BN_CLICKED(IDC_FAMILY_SPECIFIC_DATA_BUTTON, OnBnClickedFamilySpecificDataButton)
	ON_BN_CLICKED(IDC_FAMILY_POINT_LOG_BUTTON, OnBnClickedFamilyPointLogButton)
	ON_BN_CLICKED(IDC_FAMILY_FARM_LOG_BUTTON, OnBnClickedFamilyFarmLogButton)
	ON_BN_CLICKED(IDC_FAMILY_POINT_UPDATE_BUTTON, OnBnClickedFamilyPointUpdateButton)
	ON_BN_CLICKED(IDC_FAMILY_LIVESTOCK_LOG_BUTTON, OnBnClickedFamilyLivestockLogButton)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_FAMILY_MEMBER_LOG_BUTTON, OnBnClickedFamilyMemberLogButton)
	ON_BN_CLICKED(IDC_FAMILY_LOG_BUTTON, OnBnClickedFamilyLogButton)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CFamilyDialog, CDialog)
END_DISPATCH_MAP()

// ����: IID_IFamilyDialog�� ���� ������ �߰��Ͽ�
// VBA���� ���� �������� ���ε��� �����մϴ�. 
// �� IID�� .IDL ���Ͽ� �ִ� dispinterface�� GUID�� ��ġ�ؾ� �մϴ�.

// {F2BD1569-C26B-4C6C-8578-180BF25F8BD4}
static const IID IID_IFamilyDialog =
{ 0xF2BD1569, 0xC26B, 0x4C6C, { 0x85, 0x78, 0x18, 0xB, 0xF2, 0x5F, 0x8B, 0xD4 } };

BEGIN_INTERFACE_MAP(CFamilyDialog, CDialog)
	INTERFACE_PART(CFamilyDialog, IID_IFamilyDialog, Dispatch)
END_INTERFACE_MAP()


// CFamilyDialog �޽��� ó�����Դϴ�.

BOOL CFamilyDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon( AfxGetApp()->LoadIcon( IDR_MAINFRAME ), FALSE );
	SetIcon( AfxGetApp()->LoadIcon( IDR_MAINFRAME ), TRUE );

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	// list control initialization
	{
		const DWORD addStyle = LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT;
		CRect rect;

		{
			int step = -1;

			mMemberListCtrl.GetWindowRect( rect );

			CString textDbIndex;
			textDbIndex.LoadString( IDS_STRING13 );
			mMemberListCtrl.InsertColumn( ++step, textDbIndex, LVCFMT_LEFT, int( rect.Width() * 0.0f ) );

			CString textMaster;
			textMaster.LoadString( IDS_STRING19 );
			mMemberListCtrl.InsertColumn( ++step, textMaster, LVCFMT_LEFT, int( rect.Width() * 0.25f ) );

			CString textName;
			textName.LoadString( IDS_STRING20 );
			mMemberListCtrl.InsertColumn( ++step, textName, LVCFMT_LEFT, int( rect.Width() * 0.35f ) );

			CString textNick;
			textNick.LoadString( IDS_STRING21 );
			mMemberListCtrl.InsertColumn( ++step, textNick, LVCFMT_LEFT, int( rect.Width() * 0.2f ) );

			CString textLevel;
			textLevel.LoadString( IDS_STRING5 );
			mMemberListCtrl.InsertColumn( ++step, textLevel, LVCFMT_LEFT, int( rect.Width() * 0.19f ) );

			mMemberListCtrl.SetExtendedStyle( mMemberListCtrl.GetExtendedStyle() | addStyle );
		}
		
		{
			int step = -1;

			mCropListCtrl.GetWindowRect( rect );

			CString text;

			// message: number
			text.LoadString( IDS_STRING22 );
			mCropListCtrl.InsertColumn( ++step, text, LVCFMT_LEFT, int( rect.Width() * 0.0f ) );

			// message: owner player index
			text.LoadString( IDS_STRING12 );
			mCropListCtrl.InsertColumn( ++step, text, LVCFMT_LEFT, int( rect.Width() * 0.0f ) );

			// message: owner player name
			text.LoadString( IDS_STRING12 );
			mCropListCtrl.InsertColumn( ++step, text, LVCFMT_LEFT, int( rect.Width() * 0.2f ) );

			// message: type
			text.LoadString( IDS_STRING4 );
			mCropListCtrl.InsertColumn( ++step, text, LVCFMT_LEFT, int( rect.Width() * 0.2f ) );

			// message: life
			text.LoadString( IDS_STRING15 );
			mCropListCtrl.InsertColumn( ++step, text, LVCFMT_LEFT, int( rect.Width() * 0.15f ) );

			// message: seed quality
			text.LoadString( IDS_STRING16 );
			mCropListCtrl.InsertColumn( ++step, text, LVCFMT_LEFT, int( rect.Width() * 0.14f ) );
			mCropListCtrl.SetExtendedStyle( mCropListCtrl.GetExtendedStyle() | addStyle );
		}

		// 080731 LUJ, ���� ���
		{
			int step = -1;

			mLivestockListCtrl.GetWindowRect( rect );

			CString text;

			// message: number
			text.LoadString( IDS_STRING22 );
			mLivestockListCtrl.InsertColumn( ++step, text, LVCFMT_LEFT, int( rect.Width() * 0.0f ) );

			// message: owner player index
			text.LoadString( IDS_STRING12 );
			mLivestockListCtrl.InsertColumn( ++step, text, LVCFMT_LEFT, int( rect.Width() * 0.0f ) );

			// message: owner player name
			text.LoadString( IDS_STRING12 );
			mLivestockListCtrl.InsertColumn( ++step, text, LVCFMT_LEFT, int( rect.Width() * 0.2f ) );

			// message: type
			text.LoadString( IDS_STRING4 );
			mLivestockListCtrl.InsertColumn( ++step, text, LVCFMT_LEFT, int( rect.Width() * 0.2f ) );

			// message: life
			text.LoadString( IDS_STRING15 );
			mLivestockListCtrl.InsertColumn( ++step, text, LVCFMT_LEFT, int( rect.Width() * 0.15f ) );

			// message: contentment
			text.LoadString( IDS_STRING389 );
			mLivestockListCtrl.InsertColumn( ++step, text, LVCFMT_LEFT, int( rect.Width() * 0.14f ) );

			// message: interest
			text.LoadString( IDS_STRING390 );
			mLivestockListCtrl.InsertColumn( ++step, text, LVCFMT_LEFT, int( rect.Width() * 0.14f ) );

			mLivestockListCtrl.SetExtendedStyle( mLivestockListCtrl.GetExtendedStyle() | addStyle );
		}
	}

	// �޺� �ڽ� ����
	{
		CRect rect;
		mServerCombo.GetWindowRect( rect );
		mServerCombo.SetWindowPos( 0, 0, 0, rect.Width(), 200, SWP_NOMOVE | SWP_NOZORDER );
		
		mResultCombo.GetWindowRect( rect );
		mResultCombo.SetWindowPos( 0, 0, 0, rect.Width(), 200, SWP_NOMOVE | SWP_NOZORDER );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CFamilyDialog::Parse( DWORD serverIndex, const MSGROOT* message )
{
	mServerIndex = serverIndex;

	switch( message->Protocol )
	{
	case MP_RM_FAMILY_GET_LIST_ACK:
		{
			const MSG_RM_FAMILY_LIST* m = ( MSG_RM_FAMILY_LIST* )message;

			CString text;

			mResultCombo.ResetContent();
			mFamilyMasterMap.clear();

			for( DWORD i = 0; i < m->mSize; ++i )
			{
				const MSG_RM_FAMILY_LIST::Family& data = m->mFamily[ i ];

				const CString name( CA2WEX< sizeof( data.mName ) >( data.mName ) );

				text.Format( _T( "%s (%d)" ), name, data.mSize );

				mFamilyMasterMap.insert( std::make_pair( mResultCombo.GetCount(), data.mIndex ) );

				mResultCombo.AddString( text );
			}

			mResultCombo.EnableWindow( 0 < m->mSize );

			if( m->mSize )
			{
				text.Format( _T( "%d" ), m->mSize );
				mResultSizeStatic.SetWindowText( text );

				mResultCombo.SetCurSel( 0 );
			}
			else
			{
				mResultSizeStatic.SetWindowText( _T( "" ) );

				CString textThereIsNoResult;
				textThereIsNoResult.LoadString( IDS_STRING1 );
				MessageBox( textThereIsNoResult, _T( "" ), MB_ICONINFORMATION | MB_OK );
			}

			break;
		}
	case MP_RM_FAMILY_GET_LIST_NACK:
		{
			break;
		}
	case MP_RM_FAMILY_GET_DATA_ACK:
		{
			const MSG_RM_FAMILY_DATA* m = ( MSG_RM_FAMILY_DATA* )message;

			mNameEdit.SetWindowText( CA2WEX< sizeof( m->mName ) >( m->mName ) );

			CString text;
			text.Format( _T( "%d" ), m->mHonorPoint );
			mHonorEdit.SetWindowText( text );

			{
				CString enableString;
				CString unableString;

				enableString.LoadString( IDS_STRING25 );
				unableString.LoadString( IDS_STRING26 );

				mEnableNickEdit.SetWindowText( m->mIsEnableNick ? enableString : unableString );
			}
			
			mBuildDateEdit.SetWindowText( CA2WEX< sizeof( m->mBuildDate ) >( m->mBuildDate ) );

			mFamilyIndex = m->mIndex;
			
			break;
		}
	case MP_RM_FAMILY_GET_MEMBER_ACK:
		{
			const MSG_RM_FAMILY_MEMBER* m = ( MSG_RM_FAMILY_MEMBER* )message;

			mMemberListCtrl.DeleteAllItems();

			for( DWORD i = 0; i < m->mSize; ++i )
			{
				const MSG_RM_FAMILY_MEMBER::Player& player = m->mPlayer[ i ];
				CString text;

				text.Format( _T( "%d" ),player.mIndex );
				mMemberListCtrl.InsertItem( i, text, 0 );
				
				if( player.mIsMaster )
				{
					mMemberListCtrl.SetItemText( i, 1, _T( "*" ) );
				}

				mMemberListCtrl.SetItemText( i, 2, CA2WEX< sizeof( player.mName ) >( player.mName ) );
				mMemberListCtrl.SetItemText( i, 3, CA2WEX< sizeof( player.mNick ) >( player.mNick ) );

				text.Format( _T( "%d" ), player.mLevel );
				mMemberListCtrl.SetItemText( i, 4, text );
			}
			
			break;
		}
	case MP_RM_FAMILY_GET_DATA_NACK:
		{
			break;
		}
	case MP_RM_FAMILY_GET_FARM_ACK:
		{
			const MSG_RM_FAMILY_FARM* m = ( MSG_RM_FAMILY_FARM* )message;
			CString text;

			text.Format( _T( "%d" ), m->mState );
			mFarmStateEdit.SetWindowText( text );

			text.Format( _T( "%d" ), m->mGardenGrade );
			mGardenGradeEdit.SetWindowText( text );

			text.Format( _T( "%d" ), m->mHouseGrade );
			mHouseGradeEdit.SetWindowText( text );

			text.Format( _T( "%d" ), m->mWarehouseGrade );
			mWarehouseGradeEdit.SetWindowText( text );

			text.Format( _T( "%d" ), m->mCageGrade );
			mCageGradeEdit.SetWindowText( text );

			text.Format( _T( "%d" ), m->mFenceGrade );
			mFenceGradeEdit.SetWindowText( text );

			mFarmIndex = m->mFarmIndex;

			{
				mFarmStateEdit.EnableWindow( TRUE );
				mGardenGradeEdit.EnableWindow( TRUE );
				mHouseGradeEdit.EnableWindow( TRUE );
				mWarehouseGradeEdit.EnableWindow( TRUE );
				mCageGradeEdit.EnableWindow( TRUE );
				mFenceGradeEdit.EnableWindow( TRUE );
			}

			break;
		}
	case MP_RM_FAMILY_GET_CROP_ACK:
		{
			const MSG_RM_FAMILY_CROP* m = ( MSG_RM_FAMILY_CROP* )message;

			mCropListCtrl.DeleteAllItems();
			CString text;

			for( DWORD i = 0; i < m->mSize; ++i )
			{
				const MSG_RM_FAMILY_CROP::Crop& data	= m->mCrop[ i ];
				const DWORD						row		= mCropListCtrl.GetItemCount();
				int								step	= 0;
			
				text.Format( _T( "%d" ), data.mIndex );
				mCropListCtrl.InsertItem( row, text, 0 );

				text.Format( _T( "%d" ), data.mOwnerIndex );
				mCropListCtrl.SetItemText( row, ++step, text );

				mCropListCtrl.SetItemText( row, ++step, CA2WEX< sizeof( data.mOwnerName ) >( data.mOwnerName ) );

				// 080725 LUJ, ���۹� �̸��� ǥ���ϵ��� ��
				mCropListCtrl.SetItemText( row, ++step, mApplication.GetCropName( data.mKind, data.mStep ) );

				text.Format( _T( "%d" ), data.mLife );
				mCropListCtrl.SetItemText( row, ++step, text );

				text.Format( _T( "%d" ), data.mSeedGrade );
				mCropListCtrl.SetItemText( row, ++step, text );
			}

			mCropListCtrl.EnableWindow( 0 < mCropListCtrl.GetItemCount() );			
			break;
		}
		// 080731 LUJ, ���� ��� ǥ��
	case MP_RM_FAMILY_GET_LIVESTOCK_ACK:
		{
			const MSG_RM_LIVESTOCK* m = ( MSG_RM_LIVESTOCK* )message;

			mLivestockListCtrl.DeleteAllItems();
			CString text;

			for( DWORD i = 0; i < m->mSize; ++i )
			{
				const MSG_RM_LIVESTOCK::Animal& data	= m->mAnimal[ i ];
				const DWORD						row		= mLivestockListCtrl.GetItemCount();
				int								step	= 0;

				text.Format( _T( "%d" ), data.mIndex );
				mLivestockListCtrl.InsertItem( row, text, 0 );

				text.Format( _T( "%d" ), data.mOwnerIndex );
				mLivestockListCtrl.SetItemText( row, ++step, text );

				mLivestockListCtrl.SetItemText( row, ++step, CA2WEX< sizeof( data.mOwnerName ) >( data.mOwnerName ) );
				mLivestockListCtrl.SetItemText( row, ++step, mApplication.GetLivestockName( data.mKind ) );

				text.Format( _T( "%d" ), data.mLife );
				mLivestockListCtrl.SetItemText( row, ++step, text );

				text.Format( _T( "%d" ), data.mContentment );
				mLivestockListCtrl.SetItemText( row, ++step, text );

				text.Format( _T( "%d" ), data.mInterest );
				mLivestockListCtrl.SetItemText( row, ++step, text );
			}

			mLivestockListCtrl.EnableWindow( 0 < mLivestockListCtrl.GetItemCount() );
			break;
		}
	case MP_RM_FAMILY_GET_FARM_NACK:
		{
			mFarmStateEdit.EnableWindow( FALSE );
			mGardenGradeEdit.EnableWindow( FALSE );
			mHouseGradeEdit.EnableWindow( FALSE );
			mWarehouseGradeEdit.EnableWindow( FALSE );
			mCageGradeEdit.EnableWindow( FALSE );
			mFenceGradeEdit.EnableWindow( FALSE );
			break;
		}
	case MP_RM_FAMILY_SET_DATA_ACK:
		{
			CString textUpdateSucceed;
			textUpdateSucceed.LoadString( IDS_STRING28 );

			MessageBox( textUpdateSucceed, _T( "" ), MB_OK | MB_ICONINFORMATION );
			break;
		}
	case MP_RM_FAMILY_SET_DATA_NACK:
		{
			CString textUpdateFailed;
			textUpdateFailed.LoadString( IDS_STRING29 );
			MessageBox( textUpdateFailed, _T( "" ), MB_OK | MB_ICONINFORMATION );
			break;
		}
	case MP_RM_FAMILY_NACK_BY_AUTH:
		{
			CString textThereIsNoResult;
			textThereIsNoResult.LoadString( IDS_STRING18 );
			
			MessageBox( textThereIsNoResult, _T( "" ), MB_OK | MB_ICONERROR );
			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}
}


// 080731 LUJ, ���ڸ� �÷��̾� ��ȣ���� �йи� ��ȣ�� ����
void CFamilyDialog::Request( DWORD serverIndex, const CString& playerName, DWORD familyIndex )
{
	mServerIndex	= serverIndex;
	mPlayerName		= playerName;
	mFamilyIndex	= familyIndex;

	{
		MSGBASE message;
		ZeroMemory( &message, sizeof( message ) );
		message.Category	= MP_RM_FAMILY;
		message.Protocol	= MP_RM_FAMILY_GET_DATA_SYN;
		message.dwObjectID	= familyIndex;

		mApplication.Send( mApplication.GetServerName( serverIndex ), message, sizeof( message ) );
	}
}


void CFamilyDialog::OnBnClickedFamilyMemberButton()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	POSITION		position	= mMemberListCtrl.GetFirstSelectedItemPosition();
	const DWORD		row			= mMemberListCtrl.GetNextSelectedItem( position );
	const CString	playerIndex	= mMemberListCtrl.GetItemText( row, 0 );
	
	if( playerIndex.IsEmpty() )
	{
		CString textThereIsNoTarget;
		textThereIsNoTarget.LoadString( IDS_STRING31 );
		MessageBox( textThereIsNoTarget, _T( "" ), MB_ICONERROR | MB_OK );
		return;
	}

	const CString	playerName	= mMemberListCtrl.GetItemText( row, 2 );

	{
		CPlayerDialog* dialog = mApplication.GetPlayerDialog();
		ASSERT( dialog );

		dialog->Request( mServerIndex, playerName, _ttoi( playerIndex ) );
	}
}

void CFamilyDialog::OnBnClickedFamilyFindButton()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	CString keyword;

	mKeywordEdit.GetWindowText( keyword );

	if( keyword.IsEmpty() )
	{
		CString textKeywordIsEmpty;
		textKeywordIsEmpty.LoadString( IDS_STRING32 );

		MessageBox(	textKeywordIsEmpty, _T( "" ), MB_ICONERROR | MB_OK );
		return;
	}

	MSG_NAME message;
	{
		ZeroMemory( &message, sizeof( message ) );
		message.Category	= MP_RM_FAMILY;
		message.Protocol	= MP_RM_FAMILY_GET_LIST_SYN;
		StringCopySafe( message.Name, CW2AEX< MAX_PATH >( keyword ), sizeof( message.Name ) );
	}

	{
		CString serverName;

		mServerCombo.GetLBText( mServerCombo.GetCurSel(), serverName );

		mApplication.ConvertWildCardForDb( message.Name );	
		mApplication.Send( serverName, message, sizeof( message ) );
	}	
}

void CFamilyDialog::OnBnClickedFamilySpecificDataButton()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	CString text;

	mResultCombo.GetLBText( mResultCombo.GetCurSel(), text );

	int i = 0;
	const CString familyName( text.Tokenize( _T( " " ), i ) );

	if( familyName.IsEmpty() )
	{
		CString textThereIsNoTarget;
		textThereIsNoTarget.LoadString( IDS_STRING31 );

		MessageBox( textThereIsNoTarget, _T( "" ), MB_ICONERROR | MB_OK );
		return;
	}

	
	{
		MSGBASE message;
		ZeroMemory( &message, sizeof( message ) );
		message.Category	= MP_RM_FAMILY;
		message.Protocol	= MP_RM_FAMILY_GET_DATA_SYN;

		FamilyMasterMap::const_iterator it = mFamilyMasterMap.find( mResultCombo.GetCurSel() );

		if( mFamilyMasterMap.end() == it )
		{
			ASSERT( 0 );
			return;
		}

		message.dwObjectID = it->second;
		mApplication.Send( mServerIndex, message, sizeof( message ) );
	}
}


void CFamilyDialog::OnBnClickedFamilyPointLogButton()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	CString familyName;
	mNameEdit.GetWindowText( familyName );

	if( familyName.IsEmpty() )
	{
		CString textThereIsNoTarget;
		textThereIsNoTarget.LoadString( IDS_STRING31 );

		MessageBox( textThereIsNoTarget, _T( "" ), MB_ICONERROR | MB_OK );
		return;
	}

	CLogDialog* dialog = mApplication.GetLogDialog();

	if( dialog )
	{
		CString textFamily;
		textFamily.LoadString( IDS_STRING165 );

		CString textLog;
		textLog.LoadString( IDS_STRING10 );

		CString textIndex;
		textIndex.Format( _T( "%d" ), mFamilyIndex );

		const CString title( textFamily + _T( " " ) + textLog + _T( ": " ) + familyName + _T( "(" ) + textIndex + _T( ")" ) );

		dialog->DoModal(
			mServerIndex,
			CFamilyPointLogCommand( mApplication, title, mFamilyIndex ) );
	}
}

void CFamilyDialog::OnBnClickedFamilyFarmLogButton()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	CString familyName;
	mNameEdit.GetWindowText( familyName );

	if( familyName.IsEmpty() )
	{
		CString textThereIsNoTarget;
		textThereIsNoTarget.LoadString( IDS_STRING31 );

		MessageBox( textThereIsNoTarget, _T( "" ), MB_ICONERROR | MB_OK );
		return;
	}
	// �ش� ��Ʈ���� Ȱ��ȭ�� ���°� �ƴϸ� ������ ���ٴ� ��
	else if( ! mFarmStateEdit.IsWindowEnabled() )
	{
		CString textThereIsNoFarm;
		textThereIsNoFarm.LoadString( IDS_STRING33 );
		MessageBox( textThereIsNoFarm, _T( "" ), MB_ICONERROR | MB_OK );
		return;
	}

	CLogDialog* dialog = mApplication.GetLogDialog();

	if( dialog )
	{
		CString textFarm;
		textFarm.LoadString( IDS_STRING362 );

		CString textCrop;
		textCrop.LoadString( IDS_STRING14 );

		CString textLog;
		textLog.LoadString( IDS_STRING10 );

		CString textFamilyIndex;
		textFamilyIndex.Format( _T( "(%d)" ), mFamilyIndex );

		const CString title( textFarm + _T( " " ) + textCrop + _T( " " ) + textLog + _T( ": " ) + familyName + textFamilyIndex );

		dialog->DoModal(
			mServerIndex,
			CFarmCropLogCommand( mApplication, title, mFarmIndex ) );
	}
}


void CFamilyDialog::OnBnClickedFamilyPointUpdateButton()
{
	// 080624 LUJ, ��� �߰�
	{
		CString textMessage;
		textMessage.LoadString( IDS_STRING385 );
		
		if( IDCANCEL == MessageBox(
			textMessage,
			_T( "" ),
			MB_OKCANCEL ) )
		{
			return;
		}
	}

	CString point;
	mHonorEdit.GetWindowText( point );
	// 080623 LUJ, �йи� �̸��� �ٲ� �� �ֵ��� ���� �߰�
	CString name;
	mNameEdit.GetWindowText( name );

	MSG_RM_FAMILY_DATA message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category	= MP_RM_FAMILY;
	message.Protocol	= MP_RM_FAMILY_SET_DATA_SYN;
	message.dwObjectID	= mFamilyIndex;
	message.mHonorPoint	= _ttoi( point );
	StringCopySafe( message.mName,	CW2AEX< MAX_PATH >( name ),	sizeof( message.mName ) );

	mApplication.Send( mServerIndex, message, sizeof( message ) );
}


void CFamilyDialog::OnBnClickedFamilyLivestockLogButton()
{
	CString familyName;
	mNameEdit.GetWindowText( familyName );

	if( familyName.IsEmpty() )
	{
		CString textThereIsNoTarget;
		textThereIsNoTarget.LoadString( IDS_STRING31 );

		MessageBox( textThereIsNoTarget, _T( "" ), MB_ICONERROR | MB_OK );
		return;
	}
	// �ش� ��Ʈ���� Ȱ��ȭ�� ���°� �ƴϸ� ������ ���ٴ� ��
	else if( ! mFarmStateEdit.IsWindowEnabled() )
	{
		CString textThereIsNoFarm;
		textThereIsNoFarm.LoadString( IDS_STRING33 );
		MessageBox( textThereIsNoFarm, _T( "" ), MB_ICONERROR | MB_OK );
		return;
	}

	CLogDialog* dialog = mApplication.GetLogDialog();

	if( dialog )
	{
		CString textLivestock;
		textLivestock.LoadString( IDS_STRING388 );

		CString textLog;
		textLog.LoadString( IDS_STRING10 );

		CString textFamilyIndex;
		textFamilyIndex.Format( _T( "(%d)" ), mFamilyIndex );

		const CString title( textLivestock + _T( " " ) + textLog + _T( ": " ) + familyName + textFamilyIndex );

		dialog->DoModal(
			mServerIndex,
			CLivestockLogCommand( mApplication, title, mFarmIndex ) );
	}
}


// 080702 LUJ, ��Ʈ��ũ �ʱ�ȭ�� â ���� �Ŀ� �ǹǷ�, â�� ǥ�õ� �� ������ ��Ÿ���� �޺� �ڽ� ���� �ʱ�ȭ���־�� �Ѵ�
void CFamilyDialog::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	mApplication.SetServerComboBox( mServerCombo );
}

void CFamilyDialog::OnBnClickedFamilyMemberLogButton()
{
	CLogDialog* const dialog = mApplication.GetLogDialog();

	if( dialog &&
		mFamilyIndex )
	{
		CString textFamily;
		textFamily.LoadString( IDS_STRING165 );
		CString textLog;
		textLog.LoadString( IDS_STRING10 );
		CString textName;
		mNameEdit.GetWindowText( textName );
		CString title;
		title.Format(
			_T( "%s %s: %s(%d)" ),
			textFamily,
			textLog,
			textName,
			mFamilyIndex );

		CFamilyLogCommand::Configuration configuration = { 0 };
		configuration.mFamilyIndex = mFamilyIndex;
		dialog->DoModal(
			mServerIndex,
			CFamilyLogCommand( mApplication, title, configuration ) );
	}
}

void CFamilyDialog::OnBnClickedFamilyLogButton()
{
	CLogDialog* dialog = mApplication.GetLogDialog();

	if( dialog )
	{
		CString textFamily;
		textFamily.LoadString( IDS_STRING165 );
		CString textLog;
		textLog.LoadString( IDS_STRING10 );

		const CFamilyLogCommand::Configuration configuration = { 0 };
		dialog->DoModal(
			UINT_MAX,
			CFamilyLogCommand( mApplication, textFamily + _T( " " ) + textLog, configuration ) );
	}
}