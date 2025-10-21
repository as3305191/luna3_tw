// Object.cpp: implementation of the CObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Object.h"
#include "CharMove.h"
//#include "ServerSystem.h"
#include "ObjectStateManager.h"
#include "BattleSystem_Server.h"
#include "PackedData.h"
#include "Network.h"
#include "..\[CC]Header\CommonCalcFunc.h"
//for event - hs //
#include "Monster.h"
//for event - hs //
#include "Player.h"

#include "UserTable.h"
#include "../[cc]skill/server/object/skillobject.h"
#include "../[cc]skill/server/object/buffskillobject.h"
#include "../[cc]skill/server/info/buffskillinfo.h"
#include "../[cc]skill/server/Delay/Delay.h"
// 080616 LUJ, ���� �� �̺�Ʈ ó���ϱ����� ����
#include "../[cc]skill/Server/Manager/SkillManager.h"

// 080806 LYW --- Object : ��ȯ �Ŵ��� ��� ����.
#include "./SiegeRecallMgr.h"
#include "Trigger/Manager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CObject::CObject() :
mAlias(0),
mNextCheckedTick(0)
{
	m_bInited = FALSE;
	m_GridID = 0;
//	m_MapNum = 0;
//	m_tempMapNum = 0;
//	m_OnOffSkillList.Initialize(10);
	m_BuffSkillList.Initialize(10);
	m_dwShockedTime = 0;
}

CObject::~CObject()
{}

BOOL CObject::Init(EObjectKind kind,DWORD AgentNum, BASEOBJECT_INFO* pBaseObjectInfo)
{
	SetBattle(pBaseObjectInfo->BattleID,0);

	m_AgentNum = AgentNum;
	m_ObjectKind = kind;

	if(pBaseObjectInfo)
		memcpy(&m_BaseObjectInfo,pBaseObjectInfo,sizeof(m_BaseObjectInfo));
	else
		memset(&m_BaseObjectInfo,0,sizeof(BASEOBJECT_INFO));

	memset( &m_MoveInfo, 0, sizeof(MOVE_INFO) );
	
	OBJECTSTATEMGR_OBJ->InitObjectState(this);

	mEffectMoveCount = 0;
	CurCastingSkillID = 0;
	mCurrentSkillID = 0;

	mDieFlag = FALSE;

	m_dwShockedTime = 0;

	ZeroMemory(&m_YYLifeRecoverTime, sizeof(m_YYLifeRecoverTime));

	m_LifeRecoverDirectlyAmount = 0;

	mAlias = 0;
	mGroupAlias = 0;

	return TRUE;
}

void CObject::Release()
{
	//m_ObjectKind = 0;
	m_bInited = FALSE;
	m_bGridInited = FALSE;
	mEffectMoveCount = 0;

	EndAllSkill();
	RemoveSummonedAll();
}

void CObject::EndSkill()
{
	cPtrList templist;

	m_BuffSkillList.SetPositionHead();

	while(cBuffSkillObject* const pSObj = m_BuffSkillList.GetData())
	{
		if(0 == pSObj->GetInfo().Die)
		{
			pSObj->SetEndState();
			pSObj->EndState();
			templist.AddTail( pSObj );
		}
	}

	//m_BuffSkillList.RemoveAll();

	PTRLISTPOS pos = templist.GetHeadPosition();
	while( pos )
	{
		cSkillObject* const pSObj = ( cSkillObject* )templist.GetNext( pos );
		m_BuffSkillList.Remove( pSObj->GetSkillIdx() );
	}
	templist.RemoveAll();

	if(cSkillObject	* const pSObj = SKILLMGR->GetSkillObject( mCurrentSkillID ))
	{
		pSObj->SetEndState();
		pSObj->EndState();
	}
}

void CObject::EndAllSkill()
{
	m_BuffSkillList.SetPositionHead();

	while(cBuffSkillObject* const pSObj = m_BuffSkillList.GetData())
	{
		pSObj->EndState();
	}
	m_BuffSkillList.RemoveAll();
}

void CObject::GetBaseObjectInfo(BASEOBJECT_INFO* pRtInfo)
{
	memcpy(pRtInfo,&m_BaseObjectInfo,sizeof(BASEOBJECT_INFO));
}

