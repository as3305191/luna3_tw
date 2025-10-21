// LoginDialog.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "client.h"
#include "LoginDialog.h"
#include "..\common\version.h"
#include ".\logindialog.h"


// CLoginDialog ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CLoginDialog, CDialog)
CLoginDialog::CLoginDialog( CclientApp& application, CWnd* pParent /*=NULL*/)
	: CDialog(CLoginDialog::IDD, pParent)
	, mApplication( application )
{
	EnableAutomation();
}

CLoginDialog::~CLoginDialog()
{}

void CLoginDialog::OnFinalRelease()
{
	// �ڵ�ȭ ��ü�� ���� ������ ������ �����Ǹ� OnFinalRelease��
	// ȣ��˴ϴ�. �⺻ Ŭ�������� �ڵ����� ��ü�� �����մϴ�.
	// �⺻ Ŭ������ ȣ���ϱ� ���� ��ü�� �ʿ��� �߰� ���� �۾���
	// �߰��Ͻʽÿ�.

	CDialog::OnFinalRelease();
}

void CLoginDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//DDX_Text( pDX, IDC_LOGIN_ID, mIdEdit );
	//DDX_Text( pDX, IDC_LOGIN_PASSWORD, mPasswordEdit );

	//DDX_Control( pDX, IDC_LOGIN_ID, mIdEdit );
	DDX_Control( pDX, IDC_LOGIN_PASSWORD, mPasswordEdit );
	//DDX_Control(pDX, IDC_LOGIN_VERSION_STATIC, mVersionStatic);
	DDX_Control(pDX, IDC_LOGIN_SERVER_LIST, mServerListCtrl);
	DDX_Control(pDX, IDC_LOGIN_VERSION_STATIC, mVersionStatic);
	DDX_Control(pDX, IDC_LOGIN_ID2, mIdEdit);
	DDX_Control(pDX, IDC_LOGIN_CONNECT_BUTTON, mSubmitButton);
	DDX_Control(pDX, IDCANCEL, mCancelButton);
	DDX_Control(pDX, IDC_LOGIN_LOADING_PROGRESS, mLoadingProgressCtrl);
	DDX_Control(pDX, IDC_STATIC_TITLE, mTitleImageStatic);
}


BEGIN_MESSAGE_MAP(CLoginDialog, CDialog)
	//ON_BN_CLICKED(IDCANCEL, OnCancel)
	ON_BN_CLICKED(IDC_LOGIN_CONNECT_BUTTON, OnBnClickedLoginConnectButton)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CLoginDialog, CDialog)
END_DISPATCH_MAP()

// ����: IID_ILoginDialog�� ���� ������ �߰��Ͽ�
// VBA���� ���� �������� ���ε��� �����մϴ�. 
// �� IID�� .IDL ���Ͽ� �ִ� dispinterface�� GUID�� ��ġ�ؾ� �մϴ�.

// {E8293FEB-4841-4E9F-9297-89C994942D83}
static const IID IID_ILoginDialog =
{ 0xE8293FEB, 0x4841, 0x4E9F, { 0x92, 0x97, 0x89, 0xC9, 0x94, 0x94, 0x2D, 0x83 } };

BEGIN_INTERFACE_MAP(CLoginDialog, CDialog)
	INTERFACE_PART(CLoginDialog, IID_ILoginDialog, Dispatch)
END_INTERFACE_MAP()


// CLoginDialog �޽��� ó�����Դϴ�.

//void CLoginDialog::OnOK()
//{
//	CDialog::OnOK();
//}


