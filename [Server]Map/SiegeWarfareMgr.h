// SiegeWarfareMgr.h: interface for the CSiegeWarfareMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIEGEWARFAREMGR_H__0620B52A_B018_4B66_9EF9_6A9D6703763D__INCLUDED_)
#define AFX_SIEGEWARFAREMGR_H__0620B52A_B018_4B66_9EF9_6A9D6703763D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define SIEGEWARFAREMGR CSiegeWarfareMgr::GetInstance()

// 080901 LYW --- SiegeWarfareMgr : ��ȯ ���� üũ ������ ��ġ ����.
#define CHECK_SUMMON_TIME	3000

class CGuild;
class CPlayer;

enum eSiegeWarfare_State
{
	eSiegeWarfare_State_Before,
	eSiegeWarfare_State_Start,
	eSiegeWarfare_State_First,
	eSiegeWarfare_State_End,
};

enum eSiegeWarfare_Type
{
	eSiegeWarfare_Type_Nera,
	eSiegeWarfare_Type_Max,
};

enum eSiegeWarfare_Map_Type
{
	eNeraCastle_Village,					// ���� �� ����
	eNeraCastle_UnderGround,				// ���� ���� ���� ��
	eNeraCastle_Zebin,						// ���� ��������
	eNeraCastle_Lusen,						// ���� �缾����
	eSiegeWarfare_Map_Max,
};

enum eSiegeWarfare_Info_Kind
{
	eSiegeWarfare_Info_Damage_Ratio,
	eSiegeWarfare_Info_Heal_Ratio,
	eSiegeWarfare_Info_Critical_Rate,
	eSiegeWarfare_Info_WarterSeed_GuildLevel,
	eSiegeWarfare_Info_WarterSeed_Time,
	// 081017 LYW --- SiegeWarfareMgr : ������ ����ϴ� �Ÿ� ���� �з� �߰�.
	eSiegeWarfare_Info_Craving_Distance,
	eSiegeWarfare_Info_VilageInfo,
	// 081210 LYW --- SiegeWarfareMgr : ��� ���� �� �罨/���� ��� ��Ȱ��ġ �з� �߰�.
	eSiegeWarfare_Info_ReviveInfo_Rushen,
	eSiegeWarfare_Info_ReviveInfo_Zevyn,
	eSiegeWarfare_Info_Max,
};

// ������ ���� �޽���
enum eSiegeWarfare_Notify
{
	eSiegeWarfare_Notify_Befor_30m,
	eSiegeWarfare_Notify_Befor_5m,
	eSiegeWarfare_Notify_Begin,
	eSiegeWarfare_Notify_End,
	eSiegeWarfare_Notify_End_Before_10m,
	eSiegeWarfare_Notify_Max,
};

enum eSiegeWarfare_WaterSeed
{
	eSiegeWarfare_WaterSeed_Using,
	eSiegeWarfare_WaterSeed_Using_Complete,
	eSiegeWarfare_WaterSeed_Cancel,
};

enum eSiegeWarfare_State_Message_Type
{
	eSiegeWarfare_State_Message_Single,
	eSiegeWarfare_State_Message_All,
	eSiegeWarfare_State_Message_Max,
};

// 081009 LYW ---  �����ҽ� ����.
enum eSiegeWarfare_WarterSeed_Error
{
	eSiegeWarfare_WarterSeed_Error_NotState = 1,
	eSiegeWarfare_WarterSeed_Error_UsingPlayer,
	eSiegeWarfare_WarterSeed_Error_NotGuildJoin,
	eSiegeWarfare_WarterSeed_Error_CastleGuld,
	eSiegeWarfare_WarterSeed_Error_GuildLevel,
	// 081017 LYW --- SiegeWarfareMgr : ���ͽõ� ��� �Ÿ�üũ ���� �з� �߰�.
	eSiegeWarfare_WarterSeed_Error_SoFarDistance,
	// 090706 pdy --- Ŭ���̾�Ʈ ���ͽõ� �ð����� ���� �з� �߰�.
	eSiegeWarfare_WarterSeed_Error_WarterSeedTime,
	eSiegeWarfare_WarterSeed_Error_Max,
};

struct VillageWarp
{
	DWORD	MapNum;
	float	PosX;
	float	PosZ;
};

