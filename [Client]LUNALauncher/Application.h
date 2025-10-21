#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


/////////////////////////////////////////////////////////////////////////////
// CApplication:
// See MHAutoPatch.cpp for the implementation of this class
//
// 081202 LUJ, �ٿ�ε带 �޸� ���� ���Ϸ� �޴´�
class CZipMemFile;

class CApplication : public CWinApp
{
protected:
	HANDLE	m_hMap;

public:
	CApplication();
	virtual ~CApplication();

	void ReleaseHandle() { if( m_hMap ) CloseHandle(m_hMap); }
	// 080625 LUJ, ��Ʈ��ũ DLL�� �����Ѵ�
	void ReleaseNetwork() const;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CApplication)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CApplication)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	struct Position
	{
		CRect mRect;
		CPoint mPoint;
	};
	struct Config
	{
		CPoint mResolutionCombo;
		CPoint mResolutionArrow;
		CPoint mInterfaceCombo;
		CPoint mInterfaceArrow;
		// 080507 LUJ, �̹��� ��� ���� ��Ʈ�� ��ġ
		typedef std::list< CString > TextList;
		TextList mInterfaceTextList;

		bool m_isWindowMode;

		CString mImage;
		Position mHtmlPosition;
		Position mServerStatusPosition;
		Position mBackgroundPosition;
		Position mCloseUpPosition;
		Position mCloseDownPosition;
		Position mBeginUpPosition;
		Position mBeginDownPosition;
		Position mCheckedPosition;
		Position mUncheckedPosition;
		Position mWindowModePosition;
		Position mProgressPosition;
		Position mProgressCellPosition;
		Position mProgressCellPackPosition;
		Position mTitleDecoratorCellPosition;
		Position mTitleTextPosition;
		Position mVersionTextPosition;
		Position mArrowPosition;
		Position mArrowPushedPosition;
		Position mTopCombo;
		Position mTopOverCombo;
		Position mMiddleCombo;
		Position mMiddleOverCombo;
		Position mBottomCombo;
		Position mBottomOverCombo;
		Position mCoverCombo;
		Position mVersionNumTextPosition;
		Position mDiscordButton;
		Position mDiscordButtonHover;
		Position mFacebookButton;
		Position mFacebookButtonHover;
		
		struct Font
		{
			RGBQUAD mColor;
			int mWeight;
			int mSize;
			RGBQUAD mBorderColor;
			int mBorderSize;
		};
		Font mControlFont;
		Font mTitleFont;
		Font mWindowModeFont;

		struct Tip
		{
			CString mText;
			CPoint mTextOffset;
			Position mPosition;
			Font mFont;
		};
		Tip mInterfaceTip;
		Tip mResolutionTip;
		Tip mLoadingTip;
		Tip mPackingTip;
		
		// 080507 LUJ, ���õ� �̹��� ��� ���� �̸�
		CString m_SelectedInterface;
		
		// Firman Akbar 10/05/2022 Custom Button Launcher
		CString m_SelectedAliashing;
		Tip mAliashingTip;
		CPoint mAliashingCombo;
		CPoint mAliashingArrow;
		TextList mAliashingTextList;
		 
		CString m_SelectedLanguage;
		Tip mLanguageTip;
		CPoint mLanguageCombo;
		CPoint mLanguageArrow;
		TextList mLanguageTextList;
		
		CString m_textCaption;
		CString m_textTitle;
		CString m_textWindowMode;
		CString m_textCheck;
		CString m_textPack;
		CString m_textBegin;
		CString m_textDownload;
		CString m_textExtract;
		CString m_textVersion;
		CString m_textVersionNum;

		CString m_messageBoxCaption;
		CString m_messageBoxSelfUpdateFailed;
		CString m_messageBoxNotFoundVersionFile;
		CString m_messageBoxIncorrectVersionFile;
		CString m_messageBoxFailedVersionFileDelete;
		CString m_messageBoxFailedVersionFileRename;
		CString m_messageBoxFailedCreateExecutive;
		CString m_messageBoxFailedRunClient;
		CString m_messageBoxFailedExitDuringPacking;
		CString m_messageBoxFailedConnect;
		CString m_messageBoxInvalidateVersion;
		CString m_messageBoxFailedVersionFileOpen;
		CString m_messageBoxFailedFileDownload;
		CString m_messageBoxFailedExtract;

		// 090819 ONS ���÷�ó���� �޼��� �߰�
		CString m_messageBoxPrupleInvalidArgument;
		CString m_messageBoxPrupleInvalidHandle;
		CString m_fontName;
		
		// 081202 LUJ, �ٿ�ε���� �����͸� ��� �������� �����Ѵ�
		enum FileType
		{
			FileTypeNone,
			FileTypeDisk,
			FileTypeMemory,
		}
		mFileType;
		CSize mResolution;

		Config() :
		m_isWindowMode(false),
		mFileType(FileTypeDisk)
		{
			ZeroMemory(
				&mControlFont,
				sizeof(mControlFont));
			ZeroMemory(
				&mTitleFont,
				sizeof(mTitleFont));
		}
	};

	// 080515 LUJ, ���� ������ ��ȯ�Ѵ�
	Config& GetConfig();
	
	void SetLoadingText(char* text);
	CString GetVersionText(){return oVersionText;}
	CString GetLoadingText(){return oDownloadText;}
	
