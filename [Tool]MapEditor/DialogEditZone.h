#pragma once
#include "afxcmn.h"

#include "./DataGrid/DataGrid.h"

// CDialogEditZone ��ȭ �����Դϴ�.

class CDialogEditZone : public CDialog
{
	DECLARE_DYNAMIC(CDialogEditZone)

public:
	CDialogEditZone(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDialogEditZone();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_ZONE_EDIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	afx_msg void OnShow();
	virtual void OnOK();
	virtual void OnCancel();

	DECLARE_MESSAGE_MAP()
public:

	virtual BOOL OnInitDialog();

	void SetZoneDesc(GXZONE_DESC* pDesc);

	void UpdateData();
	void UpdateGrid();

	void Update(BOOL bSave);
	void EnableView();
	void DisableView();

	inline BOOL IsShowing()	{	return m_bShow;	}

	BOOL		m_bShow;

public:

	GXZONE_DESC* m_pZoneDesc;
	CDataGrid    m_cGridZone;
};
