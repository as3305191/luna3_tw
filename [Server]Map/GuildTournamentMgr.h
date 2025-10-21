// GuildTournamentMgr.h: interface for the CGuildTournamentMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GUILDTOURNAMENTMGR_H__21A71231_1238_4D1F_99B6_D53A80C23436__INCLUDED_)
#define AFX_GUILDTOURNAMENTMGR_H__21A71231_1238_4D1F_99B6_D53A80C23436__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\[CC]BattleSystem\GTournament\Battle_GTournament.h"

#define GTMGR CGuildTournamentMgr::GetInstance()
#define GTMAPNUM		94
#define GTRETURNMAPNUM	52

#define GT_NOTIFY_STARTTIME			(60000 * 5)
#define GT_NOTIFY_INTERVAL			60000

#define GT_CONVERT_DAY2MS(day)		(60000 * 60 * 24 * day)
#define GT_CONVERT_HOUR2MS(hour)	(60000 * 60 * hour)
#define GT_CONVERT_MIN2MS(min)		(60000 * min)
#define GT_CONVERT_SEC2MS(sec)		(1000 * sec)

#define GT_WAITING_NONE			0x00000000
#define GT_WAITING_INFOLOAD		0x00000001
#define GT_WAITING_INFOLOADALL	0x00000010
#define GT_WAITING_PLAYERLOAD	0x00000100

enum {
	eGTTime_OP_Add = 0,
	eGTTime_OP_Sub,
};

enum {
	eGTRewardReceiver_Master = 0,
	eGTRewardReceiver_Players,
	eGTRewardReceiver_GuildMember,
};

class CGuild;
class CPlayer;

struct GTRewardItem
{
	BYTE	nReceiver;
	DWORD	dwItemIndex;
	WORD	wNum;
};

struct GTRewardInfo
{
	DWORD	dwPoint;
	CYHHashTable<GTRewardItem>	RewardItemList;
	// 091113 ONS ��庸�� �߰�
	DWORD	dwGold;
};

struct GTLevel_Value
{
	WORD wMinLevel;
	WORD wMaxLevel;
	DWORD dwValue;

	BOOL IsInRange(WORD nLevel)
	{
		if(wMinLevel<=nLevel && nLevel<=wMaxLevel)
			return TRUE;

		return FALSE;
	};
};

struct GTImmortalInfo
{
	DWORD	dwClassCode;
	DWORD	dwTime;
};

struct GTGeneralInfo
{
	BOOL	bObAllow;				//��������뿩��
	DWORD	dwFee[2];				//������   : [0]�������Ʈ [1]������(42������)
	WORD	wLimit_Team[2];			//�������� : [0]�ּ�������, [1]�ִ�������
	WORD	wLimit_Guild[2];		//���ѱ�� : [0]������巹��, [1]�����������
	DWORD	dwEntryStart;			//��û�Ⱓ : ��û����
	DWORD	dwEntryEnd;				//��û�Ⱓ : ��û����
	DWORD	dwStartTime;			//������ : ���۽ð�
	WORD	wTableOpen;				//������ ����ǥ�����ð� (�д���)
	WORD	wPlayRule[2];			//����� : [0]���Ÿ��, [1]����ǥ
	WORD	wPlayTime[2];			//���ð� : [0]����ð�, [1]��������� ������ð� (�д���)
	GTRewardInfo	RewardInfo[2];	//�������� : [0]1������, [2]2������

	int		nRespawnListSize;		//�������ð� ����Ʈ
	GTLevel_Value* pRespawnList;

	CYHHashTable<GTImmortalInfo> ImmortalList;	//�����ð� ���̺�

	GTGeneralInfo()
	{
		RewardInfo[0].RewardItemList.Initialize(10);
		RewardInfo[1].RewardItemList.Initialize(10);
		ImmortalList.Initialize(60);
		pRespawnList = NULL;
	};

	~GTGeneralInfo()
	{
		Clear();
	}

	void Clear()
	{
		DWORD dwSize;

		int i;
		for(i=0; i<2; i++)
		{
			dwSize = RewardInfo[i].RewardItemList.GetDataNum();
			if(0 < dwSize)
			{
				RewardInfo[i].RewardItemList.SetPositionHead();
				GTRewardItem* pItem;

				while((pItem = RewardInfo[i].RewardItemList.GetData())!= NULL)
				{
					delete pItem;
					pItem = NULL;
				}
			}
			RewardInfo[i].RewardItemList.RemoveAll();
		}

		dwSize = nRespawnListSize;
		if(0 < dwSize)
		{
			delete [] pRespawnList;
		}

		dwSize = ImmortalList.GetDataNum();
		if(0 < dwSize)
		{
			ImmortalList.SetPositionHead();
			GTImmortalInfo* pInfo;

			while((pInfo = ImmortalList.GetData())!= NULL)
			{
				delete pInfo;
				pInfo = NULL;
			}
		}
		ImmortalList.RemoveAll();
	};
};

