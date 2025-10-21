// GuildDialog.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "client.h"
#include "GuildDialog.h"
#include "PlayerDialog.h"
// 080403 LUJ, Ŀ�ǵ� ó��
#include "LogDialog.h"
#include "GuildLogCommand.h"
#include "GuildScoreLogCommand.h"
#include "GuildWarehouseLogCommand.h"

IMPLEMENT_DYNAMIC(CGuildDialog, CDialog)
CGuildDialog::CGuildDialog( CclientApp& application, CWnd* pParent /*=NULL*/)
	: CDialog(CGuildDialog::IDD, pParent)
	, mApplication( application )
	, mServerIndex( 0 )
{
	EnableAutomation();

	ZeroMemory( &mDataMessage, sizeof( mDataMessage ) );
}


CGuildDialog::~CGuildDialog()
{
}

void CGuildDialog::OnFinalRelease()
{
	// �ڵ�ȭ ��ü�� ���� ������ ������ �����Ǹ� OnFinalRelease��
	// ȣ��˴ϴ�. �⺻ Ŭ�������� �ڵ����� ��ü�� �����մϴ�.
	// �⺻ Ŭ������ ȣ���ϱ� ���� ��ü�� �ʿ��� �߰� ���� �۾���
	// �߰��Ͻʽÿ�.

	CDialog::OnFinalRelease();
}

void CGuildDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//DDX_Control(pDX, IDC_GUILD_SERVER_COMBO, mServerCombo);
	DDX_Control(pDX, IDC_GUILD_KEYWORD_EDIT, mKeywordEdit);
	DDX_Control(pDX, IDC_GUILD_FIND_BUTTON, mFindButton);
	DDX_Control(pDX, IDC_GUILD_RESULT_COMBO, mResultCombo);
	DDX_Control(pDX, IDC_GUILD_DELETE_GUILD_CHECK, mDeletedGuildCheck);
	DDX_Control(pDX, IDC_GUILD_SPECIFIC_DATA_BUTTON, mSpecificDataButton);
	DDX_Control(pDX, IDC_GUILD_NAME_EDIT, mNameEdit);
	DDX_Control(pDX, IDC_GUILD_BUILD_DATE_EDIT, mBuildDateEdit);
	DDX_Control(pDX, IDC_GUILD_LEVEL_EDIT, mLevelEdit);
	DDX_Control(pDX, IDC_GUILD_SCORE_EDIT, mScoreEdit);
	DDX_Control(pDX, IDC_GUILD_ALLIANCE_NAME_EDIT, mAllianceEdit);
	DDX_Control(pDX, IDC_GUILD_DATA_UPDATE_BUTTON, mDataUpdateButton);
	DDX_Control(pDX, IDC_GUILD_STORE_LIST, mStoreItemList);
	DDX_Control(pDX, IDC_GUILD_MONEY_EDIT, mMoneyEdit);
	DDX_Control(pDX, IDC_GUILD_MONEY_UPDATE_BUTTON, mMoneyUpdateButton);
	DDX_Control(pDX, IDC_GUILD_STORE_LOG_BUTTON, mStoreLogButton);
	DDX_Control(pDX, IDC_GUILD_MEMBER_LIST, mMemberList);
	DDX_Control(pDX, IDC_GUILD_SELECTED_MEMBER_BUTTON, mViewMemberButton);
	DDX_Control(pDX, IDC_GUILD_ALLIANCE_LIST, mFriendlyList);
	DDX_Control(pDX, IDC_GUILD_FRIENDLY_DATA_BUTTON, mFriendlyDataButton);
	DDX_Control(pDX, IDC_GUILD_ENEMY_LIST, mEnemyList);
	DDX_Control(pDX, IDC_GUILD_ENEMY_DATA_BUTTON, mEnemeyDataButton);
	DDX_Control(pDX, IDC_GUILD_ALLIANCE_ENTRY_DATE_EDIT, mAllianceEntryDateEdit);
	DDX_Control(pDX, IDC_GUILD_RESULT_SIZE_STATIC, mResultSizeStatic);
	DDX_Control(pDX, IDC_GUILD_SERVER_COMBO2, mServerCombo);
	DDX_Control(pDX, IDC_GUILD_RANK_COMBO, mRankCombo);
	DDX_Control(pDX, IDC_GUILD_SKILL_LIST, mSkillListCtrl);
}


BEGIN_MESSAGE_MAP(CGuildDialog, CDialog)
	//ON_EN_CHANGE(IDC_GUILD_UNION_NAME_EDIT2, OnEnChangeGuildUnionNameEdit2)
	ON_BN_CLICKED(IDC_GUILD_FIND_BUTTON, OnBnClickedGuildFindButton)
	ON_BN_CLICKED(IDC_GUILD_SPECIFIC_DATA_BUTTON, OnBnClickedGuildSpecificDataButton)
	ON_BN_CLICKED(IDC_GUILD_DATA_UPDATE_BUTTON, OnBnClickedGuildDataUpdateButton)
	ON_BN_CLICKED(IDC_GUILD_MONEY_UPDATE_BUTTON, OnBnClickedGuildMoneyUpdateButton)
	ON_BN_CLICKED(IDC_GUILD_SELECTED_MEMBER_BUTTON, OnBnClickedGuildSelectedMemberButton)
	ON_BN_CLICKED(IDC_GUILD_FRIENDLY_DATA_BUTTON, OnBnClickedGuildFriendlyDataButton)
	ON_BN_CLICKED(IDC_GUILD_ENEMY_DATA_BUTTON, OnBnClickedGuildEnemyDataButton)
	ON_BN_CLICKED(IDC_GUILD_LOG_BUTTON, OnBnClickedGuildLogButton)
	ON_BN_CLICKED(IDC_GUILD_STORE_LOG_BUTTON, OnBnClickedGuildStoreLogButton)
	ON_BN_CLICKED(IDC_GUILD_SCORE_LOG_BUTTON, OnBnClickedGuildScoreLogButton)
	ON_BN_CLICKED(IDC_GUILD_MEMBER_MODIFY_BUTTON, OnBnClickedGuildMemberModifyButton)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_GUILD_MEMBER_LIST, OnLvnItemchangedGuildMemberList)
	ON_CBN_SELCHANGE(IDC_GUILD_RANK_COMBO, OnCbnSelchangeGuildRankCombo)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_GUILD_MEMBER_LIST, OnLvnColumnclickGuildMemberList)
	ON_BN_CLICKED(IDC_GUILD_SKILL_DELETE_BUTTON, OnBnClickedGuildSkillDeleteButton)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_GUILD_LOG_ALL_BUTTON, OnBnClickedGuildLogAllButton)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CGuildDialog, CDialog)
