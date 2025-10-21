// ItemInsertDialog.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "client.h"
#include "ItemAddDialog.h"
#include "ItemDialog.h"
#include ".\itemadddialog.h"


// CItemAddDialog ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CItemAddDialog, CDialog)
CItemAddDialog::CItemAddDialog( CclientApp& application, CWnd* pParent /*=NULL*/)
	: CDialog(CItemAddDialog::IDD, pParent),
	mApplication( application )
{
	EnableAutomation();

}

CItemAddDialog::~CItemAddDialog()
{
}

void CItemAddDialog::OnFinalRelease()
{
	// �ڵ�ȭ ��ü�� ���� ������ ������ �����Ǹ� OnFinalRelease��
	// ȣ��˴ϴ�. �⺻ Ŭ�������� �ڵ����� ��ü�� �����մϴ�.
	// �⺻ Ŭ������ ȣ���ϱ� ���� ��ü�� �ʿ��� �߰� ���� �۾���
	// �߰��Ͻʽÿ�.

	CDialog::OnFinalRelease();
}

void CItemAddDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_ITEM_ADD_TAB, mTabCtrl);
	DDX_Control(pDX, IDC_ITEM_ADD_TAB, mTabCtrl);
	//DDX_Control(pDX, IDC_ITEM_ADD_LIST, mItemListCtrl1);
	DDX_Control(pDX, IDC_ITEM_ADD_LIST_WEAPON, mWeaponListCtrl);
	DDX_Control(pDX, IDC_ITEM_ADD_LIST_POTION, mPotionListCtrl);
	DDX_Control(pDX, IDC_ITEM_ADD_LIST_MATERIAL, mMaterialListCtrl);
	//DDX_Control(pDX, IDC_ITEM_ADD_LIST_QUEST, mQuestListCtrl);
	DDX_Control(pDX, IDC_ITEM_ADD_INDEX_EDIT, mItemIndexEdit);
	DDX_Control(pDX, IDC_ITEM_ADD_NAME_STATIC, mItemNameStatic);
	DDX_Control(pDX, IDC_ITEM_ADD_COMBO, mItemNameCombo);
	DDX_Control(pDX, IDC_ITEM_ADD_LIST_ETC, mEtcListCtrl);
}


BEGIN_MESSAGE_MAP(CItemAddDialog, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_ITEM_ADD_TAB, OnTcnSelchangeItemAddTab)
	//ON_BN_CLICKED(IDC_ITEM_ADD_SUBMIT_BUTTON, OnBnClickedItemAddSubmitButton)
	ON_EN_CHANGE(IDC_ITEM_ADD_INDEX_EDIT, OnEnChangeItemAddIndexEdit)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_ITEM_ADD_LIST_WEAPON, OnLvnItemchangedItemAddListWeapon)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_ITEM_ADD_LIST_POTION, OnLvnItemchangedItemAddListPotion)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_ITEM_ADD_LIST_MATERIAL, OnLvnItemchangedItemAddListMaterial)
	ON_CBN_SELCHANGE(IDC_ITEM_ADD_COMBO, OnCbnSelchangeItemAddCombo)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_ITEM_ADD_LIST_ETC, OnLvnItemchangedItemAddListEtc)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CItemAddDialog, CDialog)
END_DISPATCH_MAP()

// ����: IID_IItemInsertDialog�� ���� ������ �߰��Ͽ�
// VBA���� ���� �������� ���ε��� �����մϴ�. 
// �� IID�� .IDL ���Ͽ� �ִ� dispinterface�� GUID�� ��ġ�ؾ� �մϴ�.

// {0CDDD890-B3DF-4A40-A0FC-562B97CBBBEB}
static const IID IID_IItemInsertDialog =
{ 0xCDDD890, 0xB3DF, 0x4A40, { 0xA0, 0xFC, 0x56, 0x2B, 0x97, 0xCB, 0xBB, 0xEB } };