void CLoginDialog::Parse( DWORD serverIndex, const MSGROOT* message )
{
	switch( message->Protocol )
	{
	case MP_RM_CONNECT_ACK:
		{
			static size_t count;

			if( mApplication.GetConnectionSize() == ++count )
			{
				// 080304 LUJ, �α�
				{
					CString id;
					mIdEdit.GetWindowText( id );

					const TCHAR* line = _T( "* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *" );

					mApplication.Log( line );
					mApplication.Log(
						_T( "\t Recovery Client %d.%d.%d is running by %s. Welcome" ),
						MAJORVERSION,
						MINORVERSION,
						BUILDNUMBER,
						id );
					mApplication.Log( line );

					// 081021 LUJ, ���۷����͸� â ���� ǥ���Ѵ�
					{
						CWnd* window = AfxGetApp()->GetMainWnd();

						if( ! window )
						{
							break;
						}

						CString title;
						window->GetWindowText( title );
						window->SetWindowText( title + _T( ": " ) + id );
					}
				}

				CString textWarning;
				textWarning.LoadString( IDS_STRING127 );

				MessageBox( textWarning, _T( "" ), MB_ICONWARNING | MB_OK );

				OnOK();

				{
					CWnd* window = mApplication.GetMainWnd();
					ASSERT( window );

					if( window )
					{
						window->EnableWindow( TRUE );
					}

					CDialog* dialog = ( CDialog* )( mApplication.GetNewUserDialog() );
					ASSERT( dialog );

					if( dialog )
					{
						dialog->ShowWindow( SW_SHOW );
					}
				}
			}
			
			break;
		}
	case MP_RM_CONNECT_NACK:
		{
			MSG_DWORD* m = ( MSG_DWORD* )message;

			switch( m->dwData )
			{
			case 1:
				{
					CString textItIsNoConnectableIp;
					textItIsNoConnectableIp.LoadString( IDS_STRING130 );

					MessageBox( textItIsNoConnectableIp, _T( "" ), MB_ICONERROR | MB_OK );

					OnCancel();
					break;
				}
			case 2:
			case 3:
				{
					CString textItIsWrongIdOrPassword;
					textItIsWrongIdOrPassword.LoadString( IDS_STRING134 );

					mVersionStatic.SetWindowText( textItIsWrongIdOrPassword );

					{
						mIdEdit.EnableWindow( TRUE );
						mPasswordEdit.EnableWindow( TRUE );
						mSubmitButton.EnableWindow( TRUE );
					}

					break;
				}
			default:
				{
					CString textItIsConnectionFault;
					textItIsConnectionFault.LoadString( IDS_STRING131 );

					MessageBox( textItIsConnectionFault, _T( "" ), MB_ICONERROR | MB_OK );

					OnCancel();
					break;
				}
			}
			
			break;
		}
	case MP_RM_CONNECT_NACK_BY_IP:
	case MP_RM_CONNECT_NACK_BY_ID:
		{
			CString textYouHaveLoggedAlready;
			textYouHaveLoggedAlready.LoadString( IDS_STRING132 );

			MessageBox( textYouHaveLoggedAlready, _T( "" ), MB_ICONERROR | MB_OK );

			OnCancel();
			break;
		}
	case MP_RM_CONNECT_NACK_BY_AUTH:
		{
			CString textYouAreBlocked;
			textYouAreBlocked.LoadString( IDS_STRING133 );

			MessageBox( textYouAreBlocked, _T( "" ), MB_OK | MB_ICONERROR );

			OnCancel();
			break;
		}
	case MP_RM_CONNECT_NACK_BY_VERSION:
		{
			const MSG_INT2* m = ( MSG_INT2* )message;

			CString text;
			{
				CString textInvalidVersionFound;
				textInvalidVersionFound.LoadString( IDS_STRING343 );

				text.Format(
					textInvalidVersionFound,
					mApplication.GetServerName( serverIndex ),
					m->nData1,
					m->nData2 );
			}

			mApplication.Log( text );

			MessageBox( text, _T( "" ), MB_OK | MB_ICONERROR );
            OnCancel();            			
			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}
}


BOOL CLoginDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

#ifdef _DEBUG
	mIdEdit.SetWindowText( _T( "ehei" ) );
	mPasswordEdit.SetWindowText( _T( "1488" ) );
#endif

	SetIcon( AfxGetApp()->LoadIcon( IDR_MAINFRAME ), FALSE );
	SetIcon( AfxGetApp()->LoadIcon( IDR_MAINFRAME ), TRUE );

	CString textBuildData;
	textBuildData.LoadString( IDS_STRING135 );

	// ����Ʈ ��Ʈ�� ����
	{
		const DWORD addStyle = LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT;
		CRect rect;

		{
			int step = -1;

			mServerListCtrl.GetWindowRect( rect );
			mServerListCtrl.SetExtendedStyle( mServerListCtrl.GetExtendedStyle() | addStyle );

			CString textIndex;
			textIndex.LoadString( IDS_STRING22 );
			mServerListCtrl.InsertColumn( ++step, textIndex, LVCFMT_LEFT, 0 );

			CString textName;
			textName.LoadString( IDS_STRING20 );
			mServerListCtrl.InsertColumn( ++step, textName, LVCFMT_LEFT, int( rect.Width() * 0.4f ) );

			CString textIpAddress;
			textIpAddress.LoadString( IDS_STRING136 );
			mServerListCtrl.InsertColumn( ++step, textIpAddress, LVCFMT_LEFT, int( rect.Width() * 0.45f ) );

			CString textEnable;
			textEnable.LoadString( IDS_STRING25 );
			mServerListCtrl.InsertColumn( ++step, textEnable, LVCFMT_LEFT, int( rect.Width() * 0.15f ) );

			mApplication.SetServerListCtrl( mServerListCtrl );
		}
	}

	// 080630 LUJ, �ε��� ���� �� Ȱ��ȭ��Ų��
	mIdEdit.EnableWindow( FALSE );
	mPasswordEdit.EnableWindow( FALSE );
	mSubmitButton.EnableWindow( FALSE );
	mCancelButton.EnableWindow( FALSE );

	const int titleImage[] = {
		IDB_BITMAP3,
		IDB_BITMAP4,
		IDB_BITMAP5,
		IDB_BITMAP6,
		IDB_BITMAP7,
		IDB_BITMAP8,
		IDB_BITMAP9,
		IDB_BITMAP10,
		IDB_BITMAP11,
		IDB_BITMAP12,
		IDB_BITMAP13,
		IDB_BITMAP14,
		IDB_BITMAP15,
		IDB_BITMAP16,
	};

	srand(
		GetTickCount());
	mTitleImage.LoadBitmap(
		titleImage[rand() % _countof(titleImage)]);
	mTitleImageStatic.SetBitmap(
		mTitleImage);

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


void CLoginDialog::OnBnClickedLoginConnectButton()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	if( ! mApplication.GetConnectionSize() )
	{
		CString textAnyConnectableServerDoNotExist;
		textAnyConnectableServerDoNotExist.LoadString( IDS_STRING137 );

		MessageBox( textAnyConnectableServerDoNotExist, _T( "" ), MB_ICONERROR | MB_OK );
		return;
	}

	CDataExchange dx( this, TRUE );
	DoDataExchange( &dx );

	CString id;
	CString password;

	mIdEdit.GetWindowText( id );
	mPasswordEdit.GetWindowText( password );

	if( id.IsEmpty() || password.IsEmpty() )
	{
		CString textIdOrPasswordIsEmpty;
		textIdOrPasswordIsEmpty.LoadString( IDS_STRING138 );

		MessageBox( textIdOrPasswordIsEmpty, _T( "" ), MB_OK );
		return;
	}

	{
		mIdEdit.EnableWindow( FALSE );
		mPasswordEdit.EnableWindow( FALSE );
		mSubmitButton.EnableWindow( FALSE );
	}

	{
		MSG_RM_LOGIN message;

		ZeroMemory( &message, sizeof( message ) );
		message.Category = MP_RM_CONNECT;
		message.Protocol = MP_RM_CONNECT_SYN;

		StringCopySafe( message.mId,	CW2AEX< MAX_PATH >( id ),		sizeof( message.mId ) );
		StringCopySafe( message.mPass,	CW2AEX< MAX_PATH >( password ),	sizeof( message.mPass ) );

		{
			MSG_RM_LOGIN::Version& version = message.mVersion;

			version.mMajor	= MAJORVERSION;
			version.mMinor	= MINORVERSION;
		}
		
		mApplication.SendAll( message, sizeof( message ) );
	}
}


// 080630 LUJ, �ε� ���α׷��� ��Ʈ���� ���� �����Ѵ�
void CLoginDialog::SetProgressValue( int value, const TCHAR* message )
{
	mLoadingProgressCtrl.SetPos( value );
	mVersionStatic.SetWindowText( message );

	// 080630 LUJ, ��� �ε������� �α����� �� �ֵ��� �Ѵ�
	{
		int minRange = 0;
		int maxRange = 0;
		mLoadingProgressCtrl.GetRange( minRange, maxRange );

		if( mLoadingProgressCtrl.GetPos() >= maxRange )
		{
			mIdEdit.EnableWindow( TRUE );
			mPasswordEdit.EnableWindow( TRUE );
			mSubmitButton.EnableWindow( TRUE );
			mCancelButton.EnableWindow( TRUE );

			// ���� ���¸� ������Ʈ�Ѵ�.
			mApplication.SetServerListCtrl( mServerListCtrl );
		}
	}	
}


void CLoginDialog::OnCancel()
{
	AfxGetMainWnd()->PostMessage( WM_CLOSE, 0, 0 );

	CDialog::OnCancel();
}