#include "stdafx.h"
#include "MainGame.h"
#include "WindowIDEnum.h"
#include "./Interface/cWindowManager.h"
#include "./Interface/cEditBox.h"
#include "./Interface/cTextArea.h"
#include "./Interface/cFont.h"
#include "cMsgBox.h"
#include "./Input/UserInput.h"
#include "./Input/cIMEWnd.h"
#include "ChatManager.h"
#include "../[CC]Header/GameResourceManager.h"
#include "MouseCursor.h"
#include "mhFile.h"
#include "OptionManager.h"
#include "MHAudioManager.h"
#include "ObjectManager.h"
#include "MHMap.h"
#include "ExitManager.h"
#include "QuestManager.h"
#include "ServerConnect.h"
#include "cResourceManager.h"
#include "../hseos/Farm/SHFarmManageDlg.h"
#include <atlbase.h>
#include <atlcom.h>
#include <atlhost.h>
#include <exdispid.h>
#include "MainTitle.h"

#ifdef _TESTCLIENT_
	#include "TSToolManager.h"
	#include "CommCtrl.h"
#endif

#ifdef _GMTOOL_
	#include "GMToolManager.h"
	#include "CommCtrl.h"
#endif

#ifdef TAIWAN_LOCAL
	#include <shellapi.h>
#endif

#ifdef _HACK_SHIELD_
	#include "HShield.h"
	#include "HackShieldManager.h"
#endif
#ifdef _NPROTECT_
// nProtect
//#define NO_GAMEGUARD
	#include "NPGameLib.h"
	#include "NProtectManager.h"
#endif

#ifdef _ERROR_REPORT_

#include "./ErrorReport/include/IErrorReportLib.h"

#ifdef _DEBUG
	#ifdef _WIN64
		#pragma comment( lib, "./ErrorReport/lib/ErrorReportLib_x64d.lib")
	#else
		#pragma comment( lib, "./ErrorReport/lib/ErrorReportLib_x86d.lib")
	#endif
#else
	#ifdef _WIN64
		#pragma comment( lib, "./ErrorReport/lib/ErrorReportLib_x64.lib")
	#else
		#pragma comment( lib, "./ErrorReport/lib/ErrorReportLib_x86.lib")
	#endif
#endif

#endif

#define _LOGOWINDOW_

//////////////////////////////////////////////////////////////////////////
// Version ����
// �� ���ú� Ŭ���̾�Ʈ�� ������ �����ش�

/////////////////////////////////////////////////////////////////////////

char g_CLIENTVERSION[32] = "LUNA2025";


/////////////////////////////////////////////////////////////////////////	

#define MAX_LOADSTRING 100

// Global Variables:
HANDLE g_hMap = NULL;
HINSTANCE g_hInst;								// current instance
HWND _g_hWnd;
char szTitle[MAX_LOADSTRING];								// The title bar text
char szWindowClass[MAX_LOADSTRING];								// The title bar text
BOOL g_bActiveApp = FALSE;
char g_AgentAddr[16];
WORD g_AgentPort;
char g_PATCHVERSION[256];

#ifdef _TW_LOCAL_
char g_szHeroIDName[21];
char g_szHeroKey[21];
#else
char g_szHeroIDName[MAX_NAME_LENGTH+1];
#endif

int	g_nServerSetNum;
ScriptCheckValue g_Check;

HIMC g_hIMC = NULL;


CComModule _Module; 
BEGIN_OBJECT_MAP(ObjectMap) 
END_OBJECT_MAP() 

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
void LeaveClientApplication();
BOOL EnterClientApplication();

//BOOL CheckParameter(LPSTR parameter);
BOOL	GetPatchVersion();
BOOL	CheckResourceFile();
void	ChangeVerInfo();
BOOL	SaveFileList();

#ifdef _HACK_SHIELD_
BOOL HS_Init();
BOOL HS_StartService();
BOOL HS_StopService();
BOOL HS_UnInit();
int __stdcall HS_CallbackProc (long lCode, long lParamSize, void* pParam );
BOOL HS_PauseService();
BOOL HS_ResumeService();
void HS_SaveFuncAddress();

LONG WINAPI TopLevelFilter(struct _EXCEPTION_POINTERS* pExPtr)
{
	HS_StopService();
	HS_UnInit();
	return EXCEPTION_EXECUTE_HANDLER;
}

#endif

#ifdef _NPROTECT_

BOOL CALLBACK NPGameMonCallback(DWORD dwMsg, DWORD dwArg);

LONG WINAPI TopLevelFilter(struct _EXCEPTION_POINTERS* pExPtr)
{
	NPROTECTMGR->Release();
	return EXCEPTION_EXECUTE_HANDLER;
}

#endif


void ClearDirectory( LPCTSTR pStrName )
{
	WIN32_FIND_DATA FindFileData;
	char buf[MAX_PATH];
	wsprintf( buf, "%s\\*", pStrName );

	HANDLE hFind = FindFirstFile( buf, &FindFileData);

	if( hFind == INVALID_HANDLE_VALUE ) return;

	while( 1 )
	{
		if( FindFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY )
		{
			if( strcmp( FindFileData.cFileName, "." ) != 0 &&
				strcmp( FindFileData.cFileName, ".." ) != 0 )
			{
				wsprintf( buf, "%s\\%s", pStrName, FindFileData.cFileName );
				ClearDirectory( buf );
				RemoveDirectory( buf );
			}
		}
		else
		{
			wsprintf( buf, "%s\\%s", pStrName, FindFileData.cFileName );
			DeleteFile( buf );
		}
		if( FindNextFile( hFind, &FindFileData ) == FALSE )
			break;
	}
	FindClose( hFind );	
}


