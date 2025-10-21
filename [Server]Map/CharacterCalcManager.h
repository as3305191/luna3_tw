#pragma once


#define CHARCALCMGR CCharacterCalcManager::GetInstance()


class CPlayer;
class CPet;

struct SetScript;
struct ITEM_INFO;

struct JobSkillInfo
{
	WORD	wClassIndex;
	DWORD	dwSkillIndex;
	BYTE	byPlusLevel;
};

class CCharacterCalcManager  
{
	cPtrList	m_HPMPDeterminateList ;
	std::list<JobSkillInfo>	m_JobSkillList;
	//std::hash_map<WORD, JobSkillInfo> m_JobSkillMap;

public:
	static CCharacterCalcManager* GetInstance();
	CCharacterCalcManager();
	virtual ~CCharacterCalcManager();

	void CalcMaxLife( CPlayer* );
	void CalcMaxMana( CPlayer* );

	BOOL LoadHPMPDeterminateList();

	// 081022 KTH -- Load File Job Skill
	BOOL LoadJobSkillList();

	void Init_HPMP_Point(CPlayer*);
	BOOL Get_HPMP_Point(CPlayer*, BYTE* hpPoint, BYTE* mpPoint);

	cPtrList* GetHPMPDetermiateList() { return &m_HPMPDeterminateList ; }

	void CalcCharPhyAttackMax( CPlayer* );
	void CalcCharPhyAttackMin( CPlayer* );
	void CalcCharPhyDefense( CPlayer* );

	void CalcCharMagAttack( CPlayer* );
	void CalcCharMagDefense( CPlayer* );

	void CalcCharAccuracy( CPlayer* );

	void CalcCharEvasion( CPlayer* );

	void CalcCharCriticalRate( CPlayer* );
	void CalcCharCriticalDamage( CPlayer* );
	void CalcCharMagicalCriticalRate( CPlayer* );
	void CalcCharMagicalCriticalDamage( CPlayer* );
	void CalcCharShieldDefense( CPlayer* );

	static void ProcessLife(CPlayer*);
	static void ProcessMana(CPlayer*);
	
	static void ProcessPetLife(CPet*);
	static void ProcessPetMana(CPet*);
	static void ProcessLifeBoss(CBossMonster*);
	static void General_LifeCount(CPlayer*, DWORD curTime, LEVELTYPE level, DWORD life, DWORD maxlife);
	static void General_ManaCount(CPlayer*, DWORD curTime, LEVELTYPE level, DWORD life, DWORD maxlife);
	static void General_PetLifeCount(CPet*, DWORD curTime, LEVELTYPE level, DWORD life, DWORD maxlife);
	static void General_PetManaCount(CPet*, DWORD curTime, LEVELTYPE level, DWORD life, DWORD maxlife);
	static void General_LifeCountBoss(CBossMonster * pBoss, DWORD curTime, DWORD life, DWORD maxlife);
	static void UpdateLife( CObject* );
	static void StartUpdateLife( CObject*, DWORD plusLife, BYTE recoverInterval=RECOVERINTERVALUNIT, DWORD recoverTime=RECOVERTIMEUNIT);

	static void UpdateMana(CPlayer*);
	static void StartUpdateMana(CPlayer*, DWORD plusMana, BYTE recoverInterval=RECOVERINTERVALUNIT, DWORD recoverTime=RECOVERTIMEUNIT);

	void Initialize(CPlayer*);
	void AddItem(CPlayer*, const ITEMBASE&);
	void RemoveItem(CPlayer*, const ITEMBASE&);
	void CalcCharStats(CPlayer*);
	void ArrangeCharLevelPoint(CPlayer*, WORD type);
	void AddPlayerJobSkill(CPlayer*);

private:
	void AddStat	( const ITEM_INFO&,		PlayerStat& );

	// 080319 LUJ, ?? ??? ??? ???
	void AddStat	( const ITEM_INFO&,		const ITEM_OPTION&,	PlayerStat& );
	void AddStat	( const PlayerStat&,	PlayerStat& );
	void RemoveStat	( const ITEM_INFO&,		PlayerStat& );

	// 080319 LUJ, ?? ??? ??? ???
	void RemoveStat	( const ITEM_INFO&,		const ITEM_OPTION&,	PlayerStat& );
	void RemoveStat	( const PlayerStat&,	PlayerStat& );

	void AddSetItemStats	( CPlayer*, const SetScript&, int setItemSize );
	void RemoveSetItemStats	( CPlayer*, const SetScript&, int setItemSize );

	// 100226 ShinJS --- ??? ?? ?? ??? ????? ??(?/???/??)
	BOOL IsInvalidEquip( CPlayer* pPlayer ) const;
};