void CObject::GetSendMoveInfo(SEND_MOVEINFO* pRtInfo,CAddableInfoList* pAddInfoList)
{
	pRtInfo->CurPos.Compress(CCharMove::GetPosition(this));
	pRtInfo->KyungGongIdx = m_MoveInfo.KyungGongIdx;
	pRtInfo->MoveMode = m_MoveInfo.MoveMode;
	pRtInfo->AddedMoveSpeed = m_MoveInfo.AddedMoveSpeed;
	/*�����Ƽ ���� - pRtInfo->AbilityKyungGongLevel = m_MoveInfo.AbilityKyungGongLevel;*/

	if(CCharMove::IsMoving(this))
	{
		if(pAddInfoList == NULL)
		{
			char temp[256];
			sprintf(temp,"Object %s,  Invalid Move State (Not Inited But Moving)");
			ASSERTMSG(0,temp);
			for(BYTE i=0;i<m_MoveInfo.GetMaxTargetPosIdx();++i)
			{
				VECTOR3* pPos = m_MoveInfo.GetTargetPosition(i);
				sprintf(temp,"tpos %d,   (%4.2f,  %4.2f)",i,pPos->x,pPos->z);
				ASSERTMSG(0,temp);
			}
			return;
		}

		COMPRESSEDTARGETPOS tpos;
		tpos.Init();
		for(BYTE i=0;i<m_MoveInfo.GetMaxTargetPosIdx();++i)
			tpos.AddTargetPos(m_MoveInfo.GetTargetPosition(i));
		pAddInfoList->AddInfo(CAddableInfoList::MoveInfo,(WORD)tpos.GetSize(),&tpos);
	}
}

// 091026 LUJ, ���� ũ�⿡ �����ϰ� ������ �� �ֵ��� ����
DWORD CObject::SetRemoveMsg(DWORD dwReceiverID, MSGBASE*& sendMessage)
{
	static MSG_DWORD message;
	ZeroMemory(&message, sizeof(message));
	message.Category = MP_USERCONN;
	message.Protocol = MP_USERCONN_OBJECT_REMOVE;
	message.dwObjectID = dwReceiverID;
	message.dwData = GetID();
	
	sendMessage = &message;
	return sizeof(message);
}

void CObject::GetPosition(VECTOR3* pos)
{
	*pos = *CCharMove::GetPosition(this);
}

void CObject::SetState(EObjectState state)
{
	m_BaseObjectInfo.ObjectState = state;
	m_ObjectState.State_Start_Time = gCurTime;
	m_ObjectState.bEndState		= FALSE;

	// 091110 ONS �� �ִϸ��̼� �߰� : ���� �׾��� ���
	if(eObjectState_Die == state)
	{
		mDieFlag = TRUE;
	}
}

int CObject::GetObjectTileSize()
{
	return 0;
}

void CObject::CalcRealDamage(CObject* pOperator,WORD PhyDamage,WORD AttrDamage,RESULTINFO* pDamageInfo)
{
	pDamageInfo->RealDamage = PhyDamage + AttrDamage;
}

// 06. 03 �������� - �̿���
void CObject::Die(CObject* pAttacker)
{
	// ���� ���� �ִ� Battle���� ó���غ��� ó������ ���ߴٸ�(return FALSE),
	// �� Ŭ�����鿡�� ó���� �ñ��.
	EndSkill();

	const DWORD AttackerID = (pAttacker ? pAttacker->GetID() : 0);

	if(FALSE == GetBattle()->OnTeamMemberDie(GetBattleTeam(),GetID(),AttackerID))
	{
		DoDie(pAttacker);
	}

	const EObjectKind attackerObjectKind = EObjectKind(pAttacker ? pAttacker->GetObjectKind() : eObjectKind_None) ;

	{
		// 091116 LUJ, ä�ο� �ش��ϴ� �޽����� �Ҵ�޵��� �Ѵ�
		Trigger::CMessage* const message = TRIGGERMGR->AllocateMessage(GetGridID());
		message->AddValue(Trigger::eProperty_ObjectIndex, GetID());
		message->AddValue(Trigger::eProperty_ObjectKind, GetObjectKind());
		message->AddValue(Trigger::eProperty_AttackerObjectIndex, AttackerID);
		message->AddValue(Trigger::eProperty_AttackerObjectKind, attackerObjectKind);
		message->AddValue(Trigger::eProperty_Event, Trigger::eEvent_IsKilled);
	}

	{
		// 091116 LUJ, ä�ο� �ش��ϴ� �޽����� �Ҵ�޵��� �Ѵ�
		Trigger::CMessage* const message = TRIGGERMGR->AllocateMessage(GetGridID());
		message->AddValue(Trigger::eProperty_ObjectIndex, AttackerID);
		message->AddValue(Trigger::eProperty_ObjectKind, attackerObjectKind);
		message->AddValue(Trigger::eProperty_VictimObjectKind, GetObjectKind());
		message->AddValue(Trigger::eProperty_Event, Trigger::eEvent_Kill);

		if(eObjectKind_Monster & GetObjectKind())
		{
			CMonster* const monster = (CMonster*)this;
			message->AddValue(Trigger::eProperty_VictimMonsterKind, monster->GetMonsterKind());
			message->AddValue(Trigger::eProperty_VictimMonsterAlias, monster->GetAlias());
			message->AddValue(Trigger::eProperty_VictimMonsterGroupAlias, monster->GetGroupAlias());
		}
	}
}