int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	GUID guid; 
	AtlAxWinInit(); 
	_Module.Init(ObjectMap, hInstance, &guid); 

#ifdef _TW_LOCAL_
	sscanf( lpCmdLine, "%s %s", g_szHeroIDName, g_szHeroKey );
#endif

	SetScriptCheckValue( g_Check );

	char buf[MAX_PATH]={0,};
	char commandToken[MAX_PATH]={0,};
	const char* delimit = " ";
	strncpy( buf, lpCmdLine, sizeof(buf) );
	char* token = strtok( buf, delimit );
	if( token )
		strncpy( commandToken, token, sizeof(commandToken) );

#ifdef _KOR_WEBLAUNCHER_

#ifdef _GMTOOL_
	if( lpCmdLine )
	{
		FILE* fpLog = NULL;
		fpLog = fopen( "./ClientStartLog.txt", "a+" );
		if( fpLog )
		{
			SYSTEMTIME sysTime;
			GetLocalTime( &sysTime );

			fprintf( fpLog, "[%04d-%02d-%02d %02d:%02d:%02d] Execute Command Line : %s\n", 
				sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond,
				lpCmdLine );

			fclose( fpLog );
		}	
	}
#endif

	token = strtok( 0, delimit );
	if( token == NULL )
	{
		// Ȩ������
		ShellExecute( 0, "open", "explorer", "http://lunaplus.enpang.com/", 0, SW_SHOWNORMAL );
		return 0;
	}

	TITLE->SetLoginKey(token);
#endif

	FILE* fp = NULL;

#ifndef _TW_LOCAL_
#ifndef _GMTOOL_
	if( strcmp( commandToken, " luna2025" ) != 0)
	{
		MessageBox( NULL, "You should not execute directly this file!!", "Error!!", MB_OK );
		return 0;
	}

#endif
#endif
	if(!EnterClientApplication()) return 0;


#ifdef _HACK_SHIELD_

	DIRECTORYMGR->Init();

	if( !HS_Init() )
	{
		HS_UnInit();
		MessageBox( NULL, "HackShield Init Failed.", "ERROR", MB_OK );	
		return 0;
	}
	if( !HS_StartService() )
	{
		HS_StopService();
		HS_UnInit();
		MessageBox( NULL, "HackShield Init Failed.", "ERROR", MB_OK );
		return 0;
	}
	HS_SaveFuncAddress();

	::SetUnhandledExceptionFilter(TopLevelFilter);

#endif

	DeleteFile("Debug.txt");
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtMemDumpAllObjectsSince(0);
	
	fp = fopen("_DONOTUSEENGINEPACK","r");
	if(fp)
	{
		g_bUsingEnginePack = FALSE;
		fclose(fp);
	}
 
	// Initialize global strings
	GetPatchVersion();

    
	GAMERESRCMNGR->LoadGameDesc();

#ifdef _KOR_
#ifdef _TESTCLIENT_
	sprintf(szTitle,"�糪�÷���(TEST) %s %s ExeVer:%s PatchVer:%s",__DATE__,__TIME__,g_CLIENTVERSION,g_PATCHVERSION);
	sprintf(szWindowClass,"�糪�÷���(TEST) %s %s ExeVer:%s PatchVer:%s",__DATE__,__TIME__, g_CLIENTVERSION,g_PATCHVERSION);
#elif _GMTOOL_
	sprintf(szTitle,"�糪�÷���[GM_TOOL] %s %s ExeVer:%s PatchVer:%s",__DATE__,__TIME__,g_CLIENTVERSION,g_PATCHVERSION);
	sprintf(szWindowClass,"�糪�÷���[GM_TOOL] %s %s ExeVer:%s PatchVer:%s",__DATE__,__TIME__, g_CLIENTVERSION,g_PATCHVERSION);
#else
	sprintf(szTitle,"�糪�÷���");
	sprintf(szWindowClass,"�糪�÷���");
#endif
#else
#ifdef _TESTCLIENT_
	sprintf(szTitle,"LUNA Online(TEST) %s %s ExeVer:%s PatchVer:%s",__DATE__,__TIME__,g_CLIENTVERSION,g_PATCHVERSION);
	sprintf(szWindowClass,"LUNA Online(TEST) %s %s ExeVer:%s PatchVer:%s",__DATE__,__TIME__, g_CLIENTVERSION,g_PATCHVERSION);
#elif _GMTOOL_
	sprintf(szTitle,"LUNA Online[GM_TOOL] %s %s ExeVer:%s PatchVer:%s",__DATE__,__TIME__,g_CLIENTVERSION,g_PATCHVERSION);
	sprintf(szWindowClass,"LUNA Online[GM_TOOL] %s %s ExeVer:%s PatchVer:%s",__DATE__,__TIME__, g_CLIENTVERSION,g_PATCHVERSION);
#else
	sprintf(szTitle,"LUNA Online");
	sprintf(szWindowClass,"LUNA Online");
