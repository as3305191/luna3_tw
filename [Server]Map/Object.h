#pragma once

#include "GridUnit.h"

class CBattle;
class cBuffSkillInfo;
// 080616 LUJ, �̺�Ʈ �߻� �� ���� ���� ��ų�� �׼��� ���ϱ� ���� ����
class CEvent;
class cBuffSkillObject;

enum EObjectKind
{
	// 080616 LUJ, �ʱ�ȭ�� �� �߰�
	eObjectKind_None,
	eObjectKind_Player = 1,
	eObjectKind_Npc		=	2,
	eObjectKind_Item	=	4,
	eObjectKind_SkillObject=16,
	eObjectKind_Monster	=	32,
	eObjectKind_BossMonster=33,
	eObjectKind_SpecialMonster=34,
	
	// �ʵ庸�� - 05.12 �̿���
	eObjectKind_FieldBossMonster=35,
	eObjectKind_FieldSubMonster=36,
	eObjectKind_ToghterPlayMonster=37,
	// desc_hseos_����Ʈ ��_01
	// S ����Ʈ �� �߰� added by hseos 2007.11.23
	eObjectKind_ChallengeZoneMonster = 38,
	// E ����Ʈ �� �߰� added by hseos 2007.11.23
	// 080616 LUJ, ���� �߰�
	eObjectKind_Trap = 39,
	// 090316 LUJ, Ż��
	eObjectKind_Vehicle		= 40,
	eObjectKind_MapObject	= 64,
	eObjectKind_Pet			= 128,
};

class CObject : public CGridUnit
{
	BOOL m_bInited;
	friend class CObjectFactory;
	friend class CCharMove;
	friend class CObjectStateManager;

	
protected:
	CObject();
	virtual ~CObject();

	DWORD m_AgentNum;
	EObjectKind m_ObjectKind;
	BASEOBJECT_INFO m_BaseObjectInfo;
	MOVE_INFO m_MoveInfo;
	STATE_INFO	m_ObjectState;

	RECOVER_TIME m_LifeRecoverTime;
	YYRECOVER_TIME m_YYLifeRecoverTime;

	// 100624 ONS HP������Ʈ�� ���� ť�� �����Ѵ�.
	std::queue< YYRECOVER_TIME >	m_YYLifeRecoverTimeQueue;
	// 100729 ONS �������� HP�� ��ȯ�Ұ�� ��� �����Ѵ�.
	DWORD			m_LifeRecoverDirectlyAmount;

	AbnormalStatus mAbnormalStatus;

	virtual void DoDamage(CObject*,RESULTINFO*,DWORD) {}
	virtual void DoManaDamage(CObject*,RESULTINFO*,DWORD) {}
	void RemoveSummonedAll();
	friend class CCharacterCalcManager;

	CYHHashTable< cBuffSkillObject > m_BuffSkillList;
	Status				mBuffStatus;
	Status				mRateBuffStatus;
	DWORD				mCurrentSkillID;
	// ����, Ʈ���ſ��� ��Ī �������� �����
	DWORD				mAlias;
	DWORD				mGroupAlias;
	WORD				mEffectMoveCount;
	// 091113 LUJ, ���� Ʈ���� �޽����� ���� �ð�
	DWORD mNextCheckedTick;
	BOOL mDieFlag;

	// 090216 LUJ, �����̳��� ������ �°� �̸� ����
	// 100309 ShinJS --- �̺�Ʈ ��ų �������θ� �Ǵ��ϱ� ���� ���� �߰�
	typedef std::list< std::pair< DWORD, eStatusKind > > EventSkillObjectIndexList;
	EventSkillObjectIndexList mEventSkillObjectIndexList;
	typedef std::map< eStatusKind, int > EventSkillKindCountMap;
	EventSkillKindCountMap m_pEventSkillKindCountMap;

	typedef DWORD SkillIndex;
	typedef DWORD ObjectIndex;
	typedef std::set< ObjectIndex > ObjectIndexContainer;
	typedef std::map< SkillIndex, ObjectIndexContainer > SummonObjectContainer;
	SummonObjectContainer mSummonObjectContainer;

	DWORD m_dwShockedTime;

    typedef std::map< DWORD, DWORD > SkillCooldownContainer;
	SkillCooldownContainer mSkillCooldown; 

public:
	DWORD CurCastingSkillID;



    DWORD GetLastCastSkill(DWORD);
	void UpdateLastCastSkill(DWORD, DWORD);

