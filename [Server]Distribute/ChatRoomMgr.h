#pragma once
//=================================================================================================
//	FILE		: CChatRoomMgr.h
//	DESC		: ä�ù� �ý����� �����ϴ� �Ŵ��� Ŭ����.
//	DATE		: APRIL 2, 2008 LYW
//				:
//	DESC		: FEBRUARY 25, 2009 - ���� �������� ���� STL MAP���� ���� �ϴ� ���� CYHHashTable
//				: ���� ������� ���� ��.
//=================================================================================================





//-------------------------------------------------------------------------------------------------
//		Include Part.
//-------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------------------------
//		Definition part.
//-------------------------------------------------------------------------------------------------
#define CHATROOMMGR		CChatRoomMgr::GetInstance()

// �� ��ũ�θ� ����ϸ�, �޽��� �ڽ��� ������ ����ϰ�, ������� ������ �α׸� ����.
//#define _USE_ERR_MSGBOX_

enum ENUM_PRINT_ERRORS														// ���� ���� Ÿ�� ����.
{
	err_IMP = 0,															// �Լ��� �Ķ���Ͱ� �߸��Ǿ���.
	err_FCMTB,																// �⺻ �޽����� ��ȯ�� �����Ͽ���.
	err_FCMTO,																// ���� �޽����� ��ȯ�� �����Ͽ���.
} ;

enum ENUM_CR_GUEST_COUNT													// ä�ù� �ִ� ������ ����.
{
	e_GC_20,																// 20��.
	e_GC_15,																// 15��.
	e_GC_10,																// 10��.
	e_GC_5,																	// 5��.

	e_GC_Max,
} ;

enum ENUM_CR_ROOM_TYPE														// ä�ù� �з� ����.
{
	e_RT_Party = 0,															// ��Ƽ����.
	e_RT_Deal,																// ��ǰ�ŷ�.
	e_RT_Normal_Chat,														// �Ϲ�ä��.
	e_RT_Local_Chat,														// ����ä��.
	e_RT_Make_Friend,														// ģ��ã��.

	e_RT_Max,
} ;

enum ENUM_CR_ROOM_MODE														// ä�ù� ����/�����.
{	
	e_RM_Open = 0 ,															// ����.
	e_RM_Close,																// �����.

	e_RM_Max,
} ;

enum ENUM_CR_MAIN_TYPE
{
	e_RTM_AllLooK = 0,														// ��ü����.
	e_RTM_Party ,															// ��Ƽ����.
	e_RTM_Deal,																// ��ǰ�ŷ�.
	e_RTM_Normal_Chat,														// �Ϲ�ä��.
	e_RTM_Local_Chat,														// ����ä��.
	e_RTM_Make_Friend,														// ģ��ã��.
	e_RTM_Searched,															// �˻����

	e_RTM_Max,
} ;

struct ST_SEARCH_NAME														// �̸� �˻��� ����ü.
{
	BYTE byRoomIdx ;														// ���ε���.
	char name[ MAX_NAME_LENGTH+1 ] ;										// �̸�.
} ;





//-------------------------------------------------------------------------------------------------
//	The ChatRoom Manager.
//-------------------------------------------------------------------------------------------------
class CChatRoomMgr
{
	CYHHashTable< ST_CR_USER >		m_htUser ; 								// ������ ���� �� �ؽ� ���̺�.
	CYHHashTable< ST_SEARCH_NAME >	m_htSearchName ;						// �˻��� ���� �̸��� ���� �� �ؽ� ���̺�.
	
	cPtrList					m_pRoomList ;								// ä�ù��� �����ϴ� ����Ʈ.
	cPtrList					m_pTitleList ;								// �˻��� ������ �����ϴ� ����Ʈ.

	BYTE						m_byRoomIdx[ MAX_ROOM_COUNT+1 ] ;			// ä�ù� �ε��� ���� �迭.


public:
	CChatRoomMgr( void ) ;													// ������ �Լ�.
	virtual ~CChatRoomMgr( void ) ;											// �Ҹ��� �Լ�.

	static CChatRoomMgr* GetInstance() ;

	void NetworkMsgParser( DWORD dwIndex, char* pMsg, DWORD dwLength ) ;	// ��Ʈ�p �޽��� �ļ� �Լ�.

private :
	// ����Ʈ ���� �Լ���.
	void ReleaseRoom() ;													// ä�ù� ���ҽ� ���� �Լ�.

	BYTE IsInRoomList( BYTE byRoomIdx ) ;									// �� ����Ʈ��, �� ������ �ִ��� üũ�ϴ� �Լ�.

	BYTE AddRoom( ST_CR_ROOM_SRV* pInfo ) ;									// ä�ù� ������ �߰��ϴ� �Լ�.
	BYTE DeleteRoom( BYTE byRoomIdx ) ;										// ä�ù� ������ �����ϴ� �Լ�.

	ST_CR_ROOM_SRV* GetRoomInfo( BYTE byRoomIdx ) ;							// ä�ù� ������ ��ȯ�ϴ� �Լ�.

