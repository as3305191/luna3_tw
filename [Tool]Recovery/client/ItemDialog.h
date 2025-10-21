#pragma once
#include "afxwin.h"
#include "afxcmn.h"

class CclientApp;

class CItemDialog : public CDialog
{
	DECLARE_DYNAMIC(CItemDialog)

public:
	CItemDialog( CclientApp&, CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CItemDialog();
	virtual void OnFinalRelease();
	void Parse( DWORD serverIndex, const MSGROOT* );
	void Request( DWORD serverIndex, const CString& playerName, DWORD playerIndex, DWORD userIndex );

protected:
	CclientApp&	mApplication;	
	// ���� �� �������� �Ϲ����� ������ ǥ��;
	CGridCtrl	mItemGrid;
	// ������ �ɼ� ������ ǥ��
	CGridCtrl	mOptionGrid;
	// 080320 LUJ, ��� �ɼ� �׸���
	CGridCtrl	mDropOptionGrid;
	// 080716 LUJ, �� �׸���
	CGridCtrl	mPetGrid;

	DWORD		mServerIndex;
	DWORD		mSelectedItemDbIndex;

	CString		mPlayerName;
	DWORD		mPlayerIndex;
	DWORD		mUserIndex;
	
	DWORD		mStorageSize;
	DWORD		mExtendedInventorySize;

	// ���õ� �����͸� ǥ���Ѵ�
	void PutData( DWORD itemDbIndex );
	// 080401 LUJ, ��ȯ Ÿ���� Item���� ����
	// 080731 LUJ, ������ ��ȯ�ϵ��� ��
	Item&		GetItem( DWORD itemDbIndex );
	// 080731 LUJ, Ư�� �κ��丮���� ������ ������ ���� ��ģ��
	void		PutItem( const Item&, CListCtrl&, bool isAdd = false );
	// 080731 LUJ, ��� �κ��丮����  ������ ������ ���� ��ģ��
	void		PutItem( const Item&, bool isAdd = false );
	// 081201 LUJ, �ɼ��� ���ڿ��� ����Ѵ�
	CString		MakeText( const ITEM_OPTION& ) const;
	// Ű: ������ DB�ε���
	typedef stdext::hash_map< DWORD, Item >	ItemMap;
	ItemMap									mItemMap;
	
	// Ű: ������ DB�ε���
	typedef stdext::hash_map< DWORD, ITEM_OPTION >	OptionMap;
	OptionMap										mOptionMap;

	// 080716 LUJ, �� ������ ����� ��. Ű: ������ DB�ε���
	typedef MSG_RM_PET::Pet					Pet;
	typedef stdext::hash_map< DWORD, Pet >	PetMap;
	PetMap									mPetMap;

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ITEMDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnLvnItemchangedItemDialogInventory(NMHDR *pNMHDR, LRESULT *pResult);
	//afx_msg void OnBnClickedItemDialogDataUpdate();
	afx_msg void OnBnClickedItemDialogDataUpdateButton();
	//afx_msg void OnLvnItemchangedItemStorage(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedItemDataDeleteButton();
	//CStatic mPlayerIndexEdit;
	CStatic mDataFrameStatic;
	afx_msg void OnBnClickedItemInventoryLogButton();
	
	afx_msg void OnBnClickedItemAddButton();
	CStatic mDataFrame;
	CEdit mEndTimeStatic;
	CEdit mRemainTimeStatic;
	//afx_msg void OnEnSetfocusItemEndTimeStatic();
	//afx_msg void OnEnSetfocusItemRemainTimeStatic();
	afx_msg void OnBnClickedItemEndTimeButton();
	afx_msg void OnBnClickedItemRemainTimeButton();
	CComboBox mStorageSizeCombo;
	CEdit mStorageMoneyEdit;
	// 080611 LUJ, ���� ��� ��ư
	CButton	mSealingRadioButton;
	CTabCtrl mValueTabCtrl;
	afx_msg void OnTcnSelchangeItemTab(NMHDR *pNMHDR, LRESULT *pResult);
	CButton mItemAddButton;
	afx_msg void OnBnClickedItemStorageUpdateButton();
	CButton mStorageUpdateButton;
	CButton mSafeMoveCheckBox;

	// 080328 LUJ, ���� ǥ��
	afx_msg void OnBnClickedItemHelpButton();
	afx_msg void OnBnClickedItemDialogSealRadio();
	CButton mRemainTimeButton;
	CButton mEndTimeButton;
	CButton mUpdateDataButton;
	CListCtrl mInventoryListCtrl;
	CListCtrl mPetInventoryListCtrl;
	CListCtrl mHouseInventoryListCtrl;
	CTabCtrl mInventoryTabCtrl;
	// 080716 LUJ, �κ��丮 ���� �ٲ� ������ ǥ�õ� ����Ʈ ��Ʈ���� �����Ѵ�
	afx_msg void OnTcnSelchangeItemDialogInventoryTab(NMHDR *pNMHDR, LRESULT *pResult);
	// 080716 LUJ, �� �κ��丮 ��� �߿��� ������ ��� �����
	afx_msg void OnLvnItemchangedItemPetInventoryList(NMHDR *pNMHDR, LRESULT *pResult);
	CButton mPetSummonButton;
	afx_msg void OnBnClickedItemPetSummonButton();
	afx_msg void OnBnClickedItemCopyClipboard();
	afx_msg void OnBnClickedItemSaveExcelButton();
};
