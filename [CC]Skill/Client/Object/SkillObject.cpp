#include "stdafx.h"
#include ".\skillobject.h"
#include "../Info/SkillInfo.h"
#include "../Info/ActiveSkillInfo.h"
#include "../Delay/TimeDelay.h"
#include "../Manager/SkillManager.h"

#include "ActionTarget.h"
#include "ObjectManager.h"
#include "ObjectStateManager.h"

#include "GameIn.h"
#include "BattleSystem_Client.h"
#include "PartyManager.h"

#include "OptionManager.h"
#include "CharacterDialog.h"
#include "ObjectActionManager.h"
#include "ObjectManager.h"

cSkillObject::cSkillObject(cSkillInfo* pSkillInfo) :
mpSkillInfo( pSkillInfo ),
mDelayTick(0)
{}

cSkillObject::~cSkillObject(void)
{
}

void cSkillObject::Init(const SKILLOBJECT_INFO& pInfo)
{
	m_bInited = TRUE;
	m_bDieFlag = FALSE;

	m_BaseObjectInfo.dwObjectID = pInfo.SkillObjectIdx;
	SafeStrCpy(m_BaseObjectInfo.ObjectName,mpSkillInfo->GetName(), MAX_NAME_LENGTH+1);
	m_BaseObjectInfo.ObjectState = eObjectState_None;

	mSkillObjectInfo.skillObjectId = pInfo.SkillObjectIdx;
	mSkillObjectInfo.level = pInfo.SkillLevel;
	mSkillObjectInfo.pSkillInfo = mpSkillInfo;
	mSkillObjectInfo.operatorId = pInfo.Operator;
	mSkillObjectInfo.mainTarget = pInfo.MainTarget;
	mSkillObjectInfo.pos = pInfo.Pos;
	mSkillObjectInfo.skillDir = pInfo.Direction;
	mSkillObjectInfo.isNew = pInfo.IsNew;

	cActiveSkillInfo* pSkillInfo = ( cActiveSkillInfo* )mpSkillInfo;

	mOperatorEffect.Init(
		pSkillInfo->GetSkillInfo()->OperatorEffect,
		GetOperator(),
		&(mSkillObjectInfo.mainTarget));
	mSkillEffect.Init(
		pSkillInfo->GetSkillInfo()->SkillEffect,
		GetOperator(),
		&(mSkillObjectInfo.mainTarget));
	mTargetEffect.Init(
		pSkillInfo->GetSkillInfo()->TargetEffect,
		GetOperator(),
		&(mSkillObjectInfo.mainTarget));

	DWORD index = pSkillInfo->GetIndex();
	BOOL bFatal = ( index / 100000 ) % 10;
	mOperatorEffect.StartEffect( bFatal, pInfo.Rate );

	if(mSkillObjectInfo.operatorId == gHeroID)
	{
		if(0 < pSkillInfo->GetSkillInfo()->DelayType)
		{
			const DWORD time = pSkillInfo->GetSkillInfo()->CastingTime + pSkillInfo->GetSkillInfo()->DelayTime;
			mDelayTick = time + gCurTime;
		}
	}

	SetOperatorState();
}

void cSkillObject::OnReceiveSkillResult( MSG_SKILL_RESULT* pmsg )
{
	if( pmsg->SkillDamageKind )
	{
		CTargetListIterator iter( &pmsg->TargetList );
		CObject* pObject;
		RESULTINFO rinfo;

		while(iter.GetNextTarget())
		{
			pObject = OBJECTMGR->GetObject(iter.GetTargetID());
			iter.GetTargetData(&rinfo);
			if(pObject == NULL)
				continue;

			OBJECTACTIONMGR->ApplyResult(
				pObject,
				GetOperator(),
				&rinfo,eDamageKind_Front);
		}

		iter.Release();
	}
	else
	{
		mOperatorEffect.SetTargetArray(
			GetOperator(),
			&(pmsg->TargetList),
			&(mSkillObjectInfo.mainTarget));
	}
}

DWORD cSkillObject::GetSkillIdx() 
{
	return mpSkillInfo->GetIndex();
}

CObject* cSkillObject::GetOperator() 
{ 
	return OBJECTMGR->GetObject(mSkillObjectInfo.operatorId);
}

cSkillInfo* cSkillObject::GetSkillInfo() 
{ 
	return mpSkillInfo;	
}

