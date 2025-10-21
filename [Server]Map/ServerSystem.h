// ServerSystem.h: interface for the CServerSystem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVERSYSTEM_H__FD3EBFC3_EE3D_4505_A5A1_24DA471D20AB__INCLUDED_)
#define AFX_SERVERSYSTEM_H__FD3EBFC3_EE3D_4505_A5A1_24DA471D20AB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "MHMap.h"
//#include "PtrList.h"
class CObject;
class CGridSystem;

class CPlayer;
class CMonster;
class CNpc;
class cSkillObject;
class CBossMonster;
class CMapObject;
class CPet;
// �ʵ庸�� - 05.12 �̿���
class CFieldBossMonster;
class CFieldSubMonster;

enum eNATION
{
	eNATION_KOREA,
	eNATION_CHINA,
};

enum{ePET_FROM_DB, ePET_FROM_ITEM};		//DB�κ��� �����, ��ȯ������ ù������� �ֻ���
enum EObjectKind;

class CServerSystem  
{
//	CYHHashTable<CItemObject> m_ItemObjectTable;
	CGridSystem* m_pGridSystem;
	CMHMap m_Map;
	
	friend class CCharMove;
	BOOL m_start;
	WORD 	m_wMapNum;
	eNATION	m_Nation;
	BOOL 	m_bTestServer;
	DWORD	m_dwQuestTime;
	BOOL	m_bQuestTime;

	BOOL	m_bCompletionChrUpdate;
//--check process time
	DWORD	m_dwMainProcessTime;

	// ���μ���Ÿ�� üũ��
	BOOL			m_bCheckProcessTime;
	LARGE_INTEGER	m_freq;
	LARGE_INTEGER	m_ObjLoopProc[2];
	LARGE_INTEGER	m_TriggerProc[2];
	LARGE_INTEGER	m_DungeonProc[2];
	double			m_fTimeObjLoopProc;
	double			m_fTimeTriggerProc;
	double			m_fTimeDungeonProc;
	double			m_fAvrObjLoopProc;
	double			m_fAvrTriggerProc;
	double			m_fAvrDungeonProc;
	DWORD			m_dwProcessCount;

public:
	void ToggleCheckProcessTime();
	DWORD GetMainProcessTime() { return m_dwMainProcessTime; }

public:

	CServerSystem();
	virtual ~CServerSystem();
	void SetNation();
	eNATION GetNation() { return m_Nation; }

	BOOL IsTestServer() { return m_bTestServer; }

	void _5minProcess();
	void Process();
	void Start(WORD ServerNum);
	void End();
	CMHMap* GetMap() { return &m_Map; }
	WORD GetMapNum() const { return m_wMapNum; }
	BOOL GetStart()	const { return m_start; }
	void SetStart(BOOL);
	CPlayer* AddPlayer(DWORD dwPlayerID, DWORD dwAgentNum,DWORD UniqueIDinAgent,int ChannelNum, eUSERLEVEL);
	CPlayer* InitPlayerInfo(BASEOBJECT_INFO*, CHARACTER_TOTALINFO*, HERO_TOTALINFO*);
	void CreateNewPet(CPlayer* pMaster, DWORD dwItemIdx, DWORD SummonItemDBIdx, WORD wPetKind, WORD PetGrade = 1);
	CMonster* AddMonster(DWORD dwSubID, BASEOBJECT_INFO* pBaseObjectInfo,MONSTER_TOTALINFO* pTotalInfo,VECTOR3* pPos,WORD wObjectKind = 32);
	CBossMonster* AddBossMonster(DWORD dwSubID, BASEOBJECT_INFO* pBaseObjectInfo,MONSTER_TOTALINFO* pTotalInfo,VECTOR3* pPos);
	
	// �ʵ庸�� - 05.12 �̿���
	CFieldBossMonster* AddFieldBossMonster(DWORD dwSubID, BASEOBJECT_INFO* pBaseObjectInfo,MONSTER_TOTALINFO* pTotalInfo,VECTOR3* pPos);
	CFieldSubMonster* AddFieldSubMonster(DWORD dwSubID, BASEOBJECT_INFO* pBaseObjectInfo,MONSTER_TOTALINFO* pTotalInfo,VECTOR3* pPos);
	
