/*
080616 LUJ, ���� ������Ʈ. ��ų�� �ߵ��ȴ�. �ֱ������� �ڽ��� ������Ų ��ų�� �����ϴ��� üũ�ϰ�
			���� ��� �ڵ� �Ҹ�ȴ�
*/
#pragma once
#include "monster.h"

class CTrap : public CMonster
{
private:
	DWORD mTriggeredTime;

public:
	CTrap(void);
	virtual ~CTrap(void);
	virtual DWORD SetAddMsg(DWORD dwReceiverID, BOOL bLogin, MSGBASE*&);
	virtual BOOL DoAttack(DWORD attackNum);
	virtual void InitMonster(MONSTER_TOTALINFO*);
};