#endif
#endif
	MyRegisterClass(hInstance);
	//LOGEX(lpCmdLine, PT_MESSAGEBOX);


	GAMERESRCMNGR->LoadDeveloperInfo();
	// quest data load
	QUESTMGR->LoadQuestString( "./system/Resource/QuestString.bin" );
	QUESTMGR->LoadQuestScript( "./system/Resource/QuestScript.bin" );
	//QUESTMGR->LoadNewbieGuide( "./system/Resource/NewbieGuide.bin" );
	QUESTMGR->LoadQuestNpc( "./system/Resource/QuestNpcList.bin" );

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		LeaveClientApplication();
		return FALSE;
	}


#ifdef _ERROR_REPORT_
	// �䷸�� �ɾ�����ø� �˴ϴ�.
	INIT_DUMP_HANDLER init;
	memset( &init, 0, sizeof(init));
	init.iBuildNum	=	1;
	init.dumpType	=	MiniDumpNormal;
	strcpy( init.szFtpServerIP, "218.234.20.167");				// ���⿡ ftp ���� ip �ִ´�.
	init.iFtpServerPort	=	21;
	strcpy( init.szUserName, "lunasrv_ftp");
	strcpy( init.szPass, "fnskeksehrftp~!");
	init.hApplicationInstance = hInstance;
	init.bUseDescriptionDialog = TRUE;

	InitErrorDumpHandler(NULL);		// szFtpServerIP == NULL �� ��� dump �� ����.

	SetUnhandledExceptionFilter( ErrorDumpHandler);
#endif

//	HACCEL hAccelTable;
//	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_TEST);

	// Main message loop:
	MSG msg;
	while (1) //GetMessage(&msg, NULL, NULL, NULL));
    {
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
//			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
//			{
#ifdef _GMTOOL_
			if( !GMTOOLMGR->IsGMDialogMessage(&msg) )
#elif _TESTCLIENT_
			if( !TSTOOLMGR->IsTSDialogMessage(&msg) )
#endif
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg); 
			}
//			}
		}
		else
		{
			if(MAINGAME->Process() == -1)
				break;
		}
	}

//	DeleteFile("recv.txt");

	LeaveClientApplication();

//	_ASSERTE( _CrtCheckMemory( ) );

#ifdef _HACK_SHIELD_
	HS_StopService();
	HS_UnInit();
#endif
#ifdef _NPROTECT_	
	NPROTECTMGR->Release();
#endif
	return msg.wParam;
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 
#ifndef _GMTOOL_
	wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_NOCLOSE;
#else
	wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
#endif
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
//	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_TEST);
//	wcex.hIcon			= NULL;
	wcex.hIcon			= LoadIcon( hInstance, MAKEINTRESOURCE(IDI_GAMEICON) );

	wcex.hCursor		= NULL;
//	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
//	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);
	wcex.hIconSm		= NULL;

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	//if(!EnterClientApplication()) return FALSE;	//A��I�Ϩ�=8�ˢ�ˢ�I ��E?A�ˢ硧��a��E��IU. //KES

	CURSOR->Init( hInstance );
	
	HWND hWnd;
	g_hInst = hInstance; // Store instance handle in our global variable
//	int x = GetSystemMetrics(SM_CXSIZEFRAME)*2;
	int x = GetSystemMetrics(SM_CXFIXEDFRAME)*2;
//	int y = GetSystemMetrics(SM_CYSIZEFRAME)*2+GetSystemMetrics(SM_CYCAPTION);
	int y = GetSystemMetrics(SM_CXFIXEDFRAME)*2+GetSystemMetrics(SM_CYCAPTION);

#ifndef _GMTOOL_
	DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_CLIPCHILDREN |
		WS_SYSMENU;
#else
	DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_CLIPCHILDREN |
		WS_SYSMENU | WS_MINIMIZEBOX;
#endif
	// 070201 LYW --- Modified window style and whidth height.
	//_g_hWnd = hWnd = CreateWindow(szWindowClass, szTitle, dwStyle, //WS_OVERLAPPEDWINDOW,
		//CW_USEDEFAULT, 0, 1280+x, 1024+y, NULL, NULL, hInstance, NULL);	//WINDOW SIZE

#ifdef _TESTCLIENT_
	dwStyle |= WS_THICKFRAME;
