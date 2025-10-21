#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CItemChangeDialog ��ȭ �����Դϴ�.

class CItemChangeDialog : public CDialog
{
	DECLARE_DYNAMIC(CItemChangeDialog)

public:
	CItemChangeDialog( CclientApp&, CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CItemChangeDialog();

	virtual void OnFinalRelease();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ITEMCHANGEDIALOG };

private:
	CclientApp& mApplication;

	// 090304 LUJ, ��ũ��Ʈ�� �д´�
private:
	struct ChangeScript
	{
		ChangeScript() :
		mItemIndex( 0 ),
		mRate( 0 )
		{}

		DWORD	mItemIndex;
		typedef size_t MinimumCount;
		typedef size_t MaximumCount;
		typedef std::pair< MinimumCount, MaximumCount > Count;
		Count	mCount;
		typedef float Rate;
		Rate mRate;
	};
	typedef float Rate;
	typedef std::map< Rate, ChangeScript > ChangeScriptMap;
	typedef std::list< ChangeScriptMap > ChangeScriptMapList;
	typedef DWORD ItemIndex;
	typedef stdext::hash_map< ItemIndex, ChangeScriptMapList > ItemChangeScriptMap;
	static ItemChangeScriptMap mItemChangeScriptMap;
	const int mMaxChangeCount;
	const int mDefaultChangecount;
private:
	void LoadScript();
	void SetItemCombo( CComboBox& );
	ItemIndex GetSelectedItemIndex() const;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	
	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

private:
	CListCtrl mResultListCtrl;
	CComboBox mItemComboBox;
	CStatic mItemDescriptionStatic;
	CToolTipCtrl mItemToolTipCtrl;
	CButton mSubmitButton;

public:
	virtual BOOL OnInitDialog();	
	afx_msg void OnCbnSelchangeItemchangeItemList();
	afx_msg void OnBnClickedItemChangeSubmitButton();
	afx_msg void OnBnClickedItemChangeCopyClipboardButton();
	virtual BOOL PreTranslateMessage( MSG* );
	afx_msg void OnBnClickedItemChangeFindButton();
	CEdit mCountEdit;
	CButton mCountMaxButton;
	afx_msg void OnBnClickedItemChangeCountMaxButton();
};