END_DISPATCH_MAP()

// ����: IID_IGuildDialog�� ���� ������ �߰��Ͽ�
// VBA���� ���� �������� ���ε��� �����մϴ�. 
// �� IID�� .IDL ���Ͽ� �ִ� dispinterface�� GUID�� ��ġ�ؾ� �մϴ�.

// {BC3B6879-2097-4B93-8867-49649BB53B46}
static const IID IID_IGuildDialog =
{ 0xBC3B6879, 0x2097, 0x4B93, { 0x88, 0x67, 0x49, 0x64, 0x9B, 0xB5, 0x3B, 0x46 } };

BEGIN_INTERFACE_MAP(CGuildDialog, CDialog)
	INTERFACE_PART(CGuildDialog, IID_IGuildDialog, Dispatch)
END_INTERFACE_MAP()

void CGuildDialog::Parse( DWORD serverIndex, const MSGROOT* message )
{
	mServerIndex = serverIndex;

	switch( message->Protocol )
	{
	case MP_RM_GUILD_GET_LIST_ACK:
		{
			const MSG_RM_GUILD_LIST* m = ( MSG_RM_GUILD_LIST* )message;
			
			CString text;

			mResultCombo.ResetContent();

			for( DWORD i = 0; i < m->mSize; ++i )
			{
				const MSG_RM_GUILD_LIST::Guild& data = m->mGuild[ i ];

				const CString name( CA2WEX< sizeof( data.mName ) >( data.mName ) );

				CString textTitleFormat;
				textTitleFormat.LoadString( IDS_STRING35 );
				text.Format( textTitleFormat, name, data.mSize, data.mLevel );

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
	case MP_RM_GUILD_GET_LIST_NACK:
		{
			break;
		}
	case MP_RM_GUILD_GET_DATA_ACK:
		{
			// 080425 LUJ, ��� ��ų ��� ����
			mSkillListCtrl.DeleteAllItems();

			memcpy( &mDataMessage, message, sizeof( mDataMessage ) );

			mGuildIndex = mDataMessage.mGuildIndex;

			CString text;

			mNameEdit.SetWindowText( CA2WEX< sizeof( mDataMessage.mGuildName ) >( mDataMessage.mGuildName ) );

			mBuildDateEdit.SetWindowText( CA2WEX< sizeof( mDataMessage.mBuiltDate ) >( mDataMessage.mBuiltDate ) );

			text.Format( _T( "%d" ), mDataMessage.mLevel );
			mLevelEdit.SetWindowText( text );

			text.Format( _T( "%d" ), mDataMessage.mScore );
			mScoreEdit.SetWindowText( text );

			mAllianceEdit.SetWindowText( CA2WEX< sizeof( mDataMessage.mAllianceName ) >( mDataMessage.mAllianceName ) );

			text.Format( _T( "%u" ), mDataMessage.mMoney );
			mMoneyEdit.SetWindowText( text );

			if( mDataMessage.mAllianceEntryDate )
			{
				stTIME time;
				time.SetTime( mDataMessage.mAllianceEntryDate );

				text.Format( _T( "%d.%d.%d %d:%d:%d" ),
					2000 + time.GetYear(),
					time.GetMonth(),
					time.GetDay(),
					time.GetHour(),
					time.GetMinute(),
					time.GetSecond() );
				mAllianceEntryDateEdit.SetWindowText( text );
			}

			break;
		}
	case MP_RM_GUILD_GET_DATA_NACK:
		{
			break;
		}
	case MP_RM_GUILD_GET_STORE_ACK:
		{
			const MSG_RM_GUILD_STORE* m = ( MSG_RM_GUILD_STORE* )message;

			CString text;

			mStoreItemList.DeleteAllItems();

			for( DWORD i = 0; i < m->mSize; ++i )
			{
				const MSG_RM_GUILD_STORE::Item& data = m->mItem[ i ];

				text.Format( _T( "%d" ), data.mDbIndex );
				mStoreItemList.InsertItem( i, text, 0 );

				mStoreItemList.SetItemText( i, 1, mApplication.GetItemName( data.mIndex ) );

				if( mApplication.IsDuplicateItem( data.mIndex ) )
				{
					text.Format( _T( "%d" ), data.mQuantity );
					mStoreItemList.SetItemText( i, 2, text );
				}
			}

			break;
		}
	case MP_RM_GUILD_GET_MEMBER_ACK:
		{
			const MSG_RM_GUILD_MEMBER* m = ( MSG_RM_GUILD_MEMBER* )message;

			CString text;

			mMemberList.DeleteAllItems();

			for( DWORD i = 0; i < m->mSize; ++i )
			{
				const MSG_RM_GUILD_MEMBER::Player& data = m->mPlayer[ i ];

				text.Format( _T( "%d" ), data.mIndex );
				mMemberList.InsertItem( i, text, 0 );

				mMemberList.SetItemText( i, 1, mApplication.GetRankName( data.mRank ) );
				mMemberList.SetItemText( i, 2, CA2WEX< sizeof( data.mName ) >( data.mName ) );

				text.Format( _T( "%d" ), data.mLevel );
				mMemberList.SetItemText( i, 3, text );

				mMemberList.SetItemText( i, 4, CA2WEX< sizeof( data.mDate ) >( data.mDate ) );
			}

			mRankCombo.SetCurSel( 0 );

			break;
		}
	case MP_RM_GUILD_GET_FRIENDLY_ACK:
		{
			const MSG_RM_GUILD_LIST* m = ( MSG_RM_GUILD_LIST* )message;

			mFriendlyList.DeleteAllItems();

			CString text;

			for( DWORD i = 0; i < m->mSize; ++i )
			{
				const MSG_RM_GUILD_LIST::Guild& data = m->mGuild[ i ];

				text.Format( _T( "%d" ), data.mIndex );
				mFriendlyList.InsertItem( i, text, 0 );

				mFriendlyList.SetItemText( i, 1, CA2WEX< sizeof( data.mName ) >( data.mName ) );

				text.Format( _T( "%d" ), data.mSize );
				mFriendlyList.SetItemText( i, 2, text );

				text.Format( _T( "%d" ), data.mLevel );
				mFriendlyList.SetItemText( i, 3, text );
			}

			mFriendlyDataButton.EnableWindow( 0 < m->mSize );
			break;
		}
	case MP_RM_GUILD_GET_ENEMY_ACK:
		{
			const MSG_RM_GUILD_LIST* m = ( MSG_RM_GUILD_LIST* )message;

			mEnemyList.DeleteAllItems();

			CString text;

			for( DWORD i = 0; i < m->mSize; ++i )
			{
				const MSG_RM_GUILD_LIST::Guild& data = m->mGuild[ i ];

				text.Format( _T( "%d" ), data.mIndex );
				mEnemyList.InsertItem( i, text, 0 );

				mEnemyList.SetItemText( i, 1, CA2WEX< sizeof( data.mName ) >( data.mName ) );

				text.Format( _T( "%d" ), data.mSize );
				mEnemyList.SetItemText( i, 2, text );

				text.Format( _T( "%d" ), data.mLevel );
				mEnemyList.SetItemText( i, 3, text );
			}

			mEnemeyDataButton.EnableWindow( 0 < m->mSize );
			break;
		}
	case MP_RM_GUILD_SET_DATA_ACK:
		{
			CString textUpdateSucceed;
			textUpdateSucceed.LoadString( IDS_STRING28 );

			MessageBox( textUpdateSucceed, _T( "" ), MB_ICONINFORMATION | MB_OK );
			break;
		}
	case MP_RM_GUILD_SET_DATA_NACK:
		{
			CString textUpdateFailed;
			textUpdateFailed.LoadString( IDS_STRING29 );

			MessageBox( textUpdateFailed, _T( "" ), MB_ICONERROR | MB_OK );
			break;
		}
	case MP_RM_GUILD_SET_RANK_ACK:
		{
			const MSG_DWORD*	m			= ( MSG_DWORD* )message;
			const CString		rankName	= mApplication.GetRankName( m->dwData );
			const DWORD			playerIndex = m->dwObjectID;

			for( int row = 0; row < mMemberList.GetItemCount(); ++row )
			{
				if( _ttoi( mMemberList.GetItemText( row, 0 ) ) != playerIndex )
				{
					continue;
				}

				mMemberList.SetItemText( row, 1, rankName );

				mRankCombo.SetCurSel( mRankCombo.FindString( 0, rankName ) );

				CString textFormat;
				textFormat.LoadString( IDS_STRING36 );

				CString text;
				text.Format(
					textFormat,
					mMemberList.GetItemText( row, 2 ),
					rankName );

				MessageBox( text, _T( "" ), MB_ICONINFORMATION | MB_OK );

				break;
			}
			
			break;
		}
	case MP_RM_GUILD_SET_RANK_NACK:
		{
			mRankCombo.SetCurSel( 0 );

			CString textRankUpdateIsFailed;
			textRankUpdateIsFailed.LoadString( IDS_STRING37 );

			MessageBox( textRankUpdateIsFailed, _T( "" ), MB_ICONINFORMATION | MB_OK );
			break;
		}
	case MP_RM_GUILD_KICK_ACK:
		{
			const MSGBASE*	m			= ( MSGBASE* )message;
			const DWORD		playerindex = m->dwObjectID;

			for( int row = 0; row < mMemberList.GetItemCount(); ++row )
			{
				if( playerindex == _ttoi( mMemberList.GetItemText( row, 0 ) ) )
				{
					CString textFormat;
					textFormat.LoadString( IDS_STRING38 );

					CString text;
					text.Format( textFormat,
						mMemberList.GetItemText( row, 2 ) );

					MessageBox( text, _T( "" ), MB_ICONINFORMATION | MB_OK );

					mMemberList.DeleteItem( row );
					break;
				}
			}

			break;
		}
	case MP_RM_GUILD_KICK_NACK:
		{
			CString textKickingIsFailedBySystemFault;
			textKickingIsFailedBySystemFault.LoadString( IDS_STRING39 );

			MessageBox( textKickingIsFailedBySystemFault, _T( "" ), MB_ICONERROR | MB_OK );
			break;
		}
	case MP_RM_GUILD_KICK_NACK_BY_NO_GUILD:
		{
			CString textKickingIsFailedByNoMember;
			textKickingIsFailedByNoMember.LoadString( IDS_STRING40 );

			MessageBox( textKickingIsFailedByNoMember, _T( "" ), MB_ICONERROR | MB_OK );			
			break;
		}
	case MP_RM_GUILD_KICK_NACK_BY_ONE_MEMBER:
		{
			const MSGBASE*	m			= ( MSGBASE* )message;
			const DWORD		playerIndex = m->dwObjectID;

			for( int row = 0; row < mMemberList.GetItemCount(); ++row )
			{
				if( playerIndex == _ttoi( mMemberList.GetItemText( row, 0 ) ) )
				{
					const CString rank = mMemberList.GetItemText( row, 2 );

					mRankCombo.SetCurSel( mRankCombo.FindString( 0, rank ) );
					break;
				}
			}

			CString textKickingIsFailedByMemberIsOne;
			textKickingIsFailedByMemberIsOne.LoadString( IDS_STRING41 );
			MessageBox( textKickingIsFailedByMemberIsOne, _T( "" ), MB_ICONERROR | MB_OK );
			break;
		}
	case MP_RM_GUILD_NACK_BY_AUTH:
		{
			CString textYouHaveNoAuthority;
			textYouHaveNoAuthority.LoadString( IDS_STRING18 );

			MessageBox( textYouHaveNoAuthority, _T( "" ), MB_OK | MB_ICONERROR );
			break;
		}
		// 080425 LUJ, ��ų ��� ǥ��
	case MP_RM_GUILD_GET_SKILL_ACK:
		{
			const MSG_SKILL_LIST* m = ( MSG_SKILL_LIST* )message;

			for( DWORD i = 0; i < m->mSize; ++i )
			{
				const SKILL_BASE&	data	= m->mData[ i ];
				const ActiveSkillScript*	script = mApplication.GetActiveSkillScript( data.wSkillIdx );

				if( ! script )
				{
					continue;
				}

				const int row = mSkillListCtrl.GetItemCount();

				CString text;
				text.Format( _T( "%d" ), data.wSkillIdx );
				mSkillListCtrl.InsertItem( row, text, 0 );

				mSkillListCtrl.SetItemText( row, 1, script->mName );

				text.Format( _T( "%d" ), data.Level );
				mSkillListCtrl.SetItemText( row, 2, text );
			}

			break;
		}
		// 080425 LUJ, ��ų ����
	case MP_RM_GUILD_REMOVE_SKILL_ACK:
		{
			const MSG_DWORD* m = ( MSG_DWORD* )message;

			for( int row = 0; row < mSkillListCtrl.GetItemCount(); ++row )
			{
				const CString skillIndex = mSkillListCtrl.GetItemText( row, 0 );

				if( _ttoi( skillIndex ) == m->dwData )
				{
					mSkillListCtrl.DeleteItem( row );
					break;
				}
			}

			CString textUpdateSucceed;
			textUpdateSucceed.LoadString( IDS_STRING28 );

			MessageBox( textUpdateSucceed, _T( "" ), MB_ICONINFORMATION | MB_OK );
			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}
}


BOOL CGuildDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	SetIcon( AfxGetApp()->LoadIcon( IDR_MAINFRAME ), FALSE );
	SetIcon( AfxGetApp()->LoadIcon( IDR_MAINFRAME ), TRUE );

	// ����Ʈ ��Ʈ�� �ʱ�ȭ
	{
		const DWORD addStyle = LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT;
		CRect		rect;

		// â�� ������ ����Ʈ
		{
			int step = -1;

			mStoreItemList.GetClientRect( rect );

			CString textDbIndex;
			textDbIndex.LoadString( IDS_STRING13 );
			mStoreItemList.InsertColumn( ++step, textDbIndex, LVCFMT_LEFT, 0 );

			CString textName;
			textName.LoadString( IDS_STRING20 );
			mStoreItemList.InsertColumn( ++step, textName, LVCFMT_LEFT, int( rect.Width() * 0.7f ) );

			CString textQuantity;
			textQuantity.LoadString( IDS_STRING42 );
			mStoreItemList.InsertColumn( ++step, textQuantity, LVCFMT_LEFT, int( rect.Width() * 0.3f ) );
			
			mStoreItemList.SetExtendedStyle( mStoreItemList.GetExtendedStyle() | addStyle );
		}

		// ��� ȸ�� ����Ʈ
		{
			int step = -1;

			mMemberList.GetClientRect( rect );

			CString textDbIndex;
			textDbIndex.LoadString( IDS_STRING13 );
			mMemberList.InsertColumn( ++step, textDbIndex, LVCFMT_LEFT, 0 );

			CString textRank;
			textRank.LoadString( IDS_STRING43 );
			mMemberList.InsertColumn( ++step, textRank,	LVCFMT_LEFT, int( rect.Width() * 0.28f ) );

			CString textName;
			textName.LoadString( IDS_STRING20 );
			mMemberList.InsertColumn( ++step, textName,	LVCFMT_LEFT, int( rect.Width() * 0.5f ) );

			CString textLevel;
			textLevel.LoadString( IDS_STRING5 );
			mMemberList.InsertColumn( ++step, textLevel,	LVCFMT_LEFT, int( rect.Width() * 0.2f ) );

			CString textJoinedDate;
			textJoinedDate.LoadString( IDS_STRING44 );
			mMemberList.InsertColumn( ++step, textJoinedDate, LVCFMT_LEFT, int( rect.Width() * 0.5f ) );
			
			mMemberList.SetExtendedStyle( mMemberList.GetExtendedStyle() | addStyle );
		}

		// ���� ��� ����Ʈ
		{
			int step = -1;

			mFriendlyList.GetClientRect( rect );

			CString textDbIndex;
			textDbIndex.LoadString( IDS_STRING13 );
			mFriendlyList.InsertColumn( ++step, textDbIndex, LVCFMT_LEFT, 0 );

			CString textName;
			textName.LoadString( IDS_STRING20 );
			mFriendlyList.InsertColumn( ++step, textName, LVCFMT_LEFT, int( rect.Width() * 0.7f ) );

			CString textMember;
			textMember.LoadString( IDS_STRING45 );
			mFriendlyList.InsertColumn( ++step, textMember, LVCFMT_LEFT, int( rect.Width() * 0.25f ) );

			mFriendlyList.SetExtendedStyle( mFriendlyList.GetExtendedStyle() | addStyle );
		}

		// �� ��� ����Ʈ
		{
			int step = -1;

			mEnemyList.GetClientRect( rect );

			CString textDbIndex;
			textDbIndex.LoadString( IDS_STRING13 );
			mEnemyList.InsertColumn( ++step, textDbIndex, LVCFMT_LEFT, 0 );

			CString textName;
			textName.LoadString( IDS_STRING20 );
			mEnemyList.InsertColumn( ++step, textName, LVCFMT_LEFT, int( rect.Width() * 0.7f ) );

			CString textMember;
			textMember.LoadString( IDS_STRING45 );
			mEnemyList.InsertColumn( ++step, textMember, LVCFMT_LEFT, int( rect.Width() * 0.25f ) );

			mEnemyList.SetExtendedStyle( mEnemyList.GetExtendedStyle() | addStyle );
		}

		// 080425 LUJ, ��ų ����Ʈ
		{
			int step = -1;

			mSkillListCtrl.GetClientRect( rect );

			CString textIndex;
			textIndex.LoadString( IDS_STRING22 );
			mSkillListCtrl.InsertColumn( ++step, textIndex, LVCFMT_LEFT, 0 );

			CString textName;
			textName.LoadString( IDS_STRING20 );
			mSkillListCtrl.InsertColumn( ++step, textName, LVCFMT_LEFT, int( rect.Width() * 0.7f ) );

			CString textLevel;
			textLevel.LoadString( IDS_STRING5 );
			mSkillListCtrl.InsertColumn( ++step, textLevel, LVCFMT_LEFT, int( rect.Width() * 0.3f ) );

			mSkillListCtrl.SetExtendedStyle( mSkillListCtrl.GetExtendedStyle() | addStyle );
		}
	}

	// ����Ʈ ��Ʈ�� �ʱ�ȭ
	{
		mKeywordEdit.SetLimitText( MAX_NAME_LENGTH );
	}

	// �޺� ��Ʈ�� �ʱ�ȭ
	{
		CRect rect;

		{			
			mResultCombo.GetWindowRect( rect );
			mResultCombo.SetWindowPos( 0, 0, 0, rect.Width(), 200, SWP_NOMOVE | SWP_NOZORDER );
		}
		
		{
			mServerCombo.GetWindowRect( rect );
			mServerCombo.SetWindowPos( 0, 0, 0, rect.Width(), 200, SWP_NOMOVE | SWP_NOZORDER );			
		}

		{
			mRankCombo.GetWindowRect( rect );
			mRankCombo.SetWindowPos( 0, 0, 0, rect.Width(), 200, SWP_NOMOVE | SWP_NOZORDER );
			mApplication.SetRankCombo( mRankCombo );
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


CString CGuildDialog::GetServerName() const
{
	CString name;

	mServerCombo.GetLBText( mServerCombo.GetCurSel(), name );

	return name;
}

//void CGuildDialog::OnEnChangeGuildUnionNameEdit2()
//{
//	// TODO:  RICHEDIT ��Ʈ���� ��� �� ��Ʈ����
//	// CDialog::����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ�
//	// CRichEditCtrl().SetEventMask()�� ȣ���ϵ��� OnInitDialog() �Լ��� ���������� ������
//	// �� �˸��� ������ �ʽ��ϴ�.
//
//	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
//}

void CGuildDialog::OnBnClickedGuildFindButton()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	// Ű����� ���� �˻�

	CString keyword;

	mKeywordEdit.GetWindowText( keyword );

    if( keyword.IsEmpty() )
	{
		CString textKeywordIsEmpty;
		textKeywordIsEmpty.LoadString( IDS_STRING32 );
		MessageBox( textKeywordIsEmpty, _T( "" ), MB_ICONERROR | MB_OK );
		return;
	}

	MSG_NAME_DWORD message;
	{
		ZeroMemory( &message, sizeof( message ) );

		message.Category	= MP_RM_GUILD;
		message.Protocol	= MP_RM_GUILD_GET_LIST_SYN;
		StringCopySafe( message.Name, CW2AEX< MAX_PATH >( keyword ), sizeof( message.Name ) );
		message.dwData		= mDeletedGuildCheck.GetCheck();
	}

	mApplication.ConvertWildCardForDb( message.Name );	
	mApplication.Send( GetServerName(), message, sizeof( message ) );
	
	//mApplication.SetWait( TRUE );
}


void CGuildDialog::OnBnClickedGuildSpecificDataButton()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	// ���õ� ����� �� ������ ���캽

	CString text;

	mResultCombo.GetLBText( mResultCombo.GetCurSel(), text );

	int i = 0;
	const CString guildName( text.Tokenize( _T( " " ), i ) );

	if( guildName.IsEmpty() )
	{
		CString textThereIsNoTarget;
		textThereIsNoTarget.LoadString( IDS_STRING31 );
		MessageBox( textThereIsNoTarget, _T( "" ), MB_ICONERROR | MB_OK );
		return;
	}

	MSG_NAME_DWORD message;
	{
		ZeroMemory( &message, sizeof( message ) );

		message.Category	= MP_RM_GUILD;
		message.Protocol	= MP_RM_GUILD_GET_DATA_SYN;
		message.dwObjectID	= mGuildIndex;

		StringCopySafe( message.Name, CW2AEX< MAX_PATH >( guildName ), sizeof( message.Name ) );
	}

	mApplication.Send( mServerIndex, message, sizeof( message ) );
}


void CGuildDialog::OnBnClickedGuildDataUpdateButton()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	// ������ ��� ������ ������
	// ��� �̸��̳� ���� �̸��� ��� �� ������ ���� ������ �ٲ��� �ʴ´�...

	CString score;
	CString	level;
	CString	money;
	CString	guildName;
	CString	allianceName;
	
	mScoreEdit.GetWindowText( score );
	mLevelEdit.GetWindowText( level );
	mMoneyEdit.GetWindowText( money );
	mNameEdit.GetWindowText( guildName );
	mAllianceEdit.GetWindowText( allianceName );

	MSG_RM_GUILD_DATA message( mDataMessage );
	{
		ZeroMemory( &message, sizeof( message ) );
		
		message.mGuildIndex	= mGuildIndex;
		message.Category	= MP_RM_GUILD;
		message.Protocol	= MP_RM_GUILD_SET_DATA_SYN;
		message.mLevel		= _ttoi( level );
		message.mScore		= _ttoi( score );
		message.mMoney		= _ttoi( money );

		StringCopySafe( message.mGuildName,		CW2AEX< MAX_PATH >( guildName ),	sizeof( message.mGuildName ) );
		StringCopySafe( message.mAllianceName,	CW2AEX< MAX_PATH >( allianceName ),	sizeof( message.mAllianceName ) );
	}

	if( ! memcmp( &message, &mDataMessage, sizeof( mDataMessage ) ) )
	{
		return;
	}
	
	CString textWarning;
	textWarning.LoadString( IDS_STRING46 );

	if( IDNO == MessageBox( textWarning, _T( "" ), MB_ICONWARNING | MB_YESNO ) )
	{
		return;
	}	
	
	mApplication.Send( mServerIndex, message, sizeof( message ) );
}


void CGuildDialog::OnBnClickedGuildMoneyUpdateButton()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	// ����� ���� ������

	OnBnClickedGuildDataUpdateButton();
}


void CGuildDialog::OnBnClickedGuildSelectedMemberButton()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	// ���õ� ����� ������ ���캽

	POSITION		position		= mMemberList.GetFirstSelectedItemPosition();
	const DWORD		selectedRow		= mMemberList.GetNextSelectedItem( position );
	const CString	playerIndex		= mMemberList.GetItemText( selectedRow, 0 );

	// �÷��̾� â�� ���� ����
	{
		const CString playerName = mMemberList.GetItemText( selectedRow, 2 );

		if( playerName.IsEmpty() )
		{
			CString textThereIsNoTarget;
			textThereIsNoTarget.LoadString( IDS_STRING31 );

			MessageBox( textThereIsNoTarget, _T( "" ), MB_ICONERROR | MB_OK );
			return;
		}

		CPlayerDialog* dialog = mApplication.GetPlayerDialog();

		if( dialog )
		{
			dialog->Request( mServerIndex, playerName, _ttoi( playerIndex ) );
		}
	}
}


