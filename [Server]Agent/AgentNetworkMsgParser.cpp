
#include "stdafx.h"
#include "AgentNetworkMsgParser.h"
#include "ServerTable.h"
#include "Network.h"
#include "AgentDBMsgParser.h"
#include "MsgTable.h"

#include "FilteringTable.h"
#include "BootManager.h"
#include "MHTimeManager.h"
#include "GMPowerList.h"
#include "PlustimeMgr.h"
#include "..\[CC]Header\Pool.h"
#include "TrafficLog.h"
#include "../hseos/Family/SHFamilyManager.h"
#include "../hseos/Date/SHDateManager.h"
#include "../hseos/ResidentRegist/SHResidentRegistManager.h"
#include "../hseos/Farm/SHFarmManager.h"
#include "PunishManager.h"
#include "./ChatRoomMgr.h"

#ifdef _HACK_SHIELD_
#include "HackShieldManager.h"
#endif

#include "ggsrv25.h"
#include "NProtectManager.h"

extern CPool< USERINFO > g_UserInfoPool;
extern int g_nServerSetNum;
extern HWND g_hWnd;
extern BOOL g_bCloseWindow;
DWORD g_dwAcceptUser2 = 0;

#ifdef _NPROTECT_
#include "ggsrv25.h"
#include "NProtectManager.h"

extern CPool<CCSAuth2> g_CCSAuth2Pool;
#endif

#include ".\giftmanager.h"

// 080728 LYW --- AgentNetworkMsgParser : Include recall manager for siege war.
#include "./SiegeRecallMgr.h"

// 080819 LYW --- AgentNetworkMsgParser : Include npc recall manager.
#include "./NpcRecallMgr.h"

void MP_MonitorMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGROOT* pTempMsg = (MSGROOT*)pMsg;
	switch(pTempMsg->Protocol)
	{
	case MP_MORNITORMAPSERVER_NOTICESEND_SYN:						MornitorMapServer_NoticeSend_Syn(dwConnectionIndex, pMsg) ;						break;
	case MP_MORNITORMAPSERVER_PING_SYN:								MornitorMapServer_Ping_Syn(pTempMsg, dwConnectionIndex, pMsg, dwLength) ;		break;
	case MP_MORNITORMAPSERVER_QUERYUSERCOUNT_SYN:					MornitorMapServer_QueryUserCount_Syn(dwConnectionIndex, pMsg) ;					break;
	case MP_MORNITORMAPSERVER_ASSERTMSGBOX_SYN:						MornitorMapServer_AssertMsgBox_Syn(pMsg) ;										break;		
	case MP_MORNITORMAPSERVER_DISCONNECT_ALLUSER_SYN:				MornitorMapServer_Disconnect_AllUser_Syn(dwConnectionIndex)	;					break;
	case MP_MORNITORMAPSERVER_SERVEROFF_SYN:						MornitorMapServer_ServerOff_Syn() ;												break;
	case MP_MORNITORMAPSERVER_CONNECTED_SERVERLIST_SYN:				MornitorMapServer_Connected_ServerList_Syn(dwConnectionIndex) ;					break;		
	case MP_MORNITORMAPSERVER_EVENTNOTICE_ON:						MornitorMapServer_EventNotice_On(pMsg) ;										break;
	case MP_MORNITORMAPSERVER_EVENTNOTICE_OFF:						MornitorMapServer_EventNotice_Off(pMsg) ;										break;
	}
}


void MornitorMapServer_NoticeSend_Syn(DWORD dwConnectionIndex, char* pMsg)
{
	// �����Ϩ�Ao��icC���� 
	MSGNOTICE* pmsg = (MSGNOTICE*)pMsg;
	USERINFO * info = NULL;
	TESTMSG msg;
	msg.Category = MP_SIGNAL;
	msg.Protocol = MP_SIGNAL_COMMONUSER;
//			strcpy(msg.Msg, pmsg->Msg);
	SafeStrCpy( msg.Msg, pmsg->Msg, MAX_CHAT_LENGTH + 1 );

	g_pUserTable->SetPositionUserHead();
	while( (info = (USERINFO *)g_pUserTable->GetUserData()) != NULL )
	{	
		//g_Network.Send2User(info->dwConnectionIndex, (char*)&msg, sizeof(TESTMSG));
		TESTMSG msgTemp = msg;
		g_Network.Send2User(info->dwConnectionIndex, (char*)&msgTemp, msgTemp.GetMsgLength());	//CHATMSG 040324
	}
	//g_Console.Log(eLogDisplay,4,pmsg->Msg);
}


void MornitorMapServer_Ping_Syn(MSGROOT* pTempMsg, DWORD dwConnectionIndex, char* pMsg, DWORD dwLength) 
{
	pTempMsg->Protocol = MP_MORNITORMAPSERVER_PING_ACK;
	g_Network.Send2Server(dwConnectionIndex, pMsg, dwLength);
//			g_Console.LOG(4, "ping msg recv...");
}


void MornitorMapServer_QueryUserCount_Syn(DWORD dwConnectionIndex, char* pMsg) 
{
	MSGUSERCOUNT  * pmsg = (MSGUSERCOUNT  *)pMsg;
	pmsg->Protocol = MP_MORNITORMAPSERVER_QUERYUSERCOUNT_ACK;
	pmsg->dwUserCount = g_pUserTable->GetDataNum();
	g_Network.Send2Server(dwConnectionIndex, pMsg, sizeof(MSGUSERCOUNT));
}


void MornitorMapServer_AssertMsgBox_Syn(char* pMsg) 
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	g_bAssertMsgBox = (BOOL)pmsg->dwData;
	if(g_bAssertMsgBox)
		g_Console.LOG(4,"Assert MsgBox is On");
	else
		g_Console.LOG(4,"Assert MsgBox is Off");
}


void MornitorMapServer_Disconnect_AllUser_Syn(DWORD dwConnectionIndex)
{
	// �ˡ��ϡ̡�ic A?Au��?�ˢ� �ˡ�eCN A��E��uO C����A|...
	g_pUserTableForUserID->SetPositionHead();
	USERINFO* pInfo;
	while( (pInfo = g_pUserTableForUserID->GetData()) != NULL )
	{
		if( pInfo->dwConnectionIndex != 0 )
		{
			DWORD dwConIdx = pInfo->dwConnectionIndex;
			OnDisconnectUser( dwConIdx );
			DisconnectUser( dwConIdx );
		}
		else
		{
			LoginCheckDelete(pInfo->dwUserID);
			g_pUserTableForUserID->RemoveUser(pInfo->dwUserID);
#ifdef _NPROTECT_
			if( pInfo->m_pCSA )
			{
				pInfo->m_pCSA->Close();
				g_CCSAuth2Pool.Free(pInfo->m_pCSA);
			}
#endif
			memset( pInfo, 0, sizeof(USERINFO) );	//�߰� 060414 KES
			g_UserInfoPool.Free(pInfo);
		}
	}

	// A|�ˡ�e����I Ao�ˡ���c�ˡ�| �ˡ��ϡ̡�iI A�ϩ��ˡ���cC�ϡ��ˡ�AAo��?�ˢ� �ˡ�eCN A��uA��I...
	while( g_DB.AllCleared() == FALSE )
	{
		g_DB.ProcessingDBMessage();
		Sleep(50);
	}

	MSGBASE Msg;
	Msg.Category = MP_MORNITORMAPSERVER;
	Msg.Protocol = MP_MORNITORMAPSERVER_DISCONNECT_ALLUSER_ACK;
	g_Network.Send2Server(dwConnectionIndex, (char*)&Msg, sizeof(Msg));
}


void MornitorMapServer_ServerOff_Syn() 
{
	g_bCloseWindow = TRUE;
}


void MornitorMapServer_Connected_ServerList_Syn(DWORD dwConnectionIndex) 
{
	MSG_PWRUP_BOOTLIST Msg;
	memset( &Msg, 0, sizeof(MSG_PWRUP_BOOTLIST) );

	Msg.Category = MP_MORNITORMAPSERVER;
	Msg.Protocol = MP_MORNITORMAPSERVER_CONNECTED_SERVERLIST_ACK;

	SERVERINFO* pInfo = NULL;
	g_pServerTable->SetPositionHead();
	while( (pInfo = g_pServerTable->GetNextServer()) != NULL )
		Msg.AddList( pInfo );

	g_Network.Send2Server( dwConnectionIndex, (char*)&Msg, Msg.GetSize() );
}


void MornitorMapServer_EventNotice_On(char* pMsg) 
{
	MSG_EVENTNOTIFY_ON* pmsg = (MSG_EVENTNOTIFY_ON*)pMsg;

	g_pServerSystem->SetEventNotifyStr( pmsg->strTitle, pmsg->strContext );
	g_pServerSystem->SetUseEventNotify( TRUE );
	
//			g_Network.Broadcast2AgentServerExceptSelf( (char*)pmsg, dwLength );
}


void MornitorMapServer_EventNotice_Off(char* pMsg) 
{
	g_pServerSystem->SetUseEventNotify(
		FALSE);
}



void MP_POWERUPMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	BOOTMNGR->NetworkMsgParse(dwConnectionIndex, pMsg, dwLength);
}
void MP_AGENTSERVERMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{}

//---KES ��ŷ ��� ����
void MP_TEMPMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	USERINFO* pUserInfo = g_pUserTable->FindUser( dwConnectionIndex );
	if( !pUserInfo ) return;

	if( pUserInfo->dwCharacterID )
	{
		MSGBASE* pTempMsg = (MSGBASE*)pMsg;

		pTempMsg->dwObjectID = pUserInfo->dwCharacterID;		//��⸦ ���� ����. ��� Ŭ���̾�Ʈ�� ���� ���� �޼����� �̷��� �ٲ���� �Ѵ�!

		TransToMapServerMsgParser( dwConnectionIndex, pMsg, dwLength );
	}
}
//------------

void MP_USERCONNMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGROOT* pTempMsg = (MSGROOT*)pMsg;
	switch(pTempMsg->Protocol)
	{
//////////////////////////////////////////////////////////////////////////
// these Protocol Come From Distibute Server
	case MP_USERCONN_NOTIFY_USERLOGIN_SYN:							UserConn_Notify_UserLogin_Syn(dwConnectionIndex, pMsg) ;			break;
	case MP_USERCONN_NOTIFYTOAGENT_ALREADYOUT:						UserConn_NotifyToAgent_AlreadyOut(dwConnectionIndex, pMsg) ;		break;
	case MP_USERCONN_NOTIFY_OVERLAPPEDLOGIN:						UserConn_Notify_OverlappedLogin(dwConnectionIndex, pMsg) ;			break;
	case MP_USERCONN_FORCE_DISCONNECT_OVERLAPLOGIN:					UserConn_Force_Disconnect_OverlapLogin(dwConnectionIndex, pMsg) ;	break;
	case MP_USERCONN_DISCONNECTED_ON_LOGIN:							UserConn_Disconnected_On_Login(pMsg) ;								break;
	case MP_USERCONN_DISCONNECT_SYN:								UserConn_Disconnect_Syn(dwConnectionIndex) ;						break;
	case MP_USERCONN_CONNECTION_CHECK_OK:							UserConn_Connection_Check_Ok(dwConnectionIndex) ;					break;
	case MP_USERCONN_CHARACTERLIST_SYN:								UserConn_Characterlist_Syn(dwConnectionIndex, pMsg) ;				break;
	case MP_USERCONN_DIRECTCHARACTERLIST_SYN:						UserConn_DirectCharacterList_Syn(dwConnectionIndex) ;				break;	
	case MP_USERCONN_CHARACTERSELECT_SYN:							UserConn_CharacterSelect_Syn(dwConnectionIndex, pMsg) ;				break;
	case MP_USERCONN_CHANNELINFO_SYN:								UserConn_ChannelInfo_Syn(dwConnectionIndex, pMsg) ;					break;
	case MP_USERCONN_CHANNELINFO_ACK:								UserConn_ChannelInfo_Ack(pMsg, dwLength) ;							break;
	case MP_USERCONN_CHANNELINFO_NACK:								UserConn_ChannelInfo_Nack(pMsg, dwLength) ;							break;
	case MP_USERCONN_GAMEIN_SYN:									UserConn_Gamein_Syn(dwConnectionIndex, pMsg) ;						break;
	case MP_USERCONN_GAMEIN_NACK:									UserConn_Gamein_Nack(pMsg) ;										break;
	case MP_USERCONN_GAMEIN_ACK:									UserConn_Gamein_Ack(dwConnectionIndex, pMsg, dwLength) ;			break;
	case MP_USERCONN_CHARACTER_MAKE_SYN:							UserConn_Character_Make_Syn(dwConnectionIndex, pMsg) ;				break;
	case MP_USERCONN_CHARACTER_NAMECHECK_SYN:						UserConn_Character_NameCheck_Syn(dwConnectionIndex, pMsg) ;			break;
	case MP_USERCONN_CHARACTER_REMOVE_SYN:							UserConn_Character_Remove_Syn(dwConnectionIndex, pMsg) ;			break;
	case MP_USERCONN_CHANGEMAP_SYN:									UserConn_ChangeMap_Syn(dwConnectionIndex, pMsg, dwLength) ;			break;
	// 070917 LYW --- AgentNetworkMsgParser : Add function to process change map through the npc.
	case MP_USERCONN_NPC_CHANGEMAP_SYN :							UserConn_Npc_ChangeMap_Syn(dwConnectionIndex, pMsg, dwLength) ;		break;
	case MP_USERCONN_MAP_OUT:										UserConn_Map_Out(pMsg) ;											break;
	case MP_USERCONN_MAP_OUT_WITHMAPNUM:							UserConn_Map_Out_WithMapNum(pMsg) ;									break;
	// 070917 LYW --- AgentNetworkMsgParser : Add process to map out through the npc.
	case MP_USERCONN_NPC_MAP_OUT :									UserConn_Npc_Map_Out(pMsg) ;										break;
	case MP_USERCONN_CHANGEMAP_ACK:									UserConn_ChangeMap_Ack(pTempMsg, dwLength) ;						break;
	case MP_USERCONN_SAVEPOINT_SYN:									UserConn_SavePoint_Syn(dwConnectionIndex, pMsg, dwLength) ;			break;
	case MP_USERCONN_BACKTOCHARSEL_SYN:								UserConn_BackToCharSel_Syn(dwConnectionIndex, pMsg, dwLength) ;		break;
	case MP_USERCONN_BACKTOCHARSEL_ACK:								UserConn_BackToCharSel_Ack(pMsg) ;									break;
	case MP_USERCONN_CHEAT_USING:									UserConn_Cheat_Using(pMsg) ;										break;
	case MP_USERCONN_LOGINCHECK_DELETE:								UserConn_LoginCheck_Delete(pMsg) ;									break;
	case MP_USERCONN_GAMEINPOS_SYN:									UserConn_GameInPos_Syn(dwConnectionIndex, pMsg) ;					break;
	case MP_USERCONN_EVENTITEM_USE:									UserConn_EventItem_Use(pMsg, dwLength) ;							break;
	case MP_USERCONN_EVENTITEM_USE2:								UserConn_EventItem_Use2(pMsg, dwLength) ;							break;
	case MP_USERCONN_RETURN_SYN:									UserConn_Return_Syn(dwConnectionIndex, pMsg, dwLength) ;			break;
	case MP_USERCONN_NPC_ADD_SYN:									TransToMapServerMsgParser(dwConnectionIndex,pMsg,dwLength);			break;
	case MP_USERCONN_NPC_DIE_SYN:									TransToMapServerMsgParser(dwConnectionIndex,pMsg,dwLength);			break;
	// 081031 LUJ, ä�� ��ȣ�� ������ �����Ѵ�
	case MP_USERCONN_CHANGE_CHANNEL_SYN:
		{
			const MSG_WORD* message			= ( MSG_WORD* )pMsg;
			const WORD		channelIndex	= message->wData;
			USERINFO* const userinfo		= g_pUserTableForObjectID->FindUser( message->dwObjectID );

			if( userinfo )
			{
				userinfo->wChannel = channelIndex;
			}

			break;
		}
	// 081010 LYW --- AgeentNetworkMsgPasrser : ���������� ����� ���̵� ó�� �߰�.
	case MP_USERCONN_SIEGEWARFARE_MAP_OUT_WITHMAPNUM_SYN :		SiegeWarfare_Map_Out_WithMapNum( dwConnectionIndex, pMsg, dwLength ) ;	break ;

	default:
		g_Network.Send2User((MSGBASE*)pTempMsg,dwLength);
		break;
	}
}

void MP_USERCONNUserMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGROOT* pTempMsg = (MSGROOT*)pMsg;
	switch(pTempMsg->Protocol)
	{
//////////////////////////////////////////////////////////////////////////
	case MP_USERCONN_FORCE_DISCONNECT_OVERLAPLOGIN:					UserConn_Force_Disconnect_OverlapLogin(dwConnectionIndex, pMsg) ;	break;
	case MP_USERCONN_DISCONNECT_SYN:								UserConn_Disconnect_Syn(dwConnectionIndex) ;						break;
	case MP_USERCONN_CONNECTION_CHECK_OK:							UserConn_Connection_Check_Ok(dwConnectionIndex) ;					break;
	case MP_USERCONN_CHARACTERLIST_SYN:								UserConn_Characterlist_Syn(dwConnectionIndex, pMsg) ;				break;
	case MP_USERCONN_DIRECTCHARACTERLIST_SYN:						UserConn_DirectCharacterList_Syn(dwConnectionIndex) ;				break;	
	case MP_USERCONN_CHARACTERSELECT_SYN:							UserConn_CharacterSelect_Syn(dwConnectionIndex, pMsg) ;				break;
	case MP_USERCONN_CHANNELINFO_SYN:								UserConn_ChannelInfo_Syn(dwConnectionIndex, pMsg) ;					break;
	case MP_USERCONN_GAMEIN_SYN:									UserConn_Gamein_Syn(dwConnectionIndex, pMsg) ;						break;
	case MP_USERCONN_CHARACTER_MAKE_SYN:							UserConn_Character_Make_Syn(dwConnectionIndex, pMsg) ;				break;
	case MP_USERCONN_CHARACTER_NAMECHECK_SYN:						UserConn_Character_NameCheck_Syn(dwConnectionIndex, pMsg) ;			break;
	case MP_USERCONN_CHARACTER_REMOVE_SYN:							UserConn_Character_Remove_Syn(dwConnectionIndex, pMsg) ;			break;
	case MP_USERCONN_CHANGEMAP_SYN:									UserConn_ChangeMap_Syn(dwConnectionIndex, pMsg, dwLength) ;			break;
	case MP_USERCONN_NPC_CHANGEMAP_SYN :							UserConn_Npc_ChangeMap_Syn(dwConnectionIndex, pMsg, dwLength) ;		break;
	case MP_USERCONN_MAP_OUT_WITHMAPNUM:							UserConn_Map_Out_WithMapNum(pMsg) ;									break;
	case MP_USERCONN_SAVEPOINT_SYN:									UserConn_SavePoint_Syn(dwConnectionIndex, pMsg, dwLength) ;			break;
	case MP_USERCONN_BACKTOCHARSEL_SYN:								UserConn_BackToCharSel_Syn(dwConnectionIndex, pMsg, dwLength) ;		break;
	case MP_USERCONN_GAMEINPOS_SYN:									UserConn_GameInPos_Syn(dwConnectionIndex, pMsg) ;					break;
	case MP_USERCONN_RETURN_SYN:									UserConn_Return_Syn(dwConnectionIndex, pMsg, dwLength) ;			break;
	case MP_USERCONN_NPC_ADD_SYN:									TransToMapServerMsgParser(dwConnectionIndex,pMsg,dwLength);			break;
	case MP_USERCONN_NPC_DIE_SYN:									TransToMapServerMsgParser(dwConnectionIndex,pMsg,dwLength);			break;
	case MP_USERCONN_OPTION_SYN:									UserConn_Option_Syn(dwConnectionIndex, pMsg, dwLength);				break;
	case MP_USERCONN_GETSERVERTIME_SYN:								TransToMapServerMsgParser(dwConnectionIndex,pMsg,dwLength);			break;
	case MP_USERCONN_CHANGE_CHANNEL_IN_GAME_SYN:					TransToMapServerMsgParser(dwConnectionIndex,pMsg,dwLength);			break;
	}
}

void MP_USERCONNServerMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGROOT* pTempMsg = (MSGROOT*)pMsg;
	switch(pTempMsg->Protocol)
	{
//////////////////////////////////////////////////////////////////////////
// these Protocol Come From Distibute Server
	case MP_USERCONN_NOTIFY_USERLOGIN_SYN:							UserConn_Notify_UserLogin_Syn(dwConnectionIndex, pMsg) ;			break;
	case MP_USERCONN_NOTIFYTOAGENT_ALREADYOUT:						UserConn_NotifyToAgent_AlreadyOut(dwConnectionIndex, pMsg) ;		break;
	case MP_USERCONN_NOTIFY_OVERLAPPEDLOGIN:						UserConn_Notify_OverlappedLogin(dwConnectionIndex, pMsg) ;			break;
	case MP_USERCONN_FORCE_DISCONNECT_OVERLAPLOGIN:					UserConn_Force_Disconnect_OverlapLogin(dwConnectionIndex, pMsg) ;	break;
	case MP_USERCONN_DISCONNECTED_ON_LOGIN:							UserConn_Disconnected_On_Login(pMsg) ;								break;
	case MP_USERCONN_DISCONNECT_SYN:								UserConn_Disconnect_Syn(dwConnectionIndex) ;						break;
	case MP_USERCONN_CONNECTION_CHECK_OK:							UserConn_Connection_Check_Ok(dwConnectionIndex) ;					break;
	case MP_USERCONN_CHARACTERLIST_SYN:								UserConn_Characterlist_Syn(dwConnectionIndex, pMsg) ;				break;
	case MP_USERCONN_DIRECTCHARACTERLIST_SYN:						UserConn_DirectCharacterList_Syn(dwConnectionIndex) ;				break;	
	case MP_USERCONN_CHARACTERSELECT_SYN:							UserConn_CharacterSelect_Syn(dwConnectionIndex, pMsg) ;				break;
	case MP_USERCONN_CHANNELINFO_SYN:								UserConn_ChannelInfo_Syn(dwConnectionIndex, pMsg) ;					break;
	case MP_USERCONN_CHANNELINFO_ACK:								UserConn_ChannelInfo_Ack(pMsg, dwLength) ;							break;
	case MP_USERCONN_CHANNELINFO_NACK:								UserConn_ChannelInfo_Nack(pMsg, dwLength) ;							break;
	case MP_USERCONN_GAMEIN_SYN:									UserConn_Gamein_Syn(dwConnectionIndex, pMsg) ;						break;
	case MP_USERCONN_GAMEIN_NACK:									UserConn_Gamein_Nack(pMsg) ;										break;
	case MP_USERCONN_GAMEIN_ACK:									UserConn_Gamein_Ack(dwConnectionIndex, pMsg, dwLength) ;			break;
	case MP_USERCONN_CHARACTER_MAKE_SYN:							UserConn_Character_Make_Syn(dwConnectionIndex, pMsg) ;				break;
	case MP_USERCONN_CHARACTER_NAMECHECK_SYN:						UserConn_Character_NameCheck_Syn(dwConnectionIndex, pMsg) ;			break;
	case MP_USERCONN_CHARACTER_REMOVE_SYN:							UserConn_Character_Remove_Syn(dwConnectionIndex, pMsg) ;			break;
	case MP_USERCONN_CHANGEMAP_SYN:									UserConn_ChangeMap_Syn(dwConnectionIndex, pMsg, dwLength) ;			break;
	// 070917 LYW --- AgentNetworkMsgParser : Add function to process change map through the npc.
	case MP_USERCONN_NPC_CHANGEMAP_SYN :							UserConn_Npc_ChangeMap_Syn(dwConnectionIndex, pMsg, dwLength) ;		break;
	case MP_USERCONN_MAP_OUT:										UserConn_Map_Out(pMsg) ;											break;
	case MP_USERCONN_MAP_OUT_WITHMAPNUM:							UserConn_Map_Out_WithMapNum(pMsg) ;									break;
	// 070917 LYW --- AgentNetworkMsgParser : Add process to map out through the npc.
	case MP_USERCONN_NPC_MAP_OUT :									UserConn_Npc_Map_Out(pMsg) ;										break;
	case MP_USERCONN_CHANGEMAP_ACK:									UserConn_ChangeMap_Ack(pTempMsg, dwLength) ;						break;
	case MP_USERCONN_SAVEPOINT_SYN:									UserConn_SavePoint_Syn(dwConnectionIndex, pMsg, dwLength) ;			break;
	case MP_USERCONN_BACKTOCHARSEL_SYN:								UserConn_BackToCharSel_Syn(dwConnectionIndex, pMsg, dwLength) ;		break;
	case MP_USERCONN_BACKTOCHARSEL_ACK:								UserConn_BackToCharSel_Ack(pMsg) ;									break;
	case MP_USERCONN_CHEAT_USING:									UserConn_Cheat_Using(pMsg) ;										break;
	case MP_USERCONN_LOGINCHECK_DELETE:								UserConn_LoginCheck_Delete(pMsg) ;									break;
	case MP_USERCONN_GAMEINPOS_SYN:									UserConn_GameInPos_Syn(dwConnectionIndex, pMsg) ;					break;
	case MP_USERCONN_EVENTITEM_USE:									UserConn_EventItem_Use(pMsg, dwLength) ;							break;
	case MP_USERCONN_EVENTITEM_USE2:								UserConn_EventItem_Use2(pMsg, dwLength) ;							break;
	case MP_USERCONN_RETURN_SYN:									UserConn_Return_Syn(dwConnectionIndex, pMsg, dwLength) ;			break;
	case MP_USERCONN_NPC_ADD_SYN:									TransToMapServerMsgParser(dwConnectionIndex,pMsg,dwLength);			break;
	case MP_USERCONN_NPC_DIE_SYN:									TransToMapServerMsgParser(dwConnectionIndex,pMsg,dwLength);			break;
	// 081031 LUJ, ä�� ��ȣ�� ������ �����Ѵ�
	case MP_USERCONN_CHANGE_CHANNEL_SYN:
		{
			const MSG_WORD* message			= ( MSG_WORD* )pMsg;
			const WORD		channelIndex	= message->wData;
			USERINFO* const userinfo		= g_pUserTableForObjectID->FindUser( message->dwObjectID );

			if( userinfo )
			{
				userinfo->wChannel = channelIndex;
			}

			break;
		}
	// 081010 LYW --- AgeentNetworkMsgPasrser : ���������� ����� ���̵� ó�� �߰�.
	case MP_USERCONN_SIEGEWARFARE_MAP_OUT_WITHMAPNUM_SYN :		SiegeWarfare_Map_Out_WithMapNum( dwConnectionIndex, pMsg, dwLength ) ;	break ;
	// 091106 LUJ, ������ ������ ä�ο� ������ �� �ֵ��� Ŭ���̾�Ʈ���� �������� ä�� ��ȣ�� �����ǵ��� �Ѵ�
	//			Ŭ���̾�Ʈ�� ������ ä�� ��ȣ�� Ŭ���̾�Ʈ�� ���ٰ� �ٽ� ���ƿ��µ�, �̸� �ŷ��ϱ� ���ؼ�
	//			�÷��׸� �״�
	case MP_USERCONN_FORCED_CHANNEL_SYN:
		{
			const MSGBASE* const message = (MSGBASE*)pMsg;
			USERINFO* const userInfo = g_pUserTableForObjectID->FindUser(message->dwObjectID);

			if(userInfo)
			{
				userInfo->bForceChannel = TRUE;
			}
		}
		break;
	// 100302 ONS ����Ʈ�� ���� ���̵� ó�� �߰�.
	case MP_USERCONN_QUEST_CHANGEMAP_SYN:
		{
			const MSG_DWORD3* const message		= ( MSG_DWORD3* )pMsg;
			USERINFO* const userinfo			= g_pUserTableForObjectID->FindUser( message->dwObjectID );
			if( userinfo )
			{
				g_Network.Send2Server(userinfo->dwMapServerConnectionIndex,(char*)pMsg,dwLength);
			}
		}
		break;
	case MP_USERCONN_BILLING_CHECK_SYN:
		{
			const MSGBASE* const receivedMessage = (MSGBASE*)pMsg;
			const DWORD userIndex = receivedMessage->dwObjectID;

			MSGBASE sendMessage;
			ZeroMemory(
				&sendMessage,
				sizeof(sendMessage));
			sendMessage.Category = MP_USERCONN;
			sendMessage.Protocol = BYTE(0 == g_pUserTableForUserID->FindUser(userIndex) ? MP_USERCONN_BILLING_CHECK_NACK : MP_USERCONN_BILLING_CHECK_ACK);
			sendMessage.dwObjectID = userIndex;

			g_Network.Send2DistributeServer(
				(char*)&sendMessage,
				sizeof(sendMessage));
		}
		break;
	case MP_USERCONN_BILLING_START_ACK:
		{
			MSG_PACKET_GAME* const message = (MSG_PACKET_GAME*)pMsg;
			const DWORD userIndex = message->dwObjectID;

			// ���� ���� ��� ��� ������Ʈ�� �� �ֵ��� �÷��̾� ��ȣ�� �������ش�
			if(const USERINFO* const userInfo = g_pUserTableForUserID->FindUser(userIndex))
			{
				message->dwObjectID = userInfo->dwCharacterID;

				g_Network.Send2Server(
					userInfo->dwMapServerConnectionIndex,
					pMsg,
					dwLength);
			}

			// �� ������ ��õ��� ��츦 ����ؼ� �����س��´�
			g_pServerSystem->SetPacketGame(
				userIndex,
				message->mPacketGame);
			break;
		}
	case MP_USERCONN_BILLING_START_NACK:
		{
			const MSGBASE* const message = (MSGBASE*)pMsg;
			const DWORD userIndex = message->dwObjectID;

			g_pServerSystem->RemovePacketGame(
				userIndex);
			break;
		}
	case MP_USERCONN_BILLING_ALERT_ACK:
		{
			MSG_PACKET_GAME* const receivedMessage = (MSG_PACKET_GAME*)pMsg;
			const DWORD userIndex = receivedMessage->dwObjectID;
			
			switch(receivedMessage->mPacketGame.Packet_Result)
			{
			case 1: // ������� ���� �ð�
				{
					if(const USERINFO* const userInfo = g_pUserTableForUserID->FindUser(userIndex))
					{
						MSG_INT2 sendMessage;
						ZeroMemory(
							&sendMessage,
							sizeof(sendMessage));
						sendMessage.Category = MP_USERCONN;
						sendMessage.Protocol = MP_USERCONN_BILLING_ALERT_ACK;
						sendMessage.nData1 = receivedMessage->mPacketGame.Packet_Result;
						sendMessage.nData2 = receivedMessage->mPacketGame.Bill_Remain;

						g_Network.Send2User(
							userInfo->dwConnectionIndex,
							(char*)&sendMessage,
							sizeof(sendMessage));
					}

					break;
				}
			case 0: // ���� �ð� ����. ���� ����
			case -1: // ȯ�� ��ġ. ���� ����
			case -2: // �ߺ� �α���. ���� ����
			case -4: // ��Ÿ ���� ����
				{
					g_pServerSystem->RemovePacketGame(
						userIndex);

					if(const USERINFO* const userInfo = g_pUserTableForUserID->FindUser(userIndex))
					{
						MSGBASE sendMessage;
						ZeroMemory(
							&sendMessage,
							sizeof(sendMessage));
						sendMessage.Category = MP_USERCONN;
						sendMessage.Protocol = MP_USERCONN_BILLING_STOP_ACK;
						sendMessage.dwObjectID = userInfo->dwCharacterID;

						g_Network.Send2Server(
							userInfo->dwMapServerConnectionIndex,
							(char*)&sendMessage,
							sizeof(sendMessage));
					}

					break;
				}
				// ����� ���� ���� ����. ���� ����
			case -3:
				{
					if(const USERINFO* const userInfo = g_pUserTableForUserID->FindUser(userIndex))
					{
						MSGBASE sendMessage;
						ZeroMemory(
							&sendMessage,
							sizeof(sendMessage));
						sendMessage.Category = MP_USERCONN;
						sendMessage.Protocol = MP_USERCONN_BILLING_START_ACK;
						sendMessage.dwObjectID = userInfo->dwCharacterID;

						g_Network.Send2Server(
							userInfo->dwMapServerConnectionIndex,
							(char*)&sendMessage,
							sizeof(sendMessage));
					}

					break;
				}
			}
		}
		break;
	default:
		g_Network.Send2User((MSGBASE*)pTempMsg,dwLength);
		break;
	}
}

