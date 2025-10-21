//-------------------------------------------------------------------------------------------------
//	NAME		: CNpcRecallMgr.cpp
//	DESC		: Implementation part of CNpcRecallMgr class.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------------------------
//		Include part.
//-------------------------------------------------------------------------------------------------
#include "stdafx.h"																						// An include file for standard system include files

#include "MHFile.h"

#include "./NpcRecallMgr.h"

#include "../[CC]ServerModule/Network.h"

#include "./AgentDBMsgParser.h"

#include "./ServerTable.h"
#include "./AgentDBMsgParser.h"





//-------------------------------------------------------------------------------------------------
//	NAME		: CNpcRecallMgr
//	DESC		: ������ �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
CNpcRecallMgr::CNpcRecallMgr(void)
{
}





//-------------------------------------------------------------------------------------------------
//	NAME		: ~CNpcRecallMgr
//	DESC		: �Ҹ��� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
CNpcRecallMgr::~CNpcRecallMgr(void)
{
}

//-------------------------------------------------------------------------------------------------
//	NAME		: MTOA_MapOut
//	DESC		: npc�� ���� �� �̵� ó���� �ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 25, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::CTOA_MapOut(DWORD dwIndex, char* pMsg, DWORD dwLength)
{
	// �Լ� ���� Ȯ��.
	if( !pMsg )
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	// ���� �޽��� ��ȯ.
	MSG_DWORD* pmsg = NULL ;
	pmsg = (MSG_DWORD*)pMsg ;

	if(!pmsg)
	{
		Throw_Error("Failed to convert a message to original!!", __FUNCTION__) ;
		return ;
	}


	// �� ��Ʈ�� �޴´�.
	WORD mapport = 0 ;

	if( pmsg->dwData < 2000 )
	{
		mapport = g_pServerTable->GetServerPort( eSK_MAP, (WORD)pmsg->dwData ) ;
	}
	else
		mapport = g_pServerTable->GetServerPort( eSK_MAP, (WORD)(pmsg->dwData-2000)) ;


	// �� ��Ʈ ��ȿ�� üũ�Ѵ�.
	if(mapport)
	{
		// Ŭ���̾�Ʈ�� ���̵� ���� �޽����� �����Ѵ�.
		MSG_DWORD msgserver ;
		msgserver.Category = MP_USERCONN ;
		msgserver.Protocol = MP_USERCONN_CHANGEMAP_ACK ;

		if( pmsg->dwData < 2000 ) msgserver.dwData = pmsg->dwData ;
		else msgserver.dwData = pmsg->dwData-2000 ;

		msgserver.dwObjectID = pmsg->dwObjectID ;

		g_Network.Send2User((MSG_DWORD*)&msgserver, sizeof(MSG_DWORD));
	}
}

