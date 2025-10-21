#pragma once
//-------------------------------------------------------------------------------------------------
//	NAME		: CSiegeRecallMgr.h
//	DESC		: The class to manage recall part for siege war.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 28, 2008
//-------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------------------------
//		Include part.
//-------------------------------------------------------------------------------------------------
#include <map>

class CSiege_AddObj ;
class CSiegeCommand ;
class CSiegeTerm ;





//-------------------------------------------------------------------------------------------------
//		Define some macroinstruction.
//-------------------------------------------------------------------------------------------------
#define SIEGERECALLMGR	CSiegeRecallMgr::GetInstance()

//#define _USE_ERRBOX_							// If use this macro, you can see the error messagebox.
												// else print error message to console dialog.

#define _ACTIVE_CHEAT_							// If use this macro, you can see notice about exe cheat.

// DB ���ν��� �� ����( �׽�Ʈ ����� ���� ��¥ ��� ���� )
#ifdef _TEST_DB_
// 081103 LYW --- SiegeRecallMgr : ���� ���� ���ν����� �ҹ��� -> �빮�� ���� �� ������� �ʴ� ���ν������� �ּ�ó���Ѵ�.
#define MP_SIEGERECALL_INSERT			"dbo.MP_TEST_SIEGERECALL_INSERT"
#define MP_SIEGERECALL_REMOVE			"dbo.MP_TEST_SIEGERECALL_REMOVE"
//#define MP_SIEGERECALL_LOAD				"dbo.MP_TEST_SIEGERECALL_LOAD"
//#define MP_SIEGERECALL_UPDATE			"dbo.MP_TEST_SIEGERECALL_UPDATE"
#define MP_SIEGERECALL_RESET			"dbo.MP_TEST_SIEGERECALL_RESET"

// 081222 LYW --- SiegeRecallMgr : ���� ���� ���ν����� �߰�.
#define MP_SIEGERECALL_KILLCOUNT		"dbo.MP_TEST_SIEGERECALL_KILLCOUNT"

#else

#define MP_SIEGERECALL_INSERT			"dbo.MP_SIEGERECALL_INSERT"
#define MP_SIEGERECALL_REMOVE			"dbo.MP_SIEGERECALL_REMOVE"
//#define MP_SIEGERECALL_LOAD				"dbo.MP_SIEGERECALL_LOAD"
//#define MP_SIEGERECALL_UPDATE			"dbo.MP_SIEGERECALL_UPDATE"
#define MP_SIEGERECALL_RESET			"dbo.MP_SIEGERECALL_RESET"
// 081222 LYW --- SiegeRecallMgr : ���� ���� ���ν����� �߰�.
#define MP_SIEGERECALL_KILLCOUNT		"dbo.MP_SIEGERECALL_KILLCOUNT"

#endif // _TEST_DB_







//-------------------------------------------------------------------------------------------------
//		The class CSiegeRecallMgr.
//-------------------------------------------------------------------------------------------------
class CSiegeRecallMgr
{
	//typedef std::map< BYTE ,CSiegeTheme >	M_THEME ;
	//M_THEME	m_Theme ;

	// 
	typedef std::map< BYTE, CSiege_AddObj >		M_MADDOBJ ;
	M_MADDOBJ	m_mAddObj ;

	// 081006 LYW --- SiegeRecallMgr : ���� �� ��ȯ���� ��ȯ�Ǿ����� ���θ� ���� �÷��� �߰�.
	WORD		m_wRecallCount ;
	BYTE		m_byReadyToUseWaterSeed ;

	// 081226 LYW --- SiegeRecallMgr : ����ũ �ε��� ���� �߰�.
	// ĳ����Ʈ�� �߰��� ��, ĳ����Ʈ�� ���� ���̵�� �߰� �� ���� ó���� �Ϸ� �Ͽ�����, 
	// ĳ����Ʈ�� �߰� �� ���� ����� Agent�� ���� �Ѿ���� ������, Agent���� ���� �ʼ�������
	// �߰��Ǵ� ĳ����Ʈ�� ���� ���̵� ���� ���ϹǷ�, �̴� �ȵȴ�.
	// ��ũ��Ʈ���� ������ ������ ��������� ���̵� ����ϸ� �̸� �ذ��� �� �ִ�.
	// ���ʿ� Agent������ ��ũ��Ʈ ������ ������ �ֵ��� ������ ���� ����.
	BYTE		m_byUniqueIdx ;

public:
	GETINSTANCE(CSiegeRecallMgr) ;

	CSiegeRecallMgr(void) ;												// Constructor.
	~CSiegeRecallMgr(void) ;											// Destructor.

	void Initialieze() ;												// The function to initialize recall manager.

	void Reset_SiegeObjInfo() ;											// The function to resetting.

	//090812 pdy ������ Agent�� Map�������� ��ȯObj �ε��������� �߸����ִ� ���� ����
	CSiege_AddObj* Get_AddObjIdx(DWORD dwObjIdx) ;						// The function to return add object info.


	// Network messaeg part.
	void NetworkMsgParser(void* pMsg ) ;								// The functions to parsing network message.

	void Add_Object(void* pMsg) ;										// The function to add object.
	void Add_CheckObjDie(void* pMsg) ;									// The function to add check routine for object die.
	void Add_CheckUseItem(void* pMsg) ;									// The function to add check routine for use item.
	void Add_Command(void* pMsg) ;										// The function to add command.


