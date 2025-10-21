// NewUserDialog.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "client.h"
#include "NewUserDialog.h"
#include "QuestDialog.h"
#include "SkillDialog.h"
#include "ItemDialog.h"
#include "PlayerDialog.h"
#include "UserDialog.h"


bool SortByStandIndex( const CNewUserDialog::Player& lhs, const CNewUserDialog::Player& rhs )
{
	if( lhs.mStandIndex == rhs.mStandIndex )
	{
		return lhs.mPlayerIndex < rhs.mPlayerIndex;
	}
	
	return lhs.mStandIndex < rhs.mStandIndex;
}


// CNewUserDialog ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CNewUserDialog, CDialog)
CNewUserDialog::CNewUserDialog( CclientApp& application, CWnd* pParent /*=NULL*/)
: CDialog(CNewUserDialog::IDD, pParent)
, mApplication( application )
{
	EnableAutomation();
}


CNewUserDialog::~CNewUserDialog()
{
}


void CNewUserDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control( pDX, IDC_NEWUSER_NAME_RADIO, mNameRadioButton );
	DDX_Control( pDX, IDC_NEWUSER_ID_RADIO, mIdRadioButton );
	DDX_Control( pDX, IDC_NEWUSER_SERVER_COMBO, mServerComboBox );
	DDX_Control( pDX, IDC_NEWUSER_RESULT_COMBO, mPlayerComboBox );

	DDX_Text( pDX, IDC_NEWUSER_KEYWORD_EDIT, mKeyword );
	DDX_Control(pDX, IDC_NEWUSER_RESULT_SIZE_STATIC2, mPlayerSizeStatic);
	DDX_Control(pDX, IDC_NEWUSER_CHARACTER_RADIO, mPlayerRadioButton);
	DDX_Control(pDX, IDC_NEWUSER_PLAYER_ID_RADIO, mIndexRadioButton);
	DDX_Control(pDX, IDC_NEWUSER_USER_COMBO, mUserComboBox);
	DDX_Control(pDX, IDC_NEWUSER_USER_SIZE_STATIC, mUserSizeStatic);
	DDX_Control(pDX, IDC_NEWUSER_KEYWORD_EDIT, mKeywordEdit);
	DDX_Control(pDX, IDC_NEWUSER_USER_INDEX_RADIO, mUserIndexRadioButton);
}


BEGIN_MESSAGE_MAP(CNewUserDialog, CDialog)
	ON_BN_CLICKED(IDC_NEWUSER_SEARCH_BUTTON, OnBnClickedNewuserSearchButton)
	ON_COMMAND(ID_MENU_NEWUSER_DIALOG, OnMenuNewuserDialog)
	ON_BN_CLICKED(IDC_NEWUSER_PLAYER_BUTTON, OnBnClickedNewuserPlayerButton)
	ON_BN_CLICKED(IDC_NEWUSER_ITEM_BUTTON, OnBnClickedNewuserItemButton)
	ON_BN_CLICKED(IDC_NEWUSER_SKILL_BUTTON, OnBnClickedNewuserSkillButton)
	ON_BN_CLICKED(IDC_NEWUSER_BUTTON_QUEST, OnBnClickedNewuserButtonQuest)
	//ON_EN_CHANGE(IDC_NEWUSER_KEYWORD_EDIT, OnEnChangeNewuserKeywordEdit)
	//	ON_WM_KEYDOWN()
	//ON_BN_CLICKED(IDC_NEWUSER_BUTTON_QUEST2, OnBnClickedNewuserButtonQuest2)
	ON_CBN_SELCHANGE(IDC_NEWUSER_USER_COMBO, OnCbnSelchangeNewuserUserCombo)
	ON_BN_CLICKED(IDC_NEWUSER_USER_DATA_BUTTON, OnBnClickedNewuserUserDataButton)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CNewUserDialog, CDialog)
END_DISPATCH_MAP()

// ����: IID_INewUserDialog�� ���� ������ �߰��Ͽ�
// VBA���� ���� �������� ���ε��� �����մϴ�. 
// �� IID�� .IDL ���Ͽ� �ִ� dispinterface�� GUID�� ��ġ�ؾ� �մϴ�.

// {14588389-2684-4AC5-90DA-170D99459F17}
static const IID IID_INewUserDialog =
{ 0x14588389, 0x2684, 0x4AC5, { 0x90, 0xDA, 0x17, 0xD, 0x99, 0x45, 0x9F, 0x17 } };

