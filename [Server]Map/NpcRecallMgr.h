#pragma once
//-------------------------------------------------------------------------------------------------
//	NAME		: CNpcRecallMgr.h
//	DESC		: NPC�� ��ȯ�ϴ� ������ �ϴ� �Ŵ��� Ŭ����.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------------------------
//		Include part.
//-------------------------------------------------------------------------------------------------
#include <map>

#include "MHFile.h"
#include "../[CC]RecallInfo/NpcRecall/NpcRecallSelf.h" 
#include "../[CC]RecallInfo/NpcRecall/NpcRecallRemainTime.h"
#include "../[CC]RecallInfo/NpcRecall/NpcRecallMoveable.h"
//class CNpcRecallSelf ;
//class CNpcRecallRemainTime ;
//class CNpcRecallMoveable ;





//-------------------------------------------------------------------------------------------------
//		Define some macroinstruction.
//-------------------------------------------------------------------------------------------------
#define NPCRECALLMGR	CNpcRecallMgr::GetInstance()

//#define _USE_NPCRECALL_ERRBOX_					// �� ��ũ�ΰ� Ȱ��ȭ �Ǿ� ������, ���� �߻��� �޽��� �ڽ��� ����,
												// ��Ȱ��ȭ ��Ű��, Sworking ���� ��, ���� �α׸� �����.

#define SCHEDULE_CHECK_TIME		60000			// �������� üũ�� �ð� ���� ����.

// DB ���ν��� �� ����( �׽�Ʈ ����� ���� ��¥ ��� ���� )


#define MP_NPCREMAINTIME_INSERT			"dbo.MP_NPCREMAINTIME_INSERT"
#define MP_NPCREMAINTIME_DELETE			"dbo.MP_NPCREMAINTIME_DELETE"
#define MP_NPCREMAINTIME_UPDATE			"dbo.MP_NPCREMAINTIME_UPDATE"
#define MP_NPCREMAINTIME_LOAD			"dbo.MP_NPCREMAINTIME_LOAD"


enum RECALL_NPC_COMMAND							// ��ũ��Ʈ���� NPC ��ȯ ������ �ε��ϱ� ���� ���ɾ� �з� ����.
{
	e_RNC_SELF_RECALL_NPC = 0,					// ������ ��ȯ/�Ҹ��ϴ� npc �߰� ���ɾ�.
	e_RNC_REMAIN_TIME_NPC,						// �����ð� ���� ��ȯ/�Ҹ� �Ǵ� npc �߰� ���ɾ�.
	e_RNC_MOVEABLE_NPC,							// �̵��� ������ npc �߰� ���ɾ�.
	e_RNC_NPCRECALLID,							// ���� �ε����� npc�� ��ȯ ID�� �����ϴ� ������.
	e_RNC_NPCID,								// ���� �ε����� npc�� ���̵� �����ϴ� ������.
	e_RNC_NPCKIND,								// ���� �ε����� npc�� ������ �����ϴ� ���ɾ�.
	e_RNC_RECALLMAP,							// ���� �ε����� npc�� ��ȯ �� ��ȣ�� �����ϴ� ���ɾ�.
	e_RNC_XPOS,									// ���� �ε����� npc�� ��ȯ ��ǥ x �� �����ϴ� ���ɾ�.
	e_RNC_ZPOS,									// ���� �ε����� npc�� ��ȯ ��ǥ z �� �����ϴ� ���ɾ�.
	e_RNC_DIR,									// ���� �ε����� npc�� ��ȯ ������ �����ϴ� ���ɾ�.
	e_RNC_NAME,									// ���� �ε����� npc�� �̸��� �����ϴ� ���ɾ�.
	e_RNC_TIME_TABLE,							// ���� �ε����� npc�� ��ȯ/�Ҹ� �ð� ���̺��� �����ϴ� ���ɾ�.
	e_RNC_REMAIN_TIME,							// ���� �ε����� npc�� ��ȯ ��ȿ �ð��� �����ϴ� ���ɾ�.
	e_RNC_MOVE_TIME,							// ���� �ε����� npc�� �̵��� ���۵Ǵ� �ð��� �����ϴ� ���ɾ�.
	e_RNC_MOVE_XPOS,							// ���� �ε����� npc�� �̵� �� ��ǥ x �� �����ϴ� ���ɾ�.
	e_RNC_MOVE_ZPOS,							// ���� �ε����� npc�� �̵� �� ��ǥ z �� �����ϴ� ���ɾ�.
	e_RNC_CHANGE_MAPNUM,						// ���� �ε����� NPC�� ���� ���̵��� �� �� ��ȣ�� �����ϴ� ���ɾ�.
	e_RNC_CHANGE_XPOS,							// ���� �ε����� NPC�� ���� ���̵��� �� �� X��ǥ�� �����ϴ� ���ɾ�.
	e_RNC_CHANGE_ZPOS,							// ���� �ε����� NPC�� ���� ���̵��� �� �� Z��ǥ�� �����ϴ� ���ɾ�.
	e_RNC_PARENT_NPCID,							// ���� �ε����� NPC�� �θ� NPCID�� �����ϴ� ���ɾ�. 
	e_RNC_PARENT_TYPE,							// ���� �ε����� NPC�� �θ� NPC�� Ÿ���� �����ϴ� ���ɾ�. ( 1 : STATIC NPC , 2: RECALL NPC ) 
	e_RNC_MAX,									// ���ɾ� ���� üũ��.
} ;


