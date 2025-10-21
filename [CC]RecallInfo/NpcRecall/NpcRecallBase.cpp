//-------------------------------------------------------------------------------------------------
//	NAME		: CNpcRecallBase.cpp
//	DESC		: Implementation part of CNpcRecallCondition class.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------------------------
//		Include part.
//-------------------------------------------------------------------------------------------------
#include "stdafx.h"																						// An include file for standard system include files

#include "MHFile.h"

#include "./NpcRecallBase.h"





//-------------------------------------------------------------------------------------------------
//	NAME		: CNpcRecallBase
//	DESC		: ������ �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
CNpcRecallBase::CNpcRecallBase(void)
{
	// ��� ���� �ʱ�ȭ.
	m_byRecallCodition = eNR_None ;

	m_dwNpcIdx			= 0 ;
	m_wNpcKind			= 0 ;

	m_RecallMapNum		= 0 ;

	m_fXpos				= 0.0f ;
	m_fZpos				= 0.0f ;

	m_byActive			= FALSE ;

	m_dwCreatedIdx		= 0 ;

	m_byReadyToDelete	= FALSE ;

	m_dwRecallTime		= 0 ;

	m_wRecallFailedMsg	= 0 ;

	m_wDir				= 0 ;

	m_dwNpcRecallIdx	= 0 ;

	m_wRecalledChenel	= 0 ;

	ZeroMemory( this, sizeof(char) * ( MAX_NAME_LENGTH+1 ) );

	m_dwParentNpcIdx	= 0 ;
	
	m_byParentType		= eParentNpcType_UnKnown ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: ~CNpcRecallBase
//	DESC		: �Ҹ��� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
CNpcRecallBase::~CNpcRecallBase(void)
{
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Set_RecallCondition
//	DESC		: ��ȯ ������ �����ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallBase::Set_RecallCondition(BYTE byCondition)
{
	// ���ڷ� �Ѿ�� ���� ��ȿ�� üũ�Ѵ�.
	if( byCondition == eNR_None || byCondition >= eNR_NpcRecall_Max )
	{
		Throw_Error("Invalid recall type!!", __FUNCTION__) ;
		return ;
	}


	// ��ȯ ������ �����Ѵ�.
	m_byRecallCodition = byCondition ;
}


void CNpcRecallBase::Set_NpcRecallIndex(DWORD dwNpcRecallIdx)
{
	if( dwNpcRecallIdx == 0 )
	{
		Throw_Error("Invalid NpcRecall Index!!", __FUNCTION__) ;
		return ;
	}

	m_dwNpcRecallIdx = dwNpcRecallIdx ;
}


//-------------------------------------------------------------------------------------------------
//	NAME		: Set_NpcIndex
//	DESC		: Npc �ε����� �����ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallBase::Set_NpcIndex(DWORD dwNpcIdx) 
{
	// ���ڷ� �Ѿ���� npc �ε��� ������ üũ�Ѵ�.
	if( dwNpcIdx >= 65000 )
	{
		Throw_Error("Invalid Npc index!!", __FUNCTION__) ;
		return ;
	}


	// Npc �ε����� �����Ѵ�.
	m_dwNpcIdx = dwNpcIdx ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Set_NpcKind
//	DESC		: Npc ������ �����ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 21, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallBase::Set_NpcKind(WORD wNpcKind) 
{
	// ���ڷ� �Ѿ���� npc �ε��� ������ üũ�Ѵ�.
	if( wNpcKind >= 65000 || wNpcKind == 0 )
	{
		Throw_Error("Invalid Npc kind!!", __FUNCTION__) ;
		return ;
	}


	// Npc �ε����� �����Ѵ�.
	m_wNpcKind = wNpcKind ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Set_RecallMap
//	DESC		: Npc�� ��ȯ �� ��ȣ�� �����ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallBase::Set_RecallMap(MAPTYPE mapNum) 
{
	// ���ڷ� �Ѿ���� �� ��ȣ�� Ȯ���Ѵ�.
	if( mapNum == 0 )
	{
		Throw_Error("Invalid map number!!", __FUNCTION__) ;
		return ;
	}


	// ��ȯ �� �� ��ȣ�� �����Ѵ�.
	m_RecallMapNum = mapNum ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Set_RecallPosX
//	DESC		: Npc�� ��ȯ X��ǥ�� �����ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallBase::Set_RecallPosX(float fXpos) 
{
	// ���ڷ� �Ҿ���� ��ȯ ��ǥ�� Ȯ���Ѵ�.
	if( fXpos <= 0.0f )
	{
		Throw_Error("Invalid recall position!!", __FUNCTION__) ;
		return ;
	}


	// ��ȯ �� ��ǥ�� �����Ѵ�.
	m_fXpos = fXpos ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Set_RecallPosZ
//	DESC		: Npc�� ��ȯ Z��ǥ�� �����ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallBase::Set_RecallPosZ(float fZpos) 
{
	// ���ڷ� �Ҿ���� ��ȯ ��ǥ�� Ȯ���Ѵ�.
	if( fZpos <= 0.0f )
	{
		Throw_Error("Invalid recall position!!", __FUNCTION__) ;
		return ;
	}


	// ��ȯ �� ��ǥ�� �����Ѵ�.
	m_fZpos = fZpos ;
}

void CNpcRecallBase::Get_RecallPos(VECTOR3* vOutPut)
{
	if( !vOutPut )
		return; 

	vOutPut->x = m_fXpos ;
	vOutPut->z = m_fZpos;
}

void CNpcRecallBase::Set_RecallDir(WORD wDir) 
{
	// ���ڷ� �Ҿ���� ��ȯ ��ǥ�� Ȯ���Ѵ�.
	if( wDir > 360 )
	{
		Throw_Error("Invalid recall Dir!!", __FUNCTION__) ;
		return ;
	}


	// ��ȯ �� ��ǥ�� �����Ѵ�.
	m_wDir = wDir ;

}

void CNpcRecallBase::Set_NpcName(char* pNpcName) 
{
	SafeStrCpy(m_NpcName, pNpcName, MAX_NPC_NAME_LENGTH+1); 
}

void CNpcRecallBase::Set_RecalledChenel(WORD wChenel )
{
	if( wChenel == 0 )
	{
		Throw_Error("Invalid RecalledChenel Index!!", __FUNCTION__) ;
		return ;
	}

	m_wRecalledChenel = wChenel ;
}

void CNpcRecallBase::Set_ParentNpcIndex(DWORD dwIndex) 
{
	// ���ڷ� �Ҿ���� ��ȯ ��ǥ�� Ȯ���Ѵ�.
	if( dwIndex == 0 )
	{
		Throw_Error("Invalid ParentNpc Index!!", __FUNCTION__) ;
		return ;
	}

	m_dwParentNpcIdx = dwIndex ;
}

void CNpcRecallBase::Set_ParentNpcType(BYTE byType) 
{
	// ���ڷ� �Ҿ���� ��ȯ ��ǥ�� Ȯ���Ѵ�.
	if( byType == eParentNpcType_UnKnown || byType >= eParentNpcType_Max )
	{
		Throw_Error("Invalid ParentNpc Type!!", __FUNCTION__) ;
		return ;
	}

	m_byParentType = byType ;
}


//-------------------------------------------------------------------------------------------------
//	NAME		: Throw_Error
//	DESC		: The function to process error message.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallBase::Throw_Error(char* szErr, char* szCaption)
{
#ifdef _CLIENT_

	#ifdef _GMTOOL_
		MessageBox( NULL, szErr, szCaption, MB_OK) ;
	#endif //_GMTOOL_

#else

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

#endif //_CLIENT_
}





//-------------------------------------------------------------------------------------------------
//	NAME		: WriteLog
//	DESC		: The function to create a error log for siege recall manager.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallBase::WriteLog(char* pMsg)
{
	SYSTEMTIME time ;
	GetLocalTime(&time) ;

	TCHAR szTime[_MAX_PATH] = {0, } ;
	sprintf(szTime, "%04d-%02d-%02d %02d:%02d:%02d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond) ;

	FILE *fp = NULL ;
#ifdef _AGENTSERVER
	fp = fopen("Log/AgentServer-SiegeRecallMgr.log", "a+") ;
#elif _MAPSERVER_
	fp = fopen("Log/MapServer-SiegeRecallMgr.log", "a+") ;
#endif // _AGENTSERVER_

	if (fp)
	{
		fprintf(fp, "%s [%s]\n", pMsg,  szTime) ; 
		fclose(fp) ;
	}
}




























