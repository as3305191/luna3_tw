/*
081021 LUJ, ���� ���� â �߰�
*/
#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CAuthorityDialog ��ȭ �����Դϴ�.

class CAuthorityDialog : public CDialog
{
	DECLARE_DYNAMIC(CAuthorityDialog)

public:
	CAuthorityDialog( CclientApp&, CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CAuthorityDialog();

	virtual void OnFinalRelease();
	void Parse( DWORD serverIndex, const MSGROOT* );
	virtual BOOL OnInitDialog();
	virtual INT_PTR DoModal( DWORD serverIndex );
	virtual void OnOK();
	
// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_AUTHORITYDIALOG };
private:
	// 081021 LUJ, �ǿ��� ���õ� ������ �����´�
	eGM_POWER	GetRankFromTab() const;
private:
	CclientApp& mApplication;
	DWORD		mServerIndex;
	// 081021 LUJ, �������� ���۵� ���� ����
	typedef std::list< MSG_RM_AUTHORITY::Authority >		AuthorityList;
	typedef stdext::hash_map< eGM_POWER, AuthorityList >	AuthorityMap;
	AuthorityMap											mAuthorityMap;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
public:
	CTabCtrl mLevelTabCtrl;
	CButton mPlayerLevelReadButton;
	CButton mPlayerLevelWriteButton;
	CButton mPlayerCreateReadButton;
	CButton mPlayerCreateWriteButton;
	CButton mPlayerDeleteReadButton;
	CButton mPlayerDeleteWriteButton;
	CButton mPlayerRecoverReadButton;
	CButton mPlayerRecoverWriteButton;
	CButton mPlayerUpdateReadButton;
	CButton mPlayerUpdateWriteButton;
	CButton mSkillReadButton;
	CButton mSkillWriteButton;
	CButton mQuestReadButton;
	CButton mQuestWriteButton;
	CButton mGuildReadButton;
	CButton mGuildWriteButton;
	CButton mFamilyReadButton;
	CButton mFamilyWriteButton;
	CButton mPlayerLogReadButton;
	CButton mPlayerLogWriteButton;
	CButton mOperatorReadButton;
	CButton mOperatorWriteButton;
	CButton mAllowIpReadButton;
	CButton mAllowIpWriteButton;
	CButton mOperatorLogReadButton;
	CButton mOperatorLogWriteButton;
	CButton mItemReadButton;
	CButton mItemWriteButton;
	afx_msg void OnTcnSelchangeAuthorityTab(NMHDR *pNMHDR, LRESULT *pResult);
};