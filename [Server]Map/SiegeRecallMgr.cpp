//-------------------------------------------------------------------------------------------------
//	NAME		: CSiegeRecallMgr.cpp
//	DESC		: Implementation part of CSiegeRecallMgr class.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 28, 2008
//-------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------------------------
//		Include part.
//-------------------------------------------------------------------------------------------------
#include "stdafx.h"																						// An include file for standard system include files
#include "./SiegeRecallMgr.h"

#include "..\[CC]Header\GameResourceStruct.h"
#include "..\[CC]Header\GameResourceManager.h"

#include "../[CC]RecallInfo/SiegeRecall/Siege_AddObj.h"

#include "./Network.h"

#include "./Object.h"

#include "./UserTable.h"

#include "./RegenManager.h"

#include "./AISystem.h"

#include "./MapDBMsgParser.h"

#include "./SiegeWarfareMgr.h"

#include "./Monster.h"
#include "./Player.h"







//-------------------------------------------------------------------------------------------------
//	NAME		: CSiegeRecallMgr
//	DESC		: The function constructor.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 28, 2008
//-------------------------------------------------------------------------------------------------
CSiegeRecallMgr::CSiegeRecallMgr(void)
{
	// 081006 LYW --- SiegeRecallMgr : ���� �� ��ȯ���� ��ȯ�Ǿ����� ���θ� ���� �÷��� �߰�.
	m_wRecallCount = 0 ;
	m_byReadyToUseWaterSeed = FALSE ;

	// 081226 LYW --- SiegeRecallMgr : ����ũ �ε��� �߰�.
	m_byUniqueIdx = 0 ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: ~CSiegeRecallMgr
//	DESC		: The function constructor.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 28, 2008
//-------------------------------------------------------------------------------------------------
CSiegeRecallMgr::~CSiegeRecallMgr(void)
{
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Initialieze
//	DESC		: The function to initialize recall manager.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 04, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Initialieze()
{
	// Resetting siege obj info.
	Reset_SiegeObjInfo() ;

	// Load object info.
	//Request_ObjFromDB() ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Reset_SiegeObjInfo
//	DESC		: The function to resetting.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 04, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Reset_SiegeObjInfo() 
{
	// 081006 LYW --- SiegeRecallMgr : ���� �� ��ȯ���� ��ȯ�Ǿ����� ���θ� ���� �÷��� �߰�.
	m_byReadyToUseWaterSeed = FALSE ;


	// Remove all obj info.
	M_MADDOBJ::iterator it ;
	
	for( it = m_mAddObj.begin() ; it != m_mAddObj.end() ; ++it )
	{
		m_mAddObj.erase(it) ;
	}

	SIEGEWARFAREMGR->Set_LoadSummonFlag( FALSE ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Get_AddObjIdx
//	DESC		: The function to return add object info.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 04, 2008
//-------------------------------------------------------------------------------------------------
CSiege_AddObj* CSiegeRecallMgr::Get_AddObjIdx(DWORD dwObjIdx) 
{
	// Check index.
	M_MADDOBJ::iterator it ;

	for( it = m_mAddObj.begin() ; it != m_mAddObj.end() ; ++it)
	{
		//090812 pdy ������ Agent�� Map�������� ��ȯObj �ε��������� �߸����ִ� ���� ���� 
		if( it->second.Get_ObjInfo()->dwObjectIdx != dwObjIdx ) continue ;

		return &it->second ;
	}


	// Return null.
	return NULL ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: NetworkMsgParser
//	DESC		: The function constructor.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 28, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::NetworkMsgParser(void* pMsg )
{
	// Check a parameter.
	if(!pMsg)
	{
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// Convert a messaeg to base message.
	MSGBASE* pmsg = NULL ;
	pmsg = (MSGBASE*)pMsg ;

	if(!pmsg)
	{
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// Check a protocol.
	switch(pmsg->Protocol)
	{
	case MP_SIEGERECALL_ADDOBJ_SYN :								Add_Object( pMsg ) ;					break ;

	case MP_SIEGERECALL_ADDCHECKOBJDIE_SYN :						Add_CheckObjDie( pMsg ) ;				break ;

	case MP_SIEGERECALL_USEITEM_SYN :								Add_CheckUseItem( pMsg ) ;				break ;

	case MP_SIEGERECALL_ADDCOMMAND_SYN :							Add_Command( pMsg ) ;					break ;

	case MP_SIEGERECALL_NOTICE_REMOVE_ALLMAP_ALLOBJ_SYN :			Remove_AllMapAllObj( pMsg ) ;			break ;

	case MP_SIEGERECALL_NOTICE_REMOVE_ALLMAP_SPECIFYOBJ_SYN :		Remove_AllMapSpecifyObj( pMsg ) ;		break ;

	case MP_SIEGERECALL_NOTICE_REMOVE_SPECIFYMAP_ALLOBJ_SYN :		Remove_SpecifyMapAllObj( pMsg ) ;		break ;

	case MP_SIEGERECALL_NOTICE_REMOVE_SPECIFYMAP_SPECIFYOBJ_SYN :	Remove_SpecifyMapSpecifyObj( pMsg ) ;	break ;

	case MP_SIEGERECALL_REQUEST_OBJINFO_ACK :						Set_LoadSummonFlag( pMsg ) ;			break ;

	// 081006 LYW --- SiegeRecallMgr : ��ȯ�ؾ� �� ��ȯ ī��Ʈ�� �޴´�.
	case MP_SIEGERECALL_REQUEST_RECALLCOUNT_ACK :					Set_RecallCount( pMsg ) ;				break ;

	// 081015 LYW --- SiegeRecallMgr : ���� ��ȯ���� ������ ų ���¸� �ε��Ͽ� ������ �Ѿ���� �� ó���ϴ� �۾� �߰�.
	case MP_SIEGERECALL_LOAD_RECALLCOUNT_ACK :						Load_RecallCount( pMsg ) ;				break ;
	case MP_SIEGE_CHEAT_NEXTSTEP_SYN :								Cheat_ExcuteNextStep( pMsg ) ;			break ;

	default : break ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Add_Object
//	DESC		: The function constructor.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 28, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Add_Object(void* pMsg)
{
	// ��ȯ�� �Ϸ� �� ���¸�, ���̻� ��ȯ ������ ���� �ʴ´�.
	//if( SIEGEWARFAREMGR->Is_LoadedSummon() ) return ;

	BYTE IsSiegeWarfareZone = BYTE( SIEGEWARFAREMGR->IsSiegeWarfareZone() );
	if( !IsSiegeWarfareZone ) return ;

	// Check a aprarmeter.
	if(!pMsg)
	{
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// Convert a message.
	MSG_SIEGERECALL_OBJINFO* pmsg = NULL ;
	pmsg = (MSG_SIEGERECALL_OBJINFO*) pMsg ;

	if(!pmsg)
	{
		Throw_Error("Failed to convert a message!!", __FUNCTION__) ;
		return ;
	}


	// Check clone object info.
	st_SIEGEOBJ* pCheckObjInfo ;
	M_MADDOBJ::iterator it ;
	for( it = m_mAddObj.begin() ; it != m_mAddObj.end() ; ++it )
	{
		pCheckObjInfo = NULL ;
		pCheckObjInfo = it->second.Get_ObjInfo() ;

		if(!pCheckObjInfo) continue ;

		if(	pCheckObjInfo->wThemeIdx	== pmsg->siegeObj.wThemeIdx		&&
			pCheckObjInfo->mapNum		== pmsg->siegeObj.mapNum		&&
			pCheckObjInfo->byStepIdx	== pmsg->siegeObj.byStepIdx		&&
			pCheckObjInfo->byAddObjIdx	== pmsg->siegeObj.byAddObjIdx	&&
			pCheckObjInfo->byComKind	== pmsg->siegeObj.byComKind		&&
			pCheckObjInfo->byComIndex	== pmsg->siegeObj.byComIndex	&&
			pCheckObjInfo->fXpos		== pmsg->siegeObj.fXpos			&&
			pCheckObjInfo->fZpos		== pmsg->siegeObj.fZpos ) 
		{
			return ;
		}
	}


	// Check map.
	if( pmsg->siegeObj.mapNum != g_pServerSystem->GetMapNum() ) 
	{
		return ;
	}

	VECTOR3 vRecallPos ;

	BASE_MONSTER_LIST* pList = NULL ;

	BYTE byRadius = pmsg->siegeObj.byRadius ;

	// �ݰ��� 0�� �ƴϸ�,
	if( pmsg->siegeObj.byUseRandomPos )
	{
		// ���� ��ġ�� �����ϰ� �����Ѵ�.
		vRecallPos.x = pmsg->siegeObj.fXpos + (rand()%(byRadius*2)) - byRadius ;
		vRecallPos.z = pmsg->siegeObj.fZpos + (rand()%(byRadius*2)) - byRadius ;
	}
	// �ݰ��� 0�̸�, 
	else
	{
		// ���� ��ġ�� �����Ѵ�.
		vRecallPos.x = pmsg->siegeObj.fXpos ;
		vRecallPos.z = pmsg->siegeObj.fZpos ;
	}

	// ���� Ÿ�Կ� ���� ���� ����Ʈ�� �޴´�.
	pList = NULL ;
	pList = GAMERESRCMNGR->GetMonsterListInfo(pmsg->siegeObj.dwObjectIdx) ;
	
	// ���� ����Ʈ�� ��ȿ�ϴٸ�,
	if(pList)
	{
		// ���� �Ŵ����� ������Ʈ�� ���� �Ѵ�.
		CMonster* pMonster = NULL ;
		pMonster = REGENMGR->RegenObject(
			g_pAISystem.GeneraterMonsterID(),
			0,
			1,
			pList->ObjectKind,
			pList->MonsterKind,
			&vRecallPos,
			DWORD(EVENTRECALL_GROUP_ID),
			0,
			0,
			FALSE,
			TRUE);

		if( pMonster )
		{
			// ���� ����Ʈ�� �߰�/
			CSiege_AddObj newObj ;
			// 081226 LYW --- SiegeRecallMgr : �ε��� ������ Map�����̳��� ������� �ϸ� ���� �߻�.
			// ����ũ �ε����� ������ �� �ֵ��� �Ѵ�.
			newObj.Set_Index( m_byUniqueIdx ) ;
			//newObj.Set_Index( m_mAddObj.size() ) ;
			//090812 pdy ������ Agent�� Map�������� ��ȯObj �ε��������� �߸����ִ� ���� ���� 
			newObj.Set_MosterID( pMonster->GetID() ) ;
			newObj.Init_ObjInfo(&pmsg->siegeObj) ;

			// 081226 LYW --- SiegeRecallMgr : ����ũ �ε����� �����Ѵ�.
			++m_byUniqueIdx ;

			m_mAddObj.insert( std::make_pair( newObj.Get_Index(), newObj ) ) ;

			if( pmsg->siegeObj.byComKind == e_CK_ADD_OBJECT )
			{
				Add_ObjToDB( pmsg->siegeObj.wThemeIdx, pmsg->siegeObj.mapNum, g_pServerSystem->GetMapNum(), 
							 pmsg->siegeObj.byStepIdx, pmsg->siegeObj.byAddObjIdx, e_CK_ADD_OBJECT, 0, 1 ) ;


				// 081028 LYW --- SiegeRecallMgr : ĳ����Ʈ ��ȯ ���� �޽��� �α׸� �����.
				if( IsSiegeWarfareZone )
				{
					char szLog[1024] = {0, } ;
					sprintf(szLog, "Recall Catapult - %d %d %d %d %d %d %d %d \n", pmsg->siegeObj.wThemeIdx, pmsg->siegeObj.mapNum, 
						g_pServerSystem->GetMapNum(), pmsg->siegeObj.byStepIdx, pmsg->siegeObj.byAddObjIdx, e_CK_ADD_OBJECT, 0, 1 ) ;
					WriteCatapultLog( szLog ) ;
				}

			}
			else if( pmsg->siegeObj.byComKind == e_CK_COMMAND )
			{
				Add_ObjToDB( pmsg->siegeObj.wThemeIdx, pmsg->wExValue, g_pServerSystem->GetMapNum(), 
							 pmsg->siegeObj.byStepIdx, pmsg->siegeObj.byAddObjIdx, e_CK_COMMAND, pmsg->siegeObj.byComIndex, 3 ) ;

				// 081028 LYW --- SiegeRecallMgr : ĳ����Ʈ ��ȯ ���� �޽��� �α׸� �����.
				if( IsSiegeWarfareZone )
				{
					char szLog[1024] = {0, } ;
					sprintf(szLog, "Recall Catapult - %d %d %d %d %d %d %d %d \n", pmsg->siegeObj.wThemeIdx, 
						pmsg->wExValue, g_pServerSystem->GetMapNum(), pmsg->siegeObj.byStepIdx, pmsg->siegeObj.byAddObjIdx, 
						e_CK_COMMAND, pmsg->siegeObj.byComIndex, 3 ) ;
					WriteCatapultLog( szLog ) ;
				}
			}
		}
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Add_CheckObjDie
//	DESC		: The function to add check routine for object die.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 01, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Add_CheckObjDie(void* pMsg)
{
	// Check parameter.
	if(!pMsg)
	{
		Throw_Error("Invalid message parameter!!", __FUNCTION__) ;
		return ;
	}


	// Convert a message.
	MSG_SIEGERECALL_CHECK_OBJ_DIE* pmsg = NULL ;
	pmsg = (MSG_SIEGERECALL_CHECK_OBJ_DIE*) pMsg ;

	if(!pmsg)
	{
		Throw_Error("Failed to convert a message!!", __FUNCTION__) ;
		return ;
	}


	// Check map.
	if( pmsg->p_mapNum != g_pServerSystem->GetMapNum() ) return ;


	// Check check type.
	switch(pmsg->byCheckKind)
	{
	// Add start term.
	case e_SIEGE_BASEKIND_START_TERM : 
		{
			// Check add obj info.
			//090812 pdy ������ Agent�� Map�������� ��ȯObj �ε��������� �߸����ִ� ���� ���� 
			if(!Check_MyAddObjInfo( pmsg->p_byStepIdx, pmsg->p_dwObjectIdx )) return ;

			// Receive add obj info.
			CSiege_AddObj* pAddObj = NULL ;
			//090812 pdy ������ Agent�� Map�������� ��ȯObj �ε��������� �߸����ִ� ���� ���� 
			pAddObj = Get_AddObjIdx( pmsg->p_dwObjectIdx ) ;

			if(!pAddObj) return ;

			// Receive start term count.
			BYTE byStermCount = pAddObj->Get_StartTermCount() ;

			// Setting obj info.
			st_CHECK_OBJ_DIE objInfo ;

			objInfo.mapNum		= pmsg->check_mapNum ;
			objInfo.byObjectIdx	= pmsg->check_ObjIdx ;

			// Check start term count.
			if( byStermCount == 0)
			{
				// Setting info index.
				objInfo.byIdx = 0 ;

				// Setting term info.
				CSiegeTerm newTerm ;

				newTerm.Set_Index( 0 ) ;

				newTerm.Insert_CheckObjDie( &objInfo ) ;

				pAddObj->Insert_Sterm( pAddObj->Get_ChildCount(), &newTerm ) ;
			}
			else
			{
				CSiegeTerm* pTermInfo = NULL ;
				pTermInfo = pAddObj->Get_StartTermInfo( byStermCount-1 ) ;

				if( !pTermInfo )
				{
					Throw_Error( "Invalid add obj Info!!", __FUNCTION__ ) ;
					return ;
				}

				objInfo.byIdx = pTermInfo->Get_ChildCount() ;

				pTermInfo->Insert_CheckObjDie( &objInfo ) ;
			}
		}
		break ;

	case e_SIEGE_BASEKIND_END_TERM : 
		{
			// Check add obj info.
			//090812 pdy ������ Agent�� Map�������� ��ȯObj �ε��������� �߸����ִ� ���� ���� 
			if(!Check_MyAddObjInfo( pmsg->p_byStepIdx, pmsg->p_dwObjectIdx )) return ;

			// Receive add obj info.
			CSiege_AddObj* pAddObj = NULL ;
			//090812 pdy ������ Agent�� Map�������� ��ȯObj �ε��������� �߸����ִ� ���� ���� 
			pAddObj = Get_AddObjIdx( pmsg->p_dwObjectIdx ) ;

			if(!pAddObj) return ;

			// Receive start term count.
			BYTE byEtermCount = pAddObj->Get_EndTermCount() ;

			// Setting obj info.
			st_CHECK_OBJ_DIE objInfo ;

			objInfo.mapNum		= pmsg->check_mapNum ;
			objInfo.byObjectIdx	= pmsg->check_ObjIdx ;

			// Check start term count.
			if( byEtermCount == 0)
			{
				// Setting info index.
				objInfo.byIdx = 0 ;

				// Setting term info.
				CSiegeTerm newTerm ;

				newTerm.Set_Index( 0 ) ;

				newTerm.Insert_CheckObjDie( &objInfo ) ;

				pAddObj->Insert_ETerm( pAddObj->Get_ChildCount(), &newTerm ) ;
			}
			else
			{
				CSiegeTerm* pTermInfo = NULL ;
				pTermInfo = pAddObj->Get_EndTermInfo( byEtermCount-1 ) ;

				if( !pTermInfo )
				{
					Throw_Error( "Invalid add obj Info!!", __FUNCTION__ ) ;
					return ;
				}

				objInfo.byIdx = pTermInfo->Get_ChildCount() ;

				pTermInfo->Insert_CheckObjDie( &objInfo ) ;
			}
		}
		break ;

	default : break ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Add_CheckUseItem
//	DESC		: The function to add check routine for use item.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 01, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Add_CheckUseItem(void* pMsg) 
{
	// Check parameter.
	if(!pMsg)
	{
		Throw_Error("Invalid message parameter!!", __FUNCTION__) ;
		return ;
	}


	// Convert a message.
	MSG_SIEGERECALL_CHECK_USE_ITEM* pmsg = NULL ;
	pmsg = (MSG_SIEGERECALL_CHECK_USE_ITEM*) pMsg ;

	if(!pmsg)
	{
		Throw_Error("Failed to convert a message!!", __FUNCTION__) ;
		return ;
	}


	// Check map.
	if( pmsg->p_mapNum != g_pServerSystem->GetMapNum() ) return ;


	// Check check type.
	switch(pmsg->byCheckKind)
	{
	// Add start term.
	case e_SIEGE_BASEKIND_START_TERM : 
		{
			// Check add obj info.
			//090812 pdy ������ Agent�� Map�������� ��ȯObj �ε��������� �߸����ִ� ���� ���� 
			if(!Check_MyAddObjInfo( pmsg->p_byStepIdx, pmsg->p_dwObjectIdx )) return ;

			// Receive add obj info.
			CSiege_AddObj* pAddObj = NULL ;
			//090812 pdy ������ Agent�� Map�������� ��ȯObj �ε��������� �߸����ִ� ���� ���� 
			pAddObj = Get_AddObjIdx( pmsg->p_dwObjectIdx ) ;

			if(!pAddObj) return ;

			// Receive start term count.
			BYTE byStermCount = pAddObj->Get_StartTermCount() ;

			// Setting item info.
			st_CHECK_USE_ITEM itemInfo ;

			itemInfo.dwItemIdx	= pmsg->dwItemIdx ;
			itemInfo.wUseCount	= pmsg->wItemCount ;

			// Check start term count.
			if( byStermCount == 0)
			{
				// Setting info index.
				itemInfo.byIdx = 0 ;

				// Setting term info.
				CSiegeTerm newTerm ;

				newTerm.Set_Index( 0 ) ;

				newTerm.Insert_CheckUseItem( &itemInfo ) ;

				// Insert term info.
				pAddObj->Insert_Sterm( pAddObj->Get_ChildCount(), &newTerm ) ;
			}
			else
			{
				// Receive term info.
				CSiegeTerm* pTermInfo = NULL ;
				pTermInfo = pAddObj->Get_StartTermInfo( byStermCount-1 ) ;

				if( !pTermInfo )
				{
					Throw_Error( "Invalid add obj Info!!", __FUNCTION__ ) ;
					return ;
				}

				itemInfo.byIdx = pTermInfo->Get_ChildCount() ;

				// Insert info.
				pTermInfo->Insert_CheckUseItem( &itemInfo ) ;
			}
		}
		break ;

	case e_SIEGE_BASEKIND_END_TERM : 
		{
			// Check add obj info.
			//090812 pdy ������ Agent�� Map�������� ��ȯObj �ε��������� �߸����ִ� ���� ���� 
			if(!Check_MyAddObjInfo( pmsg->p_byStepIdx, pmsg->p_dwObjectIdx )) return ;

			// Receive add obj info.
			CSiege_AddObj* pAddObj = NULL ;
			//090812 pdy ������ Agent�� Map�������� ��ȯObj �ε��������� �߸����ִ� ���� ���� 
			pAddObj = Get_AddObjIdx( pmsg->p_dwObjectIdx ) ;

			if(!pAddObj) return ;

			// Receive end term count.
			BYTE byEtermCount = pAddObj->Get_EndTermCount() ;

			// Setting item info.
			st_CHECK_USE_ITEM itemInfo ;

			itemInfo.dwItemIdx	= pmsg->dwItemIdx ;
			itemInfo.wUseCount	= pmsg->wItemCount ;

			// Check end term count.
			if( byEtermCount == 0)
			{
				// Setting info index.
				itemInfo.byIdx = 0 ;

				// Setting term info.
				CSiegeTerm newTerm ;

				newTerm.Set_Index( 0 ) ;

				newTerm.Insert_CheckUseItem( &itemInfo ) ;

				// Insert term info.
				pAddObj->Insert_ETerm( pAddObj->Get_ChildCount(), &newTerm ) ;
			}
			else
			{
				// Receive term info.
				CSiegeTerm* pTermInfo = NULL ;
				pTermInfo = pAddObj->Get_EndTermInfo( byEtermCount-1 ) ;

				if( !pTermInfo )
				{
					Throw_Error( "Invalid add obj Info!!", __FUNCTION__ ) ;
					return ;
				}

				itemInfo.byIdx = pTermInfo->Get_ChildCount() ;

				// Insert info.
				pTermInfo->Insert_CheckUseItem( &itemInfo ) ;
			}
		}
		break ;

	default : break ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Add_Command
//	DESC		: he function to add command.
//	PROGRAMMER	: Yongs Lee
//	DATE		: JAugust 01, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Add_Command(void* pMsg)
{
	// Check parameter.
	if(!pMsg)
	{
		Throw_Error("Invalid message parameter!!", __FUNCTION__) ;
		return ;
	}


	// Convert a message.
	MSG_SIEGERECALL_COMMAND_DIE_RECALL_OBJ* pmsg = NULL ;
	pmsg = (MSG_SIEGERECALL_COMMAND_DIE_RECALL_OBJ*) pMsg ;

	if(!pmsg)
	{
		Throw_Error("Failed to convert a message!!", __FUNCTION__) ;
		return ;
	}


	// Check map.
	if( pmsg->p_mapNum != g_pServerSystem->GetMapNum() ) return ;


	// Check add obj info.
	//090812 pdy ������ Agent�� Map�������� ��ȯObj �ε��������� �߸����ִ� ���� ���� 
	if(!Check_MyAddObjInfo( pmsg->p_byStepIdx, pmsg->p_dwObjectIdx )) return ;


	// Receive add obj info.
	CSiege_AddObj* pAddObj = NULL ;
	//090812 pdy ������ Agent�� Map�������� ��ȯObj �ε��������� �߸����ִ� ���� ���� 
	pAddObj = Get_AddObjIdx( pmsg->p_dwObjectIdx ) ;

	if(!pAddObj) return ;
 

	// Receive command count.
	BYTE byCommandCount = pAddObj->Get_CommandCount() ;


	// Check command count.
	if( byCommandCount == 0)
	{
		// Setting command info.
		CSiegeCommand newCommand ;

		newCommand.Set_Index( 0 ) ;

		newCommand.Add_Die_Recall_Obj( &pmsg->dieRecallObj ) ;

		// Insert command info.
		pAddObj->Insert_Command( pAddObj->Get_ChildCount(), &newCommand ) ;
	}
	else
	{
		// Receive command info.
		CSiegeCommand* pCommand = NULL ;
		pCommand = pAddObj->Get_CommandInfo( byCommandCount-1 ) ;

		if( !pCommand )
		{
			Throw_Error( "Invalid command Info!!", __FUNCTION__ ) ;
			return ;
		}

		pCommand->Add_Die_Recall_Obj( &pmsg->dieRecallObj ) ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Add_ObjToDB
//	DESC		: The function to add object info to database.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 11, 2008
//-------------------------------------------------------------------------------------------------
//void CSiegeRecallMgr::Add_ObjToDB(WORD wTheme, WORD wMapNum, BYTE byStepIdx, WORD wAddObjIdx, BYTE IsParent, BYTE byChildIdx)
void CSiegeRecallMgr::Add_ObjToDB(WORD wTheme, WORD wMapNum, WORD wRecallMap, BYTE byStepIdx, 
								  BYTE byAddObjIdx, BYTE byComKind, BYTE byComIdx, BYTE byRecallStep )
{
	// �ӽ� ���۸� �����Ѵ�.
	char txt[128] = {0, } ;


	// �������� �ۼ��Ѵ�.
	sprintf(txt, "EXEC  %s %d, %d, %d, %d, %d, %d, %d, %d, %d",	MP_SIEGERECALL_INSERT, 
		wTheme, wMapNum, wRecallMap, byStepIdx, byAddObjIdx, byComKind, byComIdx, byRecallStep, 200) ;


	// �������� �����Ѵ�.
	g_DB.Query(eQueryType_FreeQuery, eSiegeREcallInsert, 0, txt) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Remove_ObjFromDB
//	DESC		: The function to remove object info from database.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 11, 2008
//-------------------------------------------------------------------------------------------------
//void CSiegeRecallMgr::Remove_ObjFromDB(WORD wTheme, WORD wMapNum, BYTE byStepIdx,  WORD wAddObjIdx, BYTE IsParent, BYTE byChildIdx)
void CSiegeRecallMgr::Remove_ObjFromDB(WORD wTheme, WORD wMapNum, WORD wRecallMap, BYTE byStepIdx,  
									   BYTE byAddObjIdx, BYTE byComKind, BYTE byComIndex, BYTE byRecallStep )
{
	// �ӽ� ���۸� �����Ѵ�.
	char txt[128] = {0, } ;


	// �������� �ۼ��Ѵ�.
	//sprintf(txt, "EXEC  %s %d, %d, %d, %d, %d, %d, %d, %d",	MP_SIEGERECALL_REMOVE, FALSE, wTheme, wMapNum, wRecallMap, byStepIdx, byAddObjIdx, byComKind, byComIndex) ;
	sprintf(txt, "EXEC  %s %d, %d, %d, %d, %d, %d, %d, %d, %d",	MP_SIEGERECALL_INSERT, 
		wTheme, wMapNum, wRecallMap, byStepIdx, byAddObjIdx, byComKind, byComIndex, byRecallStep, m_wRecallCount) ;


	// �������� �����Ѵ�.
	//g_DB.Query(eQueryType_FreeQuery, eSiegeRecallRemove, 0, txt, 0) ;
	g_DB.Query(eQueryType_FreeQuery, eSiegeREcallInsert, 0, txt, 0) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Request_ObjFromDB
//	DESC		: The function to load object info from database.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 12, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Request_ObjFromDB()
{
	// ��ȯ ������Ʈ ������ ���������� ��� ����.
	if( m_mAddObj.size() != 0 )
	{
		M_MADDOBJ::iterator it ;

		for( BYTE byCount = 0 ; byCount < m_mAddObj.size() ; ++byCount )
		{
			it = m_mAddObj.find(byCount) ;
			if( it == m_mAddObj.end() ) continue ;

			m_mAddObj.erase(it) ;
		}
	}


	// 080919 LYW --- SiegeRecallMgr : ������ ������ �ʼ������� ����.
	// ������Ʈ ������ DB�� ��û�Ѵ�.
	MSG_WORD msg ;

	msg.Category	= MP_SIEGERECALL ;
	msg.Protocol	= MP_SIEGERECALL_REQUEST_OBJINFO_SYN ;

	// State �� ��Ƽ� ���� ���¸� üũ�Ͽ� ������Ʈ�� �˷��ش�.
	msg.dwObjectID	= SIEGEWARFAREMGR->GetState() ;
	msg.wData		= g_pServerSystem->GetMapNum() ;

	g_Network.Send2AgentServer( (char*)&msg, sizeof(MSG_WORD) ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Check_MyAddObjInfo
//	DESC		: The function to check addObjInfo.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 04, 2008
//-------------------------------------------------------------------------------------------------
//090812 pdy ������ Agent�� Map�������� ��ȯObj �ε��������� �߸����ִ� ���� ���� 
BYTE CSiegeRecallMgr::Check_MyAddObjInfo(BYTE byStepNum, DWORD dwObjIdx)
{
	// Check mapNum.
	M_MADDOBJ::iterator it ;

	for( it = m_mAddObj.begin() ; it != m_mAddObj.end() ; ++it )
	{
		if( it->second.Get_ParentStepIdx() != byStepNum ) continue ;

		if( it->second.Get_ObjInfo()->dwObjectIdx != dwObjIdx ) continue ;

		return TRUE ;
	}


	// Return false.
	return FALSE ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Check_ObjectDie
//	DESC		: The function to check object die.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 04, 2008
//-------------------------------------------------------------------------------------------------
//void CSiegeRecallMgr::Check_ObjectDie(DWORD dwObjectID, BYTE IsCheatMode)
void CSiegeRecallMgr::Check_ObjectDie(CObject* pKiller, DWORD dwObjectID, BYTE IsCheatMode)
{
	// Killer�� NULL�� ���� �� ������ �����Ѵ�.

	BYTE IsSiegeWarfareZone = BYTE( SIEGEWARFAREMGR->IsSiegeWarfareZone() ) ;
	if( !IsSiegeWarfareZone ) return ;


	// 081215 LYW --- SiegeRecallMgr : ���� Ÿ���� ó���� ���� �ʴ´�.(�������� ������Ʈ�� ��ȯ���� ����)
	if( SIEGEWARFAREMGR->GetMapInfo() == eNeraCastle_Village || 
		SIEGEWARFAREMGR->GetMapInfo() >= eSiegeWarfare_Map_Max )
		return ;

	// 081215 LYW --- SiegeRecallMgr : ���� Ÿ���� ĳ����Ʈ�� �ƴ϶��, return ó���� �Ѵ�.
	CMonster* pDelMonster = (CMonster*)g_pUserTable->FindUser( dwObjectID ) ;
	if( !pDelMonster ) return ;
	// ĳ����Ʈ�� ���� Ÿ���� 352~367�̴�.
	WORD wMonsterKind = pDelMonster->GetMonsterKind() ;
	if( wMonsterKind < 352 || wMonsterKind > 367 ) return ;



	// 081028 LYW --- SiegeRecallMgr : ���� ���� ó�� �Լ��� ȣ��Ǿ����� �α׸� �����.
	if( IsSiegeWarfareZone )
	{
		char szLogCall[1024] = {0, } ;
		DWORD dwKillerID = 0 ;
		if( pKiller ) dwKillerID = pKiller->GetID() ;

		sprintf( szLogCall, "Call - Check_ObjectDie : Player : %d Monster : %u IsCheatMode : %d\n", 
			dwKillerID, dwObjectID, IsCheatMode ) ;
		WriteCatapultLog( szLogCall ) ;
	}

	//if( pKiller->GetObjectKind() == eObjectKind_Player )
	if( pKiller && pKiller->GetObjectKind() == eObjectKind_Player )	// 081212 NYJ - ų�������Ͱ� ��ȿ�� ��츸 üũ
	{
		CPlayer* player = ( CPlayer* )pKiller;

		// 081027 LUJ, �α�
		InsertLogSiege(
			eSiegeLogDestoryCatapult,
			player->GetGuildIdx(),
			player->GetID() );
	}
	else
	{
		// 081027 LUJ, �α�
		InsertLogSiege(
			eSiegeLogDestoryCatapult,
			0,
			0,
			"by no player" );
	}

	// Declare temp variables.
	st_SIEGEOBJ* pObjInfo ;

	CSiegeTerm* pSTerm ;
	CSiegeTerm* pETerm ;
	CSiegeCommand* pCommand ;

	BYTE bySTermCount ;
	BYTE byETermCount ;
	BYTE byCommandCount ;

	M_MADDOBJ::iterator it ;


	// 081028 LYW --- SiegeRecallMgr : ���� ���� ó�� �Լ��� ȣ��Ǿ����� �α׸� �����.
	if( IsSiegeWarfareZone )
	{
		char szCheckBefor[1024] = {0, } ;
		sprintf( szCheckBefor, "Befor Count - %d\n", m_mAddObj.size() ) ;
		WriteCatapultLog( szCheckBefor ) ;
	}

	// Check add object.
	for( it = m_mAddObj.begin() ; it != m_mAddObj.end() ; ++it )
	{
		// Receive object info.
		pObjInfo = NULL ;
		pObjInfo = it->second.Get_ObjInfo() ;


		// Check object info.
		if(!pObjInfo) continue ;


		// Check object index.
		//090812 pdy ������ Agent�� Map�������� ��ȯObj �ε��������� �߸����ִ� ���� ���� 
		if( it->second.Get_MonsterID() != dwObjectID )
			continue;


		// Check start term.
		bySTermCount = 0 ;
		bySTermCount = it->second.Get_StartTermCount() ;

		for(BYTE bySCount = 0 ; bySCount < bySTermCount ; ++bySCount)
		{
			pSTerm = NULL ;
			pSTerm = it->second.Get_StartTermInfo( bySCount ) ;

			if( !pSTerm ) continue ;

			// Send start term info.
			Send_StartTermInfo( it->second.Get_ParentStepIdx(), pSTerm, bySCount ) ;
		}


		// Check end term.
		byETermCount = 0 ;
		byETermCount = it->second.Get_EndTermCount() ;

		for(BYTE byECount = 0 ; byECount < byETermCount ; ++byECount)
		{
			pETerm = NULL ;
			pETerm = it->second.Get_EndTermInfo( byECount ) ;

			if( !pETerm ) continue ;

			// Send start term info.
			Send_EndTermInfo( it->second.Get_ParentStepIdx(), pETerm, byECount ) ;
		}


		// Check command.
		byCommandCount = 0 ;
		byCommandCount = it->second.Get_CommandCount() ;

		for(BYTE byComCount = 0 ; byComCount < byCommandCount ; ++byComCount)
		{
			pCommand = NULL ;
			pCommand = it->second.Get_CommandInfo( byComCount ) ;

			if( !pCommand ) continue ;

			// Send start term info.
			//090812 pdy ������ Agent�� Map�������� ��ȯObj �ε��������� �߸����ִ� ���� ���� 
			Send_CommandInfo( it->second.Get_ParentStepIdx(), pCommand, it->second.Get_ObjInfo()->byAddObjIdx ) ;
		}

		if( IsSiegeWarfareZone )
		{
			WriteCatapultLog( "Ready to delete!\n" ) ;
		}


		// Remove obj info from database.
		if(pObjInfo->byComKind == e_CK_ADD_OBJECT)
		{
			Remove_ObjFromDB( pObjInfo->wThemeIdx, pObjInfo->mapNum, g_pServerSystem->GetMapNum(), 
			pObjInfo->byStepIdx, pObjInfo->byAddObjIdx, pObjInfo->byComKind, pObjInfo->byComIndex, 2) ;

			// 081028 LYW --- SiegeRecallMgr : ĳ����Ʈ ���� ���� �޽��� �α׸� �����.
			DWORD dwKillerID = 0 ;
			char killerName[ MAX_NAME_LENGTH+1 ] = {0, } ;

			if( pKiller )
			{
				dwKillerID = pKiller->GetID() ;
				SafeStrCpy( killerName, pKiller->GetObjectName(), MAX_NAME_LENGTH ) ;
			}
			else
			{
				SafeStrCpy( killerName, "Use Cheat Command!!\n", MAX_NAME_LENGTH ) ;
			}

			if( IsSiegeWarfareZone )
			{
				char szLog[1024] = {0, } ;
				sprintf(szLog, "Delete Catapult - %d %d %d %d %d %d %d %d %d %s %u\n", pObjInfo->wThemeIdx, 
					pObjInfo->mapNum, g_pServerSystem->GetMapNum(), pObjInfo->byStepIdx, pObjInfo->byAddObjIdx, 
					pObjInfo->byComKind, pObjInfo->byComIndex, 2, dwKillerID, killerName, pObjInfo->dwObjectIdx ) ;
				WriteCatapultLog( szLog ) ;
			}

			// 081029 LYW --- SiegeRecallMgr : ĳ����Ʈ ��ȯ �ȵǴ� ���� ����.
			// ġƮŰ �ΰ��...  
			if( IsCheatMode )
			{
				//090812 pdy ������ Agent�� Map�������� ��ȯObj �ε��������� �߸����ִ� ���� ���� 
				g_pServerSystem->RemoveMonster( it->second.Get_MonsterID() ) ;
			}

			// Delete obj info.
			m_mAddObj.erase( it ) ;
		}
		else
		{
			Remove_ObjFromDB( pObjInfo->wThemeIdx, pObjInfo->wParentMap, g_pServerSystem->GetMapNum(), 
			pObjInfo->byStepIdx, pObjInfo->byAddObjIdx, pObjInfo->byComKind, pObjInfo->byComIndex, 4) ;

			// 081028 LYW --- SiegeRecallMgr : ĳ����Ʈ ���� ���� �޽��� �α׸� �����.
			DWORD dwKillerID = 0 ;
			char killerName[ MAX_NAME_LENGTH+1 ] = {0, } ;

			if( pKiller )
			{
				dwKillerID = pKiller->GetID() ;
				SafeStrCpy( killerName, pKiller->GetObjectName(), MAX_NAME_LENGTH ) ;
			}
			else
			{
				SafeStrCpy( killerName, "Use Cheat Command!!\n", MAX_NAME_LENGTH ) ;
			}

			// 081028 LYW --- SiegeRecallMgr : ĳ����Ʈ ��ȯ ���� �޽��� �α׸� �����.
			if( IsSiegeWarfareZone )
			{
				char szLog[1024] = {0, } ;
				sprintf(szLog, "Delete Catapult - %d %d %d %d %d %d %d %d %d %s %u\n", pObjInfo->wThemeIdx, 
					pObjInfo->wParentMap, g_pServerSystem->GetMapNum(), pObjInfo->byStepIdx, pObjInfo->byAddObjIdx, 
					pObjInfo->byComKind, pObjInfo->byComIndex, 4, dwKillerID, killerName, pObjInfo->dwObjectIdx ) ;
				WriteCatapultLog( szLog ) ;
			}

			// 081029 LYW --- SiegeRecallMgr : ĳ����Ʈ ��ȯ �ȵǴ� ���� ����.
			// ġƮŰ �ΰ��...  
			if( IsCheatMode )
			{
				//090812 pdy ������ Agent�� Map�������� ��ȯObj �ε��������� �߸����ִ� ���� ���� 
				g_pServerSystem->RemoveMonster( it->second.Get_MonsterID() ) ;
			}

			// Delete obj info.
			m_mAddObj.erase( it ) ;
		}

		break ;
	}


	// 081028 LYW --- SiegeRecallMgr : ���� ���� ó�� �Լ��� ȣ��Ǿ����� �α׸� �����.
	if( IsSiegeWarfareZone )
	{
		char szCheckAfter[1024] = {0, } ;
		sprintf( szCheckAfter, "After Count - %d\n", m_mAddObj.size() ) ;
		WriteCatapultLog( szCheckAfter ) ;
	}

	// ���� ��ȯ �� �����̰�, ��ȯ �� ���� ��� �������, ���ͽõ� ������ ������ ���·� ���¸� �����Ѵ�.
	if( SIEGEWARFAREMGR->GetState() == eSiegeWarfare_State_Start && m_mAddObj.size() == 0 )
	{
		SIEGEWARFAREMGR->SetState(eSiegeWarfare_State_First) ;

		if( IsSiegeWarfareZone )
		{
			char szLog[1024] = {0, } ;
			sprintf( szLog, "Ready To WaterSeed Use - %d", g_pServerSystem->GetMapNum() ) ;
			WriteCatapultLog( szLog ) ;
		}
	}

	// 081222 LYW --- SiegeRecallMgr : ��� ���͸� ��Ҵ��� db�� ���� ��û�� �Ѵ�.
	RequestKillCountToDB() ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Send_StartTermInfo
//	DESC		: The function to send start term info.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 04, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Send_StartTermInfo(BYTE byParentStepIdx, CSiegeTerm* pTerm, BYTE byIdx) 
{
	// Check parameter.
	if(!pTerm)
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	// Check term kind.
	BYTE byKind ;
	BYTE byChildCount = pTerm->Get_ChildCount() ;

	for( BYTE byCount = 0 ; byCount < byChildCount ; ++byCount )
	{
		byKind = e_TERM_NONE ;
		byKind = pTerm->Get_ChildKind( byCount ) ;

		switch( byKind )
		{
		case e_TERM_DIE_OBJ :
			{
				// Notice check obj die by start term.
				st_CHECK_OBJ_DIE* pInfo = NULL ;
				pInfo = pTerm->Get_ChkObjDieInfo(byIdx) ;

				if( !pInfo )
				{
					Throw_Error("Invalid die obj info!!", __FUNCTION__) ;
					return ;
				}

				pTerm->Remove_CheckObjDie( pInfo ) ;
			}
			break ;

		case e_TERM_USE_ITEM : 
			{
				// Notice check use item by start term.
				st_CHECK_USE_ITEM* pInfo = NULL ;
				pInfo = pTerm->Get_ChkUseItemInfo(byIdx) ;

				if(!pInfo)
				{
					Throw_Error("Invalid use item info!!", __FUNCTION__) ;
					return ;
				}

				pTerm->Decrease_UseItemCount(pInfo) ;
			}
			break ;

		case e_TERM_NONE : 
		default : break ;
		}
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Send_EndTermInfo
//	DESC		: The function to send end term info.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 04, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Send_EndTermInfo(BYTE byParentStepIdx, CSiegeTerm* pTerm, BYTE byIdx) 
{
	// Check parameter.
	if(!pTerm)
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	// Check term kind.
	BYTE byKind ;
	BYTE byChildCount = pTerm->Get_ChildCount() ;

	for( BYTE byCount = 0 ; byCount < byChildCount ; ++byCount )
	{
		byKind = e_TERM_NONE ;
		byKind = pTerm->Get_ChildKind( byCount ) ;

		switch( byKind )
		{
		case e_TERM_DIE_OBJ :
			{
				// Notice check obj die by start term.
				st_CHECK_OBJ_DIE* pInfo = NULL ;
				pInfo = pTerm->Get_ChkObjDieInfo(byIdx) ;

				if( !pInfo )
				{
					Throw_Error("Invalid die obj info!!", __FUNCTION__) ;
					return ;
				}

				pTerm->Remove_CheckObjDie( pInfo ) ;

				//if( pTerm->Get_CheckObjDieCount() == 0 )
				//{
				//	// Setting message.
				//	MSG_WORD3 msg ;

				//	msg.Category	= MP_SIEGERECALL ;
				//	msg.Protocol	= MP_SIEGERECALL_NOTICE_ETERM_CHKOBJDIE_COMPLETE_SYN ;

				//	msg.dwObjectID	= 0 ;

				//	msg.wData1		= g_pServerSystem->GetMapNum() ;
				//	msg.wData2		= (WORD)byParentStepIdx ;
				//	msg.wData3		= pTerm->Get_ParentIdx() ;

				//	// Send message.
				//	g_Network.Send2AgentServer( (char*)&msg, sizeof(msg) ) ;
				//}
			}
			break ;

		case e_TERM_USE_ITEM : 
			{
				// Notice check use item by start term.
				st_CHECK_USE_ITEM* pInfo = NULL ;
				pInfo = pTerm->Get_ChkUseItemInfo(byIdx) ;

				if(!pInfo)
				{
					Throw_Error("Invalid use item info!!", __FUNCTION__) ;
					return ;
				}

				pTerm->Decrease_UseItemCount(pInfo) ;

				//if( pTerm->Get_CheckUseItemCount() == 0 )
				//{
				//	// Setting message.
				//	MSG_WORD3 msg ;

				//	msg.Category	= MP_SIEGERECALL ;
				//	msg.Protocol	= MP_SIEGERECALL_NOTICE_ETERM_USEITEM_COMPLETE_SYN ;

				//	msg.dwObjectID	= 0 ;

				//	msg.wData1		= g_pServerSystem->GetMapNum() ;
				//	msg.wData2		= (WORD)byParentStepIdx ;
				//	msg.wData3		= pTerm->Get_ParentIdx() ;

				//	// Send message.
				//	g_Network.Send2AgentServer( (char*)&msg, sizeof(msg) ) ;
				//}
			}
			break ;

		case e_TERM_NONE : 
		default : break ;
		}
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Send_CommandInfo
//	DESC		: The function to send command info.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 04, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Send_CommandInfo(BYTE byParentStepIdx, CSiegeCommand* pCommand, BYTE byIdx) 
{
	// Check parameter.
	if(!pCommand)
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	// Check term kind.
	BYTE byKind ;
	BYTE byChildCount = pCommand->Get_ChildCount() ;

	for( BYTE byCount = 0 ; byCount < byChildCount ; ++byCount )
	{
		byKind = e_TERM_NONE ;
		byKind = pCommand->Get_ChildKind( byCount ) ;

		switch( byKind )
		{
		case e_COMMAND_RECALL_DIE_OBJ :
			{
				// Notice check obj die by start term.

				st_DIE_RECALL_OBJ* pRecallObj = NULL ;
				pRecallObj = pCommand->Get_DieRecallObjInfo(byCount) ;

				if(!pRecallObj) return ;

				MSG_WORD3 msg ;

				msg.Category	= MP_SIEGERECALL ;
				msg.Protocol	= MP_SIEGERECALL_NOTICE_DIERECALLOBJ_READY_SYN ;

				msg.dwObjectID	= 0 ;

				msg.wData1		= g_pServerSystem->GetMapNum() ;
				msg.wData2		= (WORD)byParentStepIdx ;
				msg.wData3		= byIdx ;

				// Send message.
				g_Network.Send2AgentServer( (char*)&msg, sizeof(msg) ) ;
			}
			break ;

		default : break ;
		}
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Remove_AllMapAllObj
//	DESC		: ��� �� / ��� ������Ʈ�� �����ؾ� �ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 06, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Remove_AllMapAllObj( void* pMsg )
{
	BYTE IsSiegeWarfareZone = BYTE( SIEGEWARFAREMGR->IsSiegeWarfareZone() ) ;
	if( !IsSiegeWarfareZone ) return ;

	// 081028 LYW --- SiegeRecallMgr : ĳ����Ʈ ���� ���� �޽��� �α׸� �����.
	if( IsSiegeWarfareZone )
	{
		WriteCatapultLog( "Call - Remove_AllMapAllObj" ) ;
	}

	// �Լ� ���� Ȯ��.
	if(!pMsg)
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	// ���� �޽����� ��ȯ.
	MSGBASE* pmsg = NULL ;
	pmsg = (MSGBASE*)pMsg ;

	if(!pmsg)
	{
		Throw_Error("Failed to convert a message!!", __FUNCTION__) ;
		return ;
	}

	if( IsSiegeWarfareZone )
	{
		char szCheckBefor[1024] = {0, } ;
		sprintf( szCheckBefor, "Befor Count - %d\n", m_mAddObj.size() ) ;
		WriteCatapultLog( szCheckBefor ) ;
	}

	for(M_MADDOBJ::iterator it = m_mAddObj.begin() ; it != m_mAddObj.end() ; ++it )
	{
		st_SIEGEOBJ* pObjInfo = it->second.Get_ObjInfo() ;

		if( !pObjInfo ) continue ;

		if( pObjInfo->dwObjectIdx == 0 )
		{
			Throw_Error("Invalid object index!!", __FUNCTION__) ;
			return ;
		}

		if( pObjInfo->byComKind == e_CK_ADD_OBJECT )
		{
			Remove_ObjFromDB( pObjInfo->wThemeIdx, pObjInfo->mapNum, g_pServerSystem->GetMapNum(), 
			pObjInfo->byStepIdx, pObjInfo->byAddObjIdx, pObjInfo->byComKind, pObjInfo->byComIndex, 0) ;


			// 081028 LYW --- SiegeRecallMgr : ĳ����Ʈ ���� ���� �޽��� �α׸� �����.
			if( IsSiegeWarfareZone )
			{
				char szLog[1024] = {0, } ;
				sprintf(szLog, "Delete Catapult - %d %d %d %d %d %d %d %d %u %s\n", pObjInfo->wThemeIdx, 
					pObjInfo->mapNum, g_pServerSystem->GetMapNum(), pObjInfo->byStepIdx, pObjInfo->byAddObjIdx, 
					pObjInfo->byComKind, pObjInfo->byComIndex, 0,  pObjInfo->dwObjectIdx, "Use Cheat Command!!" ) ;
				WriteCatapultLog( szLog ) ;
			}
		}
		else
		{
			Remove_ObjFromDB( pObjInfo->wThemeIdx, pObjInfo->wParentMap, g_pServerSystem->GetMapNum(), 
			pObjInfo->byStepIdx, pObjInfo->byAddObjIdx, pObjInfo->byComKind, pObjInfo->byComIndex, 0) ;

			// 081028 LYW --- SiegeRecallMgr : ĳ����Ʈ ���� ���� �޽��� �α׸� �����.
			if( IsSiegeWarfareZone )
			{
				char szLog[1024] = {0, } ;
				sprintf(szLog, "Delete Catapult - %d %d %d %d %d %d %d %d %u %s\n", pObjInfo->wThemeIdx, 
					pObjInfo->mapNum, g_pServerSystem->GetMapNum(), pObjInfo->byStepIdx, pObjInfo->byAddObjIdx, 
					pObjInfo->byComKind, pObjInfo->byComIndex, 0,  pObjInfo->dwObjectIdx, "Use Cheat Command!!" ) ;
				WriteCatapultLog( szLog ) ;
			}
		}

		g_pServerSystem->RemoveMonster( pObjInfo->dwObjectIdx ) ;
	}

	m_mAddObj.clear() ;

	// 081028 LYW --- SiegeRecallMgr : ���� ���� ó�� �Լ��� ȣ��Ǿ����� �α׸� �����.
	if( IsSiegeWarfareZone )
	{
		char szCheckAfter[1024] = {0, } ;
		sprintf( szCheckAfter, "After Count - %d\n", m_mAddObj.size() ) ;
		WriteCatapultLog( szCheckAfter ) ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Remove_AllMapSpecifyObj
//	DESC		: ��� �� / ���� �� ������Ʈ�� �����ؾ� �ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 06, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Remove_AllMapSpecifyObj( void* pMsg )
{
	// �Լ� ���� Ȯ��.
	if(!pMsg)
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	// ���� �޽����� ��ȯ.
	MSG_WORD* pmsg = NULL ;
	pmsg = (MSG_WORD*)pMsg ;

	if(!pmsg)
	{
		Throw_Error("Failed to convert a message!!", __FUNCTION__) ;
		return ;
	}


	// ��ȯ �� ������Ʈ�� Ȯ���Ѵ�.
	M_MADDOBJ::iterator it ;

	st_SIEGEOBJ* pObjInfo ;

	CObject* pObject ;

	while( m_mAddObj.size() != 0 )
	{
		it = m_mAddObj.find( BYTE(m_mAddObj.size() - 1) ) ;

		if( it == m_mAddObj.end() ) break ;

		pObjInfo = NULL ;
		pObjInfo = it->second.Get_ObjInfo() ;

		if( !pObjInfo ) continue ;

		pObject = NULL ;
		pObject = g_pUserTable->FindUser(pObjInfo->dwObjectIdx) ;

		if(!pObject) continue ;

		if( pObject->GetObjectKind() != pmsg->wData ) continue ;

		if( pObjInfo->dwObjectIdx == 0 )
		{
			Throw_Error("Invalid object index!!", __FUNCTION__) ;
			return ;
		}

		g_pServerSystem->RemoveMonster( pObjInfo->dwObjectIdx ) ;

		m_mAddObj.erase( it ) ;
	}
}






//-------------------------------------------------------------------------------------------------
//	NAME		: Remove_SpecifyMapAllObj
//	DESC		: ���� �� �� / ��� ������Ʈ�� �����ؾ� �ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 06, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Remove_SpecifyMapAllObj( void* pMsg ) 
{
	// �Լ� ���� Ȯ��.
	if(!pMsg)
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	// ���� �޽����� ��ȯ.
	MSG_WORD* pmsg = NULL ;
	pmsg = (MSG_WORD*)pMsg ;

	if(!pmsg)
	{
		Throw_Error("Failed to convert a message!!", __FUNCTION__) ;
		return ;
	}


	// �� �ε����� Ȯ���Ѵ�.
	if( g_pServerSystem->GetMapNum() != pmsg->wData ) return ;


	// ��ȯ �� ������Ʈ�� Ȯ���Ѵ�.
	M_MADDOBJ::iterator it ;
	st_SIEGEOBJ* pObjInfo ;

	while( m_mAddObj.size() != 0 )
	{
		it = m_mAddObj.find( BYTE( m_mAddObj.size() - 1) ) ;

		if( it == m_mAddObj.end() ) break ;

		pObjInfo = NULL ;
		pObjInfo = it->second.Get_ObjInfo() ;

		if( !pObjInfo ) continue ;

		if( pObjInfo->dwObjectIdx == 0 )
		{
			Throw_Error("Invalid object index!!", __FUNCTION__) ;
			return ;
		}

		g_pServerSystem->RemoveMonster( pObjInfo->dwObjectIdx ) ;

		m_mAddObj.erase( it ) ;
	}
}






//-------------------------------------------------------------------------------------------------
//	NAME		: Remove_SpecifyMapSpecifyObj
//	DESC		: ���� �� �� / ���� �� ������Ʈ�� �����ؾ� �ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 06, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Remove_SpecifyMapSpecifyObj( void* pMsg ) 
{
	// �Լ� ���� Ȯ��.
	if(!pMsg)
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	// ���� �޽����� ��ȯ.
	MSG_WORD2* pmsg = NULL ;
	pmsg = (MSG_WORD2*)pMsg ;

	if(!pmsg)
	{
		Throw_Error("Failed to convert a message!!", __FUNCTION__) ;
		return ;
	}


	// �� �ε����� Ȯ���Ѵ�.
	if( g_pServerSystem->GetMapNum() != pmsg->wData1 ) return ;


	// ��ȯ �� ������Ʈ�� Ȯ���Ѵ�.
	M_MADDOBJ::iterator it ;

	st_SIEGEOBJ* pObjInfo ;

	CObject* pObject ;

	while( m_mAddObj.size() != 0 )
	{
		it = m_mAddObj.find( BYTE( m_mAddObj.size() - 1 ) ) ;

		if( it == m_mAddObj.end() ) break ;

		pObjInfo = NULL ;
		pObjInfo = it->second.Get_ObjInfo() ;

		if( !pObjInfo ) continue ;

		pObject = NULL ;
		pObject = g_pUserTable->FindUser(pObjInfo->dwObjectIdx) ;

		if(!pObject) continue ;

		if( pObject->GetObjectKind() != pmsg->wData2 ) continue ;

		if( pObjInfo->dwObjectIdx == 0 )
		{
			Throw_Error("Invalid object index!!", __FUNCTION__) ;
			return ;
		}

		g_pServerSystem->RemoveMonster( pObjInfo->dwObjectIdx ) ;

		m_mAddObj.erase( it ) ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Set_LoadSummonFlag
//	DESC		: Setting flag.
//	PROGRAMMER	: Yongs Lee
//	DATE		: September 01, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Set_LoadSummonFlag( void* pMsg )
{
	// �Լ� ���� Ȯ��.
	if(!pMsg)
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	// ���� �޽����� ��ȯ.
	MSG_WORD* pmsg = NULL ;
	pmsg = (MSG_WORD*)pMsg ;

	if(!pmsg)
	{
		Throw_Error("Failed to convert a message!!", __FUNCTION__) ;
		return ;
	}


	// �� �ε����� Ȯ���Ѵ�.
	if( g_pServerSystem->GetMapNum() != pmsg->wData ) return ;


	// �ε� �Ϸ� ������ �Ѵ�.
	SIEGEWARFAREMGR->Set_LoadSummonFlag(TRUE) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Throw_Error
//	DESC		: The function to process error message.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 28, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Throw_Error(char* szErr, char* szCaption)
{
	// Check parameter of this function.
	if(!szErr || !szCaption) return ; 


	// Check err string size.
	if(strlen(szErr) <= 1)
	{
#ifdef _USE_ERRBOX_
		MessageBox( NULL, "Invalid err string size!!", __FUNCTION__, MB_OK ) ;
#else
		char tempStr[257] = {0, } ;

		SafeStrCpy( tempStr, __FUNCTION__, 256 ) ;
		strcat( tempStr, " - " ) ;
		strcat( tempStr, "Invalid err string size!!" ) ;
		WriteLog( tempStr ) ;
#endif // _USE_ERRBOX_
	}


	// Check caption string size.
	if(strlen(szCaption) <= 1)
	{
#ifdef _USE_ERRBOX_
		MessageBox( NULL, "Invalid caption string size!!", __FUNCTION__, MB_OK ) ;
#else
		char tempStr[257] = {0, } ;

		SafeStrCpy( tempStr, __FUNCTION__, 256 ) ;
		strcat( tempStr, " - " ) ;
		strcat( tempStr, "Invalid caption string size!!" ) ;
		WriteLog( tempStr ) ;
#endif // _USE_ERRBOX_
	}


	// Print a err message.
#ifdef _USE_ERRBOX_
	MessageBox( NULL, szErr, szCaption, MB_OK) ;
#else
	char tempStr[257] = {0, } ;

	SafeStrCpy(tempStr, szCaption, 256) ;
	strcat(tempStr, " - ") ;
	strcat(tempStr, szErr) ;
	WriteLog(tempStr) ;
#endif // _USE_ERRBOX_
}





//-------------------------------------------------------------------------------------------------
//	NAME		: WriteLog
//	DESC		: The function to create a error log for siege recall manager.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 28, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::WriteLog(char* pMsg)
{
	SYSTEMTIME time ;
	GetLocalTime(&time) ;

	TCHAR szTime[_MAX_PATH] = {0, } ;
	sprintf(szTime, "%04d-%02d-%02d %02d:%02d:%02d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond) ;

	TCHAR szFile[_MAX_PATH] = {0, } ;
	sprintf( szFile, "Log/Map%d-SiegeRecallMgr.log", g_pServerSystem->GetMapNum() ) ;

	FILE *fp = fopen(szFile, "a+") ;
	if (fp)
	{
		fprintf(fp, "%s [%s]\n", pMsg,  szTime) ;
		fclose(fp) ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: WriteCatapultLog
//	DESC		: The function to create a error log for catapult.
//	PROGRAMMER	: Yongs Lee
//	DATE		: October 28, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::WriteCatapultLog( char* pMsg )
{
	// �Լ� ���� Ȯ��.
	if( !pMsg ) return ;


	// �α׸� �����.
	SYSTEMTIME time ;
	GetLocalTime(&time) ;

	TCHAR szTime[_MAX_PATH] = {0, } ;
	sprintf(szTime, "%04d-%02d-%02d %02d:%02d:%02d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond) ;

	TCHAR szFile[_MAX_PATH] = {0, } ;
	sprintf( szFile, "Log/Siege_Catapult_%d_%04d%02d%02d.log", g_pServerSystem->GetMapNum(), time.wYear, time.wMonth, time.wDay ) ;

	FILE *fp = fopen(szFile, "a+") ;
	if (fp)
	{
		fprintf(fp, "[%s]\t%s", szTime, pMsg) ;
		fclose(fp) ;
	}
}

void CSiegeRecallMgr::EndSiegeWar_ClearRecallInfo()
{
	DWORD dwRemoveObjectID ;
	M_MADDOBJ::iterator it ;

	for( it = m_mAddObj.begin() ; it != m_mAddObj.end() ; ++it )
	{
		dwRemoveObjectID = 0 ;
		//090812 pdy ������ Agent�� Map�������� ��ȯObj �ε��������� �߸����ִ� ���� ���� 
		dwRemoveObjectID =	it->second.Get_MonsterID() ;

		g_pServerSystem->RemoveMonster( dwRemoveObjectID ) ;
	}

	m_mAddObj.clear() ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: IsRecallMonster
//	DESC		: ���ڷ� �Ѿ�� ���̵��� ���Ͱ� ��ȯ �� �������� ��ȯ�ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: October 6, 2008
//-------------------------------------------------------------------------------------------------
BYTE CSiegeRecallMgr::IsRecallMonster(DWORD dwID) 
{
	st_SIEGEOBJ* pObjInfo ;
	M_MADDOBJ::iterator it ;

	for( it = m_mAddObj.begin() ; it != m_mAddObj.end() ; ++it )
	{
		pObjInfo = NULL ;
		pObjInfo = it->second.Get_ObjInfo() ; 

		if( !pObjInfo ) continue ;

		if( pObjInfo->dwObjectIdx == dwID ) return TRUE ;
	}

	return FALSE ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Set_RecallCount
//	DESC		: ��ȯ�ؾ� �� ��ȯ ī��Ʈ�� �޴´�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: October 7, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Set_RecallCount( void* pMsg )
{
	// �Լ� ���� Ȯ��
	if( !pMsg ) return ;


	// ���� �޽��� ��ȯ.
	MSG_WORD2* pmsg = NULL ;
	pmsg = (MSG_WORD2*)pMsg ;

	if( !pmsg ) return ;


	// �� ���� Ȯ��
	if( pmsg->wData1 != g_pServerSystem->GetMapNum() ) return ;


	m_wRecallCount = pmsg->wData2 ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Load_RecallCount
//	DESC		: ���� ��ȯ���� ������ ų ���¸� �ε��Ͽ� ������ �Ѿ���� �� ó���ϴ� �۾� �߰�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: October 15, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Load_RecallCount( void* pMsg )
{
	// �Լ� ���� Ȯ��
	if( !pMsg ) return ;


	// ���� �޽��� ��ȯ.
	MSG_BYTE* pmsg = NULL ;
	pmsg = (MSG_BYTE*)pMsg ;

	if( !pmsg ) return ;


	// ��ȯ ���͸� ������ ���� �� ���� �޴´�.
	m_wRecallCount = (WORD)pmsg->bData ;


	// ��� ���͸� �� �����ߴٸ�, ���ͽõ� ��� ���� ���·� �����Ѵ�.
	if( (WORD)pmsg->bData >= m_wRecallCount )
	{
		m_byReadyToUseWaterSeed = TRUE ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: IsReadyToUseWaterSeed
//	DESC		: ���ͽõ� ���� ���� �غ� �Ǿ����� ���θ� ��ȯ�ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: October 8, 2008
//-------------------------------------------------------------------------------------------------
BYTE CSiegeRecallMgr::IsReadyToUseWaterSeed()
{
	if( m_byReadyToUseWaterSeed ) return TRUE ;
	else return FALSE ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: RequestKillCountToDB
//	DESC		: ��ȯ �� ĳ����Ʈ�� ��� ���� �Ǿ����� DB�� Ȯ�� ��û�ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: December 22, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::RequestKillCountToDB()
{
	// �ӽ� ���۸� �����Ѵ�.
	char txt[128] = {0, } ;

	sprintf(txt, "EXEC  %s %d",	MP_SIEGERECALL_KILLCOUNT, g_pServerSystem->GetMapNum()) ;

	// �������� �����Ѵ�.
	g_DB.Query(eQueryType_FreeQuery, eRequuestKillCount, 0, txt) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: CheckKillCount
//	DESC		: ĳ����Ʈ�� ��� ��Ҵ��� ��� ���� �޾Ƽ� �����ϴ� �Լ� �߰�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: October 8, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::CheckKillCount(int nKillCount)
{
	// ��� ���͸� �� �����ߴٸ�, ���ͽõ� ��� ���� ���·� �����Ѵ�.
	// if( nKillCount >= m_wRecallCount )
	// {
		m_byReadyToUseWaterSeed = TRUE ;
	// }
	// else
	// {
	// 	m_byReadyToUseWaterSeed = FALSE ;
	// }
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Cheat_ExcuteNextStep
//	DESC		: ���� ��ȯ���� ��ȯ ���¸� ���� �ܰ�� �ѱ�� ġƮ ���� �߰�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: October 11, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Cheat_ExcuteNextStep( void* pMsg )
{
	// �Լ� ���� Ȯ��.
	if( !pMsg ) return ;


	// ���� �޽��� ��ȯ.
	MSG_SIEGERECALL_OBJINFO* pmsg = NULL ;
	pmsg = (MSG_SIEGERECALL_OBJINFO*)pMsg ;

	if( !pmsg ) return ;

	BYTE IsSiegeWarfareZone = BYTE( SIEGEWARFAREMGR->IsSiegeWarfareZone() );
	if( !IsSiegeWarfareZone ) return ;

	// 081028 LYW --- SiegeRecallMgr : ĳ����Ʈ ���� ���� �޽��� �α׸� �����.
	if( IsSiegeWarfareZone )
	{
		WriteCatapultLog( "Call - Cheat_ExcuteNextStep" ) ;
	}

	if( pmsg->siegeObj.mapNum != g_pServerSystem->GetMapNum() ) return ;

	// 081028 LYW --- SiegeRecallMgr : ���� ���� ó�� �Լ��� ȣ��Ǿ����� �α׸� �����.
	if( IsSiegeWarfareZone )
	{
		char szCheckBefor[1024] = {0, } ;
		sprintf( szCheckBefor, "Befor Count - %d\n", m_mAddObj.size() ) ;
		WriteCatapultLog( szCheckBefor ) ;
	}


	// ������ ��ġ�ϴ� ������ �ִ��� Ȯ���ϰ�, ������ ���� ó���Ѵ�.
	M_MADDOBJ::iterator it ;
	it = m_mAddObj.find( pmsg->siegeObj.byAddObjIdx ) ;

	if( it != m_mAddObj.end() )
	{
		// 081028 LYW --- SiegeRecallMgr : ų�� ���̵� �α׸� ����� ���� �Լ��� ������.
		//Check_ObjectDie( it->second.Get_ObjInfo()->dwObjectIdx, TRUE ) ;
		//090812 pdy ������ Agent�� Map�������� ��ȯObj �ε��������� �߸����ִ� ���� ���� 
		Check_ObjectDie( NULL, it->second.Get_MonsterID(), TRUE ) ;
	}
	else
	{
		if(pmsg->siegeObj.byComKind == e_CK_ADD_OBJECT)
		{
			Remove_ObjFromDB( pmsg->siegeObj.wThemeIdx, pmsg->siegeObj.mapNum, g_pServerSystem->GetMapNum(), 
			pmsg->siegeObj.byStepIdx, pmsg->siegeObj.byAddObjIdx, pmsg->siegeObj.byComKind, pmsg->siegeObj.byComIndex, 2) ;

			// 081028 LYW --- SiegeRecallMgr : ĳ����Ʈ ���� ���� �޽��� �α׸� �����.
			if( IsSiegeWarfareZone )
			{
				char szLog[1024] = {0, } ;
				sprintf(szLog, "Delete Catapult - %d %d %d %d %d %d %d %d %u %s\n", pmsg->siegeObj.wThemeIdx, 
					pmsg->siegeObj.mapNum, g_pServerSystem->GetMapNum(), pmsg->siegeObj.byStepIdx, pmsg->siegeObj.byAddObjIdx, 
					pmsg->siegeObj.byComKind, pmsg->siegeObj.byComIndex, 2, pmsg->siegeObj.byAddObjIdx, "Use Cheat Command!!" ) ;
				WriteCatapultLog( szLog ) ;
			}
		}
		else
		{
			Remove_ObjFromDB( pmsg->siegeObj.wThemeIdx, pmsg->siegeObj.wParentMap, g_pServerSystem->GetMapNum(), 
			pmsg->siegeObj.byStepIdx, pmsg->siegeObj.byAddObjIdx, pmsg->siegeObj.byComKind, pmsg->siegeObj.byComIndex, 4) ;

			// 081028 LYW --- SiegeRecallMgr : ĳ����Ʈ ���� ���� �޽��� �α׸� �����.
			if( IsSiegeWarfareZone )
			{
				char szLog[1024] = {0, } ;
				sprintf(szLog, "Delete Catapult - %d %d %d %d %d %d %d %d %u %s\n", pmsg->siegeObj.wThemeIdx, 
					pmsg->siegeObj.wParentMap, g_pServerSystem->GetMapNum(), pmsg->siegeObj.byStepIdx, pmsg->siegeObj.byAddObjIdx, 
					pmsg->siegeObj.byComKind, pmsg->siegeObj.byComIndex, 4, pmsg->siegeObj.byAddObjIdx, "Use Cheat Command!!" ) ;
				WriteCatapultLog( szLog ) ;
			}
		}

		if( pmsg->siegeObj.byComKind == e_CK_ADD_OBJECT )
		{
			MSG_WORD3 msg ;

			msg.Category	= MP_SIEGERECALL ;
			msg.Protocol	= MP_SIEGERECALL_NOTICE_DIERECALLOBJ_READY_SYN ;

			msg.dwObjectID	= 0 ;

			msg.wData1		= pmsg->siegeObj.mapNum ;
			msg.wData2		= pmsg->siegeObj.byStepIdx ;
			msg.wData3		= e_STEP_ADD_OBJ ;

			// Send message.
			g_Network.Send2AgentServer( (char*)&msg, sizeof(msg) ) ;
		}

		// Delete obj info.
		m_mAddObj.erase( it ) ;
	}

	// 081028 LYW --- SiegeRecallMgr : ���� ���� ó�� �Լ��� ȣ��Ǿ����� �α׸� �����.
	if( IsSiegeWarfareZone )
	{
		char szCheckAfter[1024] = {0, } ;
		sprintf( szCheckAfter, "After Count - %d\n", m_mAddObj.size() ) ;
		WriteCatapultLog( szCheckAfter ) ;
	}
}


// 081012 LYW --- SiegeRecall : ��ȯ�� �ʱ�ȭ �Լ� �߰�.
void CSiegeRecallMgr::Clear_RecallInfo()
{
	BYTE IsSiegeWarfareZone = BYTE( SIEGEWARFAREMGR->IsSiegeWarfareZone() );
	if( !IsSiegeWarfareZone ) return ;

	char szLog[1024] = {0, } ;

	// 081028 LYW --- SiegeRecallMgr : 
	if( IsSiegeWarfareZone )
	{
		sprintf( szLog, "Currnet Recall Count : %d\n", m_mAddObj.size() ) ;
		WriteCatapultLog( szLog ) ;
	}

	M_MADDOBJ::iterator it ;
	
	for( it = m_mAddObj.begin() ; it != m_mAddObj.end() ; ++it )
	{
		//090812 pdy ������ Agent�� Map�������� ��ȯObj �ε��������� �߸����ִ� ���� ���� 
		g_pServerSystem->RemoveMonster( it->second.Get_MonsterID() ) ;

		// Delete obj info.
		m_mAddObj.erase( it ) ;
	}

	if( IsSiegeWarfareZone )
	{
		sprintf( szLog, "After clear recall info : %d\n", m_mAddObj.size() ) ;
		WriteCatapultLog( szLog ) ;
	}

	// 081226 LYW --- SiegeRecallMgr : ����ũ �ε��� �߰� ó��.
	m_byUniqueIdx = 0 ;
}