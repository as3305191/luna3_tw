#include "stdafx.h"
#include "Application.h"
#include "MainDialog.h"
#include "Network.h"

/////////////////////////////////////////////////////////////////////////////
// The one and only CApplication object
CApplication theApp;

/////////////////////////////////////////////////////////////////////////////
// CApplication

BEGIN_MESSAGE_MAP(CApplication, CWinApp)
	//{{AFX_MSG_MAP(CApplication)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

void TokenizePosition(CApplication::Position& position, LPCTSTR seperator)
{
	LPCTSTR left = _tcstok(
		0,
		seperator);
	LPCTSTR top = _tcstok(
		0,
		seperator);
	LPCTSTR right = _tcstok(
		0,
		seperator);
	LPCTSTR bottom = _tcstok(
		0,
		seperator);
	LPCTSTR x = _tcstok(
		0,
		seperator);
	LPCTSTR y = _tcstok(
		0,
		seperator);

	position.mRect.left = _ttoi(left ? left : _T(""));
	position.mRect.top = _ttoi(top ? top : _T(""));
	position.mRect.right = position.mRect.left + _ttoi(right ? right : _T(""));
	position.mRect.bottom = position.mRect.top + _ttoi(bottom ? bottom : _T(""));
	position.mPoint.x = _ttoi(x ? x : _T(""));
	position.mPoint.y = _ttoi(y ? y : _T(""));
}

// �ؽ�Ʈ �� " " ������ ���ڿ��� ��ȯ�Ѵ�. ���� ��� "aa"�� ������ aa�� ��ȯ�ȴ�.
const TCHAR* GetString( const TCHAR* text )
{
	const TCHAR* begin = text;
	const TCHAR* end = text + sizeof( text );

	for( const TCHAR* ch = text; ch; ++ch )
	{
		if( '\"' == *ch )
		{
			begin = ++ch;
			break;
		}
	}

	for( const TCHAR* ch = begin; ch; ++ch )
	{
		if( '\"' == *ch )
		{
			end = --ch;
			break;
		}
	}

	const size_t size = abs( begin - end ) + 1;

	static TCHAR buffer[ MAX_PATH ];
	_tcsncpy( buffer, begin, size );
	
	buffer[ size  ] = '\0';
	return buffer;
}


/////////////////////////////////////////////////////////////////////////////
// CApplication construction