BEGIN_INTERFACE_MAP(CItemAddDialog, CDialog)
	INTERFACE_PART(CItemAddDialog, IID_IItemInsertDialog, Dispatch)
END_INTERFACE_MAP()


// CItemAddDialog �޽��� ó�����Դϴ�.

BOOL CItemAddDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	SetIcon( AfxGetApp()->LoadIcon( IDR_MAINFRAME ), FALSE );
	SetIcon( AfxGetApp()->LoadIcon( IDR_MAINFRAME ), TRUE );

	// �� �ʱ�ȭ
	{
		const DWORD addStyle = LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP;
		CRect rect;

		// ��� ��
		{
			CString textEquippment;
			textEquippment.LoadString( IDS_STRING53 );
			mTabCtrl.InsertItem( mTabCtrl.GetItemCount(), textEquippment );

			mWeaponListCtrl.GetClientRect( rect );

			CString textIndex;
			textIndex.LoadString( IDS_STRING22 );
			mWeaponListCtrl.InsertColumn( 0, textIndex, LVCFMT_LEFT, 0 );

			CString textName;
			textName.LoadString( IDS_STRING20 );
			mWeaponListCtrl.InsertColumn( 1, textName, LVCFMT_LEFT, int( rect.Width() * 0.7f ) );

			CString textLevel;
			textLevel.LoadString( IDS_STRING5 );
			mWeaponListCtrl.InsertColumn( 2, textLevel, LVCFMT_LEFT, int( rect.Width() * 0.18f ) );

			mWeaponListCtrl.SetExtendedStyle( mWeaponListCtrl.GetExtendedStyle() | addStyle );
			mWeaponListCtrl.ShowWindow( SW_HIDE );
			
			mApplication.SetItemList( mWeaponListCtrl, eItemCategory_Equip );
		}

		// ���� ��
		{
			CString textPotion;
			textPotion.LoadString( IDS_STRING59 );
			mTabCtrl.InsertItem( mTabCtrl.GetItemCount(), textPotion );

			mPotionListCtrl.GetClientRect( rect );

			CString textIndex;
			textIndex.LoadString( IDS_STRING22 );
			mPotionListCtrl.InsertColumn( 0, textIndex, LVCFMT_LEFT, 0 );

			CString textName;
			textName.LoadString( IDS_STRING20 );
			mPotionListCtrl.InsertColumn( 1, textName, LVCFMT_LEFT, int( rect.Width() * 0.7f ) );

			CString textLevel;
			textLevel.LoadString( IDS_STRING5 );

			mPotionListCtrl.InsertColumn( 2, textLevel, LVCFMT_LEFT, int( rect.Width() * 0.18f ) );
			mPotionListCtrl.SetExtendedStyle( mPotionListCtrl.GetExtendedStyle() | addStyle );
			mPotionListCtrl.ShowWindow( SW_HIDE );
			
			mApplication.SetItemList( mPotionListCtrl, eItemCategory_Expend );
		}

		// ��� ��
		{
			CString textMaterial;
			textMaterial.LoadString( IDS_STRING60 );
			mTabCtrl.InsertItem( mTabCtrl.GetItemCount(), textMaterial );

			mMaterialListCtrl.GetClientRect( rect );

			CString textIndex;
			textIndex.LoadString( IDS_STRING22 );
			mMaterialListCtrl.InsertColumn( 0, textIndex, LVCFMT_LEFT, 0 );

			CString textName;
			textName.LoadString( IDS_STRING20 );
			mMaterialListCtrl.InsertColumn( 1, textName, LVCFMT_LEFT, int( rect.Width() * 0.7f ) );

			CString textLevel;
			textLevel.LoadString( IDS_STRING5 );
			mMaterialListCtrl.InsertColumn( 2, textLevel, LVCFMT_LEFT, int( rect.Width() * 0.18f ) );

			mMaterialListCtrl.SetExtendedStyle( mMaterialListCtrl.GetExtendedStyle() | addStyle );
			mMaterialListCtrl.ShowWindow( SW_HIDE );
			
			mApplication.SetItemList( mMaterialListCtrl, eItemCategory_Material );
		}

		// ��Ÿ ��
		{
			CString textEtc;
			textEtc.LoadString( IDS_STRING61 );
			mTabCtrl.InsertItem( mTabCtrl.GetItemCount(), textEtc );

			mEtcListCtrl.GetClientRect( rect );

			CString textIndex;
			textIndex.LoadString( IDS_STRING22 );
			mEtcListCtrl.InsertColumn( 0, textIndex, LVCFMT_LEFT, 0 );

			CString textName;
			textName.LoadString( IDS_STRING20 );
			mEtcListCtrl.InsertColumn( 1, textName, LVCFMT_LEFT, int( rect.Width() * 0.7f ) );

			CString textLevel;
			textLevel.LoadString( IDS_STRING5 );

			mEtcListCtrl.InsertColumn( 2, textLevel, LVCFMT_LEFT, int( rect.Width() * 0.18f ) );
			mEtcListCtrl.SetExtendedStyle( mEtcListCtrl.GetExtendedStyle() | addStyle );
			mEtcListCtrl.ShowWindow( SW_HIDE );
			
			mApplication.SetItemList( mEtcListCtrl, eItemCategory_Etc );
		}

		mTabCtrl.SetCurSel( 0 );
		mWeaponListCtrl.ShowWindow( SW_SHOW );
	}

	// �޺�
	{
		CRect rect;
		mItemNameCombo.GetWindowRect( rect );
		mItemNameCombo.SetWindowPos( 0, 0, 0, rect.Width(), 200, SWP_NOMOVE | SWP_NOZORDER );
	}

	{
		mItemIndexEdit.SetFocus();
	}

	// 090219 LUJ, ����
	{
		mItemToolTipCtrl.Create( this );
		mItemToolTipCtrl.Activate( TRUE );
		mItemToolTipCtrl.SetMaxTipWidth( 200 );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


void CItemAddDialog::OnTcnSelchangeItemAddTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	mWeaponListCtrl.ShowWindow( SW_HIDE );
	mPotionListCtrl.ShowWindow( SW_HIDE );
	mMaterialListCtrl.ShowWindow( SW_HIDE );
	mEtcListCtrl.ShowWindow( SW_HIDE );
	
	switch( mTabCtrl.GetCurSel() )
	{
	case 0:
		{
			mWeaponListCtrl.ShowWindow( SW_SHOW );
			break;
		}
	case 1:
		{
			mPotionListCtrl.ShowWindow( SW_SHOW );
			break;
		}
	case 2:
		{
			mMaterialListCtrl.ShowWindow( SW_SHOW );
			break;
		}
	case 3:
		{
			mEtcListCtrl.ShowWindow( SW_SHOW );
			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}

	RedrawWindow();
}


DWORD CItemAddDialog::GetSelectedItemIndex() const
{
	return mSelectedItemIndex;
}


void CItemAddDialog::OnOK()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	CListCtrl* listCtrl = 0;

	switch( mTabCtrl.GetCurSel() )
	{
	case 0:
		{
			listCtrl = &mWeaponListCtrl;
			break;
		}
	case 1:
		{
			listCtrl = &mPotionListCtrl;
			break;
		}
	case 2:
		{
			listCtrl = &mMaterialListCtrl;
			break;
		}
		/*
	case 3:
		{
			listCtrl = &mQuestListCtrl;
			break;
		}
	case 4:
		{
			listCtrl = &mEtcListCtrl;
			break;
		}
		*/
	}

	if( ! listCtrl )
	{
		ASSERT( 0 );
		return;
	}

	CString item;

	mItemIndexEdit.GetWindowText( item );

	if( item.IsEmpty() )
	{
		CString textYouShouldInputIndexOrNameOfItem;
		textYouShouldInputIndexOrNameOfItem.LoadString( IDS_STRING63 );

		MessageBox( textYouShouldInputIndexOrNameOfItem, _T( "" ), MB_ICONWARNING | MB_OK );
		return;
	}

	mSelectedItemIndex	= _ttoi( item );

	if( ! mSelectedItemIndex )
	{
		CString name;
		mItemNameCombo.GetLBText( mItemNameCombo.GetCurSel(), name );
		mSelectedItemIndex = mApplication.GetItemIndex( name );
	}

	const ItemScript& info = mApplication.GetItemScript( mSelectedItemIndex );

	if( ! info.ItemIdx )
	{
		CString textItIsNoExistedItem;
		textItIsNoExistedItem.LoadString( IDS_STRING63 );

		MessageBox( textItIsNoExistedItem, _T( "" ), MB_ICONWARNING | MB_OK );
		return;
	}	

	CDialog::OnOK();
}


