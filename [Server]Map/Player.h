#pragma once


#include "Object.h"
#include "ExchangeManager.h"
#include "ItemContainer.h"
#include "InventorySlot.h"
#include "StorageSlot.h"
#include "WearSlot.h"
#include "Purse.h"
#include "..\[CC]Quest\QuestDefines.h"
#include "QuestGroup.h"
#include "QuickSlot.h"

struct sGAMEOPTION
{
	BOOL bNoDeal;
	BOOL bNoParty;
	BOOL bNoShowdown;
};

struct stFARMPLAYERINFO																		// ÷? 
{
	UINT				nCropPlantRetryTime;												// ..? ? ?µ ? ğ
	DWORD				nCropPlantRetryTimeTick;											// ..? ? ?µ ? ğ ?
	UINT				nCropManureRetryTime;												// ..? ? ? ?µ ? ğ
	DWORD				nCropManureRetryTimeTick;											// ..? ? ? ?µ ? ğ ?
	UINT				nAnimalFeedRetryTime;												// ..? ? ? ?µ ? ğ
	DWORD				nAnimalFeedRetryTimeTick;											// ..? ? ? ?µ ? ğ ?
	UINT				nAnimalCleanRetryTime;												// ..?  ?´ ? ğ
	DWORD				nAnimalCleanRetryTimeTick;											// ..?  ?´ ? ğ ?
};

class cDelay;
class cStreetStall;
class CMonster;
class CQuestBase;
class CQuestScript;
class cSkillObject;
class cSkillTree;
struct SetScript;
struct ITEMOBTAINARRAYINFO;

enum
{
	PLAYERINITSTATE_ONLY_ADDED			= 1 << 0,
	PLAYERINITSTATE_HERO_INFO			= 1 << 1,
	PLAYERINITSTATE_SKILL_INFO			= 1 << 2,
	PLAYERINITSTATE_ITEM_INFO			= 1 << 3,
	PLAYERINITSTATE_QUICK_INFO			= 1 << 4,
	PLAYERINITSTATE_STORAGE_INFO		= 1 << 5,
	PLAYERINITSTATE_QEUST_INFO			= 1 << 6,
	PLAYERINITSTATE_FARM_INFO			= 1 << 7,
	PLAYERINITSTATE_CHALLENGEZONE_INFO	= 1 << 8,
	PLAYERINITSTATE_OPTION_INFO			= 1 << 9,
};

enum
{
	MONEY_ADDITION = 0,
	MONEY_SUBTRACTION,
};

#define GETITEM_FLAG_INVENTORY	0x00000001
#define GETITEM_FLAG_WEAR		0x00000002

class CPlayer : public CObject  
{
private:
	DWORD m_UniqueIDinAgent;
	DWORD m_dwCurChannel;
	DWORD m_ChannelID;
	eUSERLEVEL m_dwUserLevel;
	DWORD m_ContinuePlayTime;
	int m_initState;
	CHARACTER_TOTALINFO m_HeroCharacterInfo;
	HERO_TOTALINFO m_HeroInfo;
	sGAMEOPTION	m_GameOption;
	cQuickSlot m_QuickSlot[8];
	BOOL m_God;
	ReviveFlag mReviveFlag;
	PlayerStat m_charStats;
	PlayerStat m_SetItemStats;
	PlayerStat m_itemBaseStats;
	PlayerStat m_itemOptionStats;
    PlayerStat m_itemStats;
	Status mPassiveStatus;
	Status mRatePassiveStatus;
	DWORD mGravity;
	DWORD	m_MarryPlayerIDX;
	friend class CCharacterCalcManager;

    DWORDEX mPlayerBalance;

public: 
    void SetPlayerBalance(DWORDEX balance) { mPlayerBalance = balance; }
    DWORDEX GetPlayerBalance() const { return mPlayerBalance; }


private:
	///   
	BOOL		m_bShowdown;
	DWORD		m_dwOpPlayerID;

	/// PK ğ 
	DWORD		m_dwPKModeStartTime;	//with gCurTime
	DWORD		m_dwPKContinueTime;

	//---KES PK 071202
	BOOL	m_bNoExpPenaltyByPK;

	// 080318 LUJ, PK  ? ?  ?
public:
	BOOL	IsNoExpPenaltyByPK() const	{ return m_bNoExpPenaltyByPK; }

private:
	void		LogOnRelease();
	void		AddBadFameReduceTime();
	/// ?   
	WORD		m_MurdererKind;
	DWORD		m_MurdererIDX;
	BOOL		m_bPenaltyByDie;
	BOOL		m_bReadyToRevive; // ¿ 
	CYHHashTable<CMonster> m_FollowMonsterList;
	
	///  ? ?
	float		mAccuracy;
	float		mAvoid;
	float		mBlock;
	float		mPhysicAttackMax;
	float		mPhysicAttackMin;
	float		mMagicAttackMax;
	float		mMagicAttackMin;
	float		mMagicAttack;
	float		mPhysicDefense;
	float		mMagicDefense;
	float		mCriticalRate;
    float		mCriticalDamage;
	float		mCriticalDamagePlus;
	float		mCriticalDamageRate;
	float		mMagicCriticalRate;
	float		mMagicCriticalDamagePlus;
	float		mMagicCriticalDamageRate;
	float		mMagicCriticalDamage;
	float		mLifeRecover;
	float		mManaRecover;
    float		mMaxLife;
	float		mMaxMana;
	DWORD		mShieldDefense;

	float		mAddDamage;
	float		mReduceDamage;
	float		mRateAddDamage;
	float		mRateReduceDamage;

	BOOL		m_bDieForGFW;
	DWORD 		mPetIndex;
	WORD		m_wGTReturnMapNum;
	int			m_ObserverBattleIdx;


	// ??
	DWORD		m_dwKillCountFromGT;	// ?? ? ?
	DWORD		m_dwKillPointFromGT;	// ?? ? ??
	DWORD		m_dwRespawnTimeOnGTMAP;	// ??  ?ğ
	DWORD		m_dwImmortalTimeOnGTMAP;// ?? ?? ğ
	VECTOR3		m_vStartPosOnGTMAP;		// ??? ?

	/// ? 
	CQuestGroup	m_QuestGroup;
public:
	BOOL		m_bNeedRevive;

	DWORD		mWeaponEnchantLevel;
	DWORD		mPhysicDefenseEnchantLevel;
	DWORD		mMagicDefenseEnchantLevel;

	cDelay*		mpBattleStateDelay;
	/// /?   -------------------------------------------/
	///--------------------------------------------------------------------/


	///--------------------------------------------------------------------/
	/// ?  -------------------------------------------------/
private:
	/// ? ? ğ
	DWORD			m_dwProgressTime;

	/// ? ü?
	int				m_nHackCount;			//skill + move
	DWORD			m_dwHackStartTime;

	/// ??  
	RECOVER_TIME	m_ManaRecoverTime;
	YYRECOVER_TIME	m_YYManaRecoverTime;

	// 100624 ONS MP?  ? ?.
	std::queue< YYRECOVER_TIME >	m_YYManaRecoverTimeQueue;