	// The function to add object info to database.
	//void Add_ObjToDB(WORD wTheme, WORD wMapNum, BYTE byStepIdx, WORD wAddObjIdx, BYTE IsParent, BYTE byChildIdx) ;
	void Add_ObjToDB(WORD wTheme, WORD wMapNum, WORD wRecallMap, BYTE byStepIdx, BYTE byAddObjIdx, BYTE byComKind, BYTE byComIdx, BYTE byRecallStep ) ;
	// The function to remove object info from database.
	//void Remove_ObjFromDB(WORD wTheme, WORD wMapNum, BYTE byStepIdx, WORD wAddObjIdx, BYTE IsParent, BYTE byChildIdx) ;
	void Remove_ObjFromDB(WORD wTheme, WORD wMapNum, WORD wRecallMap, BYTE byStepIdx,  BYTE byAddObjIdx, BYTE byComKind, BYTE byComIndex, BYTE byRecallStep ) ;
	// The function to load object info from database.
	void Request_ObjFromDB() ;


	// The function to check terms.
	//090812 pdy ������ Agent�� Map�������� ��ȯObj �ε��������� �߸����ִ� ���� ���� 
	BYTE Check_MyAddObjInfo(BYTE byStepNum, DWORD dwObjIdx)	;				// The function to check addObjInfo.
	//void Check_ObjectDie(DWORD dwObjectID, BYTE IsCheatMode) ;			// The function to check object die.
	// 081028 LYW --- SiegeRecall : ų�� ���̵� ����� ���� �Լ��� ������.
	void Check_ObjectDie(CObject* pKiller, DWORD dwObjectID, BYTE IsCheatMode) ;			// The function to check object die.


	// The function to send start term info.
	void Send_StartTermInfo(BYTE byParentStepIdx, CSiegeTerm* pTerm, BYTE byIdx) ;
	// The function to send end term info.
	void Send_EndTermInfo(BYTE byParentStepIdx, CSiegeTerm* pTerm, BYTE byIdx) ;
	// The function to send command info.
	void Send_CommandInfo(BYTE byParentStepIdx, CSiegeCommand* pCommand, BYTE byIdx) ;


	// The function to remove object info from database.
	void Remove_AllMapAllObj( void* pMsg ) ;							// ��� �� / ��� ������Ʈ�� �����ؾ� �ϴ� �Լ�.
	void Remove_AllMapSpecifyObj( void* pMsg ) ;						// ��� �� / ���� �� ������Ʈ�� �����ؾ� �ϴ� �Լ�.
	void Remove_SpecifyMapAllObj( void* pMsg ) ;						// ���� �� �� / ��� ������Ʈ�� �����ؾ� �ϴ� �Լ�.
	void Remove_SpecifyMapSpecifyObj( void* pMsg ) ;					// ���� �� �� / ���� �� ������Ʈ�� �����ؾ� �ϴ� �Լ�.
	void Set_LoadSummonFlag( void* pMsg ) ;								// ���� �ε� ���θ� �����ϴ� �Լ�.

	BYTE Get_RecalledObjectCount() { return BYTE( m_mAddObj.size() ) ; }

	void EndSiegeWar_ClearRecallInfo() ;

	BYTE IsRecallMonster(DWORD dwID) ;									// ���ڷ� �Ѿ�� ���̵��� ���Ͱ� ��ȯ �� �������� ��ȯ�ϴ� �Լ�.

	WORD GetRecallCount()	{ return m_wRecallCount ; }					// ��ȯ �� ���� ���� ��ȯ�ϴ� �Լ�.

	// 081007 LYW --- ��ȯ�ؾ� �� ��ȯ ī��Ʈ�� �޴´�.
	void Set_RecallCount( void* pMsg ) ;

	// 081015 LYW --- SiegeRecallMgr : ���� ��ȯ���� ������ ų ���¸� �ε��Ͽ� ������ �Ѿ���� �� ó���ϴ� �۾� �߰�.
	void Load_RecallCount( void* pMsg ) ;

	// 081008 LYW --- SiegeRecallMgr : ���ͽõ� ���� ���� �غ� �Ǿ����� ���θ� ��ȯ�ϴ� �Լ�.
	BYTE IsReadyToUseWaterSeed() ;
	void SetReadyToUseWaterSeed(BYTE byValue) { m_byReadyToUseWaterSeed = byValue ; }


	// The functions to print error message.
	void Throw_Error(char* szErr, char* szCaption) ;					// The function to process error message.
	void WriteLog(char* pMsg) ;											// The function to write error log.
	// 081011 LYW --- Protocol : ���� ��ȯ���� ��ȯ ���¸� ���� �ܰ�� �ѱ�� ġƮ ���� �߰�.
	void Cheat_ExcuteNextStep( void* pMsg ) ;

	// 081012 LYW --- SiegeRecallMgr : ��ȯ�� �ʱ�ȭ �Լ� �߰�.
	void Clear_RecallInfo() ;

	// 081028 LYW --- SiegeRecall : ĳ����Ʈ �α׸� ����� �Լ� �߰�.
	void WriteCatapultLog( char* pMsg ) ;

	// 081222 LYW --- SiegeRecall : ĳ����Ʈ�� ��� ��Ҵ��� ���θ� Ȯ���ϴ� �Լ� �߰�.
	void RequestKillCountToDB() ;

	// 081222 LYW --- SiegeRecall : ĳ����Ʈ�� ��� ��Ҵ��� ��� ���� �޾Ƽ� �����ϴ� �Լ� �߰�.
	void CheckKillCount(int nKillCount) ;

private :
	// 081226 LYW --- SiegeRecallMgr : ����ũ �ε����� ���� �� ��ȯ�ϴ� �Լ� �߰�.
	void SetUniqueIdx( BYTE byIdx )	{ m_byUniqueIdx = byIdx ; } ;
	BYTE GetUniqueIdx()				{ return m_byUniqueIdx ; }
};