void UserConn_Notify_UserLogin_Syn(DWORD dwConnectionIndex, char* pMsg) 
{
	MSG_DWORD3BYTE2* pmsg = (MSG_DWORD3BYTE2*)pMsg;

	DWORD UserID				= pmsg->dwObjectID;
	DWORD DistAuthKey			= pmsg->dwData1;
	DWORD DistConnectionIndex	= pmsg->dwData2;
	BYTE UserLevel				= pmsg->bData1;

	if(g_bReady == FALSE)
	{
		MSG_DWORD3 msg;
		msg.Category	= MP_USERCONN;
		msg.Protocol	= MP_USERCONN_NOTIFY_USERLOGIN_NACK;
		msg.dwObjectID	= UserID;
		msg.dwData1		= DistAuthKey;
		msg.dwData2		= LOGIN_ERROR_NOAGENTSERVER;
		msg.dwData3		= DistConnectionIndex;
		g_Network.Send2Server( dwConnectionIndex, (char*)&msg, sizeof(msg) );
		return;
	}

	//������ �̹� �� �α��� ���̵� �����ϴ��� ����. �̷����� �������� �ȵ�.
	//��¥�� ���� agent�� üũ�� �ȴ�. �ٸ� agent�� ���?
	//�׷��Ƿ�, ������ DB�� LoginCheck�ۿ� ����! �ݵ�� ��Ȯ�� �����ؾ��Ѵ�.
	//agent������ �׳� ���α׷� ������ �ʰ� ����� ���ư��� �ϴ°��� ����!
	USERINFO* pPreInfo = g_pUserTableForUserID->FindUser(UserID);
	if( pPreInfo )	//������ ������ ���������� ���.(�����ϱ�?)
	{
		if( pPreInfo->dwConnectionIndex )		
		{
			DWORD dwPreConIdx = pPreInfo->dwConnectionIndex;
			//OnDisconnectUser( dwPreConIdx );
			DisconnectUser( dwPreConIdx );	
			//���� �������� ���߿� OnDiconnectUser�� ȣ��� ���� �ִ�.
			//���� ���� ���������� ������ ����ó�� �ϰ�, ���߿� OnDisconnect�� ȣ��Ǿ� ó���� �� �ֵ��� �Ѵ�.

			MSG_DWORD3 msg;
			msg.Category	= MP_USERCONN;
			msg.Protocol	= MP_USERCONN_NOTIFY_USERLOGIN_NACK;
			msg.dwObjectID	= UserID;
			msg.dwData1		= DistAuthKey;
			msg.dwData2		= LOGIN_ERROR_NOAGENTSERVER;	//�ӽ÷� ����Ѵ�. ���� �߰�����.
			msg.dwData3		= DistConnectionIndex;
			g_Network.Send2Server( dwConnectionIndex, (char*)&msg, sizeof(msg) );
			return;
		}
		else
		{
			char temp[128];
			sprintf( temp, "[ERROR]: NOTIFY_USERLOGIN - OConIdx:%d, OUsrIdx:%d, OCharIdx:%d, NewUsrIdx:%d", pPreInfo->dwConnectionIndex, pPreInfo->dwUserID, pPreInfo->dwCharacterID, UserID );
			g_Console.LOG( 4, temp );
			//���ӵǾ� ������? �׷���찡 �߻��ϴ��� �ַܼα׸� ����. OldConIdx: 0 �̾�� �Ѵ�!

			g_pUserTableForUserID->RemoveUser( UserID );
#ifdef _NPROTECT_
			if( pPreInfo->m_pCSA )
			{
				pPreInfo->m_pCSA->Close();
				g_CCSAuth2Pool.Free(pPreInfo->m_pCSA);
			}
#endif

			memset( pPreInfo, 0, sizeof(USERINFO) );
			g_UserInfoPool.Free( pPreInfo );
		}
	}

	USERINFO* pInfo = g_UserInfoPool.Alloc();
	//���࿡ pInfo�� NULL�̶�� �׳� ���� ���°� ����. ���̻� ������Ʈ�� �Ҵɻ��°� �ǹǷ�.
	if( pInfo == NULL )
	{	//agent disable.
		MSGBASE msg;
		msg.Category	= MP_SERVER;
		msg.Protocol	= MP_SERVER_AGENTDISABLE;

		g_Network.Send2Server( dwConnectionIndex, (char*)&msg, sizeof(msg) );

		g_Console.LOG( 4, "Agent Disable : Can't accept users!!" );
		return;
	}

	memset( pInfo, 0, sizeof(USERINFO) );
	
	pInfo->dwUserID						= UserID;
	pInfo->dwLastConnectionCheckTime	= gCurTime;
	pInfo->dwUniqueConnectIdx			= g_pServerSystem->MakeAuthKey();
	pInfo->DistAuthKey					= DistAuthKey;
	pInfo->UserLevel					= UserLevel;

#ifdef _NPROTECT_
	pInfo->m_pCSA = g_CCSAuth2Pool.Alloc();
	pInfo->m_pCSA->Init();
#endif			
	g_pUserTableForUserID->AddUser( pInfo, UserID );

	MSG_DWORD3 msg;
	msg.Category	= MP_USERCONN;
	msg.Protocol	= MP_USERCONN_NOTIFY_USERLOGIN_ACK;
	msg.dwObjectID	= UserID;
	msg.dwData1		= DistAuthKey;
	msg.dwData2		= pInfo->dwUniqueConnectIdx;
	msg.dwData3		= DistConnectionIndex;
	
	g_Network.Send2Server( dwConnectionIndex, (char*)&msg, sizeof(msg) );


//---KES PUNISH
	PunishListLoad( UserID );
//-------------
}


void UserConn_NotifyToAgent_AlreadyOut(DWORD dwConnectionIndex, char* pMsg) 
{
	//�α��� ���� ��û�ϰ� Ŭ���̾�Ʈ�� ������. �׷��� ������ ��������.
	MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
	DWORD UserID		= pmsg->dwData1;
	DWORD AgentAuthKey	= pmsg->dwData2;

	USERINFO* pInfo = g_pUserTableForUserID->FindUser( UserID );

	if( pInfo )
	if( pInfo->dwUniqueConnectIdx == AgentAuthKey )
	{
		LoginCheckDelete(UserID);
		g_pUserTableForUserID->RemoveUser(UserID);
#ifdef _NPROTECT_
		if( pInfo->m_pCSA )
		{
			pInfo->m_pCSA->Close();
			g_CCSAuth2Pool.Free(pInfo->m_pCSA);
		}
#endif
		memset( pInfo, 0, sizeof(USERINFO) );
		g_UserInfoPool.Free( pInfo );
	}
}


void UserConn_Notify_OverlappedLogin(DWORD dwConnectionIndex, char* pMsg) 
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	DWORD UserID = pmsg->dwData;
	USERINFO* pInfo = g_pUserTableForUserID->FindUser(UserID);
	if(pInfo == NULL)
	{
		return;
	}

	MSGBASE msg;
	msg.Category = MP_USERCONN;
	msg.Protocol = MP_USERCONN_OTHERUSER_CONNECTTRY_NOTIFY;
	g_Network.Send2User(pInfo->dwConnectionIndex,(char*)&msg,sizeof(msg));
}


void UserConn_Force_Disconnect_OverlapLogin(DWORD dwConnectionIndex, char* pMsg) 
{
	MSGBASE* pmsg = (MSGBASE*)pMsg;

	DWORD UserID = pmsg->dwObjectID;

	USERINFO* pUserInfo = g_pUserTableForUserID->FindUser(UserID);

	if(pUserInfo == NULL)
	{
		return;
	}
	
	DWORD dwUserConIndex = pUserInfo->dwConnectionIndex;
	
	if(dwUserConIndex)
	{
		MSGBASE msg;
		msg.Category = MP_USERCONN;
		msg.Protocol = MP_USERCONN_DISCONNECTED_BY_OVERLAPLOGIN;
		g_Network.Send2User(dwUserConIndex,(char*)&msg,sizeof(msg));

		//INFORM TO MAPSERVER : FOR NO DELAY DISCONNECTING. : no wait player exit
		if( pUserInfo->dwCharacterID )
		{
			SERVERINFO* pServerInfo = g_pServerTable->FindServerForConnectionIndex(pUserInfo->dwMapServerConnectionIndex);
			if( pServerInfo )
			{
				MSGBASE msgToMap;
				msgToMap.Category = MP_USERCONN;
				msgToMap.Protocol = MP_USERCONN_NOWAITEXITPLAYER;
				msgToMap.dwObjectID	= pUserInfo->dwCharacterID;
				g_Network.Send2Server( pServerInfo->dwConnectionIndex, (char*)&msgToMap, sizeof(msgToMap) );
			}
		}

		OnDisconnectUser( dwUserConIndex );
		DisconnectUser( dwUserConIndex );
	}
	else	//Ŀ�ؼ��� ������ CharcterID�� ������ ����. ���̺������� �����ش�.
	{
		LoginCheckDelete(UserID);
		g_pUserTableForUserID->RemoveUser(UserID);
#ifdef _NPROTECT_
		if( pUserInfo->m_pCSA )
		{
			pUserInfo->m_pCSA->Close();
			g_CCSAuth2Pool.Free(pUserInfo->m_pCSA);			
		}
#endif
		memset( pUserInfo, 0, sizeof(USERINFO) );
		g_UserInfoPool.Free( pUserInfo );                
	}	
}


void UserConn_Disconnected_On_Login(char* pMsg) 
{
	// 
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	DWORD UserID = pmsg->dwObjectID;
	DWORD AuthKey = pmsg->dwData;

	USERINFO* pInfo = g_pUserTableForUserID->FindUser(UserID);
	if(pInfo == NULL)
	{
		return;
	}

	if( AuthKey != pInfo->DistAuthKey )
	{
		return;
	}

	LoginCheckDelete(UserID);
	g_pUserTableForUserID->RemoveUser(UserID);
#ifdef _NPROTECT_
	if( pInfo->m_pCSA )
	{
		pInfo->m_pCSA->Close();
		g_CCSAuth2Pool.Free(pInfo->m_pCSA);
	}
#endif
	memset( pInfo, 0, sizeof(USERINFO) );	//�߰� 060414 KES
	g_UserInfoPool.Free( pInfo );
}


void UserConn_Disconnect_Syn(DWORD dwConnectionIndex) 
{
	OnDisconnectUser( dwConnectionIndex );
	DisconnectUser( dwConnectionIndex );
}


void UserConn_Connection_Check_Ok(DWORD dwConnectionIndex) 
{
	USERINFO* pInfo = g_pUserTable->FindUser(dwConnectionIndex);
	if(!pInfo)
		return;
	pInfo->bConnectionCheckFailed = FALSE;
}


void UserConn_Characterlist_Syn(DWORD dwConnectionIndex, char* pMsg) 
{
	MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
			
	DWORD UserID		= pmsg->dwData1;
	DWORD DistAuthKey	= pmsg->dwData2;

	USERINFO* pInfo = g_pUserTableForUserID->FindUser(UserID);

	if(!pInfo)
	{
		MSGBASE msg;
		msg.Category = MP_USERCONN;
		msg.Protocol = MP_USERCONN_CHARACTERLIST_NACK;
		g_Network.Send2User(dwConnectionIndex,(char*)&msg,sizeof(msg));
		//���� ��������� ���� ����.
		OnDisconnectUser( dwConnectionIndex );
		DisconnectUser( dwConnectionIndex );
		return;
	}

	if(pInfo->DistAuthKey != DistAuthKey)	//������ �ٸ������� �Դ�.
	{

		MSGBASE msg;
		msg.Category = MP_USERCONN;
		msg.Protocol = MP_USERCONN_CHARACTERLIST_NACK;
		g_Network.Send2User(dwConnectionIndex,(char*)&msg,sizeof(msg));
		//���� ��������� ���� ����.
		OnDisconnectUser( dwConnectionIndex );
		DisconnectUser( dwConnectionIndex );
		return;
	}

	//---KES Network Process Fix 071114
	USERINFO* pPreInfo = g_pUserTable->FindUser(dwConnectionIndex);
	if( pPreInfo == NULL )	//���������� ������ �����̴�. ��������� �����ΰ�?
	{
		char temp[128];
		sprintf( temp, "[ERROR]: NO PreInfo - UsrIdx:%d", UserID );
		g_Console.LOG( 4, temp );
		//���������� ���־��ϳ�?
		return;
	}

//---KES Crypt
#ifdef _CRYPTCHECK_ 
	pInfo->crypto.Create();	// key ����
#endif

	pInfo->dwConnectionIndex = dwConnectionIndex;
	pInfo->mLoginCheckTime = gCurTime;
	pInfo->mLoginCheck = TRUE;

	//������ ��ü���ش�.
	g_pUserTable->RemoveUser(dwConnectionIndex);	//�ӽ����� ����
	g_UserInfoPool.Free( pPreInfo );				//�ӽ����� ����
	g_pUserTable->AddUser(pInfo,dwConnectionIndex);	//���ο� ������ ��ü

	UserIDXSendAndCharacterBaseInfo(
		UserID,pInfo->dwUniqueConnectIdx,
		dwConnectionIndex);
	GetUserSexKind(
		pInfo->dwUserID);
}


void UserConn_DirectCharacterList_Syn(DWORD dwConnectionIndex) 
{
	USERINFO* pInfo = g_pUserTable->FindUser(dwConnectionIndex);
	if(!pInfo)
		return;
	ASSERT(pInfo->dwUserID);
	if(pInfo->dwUserID == 0)
	{
		ASSERTMSG(0, "UserID�����ˢ� 0AI�ˡ�U.");
		return;
	}
	
	UserIDXSendAndCharacterBaseInfo(pInfo->dwUserID,pInfo->dwUniqueConnectIdx,dwConnectionIndex);
}


void UserConn_CharacterSelect_Syn(DWORD dwConnectionIndex, char* pMsg)
{
	MSG_WORD* pmsg = (MSG_WORD*)pMsg;
	USERINFO* pInfo = g_pUserTable->FindUser(dwConnectionIndex);
	if( !pInfo )	return;
	
	CHARSELECTINFO* SelInfoArray = (CHARSELECTINFO*)pInfo->SelectInfoArray;
	DWORD playerID	= pmsg->dwObjectID;
	MAPTYPE mapnum = 0;

	pInfo->wChannel = pmsg->wData;	//ä���� ��Ƶд�.

	for(int i = 0; i < MAX_CHARACTER_NUM; i++)
	{
		if(SelInfoArray[i].dwCharacterID == playerID)
		{
			mapnum = SelInfoArray[i].MapNum;
			break;
		}
		if(i == MAX_CHARACTER_NUM - 1)
		{
			MSGBASE msg;
			msg.Category = MP_USERCONN;
			msg.Protocol = MP_USERCONN_CHARACTERSELECT_NACK;
			g_Network.Send2User(dwConnectionIndex, (char*)&msg, sizeof(msg));
			return;
		}
	}
	
	SaveMapChangePointUpdate(playerID, 0);
	UnRegistLoginMapInfo(playerID);

	// desc_hseos_���͹���01
	// S ���͹��� �߰� added by hseos 2007.05.29
	// ..ó�� ���ӽÿ� �÷���Ÿ��, ���� ų ���� �ʱ�ȭ ���ش�.
	InitMonstermeterInfoOfDB(playerID);
	// E ���͹��� �߰� added by hseos 2007.05.29

	WORD mapport = g_pServerTable->GetServerPort( eSK_MAP, mapnum);

	if(mapport)
	{
		// 080424 LYW --- AgentNetworkMsgPasrser : ĳ���� �ƿ� ó��.
		USERINFO* pUserInfo = NULL ;
		pUserInfo = g_pUserTableForObjectID->FindUser(playerID) ;
		if(pUserInfo)
		{
			CHATROOMMGR->DestroyPlayer_From_Lobby(pUserInfo) ;
		}

		SERVERINFO* pSInfo = g_pServerTable->FindServer(mapport);
		MSG_BYTE msg;
		msg.Category = MP_USERCONN;
		msg.Protocol = MP_USERCONN_CHARACTERSELECT_ACK;
		msg.bData = (BYTE)mapnum;
		msg.dwObjectID = playerID;	
		g_Network.Send2User(dwConnectionIndex, (char*)&msg, sizeof(msg));

		USERINFO* pPreInfo = g_pUserTableForObjectID->FindUser(playerID);
		if( pPreInfo ) //�̹� ������Ʈ�� �ִٸ�..
		{
			char temp[128];
			sprintf( temp, "[ERROR]: CHARACTERSELECT - OConIdx:%d, OUsrIdx:%d, OCharIdx:%d, NewUsrIdx:%d", pPreInfo->dwConnectionIndex, pPreInfo->dwUserID, pPreInfo->dwCharacterID, pInfo->dwUserID );
			g_Console.LOG( 4, temp );

			g_pUserTableForObjectID->RemoveUser(playerID);

			//map���� ����ֳ�? ��������� ������.
			if( pPreInfo->dwMapServerConnectionIndex )
			{
				SERVERINFO* pServerInfo = g_pServerTable->FindServerForConnectionIndex(pPreInfo->dwMapServerConnectionIndex);
				if( pServerInfo )
				{
					MSG_DWORD msg;
					msg.Category	= MP_USERCONN;
					msg.Protocol	= MP_USERCONN_GAMEOUT_SYN;	//�ʿ��� ������ �׳� ���ִ� ���̴�.
					msg.dwObjectID	= pPreInfo->dwCharacterID;
					msg.dwData		= pPreInfo->dwUserID;
					g_Network.Send2Server(pPreInfo->dwMapServerConnectionIndex, (char*)&msg, sizeof(msg));
				}
			}
		}
		//////////////////////////////////////////////////////////////////////////
		
		g_pUserTableForObjectID->AddUser(pInfo, playerID);

		pInfo->wUserMapNum = (WORD)mapnum;
		pInfo->dwCharacterID = playerID;
		pInfo->dwMapServerConnectionIndex = pSInfo->dwConnectionIndex;
	}
	else
	{
		MSG_BYTE msg;
		msg.Category = MP_USERCONN;
		msg.Protocol = MP_USERCONN_CHARACTERSELECT_NACK;
		msg.bData = (BYTE)mapnum;
		msg.dwObjectID = playerID;
		g_Network.Send2User(dwConnectionIndex, (char*)&msg, sizeof(MSG_BYTE));
	}
}


void UserConn_ChannelInfo_Syn(DWORD dwConnectionIndex, char* pMsg) 
{
	MSGBASE* pmsg = (MSGBASE*)pMsg;
	USERINFO* pInfo = g_pUserTable->FindUser(dwConnectionIndex);
	if(!pInfo)
		return;
	CHARSELECTINFO * SelectInfoArray = (CHARSELECTINFO*)pInfo->SelectInfoArray;
	CHARSELECTINFO * pSelectedInfo;
	MSG_DWORD msg;
	msg.Category = MP_USERCONN;
	msg.Protocol = MP_USERCONN_CHANNELINFO_SYN;
	msg.dwObjectID = dwConnectionIndex;
	msg.dwData = pInfo->dwUniqueConnectIdx;
	
	MAPTYPE mapnum = 0;
	for(int i = 0; i < MAX_CHARACTER_NUM; i++)
	{
		if(SelectInfoArray[i].dwCharacterID == pmsg->dwObjectID)
		{
			pSelectedInfo = &SelectInfoArray[i];
			mapnum = pSelectedInfo->MapNum;
			break;
		}
		if(i == MAX_CHARACTER_NUM - 1)	// A?Au�����ˢ� �����ˢ�Ao����i AO�ˡ�A A�ϩ��ˡ�?����u �ˡ�U�ˡ���I A�ϩ��ˡ�?AI ��u����AA��iE
		{
			MSG_BYTE msg;
			msg.Category	= MP_USERCONN;
			msg.Protocol	= MP_USERCONN_CHANNELINFO_NACK;
			msg.bData		= 1;	//A?AuA�ϩ��ˡ�?A�ϡ��ˡ��Ϩ���u����AA
			g_Network.Send2User(dwConnectionIndex, (char*)&msg, sizeof(msg));
			return;
		}
	}
	WORD mapport = g_pServerTable->GetServerPort( eSK_MAP, mapnum);
	if(mapport)
	{
		SERVERINFO* pSInfo = g_pServerTable->FindServer(mapport);
		g_Network.Send2Server(pSInfo->dwConnectionIndex, (char*)&msg, sizeof(MSG_DWORD));
	}
	else
	{
		MSG_BYTE msg;
		msg.Category	= MP_USERCONN;
		msg.Protocol	= MP_USERCONN_CHANNELINFO_NACK;
		msg.bData		= 0;	//��u���Ϩ�oo�ϡ�����A�ϩ��ˡ�U.
		g_Network.Send2User(dwConnectionIndex, (char*)&msg, sizeof(msg));
	}
}


void UserConn_ChannelInfo_Ack(char* pMsg, DWORD dwLength)
{
	MSG_CHANNEL_INFO* pmsg = (MSG_CHANNEL_INFO*)pMsg;
	USERINFO* pInfo = g_pUserTable->FindUser(pmsg->dwObjectID);
	if(pInfo == NULL)
		return;
	if(pInfo->dwUniqueConnectIdx != pmsg->dwUniqueIDinAgent)
		return;
	g_Network.Send2User(pmsg->dwObjectID, (char*)pMsg, dwLength);
}


void UserConn_ChannelInfo_Nack(char* pMsg, DWORD dwLength) 
{
	MSG_BYTE* pmsg = (MSG_BYTE*)pMsg;
	USERINFO* pInfo = g_pUserTable->FindUser(pmsg->dwObjectID);
	if(pInfo == NULL)
		return;
	g_Network.Send2User(pmsg->dwObjectID, (char*)pMsg, dwLength);
}


void UserConn_Gamein_Syn(DWORD dwConnectionIndex, char* pMsg)
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	USERINFO* pInfo = g_pUserTable->FindUser(dwConnectionIndex);
	if(!pInfo)
		return;

	MSG_NAME_DWORD4 msg;
	ZeroMemory(
		&msg,
		sizeof(msg));
	msg.Category = MP_USERCONN;
	msg.Protocol = MP_USERCONN_GAMEIN_SYN;
	msg.dwObjectID = pInfo->dwCharacterID;
	msg.dwData1 = pInfo->dwUniqueConnectIdx;
	// 091228 LUJ, PC�� ������ �����ϱ� ���� IP ������ �����Ѵ�
	WORD port = 0;
	g_Network.GetUserAddress(
		pInfo->dwConnectionIndex,
		msg.Name,
		&port);
	
	if(pInfo->bForceMove)	// GM������ �̵� or ġƮ�̵�
	{
		if(g_pServerSystem->GetChannelNum(pInfo->wUserMapNum) < pmsg->dwData)
		{
			// ��ȿ ä�η� �̵��ϰ��� �ϴ� ��� 1��ä�η� �̵�
			msg.dwData2 = 0;
		}
		else
		{
			//ä���� ����Ȱ�� ����
			if(pInfo->wChannel != pmsg->dwData)
			{
				msg.dwData2 = pmsg->dwData;
				pInfo->wChannel = (WORD)pmsg->dwData;
			}
			//�䱸��� �̵�
			else
			{
				msg.dwData2 = pmsg->dwData;
			}
		}

		pInfo->bForceMove = FALSE;
	}
	// 091106 LUJ, Ŭ���̾�Ʈ���� ������ ä�� ���� �Ͻ������� ����Ѵ�.
	//			A�ʿ��� B������ �̵���ų��, A���� ������ ä�η� �������� �Ҷ� ����Ѵ�
	else if(pInfo->bForceChannel)
	{
		if(pInfo->wChannel <= g_pServerSystem->GetChannelNum(pInfo->wUserMapNum))
		{
			msg.dwData2 = WORD(pmsg->dwData);
		}

		pInfo->bForceChannel = FALSE;
	}
	else	// �α��� or ��Ż�� ���� �����̵�
	{
		if(pInfo->wChannel <= g_pServerSystem->GetChannelNum(pInfo->wUserMapNum))
		{
			// �����̵�
			msg.dwData2 = pInfo->wChannel;
		}
	}

	/*
	// ���� ä�η� �̵��ϰ��� �ϴ� ��� 1��ä�η� �̵���Ų��.
	if( g_pServerSystem->GetChannelNum(pInfo->wUserMapNum) <  pInfo->wChannel)
	{
		msg.dwData2 = 0;
	}
	else
	{
		if(pInfo->wChannel <= g_pServerSystem->GetChannelNum(pInfo->wUserMapNum))
		{
			msg.dwData2 = pInfo->wChannel;
		}
		else
		{
			msg.dwData2 = pmsg->dwData;
		}
	}
	*/

	// desc_hseos_����Ʈ ��_01
	// S ����Ʈ �� �߰� added by hseos 2007.11.25
	// ..ç���� �������� �̵��̶�� ä�ι�ȣ�� ���ΰ� ��Ʈ������ �˸��� ç���� �� ���� ID�� �����Ѵ�.
	// ..�ʼ������� �� ID�� �����ؼ� ä���� ������.
	if (g_csDateManager.IsChallengeZone(pInfo->wUserMapNum))
	{
		msg.dwData2 = pInfo->nChallengeZoneEnterID;
		msg.dwData4 = pInfo->nChallengeZoneEnterSection;
		pInfo->nChallengeZoneEnterID = 0;
		pInfo->nChallengeZoneEnterSection = 0;
	}
	// E ����Ʈ �� �߰� added by hseos 2007.11.25
	msg.dwData3 = pInfo->UserLevel;

	g_Network.Send2Server(pInfo->dwMapServerConnectionIndex, (char*)&msg, sizeof(msg));

	// 080331 LYW --- AgentNetworkMsgParser : Add player to chatroom system.
	if(!CHATROOMMGR->IsAddedUser(pInfo->dwCharacterID))
	{
		CHATROOMMGR->RegistPlayer_To_Lobby(pInfo, MP_CHATROOM_ADD_USER_SYN) ;
	}

	{
		MSG_PACKET_GAME message;
		ZeroMemory(
			&message,
			sizeof(message));
		message.Category = MP_USERCONN;
		message.Protocol = MP_USERCONN_BILLING_UPDATE_ACK;
		message.mPacketGame = g_pServerSystem->GetPacketGame(
			pInfo->dwUserID);
		message.dwObjectID = pInfo->dwCharacterID;

		g_Network.Send2Server(
			pInfo->dwMapServerConnectionIndex,
			(char*)&message,
			sizeof(message));
	}
}


void UserConn_Gamein_Nack(char* pMsg) 
{
	//������ �����ش�.
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	USERINFO* pUserInfo = g_pUserTableForObjectID->FindUser(pmsg->dwData);
	if( pUserInfo )
	{
		DWORD dwConIdx = pUserInfo->dwConnectionIndex;
		OnDisconnectUser( dwConIdx );
		DisconnectUser( dwConIdx );
	}
}


void UserConn_Gamein_Ack(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength) 
{
	SEND_HERO_TOTALINFO* pmsg = (SEND_HERO_TOTALINFO*)pMsg;
	DWORD uniqueID = pmsg->UniqueIDinAgent;
	DWORD CharacterID = pmsg->BaseObjectInfo.dwObjectID;
	USERINFO* pUserInfo = g_pUserTableForObjectID->FindUser(CharacterID);

	CHARSELECTINFO * SelectInfoArray = (CHARSELECTINFO*)pUserInfo->SelectInfoArray;
//			g_Console.Log(eLogDisplay,4,"GameinAck %d",CharacterID);
	if(pUserInfo == NULL)
	{				
		MSGBASE msg;
		msg.Category = MP_USERCONN;
		msg.Protocol = MP_USERCONN_GAMEIN_NACK;
		msg.dwObjectID = CharacterID;
		g_Network.Send2Server(dwConnectionIndex,(char*)&msg,sizeof(msg));
		return;
	}
	if(pUserInfo->dwUniqueConnectIdx != uniqueID)
	{
		MSGBASE msg;
		msg.Category = MP_USERCONN;
		msg.Protocol = MP_USERCONN_GAMEIN_NACK;
		msg.dwObjectID = CharacterID;
		g_Network.Send2Server(dwConnectionIndex,(char*)&msg,sizeof(msg));

		return;
	}
	// 100618 ������ ĳ���� ���̵��� �ʹ�ȣ ����
	for(int i = 0; i < MAX_CHARACTER_NUM; i++)
	{
		if(SelectInfoArray[i].dwCharacterID == pmsg->dwObjectID)
		{
			SelectInfoArray[i].MapNum	=	pUserInfo->wUserMapNum;
			break;
		}
	}


	// desc_hseos_����Ʈ��Ī01
	// S ����Ʈ��Ī �߰� added by hseos 2007.11.20	2008.01.02
	// ..����Ʈ ��Ī ä�� ���̾��ٸ� ��뿡�� ������ ������� �˸���.
	g_csResidentRegistManager.ASRV_EndDateMatchingChat(pUserInfo);
	// E ����Ʈ��Ī �߰� added by hseos 2007.11.20	2008.01.02

	pUserInfo->DateMatchingInfo	=	pmsg->ChrTotalInfo.DateMatching;			// ���ӳ� '�˻�' ������ ������Ʈ�� ������ �÷��̾�(ĳ����)������ �ӽ� ����. �� �����ִ°� �߿��� ��.
//	pUserInfo->DateMatchingInfo
	strcpy( pUserInfo->DateMatchingInfo.szName, pmsg->BaseObjectInfo.ObjectName);		// ĳ���� �̸�.
	pUserInfo->DateMatchingInfo.nRequestChatState = NULL;
	pUserInfo->DateMatchingInfo.nChatPlayerID = NULL;
    pUserInfo->mFamilyIndex = 0;

	g_Network.Send2User((MSGBASE*)pMsg,dwLength);
	MSG_DWORD msgCheck;
	msgCheck.Category = MP_USERCONN;
	msgCheck.Protocol = MP_USERCONN_GAMEIN_OTHERMAP_SYN;
	msgCheck.dwObjectID = CharacterID;

	g_Network.Broadcast2MapServerExceptOne(dwConnectionIndex, (char*)&msgCheck, sizeof(msgCheck));

	Family_LoadInfo(
		pUserInfo->dwCharacterID);
	Family_Leave_LoadInfo(
		pUserInfo->dwCharacterID);
	CHATROOMMGR->UpdatePlayerInfo(pUserInfo) ;
}