BEGIN_INTERFACE_MAP(CNewUserDialog, CDialog)
	INTERFACE_PART(CNewUserDialog, IID_INewUserDialog, Dispatch)
END_INTERFACE_MAP()


// CNewUserDialog �޽��� ó�����Դϴ�.
void CNewUserDialog::Parse( DWORD serverIndex, const MSGROOT* message )
{
	mServerIndex = serverIndex;

	switch( message->Protocol )
	{
	case MP_RM_SEARCH_ACK:
		{
			const MSG_RM_OPEN_ACK* m = ( MSG_RM_OPEN_ACK* )message;

			if( ! m->mSize )
			{
				break;
			}

			const DWORD userIndex( m->mData[ 0 ].mUserIndex );
			int userComboIndex = -1;

			for(int i = 0; i < mUserComboBox.GetCount(); ++i)
			{
				CString text;
				mUserComboBox.GetLBText(
					i,
					text);

				if(userIndex == GetUserIndex(text))
				{
					userComboIndex = i;
					break;
				}
			}

			PlayerList& playerList = mUserMap[userComboIndex];

			// ĳ���ʹ� �����Ǿ����� ����, ������ ���� �ִ�. A�� �����ߴ� ������ ����
			// B�� ���� �����ߴ� ������ ���� �ִ�. ĳ���͸��� �������� ���� ������
			// ���� ������ ��� �������� �ε����ϰ� �ߺ��� �˻� ����� �������� ���۵ȴ�
			// �̸� �ɷ������Ѵ�. �̸� ���� �÷��̾� �ε����� ��� ���� ����� 
			// �ߺ� ���θ� �˻��Ѵ�
			stdext::hash_set< DWORD > playerIndexSet;
			{
				for(
					PlayerList::const_iterator it = playerList.begin();
					playerList.end() != it;
					++it )
				{
					const Player& player = *it;

					playerIndexSet.insert( player.mPlayerIndex );
				}
			}

			for( DWORD i = 0; i < m->mSize; ++i )
			{
				const MSG_RM_OPEN_ACK::Data& data = m->mData[ i ];

				Player player = { 0 };
				{
					player.mPlayerIndex	= data.mPlayerIndex;
					player.mServerIndex	= serverIndex;
					player.mUserIndex	= data.mUserIndex;
					player.mStandIndex	= data.mStandIndex;
					player.mName		= data.mPlayerName;
				}

				if( playerIndexSet.end() == playerIndexSet.find( player.mPlayerIndex ) )
				{
					playerList.push_back( player );
				}
			}

			playerList.sort( SortByStandIndex );

			OnCbnSelchangeNewuserUserCombo();
			break;
		}
	case MP_RM_SEARCH_USER_NAME_ACK:
		{
			const MSG_NAME2* m = ( MSG_NAME2* )message;

			mUserList.push_back( User() );
			User& user = mUserList.back();
			user.mName = GetUserName(
				CString(m->str1),
				CString(m->str2));
			user.mName.TrimRight();
			user.mUserIndex	= m->dwObjectID;

			if( -1 < mUserComboBox.FindString( 0, user.mName ) )
			{
				break;
			}

			mUserComboBox.AddString( user.mName );
			mUserComboBox.SetCurSel( 0 );

			{
				CString text;
				text.Format( _T( "%d" ), mUserComboBox.GetCount() );

				mUserSizeStatic.SetWindowText( text );
			}

			break;
		}
	case MP_RM_SEARCH_NACK:
		{
			CString textThereIsNoResult;
			textThereIsNoResult.LoadString( IDS_STRING1 );

			MessageBox( textThereIsNoResult, _T( "" ), MB_ICONERROR | MB_OK );
			break;
		}
	case MP_RM_SEARCH_NACK_BY_AUTH:
		{
			CString textYouHaveNoAuthority;
			textYouHaveNoAuthority.LoadString( IDS_STRING18 );

			MessageBox( textYouHaveNoAuthority, _T( "" ), MB_OK | MB_ICONERROR );
			break;
		}
	default:
		{
			ASSERT( 0 && "It's not defined protocol" );
			break;
		}
	}
}