#endif
	DWORD width		= 0 ;
	DWORD height	= 0 ;

	const DISPLAY_INFO& dispInfo = GAMERESRCMNGR->GetResolution();
	width	= dispInfo.dwWidth ;
	height	= dispInfo.dwHeight ;

	// desc_hseos_�ػ�01
	// S �ػ� �߰� added by hseos 2007.06.25
	// ..������ ��忡�� ������� ���÷��� �ػ󵵿� ���� �ػ󵵰� ������ �������� ����������, �Ʒ����� 
	// ..�߸��� ������ �ִ�. �������� ���������� ������ ���� x = GetSystemMetrics(SM_CXFIXEDFRAME)*2;
	// ..���� ������ �������� x �� �����ֱ� �����̴�. (�¿� �������� �����ϱ� ����)
	// ..������ ��忡�� ������� ���÷��� �ػ�x�� ���� �ػ�x�� ������ �������� �������� �ʱ�� �Ѵ�.
	// ..(�ٵ�, �ٸ� �ػ󵵿����� x = 0 ���� �ص� ���� ���̰� ����.....����??? ������ ������ ���� �޶����� �� ���⵵..)
	// ..(��·�� ���� �ػ󵵿��� ������ �ǹǷ� x �� 0..)
	// ..�Ʒ����� �߸��� ������ �� �𸣰���..
	DWORD nUserScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	if (nUserScreenWidth == width) x = 0;
	// E �ػ� �߰� added by hseos 2007.06.25

	_g_hWnd = hWnd = CreateWindow(szWindowClass, szTitle, dwStyle, //WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, width+x, height+y, NULL, NULL, hInstance, NULL);	//WINDOW SIZE
	
	if (!hWnd)
	{
		return FALSE;
	}

#ifndef _NO_NPROTECT_START
#ifdef _NPROTECT_
	if(!NPROTECTMGR->Init(_g_hWnd))
	{
		return FALSE;
	}

	::SetUnhandledExceptionFilter(TopLevelFilter);
#endif
#endif

//	gMp3 = new CBGMLoader(hInstance,hWnd);

	ShowWindow(hWnd, nCmdShow);
	
//	if( g_bWindowMode )
//		ShowWindow(hWnd, nCmdShow);
//	else
//		ShowWindow(hWnd, SW_HIDE);

	UpdateWindow(hWnd);
	ShowCursor(TRUE);

	CoInitialize(NULL);

#ifndef _LOGOWINDOW_
	PlayCutscene("logo.avi",g_hInst);
#endif

	//////////////////////////////////////////////////////////////////////////
	// MAININIT
	MAINGAME->Init(hWnd);
		
#ifndef _TESTCLIENT_
	MAINGAME->SetGameState(eGAMESTATE_CONNECT);
#else
	TSTOOLMGR->CreateTSDialog();

	int initKind = eGameInInitKind_Login;
	MAINGAME->SetGameState(eGAMESTATE_GAMEIN,&initKind,sizeof(int));
#endif                 

#ifdef _GMTOOL_
	GMTOOLMGR->CreateGMDialog();
#endif
	
	
	return TRUE;
}



LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MOUSE->GetWindowMessage(hWnd, message, wParam, lParam);
	CHATMGR->MsgProc( message, wParam ) ;
	CIMEWND->OnMsg(hWnd, message, wParam, lParam);
	
	switch (message) 
	{
		// 100209 LUJ, �Ʒ� �޽��� ó���� �����, IME �Է��� ���������� ���� �ʴ´�
	case WM_IME_CHAR:
#ifndef _TW_LOCAL_
	
	case WM_IME_STARTCOMPOSITION:
#endif
	case WM_CHAR:
	case WM_SYSKEYUP:
		return 0;
	case WM_SYSCOMMAND:
		{
			if(SC_KEYMENU == wParam)
			{
				return 0;
			}
		}
		break;
	case WM_ACTIVATEAPP:
		{ 
			g_bActiveApp = (BOOL)wParam;

			if( !g_bActiveApp )
			{
				AUDIOMGR->MuteBGM( TRUE );

				CIMEWND->SetCtrlPushed( FALSE );
			}
			else
			{
				AUDIOMGR->MuteBGM( FALSE );
			}
		}
		break;

	case WM_ACTIVATE:
		
		if( g_UserInput.GetKeyboard()->GetDirectInput() )
		{
			if( WA_INACTIVE != wParam )
			{
				// Make sure the device is acquired, if we are gaining focus.
				g_UserInput.GetKeyboard()->ClearKeyState();
				g_UserInput.GetKeyboard()->UpdateKeyboardState();
				//g_UserInput.SetInputFocus( TRUE );
				OBJECTMGR->SetAllNameShow( FALSE );
			}
		}
		break;

	case WM_USER+155:
		{
			ISC_BaseNetwork* lpNet = NETWORK->GetBaseNetwork();
			if(lpNet)
				lpNet->SocketEventProc();
		}
		return TRUE;		
		
	case WM_CLOSE:
		{
			MAINGAME->SetGameState( eGAMESTATE_END );

#ifdef _NPROTECT_
			if(NPROTECTMGR->GetMsg())
			{
				MessageBox(hWnd, NPROTECTMGR->GetMsg(), "nProtect GameGuard", MB_OK);
			}
#endif
		}
		break;

	case WM_SETFOCUS:
	case WM_SETCURSOR:
		{ 
			CURSOR->Process();
		}
		return 0;
	case WM_SYSKEYDOWN:		//Alt + F4 ��Io��I�ˢ�Ao
		{
			if( wParam == VK_F4 )
			{
				if( MAINGAME->GetCurStateNum() == eGAMESTATE_TITLE )
				{
					break;		// �ˢ硧��OAO E��EcAI ����uE�ˢ硧��i ~�ˢ�ˢ�a
				}
				else if( MAINGAME->GetCurStateNum() == eGAMESTATE_CHARSELECT ||
					MAINGAME->GetCurStateNum() == eGAMESTATE_CHARMAKE )
				{
					WINDOWMGR->MsgBox( MBI_EXIT, MBT_YESNO, RESRCMGR->GetMsg( 263 ) );
					return 0;
				}
			}
		}
		return 0;
	case WM_MOVE:
#ifdef _GMTOOL_
		GMTOOLMGR->SetPositionByMainWindow();
#elif _TESTCLIENT_
		TSTOOLMGR->SetPositionByMainWindow();
		TSTOOLMGR->SetPositionByOutputWindow();
#endif
		break;


	case WM_CREATE:
		{
#ifdef _GMTOOL_
			INITCOMMONCONTROLSEX icex;
			icex.dwSize = sizeof( icex );
			icex.dwICC	= ICC_DATE_CLASSES;
			InitCommonControlsEx( &icex );
#elif _TESTCLIENT_
			INITCOMMONCONTROLSEX icex;
			icex.dwSize = sizeof( icex );
			icex.dwICC	= ICC_DATE_CLASSES;
			InitCommonControlsEx( &icex );
#endif

#ifdef _JAPAN_LOCAL_
			HIMC hImc = ImmGetContext( hWnd );
			DWORD dwConversion, dwSentence;
			ImmGetConversionStatus( hImc, &dwConversion, &dwSentence );
			dwSentence |= IME_SMODE_SINGLECONVERT;
			ImmSetConversionStatus( hImc, dwConversion, dwSentence );
			ImmReleaseContext( hWnd, hImc );
#endif
		}
		break;
	}
	
	return DefWindowProc(hWnd, message, wParam, lParam);
}