	DWORD			m_ManaRecoverDirectlyAmount;


	///  
	BOOL			m_bExit;	//÷
	DWORD			m_dwExitStartTime;
	BOOL			m_bNormalExit;
	BOOL			m_bWaitExitPlayer;

	BOOL			mIsEmergency;
	MAPTYPE			mTargetMap;
	DWORD			mTargetPosX;
	DWORD			mTargetPosZ;

	/// ?  -------------------------------------------------/
	///--------------------------------------------------------------------/

	///--------------------------------------------------------------------/
	///    ----------------------------------------------/
	///   ? ?  ?    ?
private:
	///  ??
	CItemContainer						m_ItemContainer;
	CInventorySlot						m_InventorySlot;
	CWearSlot							m_WearSlot;
	CStorageSlot						m_StorageSlot;
	CItemSlot							m_ShopItemSlot;			// ItemMall	
	CInventoryPurse						m_InventoryPurse;
	CPurse								m_StoragePurse;
	/// ??  ?
	friend class						CInventoryPurse;
	
	/// ??  
	BOOL								m_bGotWarehouseItems;
	sEXCHANGECONTAINER					m_ExchangeContainer;

	cStreetStall* m_pGuetStall;
	char m_StreetStallTitle[MAX_STREETSTALL_TITLELEN+1];
	cSkillTree*	m_SkillTree;
	DWORD m_CurComboNum;
	WORD m_SkillFailCount;
	DATE_MATCHING_INFO m_DateMatching;
	stMONSTERMETER m_stMonstermeterInfo;
	char*									m_pcFamilyEmblem;
	stFARMPLAYERINFO						m_stFarmInfo;
	ObjectIndexContainer mAggroObjectContainer;
	UINT									m_nChallengeZoneEnterFreq;
	UINT									m_nChallengeZoneEnterBonusFreq;
	UINT									m_nChallengeZonePartnerID;
	UINT									m_nChallengeZoneSection;
	UINT									m_nChallengeZoneStartState;
	DWORD									m_nChallengeZoneStartTimeTick;
	UINT									m_nChallengeZoneMonsterNum;
	UINT									m_nChallengeZoneKillMonsterNum;
	BOOL									m_bChallengeZoneNeedSaveEnterFreq;
	BOOL									m_bChallengeZoneCreateMonRightNow;
	UINT									m_nChallengeZoneMonsterNumTillNow;
	DWORD									m_nChallengeZoneClearTime;
	// 091123 ONS ç ? ? ó ?
	DWORD									m_nChallengeZoneExpRate;
	BYTE m_byHP_Point;
	BYTE m_byMP_Point;
	EXPTYPE	m_dweFamilyRewardExp;
	BYTE m_byCurFamilyMemCnt;
	CYHHashTable< CQuestBase > m_QuestList;

public:
	/// /?
	CPlayer();
	virtual ~CPlayer();
	void SetReviveFlag(ReviveFlag flag) { mReviveFlag = flag; }
	virtual BOOL	Init( EObjectKind kind, DWORD AgentNum, BASEOBJECT_INFO* pBaseObjectInfo );
	virtual void	Release();
	void			InitBaseObjectInfo( BASEOBJECT_INFO* pBaseInfo );
	void			InitClearData();
	void			InitCharacterTotalInfo(CHARACTER_TOTALINFO*);
	void			InitHeroTotalInfo(HERO_TOTALINFO*);
	virtual void	SetInitedGrid();

	///  
	/// Agent 
	void			SetUniqueIDinAgent( DWORD uniqueid ) { m_UniqueIDinAgent = uniqueid; }
	DWORD			GetUniqueIDinAgent() { return m_UniqueIDinAgent; }

	/// ä 
	void			SetChannelID( DWORD id ) { m_ChannelID = id; }
	DWORD			GetChannelID() { return m_ChannelID; }

	void			SetCurChannel(DWORD dwChannel) {m_dwCurChannel = dwChannel;}
	DWORD			GetCurChannel() {return m_dwCurChannel;}
	void			SetUserLevel(eUSERLEVEL dwUserLevel) { m_dwUserLevel = dwUserLevel; }
	eUSERLEVEL		GetUserLevel() const { return m_dwUserLevel; }
	DWORD SetAddMsg(DWORD dwReceiverID, BOOL isLogin, MSGBASE*&);
	void 			GetCharacterTotalInfo(CHARACTER_TOTALINFO* info) { *info = m_HeroCharacterInfo; }
	CHARACTER_TOTALINFO& GetCharacterTotalInfo() { return m_HeroCharacterInfo; }
	const CHARACTER_TOTALINFO& GetCharacterTotalInfo() const { return m_HeroCharacterInfo; }
	void			GetHeroTotalInfo(HERO_TOTALINFO* info) { *info = m_HeroInfo; }
	HERO_TOTALINFO*	GetHeroTotalInfo() { return &m_HeroInfo; }
	int				GetGender() { return m_HeroCharacterInfo.Gender; }

	///  ?
	sGAMEOPTION*	GetGameOption() { return &m_GameOption; }
	void			SetGameOption( sGAMEOPTION* pGameOption ) { m_GameOption = *pGameOption; }

	/// 
	int				GetInitState(){ return m_initState; }
	void			SetInitState( int initstate, DWORD protocol );
	virtual void	OnEndObjectState(EObjectState);
	void			SetPeaceMode( BOOL bPeace ) { m_HeroCharacterInfo.bPeace = bPeace; }
	BOOL			GetPeaceMode() const { return m_HeroCharacterInfo.bPeace; }
	BOOL			IsVisible() const { return m_HeroCharacterInfo.bVisible; }
	void			SetVisible(BOOL val) { m_HeroCharacterInfo.bVisible = val; }
	void			SetStage(BYTE grade, BYTE index);
	BYTE			GetStage() const { return m_HeroCharacterInfo.JobGrade ; }

	BYTE			GetJop(BYTE idx) const { return m_HeroCharacterInfo.Job[idx]; }
	BYTE			GetRacial() const { return m_HeroCharacterInfo.Race; }
	BYTE			GetJopGrade() const { return m_HeroCharacterInfo.JobGrade; }
	WORD			GetJobCodeForGT();
	void			SetJob(BYTE jobGrade, BYTE jobIdx);

	WORD			GetPlayerLoginPoint() const { return m_HeroCharacterInfo.LoginPoint_Index; }
	WORD			GetLoginMapNum() const {return m_HeroCharacterInfo.LoginMapNum;}
	void			SetLoginPoint(WORD LoginPoint) { m_HeroCharacterInfo.LoginPoint_Index = LoginPoint; }
	void			InitMapChangePoint() { m_HeroCharacterInfo.MapChangePoint_Index = 0; }
	void			SetMapChangePoint(WORD point) { m_HeroCharacterInfo.MapChangePoint_Index = point; }	// YH
	WORD			GetMapChangePoint() { return m_HeroCharacterInfo.MapChangePoint_Index;	} //hs for party
	void			SendPlayerToMap(MAPTYPE, float xpos, float zpos) ;
	BOOL			AddQuick( BYTE sheet, WORD pos, SLOT_INFO* pSlot );
	SLOT_INFO*		GetQuick( BYTE sheet, WORD pos );

