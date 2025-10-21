#pragma once

#include "AIGroupPrototype.h"

#define GROUPMGR	CAIGroupManager::GetInstance()
class CAIGroup;

class CAIGroupManager  
{
	// 100104 LUJ, first: �׷� ��ȣ, second: �׸��� ��ȣ
	typedef std::pair< DWORD, DWORD > GroupIndex;
	typedef std::map< GroupIndex, CAIGroup > GroupContainer;
	GroupContainer mGroupContainer;
	CAIGroupManager();
	virtual ~CAIGroupManager();

public:
	GETINSTANCE(CAIGroupManager)
	CAIGroup& AddGroup(DWORD groupIndex, DWORD gridIndex);
	CAIGroup* GetGroup(DWORD dwGroupID, DWORD dwGridID);
	void RegenProcess();
	// 091214 LUJ, ���� ������ ����
	const VECTOR3& GetDomain(DWORD groupIndex, DWORD gridIndex, DWORD regenIndex);
};