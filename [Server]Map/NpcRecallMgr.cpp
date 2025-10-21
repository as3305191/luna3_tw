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

#include "./CharMove.h"

#include "./Npc.h"

#include "./Network.h"

#include "./ObjectFactory.h"

#include "./UserTable.h"

#include "../[CC]Header/GameResourceManager.h"

#include "./AISystem.h"

#include "./ChannelSystem.h"

#include "./MapDBMsgParser.h"

#include "./Player.h"

#include "./LootingManager.h"

//#include "../hseos/Date/SHDateManager.h"

#include "./SiegeWarfareMgr.h" 

#include "../[CC]Skill/Server/Info/BuffSkillInfo.h"

#include "../[CC]Skill/Server/Manager/SkillManager.h"

#include "ItemManager.h"





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
//	NAME		: Initialize
//	DESC		: �ʱ�ȭ �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 19, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::Initialize() 
{
	// NPC ��ȯ ������ ������ �ִ� ��ũ��Ʈ ������ �ε��Ѵ�.
	Load_RecallNpc() ;

	// DB�� ����Ǿ��ִ� ��ȯ Npc �ҷ����� 
	Load_NpcFromDB() ;
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


	// SiegeRecall.bin ������ �б� ���� ����.
	if( !file.Init("./system/resource/NpcRecallInfo.bin", "rb") )
	{
		Throw_Error( "Failed to initialize SiegeRecall.bin", __FUNCTION__ ) ;
		return ;
	}


	// ���Ϸ� ���� ��ũ��Ʈ ������ �ε��Ѵ�.
	char szBuff[257] = {0, } ;
	BYTE byCommandKind = e_RNC_MAX ;

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

void CNpcRecallMgr::Load_NpcFromDB()
{
	// DB�� ���� �ð��� ������Ʈ 
	char txt[256] = {0, } ;
	sprintf(txt, "EXEC %s %d", MP_NPCREMAINTIME_LOAD,
		g_pServerSystem->GetMapNum() ) ;

	g_DB.Query( eQueryType_FreeQuery, eNpcRemainTime_Load, 0, txt) ;
}

void CNpcRecallMgr::OnLoadNpc_FromDB( DWORD dwRecallidx , DWORD dwChenel , DWORD dwRemainTime )
{
	BYTE byRecallType = Get_NpcType( dwRecallidx ) ;

	switch( byRecallType ) 
	{
		case eNR_ItSelf :
			break;
		case eNR_RemainTime :
			{
				CNpcRecallRemainTime* pRecallNpc = Get_TimeNpc( dwRecallidx ) ;

				if( ! pRecallNpc )
				{
					return;
				}

				CNpcRecallRemainTime Temp;
				memcpy( &Temp , pRecallNpc , sizeof( CNpcRecallRemainTime ) ) ;

				DWORD dwTimeDelta = pRecallNpc->Get_RemainTime() - dwRemainTime ;
				Temp.Set_RemainTime( dwRemainTime ) ;
				Temp.Set_RecallTime( GetTickCount() - dwTimeDelta ) ;

				Recall_Npc( &Temp ) ;
			}
			break;
		case eNR_Moveable :
			{
				CNpcRecallMoveable* pRecallNpc = Get_MoveNpc( dwRecallidx ) ;

				if( ! pRecallNpc )
				{
					return;
				}

				CNpcRecallMoveable Temp;
				memcpy( &Temp , pRecallNpc , sizeof( CNpcRecallMoveable ) ) ;

				DWORD dwTimeDelta = pRecallNpc->Get_RemainTime() - dwRemainTime ;
				Temp.Set_RemainTime( dwRemainTime ) ;
				Temp.Set_RecallTime( GetTickCount() - dwTimeDelta ) ;

				Recall_Npc( &Temp ) ;
			}
			break;
		default:
			break;
	}

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
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return byCommandKind ;
	}


	// ��ũ��Ʈ ���ɾ��� ��Ʈ�� ������ üũ.
	if( strlen(pString) <= 1 )
	{
		Throw_Error("Invalid string size!!", __FUNCTION__) ;
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
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
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
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// ���� �ε����� npc �⺻ ������ �޴´�.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		Throw_Error("Invalid Base info!!", __FUNCTION__) ;
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
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// ���� �ε����� npc �⺻ ������ �޴´�.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		Throw_Error("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}


	//// ������ npc id�� �ִ��� Ȯ���Ѵ�.
	//DWORD dwNpcId ;

	//MAP_RSELF_NPC::iterator itRS ;
	//for( itRS = m_mRSelf_Npc.begin() ; itRS != m_mRSelf_Npc.end() ; ++itRS )
	//{
	//	dwNpcId = 0 ;
	//	dwNpcId = itRS->second.Get_NpcIndex() ;

	//	if(dwNpcId == 0) continue ;

	//	if( dwNpcId == pBase->Get_NpcIndex() )
	//	{
	//		Throw_Error("Clone npc id!!", __FUNCTION__) ;
	//		return ;
	//	}
	//}

	//MAP_RTIME_NPC::iterator itRT ;
	//for( itRT = m_mRTime_Npc.begin() ; itRT != m_mRTime_Npc.end() ; ++itRT )
	//{
	//	dwNpcId = 0 ;
	//	dwNpcId = itRT->second.Get_NpcIndex() ;

	//	if(dwNpcId == 0) continue ;

	//	if( dwNpcId == pBase->Get_NpcIndex() )
	//	{
	//		Throw_Error("Clone npc id!!", __FUNCTION__) ;
	//		return ;
	//	}
	//}

	//MAP_RMOVE_NPC::iterator itRM ;
	//for( itRM = m_mRMove_Npc.begin() ; itRM != m_mRMove_Npc.end() ; ++itRM )
	//{
	//	dwNpcId = 0 ;
	//	dwNpcId = itRM->second.Get_NpcIndex() ;

	//	if(dwNpcId == 0) continue ;

	//	if( dwNpcId == pBase->Get_NpcIndex() )
	//	{
	//		Throw_Error("Clone npc id!!", __FUNCTION__) ;
	//		return ;
	//	}
	//}

	
	// Npc�ε����� �����Ѵ�.
	pBase->Set_NpcIndex( pFile->GetDword() ) ;
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
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// ���� �ε����� npc �⺻ ������ �޴´�.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		Throw_Error("Invalid Base info!!", __FUNCTION__) ;
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
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// ���� �ε����� npc �⺻ ������ �޴´�.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		Throw_Error("Invalid Base info!!", __FUNCTION__) ;
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
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// ���� �ε����� npc �⺻ ������ �޴´�.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		Throw_Error("Invalid Base info!!", __FUNCTION__) ;
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
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// ���� �ε����� npc �⺻ ������ �޴´�.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		Throw_Error("Invalid Base info!!", __FUNCTION__) ;
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
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// ���� �ε����� npc �⺻ ������ �޴´�.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		Throw_Error("Invalid Base info!!", __FUNCTION__) ;
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
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}

	// ���� �ε����� npc �⺻ ������ �޴´�.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		Throw_Error("Invalid Base info!!", __FUNCTION__) ;
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
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// ���� �ε����� npc �⺻ ������ �޴´�.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		Throw_Error("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}


	// Npc Ÿ���� Ȯ���Ѵ�.
	if( pBase->Get_RecallCondition() != eNR_ItSelf )
	{
		Throw_Error("Invalid Npc type!!", __FUNCTION__) ;
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
		Throw_Error("Can't find npc info!!", __FUNCTION__) ;
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
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// ���� �ε����� npc �⺻ ������ �޴´�.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		Throw_Error("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}


	// Npc Ÿ���� Ȯ���Ѵ�.
	BYTE byRecallCondition = pBase->Get_RecallCondition() ;
	if( byRecallCondition < eNR_RemainTime || byRecallCondition > eNR_Moveable )
	{
		Throw_Error("Invalid Npc type!!", __FUNCTION__) ;
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
			Throw_Error("Can't find npc info!!", __FUNCTION__) ;
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
			Throw_Error("Can't find npc info!!", __FUNCTION__) ;
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
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// ���� �ε����� npc �⺻ ������ �޴´�.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		Throw_Error("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}


	// Npc Ÿ���� Ȯ���Ѵ�.
	if( pBase->Get_RecallCondition() != eNR_Moveable )
	{
		Throw_Error("Invalid Npc type!!", __FUNCTION__) ;
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
		Throw_Error("Can't find npc info!!", __FUNCTION__) ;
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
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// ���� �ε����� npc �⺻ ������ �޴´�.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		Throw_Error("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}


	// Npc Ÿ���� Ȯ���Ѵ�.
	if( pBase->Get_RecallCondition() != eNR_Moveable )
	{
		Throw_Error("Invalid Npc type!!", __FUNCTION__) ;
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
		Throw_Error("Can't find npc info!!", __FUNCTION__) ;
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
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// ���� �ε����� npc �⺻ ������ �޴´�.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		Throw_Error("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}


	// Npc Ÿ���� Ȯ���Ѵ�.
	if( pBase->Get_RecallCondition() != eNR_Moveable )
	{
		Throw_Error("Invalid Npc type!!", __FUNCTION__) ;
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
		Throw_Error("Can't find npc info!!", __FUNCTION__) ;
		return ;
	}


	// ��ǥ�� �����Ѵ�.
	it->second.Set_MoveposZ(pFile->GetFloat()) ;
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
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// ���� �ε����� npc �⺻ ������ �޴´�.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		Throw_Error("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}


	// Npc Ÿ���� Ȯ���Ѵ�.
	if( pBase->Get_RecallCondition() != eNR_Moveable )
	{
		Throw_Error("Invalid Npc type!!", __FUNCTION__) ;
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
		Throw_Error("Can't find npc info!!", __FUNCTION__) ;
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
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// ���� �ε����� npc �⺻ ������ �޴´�.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		Throw_Error("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}


	// Npc Ÿ���� Ȯ���Ѵ�.
	if( pBase->Get_RecallCondition() != eNR_Moveable )
	{
		Throw_Error("Invalid Npc type!!", __FUNCTION__) ;
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
		Throw_Error("Can't find npc info!!", __FUNCTION__) ;
		return ;
	}


	// ��ǥ�� �����Ѵ�.
	it->second.Set_ChangeMapXPos(pFile->GetWord()) ;
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
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// ���� �ε����� npc �⺻ ������ �޴´�.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		Throw_Error("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}


	// Npc Ÿ���� Ȯ���Ѵ�.
	if( pBase->Get_RecallCondition() != eNR_Moveable )
	{
		Throw_Error("Invalid Npc type!!", __FUNCTION__) ;
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
		Throw_Error("Can't find npc info!!", __FUNCTION__) ;
		return ;
	}


	// ��ǥ�� �����Ѵ�.
	it->second.Set_ChangeMapZPos(pFile->GetWord()) ;
}

void CNpcRecallMgr::Load_SetParentNpcID(CMHFile* pFile) 
{
	// �Լ� ���� Ȯ��.
	if(!pFile)
	{
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}

	// ���� �ε����� npc �⺻ ������ �޴´�.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		Throw_Error("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}

	
	pBase->Set_ParentNpcIndex( pFile->GetDword() ) ;
}

void CNpcRecallMgr::Load_SetParentNpcType(CMHFile* pFile) 
{
	// �Լ� ���� Ȯ��.
	if(!pFile)
	{
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}

	// ���� �ε����� npc �⺻ ������ �޴´�.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		Throw_Error("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}

	
	pBase->Set_ParentNpcType( pFile->GetByte() ) ;

}


//-------------------------------------------------------------------------------------------------
//	NAME		: Process
//	DESC		: ���μ��� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 19, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::Process() 
{
	// �ð�üũ�� 1�� ������ �Ѵ�.
	DWORD dwCurTick = GetTickCount() ;
	static DWORD dwLastCheckTime = 0;

	if( dwCurTick - dwLastCheckTime < 60000 )	return ;	
	else dwLastCheckTime = dwCurTick ;


	// ������ ��ȯ/�Ҹ�Ǵ� npc �������� Ȯ���Ѵ�.
	CheckSchedule_SelfRecallNpc() ;


	// ���� �ð����� ��ȯ/�Ҹ�Ǵ� npc�� �������� üũ�Ѵ�.
	CheckSchedule_RemainTimeNpc() ;


	// �̵� ������ npc�� �������� üũ�Ѵ�.
	CheckSchedule_MoveableNpc() ;
}

//-------------------------------------------------------------------------------------------------
//	NAME		: NetworkMsgParser
//	DESC		: �޽��� �ļ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 20, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::NetworkMsgParser(DWORD dwIndex, void* pMsg, DWORD dwLength)
{
	// �Լ� ���ڸ� Ȯ���Ѵ�.
	if(!pMsg)
	{
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// �⺻�� �޽����� ��ȯ�Ѵ�.
	MSGBASE* pmsg = NULL ;
	pmsg = (MSGBASE*)pMsg ;

	if(!pmsg)
	{
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// ���������� Ȯ���Ѵ�.
	switch(pmsg->Protocol)
	{
	case MP_RECALLNPC_CTOM_RECALL_SYN :		CTOM_Recall_Syn(dwIndex, (char*)pMsg, dwLength) ;			break ;

	case MP_RECALLNPC_CTOAM_CHANGEMAP_SYN :	CTOAM_ChangeMap_Syn(dwIndex, (char*)pMsg, dwLength) ;		break ;

	case MP_RECALLNPC_CHEAT_RECALL_SYN :	Cheat_CTOA_Recall_Syn(dwIndex, (char*)pMsg, dwLength) ;	break ;

	case MP_RECALLNPC_ATOM_CHANGEMAP_ACK :	ATOM_ChangeMap_Ack(dwIndex, pMsg, dwLength) ;	break ;

	default : break ;
	}
}

//-------------------------------------------------------------------------------------------------
//	NAME		: CheckSchedule_SelfRecallNpc
//	DESC		: ������ ��ȯ/�Ҹ�Ǵ� npc�� �������� üũ�Ѵ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 22, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::CheckSchedule_SelfRecallNpc() 
{
	// ���� �ý����� �ð� ������ �޴´�.
	SYSTEMTIME cTime ;
	GetLocalTime( &cTime ) ;


	// �����̳ʿ� ��� npc���� �������� Ȯ���Ѵ�.
	BYTE byTableCount ;
	BYTE byCheckCount ;

	BYTE IsRunningNpc ;

	st_RECALL_TIMETABLE* pTable ;

	MAP_RSELF_NPC::iterator it ;
	MAP_ARSELF_NPC::iterator checkIt ;

	cPtrList	DeleteNpcList;

	// NPC �� ��ŭ FOR���� ������.
	for( it = m_mRSelf_Npc.begin() ; it != m_mRSelf_Npc.end() ;  ++it)
	{
		if( it->second.Get_RecallMap() != g_pServerSystem->GetMapNum() ) 
			continue;

		// ���̺� ī��Ʈ�� �޴´�.
		byTableCount = 0 ;
		byTableCount = it->second.Get_TableCount() ;

		enum eSCHEDULE { eSCHEDULE_NONE, eSCHEDULE_START, eSCHEDULE_END}	
		eResultSchedule = eSCHEDULE_NONE ;

		// ���̺� �� ��ŭ for���� ������.
		for( byCheckCount = 0 ; byCheckCount < byTableCount ; ++byCheckCount )
		{
			// ���̺� ������ �޴´�.
			pTable = NULL ;
			pTable = it->second.Get_TimeTable(byCheckCount) ;

			if(!pTable)
			{
				char szErr[125] = {0, } ;
				sprintf(szErr, "Invalid table info!! : %d, %d", it->second.Get_NpcRecallIndex(), byCheckCount ) ;
				Throw_Error(szErr, __FUNCTION__) ;
				return ;
			}

			// �������� Ȯ���Ѵ�.
			IsRunningNpc = FALSE ;


			// 091111 pdy �ð�äũ�� �߸��Ǿ� �־� ����
			// ������ �������� �ƴѰ� äũ 
			if(  pTable->wStartDay ==  pTable->wEndDay && cTime.wDayOfWeek != pTable->wStartDay )
			{
				continue;
			}

			if( pTable->wStartDay < pTable->wEndDay && 
				( cTime.wDayOfWeek < pTable->wStartDay || cTime.wDayOfWeek > pTable->wEndDay ) )
			{
				//��ȯ�ð��� �ƴϴ�.
				continue;
			}

			if( pTable->wStartDay > pTable->wEndDay && 
				cTime.wDayOfWeek <  pTable->wStartDay && cTime.wDayOfWeek > pTable->wEndDay )
			{
				//��ȯ�ð��� �ƴϴ�.
				continue;
			}

			//��ȯ ���� �ð����� ������ äũ 
			if( cTime.wDayOfWeek == pTable->wStartDay )
			{
				//�����ϴ� �ð����� �ð��� �۰ų� �ð��� ���� ���� �к��� �۴ٸ� 
				if( cTime.wHour < pTable->wStartHour ||
					cTime.wHour == pTable->wStartHour && cTime.wMinute < pTable->wStartMinute )
				{
					continue;
				}
			}

			//��ȯ ���� �ð��� �������� äũ 
			if( cTime.wDayOfWeek == pTable->wEndDay )
			{
				//������ �ð����� �ð��� �����ų� �ð��� ���� ������ �к��� ���ų� ũ�ٸ�
				if( cTime.wHour > pTable->wEndHour ||
					cTime.wHour == pTable->wEndHour && cTime.wMinute >= pTable->wEndMinute )
				{
					eResultSchedule = eSCHEDULE_END ;
					continue;
				}
			}

			//������� �Դٸ� ��ȯ �ð��̴�. 
			eResultSchedule = eSCHEDULE_START ;
			break;
		}

		if( eResultSchedule == eSCHEDULE_START ) 
		{
			// Ȱ��ȭ �� npc ����Ʈ�� ���ٸ�, �߰� ó���� �Ѵ�.
			CNpcRecallBase* pRecalledNpcBase = Get_ActiveSelfNpc( it->second.Get_NpcRecallIndex() , 1 ) ;

			if( pRecalledNpcBase == NULL )
			{
				Recall_Npc( &it->second ) ;
			}
		}
		else if( eResultSchedule == eSCHEDULE_END )
		{
			// ä�� ���� �޴´�.
			BYTE byChannelCount = (BYTE)CHANNELSYSTEM->GetChannelCount() ;

			// npc �߰� �۾�.
			for( BYTE byCount = 0 ; byCount < byChannelCount ; ++byCount )
			{
				WORD wChenel = byCount +1 ;
				CNpcRecallBase* pActivedNpcBase = Get_ActiveSelfNpc( it->second.Get_NpcRecallIndex() , wChenel ) ;

				if( pActivedNpcBase == NULL) 
					continue;

				DeleteNpcList.AddTail( pActivedNpcBase ) ;
			}
		}
	}

	PTRLISTPOS pos = DeleteNpcList.GetHeadPosition();
	while( pos )
	{
		CNpcRecallBase* pRecallBase = ( CNpcRecallBase* )DeleteNpcList.GetNext( pos );
		Delete_Npc( pRecallBase ) ;
	}
	DeleteNpcList.RemoveAll() ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: CheckSchedule_RemainTimeNpc
//	DESC		: ���� �ð����� ��ȯ/�Ҹ�Ǵ� npc�� �������� üũ�Ѵ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 22, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::CheckSchedule_RemainTimeNpc()
{
	// Ȱ��ȭ ���� npc�� �̵����� �ð�/ �Ҹ� �ð��� Ȯ���Ѵ�.
	DWORD dwRemainTime ;
	MAP_ARTIME_NPC::iterator it ;

	cPtrList	DeleteNpcList;

	// �� �����̳ʿ� ��� �� ��ŭ for���� ������.
	for( it = m_mARTime_Npc.begin() ; it != m_mARTime_Npc.end() ; ++it )
	{
		if( it->second.Get_RecallMap() != g_pServerSystem->GetMapNum() ) 
			continue;

		// ���� �ð��� �޴´�.
		dwRemainTime = 0 ;
		dwRemainTime = it->second.Get_RemainTime() ;

		// �����ð��� 1�� �̻��ϰ��, ���� �ð��� ���ҽ�Ų��.
		if( dwRemainTime >= SCHEDULE_CHECK_TIME )
		{
			dwRemainTime = dwRemainTime - SCHEDULE_CHECK_TIME ;

			if( dwRemainTime > it->second.Get_RemainTime() ) 
			{
				dwRemainTime = 0 ;
			}

			it->second.Set_RemainTime( dwRemainTime ) ;

			// DB�� ���� �ð��� ������Ʈ 
			char txt[256] = {0, } ;
			sprintf(txt, "EXEC %s %u, %d, %u", MP_NPCREMAINTIME_UPDATE,
				it->second.Get_NpcRecallIndex(), it->second.Get_RecalledChenel() ,dwRemainTime ) ;

			g_DB.Query( eQueryType_FreeQuery, eNpcRemainTime_Update, 0, txt) ;
			continue ;
		}
		// ���� �ð��� 1�� �̸��� ���, ���� �غ� �����ϰų�, ���� ó���� �Ѵ�.
		else
		{
			// �� ������, npc ���� ��û.
			if( it->second.Is_ReadyToDelete() )
			{
				DeleteNpcList.AddTail( &it->second ) ;
			}
			// ���� �����·� ����.
			else
			{
				//it->second.Set_RemainTime( 0 ) ;
				it->second.ClearRemainTime() ;
				it->second.Set_ReadyToDelete(TRUE) ;
			}
		}
	}

	PTRLISTPOS pos = DeleteNpcList.GetHeadPosition();
	while( pos )
	{
		CNpcRecallBase* pRecallBase = ( CNpcRecallBase* )DeleteNpcList.GetNext( pos );
		Delete_Npc( pRecallBase ) ;
	}
	DeleteNpcList.RemoveAll() ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: CheckSchedule_MoveableNpc
//	DESC		: �̵� ������ npc�� �������� üũ�Ѵ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 22, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::CheckSchedule_MoveableNpc() 
{
	// Ȱ��ȭ ���� npc�� �̵����� �ð�/ �Ҹ� �ð��� Ȯ���Ѵ�.
	DWORD dwRemainTime ;
	MAP_ARMOVE_NPC::iterator	it ;

	cPtrList	DeleteNpcList;

	// �� �����̳ʿ� ��� �� ��ŭ for���� ������.
	for( it = m_mARMove_Npc.begin() ; it != m_mARMove_Npc.end() ; ++it )
	{
		if( it->second.Get_RecallMap() != g_pServerSystem->GetMapNum() ) 
			continue;

		// npc ������ �̵� ���θ� ó���Ѵ�.
		if( !it->second.Is_Moved() )
		{
			// �� ������, npc �̵� ��û.
			if( GetTickCount() - it->second.Get_RecallTime() >= it->second.Get_MoveTime() )
			{
				it->second.Set_Moved(TRUE) ;

				Move_Npc( &it->second ) ;
			}
		}

		// ���� �ð��� �޴´�.
		dwRemainTime = 0 ;
		dwRemainTime = it->second.Get_RemainTime() ;

		// �����ð��� 1�� �̻��ϰ��, ���� �ð��� ���ҽ�Ų��.
		if( dwRemainTime >= SCHEDULE_CHECK_TIME )
		{
			dwRemainTime = dwRemainTime - SCHEDULE_CHECK_TIME ;

			if( dwRemainTime > it->second.Get_RemainTime() ) 
			{
				dwRemainTime = 0 ;
			}

			it->second.Set_RemainTime( dwRemainTime ) ;

			// DB�� ���� �ð��� ������Ʈ 
			char txt[256] = {0, } ;
			sprintf(txt, "EXEC %s %u, %d, %u", MP_NPCREMAINTIME_UPDATE,
				it->second.Get_NpcRecallIndex(), it->second.Get_RecalledChenel() ,dwRemainTime ) ;

			g_DB.Query( eQueryType_FreeQuery, eNpcRemainTime_Update, 0, txt) ;
			continue ;
		}
		// ���� �ð��� 1�� �̸��� ���, ���� �غ� �����ϰų�, ���� ó���� �Ѵ�.
		else
		{
			// �� ������, npc ���� ��û.
			if( it->second.Is_ReadyToDelete() )
			{
				DeleteNpcList.AddTail( &it->second ) ;
			}
			// ���� �����·� ����.
			else
			{
				it->second.ClearRemainTime() ;
				it->second.Set_ReadyToDelete(TRUE) ;
			}
		}
	}

	PTRLISTPOS pos = DeleteNpcList.GetHeadPosition();
	while( pos )
	{
		CNpcRecallBase* pRecallBase = ( CNpcRecallBase* )DeleteNpcList.GetNext( pos );
		Delete_Npc( pRecallBase ) ;
	}
	DeleteNpcList.RemoveAll() ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: CTOA_Recall_Syn
//	DESC		: Ŭ���̾�Ʈ�� ���� npc ��ȯ ��û�� ������ ��, ó���ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 19, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::CTOM_Recall_Syn(DWORD dwIndex, char* pMsg, DWORD dwLength) 
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


	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser( pmsg->dwObjectID );								

	if( !pPlayer)
	{
		Throw_Error("Invalid Player ID !!", __FUNCTION__) ;
		return ;
	}

	// NPC �ε����� Ȯ���Ͽ�, �ش��ϴ� NPC ������ �޴´�.

	CNpcRecallBase* pRecallBase = Get_RecallBase( pmsg->dwData ) ;
	BYTE byNpctype = (BYTE)((!pRecallBase)? eNR_None : pRecallBase->Get_RecallCondition() );

	if( byNpctype <= eNR_ItSelf || byNpctype >= eNR_NpcRecall_Max )
	{
		// Ŭ���̾�Ʈ�� ���� �޽����� �����Ѵ�.
		MSG_BYTE msg ;

		msg.Category	= MP_RECALLNPC ;
		msg.Protocol	= MP_RECALLNPC_MTOC_RECALL_NACK ;

		msg.dwObjectID	= dwIndex ;

		msg.bData		= e_RNET_INVALID_NPC_TYPE  ;

		pPlayer->SendMsg(&msg, sizeof(MSG_BYTE)) ;

		// ������ ���� �ڽ���, �α׸� �����.
		Throw_Error("Invalid npc type!!", __FUNCTION__) ;
		return;
	}

	// NPC Ÿ���� Ȯ���Ѵ�.
	switch(byNpctype)
	{
	case eNR_RemainTime : 
		{
			// Npc ������ Ȯ���Ѵ�.
			CNpcRecallRemainTime* pNpc = NULL ;
			pNpc = Get_TimeNpc(pmsg->dwData ) ;

			if(!pNpc)
			{
				// Ŭ���̾�Ʈ�� ���� �޽����� �����Ѵ�.
				MSG_BYTE msg ;

				msg.Category	= MP_RECALLNPC ;
				msg.Protocol	= MP_RECALLNPC_MTOC_RECALL_NACK ;

				msg.dwObjectID	= pmsg->dwObjectID ;

				msg.bData		= e_RNET_FAILED_RECEIVE_REMAINNPC  ;

				pPlayer->SendMsg(&msg, sizeof(MSG_BYTE)) ;

				// ������ ���� �ڽ���, �α׸� ����� return �Ѵ�.
				Throw_Error("Failed to receive remain npc!!", __FUNCTION__) ;
				return ;
			}

			if(! CheckHackRecallNpc( pPlayer , pmsg->dwData ) )
			{
				// Ŭ���̾�Ʈ�� ���� �޽����� �����Ѵ�.
				MSG_BYTE msg ;

				msg.Category	= MP_RECALLNPC ;
				msg.Protocol	= MP_RECALLNPC_MTOC_RECALL_NACK ;

				msg.dwObjectID	= dwIndex ;

				msg.bData		= e_RNET_FAILED_CREATE_NPC  ;

				pPlayer->SendMsg(&msg, sizeof(MSG_BYTE)) ;
				return;
			}

			// Ȱ��ȭ �� npc ������ Ȯ���Ѵ�.
			CNpcRecallRemainTime* pANpc = NULL ;
			pANpc = Get_ActiveTimeNpc(pmsg->dwData) ;

			if(pANpc)
			{
				// Ŭ���̾�Ʈ��, �̹� ��ȯ�Ǿ����� �����Ѵ�.
				MSG_DWORD msg ;

				msg.Category	= MP_RECALLNPC ;
				msg.Protocol	= MP_RECALLNPC_MTOC_ALREADY_RECALL_NACK ;

				msg.dwObjectID	= pmsg->dwObjectID ;

				msg.dwData		= pmsg->dwData ;
				pPlayer->SendMsg(&msg, sizeof(MSG_DWORD)) ;

				//// ������ ���� �ڽ���, �α׸� ����� return �Ѵ�.
				//Throw_Error("Already recalled npc!!", __FUNCTION__) ;
				return ;
			}

			Recall_Npc( pNpc ) ;
		}
		break ;

	case eNR_Moveable : 
		{
			// Npc ������ Ȯ���Ѵ�.
			CNpcRecallMoveable* pNpc = NULL ;
			pNpc = Get_MoveNpc(pmsg->dwData ) ;

			if(!pNpc)
			{
				// Ŭ���̾�Ʈ�� ���� �޽����� �����Ѵ�.
				MSG_BYTE msg ;

				msg.Category	= MP_RECALLNPC ;
				msg.Protocol	= MP_RECALLNPC_MTOC_RECALL_NACK ;

				msg.dwObjectID	= dwIndex ;

				msg.bData		= e_RNET_FAILED_RECEIVE_REMAINNPC  ;

				pPlayer->SendMsg(&msg, sizeof(MSG_BYTE)) ;
		
				// ������ ���� �ڽ���, �α׸� ����� return �Ѵ�.
				Throw_Error("Failed to receive remain npc!!", __FUNCTION__) ;
				return ;
			}

			if(! CheckHackRecallNpc( pPlayer , pmsg->dwData ) )
			{
				// Ŭ���̾�Ʈ�� ���� �޽����� �����Ѵ�.
				MSG_BYTE msg ;

				msg.Category	= MP_RECALLNPC ;
				msg.Protocol	= MP_RECALLNPC_MTOC_RECALL_NACK ;

				msg.dwObjectID	= dwIndex ;

				msg.bData		= e_RNET_FAILED_CREATE_NPC  ;

				pPlayer->SendMsg(&msg, sizeof(MSG_BYTE)) ;
				return;
			}

			// Ȱ��ȭ �� npc ������ Ȯ���Ѵ�.
			CNpcRecallMoveable* pANpc = NULL ;
			pANpc = Get_ActiveMoveNpc(pmsg->dwData) ;

			if(pANpc)
			{
				// Ŭ���̾�Ʈ��, �̹� ��ȯ�Ǿ����� �����Ѵ�.
				MSG_DWORD msg ;

				msg.Category	= MP_RECALLNPC ;
				msg.Protocol	= MP_RECALLNPC_MTOC_ALREADY_RECALL_NACK ;

				msg.dwObjectID	= pmsg->dwObjectID ;

				msg.dwData		= pmsg->dwData ;

				pPlayer->SendMsg(&msg, sizeof(MSG_DWORD)) ;

				//// ������ ���� �ڽ���, �α׸� ����� return �Ѵ�.
				//Throw_Error("Already recalled npc!!", __FUNCTION__) ;
				return ;
			}

			Recall_Npc( pNpc ) ;
		}
		break ;

	default : break ;
	}
}

//-------------------------------------------------------------------------------------------------
//	NAME		: WriteRecallNpcLog
//	DESC		: The function to create a error log for recall npc.
//	PROGRAMMER	: Yongs Lee
//	DATE		: October 28, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::WriteRecallNpcLog( char* pMsg )
{
	// �Լ� ���ڸ� Ȯ���Ѵ�.
	if( !pMsg ) return ;


	// �α׸� �����.
	SYSTEMTIME time ;
	GetLocalTime(&time) ;

	TCHAR szTime[_MAX_PATH] = {0, } ;
	sprintf(szTime, "%04d-%02d-%02d %02d:%02d:%02d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond) ;

	TCHAR szFile[_MAX_PATH] = {0, } ;
	sprintf( szFile, "Log/Siege_RecallNpcLog_%04d%02d%02d.log", time.wYear, time.wMonth, time.wDay ) ;

	FILE *fp = fopen(szFile, "a+") ;
	if (fp)
	{
		fprintf(fp, "[%s]\t%s\n", szTime, pMsg) ;
		fclose(fp) ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: LoadNpcBuffList
//	DESC		: Npc�� ������ �� �ִ� ���� ��ų ����Ʈ�� �ε��ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::LoadNPCBuffList()
{
	DWORD	dwNpcIdx ;
	DWORD	dwBuffIdx ;
	DWORDEX	dwMoney ;

	char	szBuff[2048] ;
	char	seps[] = "\t\n" ;
	char*	token ;

	CMHFile fp ;
	fp.Init("./System/Resource/NpcBuffList.bin", "rb") ;

	M_NPCBUFFMAP::iterator it ;
	stNPCBuffData::L_BUFFLIST::iterator it_list ;
	BYTE bySameBuffIdx ;
	while(FALSE == fp.IsEOF())
	{
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





// 080826 KTH
BYTE CNpcRecallMgr::ExcuteNPCBuff(CPlayer* pPlayer, WORD wNPCIndex, DWORD dwSelectBuff)
{
	// Player ������ Ȯ���Ѵ�.
	if( !pPlayer ) return e_REQUEST_BUFFSKILL_FAILED_RECEIVE_PLAYERINFO ;
	
	// Npc ������ Ȯ���Ѵ�.
	STATIC_NPCINFO* pInfo = GAMERESRCMNGR->GetStaticNpcInfo( wNPCIndex ) ;
	if( !pInfo ) return e_REQUEST_BUFFSKILL_FAILED_RECEIVE_NPCINFO ;


	// Npc�� Player�� �Ÿ��� üũ�Ѵ�.
	VECTOR3 vPos ;
	pPlayer->GetPosition( &vPos ) ;

	float dist = CalcDistanceXZ( &vPos, &pInfo->vPos ) ;

	if( dist > 3000.0f ) return e_REQUEST_BUFFSKILL_TOO_FAR_DISTANCE ;


	// Npc�� ���� �����͸� �޴´�.
	stNPCBuffData* pNPCBuff = GetNPCBuffData(wNPCIndex) ;

	if( !pNPCBuff) return e_REQUEST_BUFFSKILL_FAILED_RECEIVE_BUFFDATA ;


	// Npc ���� Ÿ��(���/Ư������)�� Ȯ���Ѵ�.
	DWORD dwBuffPay = 0 ;

	// ��ġ�ϴ� npc�� ���� �����͸� �޴´�.
	M_NPCBUFFMAP::iterator it ;
	it = m_MNpcBuffMap.find( wNPCIndex ) ;
	if( it == m_MNpcBuffMap.end() )
	{
		return e_REQUEST_BUFFSKILL_FAILED_RECEIVE_NPCINFO ; 
	}

	switch( dwSelectBuff )
	{
	case 0:
		{
			// ��ġ�ϴ� ���� ������ �޴´�.
			stNPCBuffData::L_BUFFLIST::iterator it_list ;

			for( it_list = it->second.buffList.begin() ; it_list != it->second.buffList.end() ; ++it_list )
			{
				// ��ų ������ �����Ѵ�.
				if( pPlayer->GetMoney() < it_list->dwMoney ) 
				{
					return e_REQUEST_BUFFSKILL_NOTENOUGH_PAY ;
				}
				else
				{
					pPlayer->SetMoney( it_list->dwMoney, MONEY_SUBTRACTION ) ;
				}

				// ���� ��ų ���� �ݾ��� �����Ѵ�.
				dwBuffPay += it_list->dwMoney ;

				// 081113 LYW --- NpcRecallMgr : ������������ ���� ��ų�� �ް� ���̵��ϸ�, ������ ������� ���� ����.
				cSkillInfo* pSkillInfo = SKILLMGR->GetSkillInfo( it_list->dwBuffIdx );
				if( !pSkillInfo ) return e_REQUEST_BUFFSKILL_FAILED_RECEIVE_BUFFSKILLINFO ;

				sSKILL_CREATE_INFO createinfo;

				createinfo.level			= pSkillInfo->GetLevel() ;
				createinfo.operatorId		= pPlayer->GetID() ;
				createinfo.mainTarget.SetMainTarget( pPlayer->GetID() ) ;
				createinfo.pos				= *CCharMove::GetPosition( pPlayer ) ;
				createinfo.skillObjectId	= 0 ;
				createinfo.remainTime		= 0 ;
				createinfo.count			= 1 ;

				SKILLMGR->OnBuffSkillStart( pSkillInfo, &createinfo ) ;
			}
		}
		break;
	default:
		{
			// ��ġ�ϴ� ���� ������ �޴´�.
			stNPCBuffData::L_BUFFLIST::iterator it_list ;

			for( it_list = it->second.buffList.begin() ; it_list != it->second.buffList.end() ; ++it_list )
			{
				// ��ġ�ϴ� ���� ������ ������ continue.
				if( it_list->dwBuffIdx != dwSelectBuff ) continue ;

				const cBuffSkillInfo* const pBuffSkillInfo = SKILLMGR->GetBuffInfo(it_list->dwBuffIdx);

				if( !pBuffSkillInfo ) return e_REQUEST_BUFFSKILL_FAILED_RECEIVE_BUFFSKILLINFO ;

				// ��ų ������ �����Ѵ�.
				if( pPlayer->GetMoney() < it_list->dwMoney ) 
				{
					return e_REQUEST_BUFFSKILL_NOTENOUGH_PAY ;
				}
				else
				{
					pPlayer->SetMoney( it_list->dwMoney, MONEY_SUBTRACTION ) ;
				}

				// ���� ��ų ���� �ݾ��� �����Ѵ�.
				dwBuffPay += it_list->dwMoney ;

				cSkillInfo* pSkillInfo = SKILLMGR->GetSkillInfo( it_list->dwBuffIdx );
				if( !pSkillInfo ) return e_REQUEST_BUFFSKILL_FAILED_RECEIVE_BUFFSKILLINFO ;

				sSKILL_CREATE_INFO createinfo;

				createinfo.level			= pSkillInfo->GetLevel() ;
				createinfo.operatorId		= pPlayer->GetID() ;
				createinfo.mainTarget.SetMainTarget( pPlayer->GetID() ) ;
				createinfo.pos				= *CCharMove::GetPosition( pPlayer ) ;
				createinfo.skillObjectId	= 0 ;
				createinfo.remainTime		= 0 ;
				createinfo.count			= 1 ;

				SKILLMGR->OnBuffSkillStart( pSkillInfo, &createinfo ) ;

				break ;
			}
		}
		break;
	}

	MSG_DWORD msg ; 

	msg.Category	= MP_SIEGEWARFARE ;
	msg.Protocol	= MP_SIEGEWARFARE_BUFF_ACK ;

	msg.dwData		= pPlayer->GetID() ;

	msg.dwData		= dwBuffPay ;

	pPlayer->SendMsg( &msg, sizeof(MSG_DWORD) ) ;

	return e_REQUEST_BUFFSKILL_SUCCESS ;

}

//-------------------------------------------------------------------------------------------------
//	NAME		: Cheat_CTOA_Recall_Syn
//	DESC		: �׽�Ʈ�� ġƮ �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 20, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::Cheat_CTOA_Recall_Syn(DWORD dwIndex, char* pMsg, DWORD dwLength)
{
	// �Լ� ���� Ȯ��.
	if( !pMsg )
	{
		Throw_Error("Invalid parameter!!", "CNpcRecallMgr::Cheat_CTOA_Recall_Syn") ;
		return ;
	}


	// ���� �޽��� ��ȯ.
	MSG_DWORD* pmsg = NULL ;
	pmsg = (MSG_DWORD*)pMsg ;

	if(!pmsg)
	{
		Throw_Error("Failed to convert a message to original!!", "CNpcRecallMgr::Cheat_CTOA_Recall_Syn") ;
		return ;
	}

	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser( pmsg->dwObjectID );								

	if( !pPlayer)
	{
		Throw_Error("Invalid Player ID !!", __FUNCTION__) ;
		return ;
	}


	// NPC �ε����� Ȯ���Ͽ�, �ش��ϴ� NPC ������ �޴´�.
	BYTE byNpctype = Get_NpcType( pmsg->dwData ) ;

	if( byNpctype <= eNR_ItSelf || byNpctype >= eNR_NpcRecall_Max )
	{
		// Ŭ���̾�Ʈ�� ���� �޽����� �����Ѵ�.
		MSG_BYTE msg ;

		msg.Category	= MP_RECALLNPC ;
		msg.Protocol	= MP_RECALLNPC_MTOC_RECALL_NACK ;

		msg.dwObjectID	= dwIndex ;

		msg.bData		= e_RNET_INVALID_NPC_TYPE  ;

		pPlayer->SendMsg(&msg, sizeof(MSG_BYTE)) ;

		// ������ ���� �ڽ���, �α׸� �����.
		Throw_Error("Invalid npc type!!", "CNpcRecallMgr::Cheat_CTOA_Recall_Syn") ;
	}


	// NPC Ÿ���� Ȯ���Ѵ�.
	switch(byNpctype)
	{
	case eNR_RemainTime : 
		{
			// Npc ������ Ȯ���Ѵ�.
			CNpcRecallRemainTime* pNpc = NULL ;
			pNpc = Get_TimeNpc(pmsg->dwData) ;

			if(!pNpc)
			{
				// Ŭ���̾�Ʈ�� ���� �޽����� �����Ѵ�.
				MSG_BYTE msg ;

				msg.Category	= MP_RECALLNPC ;
				msg.Protocol	= MP_RECALLNPC_MTOC_RECALL_NACK ;

				msg.dwObjectID	= dwIndex ;

				msg.bData		= e_RNET_FAILED_RECEIVE_REMAINNPC  ;

				pPlayer->SendMsg(&msg, sizeof(MSG_BYTE)) ;

				// ������ ���� �ڽ���, �α׸� ����� return �Ѵ�.
				Throw_Error("Failed to receive remain npc!!", "CNpcRecallMgr::Cheat_CTOA_Recall_Syn") ;
				return ;
			}

			Recall_Npc( pNpc ) ;
		}
		break ;

	case eNR_Moveable : 
		{
			// Npc ������ Ȯ���Ѵ�.
			CNpcRecallMoveable* pNpc = NULL ;
			pNpc = Get_MoveNpc(pmsg->dwData) ;

			if(!pNpc)
			{
				// Ŭ���̾�Ʈ�� ���� �޽����� �����Ѵ�.
				MSG_BYTE msg ;

				msg.Category	= MP_RECALLNPC ;
				msg.Protocol	= MP_RECALLNPC_MTOC_RECALL_NACK ;

				msg.dwObjectID	= dwIndex ;

				msg.bData		= e_RNET_FAILED_RECEIVE_REMAINNPC  ;

				pPlayer->SendMsg(&msg, sizeof(MSG_BYTE)) ;		

				// ������ ���� �ڽ���, �α׸� ����� return �Ѵ�.
				Throw_Error("Failed to receive remain npc!!", "CNpcRecallMgr::Cheat_CTOA_Recall_Syn" ) ;
				return ;
			}

			Recall_Npc( pNpc ) ;
		}
		break ;

	default : break ;
	}
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

	wTotalCount = WORD( wTotalCount + m_mRSelf_Npc.size() );

	wTotalCount = WORD( wTotalCount + m_mRTime_Npc.size() );

	wTotalCount = WORD( wTotalCount + m_mRMove_Npc.size() );

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


//-------------------------------------------------------------------------------------------------
//	NAME		: Get_ActiveSelfNpc
//	DESC		: ������ ��ȯ/�Ҹ�Ǵ� npc ������ ��ȯ�ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 21, 2008
//-------------------------------------------------------------------------------------------------
CNpcRecallSelf* CNpcRecallMgr::Get_ActiveSelfNpc(DWORD dwIndex , WORD wChenel) 
{
	// npc ������ Ȯ���Ѵ�.
	MAP_ARSELF_NPC::iterator it ;

	for( it = m_mARSelf_Npc.begin() ; it != m_mARSelf_Npc.end() ; ++it )
	{
		if( it->second.Get_NpcRecallIndex() != dwIndex ) continue ;
		if( it->second.Get_RecalledChenel() != wChenel ) continue ;

		return &it->second ;
	}

	return NULL ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Get_ActiveTimeNpc
//	DESC		: �����ð� ��ȯ/�Ҹ�Ǵ� npc ������ ��ȯ�ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 21, 2008
//-------------------------------------------------------------------------------------------------
CNpcRecallRemainTime* CNpcRecallMgr::Get_ActiveTimeNpc(DWORD dwIndex) 
{
	// npc ������ Ȯ���Ѵ�.
	MAP_ARTIME_NPC::iterator it ;

	for( it = m_mARTime_Npc.begin() ; it != m_mARTime_Npc.end() ; ++it )
	{
		if( it->second.Get_NpcRecallIndex() != dwIndex ) continue ;

		return &it->second ;
	}

	return NULL ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Get_ActiveMoveNpc
//	DESC		: �̵������� npc ������ ��ȯ�ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 21, 2008
//-------------------------------------------------------------------------------------------------
CNpcRecallMoveable* CNpcRecallMgr::Get_ActiveMoveNpc(DWORD dwIndex) 
{
	// npc ������ Ȯ���Ѵ�.
	MAP_ARMOVE_NPC::iterator it ;

	for( it = m_mARMove_Npc.begin() ; it != m_mARMove_Npc.end() ; ++it )
	{
		if( it->second.Get_NpcRecallIndex() != dwIndex ) continue ;

		return &it->second ;
	}

	return NULL ;
}


CNpcRecallBase* CNpcRecallMgr::Get_ActiveRecallBase(DWORD dwIndex , WORD wchenel )
{
	{
		// npc ������ Ȯ���Ѵ�.
		MAP_ARSELF_NPC::iterator it ;

		for( it = m_mARSelf_Npc.begin() ; it != m_mARSelf_Npc.end() ; ++it )
		{
			if( it->second.Get_NpcRecallIndex() != dwIndex ) continue ;
			if( it->second.Get_RecalledChenel() != wchenel ) continue ;

			return &it->second ;
		}
	}

	{
		// npc ������ Ȯ���Ѵ�.
		MAP_ARTIME_NPC::iterator it ;

		for( it = m_mARTime_Npc.begin() ; it != m_mARTime_Npc.end() ; ++it )
		{
			if( it->second.Get_NpcRecallIndex() != dwIndex ) continue ;
			if( it->second.Get_RecalledChenel() != wchenel ) continue ;

			return &it->second ;
		}
	}


	{
		// npc ������ Ȯ���Ѵ�.
		MAP_ARMOVE_NPC::iterator it ;

		for( it = m_mARMove_Npc.begin() ; it != m_mARMove_Npc.end() ; ++it )
		{
			if( it->second.Get_NpcRecallIndex() != dwIndex ) continue ;
			if( it->second.Get_RecalledChenel() != wchenel ) continue ;

			return &it->second ;
		}
	}

	return NULL ;
}


//-------------------------------------------------------------------------------------------------
//	NAME		: Get_ActiveMoveNpcByCreateIdx
//	DESC		: ���� ���̵��, ��ȯ���� �̵� npc ������ �˻��ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: September 9, 2008
//-------------------------------------------------------------------------------------------------
CNpcRecallMoveable* CNpcRecallMgr::Get_ActiveMoveNpcByCreateIdx(DWORD dwIndex)
{
	// npc ������ Ȯ���Ѵ�.
	MAP_ARMOVE_NPC::iterator it ;

	for( it = m_mARMove_Npc.begin() ; it != m_mARMove_Npc.end() ; ++it )
	{
		if( it->second.Get_CreatedIdx() != dwIndex ) continue ;

		return &it->second ;
	}

	return NULL ;
}

// ��ȯ NPC �� äũ  
BOOL CNpcRecallMgr::CheckHackRecallNpc( CPlayer* pPlayer , DWORD dwRecallIndex ) 
{
	CNpcRecallBase* pRecallBase = Get_RecallBase( dwRecallIndex ) ;

	if( ! pRecallBase || !pPlayer )
		return FALSE ;

	CNpcRecallBase* pActived = NPCRECALLMGR->Get_ActiveRecallBase( dwRecallIndex , (WORD) pPlayer->GetChannelID() ) ;

	//��ȯ���� üũ 
	if( pActived )
	{
		//��ȯ�� äũ�� ���� ��ȯ�� ��ġ ���� �÷��̾� ��ġ ������ äũ�� �Ѵ�.
		VECTOR3 vPos ,vRecallPos ;
		pPlayer->GetPosition( &vPos );
		pActived->Get_RecallPos( &vRecallPos ) ;

		float dist = CalcDistanceXZ( &vPos, &vRecallPos );
		if( dist > 3000.0f )
			return FALSE;
	}
	//��ȯ���� üũ 
	else
	{
		//��ȯ������ ��û�� RecallNpc�� ���Ͽ� �θ��� Npc������ �˾Ƴ� �˻縦 �Ѵ�.
		DWORD dwParentNpcIdx  = pRecallBase->Get_ParentNpcIndex() ;
		DWORD dwParentNpcType = (DWORD) pRecallBase->Get_ParentNpcType() ;

		if(	dwParentNpcType == CNpcRecallBase::eParentNpcType_Static )
		{
			//�θ� Static Npc��� ITEMMGR->CheckHackNpc() ȣ��
			if( ! ITEMMGR->CheckHackNpc( pPlayer, (WORD)dwParentNpcIdx ) )
				return FALSE;
		}
		else if( dwParentNpcType == CNpcRecallBase::eParentNpcType_Recall )
		{
			//�θ� RecallNpc��� ��ȯ�Ǿ��ִ� �θ�Npc������ ������ �Ÿ�üũ 
			CNpcRecallBase* pActivedParentBase = NPCRECALLMGR->Get_ActiveRecallBase( dwParentNpcIdx , (WORD) pPlayer->GetChannelID() );
			if( pActivedParentBase == NULL )
				return FALSE;

			VECTOR3 vPos , vRecallPos;

			pPlayer->GetPosition( &vPos );
			pActivedParentBase->Get_RecallPos( &vRecallPos ) ;

			float dist = CalcDistanceXZ( &vPos, &vRecallPos );
			if( dist > 3000.0f )
				return FALSE;
		}
	}

	return TRUE;
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

void CNpcRecallMgr::Recall_Npc(CNpcRecallBase* pNpcRecallBase)
{
	// 081029 LYW --- NpcRecallMgr : npc ���� �α׸� �����.
	WriteRecallNpcLog( "Call - Recall_Npc\n" ) ;

	if( pNpcRecallBase == NULL )
	{
		Throw_Error("Invalid parameter!!", "CNpcRecallMgr::Recall_Npc" ) ;
		return;
	}

	// ��ȯ ������ Ȯ���Ѵ�.
	if( pNpcRecallBase->Get_RecallMap() != g_pServerSystem->GetMapNum() ) return ;

	// Npc list ������ �޴´�.
	NPC_LIST* pNpcList = NULL ;
	pNpcList = GAMERESRCMNGR->GetNpcList(pNpcRecallBase->Get_NpcKind()) ;

	if(pNpcList == 0)
	{
		Recall_Failed( pNpcRecallBase , e_RNET_FAILED_RECEIVE_NPCLIST ) ;

		// ���� �����޽����� ���ų�, �α׸� �����.
		Throw_Error("Failed to receive npc list info!!", __FUNCTION__) ;
		return ;
	}

	// ä�� ���� �޴´�.
	BYTE byChannelCount ;
	if(pNpcRecallBase->Get_RecallCondition() == eNR_ItSelf)
	{
		byChannelCount = (BYTE)CHANNELSYSTEM->GetChannelCount() ;
	}
	else
	{
		byChannelCount = 1 ;
	}

	CNpcRecallBase*			pTempRecallBase = NULL ;
	CNpcRecallSelf			TempRecallSelf;
	CNpcRecallRemainTime	TempRemainTime;
	CNpcRecallMoveable		TempMoveble;

	switch( pNpcRecallBase->Get_RecallCondition() ) 
	{
		case eNR_ItSelf : 
			{
				TempRecallSelf.Copy( (CNpcRecallSelf*)pNpcRecallBase ) ;
				pTempRecallBase = &TempRecallSelf;
			}
			break;
		case eNR_RemainTime :
			{
				memcpy( &TempRemainTime , pNpcRecallBase , sizeof( CNpcRecallRemainTime ) ) ;
				pTempRecallBase = &TempRemainTime;
			}
			break;
		case eNR_Moveable :
			{
				memcpy( &TempMoveble , pNpcRecallBase , sizeof( CNpcRecallMoveable ) ) ;
				pTempRecallBase = &TempMoveble;
			}
			break;
		default:
			return;
	}

	// npc �߰� �۾�.
	for( BYTE byCount = 0 ; byCount < byChannelCount ; ++byCount )
	{
		// �ӽ� ���� ����.
		BASEOBJECT_INFO Baseinfo ;
		NPC_TOTALINFO NpcTotalInfo ;
		VECTOR3 vPos ;

		Baseinfo.dwObjectID = g_pAISystem.GeneraterMonsterID();

		pTempRecallBase->Set_CreatedIdx( Baseinfo.dwObjectID  ) ;

		if( pTempRecallBase->Get_RecallCondition() == eNR_ItSelf)
		{
			Baseinfo.BattleID = byCount + 1 ;	
		}
		else
		{
			Baseinfo.BattleID = 1 ;	
		}

		pTempRecallBase->Set_RecalledChenel( (WORD)Baseinfo.BattleID ) ;

		// NPC �Ӽ��� �����Ѵ�.
		SafeStrCpy(Baseinfo.ObjectName, pTempRecallBase->Get_NpcName(), MAX_NPC_NAME_LENGTH+1);
		NpcTotalInfo.Group			= 0 ;
		NpcTotalInfo.MapNum			= g_pServerSystem->GetMapNum() ;
		NpcTotalInfo.NpcJob			= pNpcList->JobKind ;
		NpcTotalInfo.NpcKind		= pNpcList->NpcKind ;
		NpcTotalInfo.dwRecallNpcIdx	= pTempRecallBase->Get_NpcRecallIndex() ;
		NpcTotalInfo.NpcUniqueIdx	= (WORD)pTempRecallBase->Get_NpcIndex() ;

		// NPC ��ġ�� �����Ѵ�.
		vPos.x = pTempRecallBase->Get_RecallposX() ;
		vPos.y = 0.0f ;
		vPos.z = pTempRecallBase->Get_RecallposZ() ;

		// NPC�� �߰��Ѵ�.
		CNpc* pNpc = NULL ;
		pNpc = g_pServerSystem->AddNpc(&Baseinfo, &NpcTotalInfo, &vPos) ;

		if( !pNpc )
		{
			Recall_Failed( pTempRecallBase , e_RNET_FAILED_CREATE_NPC );
		
			// ���� �����޽����� ���ų�, �α׸� �����.
			Throw_Error("Failed to create npc!!", __FUNCTION__) ;
			return ;
		}

		// �ڵ��Ҹ���� �ʵ��� �����Ѵ�.
		if(pNpc) pNpc->SetDieTime(0) ;

		// ���� ���̺��� npc�� �߰��Ѵ�.
		//g_pUserTable->AddUser( pNpc, pNpc->GetID() ) ;
		Recall_success( pTempRecallBase ) ;
	}

	// 081029 LYW --- NpcRecallMgr : npc ���� �α׸� �����.
	WriteRecallNpcLog( "End - Recall_Npc\n" ) ;
}

void CNpcRecallMgr::Recall_success( CNpcRecallBase* pNpcRecallBase )
{
	if( pNpcRecallBase == NULL )
	{
		Throw_Error("Invalid parameter!!", "CNpcRecallMgr::Recall_success" ) ;
		return;
	}

	// npc Ÿ���� Ȯ���Ѵ�.
	switch( pNpcRecallBase->Get_RecallCondition() )
	{
		case eNR_ItSelf :
		{
			// ������ npc�� �����ϴ��� �˻��Ѵ�.
			MAP_ARSELF_NPC::iterator it ;
			it = m_mARSelf_Npc.find( pNpcRecallBase->Get_CreatedIdx() ) ;

			if( it != m_mARSelf_Npc.end() )
			{
				Throw_Error("Clon npc!!", "CNpcRecallMgr::Recall_success" ) ;
				return ;
			}

			// ncp ������ ����, ���� ���̵� �����Ѵ�.
			CNpcRecallSelf newNpc ;
			newNpc.Copy( (CNpcRecallSelf*)pNpcRecallBase ) ; 

			if( pNpcRecallBase->Get_RecallTime() == 0 )
				newNpc.Set_RecallTime( GetTickCount() ) ;

			// npc�� Ȱ��ȭ ���� npc�� ��� �����̳ʿ� �߰��Ѵ�.
			m_mARSelf_Npc.insert( std::make_pair( pNpcRecallBase->Get_CreatedIdx() , newNpc ) ) ;

			// 081028 LYW --- NpcRecallMgr : npc ��ȯ ������ �����.
			char szLog[ 1024 ] = {0, } ;
			sprintf( szLog, "Recalled Npc - %u %u %d %d", newNpc.Get_CreatedIdx(), newNpc.Get_NpcRecallIndex() , 
				newNpc.Get_RecallMap() , newNpc.Get_RecalledChenel() ) ;
			WriteRecallNpcLog( szLog ) ;
		}
		break ;

		case eNR_RemainTime :
		{
			// ������ npc�� �����ϴ��� �˻��Ѵ�.
			MAP_ARTIME_NPC::iterator it ;
			it = m_mARTime_Npc.find( pNpcRecallBase->Get_CreatedIdx() ) ;

			if( it != m_mARTime_Npc.end() )
			{
				Throw_Error("Clon npc!!", __FUNCTION__) ;
				return ;
			}

			// ncp ������ ����, ���� ���̵� �����Ѵ�.
			CNpcRecallRemainTime newNpc ;
			memcpy(&newNpc, pNpcRecallBase, sizeof(CNpcRecallRemainTime)) ;

			if( pNpcRecallBase->Get_RecallTime() == 0 )
				newNpc.Set_RecallTime( GetTickCount() ) ;

			// npc�� Ȱ��ȭ ���� npc�� ��� �����̳ʿ� �߰��Ѵ�.
			m_mARTime_Npc.insert( std::make_pair( pNpcRecallBase->Get_CreatedIdx() , newNpc ) ) ;

			// DB�� npc ������ ������Ʈ �Ѵ�.
			DWORD dwRecallidx	= newNpc.Get_NpcRecallIndex() ;
			DWORD dwNpcIdx		= newNpc.Get_NpcIndex() ;
			DWORD dwRecallMap	= newNpc.Get_RecallMap() ;
			DWORD dwRemainTime	= newNpc.Get_RemainTime() ;
			DWORD dwChenel		= newNpc.Get_RecalledChenel() ;

			char txt[256] = {0, } ;
			sprintf( txt, "EXEC %s %u, %u, %u, %u, %u", MP_NPCREMAINTIME_INSERT, dwRecallidx, 
				dwNpcIdx, dwRecallMap, dwChenel , dwRemainTime ) ;

			g_DB.Query(  eQueryType_FreeQuery, eNpcRemainTime_Insert, 0 , txt ) ;

			// 081028 LYW --- NpcRecallMgr : npc ��ȯ ������ �����.
			char szLog[ 1024 ] = {0, } ;
			sprintf( szLog, "Recalled Npc - %u %u %d %d", newNpc.Get_CreatedIdx(), newNpc.Get_NpcRecallIndex() , 
				newNpc.Get_RecallMap() , newNpc.Get_RecalledChenel() ) ;
		}
		break ;

		case eNR_Moveable :
		{
			// ������ npc�� �����ϴ��� �˻��Ѵ�.
			MAP_ARMOVE_NPC::iterator it ;
			it = m_mARMove_Npc.find( pNpcRecallBase->Get_CreatedIdx() ) ;

			if( it != m_mARMove_Npc.end() )
			{
				Throw_Error("Clon npc!!", __FUNCTION__) ;
				return ;
			}

			// ncp ������ ����, ���� ���̵� �����Ѵ�.
			CNpcRecallMoveable newNpc ;
			memcpy(&newNpc, pNpcRecallBase, sizeof(CNpcRecallMoveable)) ;

			if( newNpc.Get_RecallTime() == 0 )
				newNpc.Set_RecallTime( GetTickCount() ) ;

			// npc�� Ȱ��ȭ ���� npc�� ��� �����̳ʿ� �߰��Ѵ�.
			m_mARMove_Npc.insert( std::make_pair( pNpcRecallBase->Get_CreatedIdx() , newNpc ) ) ;

			// DB�� npc ������ ������Ʈ �Ѵ�.
			DWORD dwRecallidx	= newNpc.Get_NpcRecallIndex() ;
			DWORD dwNpcIdx		= newNpc.Get_NpcIndex() ;
			DWORD dwRecallMap	= newNpc.Get_RecallMap() ;
			DWORD dwRemainTime	= newNpc.Get_RemainTime() ;
			DWORD dwChenel		= newNpc.Get_RecalledChenel() ;

			char txt[256] = {0, } ;
			sprintf( txt, "EXEC %s %u, %u, %u, %u, %u", MP_NPCREMAINTIME_INSERT, dwRecallidx, 
				dwNpcIdx, dwRecallMap, dwChenel , dwRemainTime ) ;

			g_DB.Query(  eQueryType_FreeQuery, eNpcRemainTime_Insert, 0 , txt ) ;

			// 081028 LYW --- NpcRecallMgr : npc ��ȯ ������ �����.
			char szLog[ 1024 ] = {0, } ;
			sprintf( szLog, "Recalled Npc - %u %u %d %d", newNpc.Get_CreatedIdx(), newNpc.Get_NpcRecallIndex() , 
				newNpc.Get_RecallMap() , newNpc.Get_RecalledChenel() ) ;
		}
		break ;

	default : break ;
	}



}

void CNpcRecallMgr::Recall_Failed( CNpcRecallBase* pNpcRecallBase , BYTE byError )
{
	// ���� �޽����� Ȯ���Ѵ�.
	switch( byError )
	{
	case e_RNET_FAILED_RECEIVE_STATICNPCINFO : break ;
	case e_RNET_FAILED_CREATE_NPC : break ;

	default : break ;
	}
}

void CNpcRecallMgr::Delete_Npc( CNpcRecallBase* pNpcRecallBase )
{
	// 081029 LYW --- NpcRecallMgr : npc ���� �α׸� �����.
	WriteRecallNpcLog( "Call - Delete_Npc\n" ) ;

	// �Լ� ���� Ȯ��.
	if( !pNpcRecallBase )
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}

	// ��ȯ ������ Ȯ���Ѵ�.
	if( pNpcRecallBase->Get_RecallMap() != g_pServerSystem->GetMapNum() ) return ;

	// Npc ���� ó���� �Ѵ�.
	CObject* pObject = NULL ;
	pObject = g_pUserTable->FindUser( pNpcRecallBase->Get_CreatedIdx() ) ;

	// 081029 LYW --- NpcRecallMgr : ��ȯ�Ǵ� npc ���̵� �����.
	char szNpcID[256] = {0, } ;
	sprintf(szNpcID, "Delete Npc Id : %u", pNpcRecallBase->Get_CreatedIdx() ) ;
	WriteRecallNpcLog( szNpcID ) ;

	if(!pObject)
	{
		Delete_Failed( pNpcRecallBase , e_RNET_FAILED_RECEIVE_NPC_FROM_USERTABLE ) ;

		Throw_Error("Failed to find npc!!", __FUNCTION__) ;
		return ;
	}

	//CCharMove::ReleaseMove(pObject) ;

	g_pServerSystem->RemoveNpc( pNpcRecallBase->Get_CreatedIdx() ) ;

	Delete_Success(  pNpcRecallBase ) ;

	WriteRecallNpcLog( "End - Delete_Npc\n" ) ;
	//g_pObjectFactory->ReleaseObject(pObject) ;
	//g_pUserTable->RemoveUser(pmsg->dwData) ;
}

void CNpcRecallMgr::Delete_Success( CNpcRecallBase* pNpcRecallBase )
{
	// �Լ� ���� Ȯ��.
	if( !pNpcRecallBase )
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}

	BYTE byRecallCondition = pNpcRecallBase->Get_RecallCondition();

	// npc Ÿ���� Ȯ���Ѵ�.
	switch( byRecallCondition )
	{
	case eNR_ItSelf :
		{
			// ������ npc�� �����ϴ��� �˻��Ѵ�.
			MAP_ARSELF_NPC::iterator it ;
			it = m_mARSelf_Npc.find( pNpcRecallBase->Get_CreatedIdx() ) ;

			if( it == m_mARSelf_Npc.end() )
			{
				Throw_Error("Can't find active npc!!", __FUNCTION__) ;
				return ;
			}

			// 081028 LYW --- NpcRecallMgr : npc ��ȯ ������ �����.
			char szLog[ 1024 ] = {0, } ;
			sprintf( szLog, "Delete Npc - %u %u %d %d", pNpcRecallBase->Get_CreatedIdx(), pNpcRecallBase->Get_NpcRecallIndex() , 
				pNpcRecallBase->Get_RecallMap() , pNpcRecallBase->Get_RecalledChenel() ) ;
			WriteRecallNpcLog( szLog ) ;

			m_mARSelf_Npc.erase( it ) ;
		}
		break ;

	case eNR_RemainTime :
		{
			// ������ npc�� �����ϴ��� �˻��Ѵ�.
			MAP_ARTIME_NPC::iterator it ;
			it = m_mARTime_Npc.find( pNpcRecallBase->Get_CreatedIdx() ) ;

			if( it == m_mARTime_Npc.end() )
			{
				Throw_Error("Can't find active npc!!", __FUNCTION__) ;
				return ;
			}

			// DB���� �����ð� ������ �����Ѵ�.
			char txt[256] = {0, } ;
			sprintf(txt, "EXEC %s %u, %d", MP_NPCREMAINTIME_DELETE,
				it->second.Get_NpcRecallIndex(), it->second.Get_RecalledChenel() ) ;

			g_DB.Query( eQueryType_FreeQuery, eNpcRemainTime_Delete, 0, txt) ;

			// 081028 LYW --- NpcRecallMgr : npc ��ȯ ������ �����.
			char szLog[ 1024 ] = {0, } ;
			sprintf( szLog, "Delete Npc - %u %u %d %d", pNpcRecallBase->Get_CreatedIdx(), pNpcRecallBase->Get_NpcRecallIndex() , 
				pNpcRecallBase->Get_RecallMap() , pNpcRecallBase->Get_RecalledChenel() ) ;
			WriteRecallNpcLog( szLog ) ;

			m_mARTime_Npc.erase( it ) ;
		}
		break ;

	case eNR_Moveable :
		{
			// ������ npc�� �����ϴ��� �˻��Ѵ�.
			MAP_ARMOVE_NPC::iterator it ;
			it = m_mARMove_Npc.find( pNpcRecallBase->Get_CreatedIdx() ) ;

			if( it == m_mARMove_Npc.end() )
			{
				Throw_Error("Can't find active npc!!", __FUNCTION__) ;
				return ;
			}

			// DB���� �����ð� ������ �����Ѵ�.
			char txt[256] = {0, } ;
			sprintf(txt, "EXEC %s %u, %d", MP_NPCREMAINTIME_DELETE,
				it->second.Get_NpcRecallIndex(), it->second.Get_RecalledChenel() ) ;

			g_DB.Query( eQueryType_FreeQuery, eNpcRemainTime_Delete, 0, txt) ;

			// 081028 LYW --- NpcRecallMgr : npc ��ȯ ������ �����.
			char szLog[ 1024 ] = {0, } ;
			sprintf( szLog, "Delete Npc - %u %u %d %d", pNpcRecallBase->Get_CreatedIdx(), pNpcRecallBase->Get_NpcRecallIndex() , 
				pNpcRecallBase->Get_RecallMap() , pNpcRecallBase->Get_RecalledChenel() ) ;
			WriteRecallNpcLog( szLog ) ;

			m_mARMove_Npc.erase( it ) ;
		}
		break ;

	default : 
		{
			Throw_Error("Can't find npc type!!", __FUNCTION__) ;
			return ;
		}
		break ;
	}

}

void CNpcRecallMgr::Delete_Failed( CNpcRecallBase* pNpcRecallBase , BYTE byError ) 
{
	// �Լ� ���� Ȯ��.
	if( !pNpcRecallBase )
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}

	// ���� �޽����� Ȯ���Ѵ�.
	switch( byError )
	{
	case e_RNET_FAILED_RECEIVE_NPC_FROM_USERTABLE : break ;

	default : break ;
	}
}

void CNpcRecallMgr::Move_Npc( CNpcRecallBase* pNpcRecallBase )
{
	// �Լ� ���� Ȯ��.
	if( !pNpcRecallBase )
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}

	// ��ȯ ������ Ȯ���Ѵ�.
	if( pNpcRecallBase->Get_RecallMap() != g_pServerSystem->GetMapNum() ) return ;


	// Npc �̵� ó���� �Ѵ�.
	CObject* pObject = NULL ;
	pObject = g_pUserTable->FindUser( pNpcRecallBase->Get_CreatedIdx() ) ;

	if(!pObject)
	{
		Move_Npc_Failed( pNpcRecallBase , e_RNET_FAILED_RECEIVE_NPC_FROM_USERTABLE ) ;
		
		Throw_Error("Failed to find npc!!", __FUNCTION__) ;
		return ;
	}

	VECTOR3 vec ;

	vec.x = ((CNpcRecallMoveable*)pNpcRecallBase)->Get_MovePosX() ;
	vec.y = 0 ;
	vec.z = ((CNpcRecallMoveable*)pNpcRecallBase)->Get_MovePosZ();

	CCharMove::Warp(pObject, &vec) ;

	//CCharMove::SetPosition(pObject, &vec) ;
	//CCharMove::InitMove(pObject, &vec) ;

	Move_Npc_Success( pNpcRecallBase ) ; 

	// 081028 LYW --- NpcRecallMgr : npc ��ȯ ������ �����.
	char szLog[ 1024 ] = {0, } ;
	sprintf( szLog, "Move Npc - %u %u", g_pServerSystem->GetMapNum() , pNpcRecallBase->Get_NpcRecallIndex() ) ;
	WriteRecallNpcLog( szLog ) ;
}

void CNpcRecallMgr::Move_Npc_Success( CNpcRecallBase* pNpcRecallBase ) 
{
	// �Լ� ���� Ȯ��.
	if( !pNpcRecallBase )
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}

	BYTE byRecallCondition = pNpcRecallBase->Get_RecallCondition() ;
	// npc Ÿ���� Ȯ���Ѵ�.
	switch( byRecallCondition )
	{
	case eNR_ItSelf :
		{
			Throw_Error("invalid move type!!", __FUNCTION__) ;
			return ;
		}
		break ;

	case eNR_RemainTime :
		{
			Throw_Error("invalid move type!!", __FUNCTION__) ;
			return ;
		}
		break ;

	case eNR_Moveable :
		{
			// ������ npc�� �����ϴ��� �˻��Ѵ�.
			MAP_ARMOVE_NPC::iterator it ;
			it = m_mARMove_Npc.find( pNpcRecallBase->Get_CreatedIdx() ) ;

			if( it == m_mARMove_Npc.end() )
			{
				Throw_Error("Can't find active npc!!", __FUNCTION__) ;
				return ;
			}

			it->second.Set_RecallPosX( it->second.Get_MovePosX() ) ;
			it->second.Set_RecallPosZ( it->second.Get_MovePosZ() ) ;
		}
		break ;

	default : 
		{
			Throw_Error("Can't find npc type!!", __FUNCTION__) ;
			return ;
		}
		break ;
	}
}

void CNpcRecallMgr::Move_Npc_Failed( CNpcRecallBase* pNpcRecallBase , BYTE byError ) 
{
	// �Լ� ���� Ȯ��.
	if( !pNpcRecallBase )
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}

	// ���� �޽����� Ȯ���Ѵ�.
	switch( byError )
	{
	case e_RNET_FAILED_RECEIVE_NPC_FROM_USERTABLE : break ;

	default : break ;
	}
}

void CNpcRecallMgr::CTOAM_ChangeMap_Syn(DWORD dwIndex, char* pMsg, DWORD dwLength) 
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

	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser( pmsg->dwObjectID );								

	if( !pPlayer)
	{
		Throw_Error("Invalid Player ID !!", __FUNCTION__) ;
		return ;
	}

	// Ȱ��ȭ ��(��ȯ��)npc ������ ��ȿ���� üũ�Ѵ�.
	CNpcRecallMoveable* pMoveNpc = NULL ;
	pMoveNpc = Get_ActiveMoveNpc(pmsg->dwData) ;

	if( !pMoveNpc )
	{
		// Ŭ���̾�Ʈ�� ���� �޽��� ����.
		MSG_BYTE msg ;

		msg.Category	= MP_RECALLNPC ;
		msg.Protocol	= MP_RECALLNPC_MTOC_CHANGEMAP_NACK ;

		msg.dwObjectID	= pmsg->dwObjectID ;

		msg.bData		= e_RNET_FAILED_RECEIVE_NPCINFO ;

		pPlayer->SendMsg(&msg, sizeof(MSG_BYTE)) ;

		// ������ ���� �޽��� ��� ó��.
		Throw_Error("Failed to receive npc info!!", __FUNCTION__) ;
		return ;
	}

	if( ! CheckHackRecallNpc( pPlayer , pmsg->dwData ) )
	{
		// Ŭ���̾�Ʈ�� ���� �޽��� ����.
		MSG_BYTE msg ;

		msg.Category	= MP_RECALLNPC ;
		msg.Protocol	= MP_RECALLNPC_MTOC_CHANGEMAP_NACK ;

		msg.dwObjectID	= pmsg->dwObjectID ;

		msg.bData		= e_RNET_FAILED_CHECK_HACK_RECALLNPC_CHECK ;

		pPlayer->SendMsg(&msg, sizeof(MSG_BYTE)) ;

		// ������ ���� �޽��� ��� ó��.
		Throw_Error("Failed to CheckHackRecallNpc !!", __FUNCTION__) ;
		return ;
	}

	MSG_DWORD3 msg ;
	msg.Category		= MP_RECALLNPC ;
	msg.Protocol		= MP_RECALLNPC_MTOA_CHANGEMAP_SYN ;

	msg.dwObjectID		= pmsg->dwObjectID ;
	msg.dwData1			= pMoveNpc->Get_RecallMap() ;
	msg.dwData2			= pMoveNpc->Get_ChangeMapNum() ;
	msg.dwData3			= pmsg->dwData;

	g_Network.Send2AgentServer((char*)&msg, sizeof(MSG_DWORD3));
}

//-------------------------------------------------------------------------------------------------
//	NAME		: ATOM_ChangeMap_Ack
//	DESC		: �̵� ������ npc�� ����, �� �̵� ��û�� ������ �� ó���ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 21, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::ATOM_ChangeMap_Ack(DWORD dwIndex, void* pMsg, DWORD dwLength) 
{
	// �Լ� ���� Ȯ��.
	if( !pMsg )
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	MSG_DWORD* pmsg = NULL ;
	pmsg = (MSG_DWORD*)pMsg ;

	if( !pmsg )
	{
		Throw_Error("Failed to convert a message to original!!", __FUNCTION__) ;
		return ;
	}

	CNpcRecallMoveable* pMoveNpc = NULL ;
	pMoveNpc = Get_ActiveMoveNpc(pmsg->dwData) ;

	if( !pMoveNpc) 
	{
		Throw_Error("ATOM_ChangeMap_Ack !pMoveNpc", __FUNCTION__) ;
		return ;
	}
			
	MAPTYPE ChangeMapNum	= pMoveNpc->Get_ChangeMapNum() ;			
	BYTE    ChannelNum		= 0 ;							
	float	fChangeMapXPos	= pMoveNpc->Get_ChangeMapXPos() ;									
	float	fChangeMapZPos	= pMoveNpc->Get_ChangeMapZPos() ;					


	// �������� ��� ��� �� �� ��ȣ�� �޾Ƶд�.
	WORD wCurMapNum = g_pServerSystem->GetMapNum() ;


	// �� �ε����� Ȯ���Ѵ�.
	if( pMoveNpc->Get_RecallMap() != wCurMapNum ) return ;


	// Player ������ �޴´�.
	CPlayer* pPlayer = NULL ;
	pPlayer = (CPlayer *)g_pUserTable->FindUser(pmsg->dwObjectID) ;

	if(pPlayer == NULL)
	{
		// ������ ���� �޽��� ��� ó���� �Ѵ�.
		Throw_Error("Failed to receive player info!!", __FUNCTION__) ;
		return ;
	}
	// 090701 LUJ, �� �̵� �� �÷��̾� ������ DB�� ���µ�, �ʱ�ȭ�� ������ �ʾ��� ��� �����Ⱚ��
	//		����� �� �ִ�. �ʱ�ȭ�� ���� ��ü�� ó���Ѵ�
	else if( FALSE == pPlayer->GetInited() )
	{
		return;
	}

	// �̵��� ���� ���� �����̶�� ���������� äũ�� ���� 
	if( SIEGEWARFAREMGR->IsSiegeWarfareZone( ChangeMapNum , FALSE ) )
	{
		// ���������� ���� ��쿡��, ���� ó���� �Ѵ�.
		if( SIEGEWARFAREMGR->GetState() <= eSiegeWarfare_State_Before )
		{
			// Ŭ���̾�Ʈ�� ���� �޽����� ������.
			MSG_BYTE msg ;

			msg.Category	= MP_RECALLNPC ;
			msg.Protocol	= MP_RECALLNPC_MTOA_CHANGEMAP_NACK ;

			msg.dwObjectID	= pmsg->dwObjectID ;

			msg.bData		= e_RNET_FAILED_NOT_SIEGEWARTIME ;

			pPlayer->SendMsg( &msg, sizeof(MSG_BYTE) ) ;

			return ;
		}
	}

	
	// �����Ʈ/ PK��� / ���� / ���� / ����Ʈ �� üũ.
	if( pPlayer->GetAutoNoteIdx() || pPlayer->IsPKMode() ||
		LOOTINGMGR->IsLootedPlayer( pPlayer->GetID() ) ||
		pPlayer->GetState() == eObjectState_Die //&& 
		//g_csDateManager.IsChallengeZoneHere() == FALSE) 
		)
	{
		// Ŭ���̾�Ʈ�� ���� �޽����� ������.
		MSG_BYTE msg ;

		msg.Category	= MP_RECALLNPC ;
		msg.Protocol	= MP_RECALLNPC_MTOA_CHANGEMAP_NACK ;

		msg.dwObjectID	= pmsg->dwObjectID ;

		msg.bData		= e_RNET_FAILED_CHANGEMAP_INVALID_STATE ;

		pPlayer->SendMsg( &msg, sizeof(MSG_BYTE) ) ;

		// ������ ���� �޽��� ��� ó���� �Ѵ�.
		Throw_Error("Invalid player state!!", __FUNCTION__) ;
		return ;
	}


	// �� / ������ �������� db�� ������Ʈ �Ѵ�.
	pPlayer->UpdateLogoutToDB(FALSE) ;


	// Player�� �� �̵� ������ �����Ѵ�.
	pPlayer->SetMapMoveInfo( ChangeMapNum, (DWORD)fChangeMapXPos, (DWORD)fChangeMapZPos ) ;

	// 081218 LUJ, �����ϱ� ���� ���� ��������
	const DWORD		playerIndex		= pPlayer->GetID();
	const DWORD		userIndex		= pPlayer->GetUserID();
	const MONEYTYPE	inventoryMoney	= pPlayer->GetMoney( eItemTable_Inventory );
	const MONEYTYPE	storageMoney	= pPlayer->GetMoney( eItemTable_Storage );

	g_pServerSystem->RemovePlayer( playerIndex );

	// 081218 LUJ, ������Ʈ���� �� �̵��� ���� ����� ��� ������Ʈ���� ���� ������ ���Եȴ�.
	//			�̸� ���� ���� ���ν��� ó�� �Ϸ� �� �����Ѵ�
	UpdatePlayerOnMapOut(
		playerIndex,
		userIndex,
		inventoryMoney,
		storageMoney,
		ChangeMapNum,
		WORD( ChannelNum ) );
}













