#include "StdAfx.h"
#include "Trap.h"

CTrap::CTrap()
{}

CTrap::~CTrap()
{}

void CTrap::InitMonster(MONSTER_TOTALINFO* totalInfo)
{
	CMonster::InitMonster(
		totalInfo);

	mTriggeredTime = gCurTime + 2000 + rand() % 5000;
	SetDieTime(
		GetSMonsterList().Life * 1000 + gCurTime);
}

DWORD CTrap::SetAddMsg(DWORD dwReceiverID, BOOL bLogin, MSGBASE*& sendMessage)
{
	// ������ �ð��� ���� HP�� �پ��� �����̴�
	m_MonsterInfo.Life = (m_DieTime - gCurTime) / 1000;

	return CMonster::SetAddMsg(dwReceiverID, bLogin, sendMessage);
}

// 090204 LUJ, ���� �� �ڻ��ϱ� ���� ���ó����
BOOL CTrap::DoAttack( DWORD attackNum )
{
	if(mTriggeredTime > gCurTime)
	{
		return FALSE;
	}
	else if(FALSE == CMonster::DoAttack( attackNum ) )
	{
		return FALSE;
	}

	// 090204 LUJ, ���� ��ų�� �ణ�� ���� �� ����ȴ�. ���� ���� �ð���
	//			�ΰ� ������ �ڻ���Ѿ� �Ѵ�.
	SetDieTime(
		gCurTime + 500);
	return TRUE;
}