// 080828 LYW --- GameResourceManager : npc ���� ������ ���� ����ü�� �����Ѵ�.
struct stNpcBuffInfo
{
	DWORD dwBuffIdx ;
	DWORDEX dwMoney ;
} ;

struct stNPCBuffData
{
	DWORD dwNpcIdx ;

	typedef std::list< stNpcBuffInfo >	L_BUFFLIST ;
	L_BUFFLIST							buffList ;
} ;

// 080828 LYW --- GameResourceManager : npc ���� ��ų ��û ���� ó�� �з� ����.
enum SIEGEWARFARE_REQUEST_BUFFSKILL_ERR
{
	e_REQUEST_BUFFSKILL_SUCCESS = 0,						// ���� ��ų ��û ����.		
	e_REQUEST_BUFFSKILL_FAILED_RECEIVE_NPCINFO,				// npc ���� �ޱ� ����.
	e_REQUEST_BUFFSKILL_FAILED_RECEIVE_BUFFDATA,			// ���� ������ �ޱ� ����.
	e_REQUEST_BUFFSKILL_FAILED_RECEIVE_PLAYERINFO,			// Player ���� �ޱ� ����.
	e_REQUEST_BUFFSKILL_TOO_FAR_DISTANCE,					// Player�� Npc�� �Ÿ��� �ʹ� �־ ����.
	e_REQUEST_BUFFSKILL_FAILED_FIND_BUFF,					// Npc�� ���� ã�� ����.
	e_REQUEST_BUFFSKILL_FAILED_RECEIVE_BUFFSKILLINFO,		// ���� ��ų ���� �ޱ� ����.
	e_REQUEST_BUFFSKILL_NOTENOUGH_PAY,						// ���� ��ų ���� ���� �ݾ� ����.
	e_REQUEST_BUFFSKILL_INVALID_PLAYER,						// ���� ���� �ޱ� ����.
	e_REQUEST_BUFFSKILL_INVALID_GUILD,						// ���� ������ ��尡 �ƴ϶�� ����.
	e_REQUEST_BUFFSKILL_INVALID_SIEGWZONE,					// ���� ������ �ƴϸ� ����.
	e_REQUEST_BUFFSKILL_INVALID_TIME,						// �������� �������Ķ�� ����.

	e_REQUEST_BUFFSKILL_MAX,								// ���� ���� üũ��.
} ;