	void			SetGod( BOOL val ) { m_God = val; }
	BOOL			IsGod() const { return m_God; }
	LEVELTYPE		GetPlayerLevelUpPoint() const { return m_HeroInfo.LevelUpPoint; }
	void			SetPlayerLevelUpPoint(LEVELTYPE);
	void SetSkillPoint( DWORD point, BYTE byForced ) ;

	DWORD				GetSkillPoint();
	///  
	LEVELTYPE			GetMaxLevel() { return m_HeroInfo.MaxLevel; }
	virtual				LEVELTYPE GetLevel();
	virtual void		SetLevel( LEVELTYPE level );

    /// ?
	EXPTYPE				GetPlayerExpPoint() { return m_HeroInfo.ExpPoint; }
	DOUBLE				GetPlayerTotalExpPoint();
	void				SetPlayerExpPoint(EXPTYPE);
	void				AddPlayerExpPoint(EXPTYPE);
	void				ReduceExpPoint( EXPTYPE minusExp, BYTE bLogType );
	FAMETYPE			GetBadFame() const { return m_HeroCharacterInfo.BadFame; }
	void				SetBadFame(FAMETYPE val) { m_HeroCharacterInfo.BadFame = val; }

	/// ?  
	PlayerStat&	GetCharStats() { return m_charStats; }

	// 080313 LUJ, ?  ?
	PlayerStat& GetSetItemStats() { return m_SetItemStats; };

	PlayerStat&	GetItemBaseStats() { return m_itemBaseStats; }
	PlayerStat&	GetItemStats() { return m_itemStats; }
	PlayerStat&	GetItemOptionStats() { return m_itemOptionStats; }


	// 070410 LYW --- Player : Add functions to return hero info.
	DWORD				GetRealVitality()		{ return m_HeroInfo.Vit ; }
	DWORD				GetRealWisdom()			{ return m_HeroInfo.Wis ; }
	DWORD				GetRealStrength()		{ return m_HeroInfo.Str ; }
	DWORD				GetRealDexterity()		{ return m_HeroInfo.Dex ; }
	DWORD				GetRealIntelligence()	{ return m_HeroInfo.Int ; }

	DWORD				GetVitality();
	DWORD				GetWisdom();
	DWORD				GetStrength();
	DWORD				GetDexterity();
	DWORD				GetIntelligence();
	
	void				SetVitality( DWORD val );
	void				SetWisdom( DWORD val );
	void				SetStrength( DWORD val );
	void				SetDexterity( DWORD val );
	void				SetIntelligence( DWORD val );

	void				AddStrength( int val );
	void				AddWisdom( int val );
	void				AddDexterity( int val );
	void				AddVitality( int val );
	void				AddIntelligence( int val );

	/// / 
	BYTE				GetLifePercent();
	BYTE				GetManaPercent();
	void				SetMaxLife( DWORD val );
	void				SetMaxMana( DWORD val );

	// 080625 LYW --- Player :   ? .
	// ??  ¿ ? ?  ?  , 
	// ? / 30%  ? ?. ? °  ¸, 
	//  ? return ó ??,   ?   ? ? ??.
	void SetLifeForce( DWORD Life, BYTE byForce, BOOL bSendMsg = TRUE) ;
	void SetManaForce(DWORD Mana, BYTE byForce, BOOL bSendMsg = TRUE) ;

	virtual DWORD		GetLife();
	virtual void		SetLife( DWORD Life, BOOL bSendMsg = TRUE );

	virtual DWORD		GetMana();
	virtual void		SetMana( DWORD valb, BOOL SendMsg = TRUE );	

	virtual DWORD		DoGetMaxLife();
	virtual DWORD		DoGetMaxMana();
	virtual DWORD		DoGetPhyAttackPowerMin();
	virtual DWORD		DoGetPhyAttackPowerMax();

	/// Ÿ
	virtual float		DoGetAddAttackRange() { return GetDexterity() / 3.f; }
	/// ?? ?
	virtual DWORD		DoGetCritical();
	virtual DWORD		DoGetDecisive();

	/// ?? 
	virtual float		DoGetMoveSpeed();
	///  
	void				CalcState();
	void				ClearMurderIdx();
	DWORD				GetMurderIDX() const { return m_MurdererIDX; }
	void						SetPenaltyByDie( BOOL bVal ) { m_bPenaltyByDie = bVal; }	//   ?  ?  TRUE //  ?? ü?  
	BOOL						IsPenaltyByDie() { return m_bPenaltyByDie; }
	BOOL						IsReadyToRevive() { return m_bReadyToRevive;	}
	void						SetReadyToRevive( BOOL val ) { m_bReadyToRevive = val;	}
	// 080602 LYW --- Player : ? ? (__int32)  (__int64)   ó.
	//DWORD						RevivePenalty(BOOL bAdditionPenalty) ;
	EXPTYPE						RevivePenalty(BOOL bAdditionPenalty) ;
	void						RevivePresentSpot();
	void						ReviveLogIn();
	void						ReviveAfterShowdown( BOOL bSendMsg = TRUE );

	void						ReviveLogInPenelty();
	// 080602 LYW --- Player : ? ? (__int32)  (__int64)   ó.
	//DWORD						ReviveBySkill();
	void						ReviveBySkill( cSkillObject* pSkillObject );

	// 081210 LYW --- Player :    ?   ? ? ??.
	void						ReviveLogIn_Normal() ;
	void						ReviveLogIn_GuildDungeon() ;
	DWORD						GetPartyIdx() const { return m_HeroInfo.PartyID; }
	void						SetPartyIdx(DWORD PartyIDx);
	void						SendLifeToParty(DWORD life);
	void						SendManaToParty(DWORD mana);
	DWORD						GetGuildIdx() const { return m_HeroCharacterInfo.MunpaID;	}
	void						SetGuildIdx( DWORD GuildIdx ) { m_HeroCharacterInfo.MunpaID = GuildIdx; }
	void						SetGuildInfo( DWORD GuildIdx, BYTE Grade, const char* GuildName, MARKNAMETYPE MarkName );
	BYTE						GetGuildMemberRank() { return m_HeroCharacterInfo.PositionInMunpa; 	}
	void						SetGuildMemberRank( BYTE rank ) { m_HeroCharacterInfo.PositionInMunpa = rank; }
	void						SetGuildName( char* GuildName ) { SafeStrCpy(m_HeroCharacterInfo.GuildName, GuildName, _countof(m_HeroCharacterInfo.GuildName)); }
	void						SetGuildMarkName( MARKNAMETYPE MarkName );
	char*						GetGuildCanEntryDate();
	void						SetGuildCanEntryDate( char* date );
	void						ClrGuildCanEntryDate();
	void						SetNickName( char* NickName );