void CGuildDialog::OnBnClickedGuildFriendlyDataButton()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	// ���õ� �Ʊ� ����� ������ ��

	POSITION position = mFriendlyList.GetFirstSelectedItemPosition();

	const CString guildIndex = mFriendlyList.GetItemText( mFriendlyList.GetNextSelectedItem( position ), 0 );

	if( guildIndex.IsEmpty() )
	{
		CString textThereIsNoTarget;
		textThereIsNoTarget.LoadString( IDS_STRING31 );

		MessageBox( textThereIsNoTarget, _T( "" ), MB_ICONERROR | MB_OK );
		return;
	}

	MSG_NAME_DWORD message;
	{
		ZeroMemory( &message, sizeof( message ) );

		message.Category	= MP_RM_GUILD;
		message.Protocol	= MP_RM_GUILD_GET_DATA_SYN;
		message.dwData		= _ttoi( guildIndex );
	}

	mApplication.Send( mServerIndex, message, sizeof( message ) );
}


void CGuildDialog::OnBnClickedGuildEnemyDataButton()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	// ���õ� ���� ����� ������ ��

	POSITION position = mEnemyList.GetFirstSelectedItemPosition();

	const CString guildIndex = mEnemyList.GetItemText( mEnemyList.GetNextSelectedItem( position ), 0 );

	if( guildIndex.IsEmpty() )
	{
		CString textThereIsNoTarget;
		textThereIsNoTarget.LoadString( IDS_STRING31 );

		MessageBox( textThereIsNoTarget, _T( "" ), MB_ICONERROR | MB_OK );
		return;
	}

	MSG_NAME_DWORD message;
	{
		ZeroMemory( &message, sizeof( message ) );

		message.Category	= MP_RM_GUILD;
		message.Protocol	= MP_RM_GUILD_GET_DATA_SYN;
		message.dwData		= _ttoi( guildIndex );
	}

	mApplication.Send( mServerIndex, message, sizeof( message ) );
}


