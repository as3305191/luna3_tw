#pragma once

#define PCROOMMGR CPCRoomManager::GetInstance()
#define MAX_PCROOM_PROVIDE_ITEM_NUM		5		// PC�� ���� ������ �ִ� ����


// PC�� ���� ������ ����
struct stPCRoomItemInfo{
	DWORD dwItemIdx;
	WORD wItemCnt;
};

// PC�� ���� ����
struct stPCRoomBuffInfo{
	stPCRoomBuffInfo()
	{
		minLv = maxLv = 0;
	}

	LEVELTYPE minLv;					// �ּ� ����
	LEVELTYPE maxLv;					// �ִ� ����
	std::vector<DWORD> vecBuffIdx;
};

// ���� PC�� ���� ����
struct stPCRoomPartyBuffInfo{
	stPCRoomPartyBuffInfo()
	{
		wConnectedNumMin = wConnectedNumMax = 0;
	}

	WORD wConnectedNumMin;				// ������ �ּ� �ο�
	WORD wConnectedNumMax;				// ������ �ִ� �ο�
	std::vector<DWORD> vecBuffIdx;
};

struct stPCRoomUserInfo{
	DWORD dwPlayerIdx;
	DWORD dwPCRoomIdx;					// PC�� Index
	DWORD dwLastProvidePointTime;		// ������ PC�� ����Ʈ ���� �ð�
	DWORD dwPoint;						// PC�� ��������Ʈ
	DWORD dwLoginPoint;					// �α��ν� PC�� ����Ʈ
	BOOL bCanProvideItem;				// PC�� ������ ���� ���� ����
	BOOL bOpenMall;						// PC�� Mall Open ����
	DWORD dwUserIdx;
	// 100105 ONS PC�� �������ް��� �����߰�
	BOOL  bPCRoomBuffProvided;									// PC����� ���� ����
	BYTE  byDay;												// ��¥�� ���� PC�� ����
	WORD  wBuffLevel;											// ������ ���� PC�� ����
	WORD  wBuffStage;											// ��Ƽ������ ���� PC�� ����
	// 100812 ONS ����/���� ����������
	GameRoomType eGameRoomType;					
};

class CPCRoomManager
{
	// Script ����
	CYHHashTable<stPCRoomItemInfo>	m_htPCRoomItem[GameRoomTypeMaxCount];						// ���� ������ ����
	CYHHashTable<stPCRoomBuffInfo>	m_htPCRoomBuff[GameRoomTypeMaxCount];						// ���� ����
	cPtrList						m_PCRoomPartyBuffList[GameRoomTypeMaxCount];				// ���� PC�� ���� ����

	// 100310 ONS PC�� ��Ƽ������ �ǽð��� ������ ���� ��PC���� ������� �����ϵ��� �Ѵ�.
	typedef std::map< DWORD, WORD >	PCRoomMemberMap;
	PCRoomMemberMap					m_PCRoomMemberMap;					// < PC�� �ε���, PC�� ����� >

	WORD							m_wMallNPCIdx;						// PC�� ���� NPC Index

	DWORD							m_dwUpdatePointTime[GameRoomTypeMaxCount];				// PC�� ����Ʈ ���� �ð�
	DWORD							m_dwUpdatePointValue[GameRoomTypeMaxCount];				// PC�� ����Ʈ ���� ��ġ
	
	struct stat						m_ScriptFileStat;


	// User Data ����
	CYHHashTable<stPCRoomUserInfo>	m_PCRoomUser;						// PC�� ���� ����

	// 100305 ONS PC��������� ������Ʈ ���¸� �����Ѵ�.
	enum eBuffInfoState
	{
		eBuffInfoState_Init = 0,
		eBuffInfoState_Update,
		eBuffInfoState_End
	};

	typedef DWORD PlayerIndex;
	typedef std::map< PlayerIndex, _PACKET_GAME > PlayerPacketContainer;
	PlayerPacketContainer mPlayerPacketContainer;

public:
	CPCRoomManager();
	virtual ~CPCRoomManager();
	static CPCRoomManager* GetInstance();
	void SetPacketGame(PlayerIndex, const _PACKET_GAME&);
	void RemovePacketGame(PlayerIndex);
	DWORD GetGameRoomIndex(PlayerIndex) const;
	void Init();
	void Release();
    void ReleaseScript();
	void LoadScript();

	void Process();
	void ProvideItem(DWORD dwPlayerIdx);
	void AddPlayer(DWORD dwPlayerIdx, DWORD userIndex, DWORD dwPoint, WORD wMemberCount, BOOL bCanProvideItem, BOOL bPrivededBuff, BYTE byDay);
	void RemovePlayer(DWORD dwPlayerIdx);

	DWORD GetPoint( DWORD dwPlayerIdx );
	void SetPoint( DWORD dwPlayerIdx, DWORD dwPoint );
	DWORD UpdatePoint( DWORD dwPlayerIdx );													// ���� �ð����� Point Update �� ��ȯ

	WORD GetMallNPCIdx() const { return m_wMallNPCIdx; }

	void NetworkMsgParse( const MSGBASE* const pMsg, DWORD dwLength );

	// 100105 ONS ���� �ο� ���� �Լ�.
	BOOL LoadBuffSkill( DWORD dwPlayerIdx );						
	void BuffSkillStart( DWORD dwPlayerIdx, std::vector<DWORD> BuffIndex );			
	void RemovePCRoomBuff( DWORD dwObjectIdx );
	void RemoveBuff( DWORD dwPlayerIdx, DWORD dwBuffIndex );
	void PCRoomBuffLoad(CPlayer&);
	// 100312 ONS ��Ƽ���� �� ��Ƽ������Ʈ ���Ű��� �Լ��߰�
	void SetPartyMemberCount( DWORD dwPartyIdx, WORD wMemberCount );
	void SetPartyMemberCountToOtherMap( DWORD dwPartyIdx, WORD wMemberCount );
	WORD GetPartyBuffStage( WORD wCurCount, GameRoomType GameRoomType );
	WORD GetMinimumOfStage(WORD wPartyStage, GameRoomType GameRoomType );
	WORD GetPartyMemberCount( DWORD dwPartyIdx );
	BOOL GetPartyBuffIndex( DWORD dwPCRoomIdx, GameRoomType GameRoomType, std::vector<DWORD>& pBuffIndex );
	BOOL GetDailyBuffIndex( LEVELTYPE PlayerLevel, std::vector<DWORD>& pBuffIndex, BYTE byDay, GameRoomType GameRoomType );
	BOOL GetPartyBuffIndexByStage( WORD wPartyStage, std::vector<DWORD>& pBuffIndex, GameRoomType GameRoomType );
	BYTE GetDayInfo();

	void UpdatePartyBuff( DWORD dwPlayerIdx, DWORD dwPCRoomIdx, GameRoomType GameRoomType);
	void UpdatePartyBuffToOtherMap( DWORD dwPlayerIdx, DWORD dwPCRoomIdx, GameRoomType GameRoomType );
	void UpdateRCRoomMemberList( MSG_PCROOM_MEMBER_LIST* pMsg );
	GameRoomType GetUserGameRoomType( DWORD playerIndex );

};