//����: CPlayer::Damage�� �������. ���������� �ִٸ� CPlayer::Damage�� �����ϵ��� �Ͻÿ�. KES
DWORD CObject::Damage(CObject* pAttacker,RESULTINFO* pDamageInfo)
{
	DWORD life = GetLife();
	DWORD beforelife = life;

	if(life > pDamageInfo->RealDamage)
		life -= pDamageInfo->RealDamage;
	else
		life = 0;
	
	SetLife(life,FALSE);

	DoDamage(pAttacker,pDamageInfo,beforelife);

	return life;
}

// 100223 ShinJS --- ���� ������ �߰�
DWORD CObject::ManaDamage(CObject* pAttacker,RESULTINFO* pDamageInfo)
{
	DWORD mana = GetMana();
	DWORD beforeMana = mana;

	mana = ( mana > pDamageInfo->ManaDamage ? mana - pDamageInfo->ManaDamage : 0 );
	SetMana( mana, FALSE );

	DoManaDamage( pAttacker, pDamageInfo, beforeMana );

	return mana;
}

void CObject::AddLife(DWORD val,DWORD* realAddLife,BOOL bSendMsg)
{	
	DWORD life = GetLife();
	DWORD maxlife = GetMaxLife();
	
	if(life+val > maxlife)
	{
		val = maxlife - life;
	}
	SetLife(life+val,bSendMsg);

	if(realAddLife)
		*realAddLife = val;
}

// 080710 LUJ, ��ų ��� �� HP�� �Ҹ��� �� �ֵ��� �߰�
void CObject::ReduceLife( DWORD value )
{
	SetLife( GetLife() < value ? 0 : GetLife() - value );
}

// RaMa 04.11.04
void CObject::AddMana(DWORD val,DWORD* realAddMana)
{
	DWORD mana = GetMana();
	DWORD maxmana = GetMaxMana();
	
	if(mana+val > maxmana)
	{
		val = maxmana - mana;
	}
	SetMana(mana+val);

	if(realAddMana)
		*realAddMana = val;
}

void CObject::ReduceMana(DWORD val)
{
	if(GetMana() < val)
		SetMana(0);
	else
		SetMana(GetMana()-val);
}

void CObject::CheckLifeValid()
{
	DWORD MaxLife = GetMaxLife();
	if(GetLife() > MaxLife)
		SetLife(MaxLife);
}
void CObject::CheckManaValid()
{
	DWORD MaxMana = GetMaxMana();
	if(GetMana() > MaxMana)
		SetMana(MaxMana);
}

void CObject::SetBattle( DWORD BattleID, BYTE Team, DWORD Param )
{
	CBattle* const battle = BATTLESYSTEM->GetBattle(BattleID);
	m_BaseObjectInfo.BattleID = BattleID;
	m_BaseObjectInfo.BattleTeam = Team;

	MSG_DWORD4 msg;
	ZeroMemory(&msg, sizeof(msg));
	msg.Category	= MP_BATTLE;
	msg.Protocol	= MP_BATTLE_CHANGE_OBJECTBATTLE;
	msg.dwObjectID	= GetID();
	msg.dwData1		= BattleID;
	msg.dwData2		= Team;
	msg.dwData3		= battle->GetBattleKind();
	msg.dwData4		= Param;

	PACKEDDATA_OBJ->QuickSend( this, &msg, sizeof(msg) );
}

CBattle* CObject::GetBattle()
{
	return BATTLESYSTEM->GetBattle(m_BaseObjectInfo.BattleID);
}

void CObject::SendMsg(MSGBASE* pMsg,int MsgLen)
{
	pMsg->dwObjectID = GetID();
	g_Network.Send2Server(GetAgentNum(),(char*)pMsg,MsgLen);
}