struct SiegeWarfareSkillInfo
{
	float	DecreaseDamageRatio;								// ������ ���ҷ�(%)
	float	DecreaseHealRatio;									// �� ���ҷ�(%)
	float	DecreaseCriticalRatio;								// ũ��Ƽ�� ���ҷ�(%)
};

struct SiegeWarfareKindMapInfo
{
	DWORD MapNum;
	WORD WarfareType;
	WORD MapType;
	float PosX;
	float PosZ;
};

// 081006 LYW --- SiegeWarfareMgr : ���� ���������� ����� ���̵� ����Ʈ ����ü �߰�.
struct st_GUILDMAPMOVEINFO
{
	MAPTYPE mapNum ;

	float fXpos ;
	float fZpos ;
} ;

// 080919 LYW --- cMapMoveDialog : ���̾�α� �뵵 �з�.
enum eMapMoveDialogUseCase
{
	eNormalStype = 0,														// �Ϲ������� ����ϴ� �뵵.
	eCastle_Rushen,															// �罨 ��� ����ȸ���� ����ϴ� �뵵.
	eCastle_Zevyn,															// ���� ��� ����ȸ���� ����ϴ� �뵵.
} ;

class CSiegeWarfareMgr  
{
	CSiegeWarfareMgr();
	~CSiegeWarfareMgr();

	DWORD					m_dwMapType;									// �������� Ÿ��
	DWORD					m_dwCastleGuildIdxList[eSiegeWarfare_Map_Max];	// �� ��� �ε���
	VillageWarp				m_stCastleVilage;								// �� ����
	SiegeWarfareSkillInfo	m_stSkillInfo;									// �� ����
	SYSTEMTIME				m_stWaterSeedUsingCompleteTime;					// ���ͽõ� ��� �Ϸ� �ð�
	WORD					m_wState;										// �� ����
	DWORD					m_dwWarfareType;								// ������ Type
	WORD					m_wNotifyStep;									// �˸� ���� (�Ϻθ� ����)

	DWORD					m_dwWaterSeedUsingGuildLevelLimit;				// ���ͽõ带 ����ϱ� ���� ��� ����
	DWORD					m_dwWaterSeedUsingSecond;						// ���ͽõ带 �ÿ��ϴ� �ð�(��)
	BOOL					m_bWaterSeedUsing;								// ���ͽõ带 ��������� üũ
	DWORD					m_dwWaterSeedUsingPlayerIdx;					// ���ͽõ带 ������� �÷��̾� �ε���

	BOOL					m_bSummonFlag;

	// 081009 LYW --- SiegeWarfareMgr : �����ҽ�����.
	BOOL					m_bWaterSeedFlag;

	std::list<SiegeWarfareKindMapInfo>	m_SiegeWarfareKindMapList;					// ������ ���� �� ����Ʈ
	std::list<DWORD>					m_ForbiddenSkillList;						// ���Ұ� ��ų ����Ʈ
	std::list<DWORD>					m_ForbiddenItemList;						// ���Ұ� ������ ����Ʈ

	std::list<SYSTEMTIME>				m_ScheduleStartTime[eSiegeWarfare_Type_Max];
	std::list<SYSTEMTIME>				m_ScheduleEndTime[eSiegeWarfare_Type_Max];

	// 080829 LYW --- SiegeWarfareMgr : ���� ��ų ����� ���� ���� ����.
	DWORD					m_dwBuffSkillPay ;

	// 080901 LYW --- SiegeWarfareMgr : �ʼ��� ���� ��, ������ �����ϴ� ���� �ִ���, ������ �ٽ� �ε� ��û�� �ϱ� ���� ���� �߰�.
	BYTE					m_IsLoadedSummon ;

	// 081017 LYW --- SiegeWarfareMgr : ���ͽõ带 ����� �� �ִ� �Ÿ��� ���� ������ �߰��Ѵ�.
	WORD					m_wCravingDistance ;