// Mesage handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}


BOOL GetPatchVersion()
{
	CMHFile file;
	if(!file.Init("LunaVerInfo.ver", "r"))
		return FALSE;
	strcpy(g_PATCHVERSION, file.GetString());

	return TRUE;
}

BOOL EnterClientApplication()
{
#ifndef _GMTOOL_
    g_hMap = CreateFileMapping((HANDLE)0xFFFFFFFF, NULL, PAGE_READWRITE, 0, 1024, "LUNA Online Client Application");
    if(g_hMap != NULL && GetLastError() == ERROR_ALREADY_EXISTS) 
    { 
		CloseHandle(g_hMap); 
		g_hMap = NULL;
		return FALSE;
    }
#endif

	return TRUE;
}


void LeaveClientApplication()
{//taiyo
	if(g_hMap)
	{
		CloseHandle(g_hMap);
		g_hMap = NULL;
	}	
}


//pjslocal
#ifdef _TW_LOCAL_
int is_big5(BYTE *str)
{
	if( str[0] & 0x80 )
	{
		if ( str[0] == 0xa2)
		{
			if( str[1]>= 0xaf && str[1]<= 0xb8 )
				return 1;
			else if( str[1] >= 0xcf && str[1] <= 0xff )
				return 1;
		} 
		if ( str[0] == 0xa3)
		{
			if( str[1]>=0x43 && str[1]<=0xff )
				return 1;
		}

		if ( str[0] == 0xa4)
		{
			if( str[1] >= 0x40 && str[1] <= 0xff )
				return 1;
		}

		if ( str[0] >= 0xa5 && str[0] <= 0xc5 )
			return 1;

		if ( str[0] == 0xc6 )
		{
			if( str[1] >= 0x00 && str[1] <= 0x7e )
				return 1;
		}
	}
	else
	{
		return 1;
	}
	return 0;
}

#endif

////////////////////////////////////////////////////////////////////////////////////////
// resource ���סˢ�u��E��Io����uEAC ���סˢ�AAI �ˢ硧��E�ˢ�ic(jsd)
///////////////////////////////////////////////////////////////////////////////////////
BOOL CheckResourceFile()
{
	HANDLE hSrch;
	WIN32_FIND_DATA wfd;
	SYSTEMTIME time;
	char Dir[MAX_PATH];
	char filename[MAX_PATH];
	BOOL bResult = TRUE;
	CMHFile file;
	char temp[256];
	WORD year, month, day, hour, min;	

	GetWindowsDirectory( Dir, MAX_PATH );
	strcat( Dir, "\\mfl.dat" );
		
	GetCurrentDirectory( MAX_PATH, filename );
	strcat( filename, "\\Resource\\*.bin" );
	hSrch = FindFirstFile( filename, &wfd );
	while( bResult )
	{
		FileTimeToSystemTime( &wfd.ftLastWriteTime, &time );

		///
		if( !file.Init( Dir, "rt", MHFILE_FLAG_DONOTDISPLAY_NOTFOUNDERROR ) )
		{
			file.Release();
			goto filesave;
		}
		while( !file.IsEOF() )
		{
			file.GetString( temp );
			if( strcmp( wfd.cFileName, temp ) == 0 )
			{
				year = file.GetWord();
				month = file.GetWord();
				day = file.GetWord();
				hour = file.GetWord();
				min = file.GetWord();
				
				if( year != time.wYear || month != time.wMonth || day != time.wDay ||
					hour != time.wHour || min != time.wMinute )
				{
					file.Release();
					return FALSE;
				}
				else
					break;
			}
		}
		file.Release();
		///		

		bResult = FindNextFile( hSrch, &wfd );
	}

	bResult = TRUE;
	GetCurrentDirectory( MAX_PATH, filename );
	strcat( filename, "\\Resource\\SkillArea\\*.bsad" );
	hSrch = FindFirstFile( filename, &wfd );
	while( bResult )
	{
		FileTimeToSystemTime( &wfd.ftLastWriteTime, &time );

		///
		if( !file.Init( Dir, "rt", MHFILE_FLAG_DONOTDISPLAY_NOTFOUNDERROR ) )
		{
			file.Release();
			goto filesave;
		}
		while( !file.IsEOF() )
		{
			file.GetString( temp );
			if( strcmp( wfd.cFileName, temp ) == 0 )
			{
				year = file.GetWord();
				month = file.GetWord();
				day = file.GetWord();
				hour = file.GetWord();
				min = file.GetWord();
				
				if( year != time.wYear || month != time.wMonth || day != time.wDay ||
					hour != time.wHour || min != time.wMinute )
				{
					file.Release();
					return FALSE;
				}
				else
					break;
			}
		}
		file.Release();
		///		

		bResult = FindNextFile( hSrch, &wfd );
	}

	return TRUE;

filesave:
	SaveFileList();
	return TRUE;
}