void CItemAddDialog::OnEnChangeItemAddIndexEdit()
{
	static DWORD lastTick = 0;

	if(lastTick > GetTickCount())
	{
		return;
	}

	lastTick = GetTickCount() + 1000;

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString keyword;
	mItemIndexEdit.GetWindowText( keyword );

	const ItemScript& info = mApplication.GetItemScript( _ttoi( keyword ) );

	if( ! info.ItemIdx )
	{
		if( keyword.IsEmpty() )
		{
			return;
		}
		else if( ! _ttoi( keyword ) )
		{
			mApplication.SetItemCombo( mItemNameCombo, keyword );

			if( ! mItemNameCombo.GetCount() )
			{
				return;
			}

			mItemNameCombo.GetLBText( mItemNameCombo.GetCurSel(), keyword );

			CString itemIndex;
			itemIndex.Format( _T( "%d" ), mApplication.GetItemIndex( keyword ) );

			if( itemIndex == "0" )
			{
				return;
			}

			const ItemScript& selectedInfo = mApplication.GetItemScript( _ttoi( itemIndex ) );

			if( ! selectedInfo.ItemIdx )
			{
				return;
			}

			keyword = itemIndex;
		}

		return;
	}

	mItemNameStatic.SetWindowText( info.mUnicodeName );

	RedrawWindow();
}

