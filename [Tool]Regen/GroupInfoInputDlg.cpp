// GroupInfoInputDlg.cpp : $)C18Gv FD@O@T4O4Y.
//

#include "stdafx.h"
#include "RegenTool.h"
#include "GroupInfoInputDlg.h"


// CGroupInfoInputDlg 4kH- ;s@Z@T4O4Y.

IMPLEMENT_DYNAMIC(CGroupInfoInputDlg, CDialog)
CGroupInfoInputDlg::CGroupInfoInputDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGroupInfoInputDlg::IDD, pParent)
	, m_strGroupName(_T("General Group_1"))
	, m_dwTargetGroup(1)
	, m_fRatio(0.5f)
	, m_dwDelay(15000)
	, m_nIsRegen(1)
	, m_dwRegenRange(1000)
{
	m_bReadOnlyGroupName = m_bReadOnlyTargetGroup = m_bReadOnlyRatio = m_bReadOnlyDelay = m_bReadOnlyIsRegen = m_bReadOnlyRegenRange = FALSE;
}

CGroupInfoInputDlg::~CGroupInfoInputDlg()
{
}

void CGroupInfoInputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_GROUPNAME, m_strGroupName);
	DDX_Text(pDX, IDC_EDIT_TARGET_GROUP, m_dwTargetGroup);
	DDX_Text(pDX, IDC_EDIT_RATIO, m_fRatio);
	DDX_Text(pDX, IDC_EDIT_DELAY, m_dwDelay);
	DDX_Text(pDX, IDC_EDIT_REGEN, m_nIsRegen);
	DDV_MinMaxInt(pDX, m_nIsRegen, 0, 1);
	DDX_Text(pDX, IDC_EDIT_REGEN_RANGE, m_dwRegenRange);
	DDV_MinMaxInt(pDX, m_dwRegenRange, 1, 9999999);
	DDX_Control(pDX, IDC_EDIT_GROUPNAME, m_GroupName);
	DDX_Control(pDX, IDC_EDIT_TARGET_GROUP, m_TargetGroup);
	DDX_Control(pDX, IDC_EDIT_RATIO, m_Ratio);
	DDX_Control(pDX, IDC_EDIT_DELAY, m_Delay);
	DDX_Control(pDX, IDC_EDIT_REGEN, m_IsRegen);
	DDX_Control(pDX, IDC_EDIT_REGEN_RANGE, m_RegenRange);
}


BEGIN_MESSAGE_MAP(CGroupInfoInputDlg, CDialog)
END_MESSAGE_MAP()


// CGroupInfoInputDlg 8^=CAv C38.1b@T4O4Y.

BOOL CGroupInfoInputDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ?)1b?! C_0! CJ1bH- @[>w@; C_0!GU4O4Y.

	this->SetWindowText( m_strTitle );

	m_GroupName.SetReadOnly( m_bReadOnlyGroupName );
	m_TargetGroup.SetReadOnly( m_bReadOnlyTargetGroup );
	m_Ratio.SetReadOnly( m_bReadOnlyRatio );
	m_Delay.SetReadOnly( m_bReadOnlyDelay );
	m_IsRegen.SetReadOnly( m_bReadOnlyIsRegen );
	m_RegenRange.SetReadOnly( m_bReadOnlyRegenRange );

	return TRUE;  // return TRUE unless you set the focus to a control
	// ?9?\: OCX <S<: Fd@LAv4B FALSE8& 9]H/GX>_ GU4O4Y.
}