void CGuildDialog::Request( const CString& serverName, const CString& playerName, DWORD playerIndex )
{
	
}


void CGuildDialog::OnBnClickedGuildLogButton()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	CString guildName;
	mNameEdit.GetWindowText( guildName );

	if( guildName.IsEmpty() )
	{
		CString textThereIsNoTarget;
		textThereIsNoTarget.LoadString( IDS_STRING31 );

		MessageBox( textThereIsNoTarget, _T( "" ), MB_ICONERROR | MB_OK );
		return;
	}

	CLogDialog* dialog = mApplication.GetLogDialog();

	if( dialog &&
		mGuildIndex )
	{
		CString textGuild;
		textGuild.LoadString( IDS_STRING164 );
		CString textLog;
		textLog.LoadString( IDS_STRING10 );
		CString guildIndex;
		guildIndex.Format( _T( "%d" ), mGuildIndex );
		const CString title( textGuild + _T( " " ) + textLog + _T( ": " ) + guildName + _T( "(" ) + guildIndex + _T( ")" ) );
		
		CGuildLogCommand::Configuration configuration;
		configuration.mGuildIndex = mGuildIndex;

		dialog->DoModal(
			mServerIndex,
			CGuildLogCommand( mApplication, title, configuration ) );
	}
}


void CGuildDialog::OnBnClickedGuildStoreLogButton()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	CString guildName;
	mNameEdit.GetWindowText( guildName );

	if( guildName.IsEmpty() )
	{
		CString textThereIsNoTarget;
		textThereIsNoTarget.LoadString( IDS_STRING31 );

		MessageBox( textThereIsNoTarget, _T( "" ), MB_ICONERROR | MB_OK );
		return;
	}

	CLogDialog* dialog = mApplication.GetLogDialog();

	if( dialog )
	{
		CString textGuild;
		textGuild.LoadString( IDS_STRING164 );

		CString textWarehouse;
		textWarehouse.LoadString( IDS_STRING282 );

		CString textLog;
		textLog.LoadString( IDS_STRING10 );

		CString guildIndex;
		guildIndex.Format( _T( "%d" ), mGuildIndex );

		const CString title( textGuild + _T( " " ) + textWarehouse + _T( " " ) + textLog + _T( ": " ) + guildName + _T( "(" ) + guildIndex + _T( ")" ) );

		dialog->DoModal(
			mServerIndex,
			CGuildWarehouseLogCommand( mApplication, title, mGuildIndex ) );
	}
}