//-------------------------------------------------------------------------------------------------
//		The class CNpcRecallMgr.
//-------------------------------------------------------------------------------------------------
class CNpcRecallMgr
{
	typedef std::map< DWORD, stNPCBuffData>		M_NPCBUFFMAP ;
	M_NPCBUFFMAP								m_MNpcBuffMap ;
	///////////////////////////////////////
	// [ ��ũ��Ʈ ������ ���� �����̳� ] //
	///////////////////////////////////////

	typedef std::map< WORD, CNpcRecallSelf >			MAP_RSELF_NPC ;			// ������ ��ȯ/�Ҹ�Ǵ� NPC�� ���� �� �����̳� ����.
	MAP_RSELF_NPC										m_mRSelf_Npc ;

	typedef std::map< WORD, CNpcRecallRemainTime >		MAP_RTIME_NPC ;			// ���� �ð����� ��ȯ/�Ҹ�Ǵ� NPC�� ���� �� �����̳� ����.
	MAP_RTIME_NPC										m_mRTime_Npc ;

	typedef std::map< WORD, CNpcRecallMoveable >		MAP_RMOVE_NPC ;			// Ư�� ����Ʈ�� ���� ������ NPC�� ���� �� �����̳� ����.
	MAP_RMOVE_NPC										m_mRMove_Npc ;


	///////////////////////////////////////
	// [ Ȱ��ȭ �� npc�� ���� �����̳� ] //
	///////////////////////////////////////

	typedef std::map< DWORD, CNpcRecallSelf >			MAP_ARSELF_NPC ;
	MAP_ARSELF_NPC										m_mARSelf_Npc ;			// Ȱ��ȭ �� RS npc�� ���� �����̳�.

	typedef std::map< DWORD, CNpcRecallRemainTime >		MAP_ARTIME_NPC ;
	MAP_ARTIME_NPC										m_mARTime_Npc ;			// Ȱ��ȭ �� RT npc�� ���� �����̳�.

	typedef std::map< DWORD, CNpcRecallMoveable >		MAP_ARMOVE_NPC ;
	MAP_ARMOVE_NPC										m_mARMove_Npc ;			// Ȱ��ȭ �� RM npc�� ���� �����̳�.

public :
	GETINSTANCE(CNpcRecallMgr) ;

	CNpcRecallMgr(void) ;														// ������ �Լ�.
	~CNpcRecallMgr(void) ;														// �Ҹ��� �Լ�.


	//////////////////////
	//  [�ʱ�ȭ ��Ʈ.]  //
	//////////////////////

	void Initialize() ;															// �ʱ�ȭ �Լ�.

	void Load_RecallNpc() ;														// ��ȯ�� npc ������ �ε��ϴ� �Լ�.

	void Load_NpcFromDB();

	void OnLoadNpc_FromDB( DWORD dwRecallidx , DWORD dwChenel , DWORD dwRemainTime ) ;

	BYTE Get_CommandKind(char* pString) ;										// ��ũ��Ʈ ���ɾ� Ÿ���� ��ȯ�ϴ� �Լ�.

	void Command_Process(BYTE byCommandKind, CMHFile* pFile) ;					// ��ũ��Ʈ ���ɾ� Ÿ�Կ� ���� ó���� �ϴ� �Լ�.

	void Load_SelfRecallNpc() ;													// ������ ��ȯ/�Ҹ�Ǵ� npc ������ �ε��ϴ� �Լ�.

	void Load_RemainTimeNpc() ;													// ���� �ð� ��ȯ/�Ҹ�Ǵ� npc ������ �ε��ϴ� �Լ�.

	void Load_MoveableNpc() ;													// �̵� ������ npc ������ �ε��ϴ� �Լ�.

	void Load_SetNpcRecallId(CMHFile* pFile) ;									// ���� �ε����� npc�� ��ȯ�ε����� �����ϴ� �Լ�.

	void Load_SetNpcId(CMHFile* pFile) ;										// ���� �ε����� npc�� �ε����� �����ϴ� �Լ�.

	void Load_SetNpcKind(CMHFile* pFile) ;										// ���� �ε����� npc�� ������ �����ϴ� �Լ�.

