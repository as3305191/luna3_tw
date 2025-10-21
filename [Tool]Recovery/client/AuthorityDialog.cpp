// 081021 LUJ, ���� ���� â �߰�
// AuthorityDialog.cpp : ���� �����Դϴ�.

#include "stdafx.h"
#include "client.h"
#include "AuthorityDialog.h"
#include ".\authoritydialog.h"

// CAuthorityDialog ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CAuthorityDialog, CDialog)
CAuthorityDialog::CAuthorityDialog( CclientApp& application, CWnd* pParent /*=NULL*/) :
CDialog(CAuthorityDialog::IDD, pParent),
mApplication( application ),
mServerIndex( 0 )
{
	EnableAutomation();
}

CAuthorityDialog::~CAuthorityDialog()
{}

void CAuthorityDialog::OnFinalRelease()
{
	// �ڵ�ȭ ��ü�� ���� ������ ������ �����Ǹ� OnFinalRelease��
	// ȣ��˴ϴ�. �⺻ Ŭ�������� �ڵ����� ��ü�� �����մϴ�.
	// �⺻ Ŭ������ ȣ���ϱ� ���� ��ü�� �ʿ��� �߰� ���� �۾���
	// �߰��Ͻʽÿ�.

	CDialog::OnFinalRelease();
}

void CAuthorityDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_AUTHORITY_TAB, mLevelTabCtrl);
	DDX_Control(pDX, IDC_AUTHORITY_READ_01_CHECK, mPlayerLevelReadButton);
	DDX_Control(pDX, IDC_AUTHORITY_WRITE_01_CHECK, mPlayerLevelWriteButton);
	DDX_Control(pDX, IDC_AUTHORITY_READ_02_CHECK, mPlayerCreateReadButton);
	DDX_Control(pDX, IDC_AUTHORITY_WRITE_02_CHECK, mPlayerCreateWriteButton);
	DDX_Control(pDX, IDC_AUTHORITY_READ_03_CHECK, mPlayerDeleteReadButton);
	DDX_Control(pDX, IDC_AUTHORITY_WRITE_03_CHECK, mPlayerDeleteWriteButton);
	DDX_Control(pDX, IDC_AUTHORITY_READ_04_CHECK, mPlayerRecoverReadButton);
	DDX_Control(pDX, IDC_AUTHORITY_WRITE_04_CHECK, mPlayerRecoverWriteButton);
	DDX_Control(pDX, IDC_AUTHORITY_READ_05_CHECK, mPlayerUpdateReadButton);
	DDX_Control(pDX, IDC_AUTHORITY_WRITE_05_CHECK, mPlayerUpdateWriteButton);
	DDX_Control(pDX, IDC_AUTHORITY_READ_06_CHECK, mSkillReadButton);
	DDX_Control(pDX, IDC_AUTHORITY_WRITE_06_CHECK, mSkillWriteButton);
	DDX_Control(pDX, IDC_AUTHORITY_READ_07_CHECK, mQuestReadButton);
	DDX_Control(pDX, IDC_AUTHORITY_WRITE_07_CHECK, mQuestWriteButton);
	DDX_Control(pDX, IDC_AUTHORITY_READ_08_CHECK, mGuildReadButton);
	DDX_Control(pDX, IDC_AUTHORITY_WRITE_08_CHECK, mGuildWriteButton);
	DDX_Control(pDX, IDC_AUTHORITY_READ_09_CHECK, mFamilyReadButton);
	DDX_Control(pDX, IDC_AUTHORITY_WRITE_09_CHECK, mFamilyWriteButton);
	DDX_Control(pDX, IDC_AUTHORITY_READ_10_CHECK, mPlayerLogReadButton);
	DDX_Control(pDX, IDC_AUTHORITY_WRITE_10_CHECK, mPlayerLogWriteButton);
	DDX_Control(pDX, IDC_AUTHORITY_READ_11_CHECK, mOperatorReadButton);
	DDX_Control(pDX, IDC_AUTHORITY_WRITE_11_CHECK, mOperatorWriteButton);
	DDX_Control(pDX, IDC_AUTHORITY_READ_12_CHECK, mAllowIpReadButton);
	DDX_Control(pDX, IDC_AUTHORITY_WRITE_12_CHECK, mAllowIpWriteButton);
	DDX_Control(pDX, IDC_AUTHORITY_READ_13_CHECK, mOperatorLogReadButton);
	DDX_Control(pDX, IDC_AUTHORITY_WRITE_13_CHECK, mOperatorLogWriteButton);	
	DDX_Control(pDX, IDC_AUTHORITY_READ_14_CHECK, mItemReadButton);
	DDX_Control(pDX, IDC_AUTHORITY_WRITE_14_CHECK, mItemWriteButton);
}