void ChangeVerInfo()
{
	char temp[256];
	FILE* fp = fopen( "LunaVerInfo.ver", "r" );
	if( fp == NULL )
		return;
	fscanf( fp, "%s", temp );
	fclose( fp );

	temp[10] = NULL;
	fp = fopen( "LunaVerInfo.ver", "w" );
	if( fp == NULL )
		return;
	fprintf( fp, "%s%s", temp, "01" );
	fclose( fp );
}

BOOL SaveFileList()
{
	DWORD count = 1;
	HANDLE hSrch;
	WIN32_FIND_DATA wfd;
	SYSTEMTIME time;
	char Dir[MAX_PATH];
	char filename[MAX_PATH];
	BOOL bResult = TRUE;
	
	GetWindowsDirectory( Dir, MAX_PATH );
	strcat( Dir, "\\mfl.dat" );
	FILE* fp = fopen( Dir, "w" );
	if( fp == NULL )
	{
		fclose( fp );
		return FALSE;
	}

	GetCurrentDirectory( MAX_PATH, filename );
	strcat( filename, "\\Resource\\*.bin" );
	hSrch = FindFirstFile( filename, &wfd );
	while( bResult )
	{
		FileTimeToSystemTime( &wfd.ftLastWriteTime, &time );
		fprintf( fp, "%d %s\t", count, wfd.cFileName );
		fprintf( fp, "%4d %02d %02d %02d %02d\n", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute );
		bResult = FindNextFile( hSrch, &wfd );
		++count;
	}
	
	bResult = TRUE;
	GetCurrentDirectory( MAX_PATH, filename );
	strcat( filename, "\\Resource\\SkillArea\\*.bsad" );
	hSrch = FindFirstFile( filename, &wfd );
	while( bResult )
	{
		FileTimeToSystemTime( &wfd.ftLastWriteTime, &time );
		fprintf( fp, "%d %s\t", count, wfd.cFileName );
		fprintf( fp, "%4d %02d %02d %02d %02d\n", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute );
		bResult = FindNextFile( hSrch, &wfd );
		++count;
	}

	fclose( fp );

	return TRUE;
}

#ifdef _HACK_SHIELD_
BOOL HS_Init(void)
{
	int		dwRet = 0;			// _AhnHS_Initialize �Լ� ���� ��


	TCHAR	szFullFilePath[MAX_PATH];	// ���� ���
	TCHAR	szMsg[MAX_PATH];		// �޽��� ����
	DWORD	dwOption = 0;			// �ʱ�ȭ �ɼ�

	// ���� ������ ���� ��θ� ���մϴ�.
	// �� ���� :
	//    GetCurrentDirectory �Լ��� �ƴ�, GetModuleFileName �Լ��� �̿��ؼ� �����������
	//    ������ ��θ� ���Ͻñ� �ٶ��ϴ�.
	strcpy(  szFullFilePath, DIRECTORYMGR->GetFullDirectoryName(eLM_Root) );
	// ���Ӱ� ������ HackShield�� EhSvc.Dll�� ��� ����	
	lstrcat ( szFullFilePath, _T( "HShield\\EhSvc.dll" ) );

	// �ʱ�ȭ �ɼ��� �����մϴ�. AHNHS_CHKOPT_�� �����ϴ� �ɼ��� ��� �����Ϸ���


	// dwOption = AHNHS_CHKOPT_ALL �� �����ϰ� ������ �� �ֽ��ϴ�.
	// �ڼ��� ������ "���α׷��� ���̵�"�� �����ϼ���.
	// �� ���� :
	//    ���� ����� ��, 
	//    AHNHS_CHKOPT_READWRITEPROCESSMEMORY, AHNHS_CHKOPT_KDTARCER
	//    AHNHS_CHKOPT_OPENPROCESS, AHNHS_CHKOPT_MESSAGEHOOK �ɼ��� ����
	//    �ϼž� ������� �����մϴ�.
	dwOption = 
#ifndef _GMTOOL_
		AHNHS_CHKOPT_READWRITEPROCESSMEMORY |
		AHNHS_CHKOPT_KDTRACER |
//		AHNHS_CHKOPT_KDTARCER |
		AHNHS_CHKOPT_OPENPROCESS |
		AHNHS_CHKOPT_MESSAGEHOOK |
#endif
		AHNHS_CHKOPT_ALL;
		

	// HackShield ���� �ʱ�ȭ


	// �ʱ�ȭ �Լ� _AhnHS_Initialize�� ������ ���� ���� ���� ������ �ֽ��ϴ�.
	// 3��°: �����ڵ�,
	// 4��°: ���̼��� Ű,
	// 6��°: ���ǵ��� �ΰ���


	// ���� �� �˸��� ������ ��ü�Ͻñ� �ٶ��ϴ�.
	// �ڼ��� ������ "���α׷��� ���̵�"�� �����ϼ���

	dwRet = _AhnHS_Initialize ( szFullFilePath, HS_CallbackProc, 5885, "01740D2BA4853F462EAE1256", 
		dwOption, AHNHS_SPEEDHACK_SENSING_RATIO_HIGH );

	// HackShield �ʱ�ȭ ���� �� ���� ó��
	if ( dwRet != HS_ERR_OK ) 
	{
		// _AhnHS_Initialize �Լ��� ȣ�� ���� �ÿ� �Ѿ���� �ڵ忡 ���� ó�����ִ� �κ��Դϴ�.
		// "���α׷��� ���̵�"�� �����Ͻþ� ������ �����޽����� �������ֽñ� �ٶ��ϴ�.
		switch ( dwRet )
		{
		case HS_ERR_INVALID_PARAM:
			{
				wsprintf ( szMsg, "[HS Code=%x] Init: �߸��� Callback�Լ� ������ ���� �� ���̼��� Ű �� ������ �ùٸ��� �ʽ��ϴ�. ", 
					dwRet );
				break;
			}
		case HS_ERR_INVALID_LICENSE:
			{
				wsprintf ( szMsg, "[HS Code=%x] Init: ���̼��� Ű�� �ùٸ��� �ʽ��ϴ�.", 
					dwRet );
				break;
			}
		default:
			{
				wsprintf ( szMsg, "[HS Code=%x] Init: �� �� ���� ������ �߻��Ͽ����ϴ�.", 
					dwRet );
				break;
			}
		}
		// ���� ������ �˸� �޽����� �߻��մϴ�.
//		MessageBox( _g_hWnd, szMsg, "HSERR", MB_OK );


		return FALSE;
	}
	return TRUE;
}

