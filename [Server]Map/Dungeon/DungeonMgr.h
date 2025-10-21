#pragma once

#define DungeonMGR USINGTON(CDungeonMgr)
#define DUNGEON_ASSERTMSG(a,b)			{ if(0 == (a)) { WriteAssertMsg(__FILE__,__LINE__,b); } }

class CMHFile;
class CDungeonProcessorBase;

struct stDungeonReservation
{
	DWORD dwCharIndex;
	DWORD dwPartyIndex;
	DWORD dwChannelID;
	DWORD dwRegistTime;
	VECTOR3 vStartPos;

	void Clear()
	{
		memset(this, 0, sizeof(stDungeonReservation));
	}

	stDungeonReservation()
	{
		Clear();
	}
};

struct stMonsterList
{
	eDIFFICULTY difficulty;
	DWORD dwIndex;
	WORD dwPosX;
	WORD dwPosZ;
	char Alias[MAX_PATH];
	char GroupAlias[MAX_PATH];
	BOOL bActive;
	char FiniteStateMachine[MAX_PATH];
};

typedef CDungeonProcessorBase* (*DungeonProcessor)();
struct stDungeonProcessor 
{
	MAPTYPE				mapnum;
	DungeonProcessor	processor;
};

enum eAliasKind
{
	eAliasKind_None,
	eAliasKind_Alias,
	eAliasKind_GroupAlias
};

struct stDungeonNoPartyList
{
	DWORD	dwCharIndex;
	DWORD	dwOldPartyIndex;
	DWORD	dwOutTime;
};

class CDungeonMgr
{
private:
	CDungeonProcessorBase*						m_pDungeonProcessor;

	CMemoryPoolTempl<stDungeon>*				m_DungeonPool;
	CMemoryPoolTempl<stDungeonReservation>*		m_ReservationPool;
	CMemoryPoolTempl<stDungeonNoPartyList>*		m_NoPartyPool;
	
	CYHHashTable<stDungeon>						m_DungeonList;
	CYHHashTable<DWORD>							m_ChannelList;
	CYHHashTable<stDungeonReservation>			m_ReservationList;
	CYHHashTable<stDungeonNoPartyList>			m_NoPartyList;

	CYHHashTable<stMonsterList>					m_MonsterList_Easy;
	CYHHashTable<stMonsterList>					m_MonsterList_Normal;
	CYHHashTable<stMonsterList>					m_MonsterList_Hard;

	CYHHashTable<stWarpInfo>					m_WarpList;
	CYHHashTable<stSwitchNpcInfo>				m_SwitchNpcList;

	// 100419 ONS ���� �δ����� �ο��� ������ �����Ͽ� �����Ѵ�.( key : �δ��ε��� )
	typedef std::vector< DWORD >				BuffVector;
	typedef std::map< DWORD, BuffVector >		DungeonBuffMap;
	DungeonBuffMap								m_DungeonBuffMap;
	
	// �δ� ���� & ����
	stDungeon* Create(DWORD dwPartyID, DWORD dwChannelID, eDIFFICULTY diffculty);		// �δ� ����
	void Remove(DWORD dwChannelID);														// �δ� ����

public:
	CDungeonMgr(void);
	virtual ~CDungeonMgr(void);

	void	Init();
	void	Process();
	void	NetworkMsgParser(DWORD dwConnectionIndex, BYTE Protocol, void* pMsg, DWORD dwLength);


	// �δ� ���� & ����
	DWORD Entrance(DWORD dwPlayerID, DWORD dwPartyID, DWORD dwKeyIndex);
	void Exit(DWORD dwPlayerID);
	void ReservationDungeon(DWORD dwPlayerID, DWORD dwPartyID, DWORD dwChannelID, DWORD dwKeyIndex);
	void PlayerEntered(DWORD dwPlayerID, DWORD dwChannelID);
	DWORD GetChannelIDFromReservationList(DWORD dwPlayerID);
	VECTOR3* GetStartPosFromeservationList(DWORD dwPlayerID);