BEGIN_MESSAGE_MAP(CAuthorityDialog, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_AUTHORITY_TAB, OnTcnSelchangeAuthorityTab)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CAuthorityDialog, CDialog)
END_DISPATCH_MAP()

// ����: IID_IAuthorityDialog�� ���� ������ �߰��Ͽ�
// VBA���� ���� �������� ���ε��� �����մϴ�. 
// �� IID�� .IDL ���Ͽ� �ִ� dispinterface�� GUID�� ��ġ�ؾ� �մϴ�.

// {0633EBEA-6AD6-478A-8350-F715ECD30D50}
static const IID IID_IAuthorityDialog =
{ 0x633EBEA, 0x6AD6, 0x478A, { 0x83, 0x50, 0xF7, 0x15, 0xEC, 0xD3, 0xD, 0x50 } };

BEGIN_INTERFACE_MAP(CAuthorityDialog, CDialog)
	INTERFACE_PART(CAuthorityDialog, IID_IAuthorityDialog, Dispatch)
END_INTERFACE_MAP()

// CAuthorityDialog �޽��� ó�����Դϴ�.

void CAuthorityDialog::Parse( DWORD serverIndex, const MSGROOT* message )
{
	switch( message->Protocol )
	{
	case MP_RM_AUTHORITY_GET_ACK:
		{
			ShowWindow( SW_SHOW );

			const MSG_RM_AUTHORITY* m = ( MSG_RM_AUTHORITY* )message;

			for( DWORD i = 0; i < m->mSize; ++ i)
			{
				const MSG_RM_AUTHORITY::Authority& authority = m->mAuthority[ i ];				
				mAuthorityMap[ authority.mPower ].push_back( authority );
			}

			OnTcnSelchangeAuthorityTab( 0, 0 );
			break;
		}
	case MP_RM_AUTHORITY_SET_ACK:
		{
			CString textUpdateSucceed;
			textUpdateSucceed.LoadString( IDS_STRING28 );

			MessageBox( textUpdateSucceed, _T( "" ), MB_ICONINFORMATION | MB_OK );
			break;
		}
	case MP_RM_AUTHORITY_NACK_BY_AUTH:
		{
			OnCancel();

			CString textYouHaveNoAuthority;
			textYouHaveNoAuthority.LoadString( IDS_STRING18 );

			MessageBox( textYouHaveNoAuthority, _T( "" ), MB_OK | MB_ICONERROR );
			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}
}

BOOL CAuthorityDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 081021 LUJ, �� �ʱ�ȭ
	for(
		int value = eGM_POWER_MASTER;
		eGM_POWER_MAX > value;
		++value )
	{
		const eGM_POWER power = eGM_POWER( value );

		if( eGM_POWER_MASTER == power )
		{
			continue;
		}

		mLevelTabCtrl.InsertItem(
			mLevelTabCtrl.GetItemCount(),
			mApplication.GetText( power ) );
	}

	mAuthorityMap.clear();

	// 081021 LUJ, DB���� ����� ���������� â�� ǥ������ �ʴ´�
	ShowWindow( SW_HIDE );

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

INT_PTR CAuthorityDialog::DoModal( DWORD serverIndex )
{
	mServerIndex = serverIndex;

	MSGBASE message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category	= MP_RM_AUTHORITY;
	message.Protocol	= MP_RM_AUTHORITY_GET_SYN;

	mApplication.Send(
		serverIndex,
		message,
		sizeof( message ),
		false );

	return CDialog::DoModal();
}

void CAuthorityDialog::OnOK()
{
	// 081021 LUJ, ���� ���� ����

	const eGM_POWER power = GetRankFromTab();

	if( eGM_POWER_MAX == power )
	{
		ASSERT( 0 );
		return;
	}

	// 081021 LUJ, ���� �ڽ��� ������ ������ ��´�
	MSG_RM_AUTHORITY message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category	= MP_RM_AUTHORITY;
	message.Protocol	= MP_RM_AUTHORITY_SET_SYN;

	{
		MSG_RM_AUTHORITY::Authority& authority = message.mAuthority[ message.mSize++ ];
		authority.mPower		= power;
		authority.mType			= AuthorityTypePlayerLevel;
		authority.mReadable		= ( 0 < mPlayerLevelReadButton.GetCheck() );
		authority.mWriteable	= ( 0 < mPlayerLevelWriteButton.GetCheck() );
	}

	{
		MSG_RM_AUTHORITY::Authority& authority = message.mAuthority[ message.mSize++ ];
		authority.mPower		= power;
		authority.mType			= AuthorityTypePlayerCreate;
		authority.mReadable		= ( 0 < mPlayerCreateReadButton.GetCheck() );
		authority.mWriteable	= ( 0 < mPlayerCreateWriteButton.GetCheck() );
	}

	{
		MSG_RM_AUTHORITY::Authority& authority = message.mAuthority[ message.mSize++ ];
		authority.mPower		= power;
		authority.mType			= AuthorityTypePlayerDelete;
		authority.mReadable		= ( 0 < mPlayerDeleteReadButton.GetCheck() );
		authority.mWriteable	= ( 0 < mPlayerDeleteWriteButton.GetCheck() );
	}

	{
		MSG_RM_AUTHORITY::Authority& authority = message.mAuthority[ message.mSize++ ];
		authority.mPower		= power;
		authority.mType			= AuthorityTypePlayerRecover;
		authority.mReadable		= ( 0 < mPlayerRecoverReadButton.GetCheck() );
		authority.mWriteable	= ( 0 < mPlayerRecoverWriteButton.GetCheck() );
	}

	{
		MSG_RM_AUTHORITY::Authority& authority = message.mAuthority[ message.mSize++ ];
		authority.mPower		= power;
		authority.mType			= AuthorityTypePlayerUpdate;
		authority.mReadable		= ( 0 < mPlayerUpdateReadButton.GetCheck() );
		authority.mWriteable	= ( 0 < mPlayerUpdateWriteButton.GetCheck() );
	}

	{
		MSG_RM_AUTHORITY::Authority& authority = message.mAuthority[ message.mSize++ ];
		authority.mPower		= power;
		authority.mType			= AuthorityTypePlayerLog;
		authority.mReadable		= ( 0 < mPlayerLogReadButton.GetCheck() );
		authority.mWriteable	= ( 0 < mPlayerLogWriteButton.GetCheck() );
	}
	
	{
		MSG_RM_AUTHORITY::Authority& authority = message.mAuthority[ message.mSize++ ];
		authority.mPower		= power;
		authority.mType			= AuthorityTypeSkill;
		authority.mReadable		= ( 0 < mSkillReadButton.GetCheck() );
		authority.mWriteable	= ( 0 < mSkillWriteButton.GetCheck() );
	}

	{
		MSG_RM_AUTHORITY::Authority& authority = message.mAuthority[ message.mSize++ ];
		authority.mPower		= power;
		authority.mType			= AuthorityTypeQuest;
		authority.mReadable		= ( 0 < mQuestReadButton.GetCheck() );
		authority.mWriteable	= ( 0 < mQuestWriteButton.GetCheck() );
	}

	{
		MSG_RM_AUTHORITY::Authority& authority = message.mAuthority[ message.mSize++ ];
		authority.mPower		= power;
		authority.mType			= AuthorityTypeGuild;
		authority.mReadable		= ( 0 < mGuildReadButton.GetCheck() );
		authority.mWriteable	= ( 0 < mGuildWriteButton.GetCheck() );
	}

	{
		MSG_RM_AUTHORITY::Authority& authority = message.mAuthority[ message.mSize++ ];
		authority.mPower		= power;
		authority.mType			= AuthorityTypeFamily;
		authority.mReadable		= ( 0 < mFamilyReadButton.GetCheck() );
		authority.mWriteable	= ( 0 < mFamilyWriteButton.GetCheck() );
	}

	{
		MSG_RM_AUTHORITY::Authority& authority = message.mAuthority[ message.mSize++ ];
		authority.mPower		= power;
		authority.mType			= AuthorityTypeOperator;
		authority.mReadable		= ( 0 < mOperatorReadButton.GetCheck() );
		authority.mWriteable	= ( 0 < mOperatorWriteButton.GetCheck() );
	}

	{
		MSG_RM_AUTHORITY::Authority& authority = message.mAuthority[ message.mSize++ ];
		authority.mPower		= power;
		authority.mType			= AuthorityTypeAllowIp;
		authority.mReadable		= ( 0 < mAllowIpReadButton.GetCheck() );
		authority.mWriteable	= ( 0 < mAllowIpWriteButton.GetCheck() );
	}

	{
		MSG_RM_AUTHORITY::Authority& authority = message.mAuthority[ message.mSize++ ];
		authority.mPower		= power;
		authority.mType			= AuthorityTypeOperatorLog;
		authority.mReadable		= ( 0 < mOperatorLogReadButton.GetCheck() );
		authority.mWriteable	= ( 0 < mOperatorLogWriteButton.GetCheck() );
	}

	// 090219 LUJ, ���� ���� �׸� [������] �߰�
	{
		MSG_RM_AUTHORITY::Authority& authority = message.mAuthority[ message.mSize++ ];
		authority.mPower		= power;
		authority.mType			= AuthorityTypeItem;
		authority.mReadable		= ( 0 < mItemReadButton.GetCheck() );
		authority.mWriteable	= ( 0 < mItemWriteButton.GetCheck() );
	}

	// 081021 LUJ, ������ ��û�� ������ �޸𸮿� �����س��´�
	{
		AuthorityList& authorityList = mAuthorityMap[ power ];
		authorityList.clear();

		for( DWORD i = 0; i < message.mSize; ++i )
		{
			const MSG_RM_AUTHORITY::Authority& authority = message.mAuthority[ i ];
			authorityList.push_back( authority );
		}
	}	

	// 081021 LUJ, ���ǻ���: ��� �������� DB ó���� ������ �ʵ���,
	//			DB�� ó���� ������ �����ؾ��Ѵ�. �ϴ� ���߿� ����
	mApplication.Send(
		mServerIndex,
		message,
		message.GetSize() );
}

void CAuthorityDialog::OnTcnSelchangeAuthorityTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	if( pResult )
	{
		*pResult = 0;
	}

	// 081021 LUJ, �ʱ�ȭ
	{
		mPlayerLevelReadButton.SetCheck( false );
		mPlayerLevelWriteButton.SetCheck( false );

		mPlayerCreateReadButton.SetCheck( false );
		mPlayerCreateWriteButton.SetCheck( false );

		mPlayerDeleteReadButton.SetCheck( false );
		mPlayerDeleteWriteButton.SetCheck( false );

		mPlayerRecoverReadButton.SetCheck( false );
		mPlayerRecoverWriteButton.SetCheck( false );

		mPlayerRecoverReadButton.SetCheck( false );
		mPlayerRecoverWriteButton.SetCheck( false );

		mPlayerUpdateReadButton.SetCheck( false );
		mPlayerUpdateWriteButton.SetCheck( false );

		mPlayerLogReadButton.SetCheck( false );
		mPlayerLogWriteButton.SetCheck( false );

		mSkillReadButton.SetCheck( false );
		mSkillWriteButton.SetCheck( false );

		mQuestReadButton.SetCheck( false );
		mQuestWriteButton.SetCheck( false );

		mGuildReadButton.SetCheck( false );
		mGuildWriteButton.SetCheck( false );

		mFamilyReadButton.SetCheck( false );
		mFamilyWriteButton.SetCheck( false );

		mOperatorReadButton.SetCheck( false );
		mOperatorWriteButton.SetCheck( false );

		mAllowIpReadButton.SetCheck( false );
		mAllowIpWriteButton.SetCheck( false );

		mOperatorLogReadButton.SetCheck( false );
		mOperatorLogWriteButton.SetCheck( false );

		mItemReadButton.SetCheck( false );
		mItemWriteButton.SetCheck( false );
	}

	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	const AuthorityMap::const_iterator map_it = mAuthorityMap.find( GetRankFromTab() );

	// 081021 LUJ, ���� ������ ������ ���� ��� �ʱ�ȭ��Ų��
	if( mAuthorityMap.end() == map_it )
	{
		return;
	}
	
	const AuthorityList& authorityList = map_it->second;

	// 081021 LUJ, üũ �ڽ��� �����Ѵ�
	for(
		AuthorityList::const_iterator it = authorityList.begin();
		authorityList.end() != it;
		++it )
	{
		const MSG_RM_AUTHORITY::Authority& authority = *it;

		switch( authority.mType )
		{
		case AuthorityTypePlayerLevel:
			{
				mPlayerLevelReadButton.SetCheck( authority.mReadable );
				mPlayerLevelWriteButton.SetCheck( authority.mWriteable );
				break;
			}
		case AuthorityTypePlayerCreate:
			{
				mPlayerCreateReadButton.SetCheck( authority.mReadable );
				mPlayerCreateWriteButton.SetCheck( authority.mWriteable );
				break;
			}
		case AuthorityTypePlayerDelete:
			{
				mPlayerDeleteReadButton.SetCheck( authority.mReadable );
				mPlayerDeleteWriteButton.SetCheck( authority.mWriteable );
				break;
			}
		case AuthorityTypePlayerRecover:
			{
				mPlayerRecoverReadButton.SetCheck( authority.mReadable );
				mPlayerRecoverWriteButton.SetCheck( authority.mWriteable );
				break;
			}
		case AuthorityTypePlayerUpdate:
			{
				mPlayerUpdateReadButton.SetCheck( authority.mReadable );
				mPlayerUpdateWriteButton.SetCheck( authority.mWriteable );
				break;
			}
		case AuthorityTypePlayerLog:
			{
				mPlayerLogReadButton.SetCheck( authority.mReadable );
				mPlayerLogWriteButton.SetCheck( authority.mWriteable );
				break;
			}
		case AuthorityTypeSkill:
			{
				mSkillReadButton.SetCheck( authority.mReadable );
				mSkillWriteButton.SetCheck( authority.mWriteable );
				break;
			}
		case AuthorityTypeQuest:
			{
				mQuestReadButton.SetCheck( authority.mReadable );
				mQuestWriteButton.SetCheck( authority.mWriteable );
				break;
			}
		case AuthorityTypeGuild:
			{
				mGuildReadButton.SetCheck( authority.mReadable );
				mGuildWriteButton.SetCheck( authority.mWriteable );
				break;
			}
		case AuthorityTypeFamily:
			{
				mFamilyReadButton.SetCheck( authority.mReadable );
				mFamilyWriteButton.SetCheck( authority.mWriteable );
				break;
			}
		case AuthorityTypeOperator:
			{
				mOperatorReadButton.SetCheck( authority.mReadable );
				mOperatorWriteButton.SetCheck( authority.mWriteable );
				break;
			}
		case AuthorityTypeAllowIp:
			{
				mAllowIpReadButton.SetCheck( authority.mReadable );
				mAllowIpWriteButton.SetCheck( authority.mWriteable );
				break;
			}
		case AuthorityTypeOperatorLog:
			{
				mOperatorLogReadButton.SetCheck( authority.mReadable );
				mOperatorLogWriteButton.SetCheck( authority.mWriteable );
				break;
			}
		case AuthorityTypeItem:
			{
				mItemReadButton.SetCheck( authority.mReadable );
				mItemWriteButton.SetCheck( authority.mWriteable );
				break;
			}
		}
	}
}

eGM_POWER CAuthorityDialog::GetRankFromTab() const
{
	// 081021 LUJ, ������ ������ � �������� �˾Ƴ���
	switch( mLevelTabCtrl.GetCurSel() )
	{
	case 0:
		{
			return eGM_POWER_MONITOR;
		}
	case 1:
		{
			return eGM_POWER_PATROLLER;
		}
	case 2:
		{
			return eGM_POWER_AUDITOR;
		}
	case 3:
		{
			return eGM_POWER_EVENTER;
		}
	case 4:
		{
			return eGM_POWER_QA;
		}
	}

	return eGM_POWER_MAX;
}