void CGuildDialog::OnBnClickedGuildScoreLogButton()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	CString guildName;
	mNameEdit.GetWindowText( guildName );

	if( guildName.IsEmpty() )
	{
		CString textThereIsNoTarget;
		textThereIsNoTarget.LoadString( IDS_STRING31 );

		MessageBox( textThereIsNoTarget, _T( "" ), MB_ICONERROR | MB_OK );
		return;
	}

	CLogDialog* dialog = mApplication.GetLogDialog();

	if( dialog )
	{
		CString textGuild;
		textGuild.LoadString( IDS_STRING164 );

		CString textScoreChange;
		textScoreChange.LoadString( IDS_STRING260 );

		CString textLog;
		textLog.LoadString( IDS_STRING10 );

		CString guildIndex;
		guildIndex.Format( _T( "%d" ), mGuildIndex );

		const CString title( textGuild + _T( " " ) + textScoreChange + _T( " " ) + textLog + _T( ": " ) + guildName + _T( "(" ) + guildIndex + _T( ")" ) );

		dialog->DoModal(
			mServerIndex,
			CGuildScoreLogCommand( mApplication, title, mGuildIndex ) );
	}
}


void CGuildDialog::OnBnClickedGuildMemberModifyButton()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	// ���� ����

	// �� ���� ���� �� ���̰� 10���� �̿�����
	const DWORD rank = mRankCombo.GetCurSel() * abs( GUILD_STUDENT - GUILD_MEMBER );

	// �÷��̾� �ε��� ����
	CString	playerRank;
	CString	playerName;
	DWORD	playerIndex = 0;
	{
		POSITION		position	= mMemberList.GetFirstSelectedItemPosition();	
		const DWORD		row			= mMemberList.GetNextSelectedItem( position );
		const CString	index		= mMemberList.GetItemText( row, 0 );

		if( index.IsEmpty() )
		{
			CString textThereIsNoTarget;
			textThereIsNoTarget.LoadString( IDS_STRING31 );

			MessageBox( textThereIsNoTarget, _T( "" ), MB_ICONERROR | MB_OK );
			return;
		}
		else if( mApplication.GetRankName( rank ) == mMemberList.GetItemText( row, 1 ) )
		{
			return;
		}

		playerIndex	= _ttoi( index );
		playerName	= mMemberList.GetItemText( row, 2 );
		playerRank	= mMemberList.GetItemText( row, 1 );
	}

	// Ż�� ó��
	if( rank == GUILD_NOTMEMBER )
	{
		if( 1 == mMemberList.GetItemCount() )
		{
			CString textKickIsFailedByMemberIsOne;
			textKickIsFailedByMemberIsOne.LoadString( IDS_STRING41 );

			MessageBox( textKickIsFailedByMemberIsOne, _T( "" ), MB_ICONERROR | MB_OK );
			return;
		}

		CString textWarningAboutKicking;
		textWarningAboutKicking.LoadString( IDS_STRING48 );

		CString text;
		text.Format(
			textWarningAboutKicking,
			playerName );

		if( IDNO == MessageBox( text, _T( "" ), MB_ICONWARNING | MB_YESNO ) )
		{
			mRankCombo.SetCurSel( mRankCombo.FindString( 0, playerRank ) );
			return;
		}

		MSGBASE message;
		message.Category	= MP_RM_GUILD;
		message.Protocol	= MP_RM_GUILD_KICK_SYN;
		message.dwObjectID	= playerIndex;

		mApplication.Send( mServerIndex, message, sizeof( message ) );
		return;
	}
    
	const CString	masterRank		= mApplication.GetRankName( GUILD_MASTER );
	CString			masterName;
	DWORD			masterIndex		= 0;
	{
		for( int row = 0; mMemberList.GetItemCount() > row; ++row )
		{
			const CString rankName = mMemberList.GetItemText( row, 1 );

			if( masterRank == rankName )
			{
				masterIndex = _ttoi( mMemberList.GetItemText( row, 0 ) );
				masterName	= mMemberList.GetItemText( row, 2 );
			}
		}
	}

	const CString playerNewRank = mApplication.GetRankName( rank );

	if( masterIndex )
	{
		CString text;

		// ���� ������ ������ ���� �� ��� ǥ��
		if( masterIndex == playerIndex )
		{
			CString textWarningThatMasterChangeToLowerRank;
			textWarningThatMasterChangeToLowerRank.LoadString( IDS_STRING48 );

			text.Format(
				textWarningThatMasterChangeToLowerRank,
				playerName,
				playerNewRank,
				masterRank );

			if( IDNO == MessageBox( text, _T( "" ), MB_ICONWARNING | MB_YESNO ) )
			{
				mRankCombo.SetCurSel( mRankCombo.FindString( 0, playerRank ) );
				return;
			}
		}
		// �����Ͱ� �ƴ� ȸ���� �����ͷ� �� �� ���
		else if( GUILD_MASTER == rank )
		{
			CString textWarningThatTwoMasterIs;
			textWarningThatTwoMasterIs.LoadString( IDS_STRING49 );

			text.Format(
				textWarningThatTwoMasterIs,
				masterName,
				masterRank );

			MessageBox( text, _T( "" ), MB_ICONWARNING | MB_OK );
			mRankCombo.SetCurSel( mRankCombo.FindString( 0, playerRank ) );
			return;
		}
		// �Ϲ� ���
		else
		{
			CString textWarningAboutChangeRank;
			textWarningAboutChangeRank.LoadString( IDS_STRING50 );
			text.Format(
				textWarningAboutChangeRank,
				playerNewRank,
				playerName );

			if( IDNO == MessageBox( text, _T( "" ), MB_ICONWARNING | MB_YESNO ) )
			{
				mRankCombo.SetCurSel( mRankCombo.FindString( 0, playerRank ) );
				return;
			}
		}
	}
	else
	{
		CString textWarningAboutChangeRank;
		textWarningAboutChangeRank.LoadString( IDS_STRING50 );

		CString text;
		text.Format(
			textWarningAboutChangeRank,
			playerNewRank,
			playerName );

		if( IDNO == MessageBox( text, _T( "" ), MB_ICONWARNING | MB_YESNO ) )
		{
			mRankCombo.SetCurSel( mRankCombo.FindString( 0, playerRank ) );
			return;
		}
	}

	MSG_DWORD2 message;
	message.Category	= MP_RM_GUILD;
	message.Protocol	= MP_RM_GUILD_SET_RANK_SYN;
	// 081129 LUJ, �߸� ������ �μ��� ������
	message.dwObjectID	= mGuildIndex;
	message.dwData1		= playerIndex;
	message.dwData2		= rank;
	
	mApplication.Send( mServerIndex, message, sizeof( message ) );
}