//-------------------------------------------------------------------------------------------------
//	NAME		: CTOA_ChangeMap_Syn
//	DESC		: Ŭ���̾�Ʈ�� ����, �̵� ������ npc�� ���� �� �̵� ��û�� ������ ��, ó���ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 21, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::MTOA_ChangeMap_Syn(DWORD dwIndex, char* pMsg, DWORD dwLength) 
{
	// �Լ� ���� Ȯ��.
	if( !pMsg )
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	// ���� �޽��� ��ȯ.
	MSG_DWORD3* pmsg = NULL ;
	pmsg = (MSG_DWORD3*)pMsg ;
	//Data1 ��û�ȸ� 
	//Data2 �̵��Ҹ�
	//Data3 npc��ȯ�ε��� 

	if(!pmsg)
	{
		Throw_Error("Failed to convert a message to original!!", __FUNCTION__) ;
		return ;
	}

	// ���� ��Ʈ�� �޴´�.
	WORD wPort = 0 ;
	wPort = g_pServerTable->GetServerPort( eSK_MAP, (WORD)pmsg->dwData2 ) ;


	// ���� ��Ʈ�� ��ȿ�ϸ�,
	if( wPort )
	{
		MSG_DWORD msg;
		msg.Category	= MP_RECALLNPC;
		msg.Protocol	= MP_RECALLNPC_ATOM_CHANGEMAP_ACK;
		msg.dwObjectID	= pmsg->dwObjectID ;
		msg.dwData		= pmsg->dwData3 ;

		g_Network.Send2Server( dwIndex, (char*)&msg, sizeof(MSG_NPCRECALL_CHANGEMAP) ) ;
	}
	// ���� ��Ʈ�� ��ȿ���� ������,
	else
	{
		// Ŭ���̾�Ʈ�� ���� �޽��� ����.
		MSG_BYTE msg ;

		msg.Category	= MP_RECALLNPC ;
		msg.Protocol	= MP_RECALLNPC_ATOC_CHANGEMAP_NACK ;

		msg.dwObjectID	= pmsg->dwObjectID ;

		msg.bData		= e_RNET_FAILED_RECEIVE_SERVERPORT ;

		g_Network.Send2User( (MSG_BYTE*)&msg, sizeof(MSG_BYTE) ) ;

		// ������ ���� �޽��� ��� ó��.
		Throw_Error("Failed to receive server port!!", __FUNCTION__) ;
		return ;
	}
}

//-------------------------------------------------------------------------------------------------
//	NAME		: Throw_Error
//	DESC		: The function to process error message.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::Throw_Error(char* szErr, char* szCaption)
{
	// Check parameter of this function.
	if(!szErr || !szCaption) return ;


	// Check err string size.
	if(strlen(szErr) <= 1)
	{
#ifdef _USE_NPCRECALL_ERRBOX_
		MessageBox( NULL, "Invalid err string size!!", __FUNCTION__, MB_OK ) ;
#else
		char tempStr[257] = {0, } ;

		SafeStrCpy( tempStr, __FUNCTION__, 256 ) ;
		strcat( tempStr, " - " ) ;
		strcat( tempStr, "Invalid err string size!!" ) ;
		WriteLog( tempStr ) ;
#endif // _USE_NPCRECALL_ERRBOX_
	}


	// Check caption string size.
	if(strlen(szCaption) <= 1)
	{
#ifdef _USE_NPCRECALL_ERRBOX_
		MessageBox( NULL, "Invalid caption string size!!", __FUNCTION__, MB_OK ) ;
#else
		char tempStr[257] = {0, } ;

		SafeStrCpy( tempStr, __FUNCTION__, 256 ) ;
		strcat( tempStr, " - " ) ;
		strcat( tempStr, "Invalid caption string size!!" ) ;
		WriteLog( tempStr ) ;
#endif // _USE_NPCRECALL_ERRBOX_
	}


	// Print a err message.
#ifdef _USE_NPCRECALL_ERRBOX_
	MessageBox( NULL, szErr, szCaption, MB_OK) ;
#else
	char tempStr[257] = {0, } ;

	SafeStrCpy(tempStr, szCaption, 256) ;
	strcat(tempStr, " - ") ;
	strcat(tempStr, szErr) ;
	WriteLog(tempStr) ;
#endif // _USE_NPCRECALL_ERRBOX_
}





//-------------------------------------------------------------------------------------------------
//	NAME		: WriteLog
//	DESC		: The function to create a error log for siege recall manager.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::WriteLog(char* pMsg)
{
	SYSTEMTIME time ;
	GetLocalTime(&time) ;

	TCHAR szTime[_MAX_PATH] = {0, } ;
	sprintf(szTime, "%04d-%02d-%02d %02d:%02d:%02d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond) ;

	FILE *fp = fopen("Log/Agent-SiegeRecallMgr.log", "a+") ;
	if (fp)
	{
		fprintf(fp, "%s [%s]\n", pMsg,  szTime) ;
		fclose(fp) ;
	}
}