	///  
	DWORD						GetGuildUnionIdx() { return m_HeroCharacterInfo.dwGuildUnionIdx; }
	void						InitGuildUnionInfo( DWORD dwGuildUnionIdx, char* pGuildUnionName, DWORD dwMarkIdx );
	void						SetGuildUnionInfo( DWORD dwGuildUnionIdx, char* pGuildUnionName, DWORD dwMarkIdx );
	void						SetGuildUnionMarkIdx( DWORD dwMarkIdx )	{ m_HeroCharacterInfo.dwGuildUnionMarkIdx = dwMarkIdx; }

	///  
	DWORD						GetFamilyIdx() const { return m_HeroCharacterInfo.FamilyID;	}
	void						SetFamilyIdx( DWORD FamilyIdx ) { m_HeroCharacterInfo.FamilyID = FamilyIdx; }
	void						SetFamilyNickName( char* NickName );

	void						AddQuest(CQuestBase*);
	BOOL						SetQuestState(DWORD QuestIdx, QSTATETYPE);
	CQuestGroup&				GetQuestGroup()	{ return m_QuestGroup; }
	
	/// 
	BOOL						AddFollowList( CMonster * pMob );
	void						RemoveFollowList( DWORD ID );
	BOOL						RemoveFollowAsFarAs( DWORD GAmount, CObject* pObject );
	WORD						m_wKillMonsterLevel;
	void						SetKillMonsterLevel( WORD wMonLevel ) { m_wKillMonsterLevel = wMonLevel; }
	WORD						GetKillMonsterLevel() { return m_wKillMonsterLevel; }
	virtual	DWORD				GetGravity() { return mGravity; }
	void						AddToAggroed(DWORD objectIndex);
	void						RemoveFromAggroed(DWORD objectIndex) { mAggroObjectContainer.erase(objectIndex); }
	void						RemoveAllAggroed();
	// ??    ?  ??
	virtual CObject*			GetTObject() const;
	virtual DWORD				GetAggroNum() { return mAggroObjectContainer.size(); }
	void						AddAggroToMyMonsters(int nAggroAdd, DWORD targetObjectIndex, DWORD skillIndex);
	void 						SetShowdown( BOOL bShowdown ) { m_bShowdown = bShowdown; }
	BOOL 						IsShowdown() { return m_bShowdown; }
	void 						SetShowdownOpPlayerID( DWORD dwOpPlayerID ) { m_dwOpPlayerID = dwOpPlayerID; }
	DWORD 						GetShowdownOpPlayerID()	{ return m_dwOpPlayerID; }

	/// PK
	int  						PKModeOn();
	BOOL  						PKModeOff();
	void  						PKModeOffForce();
	BOOL  						IsPKMode()							{ return m_HeroCharacterInfo.bPKMode; }
	void  						SetPKStartTimeReset()				{ m_dwPKModeStartTime = gCurTime; }
	//---KES PK 071202
	DWORD						GetPKContinueTime()					{ return m_dwPKContinueTime; }
	void  						SetPKContinueTime( DWORD dwTime )	{ m_dwPKContinueTime = dwTime; }
	//----------------
	void  						SetPKModeEndtime();

	/// ??
	void  						SetReturnMapNum( WORD num )	{ m_wGTReturnMapNum = num; }
	WORD  						GetReturnMapNum() { return m_wGTReturnMapNum; }
	void  						SetObserverBattleIdx( int Idx ) { m_ObserverBattleIdx = Idx; }
	int	 						GetObserverBattleIdx() { return m_ObserverBattleIdx; }
	void						AddKillCountFromGT() {m_dwKillCountFromGT++;}
	DWORD						GetKillCountFromGT() {return m_dwKillCountFromGT;}
	void						AddKillPointFromGT(WORD point) {m_dwKillPointFromGT += point;}
	DWORD						GetKillPointFromGT() {return m_dwKillPointFromGT;}
	void						SetRespawnTime(DWORD dwTime) {m_dwRespawnTimeOnGTMAP = dwTime;}
	void						SetImmortalTime(DWORD dwTime) {m_dwImmortalTimeOnGTMAP = dwTime;}
	DWORD						GetImmortalTime()				{return m_dwImmortalTimeOnGTMAP;}
	void						CheckImmortalTime();
	void						SetStartPosOnGTMAP(VECTOR3* pPos)	{m_vStartPosOnGTMAP = *pPos;}
	void						GetStartPosOnGTMAP(VECTOR3& pos)	{pos = m_vStartPosOnGTMAP;}
	void						SetDieForGFW(BOOL bDie) { m_bDieForGFW = bDie; }
	BOOL						IsDieFromGFW() const { return m_bDieForGFW; }
	///  ? ?
	float						GetAccuracy();
	float						GetAvoid();
	float						GetBlock();
	float						GetPhysicAttackMax();
	float						GetPhysicAttackMin();
	float						GetPhysicAttack();
	float						GetMagicAttackMax();
	float						GetMagicAttackMin();
	float						GetMagicAttack();
	float						GetPhysicDefense();
	float						GetMagicDefense();
	float						GetCriticalRate();
	float						GetCriticalDamageRate();
	float						GetCriticalDamagePlus();
	float						GetMagicCriticalRate();
	float						GetMagicCriticalDamageRate();
	float						GetMagicCriticalDamagePlus();
    float						GetMagicCriticalDamage();

    float						GetCriticalDamage();
	float						GetLifeRecover();
	float						GetManaRecover();
	DWORD GetShieldDefense();
	void SetShieldDefence(DWORD);
	void SpeedHackCheck();

	/// ?  ?
	virtual void	DoDie( CObject* pAttacker );
	virtual void	DoDamage( CObject* pAttacker, RESULTINFO* pDamageInfo, DWORD beforeLife );
	virtual DWORD	Damage( CObject* pAttacker, RESULTINFO* pDamageInfo );
	virtual void	DoManaDamage( CObject* pAttacker, RESULTINFO* pDamageInfo, DWORD beforeMana );
	virtual DWORD	ManaDamage( CObject* pAttacker, RESULTINFO* pDamageInfo );

	virtual void	StateProcess();
	
	/// ? ?
	void			QuestProcess();

	/// 
	void			SetWaitExitPlayer( BOOL bWait ) { m_bWaitExitPlayer = bWait; }
	BOOL			IsWaitExitPlayer() { return m_bWaitExitPlayer; }
	BOOL			IsExitStart() { return m_bExit; }
	int				CanExitStart();
	void			SetExitStart( BOOL bExit );
	int				CanExit();
	void			ExitCancel();
	void			SetNormalExit() { m_bNormalExit = TRUE; }
	BOOL			IsNormalExit() { return m_bNormalExit; }
	void			UpdateLogoutToDB(BOOL val = TRUE);
	
	void			SetEmergency() { mIsEmergency = TRUE; }
	BOOL			IsEmergency() { return mIsEmergency; }
	
	void			SetMapMoveInfo( MAPTYPE map, DWORD pos_x, DWORD pos_z )
	{
		mTargetMap = map;
		mTargetPosX = pos_x;
		mTargetPosZ = pos_z;
	}

