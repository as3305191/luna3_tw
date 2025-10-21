#include "stdafx.h"
#include "ServerSystem.h"
#include "Console.h"
#include "Network.h"
#include "DataBase.h"
#include "AgentNetworkMsgParser.h"

#include "Usertable.h"
#include "Servertable.h"

#include <MemoryPoolTempl.h>

#include "AgentDBMsgParser.h"
#include "Crypt.h"

#include "FilteringTable.h"
#include "MHTimeManager.h"
#include "BootManager.h"
#include "MHFile.h"
#include "AgentDBMsgParser.h"
#include "TrafficLog.h"
#include "GMPowerList.h"
#include "PlustimeMgr.h"
#include "..\[CC]Header\Pool.h"
#include "..\[CC]ServerModule\MiniDumper.h"
#include "../hseos/Family/SHFamilyManager.h"
#include "../hseos/Debug/SHDebug.h"
#include "PunishManager.h"
#include "./SiegeRecallMgr.h"
#include "./NpcRecallMgr.h"
#include ".\giftmanager.h"

#ifdef _HACK_SHIELD_
#include "HackShieldManager.h"
#endif

#ifdef _NPROTECT_
#include "ggsrv25.h"
#include "NProtectManager.h"
#endif

LPCTSTR g_SERVER_VERSION = "LUNA2025";

void __stdcall ProcessServer(DWORD eventIndex);
void __stdcall ProcessGame(DWORD eventIndex);
void __stdcall ProcessCheck(DWORD eventIndex);

DWORD g_dwAcceptUser = 0;
BOOL g_bCloseWindow = FALSE;
extern DWORD g_dwAcceptUser2;
extern int	g_nServerSetNum;
HWND g_hWnd;
CServerSystem * g_pServerSystem = 0;

CPool< USERINFO > g_UserInfoPool;
#ifdef _NPROTECT_
CPool<CCSAuth2> g_CCSAuth2Pool;
#endif

