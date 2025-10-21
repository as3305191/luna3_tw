#include "StdAfx.h"
#include "SkillObject.h"
#include "Battle.h"
#include "UserTable.h"

// 090205 LUJ, ���� ��ų ��� ������ �⺻��
float cSkillObject::mAllowRangeForWideSkill = 300.0f;

cSkillObject::cSkillObject(const SKILL_INFO& info) :
mSkillIndex(info.Index),
mSkillLevel(info.Level),
mSkillKind(info.Kind)
{
	// 080602 LUJ, ������Ʈ Ÿ���� �ʱ�ȭ�Ѵ�
	mType = cSkillObject::TypeNone;
}

cSkillObject::~cSkillObject(void)
{}

CObject* cSkillObject::GetOperator()
{
	return g_pUserTable->FindUser( mSkillObjectInfo.operatorId );
//	return mSkillObjectInfo.pOperator;
}

void cSkillObject::Init( sSKILL_CREATE_INFO* pInfo )
{
	/// ��ų ���� ���� ����
	memcpy(	&mSkillObjectInfo, pInfo, sizeof( sSKILL_CREATE_INFO ) );

	/// �⺻ ���� ����
	m_BaseObjectInfo.dwObjectID = pInfo->skillObjectId;
	strcpy(m_BaseObjectInfo.ObjectName,"SkillObject");
	m_BaseObjectInfo.ObjectState = eObjectState_None;
	m_SkillDir = pInfo->skillDir;

	/// ����� ������ �߸��Ǿ��� ��� ����
	if( g_pUserTable->FindUser( pInfo->operatorId ) == NULL )
	{
		return;
	}
}

void cSkillObject::EndState()
{
	mState = SKILL_STATE_DESTROY;
	GetOperator()->CurCastingSkillID = 0;
}

DWORD cSkillObject::SetRemoveMsg(DWORD dwReceiverID, MSGBASE*& sendMessage)
{
	static MSG_DWORD message;
	ZeroMemory(&message, sizeof(message));
	message.Category = MP_SKILL;
	message.Protocol = MP_SKILL_SKILLOBJECT_REMOVE;
	message.dwObjectID = dwReceiverID;
	message.dwData = GetID();

	sendMessage = &message;
	return sizeof(message);
}