	void Load_SetRecallMap(CMHFile* pFile) ;									// ��ȯ �� �� ��ȣ�� �����ϴ� �Լ�.

	void Load_SetXpos(CMHFile* pFile) ;											// ��ȯ �� X��ǥ�� �����ϴ� �Լ�.

	void Load_SetZpos(CMHFile* pFile) ;											// ��ȯ �� Z��ǥ�� �����ϴ� �Լ�.

	void Load_SetDir(CMHFile* pFile ) ;											// ��ȯ �� ������ �����ϴ� �Լ�.

	void Load_SetName(CMHFile* pFile ) ;										// ���� �ε����� npc �̸��� ����

	void Load_TimeTable(CMHFile* pFile) ;										// ������ ��ȯ�� �ð� ���̺��� �ε��ϴ� �Լ�.

	void Load_SetRemainTime(CMHFile* pFile) ;									// ��ȯ ���� �ð��� �ε��ϴ� �Լ�.

	void Load_SetMoveTime(CMHFile* pFile) ;										// �̵��� ���� �� �ð��� �ε��ϴ� �Լ�.

	void Load_SetMoveXpos(CMHFile* pFile) ;										// �̵� �� X��ǥ�� �ε��ϴ� �Լ�.

	void Load_SetMoveZpos(CMHFile* pFile) ;										// �̵� �� Z��ǥ�� �ε��ϴ� �Լ�.

	void Load_SetChangeMapNum(CMHFile* pFile) ;									// ��Ż�� ���� �̵��� �� ��ȣ�� �ε��ϴ� �Լ�.

	void Load_SetChangeMapXPos(CMHFile* pFile) ;								// ��Ż�� ���� �̵��� ���� X��ǥ�� �ε��ϴ� �Լ�.

	void Load_SetChangeMapZPos(CMHFile* pFile) ;								// ��Ż�� ���� �̵��� ���� Z��ǥ�� �ε��ϴ� �Լ�.

	void Load_SetParentNpcID(CMHFile* pFile) ;									// �θ� NPC�� ID�� ���� (���� ������ ���Ͽ�)

	void Load_SetParentNpcType(CMHFile* pFile) ;									// �θ� NPC�� Type�� ���� (���� ������ ���Ͽ�)

	
	///////////////////////////
	// [ PROCESS & PARSER. ] //
	///////////////////////////

	void Process() ;															// ���μ��� �Լ�.

	void NetworkMsgParser(DWORD dwIndex, void* pMsg, DWORD dwLength) ;			// �޽��� �ļ� �Լ�.

	void CheckSchedule_SelfRecallNpc() ;										// ������ ��ȯ/�Ҹ�Ǵ� npc�� �������� üũ�Ѵ�.

	void CheckSchedule_RemainTimeNpc() ;										// ���� �ð����� ��ȯ/�Ҹ�Ǵ� npc�� �������� üũ�Ѵ�.

	void CheckSchedule_MoveableNpc() ;											// �̵� ������ npc�� �������� üũ�Ѵ�.

	void CTOM_Recall_Syn(DWORD dwIndex, char* pMsg, DWORD dwLength) ;			// Ŭ���̾�Ʈ�� ���� npc ��ȯ ��û�� ������ ��, ó���ϴ� �Լ�.

	void CTOAM_ChangeMap_Syn(DWORD dwIndex, char* pMsg, DWORD dwLength) ;		// Ŭ���̾�Ʈ�� ����, �̵� ������ npc�� ���� �� �̵� ��û�� ������ ��, ó���ϴ� �Լ�.

	void Cheat_CTOA_Recall_Syn(DWORD dwIndex, char* pMsg, DWORD dwLength) ;		// �׽�Ʈ�� ġƮ �Լ�.


	///////////////////////////////////////////
	// [ ��ũ��Ʈ ���� �߰� & ��ȯ�� ]       //
	///////////////////////////////////////////

	WORD Get_CurTotalNpcCount() ;												// ��ũ��Ʈ �ε� ��, ���ݱ��� �ε� �� �� npc ���� ��ȯ�ϴ� �Լ�.