BOOL CNewUserDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon( AfxGetApp()->LoadIcon( IDR_MAINFRAME ), FALSE );
	SetIcon( AfxGetApp()->LoadIcon( IDR_MAINFRAME ), TRUE );

	{
		mPlayerSizeStatic.SetWindowText( _T( "" ) );
		mUserSizeStatic.SetWindowText( _T( "" ) );
	}

	// ���� ��ư
	{
		mNameRadioButton.SetCheck( BST_UNCHECKED );
		mIdRadioButton.SetCheck( BST_CHECKED );
		mIndexRadioButton.SetCheck( BST_UNCHECKED );
		mPlayerRadioButton.SetCheck( BST_UNCHECKED );
	}

	{
		mKeywordEdit.SetLimitText( MAX_NAME_LENGTH );
	}

	{
		CRect rect;

		{
			mServerComboBox.GetWindowRect( rect );
			mServerComboBox.SetWindowPos( 0, 0, 0, rect.Width(), 200, SWP_NOMOVE | SWP_NOZORDER );
		}

		{			
			mPlayerComboBox.GetWindowRect( rect );
			mPlayerComboBox.SetWindowPos( 0, 0, 0, rect.Width(), 200, SWP_NOMOVE | SWP_NOZORDER );
		}

		{
			mUserComboBox.GetWindowRect( rect );
			mUserComboBox.SetWindowPos( 0, 0, 0, rect.Width(), 200, SWP_NOMOVE | SWP_NOZORDER );
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


void CNewUserDialog::OnBnClickedNewuserSearchButton()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	if( mApplication.GetWaitDialog()->GetSafeHwnd() )
	{
		return;
	}

	CDataExchange dx( this, TRUE );
	DoDataExchange( &dx );

	if( mKeyword.IsEmpty() )
	{
		CString textKeywordIsEmpty;
		textKeywordIsEmpty.LoadString( IDS_STRING32 );

		MessageBox( textKeywordIsEmpty, _T( "" ), MB_OK );
		return;
	}
	else if( MAX_NAME_LENGTH < mKeyword.GetLength() )
	{
		ASSERT( 0 );
	}	

	// �������� �����͸� �ܼ������� ������ �߰��ϹǷ�, ����� �������� �ʱ�ȭ�ؾ��Ѵ�
	{
		mPlayerSizeStatic.SetWindowText( _T( "0" ) );
		mPlayerComboBox.ResetContent();		
		
		mUserSizeStatic.SetWindowText( _T( "0" ) );
		mUserComboBox.ResetContent();
	}

	{
		MSG_RM_OPEN_SYN message;
		ZeroMemory( &message, sizeof( message ) );
		message.Category	= MP_RM_SEARCH;
		message.Protocol	= MP_RM_SEARCH_SYN;

		if( BST_CHECKED == mIdRadioButton.GetCheck() )
		{
			message.mType	= MSG_RM_OPEN_SYN::LoginId;
		}
		else if( BST_CHECKED == mNameRadioButton.GetCheck() )
		{
			message.mType	= MSG_RM_OPEN_SYN::RealName;
		}
		else if( BST_CHECKED == mPlayerRadioButton.GetCheck() )
		{
			message.mType	= MSG_RM_OPEN_SYN::PlayerName;
		}
		else if( BST_CHECKED == mIndexRadioButton.GetCheck() )
		{
			message.mType	= MSG_RM_OPEN_SYN::PlayerIndex;

			if( ! _ttoi( mKeyword ) )
			{
				ASSERT( 0 && "Input number more than 0" );
				return;
			}
		}
		// 090122 LUJ, ����� ��ȣ�� �˻� ��� �߰�
		else if( BST_CHECKED == mUserIndexRadioButton.GetCheck() )
		{
			message.mType	= MSG_RM_OPEN_SYN::UserIndex;

			if( ! _ttoi( mKeyword ) )
			{
				ASSERT( 0 && "Input number more than 0" );
				return;
			}
		}
		else
		{
			ASSERT( 0 && "You should select one radio check at least" );
			return;
		}

		// �ʱ�ȭ
		{
			mUserMap.clear();
			mUserList.clear();
		}

		strncpy( message.mKeyword, CW2AEX< MAX_PATH >( mKeyword ), sizeof( message.mKeyword ) );
		
		CclientApp::ConvertWildCardForDb( message.mKeyword );

		CString serverName;
		mServerComboBox.GetLBText( mServerComboBox.GetCurSel(), serverName );

		mApplication.Send( serverName, message, sizeof( message ) );
	}
}


void CNewUserDialog::OnMenuNewuserDialog()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.

	ShowWindow( SW_SHOW );
}

