//-------------------------------------------------------------------------------------------------
//	NAME		: CSiege_AddObj.cpp
//	DESC		: Implementation part of CSiege_AddObj class.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 25, 2008
//-------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------------------------
//		Include part.
//-------------------------------------------------------------------------------------------------
#include "stdafx.h"																						// An include file for standard system include files
#include ".\siege_addobj.h"





//-------------------------------------------------------------------------------------------------
//	NAME		: CSiege_AddObj
//	DESC		: ������ �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 25, 2008
//-------------------------------------------------------------------------------------------------
CSiege_AddObj::CSiege_AddObj(void)
{
	// Initialze variables.
	m_byParentStepIdx = 0 ;

	m_byIdx = 0 ;

	//090812 pdy ������ Agent�� Map�������� ��ȯObj �ε��������� �߸����ִ� ���� ���� 
	m_dwMonsterID = 0 ;

	memset(&m_ObjInfo, 0, sizeof(st_SIEGEOBJ)) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: ~CSiege_AddObj
//	DESC		: �Ҹ��� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 25, 2008
//-------------------------------------------------------------------------------------------------
CSiege_AddObj::~CSiege_AddObj(void)
{
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Set_ParentStepIdx
//	DESC		: The function to setting parent step index.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 04, 2008
//-------------------------------------------------------------------------------------------------
void CSiege_AddObj::Set_ParentStepIdx(BYTE byIdx)
{
	// Check index.
	if( byIdx == 0 )
	{
		Throw_Error("Over index limit!!", __FUNCTION__) ;
		return ;
	}


	// Setting parent step index.
	m_byParentStepIdx = byIdx ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Set_Index
//	DESC		: The function to setting index.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 30, 2008
//-------------------------------------------------------------------------------------------------
void CSiege_AddObj::Set_Index(BYTE byIndex)
{
	// Check index limit.
	if( byIndex >= MAX_ADDOBJ_IDX )
	{
		Throw_Error("Over index limit!!", __FUNCTION__) ;
		return ;
	}


	// Setting index.
	m_byIdx = byIndex ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Init_ObjInfo
//	DESC		: ������Ʈ ���� ���� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 25, 2008
//-------------------------------------------------------------------------------------------------
//void CSiege_AddObj::Init_ObjInfo(WORD wThemeIdx, MAPTYPE mapNum, BYTE byStepIdx, DWORD dwObjIdx, 
//	float fXpos, float fZpos, BYTE byUseRandomPos, BYTE byRadius, BYTE byComKind, BYTE byComIndex)
void CSiege_AddObj::Init_ObjInfo(st_SIEGEOBJ* pInfo)
{
	// �Լ� ���� Ȯ��.
	if( !pInfo )
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}

	
	// ������Ʈ ������ �����Ѵ�.
	memcpy(&m_ObjInfo, pInfo, sizeof(st_SIEGEOBJ)) ;


	//// ������Ʈ ������ �����Ѵ�.
	//m_ObjInfo.wThemeIdx			= wThemeIdx ;				// �׸� �ε����� �����Ѵ�.
	//m_ObjInfo.mapNum			= mapNum ;					// ������Ʈ�� ��ȯ �� ��.
	//m_ObjInfo.byStepIdx			= byStepIdx ;				// ���� �ε����� �����Ѵ�.
	//m_ObjInfo.dwObjectIdx		= dwObjIdx ;				// ������Ʈ �ε���.
	//m_ObjInfo.fXpos				= fXpos ;					// ��ȯ ��ǥ.(X)
	//m_ObjInfo.fZpos				= fZpos ;					// ��ȯ ��ǥ.(Z)
	//m_ObjInfo.byUseRandomPos	= byUseRandomPos ;			// ��ȯ ��ġ�� ���� ���� ��ġ�� ������� ����.
	//m_ObjInfo.byRadius			= byRadius ;				// ��ȯ ��ġ�� ���� ���� ��ġ �ݰ�.
	//m_ObjInfo.byComKind			= byComKind ;				// ��ɾ� Ÿ�� �Է�.
	//m_ObjInfo.byComIndex		= byComIndex ;				// ��ɾ� �ε��� �Է�.
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Throw_Error
//	DESC		: The function to process error message.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
void CSiege_AddObj::Throw_Error(char* szErr, char* szCaption)
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
void CSiege_AddObj::WriteLog(char* pMsg)
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