void UserConn_Character_Make_Syn(DWORD dwConnectionIndex, char* pMsg)
{
	CHARACTERMAKEINFO* pmsg = (CHARACTERMAKEINFO*)pMsg;
	USERINFO* pInfo = g_pUserTable->FindUser(dwConnectionIndex);
	if(!pInfo)
		return;
	
// ĳ���� ���� ���� üũ ---------------------------------------------------------
	// 100302 ONS ���� ĳ���� ������ �������� üũó�� �߰�
	if( !CheckCharacterMakeInfo( pmsg ) ||
		(pmsg->RaceType == RaceType_Devil && !CheckDevilMakeLevel( pInfo )) )
	{
		MSGBASE msg;
		msg.Category = MP_USERCONN;
		msg.Protocol = MP_USERCONN_CHARACTER_MAKE_NACK;
		g_Network.Send2User(dwConnectionIndex, (char*)&msg, sizeof(msg));
		return;
	}
//---------------------------------------------------------------------------------

	DWORD UserID = pInfo->dwUserID;
	pmsg->UserID = UserID;
#ifndef _SELECT_GENDER_
	// desc_hseos_��������01
	// S �������� �߰� added by hseos 2007.06.16
	// ..DB���� �о�� ������ �ξ��� ���������� ����
	pmsg->SexType = (BYTE)pInfo->nSexKind;	
	// E �������� �߰� added by hseos 2007.06.16
#endif
	//string overflow����
	char buf[MAX_NAME_LENGTH+1];
	SafeStrCpy( buf, pmsg->Name, MAX_NAME_LENGTH+1 );

	if( FILTERTABLE->IsUsableName( buf ) &&
		!FILTERTABLE->IsInvalidCharInclude( (unsigned char*)buf ) )
	{			
		CreateCharacter(pmsg, (WORD)g_nServerSetNum, dwConnectionIndex);
	}
	else
	{
		MSGBASE msg;
		msg.Category = MP_USERCONN;
		msg.Protocol = MP_USERCONN_CHARACTER_MAKE_NACK;
		g_Network.Send2User(dwConnectionIndex, (char*)&msg, sizeof(msg));
	}
}


void UserConn_Character_NameCheck_Syn(DWORD dwConnectionIndex, char* pMsg)
{
	MSG_NAME* pmsg = (MSG_NAME*)pMsg;
			
	//string overflow����
	char buf[MAX_NAME_LENGTH+1];
	SafeStrCpy( buf, pmsg->Name, MAX_NAME_LENGTH+1 );
	
	if( FILTERTABLE->IsUsableName( buf ) &&
		!FILTERTABLE->IsInvalidCharInclude( (unsigned char*)buf ) )
	{
		CharacterNameCheck( buf, dwConnectionIndex);
	}
	else
	{
		MSG_WORD msg;
		msg.Category = MP_USERCONN;
		msg.Protocol = MP_USERCONN_CHARACTER_NAMECHECK_NACK;
		msg.wData = 2;
		g_Network.Send2User(dwConnectionIndex, (char*)&msg, sizeof(msg));
	}	
}


void UserConn_Character_Remove_Syn(DWORD dwConnectionIndex, char* pMsg) 
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	DeleteCharacter(pmsg->dwData, (WORD)g_nServerSetNum, dwConnectionIndex);
}


// desc_hseos_��ȥ_01
// S ��ȥ �߰� added by hseos 2008.01.30
//void UserConn_ChangeMap_Syn(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength) 
void UserConn_ChangeMap_Syn(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength, VECTOR3* pPos)
// E ��ȥ �߰� added by hseos 2008.01.30
{
	MSG_DWORD3* pmsg = (MSG_DWORD3*)pMsg;

	WORD tomapserverport = 0;

	MAPCHANGE_INFO* pChangeInfo = g_pServerSystem->GetMapChangeInfo(pmsg->dwData1);
	if( pmsg->dwData1 < 2000 )
	{
		if(!pChangeInfo)
		{	
			// �ˡ�EAI ��u�Ϩ�A�ˡ��ˡ�e ����A����I
			MSGBASE msg;
			msg.Category = MP_USERCONN;
			msg.Protocol = MP_USERCONN_CHANGEMAP_NACK;
			msg.dwObjectID = pmsg->dwObjectID;
			g_Network.Send2User((MSGBASE*)&msg, sizeof(MSGBASE));
			return;
		}
		tomapserverport = g_pServerTable->GetServerPort( eSK_MAP, pChangeInfo->MoveMapNum);
	}
	else
		tomapserverport = g_pServerTable->GetServerPort( eSK_MAP, (WORD)(pmsg->dwData1-2000));

	if(tomapserverport)
	{
		USERINFO* pInfo = g_pUserTable->FindUser(dwConnectionIndex);
		if( !pInfo ) return;

		if( GMINFO->IsEventStarted() )
		if( pChangeInfo->MoveMapNum == 59 )
		{
			MSGBASE msg;
			msg.Category = MP_USERCONN;
			msg.Protocol = MP_USERCONN_CHANGEMAP_NACK;
			msg.dwObjectID = pmsg->dwObjectID;
			g_Network.Send2User((MSGBASE*)&msg, sizeof(MSGBASE));
			return;
		}		


		// desc_hseos_��ȥ_01
		// S ��ȥ �߰� added by hseos 2008.01.30
		// g_Network.Send2Server(pInfo->dwMapServerConnectionIndex,(char*)pMsg,dwLength);
		MSG_DWORD5 stPacket;
		ZeroMemory(&stPacket, sizeof(stPacket));

		stPacket.Category	= pmsg->Category;
		stPacket.Protocol	= pmsg->Protocol;
		stPacket.dwObjectID	= pmsg->dwObjectID;
		stPacket.dwData1	= pmsg->dwData1;
		stPacket.dwData2	= pmsg->dwData2;
		stPacket.dwData3	= pmsg->dwData3;
		if (pPos)
		{
			stPacket.dwData4	= (DWORD)pPos->x;
			stPacket.dwData5	= (DWORD)pPos->z;
		}

		g_Network.Send2Server(pInfo->dwMapServerConnectionIndex,(char*)&stPacket, sizeof(stPacket));
		// E ��ȥ �߰� added by hseos 2008.01.30

		//SERVERINFO* pSInfo = g_pServerTable->FindServer((DWORD)tomapserverport);
		//���ϡ�a���� ��O CIAo?
		//pInfo->wUserMapNum = (WORD)pChangeInfo->MoveMapNum;		// ��oU�ϡ��ϡ� �ˡ�E ��o�Ϩ�E��I����I ��uA����A
		//pInfo->dwMapServerConnectionIndex = pSInfo->dwConnectionIndex;	// ��oU�ϡ��ϡ� �ˡ�E dwConnectionIndex����I ��uA����A
	}
	else
	{
		// �ˡ�EAI ��u�Ϩ�A�ˡ��ˡ�e ����A����I
		MSGBASE msg;
		msg.Category = MP_USERCONN;
		msg.Protocol = MP_USERCONN_CHANGEMAP_NACK;
		msg.dwObjectID = pmsg->dwObjectID;
		g_Network.Send2User((MSGBASE*)&msg, sizeof(MSGBASE));
		return;
	}
}

// 070917 LYW --- AgentNetworkMsgParser : Add function to process change map through the npc.
void UserConn_Npc_ChangeMap_Syn(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength) 
{
	USERINFO* pInfo = g_pUserTable->FindUser(dwConnectionIndex);
	if( !pInfo ) return;

	MSG_DWORD3* pmsg = (MSG_DWORD3*)pMsg;

	WORD tomapserverport = 0;
	tomapserverport = g_pServerTable->GetServerPort( eSK_MAP, (WORD)pmsg->dwData1);

	if(!tomapserverport)
	{
		MSGBASE msg;
		msg.Category = MP_USERCONN;
		msg.Protocol = MP_USERCONN_NPC_CHANGEMAP_NACK;
		msg.dwObjectID = pmsg->dwObjectID;
		g_Network.Send2User((MSGBASE*)&msg, sizeof(MSGBASE));
		return;
	}

	g_Network.Send2Server(pInfo->dwMapServerConnectionIndex,(char*)pMsg,dwLength);
}

void UserConn_Return_Syn(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength) 
{
	MSG_WORD* pmsg = (MSG_WORD*)pMsg;
	LOGINPOINT_INFO* pLoginInfo = g_pServerSystem->GetLoginInfo(pmsg->wData);

	if( !pLoginInfo )
		return;

	WORD tomapserverport = 0;

	tomapserverport = g_pServerTable->GetServerPort( eSK_MAP, pLoginInfo->MapNum );

	if(tomapserverport)
	{
		USERINFO* pInfo = g_pUserTable->FindUser(dwConnectionIndex);
		if( !pInfo ) return;


		g_Network.Send2Server(pInfo->dwMapServerConnectionIndex,(char*)pMsg,dwLength);

	}
	else
	{
		MSGBASE msg;
		msg.Category = MP_USERCONN;
		msg.Protocol = MP_USERCONN_CHANGEMAP_NACK;
		msg.dwObjectID = pmsg->dwObjectID;
		g_Network.Send2User((MSGBASE*)&msg, sizeof(MSGBASE));
		return;
	}
}

void UserConn_Map_Out(char* pMsg)
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;

	MAPCHANGE_INFO* ChangeInfo = NULL;
	WORD mapport = 0;

	if( pmsg->dwData < 2000 )
	{
		ChangeInfo = g_pServerSystem->GetMapChangeInfo(pmsg->dwData);
		mapport = g_pServerTable->GetServerPort( eSK_MAP, ChangeInfo->MoveMapNum);
	}
	else
		mapport = g_pServerTable->GetServerPort( eSK_MAP, (WORD)(pmsg->dwData-2000));

	if(mapport)
	{
		MSG_DWORD msgserver;
		msgserver.Category = MP_USERCONN;
		msgserver.Protocol = MP_USERCONN_CHANGEMAP_ACK;
		if( pmsg->dwData < 2000 )
			msgserver.dwData = ChangeInfo->MoveMapNum;
		else
			msgserver.dwData = pmsg->dwData-2000;
		msgserver.dwObjectID = pmsg->dwObjectID;	
		g_Network.Send2User((MSG_DWORD*)&msgserver, sizeof(MSG_DWORD));

		SERVERINFO* pSInfo = g_pServerTable->FindServer(mapport);
		USERINFO* pInfo = g_pUserTableForObjectID->FindUser(pmsg->dwObjectID);

		if(pInfo == NULL)
			return;

		if( pmsg->dwData < 2000 )
			pInfo->wUserMapNum = (WORD)ChangeInfo->MoveMapNum;
		else
			pInfo->wUserMapNum = (WORD)pmsg->dwData-2000;				
		pInfo->dwMapServerConnectionIndex = pSInfo->dwConnectionIndex;
	}
}

// 070917 LYW --- AgentNetworkMsgParser : Add function to process map out through the npc.
void UserConn_Npc_Map_Out(char* pMsg)
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;

	WORD mapport = 0;

	mapport = g_pServerTable->GetServerPort( eSK_MAP, (WORD)pmsg->dwData);

	if(mapport)
	{
		MSG_DWORD msgserver;
		msgserver.Category = MP_USERCONN;
		msgserver.Protocol = MP_USERCONN_NPC_CHANGEMAP_ACK;
		msgserver.dwObjectID = pmsg->dwObjectID;	

		msgserver.dwData = pmsg->dwData ;

		g_Network.Send2User((MSG_DWORD*)&msgserver, sizeof(MSG_DWORD));

		SERVERINFO* pSInfo = g_pServerTable->FindServer(mapport);
		USERINFO* pInfo = g_pUserTableForObjectID->FindUser(pmsg->dwObjectID);

		if(pInfo == NULL)
			return;

		pInfo->wUserMapNum = (WORD)pmsg->dwData ;			
		pInfo->dwMapServerConnectionIndex = pSInfo->dwConnectionIndex;
	}
}


void UserConn_Map_Out_WithMapNum(char* pMsg)
{
	MSG_WORD2* pmsg = (MSG_WORD2*)pMsg;
	WORD mapport = g_pServerTable->GetServerPort( eSK_MAP, pmsg->wData1);
	if(mapport)
	{
		USERINFO* pInfo = g_pUserTableForObjectID->FindUser( pmsg->dwObjectID );
		if( !pInfo ) return;

//---KES ä���̵� ����
		MSG_DWORD2 msgserver;
		msgserver.Category		= MP_USERCONN;
		msgserver.Protocol		= MP_USERCONN_CHEAT_CHANGEMAP_ACK;
		msgserver.dwData1		= pmsg->wData1;
		// 080319 KTH -- wData2�� ���� (WORD)(-1)�� ���� ���� UserInfo(pInfo)���� ä�� ������ �����´�.
		if( pmsg->wData2 != (WORD)(-1) )
			msgserver.dwData2	= pmsg->wData2;
		else
			msgserver.dwData2	= pInfo->wChannel;
		msgserver.dwObjectID	= pmsg->dwObjectID;

		g_Network.Send2User( &msgserver, sizeof(msgserver) );
//---------------------
		/////////////////
		SERVERINFO* pSInfo	= g_pServerTable->FindServer(mapport);
		pInfo->wUserMapNum	= pmsg->wData1;
		pInfo->dwMapServerConnectionIndex = pSInfo->dwConnectionIndex;
	}
}

void UserConn_Option_Syn(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	const MSG_OPT* const message = (MSG_OPT*)pMsg;
	USERINFO* const userInfo = g_pUserTable->FindUser(
		dwConnectionIndex);

	if(0 == userInfo)
	{
		return;
	}

	userInfo->GameOption.bNoFriend = BYTE(LOWORD(message->dwData4));
	userInfo->GameOption.bNoWhisper = BYTE(HIWORD(message->dwData4));
	userInfo->GameOption.bNoDateMatching = BYTE(message->dwData5);
	TransToMapServerMsgParser(
		dwConnectionIndex,pMsg,dwLength);
}

void SiegeWarfare_Map_Out_WithMapNum( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength )
{
	if( !pMsg ) return ;


	MSG_WORD* pmsg = NULL ;
	pmsg = (MSG_WORD*)pMsg ;

	if( !pmsg ) return ;

	SERVERINFO* pInfo = NULL ;
	pInfo = g_pServerTable->FindMapServer( pmsg->wData ) ;

	if( pInfo ) 
	{
		MSG_WORD msg ;

		msg.Category	= MP_SIEGEWARFARE ;
		msg.Protocol	= MP_USERCONN_SIEGEWARFARE_MAP_OUT_WITHMAPNUM_ACK ;

		msg.dwObjectID	= pmsg->dwObjectID ;

		msg.wData		= pmsg->wData ;

		g_Network.Send2Server( dwConnectionIndex, (char*)&msg, sizeof(MSGBASE) ) ;
	}
	else
	{
		MSGBASE msg ;

		msg.Category	= MP_SIEGEWARFARE ;
		msg.Protocol	= MP_USERCONN_SIEGEWARFARE_MAP_OUT_WITHMAPNUM_NACK ;

		msg.dwObjectID	= pmsg->dwObjectID ;

		g_Network.Send2Server( dwConnectionIndex, (char*)&msg, sizeof(MSGBASE) ) ;
	}
}

#define HOUSINGMAPNUM	99

void UserConn_HouseEntrance_Syn(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSG_HOUSE_VISIT* pmsg = (MSG_HOUSE_VISIT*)pMsg;

	WORD tomapserverport = 0;
	tomapserverport = g_pServerTable->GetServerPort( eSK_MAP, HOUSINGMAPNUM );

	if(tomapserverport)
	{
		USERINFO* pInfo = g_pUserTable->FindUser(dwConnectionIndex);
		if( !pInfo ) return;

		g_Network.Send2Server(pInfo->dwMapServerConnectionIndex,pMsg, dwLength);
	}
	else
	{
		MSG_DWORD4 msg;
		msg.Category = MP_HOUSE;
		msg.Protocol = MP_HOUSE_ENTRANCE_NACK;
		msg.dwObjectID = pmsg->dwObjectID;
		msg.dwData1 = eHOUSEERR_ERROR;
		msg.dwData2 = pmsg->cKind;
		msg.dwData3 = pmsg->dwValue1;
		msg.dwData4 = pmsg->dwValue2;
		g_Network.Send2User((MSG_DWORD4*)&msg, sizeof(MSG_DWORD4));
		return;
	}
}

void UserConn_DungeonEntrace_Syn(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength) 
{
	const MSG_DWORD4* const pmsg = (MSG_DWORD4*)pMsg;
	const WORD tomapserverport = g_pServerTable->GetServerPort(eSK_MAP, WORD(pmsg->dwData2));

	if(tomapserverport)
	{
		USERINFO* pInfo = g_pUserTable->FindUser(dwConnectionIndex);
		if( !pInfo ) return;

		g_Network.Send2Server(pInfo->dwMapServerConnectionIndex,pMsg, dwLength);
	}
	else
	{
		MSG_DWORD msg;
		msg.Category = MP_DUNGEON;
		msg.Protocol = MP_DUNGEON_ENTRANCE_NACK;
		msg.dwObjectID = pmsg->dwObjectID;
		msg.dwData = eDungeonERR_DungeonMapOff;
		g_Network.Send2User(&msg, sizeof(msg));
	}
}

void UserConn_ChangeMap_Ack(MSGROOT* pTempMsg, DWORD dwLength)
{
	g_Network.Send2User((MSGBASE*)pTempMsg,dwLength);
}


void UserConn_SavePoint_Syn(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	USERINFO* pInfo = g_pUserTable->FindUser(dwConnectionIndex);
	if(!pInfo)
		return;
	g_Network.Send2Server(pInfo->dwMapServerConnectionIndex,pMsg,dwLength);
}


void UserConn_BackToCharSel_Syn(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	USERINFO* pInfo = g_pUserTable->FindUser(dwConnectionIndex);
	if(!pInfo)
		return;

	CSHFamily* const family = g_csFamilyManager.GetFamily(
		pInfo->mFamilyIndex);

	g_csFamilyManager.ASRV_SendMemberConToOtherAgent(
		family,
		pInfo->dwCharacterID,
		CSHFamilyMember::MEMBER_CONSTATE_LOGOFF);
	g_csFamilyManager.DelFamilyToTbl(
		pInfo->dwCharacterID,
		family);
	g_csFamilyManager.ASRV_SendInfoToClient(
		family,
		2);
	pInfo->mFamilyIndex = 0;
	pInfo->nFamilyLeaveDate = 0;
	pInfo->nFamilyLeaveKind = 0;
	// E �йи� �߰� added by hseos 2007.07.11

	// desc_hseos_����Ʈ��Ī01
	// S ����Ʈ��Ī �߰� added by hseos 2007.11.20	2008.01.02
	// ..����Ʈ ��Ī ä�� ���̾��ٸ� ��뿡�� ������ ������� �˸���.
	g_csResidentRegistManager.ASRV_EndDateMatchingChat(pInfo);
	g_Network.Send2Server(pInfo->dwMapServerConnectionIndex,pMsg,dwLength);
}


void UserConn_BackToCharSel_Ack(char* pMsg)
{
	MSGBASE* pmsg = (MSGBASE*)pMsg;
	USERINFO * pInfo = (USERINFO*)g_pUserTableForObjectID->FindUser(pmsg->dwObjectID);
	if(!pInfo)
		return;

//KES 031111
	DWORD UserID = pInfo->dwUserID;
	UserIDXSendAndCharacterBaseInfo(UserID, pInfo->dwUniqueConnectIdx, pInfo->dwConnectionIndex);

	g_pUserTableForObjectID->RemoveUser(pInfo->dwCharacterID);
	pInfo->dwCharacterID = 0;
	pInfo->dwMapServerConnectionIndex = 0;
	pInfo->wUserMapNum = 0;

	// 080331 LYW --- AgentNetworkMsgParser : Add player to chatroom system.
	//CHATROOMMGR->DestroyPlayer_From_Lobby(pInfo) ;
}


void UserConn_Cheat_Using(char* pMsg)
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	CheatLog(pmsg->dwObjectID,pmsg->dwData);
}


void UserConn_LoginCheck_Delete(char* pMsg) 
{
	MSGBASE* pmsg = (MSGBASE*)pMsg;

	if( pmsg->dwObjectID )
	{
		//no user in agent	//any other agent?
		if( g_pUserTableForUserID->FindUser(pmsg->dwObjectID) == NULL )
		{
			LoginCheckDelete( pmsg->dwObjectID );
		}
	}
}


void UserConn_GameInPos_Syn(DWORD dwConnectionIndex, char* pMsg) 
{
	//	MSGBASE* pmsg = (MSGBASE*)pMsg;
	MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
	USERINFO* pInfo = g_pUserTable->FindUser(dwConnectionIndex);
	if(!pInfo)
		return;

	MSG_NAME_DWORD4 msg;
	ZeroMemory(
		&msg,
		sizeof(msg));
	msg.Category = MP_USERCONN;
	msg.Protocol = MP_USERCONN_GAMEINPOS_SYN;
	msg.dwObjectID = pInfo->dwCharacterID;
	msg.dwData1 = pInfo->dwUniqueConnectIdx;
	// 091228 LUJ, PC�� ������ �����ϱ� ���� IP ������ �����Ѵ�
	WORD port = 0;
	g_Network.GetUserAddress(
		pInfo->dwConnectionIndex,
		msg.Name,
		&port);
	
	if( pInfo->UserLevel < eUSERLEVEL_SUPERUSER )
	{
		msg.dwData2 = pmsg->dwData1;				//ä�ι�ȣ
		pInfo->wChannel = (WORD)pmsg->dwData1;
	}
	else
	{
		msg.dwData2	= pInfo->wChannel;	//����� ä�ι�ȣ�� ��������.
	}
	msg.dwData3 = pInfo->UserLevel;
	msg.dwData4 = pmsg->dwData2;

	g_Network.Send2Server(pInfo->dwMapServerConnectionIndex, (char*)&msg, sizeof(msg));

	{
		MSG_PACKET_GAME message;
		ZeroMemory(
			&message,
			sizeof(message));
		message.Category = MP_USERCONN;
		message.Protocol = MP_USERCONN_BILLING_CHECK_ACK;
		message.mPacketGame = g_pServerSystem->GetPacketGame(
			pInfo->dwUserID);
		message.dwObjectID = pInfo->dwUserID;

		g_Network.Send2Server(
			pInfo->dwMapServerConnectionIndex,
			(char*)&message,
			sizeof(message));
	}
}

void UserConn_EventItem_Use(char* pMsg, DWORD dwLength) 
{
	MSG_NAME* pmsg = (MSG_NAME*)pMsg;
	USERINFO* pInfo = g_pUserTableForObjectID->FindUser( pmsg->dwObjectID );
	if( !pInfo )	return;

	// 090325 ONS String Overflow ����
	char szBuf[MAX_NAME_LENGTH+1];
	SafeStrCpy( szBuf, pmsg->Name, MAX_NAME_LENGTH+1 );

	EventItemUse051108( pInfo->dwUserID, szBuf, g_nServerSetNum );
	g_Network.Send2User( (MSGBASE*)pMsg, dwLength );
}


void UserConn_EventItem_Use2(char* pMsg, DWORD dwLength) 
{
	MSG_NAME* pmsg = (MSG_NAME*)pMsg;
	USERINFO* pInfo = g_pUserTableForObjectID->FindUser( pmsg->dwObjectID );
	if( !pInfo )	return;

	// 090325 ONS String Overflow ����
	char szBuf[MAX_NAME_LENGTH+1];
	SafeStrCpy( szBuf, pmsg->Name, MAX_NAME_LENGTH+1 );

	EventItemUse2( pInfo->dwUserID, szBuf, g_nServerSetNum );
	g_Network.Send2User( (MSGBASE*)pMsg, dwLength );
}



void MP_CHATServerMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGBASE* pTempMsg = (MSGBASE*)pMsg;

	switch(pTempMsg->Protocol)
	{
	case MP_CHAT_ALL:		
	case MP_CHAT_SMALLSHOUT:
	case MP_CHAT_WHOLE:
	case MP_CHAT_TRADE:
	case MP_CHAT_GM_SMALLSHOUT:
	case MP_CHAT_MONSTERSPEECH:
	case MP_CHAT_NORMAL:
	case MP_CHAT_FORBIDCHAT:
	case MP_CHAT_PERMITCHAT:
		{
			TransToClientMsgParser(
				dwConnectionIndex,
				pMsg,
				dwLength);
			break;
		}
	case MP_CHAT_WHISPER_SYN:
		{
			Chat_Whisper_Syn(
				dwConnectionIndex,
				pMsg);
			break;
		}
	case MP_CHAT_WHISPER_GM_SYN:
		{
			Chat_Whisper_Gm_Syn(
				dwConnectionIndex,
				pMsg);
			break;
		}
	case MP_CHAT_WHISPER_ACK:
		{
			Chat_Whisper_Ack(
				pMsg,
				dwLength);
			break;
		}
	case MP_CHAT_WHISPER_NACK:
		{
			Chat_Whisper_Nack(
				pMsg,
				dwLength);
			break;
		}
	case MP_CHAT_PARTY:
		{
			Chat_Party(pMsg);
			break;
		}
	case MP_CHAT_GUILD:
		{
			Chat_Guild(
				pMsg,
				dwLength);
			break;
		}
	case MP_CHAT_FAMILY:
		{
			Chat_Family(
				pMsg,
				dwLength);
			break;
		}
	case MP_CHAT_GUILDUNION:
		{
			Chat_GuildUnion(
				pMsg,
				dwLength);
			break;
		}
	case MP_CHAT_SHOUT_ACK:
		{
			Chat_Shout_Ack(
				pMsg);
			break;
		}
	}
}

void Chat_Whisper_Syn(DWORD dwConnectionIndex, char* pMsg)
{
	MSG_WHISPER* pmsg = (MSG_WHISPER*)pMsg;
	USERINFO* pReceiverInfo = g_pUserTableForObjectID->FindUser( pmsg->dwReceiverID );
	if( !pReceiverInfo ) return;

	if( pReceiverInfo->GameOption.bNoWhisper )
	{
		MSG_BYTE msg;
		msg.Category	= MP_CHAT;
		msg.Protocol	= MP_CHAT_WHISPER_NACK;
		msg.dwObjectID	= pmsg->dwObjectID;	//�����ˡ��ϩ���o��ic��O��A ��u����AI��i�ϡ�
		msg.bData		= CHATERR_OPTION_NOWHISPER;

		g_Network.Send2Server(dwConnectionIndex, (char*)&msg, sizeof(msg));
		return;
	}

	//---�����ˡ��ϩ���o��ic��O��AAI AO�ˡ�A ��?�ˢ�AIA?������c����I
	MSG_CHAT msgChat;
	msgChat.Category	= MP_CHAT;
	msgChat.Protocol	= MP_CHAT_WHISPER_ACK;
	msgChat.dwObjectID	= pmsg->dwObjectID;	//�����ˡ��ϩ���o��ic��O��A
	SafeStrCpy( msgChat.Name, pmsg->ReceiverName, MAX_NAME_LENGTH+1 );	//��i����i���ˡ�A��ic��O��AAC AI�ˡ�����
	SafeStrCpy( msgChat.Msg, pmsg->Msg, MAX_CHAT_LENGTH+1 );
	g_Network.Send2Server( dwConnectionIndex, (char*)&msgChat, msgChat.GetMsgLength() );

	//---��i����i���ˡ�A��ic��O��"?�ˢ碮��O �����ˡ��ϩ���i����a
	msgChat.Protocol	= MP_CHAT_WHISPER;
	SafeStrCpy( msgChat.Name, pmsg->SenderName, MAX_NAME_LENGTH+1 ); //�����ˡ��ϩ���o��ic��O��AAC AI�ˡ�����
	g_Network.Send2User( pReceiverInfo->dwConnectionIndex, (char*)&msgChat, msgChat.GetMsgLength() );	//CHATMSG 040324
}


void Chat_Whisper_Gm_Syn(DWORD dwConnectionIndex, char* pMsg) 
{
	MSG_WHISPER* pmsg = (MSG_WHISPER*)pMsg;
	USERINFO* pReceiverInfo = g_pUserTableForObjectID->FindUser( pmsg->dwReceiverID );
	if( !pReceiverInfo ) return;

	MSG_CHAT msgChat;
	msgChat.Category	= MP_CHAT;
	msgChat.Protocol	= MP_CHAT_WHISPER_ACK;
	msgChat.dwObjectID	= pmsg->dwObjectID;	
	SafeStrCpy( msgChat.Name, pmsg->ReceiverName, MAX_NAME_LENGTH+1 );
	SafeStrCpy( msgChat.Msg, pmsg->Msg, MAX_CHAT_LENGTH+1 );
	g_Network.Send2Server( dwConnectionIndex, (char*)&msgChat, msgChat.GetMsgLength() );

	msgChat.Protocol	= MP_CHAT_WHISPER_GM;
	SafeStrCpy( msgChat.Name, pmsg->SenderName, MAX_NAME_LENGTH+1 );
	g_Network.Send2User( pReceiverInfo->dwConnectionIndex, (char*)&msgChat, msgChat.GetMsgLength() );	//CHATMSG 040324
}


void Chat_Whisper_Ack(char* pMsg, DWORD dwLength) 
{
	MSG_CHAT* pmsg = (MSG_CHAT*)pMsg;
	USERINFO* pInfo = g_pUserTableForObjectID->FindUser( pmsg->dwObjectID );			
	if( !pInfo ) return;

	g_Network.Send2User( pInfo->dwConnectionIndex, (char*)pmsg, dwLength );
}


void Chat_Whisper_Nack(char* pMsg, DWORD dwLength) 
{
	MSG_BYTE* pmsg = (MSG_BYTE*)pMsg;
	USERINFO* pSenderInfo = g_pUserTableForObjectID->FindUser( pmsg->dwObjectID );
	if( !pSenderInfo ) return;
	
	g_Network.Send2User( pSenderInfo->dwConnectionIndex, (char*)pmsg, dwLength );
}