void CItemAddDialog::OnLvnItemchangedItemAddListWeapon(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;

	POSITION		position	= mWeaponListCtrl.GetFirstSelectedItemPosition();
	const int		row			= mWeaponListCtrl.GetNextSelectedItem( position );
	const CString	itemIndex	= mWeaponListCtrl.GetItemText( row, 0 );

	mItemIndexEdit.SetWindowText( itemIndex );
	mItemToolTipCtrl.AddTool( this, mApplication.GetTip( _ttoi( itemIndex ) ) );
}

void CItemAddDialog::OnLvnItemchangedItemAddListPotion(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;

	POSITION		position	= mPotionListCtrl.GetFirstSelectedItemPosition();
	const int		row			= mPotionListCtrl.GetNextSelectedItem( position );
	const CString	itemIndex	= mPotionListCtrl.GetItemText( row, 0 );

	mItemIndexEdit.SetWindowText( itemIndex );
	mItemToolTipCtrl.AddTool( this, mApplication.GetTip( _ttoi( itemIndex ) ) );
}

void CItemAddDialog::OnLvnItemchangedItemAddListMaterial(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;

	POSITION		position	= mMaterialListCtrl.GetFirstSelectedItemPosition();
	const int		row			= mMaterialListCtrl.GetNextSelectedItem( position );
	const CString	itemIndex	= mMaterialListCtrl.GetItemText( row, 0 );

	mItemIndexEdit.SetWindowText( itemIndex );
	mItemToolTipCtrl.AddTool( this, mApplication.GetTip( _ttoi( itemIndex ) ) );
}


