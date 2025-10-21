#include "stdafx.h"
#include "Application.h"
#include "MainDialog.h"
#include "DirectoryName.h"
#include "HtmlViewNotice.h"
#include "../[Lib]ZipArchive/ZipArchive.h"
#include "../4dyuchiGRX_common/IExecutive.h"
#include <set>
#include <map>
#include <string>
#include <d3d9.h>
#include <accctrl.h>
#include <aclapi.h>
#include "../4DyuchiGRX_common/IFileStorage.h"



// 090512 ShinJS --- Add XTrap Library
#ifdef _XTRAP_
#pragma comment(lib, "urlmon")
#pragma comment(lib, "wininet")
#include "urlmon.h"					// MS ���� Library��

#pragma comment(lib, "XTrap4Launcher_mt" )

#include "XTrap4Launcher.h"
#endif

#include "IOutBound.h"

int TmpCVer;
int TmpSVer;
LPCTSTR packingFolder[] = 
{
	_T( "data\\3dData\\character.pak" ),
	_T( "data\\3dData\\effect.pak" ),
	_T( "data\\3dData\\farm.pak" ),
	_T( "data\\3dData\\housing.pak" ),
	_T( "data\\3dData\\map.pak" ),
	_T( "data\\3dData\\monster.pak" ),
	_T( "data\\3dData\\npc.pak" ),
	_T( "data\\interface\\2dImage\\image.pak" ),
	_T( "data\\interface\\2dImage\\npcImage.pak" )
};

LPCTSTR packingFolderPak[] = 
{
	_T( "data\\3dData\\character" ),
	_T( "data\\3dData\\effect" ),
	_T( "data\\3dData\\farm" ),
	_T( "data\\3dData\\housing" ),
	_T( "data\\3dData\\map" ),
	_T( "data\\3dData\\monster" ),
	_T( "data\\3dData\\npc" ),
	_T( "data\\interface\\2dImage\\image" ),
	_T( "data\\interface\\2dImage\\npcImage" )
};

CString GetPath(CString fullPath)
{
	CString path;
	CString text;

	int start  = 0;

	do
	{
		const TCHAR* pathSymbol = _T( "\\" );

		text = fullPath.Tokenize( CString( _T( "." ) ) + pathSymbol, start );

		if( text == _T( "pak" ) )
		{
			break;
		}

		path += text + pathSymbol;

	} while( ! text.IsEmpty() );

	return path;
}

CWinThread* packingThread;

CMainDialog::CMainDialog(CWnd* pParent ) :
CDialog(CMainDialog::IDD, pParent),
mIsReady( false ),
mHtmlViewNotice(new CHtmlViewNotice)
{}

void CMainDialog::DeleteOldFile(CString filepath)
{
	const CString deletedFilePath = filepath;

	SetFileAttributes(
		deletedFilePath,
		FILE_ATTRIBUTE_ARCHIVE);

	CString logMessage = deletedFilePath + _T(" ... ");
	DeleteFile(deletedFilePath);
	const DWORD lastError = GetLastError();

	if(0 == lastError)
	{
		logMessage += _T("[OK]");
	}
	else
	{
		LPVOID errorMessage;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			0,
			lastError,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			LPTSTR(&errorMessage),
			0,
			0);
		logMessage += CString(LPCTSTR(errorMessage));
		LocalFree(errorMessage);
	}
}
void CMainDialog::DeleteOldFile()
{
	// 080625 LUJ, ��ŷ�� ������ �ϰ� �����Ѵ�. ��ŷ ���� �߿� �����ϸ� ��ŷ�� ���������� �̷����� �ʴ´�. ��������
	//				executive.dll���� ������� ��ŷ�ϸ鼭 ������ ������ ��� ��ҵǱ� ������ �� ����.
	for( size_t size = sizeof( packingFolder ) / sizeof( *packingFolder ); size--; )
	{
		const TCHAR*	fullPath	= packingFolder[ size ];
		const CString	path		= GetPath( fullPath );

		TCHAR currentPath[ MAX_PATH ] = { 0 };
		GetCurrentDirectory( sizeof( currentPath ), currentPath );

		if( ! SetCurrentDirectory( path ) )
		{
			continue;
		}

		CDirectoryName directory;
		directory.LoadChild( LCO_FILE );

		SetCurrentDirectory( currentPath );

		directory.SetPositionHead();

		CLog::Put( _T( "%s is removing..." ), path );

		for( ;; )
		{
			CString fileName;
			const DWORD kind = directory.GetNextFileName( fileName );

			if( GETNEXTFILENAME_END == kind )
			{
				break;
			}
			else if(GETNEXTFILENAME_FILE != kind ||
				fileName.IsEmpty() )
			{
				continue;
			}

			const CString deletedFileName = path + _T( "\\" ) + fileName;
			// 091214 LUJ, �б� ���� ������ ��� ������ �ȵǾ� ���� ��ŷ�� ������ �ȴ�.
			//			���� �����ϵ��� ���� �Ӽ��� �����Ѵ�
			SetFileAttributes(
				deletedFileName,
				FILE_ATTRIBUTE_ARCHIVE);

			CString logMessage = fileName + _T(" ... ");
			DeleteFile(deletedFileName);
			const DWORD lastError = GetLastError();

			if(0 == lastError)
			{
				logMessage += _T("[OK]");
			}
			else
			{
				LPVOID errorMessage;
				FormatMessage(
					FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
					0,
					lastError,
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					LPTSTR(&errorMessage),
					0,
					0);
				logMessage += CString(LPCTSTR(errorMessage));
				LocalFree(errorMessage);
			}

			CLog::Put(
				_T( "\t%s" ),
				logMessage);
		}

		RemoveDirectory( path );
	}
}

CMainDialog::~CMainDialog()
{}

void CMainDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	// 090720 ShinJS --- Server Auto Patch �߰� �۾�
#ifndef _FOR_SERVER_
	DDX_Control(pDX, IDC_SCREENSIZE, mResolutionCombo);
	DDX_Control(pDX, IDC_INTERFACE, mInterfaceCombo);
	DDX_Control(pDX, IDC_ALIASHING, mAliashingCombo);
	DDX_Control(pDX, IDC_LANGUAGE, mLanguageCombo);
#endif
}

BEGIN_MESSAGE_MAP(CMainDialog, CDialog)
	//{{AFX_MSG_MAP(CMainDialog)
	ON_WM_PAINT()
	ON_WM_CLOSE()
	ON_WM_DRAWITEM()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	// 090720 ShinJS --- Server Auto Patch �߰� �۾�
#ifndef _FOR_SERVER_
	ON_CBN_SELCHANGE(IDC_SCREENSIZE, OnCbnSelchangeScreensize)
	ON_CBN_SELCHANGE(IDC_INTERFACE, OnCbnSelchangeInterface)
	// Firman Akbar 10/05/2022 Custom Button Launcher
	ON_CBN_SELCHANGE(IDC_ALIASHING, OnCbnSelchangeAliashing)
	ON_CBN_SELCHANGE(IDC_LANGUAGE, OnCbnSelchangeLanguage)
#endif
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainDialog message handlers

BOOL CMainDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	mImage.Load(
		theApp.GetConfig().mImage,
		CXIMAGE_FORMAT_TIF);

	mImage.Crop(
		theApp.GetConfig().mArrowPosition.mRect,
		&mInterfaceArrowImage);
	mImage.Crop(
		theApp.GetConfig().mArrowPosition.mRect,
		&mResolutionArrowImage);

	PutBackground();
	PutTitle();
	PutTip();

	// üũ �ڽ�
	{
		CxImage image;
		mImage.Crop(
			theApp.GetConfig().mUncheckedPosition.mRect,
			&image);
		m_backgroundImage.Mix(
			image,
			CxImage::OpDstCopy,
			-theApp.GetConfig().mUncheckedPosition.mPoint.x, 
			theApp.GetConfig().mUncheckedPosition.mPoint.y + theApp.GetConfig().mUncheckedPosition.mRect.Height() - m_backgroundImage.GetHeight(),
			true);

		mImage.Crop(
			theApp.GetConfig().mCheckedPosition.mRect,
			&m_checkBoxImage);
		m_checkBoxImage.Enable(
			theApp.GetConfig().m_isWindowMode);

		mImage.Crop(
			theApp.GetConfig().mCheckedPosition.mRect,
			&m_checkBoxImage);
	}

	mImage.Crop(
		theApp.GetConfig().mDiscordButton.mRect,
		&m_discordImage);
 
	mImage.Crop(
		theApp.GetConfig().mFacebookButton.mRect,
		&m_facebookImage);

	mImage.Crop(
		theApp.GetConfig().mCloseUpPosition.mRect,
		&m_closeImage);
	mImage.Crop(
		theApp.GetConfig().mProgressCellPosition.mRect,
		&m_progress.m_cellImage);
	m_progress.m_value = 0;

	SetWindowPos(
		this,
		0,
		0,
		m_backgroundImage.GetWidth(),
		m_backgroundImage.GetHeight(),
		SWP_NOZORDER);
		
	SetWindowLong(
		GetSafeHwnd(),
		GWL_EXSTYLE,
		GetWindowLong(GetSafeHwnd(), GWL_EXSTYLE) | WS_EX_LAYERED);
	SetLayeredWindowAttributes(
		RGB(0, 73, 164),
		100,
		LWA_COLORKEY);

	// 090720 ShinJS --- Server Auto Patch �߰� �۾�
#ifndef _FOR_SERVER_
	SetWindowText(
		theApp.GetConfig().m_textCaption);

	/*mHtmlViewNotice->Create(
		0,
		0,
		WS_CHILD,
		theApp.GetConfig().mHtmlPosition.mRect,
		this,
		1);
	mHtmlViewNotice->Navigate2(
		theApp.GetLocal().mURL.mContent);

	const int htmlViewBorder = 2;
	mHtmlViewNotice->SetLeft(
		-htmlViewBorder);
	mHtmlViewNotice->SetTop(
		-htmlViewBorder);
	mHtmlViewNotice->SetWidth(
		mHtmlViewNotice->GetWidth() + htmlViewBorder);
	mHtmlViewNotice->SetHeight(
		mHtmlViewNotice->GetHeight() + htmlViewBorder);
*/
	FILE* fp = fopen("./_DONOTUSEENGINEPACK","r");
	if(fp)
	{
		fclose(fp);
	}

	InvalidateRect(
		0,
		FALSE);
	UpdateWindow();
#endif

#ifdef _LOCAL_JP_
	// 090819 ONS ���÷�ó�κ��� ���޵� ���ڷ� ���� �ڵ鰪�� ����.
	// �����츦 �����.
	ModifyStyleEx(WS_EX_APPWINDOW,WS_EX_TOOLWINDOW);
	SetWindowPos( 0, 0, 0, 0, 0, SWP_HIDEWINDOW );

	// �ڵ��� ��´�.
	m_hTarget = theApp.GetTargetHandle();
	if( NULL != m_hTarget )
	{
		// ���÷�ó UI�ʱ�ȭ �޼��� ����
		CMainDialog* dialog = ( CMainDialog* )( theApp.GetMainWnd() );
		dialog->SendMessageToPurple(PURPLE_OUTBOUND_PATCH_PROGRESS, _T(""), 0);
		dialog->SendMessageToPurple(PURPLE_OUTBOUND_PATCH_PROGRESS, _T(""), 0);
		dialog->SendMessageToPurple(PURPLE_OUTBOUND_FILENAME, _T(""), 0);
		dialog->SendMessageToPurple(PURPLE_OUTBOUND_DOWNLOAD_SPEED, _T(""), 0);
		dialog->SendMessageToPurple(PURPLE_OUTBOUND_PATCH_FILE_NUMBER, _T(""), MAKELPARAM(0, 0));
	}
	else
	{
		MessageBox(theApp.GetConfig().m_messageBoxPrupleInvalidArgument);
		ExitProcess(0);
	}

	theApp.SetLocalEx();
