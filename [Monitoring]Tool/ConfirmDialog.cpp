// ConfirmDialog.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "ServerTool.h"
#include "ConfirmDialog.h"
#include ".\confirmdialog.h"


// CConfirmDialog ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CConfirmDialog, CDialog)
CConfirmDialog::CConfirmDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CConfirmDialog::IDD, pParent)
{
	EnableAutomation();	
}

CConfirmDialog::~CConfirmDialog()
{
}

void CConfirmDialog::OnFinalRelease()
{
	// �ڵ�ȭ ��ü�� ���� ������ ������ �����Ǹ� OnFinalRelease��
	// ȣ��˴ϴ�. �⺻ Ŭ�������� �ڵ����� ��ü�� �����մϴ�.
	// �⺻ Ŭ������ ȣ���ϱ� ���� ��ü�� �ʿ��� �߰� ���� �۾���
	// �߰��Ͻʽÿ�.

	CDialog::OnFinalRelease();
}

void CConfirmDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_CONFIRM_TEXT_EDIT, mConfirmTextEdit);
	DDX_Control(pDX, IDC_CONFIRM_TEXT_STATIC, mConfirmTextStatic);
}


BEGIN_MESSAGE_MAP(CConfirmDialog, CDialog)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CConfirmDialog, CDialog)
END_DISPATCH_MAP()

// ����: IID_IConfirmDialog�� ���� ������ �߰��Ͽ�
// VBA���� ���� �������� ���ε��� �����մϴ�. 
// �� IID�� .IDL ���Ͽ� �ִ� dispinterface�� GUID�� ��ġ�ؾ� �մϴ�.

// {432502EB-F926-4C86-BEEF-3438D75F946B}
static const IID IID_IConfirmDialog =
{ 0x432502EB, 0xF926, 0x4C86, { 0xBE, 0xEF, 0x34, 0x38, 0xD7, 0x5F, 0x94, 0x6B } };

BEGIN_INTERFACE_MAP(CConfirmDialog, CDialog)
	INTERFACE_PART(CConfirmDialog, IID_IConfirmDialog, Dispatch)
END_INTERFACE_MAP()


// CConfirmDialog �޽��� ó�����Դϴ�.

BOOL CConfirmDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		const TCHAR* characterSet[] = {
			_T( "0" ), _T( "1" ), _T( "2" ), _T( "3" ), _T( "4" ), _T( "5" ), _T( "6" ), _T( "7" ), _T( "8" ), _T( "9" ),
			_T( "a" ), _T( "b" ), _T( "c" ), _T( "d" ), _T( "e" ), _T( "f" ), _T( "g" ), _T( "h" ), _T( "i" ), _T( "j" ), _T( "k" ), _T( "l" ), _T( "m" ), _T( "n" ), _T( "o" ), _T( "p" ), _T( "q" ), _T( "r" ), _T( "s" ), _T( "t" ), _T( "u" ), _T( "v" ), _T( "w" ), _T( "x" ), _T( "y" ), _T( "z" ),
			_T( "A" ), _T( "B" ), _T( "C" ), _T( "D" ), _T( "E" ), _T( "F" ), _T( "G" ), _T( "H" ), _T( "I" ), _T( "J" ), _T( "K" ), _T( "L" ), _T( "M" ), _T( "N" ), _T( "O" ), _T( "P" ), _T( "Q" ), _T( "R" ), _T( "S" ), _T( "T" ), _T( "U" ), _T( "V" ), _T( "W" ), _T( "X" ), _T( "Y" ), _T( "Z" ),
		};

		const DWORD	confirmTextSize = 5;

		srand( GetTickCount() );

		for( DWORD i = 0; i < confirmTextSize; ++i )
		{
			const DWORD	characterSize = sizeof( characterSet ) / sizeof( *characterSet );

			mConfirmText += *( characterSet + ( rand() % characterSize ) );
		}

		HDC hdc = ::GetDC( 0 );

		// 080822 LUJ, CStatic ��Ʈ ����. �׷���.. ������ �𸣰����� �۵����� ����
		//				http://www.codeguru.com/forum/printthread.php?t=351067
		CFont font;
		{
			font.CreateFont(
				26,
				0,
				0,
				0,
				700,
				TRUE,
				TRUE,
				TRUE,
				DEFAULT_CHARSET,
				OUT_DEFAULT_PRECIS,
				CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY,
				DEFAULT_PITCH | FF_SWISS,
				_T( "Tahoma" ) );
		}
		
		mConfirmTextStatic.SetFont( &font );
		mConfirmTextStatic.SetWindowText( mConfirmText );
	}

	return TRUE;
}

void CConfirmDialog::OnOK()
{
	mConfirmTextEdit.GetWindowText( mInputText );

	CDialog::OnOK();
}