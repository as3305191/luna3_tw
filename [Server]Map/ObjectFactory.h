// ObjectFactory.h: interface for the CObjectFactory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBJECTFACTORY_H__324EFDDF_059A_428C_94F2_76BC8E75E765__INCLUDED_)
#define AFX_OBJECTFACTORY_H__324EFDDF_059A_428C_94F2_76BC8E75E765__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Object.h"
//class CObject;
class CPlayer;
class Pet;
class CMonster;
class CNpc;
class CBossMonster;
class CMapObject;
// 080616 LUJ, ���� �߰�
class CTrap;
// 090316 LUJ, Ż�� �߰�
class CVehicle;

// �ʵ庸�� - 05.12 �̿���
class CFieldBossMonster;
class CFieldSubMonster;
class CPet;

class CObjectFactory
{
private:
	// 090923 ONS �޸�Ǯ ��ü
	CPool<CPlayer>* PlayerPool;
	CPool<CMonster>* MonsterPool;	
	CPool<CNpc>* NpcPool;	
	CPool<CBossMonster>* BossMonsterPool;
	CPool<CPet>* PetPool;
	
	// �ʵ庸�� - 05.12 �̿���
	CPool<CFieldBossMonster>* FieldBossMonsterPool;
	CPool<CFieldSubMonster>* FieldSubMonsterPool;
	
	CPool<CMapObject>*	MapObjectPool;
	// 080616 LUJ, ���� �߰�
	CPool<CTrap>* TrapPool;
	// 090316 LUJ, Ż�� �߰�
	CPool<CVehicle>* VehiclePool;

public:
	CObjectFactory();
	virtual ~CObjectFactory();
	
	void Init();
	void Release();

	CObject* MakeNewObject(EObjectKind Kind,DWORD AgentNum, BASEOBJECT_INFO* pBaseObjectInfo);
	void ReleaseObject(CObject* pObject);
};
extern CObjectFactory * g_pObjectFactory; 
#endif // !defined(AFX_OBJECTFACTORY_H__324EFDDF_059A_428C_94F2_76BC8E75E765__INCLUDED_)
