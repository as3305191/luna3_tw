#pragma once
#include "skillunit.h"

class cAttackSkillUnit :
	public cSkillUnit
{
public:
	cAttackSkillUnit(cActiveSkillObject&);
	virtual ~cAttackSkillUnit();
	virtual BOOL Excute();
	// 100219 ShinJS --- Damage (Life/Mana) �� �����Ѵ�.
	virtual BOOL ApplyDamageResult( CObject* pAttacker, CObject* pTarget, RESULTINFO& damageInfo );
};