	BYTE GetRoomCountByType( BYTE byType ) ;								// Ÿ�Ժ� �� ���� ��ȯ �Լ�.

	BYTE GetEmptyRoomIdx() ;												// ����ִ� �� �ε��� ��ȯ �Լ�.

	DWORD GetHashCode( const char* pName ) ;								// ���ڷ� �־��� ��Ʈ���� �ؽ��ʿ� ����� �ڵ� ������ ��ȯ�ϴ� �Լ�.

	void InsertUserName( char* pName ) ;									// �˻��� �ؽ��ʿ� ���� �̸��� �߰��ϴ� �Լ�.
	void EraseUserName( char* pName ) ;										// �˻��� �ؽ����� ���� �̸��� �����ϴ� �Լ�.

	void UpdateNameRoomIdx( char* pName, BYTE byRoomIdx ) ;					// �˻��� �ؽ����� ������ ������ �� ��ȣ�� ������Ʈ �ϴ� �Լ�.

	void UpdateSearchRoomInfo( ST_CR_ROOM_SRV* pInfo ) ;					// �˻��� ���� ������ ������Ʈ �ϴ� �Լ�.

private :
	// ���� �޽��� ���� �Լ�.
	void SendErrors( DWORD dwIndex, DWORD dwPlayerID, BYTE byProtocol, BYTE byErr ) ;

	// ó�� �Լ���.
	void Add_User_Syn( DWORD dwIndex, char* pMsg, DWORD dwLength ) ;		// ���� �߰� �Լ�.
	void Force_Add_User_Syn( DWORD dwIndex, char* pMsg, DWORD dwLength ) ;	// ������ ������ �߰��ϴ� �Լ�.
	void Del_User_Syn( DWORD dwIndex, char* pMsg, DWORD dwLength ) ;		// ���� ���� �Լ�.
	void Room_Syn( DWORD dwIndex, char* pMsg, DWORD dwLength ) ;			// ä�ù� ����Ʈ�� �����ִ� �Լ�.
	void Create_Room_Syn( DWORD dwIndex, char* pMsg, DWORD dwLength ) ;		// ä�ù��� �����ϴ� �Լ�.
	void Join_Room_Syn( DWORD dwIndex, char* pMsg, DWORD dwLength ) ;		// ������ ä�ù濡 ������Ű�� �Լ�.
	void Out_Room_Syn( DWORD dwIndex, char* pMsg, DWORD dwLength ) ;		// ������ ä�ù濡�� �������� �Լ�.
	void Option_Syn( DWORD dwIndex, char* pMsg, DWORD dwLength ) ;			// �ɼ��� �����ϴ� �Լ�.
	void Owner_Syn( DWORD dwIndex, char* pMsg, DWORD dwLength ) ;			// ������ �����ϴ� �Լ�.
	void Kick_Syn( DWORD dwIndex, char* pMsg, DWORD dwLength ) ;			// ������ ���� �����Ű�� �Լ�.
	void Friend_Syn( DWORD dwIndex, char* pMsg, DWORD dwLength ) ;			// ģ����û�� �� ��, ���� ������ üũ�ϴ� �Լ�.
	void UpdateUserInfo( DWORD dwIndex, char*pMsg, DWORD dwLength ) ;		// ���� ������ ������Ʈ �ϴ� �Լ�.
	void SearchName_Syn( DWORD dwIndex, char*pMsg, DWORD dwLength ) ;		// �̸����� ä�ù� ������ �˻��ϴ� �Լ�.
	void SearchTitle_Syn( DWORD dwIndex, char*pMsg, DWORD dwLength ) ;		// �������� ä�ù� ������ �˻��ϴ� �Լ�.

private :
	void UpdateInfo_Created_Room( ST_CR_ROOM_SRV* pInfo ) ;
	void UpdateInfo_Deleted_Room( ST_CR_ROOM_SRV* pInfo ) ;

	void UpdateInfo_Changed_SecretMode( ST_CR_ROOM_SRV* pInfo ) ;
	void UpdateInfo_Changed_RoomType( ST_CR_ROOM_SRV* pInfo ) ;
	void UpdateInfo_Changed_Title( ST_CR_ROOM_SRV* pInfo ) ;
	void UpdateInfo_Changed_Current_GuestCount( ST_CR_ROOM_SRV* pInfo ) ;
	void UpdateInfo_Changed_Total_GuestCount( ST_CR_ROOM_SRV* pInfo ) ;

private :
	// ���� ó���� �ϴ� �Լ���.
	void Throw_Error( BYTE errType, char* szCaption ) ;						// ����Ÿ������ ���� ó���� �Ѵ�.
	void Throw_Error( char* szErr, char* szCaption ) ;						// ���� �޽����� ���� ó���� �Ѵ�.
	void Throw_Error( char* szCommonErr, char* szErr, char* szCaption ) ;	// ���� �޽����� ���� ó���� �Ѵ�.
	void WriteLog( char* pMsg ) ;											// �α׸� ����� �Լ�.	
};