	void			GetMapMoveInfo( MAPTYPE &map, DWORD &pos_x, DWORD &pos_z )
	{
		map = mTargetMap;
		pos_x =	mTargetPosX;
        pos_z =	mTargetPosZ;
	}

	ITEMOBTAINARRAYINFO*				GetArray( WORD id );
	void								AddArray( ITEMOBTAINARRAYINFO * pInfo );
	void								RemoveArray( DWORD id );
	CYHHashTable<ITEMOBTAINARRAYINFO>	m_ItemArrayList;

	///  ??
	CItemSlot *							GetSlot( POSTYPE absPos );
	CItemSlot *							GetSlot( eITEMTABLE tableIdx );

	void								AddStorageItem( ITEMBASE * pStorageItem );
	void								SetStorageNum( BYTE n );
	BYTE								GetStorageNum();

	void								InitItemTotalInfo(ITEM_TOTALINFO*);
	void								InitStorageInfo(BYTE storagenum, MONEYTYPE);
	BOOL								IsGotWarehouseItems() const { return m_bGotWarehouseItems; }
	void								SetGotWarehouseItems(BOOL bGot) { m_bGotWarehouseItems = bGot; }
	void								InitShopItemInfo(SEND_SHOPITEM_INFO&);
	void								GetItemtotalInfo(ITEM_TOTALINFO&, DWORD flag);
	DWORD								GetWearedItemIdx( EWEARED_ITEM WearedPosition ) const { return m_HeroCharacterInfo.WearedItemIdx[ WearedPosition ]; }
	void								SetWearedItemIdx( DWORD WearedPosition, DWORD ItemIdx );
	DWORD								GetWearedWeapon() const { return m_HeroCharacterInfo.WearedItemIdx[eWearedItem_Weapon]; }
	eWeaponType							GetWeaponEquipType();
	// 080703 LUJ, ?  ? ? Ÿ ??
	eWeaponAnimationType				GetWeaponAniType();
	eArmorType							GetArmorType( EWEARED_ITEM ) const;
	
	BOOL								RemoveItem(DWORD nItemID, DWORD nItemNum, eLogitemmoney eLogKind);
	
	MONEYTYPE							SetMoney( MONEYTYPE ChangeValue, BYTE bOper, BYTE MsgFlag = 0, eITEMTABLE tableIdx = eItemTable_Inventory, BYTE LogType = 0, DWORD TargetIdx = 0 );
	MONEYTYPE							GetMoney( eITEMTABLE tableIdx = eItemTable_Inventory );
	MONEYTYPE							GetMaxPurseMoney( eITEMTABLE TableIdx );
	void								SetMaxPurseMoney( eITEMTABLE TableIdx, DWORDEX MaxMoney );
	BOOL								IsEnoughAdditionMoney( MONEYTYPE money, eITEMTABLE tableIdx = eItemTable_Inventory );
	void								RSetMoney( MONEYTYPE money, BYTE flag );	
	void								SetExchangeContainer( sEXCHANGECONTAINER* ExchangeContainer ) { m_ExchangeContainer = *ExchangeContainer; }
	sEXCHANGECONTAINER*					GetExchangeContainer() { return &m_ExchangeContainer; }
	
	/// 
	void								SetGuestStall( cStreetStall* pStall ) { m_pGuetStall = pStall; }
	cStreetStall*						GetGuestStall() { return m_pGuetStall; }
	void								SetStreetStallTitle( char* title );
	void								GetStreetStallTitle( char* title );
	
	///    ?----------------------------------------------/
	///--------------------------------------------------------------------/

	
	///--------------------------------------------------------------------/
	/// ?   ?------------------------------------------------/

	// 080509 LUJ, ? Ÿ ü?
public:	
	// 080509 LUJ, ? Ÿ  ?  ??
	// 080516 LUJ,  ?? ü? ?    
	BOOL IsCoolTime( const ACTIVE_SKILL_INFO& );
	// 080511 LUJ, ? Ÿ ?´
	void SetCoolTime( const ACTIVE_SKILL_INFO& );
	void ResetCoolTime( const ACTIVE_SKILL_INFO& );
	BOOL IsCanCancelSkill();
	const DWORD GetSkillCancelDelay() const;

private:
	// 080509 LUJ, ?: ? ?, :   ???
	typedef stdext::hash_map< DWORD, DWORD > SkillCoolTimeMap;
	SkillCoolTimeMap mSkillCoolTimeMap;

	// 080514 LUJ, ? ??? Ÿ ü?. ??? ğ  ? ? ?   ä
	typedef stdext::hash_map< DWORD, DWORD > SkillAnimTimeMap;
	SkillAnimTimeMap mSkillAnimTimeMap;

	// 080515 LUJ, Ÿ ü?   ü
	struct CheckCoolTime
	{
		// 080515 LUJ, Ÿ ü?   ???
		DWORD mCheckedTick;
		// 080515 LUJ, Ÿ ü?  ?
		DWORD mFailureCount;
	}
	mCheckCoolTime;

	// 100617 ShinJS ? ?  ó
	enum eSkillCancelLimit
	{
		eSkillCancelLimit_Count = 3,
		eSkillCancelLimit_CheckTime = 5000,
	};

	DWORD m_dwSkillCancelCount;
	DWORD m_dwSkillCancelLastTime;

public:
	cSkillTree& GetSkillTree();
	void SetCurComboNum(DWORD combonum) { m_CurComboNum = combonum; }
	DWORD GetCurComboNum() const { return m_CurComboNum; }
	BOOL CanSkillState();
	void CancelCurrentCastingSkill( BOOL bUseSkillCancelRate );
	Status* GetPassiveStatus() { return &mPassiveStatus; }
	Status*	GetRatePassiveStatus() { return &mRatePassiveStatus; }
	// 090217 LUJ,  µ ? ? 
	void						SetHideLevel( WORD level );
	void						SetDetectLevel( WORD level );
	WORD						GetHideLevel() const { return m_HeroCharacterInfo.HideLevel; }
	WORD						GetDetectLevel() const { return m_HeroCharacterInfo.DetectLevel; }

	void						SetMonstermeterPlaytime(DWORD nTime, DWORD nTimeTotal)		{ m_stMonstermeterInfo.nPlayTime = nTime; m_stMonstermeterInfo.nPlayTimeTotal = nTimeTotal; }
	void						SetMonstermeterKillMonNum(DWORD nNum, DWORD nNumTotal)		{ m_stMonstermeterInfo.nKillMonsterNum = nNum; m_stMonstermeterInfo.nKillMonsterNumTotal = nNumTotal; }
	void						SetMonstermeterPlaytimeTick(DWORD nTick)					{ m_stMonstermeterInfo.nPlayTimeTick = nTick; }

	stMONSTERMETER*				GetMonstermeterInfo()										{ return &m_stMonstermeterInfo; }

	void						ProcMonstermeterPlayTime();
	void						ProcMonstermeterKillMon();
	DATE_MATCHING_INFO*			GetResidentRegistInfo()	{ return &m_DateMatching; }
	void						SetResidentRegistInfo(DATE_MATCHING_INFO* info) { m_DateMatching = *info; }