#endif

	// 080515 LUJ, ���� ������ �ٿ�ε�޴´�
	{
		const CApplication::Local& local = theApp.GetLocal();

		theApp.Download(
			local.mDownload.mPath,
			local.mDownload.mVersionTag + local.mDownload.mVersionFile,
			CMainDialog::CheckVersionToDisk,
			CMainDialog::CheckVersionToMemory,
			0,
			theApp.GetConfig().mFileType );

		CLog::Put( _T( "%s is downloading..." ), local.mDownload.mPath + local.mDownload.mVersionTag + local.mDownload.mVersionFile );
	}

	UpdateResolutionCombo();
	UpdateInterfaceCombo();
	UpdateAliashingCombo();

	UpdateLanguageCombo();
	return TRUE;  // return TRUE  unless you set the focus to a control
}


// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void CMainDialog::OnPaint() 
{
	CPaintDC dc( this );

	
	m_backgroundImage.Draw(
		dc.m_hDC,
		0,
		0,
		-1,
		-1,
		0,
		true);
	m_discordImage.Draw(
		dc.m_hDC,
		theApp.GetConfig().mDiscordButton.mPoint.x,
		theApp.GetConfig().mDiscordButton.mPoint.y,
		-1,
		-1,
		0,
		true);
	m_facebookImage.Draw(
		dc.m_hDC,
		theApp.GetConfig().mFacebookButton.mPoint.x,
		theApp.GetConfig().mFacebookButton.mPoint.y,
		-1,
		-1,
		0,
		true);
	m_beginImage.Draw(
		dc.m_hDC,
		theApp.GetConfig().mBeginUpPosition.mPoint.x,
		theApp.GetConfig().mBeginUpPosition.mPoint.y,
		-1,
		-1,
		0,
		true);
	m_closeImage.Draw(
		dc.m_hDC,
		theApp.GetConfig().mCloseUpPosition.mPoint.x,
		theApp.GetConfig().mCloseUpPosition.mPoint.y,
		-1,
		-1,
		0,
		true);
	m_checkBoxImage.Draw(
		dc.m_hDC,
		theApp.GetConfig().mCheckedPosition.mPoint.x,
		theApp.GetConfig().mCheckedPosition.mPoint.y,
		-1,
		-1,
		0,
		true);
	mInterfaceArrowImage.Draw(
		dc.m_hDC,
		theApp.GetConfig().mInterfaceArrow.x,
		theApp.GetConfig().mInterfaceArrow.y,
		-1,
		-1,
		0,
		true);
	mResolutionArrowImage.Draw(
		dc.m_hDC,
		theApp.GetConfig().mResolutionArrow.x,
		theApp.GetConfig().mResolutionArrow.y,
		-1,
		-1,
		0,
		true);
	mAliashingTipImage.Draw(
		dc.m_hDC,
		theApp.GetConfig().mAliashingTip.mPosition.mPoint.x,
		theApp.GetConfig().mAliashingTip.mPosition.mPoint.y,
		-1,
		-1,
		0,
		true);
	mLanguageTipImage.Draw(
		dc.m_hDC,
		theApp.GetConfig().mLanguageTip.mPosition.mPoint.x,
		theApp.GetConfig().mLanguageTip.mPosition.mPoint.y,
		-1,
		-1,
		0,
		true);

	if(mInterfaceTipImage.IsEnabled())
	{
		mInterfaceTipImage.Draw(
			dc.m_hDC,
			theApp.GetConfig().mInterfaceTip.mPosition.mPoint.x,
			theApp.GetConfig().mInterfaceTip.mPosition.mPoint.y,
			-1,
			-1,
			0,
			true);
	}
	else if(mResolutionArrowImage.IsEnabled())
	{
		mResolutionTipImage.Draw(
			dc.m_hDC,
			theApp.GetConfig().mResolutionTip.mPosition.mPoint.x,
			theApp.GetConfig().mResolutionTip.mPosition.mPoint.y,
			-1,
			-1,
			0,
			true);
	}
	if(mLoadingTipImage.IsEnabled() )
	{
		mLoadingTipImage.Draw(
			dc.m_hDC, 
			theApp.GetConfig().mLoadingTip.mPosition.mPoint.x,
			theApp.GetConfig().mLoadingTip.mPosition.mPoint.y,
			-1,
			-1,
			0,
			true);
	}
	else if(mPackingTipImage.IsEnabled() )
	{
		mPackingTipImage.Draw(
			dc.m_hDC,
			theApp.GetConfig().mPackingTip.mPosition.mPoint.x,
			theApp.GetConfig().mPackingTip.mPosition.mPoint.y,
			-1,
			-1,
			0,
			true);
	}
	// Firman Akbar 10/05/2022 Custom Button Launcher
	else if(mAliashingTipImage.IsEnabled())
	{
		mAliashingTipImage.Draw(
			dc.m_hDC,
			theApp.GetConfig().mAliashingTip.mPosition.mPoint.x,
			theApp.GetConfig().mAliashingTip.mPosition.mPoint.y,
			-1,
			-1,
			0,
			true);
	}
	else if(mLanguageTipImage.IsEnabled())
	{
		mLanguageTipImage.Draw(
			dc.m_hDC,
			theApp.GetConfig().mLanguageTip.mPosition.mPoint.x,
			theApp.GetConfig().mLanguageTip.mPosition.mPoint.y,
			-1,
			-1,
			0,
			true);
	}

	CDialog::OnPaint();
}

void CMainDialog::OnClose() 
{
	if(mHtmlViewNotice)
	{
		delete mHtmlViewNotice;
		mHtmlViewNotice = 0;
	}

	// 080515 LUJ, �ٿ�޴� ��ġ ������ �����ش�
	DeleteFile( mPatchVersion + _T( ".zip" ) );

	CDialog::OnClose();
}

UINT CMainDialog::Pack( LPVOID )
{
	CMainDialog* dialog = ( CMainDialog* )( theApp.GetMainWnd() );

	dialog->PostMessage( WM_DOWNLOAD_END );

	// �Լ��� ����������, �ڵ����� ������ 0���� �����Ѵ�
	class FlagChanger
	{
	public:
		FlagChanger(CWinThread*& thread) :
		mThread(thread)
		{}

		~FlagChanger()
		{
			mThread = 0;
		}

	private:
		CWinThread*& mThread;
	}
	flagChanger(packingThread);

	// 090903 ShinJS --- DLL �� ����� ��� Packing ���� ��ó�� ��������� �ϱ� ������ ������ ���� �����ϰ� ���ο� ��óȮ���� Packing�� �Ѵ�.

	// 090720 ShinJS --- ���������� �����ϰ� �ٿ�ε��� ���������� �����Ѵ�
	//SaveVersion();	

	// 080515 LUJ, ���ο� ��ó�� ������ �����ϰ� ������Ѵ�.
	{
		CFile file;

		if( file.Open( _T( "Launcher.new" ), CFile::modeRead ) )
		{
			dialog->EndDialog( IDRETRY );
#ifdef _LOCAL_JP_
			// 090819 ONS ��ġ�� �� �����ٴ� �޽����� ������.
			dialog->SendMessageToPurple(PURPLE_OUTBOUND_ENDING, _T(""), 0);
#endif
			return 0;
		}
		else if( file.Open( _T( "SS3DGFunc.new" ), CFile::modeRead ) )
		{
			dialog->EndDialog( IDRETRY );
#ifdef _LOCAL_JP_
			// 090819 ONS ��ġ�� �� �����ٴ� �޽����� ������.
			dialog->SendMessageToPurple(PURPLE_OUTBOUND_ENDING, _T(""), 0);
#endif
			return 0;
		}
	}
	
	I4DyuchiGXExecutive* executive = 0;

	// 080515 LUJ, ���� �ʱ�ȭ
	{
		struct
		{
			bool operator() () const
			{
				__try 
				{
					__asm _emit 0x0f 
					__asm _emit 0x57 
					__asm _emit 0xc0
				}
				__except ( EXCEPTION_EXECUTE_HANDLER ) 
				{
					return false;
				}

				return true;
			}
		}
		IsSupportSSE;

		CopyFile(
			IsSupportSSE() ? _T( "SS3DGFuncSSE.dll" ) : _T( "SS3DGFuncN.dll" ),
			_T( "SS3DGFunc.dll" ),
			FALSE );

		HMODULE	handle = LoadLibrary( _T( "Executive.dll" ) );

		if( ! handle )
		{
			return 1;
		}

		CREATE_INSTANCE_FUNC function = ( CREATE_INSTANCE_FUNC )GetProcAddress( handle, "DllCreateInstance" );

		if( S_OK != function( ( LPVOID* )&executive ) )
		{
			MsgBoxLog(
				theApp.GetConfig().m_messageBoxFailedCreateExecutive,
				theApp.GetConfig().m_messageBoxCaption,
				theApp.GetMainWnd()->GetSafeHwnd());
			return 1;
		}
		// geometry DLL�� �̸�, renderer DLL�� �̸��� �־��ָ� executive�� �˾Ƽ� ����,�ʱ�ȭ���ش�.
		// 080221 LUJ, ��ŷ��ų ���� ������ 15000 -> 25000���� ����
		else if( ! executive->InitializeFileStorageWithoutRegistry(
			"FileStorage.dll",
			45000,
			4096,
			128,
			FILE_ACCESS_METHOD_ONLY_FILE,
			NULL,
			0,
			20250813 ) )
		{
			return 1;
		}
		
		HMODULE hStorage = LoadLibrary(  _T("FileStorage.dll") );
		if( ! hStorage )
		{
			return 1;
		}


	}

	if(0 == dialog)
	{
		return 0;
	}
#ifdef _LOCAL_JP_
	// 090819 ONS ���� ���¸� ���÷�ó UI�� ǥ���Ѵ�.("���� ���� ��")
	dialog->SendMessageToPurple(PURPLE_OUTBOUND_INFORM, theApp.GetConfig().m_textPack , 0);
#endif

	size_t maxProgressStep = 0;
	typedef std::map< LPCTSTR, size_t > FileSizeContainer;
	FileSizeContainer fileSizeContainer;

	// 080515 LUJ, ���� ��ü ������ ����
	for( size_t size = sizeof( packingFolder ) / sizeof( *packingFolder ); size--; ++maxProgressStep)
	{
		LPCTSTR fullPath = packingFolder[ size ];

		TCHAR currentPath[ MAX_PATH ] = { 0 };
		GetCurrentDirectory( sizeof( currentPath ), currentPath );
		const CString path = GetPath(fullPath);

		if( SetCurrentDirectory(path) )
		{
			CDirectoryName directory;

			directory.LoadChild();

			// 080515 LUJ, pack ������ unlock/unmap/release ������ ���� �������� ���Խ�Ų��
			maxProgressStep += directory.GetSize() + 3;

			fileSizeContainer[fullPath] = directory.GetSize();
		}

		SetCurrentDirectory( currentPath );
	}

	// 080515 LUJ, �� ������ ��ŷ�Ѵ�
	{
		size_t progressStep	= 0;
		
		for( size_t size = sizeof( packingFolder ) / sizeof( *packingFolder ); size--; )
		{
			LPCTSTR fullPath = packingFolder[ size ];
			const CString path = GetPath( fullPath );

			if(fileSizeContainer.end() == fileSizeContainer.find(fullPath))
			{
				continue;
			}

			// 080515 LUJ,	I4DyuchiFileStorage�� ȹ��Ǳ� ����, �׸��� ������ �Ŀ� �α׸� ���ܾ��Ѵ�
			//				�׷��� ������, �α� ������ ��ŷ ������ ���Եȴ�. ���� �Ҹ�
			killProcess();
			CLog::Put( _T( "%s is packing..." ), fullPath );
			dialog->PutProgress(
				double( ++progressStep ) / maxProgressStep);

			I4DyuchiFileStorage* fileStorage = 0;
			
			executive->GetFileStorage( &fileStorage );

			LPVOID packFile = fileStorage->MapPackFile( (char*)(fullPath), 20250813 );

			if( ! packFile )
			{
				fileStorage->Release();
				continue;
			}

			TCHAR currentPath[ MAX_PATH ] = { 0 };
			GetCurrentDirectory( sizeof( currentPath ), currentPath );

			if( ! SetCurrentDirectory( path ) ||
				! fileStorage->LockPackFile( packFile, 0 ) )
			{
				fileStorage->UnmapPackFile( packFile );
				fileStorage->Release();
				continue;
			}
			


			CDirectoryName directory;
			directory.LoadChild( LCO_FILE );

			SetCurrentDirectory( currentPath );

			directory.SetPositionHead();

			for( ;; )
			{
				CString fileName;				
				const DWORD kind = directory.GetNextFileName( fileName );

				if( GETNEXTFILENAME_END == kind )
				{
					break;
				}
				else if(	GETNEXTFILENAME_FILE != kind ||
							fileName.IsEmpty() )
				{
					continue;
				}

				const CString name = path + _T( "\\" ) + fileName;
				
				// CW2AEX<MAX_PATH> conv((LPCWSTR)(LPCTSTR)name);

				if( ! fileStorage->InsertFileToPackFile( packFile, (char*)(const char *)name ))
				{
					continue;
				}

				dialog->PutProgress( double( ++progressStep ) / maxProgressStep );
                CLog::Put( _T( "\t%s ... %u/%u" ), fileName, progressStep, maxProgressStep );
				
				

                    
				}
				
			
		


			fileStorage->UnlockPackFile( packFile, 0 );


			CLog::Put( _T( "\tI4DyuchiFileStorage is unlocked file ... %u/%u" ), progressStep, maxProgressStep );

			fileStorage->UnmapPackFile( packFile );

			dialog->PutProgress( double( ++progressStep ) / maxProgressStep );


			CLog::Put( _T( "\tI4DyuchiFileStorage is unmaped file ... %u/%u" ), progressStep, maxProgressStep );

			fileStorage->Release();

			dialog->PutProgress( double( ++progressStep ) / maxProgressStep );

			CLog::Put( _T( "\tI4DyuchiFileStorage is released ... %u/%u" ), progressStep, maxProgressStep );

			
		}
	}
	
	

	dialog->DeleteOldFile();

#ifdef _LOCAL_JP_
	// 090819 ONS ��ŷ���¸� ���÷�ó UI�� ǥ���Ѵ�.
	dialog->SendMessageToPurple(PURPLE_OUTBOUND_PATCH_PROGRESS, _T(""), 100 );
#else
	CString nextVersion;
	nextVersion.Format(_T("%d"),TmpCVer);
	theApp.GetConfig().m_textVersion = theApp.GetVersionText() + nextVersion;
	dialog->ReloadTextAfterPack();
	int Max = 100;
	int Min = 0;
	for (Min = 0; Min < Max; Min++)
	{
		dialog->PutProgress( double( ++Min ) / Max );
	}
	
	dialog->PutProgress( 1, true );
#endif
	dialog->PostMessage( WM_PACKING_END );
	Ready();
	return 0;
}