	// 081210 LYW --- SiegeWarfareMgr : ��� ���� �� �罨/���� ��Ȱ(��������) ��ġ�� ���� ����ü.
	VillageWarp				m_stDGRP_Rushen ;
	VillageWarp				m_stDGRP_Zevyn ;

public:
	MAKESINGLETON( CSiegeWarfareMgr );
	// �� ���� �ʹ�ȣ�� ����
	VOID					SetVilageMapInfo(DWORD MapNum, float PosX, float PosZ)	{	m_stCastleVilage.MapNum = MapNum;
																						m_stCastleVilage.PosX = PosX;
																						m_stCastleVilage.PosZ = PosZ;	}
	// ���ҵ� ���������� �����´�.
	float					GetDecreaseDamageRatio()	{	return m_stSkillInfo.DecreaseDamageRatio;	}
	// ���ҵ� ������ �����´�.
	float					GetDecreaseHealRatio()		{	return m_stSkillInfo.DecreaseHealRatio;	}
	// ���ҵ� ũ��Ƽ�ü�ġ�� �����´�.
	float					GetDecreaseCriticalRatio()	{	return m_stSkillInfo.DecreaseCriticalRatio;	}

	// �� ���� ������ ������ �´�.
	VillageWarp*				GetVilageInfo()		{	return &m_stCastleVilage;	}

	// ���� ���� �ϰ� �ִ� ��� �ε��� ����
	VOID					SetCastleGuildIdx(DWORD MapType,DWORD	GuildIdx)	{	m_dwCastleGuildIdxList[MapType] = GuildIdx;	}
	// ���� ���� �ϰ� �ִ� ��� �ε����� ���´�.
	DWORD					GetCastleGuildIdx(DWORD MapType)	{	return m_dwCastleGuildIdxList[MapType];	}

	// ���� �Ӽ��� �����Ͽ� �ش�.
	VOID					SetMapInfo(DWORD MapType)	{	m_dwMapType = MapType;	}
	// ���� �Ӽ��� �����´�.
	DWORD					GetMapInfo()	{	return m_dwMapType;	}

	VOID					SetCastleGuildIndex(DWORD MapType, DWORD GuildIDX)	{	m_dwCastleGuildIdxList[MapType] = GuildIDX;	}

	VOID					SetSummonFlag(BOOL bFlag)	{	m_bSummonFlag = bFlag;	}

	// ���������ΰ�?
	BOOL					IsSiegeWarfareZone(DWORD MapNum,BOOL bCheckWarState = TRUE );
	// ���� ���� �������� �ΰ�?
	BOOL					IsSiegeWarfareZone();

	// �� ������ �о� �´�.
	VOID					LoadMapInfo();

	// �������� ������ �÷��̾��� �� ��尡 �ƴ� �÷��̾ �� ������ ���� ��Ų��.
	VOID					WarpToVilage();
	// ������ �÷��̾� �Ѹ��� ������ ���� �Լ�
	VOID					WarpToVilagePlayer(CPlayer* pPlayer);

	// ������ ����Ʈ�� ���� ��Ű�� ���� �Լ�
	BOOL					WarpToAgit(CPlayer* pPlayer);

	// 080922 LYW --- SiegeWarfareMgr : ���������� ���� �� �̵��� �ϴ� �Լ� �߰�.
	VOID					ChangeMap(CPlayer* pPlayer, WORD wType, WORD wMapNum) ;

	// ���� ����
	VOID					Process();

	// �ʱ�ȭ ������ ���� �Լ�
	VOID					Init();

	// ��Ʈ��ũ �޽��� �ļ�
	VOID					NetworkMsgParse( DWORD dwConnectionIndex, BYTE Protocol,void* pMsg );	

	// ���� ���� ���� ��������
	WORD					GetState()	{ return m_wState; }
	// State�� ������ �ִ� ���ÿ� ��Ŷ�� ������. MessageType�� ���� ������.
	VOID					SetState(WORD State, WORD MessageType);
	// State�� �������ش�.
	VOID					SetState(WORD State)		
	{	
		m_wState = State;	

		// 081028 LYW --- SiegeWarfareMgr : ���� ���� �ٲ��� �α׷� �����.
		char szLog[1024] = {0, } ;
		sprintf( szLog, "SelpMapNum : %d \t State : %d", g_pServerSystem->GetMapNum(), m_wState ) ;
		WriteSiegeSchedule( szLog ) ;
	}

	// ������ ���۵Ǿ����� üũ �ϴ� �Լ�. �� �Լ� ���ο��� ���� 30����, 5���� �޽����� ���� ó�� �Ѵ�.
	// eNeraCastle_Village �� �Ӽ��� ������ �ִ� �ʸ� üũ �Ѵ�. �� �Լ��� ��ȭ�� ��쿡 �ٸ� ���� ���� �ʿ�
	// ������ ���۵Ǿ����� �˷��ش�.
	BOOL					IsBeginSiegeWarfare();
	// ������ �Ϸ� �Ǿ��°�?
	BOOL					IsEndSiegeWarfare();