void CNewUserDialog::OnBnClickedNewuserPlayerButton()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	const Player& player = GetPlayer();

	if( ! player.mPlayerIndex )
	{
		CString textThereIsNoTarget;
		textThereIsNoTarget.LoadString( IDS_STRING31 );

		MessageBox( textThereIsNoTarget, _T( "" ), MB_OK );
		return;
	}

	{
		CPlayerDialog* dialog = mApplication.GetPlayerDialog();
		ASSERT( dialog );

		dialog->Request( GetServerIndex(), player.mName, player.mPlayerIndex );
	}
}

void CNewUserDialog::OnBnClickedNewuserItemButton()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	const Player& player = GetPlayer();

	if( ! player.mPlayerIndex )
	{
		CString textThereIsNoTarget;
		textThereIsNoTarget.LoadString( IDS_STRING31 );

		MessageBox( textThereIsNoTarget, _T( "" ), MB_OK );
		return;
	}

	{
		CItemDialog* dialog = mApplication.GetItemDialog();
		ASSERT( dialog );

		dialog->Request( GetServerIndex(), player.mName, player.mPlayerIndex, player.mUserIndex );
	}
}


const CNewUserDialog::Player& CNewUserDialog::GetPlayer() const
{
	static const Player emptyPlayer;

	UserMap::const_iterator user_it = mUserMap.find( mUserComboBox.GetCurSel() );

	if( mUserMap.end() == user_it )
	{
		return emptyPlayer;
	}

	const PlayerList& playerList = user_it->second;

	if( playerList.size() <= size_t( mPlayerComboBox.GetCurSel() ) )
	{
		return emptyPlayer;
	}

	PlayerList::const_iterator it = playerList.begin();

	std::advance( it, mPlayerComboBox.GetCurSel() );

	return *it;
}


//CString CNewUserDialog::GetPlayerName() const
//{
//	int i = 0;
//
//	if( mPlayerComboBox.GetCount() )
//	{
//		CString name;
//
//		mPlayerComboBox.GetLBText( mPlayerComboBox.GetCurSel(), name );
//
//		return name.Tokenize( "() \t ", i );
//	}
//
//	return "";
//}


DWORD CNewUserDialog::GetServerIndex() const
{
	CString serverName;

	mServerComboBox.GetLBText( mServerComboBox.GetCurSel(), serverName );

	return mApplication.GetServerIndex( serverName );
}


//DWORD CNewUserDialog::GetPlayerIndex() const
//{
//	// ����� �޺��� ������ ������ �̰� �������� ��ȸ�Ѵ�(������ �÷��̾� �̸��� ���� �� �����Ƿ�)
//
//	UserMap::const_iterator user_it = mUserMap.find( mUserComboBox.GetCurSel() );
//
//	if( mUserMap.end() == user_it )
//	{
//		return 0;
//	}
//
//	const PlayerList& playerList = user_it->second;
//
//	if( playerList.size() <= size_t( mPlayerComboBox.GetCurSel() ) )
//	{
//		return 0;
//	}
//
//	PlayerList::const_iterator it = playerList.begin();
//	
//	std::advance( it, mPlayerComboBox.GetCurSel() );
//
//	const Player& player = *it;
//
//	return player.mPlayerIndex;
//}


void CNewUserDialog::OnBnClickedNewuserSkillButton()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	const Player& player = GetPlayer();

	if( ! player.mPlayerIndex )
	{
		CString textThereIsNoTarget;
		textThereIsNoTarget.LoadString( IDS_STRING31 );

		MessageBox( textThereIsNoTarget, _T( "" ), MB_OK );
		return;
	}

	{
		CSkillDialog* dialog = mApplication.GetSkillDialog();
		ASSERT( dialog );

		dialog->Request( GetServerIndex(), player.mName, player.mPlayerIndex );
	}
}


void CNewUserDialog::OnBnClickedNewuserButtonQuest()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	const Player& player = GetPlayer();

	if( ! player.mPlayerIndex )
	{
		CString textThereIsNoTarget;
		textThereIsNoTarget.LoadString( IDS_STRING31 );

		MessageBox( textThereIsNoTarget, _T( "" ), MB_OK );
		return;
	}

	CQuestDialog* dialog = mApplication.GetQuestDialog();
	ASSERT( dialog );

	dialog->Request( GetServerIndex(), player.mName, player.mPlayerIndex );
}


