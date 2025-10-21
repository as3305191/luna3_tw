// client.h : client ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error PCH���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����Ͻʽÿ�.
#endif

#include "resource.h"       // �� ��ȣ


// CclientApp:
// �� Ŭ������ ������ ���ؼ��� client.cpp�� �����Ͻʽÿ�.
//
#include "grid_control/GridCtrl.h"


// ��ų ��ũ��Ʈ���� ����ϴ� �ڷᱸ���� MFC�� �ڷᱸ���� �浹�Ͽ� SkillInfo.h�� ������ �� ����
// ���� �⺻���� ������ �Ľ��ؼ� ������� ����ü�� �������ߴ�.
struct ActiveSkillScript
{
	DWORD	mIndex;
	CString	mName;
	BOOL	mIsBuff;
	DWORD	mMoney;	// �н��� �ʿ��� ��
	DWORD	mPoint;	// �н��� �ʿ��� ����Ʈ
};

struct QuestScript
{
	DWORD	mMonsterIndex;
	DWORD	mMonsterHuntCount;

	// Ű: ������ �ε���, ��: �ʿ� ����
	typedef std::map< DWORD, DURTYPE >	ItemMap;
	ItemMap								mItemMap;
};


class CLoginDialog;
class CNewUserDialog;
class CPlayerDialog;
class CItemDialog;
class CWaitDialog;
class CSkillDialog;
class CItemFindDialog;
class CFamilyDialog;
class CQuestDialog;
class CGuildDialog;
class CUserDialog;
class CPermissionDialog;
class COperatorDialog;
class CLogDialog;
// 081021 LUJ, ���� ���� â �߰�
class CAuthorityDialog;
class CMemoDialog;
class CTournamentDialog;

class CclientApp : public CWinApp
{
public:
	CclientApp();
	~CclientApp();


// ������
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL PreTranslateMessage( MSG* );
	void Parse( DWORD serverIndex, const MSGROOT* );
	// 081021 LUJ, ��� ������ ������ ���, ��� â�� ǥ�õ��� �ʵ��� ��
	void Send( const TCHAR* serverName, const MSGROOT&, size_t, bool isLock = true );
	void Send( DWORD serverIndex, const MSGROOT&, size_t, bool isLock = true );
	void SendAll( const MSGROOT&, size_t, bool isLock = true );
	CString	GetRankName( DWORD rank ) const;

	const TCHAR*	GetServerName( DWORD serverIndex ) const;
	DWORD			GetServerIndex( const TCHAR* serverName ) const;

	// ������ ���� ����
	size_t	GetConnectionSize() const;
	
protected:
	void SetWait( BOOL );


// ����
protected:
	HMENU m_hMDIMenu;
	HACCEL m_hMDIAccel;


protected:
	// ���� �Ҵ�� ��ȭâ�� ��Ƶΰ�, �Ͻÿ� �����Ѵ�
	typedef std::list< CDialog* >	DialogList;
	DialogList						mDialogList;

	CLoginDialog* 				mLoginDialog;
	CNewUserDialog* 			mNewUserDialog;
	CPlayerDialog* 				mPlayerDialog;
	CItemDialog* 				mItemDialog;
	CWaitDialog* 				mWaitDialog;
	CSkillDialog*				mSkillDialog;
	CItemFindDialog*			mItemFindDialog;
	CFamilyDialog*				mFamilyDialog;
	CQuestDialog*				mQuestDialog;
	CGuildDialog*				mGuildDialog;
	CUserDialog*				mUserDialog;
	CPermissionDialog*			mPermissionDialog;
	COperatorDialog*			mOperatorDialog;
	// 080401 LUJ, ���� �α� â �߰�
	CLogDialog*					mLogDialog;
	// 081021 LUJ, ���� ���� â �߰�
	CAuthorityDialog*			mAuthorityDialog;
	CMemoDialog*				mMemoDialog;
	CTournamentDialog*			mTournamentDialog;

public:
	inline CDialog*				GetWaitDialog()			const	{ return ( CDialog* )mWaitDialog; }
	inline CNewUserDialog*		GetNewUserDialog()		const	{ return mNewUserDialog; }
	inline CItemDialog*			GetItemDialog()			const	{ return mItemDialog; }	
	inline CSkillDialog*		GetSkillDialog()		const	{ return mSkillDialog; }
	inline CPlayerDialog*		GetPlayerDialog()		const	{ return mPlayerDialog; }
	inline CQuestDialog*		GetQuestDialog()		const	{ return mQuestDialog; }
	inline CFamilyDialog*		GetFamilyDialog()		const	{ return mFamilyDialog; }
	inline CUserDialog*			GetUserDialog()			const	{ return mUserDialog; }
	inline CPermissionDialog*	GetPermissionDialog()	const	{ return mPermissionDialog; }
	inline CLogDialog*			GetLogDialog()			const	{ return mLogDialog; }
	// 081021 LUJ, ���� ���� â �߰�
	inline CAuthorityDialog*	GetAuthorityDialog()	const	{ return mAuthorityDialog; }	
	inline CMemoDialog*			GetMemoDialog()			const	{ return mMemoDialog; }
	inline CTournamentDialog*	GetTournamentDialog()	const	{ return mTournamentDialog; }
	
protected:
	BOOL			mIsWaiting;