	const STATE_INFO& GetStateInfo() const { return m_ObjectState; }
	virtual void SetState(EObjectState);
	virtual CObject* GetTObject() const { return 0; }
	void AddSummoned(const ACTIVE_SKILL_INFO&, DWORD objectIndex);
	void RemoveSummoned(const ACTIVE_SKILL_INFO&);
	void RemoveSummonedOldest(const ACTIVE_SKILL_INFO&);
	int GetSummondSize(const ACTIVE_SKILL_INFO&);
	virtual DWORD GetOwnerIndex() const { return 0; }
	BYTE GetBattleTeam() const { return m_BaseObjectInfo.BattleTeam; }
	void SetBattleTeam( DWORD BattleTeam ) { m_BaseObjectInfo.BattleTeam = (BYTE)BattleTeam; }
	DWORD GetBattleID()	const {	return m_BaseObjectInfo.BattleID; }
	void SetBattleID(DWORD BattleID) { m_BaseObjectInfo.BattleID = BattleID; }
	CBattle* GetBattle();
	void SetBattle( DWORD BattleID, BYTE Team, DWORD Param=0 );
	
	virtual void SendMsg(MSGBASE* pMsg,int MsgLen);
	inline DWORD GetAgentNum() const { return m_AgentNum; }
	virtual float GetBonusRange() const { return 0; }
	virtual BOOL Init(EObjectKind kind,DWORD AgentNum, BASEOBJECT_INFO* pBaseObjectInfo);
	virtual void Release();

	inline EObjectKind GetObjectKind() const { return m_ObjectKind;	}
	inline void SetObjectKind(EObjectKind kind)	{ m_ObjectKind = kind; }
	inline LPTSTR GetObjectName() { return m_BaseObjectInfo.ObjectName; }
	inline EObjectState GetState() const {return m_BaseObjectInfo.ObjectState; }
	void GetPosition(VECTOR3*);
	BASEMOVE_INFO* GetMoveInfo() { return &m_MoveInfo; }
	void SetInited() { m_bInited = TRUE; }
	void SetNotInited() { m_bInited = FALSE; }
	BOOL GetInited() const { return m_bInited; }
	inline DWORD GetID() const { return m_BaseObjectInfo.dwObjectID; }
	inline DWORD GetUserID() const { return m_BaseObjectInfo.dwUserID; }

	void GetBaseObjectInfo(BASEOBJECT_INFO* pRtInfo);
	virtual void GetSendMoveInfo(SEND_MOVEINFO* pRtInfo,CAddableInfoList* pAddInfoList);
	// 091026 LUJ, ���� ũ�⿡ �����ϰ� ������ �� �ֵ��� ����
	virtual DWORD SetAddMsg(DWORD receivedObjectIndex, BOOL isLogin, MSGBASE*& message) { return 0; }
	virtual DWORD SetRemoveMsg(DWORD dwReceiverID, MSGBASE*&);
	// �÷��̾�� �������� �����Ǿ� ���� �ʴ�. �Ʒ� ��ġ�� �����ϴ�.
	virtual float GetRadius() const { return 100.0f; }
	virtual void OnStartObjectState(EObjectState, DWORD dwParam) {}
	virtual void OnEndObjectState(EObjectState) {}
	virtual void StateProcess() {}
	virtual void ProceedToTrigger() {}
	virtual int GetObjectTileSize();
	
	//////////////////////////////////////////////////////////////////////////
	// ������ ���� ����
	void Die(CObject* pAttacker);
	virtual void DoDie(CObject* pAttacker)	{}
	virtual void CalcRealDamage(CObject* pOperator,WORD PhyDamage,WORD AttrDamage,RESULTINFO* pDamageInfo);
	virtual DWORD Damage(CObject* pAttacker,RESULTINFO*);
	virtual DWORD ManaDamage(CObject* pAttacker,RESULTINFO*);
	virtual LEVELTYPE GetLevel() { return 0; };
	virtual void SetLevel(LEVELTYPE level) {};
	virtual DWORD GetLife() { return 0; }
	virtual void SetLife(DWORD Life,BOOL bSendMsg = TRUE) {};
	virtual DWORD GetMana() { return 0; }
	virtual void SetMana(DWORD val,BOOL bSendMsg = TRUE) {};
	virtual DWORD GetRecoverLife() { return 0; }
	virtual DWORD GetRecoverMana() { return 0; }
	DWORD GetMaxLife() { return DoGetMaxLife(); }
	DWORD GetMaxMana() { return DoGetMaxMana(); }
	DWORD GetPhyDefense() { return DoGetPhyDefense(); }
	DWORD GetPhyAttackPowerMin() { return DoGetPhyAttackPowerMin(); }
	DWORD GetPhyAttackPowerMax() { return DoGetPhyAttackPowerMax(); }
	DWORD GetMagicAttackPower() { return DoGetMagicAttackPower(); }
	DWORD GetMagicDefense() { return DoGetMagicDefense(); }
	DWORD GetShieldDefense() { return DoGetShieldDefense(); }
	DWORD GetCritical() { return DoGetCritical(); }
	DWORD GetAccuracy() { return DoGetAccuracy(); }
	DWORD GetAvoid() { return DoGetAvoid(); }