// HackShield ���� ����
BOOL HS_StartService ( void )
{
	int		nReturn = 0;
	TCHAR	szMsg[MAX_PATH];

	// ���� �Լ� ȣ�� 
	nReturn = _AhnHS_StartService();

	// HackShield ���� ���н� ���� ó��
	if ( nReturn != HS_ERR_OK )
	{
		switch ( nReturn )
		{
		case HS_ERR_NOT_INITIALIZED:
			{
				wsprintf ( szMsg, "[HS Code=%x] StartService: _Ahn_HS_Initialize �ʱ�ȭ�� �ʿ��մϴ�. ", nReturn ); 
					break;
			}
		case HS_ERR_ALREADY_SERVICE_RUNNING:
			{
				wsprintf ( szMsg, "[HS Code=%x] StartService: _Ahn_HS_Initialize �� �̹� �ʱ�ȭ �Ǿ� �ֽ��ϴ�.", nReturn );
				break;
			}

		default:
			{
				wsprintf ( szMsg, "[HS Code=%x] StartService: �� �� ���� ������ �߻��Ͽ����ϴ�. ", nReturn );
					break;
			}
		}
		// ���ӳ����� �˸� �޽����� �߻��մϴ�.
//		MsgBox (szMsg, "�˸�" ..);
		return FALSE;
	}
	return TRUE;
}

// HackShield ���� ����
BOOL HS_StopService ( void )
{
	int		nReturn = 0;
//	TCHAR	szMsg[MAX_PATH];

	nReturn = _AhnHS_StopService();

	// HackShield ���� ���� �� ���� ó��
	if ( nReturn != HS_ERR_OK ) 
	{
//		...
			return FALSE;
	}
	return TRUE;
}

// HackShield ���� ����
BOOL HS_UnInit ( void )
{
	int		nReturn = 0;
//	TCHAR	szMsg[MAX_PATH];

	nReturn = _AhnHS_Uninitialize();

	// HackShield ���� ���� �� ���� ó��
	if ( nReturn != HS_ERR_OK ) 
	{
//		..
			return FALSE;
	}
	return TRUE;
}