	// ������ ����
public:
	const TCHAR*		GetItemName( DWORD itemIndex ) const;
	DWORD				GetItemIndex( const TCHAR* ) const;
	const ItemScript&	GetItemScript( DWORD itemIndex ) const;
	BOOL				IsDuplicateItem( DWORD itemIndex ) const;
	LPCTSTR GetPetGradeText( DWORD grade ) const;
	LPCTSTR GetPetAritificialIntelligenceText( DWORD artificialIntelligence ) const;
	CString ConvertToText(MONEYTYPE) const;

	// ������ ��ġ�� ���� � ������ �κ��丮���� ��ȯ�Ѵ�
	CString	GetItemPositionType( Item::Area, POSTYPE ) const;

	CString GetText( eFamilyLog )		const;
	CString GetText( eGuildLog )		const;
	CString GetText( eLogSkill )		const;
	CString GetText( eLogitemmoney )	const;
	CString GetText( eQuestLog )		const;
	CString GetText( eGM_POWER )		const;
	CString GetText( RecoveryLog )		const;
	// 080328 LUJ, ��� �ɼ� Ű�� ���� ���ڿ� ��ȯ
	CString GetText( ITEM_OPTION::Drop::Key ) const;
	// 080402 LUJ, ���忡 ���� �α� ��ȯ. ���� �۾��ڰ� �α� �ۼ� �� ���Ƿ� �ؼ� �Ұ����ϰ� �� ���� ������ �����...
	CString GetText( FARM_LOG_KIND )	const;
	// 080403 LUJ, ���� ������ ���Կ� ���� �α� ��ȯ
	CString GetText( ItemShopLog )		const;
	// 080416 LUJ, ����ġ �α� ��ȯ
	// 080716 LUJ, �߸��� Ÿ�� ����
	CString GetText( eLogExppoint )		const;
	// 080430 LUJ, ������Ʈ ���� ���ڿ� ��ȯ
	CString GetText( EObjectKind )		const;
	// 080523 LUJ, ���� �α� ���ڿ� ��ȯ
	CString GetText( eFishingLog )		const;
	// 080716 LUJ, �� ���� ���ڿ� ��ȯ
	CString GetText( ePetState )		const;
	// 080716 LUJ, �� ���� ���ڿ� ��ȯ
	CString GetText( ePetType )			const;
	// 080716 LUJ, �� �α� ���ڿ� ��ȯ
	CString GetText( ePetLog )			const;
	// 081027 LUJ, ���� �α� ���ڿ� ��ȯ
	CString GetText( eSiegeLog )		const;
	CString GetText(eAutoNoteLogKind) const;
	CString GetText(eMoneyLogtype) const;
	CString GetText(eHouseLog) const;
	CString GetText(eCookingLog) const;
	CString GetText(eCOOKRECIPE) const;
	CString GetText(HOUSE_HIGHCATEGORY) const;
	CString GetText(eHOUSEFURNITURE_STATE) const;
	CString GetText(eConsignmentLog) const;
	void CreateGrid( CWnd* parent, CGridCtrl*, UINT id, const CRect&, const CString*, BOOL autoHeight ) const;
	
