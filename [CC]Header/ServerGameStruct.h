#pragma once

class CPlayer;
class CMonster;

// taiyo
// ȸ�� ����, ȸ�� ����� �ð� ��� --------------------------------------------------
struct RECOVER_TIME
{
	RECOVER_TIME():lastCheckTime(0),bStart(TRUE){}
	BOOL bStart;
	DWORD lastCheckTime;
};
// �๰�� ���� ȸ��
struct YYRECOVER_TIME
{
	YYRECOVER_TIME():bStart(FALSE){}
	BOOL bStart;
	BYTE count;
	DWORD recoverUnitAmout;
	DWORD lastCheckTime;
	DWORD recoverDelayTime;
};

// 100104 LUJ, ���� ���� ó�� ����
struct StateParameter
{
	DWORD AttackStartTime;
	DWORD SearchLastTime;
	DWORD CollSearchLastTime;
	DWORD CurAttackKind;
	DWORD PursuitForgiveStartTime;
	DWORD prePursuitForgiveTime;
	CMonster* pHelperMonster;
	CPlayer* pTarget;
	eMONSTER_ACTION stateNew;
	eMONSTER_ACTION stateCur;
	eMONSTER_ACTION stateOld;
	DWORD nextTime;
	BOOL bStateUpdate;
};

#pragma pack(push, 1)

struct SERVERINFO
{
	SERVERINFO()	{	memset(this,0,sizeof(SERVERINFO));	}
	SERVERINFO(const SERVERINFO* pInfo)	{	memcpy(this,pInfo,sizeof(SERVERINFO));	}
	SERVERINFO(WORD kind, WORD num, LPCTSTR ipsvr, WORD portsvr, LPCTSTR ipusr,WORD portusr)
		: wServerKind(kind),wServerNum(num),wPortForServer(portsvr),wPortForUser(portusr)
	{
		_tcscpy(szIPForServer,ipsvr);
		_tcscpy(szIPForUser,ipusr);
		dwConnectionIndex = 0;
		wAgentUserCnt = 0;
	}
	WORD	wServerKind;						// ���� ���� : agent, map, distribute
	TCHAR	szIPForServer[MAX_IPADDRESS_SIZE];
	TCHAR	szIPForUser[MAX_IPADDRESS_SIZE];	
	WORD	wPortForServer;						// ���� ��Ʈ
	WORD	wPortForUser;						// ���� ��Ʈ
	WORD	wServerNum;							// 0 ~ (���ȣ Ȥ�� ������ȣ)

	DWORD	dwConnectionIndex;					// Ŀ�ؼ� �ε��� : �����ε���

	//--------agent field----------------
	WORD	wAgentUserCnt;
};
// powerup ���� ����ü /////////////////////////////////////////////
struct MSG_PWRUP_BOOTUP : public MSGROOT
{
	DWORD dwProcessID;
	SERVERINFO BootInfo;
};

struct MSG_PWRUP_BOOTLIST : public MSGROOT
{
	void	AddList(SERVERINFO * pInfo)
	{
		BootList[Num++] = *pInfo;
	}
	DWORD GetSize() const { return sizeof( MSG_PWRUP_BOOTLIST ) - ( sizeof( BootList ) / sizeof( *BootList ) - Num ) * sizeof( *BootList ); }
	WORD		MapServerPort;
	WORD		Num;
	SERVERINFO BootList[ MAX_MAP_NUM ];
};
struct MSG_PWRUP_CONNECT_ACK : public MSGROOT
{
	SERVERINFO BootList;
	//WORD Key;
};
struct MSG_PWRUP_CONNECT_SYN : public MSGROOT
{
	SERVERINFO BootList;
};

struct MSGMC :public MSGROOT
{
	DWORD dwConnectionIdxMC;
};
struct MSGUSERCOUNT :public MSGMC
{
	WORD ServerPort;
	DWORD dwUserCount;
};
struct MSGEXECUTESERVER : public MSGMC
{
	WORD ServerType;
	WORD ServerNum;
	WORD ServerPort;
	char ServerIP[MAX_IPADDRESS_SIZE];
};
struct MSGSHUTDOWNSERVER : public MSGEXECUTESERVER
{
};

struct MSGNOTICE : public MSGMC
{
	char Msg[256];
};

struct MSERVERINFO
{
	MSERVERINFO()
	{
		*szIPForServer = 0;
		wPortForServer = 0;
		dwConnectionIndex = 0;
	}
	char	szIPForServer[MAX_IPADDRESS_SIZE];
	WORD	wPortForServer;						// ���� ��Ʈ
	char	szIPForUser[MAX_IPADDRESS_SIZE];
	WORD	wPortForUser;
	WORD	wServerType;
	WORD	wServerNum;

