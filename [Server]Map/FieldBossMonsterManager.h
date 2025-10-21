#ifndef __FIELDBOSSMONSTERMANAGER_H__
#define __FIELDBOSSMONSTERMANAGER_H__

#pragma once

#define MAX_FIELDBOSS_DROPITEM_GROUP_NUM 5
#define MAX_FIELDBOSS_DROPITEM_NUM 10

class CFieldBossMonster;
class CFieldSubMonster;

typedef struct stFieldMonsterInfo
{
	// ���� ���� �� ����
	WORD	m_MonsterKind;
	WORD	m_ObjectKind;
	WORD	m_SubKind;
	WORD	m_SubCount;

	// �����ð�
	DWORD	m_dwRegenTimeMin;
	DWORD	m_dwRegenTimeMax;

	// �Ҹ� ���� ����
	DWORD	m_dwDistructTime;
	DWORD	m_dwCountTime;
	DWORD	m_dwLastCheckTime;

	// ȸ�� ���� ����
	DWORD	m_dwPieceTime;
	DWORD	m_dwRecoverStartTime;
	DWORD	m_dwRecoverDelayTime;
	float	m_fLifeRate;

	// �ʱ�ȭ
	stFieldMonsterInfo()
	{
		m_MonsterKind = 0;
		m_ObjectKind = 0;
		m_SubKind = 0;
		m_SubCount = 0;
		
		m_dwDistructTime = 0;
		m_dwCountTime = 0;
		m_dwLastCheckTime = 0;

		m_dwRegenTimeMin = 0;
		m_dwRegenTimeMax = 0;

		m_dwPieceTime = 0;
		m_dwRecoverStartTime = 0;
		m_dwRecoverDelayTime = 0;
		m_fLifeRate = 0.0f;
	}

} FIELDMONSTERINFO;

typedef struct stFieldGroup
{
	WORD	m_GroupID;
	DWORD	m_GridID;
	DWORD	m_BossObjectID;

	CFieldBossMonster*	m_pBoss;
	cPtrList			m_SubList;

	DWORD	m_dwRegenTimeMin;
	DWORD	m_dwRegenTimeMax;

	stFieldGroup()
	{
		m_GroupID = 0;
		m_GridID = 0;

		m_pBoss = NULL;

		m_dwRegenTimeMin = 0;
		m_dwRegenTimeMax = 0;
	}
} FIELDGROUP;

// �ʵ� ���� ��� ������ ����
typedef struct stFieldBossDropItem
{
	DWORD	m_wItemIndex[MAX_FIELDBOSS_DROPITEM_NUM];
	WORD	m_wItemCount[MAX_FIELDBOSS_DROPITEM_NUM];
	
	WORD	m_wCount;
	WORD	m_wDropRate;
	WORD	m_wDamageRate;

	stFieldBossDropItem()
	{
		ZeroMemory(
			this,
			sizeof(*this));
	}
} FIELDBOSSDROPITEM;

// �ʵ� ���� ��� ������ ����Ʈ
typedef struct stFieldBossDropItemList
{
	WORD	m_wMonsterKind;

	FIELDBOSSDROPITEM m_ItemList[MAX_FIELDBOSS_DROPITEM_GROUP_NUM];

	stFieldBossDropItemList()
	{
		m_wMonsterKind = 0;
	}
} FIELDBOSSDROPITEMLIST;

#define FIELDBOSSMONMGR USINGTON(CFieldBossMonsterManager)
#define MAX_REGENPOS_NUM 10

class CFieldBossMonsterManager
{
	CYHHashTable<FIELDGROUP> m_FieldGroupTable;
	CYHHashTable<FIELDMONSTERINFO> m_FieldMonsterInfoTable;
	CYHHashTable<FIELDBOSSDROPITEMLIST> m_FieldBossDropItemTable;

	VECTOR3 m_RegenPos[ MAX_REGENPOS_NUM ];
	WORD m_RegnePosCount;

public:
	CFieldBossMonsterManager(void);
	virtual ~CFieldBossMonsterManager(void);

	void Init();	// �ʵ� ���� ���� ������ �ε�

	void AddRegenPosition(float x, float z);	// ���� ��ġ�� ����Ʈ�� �߰�
	VECTOR3 GetRegenPosition(int index = -1);	// ���� ��ġ�� �����´� -1�� ��� ����
	WORD GetRegenPositionCount() { return m_RegnePosCount; }
	
	void BossDead(CFieldBossMonster* pBoss);	// ���� �׾���
	void SubDead(CFieldSubMonster* pSub);		// ���� �׾���

	void AddFieldBossMonster( CFieldBossMonster* pBoss );	// ������ �ʵ庸���߰�
	
	void Process();

	FIELDBOSSDROPITEMLIST* GetFieldBossDropItemList( WORD MonsterKind );

	BOOL IsRemainGroup(DWORD dwBossIndex);
};

EXTERNGLOBALTON(CFieldBossMonsterManager);

#endif