struct GTPlayerInfo
{
	DWORD dwPlayerID;
	DWORD dwScore;
	char name[MAX_NAME_LENGTH+1];

	GTPlayerInfo() {dwPlayerID=dwScore=0; memset(name, 0, sizeof(name));}
};

struct GTInfo
{
	CGuild* pGuild;
	WORD	wRound;
	WORD	Position;

	CYHHashTable<GTPlayerInfo>	PlayerList;

	void ClearPlayerList()
	{
		GTPlayerInfo* pPlayerInfo = NULL;

		PlayerList.SetPositionHead();
		while((pPlayerInfo = PlayerList.GetData())!= NULL)
		{
			if(pPlayerInfo)
				delete pPlayerInfo;

			pPlayerInfo = NULL;
		}

		PlayerList.RemoveAll();
	}

	void ResetPlayerScore()
	{
		GTPlayerInfo* pPlayerInfo = NULL;

		PlayerList.SetPositionHead();
		while((pPlayerInfo = PlayerList.GetData())!= NULL)
		{
			if(pPlayerInfo)
				pPlayerInfo->dwScore = 0;
		}
	}

	GTInfo() {PlayerList.Initialize(20);}
	~GTInfo() {ClearPlayerList();}
};


class CGuildTournamentMgr
{
	GTGeneralInfo			m_GeneralInfo;						// ��ʸ�Ʈ ����
	CYHHashTable<GTInfo>	m_pEntryList;						// ��ʸ�Ʈ ��� ���
	BATTLE_INFO_GTOURNAMENT	m_BattleInfo[MAXGUILD_INTOURNAMENT];// ��Ʋ������ ���� ����
	DWORD					m_InitBattleTable[MAXGUILD_INTOURNAMENT];	// ���� ��Ʋ����ǥ
	DWORD					m_CurBattleTable[MAXGUILD_INTOURNAMENT];	// ���� ��Ʋ����ǥ

	BOOL					m_bIgnoreSchedule;					// �������� ������
	BOOL					m_bStartGame;						// ���ӽ����߳�?
	BOOL					m_bShuffleBattleTable;				// ����ǥ ȥ���߳�?
	BOOL					m_bNoMatchRound;					// �̹����� ��ȿ��Ⱑ ����.

	WORD					m_wTournamentCount;					// ��ʸ�Ʈ ���� ��
	WORD					m_wCurBattleCount;					// �� ���忡 ������ ����
	WORD					m_wInTournamentTeam;				// �� ���忡 ������� ��
	WORD					m_wEndTournamentTeam;				// �� ���忡 �������� ��

	WORD					m_wCurRound;						// ���� ������ ���� 16��->8��->4��->���
	WORD					m_wMaxRound;						// ��ü �����
	WORD					m_wInitRound;						// ���� ����� 16��=1����, 8��=2����
	WORD					m_wTournamentState;					// ���� ����� ���� (������û��, ������û��, ������, ������, ������, ��ħ)
	
	DWORD					m_dwRemainTime;						// ���� ���μ����� ���ó������ �ʰ� �̽ð���ŭ ����� �� ó��
	DWORD					m_dwLastNotifyTime;					// ������ ������ �� �ð�.


	// ����ý� ���ÿ� �ʿ��� ����
	BOOL					m_bNeedAdjust;
	WORD					m_wLastState;
	WORD					m_wLastRound;
	DWORD					m_dwWaitingFlag;
	DWORD					m_dwWaitingForDBLoad;
	

public:
	MAKESINGLETON( CGuildTournamentMgr );

	CGuildTournamentMgr();
	virtual ~CGuildTournamentMgr();

	void Init();
	void ReadyToNewTournament();
	void ReadyToNewMatch();
	void Process();

	void Process_BeforeRegist();
	void Process_Regist();
	void Process_BeforeEntrance();
	void Process_Entrance();
	void Process_Process();
	void Process_Leave();
	
	void PlayerRegist(CGuild* pGuild, DWORD* pPlayerList);
	DWORD RegistGuild( CGuild* pGuild);
	DWORD ForceRegistGuild( CGuild* pGuild, DWORD Round, DWORD Position );