void Chat_Party(char* pMsg) 
{
	MSG_CHAT_WITH_SENDERID* pmsg = (MSG_CHAT_WITH_SENDERID*)pMsg;
	USERINFO* pReceiverInfo = g_pUserTableForObjectID->FindUser( pmsg->dwObjectID );
	if( !pReceiverInfo ) return;
	
	g_Network.Send2User( pReceiverInfo->dwConnectionIndex, (char*)pmsg, pmsg->GetMsgLength() );	//CHATMSG 040324
}


void Chat_Guild(char* pMsg, DWORD dwLength) 
{
	TESTMSGID * pmsg = (TESTMSGID*)pMsg;
	USERINFO* pReceiverInfo = g_pUserTableForObjectID->FindUser( pmsg->dwObjectID );
	if( !pReceiverInfo ) return;
	
	TransToClientMsgParser(pReceiverInfo->dwConnectionIndex, pMsg, dwLength);
}

// 070601 LYW --- Add function for famil chatting.
void Chat_Family( char* pMsg, DWORD dwLength )
{
	MSG_CHAT* pstPacket = (MSG_CHAT*)pMsg;
	CSHFamily* pcsFamily = g_csFamilyManager.GetFamily(pstPacket->dwObjectID);
	if (pcsFamily == NULL) return;

	MSG_CHAT stPacket;
	USERINFO* pMemberInfo;
	for(UINT i=0; i<pcsFamily->Get()->nMemberNum; i++)
	{
		pMemberInfo = g_pUserTableForObjectID->FindUser(pcsFamily->GetMember(i)->Get()->nID);
		if (pMemberInfo == NULL) continue;

		stPacket.Category		= MP_CHAT;
		stPacket.Protocol		= MP_CHAT_FAMILY;
		stPacket.dwObjectID		= pMemberInfo->dwCharacterID;
		SafeStrCpy(stPacket.Name, pstPacket->Name, MAX_NAME_LENGTH+1);
		SafeStrCpy(stPacket.Msg, pstPacket->Msg, MAX_CHAT_LENGTH+1);

		g_Network.Send2User(&stPacket, sizeof(stPacket));
	}
	// E �йи� �߰� added by hseos 2007.11.08
}


void Chat_GuildUnion(char* pMsg, DWORD dwLength)
{
	MSG_CHAT_WITH_SENDERID* pmsg = (MSG_CHAT_WITH_SENDERID*)pMsg;
	USERINFO* pReceiverInfo = g_pUserTableForObjectID->FindUser( pmsg->dwObjectID );
	if( !pReceiverInfo ) return;
	
	TransToClientMsgParser(pReceiverInfo->dwConnectionIndex, pMsg, dwLength);
}

void Chat_Shout_Ack(char* pMsg)
{
	SEND_SHOUTBASE_ITEMINFO* pmsg = (SEND_SHOUTBASE_ITEMINFO*)pMsg;	
	MSG_CHAT_WORD msgtoUser;
	ZeroMemory(
		&msgtoUser,
		sizeof(msgtoUser));
	msgtoUser.Category = MP_CHAT;
	msgtoUser.Protocol = MP_CHAT_SHOUT_ACK;
	msgtoUser.dwObjectID = pmsg->dwObjectID;
	msgtoUser.wData = pmsg->wItemSupplyValue;
	_sntprintf(
		msgtoUser.Msg,
		_countof(msgtoUser.Msg),
		"[%s]: %s",
		pmsg->Name,
		pmsg->Msg);

	g_pUserTable->SetPositionUserHead();

	while(const USERINFO* const userInfo = g_pUserTable->GetUserData())
	{
		MSG_CHAT_WORD message = msgtoUser;
		g_Network.Send2User(
			userInfo->dwConnectionIndex,
			LPTSTR(&message),
			message.GetMsgLength());
	}
}

void MP_CHATMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)	//from client
{
	MSGBASE* pTempMsg = (MSGBASE*)pMsg;
	
	USERINFO* pInfoForCheck = g_pUserTable->FindUser( dwConnectionIndex );
	
	if( pInfoForCheck )
	{
		if( gCurTime - pInfoForCheck->dwLastChatTime < 1000 )	//1��
		{
			//����
			MSGBASE msg;
			msg.Category	= MP_CHAT;
			msg.Protocol	= MP_CHAT_FASTCHAT;
			msg.dwObjectID	= pTempMsg->dwObjectID;
			g_Network.Send2User(dwConnectionIndex, (char*)&msg, sizeof(msg));
			return;
		}
		
		pInfoForCheck->dwLastChatTime = gCurTime;

		// 081205 LUJ, GM�� ���� ��ȭ�� �α׿� �����Ѵ�
		switch( pInfoForCheck->UserLevel )
		{
		case eUSERLEVEL_GM:
		case eUSERLEVEL_PROGRAMMER:
		case eUSERLEVEL_DEVELOPER:
			{
				const MSG_CHAT_WORD* const message = ( MSG_CHAT_WORD* )pMsg;

				TCHAR chat[ MAX_CHAT_LENGTH + 1 ] = { 0 };
				SafeStrCpy( chat, message->Msg, sizeof( chat ) );
				
				// 090325 ONS ���͸�����üũ
				if(IsCharInString(chat, "'"))	return;

				g_DB.LogMiddleQuery(
					0,
					0,
					_T( "EXEC dbo.TP_CHAT_LOG_INSERT %d, \'%s\'" ),
					message->dwObjectID,
					chat );
				break;
			}
		}
	}

	switch(pTempMsg->Protocol)
	{
	case MP_CHAT_ALL:
	case MP_CHAT_SMALLSHOUT:
	case MP_CHAT_WHOLE :
	case MP_CHAT_TRADE :
	case MP_CHAT_MONSTERSPEECH:
	case MP_CHAT_NORMAL :					TransToMapServerMsgParser(dwConnectionIndex,pMsg,dwLength);	break;
	case MP_CHAT_WHISPER_SYN:				chatmsg_whisper_syn(dwConnectionIndex, pMsg) ;				break;
	case MP_CHAT_PARTY:						chatmsg_party(pMsg) ;										break;
	case MP_CHAT_GUILD:						chatmsg_guild(pMsg, dwLength) ;								break;
	case MP_CHAT_FAMILY :					chatmsg_family(pMsg, dwLength) ;							break ;
	case MP_CHAT_GUILDUNION:				chatmsg_guildunion(pMsg, dwLength) ;						break;
	// 070321 LYW --- AgentNetworkMsgParser : Add protocol to fine user.
	case MP_CHAT_FINDUSER :					chatmsg_FindUser(dwConnectionIndex, pMsg, dwLength) ;		break ;
	default:
		ASSERT(0);
		//GAMESYSTEM_OBJ->m_ConnectionTable.SendToCharacter(pTempMsg,dwLength);
		break;
	}
}

void chatmsg_FindUser(DWORD dwConnectionIndex, char*pMsg, DWORD dwLength)
{
	//TransToMapServerMsgParser(dwConnectionIndex,pMsg,dwLength);

	MSG_CHAT* pmsg = (MSG_CHAT*)pMsg ;

	char buf[ MAX_NAME_LENGTH+1 ] = {0, } ;

	SafeStrCpy( buf, pmsg->Name, MAX_NAME_LENGTH+1 ) ;

	DWORD dwKeyOut;
	if( g_MsgTable.AddMsg( pmsg, &dwKeyOut ) )
	{
		SearchWhisperUserAndSend( pmsg->dwObjectID, buf, dwKeyOut );
	}
}


void chatmsg_whisper_syn(DWORD dwConnectionIndex, char* pMsg)
{
	MSG_CHAT* pmsg = (MSG_CHAT*)pMsg;

	USERINFO* pInfo = g_pUserTable->FindUser( dwConnectionIndex );
	if( !pInfo )	return;

	//---Ŭ���̾�Ʈ���� �� ������ ���� �� ����. buf�� �ű��.
	char buf[MAX_NAME_LENGTH+1];
	SafeStrCpy( buf, pmsg->Name, MAX_NAME_LENGTH+1 );
	int nLen = strlen( buf );
	if( nLen < 4 ) return;		//ĳ���� �̸��� 4���� �����ϸ��� ����.

	// desc_hseos_�������͸�01
	// S �������͸� �߰� added by hseos 2007.06.25
	// ..���Ϳ��� ������ ���� ó���� ���� Ÿ���� �����Ѵ�.
	FILTERTABLE->SetExceptionInvalidCharIncludeType(FET_WHISPER_CHARNAME);
	// E �������͸� �߰� added by hseos 2007.06.25
	if( (FILTERTABLE->IsInvalidCharInclude((unsigned char*)buf)) == TRUE )
	{
		MSG_BYTE msg;
		msg.Category	= MP_CHAT;
		msg.Protocol	= MP_CHAT_WHISPER_NACK;
		msg.dwObjectID	= pmsg->dwObjectID;
		msg.bData		= CHATERR_NO_NAME;
		g_Network.Send2User(dwConnectionIndex, (char*)&msg, sizeof(msg));
		return;
	}

	DWORD dwKeyOut;
	if( g_MsgTable.AddMsg( pmsg, &dwKeyOut ) )
	{
		SearchWhisperUserAndSend( pmsg->dwObjectID, buf, dwKeyOut );
	}
	else
	{
		MSG_BYTE msg;
		msg.Category	= MP_CHAT;
		msg.Protocol	= MP_CHAT_WHISPER_NACK;
		msg.dwObjectID	= pmsg->dwObjectID;
		msg.bData		= CHATERR_ERROR;
		g_Network.Send2User(dwConnectionIndex, (char*)&msg, sizeof(msg));
		return ;
	}
}


void chatmsg_party(char* pMsg) 
{
	SEND_PARTY_CHAT* pmsg =	(SEND_PARTY_CHAT*)pMsg;
	USERINFO* pSenderInfo = g_pUserTableForObjectID->FindUser(pmsg->dwObjectID);
	if(!pSenderInfo)
		return;
	
	MSG_CHAT_WITH_SENDERID msgChat;
	msgChat.Category	= pmsg->Category;
	msgChat.Protocol	= pmsg->Protocol;
	msgChat.dwSenderID	= pmsg->dwObjectID;

	SafeStrCpy( msgChat.Name, pmsg->Name, MAX_NAME_LENGTH+1 );
	SafeStrCpy( msgChat.Msg, pmsg->Msg, MAX_CHAT_LENGTH+1 );

	for(int i=0; i<pmsg->MemberNum; ++i)
	{
		if(pmsg->MemberID[i] != 0)
		{
			USERINFO* pRecverInfo = g_pUserTableForObjectID->FindUser(pmsg->MemberID[i]);
			msgChat.dwObjectID	= pmsg->MemberID[i];
			if(pRecverInfo)
			{
//						g_Network.Send2User(pRecverInfo->dwConnectionIndex, (char*)&msgChat, msgChat.GetMsgLength() );	//CHATMSG 040324
				MSG_CHAT_WITH_SENDERID msgTemp = msgChat;
				g_Network.Send2User(pRecverInfo->dwConnectionIndex, (char*)&msgTemp, msgTemp.GetMsgLength() );	//CHATMSG 040324
			}
			else
			{						
				g_Network.Broadcast2AgentServerExceptSelf( (char*)&msgChat, msgChat.GetMsgLength() );
			}
		}
	}
}


void chatmsg_guild(char* pMsg, DWORD dwLength) 
{
// 090317 ONS ������Ʈ������ ��Ŷ���� ����ó���� ���� �޼��� ����
//  TESTMSGID -> MSG_GUILD_CHAT
//  pmsg->dwSenderID -> pmsg->dwObjectID
	MSG_GUILD_CHAT* pmsg = (MSG_GUILD_CHAT*)pMsg;
	USERINFO* pSenderInfo = g_pUserTableForObjectID->FindUser(pmsg->dwObjectID);
	if(!pSenderInfo)
		return;
	g_Network.Broadcast2MapServer(pMsg, dwLength);
}

// 070601 LYW --- AgentNetMsgParser : Add function for family chatting.
void chatmsg_family(char* pMsg, DWORD dwLength)
{
	MSG_CHAT* const pstPacket = (MSG_CHAT*)pMsg;
	USERINFO* const pUserInfo = g_pUserTableForObjectID->FindUser(pstPacket->dwObjectID);

	if(pUserInfo == NULL) return;

	CSHFamily* const family = g_csFamilyManager.GetFamily(
		pUserInfo->mFamilyIndex);

	if(0 == family) return;

	MSG_CHAT stPacket;
	
	for(UINT i = 0; i < family->Get()->nMemberNum; ++i)
	{
		const USERINFO* const pMemberInfo = g_pUserTableForObjectID->FindUser(
			family->GetMember(i)->Get()->nID);

		if (pMemberInfo == NULL) continue;

		stPacket.Category		= MP_CHAT;
		stPacket.Protocol		= MP_CHAT_FAMILY;
		stPacket.dwObjectID		= pMemberInfo->dwCharacterID;
		SafeStrCpy(stPacket.Name, pstPacket->Name, MAX_NAME_LENGTH+1);
		SafeStrCpy(stPacket.Msg, pstPacket->Msg, MAX_CHAT_LENGTH+1);

		g_Network.Send2User(&stPacket, sizeof(stPacket));
	}

	// �ٸ� ������Ʈ���� ������
	stPacket.Category		= MP_CHAT;
	stPacket.Protocol		= MP_CHAT_FAMILY;
	stPacket.dwObjectID		= family->Get()->nID;
	SafeStrCpy(stPacket.Name, pstPacket->Name, MAX_NAME_LENGTH+1);
	SafeStrCpy(stPacket.Msg, pstPacket->Msg, MAX_CHAT_LENGTH+1);
	g_Network.Broadcast2AgentServerExceptSelf( (char*)&stPacket, sizeof(stPacket));
	// E �йи� �߰� added by hseos 2007.07.13	2007.11.08
}


void chatmsg_guildunion(char* pMsg, DWORD dwLength) 
{
// 090317 ONS ������Ʈ������ ��Ŷ���� ����ó���� ���� �޼��� ����
//  MSG_CHAT_WITH_SENDERID -> MSG_GUILDUNION_CHAT
//  pmsg->dwSenderID -> pmsg->dwObjectID
	MSG_GUILDUNION_CHAT* pmsg = (MSG_GUILDUNION_CHAT*)pMsg;
	USERINFO* pSenderInfo = g_pUserTableForObjectID->FindUser( pmsg->dwObjectID );
	if( !pSenderInfo )	return;
	g_Network.Broadcast2MapServer( pMsg, dwLength );
}



void MP_PARTYServerMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGBASE* pTempMsg = (MSGBASE*)pMsg;
	switch(pTempMsg->Protocol)
	{
	case MP_PARTY_NOTIFYADD_TO_MAPSERVER:
		{
			g_Network.Broadcast2MapServerExceptOne(dwConnectionIndex, pMsg, dwLength);
		}
		break;
	case MP_PARTY_NOTIFYDELETE_TO_MAPSERVER:
		{
			g_Network.Broadcast2MapServerExceptOne(dwConnectionIndex, pMsg, dwLength);
		}
		break;
	case MP_PARTY_NOTIFYCHANGEMASTER_TO_MAPSERVER:
		{
			g_Network.Broadcast2MapServerExceptOne(dwConnectionIndex, pMsg, dwLength);
		}
		break;
	case MP_PARTY_NOTIFYBREAKUP_TO_MAPSERVER:
		{
			g_Network.Broadcast2MapServerExceptOne(dwConnectionIndex, pMsg, dwLength);
		}
		break;
	case MP_PARTY_NOTIFYBAN_TO_MAPSERVER:
		{
			g_Network.Broadcast2MapServerExceptOne(dwConnectionIndex, pMsg, dwLength);
		}
		break;
	case MP_PARTY_NOTIFYMEMBER_LOGIN_TO_MAPSERVER:
		{
			g_Network.Broadcast2MapServerExceptOne(dwConnectionIndex, pMsg, dwLength);
		}
		break;
	case MP_PARTY_NOTIFYMEMBER_LOGOUT_TO_MAPSERVER:
		{
			g_Network.Broadcast2MapServerExceptOne(dwConnectionIndex, pMsg, dwLength);
		}
		break;
	case MP_PARTY_NOTIFYMEMBER_LOGINMSG:
		{
			g_Network.Broadcast2MapServerExceptOne(dwConnectionIndex,pMsg,dwLength);			
		}
		break;
	case MP_PARTY_NOTIFYCREATE_TO_MAPSERVER:
		{
			g_Network.Broadcast2MapServerExceptOne(dwConnectionIndex,pMsg,dwLength);
		}
		break;
	case MP_PARTY_NOTIFYMEMBER_LEVEL:
		{
			g_Network.Broadcast2MapServerExceptOne(dwConnectionIndex,pMsg,dwLength);
		}
		break;

	// 071002 LYW --- AgentNetworkMsgParser : Add process to change option of party.
	case MP_PARTY_NOTIFY_CHANGE_OPTION :
		{
			g_Network.Broadcast2MapServerExceptOne(dwConnectionIndex,pMsg,dwLength);
		}
		break ;

	// 090526 ShinJS --- ��Ƽ�ʴ� ������ ��� ���� ( ���� Agent/���� �ٸ� Map or ���� �ٸ� Agent )
	case MP_PARTY_INVITE_BYNAME_ACK:
		{
			MSG_NAME_DWORD5* pmsg = (MSG_NAME_DWORD5*)pMsg;
			const DWORD dwRequestPlayerID =	pmsg->dwData1;
			const DWORD dwInvitedPlayerID = pmsg->dwData2;

			// �ʴ� ��û��
			USERINFO* pRequestPlayer = g_pUserTableForObjectID->FindUser( dwRequestPlayerID );
			if( !pRequestPlayer )
				break;

			// �ʴ� ���
			USERINFO* pInvitedPlayer = g_pUserTableForObjectID->FindUser( dwInvitedPlayerID );

			// ���� �ٸ� Agent
			if( !pInvitedPlayer )
			{
				// �ʴ� ��󿡰� �ʴ� ���� ������ ������ ���� �Ҽ�Agent�� ã�´�
				pmsg->Protocol = MP_PARTY_INVITE_BYNAME_ACCEPT_SYN;
				g_Network.Broadcast2AgentServer( (char*)pmsg, sizeof(MSG_NAME_DWORD4) );
			}
			// ���� Agent/���� �ٸ� Map
			else
			{
				const DWORD dwPartyIdx = pmsg->dwData3;
				const DWORD dwPartyOption = pmsg->dwData4;
				const DWORD dwPartyDiceOption = pmsg->dwData5;

				// �ʴ� ��󿡰� �ʴ� ������ ���� ������ ������
				MSG_NAME_DWORD4 inviteMsg;
				inviteMsg.Category = MP_PARTY;
				inviteMsg.Protocol = MP_PARTY_ADD_INVITE;
				inviteMsg.dwData1 = dwPartyIdx;
				inviteMsg.dwData2 = dwPartyOption;
				inviteMsg.dwData3 = dwRequestPlayerID;
				inviteMsg.dwData4 = dwPartyDiceOption;
				SafeStrCpy( inviteMsg.Name, pmsg->Name, MAX_NAME_LENGTH+1 );
				g_Network.Send2User( pInvitedPlayer->dwConnectionIndex, (char*)&inviteMsg, sizeof(inviteMsg) );
			}

			// �ʴ� ��û�ڿ��� ���������� �˸���
			MSGBASE rtMsg;
			rtMsg.Category = MP_PARTY;
			rtMsg.Protocol = MP_PARTY_INVITE_ACCEPT_ACK;
			g_Network.Send2User( pRequestPlayer->dwConnectionIndex, (char*)&rtMsg, sizeof(rtMsg) );
		}
		break;

	// 090526 ShinJS --- ��Ƽ �ʴ� ������ ���� ��Ŷ�� ���� ��� (Broadcast Agent Server)
	case MP_PARTY_INVITE_BYNAME_ACCEPT_SYN:
		{
			MSG_NAME_DWORD4* pmsg = (MSG_NAME_DWORD4*)pMsg;
			const DWORD dwInvitedPlayerID = pmsg->dwData2;

			// �ʴ� ���
			USERINFO* pInvitedPlayer = g_pUserTableForObjectID->FindUser( dwInvitedPlayerID );

			// Agent�� ���� ������ �ִ� ���
			if( pInvitedPlayer )
			{
				const DWORD dwPartyIdx = pmsg->dwData3;
				const DWORD dwPartyOption = pmsg->dwData4;

				// �ʴ� ��󿡰� �ʴ� ������ ���� ������ ������
				MSG_NAME_DWORD2 inviteMsg;
				inviteMsg.Category = MP_PARTY;
				inviteMsg.Protocol = MP_PARTY_ADD_INVITE;
				inviteMsg.dwData1	= dwPartyIdx;
				inviteMsg.dwData2	= dwPartyOption;
				SafeStrCpy( inviteMsg.Name, pmsg->Name, MAX_NAME_LENGTH+1 );
				g_Network.Send2User( pInvitedPlayer->dwConnectionIndex, (char*)&inviteMsg, sizeof(inviteMsg) );
			}
		}
		break;

	// 090526 ShinJS --- ��Ƽ �ʴ��û�� ������ ��� (��û�ڿ� ��û����� �����ʿ� ���� ���)
	case MP_PARTY_INVITE_DENY_ACK:
		{
			MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
			const DWORD dwRequestPlayerID = pmsg->dwData;

			USERINFO* pRequestPlayer = g_pUserTableForObjectID->FindUser( dwRequestPlayerID );
			if( pRequestPlayer )
			{
				MSGBASE msg;
				msg.Category = MP_PARTY;
				msg.Protocol = MP_PARTY_INVITE_DENY_ACK;

				g_Network.Send2User( pRequestPlayer->dwConnectionIndex, (char*)&msg, sizeof(msg) );
			}
		}
		break;

	default:
		TransToClientMsgParser(dwConnectionIndex,pMsg,dwLength);
		break;
	}
}

// 090526 ShinJS --- ��Ƽ �޼��� �Լ� �߰�
void MP_PARTYUserMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGBASE* pTempMsg = (MSGBASE*)pMsg;
	switch(pTempMsg->Protocol)
	{
	// 090525 ShinJS --- ĳ�����̸����� ��Ƽ�ʴ��ϱ� ( Client -> Server )
	case MP_PARTY_INVITE_BYNAME_SYN:
		{
			MSG_NAME_DWORD* pmsg = (MSG_NAME_DWORD*)pMsg;

			USERINFO* pRequestPlayer = g_pUserTableForObjectID->FindUser( pmsg->dwObjectID );
			if( !pRequestPlayer )
				break;

			// �̸��˻� ���������� Ȯ��
			if( pRequestPlayer->bIsSearchingByName )
			{
				// ERROR ����
				MSG_DWORD msg;
				msg.Category = MP_PARTY;
				msg.Protocol = MP_PARTY_INVITE_BYNAME_NACK;
				msg.dwData = eErr_AlreadySearchingByName;

				g_Network.Send2User( pRequestPlayer->dwConnectionIndex, (char*)&msg, sizeof(msg) );
				return;
			}

			// ����Ƚ���� �ʰ��� ��� �ð� Ȯ��
			if( pRequestPlayer->nInvitePartyByNameCnt >= ePartyInvite_LimitCnt )
			{
				// �ð��� ���� ������� ���� ���
				if( gCurTime < pRequestPlayer->dwInvitePartyByNameLastTime + pRequestPlayer->dwInvitePartyByNameDelayTime )
				{
					// ERROR ����
					MSG_DWORD msg;
					msg.Category = MP_PARTY;
					msg.Protocol = MP_PARTY_INVITE_BYNAME_NACK;
					msg.dwData = eErr_AlreadySearchingByName;
					
					g_Network.Send2User( pRequestPlayer->dwConnectionIndex, (char*)&msg, sizeof(msg) );
					return;
				}

				// �ð��� ����� ��� ���� ����
				pRequestPlayer->nInvitePartyByNameCnt = 0;
				pRequestPlayer->dwInvitePartyByNameDelayTime = 0;
			}

			char CharName[MAX_NAME_LENGTH+1];
			SafeStrCpy( CharName, pmsg->Name, MAX_NAME_LENGTH+1 );

			if( !FILTERTABLE->IsUsableName( CharName ) ||
				FILTERTABLE->IsInvalidCharInclude( (unsigned char*)CharName ) )
				return;

			// �̸��˻� ������ ����
			pRequestPlayer->bIsSearchingByName = TRUE;

			// DB���� ĳ���� �˻�/���� Ȯ��
			SearchCharacterForInvitePartyMember( pmsg->dwObjectID, CharName );
		}
		break;
	default:
		TransToMapServerMsgParser( dwConnectionIndex, pMsg, dwLength );
		break;
	}
}

void TransToClientMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGBASE* pTempMsg = (MSGBASE*)pMsg;
	g_Network.Send2User(pTempMsg,dwLength);
}

void TransToMapServerMsgParser(DWORD dwConnectionIndex/*A?Au��?�ˢ� �ˡ�eCNConnectionIndex*/, char* pMsg, DWORD dwLength)
{
	if(USERINFO* userinfo = g_pUserTable->FindUser(dwConnectionIndex))
	{
		g_Network.Send2Server(
			userinfo->dwMapServerConnectionIndex,
			pMsg,
			dwLength);
	}	
}
void MP_PACKEDMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGBASE* pTempMsg = (MSGBASE*)pMsg;
	switch(pTempMsg->Protocol)
	{
	case MP_PACKEDDATA_NORMAL:
		{
			const SEND_PACKED_DATA* const pack = (SEND_PACKED_DATA*)pMsg;
			const DWORD* charterIDs = (DWORD*)&pack->Data[pack->wRealDataSize];

			for(int n = 0; n < pack->wReceiverNum; ++n)
			{
				const USERINFO* const userInfo = g_pUserTableForObjectID->FindUser(charterIDs[n]);
				
				if(NULL == userInfo)
				{
					continue;
				}

				static char TempData[MAX_PACKEDDATA_SIZE] = {0};
				ZeroMemory(
					TempData,
					sizeof(TempData));
				memcpy(
					TempData,
					pack->Data,
					pack->wRealDataSize);
				g_Network.Send2User(
					userInfo->dwConnectionIndex,
					TempData,
					pack->wRealDataSize);
			}
		}
		break;

	case MP_PACKEDDATA_TOMAPSERVER:
		{
			SEND_PACKED_TOMAPSERVER_DATA* pmsg = (SEND_PACKED_TOMAPSERVER_DATA*)pMsg;
			WORD ToMapPort = g_pServerTable->GetServerPort( eSK_MAP, pmsg->ToMapNum);
			if(ToMapPort == 0)
				return;

			SERVERINFO* pInfo = g_pServerTable->FindServer(ToMapPort);
			if(pInfo == NULL)
				return;

			g_Network.Send2Server(pInfo->dwConnectionIndex,pmsg->Data,pmsg->wRealDataSize);						
		}
		break;
	case MP_PACKEDDATA_TOBROADMAPSERVER:
		{
			SEND_PACKED_TOMAPSERVER_DATA* pmsg = (SEND_PACKED_TOMAPSERVER_DATA*)pMsg;

			g_Network.Broadcast2MapServerExceptOne(dwConnectionIndex, pmsg->Data,pmsg->wRealDataSize);
			//AI����?�ˡ�e AI ��?�ˢ�AIAu������c��?�ˢ� ��?��������aAI �ϡ���!��a �ˡ�E ��u���Ϩ�oo�ˡ�A? 
		}
		break;
	}
}

void MP_FRIENDMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGBASE* pTempMsg = (MSGBASE*)pMsg;
	switch(pTempMsg->Protocol)
	{
	case MP_FRIEND_LOGIN:								Friend_Login(pTempMsg) ;						break;
	case MP_FRIEND_ADD_SYN:								Friend_Add_Syn(pMsg) ;							break;
	case MP_FRIEND_ADD_ACCEPT:							Friend_Add_Accept(pMsg) ;						break;
	case MP_FRIEND_ADD_DENY:							Friend_Add_Deny(pMsg) ;							break;
	case MP_FRIEND_DEL_SYN:								Friend_Del_Syn(pMsg) ;							break;
	case MP_FRIEND_DELID_SYN:							Friend_Delid_Syn(pMsg) ;						break;
	case MP_FRIEND_ADDID_SYN:							Friend_Addid_Syn(pMsg) ;						break;
	case MP_FRIEND_LOGOUT_NOTIFY_TO_AGENT:				Friend_Logout_Notify_To_Agent(pMsg)  ;			break;
	case MP_FRIEND_LOGOUT_NOTIFY_AGENT_TO_AGENT:		Friend_Logout_Notify_Agent_To_Agent(pMsg)  ;	break;			
	case MP_FRIEND_LIST_SYN:							Friend_List_Syn(pMsg) ;							break;
	case MP_FRIEND_ADD_ACK_TO_AGENT:					Friend_Add_Ack_To_Agent(pMsg) ;					break;
	case MP_FRIEND_ADD_NACK_TO_AGENT:					Friend_Add_Nack_To_Agent(pMsg) ;				break;
	case MP_FRIEND_ADD_ACCEPT_TO_AGENT:					Friend_Add_Accept_To_Agent(pMsg) ;				break;
	case MP_FRIEND_ADD_ACCEPT_NACK_TO_AGENT:			Friend_Add_Accept_Nack_To_Agent(pMsg) ;			break;
	case MP_FRIEND_LOGIN_NOTIFY_TO_AGENT:				Friend_Login_Notify_To_Agent(pMsg) ;			break;
	case MP_FRIEND_ADD_INVITE_TO_AGENT:					Friend_Add_Invite_To_Agent(pMsg) ;				break;
	case MP_FRIEND_ADD_NACK:							Friend_Add_Nack(pMsg) ;							break;		
	default:							TransToClientMsgParser(dwConnectionIndex,pMsg,dwLength);		break;
	}	
}

void Friend_Login(MSGBASE* pTempMsg)
{
	USERINFO * userinfo = (USERINFO *)g_pUserTableForObjectID->FindUser(pTempMsg->dwObjectID);
	if(!userinfo)
		return;
	//�ϩ����̡ˡ�| ��ii����ICN A��I�����ˡ���ie��?�ˢ碮��O ����I��������AI ��uE�ˡ��ϡ�
	FriendNotifyLogintoClient(pTempMsg->dwObjectID);
	//�ϩ���i�����ˢ� ��ii����ICN A��I�����ˡ���ieAI ����I��������AIAIAo ��uE��u������?E
//	FriendGetLoginFriends(pTempMsg->dwObjectID); CE��a ������A ��a��EAI�Ƣ�... 
	NoteIsNewNote(pTempMsg->dwObjectID);
}


void Friend_Add_Syn(char* pMsg)
{
	MSG_NAME* pmsg = (MSG_NAME*)pMsg;
	USERINFO * userinfo = (USERINFO *)g_pUserTableForObjectID->FindUser(pmsg->dwObjectID);
	if(!userinfo)
		return;
	
	char buf[MAX_NAME_LENGTH+1];
	SafeStrCpy( buf, pmsg->Name, MAX_NAME_LENGTH+1 );

	//�������� üũ "'"��...
	if( FILTERTABLE->IsInvalidCharInclude( (unsigned char*) buf ) )
		return;
	
	// 100305 ONS ������θ� �����ʰ� ģ���� �߰��ϵ��� ����.
	FriendAddFriendByName(pmsg->dwObjectID, buf);
}