void CObject::EndBuffSkillByStatus( WORD Status )
{
	m_BuffSkillList.SetPositionHead();

	while(cBuffSkillObject* const pSObj = m_BuffSkillList.GetData())
	{
		if(pSObj->GetInfo().Status == Status)
		{
			pSObj->SetEndState();
			pSObj->EndState();
		}
	}
}

DWORD CObject::GetLastCastSkill(DWORD skillIdx)
{
	auto it = mSkillCooldown.find(skillIdx); 
	if (it != mSkillCooldown.end())
	{
		return it->second;
	}
	else
	{
		return 0;
	}
}

void CObject::UpdateLastCastSkill(DWORD skillIdx, DWORD cTime)
{
	mSkillCooldown[skillIdx] = cTime;
}

// 080616 LUJ, �̺�Ʈ ó��
void CObject::Execute( const CEvent& event )
{
	EventSkillObjectIndexList::iterator removedIterator = mEventSkillObjectIndexList.end();

	for(EventSkillObjectIndexList::iterator it = mEventSkillObjectIndexList.begin();
		mEventSkillObjectIndexList.end() != it;
		++it)
	{
		cSkillObject* skill = SKILLMGR->GetSkillObject( it->first );

		if( ! skill )
		{
			removedIterator = it;
			continue;
		}

		skill->Execute( event );
	}
    
	if( removedIterator != mEventSkillObjectIndexList.end() )
	{
		const eStatusKind buffStatus = removedIterator->second;
		--m_pEventSkillKindCountMap[ buffStatus ];
		
		// 080616 LUJ, 이벤트 발생 시 무효 하나씩 제거 스킬 오브젝트 인덱스를 제거하여 정리를 한다.
		mEventSkillObjectIndexList.erase( removedIterator );
	}
}

void CObject::AddEventSkill(cBuffSkillObject& buffSkillObject)
{
	if( SKILLKIND_BUFF != buffSkillObject.GetSkillKind() )
	{
		return;
	}

	switch(buffSkillObject.GetInfo().Status)
	{
	case eStatusKind_Reflect:
	case eStatusKind_Absorb:
	case eStatusKind_DamageToLife:
	case eStatusKind_DamageToMana:
	case eStatusKind_DamageToManaDamage:
	case eStatusKind_ConvertHealthWithGivedDamage:
	case eStatusKind_ConvertManaWithGivedDamage:
	case eStatusKind_EventSkillToKiller:
	case eStatusKind_EventSkillToVictim:
		{
			mEventSkillObjectIndexList.push_back(
				std::make_pair(buffSkillObject.GetID(), buffSkillObject.GetInfo().Status));

			++m_pEventSkillKindCountMap[buffSkillObject.GetInfo().Status];
			break;
		}
	}
}

BOOL CObject::HasEventSkill( eStatusKind status )
{
	const EventSkillKindCountMap::const_iterator iterator = m_pEventSkillKindCountMap.find(
		status);

	if(m_pEventSkillKindCountMap.end() == iterator)
	{
		return FALSE;
	}

	return iterator->second > 0;
}

// 090204 LUJ, Ÿ���� ��Ȯ�� �ϰ�, ���� ȸ���� ������
void CObject::RemoveBuffCount( eBuffSkillCountType kind, int count )
{
	m_BuffSkillList.SetPositionHead();

	while(cBuffSkillObject* const skillObject = m_BuffSkillList.GetData())
	{
		skillObject->RemoveBuffCount( kind, count );
	}
}

BOOL CObject::IsEffectMove()
{
	if(0 == mEffectMoveCount)
	{
		return FALSE;
	}

	--mEffectMoveCount;
	return TRUE;
}

void CObject::AddSummoned(const ACTIVE_SKILL_INFO& skillInfo, DWORD objectIndex)
{
	const DWORD skillIndex = skillInfo.Index / 100 * 100 + 1;

	mSummonObjectContainer[skillIndex].insert(
		objectIndex);
}

void CObject::RemoveSummoned(const ACTIVE_SKILL_INFO& skillInfo)
{
	const DWORD skillIndex = skillInfo.Index / 100 * 100 + 1;
	const SummonObjectContainer::const_iterator containerIterator = mSummonObjectContainer.find(
		skillIndex);

	if(mSummonObjectContainer.end() == containerIterator)
	{
		return;
	}

	const ObjectIndexContainer& objectIndexContainer = containerIterator->second;

	for(ObjectIndexContainer::const_iterator indexIterator = objectIndexContainer.begin();
		objectIndexContainer.end() != indexIterator;
		++indexIterator)
	{
		CMonster* const monster = (CMonster*)g_pUserTable->FindUser(
			*indexIterator);

		if(0 == monster ||
			FALSE == (eObjectKind_Monster & monster->GetObjectKind()))
		{
			continue;
		}

		monster->SetDieTime(
			1);
	}

	mSummonObjectContainer.erase(
		skillIndex);
}