// 080328 LUJ, �˻��� �÷��̾� ��ȣ�� �ش��ϴ� �˻� ����� �ڵ����� �����Ѵ�
void CNewUserDialog::OnCbnSelchangeNewuserUserCombo()
{
	UserMap::const_iterator user_it = mUserMap.find( mUserComboBox.GetCurSel() );

	if( mUserMap.end() == user_it )
	{
		return;
	}

	const PlayerList& playerList = user_it->second;

	{
		CString	text;
		text.Format( _T( "%d" ), playerList.size() );

		mPlayerSizeStatic.SetWindowText( text );
	}

	mPlayerComboBox.ResetContent();

	for(
		PlayerList::const_iterator it = playerList.begin();
		playerList.end() != it;
		++it )
	{
		const Player& player = *it;

		mPlayerComboBox.AddString( player.mName + ( 5 > player.mStandIndex ? _T( "" ) : _T( " x" ) ) );
	}

	// �ڵ����� �˻��׸��� ���� ���·� ���´�
	{
		CString keyword;

		if( mPlayerRadioButton.GetCheck() )
		{
			keyword = mKeyword.GetString();
		}
		// 080328 LUJ, �˻��� �÷��̾� ��ȣ�� �ڵ� �����Ѵ�
		else if( mIndexRadioButton.GetCheck() )
		{
			const DWORD playerIndex = _ttoi( mKeyword.GetString() );

			for(
				PlayerList::const_iterator it = playerList.begin();
				playerList.end() != it;
				++it )
			{
				const Player& player = *it;

				if( player.mPlayerIndex == playerIndex )
				{
					keyword = player.mName;
					break;
				}
			}
		}
		
		mPlayerComboBox.SetCurSel( 0 );

		// 080328 LUJ, Ȱ�� ���� �÷��̾� ������� ����
		{
			const int index = mPlayerComboBox.FindString( -1, keyword );

			if( -1 < index )
			{
				mPlayerComboBox.SetCurSel( index );
			}
		}

		// 080328 LUJ, ���� ���� �÷��̾� ������� ����
		{				
			const int index = mPlayerComboBox.FindString( -1, keyword + _T( " x" ) );

			if( -1 < index )
			{
				mPlayerComboBox.SetCurSel( index );
			}
		}
	}
}


void CNewUserDialog::OnBnClickedNewuserUserDataButton()
{
	if( ! mUserComboBox.GetCount() )
	{
		return;
	}

	CString userName;

	mUserComboBox.GetLBText( mUserComboBox.GetCurSel(), userName );

	if( userName.IsEmpty() )
	{
		return;
	}
	else if(const DWORD userIndex = GetUserIndex(userName))
	{
		CUserDialog* dialog = mApplication.GetUserDialog();
		ASSERT( dialog );

		dialog->Request( GetServerIndex(), userName, userIndex );
	}
}


void CNewUserDialog::FindPlayer( DWORD serverIndex, DWORD playerIndex )
{
	mPlayerComboBox.ResetContent();

	{
		MSG_RM_OPEN_SYN message;
		ZeroMemory( &message, sizeof( message ) );

		message.Category	= MP_RM_SEARCH;
		message.Protocol	= MP_RM_SEARCH_SYN;
		message.mType		= MSG_RM_OPEN_SYN::PlayerIndex;

		StringCopySafe( message.mKeyword, CW2AEX< MAX_PATH >( mKeyword ), sizeof( message.mKeyword ) );
		
		mApplication.Send( mServerIndex, message, sizeof( message ) );
	}	
}


// 080702 LUJ, ��Ʈ��ũ �ʱ�ȭ�� â ���� �Ŀ� �ǹǷ�, â�� ǥ�õ� �� ������ ��Ÿ���� �޺� �ڽ� ���� �ʱ�ȭ���־�� �Ѵ�
void CNewUserDialog::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	mApplication.SetServerComboBox( mServerComboBox );
}

CString CNewUserDialog::GetUserName(LPCTSTR loginName, LPCTSTR enpangName) const
{
	if(0 == _tcslen(loginName))
	{
		return enpangName + CString(_T(" (����)"));
	}
	else if(0 == _tcscmp(_T(" "), enpangName))
	{
		return CString(loginName);
	}
	
	return CString(loginName) + CString(_T(" > ")) + CString(enpangName) + CString(" (����)");
}

DWORD CNewUserDialog::GetUserIndex(LPCTSTR name) const
{
	for( UserList::const_iterator it = mUserList.begin(); mUserList.end() != it; ++it )
	{
		const User& user = *it;

		if(user.mName == name)
		{
			return user.mUserIndex;
		}
	}

	return 0;
}