void Friend_Add_Accept(char* pMsg)
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	USERINFO * userinfo = (USERINFO *)g_pUserTableForObjectID->FindUser(pmsg->dwObjectID);
	if(!userinfo)
		return;
	FriendAddFriend(pmsg->dwData, pmsg->dwObjectID);//db insert
}


void Friend_Add_Deny(char* pMsg) 
{
	MSG_FRIEND_MEMBER_ADDDELETEID* pmsg = (MSG_FRIEND_MEMBER_ADDDELETEID*)pMsg;
			
	USERINFO * userinfo = (USERINFO *)g_pUserTableForObjectID->FindUser(pmsg->PlayerID);
	if(!userinfo)
		return;

	MSG_FRIEND_MEMBER_ADDDELETEID msg;
	msg.Category = MP_FRIEND;
	msg.Protocol = MP_FRIEND_ADD_NACK;
	SafeStrCpy( msg.Name, pmsg->Name, MAX_NAME_LENGTH + 1 );
	msg.PlayerID = eFriend_AddDeny;
	g_Network.Send2User(userinfo->dwConnectionIndex, (char*)&msg, sizeof(msg));
}


void Friend_Del_Syn(char* pMsg) 
{
	MSG_NAME * pmsg = (MSG_NAME*)pMsg;

	//090325 ONS String Overflow ����
	char szBuf[MAX_NAME_LENGTH+1];
	SafeStrCpy( szBuf, pmsg->Name, MAX_NAME_LENGTH+1 );

	FriendDelFriend(pmsg->dwObjectID, szBuf); //DB
	//msg
}


void Friend_Delid_Syn(char* pMsg) 
{
	MSG_DWORD2 * pmsg = (MSG_DWORD2*)pMsg;
			
	FriendDelFriendID(pmsg->dwObjectID, pmsg->dwData1, pmsg->dwData2);
}


void Friend_Addid_Syn(char* pMsg) 
{
	MSG_FRIEND_MEMBER_ADDDELETEID * pmsg = (MSG_FRIEND_MEMBER_ADDDELETEID*)pMsg;
	
	//090325 ONS String Overflow ����
	char szBuf[MAX_NAME_LENGTH+1];
	SafeStrCpy( szBuf, pmsg->Name, MAX_NAME_LENGTH+1 );
	
	FriendIsValidTarget(pmsg->dwObjectID, pmsg->PlayerID, szBuf);
}


//091110 pdy ģ�� �α׾ƿ��� �α׾ƿ� �˸� ������ �߸����� ���� ����
void Friend_Logout_Notify_To_Agent(char* pMsg) 
{
	MSG_NAME_DWORD msg ;
	memcpy( &msg , pMsg , sizeof(MSG_NAME_DWORD));

	USERINFO * userinfo = (USERINFO *)g_pUserTableForObjectID->FindUser(msg.dwObjectID);
	if(userinfo)
	{
		msg.Protocol = MP_FRIEND_LOGOUT_NOTIFY_TO_CLIENT;
		g_Network.Send2User(userinfo->dwConnectionIndex, (char*)&msg, sizeof(MSG_NAME_DWORD));
	}
	else
	{
		msg.Protocol = MP_FRIEND_LOGOUT_NOTIFY_AGENT_TO_AGENT;
		g_Network.Broadcast2AgentServerExceptSelf( (char*)&msg, sizeof(MSG_NAME_DWORD));
	}
}


void Friend_Logout_Notify_Agent_To_Agent(char* pMsg) 
{
	MSG_NAME_DWORD* pmsg = (MSG_NAME_DWORD*)pMsg;
	USERINFO * userinfo = (USERINFO *)g_pUserTableForObjectID->FindUser(pmsg->dwObjectID);
	if(userinfo)
	{
		pmsg->Protocol = MP_FRIEND_LOGOUT_NOTIFY_TO_CLIENT;
		g_Network.Send2User(userinfo->dwConnectionIndex, (char*)pmsg, sizeof(MSG_NAME));
	}
}


void Friend_List_Syn(char* pMsg) 
{
	MSG_WORD* pmsg = (MSG_WORD*)pMsg;
	USERINFO * userinfo = (USERINFO *)g_pUserTableForObjectID->FindUser(pmsg->dwObjectID);
	if(!userinfo)
		return;
				
	FriendGetFriendList(pmsg->dwObjectID);
}


void Friend_Add_Ack_To_Agent(char* pMsg) 
{
	MSG_FRIEND_MEMBER_ADDDELETEID* pmsg = (MSG_FRIEND_MEMBER_ADDDELETEID*)pMsg;
	USERINFO * pRecverInfo = (USERINFO *)g_pUserTableForObjectID->FindUser(pmsg->dwObjectID);
	if(!pRecverInfo) return;
	pmsg->Protocol = MP_FRIEND_ADD_ACK;
	g_Network.Send2User(pRecverInfo->dwConnectionIndex, (char*)pmsg, sizeof(MSG_FRIEND_MEMBER_ADDDELETEID));
}


void Friend_Add_Nack_To_Agent(char* pMsg) 
{
	MSG_FRIEND_MEMBER_ADDDELETEID* pmsg = (MSG_FRIEND_MEMBER_ADDDELETEID*)pMsg;
	USERINFO * pRecverInfo = (USERINFO *)g_pUserTableForObjectID->FindUser(pmsg->dwObjectID);
	if(!pRecverInfo) return;
	pmsg->Protocol = MP_FRIEND_ADD_NACK;
	g_Network.Send2User(pRecverInfo->dwConnectionIndex, (char*)pmsg, sizeof(MSG_FRIEND_MEMBER_ADDDELETEID));
}


void Friend_Add_Accept_To_Agent(char* pMsg)
{
	MSG_FRIEND_MEMBER_ADDDELETEID * pmsg = (MSG_FRIEND_MEMBER_ADDDELETEID*)pMsg;
	USERINFO* pToRecverInfo = g_pUserTableForObjectID->FindUser(pmsg->dwObjectID);
	if(pToRecverInfo)
	{			
		pmsg->Protocol = MP_FRIEND_ADD_ACCEPT_ACK;
		g_Network.Send2User(pToRecverInfo->dwConnectionIndex, (char*)pmsg, sizeof(MSG_FRIEND_MEMBER_ADDDELETEID));
	}
}


void Friend_Add_Accept_Nack_To_Agent(char* pMsg)
{
	MSG_FRIEND_MEMBER_ADDDELETEID * pmsg = (MSG_FRIEND_MEMBER_ADDDELETEID*)pMsg;
	USERINFO* pToRecverInfo = g_pUserTableForObjectID->FindUser(pmsg->dwObjectID);
	if(!pToRecverInfo) return;
	pmsg->Protocol = MP_FRIEND_ADD_ACCEPT_NACK;
	g_Network.Send2User(pToRecverInfo->dwConnectionIndex, (char*)pmsg, sizeof(MSG_FRIEND_MEMBER_ADDDELETEID));
}


void Friend_Login_Notify_To_Agent(char* pMsg)
{
	MSG_NAME_DWORD* pmsg = (MSG_NAME_DWORD*)pMsg;
	USERINFO* pRecverInfo = g_pUserTableForObjectID->FindUser(pmsg->dwObjectID);
	if(pRecverInfo)
	{
		pmsg->Protocol = MP_FRIEND_LOGIN_NOTIFY;
		g_Network.Send2User(pRecverInfo->dwConnectionIndex, (char*)pmsg, sizeof(MSG_NAME_DWORD));
	}
}


void Friend_Add_Invite_To_Agent(char* pMsg)
{
	MSG_FRIEND_MEMBER_ADDDELETEID* pmsg = (MSG_FRIEND_MEMBER_ADDDELETEID*)pMsg;
	USERINFO* pRecverInfo = g_pUserTableForObjectID->FindUser(pmsg->dwObjectID);
	if(pRecverInfo)
	{
		if(pRecverInfo->GameOption.bNoFriend == TRUE)
		{
			//nack
			MSG_FRIEND_MEMBER_ADDDELETEID msg;
			msg.Category = MP_FRIEND;
			msg.dwObjectID = pmsg->PlayerID;
			msg.Protocol = MP_FRIEND_ADD_NACK;
			strcpy( msg.Name,  "");
			msg.PlayerID = eFriend_OptionNoFriend;	//errcode insert

			g_Network.Broadcast2AgentServerExceptSelf((char*)&msg, sizeof(msg));
			return;
		}	
		else //ack
		{
			pmsg->Protocol = MP_FRIEND_ADD_INVITE;
			g_Network.Send2User(pRecverInfo->dwConnectionIndex, (char*)pmsg, sizeof(MSG_FRIEND_MEMBER_ADDDELETEID));
		}
	}
}


void Friend_Add_Nack(char* pMsg) 
{
	MSG_FRIEND_MEMBER_ADDDELETEID * pmsg = (MSG_FRIEND_MEMBER_ADDDELETEID*)pMsg;
	USERINFO* pRecverInfo = g_pUserTableForObjectID->FindUser(pmsg->dwObjectID);
	if(pRecverInfo)
	{
		g_Network.Send2User(pRecverInfo->dwConnectionIndex, (char*)pmsg, sizeof(MSG_FRIEND_MEMBER_ADDDELETEID));
	}
}



void MP_NOTEServerMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGBASE* pTempMsg = (MSGBASE*)pMsg;
	switch(pTempMsg->Protocol)
	{//////////////////////////////////////////////////////////////////////////
	//AEAo
	case MP_NOTE_SENDNOTE_SYN:
		{
			MSG_FRIEND_SEND_NOTE * pmsg = (MSG_FRIEND_SEND_NOTE *) pMsg;
			USERINFO * userinfo = (USERINFO *)g_pUserTableForObjectID->FindUser(pmsg->FromId);
			if(!userinfo)
			{
				return;
			}

			char bufFrom[MAX_NAME_LENGTH+1];
			char bufTo[MAX_NAME_LENGTH+1];
			char bufTitle[MAX_NOTE_TITLE+1];
			char bufNote[MAX_NOTE_LENGTH+1];
			SafeStrCpy( bufFrom, pmsg->FromName, MAX_NAME_LENGTH+1 );
			SafeStrCpy( bufTo, pmsg->ToName, MAX_NAME_LENGTH+1 );
			SafeStrCpy( bufTitle, pmsg->Title, MAX_NOTE_TITLE+1 );
			SafeStrCpy( bufNote, pmsg->Note, MAX_NOTE_LENGTH+1 );
			
			if( FILTERTABLE->IsInvalidCharInclude( (unsigned char*)bufFrom ) ||
				FILTERTABLE->IsInvalidCharInclude( (unsigned char*)bufTo ))
				return;
			//pmsg->Note[MAX_NOTE_LENGTH] = 0; // �� ���ڰ� �����°���? 

			NoteServerSendtoPlayer(pmsg->FromId, bufFrom, bufTo, bufTitle, bufNote);
		}
		break;

	case MP_NOTE_RECEIVENOTE:
		{
			MSGBASE* pmsg = (MSGBASE*) pMsg;
			USERINFO* pRecverInfo = g_pUserTableForObjectID->FindUser(pmsg->dwObjectID);
			if(!pRecverInfo)
				return;

			g_Network.Send2User(pRecverInfo->dwConnectionIndex, (char*)pMsg, dwLength);
		}
		break;

	default:
		TransToClientMsgParser( dwConnectionIndex, pMsg, dwLength );		
		break;
	}
}
void MP_NOTEMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGBASE* pTempMsg = (MSGBASE*)pMsg;
	switch(pTempMsg->Protocol)
	{//////////////////////////////////////////////////////////////////////////
	//AEAo
	case MP_NOTE_SENDNOTE_SYN:			Note_SendNote_Syn(pMsg) ;			break;
	case MP_NOTE_SENDNOTEID_SYN:		Note_SendNoteId_Syn(pMsg) ;			break;
	case MP_NOTE_RECEIVENOTE:			Note_ReceiveNote(pMsg) ;			break;		
	case MP_NOTE_DELALLNOTE_SYN:		Note_DelAllNote_Syn(pMsg) ;			break;
	case MP_NOTE_NOTELIST_SYN:			Note_NoteList_Syn(pMsg) ;			break;
	case MP_NOTE_READNOTE_SYN:			Note_ReadNote_Syn(pMsg) ;			break;
	case MP_NOTE_DELNOTE_SYN:			Note_DelNote_Syn(pMsg) ;			break;
	case MP_NOTE_SAVENOTE_SYN:			Note_SaveNote_Syn(pMsg);			break;

	default:
		TransToMapServerMsgParser(dwConnectionIndex,pMsg,dwLength);
		break;
	}
}

void Note_SendNote_Syn(char* pMsg) 
{
	MSG_FRIEND_SEND_NOTE * pmsg = (MSG_FRIEND_SEND_NOTE *) pMsg;
	USERINFO * userinfo = (USERINFO *)g_pUserTableForObjectID->FindUser(pmsg->FromId);
	if(!userinfo)
	{
		return;
	}

	char bufFrom[MAX_NAME_LENGTH+1];
	char bufTo[MAX_NAME_LENGTH+1];
	char bufTitle[MAX_NOTE_TITLE+1];
	char bufNote[MAX_NOTE_LENGTH+1];
	SafeStrCpy( bufFrom, pmsg->FromName, MAX_NAME_LENGTH+1 );
	SafeStrCpy( bufTo, pmsg->ToName, MAX_NAME_LENGTH+1 );
	SafeStrCpy( bufTitle, pmsg->Title, MAX_NOTE_TITLE+1 );
	SafeStrCpy( bufNote, pmsg->Note, MAX_NOTE_LENGTH+1 );

	if( FILTERTABLE->IsInvalidCharInclude( (unsigned char*)bufFrom ) ||
		FILTERTABLE->IsInvalidCharInclude( (unsigned char*)bufTo ))
		return;
	//pmsg->Note[MAX_NOTE_LENGTH] = 0; // �� ���ڰ� �����°���? 

	NoteSendtoPlayer(pmsg->FromId, bufFrom, bufTo, bufTitle, bufNote);
}


void Note_SendNoteId_Syn(char* pMsg) 
{
	MSG_FRIEND_SEND_NOTE_ID * pmsg = (MSG_FRIEND_SEND_NOTE_ID*)pMsg;

	// 090325 ONS String Overflow ����
	char bufFrom[MAX_NAME_LENGTH+1];
	char bufNote[MAX_NOTE_LENGTH+1];
	SafeStrCpy( bufFrom, pmsg->FromName, MAX_NAME_LENGTH+1 );
	SafeStrCpy( bufNote, pmsg->Note, MAX_NOTE_LENGTH+1 );

	NoteSendtoPlayerID(pmsg->dwObjectID, bufFrom, pmsg->TargetID, bufNote);
}


void Note_ReceiveNote(char* pMsg) 
{
	MSGBASE* pmsg = (MSGBASE*)pMsg;
	USERINFO * userinfo = (USERINFO *)g_pUserTableForObjectID->FindUser(pmsg->dwObjectID);
	if(!userinfo)
	{
		return;
	}
	g_Network.Send2User(userinfo->dwConnectionIndex, (char*)pmsg, sizeof(MSGBASE));
}


void Note_DelAllNote_Syn(char* pMsg) 
{
	MSGBASE * pmsg = (MSGBASE*)pMsg;
	NoteDelAll(pmsg->dwObjectID);

	USERINFO * userinfo = (USERINFO *)g_pUserTableForObjectID->FindUser(pmsg->dwObjectID);
	if(!userinfo)
		return;
	MSGBASE msg;
	msg.Category = MP_NOTE;
	msg.Protocol = MP_NOTE_DELALLNOTE_ACK;
	msg.dwObjectID = pmsg->dwObjectID;

	g_Network.Send2User(userinfo->dwConnectionIndex, (char*)&msg, sizeof(msg));
}


void Note_NoteList_Syn(char* pMsg) 
{
	MSG_WORD2 * pmsg = (MSG_WORD2*)pMsg;
	NoteList(pmsg->dwObjectID, pmsg->wData1, pmsg->wData2);
}


void Note_ReadNote_Syn(char* pMsg)
{
	MSG_DWORD2 * pmsg = (MSG_DWORD2*)pMsg;
	USERINFO * userinfo = (USERINFO *)g_pUserTableForObjectID->FindUser(pmsg->dwObjectID);
	if(!userinfo)
		return;

	NoteRead(pmsg->dwObjectID, pmsg->dwData1, pmsg->dwData2);
}


void Note_DelNote_Syn(char* pMsg) 
{
	MSG_FRIEND_DEL_NOTE* pmsg = (MSG_FRIEND_DEL_NOTE*)pMsg;

	if( pmsg->bLast == 1 || pmsg->bLast == 0 )
		NoteDelete(pmsg->dwObjectID, pmsg->NoteID, pmsg->bLast);
}

void Note_SaveNote_Syn(char* pMsg) 
{
	MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;

	NoteSave(pmsg->dwObjectID, pmsg->dwData1, (BOOL)pmsg->dwData2);
}

void MP_CHEATUserMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGROOT* pTempMsg = (MSGROOT*)pMsg;

	USERINFO* pUserInfo = g_pUserTable->FindUser( dwConnectionIndex );
	if( !pUserInfo ) return;
	if( !( pUserInfo->UserLevel == eUSERLEVEL_GM || pUserInfo->UserLevel == eUSERLEVEL_PROGRAMMER
		|| pUserInfo->UserLevel == eUSERLEVEL_DEVELOPER ) )
		return;

	switch( pTempMsg->Protocol )
	{
	case MP_CHEAT_GM_LOGIN_SYN:				Cheat_Gm_Login_Syn(dwConnectionIndex, pMsg) ;		return;
	}

//////
	GM_INFO* pGMInfo = GMINFO->GetGMInfo( dwConnectionIndex );

	if( pUserInfo->UserLevel == eUSERLEVEL_GM )		//AI��C ����A������ Ao��iAU.
	if( pGMInfo == NULL )
		return;	
/////
	//-- �� �� �Լ��� ����� ���� ��������? �Ф� ������...

	//--- 1. GM�̻��̸� ������ �׼� ��� ������ ġƮ��  
	switch( pTempMsg->Protocol )
	{
	case MP_CHEAT_CHANGEMAP_SYN:			Cheat_ChangeMap_Syn(dwConnectionIndex, pMsg, dwLength) ;						return;		
	case MP_CHEAT_BANCHARACTER_SYN:			Cheat_BanCharacter_Syn(pMsg) ;													return;		
	case MP_CHEAT_BANMAP_SYN:				Cheat_BanMap_Syn(pMsg) ;														return;		
	case MP_CHEAT_WHEREIS_SYN:				Cheat_WhereIs_Syn(pMsg) ;														return;
	case MP_CHEAT_AGENTCHECK_SYN:			Cheat_AgentCheck_Syn(pMsg, dwLength) ;											return;
	case MP_CHEAT_NOTICE_SYN:				Cheat_Notice_Syn(pMsg, dwLength) ;												return;
	case MP_CHEAT_MOVE_SYN:
	case MP_CHEAT_MOVEUSER_SYN:
	case MP_CHEAT_GETID_SYN:
	case MP_CHEAT_HIDE_SYN:
	case MP_CHEAT_WHEREIS_MAPSERVER_ACK:
	case MP_CHEAT_PARTYINFO_SYN:
//---KES CHEAT PKEVENT
	case MP_CHEAT_PKEVENT:
//-------------
//---KES CHEAT CHANGESIZE
	case MP_CHEAT_CHANGESIZE:
	case MP_CHEAT_VEHICLE_SUMMON_SYN:
	case MP_CHEAT_VEHICLE_GET_SYN:
	case MP_CHEAT_VEHICLE_UNSUMMON_SYN:
	case MP_CHEAT_MOB_CHAT:					TransToMapServerMsgParser(dwConnectionIndex,pMsg,dwLength);						return;
	
	case MP_CHEAT_MOVETOCHAR_SYN:
		{
			pUserInfo->bForceMove = 1;
			TransToMapServerMsgParser(dwConnectionIndex,pMsg,dwLength);
			return;
		}
	
	case MP_CHEAT_DUNGEON_OBSERVERJOIN_SYN:	Cheat_DungeonObserverJoin_Syn(dwConnectionIndex, pMsg, dwLength);				return;
	}

	//--- 2. GM �����͸� ������ ġƮ��

	if( g_pServerSystem->IsTestServer() == FALSE )
	{
		if( pGMInfo != NULL )
		if( pGMInfo->nPower != eGM_POWER_MASTER ) return;
	}

	switch( pTempMsg->Protocol )
	{
	case MP_CHEAT_EVENT_MONSTER_REGEN:		Cheat_Event_Monster_Regen(pUserInfo, dwConnectionIndex, pMsg, dwLength) ;		return;
	case MP_CHEAT_EVENT_MONSTER_DELETE:		Cheat_Event_Monster_Delete(pUserInfo, dwConnectionIndex, pMsg, dwLength) ;		return;
	case MP_CHEAT_EVENT_SYN:				Cheat_Event_Syn(pUserInfo, dwConnectionIndex, pMsg, dwLength) ;					return;
	case MP_CHEAT_EVENTNOTIFY_ON:			Cheat_EventNotify_On(pUserInfo, dwConnectionIndex, pMsg, dwLength) ;			return;
	case MP_CHEAT_EVENTNOTIFY_OFF:			Cheat_EventNotify_Off(pUserInfo, dwConnectionIndex, pMsg, dwLength) ;			return;		
	case MP_CHEAT_PLUSTIME_ON:				Cheat_PlusTime_On(pUserInfo, dwConnectionIndex, pMsg, dwLength) ;				return;
	case MP_CHEAT_PLUSTIME_ALLOFF:			Cheat_PlusTime_AllOff(pUserInfo, dwConnectionIndex, pMsg, dwLength) ;			return;
	}


	if( g_pServerSystem->IsTestServer() == FALSE )	return;

	//--- 3. �������� �Ұ����� ġƮ��
	switch( pTempMsg->Protocol )
	{
	case MP_CHEAT_BLOCKCHARACTER_SYN:		Cheat_BlockCharacter_Syn(pUserInfo, dwConnectionIndex, pMsg) ;					return;
	case MP_CHEAT_PKALLOW_SYN:				Cheat_PkAllow_Syn(pUserInfo, dwConnectionIndex, pMsg, dwLength) ;				return;		
	case MP_CHEAT_ITEM_SYN:
	case MP_CHEAT_MONEY_SYN:				Cheat_Money_Syn(pUserInfo, dwConnectionIndex, pMsg, dwLength) ;					return;
	default:								TransToMapServerMsgParser(dwConnectionIndex,pMsg,dwLength);						return;
	}
}


void Cheat_Gm_Login_Syn(DWORD dwConnectionIndex, char* pMsg)
{
	MSG_NAME2* pmsg = (MSG_NAME2*)pMsg;
	WORD port;
	char ip[16];
	g_Network.GetUserAddress( dwConnectionIndex, ip, &port );

	// 090325 ONS String Overflow ����
	char szBuf1[MAX_NAME_LENGTH+1];
	char szBuf2[MAX_NAME_LENGTH+1];
	SafeStrCpy( szBuf1, pmsg->str1, MAX_NAME_LENGTH+1 );
	SafeStrCpy( szBuf2, pmsg->str2, MAX_NAME_LENGTH+1 );

	GM_Login( dwConnectionIndex, szBuf1, szBuf2, ip );
}


void Cheat_ChangeMap_Syn(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength) 
{
	MSG_NAME_DWORD2* pmsg = (MSG_NAME_DWORD2*)pMsg;

	if( *(pmsg->Name) == 0 )
	{
		WORD tomapserverport = g_pServerTable->GetServerPort( eSK_MAP, (WORD)pmsg->dwData1);	//map��u���Ϩ�oo��o�Ϩ�E��I��?I A�ˡ� ��u���Ϩ�oo��o�Ϩ�E��I�����ˢ� A�Ϣ���i��oCIAo�ˡ�A ��uE�ˡ�A�����ˢ�?
		if(tomapserverport)
		{				
			USERINFO* pInfo = g_pUserTable->FindUser(dwConnectionIndex);
			pInfo->bForceMove = TRUE;
			
			if( pInfo->dwMapServerConnectionIndex )
				g_Network.Send2Server(pInfo->dwMapServerConnectionIndex,(char*)pMsg,dwLength);
			
		}
		else
		{	
			MSG_DWORD msg;
			msg.Category	= MP_CHEAT;
			msg.Protocol	= MP_CHEAT_CHANGEMAP_NACK;
			msg.dwObjectID	= pmsg->dwObjectID;
			msg.dwData		= 0;
			g_Network.Send2User((MSGBASE*)&msg, sizeof(msg));
			return;
		}
	}
	else
	{
		g_Network.Broadcast2MapServer( (char*)pMsg, dwLength );	//AO��oA. DB��?�ˢ硧u���� ��i����i����u������?A�ˡ�A����O �ϩ��ˡͨϩ�����?
	}
}


void Cheat_BanCharacter_Syn(char* pMsg) 
{
	MSG_NAME* pmsg = (MSG_NAME*)pMsg;
	USERINFO* pSenderInfo = g_pUserTableForObjectID->FindUser( pmsg->dwObjectID );
	if( !pSenderInfo ) return;

	// 090325 ONS String Overflow ����
	char szBuf[MAX_NAME_LENGTH+1];
	SafeStrCpy( szBuf, pmsg->Name, MAX_NAME_LENGTH+1 );

	if( (FILTERTABLE->IsInvalidCharInclude((unsigned char*)szBuf)) == TRUE )
	{
		return;
	}
	else
		GM_BanCharacter( pmsg->dwObjectID, szBuf, pmsg->dwObjectID );
}


void Cheat_BlockCharacter_Syn(USERINFO* pUserInfo, DWORD dwConnectionIndex, char* pMsg) 
{
	MSG_NAME_DWORD* pmsg = (MSG_NAME_DWORD*)pMsg;
	USERINFO* pSenderInfo = g_pUserTableForObjectID->FindUser( pmsg->dwObjectID );
	if( !pSenderInfo ) return;

	// 090325 ONS String Overflow ����
	char szBuf[MAX_NAME_LENGTH+1];
	SafeStrCpy( szBuf, pmsg->Name, MAX_NAME_LENGTH+1 );

	if( pUserInfo->UserLevel == eUSERLEVEL_GM )	//AI��C ����A������ Ao��o����Ao..
	if( GMINFO->GetGMPower( dwConnectionIndex ) > eGM_POWER_MONITOR &&
		GMINFO->GetGMPower( dwConnectionIndex ) != eGM_POWER_EVENTER )
		return;
	if( (FILTERTABLE->IsInvalidCharInclude((unsigned char*)szBuf)) == TRUE )
		return;
	
	if( pmsg->dwData == 1 )
	{
		//block
		GM_UpdateUserLevel( pmsg->dwObjectID, g_nServerSetNum, szBuf, 6 );	//6 : ����eA�ˢ�����i��������
	}
	else if( pmsg->dwData == 0 )
	{
		//nonblock
		GM_UpdateUserLevel( pmsg->dwObjectID, g_nServerSetNum, szBuf, 1 );	//1 : C����A|
	}
}


void Cheat_WhereIs_Syn(char* pMsg) 
{
	MSG_NAME* pmsg = (MSG_NAME*)pMsg;
	USERINFO* pSenderInfo = g_pUserTableForObjectID->FindUser( pmsg->dwObjectID );
	if( !pSenderInfo ) return;

	// 090325 ONS String Overflow ����
	char szBuf[MAX_NAME_LENGTH+1];
	SafeStrCpy( szBuf, pmsg->Name, MAX_NAME_LENGTH+1 );

	if( (FILTERTABLE->IsInvalidCharInclude((unsigned char*)szBuf)) == TRUE )
	{
		return;
	}
	else
		GM_WhereIsCharacter( pmsg->dwObjectID, szBuf, pmsg->dwObjectID );
}


void Cheat_Event_Monster_Regen(USERINFO* pUserInfo, DWORD dwConnectionIndex, char* pMsg, DWORD dwLength) 
{
	MSG_EVENT_MONSTERREGEN* pmsg = (MSG_EVENT_MONSTERREGEN*)pMsg;
	USERINFO* pSenderInfo = g_pUserTableForObjectID->FindUser( pmsg->dwObjectID );
	if( !pSenderInfo ) return;

	if( pUserInfo->UserLevel == eUSERLEVEL_GM )	//AI��C ����A������ Ao��o����Ao..
	if( GMINFO->GetGMPower( dwConnectionIndex ) > eGM_POWER_MASTER &&
		GMINFO->GetGMPower( dwConnectionIndex ) != eGM_POWER_EVENTER )
		return;

	//---KES ���������� ġƮ����
	if( g_pServerSystem->IsTestServer() == FALSE )
		pmsg->ItemID = 0;
	//--------------------------

	WORD wServerPort = g_pServerTable->GetServerPort( eSK_MAP, pmsg->wMap );
	SERVERINFO* pInfo = g_pServerTable->FindServer( wServerPort );

	if(pInfo)
		g_Network.Send2Server( pInfo->dwConnectionIndex, pMsg, dwLength );
}


void Cheat_Event_Monster_Delete(USERINFO* pUserInfo,DWORD dwConnectionIndex, char* pMsg, DWORD dwLength) 
{
	MSGBASE* pmsg = (MSGBASE*)pMsg;
	USERINFO* pSenderInfo = g_pUserTableForObjectID->FindUser( pmsg->dwObjectID );
	if( !pSenderInfo ) return;

	if( pUserInfo->UserLevel == eUSERLEVEL_GM )	//AI��C ����A������ Ao��o����Ao..
	if( GMINFO->GetGMPower( dwConnectionIndex ) > eGM_POWER_MASTER &&
		GMINFO->GetGMPower( dwConnectionIndex ) != eGM_POWER_EVENTER )
		return;

	g_Network.Broadcast2MapServer( pMsg, dwLength );
}


void Cheat_BanMap_Syn(char* pMsg) 
{
	MSG_WORD2* pmsg = (MSG_WORD2*)pMsg;
	USERINFO* pSenderInfo = g_pUserTableForObjectID->FindUser( pmsg->dwObjectID );
	if( !pSenderInfo ) return;

	USERINFO* pInfo;
	g_pUserTable->SetPositionUserHead();
	while( (pInfo = g_pUserTable->GetUserData()) != NULL )
	{
		if( pInfo->wUserMapNum == pmsg->wData1 )
		{
			if( pmsg->wData2 != 1 || pInfo->dwCharacterID != pSenderInfo->dwCharacterID )
			{
				DWORD dwConIdx = pInfo->dwConnectionIndex;
				OnDisconnectUser( dwConIdx );
				DisconnectUser( dwConIdx );	//3:GMTOOL_BAN
			}
		}
	}
	
	MSG_WORD msg;
	msg.Category	= MP_CHEAT;
	msg.Protocol	= MP_CHEAT_BANMAP_SYN;
	msg.wData		= pmsg->wData1;

	g_Network.Broadcast2AgentServerExceptSelf( (char*)&msg, sizeof(msg) );
}


