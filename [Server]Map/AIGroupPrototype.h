#pragma once

enum EObjectKind;

class CAIGroup
{
public:
	struct Parameter
	{
		EObjectKind	ObjectKind;
		WORD wMonsterKind;
		VECTOR3 vPos;
		TCHAR mMachine[MAX_PATH];
	};

public:
	CAIGroup();
	virtual ~CAIGroup();
	void Die(DWORD id);
	void RegenCheck();
	void AddCondition(DWORD targetGroup, float ratio, DWORD delayTick, BOOL isRegen, DWORD range);
	void AddRegenObject(EObjectKind, WORD monsterKind, const VECTOR3&, LPCTSTR finiteStateMachine);
	void ForceRegen();
	void RegenProcess();
	void SetGroupID(DWORD index) { m_dwGroupID = index; }
	DWORD GetGroupID() const { return m_dwGroupID; }
	void SetGridID(DWORD index) { m_dwGridID = index; }
	DWORD GetGridID() const { return m_dwGridID; }
	Parameter* GetRegenObject(DWORD regenIndex);
	DWORD GetRegenRange() const { return m_dwRegenRange; }
	void SetRegenDelayTime(DWORD delayTime);

private:
	BOOL Create(DWORD regenObjectIndex);

private:
	// 100104 LUJ, ������ ��� ��ȣ�� ������ �ִ�
	typedef std::set< DWORD > IndexContainer;
	IndexContainer mRegenIndexContainer;
	// 100104 LUJ, ���� ����ü
	struct Condition
	{
		DWORD mTargetGroup;
		float mRatio;
		DWORD mStartTick;
		DWORD mDelayTick;
		BOOL mIsRegen;
	};
	// 100104 LUJ, ���� ���� ������ ������ �ִ�
	typedef std::list< Condition > ConditionContainer;
	ConditionContainer mConditionContainer;
	// 100104 LUJ, Ű: ���� ���� ��ȣ, ��: ���� ��ũ��Ʈ �ε���
	typedef std::map< DWORD, DWORD > ScriptIndexContainer;
	ScriptIndexContainer mScriptIndexContainer;
	// 100104 LUJ, ���� �Ķ���͸� �����ϴ� �����̳�
	typedef stdext::hash_map< DWORD, Parameter > ParameterContainer;
	ParameterContainer mParameterContainer;
	DWORD m_dwGroupID;
	DWORD m_dwGridID;
	DWORD m_dwRegenRange;
};