	// ���� ���� �����ϴ� ��ƿ��Ƽ �Լ�
	static void SetCellValue( DWORD value,	CGridCtrl&, GV_ITEM&, int state );
	static void SetCellValue( float value,	CGridCtrl&, GV_ITEM&, int state );
	static void SetCellValue( int value,	CGridCtrl&, GV_ITEM&, int state );
	static void SetCellValue( const TCHAR*,	CGridCtrl&, GV_ITEM&, int state );

	// ��¥ �޺��� ���� �����ϴ� ��ƿ��Ƽ �Լ�
	static void SetDateCombo( CComboBox& yearCombo, CComboBox& monthCombo, CComboBox& dayCombo, BOOL isToday );

	// ���ϵ�ī�� ���ڸ� DB���� ��밡���ϰ� �ٲ��ش�
	static void ConvertWildCardForDb( char* text );

	// ����Ʈ ��Ʈ���� ������ ������ �����Ѵ�
	static void SaveToExcel( const TCHAR* fileName, const TCHAR* tableName, const CListCtrl& );

	// ���õ� �׸��� Ŭ�� ���忡 �����Ѵ�
	// 081020 LUJ, ��ü �׸��� �������� �����ϴ� �÷��� �߰�
	static void SaveToClipBoard( const CListCtrl&, BOOL isWholeCopy = FALSE );
	// 081020 LUJ, Comma Separated Values ���Ϸ� ����
	static void SaveToCSV( const TCHAR* fileName, const CListCtrl& listCtrl );

	// ����Ʈ ��Ʈ�ѿ� ���� �̸��� �����Ѵ�
	static void SetServerListCtrl( CListCtrl& );

	// ������ ����� �����Ѵ�
	void SetItemList( CListCtrl& ) const;
	void SetItemList( CListCtrl&, eItemCategory ) const;
	void SetItemList(CListCtrl&, LPCTSTR) const;
	// ���ڿ��� �ش��ϴ� �ܾ ���Ե� ��� �������� �޺��� �����Ѵ�
	void SetItemCombo( CComboBox&, const TCHAR* keyword ) const;

	// ��ȿ�� ���� ����� �����Ѵ�
	void SetServerComboBox( CComboBox& ) const;

	// ���� ����� �����Ѵ�
	void SetJobCombo( CComboBox&, DWORD jobType, DWORD race, DWORD jobLevel, DWORD jobIndex );

	// ��� ���� ����� �����Ѵ�
	void SetRankCombo( CComboBox& ) const;

	// ����� ���� ����� �����Ѵ�
	void SetUserLevelCombo( CComboBox& ) const;
 	
	// ������ ��ũ��Ʈ
protected:
	void LoadItemScript();	

	// Ű: ������ �ε���
	typedef std::map< DWORD, ItemScript >	ItemMap;
	ItemMap									mItemMap;

	// ��æƮ ��ũ��Ʈ
public:
	// ��æƮ�� �ҿ�Ǵ� ��� �ε����� ��ũ��Ʈ�� �����´�
	const EnchantScript* GetEnchantScript( DWORD itemIndex ) const;

protected:
	void LoadEnchantScript();

	typedef stdext::hash_map< DWORD, EnchantScript >	EnchantScriptMap;
	EnchantScriptMap									mEnchantScriptMap;


	// ��ų ��ũ��Ʈ
public:
	typedef std::map< DWORD, DWORD >	SkillSizeMap;

	const ActiveSkillScript* GetActiveSkillScript( DWORD skillIndex ) const;
	// 081202 LUJ, ���ڿ��� �ش��ϴ� ��ų ��ũ��Ʈ�� ��ȯ�Ѵ�
	const ActiveSkillScript* GetActiveSkillScript( CString skillName ) const;
	DWORD GetSkillSize( DWORD skillIndex ) const;
	const SkillSizeMap& GetSkillSizeMap() const;	
	const TCHAR* GetTip( DWORD index ) const;
	// 081202 LUJ, Ű���忡 �ش��ϴ� ��ų �̸��� �޺��� �ִ´�
	size_t SetSkillCombo( CComboBox&, CString keyword ) const;

protected:
	void LoadActiveSkillScript();
	void LoadTip();
	
	// Ű: ��ų �ε���
	typedef stdext::hash_map< DWORD, ActiveSkillScript >	ActiveSkillScriptMap;
	ActiveSkillScriptMap									mActiveSkillScriptMap;