void Cheat_AgentCheck_Syn(char* pMsg, DWORD dwLength) 
{
	MSGBASE* pmsg = (MSGBASE*)pMsg;
			
	g_Network.Broadcast2AgentServerExceptSelf( pMsg, dwLength );

	pmsg->Protocol = MP_CHEAT_AGENTCHECK_ACK;
	
	USERINFO* pInfo = g_pUserTableForObjectID->FindUser( pmsg->dwObjectID );
	if( pInfo )
		g_Network.Send2User( pInfo->dwConnectionIndex, (char*)pmsg, dwLength );	
}


void Cheat_PkAllow_Syn(USERINFO* pUserInfo,DWORD dwConnectionIndex, char* pMsg, DWORD dwLength) 
{
	MSG_WORD2* pmsg = (MSG_WORD2*)pMsg;
	USERINFO* pSenderInfo = g_pUserTableForObjectID->FindUser( pmsg->dwObjectID );
	if( !pSenderInfo ) return;

	if( pUserInfo->UserLevel == eUSERLEVEL_GM )	//AI��C ����A������ Ao��o����Ao..
	if( GMINFO->GetGMPower( dwConnectionIndex ) > eGM_POWER_MONITOR &&
		GMINFO->GetGMPower( dwConnectionIndex ) != eGM_POWER_EVENTER )
		return;

	WORD wServerPort = g_pServerTable->GetServerPort( eSK_MAP, pmsg->wData1 );
	SERVERINFO* pInfo = g_pServerTable->FindServer( wServerPort );
	if( pInfo )
		g_Network.Send2Server( pInfo->dwConnectionIndex, pMsg, dwLength );	
}


void Cheat_Notice_Syn(char* pMsg, DWORD dwLength) 
{
	MSG_CHAT_WORD* pmsg = (MSG_CHAT_WORD*)pMsg;
	USERINFO* pSenderInfo = g_pUserTableForObjectID->FindUser( pmsg->dwObjectID );
	if( !pSenderInfo ) return;

	TESTMSG msg;
	msg.Category = MP_SIGNAL;
	msg.Protocol = MP_SIGNAL_COMMONUSER;
	SafeStrCpy( msg.Msg, pmsg->Msg, MAX_CHAT_LENGTH + 1 );

	USERINFO* pInfo = NULL;
	g_pUserTable->SetPositionUserHead();
	while( (pInfo = (USERINFO *)g_pUserTable->GetUserData()) != NULL )
	{	
		if( pmsg->wData == 0 || pmsg->wData == pInfo->wUserMapNum )
		{
			TESTMSG msgTemp = msg;
			g_Network.Send2User( pInfo->dwConnectionIndex, (char*)&msgTemp, msgTemp.GetMsgLength() );	//CHATMSG 040324
		}
	}

	g_Network.Broadcast2AgentServerExceptSelf( (char*)pmsg, dwLength );
}

void Cheat_DungeonObserverJoin_Syn(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSG_WORD3* pmsg = (MSG_WORD3*)pMsg;

	USERINFO* pUserInfo = g_pUserTableForObjectID->FindUser( pmsg->dwObjectID );
	if( !pUserInfo )			return;

	WORD tomapserverport = g_pServerTable->GetServerPort( eSK_MAP, pmsg->wData1 );
	if(tomapserverport)
	{
		SERVERINFO* pInfo = g_pServerTable->FindServer( tomapserverport );
		if(pInfo)
		{
			pUserInfo->wUserMapNum = pmsg->wData1;
			pUserInfo->dwMapServerConnectionIndex = pInfo->dwConnectionIndex;

			MSG_WORD4 msg;
			msg.Category = MP_CHEAT;
			msg.Protocol = MP_CHEAT_DUNGEON_OBSERVERJOIN_SYN;
			msg.dwObjectID = pmsg->dwObjectID;
			msg.wData1 = pmsg->wData2;
			msg.wData2 = WORD(pUserInfo->dwUniqueConnectIdx);
			msg.wData3 = pUserInfo->UserLevel;
			msg.wData4 = pmsg->wData3;
			g_Network.Send2Server( pInfo->dwConnectionIndex, (char*)&msg, sizeof(msg));
			return;
		}
	}

//BattleJoin_Err:
	MSG_DWORD msg;
	msg.Category = MP_CHEAT;
	msg.Protocol = MP_CHEAT_DUNGEON_OBSERVERJOIN_NACK;
	msg.dwData = eGTError_ERROR;
	g_Network.Send2User( pUserInfo->dwConnectionIndex, pMsg, dwLength );
}


void Cheat_Money_Syn(USERINFO* pUserInfo,DWORD dwConnectionIndex, char* pMsg, DWORD dwLength) 
{
	if( pUserInfo->UserLevel == eUSERLEVEL_GM )	//AI��C ����A������ Ao��o����Ao..
	if( GMINFO->GetGMPower( dwConnectionIndex ) > eGM_POWER_MASTER )
		return;

	TransToMapServerMsgParser(dwConnectionIndex,pMsg,dwLength);		
}


void Cheat_Event_Syn(USERINFO* pUserInfo,DWORD dwConnectionIndex, char* pMsg, DWORD dwLength) 
{
	MSG_WORD2* pmsg = (MSG_WORD2*)pMsg;
	USERINFO* pSenderInfo = g_pUserTableForObjectID->FindUser( pmsg->dwObjectID );
	if( !pSenderInfo ) return;
	
	if( pUserInfo->UserLevel == eUSERLEVEL_GM )
	if( GMINFO->GetGMPower( dwConnectionIndex ) > eGM_POWER_MASTER &&
		GMINFO->GetGMPower( dwConnectionIndex ) != eGM_POWER_EVENTER )
		return;			
/*		
	if( pmsg->wData2 == 1 )
	{
		GMINFO->SetEventCheat( TRUE );
	}
	else
	{
		GMINFO->SetEventCheat( FALSE );
	}
	g_Network.Broadcast2AgentServerExceptSelf( (char*)pmsg, dwLength );
*/
	g_Network.Broadcast2MapServer( (char*)pmsg, dwLength );

	// 06.09.12 RaMa
	LogGMToolUse( pmsg->dwObjectID, eGMLog_Event, MP_CHEAT_EVENT_SYN, (DWORD)pmsg->wData1, (DWORD)pmsg->wData2 );
}


void Cheat_EventNotify_On(USERINFO* pUserInfo,DWORD dwConnectionIndex, char* pMsg, DWORD dwLength) 
{
	MSG_EVENTNOTIFY_ON* pmsg = (MSG_EVENTNOTIFY_ON*)pMsg;
	USERINFO* pSenderInfo = g_pUserTableForObjectID->FindUser( pmsg->dwObjectID );
	if( !pSenderInfo ) return;

	if( pUserInfo->UserLevel == eUSERLEVEL_GM )
	if( GMINFO->GetGMPower( dwConnectionIndex ) > eGM_POWER_MONITOR &&
		GMINFO->GetGMPower( dwConnectionIndex ) != eGM_POWER_EVENTER )
		return;
	
	g_pServerSystem->ResetApplyEvent();
	for( int i=0; i<eEvent_Max; ++i )
	{
		if( pmsg->EventList[i] )
		{
			g_pServerSystem->SetApplyEvent( i ) ;
		}
	}

	g_pServerSystem->SetEventNotifyStr( pmsg->strTitle, pmsg->strContext );
	g_pServerSystem->SetUseEventNotify( TRUE );	//seteventnotifystr ���Ŀ� ȣ���ؾ��Ѵ�.
	
	g_Network.Broadcast2AgentServerExceptSelf( (char*)pmsg, dwLength );
}


void Cheat_PlusTime_On(USERINFO* pUserInfo,DWORD dwConnectionIndex, char* pMsg, DWORD dwLength) 
{
	MSGBASE* pmsg = (MSGBASE*)pMsg;
	USERINFO* pSenderInfo = g_pUserTableForObjectID->FindUser( pmsg->dwObjectID );
	if( !pSenderInfo ) return;

	if( pUserInfo->UserLevel == eUSERLEVEL_GM )
	if( GMINFO->GetGMPower( dwConnectionIndex ) > eGM_POWER_MONITOR &&
		GMINFO->GetGMPower( dwConnectionIndex ) != eGM_POWER_EVENTER )
		return;

	PLUSTIMEMGR->PlusTimeOn();
	g_Network.Broadcast2AgentServerExceptSelf( (char*)pmsg, dwLength );
}


void Cheat_EventNotify_Off(USERINFO* pUserInfo,DWORD dwConnectionIndex, char* pMsg, DWORD dwLength) 
{
	MSGBASE* pmsg = (MSGBASE*)pMsg;
	USERINFO* pSenderInfo = g_pUserTableForObjectID->FindUser( pmsg->dwObjectID );
	if( !pSenderInfo ) return;

	if( pUserInfo->UserLevel == eUSERLEVEL_GM )
	if( GMINFO->GetGMPower( dwConnectionIndex ) > eGM_POWER_MONITOR &&
		GMINFO->GetGMPower( dwConnectionIndex ) != eGM_POWER_EVENTER )
		return;

	g_pServerSystem->SetUseEventNotify( FALSE );
	g_Network.Broadcast2AgentServerExceptSelf( (char*)pmsg, dwLength );
	LogGMToolUse( pmsg->dwObjectID, eGMLog_PlusTime, MP_CHEAT_EVENTNOTIFY_OFF, 0, 0 );
}

void Cheat_PlusTime_AllOff(USERINFO* pUserInfo,DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGBASE* pmsg = (MSGBASE*)pMsg;
	USERINFO* pSenderInfo = g_pUserTableForObjectID->FindUser( pmsg->dwObjectID );
	if( !pSenderInfo ) return;

	if( pUserInfo->UserLevel == eUSERLEVEL_GM )
	if( GMINFO->GetGMPower( dwConnectionIndex ) > eGM_POWER_MONITOR &&
		GMINFO->GetGMPower( dwConnectionIndex ) != eGM_POWER_EVENTER )
		return;

	PLUSTIMEMGR->PlusTimeOff();
	g_Network.Broadcast2AgentServerExceptSelf( (char*)pmsg, dwLength );
}

void Cheat_Event_Syn2(USERINFO* pUserInfo,DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;

	if( pUserInfo->UserLevel == eUSERLEVEL_GM )
	if( GMINFO->GetGMPower( dwConnectionIndex ) > eGM_POWER_MONITOR &&
		GMINFO->GetGMPower( dwConnectionIndex ) != eGM_POWER_EVENTER )
		return;

	WORD wServerPort = g_pServerTable->GetServerPort( eSK_MAP, (WORD)pmsg->dwData1 );
	SERVERINFO* pInfo = g_pServerTable->FindServer( wServerPort );
	if( pInfo )
		g_Network.Send2Server( pInfo->dwConnectionIndex, pMsg, dwLength );
}



void MP_CHEATServerMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGROOT* pTempMsg = (MSGROOT*)pMsg;
	switch( pTempMsg->Protocol )
	{
	case MP_CHEAT_BANCHARACTER_SYN:						CheatServer_BanCharacter_Syn(dwConnectionIndex, pMsg) ;			break;
	case MP_CHEAT_BANCHARACTER_ACK:						CheatServer_BanCharacter_Ack(pMsg) ;							break;
/*
	case MP_CHEAT_WHEREIS_SYN:	//<--Agent
		{
			MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;

			USERINFO* pTargetInfo = g_pUserTableForObjectID->FindUser( pmsg->dwData );
			if( pTargetInfo )
			{
				MSG_WORD msgAck;
				msgAck.Category	= MP_CHEAT;
				msgAck.Protocol	= MP_CHEAT_WHEREIS_ACK;
				msgAck.dwObjectID	= pmsg->dwObjectID;	//senderid
				msgAck.wData		= pTargetInfo->wUserMapNum;

				g_Network.Broadcast2AgentServerExceptSelf( (char*)&msgAck, sizeof(MSG_WORD) );
			}
			else
			{
				MSGBASE msgNack;
				msgNack.Category	= MP_CHEAT;
				msgNack.Protocol	= MP_CHEAT_WHEREIS_NACK;

				g_Network.Broadcast2AgentServerExceptSelf( (char*)&msgNack, sizeof(MSGBASE) );
			}
		}
		break;

	case MP_CHEAT_WHEREIS_ACK:
		{
			MSG_WORD* pmsg = (MSG_WORD*)pMsg;

			USERINFO* pSenderInfo = g_pUserTableForObjectID->FindUser( pmsg->dwObjectID );
			if( !pSenderInfo ) return;
			
			g_Network.Send2User( pSenderInfo->dwConnectionIndex, (char*)pmsg, sizeof(MSG_WORD) );
		}
		break;
*/
	case MP_CHEAT_WHEREIS_MAPSERVER_ACK:				CheatServer_WhereIs_MapServer_Ack(pMsg, dwLength) ;				break;
	case MP_CHEAT_WHEREIS_NACK:							CheatServer_WhereIs_Nack(pMsg, dwLength) ;						break;
	case MP_CHEAT_BANMAP_SYN:							CheatServer_BanMap_Syn(pMsg) ;									break;
	case MP_CHEAT_AGENTCHECK_SYN:						CheatServer_AgentCheck_Syn(dwConnectionIndex, pMsg, dwLength) ;	break;
	case MP_CHEAT_AGENTCHECK_ACK:						CheatServer_AgentCheck_Ack(pMsg, dwLength) ;					break;
	case MP_CHEAT_NOTICE_SYN:							CheatServer_Notice_Syn(pMsg) ;									break;
	case MP_CHEAT_EVENT_SYN:							CheatServer_Event_Syn(pMsg) ;									break;
	case MP_CHEAT_EVENTNOTIFY_ON:						CheatServer_EventNotify_On(pMsg) ;								break;
	case MP_CHEAT_EVENTNOTIFY_OFF:						CheatServer_EventNotify_Off(pMsg) ;								break;
	case MP_CHEAT_PLUSTIME_ON:							CheatServer_PlusTime_On(pMsg) ;									break;
	case MP_CHEAT_PLUSTIME_ALLOFF:						CheatServer_PlusTime_AllOff(pMsg) ;								break;
	default:											TransToClientMsgParser( dwConnectionIndex, pMsg, dwLength );	break;
	}
}


void CheatServer_BanCharacter_Syn(DWORD dwConnectionIndex, char* pMsg)
{
	MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
	USERINFO* pTargetInfo = g_pUserTableForObjectID->FindUser( pmsg->dwData1 );
	if( !pTargetInfo ) return;

	MSG_DWORD msg;
	msg.Category	= MP_CHEAT;
	msg.dwData		= pmsg->dwData2;	//�����ˡ��ϩ���o��ic��O��A ��u����AI��i�ϡ�

	DWORD dwConIdx = pTargetInfo->dwConnectionIndex;
	OnDisconnectUser( dwConIdx );
	DisconnectUser( dwConIdx );
	msg.Protocol	= MP_CHEAT_BANCHARACTER_ACK;

	g_Network.Send2Server( dwConnectionIndex, (char*)&msg, sizeof(msg) );
}


void CheatServer_BanCharacter_Ack(char* pMsg) 
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	USERINFO* pUserInfo = g_pUserTableForObjectID->FindUser( pmsg->dwData );
	if( !pUserInfo ) return ;

	MSGBASE msg;
	msg.Category	= MP_CHEAT;
	msg.Protocol	= MP_CHEAT_BANCHARACTER_ACK;
	msg.dwObjectID	= pmsg->dwData;
	g_Network.Send2User( pUserInfo->dwConnectionIndex, (char*)&msg, sizeof( msg ) );
}


void CheatServer_WhereIs_MapServer_Ack(char* pMsg, DWORD dwLength)
{
	MSG_WHERE_INFO* pmsg = (MSG_WHERE_INFO*)pMsg;
	USERINFO* pSenderInfo = g_pUserTableForObjectID->FindUser( pmsg->dwObjectID );
	if( !pSenderInfo ) return;

	g_Network.Send2User( pSenderInfo->dwConnectionIndex, (char*)pmsg, dwLength );
}


void CheatServer_WhereIs_Nack(char* pMsg, DWORD dwLength)
{
	MSG_BYTE* pmsg = (MSG_BYTE*)pMsg;

	USERINFO* pSenderInfo = g_pUserTableForObjectID->FindUser( pmsg->dwObjectID );
	if( !pSenderInfo ) return;

	g_Network.Send2User( pSenderInfo->dwConnectionIndex, (char*)pmsg, dwLength );
}


void CheatServer_BanMap_Syn(char* pMsg) 
{
	MSG_WORD* pmsg = (MSG_WORD*)pMsg;

	USERINFO* pInfo;
	g_pUserTable->SetPositionUserHead();
	while( (pInfo = g_pUserTable->GetUserData()) != NULL )
	{
		if( pInfo->wUserMapNum == pmsg->wData )
		{
			DWORD dwConIdx = pInfo->dwConnectionIndex;
			OnDisconnectUser( dwConIdx );
			DisconnectUser( dwConIdx );	//3:GMTOOL_BAN
		}
	}
}


void CheatServer_AgentCheck_Syn(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength) 
{
	MSGBASE* pmsg = (MSGBASE*)pMsg;
	pmsg->Protocol = MP_CHEAT_AGENTCHECK_ACK;

	g_Network.Send2Server( dwConnectionIndex, (char*)pmsg, dwLength );
}


void CheatServer_AgentCheck_Ack(char* pMsg, DWORD dwLength) 
{
	MSGBASE* pmsg = (MSGBASE*)pMsg;
	USERINFO* pInfo = g_pUserTableForObjectID->FindUser( pmsg->dwObjectID );
	if( pInfo )
		g_Network.Send2User( pInfo->dwConnectionIndex, (char*)pmsg, dwLength );
}


void CheatServer_Notice_Syn(char* pMsg) 
{
	MSG_CHAT_WORD* pmsg = (MSG_CHAT_WORD*)pMsg;

	TESTMSG msg;
	msg.Category = MP_SIGNAL;
	msg.Protocol = MP_SIGNAL_COMMONUSER;
	SafeStrCpy( msg.Msg, pmsg->Msg, MAX_CHAT_LENGTH + 1 );

	USERINFO* pInfo = NULL;
	g_pUserTable->SetPositionUserHead();
	while( (pInfo = (USERINFO *)g_pUserTable->GetUserData()) != NULL )
	{	
		if( pmsg->wData == 0 || pmsg->wData == pInfo->wUserMapNum )
		{
			TESTMSG msgTemp = msg;
			g_Network.Send2User( pInfo->dwConnectionIndex, (char*)&msgTemp, msgTemp.GetMsgLength() );	//CHATMSG 040324
		}
	}
}


void CheatServer_Event_Syn(char* pMsg) 
{
	MSG_WORD2* pmsg = (MSG_WORD2*)pMsg;
			
	if( pmsg->wData2 == 1 )
	{
		GMINFO->SetEventCheat( TRUE );
	}
	else
	{
		GMINFO->SetEventCheat( FALSE );
	}
}


void CheatServer_EventMap_Result(char* pMsg) 
{
	MSG_NAME_WORD* pmsg = (MSG_NAME_WORD*)pMsg;
	USERINFO* pInfo = NULL;
	g_pUserTable->SetPositionUserHead();
	while( (pInfo = g_pUserTable->GetUserData()) != NULL )
	{
		if( pInfo->UserLevel == eUSERLEVEL_GM )
		{
			MSG_NAME_WORD msgTemp = *pmsg;
			g_Network.Send2User( pInfo->dwConnectionIndex, (char*)&msgTemp, sizeof(msgTemp) );
		}
	}
}


void CheatServer_EventNotify_On(char* pMsg) 
{
	MSG_EVENTNOTIFY_ON* pmsg = (MSG_EVENTNOTIFY_ON*)pMsg;

	g_pServerSystem->SetEventNotifyStr( pmsg->strTitle, pmsg->strContext );
	g_pServerSystem->SetUseEventNotify( TRUE );	//seteventnotifystr���Ŀ� ȣ���Ͻÿ�
}


void CheatServer_EventNotify_Off(char* pMsg) 
{
	g_pServerSystem->SetUseEventNotify(
		FALSE);
}


void CheatServer_PlusTime_On(char* pMsg) 
{
	PLUSTIMEMGR->PlusTimeOn();
}


void CheatServer_PlusTime_AllOff(char* pMsg) 
{
	PLUSTIMEMGR->PlusTimeOff();
}



void MP_HACKCHECKMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGROOT* pTempMsg = (MSGROOT*)pMsg;

	switch(pTempMsg->Protocol)
	{

	case MP_HACKCHECK_SPEEDHACK:
		{
			MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
			USERINFO* pInfo = g_pUserTableForObjectID->FindUser( pmsg->dwObjectID );
			if( !pInfo ) break;
			
			DWORD curTime = MHTIMEMGR_OBJ->GetNewCalcCurTime();
			if( curTime - pmsg->dwData < SPEEDHACK_CHECKTIME - 3000 )	//1000->2000
			{
				MSGBASE msg;
				msg.Category = MP_HACKCHECK;
				msg.Protocol = MP_HACKCHECK_BAN_USER;
				g_Network.Send2User( pInfo->dwConnectionIndex, (char*)&msg, sizeof(msg) );
				
				MSG_DWORD msgMap;
				msgMap.Category		= MP_HACKCHECK;
				msgMap.Protocol		= MP_HACKCHECK_BAN_USER;
				msgMap.dwObjectID	= pmsg->dwObjectID;
				msgMap.dwData		= curTime - pmsg->dwData;	//
				
				g_Network.Send2Server( pInfo->dwMapServerConnectionIndex, (char*)&msgMap, sizeof(msgMap) );

				DWORD dwConIdx = pInfo->dwConnectionIndex;
				OnDisconnectUser( dwConIdx );
				DisconnectUser( dwConIdx );
			}
		}
		break;
	case MP_HACKCHECK_BAN_USER_TOAGENT:
		{
			MSGBASE* pmsg = (MSGBASE*)pMsg;
			USERINFO* pInfo = g_pUserTableForObjectID->FindUser( pmsg->dwObjectID );
			if( !pInfo ) break;
			
			MSGBASE msg;
			msg.Category = MP_HACKCHECK;
			msg.Protocol = MP_HACKCHECK_BAN_USER;
			g_Network.Send2User( pInfo->dwConnectionIndex, (char*)&msg, sizeof(msg) );
			
			MSG_DWORD msgMap;
			msgMap.Category	= MP_HACKCHECK;
			msgMap.Protocol = MP_HACKCHECK_BAN_USER;
			msgMap.dwObjectID = pmsg->dwObjectID;
			msgMap.dwData = 0;
				
			g_Network.Send2Server( pInfo->dwMapServerConnectionIndex, (char*)&msgMap, sizeof(msgMap) );

			DWORD dwConIdx = pInfo->dwConnectionIndex;
			OnDisconnectUser( dwConIdx );
			DisconnectUser( dwConIdx );
		}
		break;
		// 090109 LUJ, Ŭ���̾�Ʈ���� ������ �����ϴ� �˻� �ڵ带 �����ϴ� ��������
	case MP_HACKCHECK_SCRIPTHACK_SYN:
		{
			TransToMapServerMsgParser(dwConnectionIndex, pMsg, dwLength);
		}
		break;
		// 090109 LUJ, �������� Ŭ���̾�Ʈ�� ���� ���Ḧ �䱸�ϴ� ��������
	case MP_HACKCHECK_SCRIPTHACK_ACK:
		{
			const MSGBASE* const	message		= ( MSGBASE* )pMsg;
			const USERINFO* const	userInfo	= g_pUserTableForObjectID->FindUser( message->dwObjectID );

			if( userInfo )
			{
				g_Network.Send2User(
					userInfo->dwConnectionIndex,
					pMsg,
					dwLength);
			}
		}
		break;
		// 090115 LUJ, ���� �ð� ���� ������ ���� �������� ó���� �и�
	case MP_HACKCHECK_SCRIPTHACK_NACK:
		{
			const MSG_DWORD* const	message		= ( MSG_DWORD* )pMsg;
			const USERINFO* const	userInfo	= g_pUserTableForObjectID->FindUser( message->dwObjectID );

			if( ! userInfo )
			{
				break;
			}
			// 090115 LUJ, �����ڰ� �ƴ� ��� �α��� ���� ��� �ִ´�
			else if( eUSERLEVEL_GM < userInfo->UserLevel )
			{
				const DWORD loginBlockSecond = message->dwData;

				PunishListAdd(
					userInfo->dwUserID,
					ePunish_Login,
					loginBlockSecond );
			}

			g_Network.Send2User(
				userInfo->dwConnectionIndex,
				pMsg,
				dwLength );
		}
		break;
	}
}

void MP_GUILDServerMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	/*
	��ε�ĳ���� ����

	- ������Ʈ�� ���� ��ε�ĳ����: Ư�� ȸ������ �޽����� ������ ������
	- �ʿ� ���� ��ε�ĳ����: ��ü �ʿ� �޽����� ������ ������
	*/
	MSGBASE*				pTempMsg = ( MSGBASE* )pMsg;
	const MP_PROTOCOL_GUILD protocol = ( MP_PROTOCOL_GUILD )pTempMsg->Protocol;

	switch( protocol )
	{
	case MP_GUILD_CREATE_NOTIFY_TO_MAP:
	case MP_GUILD_ADDMEMBER_NOTIFY_TO_MAP:
	case MP_GUILD_DELETEMEMBER_NOTIFY_TO_MAP:
	case MP_GUILD_BREAKUP_NOTIFY_TO_MAP:
	case MP_GUILD_SECEDE_NOTIFY_TO_MAP:
	case MP_GUILDMARK_NOTIFY_TO_MAP:
	case MP_GUILD_LEVELUP_NOTIFY_TO_MAP:
	case MP_GUILD_MEMBERLEVEL_NOTIFY_TO_MAP:
	case MP_GUILD_CHANGERANK_NOTIFY_TO_MAP:
	case MP_GUILD_LOGINFO_NOTIFY_TO_MAP:
	case MP_GUILD_NOTICE_NOTIFY:
	case MP_GUILD_CHANGE_MASTER_TO_AGENT:
	case MP_GUILD_SET_MEMBER_TO_MAP:
	// 080417 LUJ, ��� ��ų ȹ�� ����
	case MP_GUILD_ADD_SKILL_ACK_TO_MAP:
	// 080602 LUJ, ��� ���� ���ΰ�ħ�� ��û�Ѵ�
	case MP_GUILD_GTOURNAMENT_NOTIFY_TO_MAP:
	case MP_GUILD_SCORE_UPDATE_TO_MAP:
		{
			g_Network.Broadcast2MapServerExceptOne(dwConnectionIndex, pMsg, dwLength);

			break;
		}
	case MP_GUILD_GIVENICKNAME_NOTIFY_TO_MAP:
		{
			SEND_GUILD_NICKNAME * pmsg = (SEND_GUILD_NICKNAME*)pMsg;
			USERINFO* pInfo = g_pUserTableForObjectID->FindUser( pmsg->TargetIdx );
			if(pInfo == 0)
				return;
			g_Network.Broadcast2MapServerExceptOne(dwConnectionIndex, pMsg, dwLength);

			break;
		}
	//case MP_GUILD_LOGIN_MEMBER_TO_AGENT:
	//	{
	//		pTempMsg->Protocol	= MP_GUILD_LOGIN_MEMBER_TO_USER;

	//		g_Network.Broadcast2AgentServer( ( char* )pTempMsg, dwLength );

	//		// ����: break�� ���� �� �ǵ����� ����. ��ε�ĳ��Ʈ�� �ڽ��� ������ �������� ������ ������
	//	}
	//case MP_GUILD_LOGIN_MEMBER_TO_USER:
	//	{
	//		const USERINFO* pInfo = g_pUserTableForObjectID->FindUser( pTempMsg->dwObjectID );

	//		if( pInfo )
	//		{
	//			g_Network.Send2User( pTempMsg, dwLength );
	//		}

	//		break;
	//	}
	//case MP_GUILD_MEMBER_SEARCH_TO_AGENT:
	//	{
	//		pTempMsg->Protocol	= MP_GUILD_MEMBER_SEARCH_TO_MAP;

	//		g_Network.Broadcast2MapServer( pMsg, dwLength );
	//		break;
	//	}
	//case MP_GUILD_LOGOUT_MEMBER_TO_AGENT:
	//	{
	//		pTempMsg->Protocol	= MP_GUILD_LOGOUT_MEMBER_TO_MAP;

	//		g_Network.Broadcast2MapServer( pMsg, dwLength );
	//		break;
	//	}
	//case MP_GUILD_SCORE_UPDATE_TO_MAP:
	//	{
	//		g_Network.Broadcast2MapServer( ( char* )pTempMsg, dwLength );
	//		break;
	//	}
	default:
		{
			TransToClientMsgParser(dwConnectionIndex, pMsg, dwLength);
			break;
		}
	}
}

void MP_GUILDUserMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGBASE*				pTempMsg = ( MSGBASE* )pMsg;
	const MP_PROTOCOL_GUILD protocol = ( MP_PROTOCOL_GUILD )pTempMsg->Protocol;

	switch( protocol )
	{
	case MP_GUILD_CREATE_SYN:
		{
			SEND_GUILD_CREATE_SYN * pmsg = (SEND_GUILD_CREATE_SYN*)pMsg;
			USERINFO* pInfo = g_pUserTableForObjectID->FindUser( pmsg->dwObjectID );
			if( !pInfo ) break;

			if( ( FILTERTABLE->IsInvalidCharInclude((unsigned char *)pmsg->GuildName) == TRUE ) || 
				( FILTERTABLE->IsUsableName(pmsg->GuildName) == FALSE ) )
			{
				MSG_BYTE msg;
				msg.Category = MP_GUILD;
				msg.Protocol = MP_GUILD_CREATE_NACK;
				msg.bData = eGuildErr_Create_Invalid_Name;
				g_Network.Send2User( pInfo->dwConnectionIndex, (char*)&msg, sizeof(msg) );
			}
			else
			{
				TransToMapServerMsgParser(dwConnectionIndex, pMsg, dwLength);
			}

			break;
		}
	case MP_GUILD_GIVENICKNAME_SYN:
		{
			SEND_GUILD_NICKNAME * pmsg = (SEND_GUILD_NICKNAME *)pMsg;
			USERINFO* pInfo = g_pUserTableForObjectID->FindUser( pmsg->dwObjectID );
			if( !pInfo ) break;

//			if(( FILTERTABLE->IsUsableName(pmsg->NickName, FALSE ) == FALSE ) || (FILTERTABLE->IsCharInString(pmsg->NickName, " '") == TRUE) )
//			{
//				MSG_BYTE msg;
//				msg.Category = MP_GUILD;
//				msg.Protocol = MP_GUILD_GIVENICKNAME_NACK;
//				msg.bData = eGuildErr_Nick_Filter;
//				g_Network.Send2User( pInfo->dwConnectionIndex, (char*)&msg, sizeof(msg) );
//			}
//			else
			// 090317 NYJ - "'"�� �ǿ��� ������ �����Ƿ� �ڵ庹��
			if((FILTERTABLE->IsCharInString(pmsg->NickName, " '") == TRUE) )
			{
				MSG_BYTE msg;
				msg.Category = MP_GUILD;
				msg.Protocol = MP_GUILD_GIVENICKNAME_NACK;
				msg.bData = eGuildErr_Nick_Filter;
				g_Network.Send2User( pInfo->dwConnectionIndex, (char*)&msg, sizeof(msg) );
			}
			else
			{
				TransToMapServerMsgParser(dwConnectionIndex, pMsg, dwLength);
			}

			break;
		}		
	case MP_GUILD_RETIRE_SYN:
		{
			MSG_DWORD*		message		= ( MSG_DWORD* )pTempMsg;
			const DWORD		playerIndex	= message->dwData;
			const USERINFO* pInfo		= g_pUserTableForObjectID->FindUser( playerIndex );

			message->dwObjectID	= playerIndex;

			if( pInfo )
			{
				g_Network.Send2User( message, dwLength );
			}
			else
			{
				g_Network.Broadcast2AgentServerExceptSelf( pMsg, dwLength );
			}

			break;
		}
	case MP_GUILD_RETIRE_NACK:
		{
			const USERINFO* pInfo = g_pUserTableForObjectID->FindUser( pTempMsg->dwObjectID );

			if( pInfo )
			{
				g_Network.Send2User( pTempMsg, dwLength );
			}
			else
			{
				g_Network.Broadcast2AgentServerExceptSelf( pMsg, dwLength );
			}

			break;
		}
	default:
		{
			TransToMapServerMsgParser(dwConnectionIndex, pMsg, dwLength);
			break;
		}
	}
}

