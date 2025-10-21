#pragma once
//-------------------------------------------------------------------------------------------------
//	NAME		: CSiegeRecallMgr.h
//	DESC		: �������� ���� ��ȯ���� �����ϴ� Ŭ����.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 23, 2008
//-------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------------------------
//		Include part.
//-------------------------------------------------------------------------------------------------
#include <map>

#include "MHFile.h"

class CSiegeTheme ;
class CSiege_AddObj ;
class CSiegeStep ;





//-------------------------------------------------------------------------------------------------
//		Define some macroinstruction.
//-------------------------------------------------------------------------------------------------
#define SIEGERECALLMGR	CSiegeRecallMgr::GetInstance()

//#define _USE_ERRBOX_							// �� ��ũ�ΰ� Ȱ��ȭ �Ǿ� ������, ���� �߻��� �޽��� �ڽ��� ����,
												// ��Ȱ��ȭ ��Ű��, Sworking ���� ��, ���� �α׸� �����.

#define MAX_THEME_COUNT					20		// �ִ� �׸� ī��Ʈ.
#define MAX_SIEGEMAP_COUNT_PER_THEME	10		// �� �׸� ��, �ִ� �� ī��Ʈ.
#define MAX_STEP_COUNT_PER_MAP			30		// �� �� ��, �ִ� ���� ī��Ʈ.

#define ERR_INVALID_COMMAND				255		// �߸� �� ��ũ��Ʈ ��ɾ� ���� ����.

// DB ���ν��� �� ����( �׽�Ʈ ����� ���� ��¥ ��� ���� )
#ifdef _TEST_DB_
// 081103 LYW --- SiegeRecallMgr : ���� ���� ���ν����� �ҹ��� -> �빮�� ���� �� ������� �ʴ� ���ν������� �ּ�ó���Ѵ�.
//#define MP_SIEGERECALL_INSERT			"dbo.MP_TEST_SIEGERECALL_INSERT"
//#define MP_SIEGERECALL_REMOVE			"dbo.MP_TEST_SIEGERECALL_REMOVE"
#define MP_SIEGERECALL_LOAD				"dbo.MP_TEST_SIEGERECALL_LOAD"
//#define MP_SIEGERECALL_UPDATE			"dbo.MP_TEST_SIEGERECALL_UPDATE"

#else

//#define MP_SIEGERECALL_INSERT			"dbo.MP_SIEGERECALL_INSERT"
//#define MP_SIEGERECALL_REMOVE			"dbo.MP_SIEGERECALL_REMOVE"
#define MP_SIEGERECALL_LOAD				"dbo.MP_SIEGERECALL_LOAD"
//#define MP_SIEGERECALL_UPDATE			"dbo.MP_SIEGERECALL_UPDATE"

#endif // _TEST_DB_


// 081011 LYW --- Protocol : ���� ��ȯ���� ��ȯ ���¸� ���� �ܰ�� �ѱ�� ġƮ ���� �߰�.
enum CHEAT_ERRORTYPE
{
	eCE_InvalidThemeIdx = 0,
	eCE_InvalidMapIdx,
	eCE_InvalidStepIdx,
	eCE_InvalidObjIdx,
	eCE_InvalidObjInfo,
} ;





//-------------------------------------------------------------------------------------------------
//		The class CSiegeRecallMgr.
//-------------------------------------------------------------------------------------------------
class CSiegeRecallMgr
{
	typedef std::map< BYTE, CSiegeTheme >	M_THEME ;							// �׸� ������ ���� Map �����̳�.
	M_THEME	m_Theme ;

public:
	GETINSTANCE(CSiegeRecallMgr) ;

	CSiegeRecallMgr(void) ;														// ������ �Լ�.
	~CSiegeRecallMgr(void) ;													// �Ҹ��� �Լ�.

	void Initialize() ;															// �ʱ�ȭ �Լ�.


	/////////////////////////////
	// [ ��ũ��Ʈ �ε� ��Ʈ. ] //
	/////////////////////////////
	void Load_RecallInfo() ;													// ��ȯ ������ �ε��ϴ� �Լ�.

	void Command_Process(BYTE byCommandKind, CMHFile* pFile) ;					// ��ũ��Ʈ ����� �����ϴ� �Լ�.
	BYTE Get_CommandKind(char* pString) ;										// ��ũ��Ʈ ��ɾ� Ÿ���� ��ȯ�ϴ� �Լ�.

	void Add_Theme(CMHFile* pFile) ;											// �׸� ������ �߰��ϴ� �Լ�.
	CSiegeTheme* Get_ThemeInfo(BYTE byIdx) ;									// Map �����̳ʿ� ���� �ε�����, �׸� ������ ��ȯ�ϴ� �Լ�.
	CSiegeTheme* Get_ThemeInfoByThemeIdx(WORD wIdx) ;							// �׸� �ε�����, �׸� ������ ��ȯ�ϴ� �Լ�.

	void Add_Theme_Idx(CMHFile* pFile) ;										// �׸� �ε����� �߰��ϴ� �Լ�.

	void Add_Map(CMHFile* pFile) ;												// �� ������ �߰��ϴ� �Լ�.
	void Add_Step(CMHFile* pFile) ;												// ���� ������ �߰��ϴ� �Լ�.
	void Add_Obj(CMHFile* pFile) ;												// ������Ʈ ������ �߰��ϴ� �Լ�.

