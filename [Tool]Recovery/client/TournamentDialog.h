#pragma once

#include "afxwin.h"
#include "afxcmn.h"

class CclientApp;

// CTournamentDialog ��ȭ �����Դϴ�.

class CTournamentDialog : public CDialog
{
	DECLARE_DYNAMIC(CTournamentDialog)

public:
	CTournamentDialog( CclientApp&, CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CTournamentDialog();

	virtual void OnFinalRelease();
	void Parse( DWORD serverIndex, const MSGROOT* );
	void Request( const CString& serverName );

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_TOURNAMENTDIALOG };

private:
	CclientApp&	mApplication;
	DWORD		mServerIndex;
	DWORD		mMaxTournamentCount;
	DWORD		mSelectedGuildIndex;
	DWORD		mSelectedPlayerIndex;

	// 081027 LUJ, ��� �� ���� ����
	struct Unit
	{
		typedef MSG_RM_TOURNAMENT_GUILD::Guild	Guild;
		Guild									mGuild;
		// 081027 LUJ, ��: �÷��̾� ��ȣ
		typedef std::list< DWORD >				PlayerList;
		PlayerList								mPlayerList;
	};	
	// 081027 LUJ, Ű: ��� ��ȣ, ��: ��忡 ���� �÷��̾�
	typedef stdext::hash_map< DWORD, Unit >			UnitMap;
	// 081027 LUJ, ��� ��ʸ�Ʈ�� ���� ���� �÷��̾� ������ ����. Ű: ȸ��
	typedef stdext::hash_map< DWORD, UnitMap >		TournamentMap;
	TournamentMap									mTournamentMap;
	// 081027 LUJ, ��� �̸��� ����
	// 081027 LUJ, Ű: �ε���. ��: �̸�
	typedef stdext::hash_map< DWORD, CString >		NameMap;
	NameMap											mGuildNameMap;
	NameMap											mPlayerNameMap;
	// 081027 LUJ, ��ʸ�Ʈ ���� ��� ������ ǥ�õȴ�
	CTreeCtrl	mGuildTreeCtrl;
	// 081027 LUJ, ����ǥ ������ ǥ�õȴ�
	CTreeCtrl	mTableTreeCtrl;
	CComboBox	mServerCombo;
	CComboBox	mTournamentCountCombo;
	CStatic		mStatusStatic;
private:
	void RefreshGuildTree( CTreeCtrl&, const UnitMap& );
	void RefreshTableTree( CTreeCtrl&, const UnitMap& );
	CString	GetServerName()	const;
	void MakeHeader( CString& ) const;
	CString GetName( const NameMap&, DWORD index ) const;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnCbnSelchangeTournamentCountCombo();
	afx_msg void OnTvnSelchangedTournamentGuildTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnSelchangeTournamentServerCombo();
};