	// ���ͽõ带 ����ϰ� �ִ� ���ΰ�?
	BOOL					IsWaterSeedUsing()		{	return m_bWaterSeedUsing;	}
	// ���ͽõ带 ������� �÷��̾��ΰ�?
	BOOL					IsWaterSeedUsingPlayer(DWORD PlayerIdx);

	// ��� ������ ��ų �ΰ�?
	BOOL					IsFobiddenSkill(DWORD SkillIdx);
	// ��� ������ ������ �ΰ�?
	BOOL					IsFobiddenItem(DWORD ItemIdx);

	// ���ͽõ� ����
	VOID					CancelWaterSeedUsing(CPlayer* pPlayer);

	// Ȥ�ó� �ϴ� ������ ���� �Լ� ��_��;; ���� �ð��߿� �ٽ� �׶� ������ ���� ���� �Ǿ��� ��쿡 ���ɼ� ����.
	VOID					CheckBeforeState();

	// 080829 LYW --- SiegeWarfareMgr : ���� ��ų ����� ����/��ȯ�ϴ� �Լ� �߰�.
	VOID					Set_BuffSkillPay(DWORD dwPay) { m_dwBuffSkillPay = dwPay ; }
	DWORD					Get_BuffSkillPay() { return m_dwBuffSkillPay ; }

	VOID Set_LoadSummonFlag(BYTE byFlag) { m_IsLoadedSummon = byFlag; }
	BOOL Is_LoadedSummon() { return m_IsLoadedSummon; }
	BOOL Is_CastleMap() const { return eNeraCastle_Village == m_dwMapType; }

	// 081006 LYW --- SiegeWarfareMgr : �� �� ��� ���������� ����� ���̵� ����Ʈ ������ ���� ����Ʈ �߰�.
	typedef std::list< st_GUILDMAPMOVEINFO >	L_GUILDMAPMOVEINFO ;
	L_GUILDMAPMOVEINFO		m_List_MapMove_Rushen ;
	L_GUILDMAPMOVEINFO		m_List_MapMove_Zevyn ;
    
	void Load_GuildMapMoveInfo();
	void Get_GuildMapMoveInfo_Rushen(st_GUILDMAPMOVEINFO* pInfo, MAPTYPE);
	void Get_GuildMapMoveInfo_Zevyn(st_GUILDMAPMOVEINFO* pInfo, MAPTYPE);
	BYTE Is_ValidDistance_FromWell(CPlayer*);

	L_GUILDMAPMOVEINFO& Get_MapMoveList_Rushen() { return  m_List_MapMove_Rushen; }
	L_GUILDMAPMOVEINFO& Get_MapMoveList_Zevyn()	{ return  m_List_MapMove_Zevyn; }
	void WriteSiegeSchedule( char* pMsg );

	VOID VillageProcess();
	VOID UnderGroundProcess();
	VOID EmblemProcess();
	VOID WaterSeedProcess();
	WORD MC_InstallWaterSeed(CPlayer*);
	VOID MMC_SiegeWarfareState(WORD State);
	BYTE CM_RequestCastleBuff(CPlayer*, WORD wUniqueNpcIndex);

	VOID LoadSiegeWarfareInfo();
	VOID UpdateSiegeWarfareInfo( BOOL bInit = FALSE );
	
	VillageWarp* GetDGRP_Rushen() { return &m_stDGRP_Rushen ; }
	VillageWarp* GetDGRP_Zevyn() { return &m_stDGRP_Zevyn ; }
	void WriteLog(char* pMsg) ;

	VOID UpdateWaterSeedComplateSynToDB( DWORD dwPlayerIdx , DWORD dwGuildidx );
	VOID UpdateWaterSeedComplateAckFromDB( DWORD dwResult , DWORD dwPlayerIdx ,DWORD dwMapType ,DWORD dwGuildIdx);
	void GuildBreakUp( DWORD dwGuildidx );
};

#endif // !defined(AFX_SIEGEWARFAREMGR_H__0620B52A_B018_4B66_9EF9_6A9D6703763D__INCLUDED_)