void cSkillObject::SetOperatorState()
{
	CObject* pObject = OBJECTMGR->GetObject(mSkillObjectInfo.operatorId);

	if(0 == pObject)
		return;
	
	if(pObject->GetState() == eObjectState_Die)
		return;

	float AttackSpeedRate = 1.0f;

	if( mpSkillInfo->GetKind() == SKILLKIND_PHYSIC ||
		mpSkillInfo->GetKind() == SKILLKIND_MAGIC )
	{
		DWORD idx = mpSkillInfo->GetIndex();

		cActiveSkillInfo* pSkillInfo = ( cActiveSkillInfo* )mpSkillInfo;

		if( (idx / 100000) % 10 )
		{
			if( SKILLMGR->IsUsedPhysicAttackSpeed( *pSkillInfo->GetSkillInfo() ) )
			{
				AttackSpeedRate += ( ( HERO->GetRatePassiveStatus()->PhysicSkillSpeedRate + HERO->GetRateBuffStatus()->PhysicSkillSpeedRate ) * 0.01f );
			}
			else
			{
				float SpeedRate = min( HERO->GetRateBuffStatus()->MagicSkillSpeedRate + HERO->GetRatePassiveStatus()->MagicSkillSpeedRate, 99.f );
				AttackSpeedRate = ( 1 / ( 1 - ( SpeedRate*0.01f ) ) );
				if( (pSkillInfo->GetSkillInfo()->CastingTime / AttackSpeedRate) < MIN_MAGIC_CASTING_TIME )
					AttackSpeedRate = pSkillInfo->GetSkillInfo()->CastingTime / MIN_MAGIC_CASTING_TIME;
			}
		}
		else
		{
			AttackSpeedRate += ( ( HERO->GetRatePassiveStatus()->NormalSpeedRate + HERO->GetRateBuffStatus()->NormalSpeedRate ) * 0.01f );
		}

		if( AttackSpeedRate <= 0 )
		{
			AttackSpeedRate = 1.0f;
		}
	}
	
	const DWORD dwAnimationTime = ((cActiveSkillInfo*)mpSkillInfo)->GetSkillInfo()->AnimationTime;
	const DWORD dwCastingTime = ((cActiveSkillInfo*)mpSkillInfo)->GetSkillInfo()->CastingTime;
	DWORD dwStateEndTime = (DWORD)(dwAnimationTime / AttackSpeedRate);
	if( dwCastingTime > 0 )
	{
		const DWORD dwNoRateTime = (dwAnimationTime > dwCastingTime ? dwAnimationTime - dwCastingTime : 0);
		dwStateEndTime = (DWORD)(dwCastingTime / AttackSpeedRate) + dwNoRateTime;
	}
			
	OBJECTSTATEMGR->StartObjectState(
		pObject,
		eObjectState_SkillSyn);
	OBJECTSTATEMGR->EndObjectState(
		pObject,
		eObjectState_SkillSyn,
		dwStateEndTime);
}

void cSkillObject::ResetSkillObjectId( DWORD id )
{
	mSkillObjectInfo.skillObjectId = id;
	m_BaseObjectInfo.dwObjectID = id;
}

void cSkillObject::Update()
{
	if(gCurTime > mDelayTick)
	{
		UpdateTarget();

		mDelayTick = UINT_MAX;
	}
}

void cSkillObject::UpdateTarget()
{
	cActiveSkillInfo* pSkillInfo = ( cActiveSkillInfo* )mpSkillInfo;

	// 080804 LUJ, ��Ƽ�� ��� Ÿ�� ������Ʈ�� ���������� ���� ���Ѵ�. Ŭ���̾�Ʈ������
	//				���� ��ų�� ��� �ֱ������� Ÿ�� ���縦 �ؼ� ������ ������Ʈ�Ѵ�.
	//				�׷��� ��Ƽ ��ų�� �Ҽ� ���θ� �˻��ϰ�, ���۷����Ϳ� Ÿ���� ����
	//				��Ƽ�� �ִ��� �˻����� �ʴ´�. �׷��� Ÿ�� ������Ʈ�� �ϸ� Ÿ����
	//				�߸��Ǵ� ��찡 �߻��Ѵ�. ���� ��Ƽ�� ��� Ÿ�� �˻縦 �ƿ�
	//				���� �ʵ��� �Ѵ�. �׷��ٰ� Ÿ���� ���� ������ ������, ���� ������
	//				Ÿ���� �ʱ�ȭ�ȴ�. �ƿ� ó������ �ʾƾ� �Ѵ�
	if( !	pSkillInfo	||
			pSkillInfo->GetSkillInfo()->AreaTarget == eSkillAreaTargetParty )
	{
		return;
	}
	else if(TARGET_KIND_MULTI != pSkillInfo->GetSkillInfo()->Area)
	{
		return;
	}
	// 100111 LUJ, �ڽ��� ������ ��ų�� ó���ؾ��Ѵ�. �׷��� ������, ���Ͱ� �߻���Ų ���� ��ų ������� �ٲ��
	else if(mSkillObjectInfo.operatorId != gHeroID)
	{
		return;
	}

	CActionTarget Target;
	Target.InitActionTarget(
		&mSkillObjectInfo.pos,
		0);

	VECTOR3* const pTargetPos = Target.GetTargetPosition();
	Target.SetTargetObjectsInRange(
		pTargetPos == NULL ? &mSkillObjectInfo.pos : pTargetPos,
		pSkillInfo->GetSkillInfo()->AreaData,
		WORD(pSkillInfo->GetSkillInfo()->AreaTarget));

	MSG_SKILL_UPDATE_TARGET msg;
	ZeroMemory(
		&msg,
		sizeof(msg));
	msg.Category = MP_SKILL;
	msg.Protocol = MP_SKILL_UPDATE_TARGET_SYN;
	msg.dwObjectID = HEROID;
	msg.SkillObjectID = mSkillObjectInfo.skillObjectId;
	Target.GetTargetArray(&msg.TargetList);

	NETWORK->Send(&msg,msg.GetMsgLength());
}

void cSkillObject::Cancel()
{
	if( mpSkillInfo->GetType() == cSkillInfo::TypeActive )
	{
		((cActiveSkillInfo*)mpSkillInfo)->ResetCoolTime();
	}

	mOperatorEffect.EndEffect();
	
	OBJECTSTATEMGR->StartObjectState(
		GetOperator(),
		eObjectState_None);
}