	CNpcRecallBase* Get_CurNpcBase() ;											// ���� �ε� ��, npc�� �⺻ ������ ��ȯ�ϴ� �Լ�.
	BYTE Get_NpcType(DWORD dwIndex) ;											// npc �ε����� npc�� Ÿ���� ��ȯ�ϴ� �Լ�.

	CNpcRecallSelf* Get_SelfNpc(DWORD dwIndex) ;								// ������ ��ȯ/�Ҹ�Ǵ� npc ������ ��ȯ�ϴ� �Լ�.

	CNpcRecallRemainTime* Get_TimeNpc(DWORD dwIndex) ;							// �����ð� ��ȯ/�Ҹ�Ǵ� npc ������ ��ȯ�ϴ� �Լ�.

	CNpcRecallMoveable* Get_MoveNpc(DWORD dwIndex) ;							// �̵������� npc ������ ��ȯ�ϴ� �Լ�.

	CNpcRecallBase* Get_RecallBase(DWORD dwIndex) ;

	CNpcRecallSelf* Get_ActiveSelfNpc(DWORD dwIndex , WORD wChenel) ;			// ������ ��ȯ/�Ҹ�Ǵ� npc ������ ��ȯ�ϴ� �Լ�.

	CNpcRecallRemainTime* Get_ActiveTimeNpc(DWORD dwIndex) ;					// �����ð� ��ȯ/�Ҹ�Ǵ� npc ������ ��ȯ�ϴ� �Լ�.

	CNpcRecallMoveable* Get_ActiveMoveNpc(DWORD dwIndex) ;						// �̵������� npc ������ ��ȯ�ϴ� �Լ�.

	CNpcRecallMoveable* Get_ActiveMoveNpcByCreateIdx(DWORD dwIndex) ;			// ���� ���̵��, ��ȯ���� �̵� npc ������ �˻��ϴ� �Լ�.

	CNpcRecallBase* Get_ActiveRecallBase(DWORD dwIndex , WORD wchenel );

	BOOL CheckHackRecallNpc( CPlayer* pPlayer , DWORD dwRecallIndex ) ;

	////////////////////////////////
	// [ ���� �޽��� ó�� ��Ʈ. ] //
	////////////////////////////////

	void Throw_Error(char* szErr, char* szCaption) ;							// ���� �޽��� ó���� �ϴ� �Լ�.
	void WriteLog(char* pMsg) ;													// �α׸� ����� ó���� �ϴ� �Լ�.

	void Recall_Npc( CNpcRecallBase* pNpcRecallBase );
	void Recall_success( CNpcRecallBase* pNpcRecallBase ) ;
	void Recall_Failed( CNpcRecallBase* pNpcRecallBase , BYTE byError ) ;

	void Delete_Npc( CNpcRecallBase* pNpcRecallBase );
	void Delete_Success( CNpcRecallBase* pNpcRecallBase ) ;
	void Delete_Failed( CNpcRecallBase* pNpcRecallBase , BYTE byError ) ;

	void Move_Npc( CNpcRecallBase* pNpcRecallBase );
	void Move_Npc_Success( CNpcRecallBase* pNpcRecallBase ) ;
	void Move_Npc_Failed( CNpcRecallBase* pNpcRecallBase , BYTE byError ) ;

	void ATOM_ChangeMap_Ack(DWORD dwIndex, void* pMsg, DWORD dwLength) ;

	// 080826 KTH Load NPC Buff List
	void LoadNPCBuffList();
	stNPCBuffData* GetNPCBuffData(DWORD dwNPCIdx);

	// 080826 KTH -- NPC Buff Excute Fuction
	BYTE ExcuteNPCBuff(CPlayer* pPlayer, WORD wNPCIndex, DWORD dwSelectBuff = 0);

	// 081028 LYW --- NpcRecallMgr : npc ��ȯ ���� ������ ���� �Լ� �߰�.
	void WriteRecallNpcLog( char* pMsg ) ;

};