	DWORD	dwConnectionIndex;					// Ŀ�ؼ� �ε��� : �����ε���

	DWORD	ProcessID;							//mapserver
	__time64_t	ExpiredTime;							//mapserver
	WORD	wServerState;							//mapserver

	// 090709 --- ServerTool ���� Off�Ǵ� ��� TRUE ����
	BOOL	bOffByServerTool;
};

struct GUILDMEMBERINFO_ID
{
	GUILDMEMBERINFO MemberInfo;
	DWORD GuildIdx;
};

struct MSGMSINFO : public MSGROOT
{
	MSERVERINFO MSInfo;
};

enum
{
	SERVERSTATE_NONE =0,
	SERVERSTATE_OFF=1,
	SERVERSTATE_ON=2,
	SERVERSTATE_ABNORMAL=3,
	SERVERSTATE_RECOVER=4
};

enum
{
	AUTOPATCHSTATE_START = 0,
	AUTOPATCHSTATE_PROC,
	AUTOPATCHSTATE_END,
};

struct SERVERSTATE
{
	WORD ServerType;
	WORD ServerNum;
	WORD ServerPort;
	char ServerIP[MAX_IPADDRESS_SIZE];
	WORD ServerState;
	int	 UserLevel;
	DWORD UserCount;
};

#define MAX_SERVERSTATE_NUM	128
struct MSGSERVERSTATE : public MSGMC
{
	SERVERSTATE serverState;

	// 090709 --- ServerTool ���� Off�Ǵ� ��� TRUE ����
	BOOL bOffByServerTool;
};

struct MSGMONITORTOOLALLSTATE : public MSGROOT
{

	void AddServerState(SERVERSTATE * pState)
	{
		serverState[num++] = *pState;
	}
	int GetSize()
	{
		return sizeof(MSGMONITORTOOLALLSTATE) - (MAX_SERVERSTATE_NUM - num)*sizeof(SERVERSTATE);
	}
	int num;
	SERVERSTATE serverState[MAX_SERVERSTATE_NUM];
};

struct MSSTATE
{
	char	sServerIP[MAX_IPADDRESS_SIZE];
	WORD	wServerPort;
	WORD	wAutoPatchState;
	char	sAutoPatchVersion[13];
};

struct MSGMSSTATE : public MSGROOT
{
	MSSTATE	MSState;
	MSGMSSTATE()
	{
		memset( &MSState, 0, sizeof(MSSTATE) );
	}
};

struct MSGMSSTATEALL : public MSGROOT
{
	int		nNum;
	MSSTATE	MSState[MAX_SERVERSTATE_NUM];	

	MSGMSSTATEALL()
	{
		memset( MSState, 0, sizeof(MSSTATE)*MAX_SERVERSTATE_NUM );
		nNum = 0;
	}
	void	AddMSServerState( MSSTATE* pState )
	{
		MSState[nNum++] = *pState;
	}
	int		GetSize()
	{
		return sizeof(MSGMSSTATEALL) - (MAX_SERVERSTATE_NUM-nNum)*sizeof(MSSTATE);
	}
};

// 06. 05 HIDE NPC - �̿���
struct HIDE_NPC_INFO
{
	WORD UniqueIdx;
	BOOL ChannelInfo[MAX_CHANNEL_NUM];
};

struct _PACKET_GAME
{
	int Packet_Type; // ��Ŷ Ÿ��
	int Packet_Result; // ó�� ��� ��
	char Game_Server[32]; // ���� �������� ���� ID
	char Session[32]; // GUID, user index�� ����ص� ��
	char User_CC[4]; // ���� �ڵ�(company code)
	char User_No[20]; // ����� ��ȣ�� �ֹι�ȣ
	char User_ID[40]; // ����� ���̵�
	char User_IP[24]; // ����� ���� ������
	char User_Gender[1]; // ����� ����. M: ����, W/F: ����, C: ����
	char User_Status[3]; // ����� ����. HO: ����, PC: ���ӹ�, SP: ISP, PM: �����̾�
	char User_PayType[4]; // ���� ����
	int User_Age; // ����� ����
	int Game_No; // ���� ��ȣ(���� DB�� ��ϵ� ��ȣ)
	char Bill_PayType[2]; // ���� ��� A0: �ĺ�, D0: ����, P0: ����
	char Bill_Method[2]; // ���� ���. FM/FW: ������/�ָ� ������, FD/DB/HB: �� ����/�Ⱓ ����, TX: ���� ����, TB: ����
	char Bill_Expire[12]; // ���� ������ YYMMDD
	int Bill_Remain; // ���� �ð� �� ����Ʈ
};

struct MSG_PACKET_GAME : public MSGBASE
{
	_PACKET_GAME mPacketGame;
};

#pragma pack(pop)