CApplication::CApplication()
{
	for( int i = 1; i < __argc; ++i )
	{
		// 081202 LUJ, �ɼ� ����
		if( ! _tcsicmp( __targv[ i ], _T( "/log" ) ) )
		{
			CLog::SetEnable( true );
		}
		// 081202 LUJ, �����͸� �޸𸮿� �ٿ�޴´�. ��ġ ������ ���� ȹ���� �� ������ �Ѵ�.
		//			�׷��� ��ġ ũ�Ⱑ Ŭ���� ���� �޸𸮰� Ŀ���� �����ؾ��Ѵ�
		else if( ! _tcsicmp( __targv[ i ], _T( "/memory" ) ) )
		{
			mConfig.mFileType = Config::FileTypeMemory;

			CLog::Put( _T( "Download data will be store to memory." ) );
		}
		// 081202 LUJ, ������ ǥ���Ѵ�
		else
		{
			CLog::Put( _T( "help:" ) );
			CLog::Put( _T( "	/log		put log" ) );
			CLog::Put( _T( "	/memory		download to memory" ) );
		}
	}

	CLog::Put( _T("-----------------------------------------------") );
	CLog::Put( _T( "2025 Luna Launcher %s. MMX Network." ), VERSION );
	CLog::Put( _T("-----------------------------------------------") );

#ifdef _LOCAL_JP_
	TCHAR buffer[ 256 ] = { 0 };
	_tcscpy( buffer, GetCommandLine() );

	const TCHAR* separator = _T( " " );
	// ��ó��ġ ��� ȹ��
	const TCHAR* token = _tcstok( buffer, separator );
	m_Purple.m_LauncherPath = token;
	// �ڵ��ȣ ȹ��
	token = _tcstok( 0, separator );
	CString TargetHandle = token;
	m_Purple.m_hTarget = (HWND)_ttol(TargetHandle);
	// �糪��ġ ��� ȹ��
	token = _tcstok( 0, separator );
	m_Purple.m_GameInstallPath = token;
	// ��Ÿ���� ȹ��
	token = _tcstok( 0, separator );
	m_Purple.m_ETCPath = token;
	
	// �糪 ��ġ��ΰ� �����ϸ� ���� ���丮�� �糪��ġ��η� �缳���Ѵ�.
	if(!m_Purple.m_GameInstallPath.IsEmpty())
	{
		CString fullPath = m_Purple.m_GameInstallPath;
		fullPath.Replace(_T("\\"),_T("\\\\"));
		fullPath.Remove('"');
		SetCurrentDirectory(fullPath);
	}

	CLog::Put( _T("parameter : %s %s %s %s" ), m_Purple.m_LauncherPath, TargetHandle, m_Purple.m_GameInstallPath, m_Purple.m_ETCPath);
#endif

	// 080515 LUJ, ���� ������ �ʱ�ȭ�Ѵ�
	SetLocal( Local::TypeNone );

	// 080508 LUJ, �ʱ�ȭ. sav ������ �ٲ��� �ʾƵ� ���డ���ϵ��� �Ѵ�
	{
		// Firman Akbar 10/05/2022 Custom Launcher
		const CString imagePath( _T( "data\\interface\\windows\\" ) );
		const CString blueInterface( imagePath + _T( "image_path.bin" ) );

		const CString languagePath( _T( "data\\language\\" ) );
		const CString languagepack( languagePath + _T( "LanguageList.bin" ) );
		
		mConfig.m_SelectedInterface		= blueInterface;
		mConfig.m_SelectedLanguage		= languagepack;
		mConfig.m_SelectedAliashing		= _T( "0" );
		mConfig.m_isWindowMode			= false;
	}

	// ���̺� ���� �б�
	{
		CStdioFileEx file;

		if( file.Open( _T( "system\\launcher.sav" ), CFile::modeRead | CFile::typeText ) )
		{
			for(CString line;
				file.ReadString(line);)
			{
				TCHAR buffer[ MAX_PATH ] = { 0 };
				_tcscpy( buffer, line );

				LPCTSTR separator = _T( "\n,= " );
				LPCTSTR token = _tcstok( buffer, separator );
				
				if( 0 == token		||		// empty line
					';' == token[ 0 ] ) 	// comment
				{
					continue;
				}
				// 0912221 LUJ, resolution use for both - enumerated or detect type
				else if( ! _tcsicmp( _T( "resolution" ), token ) )
				{
					LPCTSTR textWidth = _tcstok(0, separator);
					LPCTSTR textHeight = _tcstok(0, separator);

					mConfig.mResolution.cx = _ttoi(textWidth ? textWidth : _T(""));
					mConfig.mResolution.cy = _ttoi(textHeight ? textHeight : _T(""));
				}
				else if( ! _tcsicmp( _T( "windowMode" ), token ) )
				{
					token = _tcstok( 0, separator );

					mConfig.m_isWindowMode = ! _tcsicmp( _T( "true" ), token );
				}
				// 080507 LUJ, ���õ� �̹��� ��� �� ������
				else if( ! _tcsicmp( _T( "imagePath" ), token ) )
				{
					token = _tcstok( 0, separator );

					// 080508 LUJ, ���� ���� ��쿡�� �����ؾ���
					if( token )
					{
						mConfig.m_SelectedInterface = token;
					}
				}
				// Firman Akbar 10/05/2022 Custom Button Launcher
				else if( ! _tcsicmp( _T( "aliashing" ), token ) )
				{
					token = _tcstok( 0, separator );

					// 080508 LUJ, ���� ���� ��쿡�� �����ؾ���
					if( token )
					{
						mConfig.m_SelectedAliashing = token;
					}
				}
				else if( ! _tcsicmp( _T( "languagePath" ), token ) )
				{
					token = _tcstok( 0, separator );

					// 080508 LUJ, ���� ���� ��쿡�� �����ؾ���
					if( token )
					{
						mConfig.m_SelectedLanguage = token;
					}
				}
			}

			CLog::Put( _T( "system\\Launcher.sav is loaded" ) );
		}
	}
	
	// ��Ų �ʱ�ȭ
	{
		/*
		�����ڵ� ������ �о���̴� Ŭ����

		����: http://www.ucancode.net/faq/CStdioFile-MFC-Example-ReadString-WriteString.htm
		*/
		CStdioFileEx file;

// 091117 ONS ��LunaLauncher.exe���� ���� ������ ��setting.cfg���� �о� �� ���ֵ��� ����
// ���� �������� ������ �糪 �ν��� ������ setting.cfg�� �о�´�.
#ifdef _LOCAL_JP_

		// LunaLauncher.exe�� ���� ������ setting.cfg������ ����.
		CString strFilePath;
		CString strPath = m_Purple.m_LauncherPath;

		CLog::Put( _T( "LauncherFilePath : %s" ), strPath );

		if( !strPath.IsEmpty() )
		{
			const int nIndex = strPath.ReverseFind( '\\' );
			strFilePath = strPath.Left(nIndex) + _T( "\\setting.cfg" );
		}

		CLog::Put( _T( "SettingFilePath : %s" ), strFilePath );

		if( !file.Open( strFilePath, CFile::modeRead | CFile::typeText ) )
		{
			CLog::Put( _T( "%s is not found" ), strFilePath );
			if( ! file.Open( _T( "system\\setting.cfg" ), CFile::modeRead | CFile::typeText ) )
			{
				MsgBoxLog( _T( "system\\setting.cfg is not found" ), _T( "" ) );
				
				PostQuitMessage( 1 );
				return;
			}			
		}
		
#else
		if( ! file.Open( _T( "system\\setting.cfg" ), CFile::modeRead | CFile::typeText ) )
		{
			MsgBoxLog( _T( "system\\setting.cfg is not found" ), _T( "" ) );
			
			PostQuitMessage( 1 );
			return;
		}
#endif
		TCHAR buffer[ MAX_PATH ] = { 0 };
		const TCHAR* separator = _T( "\n,= " );
		CString line;
		CString usingSkin;
		bool foundSkin = false;

		while( file.ReadString( line ) )
		{
			_tcscpy( buffer, line );
					
			const TCHAR* token = _tcstok( buffer, separator );

			if( 0 == token		||	// empty line
				';' == token[ 0 ] ) 	// comment
			{
				continue;
			}
			else if( ! _tcsicmp( _T( "ftp_address" ), token ) )
			{
				mLocal.mDownload.mPath = _tcstok( 0, separator );
			}			
			else if( ! _tcsicmp( _T( "version_tag" ), token ) )
			{
				mLocal.mDownload.mVersionTag = _tcstok( 0, separator );
			}
			else if( ! _tcsicmp( _T( "version_filename" ), token ) )
			{
				mLocal.mDownload.mVersionFile = _tcstok( 0, separator );
			}
			else if( ! _tcsicmp( _T( "messageBox_caption" ), token ) )
			{
				mConfig.m_messageBoxCaption = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_selfUpdateFailed" ), token ) )
			{
				mConfig.m_messageBoxSelfUpdateFailed = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_notFoundVersionFile" ), token ) )
			{
				mConfig.m_messageBoxNotFoundVersionFile = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_incorrectVersionFile" ), token ) )
			{
				mConfig.m_messageBoxIncorrectVersionFile = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_failedVersionFileDelete" ), token ) )
			{
				mConfig.m_messageBoxFailedVersionFileDelete = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_failedVersionFileRename" ), token ) )
			{
				mConfig.m_messageBoxFailedVersionFileRename = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_failedCreateExecutive" ), token ) )
			{
				mConfig.m_messageBoxFailedCreateExecutive = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_failedRunClient" ), token ) )
			{
				mConfig.m_messageBoxFailedRunClient = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_failedExitDuringPacking" ), token ) )
			{
				mConfig.m_messageBoxFailedExitDuringPacking = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_failedConnect" ), token ) )
			{
				mConfig.m_messageBoxFailedConnect = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_failedVersionFileOpen" ), token ) )
			{
				mConfig.m_messageBoxInvalidateVersion = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_failedFileDownload" ), token ) )
			{
				mConfig.m_messageBoxFailedFileDownload = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_failedExtract" ), token ) )
			{
				mConfig.m_messageBoxFailedExtract = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_invalidateVersion" ), token ) )
			{
				mConfig.m_messageBoxInvalidateVersion = GetString( line );
			}
			//090819 ONS ���÷�ó���� �޼��� �߰�
			else if( ! _tcsicmp( _T( "messageBox_message_Pruple_InvalidArgument" ), token ) )
			{
				mConfig.m_messageBoxPrupleInvalidArgument = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_Pruple_InvalidHandle" ), token ) )
			{
				mConfig.m_messageBoxPrupleInvalidHandle = GetString( line );
			}
			else if( ! _tcsicmp( _T( "local" ), token ) )
			{
				const TCHAR* local = _tcstok( 0, separator );

				// Firman Akbar 10/05/2022 Custom Launcher		  
				if( ! _tcsicmp( _T( "Arcane" ), local ) )
				{
					SetLocal( Local::TypeArcane );

					CLog::Put( _T( "Local is Arcane" ) );
				}
				else if( ! _tcsicmp( _T( "WtaeOffline" ), local ) )
				{
					SetLocal( Local::TypeWtaeOffline );

					CLog::Put( _T( "Local is WtaeOffline" ) );
				}
				continue;
			}
            
			else if( ! _tcsicmp( _T( "usingSkin" ), token ) )
			{
				usingSkin = _tcstok( 0, separator );
				continue;
			}

			if( '[' == token[ 0 ] )
			{
				// ���� ��Ų �̸��� ã�� ������ �ݺ�
				foundSkin = ( usingSkin == token );
				continue;
			}
			
			if( ! foundSkin )
			{
				continue;
			}

			else if(! _tcsicmp( _T( "image_discord_btn" ), token ))
			{
				TokenizePosition(
					mConfig.mDiscordButton,
					separator);
			}
			else if(! _tcsicmp( _T( "image_facebook_btn" ), token ))
			{
				TokenizePosition(
					mConfig.mFacebookButton,
					separator);
			}
			else if(! _tcsicmp( _T( "image_discord_btn_hover" ), token ))
			{
				TokenizePosition(
					mConfig.mDiscordButtonHover,
					separator);
			}
			else if(! _tcsicmp( _T( "image_facebook_btn_hover" ), token ))
			{
				TokenizePosition(
					mConfig.mFacebookButtonHover, 
					separator);
			}
			else if( ! _tcsicmp( _T( "control_html" ), token ) )
			{
				TokenizePosition(
					mConfig.mHtmlPosition,
					separator);
			}
			else if (!_tcsicmp(_T("control_server_status"), token))
			{
				TokenizePosition(
					mConfig.mServerStatusPosition,
					separator);
			}
			else if( ! _tcsicmp( _T( "control_resolution_combo" ), token ) )
			{
				LPCTSTR x = _tcstok(
					0,
					separator);
				LPCTSTR y = _tcstok(
					0,
					separator);

				mConfig.mResolutionCombo.x = _ttoi(x ? x : _T(""));
				mConfig.mResolutionCombo.y = _ttoi(y ? y : _T(""));
			}
			else if( ! _tcsicmp( _T( "control_resolution_arrow" ), token ) )
			{
				LPCTSTR x = _tcstok(
					0,
					separator);
				LPCTSTR y = _tcstok(
					0,
					separator);

				mConfig.mResolutionArrow.x = _ttoi(x ? x : _T(""));
				mConfig.mResolutionArrow.y = _ttoi(y ? y : _T(""));
			}
			// 080507 LUJ, �̹��� ��� ���� ��Ʈ�� ��ġ
			else if( ! _tcsicmp( _T( "control_interface_combo" ), token ) )
			{
				LPCTSTR x = _tcstok(
					0,
					separator);
				LPCTSTR y = _tcstok(
					0,
					separator);

				mConfig.mInterfaceCombo.x = _ttoi(x ? x : _T(""));
				mConfig.mInterfaceCombo.y = _ttoi(y ? y : _T(""));
			}
			else if( ! _tcsicmp( _T( "control_interface_arrow" ), token ) )
			{
				LPCTSTR x = _tcstok(
					0,
					separator);
				LPCTSTR y = _tcstok(
					0,
					separator);

				mConfig.mInterfaceArrow.x = _ttoi(x ? x : _T(""));
				mConfig.mInterfaceArrow.y = _ttoi(y ? y : _T(""));
			}
			else if(0 == _tcsicmp(_T("image"), token))
			{
				mConfig.mImage = _tcstok(0, separator);
			}
			else if( ! _tcsicmp( _T( "image_background" ), token ) )
			{
				TokenizePosition(
					mConfig.mBackgroundPosition,
					separator);
			}
			else if( ! _tcsicmp( _T( "image_close_up" ), token ) )
			{
				TokenizePosition(
					mConfig.mCloseUpPosition,
					separator);
			}
			else if( ! _tcsicmp( _T( "image_close_down" ), token ) )
			{
				TokenizePosition(
					mConfig.mCloseDownPosition,
					separator);
			}
			else if( ! _tcsicmp( _T( "image_checkBoxChecked" ), token ) )
			{
				TokenizePosition(
					mConfig.mCheckedPosition,
					separator);
			}
			else if( ! _tcsicmp( _T( "image_checkBoxUnchecked" ), token ) )
			{
				TokenizePosition(
					mConfig.mUncheckedPosition,
					separator);
			}
			else if( ! _tcsicmp( _T( "image_begin_up" ), token ) )
			{
				TokenizePosition(
					mConfig.mBeginUpPosition,
					separator);
			}
			else if( ! _tcsicmp( _T( "image_begin_down" ), token ) )
			{
				TokenizePosition(
					mConfig.mBeginDownPosition,
					separator);
			}
			else if( ! _tcsicmp( _T( "image_progressBar" ), token ) )
			{
				TokenizePosition(
					mConfig.mProgressPosition,
					separator);
			}
			else if( ! _tcsicmp( _T( "image_progressCell" ), token ) )
			{
				TokenizePosition(
					mConfig.mProgressCellPosition,
					separator);
			}
			else if( ! _tcsicmp( _T( "image_progressCellPack" ), token ) )
			{
				TokenizePosition(
					mConfig.mProgressCellPackPosition,
					separator);
			}
			else if( ! _tcsicmp( _T( "image_titleDecorator" ), token ) )
			{
				TokenizePosition(
					mConfig.mTitleDecoratorCellPosition,
					separator);
			}
			else if(0 ==  _tcsicmp(_T("text_title_position"), token))
			{
				TokenizePosition(
					mConfig.mTitleTextPosition,
					separator);
			}
			else if(0 ==  _tcsicmp(_T("text_version_position"), token))
			{
				TokenizePosition(
					mConfig.mVersionTextPosition,
					separator);
			}
			else if(0 ==  _tcsicmp(_T("text_versionnum_position"), token))
			{
				TokenizePosition(
					mConfig.mVersionNumTextPosition,
					separator);
			}
			else if(0 ==  _tcsicmp(_T("image_combo_top"), token))
			{
				TokenizePosition(
					mConfig.mTopCombo,
					separator);
			}
			else if(0 ==  _tcsicmp(_T("image_combo_top_over"), token))
			{
				TokenizePosition(
					mConfig.mTopOverCombo,
					separator);
			}
			else if(0 ==  _tcsicmp(_T("image_combo_middle"), token))
			{
				TokenizePosition(
					mConfig.mMiddleCombo,
					separator);
			}
			else if(0 ==  _tcsicmp(_T("image_combo_middle_over"), token))
			{
				TokenizePosition(
					mConfig.mMiddleOverCombo,
					separator);
			}
			else if(0 ==  _tcsicmp(_T("image_combo_bottom"), token))
			{
				TokenizePosition(
					mConfig.mBottomCombo,
					separator);
			}
			else if(0 ==  _tcsicmp(_T("image_combo_bottom_over"), token))
			{
				TokenizePosition(
					mConfig.mBottomOverCombo,
					separator);
			}
			else if(0 ==  _tcsicmp(_T("image_combo_cover"), token))
			{
				TokenizePosition(
					mConfig.mCoverCombo,
					separator);
			}
			else if(0 == _tcsicmp(_T("image_arrow"), token))
			{
				TokenizePosition(
					mConfig.mArrowPosition,
					separator);
			}
			else if(0 == _tcsicmp(_T("image_arrow_push"), token))
			{
				TokenizePosition(
					mConfig.mArrowPushedPosition,
					separator);
			}
			else if(0 == _tcsicmp(_T("text_windowMode_position"), token))
			{
				TokenizePosition(
					mConfig.mWindowModePosition,
					separator);
			}
			else if( ! _tcsicmp( _T( "text_caption" ), token ) )
			{
				mConfig.m_textCaption = GetString( line );
			}
			else if( ! _tcsicmp( _T( "text_title" ), token ) )
			{
				mConfig.m_textTitle = GetString( line );
			}
			else if( ! _tcsicmp( _T( "text_version" ), token ) )
			{
				mConfig.m_textVersion = GetString( line );
				CString clientVersion;
				CStdioFileEx file;

				if( file.Open( _T( "LunaVerInfo.ver" ), CFile::modeRead | CFile::typeText ) )
				{
					file.ReadString( clientVersion );
				}
				else
				{
					clientVersion = _T("0");
				}
				oVersionText = GetString( line );
				mConfig.m_textVersion = mConfig.m_textVersion + clientVersion;
			}
			else if( ! _tcsicmp( _T( "text_check" ), token ) )
			{
				mConfig.m_textCheck = GetString( line );
			}
			else if( ! _tcsicmp( _T( "text_windowMode" ), token ) )
			{
				mConfig.m_textWindowMode += GetString( line );
			}
			else if( ! _tcsicmp( _T( "text_packing" ), token ) )
			{
				mConfig.m_textPack = GetString( line );
			}
			else if( ! _tcsicmp( _T( "text_begin" ), token ) )
			{
				mConfig.m_textBegin = GetString( line );
			}
			else if( ! _tcsicmp( _T( "text_download" ), token ) )
			{
				mConfig.m_textDownload = GetString( line );
			}
			else if( ! _tcsicmp( _T( "text_extract" ), token ) )
			{
				mConfig.m_textExtract = GetString( line );
			}
			else if(0 == _tcsicmp(_T("text_interface"), token))
			{
                mConfig.mInterfaceTextList.push_back(
					GetString(line));
			}
			else if(0 == _tcsicmp(_T("text_interface_tip_text"), token))
			{
				mConfig.mInterfaceTip.mText = GetString(
					line);
			}
			else if(0 == _tcsicmp(_T("text_interface_tip_text_offset"), token))
			{
				LPCTSTR x = _tcstok(
					0,
					separator);
				LPCTSTR y = _tcstok(
					0,
					separator);

				mConfig.mInterfaceTip.mTextOffset.x = _ttoi(x ? x : _T(""));
				mConfig.mInterfaceTip.mTextOffset.y = _ttoi(y ? y : _T(""));
			}
			else if(0 == _tcsicmp(_T("text_interface_tip_image"), token))
			{
				TokenizePosition(
					mConfig.mInterfaceTip.mPosition,
					separator);
			}
			else if(0 == _tcsicmp(_T("text_resolution_tip_text"), token))
			{
				mConfig.mResolutionTip.mText = GetString(
					line);
			}
			else if(0 == _tcsicmp(_T("text_resolution_tip_text_offset"), token))
			{
				LPCTSTR x = _tcstok(
					0,
					separator);
				LPCTSTR y = _tcstok(
					0,
					separator);

				mConfig.mResolutionTip.mTextOffset.x = _ttoi(x ? x : _T(""));
				mConfig.mResolutionTip.mTextOffset.y = _ttoi(y ? y : _T(""));
			}
			else if(0 == _tcsicmp(_T("text_resolution_tip_image"), token))
			{
				TokenizePosition(
					mConfig.mResolutionTip.mPosition,
					separator);
			}
			else if(0 == _tcsicmp(_T("text_interface_loading_text"), token))
			{
				mConfig.mLoadingTip.mText = GetString( line );
				CString clientVersion;
				int patchVersion;
				CStdioFileEx file;

				if( file.Open( _T( "LunaVerInfo.ver" ), CFile::modeRead | CFile::typeText ) )
				{
					file.ReadString( clientVersion );
					const TCHAR* cstr = (LPCTSTR)clientVersion;
					patchVersion = _ttoi(cstr)+1;
				}
				else
				{
					clientVersion = _T("0");
				}
				oDownloadText = GetString( line );
				clientVersion.Format(_T("%d"),patchVersion);
				mConfig.mLoadingTip.mText = mConfig.mLoadingTip.mText + clientVersion;
			}
			else if(0 == _tcsicmp(_T("text_interface_loading_text_offset"), token))
			{
				LPCTSTR x	=	_tcstok( 
					0, 
					separator );
				LPCTSTR y	=	_tcstok( 
					0, 
					separator );

				mConfig.mLoadingTip.mTextOffset.x	=	_ttoi( x ? x : _T(""));
				mConfig.mLoadingTip.mTextOffset.y	=	_ttoi( y ? y : _T(""));
			}
			else if(0 == _tcsicmp(_T("text_interface_loading_image"), token))
			{
				TokenizePosition( 
					mConfig.mLoadingTip.mPosition, 
					separator );
			}	
			else if(0 == _tcsicmp(_T("text_interface_Packing_text"), token))
			{
				mConfig.mPackingTip.mText	=	GetString( 
					line );
			}
			else if(0 == _tcsicmp(_T("text_interface_Packing_text_offset"), token))
			{
				LPCTSTR	x	=	_tcstok(
					0,
					separator );
				LPCTSTR y	=	_tcstok(
					0,
					separator );

				mConfig.mPackingTip.mTextOffset.x	=	_ttoi( x ? x : _T(""));
				mConfig.mPackingTip.mTextOffset.y	=	_ttoi( y ? y : _T(""));
			}
			else if(0 == _tcsicmp(_T("text_interface_Packing_image"), token))
			{
				TokenizePosition(
					mConfig.mPackingTip.mPosition,
					separator );
			}
			else if(0 == _tcsicmp(_T("text_tip_text"), token))
			{
				LPCTSTR red = _tcstok(
					0,
					separator);
				LPCTSTR green = _tcstok(
					0,
					separator);
				LPCTSTR blue = _tcstok(
					0,
					separator);
				LPCTSTR weight = _tcstok(
					0,
					separator);
				LPCTSTR size = _tcstok(
					0,
					separator);

				Config::Font& font = mConfig.mInterfaceTip.mFont;
				font.mColor.rgbRed = BYTE(_ttoi(red ? red : _T("")));
				font.mColor.rgbGreen = BYTE(_ttoi(green ? green : _T("")));
				font.mColor.rgbBlue = BYTE(_ttoi(blue ? blue : _T("")));
				font.mWeight = _ttoi(weight ? weight : _T(""));
				font.mSize = _ttoi(size ? size : _T(""));

				mConfig.mResolutionTip.mFont = font;
				
				mConfig.mLoadingTip.mFont	=	font;
				mConfig.mPackingTip.mFont	=	font;
			}
			else if(0 == _tcsicmp(_T("text_tip_text_outline"), token))
			{
				LPCTSTR red = _tcstok(
					0,
					separator);
				LPCTSTR green = _tcstok(
					0,
					separator);
				LPCTSTR blue = _tcstok(
					0,
					separator);
				LPCTSTR border = _tcstok(
					0,
					separator);

				Config::Font& font = mConfig.mInterfaceTip.mFont;
				font.mBorderColor.rgbRed = BYTE(_ttoi(red ? red : _T("")));
				font.mBorderColor.rgbGreen = BYTE(_ttoi(green ? green : _T("")));
				font.mBorderColor.rgbBlue = BYTE(_ttoi(blue ? blue : _T("")));
				font.mBorderSize = _ttoi(border ? border : _T(""));

				mConfig.mResolutionTip.mFont = font;

				mConfig.mLoadingTip.mFont	=	font;
				mConfig.mPackingTip.mFont	=	font;
				// Firman Akbar 10/05/2022 Custom Button Launcher
				mConfig.mAliashingTip.mFont =	font;
				mConfig.mLanguageTip.mFont =	font;
			}
			else if( ! _tcsicmp( _T( "messageBox_caption" ), token ) )
			{
				mConfig.m_messageBoxCaption = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_selfUpdateFailed" ), token ) )
			{
				mConfig.m_messageBoxSelfUpdateFailed = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_notFoundVersionFile" ), token ) )
			{
				mConfig.m_messageBoxNotFoundVersionFile = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_incorrectVersionFile" ), token ) )
			{
				mConfig.m_messageBoxIncorrectVersionFile = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_failedVersionFileDelete" ), token ) )
			{
				mConfig.m_messageBoxFailedVersionFileDelete = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_failedVersionFileRename" ), token ) )
			{
				mConfig.m_messageBoxFailedVersionFileRename = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_failedCreateExecutive" ), token ) )
			{
				mConfig.m_messageBoxFailedCreateExecutive = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_failedRunClient" ), token ) )
			{
				mConfig.m_messageBoxFailedRunClient = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_failedExitDuringPacking" ), token ) )
			{
				mConfig.m_messageBoxFailedExitDuringPacking = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_failedConnect" ), token ) )
			{
				mConfig.m_messageBoxFailedConnect = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_failedVersionFileOpen" ), token ) )
			{
				mConfig.m_messageBoxInvalidateVersion = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_failedFileDownload" ), token ) )
			{
				mConfig.m_messageBoxFailedFileDownload = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_failedExtract" ), token ) )
			{
				mConfig.m_messageBoxFailedExtract = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_invalidateVersion" ), token ) )
			{
				mConfig.m_messageBoxInvalidateVersion = GetString( line );
			}
			//090819 ONS ���÷�ó���� �޼��� �߰�
            else if( ! _tcsicmp( _T( "messageBox_message_Pruple_InvalidArgument" ), token ) )
			{
				mConfig.m_messageBoxPrupleInvalidArgument = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_Pruple_InvalidHandle" ), token ) )
			{
				mConfig.m_messageBoxPrupleInvalidHandle = GetString( line );
			}
			else if( ! _tcsicmp( _T( "font_name" ), token ) )
			{
				mConfig.m_fontName = GetString( line );
			}
			else if( ! _tcsicmp( _T( "font_control_color" ), token ) )
			{
				LPCTSTR red = _tcstok(
					0,
					separator);
				LPCTSTR green = _tcstok(
					0,
					separator);
				LPCTSTR blue = _tcstok(
					0,
					separator);
				LPCTSTR weight = _tcstok(
					0,
					separator);
				LPCTSTR size = _tcstok(
					0,
					separator);

				mConfig.mControlFont.mColor.rgbRed = BYTE(_ttoi(red ? red : _T("")));
				mConfig.mControlFont.mColor.rgbGreen = BYTE(_ttoi(green ? green : _T("")));
				mConfig.mControlFont.mColor.rgbBlue = BYTE(_ttoi(blue ? blue : _T("")));
				mConfig.mControlFont.mWeight = _ttoi(weight ? weight : _T(""));
				mConfig.mControlFont.mSize = _ttoi(size ? size : _T(""));
			}
			else if( ! _tcsicmp( _T( "font_control_outline" ), token ) )
			{
				LPCTSTR red = _tcstok(
					0,
					separator);
				LPCTSTR green = _tcstok(
					0,
					separator);
				LPCTSTR blue = _tcstok(
					0,
					separator);
				LPCTSTR border = _tcstok(
					0,
					separator);

				mConfig.mControlFont.mBorderColor.rgbRed = BYTE(_ttoi(red ? red : _T("")));
				mConfig.mControlFont.mBorderColor.rgbGreen = BYTE(_ttoi(green ? green : _T("")));
				mConfig.mControlFont.mBorderColor.rgbBlue = BYTE(_ttoi(blue ? blue : _T("")));
				mConfig.mControlFont.mBorderSize = _ttoi(border ? border : _T(""));
			}
			else if( ! _tcsicmp( _T( "font_title_color" ), token ) )
			{
				LPCTSTR red = _tcstok(
					0,
					separator);
				LPCTSTR green = _tcstok(
					0,
					separator);
				LPCTSTR blue = _tcstok(
					0,
					separator);
				LPCTSTR weight = _tcstok(
					0,
					separator);
				LPCTSTR size = _tcstok(
					0,
					separator);

				mConfig.mTitleFont.mColor.rgbRed = BYTE(_ttoi(red ? red : _T("")));
				mConfig.mTitleFont.mColor.rgbGreen = BYTE(_ttoi(green ? green : _T("")));
				mConfig.mTitleFont.mColor.rgbBlue = BYTE(_ttoi(blue ? blue : _T("")));
				mConfig.mTitleFont.mWeight = _ttoi(weight ? weight : _T(""));
				mConfig.mTitleFont.mSize = _ttoi(size ? size : _T(""));
			}
			else if( ! _tcsicmp( _T( "font_title_outline" ), token ) )
			{
				LPCTSTR red = _tcstok(
					0,
					separator);
				LPCTSTR green = _tcstok(
					0,
					separator);
				LPCTSTR blue = _tcstok(
					0,
					separator);
				LPCTSTR border = _tcstok(
					0,
					separator);

				mConfig.mTitleFont.mBorderColor.rgbRed = BYTE(_ttoi(red ? red : _T("")));
				mConfig.mTitleFont.mBorderColor.rgbGreen = BYTE(_ttoi(green ? green : _T("")));
				mConfig.mTitleFont.mBorderColor.rgbBlue = BYTE(_ttoi(blue ? blue : _T("")));
				mConfig.mTitleFont.mBorderSize = _ttoi(border ? border : _T(""));
			}
			else if(0 == _tcsicmp(_T("font_windowMode_color"), token))
			{
				LPCTSTR red = _tcstok(
					0,
					separator);
				LPCTSTR green = _tcstok(
					0,
					separator);
				LPCTSTR blue = _tcstok(
					0,
					separator);
				LPCTSTR weight = _tcstok(
					0,
					separator);
				LPCTSTR size = _tcstok(
					0,
					separator);

				mConfig.mWindowModeFont.mColor.rgbRed = BYTE(_ttoi(red ? red : _T("")));
				mConfig.mWindowModeFont.mColor.rgbGreen = BYTE(_ttoi(green ? green : _T("")));
				mConfig.mWindowModeFont.mColor.rgbBlue = BYTE(_ttoi(blue ? blue : _T("")));
				mConfig.mWindowModeFont.mWeight = _ttoi(weight ? weight : _T(""));
				mConfig.mWindowModeFont.mSize = _ttoi(size ? size : _T(""));
			}
			else if( ! _tcsicmp( _T( "font_windowMode_outline" ), token ) )
			{
				LPCTSTR red = _tcstok(
					0,
					separator);
				LPCTSTR green = _tcstok(
					0,
					separator);
				LPCTSTR blue = _tcstok(
					0,
					separator);
				LPCTSTR border = _tcstok(
					0,
					separator);

				mConfig.mWindowModeFont.mBorderColor.rgbRed = BYTE(_ttoi(red ? red : _T("")));
				mConfig.mWindowModeFont.mBorderColor.rgbGreen = BYTE(_ttoi(green ? green : _T("")));
				mConfig.mWindowModeFont.mBorderColor.rgbBlue = BYTE(_ttoi(blue ? blue : _T("")));
				mConfig.mWindowModeFont.mBorderSize = _ttoi(border ? border : _T(""));
			}
			// Firman Akbar 10/05/2022 Custom Button Launcher
			else if(0 == _tcsicmp(_T("text_aliashing"), token))
			{
                mConfig.mAliashingTextList.push_back(
					GetString(line));
			}
			else if(0 == _tcsicmp(_T("text_aliashing_tip_text"), token))
			{
				mConfig.mAliashingTip.mText = GetString(
					line);
			}
			else if(0 == _tcsicmp(_T("text_aliashing_tip_text_offset"), token))
			{
				LPCTSTR x = _tcstok(
					0,
					separator);
				LPCTSTR y = _tcstok(
					0,
					separator);

				mConfig.mAliashingTip.mTextOffset.x = _ttoi(x ? x : _T(""));
				mConfig.mAliashingTip.mTextOffset.y = _ttoi(y ? y : _T(""));
			}
			else if(0 == _tcsicmp(_T("text_aliashing_tip_image"), token))
			{
				TokenizePosition(
					mConfig.mAliashingTip.mPosition,
					separator);
			}
			else if( ! _tcsicmp( _T( "control_aliashing_combo" ), token ) )
			{
				LPCTSTR x = _tcstok(
					0,
					separator);
				LPCTSTR y = _tcstok(
					0,
					separator);

				mConfig.mAliashingCombo.x = _ttoi(x ? x : _T(""));
				mConfig.mAliashingCombo.y = _ttoi(y ? y : _T(""));
			}
			else if(0 == _tcsicmp(_T("text_language"), token))
			{
                mConfig.mLanguageTextList.push_back(
					GetString(line)); 
			}
			else if(0 == _tcsicmp(_T("text_language_tip_text"), token))
			{
				mConfig.mLanguageTip.mText = GetString(
					line);
			}
			else if(0 == _tcsicmp(_T("text_language_tip_text_offset"), token))
			{
				LPCTSTR x = _tcstok(
					0,
					separator);
				LPCTSTR y = _tcstok(
					0,
					separator);

				mConfig.mLanguageTip.mTextOffset.x = _ttoi(x ? x : _T(""));
				mConfig.mLanguageTip.mTextOffset.y = _ttoi(y ? y : _T(""));
			}
			else if(0 == _tcsicmp(_T("text_language_tip_image"), token))
			{
				TokenizePosition(
					mConfig.mLanguageTip.mPosition,
					separator);
			}
			else if( ! _tcsicmp( _T( "control_language_combo" ), token ) )
			{
				LPCTSTR x = _tcstok(
					0,
					separator);
				LPCTSTR y = _tcstok(
					0,
					separator);

				mConfig.mLanguageCombo.x = _ttoi(x ? x : _T(""));
				mConfig.mLanguageCombo.y = _ttoi(y ? y : _T(""));
			}
		}

		CLog::Put( _T( "system\\setting.cfg is loaded" ) );		
	}
}

CApplication::~CApplication()
{
	// 080515 LUJ, �Ҵ�� �޸� �ʱ�ȭ
	for(
		POSITION position = mDownloadVariableList.GetHeadPosition();
		position; )
	{
		DownloadVariable* variable = mDownloadVariableList.GetNext( position );

		if( ! variable )
		{
			continue;
		}

		delete variable;
		variable = 0;

		CLog::Put( _T( "allocated memories are released" ) );
	}

	// 080515 LUJ, ���� ������ �����Ѵ�
	{
		CStringList record;
		CString		text;

		{
			text.Format(
				_T("resolution = %d %d\r\n"),
				mConfig.mResolution.cx,
				mConfig.mResolution.cy);

			record.AddTail( _T( "; resolution\r\n" ) );
			record.AddTail( _T( ";0 - 800 X 600\r\n" ) );
			record.AddTail( _T( ";1 - 1024 x 768\r\n" ) );
			record.AddTail( _T( ";2 - 1280 x 1024\r\n" ) );
			record.AddTail( text );
			record.AddTail( _T( "\r\n" ) );
		}

		{
			text.Format( _T( "windowMode = %s\r\n" ), mConfig.m_isWindowMode ? _T( "true" ) : _T( "false" ) );

			record.AddTail( _T( "; window mode\r\n" ) );
			record.AddTail( text );
			record.AddTail( _T( "\r\n" ) );
		}

		// 080507 LUJ, ���õ� �̹��� ��� ����
		{
			text.Format( _T( "imagePath = %s\r\n" ), mConfig.m_SelectedInterface );

			record.AddTail( _T( "; image path\r\n" ) );
			record.AddTail( text );
			record.AddTail( _T( "\r\n" ) );
		}
		// Firman Akbar 10/05/2022 Custom Button Launcher
		{
			text.Format( _T( "aliashing = %s\r\n" ), mConfig.m_SelectedAliashing );

			record.AddTail( _T( "; aliashing\r\n" ) );
			record.AddTail( text );
			record.AddTail( _T( "\r\n" ) );
		}
		{
			text.Format( _T( "languagePath = %s\r\n" ), mConfig.m_SelectedLanguage );

			record.AddTail( _T( "; language path\r\n" ) );
			record.AddTail( text );
			record.AddTail( _T( "\r\n" ) );
		}
		
		CStdioFileEx outputFile;

		if( ! outputFile.Open( _T( "system\\launcher.sav" ), CFile::modeCreate | CFile::modeWrite ) )
		{
			ASSERT( 0 && _T( "system\\launcher.sav is not found" ) );
			return;
		}

		for( POSITION pos = record.GetHeadPosition(); pos; )
		{
			const CString& line = record.GetNext( pos );

			outputFile.WriteString( line );
		}

		CLog::Put( _T( "Launcher.sav is saved" ) );

	}
}

//check if run as administrator
BOOL IsRunAsAdministrator()
{
    BOOL fIsRunAsAdmin = FALSE;
    DWORD dwError = ERROR_SUCCESS;
    PSID pAdministratorsGroup = NULL;

    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
    if (!AllocateAndInitializeSid(
        &NtAuthority, 
        2, 
        SECURITY_BUILTIN_DOMAIN_RID, 
        DOMAIN_ALIAS_RID_ADMINS, 
        0, 0, 0, 0, 0, 0, 
        &pAdministratorsGroup))
    {
        dwError = GetLastError();
        goto Cleanup;
    }

    if (!CheckTokenMembership(NULL, pAdministratorsGroup, &fIsRunAsAdmin))
    {
        dwError = GetLastError();
        goto Cleanup;
    }

Cleanup:

    if (pAdministratorsGroup)
    {
        FreeSid(pAdministratorsGroup);
        pAdministratorsGroup = NULL;
    }

    if (ERROR_SUCCESS != dwError)
    {
        throw dwError;
    }

    return fIsRunAsAdmin;
}

/////////////////////////////////////////////////////////////////////////////
// CApplication initialization

BOOL CApplication::InitInstance()
{
	if(!IsRunAsAdministrator())
	{
		MessageBox(NULL,_T("Please Right Click > Run as Administrator"),_T("Error"),MB_OK);
		ExitProcess(0);
	}
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifndef _FOR_SERVER_
	m_hMap = CreateFileMapping(
		(HANDLE)0xFFFFFFFF,
		NULL,
		PAGE_READWRITE,
		0,
		1024,
		_T( "Luna Online Client Application" ) );
	
	if( Local::TypeNone == mLocal.mType )
	{
		MsgBoxLog( _T( "Local is not defined in system\\setting.cfg" ), _T( "" ) );

		//CLog::Put( _T( "\tLocal is not set. Program'll exit. Check this in system\\setting.cfg: local = [type]" ) );

		PostQuitMessage( 0 );
		CloseHandle( m_hMap );

		return FALSE;
	}
	else if( m_hMap != NULL && GetLastError() == ERROR_ALREADY_EXISTS ) 
	{ 
		PostQuitMessage( 0 );
		CloseHandle( m_hMap );

		return FALSE;
	}
	else if( ! _tcsicmp( AfxGetApp()->m_lpCmdLine, _T( "INITFILELIST" ) ) )
	{
		PostQuitMessage( 0 );
		CloseHandle( m_hMap );

		return FALSE;
	}

	CoInitialize( 0 );
#endif

	CMainDialog dlg;
	m_pMainWnd = &dlg;

	//SetActiveWindow(
	//	m_pMainWnd->GetSafeHwnd());
	
	const INT_PTR response = dlg.DoModal();

	CoUninitialize();

	if ( response == IDOK )
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if ( response == IDCANCEL )
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
	else if( response == IDRETRY )
	{
		CLog::Put( _T( "LunaExecuter is running to patch LunaLauncher.exe" ) );
      
		STARTUPINFO	sp = { 0 };
		sp.cb			= sizeof(STARTUPINFO);
		sp.dwFlags		= STARTF_USEPOSITION | STARTF_USESIZE | STARTF_USESTDHANDLES; //STARTF_USESHOWWINDOW 
		sp.wShowWindow	= SW_SHOW; 

		PROCESS_INFORMATION pi;
		ZeroMemory( &pi, sizeof( pi ) );

		char CmdLine[256] = _T(" ");

		if( ! CreateProcess( 
			_T( "LunaExecuter.exe" ),
			CmdLine,											// command line string
			0,													// SD
			0,													// SD
			FALSE,												// handle inheritance option
			CREATE_DEFAULT_ERROR_MODE | NORMAL_PRIORITY_CLASS,  // creation flags
			0,													// new environment block
			0,													// current directory name
			&sp,												// startup information
			&pi	) )												// process information
		{
			CLog::Put( _T( "LunaExecuter.exe is failed to run" ) );
		}
	}
	
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.	
	return TRUE;
}

// 080515 LUJ, ���� ������ ���� ������ ��ȯ�Ѵ�
const CApplication::Local& CApplication::GetLocal() const
{
	return mLocal;
}

// 080515 LUJ, ���� ������ �����Ѵ�
void CApplication::SetLocal( Local::Type type )
{
	mLocal.mType = type;

	switch( type )
	{
    case Local::TypeWtaeOffline:
    {
        {
            Local::Download& download = mLocal.mDownload;
            download.mPath			= _T( "" );
            download.mVersionTag	= _T( "" );
            download.mVersionFile	= _T( "" );
        }
        {
            Local::URL& url = mLocal.mURL;
            url.mJoin		= _T( "" );
            url.mHomepage	= _T( "" );
            url.mContent	= _T( "" );
        }

        break;
    }
	case Local::TypeArcane:
		{
			Local::Download& download = mLocal.mDownload;
			Local::URL& url = mLocal.mURL;

			CStdioFileEx file;

			if( file.Open( _T("Launcher.ini"), CFile::modeRead | CFile::typeText ) )
			{
				for(CString line;
					file.ReadString(line);)
				{
					TCHAR buffer[ MAX_PATH ] = { 0 };
					_tcscpy( buffer, line );

					LPCTSTR separator = _T( "\n,= " );
					LPCTSTR token = _tcstok( buffer, separator );
				
					if( 0 == token		||		// empty line
						';' == token[ 0 ] ) 	// comment
					{
						continue;
					}
					else if (!_tcsicmp(_T("url"), token))
					{
						token = _tcstok(0, separator);
						if (token)
						{
							download.mPath = token;
						}
					}
					else if( ! _tcsicmp( _T( "homepage" ), token ) )
					{
						token = _tcstok( 0, separator );
						if( token )
						{
							url.mHomepage = token;
						}
					}
					else if( ! _tcsicmp( _T( "register" ), token ) )
					{
						token = _tcstok( 0, separator );
						if( token )
						{
							url.mJoin = token;
						}
					}
					
					else if( ! _tcsicmp( _T( "news" ), token ) )
					{
						token = _tcstok( 0, separator );
						if( token )
						{
							url.mContent = token;
						}
					}
				}

				file.Close();
			}
			else
			{
				download.mPath = _T("http://patch.arcaneluna.com:80/");
				url.mHomepage = _T("https://arcaneluna.com/");
				url.mContent = _T("https://launcher.arcaneluna.com/");
			}
			
			download.mVersionTag	= _T("Patch");
			download.mVersionFile	= _T("Version.txt");
			
			break;
		}
	case Local::TypeNone:
		{
			{
				Local::Download& download = mLocal.mDownload;

				download.mPath			= _T( "" );
				download.mVersionTag	= _T( "" );
				download.mVersionFile	= _T( "" );
			}

			{
				Local::URL& url = mLocal.mURL;

				url.mJoin		= _T( "" );
				url.mHomepage	= _T( "" );
				url.mContent	= _T( "" );
			}
			
			break;
		}
	}
}
	

// 080515 LUJ, ������ �ٿ�ε�ޱ� ���� �����带 �����Ѵ�
// 081201 LUJ, �ٿ�ε� ���� �� ó���� �Լ��� ������ �����Ѵ�
void CApplication::Download( const TCHAR* path, const TCHAR* file, CApplication::DiskCallBackFunction diskFunction, CApplication::MemoryCallBackFunction memoryFunction, CApplication::ProgressCallBackFunction progressFunction, CApplication::Config::FileType fileType )
{
	DownloadVariable* variable = new DownloadVariable;

	if( ! variable )
	{
		ASSERT( 0 && _T( "DownloadVariable is 0" ) );
		return;
	}

	mDownloadVariableList.AddTail( variable );
	
	variable->mPath				= path;
	variable->mFile				= file;
	variable->mDiskCallBack		= diskFunction;
	variable->mMemoryCallBack	= memoryFunction;
	variable->mProgressCallBack	= progressFunction;
	variable->mType				= DownloadVariable::TypeNone;

	switch( fileType )
	{
	case Config::FileTypeDisk:
		{
			variable->mType = DownloadVariable::TypeDisk;
			break;
		}
	case Config::FileTypeMemory:
		{
			variable->mType = DownloadVariable::TypeMemory;
			break;
		}
	}

	AfxBeginThread(
		CApplication::Download,
		( LPVOID )variable );
}

// 080515 LUJ, ������ �ٿ�ε�ް� �Ϸ� �� �ݹ� �Լ��� ����� �����Ѵ�. �����带 �����ؼ� ����Ǿ�� �Ѵ�
UINT CApplication::Download( LPVOID parameter )
{
	const DownloadVariable* variable = ( DownloadVariable* )parameter;

	if( ! variable )
	{
		ASSERT( 0 && _T( "DownloadVariable is 0" ) );
		return 1;
	}

	CNetwork::GetInstance().Download(
		variable->mPath,
		variable->mFile,
		variable->mProgressCallBack,
		DownloadVariable::TypeMemory == variable->mType);

	switch(variable->mType)
	{
	case DownloadVariable::TypeMemory:
		{
			if(const MemoryCallBackFunction memoryFunction	= variable->mMemoryCallBack)
			{
				( *memoryFunction )( CNetwork::GetInstance().GetMemoryFile() );
			}

			break;
		}
	case DownloadVariable::TypeDisk:
		{
			if(const DiskCallBackFunction diskFunction	= variable->mDiskCallBack)
			{
				( *diskFunction )( variable->mFile );
			}

			break;
		}
	}

	return 0;
}

// 080515 LUJ, ���� ������ ��ȯ�Ѵ�
CApplication::Config& CApplication::GetConfig()
{
	return mConfig;
}

// 080625 LUJ, ��Ʈ��ũ DLL�� �����Ѵ�
void CApplication::ReleaseNetwork() const
{
	CNetwork::GetInstance().Release();
}

#ifdef _LOCAL_JP_
// 090831 ONS ���ڷ� ���޵� URL�� �����Ѵ�.
void CApplication::SetLocalEx()
{
	if(m_Purple.m_ETCPath.IsEmpty()) return;

	Local::Download& download = mLocal.mDownload;
	download.mPath = m_Purple.m_ETCPath;
}
#endif

void CApplication::SetLoadingText(char* text)
{
	mConfig.mLoadingTip.mText = text;
}