// 070801 ����, ���� �йи� �����ϸ� �ּ�ó��

//void MP_FAMILYServerMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
//{
//	MSGBASE* pTempMsg = (MSGBASE*)pMsg;
//	switch(pTempMsg->Protocol)
//	{
//	case MP_FAMILY_CREATE_NOTIFY_TO_MAP:
//	case MP_FAMILY_ADDMEMBER_NOTIFY_TO_MAP:
//	case MP_FAMILY_DELETEMEMBER_NOTIFY_TO_MAP:
//	case MP_FAMILY_BREAKUP_NOTIFY_TO_MAP:
//	case MP_FAMILY_SECEDE_NOTIFY_TO_MAP:
//	case MP_FAMILYMARK_NOTIFY_TO_MAP:
//	case MP_FAMILY_LEVELUP_NOTIFY_TO_MAP:
//	case MP_FAMILY_MEMBERLEVEL_NOTIFY_TO_MAP:
//	case MP_FAMILY_CHANGERANK_NOTIFY_TO_MAP:
//	case MP_FAMILY_LOGINFO_NOTIFY_TO_MAP:
//	case MP_FAMILY_CHANGE_LOCATION_NOTIFY_TO_MAP:
//	case MP_FAMILY_NOTICE_NOTIFY:
//	case MP_FAMILY_HUNTEDMONSTER_TOTALCOUNT_TO_MAP:
//	case MP_FAMILY_PLUSTIME_SET_TO_MAP:
//	case MP_FAMILY_PLUSTIME_END_TO_MAP:
//	case MP_FAMILY_POINT_ADD_TO_MAP:
//	case MP_FAMILY_POINT_USEINFO_TO_MAP:
//	case MP_FAMILY_POINT_TO_MAP:
//		{
//			g_Network.Broadcast2MapServerExceptOne(dwConnectionIndex, pMsg, dwLength);
//		}		
//		break;
//	case MP_FAMILY_GIVENICKNAME_NOTIFY_TO_MAP:
//		{
//			SEND_FAMILY_NICKNAME * pmsg = (SEND_FAMILY_NICKNAME*)pMsg;
//			USERINFO* pInfo = g_pUserTableForObjectID->FindUser( pmsg->TargetIdx );
//			if(pInfo == 0)
//				return;
//			g_Network.Broadcast2MapServerExceptOne(dwConnectionIndex, pMsg, dwLength);
//		}
//		break;
//	default:
//		TransToClientMsgParser(dwConnectionIndex, pMsg, dwLength);
//	}
//}


// 070801 ����, ���� �йи� �����ϸ� �ּ�ó��

//void MP_FAMILYUserMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
//{
//	MSGBASE* pTempMsg = (MSGBASE*)pMsg;
//	switch(pTempMsg->Protocol)
//	{
//	case MP_FAMILY_CREATE_SYN:
//		{
//			SEND_FAMILY_CREATE_SYN * pmsg = (SEND_FAMILY_CREATE_SYN*)pMsg;
//			USERINFO* pInfo = g_pUserTableForObjectID->FindUser( pmsg->dwObjectID );
//			if( !pInfo ) break;
//
//			if( ( FILTERTABLE->IsInvalidCharInclude((unsigned char *)pmsg->FamilyName) == TRUE ) || 
//				( FILTERTABLE->IsUsableName(pmsg->FamilyName) == FALSE ) )
//			{
//				MSG_BYTE msg;
//				msg.Category = MP_FAMILY;
//				msg.Protocol = MP_FAMILY_CREATE_NACK;
//				msg.bData = eFamilyErr_Create_Name;
//				g_Network.Send2User( pInfo->dwConnectionIndex, (char*)&msg, sizeof(msg) );
//			}
//			else
//				TransToMapServerMsgParser(dwConnectionIndex, pMsg, dwLength);
//		}
//		break;
//	case MP_FAMILY_GIVENICKNAME_SYN:
//		{
//			SEND_FAMILY_NICKNAME * pmsg = (SEND_FAMILY_NICKNAME *)pMsg;
//			USERINFO* pInfo = g_pUserTableForObjectID->FindUser( pmsg->dwObjectID );
//			if( !pInfo ) break;
//
//			if(( FILTERTABLE->IsUsableName(pmsg->NickName, FALSE ) == FALSE ) || (FILTERTABLE->IsCharInString(pmsg->NickName, " '") == TRUE) )
//			{
//				MSG_BYTE msg;
//				msg.Category = MP_FAMILY;
//				msg.Protocol = MP_FAMILY_GIVENICKNAME_NACK;
//				msg.bData = eFamilyErr_Nick_Filter;
//				g_Network.Send2User( pInfo->dwConnectionIndex, (char*)&msg, sizeof(msg) );
//			}
//			else
//				TransToMapServerMsgParser(dwConnectionIndex, pMsg, dwLength);
//		}
//		break;
//	default:
//		TransToMapServerMsgParser(dwConnectionIndex, pMsg, dwLength);
//	}
//}

void MP_GUILD_WARServerMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGBASE* pTempMsg = (MSGBASE*)pMsg;
	switch( pTempMsg->Protocol )
	{
	case MP_GUILD_WAR_DECLARE_DENY_NOTIFY_TOMAP:
	case MP_GUILD_WAR_START_NOTIFY_TOMAP:
	case MP_GUILD_WAR_END_NOTIFY_TOMAP:
	case MP_GUILD_WAR_SUGGESTEND_NOTIFY_TOMAP:
	case MP_GUILD_WAR_SUGGESTEND_ACCEPT_NOTIFY_TOMAP:
	case MP_GUILD_WAR_SUGGESTEND_DENY_NOTIFY_TOMAP:
	case MP_GUILD_WAR_SURRENDER_NOTIFY_TOMAP:
		{
			g_Network.Broadcast2MapServerExceptOne(dwConnectionIndex, pMsg, dwLength);
		}		
		break;		
	case MP_GUILD_WAR_DECLARE_NACK:
		{
			MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
			USERINFO* pInfo = g_pUserTableForObjectID->FindUser( pmsg->dwData1 );
			if( pInfo )
			{
				g_Network.Send2User( pInfo->dwConnectionIndex, pMsg, sizeof(MSG_DWORD2) );
			}
		}
		break;
	case MP_GUILD_WAR_ADDMONEY_TOMAP:
		{
			MSG_DWORDEX2* pmsg = (MSG_DWORDEX2*)pMsg;

			AddGuildFieldWarMoney( dwConnectionIndex, (DWORD)pmsg->dweData1, pmsg->dweData2 );
		}
		break;
	case MP_GUILD_WAR_RESULT_TOALLUSER:
		{
			MSG_NAME2* pmsg = (MSG_NAME2*)pMsg;
			MSG_NAME2 Msg;

			USERINFO* pInfo = NULL;
			g_pUserTable->SetPositionUserHead();
			while( (pInfo = (USERINFO*)g_pUserTable->GetUserData()) != NULL )
			{	
				Msg = *pmsg;
				g_Network.Send2User( pInfo->dwConnectionIndex, (char*)&Msg, dwLength );
			}
		}
		break;
	default:
		TransToClientMsgParser(dwConnectionIndex, pMsg, dwLength);
	}
}

void MP_GUILD_WARUserMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGBASE* pTempMsg = (MSGBASE*)pMsg;
	switch( pTempMsg->Protocol )
	{
	case MP_GUILD_WAR_DECLARE:
	case MP_GUILD_WAR_SUGGESTEND:
		{
			MSG_NAME_DWORD* pmsg = (MSG_NAME_DWORD*)pMsg;

			// 090325 ONS String Overflow ����
			char szBuf[MAX_NAME_LENGTH+1];
			SafeStrCpy( szBuf, pmsg->Name, MAX_NAME_LENGTH+1 );

			CheckGuildMasterLogin( dwConnectionIndex, pmsg->dwObjectID, szBuf, pmsg->dwData, pTempMsg->Protocol );			
		}
		break;
	case MP_GUILD_WAR_DECLARE_ACCEPT:
		{
			MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
			CheckGuildFieldWarMoney( dwConnectionIndex, pmsg->dwObjectID, pmsg->dwData1, pmsg->dwData2 );
		}
		break;
	default:
		{
			TransToMapServerMsgParser(dwConnectionIndex, pMsg, dwLength);
		}
		break;
	}
}

BOOL CheckCharacterMakeInfo( CHARACTERMAKEINFO* pmsg )
{	
// �ӽ÷� ����......
//------------------------------------------------------------------------------------------------------------------------------	
	int nHairType = 0 ;
	int nFaceType = 0 ;

	// 090504 ONS �ű����� ĳ���� ���� üũ
	switch(pmsg->RaceType)
	{
	case RaceType_Human :
		{
			switch(pmsg->SexType)
			{
			case 0 :
				{
					nHairType = 9 ;
					nFaceType = 9 ;
				}
				break ;

			case 1 :
				{
					nHairType = 9 ;
					nFaceType = 9 ;
				}
				break ;
			}
		}
		break ;

	case RaceType_Elf :
		{
			switch(pmsg->SexType)
			{
			case 0 :
				{
					nHairType = 9 ;
					nFaceType = 9 ;
				}
				break ;

			case 1 :
				{
					nHairType = 9 ;
					nFaceType = 9 ;
				}
				break ;
			}
		}
		break ;
	case RaceType_Devil :
		{
			switch(pmsg->SexType)
			{
			case 0 :
				{
					nHairType = 9 ;
					nFaceType = 9 ;
				}
				break ;

			case 1 :
				{
					nHairType = 9 ;
					nFaceType = 9 ;
				}
				break ;
			}
		}
		break ;
	}

	if( pmsg->SexType > 1 )
		return FALSE;
	if( pmsg->HairType > nHairType )
		return FALSE;
	if( pmsg->FaceType > nFaceType )
		return FALSE;
//	if( pmsg->StartArea != 17 )
//		return FALSE;
	return TRUE;
}

void MP_ITEMUserMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGROOT* pTempMsg = (MSGROOT*)pMsg;
	switch(pTempMsg->Protocol)
	{
	case MP_ITEM_GIFT_EVENT_START:
		{
			MSGBASE* pmsg = ( MSGBASE* )pMsg;
			USERINFO* pUser = g_pUserTableForObjectID->FindUser( pmsg->dwObjectID );
			if( !pUser )		
			{
				FILE* pFile = fopen( "./log/GiftEventLog.txt", "a+" );
				fprintf( pFile, "%u\tInvalid User(MP_ITEMUserMsgParser) - character_idx : %u\n", gCurTime, pmsg->dwObjectID );
				fclose( pFile );
				return;
			}

			// 090903 pdy �Ϲ������� Gift Event�� ���� ���ϰ� �Ǿ��ִ� ����ó�� �ּ� 
			/*if( pUser->UserLevel > eUSERLEVEL_GM )
				return;*/

			DWORD count = GIFTMGR->GetStartEventCount();
			DWORD* index = GIFTMGR->GetStartEventIndex();

			for( DWORD i = 0; i < count; i++ )
			{
				GIFTMGR->AddEvent( pmsg->dwObjectID, index[ i ] );
			}
			//GiftEvent( pUser->dwUserID );
		}
		break;
	default:
		TransToMapServerMsgParser( dwConnectionIndex, pMsg, dwLength );
	}
}




void MP_ITEMServerMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGROOT* pTempMsg = (MSGROOT*)pMsg;
	switch(pTempMsg->Protocol)
	{
	case MP_ITEM_CHANGE_CHARACTER_NAME_SYN:
		{
			const MSG_CHANGE_CHARACTER_NAME_AGENT* const msg = (MSG_CHANGE_CHARACTER_NAME_AGENT*)pMsg;

			g_DB.LoginMiddleQuery(
				NULL,
				0,
				"EXEC dbo.UP_CHARACTER_UPDATE_NAME %d, %d, \'%s\', %d",
				msg->CharacterIdx,
				msg->UserIdx,
				msg->changeName,
				g_nServerSetNum);
			break;
		}
	case MP_ITEM_ENCHANT_LEVEL_NOTIFY:
		{
			const MSG_NAME_DWORD3* const pmsg = (MSG_NAME_DWORD3*)pMsg;
			const USERINFO* const pSenderInfo = g_pUserTableForObjectID->FindUser(
				pmsg->dwObjectID);

			if(NULL == pSenderInfo)
			{
				return;
			}

			g_pUserTable->SetPositionUserHead();

			while(const USERINFO* const pInfo = g_pUserTable->GetUserData())
			{
				MSG_NAME_DWORD3 msgTemp = *pmsg;
				g_Network.Send2User(
					pInfo->dwConnectionIndex,
					(char*)&msgTemp,
					sizeof(msgTemp));
			}

			// �ٸ� ������Ʈ�� �޼����� �����Ѵ�.
			MSG_NAME_DWORD3 msg;
			ZeroMemory(&msg, sizeof(msg));
			msg.Category = MP_ITEM;
			msg.Protocol = MP_ITEM_ENCHANT_LEVEL_NOTIFY_OTHER_AGENT;
			msg.dwData1	= pmsg->dwData1;
			msg.dwData2	= pmsg->dwData2;
			SafeStrCpy(
				msg.Name,
				pmsg->Name,
				MAX_NAME_LENGTH + 1);
			g_Network.Broadcast2AgentServerExceptSelf(
				(char*)&msg,
				sizeof(msg));
			break;
		}
	case MP_ITEM_ENCHANT_LEVEL_NOTIFY_OTHER_AGENT:
		{
			g_pUserTable->SetPositionUserHead();

			while(const USERINFO* const pInfo = g_pUserTable->GetUserData())
			{
				MSG_NAME_DWORD3 msgTemp = *((MSG_NAME_DWORD3*)pMsg);
				msgTemp.Protocol = MP_ITEM_ENCHANT_LEVEL_NOTIFY;
				g_Network.Send2User(
					pInfo->dwConnectionIndex,
					(char*)&msgTemp,
					sizeof(msgTemp));
			}

			break;
		}
	default:
		{
			TransToClientMsgParser(
				dwConnectionIndex,
				pMsg,
				dwLength);
			break;
		}
	}
}

#define GTMAPNUM		94
#define GTRETURNMAPNUM	52

void MP_GTOURNAMENTUserMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGROOT* pTempMsg = (MSGROOT*)pMsg;
	switch(pTempMsg->Protocol)
	{
	case MP_GTOURNAMENT_MOVETOBATTLEMAP_SYN:			Gtournament_MoveToBattleMap_Syn(pMsg, dwLength) ;					break;
	case MP_GTOURNAMENT_STANDINGINFO_SYN:				Gtournament_StandingInfo_Syn(pMsg) ;								break;
	case MP_GTOURNAMENT_BATTLEJOIN_SYN:
	case MP_GTOURNAMENT_OBSERVERJOIN_SYN:				Gtournament_ObserverJoin_Syn(pMsg, dwLength) ;						break;
	case MP_GTOURNAMENT_LEAVE_SYN:						Gtournament_Leave_Syn(pMsg) ;										break;
	//case MP_GTOURNAMENT_CHEAT:							Gtournament_Cheat(pMsg) ;											break;
	default:											TransToMapServerMsgParser( dwConnectionIndex, pMsg, dwLength );		break;
	}
}


void Gtournament_MoveToBattleMap_Syn(char* pMsg, DWORD dwLength)
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;

	USERINFO* pUserInfo = g_pUserTableForObjectID->FindUser( pmsg->dwObjectID );
	if( !pUserInfo )			return;

	WORD tomapserverport = 0;
	tomapserverport = g_pServerTable->GetServerPort( eSK_MAP, GTMAPNUM );
	if(tomapserverport)
	{
		SERVERINFO* pInfo = g_pServerTable->FindServer( tomapserverport );
		if(pInfo)
		{
			////g_Network.Send2Server( pInfo->dwConnectionIndex, pMsg, dwLength );
			//g_Network.Send2Server( pUserInfo->dwMapServerConnectionIndex, pMsg, dwLength );
			MSG_DWORD2 msg;
			msg.Category = MP_GTOURNAMENT;
			msg.Protocol = MP_GTOURNAMENT_MOVETOBATTLEMAP_SYN;
			msg.dwData1 = pmsg->dwObjectID;
			msg.dwData2 = pmsg->dwData;
			g_Network.Send2Server( pUserInfo->dwMapServerConnectionIndex, pMsg, dwLength );
			return;
		}
		else
			goto MoveBattleMap_Error;
	}
	else
		goto MoveBattleMap_Error;

MoveBattleMap_Error:
	MSG_DWORD msg;
	msg.Category = MP_GTOURNAMENT;
	msg.Protocol = MP_GTOURNAMENT_MOVETOBATTLEMAP_NACK;
	msg.dwData = eGTError_ERROR;
	g_Network.Send2User( pUserInfo->dwConnectionIndex, (char*)&msg, sizeof(msg) );
}


void Gtournament_StandingInfo_Syn(char* pMsg) 
{
	MSG_WORD* pmsg = (MSG_WORD*)pMsg;

	USERINFO* pUserInfo = g_pUserTableForObjectID->FindUser( pmsg->dwObjectID );
	if( !pUserInfo )			return;

	WORD tomapserverport = 0;
	tomapserverport = g_pServerTable->GetServerPort( eSK_MAP, GTMAPNUM );
	if(tomapserverport)
	{
		SERVERINFO* pInfo = g_pServerTable->FindServer( tomapserverport );
		if(pInfo)
		{
			MSG_DWORD2 msg;
			msg.Category = MP_GTOURNAMENT;
			msg.Protocol = MP_GTOURNAMENT_STANDINGINFO_SYN;
			msg.dwData1 = g_pServerSystem->GetServerNum();
			msg.dwData2 = pmsg->dwObjectID;
			g_Network.Send2Server( pInfo->dwConnectionIndex, (char*)&msg, sizeof(msg) );
			return;
		}
		else
			goto StandingInfo_Error;
	}
	else
		goto StandingInfo_Error;

StandingInfo_Error:
	MSG_DWORD msg;
	msg.Category = MP_GTOURNAMENT;
	msg.Protocol = MP_GTOURNAMENT_STANDINGINFO_NACK;
	msg.dwData = eGTError_ERROR;
	g_Network.Send2User( pUserInfo->dwConnectionIndex, (char*)&msg, sizeof(msg) );
}

void Gtournament_ObserverJoin_Syn(char* pMsg, DWORD dwLength) 
{
	MSG_DWORD3* pmsg = (MSG_DWORD3*)pMsg;

	USERINFO* pUserInfo = g_pUserTableForObjectID->FindUser( pmsg->dwObjectID );
	if( !pUserInfo )			return;

	WORD tomapserverport = g_pServerTable->GetServerPort( eSK_MAP, GTMAPNUM );
	if(tomapserverport)
	{
		SERVERINFO* pInfo = g_pServerTable->FindServer( tomapserverport );
		if(pInfo)
		{
			pUserInfo->wUserMapNum = (WORD)pmsg->dwData3;
			pUserInfo->dwMapServerConnectionIndex = pInfo->dwConnectionIndex;

			SEND_BATTLEJOIN_INFO msg;
			msg.Category = MP_GTOURNAMENT;
			msg.Protocol = pmsg->Protocol;
			msg.dwObjectID = pmsg->dwObjectID;
			msg.AgentIdx = pUserInfo->dwUniqueConnectIdx;					
			msg.UserLevel = pUserInfo->UserLevel;
			msg.GuildIdx = pmsg->dwData1;
			msg.BattleIdx = pmsg->dwData2;
			msg.ReturnMapNum = pmsg->dwData3;
			g_Network.Send2Server( pInfo->dwConnectionIndex, (char*)&msg, sizeof(msg));
			return;
		}
	}

//BattleJoin_Err:
	MSG_DWORD msg;
	msg.Category = MP_GTOURNAMENT;
	msg.Protocol = MP_GTOURNAMENT_BATTLEJOIN_NACK;
	msg.dwData = eGTError_ERROR;
	g_Network.Send2User( pUserInfo->dwConnectionIndex, pMsg, dwLength );
}


void Gtournament_Leave_Syn(char* pMsg) 
{
	MSGBASE* pmsg = (MSGBASE*)pMsg;

	USERINFO* pUserInfo = g_pUserTableForObjectID->FindUser( pmsg->dwObjectID );
	if( !pUserInfo )			return;

	WORD tomapserverport = g_pServerTable->GetServerPort( eSK_MAP, GTRETURNMAPNUM );
	if(tomapserverport)
	{
		SERVERINFO* pInfo = g_pServerTable->FindServer( tomapserverport );
		if(pInfo)
		{
			pUserInfo->wUserMapNum = GTRETURNMAPNUM;
			pUserInfo->dwMapServerConnectionIndex = pInfo->dwConnectionIndex;
			// 090824 ONS �����ʸ�Ʈ ����� 1��ä�η� �����ϵ��� ����.
			pUserInfo->wChannel = 0;

			MSG_DWORD3 msg;
			msg.Category = MP_GTOURNAMENT;
			msg.Protocol = MP_GTOURNAMENT_LEAVE_SYN;
			msg.dwObjectID = pmsg->dwObjectID;
			msg.dwData1 = pUserInfo->dwUniqueConnectIdx;
			msg.dwData2 = pUserInfo->UserLevel;
			msg.dwData3 = pUserInfo->wChannel;
			g_Network.Send2Server( pInfo->dwConnectionIndex, (char*)&msg, sizeof(msg));
			return;
		}
	}
	else
	{
		// �̵��Ϸ��� ���� ����..��.��
	}
}


void Gtournament_Cheat(char* pMsg) 
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
			
	USERINFO* pUserInfo = g_pUserTableForObjectID->FindUser( pmsg->dwObjectID );
	if( !pUserInfo )			return;
	
	MSG_DWORD2 msg;
	msg.Category = MP_GTOURNAMENT;
	msg.Protocol = MP_GTOURNAMENT_CHEAT;
	msg.dwData1 = pmsg->dwObjectID;
	msg.dwData2 = pmsg->dwData;
	if( pmsg->dwData == 1 )
	{
		g_Network.Send2Server( pUserInfo->dwMapServerConnectionIndex, (char*)&msg, sizeof(msg));
	}
	else if( pmsg->dwData == 2 )
	{
		WORD tomapserverport = g_pServerTable->GetServerPort( eSK_MAP, GTMAPNUM );
		if(tomapserverport)
		{
			SERVERINFO* pInfo = g_pServerTable->FindServer( tomapserverport );
			if(pInfo)
			{
				g_Network.Send2Server( pInfo->dwConnectionIndex, (char*)&msg, sizeof(msg));
			}
		}
	}
}




void MP_GTOURNAMENTServerMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGROOT* pTempMsg = (MSGROOT*)pMsg;
	switch(pTempMsg->Protocol)
	{
	case MP_GTOURNAMENT_STANDINGINFO_NACK:			GtournamentServer_StandingInfo_Nack(pMsg, dwLength);					break;
	case MP_GTOURNAMENT_STANDINGINFO_ACK:			GtournamentServer_StandingInfo_Ack(pMsg, dwLength);						break;
	case MP_GTOURNAMENT_NOTIFY_WINLOSE:				GtournamentServer_Notify_WinLose(pMsg, dwLength) ;						break;
	case MP_GTOURNAMENT_NOTIFY:						GtournamentServer_Notify(pMsg, dwLength);								break;
	case MP_GTOURNAMENT_NOTIFY2GUILD_TOMAP:			GtournamentServer_Notify2GuildToMap(pMsg, dwLength);					break;
	case MP_GTOURNAMENT_NOTIFY2PLAYER_TOMAP:		GtournamentServer_Notify2PlayerToMap(pMsg, dwLength);					break;
	case MP_GTOURNAMENT_REWARD:						GtournamentServer_Reward(pMsg, dwLength);								break;
	// 091202 ONS ���� ��°�� ���� ó�� �߰�
	case MP_GTOURNAMENT_REWARD_MONEY_TO_MAP:		GtournamentServer_Reward_Money(pMsg, dwLength);							break;
	default:										TransToClientMsgParser( dwConnectionIndex, pMsg, dwLength );			break;
	}
}


void GtournamentServer_Cheat(char* pMsg, DWORD dwLength)
{
	MSG_DWORD3* pmsg = (MSG_DWORD3*)pMsg;

	USERINFO* pUserInfo = g_pUserTableForObjectID->FindUser( pmsg->dwData1 );
	if( !pUserInfo )			return;

	g_Network.Send2User( pUserInfo->dwConnectionIndex, pMsg, dwLength );
}

void GtournamentServer_StandingInfo_Nack(char* pMsg, DWORD dwLength)
{
	MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;

	USERINFO* pUserInfo = g_pUserTableForObjectID->FindUser(pmsg->dwData1);
	if( !pUserInfo )			return;

	g_Network.Send2User( pUserInfo->dwConnectionIndex, pMsg, dwLength);
}

void GtournamentServer_StandingInfo_Ack(char* pMsg, DWORD dwLength)
{
	SEND_REGISTEDGUILDINFO* pmsg = (SEND_REGISTEDGUILDINFO*)pMsg;

	USERINFO* pUserInfo = g_pUserTableForObjectID->FindUser( pmsg->PlayerID );
	if( !pUserInfo )			return;

	g_Network.Send2User(pUserInfo->dwConnectionIndex, pMsg, dwLength);
}


void GtournamentServer_ReturntoMap(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;

	USERINFO* pUserInfo = g_pUserTableForObjectID->FindUser( pmsg->dwObjectID );
	if( !pUserInfo )			return;

	WORD tomapserverport = g_pServerTable->GetServerPort( eSK_MAP, (WORD)pmsg->dwData );
	if(tomapserverport)
	{
		SERVERINFO* pInfo = g_pServerTable->FindServer( tomapserverport );
		if( pInfo )
		{
			pUserInfo->wUserMapNum = (WORD)pmsg->dwData;
			pUserInfo->dwMapServerConnectionIndex = pInfo->dwConnectionIndex;
		}
	}

	TransToClientMsgParser( dwConnectionIndex, pMsg, dwLength );

	//--------------------------
}


void GtournamentServer_Notify_WinLose(char* pMsg, DWORD dwLength)
{
	SEND_GTWINLOSE* pmsg = (SEND_GTWINLOSE*)pMsg;

	USERINFO* info = NULL;
	g_pUserTable->SetPositionUserHead();
	while( (info = g_pUserTable->GetUserData()) != NULL )
	{
		SEND_GTWINLOSE msg = *pmsg;
		g_Network.Send2User( info->dwConnectionIndex, (char*)&msg, dwLength );
	}
}

void GtournamentServer_Notify(char* pMsg, DWORD dwLength)
{
	MSG_DWORD3* pmsg = (MSG_DWORD3*)pMsg;

	USERINFO* info = NULL;
	g_pUserTable->SetPositionUserHead();
	while( (info = g_pUserTable->GetUserData()) != NULL )
	{
		MSG_DWORD3 msg = *pmsg;
		g_Network.Send2User( info->dwConnectionIndex, (char*)&msg, dwLength );
	}
}

void GtournamentServer_Notify2GuildToMap(char* pMsg, DWORD dwLength)
{
	g_Network.Broadcast2MapServer(pMsg, dwLength);
}

void GtournamentServer_Notify2PlayerToMap(char* pMsg, DWORD dwLength)
{
	MSG_DWORD5* pmsg = (MSG_DWORD5*)pMsg;

	USERINFO* pInfo = g_pUserTableForObjectID->FindUser(pmsg->dwData1);
	if(pInfo)
	{
		MSG_DWORD5 msg;
		memcpy(&msg, pMsg, sizeof(msg));
		msg.Protocol = MP_GTOURNAMENT_NOTIFY2PLAYER;		// Ŭ���̾�Ʈ���� ������ ��Ŷ���� ����

		g_Network.Send2User( pInfo->dwConnectionIndex, (char*)&msg, sizeof(msg) );
	}
}

void GtournamentServer_Reward(char* pMsg, DWORD dwLength) 
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;

	USERINFO* pInfo = g_pUserTableForObjectID->FindUser(pmsg->dwObjectID);
	if(pInfo)
	{
		g_Network.Send2User( pInfo->dwConnectionIndex, pMsg, dwLength );
	}
}

// 091202 ONS ���� ��°�� ���� ó�� �߰�
void GtournamentServer_Reward_Money(char* pMsg, DWORD dwLength) 
{
	g_Network.Broadcast2MapServer(pMsg, dwLength);
}


void GtournamentServer_Battle_Notify(char* pMsg, DWORD dwLength)
{
	MSG_DWORD3* pmsg = (MSG_DWORD3*)pMsg;

	USERINFO* info = NULL;
	g_pUserTable->SetPositionUserHead();
	while( (info = g_pUserTable->GetUserData()) != NULL )
	{
		MSG_DWORD3 msg = *pmsg;
		g_Network.Send2User( info->dwConnectionIndex, (char*)&msg, dwLength );
	}
}


void GtournamentServer_EndBattle(char* pMsg, DWORD dwLength) 
{
	MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;

	USERINFO * info = NULL;
	g_pUserTable->SetPositionUserHead();
	while( (info = g_pUserTable->GetUserData()) != NULL )
	{
		MSG_DWORD2 msg = *pmsg;
		g_Network.Send2User( info->dwConnectionIndex, (char*)&msg, dwLength );
	}
}