void CObject::RemoveSummonedOldest(const ACTIVE_SKILL_INFO& skillInfo)
{
	const DWORD skillIndex = skillInfo.Index / 100 * 100 + 1;
	const SummonObjectContainer::const_iterator containerIterator = mSummonObjectContainer.find(
		skillIndex);

	if(mSummonObjectContainer.end() == containerIterator)
	{
		return;
	}

	const ObjectIndexContainer& objectIndexContainer = containerIterator->second;

	for(ObjectIndexContainer::const_iterator indexIterator = objectIndexContainer.begin();
		objectIndexContainer.end() != indexIterator;
		++indexIterator)
	{
		CMonster* const monster = (CMonster*)g_pUserTable->FindUser(
			*indexIterator);

		if(0 == monster ||
			FALSE == (eObjectKind_Monster & monster->GetObjectKind()))
		{
			continue;
		}

		monster->SetDieTime(
			1);
		// �Ѱ��� ����� �����Ѵ�
		break;
	}
}

void CObject::RemoveSummonedAll()
{
	while(false == mSummonObjectContainer.empty())
	{
		const SummonObjectContainer::iterator containerIterator = mSummonObjectContainer.begin();
		const ObjectIndexContainer& objectIndexContainer = containerIterator->second;

		for(ObjectIndexContainer::const_iterator indexIterator = objectIndexContainer.begin();
			objectIndexContainer.end() != indexIterator;
			++indexIterator)
		{
			CMonster* const monster = (CMonster*)g_pUserTable->FindUser(
				*indexIterator);

			if(0 == monster ||
				FALSE == (eObjectKind_Monster & monster->GetObjectKind()))
			{
				continue;
			}

			monster->SetDieTime(
				1);
		}

		mSummonObjectContainer.erase(
			containerIterator);
	}
}

int CObject::GetSummondSize(const ACTIVE_SKILL_INFO& skillInfo)
{
	const DWORD skillIndex = skillInfo.Index / 100 * 100 + 1;
	SummonObjectContainer::iterator containerIterator = mSummonObjectContainer.find(
		skillIndex);

	if(mSummonObjectContainer.end() == containerIterator)
	{
		return 0;
	}

	ObjectIndexContainer& objectIndexContainer = containerIterator->second;
	ObjectIndexContainer updatedObjectIndexContainer;

	for(ObjectIndexContainer::const_iterator iterator = objectIndexContainer.begin();
		objectIndexContainer.end() != iterator;
		++iterator)
	{
		const ObjectIndex objectIndex = *iterator;

		if(0 == g_pUserTable->FindUser(objectIndex))
		{
			continue;
		}

		updatedObjectIndexContainer.insert(
			objectIndex);
	}

	mSummonObjectContainer[skillIndex] = updatedObjectIndexContainer;

	return updatedObjectIndexContainer.size();
}

void CObject::SetShockTime( DWORD dwTime )
{
	m_dwShockedTime = gCurTime + dwTime;
}

BOOL CObject::IsShocked()
{
	if( gCurTime < m_dwShockedTime )
	{
		return TRUE;
	}

	m_dwShockedTime = 0;
	return FALSE;
}

// 100624 ONS HP������Ʈ���� ó�� �߰�
void CObject::AddLifeRecoverTime( const YYRECOVER_TIME& recoverTime )
{
	m_YYLifeRecoverTime = recoverTime;
}

void CObject::UpdateLife()
{
	// 100729 ONS �������� HP�� ��ȯ�Ұ�� ��� �����Ѵ�.
	if( 0 != m_LifeRecoverDirectlyAmount )
	{
		AddLife( m_LifeRecoverDirectlyAmount, NULL );
		m_LifeRecoverDirectlyAmount = 0;
		return;
	}

	if( m_YYLifeRecoverTime.count )
	{
		if( m_YYLifeRecoverTime.lastCheckTime < gCurTime )
		{
			m_YYLifeRecoverTime.lastCheckTime = gCurTime + m_YYLifeRecoverTime.recoverDelayTime;
			AddLife( m_YYLifeRecoverTime.recoverUnitAmout, NULL );
			--m_YYLifeRecoverTime.count;
		}
	}
}