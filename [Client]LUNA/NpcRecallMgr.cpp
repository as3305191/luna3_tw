#include "stdafx.h"
#include "MHFile.h"
#include "NpcRecallMgr.h"
#include "ChatManager.h"
#include "WindowIDEnum.h"
#include "cMsgBox.h"
#include "interface/cWindowManager.h"
#include "../[CC]Skill/Client/Info/BuffSkillInfo.h"
#include "GameIn.h"
#include "ObjectManager.h"
#include "DealDialog.h"
#include "NpcScriptDialog.h"
#include "QuestManager.h"

CNpcRecallMgr::CNpcRecallMgr(void)
{
	m_CurSelectedNpcID = 0 ;

	// 081023 LYW --- NpcRecallMgr : �޸� ������ ����, new delete�� ���� ���ϴ� ������ �����Ѵ�.
	//// 080826 KTH -- Initalize NPC Buff HashMap
	//m_NPCBuff.Initialize(20);
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
//	NAME		: Initialize
//	DESC		: �ʱ�ȭ �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 19, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::Initialize() 
{
	// NPC ��ȯ ������ ������ �ִ� ��ũ��Ʈ ������ �ε��Ѵ�.
	Load_RecallNpc() ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Load_RecallNpc
//	DESC		: ��ȯ�� npc ������ �ε��ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 19, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::Load_RecallNpc()
{
	// ���� Ŭ���� ����.
	CMHFile file ;

	if( !file.Init("system/resource/NpcRecallInfo.bin", "rb") )
	{
		PrintError( "Failed to initialize SiegeRecall.bin", __FUNCTION__ ) ;
		return ;
	}

	char szBuff[257] = {0 };
	BYTE byCommandKind = e_RNC_MAX;

	while( TRUE )
	{
		// ������ ������ üũ�Ѵ�.
		if( file.IsEOF() ) break ;

		// ���ɾ �޴´�.
		memset( szBuff, 0, strlen(szBuff) ) ;
		file.GetString(szBuff) ;

		// �ּ� ���ɾ� ���, continue ó���� �Ѵ�..
		if(strcmp( szBuff, "//" ) == 0)
		{
			memset( szBuff, 0, strlen(szBuff) ) ;
			file.GetLine(szBuff, sizeof(256)) ;
			continue ;
		}
		else if( (strcmp( szBuff, "{" ) == 0) || (strcmp( szBuff, "}" ) == 0 ) ) continue ;

		// ��ũ��Ʈ ���ɾ� Ÿ���� �޴´�.
		byCommandKind = Get_CommandKind( szBuff ) ;

		// ���ɾ� Ÿ�Կ� ���� ó���� �Ѵ�.
		Command_Process(byCommandKind, &file) ;
	}


	// ���� Ŭ������ �����Ѵ�.
	file.Release() ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Get_CommandKind
//	DESC		: ��ũ��Ʈ ���ɾ� Ÿ���� ��ȯ�ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 19, 2008
//-------------------------------------------------------------------------------------------------
BYTE CNpcRecallMgr::Get_CommandKind(char* pString)
{
	BYTE byCommandKind = e_RNC_MAX ;

	// �Լ� ���� Ȯ��.
	if(!pString)
	{
		PrintError("Invalid a parameter!!", __FUNCTION__) ;
		return byCommandKind ;
	}


	// ��ũ��Ʈ ���ɾ��� ��Ʈ�� ������ üũ.
	if( strlen(pString) <= 1 )
	{
		PrintError("Invalid string size!!", __FUNCTION__) ;
		return byCommandKind ;
	}


	// ��ũ��Ʈ ���ɾ� Ÿ���� �����Ѵ�.
	if( strcmp( pString, "$SELF_RECALL_NPC" ) == 0 )
	{
		byCommandKind = e_RNC_SELF_RECALL_NPC ;
	}

	if( strcmp( pString, "$REMAIN_TIME_NPC" ) == 0 )
	{
		byCommandKind = e_RNC_REMAIN_TIME_NPC ;
	}

	if( strcmp( pString, "$MOVEABLE_NPC" ) == 0 )
	{
		byCommandKind = e_RNC_MOVEABLE_NPC ;
	}

	if( strcmp( pString, "#NPCRECALLID" ) == 0 )
	{
		byCommandKind = e_RNC_NPCRECALLID ;
	}

	if( strcmp( pString, "#NPCID" ) == 0 )
	{
		byCommandKind = e_RNC_NPCID ;
	}

	if( strcmp( pString, "#NPCKIND" ) == 0 )
	{
		byCommandKind = e_RNC_NPCKIND ;
	}

	if( strcmp( pString, "#RECALLMAP" ) == 0 )
	{
		byCommandKind = e_RNC_RECALLMAP ;
	}

	if( strcmp( pString, "#XPOS" ) == 0 )
	{
		byCommandKind = e_RNC_XPOS ;
	}

	if( strcmp( pString, "#ZPOS" ) == 0 )
	{
		byCommandKind = e_RNC_ZPOS ;
	}

	if( strcmp( pString, "#DIR" ) == 0 )
	{
		byCommandKind = e_RNC_DIR ;
	}

	if( strcmp( pString, "#NAME" ) == 0 )
	{
		byCommandKind = e_RNC_NAME ;
	}


	if( strcmp( pString, "#TIMETABLE" ) == 0 )
	{
		byCommandKind = e_RNC_TIME_TABLE ;
	}

	if( strcmp( pString, "#REMAINTIME" ) == 0 )
	{
		byCommandKind = e_RNC_REMAIN_TIME ;
	}

	if( strcmp( pString, "#MOVETIME" ) == 0 )
	{
		byCommandKind = e_RNC_MOVE_TIME ;
	}

	if( strcmp( pString, "#MOVEXPOS" ) == 0 )
	{
		byCommandKind = e_RNC_MOVE_XPOS ;
	}

	if( strcmp( pString, "#MOVEZPOS" ) == 0 )
	{
		byCommandKind = e_RNC_MOVE_ZPOS ;
	}

	if( strcmp( pString, "#CHANGEMAPNUM" ) == 0 )
	{
		byCommandKind = e_RNC_CHANGE_MAPNUM ;
	}

	if( strcmp( pString, "#CHANGEMAPXPOS" ) == 0 )
	{
		byCommandKind = e_RNC_CHANGE_XPOS ;
	}

	if( strcmp( pString, "#CHANGEMAPZPOS" ) == 0 )
	{
		byCommandKind = e_RNC_CHANGE_ZPOS ;
	}

	if( strcmp( pString, "#RECALL_FAILED_MSG" ) == 0 )
	{
		byCommandKind = e_RNC_RECALL_FAILED_MSG ;
	}

	if( strcmp( pString, "#PARENT_NPCID" ) == 0 )
	{
		byCommandKind = e_RNC_PARENT_NPCID ; 
	}

	if( strcmp( pString, "#PARENT_NPCTYPE" ) == 0 )
	{
		byCommandKind = e_RNC_PARENT_TYPE ;
	}


	// ��ũ��Ʈ ���ɾ� Ÿ���� ��ȯ�Ѵ�.
	return byCommandKind ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Command_Process
//	DESC		: ��ũ��Ʈ ���ɾ� Ÿ�Կ� ���� ó���� �ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 19, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::Command_Process(BYTE byCommandKind, CMHFile* pFile)
{
	// �Լ� ���� Ȯ��.
	if(!pFile)
	{
		PrintError("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}

	switch(byCommandKind)
	{
	case e_RNC_SELF_RECALL_NPC :	Load_SelfRecallNpc() ;				break ;
	case e_RNC_REMAIN_TIME_NPC :	Load_RemainTimeNpc() ;				break ;
	case e_RNC_MOVEABLE_NPC :		Load_MoveableNpc() ;				break ;
	case e_RNC_NPCRECALLID :		Load_SetNpcRecallId( pFile ) ;		break ;
	case e_RNC_NPCID :				Load_SetNpcId( pFile ) ;			break ;
	case e_RNC_NPCKIND :			Load_SetNpcKind( pFile ) ;			break ;
	case e_RNC_RECALLMAP :			Load_SetRecallMap( pFile ) ;		break ;
	case e_RNC_XPOS :				Load_SetXpos( pFile ) ;				break ;
	case e_RNC_ZPOS :				Load_SetZpos( pFile ) ;				break ;
	case e_RNC_DIR:					Load_SetDir( pFile ) ;				break ;
	case e_RNC_NAME:				Load_SetName( pFile ) ;				break ;
	case e_RNC_TIME_TABLE :			Load_TimeTable( pFile ) ;			break ;
	case e_RNC_REMAIN_TIME :		Load_SetRemainTime( pFile ) ;		break ;
	case e_RNC_MOVE_TIME :			Load_SetMoveTime( pFile ) ;			break ;
	case e_RNC_MOVE_XPOS :			Load_SetMoveXpos( pFile ) ;			break ;
	case e_RNC_MOVE_ZPOS :			Load_SetMoveZpos( pFile ) ;			break ;
	case e_RNC_CHANGE_MAPNUM :		Load_SetChangeMapNum( pFile ) ;		break ;
	case e_RNC_CHANGE_XPOS :		Load_SetChangeMapXPos( pFile ) ;	break ;
	case e_RNC_CHANGE_ZPOS :		Load_SetChangeMapZPos( pFile ) ;	break ;
	case e_RNC_RECALL_FAILED_MSG :	Load_SetRecallFailedMsg( pFile ) ;	break ;
	case e_RNC_PARENT_NPCID:		Load_SetParentNpcID( pFile ) ; 		break ;	
	case e_RNC_PARENT_TYPE:			Load_SetParentNpcType( pFile ) ; 	break ;	

	case e_RNC_MAX :
	default : break ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Load_SelfRecallNpc
//	DESC		: ������ ��ȯ/�Ҹ�Ǵ� npc ������ �ε��ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 19, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::Load_SelfRecallNpc() 
{
	// ������ ��ȯ/�Ҹ��� �� �ִ� npc�� �߰��Ѵ�.
	CNpcRecallSelf newNpc ;

	newNpc.Set_NpcIndex(Get_CurTotalNpcCount()) ;
	newNpc.Set_RecallCondition(eNR_ItSelf) ;

	m_mRSelf_Npc.insert( std::make_pair(Get_CurTotalNpcCount(), newNpc) ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Load_RemainTimeNpc
//	DESC		: ���� �ð� ��ȯ/�Ҹ�Ǵ� npc ������ �ε��ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 19, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::Load_RemainTimeNpc()
{
	// ���� �ð� ��ȯ/�Ҹ�Ǵ� npc�� �߰��Ѵ�.
	CNpcRecallRemainTime newNpc ;

	newNpc.Set_NpcIndex(Get_CurTotalNpcCount()) ;
	newNpc.Set_RecallCondition(eNR_RemainTime) ;

	m_mRTime_Npc.insert( std::make_pair(Get_CurTotalNpcCount(), newNpc) ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Load_MoveableNpc
//	DESC		: �̵� ������ npc ������ �ε��ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 19, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::Load_MoveableNpc() 
{
	// �̵� ������ npc�� �߰��Ѵ�.
	CNpcRecallMoveable newNpc ;

	newNpc.Set_NpcIndex(Get_CurTotalNpcCount()) ;
	newNpc.Set_RecallCondition(eNR_Moveable) ;

	m_mRMove_Npc.insert( std::make_pair(Get_CurTotalNpcCount(), newNpc) ) ;
}





void CNpcRecallMgr::Load_SetNpcRecallId(CMHFile* pFile) 
{
	// �Լ� ���� Ȯ��.
	if(!pFile)
	{
		PrintError("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}

	// ���� �ε����� npc �⺻ ������ �޴´�.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		PrintError("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}

	// Npc�ε����� �����Ѵ�.
	pBase->Set_NpcRecallIndex( pFile->GetDword() ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Load_SetNpcId
//	DESC		: ���� �ε����� npc�� �ε����� �����ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 19, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::Load_SetNpcId(CMHFile* pFile)
{
	// �Լ� ���� Ȯ��.
	if(!pFile)
	{
		PrintError("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// ���� �ε����� npc �⺻ ������ �޴´�.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		PrintError("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}

	pBase->Set_NpcIndex( pFile->GetWord() ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Load_SetNpcKind
//	DESC		: ���� �ε����� npc�� ������ �����ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 21, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::Load_SetNpcKind(CMHFile* pFile)
{
	// �Լ� ���� Ȯ��.
	if(!pFile)
	{
		PrintError("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// ���� �ε����� npc �⺻ ������ �޴´�.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		PrintError("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}

	
	// Npc�ε����� �����Ѵ�.
	pBase->Set_NpcKind( pFile->GetWord() ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Load_SetRecallMap
//	DESC		: ��ȯ �� �� ��ȣ�� �����ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 19, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::Load_SetRecallMap(CMHFile* pFile) 
{
	// �Լ� ���� Ȯ��.
	if(!pFile)
	{
		PrintError("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// ���� �ε����� npc �⺻ ������ �޴´�.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		PrintError("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}

	
	// ��ȯ �� �� �ε����� �����Ѵ�.
	pBase->Set_RecallMap( pFile->GetWord() ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Load_SetXpos
//	DESC		: ��ȯ �� X��ǥ�� �����ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 19, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::Load_SetXpos(CMHFile* pFile)
{
	// �Լ� ���� Ȯ��.
	if(!pFile)
	{
		PrintError("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// ���� �ε����� npc �⺻ ������ �޴´�.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		PrintError("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}

	
	// ��ȯ�� X��ǥ�� �����Ѵ�.
	pBase->Set_RecallPosX( pFile->GetFloat() ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Load_SetZpos
//	DESC		: ��ȯ �� Z��ǥ�� �����ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 19, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::Load_SetZpos(CMHFile* pFile) 
{
	// �Լ� ���� Ȯ��.
	if(!pFile)
	{
		PrintError("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// ���� �ε����� npc �⺻ ������ �޴´�.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		PrintError("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}

	
	// ��ȯ�� Z��ǥ�� �����Ѵ�.
	pBase->Set_RecallPosZ( pFile->GetFloat() ) ;
}


void CNpcRecallMgr::Load_SetDir(CMHFile* pFile) 
{
	// �Լ� ���� Ȯ��.
	if(!pFile)
	{
		PrintError("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// ���� �ε����� npc �⺻ ������ �޴´�.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		PrintError("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}

	
	// ��ȯ�� Z��ǥ�� �����Ѵ�.
	pBase->Set_RecallDir( pFile->GetWord() ) ;
}

void CNpcRecallMgr::Load_SetName(CMHFile* pFile ) 
{
	// �Լ� ���� Ȯ��.
	if(!pFile)
	{
		PrintError("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}

	// ���� �ε����� npc �⺻ ������ �޴´�.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		PrintError("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}

	pBase->Set_NpcName( pFile->GetString() ) ;
}


//-------------------------------------------------------------------------------------------------
//	NAME		: Load_TimeTable
//	DESC		: ������ ��ȯ�� �ð� ���̺��� �ε��ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 19, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::Load_TimeTable(CMHFile* pFile)
{
	// �Լ� ���� Ȯ��.
	if(!pFile)
	{
		PrintError("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// ���� �ε����� npc �⺻ ������ �޴´�.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		PrintError("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}


	// Npc Ÿ���� Ȯ���Ѵ�.
	if( pBase->Get_RecallCondition() != eNR_ItSelf )
	{
		PrintError("Invalid Npc type!!", __FUNCTION__) ;
		return ;
	}


	// ������ ��ȯ/�Ҹ��ϴ� ���� npc ������ �޴´�.
	MAP_RSELF_NPC::iterator it ;
	for( it = m_mRSelf_Npc.begin() ; it != m_mRSelf_Npc.end() ; ++it )
	{
		if( it->second.Get_NpcRecallIndex() != pBase->Get_NpcRecallIndex() ) continue ;

		else break ;
	}


	// Npc ������ Ȯ���Ѵ�.
	if( it == m_mRSelf_Npc.end() )
	{
		PrintError("Can't find npc info!!", __FUNCTION__) ;
		return ;
	}


	// Ÿ�� ���̺� ������ �޴´�.
	st_RECALL_TIMETABLE stTable ;
	memset(&stTable, 0, sizeof(st_RECALL_TIMETABLE)) ;

	stTable.byTableIdx		= it->second.Get_TableCount() ;

	stTable.wStartDay		= pFile->GetWord() ;			// ���� ��¥.
	stTable.wStartHour		= pFile->GetWord() ;			// ���� �ð�.
	stTable.wStartMinute	= pFile->GetWord() ;			// ���� ��.

	stTable.wEndDay			= pFile->GetWord() ;			// ���� ��¥.
	stTable.wEndHour		= pFile->GetWord() ;			// ���� �ð�.
	stTable.wEndMinute		= pFile->GetWord() ;			// ���� ��.


	// Npc ������ �ð� ���̺��� �߰��Ѵ�.
	it->second.Insert_TimeTable(&stTable) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Load_SetRemainTime
//	DESC		: ��ȯ ���� �ð��� �ε��ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 19, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::Load_SetRemainTime(CMHFile* pFile) 
{
	// �Լ� ���� Ȯ��.
	if(!pFile)
	{
		PrintError("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// ���� �ε����� npc �⺻ ������ �޴´�.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		PrintError("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}


	// Npc Ÿ���� Ȯ���Ѵ�.
	BYTE byRecallCondition = pBase->Get_RecallCondition() ;
	if( byRecallCondition < eNR_RemainTime || byRecallCondition > eNR_Moveable )
	{
		PrintError("Invalid Npc type!!", __FUNCTION__) ;
		return ;
	}


	// 
	if( byRecallCondition == eNR_RemainTime )
	{
		MAP_RTIME_NPC::iterator it ;
		for( it = m_mRTime_Npc.begin() ; it != m_mRTime_Npc.end() ; ++it )
		{
			if( it->second.Get_NpcRecallIndex() != pBase->Get_NpcRecallIndex() ) continue ;

			else break ;
		}


		// Npc ������ Ȯ���Ѵ�.
		if( it == m_mRTime_Npc.end() )
		{
			PrintError("Can't find npc info!!", __FUNCTION__) ;
			return ;
		}


		// ��ȯ ���� �ð��� �����Ѵ�.
		it->second.Set_RemainTime(pFile->GetWord()) ;
	}
	else if( byRecallCondition == eNR_Moveable )
	{
		MAP_RMOVE_NPC::iterator it ;
		for( it = m_mRMove_Npc.begin() ; it != m_mRMove_Npc.end() ; ++it )
		{
			if( it->second.Get_NpcRecallIndex() != pBase->Get_NpcRecallIndex() ) continue ;

			else break ;
		}


		// Npc ������ Ȯ���Ѵ�.
		if( it == m_mRMove_Npc.end() )
		{
			PrintError("Can't find npc info!!", __FUNCTION__) ;
			return ;
		}


		// ��ȯ ���� �ð��� �����Ѵ�.
		it->second.Set_RemainTime(pFile->GetDword()) ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Load_SetMoveTime
//	DESC		: �̵��� ���� �� �ð��� �ε��ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 19, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::Load_SetMoveTime(CMHFile* pFile)
{
	// �Լ� ���� Ȯ��.
	if(!pFile)
	{
		PrintError("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// ���� �ε����� npc �⺻ ������ �޴´�.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		PrintError("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}


	// Npc Ÿ���� Ȯ���Ѵ�.
	if( pBase->Get_RecallCondition() != eNR_Moveable )
	{
		PrintError("Invalid Npc type!!", __FUNCTION__) ;
		return ;
	}


	// ������ ��ȯ/�Ҹ��ϴ� ���� npc ������ �޴´�.
	MAP_RMOVE_NPC::iterator it ;
	for( it = m_mRMove_Npc.begin() ; it != m_mRMove_Npc.end() ; ++it )
	{
		if( it->second.Get_NpcRecallIndex() != pBase->Get_NpcRecallIndex() ) continue ;

		else break ;
	}


	// Npc ������ Ȯ���Ѵ�.
	if( it == m_mRMove_Npc.end() )
	{
		PrintError("Can't find npc info!!", __FUNCTION__) ;
		return ;
	}


	// ��ȯ ���� �ð��� �����Ѵ�.
	it->second.Set_MoveTime(pFile->GetDword()) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Load_SetMoveXpos
//	DESC		: �̵� �� X��ǥ�� �ε��ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 19, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::Load_SetMoveXpos(CMHFile* pFile) 
{
	// �Լ� ���� Ȯ��.
	if(!pFile)
	{
		PrintError("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// ���� �ε����� npc �⺻ ������ �޴´�.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		PrintError("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}


	// Npc Ÿ���� Ȯ���Ѵ�.
	if( pBase->Get_RecallCondition() != eNR_Moveable )
	{
		PrintError("Invalid Npc type!!", __FUNCTION__) ;
		return ;
	}


	// ������ ��ȯ/�Ҹ��ϴ� ���� npc ������ �޴´�.
	MAP_RMOVE_NPC::iterator it ;
	for( it = m_mRMove_Npc.begin() ; it != m_mRMove_Npc.end() ; ++it )
	{
		if( it->second.Get_NpcRecallIndex() != pBase->Get_NpcRecallIndex() ) continue ;

		else break ;
	}


	// Npc ������ Ȯ���Ѵ�.
	if( it == m_mRMove_Npc.end() )
	{
		PrintError("Can't find npc info!!", __FUNCTION__) ;
		return ;
	}


	// ��ǥ�� �����Ѵ�.
	it->second.Set_MoveposX(pFile->GetFloat()) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Load_SetMoveZpos
//	DESC		: �̵� �� Z��ǥ�� �ε��ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 19, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::Load_SetMoveZpos(CMHFile* pFile) 
{
	// �Լ� ���� Ȯ��.
	if(!pFile)
	{
		PrintError("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// ���� �ε����� npc �⺻ ������ �޴´�.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		PrintError("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}


	// Npc Ÿ���� Ȯ���Ѵ�.
	if( pBase->Get_RecallCondition() != eNR_Moveable )
	{
		PrintError("Invalid Npc type!!", __FUNCTION__) ;
		return ;
	}


	// ������ ��ȯ/�Ҹ��ϴ� ���� npc ������ �޴´�.
	MAP_RMOVE_NPC::iterator it ;
	for( it = m_mRMove_Npc.begin() ; it != m_mRMove_Npc.end() ; ++it )
	{
		if( it->second.Get_NpcRecallIndex() != pBase->Get_NpcRecallIndex() ) continue ;

		else break ;
	}


	// Npc ������ Ȯ���Ѵ�.
	if( it == m_mRMove_Npc.end() )
	{
		PrintError("Can't find npc info!!", __FUNCTION__) ;
		return ;
	}


	// ��ǥ�� �����Ѵ�.
	it->second.Set_MoveposX(pFile->GetFloat()) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Load_SetChangeMapNum
//	DESC		: ��Ż�� ���� �̵��� �� ��ȣ�� �ε��ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 21, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::Load_SetChangeMapNum(CMHFile* pFile) 
{
	// �Լ� ���� Ȯ��.
	if(!pFile)
	{
		PrintError("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// ���� �ε����� npc �⺻ ������ �޴´�.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		PrintError("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}


	// Npc Ÿ���� Ȯ���Ѵ�.
	if( pBase->Get_RecallCondition() != eNR_Moveable )
	{
		PrintError("Invalid Npc type!!", __FUNCTION__) ;
		return ;
	}


	// ������ ��ȯ/�Ҹ��ϴ� ���� npc ������ �޴´�.
	MAP_RMOVE_NPC::iterator it ;
	for( it = m_mRMove_Npc.begin() ; it != m_mRMove_Npc.end() ; ++it )
	{
		if( it->second.Get_NpcRecallIndex() != pBase->Get_NpcRecallIndex() ) continue ;

		else break ;
	}


	// Npc ������ Ȯ���Ѵ�.
	if( it == m_mRMove_Npc.end() )
	{
		PrintError("Can't find npc info!!", __FUNCTION__) ;
		return ;
	}


	// ��ǥ�� �����Ѵ�.
	it->second.Set_ChangeMapNum(pFile->GetWord()) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Load_SetChangeMapXPos
//	DESC		: ��Ż�� ���� �̵��� ���� X��ǥ�� �ε��ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 21, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::Load_SetChangeMapXPos(CMHFile* pFile) 
{
	// �Լ� ���� Ȯ��.
	if(!pFile)
	{
		PrintError("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// ���� �ε����� npc �⺻ ������ �޴´�.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		PrintError("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}


	// Npc Ÿ���� Ȯ���Ѵ�.
	if( pBase->Get_RecallCondition() != eNR_Moveable )
	{
		PrintError("Invalid Npc type!!", __FUNCTION__) ;
		return ;
	}


	// ������ ��ȯ/�Ҹ��ϴ� ���� npc ������ �޴´�.
	MAP_RMOVE_NPC::iterator it ;
	for( it = m_mRMove_Npc.begin() ; it != m_mRMove_Npc.end() ; ++it )
	{
		if( it->second.Get_NpcRecallIndex() != pBase->Get_NpcRecallIndex() ) continue ;

		else break ;
	}


	// Npc ������ Ȯ���Ѵ�.
	if( it == m_mRMove_Npc.end() )
	{
		PrintError("Can't find npc info!!", __FUNCTION__) ;
		return ;
	}


	// ��ǥ�� �����Ѵ�.
	it->second.Set_ChangeMapXPos(pFile->GetFloat()) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Load_SetChangeMapZPos
//	DESC		: ��Ż�� ���� �̵��� ���� Z��ǥ�� �ε��ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 21, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::Load_SetChangeMapZPos(CMHFile* pFile) 
{
	// �Լ� ���� Ȯ��.
	if(!pFile)
	{
		PrintError("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// ���� �ε����� npc �⺻ ������ �޴´�.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		PrintError("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}


	// Npc Ÿ���� Ȯ���Ѵ�.
	if( pBase->Get_RecallCondition() != eNR_Moveable )
	{
		PrintError("Invalid Npc type!!", __FUNCTION__) ;
		return ;
	}


	// ������ ��ȯ/�Ҹ��ϴ� ���� npc ������ �޴´�.
	MAP_RMOVE_NPC::iterator it ;
	for( it = m_mRMove_Npc.begin() ; it != m_mRMove_Npc.end() ; ++it )
	{
		if( it->second.Get_NpcRecallIndex() != pBase->Get_NpcRecallIndex() ) continue ;
		else break ;
	}


	// Npc ������ Ȯ���Ѵ�.
	if( it == m_mRMove_Npc.end() )
	{
		PrintError("Can't find npc info!!", __FUNCTION__) ;
		return ;
	}


	// ��ǥ�� �����Ѵ�.
	it->second.Set_ChangeMapZPos(pFile->GetFloat()) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Load_SetRecallFailedMsg
//	DESC		: �̹� ��ȯ ��, npc ��ȯ ���� ���� �޽��� ��ȣ�� �ε��ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 27, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::Load_SetRecallFailedMsg(CMHFile* pFile)
{
	// �Լ� ���� Ȯ��.
	if(!pFile)
	{
		PrintError("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// ���� �ε����� npc �⺻ ������ �޴´�.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		PrintError("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}

	
	// ��ȯ ���н� ����� ���� �޽����� �����Ѵ�.
	pBase->Set_RecallFailedMsg( pFile->GetWord() ) ;
}

void CNpcRecallMgr::Load_SetParentNpcID(CMHFile* pFile) 
{
	// �Լ� ���� Ȯ��.
	if(!pFile)
	{
		PrintError("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}

	// ���� �ε����� npc �⺻ ������ �޴´�.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		PrintError("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}

	
	pBase->Set_ParentNpcIndex( pFile->GetDword() ) ;
}

void CNpcRecallMgr::Load_SetParentNpcType(CMHFile* pFile) 
{
	// �Լ� ���� Ȯ��.
	if(!pFile)
	{
		PrintError("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}

	// ���� �ε����� npc �⺻ ������ �޴´�.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		PrintError("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}

	
	pBase->Set_ParentNpcType( pFile->GetByte() ) ;

}




//-------------------------------------------------------------------------------------------------
//	NAME		: NetworkMsgParser
//	DESC		: ������ ������ ��Ʈ��ũ �޽����� �Ľ��ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 28, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::NetworkMsgParser( BYTE Protocol, void* pMsg ) 
{
	// �Լ� ���� Ȯ��.
	if( !pMsg )
	{
		PrintError("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	// �������� Ȯ��.
	switch( Protocol )
	{
	case MP_RECALLNPC_MTOC_RECALL_NACK :			RecallNpc_Nack( pMsg ) ;		break;

	case MP_RECALLNPC_MTOC_ALREADY_RECALL_NACK :	AlreadyRecallNpc_Nack( pMsg ) ;	break;

	case MP_RECALLNPC_ATOC_CHANGEMAP_NACK :
	case MP_RECALLNPC_MTOC_CHANGEMAP_NACK :			ChangeMap_Nack( pMsg ) ;		break;

	default : break ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: RecallNpc_Nack
//	DESC		: NPC ��ȯ ���� ó���� �ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 28, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::RecallNpc_Nack(void* pMsg) 
{
	// �Լ� ���� Ȯ��.
	if( !pMsg ) 
	{
		PrintError("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	// ���� �޽��� ��ȯ.
	MSG_BYTE* pmsg = NULL ;
	pmsg = (MSG_BYTE*)pMsg ;

	if( !pmsg )
	{
		PrintError("Failed to conver a message!!", __FUNCTION__) ;
		return ;
	}
    

	// ���� �޽����� Ȯ���Ѵ�.
	switch( pmsg->bData )
	{
	case e_RNET_INVALID_NPC_TYPE : break ;

	case e_RNET_FAILED_RECEIVE_REMAINNPC : break ;

	case e_RNET_FAILED_ALREADY_RECALLED : break ;

	default : break ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: ChangeMap_Nack
//	DESC		: npc�� ���� ���̵� ���� �޽��� ó���� �ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: September 2, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::ChangeMap_Nack(void* pMsg) 
{
	// �Լ� ���� Ȯ��.
	if( !pMsg ) 
	{
		PrintError("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	// ���� �޽��� ��ȯ.
	MSG_BYTE* pmsg = NULL ;
	pmsg = (MSG_BYTE*)pMsg ;

	if( !pmsg )
	{
		PrintError("Failed to conver a message!!", __FUNCTION__) ;
		return ;
	}
    

	// ���� �޽����� Ȯ���Ѵ�.
	switch( pmsg->bData )
	{
	case e_RNET_FAILED_RECEIVE_USERINFO : break ;

	case e_RNET_FAILED_NOT_SIEGEWARTIME : 
		{
			WINDOWMGR->MsgBox(MBI_RECALLPORTAL_NOTUSE, MBT_OK, "�������� �ƴ϶� �̵��� �� �����ϴ�.") ;
		}
		break ;

	case e_RNET_FAILED_CHANGEMAP_INVALID_STATE : break ;

	default : break ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: AlreadyRecallNpc_Nack
//	DESC		: �̹� ��ȯ �� npc�̱� ������ ��ȯ ����ó���� �ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 28, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::AlreadyRecallNpc_Nack(void* pMsg)
{
	// �Լ� ���� Ȯ��.
	if( !pMsg ) 
	{
		PrintError("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	// ���� �޽��� ��ȯ.
	MSG_DWORD* pmsg = NULL ;
	pmsg = (MSG_DWORD*)pMsg ;

	if( !pmsg )
	{
		PrintError("Failed to conver a message!!", __FUNCTION__) ;
		return ;
	}


	// Npc Ÿ���� Ȯ���Ѵ�.
	WORD wErrorMsg = 0 ;
	BYTE byNpcType = Get_NpcType(pmsg->dwData) ;

	switch(byNpcType)
	{
	case eNR_ItSelf : 
		{
			// npc ������ �޴´�.
			CNpcRecallSelf* pInfo = NULL ;
			pInfo = Get_SelfNpc( pmsg->dwData ) ;

			if( !pInfo )
			{
				PrintError("Failed to receive move npc!!", __FUNCTION__) ;
				return ;
			}

			// ���� �޽����� �޴´�.
			wErrorMsg = pInfo->Get_RecallFailedMsg() ;
		}
		break ;

	case eNR_RemainTime : 
		{
			// npc ������ �޴´�.
			CNpcRecallRemainTime* pInfo = NULL ;
			pInfo = Get_TimeNpc( pmsg->dwData ) ;

			if( !pInfo )
			{
				PrintError("Failed to receive move npc!!", __FUNCTION__) ;
				return ;
			}

			// ���� �޽����� �޴´�.
			wErrorMsg = pInfo->Get_RecallFailedMsg() ;
		}
		break ;

	case eNR_Moveable : 
		{
			// npc ������ �޴´�.
			CNpcRecallMoveable* pInfo = NULL ;
			pInfo = Get_MoveNpc( pmsg->dwData ) ;

			if( !pInfo )
			{
				PrintError("Failed to receive move npc!!", __FUNCTION__) ;
				return ;
			}

			// ���� �޽����� �޴´�.
			wErrorMsg = pInfo->Get_RecallFailedMsg() ;
		}
		break ;

	default : break ;
	}	

	WINDOWMGR->MsgBox( MBI_NPCRECALLFAILED, MBT_OK, CHATMGR->GetChatMsg(wErrorMsg) );
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Get_CurTotalNpcCount
//	DESC		: ��ũ��Ʈ �ε� ��, ���ݱ��� �ε� �� �� npc ���� ��ȯ�ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 19, 2008
//-------------------------------------------------------------------------------------------------
WORD CNpcRecallMgr::Get_CurTotalNpcCount()
{
	// NPC Ÿ�Ժ�, �� ���� ��ȯ�Ѵ�.
	WORD wTotalCount = 0 ;

	wTotalCount = wTotalCount + (WORD)m_mRSelf_Npc.size() ;

	wTotalCount = wTotalCount + (WORD)m_mRTime_Npc.size() ;

	wTotalCount = wTotalCount + (WORD)m_mRMove_Npc.size() ;

	return wTotalCount ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Get_CurNpcBase
//	DESC		: ���� �ε� ��, npc�� �⺻ ������ ��ȯ�ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 19, 2008
//-------------------------------------------------------------------------------------------------
CNpcRecallBase* CNpcRecallMgr::Get_CurNpcBase()
{
	// ���� �ε����� npc�� �ε����� üũ�Ѵ�.
	WORD wReturnNpcIdx = 0 ;
	wReturnNpcIdx = Get_CurTotalNpcCount() ;

	if(wReturnNpcIdx == 0) return NULL ;

	--wReturnNpcIdx ; 


	// ������ ��ȯ/�Ҹ�Ǵ� npc���� Ȯ��.
	MAP_RSELF_NPC::iterator itRS ;
	itRS = m_mRSelf_Npc.find(wReturnNpcIdx) ;

	if( itRS != m_mRSelf_Npc.end() ) return ((CNpcRecallBase*)&itRS->second) ;


	// �����ð� ���� ��ȯ�Ǵ� npc���� Ȯ��.
	MAP_RTIME_NPC::iterator itRT ;
	itRT = m_mRTime_Npc.find(wReturnNpcIdx) ;

	if( itRT != m_mRTime_Npc.end() ) return ((CNpcRecallBase*)&itRT->second) ;


	// �̵��� ������ npc���� Ȯ��.
	MAP_RMOVE_NPC::iterator itRM ;
	itRM = m_mRMove_Npc.find(wReturnNpcIdx) ;

	if( itRM != m_mRMove_Npc.end() ) return ((CNpcRecallBase*)&itRM->second) ;


	// null�� return.
	return NULL ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Get_NpcType
//	DESC		: npc �ε����� npc�� Ÿ���� ��ȯ�ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 19, 2008
//-------------------------------------------------------------------------------------------------
BYTE CNpcRecallMgr::Get_NpcType(DWORD dwIndex) 
{
	// ���ڷ� �Ѿ�� �ε����� npc Ÿ���� üũ�Ѵ�.
	BYTE byNpcType = eNR_None ;


	// ������ ��ȯ/�Ҹ�Ǵ� npc���� Ȯ��.
	MAP_RSELF_NPC::iterator itRS ;

	for( itRS = m_mRSelf_Npc.begin() ; itRS != m_mRSelf_Npc.end() ; ++itRS )
	{
		if( itRS->second.Get_NpcRecallIndex() == dwIndex )
		{
			return eNR_ItSelf ;
		}
	}


	// �����ð� ���� ��ȯ�Ǵ� npc���� Ȯ��.
	MAP_RTIME_NPC::iterator itRT ;

	for( itRT = m_mRTime_Npc.begin() ; itRT != m_mRTime_Npc.end() ; ++itRT )
	{
		if( itRT->second.Get_NpcRecallIndex() == dwIndex )
		{
			return eNR_RemainTime ;
		}
	}


	// �̵��� ������ npc���� Ȯ��.
	MAP_RMOVE_NPC::iterator itRM ;

	for( itRM = m_mRMove_Npc.begin() ; itRM != m_mRMove_Npc.end() ; ++itRM )
	{
		if( itRM->second.Get_NpcRecallIndex() == dwIndex )
		{
			return eNR_Moveable ;
		}
	}


	// ����� return �Ѵ�.
	return byNpcType ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Get_SelfNpc
//	DESC		: ������ ��ȯ/�Ҹ�Ǵ� npc ������ ��ȯ�ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 19, 2008
//-------------------------------------------------------------------------------------------------
CNpcRecallSelf* CNpcRecallMgr::Get_SelfNpc(DWORD dwIndex)
{
	// npc ������ Ȯ���Ѵ�.
	MAP_RSELF_NPC::iterator it ;

	for( it = m_mRSelf_Npc.begin() ; it != m_mRSelf_Npc.end() ; ++it )
	{
		if( it->second.Get_NpcRecallIndex() != dwIndex ) continue ;

		return &it->second ;
	}

	return NULL ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Get_TimeNpc
//	DESC		: �����ð� ��ȯ/�Ҹ�Ǵ� npc ������ ��ȯ�ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 19, 2008
//-------------------------------------------------------------------------------------------------
CNpcRecallRemainTime* CNpcRecallMgr::Get_TimeNpc(DWORD dwIndex) 
{
	// npc ������ Ȯ���Ѵ�.
	MAP_RTIME_NPC::iterator it ;	

	for( it = m_mRTime_Npc.begin() ; it != m_mRTime_Npc.end() ; ++it )
	{
		if( it->second.Get_NpcRecallIndex() != dwIndex ) continue ;

		return &it->second ;
	}

	return NULL ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Get_MoveNpc
//	DESC		: �̵������� npc ������ ��ȯ�ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 19, 2008
//-------------------------------------------------------------------------------------------------
CNpcRecallMoveable* CNpcRecallMgr::Get_MoveNpc(DWORD dwIndex)
{
	// npc ������ Ȯ���Ѵ�.
	MAP_RMOVE_NPC::iterator it ;
	
	for( it = m_mRMove_Npc.begin() ; it != m_mRMove_Npc.end() ; ++it )
	{
		if( it->second.Get_NpcRecallIndex() != dwIndex ) continue ;

		return &it->second ;
	}

	return NULL ;
}

CNpcRecallBase* CNpcRecallMgr::Get_RecallBase(DWORD dwIndex)
{
	// ������ ��ȯ/�Ҹ�Ǵ� npc���� Ȯ��.
	MAP_RSELF_NPC::iterator itRS ;

	for( itRS = m_mRSelf_Npc.begin() ; itRS != m_mRSelf_Npc.end() ; ++itRS )
	{
		if( itRS->second.Get_NpcRecallIndex() == dwIndex )
		{
			return &itRS->second ;
		}
	}


	// �����ð� ���� ��ȯ�Ǵ� npc���� Ȯ��.
	MAP_RTIME_NPC::iterator itRT ;

	for( itRT = m_mRTime_Npc.begin() ; itRT != m_mRTime_Npc.end() ; ++itRT )
	{
		if( itRT->second.Get_NpcRecallIndex() == dwIndex )
		{
			return &itRT->second ;
		}
	}


	// �̵��� ������ npc���� Ȯ��.
	MAP_RMOVE_NPC::iterator itRM ;

	for( itRM = m_mRMove_Npc.begin() ; itRM != m_mRMove_Npc.end() ; ++itRM )
	{
		if( itRM->second.Get_NpcRecallIndex() == dwIndex )
		{
			return &itRM->second ;
		}
	}

	return NULL ;
}

CNpcRecallBase* CNpcRecallMgr::Get_RecallBaseByNpcId(DWORD dwNpcIndex)
{
	// ������ ��ȯ/�Ҹ�Ǵ� npc���� Ȯ��.
	MAP_RSELF_NPC::iterator itRS ;

	for( itRS = m_mRSelf_Npc.begin() ; itRS != m_mRSelf_Npc.end() ; ++itRS )
	{
		if( itRS->second.Get_NpcIndex() == dwNpcIndex )
		{
			return &itRS->second ;
		}
	}


	// �����ð� ���� ��ȯ�Ǵ� npc���� Ȯ��.
	MAP_RTIME_NPC::iterator itRT ;

	for( itRT = m_mRTime_Npc.begin() ; itRT != m_mRTime_Npc.end() ; ++itRT )
	{
		if( itRT->second.Get_NpcIndex() == dwNpcIndex )
		{
			return &itRT->second ;
		}
	}


	// �̵��� ������ npc���� Ȯ��.
	MAP_RMOVE_NPC::iterator itRM ;

	for( itRM = m_mRMove_Npc.begin() ; itRM != m_mRMove_Npc.end() ; ++itRM )
	{
		if( itRM->second.Get_NpcIndex() == dwNpcIndex )
		{
			return &itRM->second ;
		}
	}

	return NULL ;
}


//-------------------------------------------------------------------------------------------------
//	NAME		: Check_RecallNpc
//	DESC		: ��ȯ �� npc���� üũ�Ͽ�, ���� �̺�Ʈ ó���� �ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 22, 2008
//-------------------------------------------------------------------------------------------------
BYTE CNpcRecallMgr::Check_RecallNpc(DWORD dwRecallIdx) 
{
	// ���� ���� �� npc �ε����� �����Ѵ�.
	Set_CurSelectNpcId( dwRecallIdx ) ;


	// Npc Ÿ���� Ȯ���Ѵ�.
	BYTE byNpcType = Get_NpcType( dwRecallIdx ) ;

	switch(byNpcType)
	{
	case eNR_ItSelf : break ;
	case eNR_RemainTime : break ;
	case eNR_Moveable : 
		{
			// �̵� ���� ������ �޴´�.
			CNpcRecallMoveable* pInfo = NULL ;
			pInfo = Get_MoveNpc( dwRecallIdx ) ;

			if( !pInfo )
			{
				PrintError("Failed to receive move npc!!", __FUNCTION__) ;
				return FALSE ;
			}

			// �� �̵��� �� ������ ���� �޽��� �ڽ��� ����Ѵ�.
			WINDOWMGR->MsgBox(MBI_RECALLPORTAL_CHANGEMAP, MBT_YESNO, CHATMGR->GetChatMsg(480), GetMapName( pInfo->Get_ChangeMapNum() )) ;

			return TRUE ;
		}
		break ;

	default : break ;
	}

	return FALSE ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: PrintError
//	DESC		: ���� �޽����� ����ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 21, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::PrintError(char* szErr, char* szCaption)
{
#ifdef _USE_NPCRECALL_ERRBOX_ 
	MessageBox(NULL, szErr, szCaption, MB_OK) ;
#endif //_USE_NPCRECALL_ERRBOX_
}





//-------------------------------------------------------------------------------------------------
//	NAME		: LoadNpcBuffList
//	DESC		: Npc�� ������ �� �ִ� ���� ��ų ����Ʈ�� �ε��ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::LoadNPCBuffList()
{
	DWORD dwNpcIdx = 0;
	DWORD dwBuffIdx = 0;
	DWORDEX dwMoney = 0;
	char szBuff[2048] = {0};
	LPCTSTR seps = "\t\n" ;
	char* token = "";

	CMHFile fp;
	fp.Init("System/Resource/NpcBuffList.bin", "rb") ;
	
	M_NPCBUFFMAP::iterator it ;
	stNPCBuffData::L_BUFFLIST::iterator it_list ;
	BYTE bySameBuffIdx ;
	while(TRUE)
	{
		// ������ ������ üũ�Ѵ�.
		if( fp.IsEOF() ) break ;

		// ������ �о���δ�.
		fp.GetLine( szBuff, 2048 ) ;
		if(strlen(szBuff) == 0) continue ;

		// 081023 LYW --- NpcRecallMgr : npc ���� ������ �ε��ϴ� ��ũ��Ʈ����, �ּ� ������ ��ŵ ó���Ѵ�.
		// �ٸ� ��ũ��Ʈ���� ���� Type A ����� ����ϴµ�, �̷���, ���ڿ� �߰��� �ּ� ������ ó�� 
		// �Ǿ��� ���, ����� �ּ� ó���� ���ϰ� �ȴ�. ���ڿ� �߰��� �ּ� ������ �� ���ܸ� �����Ͽ�
		// Type B�� ����ϵ��� �ϰڴ�.
		// Type A :
		//if( strstr(szBuff, "//") || IsEmptyLine(szBuff) ) continue ;

		// Type B :
		BYTE byIsCommend = FALSE ;
		for( const TCHAR* subString = szBuff ; *subString ; subString = _tcsinc( subString ) )
		{
			if( !_tcsncmp( subString, "//", _tcslen("//") ) )
			{
				byIsCommend = TRUE ;
				break ;
			}
		}

		if( byIsCommend ) continue ;

		// ������ ������ Ȯ���Ѵ�.
		token = strtok( szBuff, seps ) ;

		if(token == NULL) continue ;

		// npc �ε����� �޴´�.
		else dwNpcIdx = atoi(token) ;

		// 081023 LYW --- NpcRecallMgr : �̹� ���� �� npc ������ �ִ��� Ȯ���Ѵ�.
		it = m_MNpcBuffMap.end() ;
		it = m_MNpcBuffMap.find( dwNpcIdx ) ;

		// ������ ������ ������, �����Ѵ�.
		if( it == m_MNpcBuffMap.end() )
		{
			stNPCBuffData buffData ;

			buffData.dwNpcIdx = dwNpcIdx ;

			while(TRUE)
			{
				// ���� �ε����� �о���δ�.
				token = strtok( NULL, seps ) ;
				if(token == NULL) break ;

				dwBuffIdx = atoi(token) ;

				// �ݾ��� �о���δ�.
				token = strtok( NULL, seps ) ;
				if(token == NULL) break ;

				dwMoney = _atoi64(token) ;

				// ���� �ε����� �ݾ��� �����Ѵ�.
				if( dwBuffIdx != 0 )
				{
					stNpcBuffInfo buffInfo ;

					buffInfo.dwBuffIdx  = dwBuffIdx ;
					buffInfo.dwMoney	= dwMoney ;

					buffData.buffList.push_back( buffInfo ) ;
				}
			}

			m_MNpcBuffMap.insert(std::make_pair( dwNpcIdx, buffData )) ;
		}
		// ���� �� ������ ������ �߰��Ѵ�.
		else 
		{
			while(TRUE)
			{
				// ���� �ε����� �о���δ�.
				token = strtok( NULL, seps ) ;
				if(token == NULL) break ;

				dwBuffIdx = atoi(token) ;

				// �ݾ��� �о���δ�.
				token = strtok( NULL, seps ) ;
				if(token == NULL) break ;

				dwMoney = atoi(token) ;

				// �̹� �ִ� �������� Ȯ���Ѵ�.
				bySameBuffIdx = FALSE ;
				for( it_list = it->second.buffList.begin() ; it_list != it->second.buffList.end() ;	++it_list )
				{
					if( it_list->dwBuffIdx == dwBuffIdx ) 
					{
						bySameBuffIdx = TRUE ;
						break ;
					}
				}

				if( bySameBuffIdx ) continue ;

				// ���� �ε����� �ݾ��� �����Ѵ�.
				if( dwBuffIdx != 0 )
				{
					stNpcBuffInfo buffInfo ;

					buffInfo.dwBuffIdx  = dwBuffIdx ;
					buffInfo.dwMoney	= dwMoney ;

					it->second.buffList.push_back( buffInfo ) ;
				}
			}
		}
	}


	// ���� Ŭ������ �����Ѵ�.
	fp.Release() ;
}

stNPCBuffData* CNpcRecallMgr::GetNPCBuffData(DWORD dwNPCIdx) 
{
	// 081023 LYW --- NpcRecallMgr : �޸� ������ ����, new delete�� ���� ���ϴ� ������ �����Ѵ�.
	//return m_NPCBuff.GetData(dwNPCIdx);

	M_NPCBUFFMAP::iterator it ;

	it = m_MNpcBuffMap.find( dwNPCIdx ) ;

	if( it == m_MNpcBuffMap.end() ) return NULL ;
	else return &it->second ;
}

void CNpcRecallMgr::OnAddRecallNpc(CNpc* pNpc )
{
	//����Ʈ ��ũ�� �޾�����
	QUESTMGR->ProcessNpcMark();
}

void CNpcRecallMgr::OnRemoveRecallNpc(CNpc* pNpc )
{
	if( pNpc == NULL )
		return;

	WORD RemoveNpcIndex = pNpc->GetNpcUniqueIdx() ;
	DWORD dwRecallNpcIdx = pNpc->GetNpcTotalInfo()->dwRecallNpcIdx ;
	BYTE byRecallType = Get_NpcType( dwRecallNpcIdx ) ;

	// ��ȯ npc��� 
	if( byRecallType != eNR_None )
	{
		CDealDialog* pDealDlg = GAMEIN->GetDealDialog();
		cNpcScriptDialog* pNpcScriptDlg	= GAMEIN->GetNpcScriptDialog();

		// DealDialog�� Ȱ��ȭ �Ǿ� �ְ� ������ ��ȯNPC�� ��� �۾�â�� �ݴ´�.
		if( pDealDlg )
		if( pDealDlg->IsActive() && pDealDlg->GetDealerIdx() == RemoveNpcIndex )
		{
			cDialog* pBuyDivideBox	= WINDOWMGR->GetWindowForID( DBOX_BUY );			// ���� �춧 ���� �Է�â
			cDialog* pSellDivideBox	= WINDOWMGR->GetWindowForID( DBOX_SELL );			// ���� �ȶ� ���� �Է�â
			cDialog* pBuyMsgBox		= WINDOWMGR->GetWindowForID( MBI_BUYITEM );			// ���� �춧 MSG BOX
			cDialog* pSellMsgBox	= WINDOWMGR->GetWindowForID( MBI_SELLITEM );		// ���� �ȶ� MSG BOX
			
			if( pBuyDivideBox )
			{
				pBuyDivideBox->SetActive( FALSE );
				WINDOWMGR->AddListDestroyWindow( pBuyDivideBox );
			}
			if( pSellDivideBox )
			{
				pSellDivideBox->SetActive( FALSE );
				WINDOWMGR->AddListDestroyWindow( pSellDivideBox );
			}
			if( pBuyMsgBox )
			{
				pBuyMsgBox->SetActive( FALSE );
				WINDOWMGR->AddListDestroyWindow( pBuyMsgBox );
			}
			if( pSellMsgBox )
			{
				pSellMsgBox->SetActive( FALSE );
				WINDOWMGR->AddListDestroyWindow( pSellMsgBox );
			}

			((cDialog*)GAMEIN->GetInventoryDialog())->SetDisable( FALSE );
			((cDialog*)GAMEIN->GetInventoryDialog())->SetActive( FALSE );				// �κ��丮�� �ݴ´�
			pDealDlg->SetDisable( FALSE );
			pDealDlg->SetActive( FALSE );	
		}

		// ScriptDialog�� Ȱ��ȭ �Ǿ� �ְ� ��ȯNPC�� ��� �ݴ´�.
		if( pNpcScriptDlg )
		if( pNpcScriptDlg->IsActive() && pNpcScriptDlg->GetCurNpcIdx() == RemoveNpcIndex )
		{
			pNpcScriptDlg->SetActive( FALSE );											// NPC Script â�� �ݴ´�
		}
	}
}