	// Ű: ��ų �ε���, ��: ��ų �ִ� ����	
	SkillSizeMap						mSkillSizeMap;

	// Ű: ��ų �ε���
	typedef stdext::hash_map< DWORD, CString >	SkillTipMap;
	SkillTipMap									mSkillTipMap;


	// ����Ʈ ��ũ��Ʈ
public:
	const TCHAR*	GetQuestTitle( DWORD mainQuestIndex, DWORD subQuestIndex ) const;
	const TCHAR*	GetQuestDescription( DWORD mainQuestIndex, DWORD subQuestIndex ) const;
	const TCHAR*	GetQuestItemName( DWORD questItemIndex ) const;
	const TCHAR*	GetQuestItemDescription( DWORD questItemIndex ) const;
	DWORD			GetMainQuestIndex( DWORD questItemIndex ) const;
	DWORD			GetSubQuestIndex( DWORD questItemIndex ) const;

protected:
	// ����Ʈ �������� �д´�
	void LoadQuestItemScript();

public:
	struct QuestItem
	{
		DWORD	mMainQuestIndex;
		DWORD	mSubQuestIndex;
		CString mName;
		CString mDescription;
	};

	// Ű: ����Ʈ ������ �ε���
	typedef stdext::hash_map< DWORD, QuestItem >	QuestItemMap;
	const QuestItemMap& GetQuestItemMap() const;

protected:
	QuestItemMap mQuestItemMap;

// ����Ʈ ���ڿ��� �д´�
	void LoadQuestStringScript();

public:
	struct QuestString
	{
		DWORD	mMainQuestIndex;
		DWORD	mSubQuestIndex;

		CString	mTitle;
		CString	mDescription;
	};

	// Ű: ��������Ʈ �ε����� ��������Ʈ �ε����� MAKELPPARAM���� ���յǾ� �ִ�
	typedef stdext::hash_map< DWORD, QuestString >	QuestStringMap;	
	const QuestStringMap& GetQuestStringMap() const;

protected:	
	QuestStringMap mQuestStringMap;


	// ����Ʈ ��ũ��Ʈ�� �д´�
public:
	const QuestScript* GetQuestScript( DWORD mainQuestIndex, DWORD subQuestIndex ) const;
	
protected:
	void LoadQuestScript();

	// Ű: ��������Ʈ �ε����� ��������Ʈ �ε����� MAKELPPARAM���� ���յǾ� �ִ�
	typedef stdext::hash_map< DWORD, QuestScript >	QuestScriptMap;
	QuestScriptMap									mQuestScriptMap;


	// �������̽� �޽���
public:
	LPCTSTR GetJobName( DWORD code ) const;
	LPCTSTR GetInterfaceMessage( DWORD Index ) const;

protected:
	void LoadInterfaceMessage();

	typedef stdext::hash_map< DWORD, CString >	InterfaceMessage;
	InterfaceMessage							mInterfaceMessage;

	
	// �ֹε���� ������ �д´�
public:
	typedef std::map< DWORD, CString >	LicenseFavoriteMap;
	typedef std::map< DWORD, CString >	LicenseAreaMap;
	typedef std::map< DWORD, CString >	LicenseAgeMap;

	const LicenseFavoriteMap&	GetLicenseFavoriteMap() const;
	const LicenseAreaMap&		GetLicenseAreaMap() const;
	const LicenseAgeMap&		GetLicenseAgeMap() const;

protected:
	void LoadLicenseScript();

	LicenseFavoriteMap							mLicenseFavoriteMap;
	LicenseAreaMap								mLicenseAreaMap;
	LicenseAgeMap								mLicenseAgeMap;