typedef void (*MSGPARSER)(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
MSGPARSER g_pServerMsgParser[MP_MAX] = {0};
MSGPARSER g_pUserMsgParser[MP_MAX] = {0};

CServerSystem::CServerSystem()
{
	CoInitialize(NULL);
#ifdef _CRYPTCHECK_ 
	m_bUseCrypt = TRUE;
#else
	m_bUseCrypt = FALSE;
#endif

	m_Nation = eNATION_KOREA;
	m_bUseEventNotify = FALSE;
	ZeroMemory( m_strEventNotifyTitle, sizeof( m_strEventNotifyTitle ) );
	ZeroMemory( m_strEventNotifyContext, sizeof( m_strEventNotifyContext ) );
	memset( m_bEvenList, 0, sizeof(BOOL)*eEvent_Max );
	m_bTestServer = FALSE;

	// 100301 ShinJS --- ĳ���͵��� �ʱ� ���� ����
	LoadCharBaseStatusInfo(); 
}

CServerSystem::~CServerSystem()
{
	CoUninitialize();
}

void CServerSystem::Start(WORD ServerNum)
{

//�ӽ� ����
	SetNation();

	//---KES �׽�Ʈ �������� Ȯ��
	CMHFile file2;
	if( file2.Init( "_TESTSERVER", "rt", MHFILE_FLAG_DONOTDISPLAY_NOTFOUNDERROR ) )
	{
		m_bTestServer = TRUE;
		file2.Release();
	}
	//--------------------------

	m_wServerNum = ServerNum;
	srand(GetTickCount());

	//SW050729

	// 071115 ����, ���� ��� ����
	//JACKPOTMGR->Start();

//	m_IdxCreater = ICCreate();
//	ICInitialize(m_IdxCreater,5000);
	
	//////////////////////////////////////////////////////////////////////////
	// �ܼ� �ʱ�ȭ
	// ��ư ����.�ܺ��Է¿�E��ư�� 28�� �غ�Ǿ��ִ�.�Ѱ��� �翁E���E���� �濁EpCustomButton �ʵ带 NULL�� ä����.
	MENU_CUSTOM_INFO pMenu[3];
	strcpy(pMenu[0].szMenuName, "Button1");
	pMenu[0].cbMenuCommand = ButtonProc1;
	strcpy(pMenu[1].szMenuName, "Button2");
	pMenu[1].cbMenuCommand = ButtonProc2;
	strcpy(pMenu[2].szMenuName, "Button3");
	pMenu[2].cbMenuCommand = ButtonProc3;

	if(!g_Console.Init(3,pMenu,OnCommand))
		MessageBox(NULL,"Console Initializing Failed",0,0);

	char TitleText[255];
	sprintf(TitleText, "Agent Server: %d", ServerNum);

	g_hWnd = GetActiveWindow();
	SetWindowText(g_hWnd, TitleText);
	
//	DWORD    dwProcessID = NULL;
  //  DWORD    dwTreadID = ::GetWindowThreadProcessId(g_hWnd, &dwProcessID );

//	HOOKPROC hProc;
//    hProc = CallWndProc;
//    hHook = ::SetWindowsHookEx(WH_CALLWNDPROC, hProc, (HINSTANCE)NULL, dwTreadID);


	SetMapChangeInfo();
	SetLoginInfo();
	//////////////////////////////////////////////////////////////////////////
	// ��Ʈ��ũ ����

	/// ��Ʈ��ũ �ļ� ����-------------------------------
	g_pServerMsgParser[0] = NULL;	// YH 0���� ��E? MP_SERVER == 1
	int i;
	for(i=1 ; i<MP_MAX ; ++i)
		g_pServerMsgParser[i] = TransToClientMsgParser;
	g_pServerMsgParser[MP_SERVER] = MP_AGENTSERVERMsgParser;
	g_pServerMsgParser[MP_POWERUP] = MP_POWERUPMsgParser;
	g_pServerMsgParser[MP_ITEM] = MP_ITEMServerMsgParser;
	g_pServerMsgParser[MP_CHAT] = MP_CHATServerMsgParser;
	g_pServerMsgParser[MP_USERCONN] = MP_USERCONNServerMsgParser;
	g_pServerMsgParser[MP_PACKEDDATA] = MP_PACKEDMsgParser;
	g_pServerMsgParser[MP_PARTY] = MP_PARTYServerMsgParser;
	g_pServerMsgParser[MP_FRIEND] = MP_FRIENDMsgParser;
	g_pUserMsgParser[MP_NOTE] = MP_NOTEMsgParser;
	g_pServerMsgParser[MP_NOTE] = MP_NOTEServerMsgParser;
	g_pServerMsgParser[MP_MORNITORMAPSERVER] = MP_MonitorMsgParser;
	g_pServerMsgParser[MP_CHEAT] = MP_CHEATServerMsgParser;
	g_pServerMsgParser[MP_HACKCHECK] = MP_HACKCHECKMsgParser;
	g_pServerMsgParser[MP_GUILD] = MP_GUILDServerMsgParser;
	//g_pServerMsgParser[MP_FAMILY] = MP_GUILDServerMsgParser;
	g_pServerMsgParser[MP_GUILD_WAR] = MP_GUILD_WARServerMsgParser;	

	// 080513 KTH -- npc message parser
	// 090227 ShinJS --- �ϻ��� �߰��۾� : �̵�NPCó���� ���� ���
	g_pServerMsgParser[MP_NPC] = MP_NPCMsgParser;

	// 071115 ����, ���� ��� ����
	//g_pServerMsgParser[MP_JACKPOT] = MP_JACKPOTServerMsgParser;
	g_pServerMsgParser[MP_SKILL] = MP_SkillServerMsgParser;
	g_pServerMsgParser[MP_GUILD_UNION] = MP_GUILD_UNIONServerMsgParser;
	g_pServerMsgParser[MP_GTOURNAMENT] = MP_GTOURNAMENTServerMsgParser;

	g_pServerMsgParser[MP_SIEGEWARFARE] = MP_SIEGEWARFAREServerMsgParser;

	// S �ֹε�� �߰� added by hseos 2007.06.06
	g_pServerMsgParser[MP_RESIDENTREGIST]	=	MP_RESIDENTREGIST_ServerMsgParser;
	// E �ֹε�� �߰� added by hseos 2007.06.06

	// desc_hseos_�йи�01
	// S �йи� �߰� added by hseos 2007.07.11
	g_pServerMsgParser[MP_FAMILY]			=	MP_FAMILY_ServerMsgParser;
	// E �йи� �߰� added by hseos 2007.07.11

	// desc_hseos_����ý���_01
	// S ����ý��� �߰� added by hseos 2007.09.10
	g_pServerMsgParser[MP_FARM]				=	MP_FARM_ServerMsgParser;
	// E ����ý��� �߰� added by hseos 2007.09.10

	// desc_hseos_����Ʈ ��_01
	// S ����Ʈ �� �߰� added by hseos 2007.11.16
	g_pServerMsgParser[MP_DATE]				=	MP_DATE_ServerMsgParser;
	g_pServerMsgParser[MP_AUTONOTE] = MP_AUTONOTE_ServerMsgParser;
	//---------------

	//---KES PUNISH
	g_pServerMsgParser[MP_PUNISH] = MP_PUNISH_ServerMsgParser;
	//---------------

	// 080407 LYW --- ServerSystem : Add a server message parser for chatroom system.
	g_pServerMsgParser[MP_CHATROOM] = MP_CHATROOM_ServerMsgParser ;

	// 080327 NYJ --- ���ýý���
	g_pServerMsgParser[MP_FISHING] = MP_FISHING_ServerMsgParser;
  	g_pServerMsgParser[MP_SIEGERECALL] = MP_SIEGERECALL_ServerMsgParser ;
  
  	// 080819 LYW --- ServerSystem : npc ��ȯ ���� ���� �޽����� �Ľ��ϴ� �Լ� �߰�.
  	g_pServerMsgParser[MP_RECALLNPC] = MP_RECALLNPC_ServerMsgParser ;
	g_pServerMsgParser[MP_COOK] = MP_COOKING_ServerMsgParser;
	g_pServerMsgParser[MP_HOUSE] = MP_HOUSING_ServerMsgParser;

#ifdef _NPROTECT_
	g_pServerMsgParser[MP_NPROTECT] = MP_NPROTECTUserMsgParser;
#endif

	for(i=1 ; i<MP_MAX ; ++i)
		g_pUserMsgParser[i] = TransToMapServerMsgParser;

	//---KES ��ŷ ��� ����
	g_pUserMsgParser[MP_EXCHANGE] = MP_TEMPMsgParser;
	g_pUserMsgParser[MP_STREETSTALL] = MP_TEMPMsgParser;
	//---------------------	

	g_pUserMsgParser[MP_USERCONN] = MP_USERCONNUserMsgParser;
	g_pUserMsgParser[MP_CHAT] = MP_CHATMsgParser;
	g_pUserMsgParser[MP_FRIEND] = MP_FRIENDMsgParser;
	g_pUserMsgParser[MP_NOTE] = MP_NOTEMsgParser;
	g_pUserMsgParser[MP_CHEAT] = MP_CHEATUserMsgParser;
	g_pUserMsgParser[MP_HACKCHECK] = MP_HACKCHECKMsgParser;
	g_pUserMsgParser[MP_GUILD] = MP_GUILDUserMsgParser;
	//g_pUserMsgParser[MP_FAMILY] = MP_GUILDUserMsgParser;
	g_pUserMsgParser[MP_GUILD_WAR] = MP_GUILD_WARUserMsgParser;
	g_pUserMsgParser[MP_ITEM] = MP_ITEMUserMsgParser;
	g_pUserMsgParser[MP_GTOURNAMENT] = MP_GTOURNAMENTUserMsgParser;
	g_pUserMsgParser[MP_GUILD_UNION] = MP_GUILD_UNIONUserMsgParser;
 	g_pUserMsgParser[MP_SIEGEWARFARE] = MP_SIEGEWARFAREUserMsgParser;
	// desc_hseos_�ֹε��01
	// S �ֹε�� �߰� added by hseos 2007.06.10
	g_pUserMsgParser[MP_RESIDENTREGIST]	=	MP_RESIDENTREGIST_UserMsgParser;
	g_pUserMsgParser[MP_FAMILY]	=	MP_FAMILY_UserMsgParser;
	g_pUserMsgParser[MP_FARM]	=	MP_FARM_UserMsgParser;
	g_pUserMsgParser[MP_DATE]	=	MP_DATE_UserMsgParser;
	g_pUserMsgParser[MP_AUTONOTE] = MP_AUTONOTE_UserMsgParser;
	//---------------

	// 080407 LYW --- ServerSystem : Add a user message parser for chatroom system.
	g_pUserMsgParser[MP_CHATROOM] = MP_CHATROOM_UserMsgParser ;
	
	// 080327 NYJ --- ���ýý���
	g_pUserMsgParser[MP_FISHING] = MP_FISHING_UserMsgParser;

	// 080728 LYW --- ServerSystem : Ŭ���̾�Ʈ�� ������ ������ ��ȯ�� ���� �޽����� �Ľ��ϴ� �Լ� �߰�.
  	g_pUserMsgParser[MP_SIEGERECALL] = MP_SIEGERECALL_UserMsgParser ;
  
  	// 080819 LYW --- ServerSystem : Ŭ���̾�Ʈ�� ������ npc ��ȯ �޽��� ó���� �Ľ��ϴ� �Լ� �߰�.
  	g_pUserMsgParser[MP_RECALLNPC] = MP_RECALLNPC_UserMsgParser ;
	g_pUserMsgParser[MP_COOK] = MP_COOKING_UserMsgParser;
	g_pUserMsgParser[MP_HOUSE] = MP_HOUSING_UserMsgParser;
	g_pUserMsgParser[MP_DUNGEON] = MP_DUNGEON_UserMsgParser;
#ifdef _HACK_SHIELD_
	g_pUserMsgParser[MP_HACKSHIELD] = MP_HACKSHIELDUserMsgParser;
#endif
#ifdef _NPROTECT_
	g_pUserMsgParser[MP_NPROTECT] = MP_NPROTECTUserMsgParser;
#endif
	
	// 090526 ShinJS --- ��Ƽ �޼��� �Լ� �߰�
	g_pUserMsgParser[MP_PARTY] = MP_PARTYUserMsgParser;

	// servertable usertable �ʱ�ȭ ---------------------
	
	g_UserInfoPool.Init(MAX_USER_IN_A_AGENT/2, 500,"UserInfoPool");
#ifdef _NPROTECT_
	g_CCSAuth2Pool.Init(MAX_USER_IN_A_AGENT/2, 500,"CCSAuth2Pool");
#endif
	g_pUserTable = new CUserTable;					// KEY : dwConnectionIndex
	g_pUserTable->Init(MAX_USER_IN_A_AGENT/2, 0);
	g_pUserTableForUserID = new CUserTable;			// KEY : dwUserIdx
	g_pUserTableForUserID->Init(MAX_USER_IN_A_AGENT/2, 0);
	g_pUserTableForObjectID = new CUserTable;		// KEY : dwObjectID
	g_pUserTableForObjectID->Init(MAX_USER_IN_A_AGENT/2, 0);
	g_pServerTable = new CServerTable;				// KEY : ServerPort
	g_pServerTable->Init(MAX_SERVER_CONNECTION);

	CUSTOM_EVENT customEvent[] = {
		{10, ProcessServer},
		{100, ProcessGame},
		{1000 * 30, ProcessCheck},
	};

	DESC_NETWORK desc = {0};
	desc.OnAcceptServer = OnAcceptServer;
	desc.OnDisconnectServer = OnDisconnectServer;
	desc.OnAcceptUser = OnAcceptUser;
	desc.OnDisconnectUser = OnDisconnectUser;
	desc.OnRecvFromServerTCP = ReceivedMsgFromServer;
	desc.OnRecvFromUserTCP = ReceivedMsgFromUser;
	desc.dwCustomDefineEventNum	= sizeof(customEvent) / sizeof(*customEvent);
	desc.pEvent = customEvent;
	desc.dwMainMsgQueMaxBufferSize = 20480000;			//max 20480000
	desc.dwMaxServerNum = MAX_SERVER_CONNECTION;		//max 3000
	desc.dwMaxUserNum = MAX_USER_IN_A_AGENT;			//max 3000
	desc.dwServerBufferSizePerConnection = 512000;		//max 512000
	desc.dwServerMaxTransferSize = 65535;				//max 65535
	desc.dwUserBufferSizePerConnection = 256000;		//max 512000
	desc.dwUserMaxTransferSize = 65535;					//max 65535
	desc.dwConnectNumAtSameTime = MAX_USER_IN_A_AGENT/4;	//about divide by 4
	desc.dwFlag = 0;

	g_Network.Init(&desc);


	// 080703 LYW --- ServerSystem : ���� �ܼ�â�� ó������ ���� �������� ����Ѵ�.
	g_Console.LOG(4, "��") ;
	g_Console.LOG(4, "[ Server Version : %s ]", g_SERVER_VERSION) ;
	g_Console.LOG(4, "��") ;

	BOOTMNGR->AddSelfBootList(AGENT_SERVER, ServerNum, g_pServerTable);
	if(!BOOTMNGR->StartServer(&g_Network, g_pServerTable->GetSelfServer()))
	{
        ASSERT(0);
	}
	if(!BOOTMNGR->ConnectToMS(&g_Network, g_pServerTable))
	{
		SERVERINFO info;
		info.wServerKind = MONITOR_SERVER;
		OnConnectServerFail(&info);		
	}

	//////////////////////////////////////////////////////////////////////////
	// DB ����
	DWORD maxthread = 2;
	DWORD maxqueryinsametime = 1024;
	FILE* fp = fopen("AgentDBInfo.txt","r");
	if(fp)
	{
		fscanf(fp,"%d %d",&maxthread,&maxqueryinsametime);
		fclose(fp);
	}
	if(g_DB.Init(maxthread,maxqueryinsametime,TRUE) == FALSE)
		MessageBox(NULL,"DataBase Initializing Failed",0,0);

	GMINFO->Init();
	PLUSTIMEMGR->Init();
	//TRAFFIC->StartRecord();
	TRAFFIC->Init();
	PUNISHMGR->Init();
//-------------

	GIFTMGR->Init();

	// 080724 LYW --- Serversystem : Load recall info of siege war.
   	SIEGERECALLMGR->Initialize() ;
  
	// Dump
	MiniDumper md(MiniDumper::DUMP_LEVEL_0);
	
	g_Console.LOG(4, "-----------   AGENT SERVER START  -----------------");
	// �� �κ��� �������� �޽��� ����.CONSOLE�� ����ϵ� ������ �޽��� ������� ¥�� ������ε� �̺�Ʈ�� ����ؾ���
	// �ϹǷ� ������ �޽��� ���� �ۼ��� �ϰ� ���� �ʴٸ� �� �޼ҵ带 ����Ѵ�.�� ��� ���� �޽��� ������ ������ ����� 
	// �����Ѵ�.�� �޼ҵ�� I4DyuchiCONSOLE ���̾�α��� x��ư�� ���������� �������� �ʴ´�.�� �޼ҵ尡 �����Ѵٸ� ��
	// �α׷��� �����ϴ� ���̴�.
	
	g_bReady = TRUE;

//Get GMPowerList
//	GM_GetGMPowerList( 0, 0 );
	

	//������Ʈ�� ų �� �� ������Ʈ�� �ִ� ������ �α��� ���̺����� �����.
	char txt[128];
	// 081012 LUJ, �Ϻ� ������ ���ν��� ȣ�� �� ��ҹ��ڱ��� ��ġ�ؾ� �Ѵ�. ��, dbo�� �ҹ��ڷ� �ؾ���. �̿� ��� ���ν��� �̸��� ������
	sprintf(txt, "EXEC UP_AGENT_DOWN %d, %d", g_nServerSetNum , g_pServerTable->GetSelfServer()->wServerNum);
	g_DB.LoginQuery(eQueryType_FreeQuery, eAgentDown, 0, txt);

	g_Console.WaitMessage();
}
void CServerSystem::Process()
{	// YH ���� 1�ʸ��� �ѹ��� ����	
	// KES ���� 0.1���ٷ� �ٲ� 050113

	if(g_bCloseWindow)
	{
		SendMessage(g_hWnd, WM_CLOSE, 0,0);
		g_bCloseWindow = FALSE;
		g_bReady = FALSE;
	}

	MHTIMEMGR_OBJ->Process();

	//---KES DB Process Fix 071114
	//g_DB.ProcessingQueue();	//db que������ �׻� ���ɴ�.
	//---------------------------

	TRAFFIC->Process();

	//SHOUTMGR->Process();	// 080317 NYJ --- ShoutMgr Ŭ������ ������� ����.
	
	//SW050729

	// 071115 ����, ���� ��� ����
	// JACKPOTMGR->Process();

	// desc_hseos_�йи�01
	// S �йи� �߰� added by hseos 2007.07.15
	g_csFamilyManager.Process();
	// E �йи� �߰� added by hseos 2007.07.15

//---KES PUNISH
	PUNISHMGR->Process();
//-------------

	GIFTMGR->Process();

#ifdef _NPROTECT_
	NPROTECTMGR->Update();
#endif
}

void CServerSystem::ConnectionCheck()
{	// YH ����E30�ʸ��� �ѹ��� �����
	DWORD _60sec = 60*1000;
	USERINFO* pInfo;
	DWORD elapsedtime;
	static int nForBill = 0;

	if(g_bReady == FALSE)
		return;
	
	cPtrList removelist;
	cPtrList disconnectlist;

	//---KES Network Process Fix 071114
//	g_pUserTableForUserID->SetPositionHead();
//	while(pInfo = g_pUserTableForUserID->GetData())
	g_pUserTable->SetPositionUserHead();
	while( (pInfo = g_pUserTable->GetData()) != NULL )
	{
		if(pInfo->dwConnectionIndex == 0)
		{
			// ���� ������ ����� �̷����� ���� ���
			elapsedtime = gCurTime - pInfo->dwLastConnectionCheckTime;
			if(elapsedtime > _60sec*2)
			{
				removelist.AddTail(pInfo);
			}
		}
		else
		{
			DWORD logintime = gCurTime - pInfo->mLoginCheckTime;

			if( pInfo->mLoginCheck )
			if( logintime > ( 60 * 60 * 1000 ) )
			{
				pInfo->mLoginCheck = FALSE;
			}
			//1. �� �ε�(���̵�)���� �ð��� ��� ó���ϳ�?
			//2. ĳ���� ��åâ�� �ð��� ��� ó���ϳ�?

			//if( nForBill == 1 )
			if(pInfo->UserLevel > eUSERLEVEL_GM)
				SendSpeedHackCheckMsg(pInfo);

			if(pInfo->UserLevel == eUSERLEVEL_PROGRAMMER)
				continue;

			elapsedtime = gCurTime - pInfo->dwLastConnectionCheckTime;
			
			if( elapsedtime > _60sec*5 )	//5min
			{
				// ������ �̷��� ���
				if(pInfo->bConnectionCheckFailed)		// ���� ���ؼ� üũ�� ���´µ� 1�е��� ������ �����ٸ�
				{
					disconnectlist.AddTail(pInfo);
					//DisconnectUser(pInfo->dwConnectionIndex, 1);
					continue;
				}
				else
				{
					pInfo->bConnectionCheckFailed = TRUE;
					SendConnectionCheckMsg(pInfo);
					pInfo->dwLastConnectionCheckTime = gCurTime;
				}
			}
		}
	}

	PTRLISTPOS pos = removelist.GetHeadPosition();
	while( pos )
	{
		USERINFO* p = (USERINFO*)removelist.GetNext(pos);

		LoginCheckDelete(p->dwUserID);		// �α���üũ ���̺����� ����
		g_pUserTableForUserID->RemoveUser(p->dwUserID);
		TRAFFIC->RemoveUser(p->dwUserID);
#ifdef _NPROTECT_
		if( p->m_pCSA )
		{
			p->m_pCSA->Close();
			g_CCSAuth2Pool.Free(p->m_pCSA);
		}
#endif
		memset(p,0,sizeof(USERINFO));
		g_UserInfoPool.Free(p);
	}
	removelist.RemoveAll();

	pos = disconnectlist.GetHeadPosition();
	while( pos )
	{
		USERINFO* p = (USERINFO*)disconnectlist.GetNext(pos);

		DWORD dwConIdx = p->dwConnectionIndex;
		OnDisconnectUser( dwConIdx );
		DisconnectUser( dwConIdx );
	}
	disconnectlist.RemoveAll();


	if( nForBill == 1 )		//2���� �ѹ�... ��, 1�и���.
	{
		nForBill = 0;
	}
	else
	{
		++nForBill;
	}
	
	// Plustime
	PLUSTIMEMGR->Process();
}

void CServerSystem::SendConnectionCheckMsg(USERINFO* pInfo)
{
	MSGBASE msg;
	msg.Category = MP_USERCONN;
	msg.Protocol = MP_USERCONN_CONNECTION_CHECK;
	g_Network.Send2User(pInfo->dwConnectionIndex,(char*)&msg,sizeof(msg));

/*
	MSG_DWORD msg;
	msg.Category	= MP_USERCONN;
	msg.Protocol	= MP_USERCONN_CONNECTION_CHECK;
	msg.dwData		= gCurTime;
	g_Network.Send2User(pInfo->dwConnectionIndex,(char*)&msg,sizeof(msg));
*/
}

void CServerSystem::SendSpeedHackCheckMsg(USERINFO* pInfo)
{
	if( pInfo->wUserMapNum )
	{
		MSG_DWORD msg;
		msg.Category	= MP_HACKCHECK;
		msg.Protocol	= MP_HACKCHECK_SPEEDHACK;
		msg.dwData		= gCurTime;	//���� �ð��� �ִ� ���� ������.
		g_Network.Send2User(pInfo->dwConnectionIndex,(char*)&msg,sizeof(msg));
	}

#ifdef _HACK_SHIELD_
	HACKSHIELDMGR->SendReq(pInfo);
#endif
#ifdef _NPROTECT_

	if( pInfo->m_nCSAInit == 1 || pInfo->m_nCSAInit == 2 ) //�α��ν� ó�� 2�� ���� ��
	{
		if( gCurTime - pInfo->dwLastNProtectCheck >= 1000*60 ) //60�ʳ��� ���� ���� �ȿ���
		{
			pInfo->m_nCSAInit = 4;	//���� ����.

			MSGBASE msg;
			msg.Category = MP_NPROTECT;
			msg.Protocol = MP_NPROTECT_DISCONNECT;
			g_Network.Send2User(pInfo->dwConnectionIndex,(char*)&msg,sizeof(msg));
			if( pInfo->UserLevel >= eUSERLEVEL_GM )				//---KES NProtect GM�̻��� ���� ���� ���Ѵ�
			{
				DWORD dwConIdx = pInfo->dwConnectionIndex;
				OnDisconnectUser( dwConIdx );
				DisconnectUser( dwConIdx );
			}
		}
	}
	else if( pInfo->m_nCSAInit == 3 )
	{
		if(gCurTime - pInfo->dwLastNProtectCheck >= 1000*60*3)
		{
			NPROTECTMGR->SendAuthQuery(pInfo);
		}
	}

#endif
}

void CServerSystem::End()
{
	g_bReady = FALSE;

	g_Network.Release();			// ���� ���� ����� �� �ڵ� ���Ŀ� ���;� �Ѵ�. - 2007/04/11 ������.

	//TRAFFIC->EndRecord();
	TRAFFIC->End();

//	UnhookWindowsHookEx(hHook);

//---KES PUNISH
	PUNISHMGR->Release();
//-------------
	
	g_pUserTableForUserID->SetPositionHead();
	USERINFO* pInfo;
	while( (pInfo = g_pUserTableForUserID->GetData()) != NULL )
	{
		LoginCheckDelete(pInfo->dwUserID);
		g_pServerSystem->ReleaseAuthKey(pInfo->dwUniqueConnectIdx);
#ifdef _NPROTECT_
		if( pInfo->m_pCSA )
		{
			pInfo->m_pCSA->Close();
			g_CCSAuth2Pool.Free(pInfo->m_pCSA);
		}
#endif
		memset(pInfo, 0, sizeof(USERINFO));
		g_UserInfoPool.Free(pInfo);
	}
	g_pUserTableForUserID->RemoveAll();

//	ICRelease(m_IdxCreater);
	
	g_DB.Release();

	g_Console.Release();

	if(g_pUserTable)
	{
		g_pUserTable->RemoveAll();
		delete g_pUserTable;
		g_pUserTable = NULL;
	}
	if(g_pUserTableForObjectID)
	{
		g_pUserTableForObjectID->RemoveAll();
		delete g_pUserTableForObjectID;
		g_pUserTableForObjectID = NULL;
	}
	if(g_pUserTableForUserID)
	{
		g_pUserTableForUserID->RemoveAll();
		delete g_pUserTableForUserID;
		g_pUserTableForUserID = NULL;
	}
	if(g_pServerTable)
	{
		g_pServerTable->RemoveAllServer();
		delete g_pServerTable;
		g_pServerTable = NULL;
	}
	CoFreeUnusedLibraries();
}

void CServerSystem::SetMapChangeInfo()
{
	char filename[64];
	CMHFile file;

	sprintf(filename,"./System/Resource/MapMoveList.bin");
	if(!file.Init(filename, "rb"))
		return;

	int n =0;

	while(FALSE == file.IsEOF())
	{
		m_MapChange[n].Kind = file.GetWord();
//		file.GetString(m_MapChange[n].CurMapName);
//		file.GetString(m_MapChange[n].ObjectName);
		SafeStrCpy( m_MapChange[n].CurMapName, file.GetString(), MAX_NAME_LENGTH+1 );
		SafeStrCpy( m_MapChange[n].ObjectName, file.GetString(), MAX_NAME_LENGTH+1 );
		m_MapChange[n].CurMapNum = file.GetWord();
		m_MapChange[n].MoveMapNum = file.GetWord();
		m_MapChange[n].CurPoint.x = file.GetFloat();
		m_MapChange[n].CurPoint.y = 0;
		m_MapChange[n].CurPoint.z = file.GetFloat();
		m_MapChange[n].MovePoint.x = file.GetFloat();
		m_MapChange[n].MovePoint.y = 0;
		m_MapChange[n].MovePoint.z = file.GetFloat();
		m_MapChange[n].chx_num = file.GetWord();
		
		++n;
	}
	
	file.Release();

	// �� ���� ä�� �б�.
	bool isFound	= false;
	char buf[256];
	int nMapNum;
	sprintf(filename,"system/MapChannel.bin");
	ZeroMemory(m_MapChannelNum, sizeof(m_MapChannelNum));

	if( file.Init( filename,"rb") )
	{
		while( ! file.IsEOF() )
		{
			file.GetString( buf );

			if( strcmp( buf, "$MAP_CHANNEL" ) == 0)
			{
				nMapNum = file.GetWord();
				m_MapChannelNum[nMapNum] = file.GetWord();
				isFound = true;
			}
		}
	}
	file.Release();

	if( false == isFound )
	{
		char buf[ 80 ];
		sprintf( buf, "���� : %s �б� ����!", filename );
		ASSERTMSG( isFound, buf );
	}
}

MAPCHANGE_INFO* CServerSystem::GetMapChangeInfo(DWORD Index)
{
	// 081020 LYW --- ServerSystem : MapMoveList.bin���� �߰��� ������ 100�� �̻� �Ѿ��
	// ���̵� �ȵǴ� ���� ����.
	//for(int i=0; i<MAX_MAP_NUM; i++)
	for(int i=0; i<MAX_POINT_NUM; i++)
	{
		if(m_MapChange[i].Kind == Index)
			return &m_MapChange[i];
	}

	return NULL;
}

// desc_hseos_����Ʈ ��_01
// S ����Ʈ �� �߰� added by hseos 2007.11.29
UINT CServerSystem::GetMapChangeIndex(UINT nMapNum)
{
	for(int i=0; i<MAX_MAP_NUM; i++)
	{
		if(m_MapChange[i].MoveMapNum == nMapNum)
			return m_MapChange[i].Kind;
	}

	return NULL;
}
// E ����Ʈ �� �߰� added by hseos 2007.11.29

void CServerSystem::SetLoginInfo()
{
	char filename[64];
	CMHFile file;
	sprintf(filename,"System/Resource/LoginPoint.bin");	
	if(!file.Init(filename, "rb"))
		return;

	int n =0;
	while(1)
	{
		if(file.IsEOF())
			break;
		m_Login[n].Kind = file.GetWord();
		//file.GetString(pLogin[n].MapName);
		SafeStrCpy( m_Login[n].MapName, file.GetString(), MAX_NAME_LENGTH+1 );
		m_Login[n].MapNum = file.GetWord();
		m_Login[n].MapLoginPointNum = file.GetByte();
		if(m_Login[n].MapLoginPointNum == 0)
		{
			MessageBox(NULL,"LoginPointNum is Zero!!!",0,0);
		}
		if(m_Login[n].MapLoginPointNum > 10)
		{
			MessageBox(NULL,"LoginPointNum is too big!!!",0,0);
		}
		for(int i=0;i<m_Login[n].MapLoginPointNum;++i)
		{
			m_Login[n].CurPoint[i].x = file.GetFloat();
			m_Login[n].CurPoint[i].y = 0;
			m_Login[n].CurPoint[i].z = file.GetFloat();
		}
		m_Login[n].chx_num = file.GetWord();
		++n;
	}

	file.Release();
}

LOGINPOINT_INFO* CServerSystem::GetLoginInfo(DWORD Index)
{
	for( int i = 0; i < MAX_POINT_NUM; i++)
	{
		if(m_Login[i].Kind == Index)
			return &m_Login[i];
	}
	return NULL;
}

//�ʼ��� �ٿ�� �ٿ�� �ʿ� �ִ� �������� �ʴٿ��� �˸�
void CServerSystem::SendMsgMapServerDown( WORD wServerNum )
{
	USERINFO* pInfo;
	g_pUserTable->SetPositionUserHead();

	while( (pInfo = g_pUserTable->GetUserData()) != NULL )
	{
		if( pInfo->wUserMapNum == wServerNum )
		{
			const DWORD connectionIndex = pInfo->dwConnectionIndex;
			const DWORD userIndex = pInfo->dwUserID;

			OnDisconnectUser(
				connectionIndex);
			DisconnectUser(
				connectionIndex);
			LoginCheckDelete(
				userIndex);
		}
	}
}



DWORD CServerSystem::MakeAuthKey()
{
	//return ICAllocIndex(m_IdxCreater) + 1;
	static DWORD ID = 1;//GetTickCount()*GetTickCount();
	if(ID == 0)
		++ID;
	return ID++;
}
void CServerSystem::ReleaseAuthKey(DWORD key)
{
	//ICFreeIndex(m_IdxCreater,key-1);
}

void CServerSystem::SetNation()
{
	CMHFile file;
	if( !file.Init( "LocalizingInfo.txt", "rt", MHFILE_FLAG_DONOTDISPLAY_NOTFOUNDERROR ) )
		return;

	if( strcmp( file.GetString(), "*NATION" ) == 0 )
	{
		if( strcmp( file.GetString(), "CHINA" ) == 0 )
		{
			m_Nation = eNATION_CHINA;
		}
	}

	file.Release();
}


void CServerSystem::SetEventNotifyStr( char* pStrTitle, char* pStrContext )
{
	SafeStrCpy( m_strEventNotifyTitle, pStrTitle, 32 );
	SafeStrCpy( m_strEventNotifyContext, pStrContext, 128 );
}

void CServerSystem::SetUseEventNotify( BOOL bUse )
{
	m_bUseEventNotify = bUse;

	if( bUse )
	{
		USERINFO* pInfo;
		g_pUserTable->SetPositionUserHead();

		MSG_EVENTNOTIFY_ON msg;
		msg.Category	= MP_CHEAT;
		msg.Protocol	= MP_CHEAT_EVENTNOTIFY_ON;
		SafeStrCpy( msg.strTitle, m_strEventNotifyTitle, 32 );
		SafeStrCpy( msg.strContext, m_strEventNotifyContext, 128 );

		for( int i=0; i<eEvent_Max; ++i )
		{
			if( m_bEvenList[i] )				msg.EventList[i] = 1;
			else								msg.EventList[i] = 0;
		}

		while( (pInfo = g_pUserTable->GetUserData()) != NULL )
		{
			MSG_EVENTNOTIFY_ON msgTemp = msg;
			g_Network.Send2User( pInfo->dwConnectionIndex, (char*)&msgTemp, sizeof(msgTemp) );
		}
	}
	else
	{
		MSGBASE msg;
		msg.Category	= MP_CHEAT;
		msg.Protocol	= MP_CHEAT_EVENTNOTIFY_OFF;

		USERINFO* pInfo;
		g_pUserTable->SetPositionUserHead();

		while( (pInfo = g_pUserTable->GetUserData()) != NULL )
		{
			MSGBASE msgTemp = msg;
			g_Network.Send2User( pInfo->dwConnectionIndex, (char*)&msgTemp, sizeof(msgTemp) );
		}
	}
}

void CServerSystem::LoadCharBaseStatusInfo()
{
	// GameResourceManager�� ��ġ�ؾ���.
	enum Block
	{
		BlockNone,
		BlockRace,
		BlockClass,
		BlockStatus,
	}
	blockType = BlockNone;

	DWORD dwOpenCnt = 0;
	BOOL bIsComment = FALSE;

	CharBaseStatusInfoKey curStatusKey;

	CMHFile file;
	file.Init( "./System/Resource/CharacterBaseStatus.bin", "rb" );

	while(FALSE == file.IsEOF())
	{
		char txt[ MAX_PATH ] = {0,};
		file.GetLine( txt, MAX_PATH );

		int txtLen = _tcslen( txt );

		// �߰� �ּ� ����
		for( int i=0 ; i<txtLen-1 ; ++i )
		{
			if( txt[i] == '/' && txt[i+1] == '/' )
			{
				txt[i] = 0;
				break;
			}
			else if( txt[i] == '/' && txt[i+1] == '*' )
			{
				txt[i] = 0;
				bIsComment = TRUE;
			}
			else if( txt[i] == '*' && txt[i+1] == '/' )
			{
				txt[i] = ' ';
				txt[i+1] = ' ';
				bIsComment = FALSE;
			}
			else if( bIsComment )
			{
				txt[i] = ' ';
			}
		}

		char buff[ MAX_PATH ] = {0,};
		SafeStrCpy( buff, txt, MAX_PATH );
		char* delimit = " \n\t=";
		char* markBegin = "{";
		char* markEnd = "}";
		char* token = strtok( buff, delimit );

		if( ! token )
		{
			continue;
		}
		else if( ! stricmp( "Human", token ) )
		{
			blockType = BlockRace;
			curStatusKey.first = RaceType_Human;
		}
		else if( ! stricmp( "Elf", token ) )
		{
			blockType = BlockRace;
			curStatusKey.first = RaceType_Elf;
		}
		else if( ! stricmp( "Devil", token ) )
		{
			// ������ Ŭ������ ����.
			blockType = BlockClass;
			curStatusKey.first = RaceType_Devil;
			curStatusKey.second = 4;
		}
		else if( ! strnicmp( markBegin, token, strlen( markBegin ) ) )
		{
			++dwOpenCnt;
		}
		else if( ! strnicmp( markEnd, token, strlen( markEnd ) ) )
		{
			--dwOpenCnt;

			switch( blockType )
			{
			case BlockRace:
				{
					blockType = BlockNone;
				}
				break;
			case BlockClass:
				{
					blockType = BlockRace;
				}
				break;
			}
		}

		switch( blockType )
		{
		case BlockRace:
			{
				if( ! stricmp( "Fighter", token ) )
				{
					blockType = BlockClass;
					curStatusKey.second = 1;
				}
				else if( ! stricmp( "Rogue", token ) )
				{
					blockType = BlockClass;
					curStatusKey.second = 2;
				}
				else if( ! stricmp( "Mage", token ) )
				{
					blockType = BlockClass;
					curStatusKey.second = 3;
				}
			}
			break;

		case BlockClass:
			{
				PlayerStat& stat = m_CharBaseStatusInfoMap[ curStatusKey ];
				
				if( ! stricmp( "str", token ) )
				{
					token = strtok( 0, delimit );
					if( !token )	break;
					stat.mStrength.mPlus = float( _tstof( token ) );
				}
				else if( ! stricmp( "dex", token ) )
				{
					token = strtok( 0, delimit );
					if( !token )	break;
					stat.mDexterity.mPlus = float( _tstof( token ) );
				}
				else if( ! stricmp( "vit", token ) )
				{
					token = strtok( 0, delimit );
					if( !token )	break;
					stat.mVitality.mPlus = float( _tstof( token ) );
				}
				else if( ! stricmp( "int", token ) )
				{
					token = strtok( 0, delimit );
					if( !token )	break;
					stat.mIntelligence.mPlus = float( _tstof( token ) );
				}
				else if( ! stricmp( "wis", token ) )
				{
					token = strtok( 0, delimit );
					if( !token )	break;
					stat.mWisdom.mPlus = float( _tstof( token ) );
				}
			}
			break;
		}
	}
}

const PlayerStat& CServerSystem::GetBaseStatus( RaceType race, BYTE byClass )
{
	static PlayerStat emptyPlayerStat;

	CharBaseStatusInfoKey key;
	key.first = race;
	key.second = byClass;

	CharBaseStatusInfoMap::const_iterator iter = m_CharBaseStatusInfoMap.find( key );
	if( iter == m_CharBaseStatusInfoMap.end() )
		return emptyPlayerStat;

	return iter->second;
}

const _PACKET_GAME& CServerSystem::GetPacketGame(UserIndex userIndex) const
{
	static const _PACKET_GAME emptyPackGame = {0};
	const UserPacketContainer::const_iterator iterator = mUserPacketContainer.find(
		userIndex);

	return mUserPacketContainer.end() == iterator ? emptyPackGame : iterator->second;
}

void CServerSystem::SetPacketGame(UserIndex userIndex, const _PACKET_GAME& packetGame)
{
	mUserPacketContainer[userIndex] = packetGame;
}

void CServerSystem::RemovePacketGame(UserIndex userIndex)
{
	mUserPacketContainer.erase(
		userIndex);
}

void __stdcall OnConnectServerSuccess(DWORD dwConnectionIndex, void* pVoid)
{
	SERVERINFO* info = (SERVERINFO*)pVoid;
	info->dwConnectionIndex = dwConnectionIndex;
	if(info->wServerKind == MONITOR_SERVER)
	{
		BOOTMNGR->NotifyBootUpToMS(&g_Network);
		g_Console.LOG(4, "Connected to the MS : %s, %d, (%d)", info->szIPForServer, info->wPortForServer, dwConnectionIndex);
	}
	else
	{
		BOOTMNGR->SendConnectSynMsg(&g_Network, dwConnectionIndex, g_pServerTable);
		g_Console.LOG(4, "Connected to the Server : %s, %d, (%d)", info->szIPForServer, info->wPortForServer, dwConnectionIndex);
	}

/*
	SERVERINFO* info = (SERVERINFO*)pVoid;
	if(!dwConnectionIndex) return;

	char strr[255];
	wsprintf(strr, ""%d �� ������ ���Ӽ���", dwConnectionIndex);//pjslocal
	g_Console.LOG(4, strr);
	

	info->dwConnectionIndex = dwConnectionIndex;					// �̰� ����� �����ϳ�? üũ
	MSG_WORD msg;
	msg.Category = MP_SERVER;
	msg.Protocol = MP_SERVER_PWRUP;
	msg.wData = g_pServerTable->GetSelfServer()->wPortForServer;
	g_Network.Send2Server(dwConnectionIndex, (char*)&msg, sizeof(msg));*/


	
	//YH2DO �ּ�ó�� �Ǿ��ִ� ���� ����
	if(g_pServerTable->GetMaxServerConnectionIndex() < dwConnectionIndex)			//??????
		g_pServerTable->SetMaxServerConnectionIndex(dwConnectionIndex);	
}

// 080813 LUJ, ������ inetwork ȣ�� ���Ŀ� ���� ����
void __stdcall OnDisconnectServer(DWORD dwConnectionIndex)
{
	char strr[255];
	
	SERVERINFO* serverinfo = g_pServerTable->FindServerForConnectionIndex(dwConnectionIndex);

	wsprintf(strr, "%d �� connectionIndex ���� ���� ����", dwConnectionIndex);//pjslocal
	g_Console.LOG(4, strr);

	// ����� �ʵ� �ų�??------------------------------------------------------------------------
//	if( serverinfo )
//	if( serverinfo->wServerKind == AGENT_SERVER )	//Agent�����ΰ��
//	{
		//�ٽ�����...
//		SERVERINFO Info;
//		g_Network->ConnectToServer( serverinfo->szIPForServer, serverinfo->wPortForServer, (void*)&Info );
//		return;
//	}
	//-------------------------------------------------------------------------------------------

	if(serverinfo)
	if( serverinfo->wServerKind == 3 )	//map�����ΰ��
	{
		//�ʼ����� ����� �������� �ʼ��� �ٿ��� �˸�
		g_pServerSystem->SendMsgMapServerDown(serverinfo->wServerNum);
	}

	SERVERINFO * delInfo = g_pServerTable->RemoveServer(dwConnectionIndex);
	if(!delInfo)
	{
		ASSERT(0);
	}
	delete delInfo;
}

// 080813 LUJ, ������ inetwork ȣ�� ���Ŀ� ���� ����
void __stdcall OnConnectServerFail(void* pVoid)
{
	SERVERINFO* info = (SERVERINFO*)pVoid;
	if(info->wServerKind == MONITOR_SERVER)
	{
		BOOTMNGR->AddBootListINI(AGENT_SERVER, g_pServerSystem->GetServerNum(), g_pServerTable);
		BOOTMNGR->BactchConnectToMap(&g_Network, g_pServerTable);
		g_Console.LOG(4, "Failed to Connect to the MS : %s, %d", info->szIPForServer, info->wPortForServer);
	}
	else
	{
		g_Console.LOG(4, "Failed to Connect to the Server : %s, %d", info->szIPForServer, info->wPortForServer);
		BOOTMNGR->RemoveBootList(g_pServerTable, info->wPortForServer);
	}	
		
}

// 080813 LUJ, ������ inetwork ȣ�� ���Ŀ� ���� ����
void __stdcall OnAcceptServer(DWORD dwConnectionIndex)
{
	g_Console.LOG(4, "Server Connected : ConnectionIndex %d", dwConnectionIndex );
}

// 080813 LUJ, ������ inetwork ȣ�� ���Ŀ� ���� ����
void __stdcall OnAcceptUser(DWORD dwConnectionIndex)
{
	if(g_bReady == FALSE)
	{
		//---KES Network Process Fix 071114
		g_Network.DisconnectUser( dwConnectionIndex );	//���ӵǾ��� ���� �����ش�.
		//-----------------------------
		return;
	}
	
	++g_dwAcceptUser;

	//---KES Network Process Fix 071114
	USERINFO* pInfo = g_UserInfoPool.Alloc();
	memset( pInfo, 0, sizeof(USERINFO) );
	pInfo->dwConnectionIndex = dwConnectionIndex;
	pInfo->dwLastConnectionCheckTime = gCurTime;
	g_pUserTable->AddUser( pInfo, dwConnectionIndex );	//�ϴ� �ӽ������� �־��ش�.
	//-----------------------------

	MSGBASE send;
	send.Category = MP_USERCONN;
	send.Protocol = MP_USERCONN_AGENT_CONNECTSUCCESS;
	g_Network.Send2User(dwConnectionIndex, (char*)&send, sizeof(send));

	//event����
	if( g_pServerSystem->IsUseEventNotify() )
	{
		MSG_EVENTNOTIFY_ON msg;
		msg.Category	= MP_CHEAT;
		msg.Protocol	= MP_CHEAT_EVENTNOTIFY_ON;
		SafeStrCpy( msg.strTitle, g_pServerSystem->GetEventNotifyTitle(), 32 );
		SafeStrCpy( msg.strContext, g_pServerSystem->GetEventNotifyContext(), 128 );
				
		g_Network.Send2User(dwConnectionIndex, (char*)&msg, sizeof(msg));
	}	
	
	//user count ���=============================================================================
	SERVERINFO * myInfo = g_pServerTable->GetSelfServer();
	++myInfo->wAgentUserCnt;

	MSG_WORD2 msg2;
	msg2.Category = MP_SERVER;
	msg2.Protocol = MP_SERVER_USERCNT;
	msg2.wData1 = myInfo->wPortForServer;
	msg2.wData2 = myInfo->wAgentUserCnt;
	
	g_pServerTable->SetPositionHead();
	SERVERINFO* pOtherAgentInfo = NULL;
	while( (pOtherAgentInfo = g_pServerTable->GetNextDistServer()) != NULL )
	{
		if(myInfo == pOtherAgentInfo)
			continue;

		g_Network.Send2Server(pOtherAgentInfo->dwConnectionIndex, (char *)&msg2, sizeof(msg2));
	}

	g_pUserTable->SetCalcMaxCount(
		g_pUserTable->GetDataNum());
}

// 080813 LUJ, ������ inetwork ȣ�� ���Ŀ� ���� ����
void __stdcall OnDisconnectUser(DWORD dwConnectionIndex)
{
	if(g_bReady == FALSE)
		return;

	g_pUserTable->OnDisconnectUser(dwConnectionIndex);
}

// 080813 LUJ, ������ inetwork ȣ�� ���Ŀ� ���� ����
void __stdcall ReceivedMsgFromServer(DWORD dwConnectionIndex,char* pMsg,DWORD dwLength)
{
	MSGROOT* pTempMsg = reinterpret_cast<MSGROOT*>(pMsg);
	ASSERT(g_pServerMsgParser[pTempMsg->Category] != NULL);
	
//KES
	if( pTempMsg->Category <= 0 || pTempMsg->Category >= MP_MAX ||
		g_pServerMsgParser[pTempMsg->Category] == NULL )
	{
		char buf[128];
		sprintf( buf, "Category error : %d", pTempMsg->Category );
		ASSERTMSG( 0, buf );
		return;
	}

	// desc_hseos_����ó��01
	// S ����ó�� �߰� added by hseos 2007.07.27
	// g_pServerMsgParser[pTempMsg->Category](dwConnectionIndex, pMsg, dwLength);
	if (CSHDebug::GetExceptionProcGrade())
	{
		try
		{
			g_pServerMsgParser[pTempMsg->Category](dwConnectionIndex, pMsg, dwLength);
		}
		catch(...)
		{
			char szTxt[256];
			sprintf(szTxt, "A  ReceivedMsgFromUser, %u, %u, %u, %u", dwConnectionIndex, pTempMsg->Category, pTempMsg->Protocol, dwLength);
			CSHDebug::LogExceptionError(szTxt);
		}
	}
	else
	{
		g_pServerMsgParser[pTempMsg->Category](dwConnectionIndex, pMsg, dwLength);
	}
	// E ����ó�� �߰� added by hseos 2007.07.27
}

// 080813 LUJ, ������ inetwork ȣ�� ���Ŀ� ���� ����
void __stdcall ReceivedMsgFromUser(DWORD dwConnectionIndex,char* pMsg,DWORD dwLength)
{
	MSGROOT* pTempMsg = reinterpret_cast<MSGROOT*>(pMsg);

	USERINFO* pInfo = g_pUserTable->FindUser(dwConnectionIndex);

	if( pInfo )
	if( g_pServerSystem->IsEnableCrypt() )
	{
//---KES CheckSum 071003
		if( pInfo->cbCheckSum == 0 )
		{
			pInfo->cbCheckSum = pTempMsg->CheckSum;
		}
		else if( pInfo->cbCheckSum != pTempMsg->CheckSum )
		{
			OnDisconnectUser( dwConnectionIndex );
			DisconnectUser( dwConnectionIndex );
			return;
		}

		++pInfo->cbCheckSum;
//----------------------

		int headerSize = sizeof( MSGROOT );

		if( !pInfo->crypto.Decrypt( ( char * )pTempMsg +  headerSize, dwLength - headerSize ) )
		{
			ASSERTMSG(0,"Decrypt Error");
			OnDisconnectUser( dwConnectionIndex );
			DisconnectUser( dwConnectionIndex );
			return;
		}
		char aaa = pInfo->crypto.GetDeCRCConvertChar();
		if( pTempMsg->Code !=  aaa )
		{
			ASSERTMSG(0,"Decrypt CRC Error");
			OnDisconnectUser( dwConnectionIndex );
			DisconnectUser( dwConnectionIndex );
			return;
		}

		TRAFFIC->AddReceivePacketAll( pTempMsg->Category, pTempMsg->Protocol, dwLength );
		TRAFFIC->AddUserPacket( pInfo->dwUserID, pTempMsg->Category );
	}

	ASSERT(g_pUserMsgParser[pTempMsg->Category] != NULL);
	ASSERT(pTempMsg->Category < MP_MAX);
	ASSERT(pTempMsg->Category != 0);
	if( g_pUserMsgParser[pTempMsg->Category] == NULL ||
		pTempMsg->Category >= MP_MAX ||
		pTempMsg->Category == 0)
		return;

	// 090317 ONS ��Ŷ���� ����ó�� �߰�
	MSGBASE* pmsg = (MSGBASE*)pTempMsg;
	if(pInfo && pInfo->dwCharacterID) pmsg->dwObjectID = pInfo->dwCharacterID;

	if (CSHDebug::GetExceptionProcGrade())
	{
		try
		{
			g_pUserMsgParser[pTempMsg->Category](dwConnectionIndex, pMsg, dwLength);
		}
		catch(...)
		{
			char szTxt[256];
			sprintf(szTxt, "A  ReceivedMsgFromUser, %u, %u, %u, %u", dwConnectionIndex, pTempMsg->Category, pTempMsg->Protocol, dwLength);
			CSHDebug::LogExceptionError(szTxt);
		}
	}
	else
	{
		g_pUserMsgParser[pTempMsg->Category](dwConnectionIndex, pMsg, dwLength);
	}
	// E ����ó�� �߰� added by hseos 2007.07.27
}

void __stdcall ProcessServer(DWORD)
{
	g_DB.ProcessingDBMessage();
}

void __stdcall ProcessGame(DWORD)
{
	g_pServerSystem->Process();
}

void __stdcall ProcessCheck(DWORD)
{
	g_pServerSystem->ConnectionCheck();
}

void ButtonProc1()
{
	int servercount = g_pServerTable->GetDataNum();
	g_Console.LOG(4, "Connected Server Count : %d", servercount);

	char temp[128] = {0,};
	SERVERINFO* pInfo = NULL;
	g_pServerTable->SetPositionHead();
	while( (pInfo = g_pServerTable->GetNextServer()) != NULL )
	{
		switch( pInfo->wServerKind )
		{
		case DISTRIBUTE_SERVER:	sprintf( temp, "DistributeServer" ); break;
		case AGENT_SERVER:		sprintf( temp, "AgentServer" );	break;
		case MAP_SERVER:		sprintf( temp, "MapServer" );	break;		
		}
		g_Console.LOG(4, "%s : IP = %s, Port = %d", temp, pInfo->szIPForServer, pInfo->wPortForServer );
	}
}

void ButtonProc2()
{
	g_Console.LOG(4, "UserCount %d %d %d, AcceptUser %d %d [ USERIDX[%d, %d] UT[%d, %d] OBJECTIDX[%d, %d]]",
		g_pUserTableForUserID->GetDataNum(),
		g_pUserTable->GetDataNum(),
		g_pUserTableForObjectID->GetDataNum(),
		g_dwAcceptUser, g_dwAcceptUser2, 
		g_pUserTableForUserID->GetAddCount(), 
		g_pUserTableForUserID->GetRemoveCount(), 
		g_pUserTable->GetAddCount(), 
		g_pUserTable->GetRemoveCount(),
		g_pUserTableForObjectID->GetAddCount(), 
		g_pUserTableForObjectID->GetRemoveCount() );
}

void ButtonProc3()
{
	g_Console.LOG(4, "INetwork GetConnectedUserNum() : %d", g_Network.GetINet()->GetConnectedUserNum());

	TRAFFIC->WriteTrafficLogFile();
}

void OnCommand(char* szCommand)
{
	strupr(szCommand);
	char Header[256] = {0,};	
	sscanf(szCommand,"%s",Header);

	if(0 == strstr(Header, "FINDUSER"))
	{
		return;
	}

	DWORD UserID = 0;
	sscanf(szCommand,"%s %d",Header,&UserID);

	if(const USERINFO* const pInfo = g_pUserTableForUserID->FindUser(UserID))
	{
		g_Console.Log(
			eLogDisplay,
			4,
			"UserID:%d  ConIdx:%d  DistAuthKey:%d  AgentAuthKey:%d",
			UserID,
			pInfo->dwConnectionIndex,
			pInfo->DistAuthKey,
			pInfo->dwUniqueConnectIdx);
	}
	else
	{
		g_Console.Log(
			eLogDisplay,
			4,
			"No User (%d)",
			UserID);
	}
}