	virtual DWORD DoGetMaxLife()	{return 0;}
	virtual DWORD DoGetMaxMana()	{return 0;}
	virtual DWORD DoGetPhyDefense()	{return 0;}
	virtual DWORD DoGetPhyAttackPowerMin()	{return 0;}
	virtual DWORD DoGetPhyAttackPowerMax()	{return 0;}
	virtual DWORD DoGetMagicAttackPower()	{return 0;}
	virtual DWORD DoGetMagicDefense()	{return 0;}
	virtual DWORD DoGetShieldDefense()	{return 0;}
	virtual DWORD DoGetCritical()	{return 0;}
	virtual DWORD DoGetAccuracy()	{return 0;}
	virtual DWORD DoGetAvoid()	{return 0;}


	float GetMoveSpeed() { return DoGetMoveSpeed(); }
	float GetUngiPlusRate() { return DoGetUngiPlusRate(); }
	float GetAddAttackRange() { return DoGetAddAttackRange(); }
	float GetDodgeRate() { return DoGetDodgeRate(); }
	
	virtual float DoGetMoveSpeed()	{return 0;}
	virtual float DoGetUngiPlusRate()	{return 0;}
	virtual float DoGetAddAttackRange()	{return 0;}
	virtual float DoGetDodgeRate()	{return 0;}
	virtual float GetAttDefense(WORD) { return 0; }
	virtual float DoGetAttDefense(WORD)	{return 0;}
	virtual void SetMaxLife(WORD life) {};
	virtual void SetMaxMana(WORD val) {};
	void AddLife(DWORD val, DWORD* realAddLife,BOOL bSendMsg = TRUE);
	void ReduceLife(DWORD val);
	void AddMana(DWORD val,DWORD* realAddMana);
	void ReduceMana(DWORD val);
	
	void CheckLifeValid();
	void CheckManaValid();

	virtual DWORD GetGravity() { return 0; }
	CYHHashTable< cBuffSkillObject >& GetBuffList() { return m_BuffSkillList; }
	AbnormalStatus*	GetAbnormalStatus() { return &mAbnormalStatus; }

	Status*	GetBuffStatus() { return &mBuffStatus; }
	Status*	GetRateBuffStatus() { return &mRateBuffStatus; }
	// 090204 LUJ, ������ �����ǵ��� �ϴ� �˻� ���� ���ҽ�Ų��
	virtual void RemoveBuffCount( eBuffSkillCountType, int count );
	// 090204 LUJ, �Ļ� ��ü���� �������̵��� �� �ֵ��� ���� �Լ��� �����
	virtual void AddSpecialSkill( cBuffSkillInfo* ) {}
	virtual void RemoveSpecialSkill( cBuffSkillInfo* ) {}
	void	EndSkill();
	void	EndAllSkill();
	inline void SetCurrentSkill(DWORD idx) { mCurrentSkillID = idx; }
	inline DWORD GetCurrentSkill() const { return mCurrentSkillID; }
	void EndBuffSkillByStatus(WORD Status);
	virtual	void SetObjectBattleState(eObjectBattleState state ) { m_BaseObjectInfo.ObjectBattleState = state; }
	eObjectBattleState GetObjectBattleState() const { return m_BaseObjectInfo.ObjectBattleState; }
	void SetEffectMove() { ++mEffectMoveCount; }
	BOOL IsEffectMove();
	virtual DWORD GetAggroNum() { return 0; }
	void SetAlias(DWORD alias) { mAlias = alias; }
	DWORD GetAlias() const { return mAlias; }
	void SetGroupAlias(DWORD alias) { mGroupAlias = alias; }
	DWORD GetGroupAlias() const { return mGroupAlias; }
	virtual void Execute(const CEvent&);
	void AddEventSkill(cBuffSkillObject&);
	BOOL HasEventSkill( eStatusKind status );
	BOOL GetDieFlag() const { return mDieFlag; }
	void SetShockTime( DWORD dwTime );
	BOOL IsShocked();
	// 100624 ONS ť�� ������ ���� / HP������Ʈó�� �߰�
	virtual void AddLifeRecoverTime( const YYRECOVER_TIME& recoverTime );
	virtual void UpdateLife();
	// 100729 ONS �������� HP�� ��ȯ�Ұ�� ��� �����Ѵ�.
	virtual void SetLifeRecoverDirectly( DWORD recoverUnitAmout ) { m_LifeRecoverDirectlyAmount = recoverUnitAmout; }
};