private:
	Config mConfig;
	//original Text 
	CString oVersionText;
	CString oDownloadText;
	// 080515 LUJ, ���� ���� ó��
public:
	struct Local
	{
		enum Type
		{
			TypeNone,
			// Firman Akbar 10/05/2022 Custom Launcher
			TypeArcane,
			TypeWtaeOffline,
		}
		mType;

		struct Download
		{
			CString mPath;
			CString mVersionTag;
			CString mVersionFile;
		}
		mDownload;

		struct URL
		{
			CString mJoin;
			CString mHomepage;
			CString mContent;
		}
		mURL;
	};

	// 080515 LUJ, ���� ������ ���� ������ ��ȯ�Ѵ�
	const Local& GetLocal() const;

private:
	// 080515 LUJ, ���� ������ �����Ѵ�
	void SetLocal( Local::Type );

	Local mLocal;

	// 080515 LUJ, �ٿ�ε� ó��
public:
	// 080515 LUJ, ��� �� ���� ó���� ���� �ݹ� �Լ� ������ ����
	typedef void ( *DiskCallBackFunction )		( const TCHAR* );
	typedef void ( *MemoryCallBackFunction )	( CZipMemFile& );
	// 090819 ONS ���÷�óUI�� ���ۼӵ��� �����ð��� �����ϱ����� ����
	typedef void ( *ProgressCallBackFunction )	( double percent, double remaintime, double down_speed );
	// 080515 LUJ, ������ �ٿ�ε�ް� �Ϸ� �� �ݹ� �Լ��� ����� �����Ѵ�
	// 081201 LUJ, �ٿ�ε� ���� �� ó���� �ݹ� �Լ��� �����Ѵ�
	// 081202 LUJ, �ٿ�ε���� �����͸� �޸𸮿� ���� �����ϴ� �÷��� �߰�
	void Download( const TCHAR* path, const TCHAR* file, DiskCallBackFunction, MemoryCallBackFunction, ProgressCallBackFunction, CApplication::Config::FileType );	

private:
	struct DownloadVariable
	{
		// 081201 LUJ, �����͸� � ���·� ������ �����ϴ� �÷���
		enum Type
		{
			TypeNone,
			TypeDisk,
			TypeMemory,
		}
		mType;
		CString						mPath;
		CString						mFile;
		DiskCallBackFunction		mDiskCallBack;
		MemoryCallBackFunction		mMemoryCallBack;
		ProgressCallBackFunction	mProgressCallBack;

		DownloadVariable() :
			mDiskCallBack( 0 ),
			mMemoryCallBack( 0 ),
			mProgressCallBack( 0 )
		{}
	};

	// 080515 LUJ, ���� �Ҵ�� �ٿ�ε� ���� �����̳�
	typedef CList< DownloadVariable*, DownloadVariable*& >	DownloadVariableList;
	DownloadVariableList									mDownloadVariableList;

	// 080515 LUJ, ������ �ٿ�ε�ް� �Ϸ� �� �ݹ� �Լ��� ����� �����Ѵ�. ������� ����Ǿ�� �Ѵ�
	static UINT Download( LPVOID );

#ifdef _LOCAL_JP_
	struct PurpleInfo
	{
		HWND		m_hTarget;
		CString		m_LauncherPath;
		CString		m_GameInstallPath;
		CString		m_ETCPath;
	}
	m_Purple;

public:
	HWND			GetTargetHandle() { return m_Purple.m_hTarget; }
	// 090819 ONS ���÷�ó�κ��� ���޵� �����ּҸ� ����
	void SetLocalEx();

#endif
};

extern CApplication theApp;