	void		SetDateMatchingSerchTimeTick(DWORD nTick)			{ m_DateMatching.nSerchTimeTick = nTick; }
	void		SetDateMatchingChatPlayerID(DWORD nChatPlayerID)	{ m_DateMatching.nChatPlayerID = nChatPlayerID; }
	void		SetDateMatchingChatState(DWORD nState)				{ m_DateMatching.nRequestChatState = nState; }
	void		SetDateMatchingChatTimeTick(DWORD nTick)			{ m_DateMatching.nRequestChatTimeTick = nTick; }
	// E ?? ? added by hseos 2007.06.04	2007.06.09

	// desc_hseos_??01
	// S ?? ? added by hseos 2007.07.06	2007.07.17	2007.11.22
	void						SetFamilyEmblem(char* pEmblem)			{ m_pcFamilyEmblem = pEmblem; }
	char*						GetFamilyEmblem()						{ return m_pcFamilyEmblem; }
	void						SetFamilyEmblemChangedFreq(DWORD nFreq)	{ m_HeroCharacterInfo.nFamilyEmblemChangedFreq = nFreq; }
	DWORD						GetFamilyEmblemChangedFreq()			{ return m_HeroCharacterInfo.nFamilyEmblemChangedFreq; }
	// E ?? ? added by hseos 2007.07.06	2007.07.17	2007.11.22

	// desc_hseos_ı_01
	// S ı ? added by hseos 2007.08.23
	stFARMPLAYERINFO*				GetFarmInfo()							{ return &m_stFarmInfo; }
	void							SetFarmCropPlantRetryTime(UINT nTime)	{ m_stFarmInfo.nCropPlantRetryTime = nTime; m_stFarmInfo.nCropPlantRetryTimeTick = gCurTime; }
	void							SetFarmCropManureRetryTime(UINT nTime)	{ m_stFarmInfo.nCropManureRetryTime = nTime; m_stFarmInfo.nCropManureRetryTimeTick = gCurTime; }
	void							ProcFarmTime();
	// E ı ? added by hseos 2007.08.23

	// desc_hseos_? _01
	// S ?  ? added by hseos 2007.11.27	2007.12.05	2008.01.22
	UINT							GetChallengeZoneEnterFreq()						{ return m_nChallengeZoneEnterFreq; }
	UINT							GetChallengeZoneEnterBonusFreq()				{ return m_nChallengeZoneEnterBonusFreq; }
	void							IncreaseChallengeZoneEnterFreq()				{ m_nChallengeZoneEnterFreq++; }
	void							SetChallengeZoneEnterFreq(UINT nFreq)			{ m_nChallengeZoneEnterFreq = nFreq; }
	void							SetChallengeZoneEnterBonusFreq(UINT nFreq)		{ m_nChallengeZoneEnterBonusFreq = nFreq; }

	UINT							GetChallengeZonePartnerID()						{ return m_nChallengeZonePartnerID; }
	void							SetChallengeZonePartnerID(DWORD nPlayerID)		{ m_nChallengeZonePartnerID = nPlayerID; }

	UINT							GetChallengeZoneSection()						{ return m_nChallengeZoneSection; }
	void							SetChallengeZoneSection(UINT nSection)			{ m_nChallengeZoneSection = nSection; }
	UINT							GetChallengeZoneStartState()					{ return m_nChallengeZoneStartState; }
	void							SetChallengeZoneStartState(UINT nState)			{ m_nChallengeZoneStartState = nState; }
	UINT							GetChallengeZoneStartTimeTick()					{ return m_nChallengeZoneStartTimeTick; }
	void							SetChallengeZoneStartTimeTick(UINT nTimeTick)	{ m_nChallengeZoneStartTimeTick = nTimeTick; }

	UINT							GetChallengeZoneMonsterNum()					{ return m_nChallengeZoneMonsterNum; }
	void							SetChallengeZoneMonsterNum(UINT nNum)			{ m_nChallengeZoneMonsterNum = nNum; }
	UINT							GetChallengeZoneKillMonsterNum()				{ return m_nChallengeZoneKillMonsterNum; }
	void							SetChallengeZoneKillMonsterNum(UINT nNum)		{ m_nChallengeZoneKillMonsterNum = nNum; }
	void							IncreaseChallengeZoneKillMonsterNum()			{ m_nChallengeZoneKillMonsterNum++; }

	UINT							GetChallengeZoneNeedSaveEnterFreq()				{ return m_bChallengeZoneNeedSaveEnterFreq; }
	void							SetChallengeZoneNeedSaveEnterFreq(BOOL bNeed)	{ m_bChallengeZoneNeedSaveEnterFreq = bNeed; }

	UINT							IsChallengeZoneCreateMonRightNow()				{ return m_bChallengeZoneCreateMonRightNow; }
	void							SetChallengeZoneCreateMonRightNow(BOOL bSet)	{ m_bChallengeZoneCreateMonRightNow = bSet; }

	UINT							GetChallengeZoneMonsterNumTillNow()				{ return m_nChallengeZoneMonsterNumTillNow; }
	void							SetChallengeZoneMonsterNumTillNow(UINT nNum)	{ m_nChallengeZoneMonsterNumTillNow = nNum; }

	DWORD							GetChallengeZoneClearTime()						{ return m_nChallengeZoneClearTime; }
	void							SetChallengeZoneClearTime(DWORD nClearTime)		{ m_nChallengeZoneClearTime = nClearTime; }

	// 091123 ONS ç ? ? ó ?
	DWORD							GetChallengeZoneExpRate()						{ return m_nChallengeZoneExpRate; }
	void							SetChallengeZoneExpRate(DWORD nExpRate)			{ m_nChallengeZoneExpRate = nExpRate; }
	// 070813 , ?? ? ü?
	BOOL IsInventoryPosition( POSTYPE );

	// 080507 KTH --
	void							SetFarmAnimalFeedRetryTime(UINT nTime)	{ m_stFarmInfo.nAnimalFeedRetryTime = nTime; m_stFarmInfo.nAnimalFeedRetryTimeTick = gCurTime; }
	void							SetFarmAnimalCleanRetryTime(UINT nTime)	{ m_stFarmInfo.nAnimalCleanRetryTime = nTime; m_stFarmInfo.nAnimalCleanRetryTimeTick = gCurTime; }
	
	// 070705 ?  ? 
public:
	// 071207 LUJ ?  ??
	void	ResetSetItemStatus();

	typedef std::map< const SetScript*, int >	SetItemLevel;

	const SetItemLevel&	GetSetItemLevel() const;
	SetItemLevel&		GetSetItemLevel();
private:
	SetItemLevel mSetItemLevel;

	// 070719 ? ? 
	// , 070719 ? ?ú ? 
public:
	void AddSetSkill(DWORD skillIndex, LEVELTYPE);
	void RemoveSetSkill(DWORD skillIndex, LEVELTYPE);
	void AddJobSkill( DWORD skillIndex, BYTE level );
	void ClearJobSkill();
private:
	typedef std::list<SKILL_BASE> JobSkillList;
	JobSkillList m_JobSkillList;
	