void MP_SkillServerMsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength )
{
	MSGROOT* pTempMsg = (MSGROOT*)pMsg;
	switch(pTempMsg->Protocol)
	{
	// 080602 LUJ, ������ ��� ��ų ����� �ٸ� ������ �˸���
	case MP_SKILL_START_TO_MAP:
		{
			g_Network.Broadcast2MapServerExceptOne( dwConnectionIndex, pMsg, dwLength );
		}
		break;
	default:
		TransToClientMsgParser( dwConnectionIndex, pMsg, dwLength );		
		break;
	}
}

// guildunion
void MP_GUILD_UNIONUserMsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength )
{
	MSGBASE* pTempMsg = (MSGBASE*)pMsg;
	switch( pTempMsg->Protocol )
	{
	case MP_GUILD_UNION_CREATE_SYN:
		{
			MSG_NAME_DWORD* pmsg = (MSG_NAME_DWORD*)pMsg;
			USERINFO* pInfo = g_pUserTableForObjectID->FindUser( pmsg->dwObjectID );
			if( !pInfo )	return;

			if( (FILTERTABLE->IsInvalidCharInclude( (unsigned char*)pmsg->Name ) == TRUE ) || 
				(FILTERTABLE->IsUsableName( pmsg->Name ) == FALSE ) )
			{
				MSG_DWORD2 Msg;
				Msg.Category = MP_GUILD_UNION;
				Msg.Protocol = MP_GUILD_UNION_CREATE_NACK;
				Msg.dwData1 = eGU_Not_ValidName;
				Msg.dwData2 = 0;
				g_Network.Send2User( pInfo->dwConnectionIndex, (char*)&Msg, sizeof(Msg) );
			}
			else
				TransToMapServerMsgParser( dwConnectionIndex, pMsg, dwLength );
		}
		break;
	default:
		TransToMapServerMsgParser( dwConnectionIndex, pMsg, dwLength );
	}
}

void MP_GUILD_UNIONServerMsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength )
{
	MSGBASE* pTempMsg = (MSGBASE*)pMsg;
	switch( pTempMsg->Protocol )
	{
	case MP_GUILD_UNION_CREATE_NOTIFY_TO_MAP:
	case MP_GUILD_UNION_DESTROY_NOTIFY_TO_MAP:
	case MP_GUILD_UNION_INVITE_ACCEPT_NOTIFY_TO_MAP:
	case MP_GUILD_UNION_ADD_NOTIFY_TO_MAP:
	case MP_GUILD_UNION_REMOVE_NOTIFY_TO_MAP:
	case MP_GUILD_UNION_SECEDE_NOTIFY_TO_MAP:
	case MP_GUILD_UNION_MARK_REGIST_NOTIFY_TO_MAP:
		{
			g_Network.Broadcast2MapServerExceptOne(dwConnectionIndex, pMsg, dwLength);
		}
		break;
	default:
		TransToClientMsgParser( dwConnectionIndex, pMsg, dwLength );
	}	
}

void MP_RESIDENTREGIST_UserMsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	g_csResidentRegistManager.ASRV_ParseRequestFromClient(dwConnectionIndex, pMsg, dwLength);
}

void MP_RESIDENTREGIST_ServerMsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	g_csResidentRegistManager.ASRV_ParseRequestFromServer(dwConnectionIndex, pMsg, dwLength);

}
// E �ֹε�� �߰� added by hseos 2007.06.06

// desc_hseos_�йи�01
// S �йи� �߰� added by hseos 2007.07.09	2007.07.11
void MP_FAMILY_UserMsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	g_csFamilyManager.ASRV_ParseRequestFromClient(dwConnectionIndex, pMsg, dwLength);
}

void MP_FAMILY_ServerMsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	g_csFamilyManager.ASRV_ParseRequestFromServer(dwConnectionIndex, pMsg, dwLength);
}
// E �йи� �߰� added by hseos 2007.07.09	2007.07.11

// desc_hseos_����ý���_01
// S ����ý��� �߰� added by hseos 2007.09.10
void MP_FARM_UserMsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	g_csFarmManager.ASRV_ParseRequestFromClient(dwConnectionIndex, pMsg, dwLength);
}

void MP_FARM_ServerMsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	g_csFarmManager.ASRV_ParseRequestFromServer(dwConnectionIndex, pMsg, dwLength);
}
// E ����ý��� �߰� added by hseos 2007.09.10

// desc_hseos_����Ʈ ��_01
// S ����Ʈ �� �߰� added by hseos 2007.11.16
void MP_DATE_UserMsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	g_csDateManager.ASRV_ParseRequestFromClient(dwConnectionIndex, pMsg, dwLength);
}

void MP_DATE_ServerMsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	g_csDateManager.ASRV_ParseRequestFromServer(dwConnectionIndex, pMsg, dwLength);
}

void MP_AUTONOTE_UserMsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	USERINFO* pUserInfo = g_pUserTable->FindUser( dwConnectionIndex );
	if( !pUserInfo ) return;

	MSGBASE* pTempMsg = (MSGBASE*)pMsg;

	if( pUserInfo->dwCharacterID )	pTempMsg->dwObjectID = pUserInfo->dwCharacterID;	//---KES ��� �޼����� ���ؼ� ����Ǿ�� �Ѵ�.. ���� ��ġ��.

	switch( pTempMsg->Protocol )
	{
	case MP_AUTONOTE_ASKTOAUTO_SYN:
		{
			if( pUserInfo->UserLevel > eUSERLEVEL_GM )
			if( CPunishUnit* pPunishUnit = PUNISHMGR->GetPunishUnit( pUserInfo->dwUserID, ePunish_AutoNote ) )
			{
				DWORD dwRemainTime = pPunishUnit->GetRemainTime();
				MSG_DWORD msg;
				msg.Category	= MP_AUTONOTE;					//--- MP_PUNISH �� �ٲ���
				msg.Protocol	= MP_AUTONOTE_PUNISH;
				msg.dwData		= dwRemainTime;
				g_Network.Send2User( pUserInfo->dwConnectionIndex, (char*)&msg, sizeof(msg) );
				return;
			}

			TransToMapServerMsgParser( dwConnectionIndex, pMsg, dwLength );
		}
		break;
	default:
		{
			TransToMapServerMsgParser( dwConnectionIndex, pMsg, dwLength );
		}
		break;
	}
}

void MP_AUTONOTE_ServerMsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGBASE* pTempMsg = (MSGBASE*)pMsg;

	switch( pTempMsg->Protocol )
	{
	case MP_AUTONOTE_ASKTOAUTO_ACK:
		{
			USERINFO* pUserInfo = g_pUserTableForObjectID->FindUser(pTempMsg->dwObjectID);
			if( !pUserInfo ) return;

			//---�Ű��ڰ� 2�� �����Ʈ�� ��� ���ϵ���
			PunishListAdd( pUserInfo->dwUserID, ePunish_AutoNote, 2*60 );
			PUNISHMGR->AddPunishUnit( pUserInfo->dwUserID, ePunish_AutoNote, 2*60 );

			g_Network.Send2User( pUserInfo->dwConnectionIndex, pMsg, dwLength );
		}
		break;

	case MP_AUTONOTE_NOTAUTO:		//---���䰡 ���� ���߾��ٰ� �Ű��ڿ��� �����ش�.
		{
			MSG_DWORD*	pmsg = (MSG_DWORD*)pMsg;

			USERINFO* pUserInfo = g_pUserTableForUserID->FindUser( pmsg->dwData );	//---�ʼ������� ��� ������Ʈ�� ��ε�ĳ���� �Ȱ��̹Ƿ� ���� �ɸ���.
			if( !pUserInfo ) return;

			PUNISHMGR->AddPunishUnit( pUserInfo->dwUserID, ePunish_AutoNote, 60*60 );	//60��	// �Ű��ڰ� �����Ʈ�� �����ð� ��� ���ϵ��� ���縦 ����.

			if( pUserInfo->dwCharacterID )
			{
				MSGBASE msg;
				msg.Category	= MP_AUTONOTE;
				msg.Protocol	= MP_AUTONOTE_NOTAUTO;
				g_Network.Send2User( pUserInfo->dwConnectionIndex, (char*)&msg, sizeof(msg) );
			}
		}
		break;

	case MP_AUTONOTE_ANSWER_ACK:	//---�亯 ���� ���信�� ������ �޼���
		{
			MSG_DWORD*	pmsg = (MSG_DWORD*)pMsg;

			//---��밡 �亯�� ���߾�����, �Ű��ڰ� �����Ʈ�� �����ð� ��� ���ϵ���
			PunishListAdd( pmsg->dwData, ePunish_AutoNote, 60*60 );	//������Ʈ���� ���ϴ� ������ 1�ð� �����Ʈ ������ DB�� �߰�

			MSGBASE msg;
			msg.Category	= MP_AUTONOTE;
			msg.Protocol	= MP_AUTONOTE_ANSWER_ACK;
			msg.dwObjectID	= pmsg->dwObjectID;
			g_Network.Send2User( &msg, sizeof(msg) );
		}
		break;

	case MP_AUTONOTE_ANSWER_FAIL:
		{
			MSG_DWORD*	pmsg = (MSG_DWORD*)pMsg;

			PunishCountAdd( pmsg->dwData, ePunishCount_AutoUse, 1 );		//����������. ������ȸ�� 1ȸ����

			MSGBASE msg;
			msg.Category	= MP_AUTONOTE;
			msg.Protocol	= MP_AUTONOTE_ANSWER_FAIL;
			msg.dwObjectID	= pmsg->dwObjectID;
			g_Network.Send2User( &msg, sizeof(msg) );
		}
		break;

	case MP_AUTONOTE_ANSWER_LOGOUT:
		{
			MSG_DWORD*	pmsg = (MSG_DWORD*)pMsg;

            PunishCountAdd( pmsg->dwData, ePunishCount_AutoUse, 1 );		//����������. ������ȸ�� 1ȸ����
		}
		break;

	case MP_AUTONOTE_ANSWER_TIMEOUT:
		{
			MSG_DWORD*	pmsg = (MSG_DWORD*)pMsg;

            PunishCountAdd( pmsg->dwData, ePunishCount_AutoUse, 1 );		//����������. ������ȸ�� 1ȸ����

			USERINFO* pUserInfo = g_pUserTableForObjectID->FindUser(pTempMsg->dwObjectID);
			if( pUserInfo )
				g_Network.Send2User( pUserInfo->dwConnectionIndex, pMsg, dwLength );
		}
		break;

	case MP_AUTONOTE_KILLAUTO:
		{
			MSG_DWORD*	pmsg = (MSG_DWORD*)pMsg;
			USERINFO* pUserInfo = g_pUserTableForUserID->FindUser( pmsg->dwData );
			if( !pUserInfo ) return;

			MSGBASE msg;
			msg.Category = MP_AUTONOTE;
			msg.Protocol = MP_AUTONOTE_KILLAUTO;

			if( pUserInfo->dwCharacterID )
				g_Network.Send2User( pUserInfo->dwConnectionIndex, (char*)&msg, sizeof(msg) ); 
		}
		break;

	case MP_AUTONOTE_DISCONNECT:
		{
			MSG_DWORD*	pmsg = (MSG_DWORD*)pMsg;
			USERINFO* pUserInfo = g_pUserTableForUserID->FindUser( pmsg->dwData );
			if( !pUserInfo ) return;

			DWORD dwConnectionIdx = pUserInfo->dwConnectionIndex;
			OnDisconnectUser( dwConnectionIdx );
			DisconnectUser( dwConnectionIdx );
		}
		break;

	default:
		{
			USERINFO* pUserInfo = g_pUserTableForObjectID->FindUser(pTempMsg->dwObjectID);
			if( pUserInfo )
				g_Network.Send2User( pUserInfo->dwConnectionIndex, pMsg, dwLength );
		}
		break;
	}
}
//----------------

//---KES PUNISH
void MP_PUNISH_ServerMsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGBASE* pTempMsg = (MSGBASE*)pMsg;

	switch( pTempMsg->Protocol )
	{
	case MP_PUNISH_PUNISH:
		{
//			MSG_DWORD3* pTempMsg = (MSG_DWORD3*)pMsg;
//			PUNISHMGR->AddPunishUnit( pANRoom->GetAskCharacterIdx(), ePunish_AutoNote, 60*60*1000 ); //60��
		}
		break;
	}
}
//-------------

void MP_FISHING_UserMsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	TransToMapServerMsgParser( dwConnectionIndex, pMsg, dwLength );
}

void MP_FISHING_ServerMsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	TransToClientMsgParser( dwConnectionIndex, pMsg, dwLength );
}

void MP_COOKING_UserMsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	TransToMapServerMsgParser( dwConnectionIndex, pMsg, dwLength );
}

void MP_COOKING_ServerMsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	TransToClientMsgParser( dwConnectionIndex, pMsg, dwLength );
}

void MP_HOUSING_UserMsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGROOT* pTempMsg = (MSGROOT*)pMsg;
	switch(pTempMsg->Protocol)
	{
	case MP_HOUSE_ENTRANCE_SYN:							UserConn_HouseEntrance_Syn(dwConnectionIndex, pMsg, dwLength);		break;
	default:											TransToMapServerMsgParser( dwConnectionIndex, pMsg, dwLength );		break;
	}
}

void MP_HOUSING_ServerMsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGROOT* pTempMsg = (MSGROOT*)pMsg;
	switch(pTempMsg->Protocol)
	{
	case MP_HOUSE_ENTRANCE_FROM_SERVER_SYN:				UserConn_HouseEntrance_Syn(dwConnectionIndex, pMsg, dwLength);		break;
	case MP_HOUSE_RANKINFO_FORSERVER:
		{
			g_Network.Broadcast2MapServer( pMsg, dwLength );
		}
		break;

	default:											TransToClientMsgParser( dwConnectionIndex, pMsg, dwLength );		break;
	}
}

void MP_DUNGEON_UserMsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGROOT* pTempMsg = (MSGROOT*)pMsg;
	switch(pTempMsg->Protocol)
	{
	case MP_DUNGEON_ENTRANCE_SYN:	UserConn_DungeonEntrace_Syn(dwConnectionIndex, pMsg, dwLength);		break;
	default:						TransToMapServerMsgParser( dwConnectionIndex, pMsg, dwLength );		break;
	}
}

// 080407 LYW --- AgentNetworkMsgParser : Add a user message parser for chatroom system.
void MP_CHATROOM_UserMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	ASSERT(pMsg) ;

	if(!pMsg)
	{
		MessageBox( NULL, "Invalid a message parameter!!", "MP_CHATROOM_UserMsgParser", MB_OK) ;
		return ;
	}

	CHATROOMMGR->UserMsgParser(dwConnectionIndex, pMsg, dwLength) ;
}

// 080407 LYW --- AgentNetworkMsgParser : Add a server message parser for chatroom system.
void MP_CHATROOM_ServerMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	ASSERT(pMsg) ;

	if(!pMsg)
	{
		MessageBox( NULL, "Invalid a message parameter!!", "MP_CHATROOM_ServerMsgParser", MB_OK) ;
		return ;
	}

	CHATROOMMGR->ServerMsgParser(dwConnectionIndex, pMsg, dwLength) ;
}

// 080728 LYW --- AgnetNetworkMsgParser : ������ ��ȯ�� ���� ���� �޽��� �Ľ��� �ϴ� �Լ� �߰�.
void MP_SIEGERECALL_ServerMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	ASSERT(pMsg) ;

	if(!pMsg)
	{
  		MessageBox( NULL, "Invalid a message parameter!!", "MP_SIEGERECALL_ServerMsgParser", MB_OK) ;
  		return ;
	}

	SIEGERECALLMGR->ServerMsgPaser(dwConnectionIndex, pMsg, dwLength) ;
}

// 080728 LYW --- AgentNetworkMsgParser : ������ ��ȯ�� ���� ���� �޽��� �Ľ��� �ϴ� �Լ� �߰�.
void MP_SIEGERECALL_UserMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	ASSERT(pMsg) ;

	if(!pMsg)
	{
  		MessageBox( NULL, "Invalid a message parameter!!", "MP_SIEGERECALL_UserMsgParser", MB_OK) ;
  		return ;
	}

	SIEGERECALLMGR->UserMsgParser(dwConnectionIndex, pMsg, dwLength) ;
}

// 080819 LYW --- AgentNetworkMsgParser : Ŭ���̾�Ʈ�� ������ npc ��ȯ �޽����� �Ľ��ϴ� �Լ� �߰�.
void MP_RECALLNPC_UserMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	// �Լ� ���� Ȯ��.
	if( !pMsg )
	{
		NPCRECALLMGR->Throw_Error("Invalid parameter!!", __FUNCTION__) ;
  		return ;
	}

	MSGROOT* pTempMsg = (MSGROOT*)pMsg;

	switch( pTempMsg->Protocol ) 
	{
		case MP_RECALLNPC_CTOAM_CHANGEMAP_SYN:
			{
				MSGBASE* pmsg =  (MSGBASE*)pMsg;
				// ���� ������ �޴´�.
				USERINFO* pInfo = NULL ;
				pInfo = g_pUserTable->FindUser(dwConnectionIndex) ;

				if( !pInfo )
				{
					// Ŭ���̾�Ʈ�� ���� �޽��� ����.
					MSG_BYTE msg ;

					msg.Category	= MP_RECALLNPC ;
					msg.Protocol	= MP_RECALLNPC_ATOC_CHANGEMAP_NACK ;

					msg.dwObjectID	= pmsg->dwObjectID ;

					msg.bData		= e_RNET_FAILED_RECEIVE_USERINFO ;

					g_Network.Send2User( (MSG_BYTE*)&msg, sizeof(MSG_BYTE) ) ;

					// ������ ���� �޽��� ��� ó��.
					NPCRECALLMGR->Throw_Error("Failed to receive user info!!", __FUNCTION__) ;
					return ;
				}
			}
			break;

		case MP_RECALLNPC_MTOA_MAP_OUT:
			{
				NPCRECALLMGR->CTOA_MapOut(dwConnectionIndex , pMsg , dwLength) ;
				return;
			}
			break;
	}

	TransToMapServerMsgParser( dwConnectionIndex, pMsg, dwLength );
}

// 080819 LYW --- AgentNetworkMsgParser : npc ��ȯ ���� ���� �޽����� �Ľ��ϴ� �Լ� �߰�.
void MP_RECALLNPC_ServerMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	// �Լ� ���� Ȯ��.
	if( !pMsg )
	{
		NPCRECALLMGR->Throw_Error("Invalid parameter!!", __FUNCTION__) ;
  		return ;
	}

	MSGROOT* pTempMsg = (MSGROOT*)pMsg;

	switch( pTempMsg->Protocol ) 
	{
		case MP_RECALLNPC_MTOA_CHANGEMAP_SYN:
			{
				NPCRECALLMGR->MTOA_ChangeMap_Syn(dwConnectionIndex , pMsg , dwLength) ;
			}
			break;
		default:				
			{
				TransToClientMsgParser( dwConnectionIndex, pMsg, dwLength );		
			}
			break;
	}
}

void MP_SIEGEWARFAREUserMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	TransToMapServerMsgParser( dwConnectionIndex, pMsg, dwLength );
}

void MP_SIEGEWARFAREServerMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGROOT* pTempMsg = (MSGROOT*)pMsg;
	WORD wMessageSendType = 1;

	switch(pTempMsg->Protocol)
	{
	case MP_SIEGEWARFARE_STATE_ACK:
  		{
  			MSG_WORD2* msg = (MSG_WORD2*)pMsg;
  			wMessageSendType = msg->wData1;

  			// Ŭ���̾�Ʈ�� ���� ������ Broadcast �Ѵ�.
  			TransToClientMsgParser( dwConnectionIndex, pMsg, dwLength ) ;
  		}
	case MP_SIEGEWARFARE_WATERSEED_COMPLETE:
  		{
  			if( wMessageSendType )
  			{
  				// �ʼ����� Ŭ���̾�Ʈ ��ο��� �޽����� �����ش�.
  				g_Network.Broadcast2MapServer( pMsg, dwLength );

  				// ������ �ִ� ��� �������� ������
  				USERINFO* pInfo = NULL;
  				g_pUserTable->SetPositionUserHead();

				// 081007 LYW --- AgentNetworkMsgParser : ���� �ҽ� Merge.
				static char TempData[MAX_PACKEDDATA_SIZE];

  				while( (pInfo = (USERINFO *)g_pUserTable->GetUserData()) != NULL )
  				{
					// 081007 LYW --- AgentNetworkMsgParser : ���� �ҽ� Merge.
					memcpy(TempData, pMsg, dwLength);
  					g_Network.Send2User( pInfo->dwConnectionIndex, TempData, dwLength );
  					//g_Network.Send2User( pInfo->dwConnectionIndex, pMsg, dwLength );
  				}
  			}
  			else
  			{
  				TransToClientMsgParser( dwConnectionIndex, pMsg, dwLength );	
  			}
  		}
  		break;
	case MP_SIEGEWARFARE_NOTIFY:
  		{
  			USERINFO* pInfo = NULL;
  			g_pUserTable->SetPositionUserHead();

			// 081007 LYW --- AgentNetworkMsgParser : ���� �ҽ� Merge.
			static char TempData[MAX_PACKEDDATA_SIZE];

  			while( (pInfo = (USERINFO *)g_pUserTable->GetUserData()) != NULL )
  			{
				// 081007 LYW --- AgentNetworkMsgParser : ���� �ҽ� Merge.
				memcpy(TempData, pMsg, dwLength);
  				g_Network.Send2User( pInfo->dwConnectionIndex, TempData, dwLength );
  				//g_Network.Send2User( pInfo->dwConnectionIndex, pMsg, dwLength );
  			}
  		}
  		break;

	// 081017 LYW --- SiegeRecallMgr : ���ͽõ� ���� ���� �߰� - �ٸ� Player�� �Ⱥ��̴� ���� ����.
	case MP_SIEGEWARFARE_USE_WATERSEED_ACK :	
	case MP_SIEGEWARFARE_USE_WATERSEED_CANCEL_ACK :
		{
			// ���� �޽����� ��ȯ�Ѵ�.
			MSG_DWORD2* pmsg = NULL ;
			pmsg = (MSG_DWORD2*)pMsg ;

			if( !pmsg ) return ;


			//���� ������ �˻��Ѵ�.
			USERINFO* pInfo ;
			g_pUserTable->SetPositionUserHead() ;

			while( (pInfo = g_pUserTable->GetUserData()) != NULL )
			{
				// ���� �ʿ� �ִ� �������Ը�, ���ͽõ� ��� �޽����� �����Ѵ�.
				if( pInfo->wUserMapNum != (WORD)pmsg->dwData1 ) continue ;

				MSG_DWORD msg ;

				msg.Category	= MP_SIEGEWARFARE ;
				msg.Protocol	= pmsg->Protocol ;

				msg.dwData		= pmsg->dwData2 ;

				g_Network.Send2User( pInfo->dwConnectionIndex, (char*)&msg, sizeof(MSG_DWORD) ) ;
			}
		}
		break ;

	// 081020 LYW --- AgentNetworkMsgParser : ���� �� A�� B�� �׿��ٴ� �� ���� ó�� �߰�. - �۰���
	case MP_SIEGEWARFARE_NOTICE_KILLANDKILLER_SYN :
		{
			// ���� �޽����� ��ȯ�Ѵ�.
			MSG_DWORD2* pmsg = NULL ;
			pmsg = (MSG_DWORD2*)pMsg ;

			if( !pmsg ) return ;


			// ����� �̻���� �� ���̳�, ų���� ���� ����� �� üũ�� ����.
			// ������� ���� �ޱ�.
			USERINFO* pDeadMan = NULL ;
			g_pUserTable->SetPositionUserHead() ;
			while( (pDeadMan = g_pUserTable->GetUserData()) != NULL )
			{
				if( pDeadMan->dwCharacterID != pmsg->dwData1 ) continue ;
				else break ;
			}

			if( !pDeadMan ) return ;

			// ���λ�� ���� �ޱ�.
			USERINFO* pKiller = NULL ;
			g_pUserTable->SetPositionUserHead() ;
			while( (pKiller = g_pUserTable->GetUserData()) != NULL )
			{
				if( pKiller->dwCharacterID != pmsg->dwData2 ) continue ;
				else break ;
			}

			if( !pKiller ) return ;

			// �θ� �� ���� �ʿ� ���� �ϴ��� üũ�Ѵ�.
			if( pDeadMan->wUserMapNum != pKiller->wUserMapNum )
			{
				char errMsg[256] = {0,} ;
				sprintf(errMsg, "������(%d)�� ������(%d)�� ���� �ٸ���!", pDeadMan->wUserMapNum, pKiller->wUserMapNum ) ;
				g_Console.LOG(4, errMsg ) ;

				return ;
			}


			//���� ������ �˻��Ѵ�.
			USERINFO* pInfo ;
			g_pUserTable->SetPositionUserHead() ;

			WORD wMapNum = pDeadMan->wUserMapNum ;

			while( (pInfo = g_pUserTable->GetUserData()) != NULL )
			{
				// ���� �ʿ� �ִ� �������Ը�, ���ͽõ� ��� �޽����� �����Ѵ�.
				if( pInfo->wUserMapNum != wMapNum ) continue ;

				MSG_DWORD2 msg ;

				msg.Category	= MP_SIEGEWARFARE ;
				msg.Protocol	= MP_SIEGEWARFARE_NOTICE_KILLANDKILLER_ACK ;

				msg.dwData1		= pmsg->dwData1 ;
				msg.dwData2		= pmsg->dwData2 ;

				g_Network.Send2User( pInfo->dwConnectionIndex, (char*)&msg, sizeof(MSG_DWORD2) ) ;
			}
		}
		break ;

	//case MP_SIEGEWARFARE_WATERSEED_COMPLETE :
	//	{
	//		// ���� �޽����� ��ȯ�Ѵ�.
	//		MSG_WATERSEED_COMPLETE* pmsg = NULL ;
	//		pmsg = (MSG_WATERSEED_COMPLETE*)pMsg ;

	//		if( !pmsg ) return ;

	//		USERINFO* pPlayer = NULL ;
	//		pPlayer = g_pUserTable->FindUser( pmsg->dwObjectID ) ;

	//		if( !pPlayer ) return ;


	//		//���� ������ �˻��Ѵ�.
	//		USERINFO* pInfo ;
	//		g_pUserTable->SetPositionUserHead() ;

	//		while( pInfo = g_pUserTable->GetUserData() )
	//		{
	//			// ���� �ʿ� �ִ� �������Ը�, ���ͽõ� ��� �޽����� �����Ѵ�.
	//			if( pInfo->wUserMapNum != pPlayer->wUserMapNum ) continue ;

	//			MSG_WATERSEED_COMPLETE msg ;

	//			msg.Category	= MP_SIEGEWARFARE ;
	//			msg.Protocol	= pmsg->Protocol ;

	//			msg.dwObjectID	= pmsg->dwObjectID ;

	//			msg.dwGuildIdx	= pmsg->dwGuildIdx ;
	//			SafeStrCpy( msg.szGuildName, pmsg->szGuildName, MAX_NAME_LENGTH ) ;

	//			msg.dwMapType	= pmsg->dwMapType ;

	//			g_Network.Send2User( pInfo->dwConnectionIndex, (char*)&msg, sizeof(MSG_WATERSEED_COMPLETE) ) ;
	//		}
	//	}
	//	break ;

	default:
  		TransToClientMsgParser( dwConnectionIndex, pMsg, dwLength );		
  		break;
	}
}

void MP_NPCMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	ASSERT( pMsg );
	if( !pMsg )		return;

	MSGROOT* pTempMsg = (MSGROOT*)pMsg;
	switch(pTempMsg->Protocol)
	{
	// 090227 ShinJS --- �ϻ��� �߰��۾� : �̵�NPC�� �������� �߰�
	case MP_NPC_MOVE_MOVENPC_SYN:
		{
			// �̵�NPC �̵� �޼����� ������� : �̵���/���� ���� �ٸ����
			// �̵��� �ʹ�ȣ�� ������ �ִ� ��� �ش� �ʿ� �̵� �޼����� �����Ѵ�.
			MSG_WORD4* pmsg = (MSG_WORD4*)pMsg;			// (�̵����ʹ�ȣ, �̵��ĸʹ�ȣ, NPC Index, NPC MoveIndex)
			WORD wMapport = g_pServerTable->GetServerPort( eSK_MAP, pmsg->wData2 );
			if( wMapport )
			{
				SERVERINFO* pSInfo	= g_pServerTable->FindServer( wMapport );
				if( !pSInfo )			break;

				MSG_WORD3 msg;
				ZeroMemory( &msg, sizeof(MSG_WORD3) );

				msg.Category	= MP_NPC;
				msg.Protocol	= MP_NPC_MOVE_MOVENPC_SYN;
				msg.wData1		= pmsg->wData1;			// �̵��� �ʹ�ȣ
				msg.wData2		= pmsg->wData3;			// �̵�NPC Index
				msg.wData3		= pmsg->wData4;			// �̵�NPC �̵�Index
				g_Network.Send2Server( pSInfo->dwConnectionIndex, (char*)&msg, sizeof(MSG_WORD3) );
			}
		}
		break;
	
	case MP_NPC_MOVE_MOVENPC_ACK:
		{
			// �̵�NPC �̵��Ϸ� �� �̵��� �ʿ� �̵�NPC ���� �޼��� ����
			MSG_WORD2* pmsg = (MSG_WORD2*)pMsg;			// (�ʹ�ȣ, �̵�NPC Index)
			WORD wMapport = g_pServerTable->GetServerPort( eSK_MAP, pmsg->wData1 );
			if( wMapport )
			{
				SERVERINFO* pSInfo	= g_pServerTable->FindServer( wMapport );
				if( !pSInfo )			break;

				MSG_WORD msg;
				ZeroMemory( &msg, sizeof(MSG_WORD) );

				msg.Category	= MP_NPC;
				msg.Protocol	= MP_NPC_REMOVE_MOVENPC_SYN;
				msg.wData		= pmsg->wData2;
				g_Network.Send2Server( pSInfo->dwConnectionIndex, (char*)&msg, sizeof(MSG_WORD) );
			}
		}
		break;

	default:
		TransToClientMsgParser(dwConnectionIndex,pMsg,dwLength);
		break;
	};
}

#ifdef _HACK_SHIELD_
void MP_HACKSHIELDUserMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	HACKSHIELDMGR->NetworkMsgParse(dwConnectionIndex, pMsg, dwLength);
}
#endif

#ifdef _NPROTECT_
void MP_NPROTECTUserMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	NPROTECTMGR->NetworkMsgParse(dwConnectionIndex, pMsg, dwLength);
}
#endif

// 100302 ONS ���� ĳ���� ������ �������� üũó�� �߰�
BOOL CheckDevilMakeLevel( USERINFO* pUserInfo )
{
	if( !pUserInfo )
		return FALSE;

	for(BYTE count = 0; count < MAX_CHARACTER_NUM; ++count)
	{
		if( 0 == pUserInfo->SelectInfoArray[count].dwCharacterID ) 
			continue;

		// ����50�̻��� ĳ���Ͱ� �����Ұ�� ������������
		if( MAKE_DEVIL_LIMIT_LEVEL <= pUserInfo->SelectInfoArray[count].Level )	
			return TRUE;
	}

	return FALSE;
}