void CGuildDialog::OnLvnItemchangedGuildMemberList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;

	CString rankname;
	{
		POSITION position = mMemberList.GetFirstSelectedItemPosition();

		rankname = mMemberList.GetItemText( mMemberList.GetNextSelectedItem( position ), 1 );

		if( rankname.IsEmpty() )
		{
			return;
		}
	}

	mRankCombo.SetCurSel( mRankCombo.FindString( 0, rankname ) );
}


void CGuildDialog::OnCbnSelchangeGuildRankCombo()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	//// ��� ����� �ִµ��� ������ �������� �ʾ����� �⺻������ �����ؾ��Ѵ�.
	//if( mMemberList.GetItemCount() )
	//{
	//	if( ! mRankCombo.GetCurSel() )
	//	{
	//		POSITION position = mMemberList.GetFirstSelectedItemPosition();

	//		const CString rankName = mMemberList.GetItemText( mMemberList.GetNextSelectedItem( position ), 1 );

	//		mRankCombo.SetCurSel( mRankCombo.FindString( 0, rankName ) );
	//	}
	//}
	// ȸ�� ����� ������ ������ �⺻��
	if( ! mMemberList.GetItemCount() )
	{
		mRankCombo.SetCurSel( 0 );
	}
}



void CGuildDialog::OnLvnColumnclickGuildMemberList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;

	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	LV_ITEM* pItem= &(pDispInfo)->item;
	
	if (pItem->mask & LVIF_TEXT) //valid text buffer?
	{
		
	}
}