void CMainDialog::OnOK() 
{
	theApp.ReleaseHandle();

	LPTSTR	lpCmdLine;
	LPTSTR	lpProcess;
	
	lpCmdLine = _T( " luna2025" );

	FILE* fp = fopen( "TOOL", "r" );
	if( fp )
	{
		lpProcess = _T( "LUNA_DevR.exe" );
		fclose( fp );
	}
	else
	{
		lpProcess = _T( "Luna.exe" );
	}

	// 090512 ShinJS --- XTrap ������� �ʴ� ���
#ifndef _XTRAP_

	// 080515 LUJ, ������ �����Ѵ�
	{
		STARTUPINFO	sp = { 0 };
		sp.cb			= sizeof(STARTUPINFO);
		sp.dwFlags		= STARTF_USEPOSITION | STARTF_USESIZE | STARTF_USESTDHANDLES; //STARTF_USESHOWWINDOW 
		sp.wShowWindow	= SW_SHOW; 

		PROCESS_INFORMATION pi;
		ZeroMemory( &pi, sizeof( pi ) );

		if( ! CreateProcess( 
			lpProcess,
			lpCmdLine,											// command line string
			0,													// SD
			0,													// SD
			FALSE,												// handle inheritance option
			CREATE_DEFAULT_ERROR_MODE | NORMAL_PRIORITY_CLASS,  // creation flags
			0,													// new environment block
			0,													// current directory name
			&sp,												// startup information
			&pi	) )												// process information
		{
			MessageBox(
				theApp.GetConfig().m_messageBoxFailedRunClient,
				theApp.GetConfig().m_messageBoxCaption,
				MB_OK);
			return;
		}

		CloseHandle( pi.hProcess );
		CloseHandle( pi.hThread );
	}

	// XTrap ���� ShellExcute ���
#else
	// Wiselogic ���������� �߱޹��� Ű
	// 090710 ShinJS --- Key ��߱�( http://xtrap.patch.gamecyber.net/luna/ )
	char szArg[] = "660970B4CA19DA14A1326D9844CFE562A6B290ED238EFCB20F553DF9EB79358504871E4FF695DDC90D80C863DD2628156BBEBDECDBBB3D32508DF18131364E050F7D04245246E41FB1670C4E95407619E96980387320B6505436FCED87B1E5752B8B6662F7";	

	OSVERSIONINFO osvi = {0,};
	SHELLEXECUTEINFO sei = {0,};

	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	sei.cbSize = sizeof(SHELLEXECUTEINFO);

	::GetVersionEx( &osvi );

	// ������ ��Ÿ�� ��� ���� ��� ����
	if( osvi.dwMajorVersion == 6 )
		sei.lpVerb = _T( "runas" );

	sei.lpFile = lpProcess;
	sei.lpParameters = lpCmdLine;
	sei.nShow = SW_SHOW;

	XTrap_L_Patch( szArg,				// XTrap Patch URL ������ ����ִ� ��ȣȭ ���ڿ�
		NULL,							// ���� Ŭ���̾�Ʈ ������( ��ó�� ���� ����� ��� NULL ��� ���� )
		60 );							// �ٿ�ε� Ÿ�Ӿƿ� ��ġ, ��Ŷ 1���� Send/Recv�� �ҿ�Ǵ� �ð�(sec)

	ShellExecuteEx( &sei );
#endif

	CDialog::OnOK();
}

void CMainDialog::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	switch( nIDCtl )
	{
	case IDC_SCREENSIZE:
		{
			mResolutionCombo.DrawItem(
				lpDrawItemStruct);
			mResolutionCombo.DrawSelectItem();

			if(mResolutionCombo.GetDroppedState())
			{
				mImage.Crop(
					theApp.GetConfig().mArrowPushedPosition.mRect,
					&mResolutionArrowImage);

				const CApplication::Position& position = theApp.GetConfig().mResolutionTip.mPosition;
				InvalidateRect(
					CRect(position.mPoint, position.mRect.Size()),
					FALSE);
				mResolutionTipImage.Enable(
					FALSE);
			}
			else
			{
				mImage.Crop(
					theApp.GetConfig().mArrowPosition.mRect,
					&mResolutionArrowImage);
			}

			InvalidateRect(
				CRect(theApp.GetConfig().mResolutionArrow, theApp.GetConfig().mArrowPosition.mRect.Size()),
				false);
			break;
		}
		// 080507 LUJ, �������̽� ���� �޺� �׸���
	case IDC_INTERFACE:
		{
			mInterfaceCombo.DrawItem(
				lpDrawItemStruct);
			mInterfaceCombo.DrawSelectItem();

			if(mInterfaceCombo.GetDroppedState())
			{
				mImage.Crop(
					theApp.GetConfig().mArrowPushedPosition.mRect,
					&mInterfaceArrowImage);

				const CApplication::Position& position = theApp.GetConfig().mInterfaceTip.mPosition;
				InvalidateRect(
					CRect(position.mPoint, position.mRect.Size()),
					FALSE);
				mInterfaceTipImage.Enable(
					FALSE);
			}
			else
			{
				mImage.Crop(
					theApp.GetConfig().mArrowPosition.mRect,
					&mInterfaceArrowImage);
			}

			InvalidateRect(
				CRect(theApp.GetConfig().mInterfaceArrow, theApp.GetConfig().mArrowPosition.mRect.Size()),
				FALSE);
			break;
		}
	case IDC_HTML_VIEW:
		{
			mHtmlViewNotice->SetRedraw( true );
			break;
		}
	// Firman Akbar 10/05/2022 Custom Button Launcher
	case IDC_ALIASHING:
		{
			mAliashingCombo.DrawItem(
				lpDrawItemStruct);
			mAliashingCombo.DrawSelectItem();

			if(mAliashingCombo.GetDroppedState())
			{
				const CApplication::Position& position = theApp.GetConfig().mAliashingTip.mPosition;
				InvalidateRect(
					CRect(position.mPoint, position.mRect.Size()),
					FALSE);
				mAliashingTipImage.Enable(
					FALSE);
			}

			InvalidateRect(
				CRect(theApp.GetConfig().mAliashingArrow, theApp.GetConfig().mArrowPosition.mRect.Size()),
				FALSE);
			break;
		}
	case IDC_LANGUAGE:
		{
			mLanguageCombo.DrawItem(
				lpDrawItemStruct);
			mLanguageCombo.DrawSelectItem();

			if(mLanguageCombo.GetDroppedState())
			{
				const CApplication::Position& position = theApp.GetConfig().mLanguageTip.mPosition;
				InvalidateRect(
					CRect(position.mPoint, position.mRect.Size()),
					FALSE);
				mLanguageTipImage.Enable(
					FALSE);
			}

			InvalidateRect(
				CRect(theApp.GetConfig().mLanguageArrow, theApp.GetConfig().mArrowPosition.mRect.Size()),
				FALSE);
			break;
		}
	}
}

void CMainDialog::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if(mIsReady)
	{
		const CRect rect(
			theApp.GetConfig().mBeginUpPosition.mPoint,
			theApp.GetConfig().mBeginUpPosition.mRect.Size());

		if(rect.PtInRect(point))
		{
			OnOK();
			return;
		}
	}
	
	{
		const CRect rect(
			theApp.GetConfig().mDiscordButton.mPoint,
			theApp.GetConfig().mDiscordButton.mRect.Size());

		if(rect.PtInRect(point))
		{
			ShellExecute(NULL, _T("open"), _T("https://arcaneluna.com/discord"), NULL, NULL, SW_SHOWNORMAL);
			return;
		}
	}

	{
		const CRect rect(
			theApp.GetConfig().mFacebookButton.mPoint,
			theApp.GetConfig().mFacebookButton.mRect.Size());

		if(rect.PtInRect(point))
		{
			ShellExecute(NULL, _T("open"), _T("https://www.facebook.com/PlayArcaneLuna"), NULL, NULL, SW_SHOWNORMAL);
			return;
		}
	}

	// �ݱ� ��ư Ŭ�� �˻�
	{
		const CRect rect(
			theApp.GetConfig().mCloseUpPosition.mPoint,
			theApp.GetConfig().mCloseUpPosition.mRect.Size());

		if(rect.PtInRect(point))
		{
			OnCancel();
			return;
		}
	}
	
	// üũ �ڽ� Ŭ�� �˻�
	{
		const CRect rect(
			theApp.GetConfig().mUncheckedPosition.mPoint,
			theApp.GetConfig().mUncheckedPosition.mRect.Size());

		if(rect.PtInRect(point))
		{
			m_checkBoxImage.Enable(
				!m_checkBoxImage.IsEnabled());
			theApp.GetConfig().m_isWindowMode = m_checkBoxImage.IsEnabled();

			InvalidateRect(
				rect,
				FALSE);
			return;
		}
	}

	// �������̽� ȭ��ǥ Ŭ�� �˻�
	{
		const CRect rect(
			theApp.GetConfig().mInterfaceArrow,
			theApp.GetConfig().mArrowPosition.mRect.Size());

		if(rect.PtInRect(point) &&
			FALSE == mInterfaceCombo.GetDroppedState())
		{
			mInterfaceCombo.ShowDropDown();
			return;
		}
	}

	// �ػ� ȭ��ǥ Ŭ�� �˻�
	{
		const CRect rect(
			theApp.GetConfig().mResolutionArrow,
			theApp.GetConfig().mArrowPosition.mRect.Size());

		if(rect.PtInRect(point) &&
			FALSE == mResolutionCombo.GetDroppedState())
		{
			mResolutionCombo.ShowDropDown();
			return;
		}
	}
	// Firman Akbar 10/05/2022 Custom Button Launcher
	{
		const CRect rect(
			theApp.GetConfig().mAliashingArrow,
			theApp.GetConfig().mArrowPosition.mRect.Size());

		if(rect.PtInRect(point) &&
			FALSE == mAliashingCombo.GetDroppedState())
		{
			mAliashingCombo.ShowDropDown();
			return;
		}
	}
	{
		const CRect rect(
			theApp.GetConfig().mLanguageArrow,
			theApp.GetConfig().mArrowPosition.mRect.Size());

		if(rect.PtInRect(point) &&
			FALSE == mLanguageCombo.GetDroppedState())
		{
			mLanguageCombo.ShowDropDown();
			return;
		}
	}

	if(m_backgroundImage.IsInside(point.x, point.y))
	{
		SendMessage( WM_NCLBUTTONDOWN, HTCAPTION, 0 );
	}

	CDialog::OnLButtonDown(nFlags, point);
}