	void Add_StartTerm(CMHFile* pFile) ;										// ���� ������ �߰��ϴ� �Լ�.
	void Add_EndTerm(CMHFile* pFile) ;											// ���� ������ �߰��ϴ� �Լ�.
	void Add_Command(CMHFile* pFile) ;											// ���� �� ������ ����� �߰��ϴ� �Լ�.

	void Die_RecallObj(CMHFile* pFile) ;										// ������Ʈ�� �������, �ٸ� ��ȯ���� ��ȯ�ϴ� ������ �߰��ϴ� �Լ�.

	void Check_Die_Obj(CMHFile* pFile) ;										// ������Ʈ�� �Ҹ�Ǿ����� ���θ� üũ�ϴ�, ������ �߰��ϴ� �Լ�.
	void Check_Use_Item(CMHFile* pFile) ;										// �������� ����ؾ� �ϴ� ������ �߰��ϴ� �Լ�.


	//////////////////////////////////////////
	// [ ��Ʈ��ũ �޽����� �Ľ��ϴ� ��Ʈ. ] //
	//////////////////////////////////////////
	void UserMsgParser(DWORD dwIndex, char* pMsg, DWORD dwLength) ;				// Ŭ���̾�Ʈ�� ������ �޽����� �Ľ��ϴ� �Լ�.
	void ServerMsgPaser(DWORD dwIndex, char* pMsg, DWORD dwLength) ;			// ������ ������ �޽����� �Ľ��ϴ� �Լ�.

	void Execute_Step(DWORD dwIndex, char* pMsg, DWORD dwLength) ;				// ���� �� ������ �����ϴ� �Լ�.
	void Execute_AddObj(DWORD dwIndex, CSiegeStep* pStep, BYTE byChildIdx) ;	// ������Ʈ �߰��� �����ϴ� �Լ�.
	void Execute_TermsAndCommand(CSiegeStep* pStep, CSiege_AddObj* pAddObj) ;	// �������� / ���� ���� / ���� ��ɾ �����ϴ� �Լ�.

	void Remove_Object(char* pMsg) ;											// ��� �� ������ ������Ʈ����, ���/������Ʈ Ÿ�Կ� ���� �����ϴ� �Լ�.
	void Remove_Object_SpecifyMap(char* pMsg) ;									// ���� �� �� ������ ������Ʈ����, ���/������Ʈ Ÿ�Կ� ���� �����ϴ� �Լ�.

	void Add_Obj_Ack(DWORD dwIndex, char* pMsg) ;								// ������Ʈ �߰� ���� �޽��� ó���� �ϴ� �Լ�.
	void Add_Obj_Nack(DWORD dwIndex, char* pMsg) ;								// ������Ʈ �߰� ���� �޽��� ó���� �ϴ� �Լ�.

	void Ready_Execute_Command(DWORD dwIndex, char* pMsg) ;						// The function to execute command

	void Request_ObjInfo_Syn(DWORD dwIndex, char* pMsg) ;						// �� ������ ����, ������Ʈ �ε� ������ ���� ó���ϴ� �Լ�.
	void Result_ObjInfo_Syn( LPQUERY pData, LPDBMESSAGE pMessage ) ;			// DB�� ��� �� ������Ʈ ������ �����Ͽ�, ������Ʈ ���ε� ó���� �Ѵ�.

	void RSiegeRecallUpdate( LPQUERY pData, LPDBMESSAGE pMessage ) ;			// DB�� ��� �� ������Ʈ ������ �����Ͽ�, ������Ʈ ������Ʈ ��� ó���� �Ѵ�.
	void RSiegeRecallInsert( LPQUERY pData, LPDBMESSAGE pMessage ) ;			// DB�� ��ȯ ������Ʈ ���� �߰� ��, �� ������ ��ȯ ��ɾ ������ ó���� �Ѵ�.
	//void RSiegeRecallRemove( LPQUERY pData, LPDBMESSAGE pMessage ) ;			// DB�� ��ȯ ������Ʈ ���� ���� ��, �� ������ ��ȯ ���� ���� ����� ������ ó���� �Ѵ�.

	// 081007 LYW --- SiegeRecallMgr : �� ��������, �ش���� ��ȯ���� ����� ��û�ϴ� �۾� �߰�.
	void RequestRecallCount_Syn( DWORD dwIndex, void* pMsg ) ;

	// 081011 LYW --- Protocol : ���� ��ȯ���� ��ȯ ���¸� ���� �ܰ�� �ѱ�� ġƮ ���� �߰�.
	void Cheat_Execute_NextStep( DWORD dwIndex, char* pMsg ) ;
	void Cheat_Execute_EndStep( DWORD dwIndex, char* pMsg ) ;


	////////////////////////////////
	// [ ���� �޽��� ó�� ��Ʈ. ] //
	////////////////////////////////
	void Throw_Error(char* szErr, char* szCaption) ;							// ���� �޽��� ó���� �ϴ� �Լ�.
	void WriteLog(char* pMsg) ;													// �α׸� ����� ó���� �ϴ� �Լ�.


	/////////////////////////
	// [ ġƮ ó�� ��Ʈ. ] //
	/////////////////////////
	void Cheat_Execute_Step(DWORD dwIndex, char* pMsg) ;						// The function to execute step by cheat code.
	//void Cheat_AddObj(DWORD dwIndex, void* pMsg) ;						// The function to add object by cheat code.
};
