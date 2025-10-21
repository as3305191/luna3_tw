#pragma once

#include "Object.h"

struct MOD_LIST;
class cStreetStallTitleTip;
class cImageSelf;
struct SetScript;


class CPlayer : public CObject  
{
protected: 
	CHARACTER_TOTALINFO	m_CharacterInfo;
	cStreetStallTitleTip* m_pSSTitle;
	cTimeDelay m_FacialTime;
	BOOL m_bStartCheck;
	cTimeDelay m_EyeTime;
	BOOL m_bActiveEye;
	BOOL m_bClosed;
	BOOL m_bRunningEmotion;
	CEngineObject* m_pWingObject;
	DWORD m_ChangedPartIdx[eWearedItem_Max];

private:
	int m_StandardMotion;
	int	m_Move_Start_Motion;
	int	m_Move_Ing_Motion;
	int	m_Move_End_Motion;
	int	m_ItemUseMotion;
	int	m_ItemUseEffect;
	CEngineObject* m_pFaceShapeObject;
	stFACEACTION m_stFaceAction;
	cImageSelf*	m_pcsFamilyEmblem;
	DWORD m_nFarmMotionTimeTick;
	DWORD m_nFarmMotionTime;
	DWORD m_dwImmortalTimeOnGTMAP;
	BOOL bWingsMove;

public:
	DWORD mWeaponEffect[3];
	DWORD mJobEffect;

	CPlayer();
	virtual	~CPlayer();

private:
	void InitPlayer(CHARACTER_TOTALINFO*);
	void ProcFarmMotion();
	void ProcessFaceAction();

public:
	/// ���� Ŭ���� �� �Լ�
	friend class			CObjectManager;
	friend class			CAppearanceManager;
	friend void				CS_BtnFuncEnter( LONG lId, void* p, DWORD we );
	void GetCharacterTotalInfo(CHARACTER_TOTALINFO*);
	CHARACTER_TOTALINFO* GetCharacterTotalInfo() { return &m_CharacterInfo; }
	virtual void SetLevel(LEVELTYPE level) { m_CharacterInfo.Level = level; }
	LEVELTYPE GetLevel() const { return m_CharacterInfo.Level; }
	BYTE GetGender() const { return m_CharacterInfo.Gender; }
	BYTE GetRace() const { return m_CharacterInfo.Race; }
	virtual void SetStage( BYTE grade, BYTE index );
	BYTE GetStage() const { return m_CharacterInfo.JobGrade; }
	void SetCharChangeInfo(CHARACTERCHANGE_INFO*);
	void SetCharacterSize(float fSize);
	float GetCharacterSize() const { return m_CharacterInfo.Height; }

	virtual DWORD GetLife() { return m_CharacterInfo.Life; }
	virtual void SetMaxLife(DWORD maxlife) { m_CharacterInfo.MaxLife = maxlife; }
	virtual void SetLife( DWORD life, BYTE type = 1 );
	virtual DWORD DoGetMaxLife();
	virtual float DoGetMoveSpeed();
	BYTE GetLifePercent();

	void SetGuildMark();
	void SetNickName();
	void SetFamilyMark();
	virtual void			ClearGuildInfo();
	void					SetGuildIdx( DWORD GuildIdx );
	DWORD					GetGuildIdx() { return m_CharacterInfo.MunpaID; }
	void					SetGuildIdxRank(DWORD GuildIdx, BYTE Rank); 
	void					SetGuildMemberRank(BYTE rank) { m_CharacterInfo.PositionInMunpa = rank; } //���Ŀ����� �ڽ��� ���� ����
	BYTE					GetGuildMemberRank() { return m_CharacterInfo.PositionInMunpa; } //���Ŀ����� �ڽ��� ����
	void					SetGuildMarkName( MARKNAMETYPE MarkName );
	MARKNAMETYPE			GetGuildMarkName();
	void					SetGuildName( char* GuildName );
	char*					GetGuildName();
	void					SetNickName( char* pNickName );
	char*					GetNickName();

	// ���� ���
	void					SetGuildUnionIdx( DWORD dwUnionIdx ) { m_CharacterInfo.dwGuildUnionIdx = dwUnionIdx; }
	void					SetGuildUnionName( char* pName ) { strncpy( m_CharacterInfo.sGuildUnionName, pName, MAX_GUILD_NAME+1 ); }
	void					SetGuildUnionMarkIdx( DWORD dwMarkIdx )	{ m_CharacterInfo.dwGuildUnionMarkIdx = dwMarkIdx; }
	DWORD					GetGuildUnionIdx() { return m_CharacterInfo.dwGuildUnionIdx; }
	char*					GetGuildUnionName() { return m_CharacterInfo.sGuildUnionName; }
	DWORD					GetGuildUnionMarkIdx() { return m_CharacterInfo.dwGuildUnionMarkIdx; }