void CMainDialog::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if( m_backgroundImage.IsInside(point.x, point.y))
	{
		SendMessage( WM_NCLBUTTONUP, HTCAPTION, 0 );
	}

	CDialog::OnLButtonUp(nFlags, point);
}

void CMainDialog::OnCancel() 
{
	if( packingThread )
	{
		const CApplication::Config& config	= theApp.GetConfig();

		// ��ŷ �����尡 ���� ���� ������ �� ���� ����:
		//
		// ������ ��ŷ�ϴ� ������� Executive�� ����ϴµ�, ���� �����带 �����ϸ� �̰��� �����ȴ�.
		// �׷��� ������� Executive�� ���ŵǾ����� ������ üũ�ϴ� �� �ۿ� ����� ����. �̸� ���ؽ���
		// ũ��Ƽ�� �������� �ɾ� �ذ��� ���� ������, ��ŷ�ϴ� ���� �ܰ谡 �����Ͽ� �߰��� ������������
		// �Ϸ��� �ڵ带 ���� �����ؾ� �Ѵ�.
		MessageBox( config.m_messageBoxFailedExitDuringPacking, config.m_messageBoxCaption, MB_OK );
		return;
	}

	CDialog::OnCancel();
}

// 091221 LUJ, �ػ󵵸� �ڵ����� üũ�Ͽ� �����Ѵ�
//
//	��ó: http://www.flipcode.net/archives/DirectX8_Graphics_Wrapper.shtml
void CMainDialog::UpdateResolutionCombo()
{
	mResolutionCombo.SetImage(
		mImage);
	mResolutionCombo.ResetContent();

	LPDIRECT3D9 videoDevice = Direct3DCreate9(
		D3D_SDK_VERSION);

	typedef std::pair< UINT, UINT > Resolution;
	typedef std::set< Resolution > ResolutionSet;
	ResolutionSet resolutionSet;
	
	//���� ������� �ػ󵵸� �������� ����
	UINT	adapterIndex	=	0;

	D3DADAPTER_IDENTIFIER9 adapterIdentifier = {0};

	if(FAILED(videoDevice->GetAdapterIdentifier(
		adapterIndex,
		D3DENUM_WHQL_LEVEL,
		&adapterIdentifier)))
	{
		return;
	}

	D3DCAPS9 caps;
	ZeroMemory(
		&caps,
		sizeof(caps));

	if(FAILED(videoDevice->GetDeviceCaps(
		adapterIndex,
		D3DDEVTYPE_HAL,
		&caps)))
	{
		return;
	}

	const UINT displayModeSize = videoDevice->GetAdapterModeCount(adapterIndex,D3DFMT_X8R8G8B8);

	for(UINT modeIndex = 0; modeIndex < displayModeSize; ++modeIndex)
	{
		D3DDISPLAYMODE displayMode = {0};
		//D3DFMT_DXT1
		if(FAILED(videoDevice->EnumAdapterModes(
			adapterIndex,
			D3DFMT_X8R8G8B8,
			modeIndex,
			&displayMode)))
		{
			continue;
		}
		else if(800 > displayMode.Width)
		{
			continue;
		}
		else if(600 > displayMode.Height)
		{
			continue;
		}

		const Resolution resolution(
			displayMode.Width,
			displayMode.Height);

		if(resolutionSet.end() != resolutionSet.find(resolution))
		{
			continue;
		}

		resolutionSet.insert(
			resolution);

		TCHAR textResolution[MAX_PATH] = {0};
		_stprintf(
			textResolution,
			_T("%4d x %d"),
			resolution.first,
			resolution.second);
		mResolutionCombo.AddString(
			textResolution);
	}


	mResolutionCombo.SetPosition(
		theApp.GetConfig().mResolutionCombo);
	mResolutionCombo.SetCurSel(
		0);
	
	CString textResolutionSelected;
	textResolutionSelected.Format(
		_T("%4d x %d"),
		theApp.GetConfig().mResolution.cx,
		theApp.GetConfig().mResolution.cy);	

	for( int i = 0; i < mResolutionCombo.GetCount(); ++i)
	{
		CString textMenu;
		mResolutionCombo.GetLBText(
			i,
			textMenu);

		if(textMenu == textResolutionSelected)
		{
			mResolutionCombo.SetCurSel(i);
		}
	}

	mResolutionCombo.OnCbnSelchange();
}

void CMainDialog::OnMouseMove(UINT nFlags, CPoint point)
{
	// �ݱ� ��ư �˻�
	{
		const CRect rect(
			theApp.GetConfig().mCloseUpPosition.mPoint,
			theApp.GetConfig().mCloseUpPosition.mRect.Size());

		if(rect.PtInRect(point))
		{
			mImage.Crop(
				theApp.GetConfig().mCloseDownPosition.mRect,
				&m_closeImage);
		}
		else
		{
			mImage.Crop(
				theApp.GetConfig().mCloseUpPosition.mRect,
				&m_closeImage);
		}

		InvalidateRect(
			rect,
			FALSE);
	}
	// Ariya 07/02/2023 Discord & Facebook
	{
		const CRect rect(
			theApp.GetConfig().mDiscordButton.mPoint,
			theApp.GetConfig().mDiscordButton.mRect.Size());

		if(rect.PtInRect(point))
		{
			mImage.Crop(
				theApp.GetConfig().mDiscordButtonHover.mRect,
				&m_discordImage);
		}
		else
		{
			mImage.Crop(
				theApp.GetConfig().mDiscordButton.mRect,
				&m_discordImage);
		}

		InvalidateRect(
			rect,
			FALSE);

	}

	{
		const CRect rect(
			theApp.GetConfig().mFacebookButton.mPoint,
			theApp.GetConfig().mFacebookButton.mRect.Size());

		if(rect.PtInRect(point))
		{
			mImage.Crop(
				theApp.GetConfig().mFacebookButtonHover.mRect,
				&m_facebookImage);
		}
		else
		{
			mImage.Crop(
				theApp.GetConfig().mFacebookButton.mRect,
				&m_facebookImage);
		}

		InvalidateRect(
			rect,
			FALSE);

	}
	// ���� ��ư �˻�
	if( mIsReady )
	{
		const CRect rect(
			theApp.GetConfig().mBeginUpPosition.mPoint,
			theApp.GetConfig().mBeginUpPosition.mRect.Size());

		if(rect.PtInRect(point))
		{
			mImage.Crop(
				theApp.GetConfig().mBeginDownPosition.mRect,
				&m_beginImage);
		}
		else
		{
			mImage.Crop(
				theApp.GetConfig().mBeginUpPosition.mRect,
				&m_beginImage);
		}

		InvalidateRect(
			rect,
			FALSE);
	}

	if(mInterfaceTipImage.IsEnabled())
	{
		const CRect rect(
			theApp.GetConfig().mInterfaceCombo,
			theApp.GetConfig().mCoverCombo.mRect.Size());

		if(FALSE == rect.PtInRect(point))
		{
			mInterfaceTipImage.Enable(
				FALSE);

			const CApplication::Position& position = theApp.GetConfig().mInterfaceTip.mPosition;
			InvalidateRect(
				CRect(position.mPoint, position.mRect.Size()),
				FALSE);
		}
	}
	else if(mResolutionTipImage.IsEnabled())
	{
		const CRect rect(
			theApp.GetConfig().mResolutionCombo,
			theApp.GetConfig().mCoverCombo.mRect.Size());

		if(FALSE == rect.PtInRect(point))
		{
			mResolutionTipImage.Enable(
				false);

			const CApplication::Position& position = theApp.GetConfig().mResolutionTip.mPosition;
			InvalidateRect(
				CRect(position.mPoint, position.mRect.Size()),
				false);
		}
	}
	// Firman Akbar 10/05/2022 Custom Button Launcher
	else if(mAliashingTipImage.IsEnabled())
	{
		const CRect rect(
			theApp.GetConfig().mAliashingCombo,
			theApp.GetConfig().mCoverCombo.mRect.Size());

		if(FALSE == rect.PtInRect(point))
		{
			mAliashingTipImage.Enable(
				FALSE);

			const CApplication::Position& position = theApp.GetConfig().mAliashingTip.mPosition;
			InvalidateRect(
				CRect(position.mPoint, position.mRect.Size()),
				FALSE);
		}
	}
	else if(mLanguageTipImage.IsEnabled())
	{
		const CRect rect(
			theApp.GetConfig().mLanguageCombo,
			theApp.GetConfig().mCoverCombo.mRect.Size());

		if(FALSE == rect.PtInRect(point))
		{
			mLanguageTipImage.Enable(
				FALSE);

			const CApplication::Position& position = theApp.GetConfig().mLanguageTip.mPosition;
			InvalidateRect(
				CRect(position.mPoint, position.mRect.Size()),
				FALSE);
		}
	}
	
	TRACKMOUSEEVENT trackMouseEvent = {0};
	trackMouseEvent.cbSize = sizeof(trackMouseEvent);
	trackMouseEvent.dwFlags = TME_LEAVE;
	trackMouseEvent.hwndTrack = GetSafeHwnd();
	trackMouseEvent.dwHoverTime = 0;
	_TrackMouseEvent(
		&trackMouseEvent);

	CDialog::OnMouseMove(nFlags, point);
}

void CMainDialog::PutProgress( double ratio, bool isInstant )
{
	if(m_progress.m_value == ratio)
	{
		return;
	}
	else if( 0 >= ratio ||
		m_progress.m_value > ratio )
	{
		CxImage image;
		mImage.Crop(
			theApp.GetConfig().mProgressPosition.mRect,
			&image);

		const CRect rect(
			theApp.GetConfig().mProgressPosition.mPoint,
			theApp.GetConfig().mProgressPosition.mRect.Size());
		m_backgroundImage.Mix( 
			image,
			CxImage::OpDstCopy,
			-rect.left,
			rect.bottom - m_backgroundImage.GetHeight());
		m_progress.m_value = 0;

		InvalidateRect( 
			rect,
			FALSE);
		return;
	}

	const int offset = theApp.GetConfig().mProgressCellPosition.mRect.Height() / 2;

	LONG begin = m_progress.m_value * theApp.GetConfig().mProgressPosition.mRect.Height();
	LONG end = ratio * theApp.GetConfig().mProgressPosition.mRect.Height();

	if(begin == end)
	{
		begin = begin - offset;
		end = end + offset;
	}

	for(LONG i = begin; i < end; i = i + offset)
	{
		CRect rect;
		rect.left = theApp.GetConfig().mProgressCellPosition.mPoint.x;
		rect.top = theApp.GetConfig().mProgressPosition.mPoint.y + theApp.GetConfig().mProgressPosition.mRect.Height() - theApp.GetConfig().mProgressCellPosition.mRect.Height() - i;
		rect.right = rect.left + theApp.GetConfig().mProgressCellPosition.mRect.Width();
		rect.bottom = rect.top + theApp.GetConfig().mProgressCellPosition.mRect.Height();

		if(rect.bottom > theApp.GetConfig().mProgressPosition.mPoint.y + theApp.GetConfig().mProgressPosition.mRect.Height())
		{
			continue;
		}
		else if(rect.top < theApp.GetConfig().mProgressPosition.mPoint.y)
		{
			continue;
		}
		
		m_backgroundImage.Mix(
			m_progress.m_cellImage,
			CxImage::OpDstCopy,
			-rect.left,
			rect.bottom - m_backgroundImage.GetHeight(),
			true);

		if(isInstant)
		{
			continue;
		}

		InvalidateRect( 
			rect,
			FALSE);
	}

	m_progress.m_value = ratio;
	InvalidateRect(
		theApp.GetConfig().mProgressPosition.mRect,
		FALSE);
}