	// ���۹� �̸��� �д´�
public:
	const TCHAR* GetCropName( DWORD cropIndex, DWORD cropLevel ) const;
	// 080630 LUJ, ���� �̸��� ��ȯ�Ѵ�
	const TCHAR* GetLivestockName( DWORD kind ) const;

protected:
	void LoadFarmScript();
	// Ű: MAKELONG( ���۹� ����, ) 
	typedef std::map< DWORD, CString >	FarmCropMap;
	FarmCropMap							mFarmCropMap;
	// 080630 LUJ, ���� �̸�
	typedef std::map< DWORD, CString >	LivestockMap;
	LivestockMap						mLivestockMap;

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnMenuNewUser();
	afx_msg void OnMenuItemSearch();
	afx_msg void OnMenuGuild();
	afx_msg void OnMenuItemLog();
	afx_msg void OnMenuFamliy();
	afx_msg void OnMenuPermission();
	afx_msg void OnMenuOperator();
	afx_msg void OnMenuPassword();
	afx_msg void OnMenuOperatorLog();
	afx_msg void OnMenuItemOptionLog();
	// 080320 LUJ, ��� �ɼ� �α� �޴�
	afx_msg void OnMenuItemDropOptionLog();
	// 080401 LUJ, ���� �α�
	afx_msg void OnMenuFarmLog();
	// 080403 LUJ, ���� ������ ���� �α�
	afx_msg void OnMenuItemShopLog();
	// 080523 LUJ, ���� �α�
	afx_msg void OnMenuFishLog();
	// 080630 LUJ, ���� �α�
	afx_msg void OnMenuLivestockLog();
	// 080716 LUJ, ������ ���� �α�
	afx_msg void OnMenuItemGlobalLog();
	// 081021 LUJ, ���� ����
	afx_msg void OnMenuAuthority();
	// 081027 LUJ, ��ʸ�Ʈ
	afx_msg void OnMenuTournament();
	// 081027 LUJ, ���� �α�
	afx_msg void OnMenuSiegeLog();
	// 081205 LUJ, ä�� �α�
	afx_msg void OnMenuChatLog();
	// 090122 LUJ, ��ũ��Ʈ ��ŷ �α�
	afx_msg void OnMenuScriptHackLog();
	// 090122 LUJ, �̸� ���� �α�
	afx_msg void OnMenuRenameLog();
	// 090304 LUJ, ������ ��ȯ �ùķ��̼�
	afx_msg void OnMenuSimulationItemChange();
	// 090406 LUJ, �� �α�
	afx_msg void OnMenuMoneyLog();
	afx_msg void OnHouseLog();
	afx_msg void OnCookLog();
	afx_msg void OnRecipeLog();
	afx_msg void OnConsignLog();
	afx_msg void OnGameRoomLog();
	
public:
	// 080304 LUJ, �α׸� ǥ��
	void Log( const TCHAR*, ... );

	// 080328 LUJ, �α� �������� �ʴ� ȭ�� ���
	void Put( const TCHAR*, ... );

	// 080401 LUJ, ��Ƽ����Ʈ ���ڿ��� ���Ե� Ư���� ���ڿ��� �����Ѵ�
	CString RemoveString( const char* source, const char* extractedString ) const;
	// 090304 LUJ, �ʸ� �ð� ���ڿ��� ��ȯ�Ѵ�
	CString GetDateString( DWORD second ) const;
	// 090304 LUJ, 0~1������ �Ǽ��� ��´�
	float GetRandomFloat() const;
	// 080630 LUJ, ��ũ��Ʈ�� �ε��Ѵ�. ������� ó���Ǿ�, �α��� â�� �ε� ���¸� ǥ�����ش�
	static UINT LoadScript( LPVOID );


public:
	// 080702 LUJ, �� ������ â�� ��ȯ�Ѵ�
	inline virtual CWnd* GetMainWnd()	{ return m_pMainWnd; }


public:
	// 080716 LUJ, �� �̸��� �д´�
	void	LoadPetScript();
	LPCTSTR	GetPetName( DWORD index ) const;
private:
	typedef stdext::hash_map< DWORD, CString >	PetNameMap;
	PetNameMap									mPetNameMap;

	// 080731 LUJ, �� ����
public:
	// 080731 LUJ, �� ���� ������ �д´�
	void	LoadPetStatusScript();
	// 080731 LUJ, �� Ÿ�� ������ ���� ��
	typedef std::set< ePetType > PetTypeSet;
	// 080731 LUJ, ������ ���� ���� �� �ִ� Ÿ���� ��ȯ
	const PetTypeSet&	GetPetType( DWORD level ) const;