	// �йи�
	virtual void			ClearFamilyInfo();
	void					SetFamilyIdx( DWORD FamilyIdx );
	DWORD					GetFamilyIdx() { return m_CharacterInfo.FamilyID; }
	void					SetFamilyMarkName(MARKNAMETYPE);
	MARKNAMETYPE			GetFamilyMarkName();
	void					SetFamilyName( char* FamilyName );
	char*					GetFamilyName();
	void					SetFamilyNickName( char* pNickName );
	char*					GetFamilyNickName();
	// �����ʸ�Ʈ
	void					SetImmortalTimeOnGTMAP(DWORD dwTime) {m_dwImmortalTimeOnGTMAP = dwTime;}
	DWORD					GetImmortalTime() {return m_dwImmortalTimeOnGTMAP;}
	void					SetGTName(DWORD NameType);
	void SetPKMode(BOOL bPKMode);
	BOOL IsPKMode() const { return m_CharacterInfo.bPKMode; }
	virtual void SetBadFame(FAMETYPE val);
	FAMETYPE GetBadFame() const { return m_CharacterInfo.BadFame; }
	virtual void SetMotionInState(EObjectState);
	void SetRunningEmotion(BOOL val) { m_bRunningEmotion  = val ; }

	// 070509 LYW --- Player : Add function to return to check emotion.
	BOOL IsRunningEmotion() { return m_bRunningEmotion ; }

	void					SetBaseMotion();
	int						GetStandardMotion() { return m_StandardMotion; }

	/// ����/��ȭ���
	void					SetPeaceMode( bool bPeace )	{ m_CharacterInfo.bPeace = bPeace; }

	/// �ൿ
	virtual BOOL			StartSocietyAct( int nStartMotion, int nIngMotion = -1, int nEndMotion = -1, BOOL bHideWeapon = FALSE );

	/// ����Ʈ
	BOOL					OnEndEffectProc( HEFFPROC hEff );

	/// ������ ��� ����
	void					SetItemUseMotionIdx( int Idx ) { m_ItemUseMotion = Idx;	}
	void					SetItemUseEffectIdx( int Idx ) { m_ItemUseEffect = Idx;	}

	///--------------------------------------------------------------------/
	/// ���μ��� ���� �Լ�-------------------------------------------------/

	/// ���μ���
	void					Process();
	virtual void			OnStartObjectState(EObjectState);
	virtual void			OnEndObjectState(EObjectState);
	virtual void			Die( CObject* pAttacker, BOOL bFatalDamage, BOOL bCritical, BOOL bDecisive );
	// 080910 LUJ, ������ ���� ó���� RESULTINFO ����ü�� �޾� ó���ϵ��� ��
	virtual void			Damage( CObject* pAttacker, BYTE DamageKind, DWORD Damage, const RESULTINFO&);
	virtual void			ManaDamage( CObject* pAttacker, BYTE DamageKind, DWORD Damage, const RESULTINFO&);
	virtual void			Heal( CObject* pHealer, BYTE HealKind, DWORD HealVal );
	virtual void			Recharge( CObject* pHealer, BYTE RechargeKind, DWORD RechargeVal );

	        
    void SetWearedItemTransmog(DWORD WearedPosition, DWORD ItemIdx);
	DWORD GetWearedItemTransmog(DWORD WearedPosition) const { return m_CharacterInfo.WearedItemTransmog[WearedPosition]; }
    
	
	void SetWearedItemIdx(DWORD WearedPosition, DWORD ItemIdx);
	DWORD GetWearedItemIdx(DWORD WearedPosition) const { return m_CharacterInfo.WearedItemIdx[WearedPosition]; }
	eWeaponType	GetWeaponEquipType();
	eWeaponAnimationType GetWeaponAniType();
	eWeaponAnimationType GetWeaponAniType(EWEARED_ITEM);
	eArmorType GetArmorType(EWEARED_ITEM );
	DWORD GetChangePart(DWORD WearedPosition);
	void ClearChangeParts();

	void ShowStreetStallTitle(BOOL bShow, char* strTitle, DWORD dwFontColor = RGB_HALF(10,10,10));
	void SetStreetStallTitle(char* strTitle);
	void ShowStreetBuyStallTitle(BOOL bShow, char* strTitle, DWORD dwFontColor = RGB_HALF(10,10,10));
	void SetStreetBuyStallTitle(char* strTitle);
	void RefreshStreetStallTitle();
	void ChangeFace(BYTE faceNum);
	void ReturnFace();
	void PlayEmoticon_Syn(BYTE EmoticonNum);
	void PlayEmoticon(int nEmoticonNum);
	void CloseEye();
	void InitializeEye();
	MOD_LIST* GetModList();
	cTimeDelay* GetEyeTime() { return &m_EyeTime; }
	void SetResidentRegistInfo(DATE_MATCHING_INFO* pInfo) { m_CharacterInfo.DateMatching = *pInfo; }
	void SetFaceObject(CEngineObject* pObject) { m_pFaceShapeObject = pObject; }
	CEngineObject* GetFaceObject() { return m_pFaceShapeObject ; }
	void SetFaceAction(int nAction, int nShape);
	void SetFaceShape(int nShape) { if(m_pFaceShapeObject)m_pFaceShapeObject->SetMaterialIndex(nShape); }
	cImageSelf*	GetFamilyEmblem() { return m_pcsFamilyEmblem; }
	void SetFamilyEmblem(cImageSelf* pcsEmblem) { m_pcsFamilyEmblem = pcsEmblem; }
	virtual void Release();
	void StartFarmMotion(int nMotionNum, DWORD nMotionTime);