// 080425 LUJ, ��ų ����
void CGuildDialog::OnBnClickedGuildSkillDeleteButton()
{
	if( ! mSkillListCtrl.GetItemCount() )
	{
		return;
	}	
	
	{
		CString textDoYouRemoveReally;
		textDoYouRemoveReally.LoadString( IDS_STRING108 );

		if( IDCANCEL == MessageBox( textDoYouRemoveReally, _T( "" ), MB_OKCANCEL | MB_ICONWARNING ) )
		{
			return;
		}
	}

	POSITION	position	= mSkillListCtrl.GetFirstSelectedItemPosition();
	const int	row			= mSkillListCtrl.GetNextSelectedItem( position );

	if( 0 > row )
	{
		return;
	}
    
	const CString	skillIndex = mSkillListCtrl.GetItemText( row, 0 );
	const CString	skillLevel = mSkillListCtrl.GetItemText( row, 2 );

	MSG_DWORD2 message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category	= MP_RM_GUILD;
	message.Protocol	= MP_RM_GUILD_REMOVE_SKILL_SYN;
	message.dwObjectID	= mGuildIndex;
	message.dwData1		= _ttoi( skillIndex );
	message.dwData2		= _ttoi( skillLevel );

	mApplication.Send( mServerIndex, message, sizeof( message ) );
}


// 080702 LUJ, ��Ʈ��ũ �ʱ�ȭ�� â ���� �Ŀ� �ǹǷ�, â�� ǥ�õ� �� ������ ��Ÿ���� �޺� �ڽ� ���� �ʱ�ȭ���־�� �Ѵ�
void CGuildDialog::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	mApplication.SetServerComboBox( mServerCombo );
}

void CGuildDialog::OnBnClickedGuildLogAllButton()
{
	CLogDialog* dialog = mApplication.GetLogDialog();

	if( dialog )
	{
		CString textGuild;
		textGuild.LoadString( IDS_STRING164 );
		CString textLog;
		textLog.LoadString( IDS_STRING10 );

		CGuildLogCommand::Configuration configuration = { 0 };
		dialog->DoModal(
			UINT_MAX,
			CGuildLogCommand( mApplication, textGuild + _T( " " ) + textLog, configuration ) );
	}
}