	struct PetStatusScript
	{
		int	mStrength;
		int	mDexterity;
		int	mVitality;
		int	mIntelligence;
		int	mWisdom;
	};
	// 080731 LUJ, �� ���� ��ũ��Ʈ�� ��ȯ�Ѵ�
	const PetStatusScript& GetPetStatusScript( ePetType, DWORD level ) const;
private:
	// 080731 LUJ, �� ������ ���� ���� �� �ִ� Ÿ��
	typedef stdext::hash_map< DWORD, PetTypeSet >	PetStatusMap;
	PetStatusMap									mPetStatusMap;

	// 080731 LUJ, �� ������ ������ Ű�� ���´�.
	typedef std::pair< ePetType, DWORD >				PetStatusKey;
	typedef std::map< PetStatusKey, PetStatusScript >	PetStatusScriptMap;
	PetStatusScriptMap									mPetStatusScriptMap;

	// 080731 LUJ, �� HP/MP ����
public:
	// 080731 LUJ, �� HP/MP ������ �д´�
	void	LoadPetBodyScript();
	// 080731 LUJ, �� ����/��޿� ���� ������ ����
	struct PetBodyScript
	{
		DWORD	mHealth;
		DWORD	mMana;
	};
	// 080731 LUJ, �� ����/��޿� ���� ������ ��ȯ�Ѵ�
	const PetBodyScript& GetPetBodyScript( ePetType, DWORD grade ) const;
	// 080731 LUJ, ������ ���� ��ȿ�� �� ���� ������ ���ڿ��� ��ȯ�Ѵ�.
	void GetPetBodyConditionText( CString& ) const;
private:
	// 080731 LUJ, Ű: �� Ÿ�� * 10 + �� ���
	typedef std::map< DWORD, PetBodyScript >	PetBodyMap;
	PetBodyMap									mPetBodyMap;

	// 080731 LUJ, �� ���� ����
public:
	struct PetSystemScript
	{
		struct SkillSlot
		{
			DWORD	mMinSize;
			DWORD	mMaxSize;
		}
		mSkillSlot;

        DWORD	mMaxGrade;
		DWORD	mMaxLevel;
		
	};
	// 080731 LUJ, �� ���� ������ ��ȯ�Ѵ�
	const PetSystemScript& GetPetSystemScript() const;
	// 080731 LUJ, �� ���� ������ �д´�
	void	LoadPetSystemScript();
private:
	PetSystemScript	mPetSystemScript;

public:
	void LoadMonsterScript();
	LPCTSTR	GetMonsterName( DWORD index ) const;

private:
	typedef stdext::hash_map< DWORD, CString >	MonsterNameMap;
	MonsterNameMap								mMonsterNameMap;

private:
	typedef stdext::hash_map< DWORD, CString > FurnitureNameMap;
	FurnitureNameMap mFurnitureNameMap;

public:
	void LoadFurnitureScript();
	LPCTSTR	GetFurnitureName(DWORD index) const;

	struct CookScript
	{
		typedef std::map< LEVELTYPE, EXPTYPE > ExperienceContainer;
		ExperienceContainer mCookCountContainer;
		ExperienceContainer mEatCountContainer;
		ExperienceContainer mFireCountContainer;
	};
	struct RecipeScript
	{
		typedef DWORD ItemIndex;
		typedef std::map< ItemIndex, size_t > MaterialContainer;
		MaterialContainer mMaterialContainer;

		ItemIndex mResultItemIndex;
		DWORD mRemainTime;
		EXPTYPE mMinimumExperience;
		EXPTYPE mMaximumExperience;
		LEVELTYPE mLevel;
	};

	typedef DWORD RecipeIndex;
	typedef std::map< RecipeIndex, RecipeScript > RecipeContainer;
	RecipeContainer mRecipeContainer;

	void LoadCookScript();
	EXPTYPE GetMaxCookCount(LEVELTYPE) const;
	EXPTYPE GetMaxEatCount(LEVELTYPE) const;
	EXPTYPE GetMaxFireCount(LEVELTYPE) const;
	LEVELTYPE GetMaxCookLevel() const { return LEVELTYPE(mCookScript.mCookCountContainer.size()); }
	LPCTSTR GetRecipeName(RecipeIndex) const;
	LEVELTYPE GetRecipeLevel(RecipeIndex) const;
	void UpdateRecipeListCtrl(const MSG_RM_COOK_RECIPE&, CListCtrl& learned, CListCtrl& forgotten);

private:
	CookScript mCookScript;
};

extern CclientApp theApp;