	// 071005 , Ÿ
public:
	BOOL AddCoolTime(DWORD coolTimeGroupIndex, DWORD coolTime);
	void RemoveCoolTime( DWORD coolTimeGroupIndex);
	
private:
	void MoneyUpdate(MONEYTYPE);
	void UpdateGravity();
	void ProcCoolTime();
	virtual	void SetObjectBattleState(eObjectBattleState);

	struct CoolTime
	{
		DWORD mBeginTick;
		DWORD mEndTick;
	};

	// ?: Ÿ ? ?
	typedef std::map< DWORD, CoolTime > CoolTimeMap;
	CoolTimeMap mCoolTimeMap;
	DWORD m_dwLastTimeCheckItemDBUpdate;
	DWORD m_dwLastTimeCheckItem;
public:
	void ProcessTimeCheckItem( BOOL bForceDBUpdate );

	// 071128 LYW --- Player : HPMP .
	void SetHP_Point( BYTE hpPoint ) { m_byHP_Point = hpPoint ; }
	BYTE GetHP_Point() { return m_byHP_Point ; }
	void SetMP_Point( BYTE mpPoint ) { m_byMP_Point = mpPoint ; }
	BYTE GetMP_Point() { return m_byMP_Point ; }

	// 080213 KTH -- ClearInventory
	BOOL ClearInventory();
	
	// 071206 KTH -- ?  ??  ?.
	BOOL	IsResetSkill()	{	return m_bResetSkill;	}
	void	SetResetSkill( BOOL bResetSkill )	{	m_bResetSkill = bResetSkill;	}
	BOOL	IsResetStat()	{	return m_bResetStat;	}
	void	SetResetStat( BOOL bResetStat )	{	m_bResetStat = bResetStat;	}

private:
	BOOL	m_bResetSkill;
	BOOL	m_bResetStat;

protected:
	DWORDEX	m_dwReturnSkillMoney;	//  ?  
public:
	void	SetRetrunSkillMoney( DWORDEX ReturnSkillMoney )	{	m_dwReturnSkillMoney = ReturnSkillMoney;	}
	DWORDEX	GetReturnSkillMoney()							{	return m_dwReturnSkillMoney;	}

	// 071210 KTH -- ??? 
	WORD	GetInventoryExpansion()							{ return m_HeroCharacterInfo.wInventoryExpansion; }
	// 071225 KTH -- (Fix) ? ??    ? ?   ? ? ?.
	void	SetInventoryExpansion(WORD InventoryExpansion)	{ 
																m_HeroCharacterInfo.wInventoryExpansion = InventoryExpansion; 
																m_InventorySlot.SetSlotNum( (POSTYPE) ( SLOT_INVENTORY_NUM + GetInventoryExpansionSize() ));
															}
	BOOL	IncreaseInventoryExpansion();
	// 071213 KTH -- ??? ? 
	WORD	GetInventoryExpansionSize()						{	return (WORD) ( m_HeroCharacterInfo.wInventoryExpansion * TABCELL_INVENTORY_NUM ) ;	}

	// 071226 KTH -- ? .
	VOID	SetCharacterName(char* name)					{	strcpy(m_BaseObjectInfo.ObjectName, name);	}
	virtual void ProceedToTrigger();
	void PassiveSkillCheckForWeareItem();

//---KES AUTONOTE
protected:
	DWORD m_dwAutoNoteIdx;
	DWORD m_dwLastActionTime;

	// 090105 ShinJS --- ?  ?  ?  ğ  ?
	DWORD m_dwAutoNoteLastExecuteTime;			// ?   ğ( ? )
public:
	DWORD GetAutoNoteIdx() const { return m_dwAutoNoteIdx; }
	void SetAutoNoteIdx(DWORD AutoNoteIdx) { m_dwAutoNoteIdx = AutoNoteIdx; }

	void SetCharacterSize(float fSize) { m_HeroCharacterInfo.Width = m_HeroCharacterInfo.Height = fSize; }
	float GetCharacterSize() const { return m_HeroCharacterInfo.Height; }
	DWORD GetLastActionTime() const { return m_dwLastActionTime; }
	void SetLastActionTime( DWORD dwLastActionTime ) { m_dwLastActionTime = dwLastActionTime; }

	// 090105 ShinJS --- ? ğ  ? ?
	DWORD GetAutoNoteLastExecuteTime() const { return m_dwAutoNoteLastExecuteTime; }
	void SetAutoNoteLastExecuteTime() { m_dwAutoNoteLastExecuteTime = gCurTime; }

	void SetWeaponEnchant( BYTE level )
	{
		m_HeroCharacterInfo.WeaponEnchant = level;
	}

// 080411 NYJ - 
protected:
	DWORD m_dwFishingPlaceIdx;				// NPC Instance Index
	DWORD m_dwFishingStartTime;				// ?ğ
	DWORD m_dwFishingProcessTime;			// ğ
	DWORD m_dwFishingBaitIdx;				//  ? Item index

	float m_fFishItemRate[eFishItem_Max];	//   ? ?
	float m_fFishBaitRate[eFishItem_Max];	// ?  ? ?


	DWORD m_dwFM_MissionCode;				//  ?
	DWORD m_dwFM_StartTime;					// ?  ğ

	// ğõŸ
	WORD  m_wFishingLevel;					// üõ
	// 080602 LYW --- Player : ? ? (__int32)  (__int64)   ó.
	//DWORD m_dwFishingExp;					// üõ
	EXPTYPE m_dwFishingExp;					// üõ

	DWORDEX m_dwFishPoint;					// ?
	
public:
	std::list<DWORD> m_lstGetFishList;		// ? ?  3 , FM(FishingMission)

	void FishingInfoClear();				// ğ ?? ?.

	void SetFishingPlace(DWORD dwIndex)						{m_dwFishingPlaceIdx = dwIndex;}
	DWORD GetFishingPlace()									{return m_dwFishingPlaceIdx;}

	void SetFishingStartTime(DWORD dwTime)					{m_dwFishingStartTime = dwTime;}
	DWORD GetFishingStartTime()								{return m_dwFishingStartTime;}

	void SetFishingProcessTime(DWORD dwTime)				{m_dwFishingProcessTime = dwTime;}
	DWORD GetFishingProcessTime()							{return m_dwFishingProcessTime;}

	void SetFishingBait(DWORD dwIndex)						{m_dwFishingBaitIdx = dwIndex;}
	DWORD GetFishingBait()									{return m_dwFishingBaitIdx;}

	float* GetFishItemRate()								{return m_fFishItemRate;}
	float GetFishItemRate_Grade(int nGrade)					{if(0<=nGrade && nGrade<eFishItem_Max)	return m_fFishItemRate[nGrade];	return 0.0f;}