void CItemAddDialog::OnLvnItemchangedItemAddListEtc(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;

	POSITION		position	= mEtcListCtrl.GetFirstSelectedItemPosition();
	const int		row			= mEtcListCtrl.GetNextSelectedItem( position );
	const CString	itemIndex	= mEtcListCtrl.GetItemText( row, 0 );

	mItemIndexEdit.SetWindowText( itemIndex );
	mItemToolTipCtrl.AddTool( this, mApplication.GetTip( _ttoi( itemIndex ) ) );
}


void CItemAddDialog::OnCbnSelchangeItemAddCombo()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString itemName;
	mItemNameCombo.GetLBText( mItemNameCombo.GetCurSel(), itemName );

	LPCTSTR separator = L", ";
	int start = 0;
	const CString textItemIndex = itemName.Tokenize( separator, start );
	const DWORD itemIndex = _ttoi( textItemIndex );
	mItemToolTipCtrl.AddTool( this, mApplication.GetTip( itemIndex ) );
	mItemIndexEdit.SetWindowText( textItemIndex );

	// 090219 LUJ, ���õ� ���������� ����Ʈ�� ��Ŀ���� �ű��
	{
		const ItemScript& itemScript = mApplication.GetItemScript( itemIndex );

		if( itemScript.Stack )
		{
			for( int row = 0; row < mPotionListCtrl.GetItemCount(); ++row )
			{
				const CString text = mWeaponListCtrl.GetItemText( row, 0 );

				if( text == textItemIndex )
				{
					mTabCtrl.SetCurSel( 1 );
					OnTcnSelchangeItemAddTab( 0, 0 );
					mPotionListCtrl.SetItemState( row, LVIS_SELECTED, LVIS_SELECTED );
					mPotionListCtrl.EnsureVisible( row, FALSE );
					return;
				}
			}

			for( int row = 0; row < mMaterialListCtrl.GetItemCount(); ++row )
			{
				const CString text = mWeaponListCtrl.GetItemText( row, 0 );

				if( text == textItemIndex )
				{
					mTabCtrl.SetCurSel( 2 );
					OnTcnSelchangeItemAddTab( 0, 0 );
					mMaterialListCtrl.SetItemState( row, LVIS_SELECTED, LVIS_SELECTED );
					mMaterialListCtrl.EnsureVisible( row, FALSE );
					return;
				}
			}
		}
		else
		{
			for( int row = 0; row < mWeaponListCtrl.GetItemCount(); ++row )
			{
				const CString text = mWeaponListCtrl.GetItemText( row, 0 );

				if( text == textItemIndex )
				{
					mTabCtrl.SetCurSel( 0 );
					OnTcnSelchangeItemAddTab( 0, 0 );
					mWeaponListCtrl.SetItemState( row, LVIS_SELECTED, LVIS_SELECTED );
					mWeaponListCtrl.EnsureVisible( row, FALSE );
					return;
				}
			}
		}
		
		for( int row = 0; row < mEtcListCtrl.GetItemCount(); ++row )
		{
			const CString text = mWeaponListCtrl.GetItemText( row, 0 );

			if( text == textItemIndex )
			{
				mTabCtrl.SetCurSel( 3 );
				OnTcnSelchangeItemAddTab( 0, 0 );
				mEtcListCtrl.SetItemState( row, LVIS_SELECTED, LVIS_SELECTED );
				mEtcListCtrl.EnsureVisible( row, FALSE );				
				return;
			}
		}
	}
}

BOOL CItemAddDialog::PreTranslateMessage( MSG* pMsg )
{
	// 090219 LUJ, ���� ǥ��. ����: http://kalguksu.egloos.com/130355
	switch( pMsg->message )
	{
	case WM_MOUSEMOVE:
		{
			mItemToolTipCtrl.RelayEvent( pMsg );
			break;
		}
	case WM_MOUSEHOVER:
		{
			if( pMsg->hwnd == mItemNameStatic.GetSafeHwnd() )
			{
				mItemToolTipCtrl.RelayEvent( pMsg );
			}

			break;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}