	CNpc* AddNpc(BASEOBJECT_INFO* pBaseObjectInfo,NPC_TOTALINFO* pTotalInfo,VECTOR3* pPos);
	cSkillObject* AddSkillObject(cSkillObject* pSkillObj,VECTOR3* pPos);
	CMapObject* AddMapObject(DWORD Kind, BASEOBJECT_INFO* pBaseObjectInfo, MAPOBJECT_INFO* pMOInfo, VECTOR3* pPos);
	CPet* AddPet(BASEOBJECT_INFO&, const PET_OBJECT_INFO&, CPlayer* pPlayer);
	void RemovePlayer(DWORD dwPlayerID, BOOL bRemoveFromUserTable = TRUE);
	// 091214 ONS �� ���̵��� ��ȯ/���� ���� �޼����� ������� �ʵ��� ó��.
	void RemovePet(DWORD dwPetObjectID, BOOL bSummoned = TRUE);
	void RemoveMonster(DWORD dwPlayerID);
	void RemoveBossMonster(DWORD dwMonster);
	void RemoveNpc(DWORD dwPlayerID);
	void RemoveItemObject(DWORD ItemID);
	void RemoveSkillObject(DWORD SkillObjectID);
	void RemoveMapObject( DWORD MapObjID );

	
	inline CGridSystem* GetGridSystem()	{	return m_pGridSystem;	}

	void SendToOne(CObject* pObject,void* pMsg,int MsgLen);
	
	void ReloadResourceData();
	// 091127 LUJ, ��ȯ ������ ������ ��ȯ�Ѵ�
	BOOL IsNoRecallMap(CObject&);
	void	SetCharUpdateCompletion(BOOL bVal)	{ m_bCompletionChrUpdate = bVal;	}
	BOOL	GetCharUpdateCompletion()	{	return m_bCompletionChrUpdate;	}
	void	RemoveServerForKind( WORD wSrvKind );
	void	HandlingBeforeServerEND();

	void	LoadHackCheck();
	// 091106 LUJ, ä�κ� ����
	void RemoveMonsterInGrid(DWORD gridIndex);
	DWORD GetMonsterCount(DWORD gridIndex) const;
	DWORD AddGameRoom(LPCTSTR address, LPCTSTR name, LPCTSTR upperIndex, LPCTSTR lowerIndex);
	void UpdateFiniteStateMachine(CMonster&, DWORD subIndex) const;

private:
	void AddMonsterCount(CObject&);
	void RemoveMonsterCount(CObject&);
	BOOL IsUncountable(EObjectKind) const;
	ULONGLONG GetGameRoomKey(LPCTSTR address) const;
	// 091106 LUJ, Ű: ���� ������Ʈ ��ȣ
	typedef stdext::hash_set< DWORD > MonsterSet;
	// 091106 LUJ, Ű: ä�� ��ȣ. ä�κ� ���� ������Ʈ ���� ����ȴ�
	typedef stdext::hash_map< DWORD, MonsterSet > ChannelMonsterMap;
	ChannelMonsterMap mChannelMonsterMap;
	// 091228 LUJ, PC�� ����
	struct GameRoom
	{
		DWORD mIndex;
		// 091228 LUJ, ������ȣ�� DB���� VARCHAR(20) �����̸�, �����ڰ� ���Ե� �� �ִ�
		TCHAR mDbIndex[21];
		TCHAR mName[MAX_NAME_LENGTH + 1];
		// 100216 ShinJS --- DB Load �ð� ����
		DWORD mDBLoadTime;
	};
	// 091228 LUJ, Ű: IPv4 Ȥ�� v6 ������ ���� �� ������ ��ġ������ �����Ѵ�(��: 192.168.1.130 -> 192168001130)
	//			���� ���û�� ���� ����, PC���� �ƴ� ��쿡�� �����Ѵ�. �̶��� mIndex�� 0�̴�.
	typedef stdext::hash_map< ULONGLONG, GameRoom > GameRoomContainer;
	GameRoomContainer mGameRoomContainer;
};

void __stdcall OnConnectServerSuccess(DWORD dwIndex, void* pVoid);
void __stdcall OnConnectServerFail(void* pVoid);

extern CServerSystem * g_pServerSystem;
BOOL LoadEventRate(char* strFileName);


#endif // !defined(AFX_SERVERSYSTEM_H__FD3EBFC3_EE3D_4505_A5A1_24DA471D20AB__INCLUDED_)