	BOOL CheckHeart(CPlayer*);
	const DATE_MATCHING_INFO& GetDateMatchingInfo() const;
	void SetHeart( BOOL );
	virtual void SetObjectBattleState(eObjectBattleState eBattleState);

protected:
	// ��Ʈ ����Ʈ�� �׸��� �ִ��� ����
	BOOL m_IsDrawHeart;
	

	// ����, 070705 ��Ʈ ������ �ɷ� ����
public:
	typedef std::map< const SetScript*, int >	SetItemLevel;

	const SetItemLevel&	GetSetItemLevel() const;
	SetItemLevel&		GetSetItemLevel();

private:
	SetItemLevel			mSetItemLevel;

public:
	void SetWingObject( CEngineObject* pObj ) { m_pWingObject = pObj; }
	void SetWingObjectMotion(eWingMotion);
	void SetWingObjectBaseMotion(eWingMotion);
	void RemoveEffects();
	void StartJobEffect();
	void EndJobEffect();

	// 090422 ShinJS --- Ż�� �߰��۾�
private:
	DWORD m_dwVehicleID;										// Ż�Ϳ� ������ �ִ� ��� Ż�� ID ���� (���� 0�� ���°����� ����)
	DWORD m_dwVehicleSeatPos;									// Ż�Ϳ� ž�µǾ� �ִ� ��ġ (1������, 0:Ÿ�� ���� ����)
	DWORD m_dwLastVehicleSummonTime;							// ���������� Ż�� ��ȯ/������ �ð����� (��ȯ/���� �����̽� ���)
	DWORD m_dwLastVehicleGetOffTime;							// ���������� ž�������� �ð�(��ž�� ���ɽð� üũ�� ���)

public:
	void SetVehicleID( DWORD dwVehicleID )			{ m_dwVehicleID = dwVehicleID; }
	void SetVehicleSeatPos( DWORD dwVehicleSeatPos ){ m_dwVehicleSeatPos = dwVehicleSeatPos; }
	void SetLastVehicleSummonTime( DWORD dwTime )	{ m_dwLastVehicleSummonTime = dwTime; }
	void SetLastVehicleGetOffTime( DWORD dwTime )	{ m_dwLastVehicleGetOffTime = dwTime; }

	DWORD GetVehicleID() const						{ return m_dwVehicleID; }
	DWORD GetVehicleSeatPos() const					{ return m_dwVehicleSeatPos; }
	DWORD GetLastVehicleSummonTime() const			{ return m_dwLastVehicleSummonTime; }
	DWORD GetLastVehicleGetOffTime() const			{ return m_dwLastVehicleGetOffTime; }

	// ž�¿��� Ȯ��
	BOOL IsGetOnVehicle() const						{ return m_dwVehicleID && m_dwVehicleSeatPos; }

	// 090423 ShinJS --- ��ȯ/���� ���� ���� �˻�
	BOOL CanSummonVehicle();
	BOOL CanSealVehicle();

private:
	DWORD m_dwRideFurnitureMotionIndex;
	DWORD m_dwRideFurnitureID;										
	DWORD m_dwRideFurnitureSeatPos;	

	//090603 pdy �Ͽ�¡ ž�¹��� ���� m_bRideFurniture �߰� 
	BOOL  m_bRideFurniture;

public:
	void SetRideFurnitureMotion(DWORD dwRideMotionIndex)		{ m_dwRideFurnitureMotionIndex = dwRideMotionIndex;}
	void SetRideFurnitureID( DWORD dwFurnitureID )				{ m_dwRideFurnitureID = dwFurnitureID; }
	void SetRideFurnitureSeatPos( DWORD dwFurnitureSeatPos )	{ m_dwRideFurnitureSeatPos = dwFurnitureSeatPos; }

	DWORD GetRideFurnitureID() const								{ return m_dwRideFurnitureID; }
	DWORD GetRideFurnitureSeatPos() const							{ return m_dwRideFurnitureSeatPos; }

	//090603 pdy �Ͽ�¡ ž�¹��� ���� m_bRideFurniture �߰� 
	void SetRideFurniture(BOOL bRide)							{ m_bRideFurniture = bRide; }
	BOOL IsRideFurniture()										{return m_bRideFurniture;}

public:
	// 091123 ������ ���ͽõ� ����� �÷��� �߰� 
	void SetUsingWaterSeedFlag( BOOL bFlag ) { m_CharacterInfo.bUsingWaterSeed = bFlag ; }
	void SetMoveType(BOOL bValue) { bWingsMove = bValue; }
	BOOL GetMoveType(){ return bWingsMove; }
};