	void MakeBattleTable();
	void CreateBattle();
	void StartTournament();

	void MoveToBattleMap_Syn(DWORD dwConnectionIndex, void* pMsg);
	void NetworkMsgParse( DWORD dwConnectionIndex, BYTE Protocol,void* pMsg );

	int GetBattleID( CGuild* pGuild );
	void SetResult( DWORD GuildIdx, BOOL bWin, BOOL bUnearnedWin=FALSE );

	void FillLastGTInfo(DWORD dwGuildIdx, WORD wPosition, WORD wRound);
	void FillPlayerInfo(WORD wGTCount, DWORD dwGuildIdx, DWORD* pPlayers);
	GTInfo* GetGTEntryInfo(DWORD dwGuildIdx) {return m_pEntryList.GetData(dwGuildIdx);}
	void RemoveGTEntryInfo(DWORD dwGuildIdx);// {m_pEntryList.Remove(dwGuildIdx);}

	DWORD GetRespawnByLevel(int nLevel);
	DWORD GetImmortalTimeByClass(WORD wClassCode);
	WORD GetTournamentCount()				{return m_wTournamentCount;}
	void SetTournamentCount(WORD wCount)	{m_wTournamentCount = wCount;}
	WORD GetTournamentState()				{return m_wTournamentState;}
	void SetTournamentState(WORD wState)	{m_wTournamentState = wState;}
	void SetCurRound(WORD wRound)			{m_wCurRound = wRound;}
	// 081027 LUJ, ���� ���� ��ȯ
	WORD GetCurRound() const				{return m_wCurRound;};

	void SetLastState(WORD wState)			{m_wLastState = wState;}
	void SetLastRound(WORD wRound)			{m_wLastRound = wRound;}

	void SetPlayerScore(DWORD dwGuildIdx, DWORD dwPlayerIdx, DWORD dwScore);
	DWORD GetPlayerScore(DWORD dwGuildIdx, DWORD dwPlayerIdx);

	DWORD GetFeePoint() {return m_GeneralInfo.dwFee[0];}
	DWORD GetFeeMoney() {return m_GeneralInfo.dwFee[1];}
	WORD GetGoal()		{return m_GeneralInfo.wPlayRule[1];}
	WORD GetInitRound() {return m_wInitRound;}
	void GetPlayTypeObject(DWORD& dwType, DWORD& dwObject);

	void SetStateToRegist(WORD wTournamentCount);
	WORD AdjustedState();

	// SendToMsg
	void SendReward(WORD wRank, DWORD dwGuildIdx);
	void Notify_Send2Guild(DWORD dwGuildIdx, DWORD dwNotifyType, DWORD dwParam1, DWORD dwParam2=0, DWORD dwParma3=0);
	void Notify_Send2GTPlayer(DWORD dwGuildIdx, int nNotifyType, DWORD dwParam1, DWORD dwParam2=0, DWORD dwParma3=0);
	void Notify_Send2AllGTPlayer(int nNotifyType, DWORD dwParam1, DWORD dwParam2=0, DWORD dwParma3=0);
	void Notify_Send2AllUser(int nNotifyType, DWORD dwParam1, DWORD dwParam2=0);

	// Utility
	void GetTimeFromGTTime(DWORD dwSrcTime, WORD& wD, WORD& wH, WORD& wM);
	DWORD GetGTTimeOP(DWORD dwSrcTime, DWORD dwOPTime, BYTE OPType);
	DWORD GetRemaindTime(DWORD dwDestTime);

	void AddWaitingFlag(DWORD dwWait)		{m_dwWaitingFlag |= dwWait;}
	void RemoveWiatingFlag(DWORD dwWait)	{m_dwWaitingFlag &= ~dwWait;}
	void SetNeedAdjust(BOOL bVal)			{m_bNeedAdjust = bVal;}


	// �Ʒ� �Լ� ���� �ݵ�� �Լ� ��ȣ���� ������ �����ؾ���.
	// 1)IsRegistDay() , 2)IsBeforeEntrace(), 3)IsBeforeRegist() �� ������� ���Ǿ�� �մϴ�.
	BOOL IsRegistDay();
	BOOL IsBeforeEntrace();
	BOOL IsBeforeRegist();
};

#endif // !defined(AFX_GUILDTOURNAMENTMGR_H__21A71231_1238_4D1F_99B6_D53A80C23436__INCLUDED_)