void CMainDialog::OnCbnSelchangeScreensize()
{
	mResolutionCombo.OnCbnSelchange();
	CString	textMenu;
	mResolutionCombo.GetLBText(
		mResolutionCombo.GetCurSel(),
		textMenu);

	LPCTSTR seperator = _T(" x");
	int start = 0;
	theApp.GetConfig().mResolution.cx = _ttoi(textMenu.Tokenize(
		seperator,
		start));
	theApp.GetConfig().mResolution.cy = _ttoi(textMenu.Tokenize(
		seperator,
		start));
}

// 080507 LUJ, �������̽� ����
void CMainDialog::OnCbnSelchangeInterface()
{
	mInterfaceCombo.OnCbnSelchange();

	const CString path(_T( "data\\interface\\windows\\"));

	switch( mInterfaceCombo.GetCurSel() )
	{
	case 0:
		{
			theApp.GetConfig().m_SelectedInterface = path + _T( "image_path.bin" );
			break;
		}
	case 1:
		{
			theApp.GetConfig().m_SelectedInterface = path + _T( "image_path_grey.bin" );
			break;
		}
	case 2:
		{
			theApp.GetConfig().m_SelectedInterface = path + _T( "image_path_pink.bin" );
			break;
		}
	}
}

void CMainDialog::OnCbnSelchangeAliashing()
{
	mAliashingCombo.OnCbnSelchange();

	switch( mAliashingCombo.GetCurSel() )
	{
	case 0:
		{
			theApp.GetConfig().m_SelectedAliashing = _T( "0" );
			break;
		}
	case 1:
		{
			theApp.GetConfig().m_SelectedAliashing = _T( "2" );
			break;
		}
	case 2:
		{
			theApp.GetConfig().m_SelectedAliashing = _T( "4" );
			break;
		}
	case 3:
		{
			theApp.GetConfig().m_SelectedAliashing = _T( "8" );
			break;
		}
	case 4:
		{
			theApp.GetConfig().m_SelectedAliashing = _T( "16" );
			break;
		}
	}
}

