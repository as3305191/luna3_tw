//-------------------------------------------------------------------------------------------------
//	NAME		: CSiegeRecallMgr.cpp
//	DESC		: Implementation part of CSiegeRecallMgr class.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 23, 2008
//-------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------------------------
//		Include part.
//-------------------------------------------------------------------------------------------------
#include "stdafx.h"																						// An include file for standard system include files

#include "MHFile.h" 

#include "../[CC]ServerModule/Network.h"

#include "../[CC]RecallInfo/SiegeRecall/SiegeTheme.h" 
#include "../[CC]RecallInfo/SiegeRecall/SiegeMap.h"

#include "./SiegeRecallMgr.h"

#include "./AgentDBMsgParser.h"





//-------------------------------------------------------------------------------------------------
//	NAME		: CSiegeRecallMgr
//	DESC		: ������ �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 23, 2008
//-------------------------------------------------------------------------------------------------
CSiegeRecallMgr::CSiegeRecallMgr(void)
{
}





//-------------------------------------------------------------------------------------------------
//	NAME		: ~CSiegeRecallMgr
//	DESC		: �Ҹ��� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 23, 2008
//-------------------------------------------------------------------------------------------------
CSiegeRecallMgr::~CSiegeRecallMgr(void)
{
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Initialize
//	DESC		: �ʱ�ȭ �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 24, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Initialize()
{
	// ������ ��ȯ�� ������ ��� ��ũ��Ʈ�� �ε��Ѵ�.
	Load_RecallInfo() ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Load_RecallInfo
//	DESC		: ��ȯ ���� �ε� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 23, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Load_RecallInfo()
{
	// ���� Ŭ���� ����.
	CMHFile file ;


	// SiegeRecall.bin ������ �б� ���� ����.
	if( !file.Init("./system/resource/SiegeRecall.bin", "rb") )
	{
		Throw_Error( "Failed to initialize SiegeRecall.bin", __FUNCTION__ ) ;
		return ;
	}

	char szBuff[257] = {0};
	BYTE byCommandKind = ERR_INVALID_COMMAND ;

	while( TRUE )
	{
		// ������ ������ üũ�Ѵ�.
		if( file.IsEOF() ) break ;

		// ��ɾ �޴´�.
		memset( szBuff, 0, strlen(szBuff) ) ;
		file.GetString(szBuff) ;

		// �ּ� ��ɾ� ���, continue ó���� �Ѵ�..
		if(strcmp( szBuff, "//" ) == 0)
		{
			memset( szBuff, 0, strlen(szBuff) ) ;
			file.GetLine(szBuff, sizeof(256)) ;
			continue ;
		}
		else if( (strcmp( szBuff, "{" ) == 0) || (strcmp( szBuff, "}" ) == 0 ) ) continue ;

		// ��ũ��Ʈ ��ɾ� Ÿ���� �޴´�.
		byCommandKind = Get_CommandKind( szBuff ) ;

		// ��ɾ� Ÿ�Կ� ���� ó���� �Ѵ�.
		Command_Process(byCommandKind, &file) ;
	}


	// ���� Ŭ������ �����Ѵ�.
	file.Release() ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Get_CommandKind
//	DESC		: ��ũ��Ʈ ��ɾ� Ÿ���� ��ȯ�ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 24, 2008
//-------------------------------------------------------------------------------------------------
BYTE CSiegeRecallMgr::Get_CommandKind(char* pString)
{
	BYTE byCommandKind = ERR_INVALID_COMMAND ;

	// �Լ� ���� Ȯ��.
	if(!pString)
	{
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return byCommandKind ;
	}


	// ��ũ��Ʈ ��ɾ��� ��Ʈ�� ������ üũ.
	if( strlen(pString) <= 1 )
	{
		Throw_Error("Invalid string size!!", __FUNCTION__) ;
		return byCommandKind ;
	}


	// ��ũ��Ʈ ��ɾ� Ÿ���� �����Ѵ�.
	if( strcmp( pString, "#ADD_THEME" ) == 0 )
	{
		byCommandKind = e_CK_ADD_THEME ;
		return byCommandKind ;
	}

	if( strcmp( pString, "#ADD_THEME_IDX" ) == 0 )
	{
		byCommandKind = e_CK_ADD_THEME_IDX ;
		return byCommandKind ;
	}

	if( strcmp( pString, "#ADD_MAP" ) == 0 )
	{
		byCommandKind = e_CK_ADD_MAP ;
		return byCommandKind ;
	}

	if( strcmp( pString, "#ADD_STEP" ) == 0 )
	{
		byCommandKind = e_CK_ADD_STEP ;
		return byCommandKind ;
	}

	if( strcmp( pString, "#ADD_OBJECT" ) == 0 )
	{
		byCommandKind = e_CK_ADD_OBJECT ;
		return byCommandKind ;
	}

	if( strcmp( pString, "#START_TERM" ) == 0 )
	{
		byCommandKind = e_CK_START_TERM ;
		return byCommandKind ;
	}

	if( strcmp( pString, "#END_TERM" ) == 0 )
	{
		byCommandKind = e_CK_END_TERM ;
		return byCommandKind ;
	}

	if( strcmp( pString, "#COMMAND" ) == 0 )
	{
		byCommandKind = e_CK_COMMAND ;
		return byCommandKind ;
	}

	if( strcmp( pString, "#DIE_RECALL_OBJ" ) == 0 )
	{
		byCommandKind = e_CK_DIE_RECALL_OBJ ;
		return byCommandKind ;
	}

	if( strcmp( pString, "#CHECK_DIE_OBJ" ) == 0 )
	{
		byCommandKind = e_CK_CHECK_DIE_OBJ ;
		return byCommandKind ;
	}

	if( strcmp( pString, "#CHECK_USE_ITEM" ) == 0 )
	{
		byCommandKind = e_CK_USE_ITEM ;
		return byCommandKind ;
	}


	// ��ũ��Ʈ ��ɾ� Ÿ���� ��ȯ�Ѵ�.
	return byCommandKind ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Command_Process
//	DESC		: ��ũ��Ʈ ��ɾ� Ÿ�Կ� ���� ó���� �ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 24, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Command_Process(BYTE byCommandKind, CMHFile* pFile)
{
	// �Լ� ���� Ȯ��.
	if(!pFile)
	{
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}

	switch(byCommandKind)
	{
	case e_CK_ADD_THEME :				Add_Theme( pFile ) ;		break ;
	//	
	case e_CK_ADD_THEME_IDX :			Add_Theme_Idx( pFile ) ;	break ;

	case e_CK_ADD_MAP :					Add_Map( pFile ) ;			break ;

	case e_CK_ADD_STEP :				Add_Step( pFile ) ;			break ;

	case e_CK_ADD_OBJECT :				Add_Obj( pFile ) ;			break ;

	case e_CK_START_TERM :				Add_StartTerm( pFile ) ;	break ;
	case e_CK_END_TERM :				Add_EndTerm( pFile ) ;		break ;
	case e_CK_COMMAND :					Add_Command( pFile ) ;		break ;

	case e_CK_DIE_RECALL_OBJ :			Die_RecallObj( pFile ) ;	break ;

	case e_CK_CHECK_DIE_OBJ :			Check_Die_Obj( pFile ) ;	break ;

	case e_CK_USE_ITEM :				Check_Use_Item( pFile ) ;	break ;

	default : return ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Add_Theme
//	DESC		: ���� �׸� ������ �߰��ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 25, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Add_Theme(CMHFile* pFile)
{
	// �Լ� ���� Ȯ��.
	if(!pFile)
	{
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// �׸� �ε����� �޴´�.
	BYTE byIdx = (BYTE)m_Theme.size() ;


	// �ߺ� �׸��� �ִ��� Ȯ���Ѵ�.
	M_THEME::iterator it ;
	it = m_Theme.find(byIdx) ;

	if(it != m_Theme.end())
	{
		Throw_Error("Failed add theme, Clone theme!!", __FUNCTION__) ;
		return ;
	}


	// ���ο� �׸��� �߰��Ѵ�.
	CSiegeTheme newTheme ;
	newTheme.Set_Idx(byIdx) ;

	m_Theme.insert(std::make_pair(byIdx, newTheme)) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Get_ThemeInfo
//	DESC		: �����̳ʿ� ���� �ε����� �׸� ������ ��ȯ�ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 28, 2008
//-------------------------------------------------------------------------------------------------
CSiegeTheme* CSiegeRecallMgr::Get_ThemeInfo(BYTE byIdx)
{
	// �Լ� ���� Ȯ��.
	M_THEME::iterator it ;
	
	for( it = m_Theme.begin() ; it != m_Theme.end() ; ++it )
	{
		if( it == m_Theme.end() ) return NULL ;

		if( it->second.Get_Idx() != byIdx ) continue ;

		// �׸� ������ ��ȯ�Ѵ�.
		return &it->second ;
	}


	// NULL�� ��ȯ�Ѵ�.
	return NULL ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Get_ThemeInfoByThemeIdx
//	DESC		: �׸� �ε����� �׸� ������ ��ȯ�ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 31, 2008
//-------------------------------------------------------------------------------------------------
CSiegeTheme* CSiegeRecallMgr::Get_ThemeInfoByThemeIdx(WORD wIdx) 
{
	// �׸� �ε����� Ȯ���Ѵ�.
	M_THEME::iterator it ;
	
	for( it = m_Theme.begin() ; it != m_Theme.end() ; ++it )
	{
		if( it->second.Get_ThemeIdx() != wIdx ) continue ;

		// �׸� ������ Ȯ���Ѵ�.
		return &it->second ;
	}


	// NULL�� ��ȯ�Ѵ�.
	return NULL ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Add_Theme_Idx
//	DESC		: �׸� �ε����� �߰��ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 28, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Add_Theme_Idx(CMHFile* pFile)
{
	// �Լ� ���� Ȯ��.
	if(!pFile)
	{
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// �׸� �ε����� Ȯ���Ѵ�.
	BYTE byIdx = (BYTE)m_Theme.size() ;


	// �ߺ� �׸� ���θ� Ȯ���Ѵ�.
	M_THEME::iterator it ;
	it = m_Theme.find(byIdx-1) ;

	if(it == m_Theme.end() || byIdx == 0)
	{
		Throw_Error("No theme info!!", __FUNCTION__) ;
		return ;
	}


	// �׸� �ε����� �����Ѵ�.
	it->second.Set_ThemeIdx( pFile->GetWord() ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Add_Map
//	DESC		: ��ȯ������ ��ȯ �� �� ������ �߰��ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 25, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Add_Map(CMHFile* pFile)
{
	// �Լ� ���� Ȯ��.
	if(!pFile)
	{
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// �� ��ȣ�� �д´�.
	MAPTYPE mapNum = 0 ;
	mapNum = pFile->GetWord() ;

	if( mapNum == 0 )
	{
		Throw_Error("Failed add map, Invalid map num!!", __FUNCTION__) ;
		return ;
	}


	// ���� �׸��� �ε����� �޴´�.
	int nIndex = 0 ;
	nIndex = m_Theme.size() ;

	if(nIndex <= 0)
	{
		Throw_Error("Failed add map, Empty theme!!", __FUNCTION__) ;
		return ;
	}


	// �θ� �׸��� �޴´�.
	M_THEME::iterator it ;
	it = m_Theme.find( (BYTE)(nIndex-1) ) ;
	if( it == m_Theme.end() )
	{
		Throw_Error("Failed add map, No parent theme!!", __FUNCTION__) ;
		return ;
	}


	// Check clone map.
	CSiegeMap* pCloneMap = NULL ;
	pCloneMap = it->second.Get_SiegeMapByMapNum( mapNum ) ;

	if( pCloneMap )
	{
		Throw_Error("Failed add map, Clone map!!", __FUNCTION__) ;
		return ;
	}


	// Create a new siege map.
	CSiegeMap newMap ;
	newMap.Set_ThemeIdx( it->second.Get_ThemeIdx() ) ;
	newMap.Set_Idx( it->second.Get_MapCount() ) ;
	newMap.Set_MapNum(mapNum) ;


	// Add siege map to current theme.
	it->second.Add_SiegeMap( &newMap ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Add_Step
//	DESC		: The function to add step info.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 29, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Add_Step(CMHFile* pFile)
{
	// Check parameter.
	if(!pFile)
	{
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// Receive a index of current theme.
	int nIndex = 0 ;
	nIndex = m_Theme.size() ;

	if(nIndex <= 0)
	{
		Throw_Error("Failed add map, Empty theme!!", __FUNCTION__) ;
		return ;
	}


	// Check parent theme.
	M_THEME::iterator it ;
	it = m_Theme.find( (BYTE)(nIndex-1)) ;
	if( it == m_Theme.end() )
	{
		Throw_Error("Failed add map, No parent theme!!", __FUNCTION__) ;
		return ;
	}


	// Receive a current siege map.
	BYTE byMapCount = it->second.Get_MapCount() ;

	CSiegeMap* pCurMap = NULL ;
	pCurMap = it->second.Get_SiegeMapByIdx(byMapCount-1) ;

	if(!pCurMap)
	{
		Throw_Error("Failed to add obj, No parent map!!", __FUNCTION__) ;
		return ;
	}


	// Receive step count.
	BYTE byStepCount = 0 ;
	byStepCount = pCurMap->Get_StepCount() ;


	// Setting step index.
	CSiegeStep newStep ;
	newStep.Set_ThemeIdx( it->second.Get_ThemeIdx() ) ;
	newStep.Set_ParentMapNum( pCurMap->Get_MapNum() ) ;
	newStep.Set_StepIndex(byStepCount) ;


	// Add step.
	pCurMap->Add_Step(byStepCount, &newStep) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Add_Obj
//	DESC		: The function to add the object info.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 25, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Add_Obj(CMHFile* pFile)
{
	// Check parameter.
	if(!pFile)
	{
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// Receive recall info.
	MAPTYPE mapNum		= pFile->GetWord() ;
	DWORD dwObjectIdx	= pFile->GetDword() ;

	float fXpos			= pFile->GetFloat() ;
	float fZpos			= pFile->GetFloat() ;

	BYTE byUseRandomPos = pFile->GetByte() ;
	BYTE byRadius		= pFile->GetByte() ;


	// Receive a index of current theme.
	int nIndex = 0 ;
	nIndex = m_Theme.size() ;

	if(nIndex <= 0)
	{
		Throw_Error("Failed to add obj, Empty theme!!", __FUNCTION__) ;
		return ;
	}


	// Check parent theme.
	M_THEME::iterator it ;
	it = m_Theme.find( (BYTE)(nIndex-1) ) ;

	if( it == m_Theme.end() )
	{
		Throw_Error("Failed to add obj, No parent theme!!", __FUNCTION__) ;
		return ;
	}


	// Receive a current siege map.
	BYTE byMapCount = it->second.Get_MapCount() ;

	CSiegeMap* pCurMap = NULL ;
	pCurMap = it->second.Get_SiegeMapByIdx(byMapCount-1) ;

	if(!pCurMap)
	{
		Throw_Error("Failed to add obj, No parent map!!", __FUNCTION__) ;
		return ;
	}


	// Receive step count.
	BYTE byStepCount = 0 ;
	byStepCount = pCurMap->Get_StepCount() ;


	// Receive step info.
	CSiegeStep* pStep = NULL ;
	pStep = pCurMap->Get_StepInfo(byStepCount-1) ;

	if(!pStep)
	{
		Throw_Error("Failed to add obj, No step!!", __FUNCTION__) ;
		return ;
	}


	// Receive child count.
	BYTE byChildCount = 0 ;
	byChildCount = pStep->Get_ChildCount() ;


	// Setting index.
	CSiege_AddObj newAddObj ;
	newAddObj.Set_Index( byChildCount ) ;


	// Setting object info.
	//newAddObj.Init_ObjInfo(it->second.Get_ThemeIdx(), mapNum, pStep->Get_StepIndex(), dwObjectIdx, fXpos, fZpos, byUseRandomPos, byRadius, 0, 0 ) ;

	st_SIEGEOBJ objInfo ;
	memset(&objInfo, 0, sizeof(st_SIEGEOBJ)) ;

	objInfo.wThemeIdx		= it->second.Get_ThemeIdx() ;
	objInfo.mapNum			= mapNum ;
	objInfo.byStepIdx		= pStep->Get_StepIndex() ;
	objInfo.dwObjectIdx		= dwObjectIdx ;
	objInfo.fXpos			= fXpos ;
	objInfo.fZpos			= fZpos ;
	objInfo.byUseRandomPos	= byUseRandomPos ;
	objInfo.byRadius		= byRadius ;

	newAddObj.Init_ObjInfo(&objInfo) ;


	// Insert add object info.
	pStep->Insert_AddObj( byChildCount, &newAddObj ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Add_StartTerm
//	DESC		: The function to add start term.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 30, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Add_StartTerm(CMHFile* pFile)
{
	// Check parameter.
	if(!pFile)
	{
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// Receive a index of current theme.
	int nIndex = 0 ;
	nIndex = m_Theme.size() ;

	if(nIndex <= 0)
	{
		Throw_Error("Failed to add obj, Empty theme!!", __FUNCTION__) ;
		return ;
	}


	// Check parent theme.
	M_THEME::iterator it ;
	it = m_Theme.find( (BYTE)(nIndex-1) ) ;

	if( it == m_Theme.end() )
	{
		Throw_Error("Failed to add obj, No parent theme!!", __FUNCTION__) ;
		return ;
	}


	// Receive a current siege map.
	BYTE byMapCount = it->second.Get_MapCount() ;

	CSiegeMap* pCurMap = NULL ;
	pCurMap = it->second.Get_SiegeMapByIdx(byMapCount-1) ;

	if(!pCurMap)
	{
		Throw_Error("Failed to add obj, No parent map!!", __FUNCTION__) ;
		return ;
	}


	// Receive step count.
	BYTE byStepCount = 0 ;
	byStepCount = pCurMap->Get_StepCount() ;


	// Receive step info.
	CSiegeStep* pStep = NULL ;
	pStep = pCurMap->Get_StepInfo(byStepCount-1) ;

	if(!pStep)
	{
		Throw_Error("Failed to add obj, No step!!", __FUNCTION__) ;
		return ;
	}


	// Check step kind.
	BYTE byChildCount = pStep->Get_ChildCount() ;
	BYTE byChildKind = pStep->Get_ChildKind(byChildCount-1) ;

	switch(byChildKind)
	{
	case e_STEP_ADD_OBJ :
		{
			CSiege_AddObj* pAddInfo = NULL ;
			pAddInfo = pStep->Get_AddObjInfo( byChildCount-1 ) ;

			if(!pAddInfo)
			{
				Throw_Error("No parent addobj info!!", __FUNCTION__) ;
				return ;
			}

			BYTE byIndex = pAddInfo->Get_ChildCount() ;

			CSiegeTerm newTerm ;
			newTerm.Set_Index( byIndex ) ;

			pAddInfo->Insert_Sterm( byIndex, &newTerm ) ;
		}
		break ;

	default : break ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Add_EndTerm
//	DESC		: The function to add end term.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 30, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Add_EndTerm(CMHFile* pFile)
{
	// Check parameter.
	if(!pFile)
	{
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// Receive a index of current theme.
	int nIndex = 0 ;
	nIndex = m_Theme.size() ;

	if(nIndex <= 0)
	{
		Throw_Error("Failed to add obj, Empty theme!!", __FUNCTION__) ;
		return ;
	}


	// Check parent theme.
	M_THEME::iterator it ;
	it = m_Theme.find( (BYTE)(nIndex-1)) ;

	if( it == m_Theme.end() )
	{
		Throw_Error("Failed to add obj, No parent theme!!", __FUNCTION__) ;
		return ;
	}


	// Receive a current siege map.
	BYTE byMapCount = it->second.Get_MapCount() ;

	CSiegeMap* pCurMap = NULL ;
	pCurMap = it->second.Get_SiegeMapByIdx(byMapCount-1) ;

	if(!pCurMap)
	{
		Throw_Error("Failed to add obj, No parent map!!", __FUNCTION__) ;
		return ;
	}


	// Receive step count.
	BYTE byStepCount = 0 ;
	byStepCount = pCurMap->Get_StepCount() ;


	// Receive step info.
	CSiegeStep* pStep = NULL ;
	pStep = pCurMap->Get_StepInfo(byStepCount-1) ;

	if(!pStep)
	{
		Throw_Error("Failed to add obj, No step!!", __FUNCTION__) ;
		return ;
	}


	// Check step kind.
	BYTE byChildCount = pStep->Get_ChildCount() ;
	BYTE byChildKind = pStep->Get_ChildKind(byChildCount-1) ;

	switch(byChildKind)
	{
	case e_STEP_ADD_OBJ :
		{
			CSiege_AddObj* pAddInfo = NULL ;
			pAddInfo = pStep->Get_AddObjInfo( byChildCount-1 ) ;

			if(!pAddInfo)
			{
				Throw_Error("No parent addobj info!!", __FUNCTION__) ;
				return ;
			}

			BYTE byIndex = pAddInfo->Get_ChildCount() ;

			CSiegeTerm newTerm ;
			newTerm.Set_Index( byIndex ) ;

			pAddInfo->Insert_ETerm( byIndex, &newTerm ) ;
		}
		break ;

	default : break ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Add_Command
//	DESC		: The function to add a command info.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 30, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Add_Command(CMHFile* pFile)
{
	// Check parameter.
	if(!pFile)
	{
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// Receive a index of current theme.
	int nIndex = 0 ;
	nIndex = m_Theme.size() ;

	if(nIndex <= 0)
	{
		Throw_Error("Failed to add obj, Empty theme!!", __FUNCTION__) ;
		return ;
	}


	// Check parent theme.
	M_THEME::iterator it ;
	it = m_Theme.find( (BYTE)(nIndex-1) ) ;

	if( it == m_Theme.end() )
	{
		Throw_Error("Failed to add obj, No parent theme!!", __FUNCTION__) ;
		return ;
	}


	// Receive a current siege map.
	BYTE byMapCount = it->second.Get_MapCount() ;

	CSiegeMap* pCurMap = NULL ;
	pCurMap = it->second.Get_SiegeMapByIdx(byMapCount-1) ;

	if(!pCurMap)
	{
		Throw_Error("Failed to add obj, No parent map!!", __FUNCTION__) ;
		return ;
	}


	// Receive step count.
	BYTE byStepCount = 0 ;
	byStepCount = pCurMap->Get_StepCount() ;


	// Receive step info.
	CSiegeStep* pStep = NULL ;
	pStep = pCurMap->Get_StepInfo(byStepCount-1) ;

	if(!pStep)
	{
		Throw_Error("Failed to add obj, No step!!", __FUNCTION__) ;
		return ;
	}


	// Check step kind.
	BYTE byChildCount = pStep->Get_ChildCount() ;
	BYTE byChildKind = pStep->Get_ChildKind(byChildCount-1) ;

	switch(byChildKind)
	{
	case e_STEP_ADD_OBJ :
		{
			CSiege_AddObj* pAddInfo = NULL ;
			pAddInfo = pStep->Get_AddObjInfo( byChildCount-1 ) ;

			if(!pAddInfo)
			{
				Throw_Error("No parent addobj info!!", __FUNCTION__) ;
				return ;
			}

			BYTE byIndex = pAddInfo->Get_ChildCount() ;

			CSiegeCommand newCommand ;
			newCommand.Set_Index( byIndex ) ;

			pAddInfo->Insert_Command( byIndex, &newCommand ) ;
		}
		break ;

	default : break ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Die_RecallObj
//	DESC		: The function to recall object when a object die.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 25, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Die_RecallObj(CMHFile* pFile)
{
	// Check parameter.
	if(!pFile)
	{
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// Receive a index of current theme.
	int nIndex = 0 ;
	nIndex = m_Theme.size() ;

	if(nIndex <= 0)
	{
		Throw_Error("Empty theme!!", __FUNCTION__) ;
		return ;
	}


	// Check parent theme.
	M_THEME::iterator it ;
	it = m_Theme.find( (BYTE)(nIndex-1) ) ;
	if( it == m_Theme.end() )
	{
		Throw_Error("No parent theme!!", __FUNCTION__) ;
		return ;
	}


	// Receive a current siege map.
	BYTE byMapCount = it->second.Get_MapCount() ;
	CSiegeMap* pCurMap = NULL ;
	pCurMap = it->second.Get_SiegeMapByIdx(byMapCount-1) ;

	if(!pCurMap)
	{
		Throw_Error("No parent map!!", __FUNCTION__) ;
		return ;
	}


	// Receive a step info.
	BYTE byStepCount = pCurMap->Get_StepCount() ;

	CSiegeStep* pCurStep = NULL ;
	pCurStep = pCurMap->Get_StepInfo(byStepCount-1) ;


	// Check step info.
	if(!pCurStep)
	{
		Throw_Error("No parent step!!", __FUNCTION__) ;
		return ;
	}


	// Check step kind.
	BYTE byChildCount = pCurStep->Get_ChildCount() ;
	BYTE byChildKind = pCurStep->Get_ChildKind(byChildCount-1) ;

	switch(byChildKind)
	{
	case e_STEP_ADD_OBJ :
		{
			CSiege_AddObj* pAddInfo = NULL ;
			pAddInfo = pCurStep->Get_AddObjInfo( byChildCount-1 ) ;

			if(!pAddInfo)
			{
				Throw_Error("No parent addobj info!!", __FUNCTION__) ;
				return ;
			}

			BYTE byIndex = pAddInfo->Get_ChildCount() ;

			CSiegeCommand* pCurCommand = NULL ;
			pCurCommand = pAddInfo->Get_CommandInfo(byIndex-1) ;

			if(!pCurCommand)
			{
				Throw_Error("No parent command info!!", __FUNCTION__) ;
				return ;
			}

			BYTE byCommandChildCount = pCurCommand->Get_ChildCount() ;

			st_DIE_RECALL_OBJ recallObj ;

			recallObj.byIdx = byCommandChildCount ;

			recallObj.objInfo.wThemeIdx			= it->second.Get_ThemeIdx() ;
			recallObj.objInfo.mapNum			= pFile->GetWord() ;
			recallObj.objInfo.byStepIdx			= pCurStep->Get_StepIndex() ;
			recallObj.objInfo.dwObjectIdx		= pFile->GetDword() ;
			recallObj.objInfo.fXpos				= pFile->GetFloat() ;
			recallObj.objInfo.fZpos				= pFile->GetFloat() ;
			recallObj.objInfo.byUseRandomPos	= pFile->GetByte() ;
			recallObj.objInfo.byRadius			= pFile->GetByte() ;

			pCurCommand->Add_Die_Recall_Obj( &recallObj ) ;
		}
		break ;

	default : break ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Check_Die_Obj
//	DESC		: The function to check whether object die or not.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 28, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Check_Die_Obj(CMHFile* pFile)
{
	// Check parameter.
	if(!pFile)
	{
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// Receive a index of current theme.
	int nIndex = 0 ;
	nIndex = m_Theme.size() ;

	if(nIndex <= 0)
	{
		Throw_Error("Empty theme!!", __FUNCTION__) ;
		return ;
	}


	// Check parent theme.
	M_THEME::iterator it ;
	it = m_Theme.find( (BYTE)(nIndex-1) ) ;
	if( it == m_Theme.end() )
	{
		Throw_Error("No parent theme!!", __FUNCTION__) ;
		return ;
	}


	// Receive a current siege map.
	BYTE byMapCount = it->second.Get_MapCount() ;
	CSiegeMap* pCurMap = NULL ;
	pCurMap = it->second.Get_SiegeMapByIdx(byMapCount-1) ;

	if(!pCurMap)
	{
		Throw_Error("No parent map!!", __FUNCTION__) ;
		return ;
	}


	// Receive a step info.
	BYTE byStepCount = pCurMap->Get_StepCount() ;

	CSiegeStep* pCurStep = NULL ;
	pCurStep = pCurMap->Get_StepInfo(byStepCount-1) ;


	// Check step info.
	if(!pCurStep)
	{
		Throw_Error("No parent step!!", __FUNCTION__) ;
		return ;
	}


	// Check step kind.
	BYTE byChildCount = pCurStep->Get_ChildCount() ;
	BYTE byChildKind = pCurStep->Get_ChildKind(byChildCount-1) ;

	switch(byChildKind)
	{
	case e_STEP_ADD_OBJ :
		{
			CSiege_AddObj* pAddInfo = NULL ;
			pAddInfo = pCurStep->Get_AddObjInfo( byChildCount-1 ) ;

			if(!pAddInfo)
			{
				Throw_Error("No parent addobj info!!", __FUNCTION__) ;
				return ;
			}

			BYTE byIndex = pAddInfo->Get_ChildCount() ;

			BYTE byKind = pAddInfo->Get_ChildKind(byIndex-1) ;

			CSiegeTerm* pSiegeTerm = NULL ;

			switch(byKind)
			{
			case e_SIEGE_BASEKIND_START_TERM : 
				{
					pSiegeTerm = pAddInfo->Get_StartTermInfo(byIndex-1) ;

					if(!pSiegeTerm)
					{
						Throw_Error("No start term info!!", __FUNCTION__) ;
						return ;
					}
				}
				break ;

			case e_SIEGE_BASEKIND_END_TERM :
				{
					pSiegeTerm = pAddInfo->Get_EndTermInfo(byIndex-1) ;

					if(!pSiegeTerm)
					{
						Throw_Error("No start term info!!", __FUNCTION__) ;
						return ;
					}
				}
				break ;

			default : break ;
			}

			MAPTYPE mapNum = pFile->GetWord() ;
			BYTE byObjIdx = pFile->GetByte() ;

			BYTE bychildIndex = pSiegeTerm->Get_ChildCount() ;

			st_CHECK_OBJ_DIE checkInfo ;
			checkInfo.byIdx = bychildIndex ;
			checkInfo.mapNum = mapNum ;
			checkInfo.byObjectIdx = byObjIdx ;

			pSiegeTerm->Insert_CheckObjDie(&checkInfo) ;
		}
		break ;

	default : break ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Check_Use_Item
//	DESC		: The function to check use item.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 30, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Check_Use_Item(CMHFile* pFile)
{
	// Check parameter.
	if(!pFile)
	{
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// Receive a index of current theme.
	int nIndex = 0 ;
	nIndex = m_Theme.size() ;

	if(nIndex <= 0)
	{
		Throw_Error("Empty theme!!", __FUNCTION__) ;
		return ;
	}


	// Check parent theme.
	M_THEME::iterator it ;
	it = m_Theme.find( (BYTE)(nIndex-1) ) ;
	if( it == m_Theme.end() )
	{
		Throw_Error("No parent theme!!", __FUNCTION__) ;
		return ;
	}


	// Receive a current siege map.
	BYTE byMapCount = it->second.Get_MapCount() ;
	CSiegeMap* pCurMap = NULL ;
	pCurMap = it->second.Get_SiegeMapByIdx(byMapCount-1) ;

	if(!pCurMap)
	{
		Throw_Error("No parent map!!", __FUNCTION__) ;
		return ;
	}


	// Receive a step info.
	BYTE byStepCount = pCurMap->Get_StepCount() ;

	CSiegeStep* pCurStep = NULL ;
	pCurStep = pCurMap->Get_StepInfo(byStepCount-1) ;


	// Check step info.
	if(!pCurStep)
	{
		Throw_Error("No parent step!!", __FUNCTION__) ;
		return ;
	}


	// Check step kind.
	BYTE byChildCount = pCurStep->Get_ChildCount() ;
	BYTE byChildKind = pCurStep->Get_ChildKind(byChildCount-1) ;

	switch(byChildKind)
	{
	case e_STEP_ADD_OBJ :
		{
			CSiege_AddObj* pAddInfo = NULL ;
			pAddInfo = pCurStep->Get_AddObjInfo( byChildCount-1 ) ;

			if(!pAddInfo)
			{
				Throw_Error("No parent addobj info!!", __FUNCTION__) ;
				return ;
			}

			BYTE byIndex = pAddInfo->Get_ChildCount() ;

			BYTE byKind = pAddInfo->Get_ChildKind(byIndex-1) ;

			CSiegeTerm* pSiegeTerm = NULL ;

			switch(byKind)
			{
			case e_SIEGE_BASEKIND_START_TERM : 
				{
					pSiegeTerm = pAddInfo->Get_StartTermInfo(byIndex-1) ;

					if(!pSiegeTerm)
					{
						Throw_Error("No start term info!!", __FUNCTION__) ;
						return ;
					}
				}
				break ;

			case e_SIEGE_BASEKIND_END_TERM :
				{
					pSiegeTerm = pAddInfo->Get_EndTermInfo(byIndex-1) ;

					if(!pSiegeTerm)
					{
						Throw_Error("No start term info!!", __FUNCTION__) ;
						return ;
					}
				}
				break ;

			default : break ;
			}

			DWORD  dwItemIdx = pFile->GetDword() ;
			WORD	wUseCount = pFile->GetWord() ;

			BYTE bychildIndex = pSiegeTerm->Get_ChildCount() ;

			st_CHECK_USE_ITEM checkInfo ;
			checkInfo.byIdx = bychildIndex ;
			checkInfo.dwItemIdx = dwItemIdx ;
			checkInfo.wUseCount = wUseCount ;

			pSiegeTerm->Insert_CheckUseItem(&checkInfo) ;
		}
		break ;

	default : break ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: UserMsgParser
//	DESC		: The function to parsing network message from client.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 28, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::UserMsgParser(DWORD dwIndex, char* pMsg, DWORD dwLength)
{
	// Check parameter.
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
	case MP_SIEGE_CHEAT_EXECUTE_STEP_SYN :						Cheat_Execute_Step( dwIndex, pMsg ) ;		break ;

	case MP_SIEGE_CHEAT_REMOVE_ALLMAP_ALLOBJ_SYN :
	case MP_SIEGE_CHEAT_REMOVE_ALLMAP_SPECIFYOBJ_SYN :			Remove_Object( pMsg ) ;						break ;

	case MP_SIEGE_CHEAT_REMOVE_SPECIFYMAP_ALLOBJ_SYN :
	case MP_SIEGE_CHEAT_REMOVE_SPECIFYMAP_SPECIFYOBJ_SYN :		Remove_Object_SpecifyMap( pMsg ) ;			break ;

	//case MP_SIEGE_CHEAT_ADDOBJ_SYN :			Cheat_AddObj(dwIndex, pMsg) ;	break ;

	//case MP_SIEGE_CHEAT_INCREASE_STEP_SYN :		Increase_Step(dwIndex, pmsg->dwObjectID, pMsg) ;	break ;

	//case 0 :	Increase_Step(pMsg) ;	break ;

	//case 1 :	Decrease_Step(pMsg) ;	break ;

	// 081011 LYW --- Protocol : ���� ��ȯ���� ��ȯ ���¸� ���� �ܰ�� �ѱ�� ġƮ ���� �߰�.
	case MP_SIEGE_CHEAT_NEXTSTEP_SYN :		
	case MP_SIEGE_CHEAT_NEXTSTEP_OTHER_AGENT_SYN :				Cheat_Execute_NextStep( dwIndex, pMsg ) ;	break ;

	case MP_SIEGE_CHEAT_ENDSTEP_SYN :
	case MP_SIEGE_CHEAT_ENDSTEP_OTHER_AGENT_SYN :				Cheat_Execute_EndStep( dwIndex, pMsg ) ;	break ;		
		
	default : break ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: ServerMsgPaser
//	DESC		: ������ ������ �޽����� �Ľ��ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 28, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::ServerMsgPaser(DWORD dwIndex, char* pMsg, DWORD dwLength)
{
	// �Լ� ���� Ȯ��.
	if(!pMsg)
	{
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// �⺻ �޽����� ��ȯ.
	MSGBASE* pmsg = NULL ;
	pmsg = (MSGBASE*)pMsg ;

	if(!pmsg)
	{
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// �������� Ȯ��.
	switch(pmsg->Protocol)
	{
	case MP_SIEGERECALL_EXECUTE_STEP_SYN :					Execute_Step( dwIndex, pMsg, dwLength ) ;		break ;	

	case MP_SIEGERECALL_ADDOBJ_ACK :						Add_Obj_Ack( dwIndex, pMsg ) ;					break ;

	case MP_SIEGERECALL_ADDOBJ_NACK :						Add_Obj_Nack( dwIndex, pMsg ) ;					break ;

	case MP_SIEGERECALL_REMOVE_ALLMAP_OBJ_SYN :				Remove_Object( pMsg ) ;							break ;

	case MP_SIEGERECALL_REMOVE_SPECIFYMAP_OBJ_SYN :			Remove_Object_SpecifyMap( pMsg ) ;				break ;

	case MP_SIEGERECALL_REQUEST_OBJINFO_SYN :				Request_ObjInfo_Syn( dwIndex, pMsg ) ;			break ;

	case MP_SIEGERECALL_NOTICE_DIERECALLOBJ_READY_SYN :		Ready_Execute_Command( dwIndex, pMsg ) ;		break ;

	// 081007 LYW --- SiegeRecallMgr : �� ��������, �ش���� ��ȯ���� ����� ��û�ϴ� �۾� �߰�.
	case MP_SIEGERECALL_REQUEST_RECALLCOUNT_SYN :			RequestRecallCount_Syn( dwIndex, pMsg ) ;		break ;
		
	default : break ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Execute_Step
//	DESC		: ���� �� ������ �����ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 31, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Execute_Step(DWORD dwIndex, char* pMsg, DWORD dwLength)
{
	// �Լ� ���� Ȯ��.
	if(!pMsg)
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	// ���� �޽��� ��ȯ.
	MSG_WORD* pmsg = NULL ;
	pmsg = (MSG_WORD*)pMsg ;

	if(!pmsg)
	{
		Throw_Error("Failed to convert a message to original!!", __FUNCTION__) ;
		return ;
	}


	// �׸� �� Ȯ���Ѵ�.
	CSiegeTheme* pTheme = NULL ;
	pTheme = Get_ThemeInfoByThemeIdx(pmsg->wData) ;

	if(!pTheme)
	{
		Throw_Error("Invalid theme info!!", __FUNCTION__) ;
		return ;
	}


	// ���� Ȯ���Ѵ�.
	CSiegeMap* pMap ;
	CSiegeStep* pStep ;
	BYTE byChildCount ;
	BYTE byKind ;

	for( BYTE count = 0 ; count < pTheme->Get_MapCount() ; ++count )
	{
		pMap = NULL ;
		pMap = pTheme->Get_SiegeMapByIdx( count ) ;

		if(!pMap)
		{
			Throw_Error("Invalid map info!!", __FUNCTION__) ;
			continue ;
		}

		// ������ Ȯ���Ѵ�.
		for( BYTE sCount = 0 ; sCount < pMap->Get_StepCount() ; ++sCount )
		{
			pStep = NULL ;
			pStep = pMap->Get_StepInfo( sCount ) ;

			if(!pStep)
			{
				Throw_Error("Invalid step info!!", __FUNCTION__) ;
				continue ;
			}

			// ������ �����Ѵ�.
			byChildCount = 0 ;
			byChildCount = pStep->Get_ChildCount() ;

			for( BYTE byCount = 0 ; byCount < byChildCount ; ++byCount )
			{
				byKind = e_STEP_ADD_OBJ ;
				byKind = pStep->Get_ChildKind(byCount) ;

				switch(byKind)
				{
				case e_STEP_ADD_OBJ :	Execute_AddObj( dwIndex, pStep, byCount ) ;	break ;

				default : break ;
				}
			}
		}

		// �� ������, ��ȯ�� �ε� �Ϸ� �޽����� ���� �Ѵ�.
		MSG_WORD msg ;

		msg.Category	= MP_SIEGERECALL ;
		msg.Protocol	= MP_SIEGERECALL_REQUEST_OBJINFO_ACK ;

		msg.dwObjectID	= 0 ;

		msg.wData		= pMap->Get_MapNum() ;

		g_Network.Broadcast2MapServer( (char*)&msg, sizeof(MSG_WORD) ) ;
	}


	//// �ش� ���� ����ִ��� Ȯ���Ѵ�.
	//g_pServerTable->SetPositionHead();
	//SERVERINFO* pInfo = g_pServerTable->FindMapServer(pmsg->wData2) ;
	//if( !pInfo )
	//{
	//	// 080831 LYW --- SiegeRecallMgr : ��ȯ ������ DB�� ������Ʈ �ϰ�, 
	//	// ����� ���� ��, map ������ ��ȯ ����� �����ϵ��� �Ѵ�.
	//	char txt[128] = {0, } ;

	//	// �������� �ۼ��Ѵ�.
	//	sprintf(txt, "EXEC  %s %d, %d, %d, %d, %d, %d, %d",	MP_SIEGERECALL_INSERT, 
	//		pmsg->wData1, pmsg->wData2, pmsg->wData2, pmsg->wData3, 0, e_CK_ADD_OBJECT, 0) ;

	//	// �������� �����Ѵ�.
	//	g_DB.Query(eQueryType_FreeQuery, eSiegeRecallInsert, 0, txt) ;

	//	return ;
	//}


	// �� ��ȣ�� �����ؼ� ����� ���, �� �ڵ带 ����Ѵ�.
	//CSiegeMap* pMap = NULL ;
	//pMap = pTheme->Get_SiegeMapByMapNum( pmsg->wData2 ) ;

	//if(!pMap)
	//{
	//	Throw_Error("Invalid map info!!", __FUNCTION__) ;
	//	return ;
	//}


	//// ������ Ȯ���Ѵ�.
	//CSiegeStep* pStep = NULL ;
	//pStep = pMap->Get_StepInfo( (BYTE)(pmsg->wData3) ) ;

	//if(!pStep)
	//{
	//	Throw_Error("Invalid step info!!", __FUNCTION__) ;
	//	return ;
	//}


	//// ������ �����Ѵ�.
	//BYTE byChildCount = 0 ;
	//byChildCount = pStep->Get_ChildCount() ;

	//BYTE byKind ;

	//for( BYTE byCount = 0 ; byCount < byChildCount ; ++byCount )
	//{
	//	byKind = e_STEP_ADD_OBJ ;
	//	byKind = pStep->Get_ChildKind(byCount) ;

	//	switch(byKind)
	//	{
	//	case e_STEP_ADD_OBJ :	Execute_AddObj( dwIndex, pStep, byCount ) ;	break ;

	//	default : break ;
	//	}
	//}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Execute_AddObj
//	DESC		: ������Ʈ �߰��� �����ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 31, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Execute_AddObj(DWORD dwIndex, CSiegeStep* pStep, BYTE byChildIdx)
{
	// �Լ� ���� Ȯ��.
	if(!pStep)
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	// ������Ʈ �߰����� Ȯ��.
	CSiege_AddObj* pAddObj = NULL ;
	pAddObj = pStep->Get_AddObjInfo(byChildIdx) ;

	if(!pAddObj)
	{
		Throw_Error("Invalid addobj info!!", __FUNCTION__) ;
		return ;
	}


	// �⺻ ������Ʈ ���� Ȯ��.
	st_SIEGEOBJ* pObjInfo = NULL ;
	pObjInfo = pAddObj->Get_ObjInfo() ;

	if(!pObjInfo)
	{
		Throw_Error("Invalid base obj info!!", __FUNCTION__) ;
		return ;
	}


	// 080916 LYW --- SiegeRecallMgr : ���� �߰� ó�� ����.
	//////// 080831 LYW --- SiegeRecallMgr : ��ȯ ������ DB�� ������Ʈ �ϰ�, 
	//////// ����� ���� ��, map ������ ��ȯ ����� �����ϵ��� �Ѵ�.
	//////char txt[128] = {0, } ;

	//////// �������� �ۼ��Ѵ�.
	//////sprintf(txt, "EXEC  %s %d, %d, %d, %d, %d, %d, %d",	MP_SIEGERECALL_INSERT, 
	//////	pObjInfo->wThemeIdx, pObjInfo->mapNum, pObjInfo->mapNum, pObjInfo->byStepIdx, byChildIdx, e_CK_ADD_OBJECT, 0) ;

	//////// �������� �����Ѵ�.
	//////g_DB.Query(eQueryType_FreeQuery, eSiegeRecallInsert, 0, txt) ;


	// ������Ʈ �߰� ������ ���� Agent�� ���� ��, �� ������ �˸���.
	MSG_SIEGERECALL_OBJINFO msg ;

	msg.Category	= MP_SIEGERECALL ;
	msg.Protocol	= MP_SIEGERECALL_ADDOBJ_SYN ;

	msg.dwObjectID	= dwIndex ;

	pObjInfo->byAddObjIdx	= pAddObj->Get_Index() ;
	pObjInfo->byComKind		= e_CK_ADD_OBJECT ;
	pObjInfo->byComIndex	= 0 ;
	pObjInfo->wParentMap	= 0 ;

	memcpy(&msg.siegeObj, pObjInfo, sizeof(st_SIEGEOBJ)) ;

	g_Network.Broadcast2MapServer( (char*)&msg, sizeof(msg) ) ;


	// �������� / �������� / ���� ��ɾ Ȯ���Ѵ�.
	Execute_TermsAndCommand(pStep, pAddObj) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Execute_TermsAndCommand
//	DESC		: The function to execute terms and command.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 31, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Execute_TermsAndCommand(CSiegeStep* pStep, CSiege_AddObj* pAddObj) 
{
	// Check parameter.
	if( !pAddObj )
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	// Check start terms.
	BYTE byStartTermCount = pAddObj->Get_StartTermCount() ;

	CSiegeTerm* pStartTerm ;
	for( BYTE bySCount = 0 ; bySCount < byStartTermCount ; ++bySCount )
	{
		// Receive start term info.
		pStartTerm = NULL ;
		pStartTerm = pAddObj->Get_StartTermInfo(bySCount) ;

		// Check term info.
		if(!pStartTerm) continue ;

		// Check child kind.
		switch(pStartTerm->Get_ChildKind(bySCount))
		{
		case e_TERM_NONE :		break ;

		case e_TERM_DIE_OBJ :
			{
				// Receive info.
				st_CHECK_OBJ_DIE* pCheckObjInfo = NULL ;
				pCheckObjInfo = pStartTerm->Get_ChkObjDieInfo(bySCount) ;

				// Check obj info.
				if( pCheckObjInfo )
				{
					MSG_SIEGERECALL_CHECK_OBJ_DIE msg ;

					msg.Category		= MP_SIEGERECALL ;
					msg.Protocol		= MP_SIEGERECALL_ADDCHECKOBJDIE_SYN ;

					msg.dwObjectID		= 0 ;

					msg.p_mapNum		= pStep->Get_ParentMapNum() ;
					msg.p_byStepIdx		= pStep->Get_StepIndex() ;
					//090812 pdy ������ Agent�� Map�������� ��ȯObj �ε��������� �߸����ִ� ���� ���� 
					//msg.p_byChildIdx	= pAddObj->Get_Index() ;
					msg.p_dwObjectIdx	= pAddObj->Get_ObjInfo()->dwObjectIdx;

					msg.byCheckKind		= e_SIEGE_BASEKIND_START_TERM ;

					msg.check_mapNum	= pCheckObjInfo->mapNum ;
					msg.check_ObjIdx	= pCheckObjInfo->byObjectIdx ;

					g_Network.Broadcast2MapServer( (char*)&msg, sizeof(msg) ) ;
				}
			}
			break ;

		case e_TERM_USE_ITEM :
			{
				// Receive info.
				st_CHECK_USE_ITEM* pUseItemInfo = NULL ;
				pUseItemInfo = pStartTerm->Get_ChkUseItemInfo(bySCount) ;

				// Check use item info.
				if( pUseItemInfo )
				{
					MSG_SIEGERECALL_CHECK_USE_ITEM msg ;

					msg.Category		= MP_SIEGERECALL ;
					msg.Protocol		= MP_SIEGERECALL_USEITEM_SYN ;

					msg.dwObjectID		= 0 ;

					msg.p_mapNum		= pStep->Get_ParentMapNum() ;
					msg.p_byStepIdx		= pStep->Get_StepIndex() ;
					//090812 pdy ������ Agent�� Map�������� ��ȯObj �ε��������� �߸����ִ� ���� ���� 
					//msg.p_byChildIdx	= pAddObj->Get_Index() ;
					msg.p_dwObjectIdx	= pAddObj->Get_ObjInfo()->dwObjectIdx;

					msg.byCheckKind		= e_SIEGE_BASEKIND_START_TERM ;

					msg.dwItemIdx		= pUseItemInfo->dwItemIdx ;
					msg.wItemCount		= pUseItemInfo->wUseCount ;

					g_Network.Broadcast2MapServer( (char*)&msg, sizeof(msg) ) ;
				}
			}
			break ;

		default : break ;
		}
	}


	// Check end terms.
	BYTE byEndTermCount = pAddObj->Get_EndTermCount() ;

	CSiegeTerm* pEndTerm ;
	for( BYTE byECount = 0 ; byECount < byEndTermCount ; ++byECount )
	{
		// Receive end term info.
		pEndTerm = NULL ;
		pEndTerm = pAddObj->Get_EndTermInfo(byECount) ;

		// Check term info.
		if(!pEndTerm) continue ;

		// Check child kind.
		switch(pEndTerm->Get_ChildKind(byECount))
		{
		case e_TERM_NONE :		break ;

		case e_TERM_DIE_OBJ :
			{
				// Receive info.
				st_CHECK_OBJ_DIE* pCheckObjInfo = NULL ;
				pCheckObjInfo = pEndTerm->Get_ChkObjDieInfo(byECount) ;

				// Check obj info.
				if( pCheckObjInfo )
				{
					MSG_SIEGERECALL_CHECK_OBJ_DIE msg ;

					msg.Category		= MP_SIEGERECALL ;
					msg.Protocol		= MP_SIEGERECALL_ADDCHECKOBJDIE_SYN ;

					msg.dwObjectID		= 0 ;

					msg.p_mapNum		= pStep->Get_ParentMapNum() ;
					msg.p_byStepIdx		= pStep->Get_StepIndex() ;
					//090812 pdy ������ Agent�� Map�������� ��ȯObj �ε��������� �߸����ִ� ���� ���� 
					//msg.p_byChildIdx	= pAddObj->Get_Index() ;
					msg.p_dwObjectIdx	= pAddObj->Get_ObjInfo()->dwObjectIdx;

					msg.byCheckKind		= e_SIEGE_BASEKIND_END_TERM ;

					msg.check_mapNum	= pCheckObjInfo->mapNum ;
					msg.check_ObjIdx	= pCheckObjInfo->byObjectIdx ;

					g_Network.Broadcast2MapServer( (char*)&msg, sizeof(msg) ) ;
				}
			}
			break ;

		case e_TERM_USE_ITEM :
			{
				// Receive info.
				st_CHECK_USE_ITEM* pUseItemInfo = NULL ;
				pUseItemInfo = pEndTerm->Get_ChkUseItemInfo(byECount) ;

				// Check use item info.
				if( pUseItemInfo )
				{
					MSG_SIEGERECALL_CHECK_USE_ITEM msg ;

					msg.Category		= MP_SIEGERECALL ;
					msg.Protocol		= MP_SIEGERECALL_USEITEM_SYN ;

					msg.dwObjectID		= 0 ;

					msg.p_mapNum		= pStep->Get_ParentMapNum() ;
					msg.p_byStepIdx		= pStep->Get_StepIndex() ;
					//090812 pdy ������ Agent�� Map�������� ��ȯObj �ε��������� �߸����ִ� ���� ���� 
					//msg.p_byChildIdx	= pAddObj->Get_Index() ;
					msg.p_dwObjectIdx	= pAddObj->Get_ObjInfo()->dwObjectIdx;

					msg.byCheckKind		= e_SIEGE_BASEKIND_END_TERM ;

					msg.dwItemIdx		= pUseItemInfo->dwItemIdx ;
					msg.wItemCount		= pUseItemInfo->wUseCount ;

					g_Network.Broadcast2MapServer( (char*)&msg, sizeof(msg) ) ;
				}
			}
			break ;

		default : break ;
		}
	}


	// Check command.
	BYTE byCommandCount = pAddObj->Get_CommandCount() ;

	CSiegeCommand* pCommand ;
	for( BYTE byCCount = 0 ; byCCount < byCommandCount ; ++byCCount )
	{
		// Receive start term info.
		pCommand = NULL ;
		pCommand = pAddObj->Get_CommandInfo(byCCount) ;

		// Check term info.
		if(!pCommand) continue ;

		// Check child kind.
		switch(pCommand->Get_ChildKind(byCCount))
		{
		case e_COMMAND_NONE :					break ;

		case e_COMMAND_RECALL_DIE_OBJ :
			{
				// Receive info.
				st_DIE_RECALL_OBJ* pDieRecallObjInfo = NULL ;
				pDieRecallObjInfo = pCommand->Get_DieRecallObjInfo(byCCount) ;

				// Check obj info.
				if( pDieRecallObjInfo )
				{
					MSG_SIEGERECALL_COMMAND_DIE_RECALL_OBJ msg ;

					msg.Category		= MP_SIEGERECALL ;
					msg.Protocol		= MP_SIEGERECALL_ADDCOMMAND_SYN ;

					msg.dwObjectID		= 0 ;

					msg.byCommandKind	= e_COMMAND_RECALL_DIE_OBJ ;

					msg.p_mapNum		= pStep->Get_ParentMapNum() ;
					msg.p_byStepIdx		= pStep->Get_StepIndex() ;
					//090812 pdy ������ Agent�� Map�������� ��ȯObj �ε��������� �߸����ִ� ���� ���� 
					//msg.p_byChildIdx	= pAddObj->Get_Index() ;
					msg.p_dwObjectIdx	= pAddObj->Get_ObjInfo()->dwObjectIdx;
					
					msg.dieRecallObj.byIdx = pDieRecallObjInfo->byIdx ;
                    memcpy(&msg.dieRecallObj.objInfo, &pDieRecallObjInfo->objInfo, sizeof(st_SIEGEOBJ)) ;

					g_Network.Broadcast2MapServer( (char*)&msg, sizeof(msg) ) ;
				}
			}
			break ;

		default : break ;
		}
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Remove_Object
//	DESC		: ��� �� ������ ������Ʈ����, ���/������Ʈ Ÿ�Կ� ���� �����ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 06, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Remove_Object(char* pMsg) 
{
	// �Լ� ���� Ȯ��.
	if(!pMsg)
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	// ���� �޽��� ��ȯ.
	MSG_WORD2* pmsg = NULL ;
	pmsg = (MSG_WORD2*)pMsg ;

	if(!pmsg)
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	// wData1 = ��� / �Ϻ� ������Ʈ ���� ����.
	// wData2 = ���� �� ������Ʈ Ÿ��.



	// ��� ������Ʈ�� �����ϴ� ��ɾ���,
	if(pmsg->wData1 == e_SIEGERECALL_REMOVE_ALL_OBJ)
	{
		MSGBASE msg ;

		msg.Category	= MP_SIEGERECALL ;
		msg.Protocol	= MP_SIEGERECALL_NOTICE_REMOVE_ALLMAP_ALLOBJ_SYN ;

		g_Network.Broadcast2MapServer( (char*)&msg, sizeof(MSGBASE) ) ;
	}
	// ���� �� ������Ʈ Ÿ�Ը� �����ϴ� ����̶��,
	else if(pmsg->wData1 == e_SIEGERECALL_REMOVE_SPECIFY_OBJ)
	{
		MSG_WORD msg ;

		msg.Category	= MP_SIEGERECALL ;
		msg.Protocol	= MP_SIEGERECALL_NOTICE_REMOVE_ALLMAP_SPECIFYOBJ_SYN ;

		// ���� �� ������Ʈ Ÿ���� �����Ѵ�.
		msg.wData		= pmsg->wData2 ;

		g_Network.Broadcast2MapServer( (char*)&msg, sizeof(MSG_WORD) ) ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Remove_Object_SpecifyMap
//	DESC		: ���� �� �� ������ ������Ʈ����, ���/������Ʈ Ÿ�Կ� ���� �����ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 06, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Remove_Object_SpecifyMap(char* pMsg)
{
	// �Լ� ���� Ȯ��.
	if(!pMsg)
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	// ���� �޽��� ��ȯ.
	MSG_WORD3* pmsg = NULL ;
	pmsg = (MSG_WORD3*)pMsg ;

	if(!pmsg)
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	// wData1 = ��� / �Ϻ� ������Ʈ ���� ����.
	// wData2 = ������ ���� �� ��.
	// wData3 = ���� �� ������Ʈ Ÿ��.


	// ��� ������Ʈ�� �����ϴ� ��ɾ���,
	if(pmsg->wData1 == e_SIEGERECALL_REMOVE_ALL_OBJ)
	{
		MSG_WORD msg ;

		msg.Category	= MP_SIEGERECALL ;
		msg.Protocol	= MP_SIEGERECALL_NOTICE_REMOVE_SPECIFYMAP_ALLOBJ_SYN ;

		// ������ ���� �� ���� �����Ѵ�.
		msg.wData		= pmsg->wData2 ;

		g_Network.Broadcast2MapServer( (char*)&msg, sizeof(MSG_WORD) ) ;
	}
	// ���� �� ������Ʈ Ÿ�Ը� �����ϴ� ����̶��,
	else if(pmsg->wData1 == e_SIEGERECALL_REMOVE_SPECIFY_OBJ)
	{
		MSG_WORD2 msg ;

		msg.Category	= MP_SIEGERECALL ;
		msg.Protocol	= MP_SIEGERECALL_NOTICE_REMOVE_SPECIFYMAP_SPECIFYOBJ_SYN ;

		// ������ ���� �� ���� �����Ѵ�.
		msg.wData1		= pmsg->wData2 ;

		// ������ ���� �� ������Ʈ Ÿ���� �����Ѵ�.
		msg.wData2		= pmsg->wData3 ;

		g_Network.Broadcast2MapServer( (char*)&msg, sizeof(MSG_WORD2) ) ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Request_ObjInfo_Syn
//	DESC		: �� ������ ����, ������Ʈ �ε� ������ ���� ó���ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 12, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Request_ObjInfo_Syn(DWORD dwIndex, char* pMsg) 
{
	// 080916 LYW --- SiegeRecallMgr : �ε� ó���� �����Ѵ�.
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
		Throw_Error("Failed to convert a message to original!!", __FUNCTION__) ;
		return ;
	}

	//if( !pmsg->dwObjectID )
	//{
	//	// DB�� ���� �޽����� ������ ����.
	//	char txt[128] = {0, } ;
	//	
	//	sprintf(txt, "EXEC  %s %d",	MP_SIEGERECALL_REMOVE, pmsg->wData) ;
	//	g_DB.Query(eQueryType_FreeQuery, eSiegeRecallRemove, 0, txt, 0) ;

	//	return ;
	//}


	// �ӽ� ���۸� �����Ѵ�.
	char txt[128] = {0, } ;
	// �������� �ۼ��Ѵ�.
	sprintf(txt, "EXEC  %s %d",	MP_SIEGERECALL_LOAD, pmsg->wData) ;
	g_DB.Query(eQueryType_FreeQuery, eSiegeRecallLoad, pmsg->wData, txt, 0) ;

	//// �Լ� ���� Ȯ��.
	//if(!pMsg)
	//{
	//	Throw_Error("Invalid parameter!!", __FUNCTION__) ;
	//	return ;
	//}

	//// ���� �޽����� ��ȯ.
	//MSG_SIEGERECALL_LOADOBJINFO* pmsg = NULL ;
	//pmsg = (MSG_SIEGERECALL_LOADOBJINFO*)pMsg ;

	//if(!pmsg)
	//{
	//	Throw_Error("Failed to convert a message to original!!", __FUNCTION__) ;
	//	return ;
	//}


	//// �׸� �� Ȯ���Ѵ�.
	//CSiegeTheme* pTheme = NULL ;
	//pTheme = Get_ThemeInfoByThemeIdx((WORD)pmsg->ThemeIdx) ;

	//if(!pTheme)
	//{
	//	Throw_Error("Invalid theme info!!", __FUNCTION__) ;
	//	return ;
	//}


	//// ���� Ȯ���Ѵ�.
	//CSiegeMap* pMap = NULL ;
	//pMap = pTheme->Get_SiegeMapByMapNum( pmsg->MapIdx ) ;

	//if(!pMap)
	//{
	//	Throw_Error("Invalid map info!!", __FUNCTION__) ;
	//	return ;
	//}


	//// ������ Ȯ���Ѵ�.
	//CSiegeStep* pStep = NULL ;
	//pStep = pMap->Get_StepInfo( pmsg->StepIdx ) ;

	//if(!pStep)
	//{
	//	Throw_Error("Invalid step info!!", __FUNCTION__) ;
	//	return ;
	//}


	//// �θ�����, �ڽ����� Ȯ���Ѵ�.
	//BYTE byKind = e_STEP_ADD_OBJ ;
	//if(pmsg->ClassType == e_ObjParent)
	//{
	//	byKind = pStep->Get_ChildKind(pmsg->ClassIdx) ;

	//	switch(byKind)
	//	{
	//	case e_STEP_ADD_OBJ :	
	//		{
	//			// ������Ʈ �߰����� Ȯ��.
	//			CSiege_AddObj* pAddObj = NULL ;
	//			pAddObj = pStep->Get_AddObjInfo(pmsg->ClassIdx) ;

	//			if(!pAddObj)
	//			{
	//				Throw_Error("Invalid addobj info!!", __FUNCTION__) ;
	//				return ;
	//			}


	//			// �⺻ ������Ʈ ���� Ȯ��.
	//			st_SIEGEOBJ* pObjInfo = NULL ;
	//			pObjInfo = pAddObj->Get_ObjInfo() ;

	//			if(!pObjInfo)
	//			{
	//				Throw_Error("Invalid base obj info!!", __FUNCTION__) ;
	//				return ;
	//			}


	//			// ������Ʈ �߰� ������ ���� Agent�� ���� ��, �� ������ �˸���.
	//			MSG_SIEGERECALL_OBJINFO msg ;

	//			msg.Category	= MP_SIEGERECALL ;
	//			msg.Protocol	= MP_SIEGERECALL_ADDOBJ_SYN ;

	//			msg.dwObjectID	= dwIndex ;

	//			msg.siegeObj.IsParent	= e_ObjParent ;
	//			msg.siegeObj.byChildIdx	= pmsg->ClassIdx ;

	//			memcpy(&msg.siegeObj, pObjInfo, sizeof(st_SIEGEOBJ)) ;

	//			g_Network.Broadcast2MapServer( (char*)&msg, sizeof(msg) ) ;


	//			// �������� / �������� / ���� ��ɾ Ȯ���Ѵ�.
	//			Execute_TermsAndCommand(pStep, pAddObj) ;
	//		}
	//		break ;

	//	default : break ;
	//	}
	//}
	//else
	//{
	//	byKind = pStep->Get_ChildKind(pmsg->ClassIdx) ;

	//	switch(byKind)
	//	{
	//	case e_STEP_ADD_OBJ :	
	//		{
	//			// ������Ʈ �߰����� Ȯ��.
	//			CSiege_AddObj* pAddObj = NULL ;
	//			pAddObj = pStep->Get_AddObjInfo(pmsg->ClassIdx) ;

	//			if(!pAddObj)
	//			{
	//				Throw_Error("Invalid addobj info!!", __FUNCTION__) ;
	//				return ;
	//			}

	//			st_SIEGEOBJ* pAddObjInfo = NULL ;
	//			pAddObjInfo = pAddObj->Get_ObjInfo() ;

	//			if(!pAddObjInfo)
	//			{
	//				Throw_Error("Invalid addobj base info!!", __FUNCTION__) ;
	//				return ;
	//			}

	//			char szQuery[256] = {0, } ;
	//			sprintf(szQuery, "%s %d, %d, %d, %d, %d, %d", MP_SIEGERECALL_UPDATE, pTheme->Get_ThemeIdx(), 
	//				pMap->Get_MapNum(), pStep->Get_StepIndex(), pAddObj->Get_Index(), e_ObjChild, pmsg->ClassIdx) ;

	//			g_DB.Query( eQueryType_FreeQuery, eSiegeRecallUpdate, 0, szQuery ) ;


	//			CSiegeCommand* pCommand = pAddObj->Get_CommandInfo(pmsg->ClassIdx) ;
	//			if(!pCommand)
	//			{
	//				Throw_Error("Invalid command info!!", __FUNCTION__) ;
	//				return ;
	//			}

	//			st_DIE_RECALL_OBJ* pObjInfo ;
	//			for(BYTE byCount = 0 ; byCount < pCommand->Get_ChildCount() ; ++byCount)
	//			{
	//				pObjInfo = NULL ;
	//				pObjInfo = pCommand->Get_DieRecallObjInfo(byCount) ;

	//				if(!pObjInfo)
	//				{
	//					Throw_Error("Invalid obj info!!", __FUNCTION__) ;
	//					return ;
	//				}


	//				char szQuery[256] = {0, } ;
	//				sprintf(szQuery, "%s %d, %d, %d, %d, %d, %d", MP_SIEGERECALL_UPDATE, pAddObjInfo->wThemeIdx, 
	//				pAddObjInfo->mapNum, pAddObjInfo->byStepIdx, pAddObj->Get_Index(), e_ObjChild, 0) ;

	//				g_DB.Query( eQueryType_FreeQuery, eSiegeRecallUpdate, 0, szQuery ) ;

	//				// ������Ʈ �߰� ������ ���� Agent�� ���� ��, �� ������ �˸���.
	//				MSG_SIEGERECALL_OBJINFO msg ;

	//				msg.Category	= MP_SIEGERECALL ;
	//				msg.Protocol	= MP_SIEGERECALL_ADDOBJ_SYN ;

	//				msg.dwObjectID	= dwIndex ;

	//				msg.siegeObj.IsParent	= e_ObjChild ;
	//				msg.siegeObj.byChildIdx	= byCount ;

	//				memcpy(&msg.siegeObj, &pObjInfo->objInfo, sizeof(st_SIEGEOBJ)) ;

	//				g_Network.Broadcast2MapServer( (char*)&msg, sizeof(msg) ) ;	
	//			}
	//		}
	//		break ;

	//	default : break ;
	//	}
	//}

	////// �Լ� ���� Ȯ��.
	////if(!pMsg)
	////{
	////	Throw_Error("Invalid parameter!!", __FUNCTION__) ;
	////	return ;
	////}

	////// ���� �޽����� ��ȯ.
	////MSG_SIEGERECALL_LOADOBJINFO* pmsg = NULL ;
	////pmsg = (MSG_SIEGERECALL_LOADOBJINFO*)pMsg ;

	////if(!pmsg)
	////{
	////	Throw_Error("Failed to convert a message to original!!", __FUNCTION__) ;
	////	return ;
	////}

	////// ���� ��Ʈ�� Ȯ���Ѵ�.
	////SERVERINFO* pSInfo = NULL ;
	////pSInfo = g_pServerTable->FindMapServer( pmsg->MapIdx ) ;

	////if(!pSInfo)
	////{
	////	Throw_Error("Failed to recieve server info!!", __FUNCTION__) ;
	////	return ;
	////}


	////// �׸� �� Ȯ���Ѵ�.
	////CSiegeTheme* pTheme = NULL ;
	////pTheme = Get_ThemeInfoByThemeIdx((WORD)pmsg->ThemeIdx) ;

	////if(!pTheme)
	////{
	////	Throw_Error("Invalid theme info!!", __FUNCTION__) ;
	////	return ;
	////}


	////// ���� Ȯ���Ѵ�.
	////CSiegeMap* pMap = NULL ;
	////pMap = pTheme->Get_SiegeMapByMapNum( pmsg->MapIdx ) ;

	////if(!pMap)
	////{
	////	Throw_Error("Invalid map info!!", __FUNCTION__) ;
	////	return ;
	////}


	////// ������ Ȯ���Ѵ�.
	////CSiegeStep* pStep = NULL ;
	////pStep = pMap->Get_StepInfo( pmsg->StepIdx ) ;

	////if(!pStep)
	////{
	////	Throw_Error("Invalid step info!!", __FUNCTION__) ;
	////	return ;
	////}


	////// ��ɾ� ������ Ȯ���Ѵ�.
	////switch(pmsg->ComKind)
	////{
	////case e_CK_ADD_OBJECT :
	////	{
	////		// ������Ʈ �߰����� Ȯ��.
	////		CSiege_AddObj* pAddObj = NULL ;
	////		pAddObj = pStep->Get_AddObjInfo(pmsg->AddObjIdx) ;

	////		if(!pAddObj)
	////		{
	////			Throw_Error("Invalid addobj info!!", __FUNCTION__) ;
	////			return ;
	////		}


	////		// �⺻ ������Ʈ ���� Ȯ��.
	////		st_SIEGEOBJ* pObjInfo = NULL ;
	////		pObjInfo = pAddObj->Get_ObjInfo() ;

	////		if(!pObjInfo)
	////		{
	////			Throw_Error("Invalid base obj info!!", __FUNCTION__) ;
	////			return ;
	////		}



	////		// ������Ʈ �߰� ������ ���� Agent�� ���� ��, �� ������ �˸���.
	////		MSG_SIEGERECALL_OBJINFO msg ;

	////		msg.Category	= MP_SIEGERECALL ;
	////		msg.Protocol	= MP_SIEGERECALL_ADDOBJ_SYN ;

	////		msg.dwObjectID	= 0 ;

	////		pObjInfo->byAddObjIdx	= pAddObj->Get_Index() ;
	////		pObjInfo->byComKind		= e_CK_ADD_OBJECT ;
	////		pObjInfo->byComIndex	= 0 ;
	////		pObjInfo->wParentMap	= pmsg->MapIdx ;

	////		memcpy(&msg.siegeObj, pObjInfo, sizeof(st_SIEGEOBJ)) ;

	////		g_Network.Send2Server( pSInfo->dwConnectionIndex, (char*)&msg, sizeof(MSG_SIEGERECALL_OBJINFO) ) ;
	////		//g_Network.Broadcast2MapServer( (char*)&msg, sizeof(msg) ) ;


	////		// �������� / �������� / ���� ��ɾ Ȯ���Ѵ�.
	////		Execute_TermsAndCommand(pStep, pAddObj) ;
	////	}
	////	break ;

	////case e_CK_COMMAND :
	////	{
	////		// ������Ʈ �߰����� Ȯ��.
	////		CSiege_AddObj* pAddObj = NULL ;
	////		pAddObj = pStep->Get_AddObjInfo(pmsg->AddObjIdx) ;

	////		if(!pAddObj)
	////		{
	////			Throw_Error("Invalid addobj info!!", __FUNCTION__) ;
	////			return ;
	////		}

	////		st_SIEGEOBJ* pAddObjInfo = NULL ;
	////		pAddObjInfo = pAddObj->Get_ObjInfo() ;

	////		if(!pAddObjInfo)
	////		{
	////			Throw_Error("Invalid addobj base info!!", __FUNCTION__) ;
	////			return ;
	////		}

	////		CSiegeCommand* pCommand = pAddObj->Get_CommandInfo(pmsg->ComIndex) ;
	////		if(!pCommand)
	////		{
	////			Throw_Error("Invalid command info!!", __FUNCTION__) ;
	////			return ;
	////		}

	////		st_DIE_RECALL_OBJ* pObjInfo ;
	////		for(BYTE byCount = 0 ; byCount < pCommand->Get_ChildCount() ; ++byCount)
	////		{
	////			pObjInfo = NULL ;
	////			pObjInfo = pCommand->Get_DieRecallObjInfo(byCount) ;

	////			if(!pObjInfo)
	////			{
	////				Throw_Error("Invalid obj info!!", __FUNCTION__) ;
	////				continue ;
	////			}


	////			// ������Ʈ �߰� ������ ���� Agent�� ���� ��, �� ������ �˸���.
	////			MSG_SIEGERECALL_OBJINFO msg ;

	////			msg.Category	= MP_SIEGERECALL ;
	////			msg.Protocol	= MP_SIEGERECALL_ADDOBJ_SYN ;

	////			msg.dwObjectID	= 0 ;

	////			msg.wExValue	= pAddObjInfo->mapNum ;

	////			pObjInfo->objInfo.byAddObjIdx	= pAddObj->Get_Index() ;
	////			pObjInfo->objInfo.byComKind		= e_CK_COMMAND ;
	////			pObjInfo->objInfo.byComIndex	= pCommand->Get_Index() ;
	////			pObjInfo->objInfo.wParentMap	= pAddObjInfo->mapNum ;

	////			memcpy(&msg.siegeObj, &pObjInfo->objInfo, sizeof(st_SIEGEOBJ)) ;

	////			g_Network.Send2Server( pSInfo->dwConnectionIndex, (char*)&msg, sizeof(MSG_SIEGERECALL_OBJINFO) ) ;
	////			//g_Network.Broadcast2MapServer( (char*)&msg, sizeof(msg) ) ;	
	////		}
	////	}
	////	break ;
	////}

	////// �� ������, ��ȯ�� �ε� �Ϸ� �޽����� ���� �Ѵ�.
	////MSG_WORD msg ;

	////msg.Category	= MP_SIEGERECALL ;
	////msg.Protocol	= MP_SIEGERECALL_REQUEST_OBJINFO_ACK ;

	////msg.dwObjectID	= 0 ;

	////msg.wData		= pmsg->MapIdx ;

	////g_Network.Send2Server( pSInfo->dwConnectionIndex, (char*)&msg, sizeof(MSG_WORD) ) ;
	//////g_Network.Broadcast2MapServer( (char*)&msg, sizeof(MSG_WORD) ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Result_ObjInfo_Syn
//	DESC		: DB�� ��� �� ������Ʈ ������ �����Ͽ�, ������Ʈ ���ε� ó���� �Ѵ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 13, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Result_ObjInfo_Syn( LPQUERY pData, LPDBMESSAGE pMessage )
{
	WORD wMapNum = 0 ;
	SERVERINFO* pSInfo ;

	// 081015 LYW --- SiegeRecallMgr : ��ȯ ���͸� ������ ������ ���� ���� ���� ����.
	BYTE byCompleteKillCount = 0 ;

	if( pMessage->dwResult == 0 )
	{
		BYTE byThemeIdx = 0 ;

		switch( pMessage->dwID )
		{
		case 52 :
		case 81 :
		case 82 :
		case 83 :	byThemeIdx = 0 ; break ;

		default : return ;
		}

		MSG_WORD msg ;		
		memset( &msg, 0, sizeof(MSG_WORD) ) ;

		msg.wData = byThemeIdx ;
		
		Execute_Step( 0, (char*)&msg, sizeof(MSG_WORD) ) ;
	}


	for( DWORD count = 0 ;  count < pMessage->dwResult ; ++count )
	{
		BYTE byThemeIdx		= (BYTE)atoi((char*)pData[count].Data[eSRLoad_ThemeIdx]) ;
		WORD wMapIdx		= (WORD)atoi((char*)pData[count].Data[eSRLoad_MapIdx]) ;
		WORD wRecallMap		= (WORD)atoi((char*)pData[count].Data[eSRLoad_RecallMap]) ;
		BYTE byStepIdx		= (BYTE)atoi((char*)pData[count].Data[eSRLoad_StepIdx]) ;
		BYTE byAddObjIdx	= (BYTE)atoi((char*)pData[count].Data[eSRLoad_AddObjIdx]) ;
		BYTE byComIndex		= (BYTE)atoi((char*)pData[count].Data[eSRLoad_ComIndex]) ;
		//BYTE byResult		= (BYTE)atoi((char*)pData[count].Data[eSRLoad_Result]) ;
		BYTE byRecallStep	= (BYTE)atoi((char*)pData[count].Data[eSRLoad_RecallStep]) ;
		WORD wRequestMap	= (BYTE)atoi((char*)pData[count].Data[eSRLoad_RequestMap]) ;

		// 081015 LYW --- SiegeRecallMgr : �� �ε���, ������ ������ ��ȯ ������ �޾Ƽ�, 
		// �ش� �� �������� ������ �����Ѵ�.
		if( pMessage->dwID == (DWORD)wRecallMap )
		{
			if( byRecallStep == 4 )
			{
				++byCompleteKillCount ;
			}
		}

		wMapNum = wRequestMap ;

		pSInfo = NULL ;
		pSInfo = g_pServerTable->FindMapServer( wRecallMap ) ;

		if(!pSInfo)
		{
			Throw_Error("Failed to recieve map server info!!", __FUNCTION__) ;
			continue ;
		}


		// �׸� �� Ȯ���Ѵ�.
		CSiegeTheme* pTheme = NULL ;
		pTheme = Get_ThemeInfoByThemeIdx((WORD)byThemeIdx) ;

		if(!pTheme)
		{
			Throw_Error("Invalid theme info!!", __FUNCTION__) ;
			continue ;
		}


		// ���� Ȯ���Ѵ�.
		CSiegeMap* pMap = NULL ;
		pMap = pTheme->Get_SiegeMapByMapNum( wMapIdx ) ;

		if(!pMap)
		{
			Throw_Error("Invalid map info!!", __FUNCTION__) ;
			continue ;
		}


		// ������ Ȯ���Ѵ�.
		CSiegeStep* pStep = NULL ;
		pStep = pMap->Get_StepInfo( byStepIdx ) ;

		if(!pStep)
		{
			Throw_Error("Invalid step info!!", __FUNCTION__) ;
			continue ;
		}


		// ������Ʈ �߰����� Ȯ��.
		CSiege_AddObj* pAddObj = NULL ;
		pAddObj = pStep->Get_AddObjInfo(byAddObjIdx) ;

		if(!pAddObj)
		{
			Throw_Error("Invalid addobj info!!", __FUNCTION__) ;
			continue ;
		}


		// ��ȯ ������ Ȯ���Ѵ�.
		switch(byRecallStep)
		{
		case 0 :
		case 4 : break ;

		case 1 : 
			{
				if( wRequestMap != wRecallMap ) continue ;

				// �⺻ ������Ʈ ���� Ȯ��.
				st_SIEGEOBJ* pObjInfo = NULL ;
				pObjInfo = pAddObj->Get_ObjInfo() ;

				if(!pObjInfo)
				{
					Throw_Error("Invalid base obj info!!", __FUNCTION__) ;
					continue ;
				}



				// ������Ʈ �߰� ������ ���� Agent�� ���� ��, �� ������ �˸���.
				MSG_SIEGERECALL_OBJINFO msg ;

				msg.Category	= MP_SIEGERECALL ;
				msg.Protocol	= MP_SIEGERECALL_ADDOBJ_SYN ;

				msg.dwObjectID	= 0 ;

				pObjInfo->byAddObjIdx	= pAddObj->Get_Index() ;
				pObjInfo->byComKind		= e_CK_ADD_OBJECT ;
				pObjInfo->byComIndex	= 0 ;
				pObjInfo->wParentMap	= wMapIdx ;

				memcpy(&msg.siegeObj, pObjInfo, sizeof(st_SIEGEOBJ)) ;

				g_Network.Send2Server( pSInfo->dwConnectionIndex, (char*)&msg, sizeof(MSG_SIEGERECALL_OBJINFO) ) ;
				//g_Network.Broadcast2MapServer( (char*)&msg, sizeof(msg) ) ;


				// �������� / �������� / ���� ��ɾ Ȯ���Ѵ�.
				Execute_TermsAndCommand(pStep, pAddObj) ;
			}
			break ;

		case 2 :
		case 3 :
			{
				st_SIEGEOBJ* pAddObjInfo = NULL ;
				pAddObjInfo = pAddObj->Get_ObjInfo() ;

				if(!pAddObjInfo)
				{
					Throw_Error("Invalid addobj base info!!", __FUNCTION__) ;
					continue ;
				}

				CSiegeCommand* pCommand = pAddObj->Get_CommandInfo(byComIndex) ;
				if(!pCommand)
				{
					Throw_Error("Invalid command info!!", __FUNCTION__) ;
					continue ;
				}

				st_DIE_RECALL_OBJ* pObjInfo ;
				for(BYTE byCount = 0 ; byCount < pCommand->Get_ChildCount() ; ++byCount)
				{
					pObjInfo = NULL ;
					pObjInfo = pCommand->Get_DieRecallObjInfo(byCount) ;

					if(!pObjInfo)
					{
						Throw_Error("Invalid obj info!!", __FUNCTION__) ;
						continue ;
					}

					if( wRequestMap != pObjInfo->objInfo.mapNum ) continue ;


					// ������Ʈ �߰� ������ ���� Agent�� ���� ��, �� ������ �˸���.
					MSG_SIEGERECALL_OBJINFO msg ;

					msg.Category	= MP_SIEGERECALL ;
					msg.Protocol	= MP_SIEGERECALL_ADDOBJ_SYN ;

					msg.dwObjectID	= 0 ;

					msg.wExValue	= pAddObjInfo->mapNum ;

					pObjInfo->objInfo.byAddObjIdx	= pAddObj->Get_Index() ;
					pObjInfo->objInfo.byComKind		= e_CK_COMMAND ;
					pObjInfo->objInfo.byComIndex	= pCommand->Get_Index() ;
					pObjInfo->objInfo.wParentMap	= pAddObjInfo->mapNum ;

					memcpy(&msg.siegeObj, &pObjInfo->objInfo, sizeof(st_SIEGEOBJ)) ;

					if(byRecallStep == 2)
					{
						pSInfo = NULL ;
						pSInfo = g_pServerTable->FindMapServer( pObjInfo->objInfo.mapNum ) ;

						if(!pSInfo)
						{
							Throw_Error("Failed to recieve map server info!!", __FUNCTION__) ;
							continue ;
						}
					}

					g_Network.Send2Server( pSInfo->dwConnectionIndex, (char*)&msg, sizeof(MSG_SIEGERECALL_OBJINFO) ) ;
					//g_Network.Broadcast2MapServer( (char*)&msg, sizeof(msg) ) ;	
				}
			}
			break ;
		}


		//// ��ɾ� ������ Ȯ���Ѵ�.
		//switch(byComKind)
		//{
		//case e_CK_ADD_OBJECT :
		//	{
		//		// �⺻ ������Ʈ ���� Ȯ��.
		//		st_SIEGEOBJ* pObjInfo = NULL ;
		//		pObjInfo = pAddObj->Get_ObjInfo() ;

		//		if(!pObjInfo)
		//		{
		//			Throw_Error("Invalid base obj info!!", __FUNCTION__) ;
		//			continue ;
		//		}



		//		// ������Ʈ �߰� ������ ���� Agent�� ���� ��, �� ������ �˸���.
		//		MSG_SIEGERECALL_OBJINFO msg ;

		//		msg.Category	= MP_SIEGERECALL ;
		//		msg.Protocol	= MP_SIEGERECALL_ADDOBJ_SYN ;

		//		msg.dwObjectID	= 0 ;

		//		pObjInfo->byAddObjIdx	= pAddObj->Get_Index() ;
		//		pObjInfo->byComKind		= e_CK_ADD_OBJECT ;
		//		pObjInfo->byComIndex	= 0 ;
		//		pObjInfo->wParentMap	= wMapIdx ;

		//		memcpy(&msg.siegeObj, pObjInfo, sizeof(st_SIEGEOBJ)) ;

		//		g_Network.Send2Server( pSInfo->dwConnectionIndex, (char*)&msg, sizeof(MSG_SIEGERECALL_OBJINFO) ) ;
		//		//g_Network.Broadcast2MapServer( (char*)&msg, sizeof(msg) ) ;


		//		// �������� / �������� / ���� ��ɾ Ȯ���Ѵ�.
		//		Execute_TermsAndCommand(pStep, pAddObj) ;
		//	}
		//	break ;

		//case e_CK_COMMAND :
		//	{
		//		st_SIEGEOBJ* pAddObjInfo = NULL ;
		//		pAddObjInfo = pAddObj->Get_ObjInfo() ;

		//		if(!pAddObjInfo)
		//		{
		//			Throw_Error("Invalid addobj base info!!", __FUNCTION__) ;
		//			continue ;
		//		}

		//		CSiegeCommand* pCommand = pAddObj->Get_CommandInfo(byComIndex) ;
		//		if(!pCommand)
		//		{
		//			Throw_Error("Invalid command info!!", __FUNCTION__) ;
		//			continue ;
		//		}

		//		st_DIE_RECALL_OBJ* pObjInfo ;
		//		for(BYTE byCount = 0 ; byCount < pCommand->Get_ChildCount() ; ++byCount)
		//		{
		//			pObjInfo = NULL ;
		//			pObjInfo = pCommand->Get_DieRecallObjInfo(byCount) ;

		//			if(!pObjInfo)
		//			{
		//				Throw_Error("Invalid obj info!!", __FUNCTION__) ;
		//				continue ;
		//			}


		//			// ������Ʈ �߰� ������ ���� Agent�� ���� ��, �� ������ �˸���.
		//			MSG_SIEGERECALL_OBJINFO msg ;

		//			msg.Category	= MP_SIEGERECALL ;
		//			msg.Protocol	= MP_SIEGERECALL_ADDOBJ_SYN ;

		//			msg.dwObjectID	= 0 ;

		//			msg.wExValue	= pAddObjInfo->mapNum ;

		//			pObjInfo->objInfo.byAddObjIdx	= pAddObj->Get_Index() ;
		//			pObjInfo->objInfo.byComKind		= e_CK_COMMAND ;
		//			pObjInfo->objInfo.byComIndex	= pCommand->Get_Index() ;
		//			pObjInfo->objInfo.wParentMap	= pAddObjInfo->mapNum ;

		//			memcpy(&msg.siegeObj, &pObjInfo->objInfo, sizeof(st_SIEGEOBJ)) ;

		//			g_Network.Send2Server( pSInfo->dwConnectionIndex, (char*)&msg, sizeof(MSG_SIEGERECALL_OBJINFO) ) ;
		//			//g_Network.Broadcast2MapServer( (char*)&msg, sizeof(msg) ) ;	
		//		}
		//	}
		//	break ;
		//}
	}


	// �� ������, ��ȯ�� �ε� �Ϸ� �޽����� ���� �Ѵ�.
	{
		MSG_WORD msg ;

		msg.Category	= MP_SIEGERECALL ;
		msg.Protocol	= MP_SIEGERECALL_REQUEST_OBJINFO_ACK ;

		msg.dwObjectID	= 0 ;

		pSInfo = NULL ;

		if( pMessage->dwResult > 0 )
		{
			msg.wData		= wMapNum ;
			pSInfo = g_pServerTable->FindMapServer( wMapNum ) ;
		}
		else
		{
			msg.wData		= (WORD)pMessage->dwID ;
			pSInfo = g_pServerTable->FindMapServer( (WORD)pMessage->dwID ) ;
		}

		if(!pSInfo)
		{
			//Throw_Error("Failed to recieve map server info!!", __FUNCTION__) ;
			return ;
		}

		g_Network.Send2Server( pSInfo->dwConnectionIndex, (char*)&msg, sizeof(MSG_WORD) ) ;
		//g_Network.Broadcast2MapServer( (char*)&msg, sizeof(MSG_WORD) ) ;
	}


	// 081015 LYW --- SiegeRecallMgr : ��ȯ ���͸� ������ ������ ���� �� ������ �����Ѵ�.
	if( byCompleteKillCount )
	{
		MSG_BYTE msg ;

		msg.Category	= MP_SIEGERECALL ;
		msg.Protocol	= MP_SIEGERECALL_LOAD_RECALLCOUNT_ACK ;

		msg.dwObjectID	= 0 ;

		msg.bData		= byCompleteKillCount ;

		pSInfo = NULL ;

		if( pMessage->dwResult > 0 )
		{
			pSInfo = g_pServerTable->FindMapServer( wMapNum ) ;
		}
		else
		{
			pSInfo = g_pServerTable->FindMapServer( (WORD)pMessage->dwID ) ;
		}

		if(!pSInfo) return ;

		g_Network.Send2Server( pSInfo->dwConnectionIndex, (char*)&msg, sizeof(MSG_BYTE) ) ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: RSiegeRecallUpdate
//	DESC		: DB�� ��� �� ������Ʈ ������ �����Ͽ�, ������Ʈ ������Ʈ ��� ó���� �Ѵ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: September 2, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::RSiegeRecallUpdate( LPQUERY pData, LPDBMESSAGE pMessage ) 
{
	MAPTYPE mapNum = 0 ; 

	for( DWORD count = 0 ;  count < pMessage->dwResult ; ++count )
	{
		BYTE byThemeIdx		= (BYTE)atoi((char*)pData[count].Data[eSRLoad_ThemeIdx]) ;
		WORD wMapIdx		= (WORD)atoi((char*)pData[count].Data[eSRLoad_MapIdx]) ;
		WORD wRecallMap		= (WORD)atoi((char*)pData[count].Data[eSRLoad_RecallMap]) ;
		BYTE byStepIdx		= (BYTE)atoi((char*)pData[count].Data[eSRLoad_StepIdx]) ;
		BYTE byAddObjIdx	= (BYTE)atoi((char*)pData[count].Data[eSRLoad_AddObjIdx]) ;
		BYTE byComKind		= (BYTE)atoi((char*)pData[count].Data[eSRLoad_ComKind]) ;
		BYTE byComIndex		= (BYTE)atoi((char*)pData[count].Data[eSRLoad_ComIndex]) ;

		mapNum = wRecallMap ;


		// �׸� �� Ȯ���Ѵ�.
		CSiegeTheme* pTheme = NULL ;
		pTheme = Get_ThemeInfoByThemeIdx((WORD)byThemeIdx) ;

		if(!pTheme)
		{
			Throw_Error("Invalid theme info!!", __FUNCTION__) ;
			continue ;
		}


		// ���� Ȯ���Ѵ�.
		CSiegeMap* pMap = NULL ;
		pMap = pTheme->Get_SiegeMapByMapNum( wMapIdx ) ;

		if(!pMap)
		{
			Throw_Error("Invalid map info!!", __FUNCTION__) ;
			continue ;
		}


		// ������ Ȯ���Ѵ�.
		CSiegeStep* pStep = NULL ;
		pStep = pMap->Get_StepInfo( byStepIdx ) ;

		if(!pStep)
		{
			Throw_Error("Invalid step info!!", __FUNCTION__) ;
			continue ;
		}


		// ��ɾ� ������ Ȯ���Ѵ�.
		switch(byComKind)
		{
		case e_CK_ADD_OBJECT :
			{
				// ������Ʈ �߰����� Ȯ��.
				CSiege_AddObj* pAddObj = NULL ;
				pAddObj = pStep->Get_AddObjInfo(byAddObjIdx) ;

				if(!pAddObj)
				{
					Throw_Error("Invalid addobj info!!", __FUNCTION__) ;
					continue ;
				}


				// �⺻ ������Ʈ ���� Ȯ��.
				st_SIEGEOBJ* pObjInfo = NULL ;
				pObjInfo = pAddObj->Get_ObjInfo() ;

				if(!pObjInfo)
				{
					Throw_Error("Invalid base obj info!!", __FUNCTION__) ;
					continue ;
				}



				// ������Ʈ �߰� ������ ���� Agent�� ���� ��, �� ������ �˸���.
				MSG_SIEGERECALL_OBJINFO msg ;

				msg.Category	= MP_SIEGERECALL ;
				msg.Protocol	= MP_SIEGERECALL_ADDOBJ_SYN ;

				msg.dwObjectID	= 0 ;

				pObjInfo->byAddObjIdx	= pAddObj->Get_Index() ;
				pObjInfo->byComKind		= e_CK_ADD_OBJECT ;
				pObjInfo->byComIndex	= 0 ;
				pObjInfo->wParentMap	= wMapIdx ;

				memcpy(&msg.siegeObj, pObjInfo, sizeof(st_SIEGEOBJ)) ;

				g_Network.Broadcast2MapServer( (char*)&msg, sizeof(msg) ) ;


				// �������� / �������� / ���� ��ɾ Ȯ���Ѵ�.
				Execute_TermsAndCommand(pStep, pAddObj) ;
			}
			break ;

		case e_CK_COMMAND :
			{
				// ������Ʈ �߰����� Ȯ��.
				CSiege_AddObj* pAddObj = NULL ;
				pAddObj = pStep->Get_AddObjInfo(byAddObjIdx) ;

				if(!pAddObj)
				{
					Throw_Error("Invalid addobj info!!", __FUNCTION__) ;
					continue ;
				}

				st_SIEGEOBJ* pAddObjInfo = NULL ;
				pAddObjInfo = pAddObj->Get_ObjInfo() ;

				if(!pAddObjInfo)
				{
					Throw_Error("Invalid addobj base info!!", __FUNCTION__) ;
					continue ;
				}

				CSiegeCommand* pCommand = pAddObj->Get_CommandInfo(byComIndex) ;
				if(!pCommand)
				{
					Throw_Error("Invalid command info!!", __FUNCTION__) ;
					continue ;
				}

				st_DIE_RECALL_OBJ* pObjInfo ;
				for(BYTE byCount = 0 ; byCount < pCommand->Get_ChildCount() ; ++byCount)
				{
					pObjInfo = NULL ;
					pObjInfo = pCommand->Get_DieRecallObjInfo(byCount) ;

					if(!pObjInfo)
					{
						Throw_Error("Invalid obj info!!", __FUNCTION__) ;
						continue ;
					}


					// ������Ʈ �߰� ������ ���� Agent�� ���� ��, �� ������ �˸���.
					MSG_SIEGERECALL_OBJINFO msg ;

					msg.Category	= MP_SIEGERECALL ;
					msg.Protocol	= MP_SIEGERECALL_ADDOBJ_SYN ;

					msg.dwObjectID	= 0 ;

					msg.wExValue	= pAddObjInfo->mapNum ;

					pObjInfo->objInfo.byAddObjIdx	= pAddObj->Get_Index() ;
					pObjInfo->objInfo.byComKind		= e_CK_COMMAND ;
					pObjInfo->objInfo.byComIndex	= pCommand->Get_Index() ;
					pObjInfo->objInfo.wParentMap	= pAddObjInfo->mapNum ;

					memcpy(&msg.siegeObj, &pObjInfo->objInfo, sizeof(st_SIEGEOBJ)) ;

					g_Network.Broadcast2MapServer( (char*)&msg, sizeof(msg) ) ;	
				}
			}
			break ;
		}
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: RSiegeRecallInsert
//	DESC		: DB�� ��ȯ ������Ʈ ���� �߰� ��, �� ������ ��ȯ ��ɾ ������ ó���� �Ѵ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: September 4, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::RSiegeRecallInsert( LPQUERY pData, LPDBMESSAGE pMessage ) 
{
	// �Լ� ���� Ȯ��.
	if( !pData )
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	// �߰� ����� �޴´�.
	for( DWORD count = 0 ;  count < pMessage->dwResult ; ++count )
	{
		BYTE byThemeIdx		= (BYTE)atoi((char*)pData[count].Data[eSRLoad_ThemeIdx]) ;
		WORD wMapIdx		= (WORD)atoi((char*)pData[count].Data[eSRLoad_MapIdx]) ;
		BYTE byStepIdx		= (BYTE)atoi((char*)pData[count].Data[eSRLoad_StepIdx]) ;
		BYTE byAddObjIdx	= (BYTE)atoi((char*)pData[count].Data[eSRLoad_AddObjIdx]) ;

		CSiegeTheme* pTheme = Get_ThemeInfoByThemeIdx(byThemeIdx) ;

		if(!pTheme)
		{
			Throw_Error("Invalid theme info!!", __FUNCTION__) ;
			continue ;
		}


		// �� ��ȣ�� �����ؼ� ����� ���, �� �ڵ带 ����Ѵ�.
		CSiegeMap* pMap = NULL ;
		pMap = pTheme->Get_SiegeMapByMapNum( wMapIdx ) ;

		if(!pMap)
		{
			Throw_Error("Invalid map info!!", __FUNCTION__) ;
			continue ;
		}


		// ������ Ȯ���Ѵ�.
		CSiegeStep* pStep = NULL ;
		pStep = pMap->Get_StepInfo( (BYTE)(byStepIdx) ) ;

		if(!pStep)
		{
			Throw_Error("Invalid step info!!", __FUNCTION__) ;
			return ;
		}


		// ������ �����Ѵ�.
		BYTE byChildCount = 0 ;
		byChildCount = pStep->Get_ChildCount() ;

		BYTE byKind = e_STEP_ADD_OBJ ;
		byKind = pStep->Get_ChildKind(byAddObjIdx) ;

		if( byKind == e_STEP_ADD_OBJ )
		{
			// ������Ʈ �߰����� Ȯ��.
			CSiege_AddObj* pAddObj = NULL ;
			pAddObj = pStep->Get_AddObjInfo(byAddObjIdx) ;

			if(!pAddObj)
			{
				Throw_Error("Invalid addobj info!!", __FUNCTION__) ;
				continue ;
			}


			// �⺻ ������Ʈ ���� Ȯ��.
			st_SIEGEOBJ* pObjInfo = NULL ;
			pObjInfo = pAddObj->Get_ObjInfo() ;

			if(!pObjInfo)
			{
				Throw_Error("Invalid base obj info!!", __FUNCTION__) ;
				continue ;
			}

			
			// ������Ʈ �߰� ������ ���� Agent�� ���� ��, �� ������ �˸���.
			MSG_SIEGERECALL_OBJINFO msg ;

			msg.Category	= MP_SIEGERECALL ;
			msg.Protocol	= MP_SIEGERECALL_ADDOBJ_SYN ;

			msg.dwObjectID	= 0 ;

			memcpy( &msg.siegeObj, pObjInfo, sizeof(st_SIEGEOBJ) ) ;

			g_Network.Broadcast2MapServer( (char*)&msg, sizeof(msg) ) ;
		}
	}
}





////-------------------------------------------------------------------------------------------------
////	NAME		: RSiegeRecallRemove
////	DESC		: DB�� ��ȯ ������Ʈ ���� ���� ��, �� ������ ��ȯ ���� ���� ����� ������ ó���� �Ѵ�.
////	PROGRAMMER	: Yongs Lee
////	DATE		: September 7, 2008
////-------------------------------------------------------------------------------------------------
//void CSiegeRecallMgr::RSiegeRecallRemove( LPQUERY pData, LPDBMESSAGE pMessage )
//{
//	// �Լ� ���� Ȯ��.
//	if( !pData )
//	{
//		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
//		return ;
//	}
//
//
//	// �߰� ����� �޴´�.
//	for( DWORD count = 0 ;  count < pMessage->dwResult ; ++count )
//	{
//		BYTE byThemeIdx		= (BYTE)atoi((char*)pData[count].Data[eSRLoad_ThemeIdx]) ;
//		WORD wMapIdx		= (WORD)atoi((char*)pData[count].Data[eSRLoad_MapIdx]) ;
//		WORD wRecallMap		= (WORD)atoi((char*)pData[count].Data[eSRLoad_RecallMap]) ;
//		BYTE byStepIdx		= (BYTE)atoi((char*)pData[count].Data[eSRLoad_StepIdx]) ;
//		BYTE byAddObjIdx	= (BYTE)atoi((char*)pData[count].Data[eSRLoad_AddObjIdx]) ;
//		BYTE byComKind		= (BYTE)atoi((char*)pData[count].Data[eSRLoad_ComKind]) ;
//		BYTE byComIdx		= (BYTE)atoi((char*)pData[count].Data[eSRLoad_ComIndex]) ;
//
//
//		// �׸� �� Ȯ���Ѵ�.
//		CSiegeTheme* pTheme = NULL ;
//		pTheme = Get_ThemeInfoByThemeIdx(byThemeIdx) ;
//
//		if(!pTheme)
//		{
//			Throw_Error("Invalid theme info!!", __FUNCTION__) ;
//			continue ;
//		}
//
//
//		// �� ��ȣ�� �����ؼ� ����� ���, �� �ڵ带 ����Ѵ�.
//		CSiegeMap* pMap = NULL ;
//		pMap = pTheme->Get_SiegeMapByMapNum( wMapIdx ) ;
//
//		if(!pMap)
//		{
//			Throw_Error("Invalid map info!!", __FUNCTION__) ;
//			continue ;
//		}
//
//
//		// ������ Ȯ���Ѵ�.
//		CSiegeStep* pStep = NULL ;
//		pStep = pMap->Get_StepInfo( (BYTE)(byStepIdx) ) ;
//
//		if(!pStep)
//		{
//			Throw_Error("Invalid step info!!", __FUNCTION__) ;
//			return ;
//		}
//
//
//		// ������ �����Ѵ�.
//		BYTE byChildCount = 0 ;
//		byChildCount = pStep->Get_ChildCount() ;
//
//		BYTE byKind = e_STEP_ADD_OBJ ;
//		byKind = pStep->Get_ChildKind(byAddObjIdx) ;
//
//		if( byKind == e_STEP_ADD_OBJ )
//		{
//			// ������Ʈ �߰����� Ȯ��.
//			CSiege_AddObj* pAddObj = NULL ;
//			pAddObj = pStep->Get_AddObjInfo(byAddObjIdx) ;
//
//			if(!pAddObj)
//			{
//				Throw_Error("Invalid addobj info!!", __FUNCTION__) ;
//				continue ;
//			}
//
//
//			// �⺻ ������Ʈ ���� Ȯ��.
//			st_SIEGEOBJ* pObjInfo = NULL ;
//			pObjInfo = pAddObj->Get_ObjInfo() ;
//
//			if(!pObjInfo)
//			{
//				Throw_Error("Invalid base obj info!!", __FUNCTION__) ;
//				continue ;
//			}
//
//			
//			// ������Ʈ �߰� ������ ���� Agent�� ���� ��, �� ������ �˸���.
//			MSG_SIEGERECALL_OBJINFO msg ;
//
//			msg.Category	= MP_SIEGERECALL ;
//			msg.Protocol	= MP_SIEGERECALL_ADDOBJ_SYN ;
//
//			msg.dwObjectID	= 0 ;
//
//			memcpy( &msg.siegeObj, pObjInfo, sizeof(st_SIEGEOBJ) ) ;
//
//			g_Network.Broadcast2MapServer( (char*)&msg, sizeof(msg) ) ;
//		}
//	}
//}





//-------------------------------------------------------------------------------------------------
//	NAME		: Add_Obj_Ack
//	DESC		: ������Ʈ �߰� ���� �޽��� ó���� �ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 11, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Add_Obj_Ack(DWORD dwIndex, char* pMsg)
{
	// �Լ� ���� Ȯ��.
	if(!pMsg)
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}

	// ���� �޽����� ��ȯ.
	MSG_SIEGERECALL_OBJINFO* pmsg = NULL ;
	pmsg = (MSG_SIEGERECALL_OBJINFO*)pMsg ;

	if(!pmsg)
	{
		Throw_Error("Failed to convert a message to original!!", __FUNCTION__) ;
		return ;
	}


	// Ŭ���̾�Ʈ�� ���� ������ �����Ѵ�.
	g_Network.Send2User( dwIndex, (char*)pmsg, sizeof(MSG_SIEGERECALL_OBJINFO) ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Add_Obj_Nack
//	DESC		: ������Ʈ �߰� ���� �޽��� ó���� �ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 11, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Add_Obj_Nack(DWORD dwIndex, char* pMsg)
{
	// �Լ� ���� Ȯ��.
	if(!pMsg)
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}

	// ���� �޽����� ��ȯ.
	MSG_SIEGERECALL_OBJINFO* pmsg = NULL ;
	pmsg = (MSG_SIEGERECALL_OBJINFO*)pMsg ;

	if(!pmsg)
	{
		Throw_Error("Failed to convert a message to original!!", __FUNCTION__) ;
		return ;
	}

	// Ŭ���̾�Ʈ�� ���� ������ �����Ѵ�.
	g_Network.Send2User( dwIndex, (char*)pmsg, sizeof(MSG_SIEGERECALL_OBJINFO) ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Ready_Execute_Command
//	DESC		: The function to execute command.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 04, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Ready_Execute_Command(DWORD dwIndex, char* pMsg) 
{
	// Check pararmeter.
	if(!pMsg)
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	// Convert a message.
	MSG_WORD3* pmsg = NULL ;
	pmsg = (MSG_WORD3*)pMsg ;

	if(!pmsg)
	{
		Throw_Error("Failed to convert a messsage to original!!", __FUNCTION__) ;
		return ;
	}


	// Check theme info.	
	M_THEME::iterator it_theme ;

	CSiegeMap* pMap = NULL ;

	for( it_theme = m_Theme.begin() ; it_theme != m_Theme.end() ; ++it_theme )
	{
		// Check map info.
		pMap = NULL ;
		pMap = it_theme->second.Get_SiegeMapByMapNum( pmsg->wData1 ) ;

		if(!pMap)
		{
			Throw_Error("Invalid map info!!", __FUNCTION__) ;
			return ;
		}
	}


	// Check step info.
	CSiegeStep* pStep = NULL ;
	pStep = pMap->Get_StepInfo( (BYTE)pmsg->wData2 ) ;

	if(!pStep)
	{
		Throw_Error("Invalid step info!!", __FUNCTION__) ;
		return ;
	}


	// Check child kind.
	BYTE byKind = pStep->Get_ChildKind( (BYTE)pmsg->wData3 ) ;

	switch(byKind)
	{
	case e_STEP_ADD_OBJ :
		{
			// Receive add obj info.
			CSiege_AddObj* pAddObj = NULL ;
			pAddObj = pStep->Get_AddObjInfo( (BYTE)pmsg->wData3 ) ;

			if(!pAddObj)
			{
				Throw_Error("Invalid add obj info!!", __FUNCTION__) ;
				return ;
			}

			// Check command info.
			BYTE byCommandCount = pAddObj->Get_CommandCount() ;

			CSiegeCommand* pExeCommand ;
			for( BYTE byCCount = 0 ; byCCount < byCommandCount ; ++byCCount )
			{
				pExeCommand = NULL ;
				pExeCommand = pAddObj->Get_CommandInfo(byCCount) ;

				if(!pExeCommand)
				{
					Throw_Error("Invalid exe command info!!", __FUNCTION__) ;
					continue ;
				}

				BYTE byChildCount = pExeCommand->Get_ChildCount() ;

				for( BYTE byCount = 0 ; byCount < byChildCount ; ++byCount )
				{
					switch(pExeCommand->Get_ChildKind(byCount))
					{
					case e_COMMAND_RECALL_DIE_OBJ : 
						{
							st_DIE_RECALL_OBJ* pInfo = NULL ;
							pInfo = pExeCommand->Get_DieRecallObjInfo( byCount) ;

							if(!pInfo)
							{
								Throw_Error("Invalid die recall obj info!!", __FUNCTION__) ;
								return ;
							}

							// 080916 LYW --- ��ȯ�� ��� ó�� ����.
							//////char szQuery[256] = {0, } ;
							//////sprintf(szQuery, "%s %d, %d, %d, %d, %d, %d, %d", MP_SIEGERECALL_UPDATE, pMap->Get_ThemeIdx(),
							//////	pMap->Get_MapNum(), pInfo->objInfo.mapNum, pStep->Get_StepIndex(), pAddObj->Get_Index(), e_CK_COMMAND, byCount) ;

							//////g_DB.Query( eQueryType_FreeQuery, eSiegeRecallUpdate, 0, szQuery ) ;

							// Broadcast add object info.
							MSG_SIEGERECALL_OBJINFO msg ;

							msg.Category	= MP_SIEGERECALL ;
							msg.Protocol	= MP_SIEGERECALL_ADDOBJ_SYN ;

							msg.dwObjectID	= 0 ;

							msg.wExValue	= pAddObj->Get_ObjInfo()->mapNum ;

							pInfo->objInfo.byAddObjIdx	= pAddObj->Get_Index() ;
							pInfo->objInfo.byComKind	= e_CK_COMMAND ;
							pInfo->objInfo.byComIndex	= pExeCommand->Get_Index() ;
							pInfo->objInfo.wParentMap	= pAddObj->Get_ObjInfo()->mapNum ;

							memcpy(&msg.siegeObj, &pInfo->objInfo, sizeof(st_SIEGEOBJ)) ;

							g_Network.Broadcast2MapServer( (char*)&msg, sizeof(msg) ) ;
						}
						break ;

					default : break ;
					}
				}
			}
		}
		break ;

	default : break ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Throw_Error
//	DESC		: The function to process error message.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 24, 2008
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
//	DATE		: July 24, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::WriteLog(char* pMsg)
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





//-------------------------------------------------------------------------------------------------
//	NAME		: Cheat_Execute_Step
//	DESC		: The function to execute step by cheat code.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 31, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Cheat_Execute_Step(DWORD dwIndex, char* pMsg)
{
	// Check pararmeter.
	if(!pMsg)
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	// Convert a message.
	MSG_WORD3* pmsg = NULL ;
	pmsg = (MSG_WORD3*)pMsg ;

	if(!pmsg)
	{
		Throw_Error("Failed to convert a message!!", __FUNCTION__) ;
		return ;
	}


	// Check theme.
	CSiegeTheme* pTheme = NULL ;
	pTheme = Get_ThemeInfoByThemeIdx(pmsg->wData1) ;

	if(!pTheme)
	{
		Throw_Error("Invalid theme info!!", __FUNCTION__) ;
		return ;
	}


	// Check map.
	CSiegeMap* pMap = NULL ;
	pMap = pTheme->Get_SiegeMapByMapNum( pmsg->wData2 ) ;

	if(!pMap)
	{
		Throw_Error("Invalid map info!!", __FUNCTION__) ;
		return ;
	}


	// Check step.
	CSiegeStep* pStep = NULL ;
	pStep = pMap->Get_StepInfo( (BYTE)(pmsg->wData3) ) ;

	if(!pStep)
	{
		Throw_Error("Invalid step info!!", __FUNCTION__) ;
		return ;
	}


	// Execute step.
	BYTE byChildCount = 0 ;
	byChildCount = pStep->Get_ChildCount() ;

	BYTE byKind ;

	for( BYTE byCount = 0 ; byCount < byChildCount ; ++byCount )
	{
		byKind = e_STEP_ADD_OBJ ;
		byKind = pStep->Get_ChildKind(byCount) ;

		switch(byKind)
		{
		case e_STEP_ADD_OBJ :	Execute_AddObj( dwIndex, pStep, byCount ) ;	break ;

		default : break ;
		}
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: RequestRecallCount_Syn
//	DESC		: �� ��������, �ش���� ��ȯ���� ����� ��û�ϴ� �۾� �߰�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: October 7, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::RequestRecallCount_Syn( DWORD dwIndex, void* pMsg )
{
	// �Լ� ���ڸ� Ȯ���Ѵ�.
	if( !pMsg ) return ;


	// ���� �޽��� ��ȯ.
	MSG_WORD* pmsg = NULL ;
	pmsg = (MSG_WORD*)pMsg ;

	if( !pmsg ) return ;


	// ī��Ʈ ������ �ʿ��� ���� ���� ����.
	WORD wRecallCount = 0 ;

	M_THEME::iterator it_theme ;		// �׸� ����

	CSiegeMap* pMap ;					// �� ����
	BYTE byMapCount ;

	CSiegeStep* pStep ;					// ���� ����
	BYTE byStepCount ;

	CSiege_AddObj* pAddObj ;			// ������Ʈ ����
	BYTE byAddObjCount ;

	st_SIEGEOBJ* pInfo ;				// ������Ʈ ��ȯ ���� ����

	CSiegeCommand* pComm ;				// ������Ʈ�� ��ɾ� ����
	BYTE byCommCount ;

	st_DIE_RECALL_OBJ* pRecallInfo ;	// ������Ʈ ��ɾ��� ��ȯ ���� ����


	// ��û�� ���� �ʰ� ���� �� ��ȯ ī��Ʈ�� �����Ѵ�.
	for( it_theme = m_Theme.begin() ; it_theme != m_Theme.end() ; ++it_theme )
	{
		// �� �׸��� �ش��ϴ� �� ������ Ȯ���Ѵ�.
		byMapCount = it_theme->second.Get_MapCount() ;

		for( BYTE count1 = 0 ; count1 < byMapCount ; ++count1 )
		{
			pMap = NULL ;
			pMap = it_theme->second.Get_SiegeMapByIdx( count1 ) ;

			if( !pMap ) continue ;

			// �� �ʿ� �ش��ϴ� ���� ������ Ȯ���Ѵ�.
			byStepCount = pMap->Get_StepCount() ;

			for( BYTE count2 = 0 ;  count2 < byStepCount ; ++count2 )
			{
				pStep = NULL ;
				pStep = pMap->Get_StepInfo( count2 ) ;

				if( !pStep ) continue ;

				// �� ���ܿ� �ش��ϴ� ������Ʈ�� Ȯ���Ѵ�.
				byAddObjCount = pStep->Get_ChildCount() ;

				for( BYTE count3 = 0 ; count3 < byAddObjCount ; ++count3 )
				{
					pAddObj = NULL ;
					pAddObj = pStep->Get_AddObjInfo( count3 ) ;

					if( !pAddObj ) continue ;

					// ������Ʈ�� ��ȯ ������ Ȯ���Ѵ�.
					pInfo = NULL ;
					pInfo = pAddObj->Get_ObjInfo() ;

					if( !pInfo ) continue ;

					//if( pInfo->mapNum != pmsg->wData ) continue ;

					if( pInfo->mapNum == pmsg->wData ) 
					{
						++wRecallCount ;
					}

					// �� ������Ʈ�� ��ɾ� �κ��� Ȯ���Ѵ�.
					byCommCount = pAddObj->Get_CommandCount() ;

					for( BYTE count4 = 0 ; count4 < byCommCount ; ++count4 )
					{
						pComm = NULL ;
						pComm = pAddObj->Get_CommandInfo( count4 ) ;

						if( !pComm ) continue ;

						// ������Ʈ�� ����Ͽ��� ��, �߻��ϴ� ��ȯ ��ɾ Ȯ���Ѵ�.
						pRecallInfo = NULL ;
						pRecallInfo = pComm->Get_DieRecallObjInfo( count4 ) ;

						if( !pRecallInfo ) continue ;

						if( pRecallInfo->objInfo.mapNum != pmsg->wData ) continue ;

						++wRecallCount ;
					}
				}
			}
		}
	}

	// ��ȯ ī��Ʈ�� �ʼ����� �����Ѵ�.
	MSG_WORD2 msg ;

	msg.Category	= MP_SIEGERECALL ;
	msg.Protocol	= MP_SIEGERECALL_REQUEST_RECALLCOUNT_ACK ;

	msg.dwObjectID	= 0 ;

	msg.wData1		= pmsg->wData ;
	msg.wData2		= wRecallCount ;

	g_Network.Send2Server( dwIndex, (char*)&msg, sizeof( MSG_WORD2 ) ) ;
}





////-------------------------------------------------------------------------------------------------
////	NAME		: Cheat_AddObj
////	DESC		: The function to add object by cheat code.
////	PROGRAMMER	: Yongs Lee
////	DATE		: July 28, 2008
////-------------------------------------------------------------------------------------------------
//void CSiegeRecallMgr::Cheat_AddObj(DWORD dwIndex, void* pMsg)
//{
//	// Check parameter.
//	if(!pMsg)
//	{
//		Throw_Error( "Invalid a message parameter!!", __FUNCTION__) ;
//		return ;
//	}
//
//
//	// Convert a message.
//	MSG_SIEGERECALL_OBJINFO* pmsg = NULL ;
//	pmsg = (MSG_SIEGERECALL_OBJINFO*)pMsg ;
//
//	if(!pmsg)
//	{
//		Throw_Error( "Failed to convert a message!!", __FUNCTION__) ;
//		return ;
//	}
//
//	MSG_SIEGERECALL_OBJINFO msg ;
//
//	msg.Category	= MP_SIEGERECALL ;
//	msg.Protocol	= MP_SIEGE_CHEAT_ADDOBJ_SYN ;
//
//	msg.dwObjectID	= pmsg->dwObjectID ;
//
//	msg.siegeObj.mapNum			= pmsg->siegeObj.mapNum ;
//	msg.siegeObj.wObjectKind	= pmsg->siegeObj.wObjectKind ;
//	msg.siegeObj.dwObjectIdx	= pmsg->siegeObj.dwObjectIdx ;
//	msg.siegeObj.fXpos			= pmsg->siegeObj.fXpos ;
//	msg.siegeObj.fZpos			= pmsg->siegeObj.fZpos ;
//	msg.siegeObj.byUseRandomPos	= pmsg->siegeObj.byUseRandomPos ;
//	msg.siegeObj.byRadius		= pmsg->siegeObj.byRadius ;
//
//
//	// Send recall message.
//	g_Network.Send2Server(dwIndex, (char*)&msg, sizeof(MSG_SIEGERECALL_OBJINFO)) ;
//}





//-------------------------------------------------------------------------------------------------
//	NAME		: Cheat_Execute_NextStep
//	DESC		: ���� ��ȯ���� ��ȯ ���¸� ���� �ܰ�� �ѱ�� ġƮ ���� �߰�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: October 11, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Cheat_Execute_NextStep( DWORD dwIndex, char* pMsg )
{
	// �Լ� ���� Ȯ��.
	if( !pMsg ) return ;


	// ���� �޽��� ��ȯ.
	MSG_BYTE5* pmsg = NULL ;
	pmsg = (MSG_BYTE5*)pMsg ;

	if( !pmsg ) return ;


	// ���������� Ȯ���Ѵ�.
	if( pmsg->Protocol == MP_SIEGE_CHEAT_NEXTSTEP_SYN )
	{
		MSG_BYTE5 msg ;

		msg.Category	= MP_SIEGERECALL ;
		msg.Protocol	= MP_SIEGE_CHEAT_NEXTSTEP_OTHER_AGENT_SYN ;

		msg.dwObjectID	= pmsg->dwObjectID ;

		msg.bData1		= pmsg->bData1 ;
		msg.bData2		= pmsg->bData2 ;
		msg.bData3		= pmsg->bData3 ;
		msg.bData4		= pmsg->bData4 ;
		msg.bData5		= pmsg->bData5 ;

		g_Network.Broadcast2AgentServerExceptSelf( (char*)&msg, sizeof(MSG_BYTE5) ) ;
	}


	// �׸� �� Ȯ���Ѵ�.
	CSiegeTheme* pTheme = NULL ;
	pTheme = Get_ThemeInfoByThemeIdx(pmsg->bData1) ;

	if(!pTheme)
	{
		MSG_BYTE msg ;
		msg.Category	= MP_SIEGERECALL ;
		msg.Protocol	= MP_SIEGE_CHEAT_NEXTSTEP_NACK ;

		msg.dwObjectID	= pmsg->dwObjectID ;

		msg.bData		= eCE_InvalidThemeIdx ;

		g_Network.Send2User( dwIndex, (char*)&msg, sizeof(MSG_BYTE) ) ;

		return ;
	}


	// �� ��ȣ�� Ȯ���Ѵ�.
	CSiegeMap* pMap = NULL ;
	pMap = pTheme->Get_SiegeMapByIdx( pmsg->bData2 ) ;

	// �� ��ȣ ��ġ ���� ������ ó��.
	if( !pMap )
	{
		MSG_BYTE msg ;
		msg.Category	= MP_SIEGERECALL ;
		msg.Protocol	= MP_SIEGE_CHEAT_NEXTSTEP_NACK ;

		msg.dwObjectID	= pmsg->dwObjectID ;

		msg.bData		= eCE_InvalidMapIdx ;

		g_Network.Send2User( dwIndex, (char*)&msg, sizeof(MSG_BYTE) ) ;

		return ;
	}


	// ������ Ȯ���Ѵ�.
	CSiegeStep* pStep = NULL ;
	pStep = pMap->Get_StepInfo( pmsg->bData3 ) ;

	// ���� ��ȣ ��ġ ���� ������ ó��.
	if( !pStep )
	{
		MSG_BYTE msg ;
		msg.Category	= MP_SIEGERECALL ;
		msg.Protocol	= MP_SIEGE_CHEAT_NEXTSTEP_NACK ;

		msg.dwObjectID	= pmsg->dwObjectID ;

		msg.bData		= eCE_InvalidStepIdx ;

		g_Network.Send2User( dwIndex, (char*)&msg, sizeof(MSG_BYTE) ) ;

		return ;
	}


	// ������Ʈ ��ȣ�� Ȯ���Ѵ�.
	CSiege_AddObj* pAddObj = NULL ;
	pAddObj = pStep->Get_AddObjInfo( pmsg->bData4 ) ;

	// ������Ʈ ��ȣ ��ġ ���� ������ ó��.
	if( !pAddObj )
	{
		MSG_BYTE msg ;
		msg.Category	= MP_SIEGERECALL ;
		msg.Protocol	= MP_SIEGE_CHEAT_NEXTSTEP_NACK ;

		msg.dwObjectID	= pmsg->dwObjectID ;

		msg.bData		= eCE_InvalidObjIdx ;

		g_Network.Send2User( dwIndex, (char*)&msg, sizeof(MSG_BYTE) ) ;

		return ;
	}


	// 1�� ��ȯ�� �ΰ�? 2�� ��ȯ���ΰ�?
	if( pmsg->bData5 == 1 )
	{
		// ������Ʈ ������ �޴´�.
		st_SIEGEOBJ* pObjInfo = NULL ;
		pObjInfo = pAddObj->Get_ObjInfo() ;

		if(!pObjInfo)
		{
			MSG_BYTE msg ;
			msg.Category	= MP_SIEGERECALL ;
			msg.Protocol	= MP_SIEGE_CHEAT_NEXTSTEP_NACK ;

			msg.dwObjectID	= pmsg->dwObjectID ;

			msg.bData		= eCE_InvalidObjInfo ;

			g_Network.Send2User( dwIndex, (char*)&msg, sizeof(MSG_BYTE) ) ;

			return ;
		}

		MSG_SIEGERECALL_OBJINFO msg ;

		msg.Category	= MP_SIEGERECALL ;
		msg.Protocol	= MP_SIEGE_CHEAT_NEXTSTEP_SYN ;

		msg.dwObjectID	= dwIndex ;

		pObjInfo->byAddObjIdx	= pAddObj->Get_Index() ;
		pObjInfo->byComKind		= e_CK_ADD_OBJECT ;
		pObjInfo->byComIndex	= 0 ;
		pObjInfo->wParentMap	= 0 ;

		memcpy(&msg.siegeObj, pObjInfo, sizeof(st_SIEGEOBJ)) ;

		g_Network.Broadcast2MapServer( (char*)&msg, sizeof(msg) ) ;
	}
	else if( pmsg->bData5 == 2 )
	{
		
		{
			// ������Ʈ ������ �޴´�.
			st_SIEGEOBJ* pObjInfo = NULL ;
			pObjInfo = pAddObj->Get_ObjInfo() ;

			if(!pObjInfo)
			{
				MSG_BYTE msg ;
				msg.Category	= MP_SIEGERECALL ;
				msg.Protocol	= MP_SIEGE_CHEAT_NEXTSTEP_NACK ;

				msg.dwObjectID	= pmsg->dwObjectID ;

				msg.bData		= eCE_InvalidObjInfo ;

				g_Network.Send2User( dwIndex, (char*)&msg, sizeof(MSG_BYTE) ) ;

				return ;
			}

			MSG_SIEGERECALL_OBJINFO msg ;

			msg.Category	= MP_SIEGERECALL ;
			msg.Protocol	= MP_SIEGE_CHEAT_NEXTSTEP_SYN ;

			msg.dwObjectID	= dwIndex ;

			pObjInfo->byAddObjIdx	= pAddObj->Get_Index() ;
			pObjInfo->byComKind		= e_CK_ADD_OBJECT ;
			pObjInfo->byComIndex	= 0 ;
			pObjInfo->wParentMap	= 0 ;

			memcpy(&msg.siegeObj, pObjInfo, sizeof(st_SIEGEOBJ)) ;

			g_Network.Broadcast2MapServer( (char*)&msg, sizeof(msg) ) ;
		}

		{
			// Check command.
			BYTE byCommandCount = pAddObj->Get_CommandCount() ;

			CSiegeCommand* pCommand ;
			for( BYTE byCCount = 0 ; byCCount < byCommandCount ; ++byCCount )
			{
				// Receive start term info.
				pCommand = NULL ;
				pCommand = pAddObj->Get_CommandInfo(byCCount) ;

				// Check term info.
				if(!pCommand) continue ;

				// Check child kind.
				switch(pCommand->Get_ChildKind(byCCount))
				{
				case e_COMMAND_NONE :					break ;

				case e_COMMAND_RECALL_DIE_OBJ :
					{
						// Receive info.
						st_DIE_RECALL_OBJ* pDieRecallObjInfo = NULL ;
						pDieRecallObjInfo = pCommand->Get_DieRecallObjInfo(byCCount) ;

						// Check obj info.
						if( pDieRecallObjInfo )
						{
							MSG_SIEGERECALL_OBJINFO msg ;

							msg.Category	= MP_SIEGERECALL ;
							msg.Protocol	= MP_SIEGE_CHEAT_NEXTSTEP_SYN ;

							msg.dwObjectID	= dwIndex ;

							pDieRecallObjInfo->objInfo.byAddObjIdx	= pAddObj->Get_Index() ;
							pDieRecallObjInfo->objInfo.byComKind	= e_CK_COMMAND ;
							pDieRecallObjInfo->objInfo.byComIndex	= 0 ;
							pDieRecallObjInfo->objInfo.wParentMap	= pAddObj->Get_ObjInfo()->mapNum ;

							memcpy(&msg.siegeObj, &pDieRecallObjInfo->objInfo, sizeof(st_SIEGEOBJ)) ;

							g_Network.Broadcast2MapServer( (char*)&msg, sizeof(msg) ) ;
						}
					}
					break ;

				default : break ;
				}
			}
		}

	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Cheat_Execute_NextStep
//	DESC		: ���� ��ȯ���� ��ȯ ���¸� ��� �Ϸ��ϴ� �Լ� �߰�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: October 11, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Cheat_Execute_EndStep( DWORD dwIndex, char* pMsg )
{
	// �Լ� ���� Ȯ��.
	if( !pMsg ) return ;


	// ���� �޽��� ��ȯ.
	MSG_BYTE2* pmsg = NULL ;
	pmsg = (MSG_BYTE2*)pMsg ;

	if( !pmsg ) return ;


	// ���������� Ȯ���Ѵ�.
	if( pmsg->Protocol == MP_SIEGE_CHEAT_ENDSTEP_SYN )
	{
		MSG_BYTE2 msg ;

		msg.Category	= MP_SIEGERECALL ;
		msg.Protocol	= MP_SIEGE_CHEAT_ENDSTEP_OTHER_AGENT_SYN ;

		msg.dwObjectID	= pmsg->dwObjectID ;

		msg.bData1		= pmsg->bData1 ;
		msg.bData2		= pmsg->bData2 ;

		g_Network.Broadcast2AgentServerExceptSelf( (char*)&msg, sizeof(MSG_BYTE2) ) ;
	}


	// �׸� �� Ȯ���Ѵ�.
	CSiegeTheme* pTheme = NULL ;
	pTheme = Get_ThemeInfoByThemeIdx(pmsg->bData1) ;

	if(!pTheme)
	{
		MSG_BYTE msg ;
		msg.Category	= MP_SIEGERECALL ;
		msg.Protocol	= MP_SIEGE_CHEAT_NEXTSTEP_NACK ;

		msg.dwObjectID	= pmsg->dwObjectID ;

		msg.bData		= eCE_InvalidThemeIdx ;

		g_Network.Send2User( dwIndex, (char*)&msg, sizeof(MSG_BYTE) ) ;

		return ;
	}


	// ���� Ȯ���Ѵ�.
	CSiegeMap* pMap ;
	CSiegeStep* pStep ;
	BYTE byChildCount ;
	BYTE byKind ;

	for( BYTE count = 0 ; count < pTheme->Get_MapCount() ; ++count )
	{
		pMap = NULL ;
		pMap = pTheme->Get_SiegeMapByIdx( count ) ;

		if(!pMap) continue ;

		if( pMap->Get_MapNum() != pmsg->bData2 ) continue ;

		// ������ Ȯ���Ѵ�.
		for( BYTE sCount = 0 ; sCount < pMap->Get_StepCount() ; ++sCount )
		{
			pStep = NULL ;
			pStep = pMap->Get_StepInfo( sCount ) ;

			if(!pStep)
			{
				Throw_Error("Invalid step info!!", __FUNCTION__) ;
				continue ;
			}

			// ������ �����Ѵ�.
			byChildCount = 0 ;
			byChildCount = pStep->Get_ChildCount() ;

			for( BYTE byCount = 0 ; byCount < byChildCount ; ++byCount )
			{
				byKind = e_STEP_ADD_OBJ ;
				byKind = pStep->Get_ChildKind(byCount) ;

				switch(byKind)
				{
				case e_STEP_ADD_OBJ :
					{
						CSiege_AddObj* pAddObj = NULL ;
						pAddObj = pStep->Get_AddObjInfo( byCount ) ;

						if( !pAddObj ) continue ;

						{
							// ������Ʈ ������ �޴´�.
							st_SIEGEOBJ* pObjInfo = NULL ;
							pObjInfo = pAddObj->Get_ObjInfo() ;

							if(!pObjInfo)
							{
								MSG_BYTE msg ;
								msg.Category	= MP_SIEGERECALL ;
								msg.Protocol	= MP_SIEGE_CHEAT_NEXTSTEP_NACK ;

								msg.dwObjectID	= pmsg->dwObjectID ;

								msg.bData		= eCE_InvalidObjInfo ;

								g_Network.Send2User( dwIndex, (char*)&msg, sizeof(MSG_BYTE) ) ;

								return ;
							}

							MSG_SIEGERECALL_OBJINFO msg ;

							msg.Category	= MP_SIEGERECALL ;
							msg.Protocol	= MP_SIEGE_CHEAT_NEXTSTEP_SYN ;

							msg.dwObjectID	= dwIndex ;

							pObjInfo->byAddObjIdx	= pAddObj->Get_Index() ;
							pObjInfo->byComKind		= e_CK_ADD_OBJECT ;
							pObjInfo->byComIndex	= 0 ;
							pObjInfo->wParentMap	= 0 ;

							memcpy(&msg.siegeObj, pObjInfo, sizeof(st_SIEGEOBJ)) ;

							g_Network.Broadcast2MapServer( (char*)&msg, sizeof(msg) ) ;
						}

						{
							// Check command.
							BYTE byCommandCount = pAddObj->Get_CommandCount() ;

							CSiegeCommand* pCommand ;
							for( BYTE byCCount = 0 ; byCCount < byCommandCount ; ++byCCount )
							{
								// Receive start term info.
								pCommand = NULL ;
								pCommand = pAddObj->Get_CommandInfo(byCCount) ;

								// Check term info.
								if(!pCommand) continue ;

								// Check child kind.
								switch(pCommand->Get_ChildKind(byCCount))
								{
								case e_COMMAND_NONE :					break ;

								case e_COMMAND_RECALL_DIE_OBJ :
									{
										// Receive info.
										st_DIE_RECALL_OBJ* pDieRecallObjInfo = NULL ;
										pDieRecallObjInfo = pCommand->Get_DieRecallObjInfo(byCCount) ;

										// Check obj info.
										if( pDieRecallObjInfo )
										{
											MSG_SIEGERECALL_OBJINFO msg ;

											msg.Category	= MP_SIEGERECALL ;
											msg.Protocol	= MP_SIEGE_CHEAT_NEXTSTEP_SYN ;

											msg.dwObjectID	= dwIndex ;

											pDieRecallObjInfo->objInfo.byAddObjIdx	= pAddObj->Get_Index() ;
											pDieRecallObjInfo->objInfo.byComKind	= e_CK_COMMAND ;
											pDieRecallObjInfo->objInfo.byComIndex	= 0 ;
											pDieRecallObjInfo->objInfo.wParentMap	= pAddObj->Get_ObjInfo()->mapNum ;

											memcpy(&msg.siegeObj, &pDieRecallObjInfo->objInfo, sizeof(st_SIEGEOBJ)) ;

											g_Network.Broadcast2MapServer( (char*)&msg, sizeof(msg) ) ;
										}
									}
									break ;

								default : break ;
								}
							}
						}
					}
					break ;

				default : break ;
				}
			}
		}
	}
}









