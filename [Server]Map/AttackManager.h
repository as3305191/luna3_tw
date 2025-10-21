#pragma once

class CObject;

#define  ATTACKMGR CAttackManager::GetInstance()
class CAttackManager  
{
public:
	GETINSTANCE(CAttackManager);
	CAttackManager();
	virtual ~CAttackManager();
	void RecoverLife(CObject* pOperator,CObject* pObject, float RecoverLifeVal,RESULTINFO* pResultInfo);
	void RecoverMana(CObject* pOperator,CObject* pObject,float RecoverManaVal,RESULTINFO* pResultInfo);
	void sendDieMsg(CObject * pAttacker, CObject* pTarget);

	typedef void (CAttackManager::*AttackFunction)(CObject*, CObject*, RESULTINFO*, float, float, WORD);
	void PhysicAttack( CObject* pAttacker, CObject* pTarget, RESULTINFO* pDamageInfo, float Accuracy = 0, float AddDamage = 0, WORD AddType = 0 );
	void MagicAttack( CObject* pAttacker, CObject* pTarget, RESULTINFO* pDamageInfo, float Accuracy = 0, float AddDamage = 0, WORD AddType = 0 );
	void SpecialAttack( CObject* pAttacker, CObject* pTarget, RESULTINFO* pDamageInfo, float Damage = 0, WORD Type = 0, BOOL bFixed = TRUE );
	// 090204 LUJ, �?� �� MP�� �???�
	void BurnMana( CObject* pAttacker, CObject* pTarget, const BUFF_SKILL_INFO* );
	void SetPvpDamageRatio(BYTE ratio)	{	m_PvpDamageRatio = ratio; }
	float GetPvpDamageRatio()	{ return m_PvpDamageRatio; }
	float GetGTDamageRatio()	{ return m_GTDamageRatio; }

    float GetPvpPhysicDamageRatio() { return m_PvpPhysicDamageRatio; }
	float GetPvpMagicDamageRatio()	{ return m_PvpMagicDamageRatio; }
    
private:
	// 080115 KTH -- PVP Damage Ratio
	float	m_PvpDamageRatio;
	float	m_GTDamageRatio; 
    float	m_PvpPhysicDamageRatio;
	float	m_PvpMagicDamageRatio;
};