// HackShield ���񽺷κ��� �����Ǵ� ��� ��ŷ���� �̺�Ʈ�� �����ϱ� ���� �ݹ� �Լ�
int __stdcall HS_CallbackProc( long lCode, long lParamSize, void* pParam )
{
	TCHAR	szMsg[MAX_PATH];

	switch ( lCode )
	{
	case AHNHS_ENGINE_DETECT_GAME_HACK:
		{
			wsprintf ( szMsg, "[HS Code=%x] MoSiang can't play with this program[%s].", lCode, ( LPTSTR ) pParam );
			MessageBox( _g_hWnd, szMsg, "HSError", MB_OK );

				// ���� ������ �˸� �޽����� �߻��մϴ�.
//				MsgBox (szMsg, "�˸�" ..);
			// �������Ḧ �����մϴ�.
			//PostQuitMessage(0);
			PostMessage( _g_hWnd, WM_CLOSE, NULL, NULL );
		}
		break;
//	case AHNHS_ACTAPC_DETECT_SPEEDHACK_APP:
	case AHNHS_ACTAPC_DETECT_SPEEDHACK:
		{
			wsprintf ( szMsg, "[HS Code=%x] Detected SpeedHack.", lCode);
			MessageBox( _g_hWnd, szMsg, "HSError", MB_OK );
				// ���� ������ �˸� �޽����� �߻��մϴ�.
	//			MsgBox (szMsg, "�˸�" ..);
			// ���ǵ��� ��뿡 �ΰ��� �������α׷��� ���� �����մϴ�.
			//PostQuitMessage(0);
			PostMessage( _g_hWnd, WM_CLOSE, NULL, NULL );
		}
		break;
	case AHNHS_ACTAPC_DETECT_KDTRACE_CHANGED:
	case AHNHS_ACTAPC_DETECT_KDTRACE:
		{
			wsprintf ( szMsg, "[HS Code=%x] Detected Debuging.", lCode);
			MessageBox( _g_hWnd, szMsg, "HSError", MB_OK );

			// ���� ������ �˸� �޽����� �߻��մϴ�.
//			MsgBox (szMsg, "�˸�" ..);
			// ����ڰ� �������α׷��� ����� �۾��� ������ ���ɼ��� �����Ƿ� ���Ḧ �����մϴ�.
			//PostQuitMessage(0);
			PostMessage( _g_hWnd, WM_CLOSE, NULL, NULL );
		}
		break;
	case AHNHS_ACTAPC_DETECT_HOOKFUNCTION:
	case AHNHS_ACTAPC_DETECT_MESSAGEHOOK:
	case AHNHS_ACTAPC_DETECT_MODULE_CHANGE:
		{
			wsprintf ( szMsg, "[HS Code=%x] HackShield Error.", lCode );
			MessageBox( _g_hWnd, szMsg, "HSError", MB_OK );
			// ���� ������ �˸� �޽����� �߻��մϴ�.
//			MsgBox (szMsg, "�˸�" ..);
			// �������Ḧ ������


			//PostQuitMessage(0);
			PostMessage( _g_hWnd, WM_CLOSE, NULL, NULL );
		}
		break;
//	case AHNHS_ENGINE_DETECT_GENERAL_HACK:
	case AHNHS_ACTAPC_DETECT_ALREADYHOOKED:
	case AHNHS_ACTAPC_DETECT_DRIVERFAILED:
//	case AHNHS_ACTAPC_DETECT_SPEEDHACK_APP:
		{
			// ���� ���� ������ ó������ �����ŵ� �˴ϴ�.
			break;
		}
	}
	return 1;
}

// �� HS_PauseService :
//    Ư�� XP OS���� ä��â �ѱ�Ű �Է��� �Ұ����� ���,
//    Ű���� MSG Hooking ��ȣ ����� �Ͻ������� �����ϴ� �Լ��Դϴ�.
BOOL HS_PauseService ( void )
{
	int		nReturn = 0;
	TCHAR	szMsg[MAX_PATH];

	nReturn = _AhnHS_PauseService ( AHNHS_CHKOPT_MESSAGEHOOK );

	if ( nReturn != HS_ERR_OK )
	{
		switch ( nReturn )
		{
		case HS_ERR_NOT_INITIALIZED:
			{
				wsprintf ( szMsg, "[HS Code=%x] PauseService: _Ahn_HS_Initialize �ʱ�ȭ�� �ʿ��մϴ�. ", nReturn );
					break;
			}
		case HS_ERR_SERVICE_NOT_RUNNING:
			{
				wsprintf ( szMsg, "[HS Code=%x] PauseService: _Ahn_HS_StartService ������ �ʿ��մϴ�.", nReturn );
				break;
			}

		default:
			{
				wsprintf ( szMsg, "[HS Code=%x] PauseService: �˼����� ������ �߻� �Ͽ����ϴ�.", 
					nReturn );
				break;
			}
		}
		// ���ӳ����� �˸� �޽����� �߻��մϴ�.
//		MsgBox (szMsg, "�˸�" ..);
		return FALSE;
	}
	return TRUE;
}

// _AhnHS_PauseService() �Լ��� �̿��Ͽ�,
// ������ Ű���� MSG Hooking ��ȣ ����� ����� �Ѵ�.
BOOL HS_ResumeService ( void )
{
	int		nReturn = 0;
	TCHAR	szMsg[MAX_PATH];

	nReturn = _AhnHS_ResumeService ( AHNHS_CHKOPT_MESSAGEHOOK );

	if ( nReturn != HS_ERR_OK )
	{
		switch ( nReturn )
		{
		case HS_ERR_NOT_INITIALIZED:
			{
				wsprintf ( szMsg, "[HS Code=%x] ResumeService: _Ahn_HS_Initialize �ʱ�ȭ�� �ʿ��մϴ�.", nReturn );
					break;
			}
		case HS_ERR_SERVICE_NOT_RUNNING:
			{
				wsprintf ( szMsg, "[HS Code=%x] ResumeService: _Ahn_HS_StartService ������ �ʿ��մϴ�.", nReturn );
					break;
			}

		default:
			{
				wsprintf( szMsg, "[HS Code=%x] ResumeService: �� �� ���� ������ �߻��Ͽ����ϴ�.", nReturn );
					break;
			}
		}
		// ���ӳ����� �˸� �޽����� �߻��մϴ�.
//		MsgBox (szMsg, "�˸�" ..);
		return FALSE;
	}
	return TRUE;
}

void HS_SaveFuncAddress()
{
//	TCHAR	szMsg[MAX_PATH];

    int nResult = _AhnHS_SaveFuncAddress( 3,
											HS_Init,
											HS_CallbackProc,
											CHackShieldManager::NetworkMsgParse
											);
}

#endif

#ifdef _NPROTECT_
BOOL CALLBACK NPGameMonCallback(DWORD dwMsg, DWORD dwArg)
{
	return NPROTECTMGR->MsgProc(dwMsg, dwArg);
}
#endif