	// �δ� �˻�
	DWORD GetPlayerNumInDungeon(DWORD dwChannelID);
	DWORD GetPartyIndexFromDungeon(DWORD dwChannelID);
	
	DWORD GetDungeonNum();
	DWORD GetPlayerNum();
	DWORD GetMonsterNum();
	DWORD GetNpcNum();
	DWORD GetExtraNum();


	// �δ� ��Ʈ��
	void AddPoint(DWORD dwChannelID, DWORD value);
	void SubtractPoint(DWORD dwChannelID, DWORD value);
	DWORD GetPoint(DWORD dwChannelID);

	void SendMsgToChannel(MSGBASE* pMsg, DWORD dwLength, DWORD dwChannelID);

	void SetSwitch(DWORD dwChannelID, WORD num, BOOL val);
	int GetSwitch(DWORD dwChannelID, WORD num);
	void ResetSwitch(DWORD dwChannelID);

	void AddNoPartyList(DWORD dwPlayerID, DWORD dwPartyID);
	void RemoveNoPartyList(DWORD dwPlayerID);
	void MoveToLoginMap(DWORD dwCharIndex);

	// ���� ����
	void LoadMonsterList(CMHFile& file);
	void LoadMonsterRegenFile( char* filename, CYHHashTable<stMonsterList>& monsterList, eDIFFICULTY eDiff );
	void SetMonsterList(DWORD dwChannelID, eDIFFICULTY difficulty);
	void SummonMonsterGroup(DWORD dwGroupAlias, DWORD dwChannelID, eAliasKind alias);
	CMonster* AddMonster(stMonsterList* pMonster, DWORD dwChannelID, eDIFFICULTY difficulty);

	// NPC ����
	void LoadWarpList(CMHFile& file);
	void LoadSwitchList(CMHFile& file);
	void InitWarpList(DWORD dwChannelID);
	void InitSwitchNpcList(DWORD dwChannelID);
	void SetWarp(DWORD dwChannelID, DWORD dwWarpIndex, BOOL bActive);
	void SetSwitchNpc(DWORD dwChannelID, WORD wWarpIndex, BOOL bActive);
	BOOL CheckHackNpc( CPlayer* pPlayer, WORD wNpcIdx );
	DWORD GetNpcObjectIndex(DWORD dwChannelID, WORD dwNpcIndex);
	DWORD AddNpc( stSwitchNpcInfo* pSwitchNpc, DWORD dwChannelID );
	
	void SetBossMonster(DWORD dwChannelID);

	// Ʈ���� ����
	void AddTimer(DWORD dwChannelID, DWORD timerAlias);
	void RemoveTimer(DWORD dwChannelID);

	// Uitility Functions
	void		LoadDungeonInfo();
	DWORD		GetElapsedSecondFromStartTime(DWORD dwChannelID);
	eDIFFICULTY GetDifficulty(DWORD dwChannelID);
	eDIFFICULTY GetDifficultyFromKey(DWORD keyItemIndex);
	BOOL		IsDungeon(MAPTYPE) const;
	const stDungeon& GetDungeon(DWORD channelIndex);
	DWORD GetAliasHashCode( stMonsterList* pMonster, DWORD dwChannelID, eAliasKind eAlias );

	// ���� ����͸� ����
	void	SendInfoSummary(DWORD dwConnectionIndex, char* pMsg);
	void	SendInfoDetail(DWORD dwConnectionIndex, char* pMsg);

	// 100419 ONS ���� �δ����� �ο��� ���� ����/����
	void	StoreSwitchBuff( DWORD dwChannelID, DWORD dwBuffIdx );
	void	DelSwitchBuff( DWORD dwChannelID, DWORD dwBuffIdx );
};

EXTERNGLOBALTON(CDungeonMgr)