void CMainDialog::OnCbnSelchangeLanguage()
{
	mLanguageCombo.OnCbnSelchange();

	const CString path(_T( "data\\language\\"));

	switch( mLanguageCombo.GetCurSel() )
	{
	case 0:
		{
			theApp.GetConfig().m_SelectedLanguage = path + _T("Normal.bin");
			break;
		}
	case 1:
		{
			theApp.GetConfig().m_SelectedLanguage = path + _T("Chinese.bin");
			break;
		}
	}
}
// 080306 LUJ, ESCŰ�� ���� ���α׷��� �����ϰų�, ����Ű�� ���� ��ġ �߿� ������ ������ �� �־� �̸� ���´�
BOOL CMainDialog::PreTranslateMessage(MSG* pMsg)
{
	switch(pMsg->message)
	{
	case WM_KEYDOWN:
		{
			switch(pMsg->wParam)
			{
			case VK_RETURN:
			case VK_ESCAPE:
				{
					return FALSE;
				}
			}

			break;
		}
	case WM_MOUSEMOVE_COMBO:
		{
			switch(pMsg->wParam)
			{
			case IDC_SCREENSIZE:
				{
					mResolutionTipImage.Enable();

					const CApplication::Position& position = theApp.GetConfig().mResolutionTip.mPosition;
					InvalidateRect(
						CRect(position.mPoint, position.mRect.Size()),
						FALSE);
					break;
				}
			case IDC_INTERFACE:
				{
					mInterfaceTipImage.Enable();

					const CApplication::Position& position = theApp.GetConfig().mInterfaceTip.mPosition;
					InvalidateRect(
						CRect(position.mPoint, position.mRect.Size()),
						FALSE);
					break;
				}
			// Firman Akbar 10/05/2022 Custom Button Launcher
			case IDC_ALIASHING:
				{
					mAliashingTipImage.Enable();

					const CApplication::Position& position = theApp.GetConfig().mAliashingTip.mPosition;
					InvalidateRect(
						CRect(position.mPoint, position.mRect.Size()),
						FALSE);
					break;
				}
			case IDC_LANGUAGE:
				{
					mLanguageTipImage.Enable();

					const CApplication::Position& position = theApp.GetConfig().mLanguageTip.mPosition;
					InvalidateRect(
						CRect(position.mPoint, position.mRect.Size()),
						FALSE);
					break;
				}
			}

			break;
		}

	case WM_DOWNLOAD_END:
		{
			const CRect rect(
			theApp.GetConfig().mBeginUpPosition.mPoint,
			theApp.GetConfig().mBeginUpPosition.mRect.Size());

			mLoadingTipImage.Enable( 
				FALSE);

			const CApplication::Position& position = theApp.GetConfig().mLoadingTip.mPosition;
			InvalidateRect(
				CRect(position.mPoint, position.mRect.Size()),
				FALSE);

			mPackingTipImage.Enable(true);
			
		}
		break;

	case WM_PACKING_END:
		{
			const CRect rect( 
			theApp.GetConfig().mBeginUpPosition.mPoint,
			theApp.GetConfig().mBeginUpPosition.mRect.Size());

			mPackingTipImage.Enable(
				FALSE);

			const CApplication::Position& position	=	theApp.GetConfig().mPackingTip.mPosition;
			InvalidateRect(
				CRect(position.mPoint, position.mRect.Size()),
				FALSE);
		}
		break;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

// 080515 LUJ, ���� ������ üũ�Ѵ�
void CMainDialog::CheckVersionToDisk( const TCHAR* fileName )
{
	struct
	{
		void operator() ( const TCHAR* fileName, CString& version ) const
		{
			CStdioFileEx file;

			if( ! file.Open( fileName, CFile::modeRead | CFile::typeText ) )
			{
				CLog::Put( _T( "%s is not found" ), fileName );
				return;
			}

			file.ReadString( version );
		}
	}
	GetVersion;

	CString serverVersion;
	GetVersion( fileName, serverVersion );
	DeleteFile( fileName );

	CString clientVersion;
	GetVersion( _T( "LunaVerInfo.ver" ), clientVersion );

	if (clientVersion.IsEmpty())
		clientVersion = _T("0");
	
	CheckVersion( serverVersion, clientVersion );
}

// 080515 LUJ, ���� ������ üũ�Ѵ�
void CMainDialog::CheckVersionToMemory( CZipMemFile& file )
{
	CString serverVersion;
	{
		char version[ MAX_PATH ] = { 0 };
		file.Read( version, sizeof( version ) );

		if( ! strlen( version ) )
		{
			AfxMessageBox(_T("Error, Server Version is Empty!!\t E04"), MB_OK|MB_ICONSTOP);
			ExitProcess(0);
			return;
		}

		serverVersion = version;
	}
	
	CString clientVersion;
	{
		CStdioFileEx file;

		if( file.Open( _T( "LunaVerInfo.ver" ), CFile::modeRead | CFile::typeText ) )
		{
			file.ReadString( clientVersion );
		}
		// 080515 LUJ, Ŭ���̾�Ʈ�� ���� ������ ���� ��� �⺻������ �������ش�
		else
		{
			clientVersion = _T( "0" );
		}
	}

	if (clientVersion.IsEmpty())
	{
		AfxMessageBox(_T("Error, Client Version is Empty!!\tE02"), MB_OK|MB_ICONSTOP);
		ExitProcess(0);
		return;
	}
	
	if (! Validate( clientVersion ))
	{
		AfxMessageBox(_T("Client Version Error!!\tE02"), MB_OK|MB_ICONSTOP);
		ExitProcess(0);
		return;
	}
	
	CheckVersion( serverVersion, clientVersion );
}

// 081202 LUJ, ��ġ ������ üũ�Ѵ�
void CMainDialog::CheckVersion( CString serverVersion, CString clientVersion )
{
	// 080702 LUJ, ��ġ ������ �����ִ� ���� ������ ���� ������ ���� ���� ���
	//				���� ������ �о������ ���Ѵ�. �̶��� �� �̻� �����ؼ��� �ȵȴ�.

    const CApplication::Local& local = theApp.GetLocal();
    if (local.mType == CApplication::Local::TypeWtaeOffline)
    { 
        CLog::Put(_T("Offline mode: CheckVersion - starting local pack process"));
        
        CMainDialog* dialog = (CMainDialog*)(theApp.GetMainWnd());
        if (dialog)
        {
            if (!packingThread)
            {
                packingThread = AfxBeginThread(CMainDialog::Pack, 0);
            }
        }
        return;
    }

	if( serverVersion.IsEmpty() )
	{
       
            AfxMessageBox(_T("Failed to connect to server. Please check your connection and firewall settings.\tE05"), MB_OK|MB_ICONSTOP);
            ExitProcess(0);
		    return;
     
	}

	if (!Validate(serverVersion))
	{
       
            AfxMessageBox(_T("Server version error.\tE06"), MB_OK|MB_ICONSTOP);
            ExitProcess(0);
		    return;
        
	}
	
	CMainDialog* dialog = ( CMainDialog* )( theApp.GetMainWnd() );

	if( ! dialog )
	{
		ASSERT( 0 );
		return;
	}
	TmpCVer = _ttoi (clientVersion);
	TmpSVer = _ttoi (serverVersion);

	CLog::Put( _T( "Version: server is %s, client is %s" ), serverVersion, clientVersion.IsEmpty() ? clientVersion : _T( "not defined" ) );
	dialog->mPatchVersion = clientVersion;

	// 080515 LUJ, Ŭ���̾�Ʈ�� ���� ������ ���� ��� �⺻������ �������ش�
	if( clientVersion.IsEmpty() )
	{
		clientVersion = _T( "0" );
	}
	// 080515 LUJ, ������ Ŭ���̾�Ʈ�� ������ ������ ��ġ�� �ʿ䰡 ����. ��ŷ Ǯ�� ���� ����
	else if( serverVersion == clientVersion )
	{
		CLog::Put( _T( "Patch is not need" ) );

		switch( theApp.GetConfig().mFileType )
		{
		case CApplication::Config::FileTypeDisk:
			{
				CMainDialog::ExtractToDisk( clientVersion + _T( ".zip" ) );
				break;
			}
		case CApplication::Config::FileTypeMemory:
			{
				static CZipMemFile emptyFile;
				CMainDialog::ExtractToMemory( emptyFile );
				break;
			}
		default:
			{
				CLog::Put( _T( "File type is not defined" ) );
				break;
			}
		}

		return;
	}

	else if( serverVersion != clientVersion )

													
	{
		dialog->ReloadTip();
		if(TmpCVer > TmpSVer)
		{
			if (TmpCVer == TmpSVer + 1)
			{
				int TmpVerReset = _ttoi(serverVersion);
				CString NewVer;
				NewVer.Format(_T("%d"), TmpVerReset);
				dialog->mPatchVersion = NewVer;
			}
			else
			{
				dialog->mPatchVersion = "0";
			}
			SaveVersion();
			AfxMessageBox(_T("Client patch is newer than server patch. Please re-open the launcher.\tE07"), MB_OK | MB_ICONSTOP);
			ExitProcess(0);
			return;
		}

		int TmpVer = _ttoi (clientVersion);
		int IncCVer = TmpVer+1;
		CString NewVer;
		NewVer.Format (_T("%d"),IncCVer);
		dialog->mPatchVersion = NewVer;//serverVersion;
		killProcess();
		// 080515 LUJ, ��ġ ������ �ٿ�ε� ����
		{
			const CApplication::Local& local = theApp.GetLocal();
#ifdef _LOCAL_JP_
			// 090819 ONS ��ġ���� �ٿ�ε� ���¸� ���÷�ó UI�� ǥ���Ѵ�.
			dialog->SendMessageToPurple(PURPLE_OUTBOUND_INFORM, theApp.GetConfig().m_textDownload , 0);
			dialog->SendMessageToPurple(PURPLE_OUTBOUND_FILENAME, clientVersion + _T( ".zip" ), 0);	
			dialog->SendMessageToPurple(PURPLE_OUTBOUND_PATCH_FILE_NUMBER, _T(""), MAKELPARAM(1, 1));
#endif
			// 081201 LUJ, ��ġ ���� �ٿ�ε忡 ������ ��� ��ü ��ġ�� �����޵��� �Ѵ�
			theApp.Download(
				local.mDownload.mPath,
				NewVer + _T( ".zip" ),
				CMainDialog::CheckPatchToDisk,
				CMainDialog::CheckPatchToMemory,
				CMainDialog::Pursuit,
				theApp.GetConfig().mFileType );

			CLog::Put( _T( "%s is downloading..." ), local.mDownload.mPath + clientVersion + _T( ".zip" ) );
		}
	}
}

void CMainDialog::killProcess()
{
    const int maxProcIds = 1024;
    DWORD procList[maxProcIds];
    DWORD procCount;
    const TCHAR* exeName = _T("LunaClient.exe");
    TCHAR processName[MAX_PATH];

    if (!EnumProcesses(procList, sizeof(procList), &procCount))
        return;

    procCount = procCount / sizeof(DWORD);

    for (DWORD procIdx = 0; procIdx < procCount; procIdx++)
    {
        HANDLE procHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_TERMINATE, FALSE, procList[procIdx]);
        if (procHandle == NULL)
            continue;

        if (GetProcessImageFileName(procHandle, processName, MAX_PATH) > 0)
        {
            if (_tcsstr(processName, exeName) != NULL)
            {
                TerminateProcess(procHandle, 0);
            }
        }
        CloseHandle(procHandle);
    }
    Sleep(1500);
}

// 080515 LUJ, �ٿ�ε���� ��ġ ������ ������ �����Ѵ�
void CMainDialog::ExtractToMemory( CZipMemFile& file )
{
	// 080625 LUJ, libcurl.dll�� ������̶�� �����ǵ��� �Ѵ�. ���� ���Ͽ� libcurl.dll�� �ְ� �̸� 
	//				������� �� ���� ������ �� �� �����Ƿ� ���� ���� �������� ������ �߻��Ѵ�
	//theApp.ReleaseNetwork();

	CMainDialog* dialog = ( CMainDialog* )( theApp.GetMainWnd() );

	if( dialog )
	{
		dialog->SendMessageToPurple(PURPLE_OUTBOUND_INFORM, theApp.GetConfig().m_textExtract , 0);
	}
	else
	{
		ASSERT( 0 );
		return;
	}

	// 080515 LUJ, CZipArchive�� ������ �߻��ϸ� ���ܸ� �����Ƿ� ��Ȳ�� �°� ó���� ��
	try
	{
		CZipArchive archive;
		archive.Open( file, CZipArchive::zipOpen );
		killProcess();
		CLog::Put( _T( "Extracting..." ) );

		for ( int i = 0; i < archive.GetCount(); ++ i )
		{
			CZipFileHeader head;

			if( ! archive.GetFileInfo( head, i ) )
			{
				archive.Close();
				break;
			}

			SetFileAttributes( head.GetFileName(), FILE_ATTRIBUTE_NORMAL );

			if( ! archive.ExtractFile( i, 0 ) )
			{
				archive.Close();
				break;
			}

			// 090819 ONS �������� ���α׷��� ���¸� ���÷�óUI�� ǥ��
			dialog->SendMessageToPurple(PURPLE_OUTBOUND_PATCH_FILE_NUMBER, _T(""), MAKELPARAM(i+1, archive.GetCount()));
			dialog->SendMessageToPurple(PURPLE_OUTBOUND_PATCH_PROGRESS, _T(""), ((i+1)*100.0f) / archive.GetCount() );
			dialog->SendMessageToPurple(PURPLE_OUTBOUND_FILENAME, head.GetFileName(), 0);							
			dialog->PutProgress(0.5f + float( i + 1 ) / archive.GetCount() / 2.0f);
			CLog::Put( _T( "\t%s" ), head.GetFileName() );
		}

		CLog::Put( _T( "Extraction is done" ) );

		// 090720 ShinJS --- ���������� �����ϰ� �ٿ�ε��� ���������� �����Ѵ�
		//SaveVersion();

		// �Ϸ� Log ���
		MsgBoxLog( _T("Success Patch"), _T("") );

		// ������ ��� ���� ������ �����Ѵ�.
		dialog->SendMessage( WM_CLOSE );
	}
	catch ( CException* )
	{
		// 080515 LUJ,	���� ���� ���� �� CZipArchive���� ������ ���� ����. ĳġ�ؼ� �α׿� ������ ��
		//				������ ���� ��� 404 Error�� ���� HTML�� zip���Ϸ� �޴´�. �̿� ���� ����
		//				������ �����Ѵ�.
		CLog::Put( _T( "Exception catched in extraction" ) );

		Ready();
	}
}

// 080515 LUJ, �ٿ�ε���� ��ġ ������ ������ �����Ѵ�
void CMainDialog::ExtractToDisk( const TCHAR* fileName )
{
	// 080625 LUJ, libcurl.dll�� ������̶�� �����ǵ��� �Ѵ�. ���� ���Ͽ� libcurl.dll�� �ְ� �̸� 
	//				������� �� ���� ������ �� �� �����Ƿ� ���� ���� �������� ������ �߻��Ѵ�
	//theApp.ReleaseNetwork();
	CMainDialog* dialog = ( CMainDialog* )( theApp.GetMainWnd() );

	if( dialog )
	{
#ifdef _LOCAL_JP_
		dialog->SendMessageToPurple(PURPLE_OUTBOUND_INFORM, theApp.GetConfig().m_textExtract , 0);
		dialog->SendMessageToPurple(PURPLE_OUTBOUND_DOWNLOAD_SPEED, _T(""), 0);
#endif
	}
	else
	{
		ASSERT( 0 );
		return;
	}

	// 080515 LUJ, CZipArchive�� ������ �߻��ϸ� ���ܸ� �����Ƿ� ��Ȳ�� �°� ó���� ��
	try
	{
		CZipArchive archive;
		archive.Open( fileName, CZipArchive::zipOpen );
		killProcess();
		CLog::Put( _T( "%s is extracting..." ), fileName );

		for ( int i = 0; i < archive.GetCount(); ++ i )
		{
			CZipFileHeader head;

			if( ! archive.GetFileInfo( head, i ) )
			{
				archive.Close();
				break;
			}

			SetFileAttributes( head.GetFileName(), FILE_ATTRIBUTE_NORMAL );

			if( ! archive.ExtractFile( i, 0 ) )
			{
				archive.Close();
				break;
			}

			// 090819 ONS �������� ���α׷��� ���¸� ���÷�óUI�� ǥ��
#ifdef _LOCAL_JP_
			dialog->SendMessageToPurple(PURPLE_OUTBOUND_PATCH_FILE_NUMBER, _T(""), MAKELPARAM(i+1, archive.GetCount()));
			dialog->SendMessageToPurple(PURPLE_OUTBOUND_PATCH_PROGRESS, _T(""), ((i+1)*100.0f) / archive.GetCount() );
			dialog->SendMessageToPurple(PURPLE_OUTBOUND_FILENAME, head.GetFileName(), 0);							
#else
			dialog->PutProgress(0.5f + float( i + 1 ) / archive.GetCount() / 2.0f);
#endif
			CLog::Put( _T( "\t%s" ), head.GetFileName() );
		}

		CLog::Put( _T( "Extraction is done" ) );
	}
	catch ( CException* )
	{
		// 080515 LUJ,	���� ���� ���� �� CZipArchive���� ������ ���� ����. ĳġ�ؼ� �α׿� ������ ��
		//				������ ���� ��� 404 Error�� ���� HTML�� zip���Ϸ� �޴´�. �̿� ���� ����
		//				������ �����Ѵ�.
		CLog::Put( _T( "Exception catched: extracting %s" ), fileName );
	}

	DeleteFile( fileName );

#ifdef _FOR_SERVER_
	// 090720 ShinJS --- ���������� �����ϰ� �ٿ�ε��� ���������� �����Ѵ�
	//SaveVersion();

	// �Ϸ� Log ���
	MsgBoxLog( _T("Success Patch"), _T("") );

	// ������ ��� ���� ������ �����Ѵ�.
	dialog->SendMessage( WM_CLOSE );
	return;
#endif
	SaveVersion();
	if(TmpCVer != TmpSVer)
	{
		//dialog->EndDialog( IDRETRY );
		//check back server version to download new patch rather than close and reopen the launcher
		dialog->PutProgress(0);
		CString nextVersion;
		CString nextload;
		nextVersion.Format(_T("%d"),TmpCVer);
		nextload.Format(_T("%d"),TmpCVer+1);
		theApp.GetConfig().mLoadingTip.mText = theApp.GetLoadingText() + nextload;
		theApp.GetConfig().m_textVersion = theApp.GetVersionText() + nextVersion;
		dialog->ReloadText();
	}
	else
	{
		dialog->mImage.Crop(
			theApp.GetConfig().mProgressCellPackPosition.mRect,
			&(dialog->m_progress.m_cellImage));
		dialog->m_progress.m_value = 0;

		packingThread = AfxBeginThread(
			CMainDialog::Pack,
			dialog->GetSafeHwnd());
	}
}

// 080515 LUJ, ����� ǥ���Ѵ�
void CMainDialog::Pursuit( double percent, double remaintime, double down_speed )
{
	CMainDialog* dialog = ( CMainDialog* )theApp.GetMainWnd();

	if( ! dialog )
	{
		return;
	}

#ifdef _LOCAL_JP_
	// 0901819 ONS �ٿ�ε� ���¸� ���÷�óUI�� ǥ���Ѵ�.(�ʴ�1ȸ���)
	static old_remaintime = 0;
	dialog->SendMessageToPurple(PURPLE_OUTBOUND_DOWNLOAD_PROGRESS, _T(""), (int)(percent*100) );
	if(old_remaintime/1000 != (UINT)remaintime)
	{
		dialog->SendMessageToPurple(PURPLE_OUTBOUND_DOWNLOAD_RESTTIME, _T(""), (UINT)(remaintime*1000) );
		dialog->SendMessageToPurple(PURPLE_OUTBOUND_DOWNLOAD_SPEED,  _T(""), down_speed);
		old_remaintime = (UINT)(remaintime*1000);
	}
#else
		dialog->PutProgress(percent / 2.0f);
#endif
}

// 080812 LUJ, ��ġ ������ �޾Ҵ��� üũ�Ѵ�
void CMainDialog::CheckPatchToDisk( const TCHAR* fileName )
{
	// 080812 LUJ, HTTP ��Ŀ��� ���� ������ ���� ��� 404 ������ ǥ�õ� html �������� �ٿ�޴´�. �� ������
	//				������ Ǯ���� �����Ƿ�, ���ܷ� ó���Ǿ� catch���� ������ ����ȴ�
	try
	{
		CZipArchive archive;
		archive.Open( fileName, CZipArchive::zipOpen );

		CZipFileHeader head;
		if( archive.GetFileInfo( head, 0 ) )
		{
			SetFileAttributes( head.GetFileName(), FILE_ATTRIBUTE_NORMAL );

			if( archive.ExtractFile( 0, 0 ) )
			{
				archive.Close();

				CMainDialog::ExtractToDisk( fileName );
			}
		}
	}
	// 080812 LUJ, ������ Ǯ�� ���� ���� HTTP ��Ŀ��� ��ġ ������ ���� ���� �����. �⺻ ������ �ٿ�޵��� �Ѵ�
	catch ( CException* )
	{
		CLog::Put( _T( "%s is invalid to extract. Full patch tried." ), fileName );
		killProcess();
		const CApplication::Local& local = theApp.GetLocal();
		const CString defaultPatch = _T( "1.zip" );

		theApp.Download(
			local.mDownload.mPath,
			defaultPatch,
			CMainDialog::ExtractToDisk,
			CMainDialog::ExtractToMemory,
			CMainDialog::Pursuit,
			theApp.GetConfig().mFileType );

		CLog::Put( _T( "%s is downloading..." ), defaultPatch );
	}
}

// 080812 LUJ, ��ġ ������ �޾Ҵ��� üũ�Ѵ�
void CMainDialog::CheckPatchToMemory( CZipMemFile& file )
{
	// 080812 LUJ, HTTP ��Ŀ��� ���� ������ ���� ��� 404 ������ ǥ�õ� html �������� �ٿ�޴´�. �� ������
	//				������ Ǯ���� �����Ƿ�, ���ܷ� ó���Ǿ� catch���� ������ ����ȴ�
	try
	{
		CZipArchive archive;
		archive.Open( file, CZipArchive::zipOpen );

		CZipFileHeader head;
		if( archive.GetFileInfo( head, 0 ) )
		{
			SetFileAttributes( head.GetFileName(), FILE_ATTRIBUTE_NORMAL );

			if( archive.ExtractFile( 0, 0 ) )
			{
				archive.Close();

				CMainDialog::ExtractToMemory( file );
			}
		}
	}
	// 080812 LUJ, ������ Ǯ�� ���� ���� HTTP ��Ŀ��� ��ġ ������ ���� ���� �����. �⺻ ������ �ٿ�޵��� �Ѵ�
	catch ( CException* )
	{
		CLog::Put( _T( "File is invalid to extract. Full patch tried." ) );
		killProcess();
		const CApplication::Local& local = theApp.GetLocal();
		const CString defaultPatch = _T( "1.zip" );

		theApp.Download(
			local.mDownload.mPath,
			defaultPatch,
			CMainDialog::ExtractToDisk,
			CMainDialog::ExtractToMemory,
			CMainDialog::Pursuit,
			theApp.GetConfig().mFileType );

		CLog::Put( _T( "%s is downloading..." ), defaultPatch );
	}
}

// 080515 LUJ, ��ó�� ǥ�� ���¸� �����Ѵ�
void CMainDialog::Ready()
{	
	CMainDialog* dialog = ( CMainDialog* )( theApp.GetMainWnd() );

	if( ! dialog )
	{
		ASSERT( 0 );
		return;
	}

	dialog->mIsReady = true;
	dialog->m_beginImage.Enable();

#ifdef _LOCAL_JP_
	// 090819 ONS "���� ����" ���� ǥ��
	dialog->SendMessageToPurple(PURPLE_OUTBOUND_INFORM, theApp.GetConfig().m_textBegin , 0);
#endif
	dialog->mImage.Crop(
		theApp.GetConfig().mBeginUpPosition.mRect,
		&dialog->m_beginImage);
	const CRect rect(
		theApp.GetConfig().mBeginUpPosition.mPoint,
		theApp.GetConfig().mBeginUpPosition.mRect.Size());
	dialog->InvalidateRect(
		rect,
		FALSE);
	// 100219 LUJ, �޽����� ������ UpdateWindow()�� ó���ȴ�
	dialog->SendMessage(
		WM_MOUSEMOVE);

#ifdef _LOCAL_JP_
	// 090819 ONS ��ġ �������� �޽����� ������.
	dialog->SendMessageToPurple(PURPLE_OUTBOUND_ENDING, _T(""), 0);
	dialog->SendMessageToPurple(PURPLE_OUTBOUND_PATCH_PROGRESS, _T(""), 100 );
	CLog::Put( _T( "game is ready" ) );
	ExitProcess(0);
#else
	dialog->PutProgress( 1 );
#endif
	CLog::Put( _T( "game is ready" ) );
	const CApplication::Local& local = theApp.GetLocal();
	DeleteFile(local.mDownload.mVersionTag + local.mDownload.mVersionFile);
	//delete all trash 1kb zip file
	WIN32_FIND_DATA fd; 
    HANDLE hFind; 

    hFind = FindFirstFile(_T(".\\*.zip"), &fd); 
    if(hFind == INVALID_HANDLE_VALUE) 
        // No files found 
	{
        //return; 
	}
    do 
    {
        DeleteFile(fd.cFileName); 
    }
    while(FindNextFile(hFind, &fd)); 
    FindClose(hFind);
	
	theApp.ReleaseNetwork();
}

void CMainDialog::SaveVersion()
{
	CMainDialog* dialog = ( CMainDialog* )( theApp.GetMainWnd() );
	ASSERT( dialog );

	// 080515 LUJ, ������ ���Ͽ� ��������
	{
		CStdioFileEx file;

		if( file.Open( _T( "LunaVerInfo.ver" ), CFile::modeCreate | CFile::modeWrite | CFile::typeText ) )
		{
			file.WriteString( dialog->mPatchVersion );

			if(TmpCVer < TmpSVer)
				TmpCVer += 1;
			
			CLog::Put( _T( "%s is saved to LunaVerInfo.ver" ), dialog->mPatchVersion );
		}
		file.Close();
	}
}

void CMainDialog::SendMessageToPurple(int nType, LPCTSTR lpszMsg, DWORD dwValue)
{
	if(!m_hTarget)
	{
		MessageBox(theApp.GetConfig().m_messageBoxPrupleInvalidHandle);
		return;
	}

	struct PurpleCDS_Unicode
	{
		char szString[160];
		DWORD dwIndex;
	};

	PurpleCDS_Unicode pds_u;
	ZeroMemory( &pds_u, sizeof(PurpleCDS_Unicode) );
	strcpy(pds_u.szString, CString( lpszMsg ));	
	memcpy( &pds_u.dwIndex, &dwValue, sizeof(DWORD) );

	// WM_COPYDATA�� �ѱ� ����ü�� �����Ѵ�. 
	COPYDATASTRUCT cd;

	cd.dwData = nType;
	cd.cbData = sizeof(pds_u);
	cd.lpData = &pds_u;

	LRESULT r = ::SendMessage(m_hTarget, WM_COPYDATA, (WPARAM)this->m_hWnd, (LPARAM)&cd);

	if( r == 0 )
	{
		// 091222 ONS ���÷�ó���� ���Ῡ�θ� �ٽ� ���� �޼��� �ڽ��� ��µǱ⶧���� 
		// ���� �޼����ڽ��� ������� �ʰ� �ٷ� �����Ų��.
		ExitProcess(0);
	}
}


BOOL CMainDialog::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CMainDialog::PutBackground()
{
	mImage.Crop(
		theApp.GetConfig().mBackgroundPosition.mRect,
		&m_backgroundImage);
	DrawText(
		m_backgroundImage,
		theApp.GetConfig().mWindowModePosition.mRect.TopLeft(),
		theApp.GetConfig().m_textWindowMode,
		theApp.GetConfig().mWindowModeFont.mColor,
		theApp.GetConfig().mWindowModeFont.mSize,
		theApp.GetConfig().m_fontName,
		theApp.GetConfig().mWindowModeFont.mWeight,
		theApp.GetConfig().mWindowModeFont.mBorderSize,
		theApp.GetConfig().mWindowModeFont.mBorderColor);
}

void CMainDialog::PutTitle()
{
	const int offset = (theApp.GetConfig().mTitleTextPosition.mRect.Height() - theApp.GetConfig().mTitleDecoratorCellPosition.mRect.Height()) / 2;
	CxImage decoratorImage;
	mImage.Crop(
		theApp.GetConfig().mTitleDecoratorCellPosition.mRect,
		&decoratorImage);
	m_backgroundImage.Mix( 
		decoratorImage,
		CxImage::OpDstCopy,
		-(theApp.GetConfig().mTitleTextPosition.mRect.left - theApp.GetConfig().mTitleDecoratorCellPosition.mRect.Width()),
		theApp.GetConfig().mTitleTextPosition.mRect.top - m_backgroundImage.GetHeight() + offset);
	m_backgroundImage.Mix(
		decoratorImage,
		CxImage::OpDstCopy,
		-int(theApp.GetConfig().mTitleTextPosition.mRect.right),
		theApp.GetConfig().mTitleTextPosition.mRect.top - m_backgroundImage.GetHeight() + offset);

	DrawText(
		m_backgroundImage,
		theApp.GetConfig().mTitleTextPosition.mPoint,
		theApp.GetConfig().m_textTitle,
		theApp.GetConfig().mTitleFont.mBorderColor,
		theApp.GetConfig().mTitleFont.mSize,
		theApp.GetConfig().m_fontName,
		theApp.GetConfig().mTitleFont.mWeight,
		theApp.GetConfig().mTitleFont.mBorderSize,
		theApp.GetConfig().mTitleFont.mBorderColor);
	DrawText(
		m_backgroundImage,
		theApp.GetConfig().mTitleTextPosition.mPoint,
		theApp.GetConfig().m_textTitle,
		theApp.GetConfig().mTitleFont.mColor,
		theApp.GetConfig().mTitleFont.mSize,
		theApp.GetConfig().m_fontName,
		theApp.GetConfig().mTitleFont.mWeight);
	const int offsetx = (theApp.GetConfig().mVersionTextPosition.mRect.Height() - theApp.GetConfig().mTitleDecoratorCellPosition.mRect.Height()) / 2;
	CxImage decoratorImagex;
	mImage.Crop(
		theApp.GetConfig().mTitleDecoratorCellPosition.mRect,
		&decoratorImagex);
	m_backgroundImage.Mix(
		decoratorImagex,
		CxImage::OpDstCopy,
		-(theApp.GetConfig().mVersionTextPosition.mRect.left - theApp.GetConfig().mTitleDecoratorCellPosition.mRect.Width()),
		theApp.GetConfig().mVersionTextPosition.mRect.top - m_backgroundImage.GetHeight() + offsetx);
	m_backgroundImage.Mix(
		decoratorImagex,
		CxImage::OpDstCopy,
		-int(theApp.GetConfig().mVersionTextPosition.mRect.right),
		theApp.GetConfig().mVersionTextPosition.mRect.top - m_backgroundImage.GetHeight() + offsetx);

	DrawText(
		m_backgroundImage,
		theApp.GetConfig().mVersionTextPosition.mPoint,
		theApp.GetConfig().m_textVersion,
		theApp.GetConfig().mTitleFont.mBorderColor,
		theApp.GetConfig().mTitleFont.mSize,
		theApp.GetConfig().m_fontName,
		theApp.GetConfig().mTitleFont.mWeight,
		theApp.GetConfig().mTitleFont.mBorderSize,
		theApp.GetConfig().mTitleFont.mBorderColor);
	DrawText(
		m_backgroundImage,
		theApp.GetConfig().mVersionTextPosition.mPoint,
		theApp.GetConfig().m_textVersion,
		theApp.GetConfig().mTitleFont.mColor,
		theApp.GetConfig().mTitleFont.mSize,
		theApp.GetConfig().m_fontName,
		theApp.GetConfig().mTitleFont.mWeight); 
}

void CMainDialog::PutTip()
{
	mImage.Crop(
		theApp.GetConfig().mInterfaceTip.mPosition.mRect,
		&mInterfaceTipImage);
	DrawText(
		mInterfaceTipImage,
		theApp.GetConfig().mResolutionTip.mTextOffset,
		theApp.GetConfig().mResolutionTip.mText,
		theApp.GetConfig().mResolutionTip.mFont.mColor,
		theApp.GetConfig().mResolutionTip.mFont.mSize,
		theApp.GetConfig().m_fontName,
		theApp.GetConfig().mResolutionTip.mFont.mWeight,
		theApp.GetConfig().mResolutionTip.mFont.mBorderSize,
		theApp.GetConfig().mResolutionTip.mFont.mBorderColor);
	mInterfaceTipImage.Enable(
		false);

	mImage.Crop(
		theApp.GetConfig().mResolutionTip.mPosition.mRect,
		&mResolutionTipImage);
	DrawText(
		mResolutionTipImage,
		theApp.GetConfig().mInterfaceTip.mTextOffset,
		theApp.GetConfig().mInterfaceTip.mText,
		theApp.GetConfig().mInterfaceTip.mFont.mColor,
		theApp.GetConfig().mInterfaceTip.mFont.mSize,
		theApp.GetConfig().m_fontName,
		theApp.GetConfig().mInterfaceTip.mFont.mWeight,
		theApp.GetConfig().mInterfaceTip.mFont.mBorderSize,
		theApp.GetConfig().mInterfaceTip.mFont.mBorderColor);
	mResolutionTipImage.Enable(
		false);

	mImage.Crop( 
		theApp.GetConfig().mLoadingTip.mPosition.mRect,
		&mLoadingTipImage);
		
	if(TmpCVer == TmpSVer)
		theApp.GetConfig().mLoadingTip.mText = "Checking for New Patch";
	
	DrawText( 
		mLoadingTipImage,
		theApp.GetConfig().mLoadingTip.mTextOffset,
		theApp.GetConfig().mLoadingTip.mText,
		theApp.GetConfig().mLoadingTip.mFont.mColor,
		theApp.GetConfig().mLoadingTip.mFont.mSize,
		theApp.GetConfig().m_fontName,
		theApp.GetConfig().mLoadingTip.mFont.mWeight,
		theApp.GetConfig().mLoadingTip.mFont.mBorderSize,
		theApp.GetConfig().mLoadingTip.mFont.mBorderColor );
	mLoadingTipImage.Enable();

	mImage.Crop(
		theApp.GetConfig().mPackingTip.mPosition.mRect,
		&mPackingTipImage);
	DrawText(
		mPackingTipImage,
		theApp.GetConfig().mPackingTip.mTextOffset,
		theApp.GetConfig().mPackingTip.mText,
		theApp.GetConfig().mPackingTip.mFont.mColor,
		theApp.GetConfig().mPackingTip.mFont.mSize,
		theApp.GetConfig().m_fontName,
		theApp.GetConfig().mPackingTip.mFont.mWeight,
		theApp.GetConfig().mPackingTip.mFont.mBorderSize,
		theApp.GetConfig().mPackingTip.mFont.mBorderColor );
	mPackingTipImage.Enable(false);
	
	// Firman Akbar 10/05/2022 Custom Button Launcher
	mImage.Crop(
		theApp.GetConfig().mAliashingTip.mPosition.mRect,
		&mAliashingTipImage);
	DrawText(
		mAliashingTipImage,
		theApp.GetConfig().mAliashingTip.mTextOffset,
		theApp.GetConfig().mAliashingTip.mText,
		theApp.GetConfig().mAliashingTip.mFont.mColor,
		theApp.GetConfig().mAliashingTip.mFont.mSize,
		theApp.GetConfig().m_fontName,
		theApp.GetConfig().mAliashingTip.mFont.mWeight,
		theApp.GetConfig().mAliashingTip.mFont.mBorderSize,
		theApp.GetConfig().mAliashingTip.mFont.mBorderColor);
	mAliashingTipImage.Enable(
		false);
		
	mImage.Crop(
		theApp.GetConfig().mLanguageTip.mPosition.mRect,
		&mLanguageTipImage);
	DrawText(
		mLanguageTipImage,
		theApp.GetConfig().mLanguageTip.mTextOffset,
		theApp.GetConfig().mLanguageTip.mText,
		theApp.GetConfig().mLanguageTip.mFont.mColor,
		theApp.GetConfig().mLanguageTip.mFont.mSize,
		theApp.GetConfig().m_fontName,
		theApp.GetConfig().mLanguageTip.mFont.mWeight,
		theApp.GetConfig().mLanguageTip.mFont.mBorderSize,
		theApp.GetConfig().mLanguageTip.mFont.mBorderColor);
	mLanguageTipImage.Enable(
		false);
}

void CMainDialog::ReloadText()
{
	const CRect rect(
	theApp.GetConfig().mBeginUpPosition.mPoint,
	theApp.GetConfig().mBeginUpPosition.mRect.Size());

	mLoadingTipImage.Enable( 
		FALSE);

	const CApplication::Position& position = theApp.GetConfig().mLoadingTip.mPosition;
	InvalidateRect(
		CRect(position.mPoint, position.mRect.Size()),
		FALSE);
	//copy from init
	mImage.Load(
		theApp.GetConfig().mImage,
		CXIMAGE_FORMAT_TIF);

	PutBackground();
	PutTitle();
	PutTip();

	// üũ �ڽ�
	{
		CxImage image;
		mImage.Crop(
			theApp.GetConfig().mUncheckedPosition.mRect,
			&image);
		m_backgroundImage.Mix(
			image,
			CxImage::OpDstCopy,
			-theApp.GetConfig().mUncheckedPosition.mPoint.x, 
			theApp.GetConfig().mUncheckedPosition.mPoint.y + theApp.GetConfig().mUncheckedPosition.mRect.Height() - m_backgroundImage.GetHeight(),
			true);

		mImage.Crop(
			theApp.GetConfig().mCheckedPosition.mRect,
			&m_checkBoxImage);
		m_checkBoxImage.Enable(
			theApp.GetConfig().m_isWindowMode);
	}

#ifndef _FOR_SERVER_
	SetWindowText(
		theApp.GetConfig().m_textCaption);

	InvalidateRect(
		0,
		FALSE);
	UpdateWindow();
#endif

	{
		const CApplication::Local& local = theApp.GetLocal();
		theApp.Download(
			local.mDownload.mPath,
			local.mDownload.mVersionTag + local.mDownload.mVersionFile,
			CMainDialog::CheckVersionToDisk,
			CMainDialog::CheckVersionToMemory,
			0,
			theApp.GetConfig().mFileType );

		CLog::Put( _T( "%s is downloading..." ), local.mDownload.mPath + local.mDownload.mVersionTag + local.mDownload.mVersionFile );
	}
}

void CMainDialog::ReloadTextAfterPack()
{
	//copy from init
	mImage.Load(
		theApp.GetConfig().mImage,
		CXIMAGE_FORMAT_TIF);

	PutBackground();

	PutTitle();
	// üũ �ڽ�
	{
		CxImage image;
		mImage.Crop(
			theApp.GetConfig().mUncheckedPosition.mRect,
			&image);
		m_backgroundImage.Mix(
			image,
			CxImage::OpDstCopy,
			-theApp.GetConfig().mUncheckedPosition.mPoint.x, 
			theApp.GetConfig().mUncheckedPosition.mPoint.y + theApp.GetConfig().mUncheckedPosition.mRect.Height() - m_backgroundImage.GetHeight(),
			true);

		mImage.Crop(
			theApp.GetConfig().mCheckedPosition.mRect,
			&m_checkBoxImage);
		m_checkBoxImage.Enable(
			theApp.GetConfig().m_isWindowMode);
	}

#ifndef _FOR_SERVER_
	SetWindowText(
		theApp.GetConfig().m_textCaption);

	InvalidateRect(
		0,
		FALSE);
	UpdateWindow();
#endif

}

void CMainDialog::ReloadTip()
{
	const CRect rect(
	theApp.GetConfig().mBeginUpPosition.mPoint,
	theApp.GetConfig().mBeginUpPosition.mRect.Size());

	mLoadingTipImage.Enable( 
		FALSE);

	const CApplication::Position& position = theApp.GetConfig().mLoadingTip.mPosition;
	InvalidateRect(
		CRect(position.mPoint, position.mRect.Size()),
		FALSE);
	//copy from init
	mImage.Load(
		theApp.GetConfig().mImage,
		CXIMAGE_FORMAT_TIF);
	//after reload
	CString nextload;
	nextload.Format(_T("%d"),TmpCVer+1);
	theApp.GetConfig().mLoadingTip.mText = theApp.GetLoadingText() + nextload;

	PutBackground();
	PutTitle();
	PutTip();

	// üũ �ڽ�
	{
		CxImage image;
		mImage.Crop(
			theApp.GetConfig().mUncheckedPosition.mRect,
			&image);
		m_backgroundImage.Mix(
			image,
			CxImage::OpDstCopy,
			-theApp.GetConfig().mUncheckedPosition.mPoint.x, 
			theApp.GetConfig().mUncheckedPosition.mPoint.y + theApp.GetConfig().mUncheckedPosition.mRect.Height() - m_backgroundImage.GetHeight(),
			true);

		mImage.Crop(
			theApp.GetConfig().mCheckedPosition.mRect,
			&m_checkBoxImage);
		m_checkBoxImage.Enable(
			theApp.GetConfig().m_isWindowMode);
	}

#ifndef _FOR_SERVER_
	SetWindowText(
		theApp.GetConfig().m_textCaption);

	InvalidateRect(
		0,
		FALSE);
	UpdateWindow();
#endif

}
														   
void CMainDialog::UpdateInterfaceCombo()
{
	mInterfaceCombo.ResetContent();
	mInterfaceCombo.SetImage(
		mImage);

	for(CApplication::Config::TextList::iterator iterator = theApp.GetConfig().mInterfaceTextList.begin();
		theApp.GetConfig().mInterfaceTextList.end() != iterator;
		++iterator)
	{
		mInterfaceCombo.AddString(
			*iterator);
	}

	mInterfaceCombo.SetPosition(
		theApp.GetConfig().mInterfaceCombo);
	mInterfaceCombo.SetCurSel(
		1);

	for( int i = 0; i < mInterfaceCombo.GetCount(); ++i )
	{
		const CString path( _T( "data\\interface\\windows\\" ) );
		const CString blueInterface( path + _T( "image_path.bin" ) );
		const CString pinkInterface( path + _T( "image_path_pink.bin" ) );
		const CString grayInterface( path + _T( "image_path_grey.bin" ) );

		if( blueInterface == theApp.GetConfig().m_SelectedInterface )
		{
			mInterfaceCombo.SetCurSel(
				0);
		}
		else if( grayInterface == theApp.GetConfig().m_SelectedInterface )
		{
			mInterfaceCombo.SetCurSel(
				1);
		}
		else if( pinkInterface == theApp.GetConfig().m_SelectedInterface )
		{
			mInterfaceCombo.SetCurSel(
				2);
		}
	}

	mInterfaceCombo.OnCbnSelchange();
}

void CMainDialog::UpdateAliashingCombo()
{
	mAliashingCombo.ResetContent();
	mAliashingCombo.SetImage(
		mImage);

	for(CApplication::Config::TextList::iterator iterator = theApp.GetConfig().mAliashingTextList.begin();
		theApp.GetConfig().mAliashingTextList.end() != iterator;
		++iterator)
	{
		mAliashingCombo.AddString(
			*iterator);
	}

	mAliashingCombo.SetPosition(
		theApp.GetConfig().mAliashingCombo);
	mAliashingCombo.SetCurSel(
		1);

	for( int i = 0; i < mAliashingCombo.GetCount(); ++i )
	{
		const CString NoneAliashing( _T( "0" ) );
		const CString x2Aliashing( _T( "2" ) );
		const CString x4Aliashing( _T( "4" ) );
		const CString x8Aliashing( _T( "8" ) );
		const CString x16Aliashing( _T( "16" ) );

		if( NoneAliashing == theApp.GetConfig().m_SelectedAliashing )
		{
			mAliashingCombo.SetCurSel(0);
		}
		else if( x2Aliashing == theApp.GetConfig().m_SelectedAliashing )
		{
			mAliashingCombo.SetCurSel(1);
		}
		else if( x4Aliashing == theApp.GetConfig().m_SelectedAliashing )
		{
			mAliashingCombo.SetCurSel(2);
		}
		else if( x8Aliashing == theApp.GetConfig().m_SelectedAliashing )
		{
			mAliashingCombo.SetCurSel(3);
		}
		else if( x16Aliashing == theApp.GetConfig().m_SelectedAliashing )
		{
			mAliashingCombo.SetCurSel(4);
		}
	}

	mAliashingCombo.OnCbnSelchange();
}

void CMainDialog::UpdateLanguageCombo()
{
	mLanguageCombo.ResetContent();
	mLanguageCombo.SetImage(
		mImage);

	for(CApplication::Config::TextList::iterator iterator = theApp.GetConfig().mLanguageTextList.begin();
		theApp.GetConfig().mLanguageTextList.end() != iterator;
		++iterator)
	{
		mLanguageCombo.AddString(
			*iterator);
	}

	mLanguageCombo.SetPosition(
		theApp.GetConfig().mLanguageCombo);
	mLanguageCombo.SetCurSel(
		1);

	for( int i = 0; i < mLanguageCombo.GetCount(); ++i )
	{
		const CString path( _T( "data\\language\\" ) );
		const CString Original( path + _T( "Normal.bin" ) );
		const CString Chinese( path + _T( "Chinese.bin" ) );

		if( Original == theApp.GetConfig().m_SelectedLanguage )
		{
			mLanguageCombo.SetCurSel(
				0);
		}
		else if( Chinese == theApp.GetConfig().m_SelectedLanguage )
		{
			mLanguageCombo.SetCurSel(
				1);
		}
	}

	mLanguageCombo.OnCbnSelchange();
}

LRESULT CMainDialog::OnMouseLeave(WPARAM, LPARAM)
{
	if(FALSE == mIsReady)
	{
		return TRUE;
	}

	mImage.Crop(
		theApp.GetConfig().mBeginUpPosition.mRect,
		&m_beginImage);
	InvalidateRect(
		CRect(theApp.GetConfig().mBeginUpPosition.mPoint, theApp.GetConfig().mBeginUpPosition.mRect.Size()),
		FALSE);

	return TRUE;
}

bool CMainDialog::Validate(CString str)
{
	if(str.SpanIncluding(_T("0123456789")) == str)
		return 1;
	else
		return 0;
}