	float* GetFishBaitRate()								{return m_fFishBaitRate;}
	float GetFishBaitRate_Grade(int nGrade)					{if(0<=nGrade && nGrade<eFishItem_Max)	return m_fFishBaitRate[nGrade];	return 0.0f;}


	void SetFM_StartTime(DWORD dwStartTime)					{m_dwFM_StartTime = dwStartTime;}
	DWORD GetFM_StartTime()									{return m_dwFM_StartTime;}

	// ù??
	void SetFM_Mission(DWORD dwCode)						{m_dwFM_MissionCode = dwCode;}
	DWORD GetFM_Mission()									{return m_dwFM_MissionCode;}

	// ğõŸ
	void SetFishingLevel(WORD wLevel)						{m_wFishingLevel = wLevel;}
	WORD GetFishingLevel()									{return m_wFishingLevel;}
	void SetFishingExp(EXPTYPE);
	EXPTYPE GetFishingExp() const {return m_dwFishingExp;}
	void SetFishPoint(DWORDEX dwPoint) {m_dwFishPoint = dwPoint; if(MAX_FISHPOINT<m_dwFishPoint) m_dwFishPoint=MAX_FISHPOINT;}
	DWORDEX GetFishPoint() const {return m_dwFishPoint;}
	BOOL IsNoEquip(eArmorType, eWeaponType, eWeaponAnimationType);
	BOOL IsEnable(const BUFF_SKILL_INFO&);
	virtual float GetBonusRange() const;

	void SetPetItemDbIndex(DWORD itemDbIndex) { mPetIndex = itemDbIndex; }
	DWORD GetPetItemDbIndex() const { return mPetIndex; }
	virtual void AddSpecialSkill(const cBuffSkillInfo*);
	virtual void RemoveSpecialSkill(const cBuffSkillInfo*);
private:
	// 090204 LUJ, ? ? ? Ÿ? ü??
	void ProcSpecialSkill();

	struct SpecialSkillData
	{
		const cBuffSkillInfo* mBuffSkillInfo;
		BOOL mIsTurnOn;
	};
	typedef std::list< SpecialSkillData > SpecialSkillList;
	SpecialSkillList mSpecialSkillList;

	// 081119 NYJ - ?
protected:
	WORD					m_wCookLevel;
	WORD					m_wCookCount;
	WORD					m_wEatCount;
	WORD					m_wFireCount;
	DWORD					m_dwLastCookTime;
	stRecipeLv4Info			m_MasterRecipe[MAX_RECIPE_LV4_LIST];
public:
	void					SetCookLevel(WORD wLevel)	{m_wCookLevel = wLevel;}
	void					SetCookCount(WORD wCount)	{m_wCookCount = wCount;}
	void					SetEatCount(WORD wCount)	{m_wEatCount = wCount;}
	void					SetFireCount(WORD wCount)	{m_wFireCount = wCount;}
	void					SetLastCookTime(DWORD dwTime){m_dwLastCookTime = dwTime;}
	void					SetMasterRecipe(POSTYPE pos, DWORD dwRecipe, DWORD dwRemainTime);

	WORD					GetCookLevel()				{return m_wCookLevel;}
	WORD					GetCookCount()				{return m_wCookCount;}
	WORD					GetEatCount()				{return m_wEatCount;}
	WORD					GetFireCount()				{return m_wFireCount;}
	DWORD					GetLastCookTime()			{return m_dwLastCookTime;}
	stRecipeLv4Info*		GetMasterRecipe(POSTYPE pos);
	
	int						CanAddRecipe(DWORD dwRecipe);
	void					ProcessRecipeTimeCheck(DWORD dwElapsedTime);


	/// 090213 LYW --- Player :   ??    ?  ó.
public :
	void							SetFamilyRewardExp( EXPTYPE rewardExp )	{ m_dweFamilyRewardExp = rewardExp ; }
	void							SetCurFamilyMemCnt( BYTE byCount )		{ m_byCurFamilyMemCnt = byCount ; }
	BYTE							GetCurFamilyMemCnt() const				{ return m_byCurFamilyMemCnt ; }

	// 090316 LUJ, ?
public:
	inline DWORD GetSummonedVehicle() const { return mSummonedVehicleIndex; }
	inline void SetSummonedVehicle( DWORD vehicleIndex ) { mSummonedVehicleIndex = vehicleIndex; }
	inline DWORD GetMountedVehicle() const { return mMountedVehicleIndex; }
	inline void SetMountedVehicle( DWORD vehicleIndex ) { mMountedVehicleIndex = vehicleIndex; }
private:
	DWORD mSummonedVehicleIndex;
	DWORD mMountedVehicleIndex;

public:
	// 091123  ?õ  ÷ ? 
	void SetUsingWaterSeedFlag( BOOL bFlag ) { m_HeroCharacterInfo.bUsingWaterSeed = bFlag ; }


	// NYJ, ??
private:
	char				m_szHouseName[MAX_HOUSING_NAME_LENGTH];

public:
	void				SetHouseName(char* pHouseName)	{strcpy(m_szHouseName, pHouseName);}
	char*				GetHouseName()					{return m_szHouseName;}
	
	// 100211 ONS ??   ?ó ?.
private:
	DWORD		m_dwCurrentResurrectIndex;					//  ?? ?, ? ? ? ?.
public:
    void		SetCurResurrectIndex( DWORD dwIndex ) { if( !m_dwCurrentResurrectIndex ) m_dwCurrentResurrectIndex = dwIndex; }
	DWORD		GetCurResurrectIndex(){ return m_dwCurrentResurrectIndex; }
	EXPTYPE		OnResurrect();

	// NYJ, ??
private:
	BOOL		m_bDungeonObserver;
	VECTOR3		m_DungeonObserverPos;
public:
	void		SetDungeonObserver(BOOL bVal)		{m_bDungeonObserver = bVal;}
	BOOL		IsDungeonObserver()					{return m_bDungeonObserver;}
	void		SetDungeonObserverPos(VECTOR3* pPos){memcpy(&m_DungeonObserverPos, pPos, sizeof(VECTOR3));}
	VECTOR3*	GetDungeonObserverPos()				{return &m_DungeonObserverPos;}

private:
	DWORD		m_dwConsignmentTick;
public:
	void		SetConsignmentTick()				{m_dwConsignmentTick = gCurTime;}
	DWORD		GetConsignmentTick()				{return m_dwConsignmentTick;}

	// 100624 ONS ?   / HP?ó ?
	void AddLifeRecoverTime( const YYRECOVER_TIME& recoverTime );
	void AddManaRecoverTime( const YYRECOVER_TIME& recoverTime );
	void UpdateLife();
	void UpdateMana();
	void SetManaRecoverDirectly( DWORD recoverUnitAmout ) { m_ManaRecoverDirectlyAmount = recoverUnitAmout; }

// 100611 ONS ÷?? äñ ó ?.
private:
	__time64_t	ForbidChatTime;
public:
	void		SetForbidChatTime( __time64_t time ) { ForbidChatTime = time; }
	__time64_t	GetForbidChatTime() const { return ForbidChatTime; }
	BOOL		IsForbidChat() const;

};
