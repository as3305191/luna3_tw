//=================================================================================================
//	FILE		: CChatRoomMgr.cpp
//	DESC		: ä�ù� �Ŵ��� ������.
//	DATE		: APRIL 2, 2008 LYW
//				:
//	DESC		: FEBRUARY 25, 2009 - ���� �������� ���� STL MAP���� ���� �ϴ� ���� CYHHashTable
//				: ���� ������� ���� ��.
//=================================================================================================





//-------------------------------------------------------------------------------------------------
//		������� ����.
//-------------------------------------------------------------------------------------------------
#include "stdafx.h"

#include "../[CC]ServerModule/Network.h"

#include "./ChatRoomMgr.h"





//-------------------------------------------------------------------------------------------------
//	NAME : CChatRoomMgr
//	DESC : ������ �Լ�.
//  DATE : APRIL 2, 2008 LYW
//-------------------------------------------------------------------------------------------------
CChatRoomMgr::CChatRoomMgr( void) 
{
	// ���� ������ �ϴ� �ؽ� ���̺��� �ʱ�ȭ�Ѵ�.
	m_htUser.Initialize( MAX_USER_BUCKETCNT ) ;



	// �˻��� �̸��� �����ϴ� �ؽ� ���̺��� �ʱ�ȭ�Ѵ�.
	m_htSearchName.Initialize( MAX_USER_BUCKETCNT ) ;

    

	// ä�ù� �ε��� ���� �迭 �ʱ�ȭ.
	memset( m_byRoomIdx, 0, sizeof(BYTE)*MAX_ROOM_COUNT ) ;

	m_byRoomIdx[0] = 1 ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : ~CChatRoomMgr
//	DESC : �Ҹ��� �Լ�.
//  DATE : APRIL 2, 2008 LYW
//-------------------------------------------------------------------------------------------------
CChatRoomMgr::~CChatRoomMgr( void )
{
	// ä�ù�/ �˻��� ä�ù� ����.
	ReleaseRoom() ;



	// �˻��� �̸� ���� �ؽ� ���̺��� �����Ѵ�.
	ST_SEARCH_NAME* pNameInfo = NULL ;
	m_htSearchName.SetPositionHead() ;
	while( (pNameInfo = m_htSearchName.GetData()) != NULL )
	{
		delete pNameInfo ;
		pNameInfo = NULL ;
	}
	m_htSearchName.RemoveAll() ;



	// ���� ������ �ؽ� ���̺��� �����Ѵ�.
	ST_CR_USER* pUserInfo = NULL ;
	m_htUser.SetPositionHead() ;
	while( (pUserInfo = m_htUser.GetData()) != NULL )
	{
		delete pUserInfo ;
		pUserInfo = NULL;
	}
	m_htUser.RemoveAll() ;
}




//-------------------------------------------------------------------------------------------------
//	NAME		: GetInstance()
//	DESC		: The function that returns this class's instance to static type.
//	PROGRAMMER	: Yongs Lee
//	DATE		: February 25, 2009
//-------------------------------------------------------------------------------------------------
CChatRoomMgr* CChatRoomMgr::GetInstance()
{
	// ����: http://www.codeproject.com/useritems/VC2003MeyersSingletonBug.asp
	//
	// ���� �ζ��� �Լ��� �����ø��� �����ȴ�(�ζ��� �Լ��� ���� ��ġ�� �� ��ũ���� �ʴ´�). 
	// �̷� ���� �̱��� �������� �����ø��� �����ȴ�!
	// �̸� �������� ������ ��ũ ������ ���������Ѵ�. C ���Ͽ� ���� �����ϸ� �ذ�ȴ�.



	static CChatRoomMgr instance ;

	return &instance ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : ReleaseRoom
//	DESC : ä�ù� ���ҽ� ���� �Լ�.
//  DATE : APRIL 22, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::ReleaseRoom()
{
	// �� ����.
	ST_CR_ROOM_SRV* pInfo = NULL ;
	PTRLISTPOS pos = NULL ;

	pos = m_pRoomList.GetHeadPosition() ;

	while( pos )
	{
		pInfo = (ST_CR_ROOM_SRV*)m_pRoomList.GetNext( pos ) ;

		if( !pInfo ) continue ;

		m_pRoomList.Remove( pInfo ) ;

		delete pInfo ;
		pInfo = NULL ;
	}

	m_pRoomList.RemoveAll() ;



	// �˻��� �� ����.
	ST_CR_ROOM_CLT* pTitle = NULL ;
	pos = m_pRoomList.GetHeadPosition() ;

	while( pos )
	{
		pTitle = (ST_CR_ROOM_CLT*)m_pTitleList.GetNext( pos ) ;

		if( !pTitle ) continue ;

		m_pTitleList.Remove( pTitle ) ;

		delete pTitle ;
		pTitle = NULL ;
	}

	m_pTitleList.RemoveAll() ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : IsInRoomList
//	DESC : �� ����Ʈ��, �� ������ �ִ��� üũ�ϴ� �Լ�.
//  DATE : APRIL 22, 2008 LYW
//-------------------------------------------------------------------------------------------------
BYTE CChatRoomMgr::IsInRoomList( BYTE byRoomIdx ) 
{
	ST_CR_ROOM_SRV* pItem ;

	PTRLISTPOS pos = NULL ;
	pos = m_pRoomList.GetHeadPosition() ;

	while( pos )
	{
		pItem = (ST_CR_ROOM_SRV*)m_pRoomList.GetNext( pos ) ;

		if( !pItem ) continue ;

		if( pItem->byIdx == byRoomIdx ) return TRUE ;
	}

	return FALSE ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : AddRoom
//	DESC : ä�ù� ������ �߰��ϴ� �Լ�.
//		 : ä�ù� ���� �� �ƴ϶�, �˻��� ä�ù� ������ ���� �߰��� �Ѵ�.
//  DATE : APRIL 22, 2008 LYW
//-------------------------------------------------------------------------------------------------
BYTE CChatRoomMgr::AddRoom( ST_CR_ROOM_SRV* pInfo )
{
	// �Լ� �Ķ���� üũ.
	ASSERT( pInfo ) ;

	if( !pInfo )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return FALSE ;
	}



	// ���߰�.
	m_pRoomList.AddTail( pInfo ) ;



	// �� �ε��� ����.
	m_byRoomIdx[ pInfo->byIdx ] = pInfo->byIdx ;



	// �˻� �� �� �߰�.
	ST_CR_ROOM_CLT* pTitle = NULL ;
	pTitle = new ST_CR_ROOM_CLT ;

	ASSERT( pInfo ) ;

	if( !pInfo )
	{
		Throw_Error( "Failed to create title room!!", __FUNCTION__ ) ;
		return FALSE ;
	}

	pTitle->byIdx				= pInfo->byIdx ;
	pTitle->bySecretMode		= pInfo->bySecretMode ;
	pTitle->byRoomType			= pInfo->byRoomType ;
	pTitle->byCurGuestCount		= pInfo->byCurGuestCount ;
	pTitle->byTotalGuestCount	= pInfo->byTotalGuestCount ;

	SafeStrCpy( pTitle->title, pInfo->title, TITLE_SIZE ) ;

	m_pTitleList.AddTail( pTitle ) ;

	return TRUE ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : DeleteRoom
//	DESC : ä�ù� ������ �����ϴ� �Լ�.
//  DATE : APRIL 22, 2008 LYW
//-------------------------------------------------------------------------------------------------
BYTE CChatRoomMgr::DeleteRoom( BYTE byRoomIdx )
{
	// �� ���� ó��.
	ST_CR_ROOM_SRV* pItem = NULL ;

	PTRLISTPOS pos = NULL ;
	pos = m_pRoomList.GetHeadPosition() ;

	while( pos )
	{
		pItem = (ST_CR_ROOM_SRV*)m_pRoomList.GetNext( pos ) ;

		if( !pItem ) continue ;

		if( pItem->byIdx != byRoomIdx ) continue ;

		m_pRoomList.Remove( pItem ) ;

		delete pItem ;
		pItem = NULL ;

		m_byRoomIdx[ byRoomIdx ] = 0 ;

		break ;
	}



	// �˻��� �� ���� ó��.
	ST_CR_ROOM_CLT* pTitle ;
	pos = m_pTitleList.GetHeadPosition() ;

	while( pos )
	{
		pTitle = NULL ;
		pTitle = (ST_CR_ROOM_CLT*)m_pTitleList.GetNext( pos ) ;

		if( !pTitle ) continue ;

		if( pTitle->byIdx != byRoomIdx ) continue ;

		m_pTitleList.Remove( pTitle ) ;

		delete pTitle ;
		pTitle = NULL ;

		break ;
	}

	return TRUE ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : GetRoomInfo
//	DESC : ä�ù� ������ ��ȯ�ϴ� �Լ�.
//  DATE : APRIL 22, 2008 LYW
//-------------------------------------------------------------------------------------------------
ST_CR_ROOM_SRV* CChatRoomMgr::GetRoomInfo( BYTE byRoomIdx ) 
{
	ST_CR_ROOM_SRV* pItem ;

	PTRLISTPOS pos = NULL ;
	pos = m_pRoomList.GetHeadPosition() ;

	while( pos )
	{
		pItem = (ST_CR_ROOM_SRV*)m_pRoomList.GetNext( pos ) ;

		if( !pItem ) continue ;

		if( pItem->byIdx != byRoomIdx ) continue ;

		return pItem ;
	}

	return NULL ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : GetRoomCountByType
//
//	DESC : ä�ù� �з��� ����, ����Ʈ�� ��� ��� �ִ��� ��ȯ�ϴ� �Լ�.
//		 : 
//		 : * �� ������ �ִ� 250�� �̹Ƿ� BYTE���� ����Ѵ�.
//		 : 
//  DATE : APRIL 22, 2008 LYW
//-------------------------------------------------------------------------------------------------
BYTE CChatRoomMgr::GetRoomCountByType( BYTE byType )
{
	BYTE byRoomCount = 0 ;

	ST_CR_ROOM_SRV* pItem ;

	PTRLISTPOS pos = NULL ;
	pos = m_pRoomList.GetHeadPosition() ;

	while( pos )
	{
		pItem = NULL ;
		pItem = (ST_CR_ROOM_SRV*)m_pRoomList.GetNext( pos ) ;

		if( !pItem ) continue ;

		if( pItem->byRoomType != byType ) continue ;

		++byRoomCount ;
	}

	return byRoomCount ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : SendErrors
//	DESC : ���� �޽����� �����ϴ� �Լ�.
//  DATE : APRIL 7, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::SendErrors( DWORD dwIndex, DWORD dwPlayerID, BYTE byProtocol, BYTE byErr ) 
{
	MSG_BYTE msg ;
	memset( &msg, 0, sizeof(MSG_BYTE) ) ;

	msg.Category	= MP_CHATROOM ;
	msg.Protocol	= byProtocol ;
	msg.dwObjectID	= dwPlayerID ;

	msg.bData		= byErr ;

	g_Network.Send2Server( dwIndex, (char*)&msg, sizeof(MSG_BYTE) ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : NetworkMsgParser
//	DESC : ä�ù�� ������, Dist ������ �Ѿ���� �޽����� ó���ϴ� �Լ�.
//  DATE : APRIL 2, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::NetworkMsgParser( DWORD dwIndex, char* pMsg, DWORD dwLength )
{
	// �Լ� �Ķ���� üũ.
	ASSERT( pMsg ) ;

	if( !pMsg )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return ;
	}



	// �⺻ �޽��� ��ȯ.
	MSGBASE* pmsg = NULL ;
	pmsg = (MSGBASE*)pMsg ;

	ASSERT( pmsg ) ;

	if( !pmsg )
	{
		Throw_Error( err_FCMTB, __FUNCTION__ ) ;
		return ;
	}



	// �������� üũ.
	switch( pmsg->Protocol )
	{
	case MP_CHATROOM_ADD_USER_SYN :			Add_User_Syn( dwIndex, pMsg, dwLength ) ;		break ;
	case MP_CHATROOM_FORCE_ADD_USER_SYN :	Force_Add_User_Syn( dwIndex, pMsg, dwLength ) ;	break ;
	case MP_CHATROOM_DEL_USER_SYN :			Del_User_Syn( dwIndex, pMsg, dwLength ) ;		break ;
	case MP_CHATROOM_ROOM_SYN :				Room_Syn( dwIndex, pMsg, dwLength ) ;			break ;
	case MP_CHATROOM_CREATE_ROOM_SYN :		Create_Room_Syn( dwIndex, pMsg, dwLength ) ;	break ;
	case MP_CHATROOM_JOIN_ROOM_SYN :		Join_Room_Syn( dwIndex, pMsg, dwLength ) ;		break ;
	case MP_CHATROOM_OUT_ROOM_SYN :			Out_Room_Syn( dwIndex, pMsg, dwLength ) ;		break ;
	case MP_CHATROOM_CHANGE_OPTION_SYN :	Option_Syn( dwIndex, pMsg, dwLength ) ;			break ;	
	case MP_CHATROOM_CHANGE_OWNER_SYN :		Owner_Syn( dwIndex, pMsg, dwLength ) ;			break ;	
	case MP_CHATROOM_KICK_GUEST_SYN :		Kick_Syn( dwIndex, pMsg, dwLength ) ;			break ;	
	case MP_CHATROOM_REQUEST_FRIEND_SYN :	Friend_Syn( dwIndex, pMsg, dwLength ) ;			break ;	
	case MP_CHATROOM_UPDATE_USERINFO_SYN :	UpdateUserInfo( dwIndex, pMsg, dwLength ) ;		break ;	
	case MP_CHATROOM_SEARCH_FOR_NAME_SYN :	SearchName_Syn( dwIndex, pMsg, dwLength ) ;		break ;	
	case MP_CHATROOM_SEARCH_FOR_TITLE_SYN :	SearchTitle_Syn( dwIndex, pMsg, dwLength ) ;	break ;	
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : Add_GuestInfo_Syn
//		 : 
//	DESC : ���� ������ ����ϴ� �Լ�.
//		 :
//		 : 1. ä������ �������� üũ.
//		 : 2. ���� ���.
//		 : 3. �˻��� �̸� ���.
//		 : 4. ��� ����.
//		 :
//  DATE : APRIL 4, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Add_User_Syn( DWORD dwIndex, char* pMsg, DWORD dwLength ) 
{
	// �Լ� �Ķ���� üũ.
	ASSERT( pMsg ) ;

	if( !pMsg )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return ;
	}



	// ���� �޽��� ��ȯ.
	MSG_CR_USER* pmsg = NULL ;
	pmsg = (MSG_CR_USER*)pMsg ;

	ASSERT( pmsg ) ;

	if( !pmsg )
	{
		Throw_Error( err_FCMTO, __FUNCTION__ ) ;
		return ;
	}



	// ä�������� üũ.
	ST_CR_USER* pUserInfo = m_htUser.GetData( pmsg->user.dwPlayerID ) ;
	if( pUserInfo ) return ;



	// ���� ���� ���.
	ST_CR_USER* pNewUser = new ST_CR_USER ;
	ASSERT( pNewUser ) ;
	if( !pNewUser ) return ;

	memcpy( pNewUser, &pmsg->user, sizeof(ST_CR_USER) ) ;
	pNewUser->dwConnectionIdx = dwIndex ;

	m_htUser.Add( pNewUser, pNewUser->dwPlayerID ) ;



	// ��� ����.
	MSGBASE msg ;
	memset(&msg, 0, sizeof(MSGBASE)) ;

	msg.Category	= MP_CHATROOM ;
	msg.Protocol	= MP_CHATROOM_ADD_USER_ACK ;
	msg.dwObjectID	= pmsg->dwObjectID ;

	g_Network.Send2Server( dwIndex, (char*)&msg, sizeof(MSGBASE) ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : Force_Add_User_Syn
//		 : 
//	DESC : �̹� ��ϵǾ� �ִ� ���� ������ �ִ�����, ������ ������ ����ϰ�, ä�ù� ����Ʈ�� �����Ѵ�.
//		 : 
//		 : 1. ��� �� �������� üũ.
//		 :    1-1. ä�������� üũ.
//		 :	  1-2. ä�ù� �ޱ�.
//		 :    1-3. ���� �ƿ� ó��.
//		 :    1-4. �˻��� �̸� ����.
//		 :    1-5. ���� ���� ����.
//		 : 2. ���� ���.
//		 : 3. �˻��� �̸� ���.
//		 : 
//  DATE : APRIL 22, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Force_Add_User_Syn( DWORD dwIndex, char* pMsg, DWORD dwLength ) 
{
	// �Լ� �Ķ���� üũ.
	ASSERT( pMsg ) ;

	if( !pMsg )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return ;
	}



	// ���� �޽��� ��ȯ.
	MSG_CR_USER* pmsg = NULL ;
	pmsg = (MSG_CR_USER*)pMsg ;

	ASSERT( pmsg ) ;

	if( !pmsg )
	{
		Throw_Error( err_FCMTO, __FUNCTION__ ) ;
		return ;
	}



	// ���� �ޱ�.
	ST_CR_USER* pUserInfo = m_htUser.GetData( pmsg->user.dwPlayerID ) ;
	if( pUserInfo )
	{
		// �� �ޱ�.
		ST_CR_ROOM_SRV* pRoom = NULL ;
		pRoom = GetRoomInfo( pUserInfo->byIdx ) ;

		// �� ������,
		if( pRoom )
		{
			// �ƿ� ó��.
			MSGBASE msg ;
			memset( &msg, 0, sizeof(MSGBASE) ) ;

			msg.Category	= MP_CHATROOM ;
			msg.Protocol	= MP_CHATROOM_OUT_ROOM_SYN ;
			msg.dwObjectID	= pmsg->dwObjectID ;

			Out_Room_Syn( dwIndex, (char*)&msg, sizeof(MSGBASE) ) ;
		}

		// ���� ���� ����.
		m_htUser.Remove( pUserInfo->dwPlayerID ) ;
	}



	// ���� ���.
	ST_CR_USER* pNewUser = new ST_CR_USER ;
	ASSERT( pNewUser ) ;
	if( !pNewUser ) return ;

	memcpy( pNewUser, &pmsg->user, sizeof(ST_CR_USER) ) ;
	pNewUser->dwConnectionIdx = dwIndex ;

	m_htUser.Add( pNewUser, pNewUser->dwPlayerID ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : Del_GuestInfo_SynDWORD
//		 : 
//	DESC : ĳ���Ͱ� �α� �ƿ��� ���� ��, ���� ������ ������ ���� ��Ű�� �Լ�.
//		 : 
//		 : 1. ��� �� �������� üũ.
//		 :    1-1. ä�������� üũ.
//		 :	  1-2. ä�ù� �ޱ�.
//		 :    1-3. ���� �ƿ� ó��.
//		 : 2. ���� ����.
//		 : 3. �˻��� �̸� ����.
//		 : 
//  DATE : APRIL 22, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Del_User_Syn( DWORD dwIndex, char* pMsg, DWORD dwLength ) 
{
	// �Լ� �Ķ���� üũ.
	ASSERT( pMsg ) ;

	if( !pMsg )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return ;
	}



	// ���� �޽��� ��ȯ.
	MSGBASE* pmsg = NULL ;
	pmsg = (MSGBASE*)pMsg ;

	ASSERT( pmsg ) ;

	if( !pmsg )
	{
		Throw_Error( err_FCMTO, __FUNCTION__ ) ;
		return ;
	}



	// ���� �ޱ�.
	ST_CR_USER* pUserInfo = m_htUser.GetData( pmsg->dwObjectID ) ;
	if( pUserInfo )
	{
		// �� �ޱ�.
		ST_CR_ROOM_SRV* pRoom = NULL ;
		pRoom = GetRoomInfo( pUserInfo->byIdx ) ;

		// �� ������,
		if( pRoom )
		{
			// �ƿ� ó��.
			MSGBASE msg ;
			memset( &msg, 0, sizeof(MSGBASE) ) ;

			msg.Category	= MP_CHATROOM ;
			msg.Protocol	= MP_CHATROOM_OUT_ROOM_SYN ;
			msg.dwObjectID	= pmsg->dwObjectID ;

			Out_Room_Syn( dwIndex, (char*)&msg, sizeof(MSGBASE) ) ;
		}

		// �˻��� ���� �̸� ����.
		EraseUserName( pUserInfo->name ) ;

		// ���� ���� ����.
		m_htUser.Remove( pUserInfo->dwPlayerID ) ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : RoomList_Syn
//		 : 
//	DESC : ��û�� ���� �������� �ش��ϴ� �� ������ ��Ƽ� Agent�� �����Ѵ�.
//		 : 
//		 : 1. ����Ʈ ��� ����.
//		 : 2. ä�ù� �з��� ���� �������� ��´�.
//		 : 3. ������ ����.
//		 : 4. �����ϴ� ����Ʈ �з� ����.
//		 : 5. ��� ����.
//		 : 
//  DATE : APRIL 22, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Room_Syn( DWORD dwIndex, char* pMsg, DWORD dwLength ) 
{
	// ���� �޽��� ����.
	// 0 = err_Empty_Room,
	// 1 = err_Delay_Time,



	// �Լ� �Ķ���� üũ.
	ASSERT( pMsg ) ;

	if( !pMsg )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return ;
	}



	// ���� �޽��� ��ȯ.
	MSG_BYTE2* pmsg = NULL ;
	pmsg = (MSG_BYTE2*)pMsg ;

	ASSERT( pmsg ) ;

	if( !pmsg )
	{
		Throw_Error( err_FCMTO, __FUNCTION__ ) ;
		return ;
	}



	// ���� �޽��� ����.
	MSG_CR_ROOMLIST msg ;
	memset( &msg, 0, sizeof(MSG_CR_ROOMLIST) ) ;

	msg.Category	= MP_CHATROOM ;
	msg.Protocol	= MP_CHATROOM_ROOMLIST_ACK ;
	msg.dwObjectID	= pmsg->dwObjectID ;



	// �������� ���� ���� �ε��� ����.
	BYTE byIndex = 0 ;
	BYTE byStartIdx = 0 ;
	byStartIdx = pmsg->bData2 * ROOM_COUNT_PER_PAGE ;



	// ����Ʈ ��� ����.
	ST_CR_ROOM_SRV* pInfo = NULL ;
	PTRLISTPOS pos = NULL ;
	pos = m_pRoomList.GetHeadPosition() ;

	while( pos )
	{
		// ������ �ޱ�.
		pInfo = (ST_CR_ROOM_SRV*)m_pRoomList.GetNext( pos ) ;
		if( !pInfo ) continue ;

		// �ε��� ��ȿ üũ.
		if( byIndex < byStartIdx )
		{
			++byIndex ;
			continue ;
		}

		// ���� �� ���� üũ.
		if( msg.byCount >= ROOM_COUNT_PER_PAGE ) break ;

		if( pmsg->bData1 != e_RTM_AllLooK )
		{
			if( pInfo->byRoomType != pmsg->bData1-1 ) continue ;
		}

		// �� ���� ���.
		msg.room[ msg.byCount ].byIdx				= pInfo->byIdx ;
		msg.room[ msg.byCount ].bySecretMode		= pInfo->bySecretMode ;
		msg.room[ msg.byCount ].byRoomType			= pInfo->byRoomType ;
		msg.room[ msg.byCount ].byCurGuestCount		= pInfo->byCurGuestCount ;
		msg.room[ msg.byCount ].byTotalGuestCount	= pInfo->byTotalGuestCount ;

		SafeStrCpy( msg.room[ msg.byCount ].title, pInfo->title, TITLE_SIZE ) ;

		++msg.byCount ;
	}



	// ������ ����.
	msg.byCurPage	= pmsg->bData2 ;
	if( pmsg->bData1 != e_RTM_AllLooK )
	{
		msg.byTotalPage	= GetRoomCountByType( pmsg->bData1-1 ) / ROOM_COUNT_PER_PAGE ;
	}
	else
	{
		msg.byTotalPage	= (BYTE)(m_pRoomList.GetCount() / ROOM_COUNT_PER_PAGE);
	}



	// ���� ä�ù� �з� ����.
	msg.byRoomType	= pmsg->bData1 ;



	// ��� ����.
	g_Network.Send2Server( dwIndex, (char*)&msg, sizeof(MSG_CR_ROOMLIST) ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : Create_Room_Syn
//		 : 
//	DESC : ä�ù� ���� ��û�� ���� �� ó���ϴ� �Լ�.
//		 : 
//		 : 1. ä�ù� ���� üũ.
//		 : 2. ��� �� �������� üũ.
//		 : 3. ä�������� üũ.
//		 : 4. ä�ù� ����.
//		 : 5. ���� ����Ʈ�� �� �߰�.
//		 : 6. ������ ä�ù� ��ȣ ����.
//		 : 7. ��� ����.
//		 :
//  DATE : APRIL 4, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Create_Room_Syn( DWORD dwIndex, char* pMsg, DWORD dwLength ) 
{
	// ���� �޽��� ����.
	// 0 = err_Already_Chatting,
	// 1 = err_RoomCount_Over,
	// 2 = err_User_Is_Not_In_Lobby,
	// 3 = err_Registed_But_Not_Found_Room,
	// 4 = err_Registed_But_Not_Found_User,



	// �Լ� �Ķ���� üũ.
	ASSERT( pMsg ) ;

	if( !pMsg )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return ;
	}



	// ���� �޽��� ��ȯ.
	MSG_CR_ELEMENT* pmsg = NULL ;
	pmsg = (MSG_CR_ELEMENT*)pMsg ;

	ASSERT( pmsg ) ;

	if( !pmsg )
	{
		Throw_Error( err_FCMTO, __FUNCTION__ ) ;
		return ;
	}



	// ä�ù� ���� üũ.
	if( m_pRoomList.GetCount() >= MAX_ROOM_COUNT )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_CREATE_ROOM_NACK, 1 ) ;
		return ;
	}



	// ��� �� �������� üũ.
	ST_CR_USER* pUserInfo = m_htUser.GetData( pmsg->dwObjectID ) ;
	if( !pUserInfo )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_CREATE_ROOM_NACK, 2 ) ;
		return ;
	}



	// ä�������� üũ.
	if( pUserInfo->byIdx != 0 )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_CREATE_ROOM_NACK, 0 ) ;
		return ;
	}



	// ä�ù� ����.
	ST_CR_ROOM_SRV* pRoom = NULL ;
	pRoom = new ST_CR_ROOM_SRV ;

	ASSERT( pRoom ) ;

	if( !pRoom )
	{
		Throw_Error( "Failed to create chatroom !!", __FUNCTION__ ) ;
		return ;
	}



	// �⺻ �� ���� ����.
	pRoom->byIdx			= GetEmptyRoomIdx() ;
	pRoom->dwOwnerIdx		= pmsg->dwObjectID ;

	pRoom->bySecretMode		= pmsg->bySecretMode ;
	pRoom->byRoomType		= pmsg->byRoomType ;
	pRoom->byCurGuestCount	= 1 ;

	SafeStrCpy( pRoom->code, pmsg->code, SECRET_CODE_SIZE ) ;



	// �ִ� ���� �ο� ����.
	BYTE byTotalGuest = e_GC_20 ;

	switch( pmsg->byTotalGuestCount )
	{
	case e_GC_20 :	byTotalGuest = e_GC_20 ;	break ;
	case e_GC_15 :	byTotalGuest = e_GC_15 ;	break ;
	case e_GC_10 :	byTotalGuest = e_GC_10 ;	break ;
	case e_GC_5 :	byTotalGuest = e_GC_5 ;		break ;
	default :									break ;
	}

	pRoom->byTotalGuestCount	= byTotalGuest ;



	// �� ���� ����.
	SafeStrCpy( pRoom->title, pmsg->title, TITLE_SIZE ) ;



	// ������ ���̵� ����.
	pRoom->dwUser[ 0 ] = pUserInfo->dwPlayerID ;



	// ���� ����Ʈ�� �� �߰�.
	AddRoom( pRoom ) ;



	// ������ ä�ù� ��ȣ ����.
	pUserInfo->byIdx = pRoom->byIdx ;



	// �˻��� �̸� ���.
	InsertUserName( pUserInfo->name ) ;



	// �˻��� ���� ���� ����.
	UpdateNameRoomIdx( pUserInfo->name, pRoom->byIdx ) ;



	// ��� ����.
	MSG_CR_ROOMINFO msg ;
	memset( &msg, 0, sizeof(MSG_CR_ROOMINFO) ) ;

	msg.Category	= MP_CHATROOM ;
	msg.Protocol	= MP_CHATROOM_CREATE_ROOM_ACK ;
	msg.dwObjectID	= pmsg->dwObjectID ;

	msg.room		= *pRoom ;
	msg.user[ 0 ]	= *pUserInfo ;

	g_Network.Send2Server( dwIndex, (char*)&msg, sizeof(MSG_CR_ROOMINFO) ) ;



	// �ٸ� �����鿡�� ���� ����Ǿ����� �����Ѵ�.
	UpdateInfo_Created_Room( pRoom ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : Join_Room_Syn
//		 : 
//	DESC : ä�ù� ���� ��û�� ������ �� ó���ϴ� �Լ�.
//		 : 
//		 : 1. ��� �� �������� üũ.
//		 : 2. ä�������� üũ.
//		 : 3. ä�ù� �ޱ�.
//		 : 4. ��� ��ȣ üũ.
//		 : 5. ä�ù��� ���� �ο� üũ.
//		 : 6. ���� ������ �� üũ.
//		 : 7. ä�ù��� ������ �� ����.
//		 : 8. ������ ä�ù� �ε��� ����.
//		 : 9. ���� ���� �޽��� ó��.
//		 :10. ���� ���� �޽��� ó��.
//		 : 
//  DATE : APRIL 4, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Join_Room_Syn( DWORD dwIndex, char* pMsg, DWORD dwLength ) 
{
	// ���� �޽��� ����
    // 0 = err_Already_Chatting,
	// 1 = err_Invalid_Room_Info,
	// 2 = err_Invalid_Secret_Code,
	// 3 = err_Guest_Count_Is_Over,
	// 4 = err_Invalid_User_Info,



	// �Լ� �Ķ���� üũ.
	ASSERT( pMsg ) ;

	if( !pMsg )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return ;
	}



	// ���� �޽��� ��ȯ.
	MSG_CR_JOIN_SYN* pmsg = NULL ;
	pmsg = (MSG_CR_JOIN_SYN*)pMsg ;

	ASSERT( pmsg ) ;

	if( !pmsg )
	{
		Throw_Error( err_FCMTO, __FUNCTION__ ) ;
		return ;
	}



	// ��� �� �������� üũ.
	ST_CR_USER* pUserInfo = m_htUser.GetData( pmsg->dwObjectID ) ;
	if( !pUserInfo )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_JOIN_ROOM_NACK, 4 ) ;
		return ;
	}



	// ä�������� üũ.
	if( pUserInfo->byIdx != 0 )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_JOIN_ROOM_NACK, 0 ) ;
		return ;
	}



	// ä�ù� �ޱ�.
	if( pmsg->byRoomIdx == 0 )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_JOIN_ROOM_NACK, 1 ) ;
		return ;
	}

	ST_CR_ROOM_SRV* pRoom = NULL ;
	pRoom = GetRoomInfo( pmsg->byRoomIdx ) ;
	if( !pRoom )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_JOIN_ROOM_NACK, 1 ) ;
		return ;
	}



	// ��� ��ȣ üũ.
	if( pRoom->bySecretMode == e_RM_Close )
	{
		if( strcmp( pRoom->code, pmsg->code) != 0 )
		{
			SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_JOIN_ROOM_NACK, 2 ) ;
			return ;
		}
	}



	// ä�ù��� ���� �ο� üũ.
	BYTE byTotalCount = 0 ;
	switch( pRoom->byTotalGuestCount )
	{
	case e_GC_20 :	byTotalCount = 20 ; break ;
	case e_GC_15 :	byTotalCount = 15 ; break ;
	case e_GC_10 :	byTotalCount = 10 ; break ;
	case e_GC_5 :	byTotalCount =  5 ; break ;
	default :							break ;
	}



	// ���� ������ �� üũ.
	if( pRoom->byCurGuestCount >= byTotalCount )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_JOIN_ROOM_NACK, 3 ) ;
		return ;
	}



	// ������ �ε��� ����.
	pRoom->dwUser[ pRoom->byCurGuestCount ] = pUserInfo->dwPlayerID ;



	// ä�ù��� ������ �� ����.
	++pRoom->byCurGuestCount ;



	// ������ ä�ù� �ε��� ����.
	pUserInfo->byIdx = pRoom->byIdx ;



	// �˻��� �̸� ���.
	InsertUserName( pUserInfo->name ) ;



	// ������ �����ϴ� �� �ε��� ������Ʈ.
	UpdateNameRoomIdx( pUserInfo->name, pRoom->byIdx ) ;



	// ���� ���� �޽��� ó��.
	MSG_CR_ROOMINFO msg ;
	memset( &msg, 0, sizeof(MSG_CR_ROOMINFO) ) ;

	msg.Category	= MP_CHATROOM ;
	msg.Protocol	= MP_CHATROOM_JOIN_ROOM_ACK ;
	msg.dwObjectID	= pmsg->dwObjectID ;

	msg.room		= *pRoom ;



	// ������ ���� ���.
	ST_CR_USER* pMemberInfo = NULL ;
	for( BYTE count = 0 ; count < MAX_USER_PER_ROOM ; ++count )
	{
		pMemberInfo = m_htUser.GetData( pRoom->dwUser[ count ] ) ;
		if( !pMemberInfo ) continue ;

		msg.user[ msg.byCount ] = *pMemberInfo ;

		++msg.byCount ;
	}



	// ��� ����.
	g_Network.Send2Server( dwIndex, (char*)&msg, sizeof(MSG_CR_ROOMINFO) ) ;



	// ���� ���� �޽��� ó��.
	MSG_CR_JOIN_NOTICE msg2 ;
	memset( &msg2, 0, sizeof(MSG_CR_JOIN_NOTICE) ) ;

	msg2.Category	= MP_CHATROOM ;
	msg2.Protocol	= MP_CHATROOM_JOIN_ROOM_NOTICE ;
	msg2.dwObjectID	= pmsg->dwObjectID ;

	msg2.user		= *pUserInfo ;



	// ������ �ε��� ���.
	for( BYTE count = 0 ; count < pRoom->byCurGuestCount ; ++count )
	{
		pMemberInfo = m_htUser.GetData( pRoom->dwUser[ count ] ) ;
		if( !pMemberInfo ) continue ;

		if( pMemberInfo->dwPlayerID == pUserInfo->dwPlayerID ) continue ;

		msg2.dwUser[ msg2.byCount ] = pMemberInfo->dwPlayerID ;

		++msg2.byCount ;
	}

	// ��� ����.
	g_Network.Send2Server( dwIndex, (char*)&msg2, sizeof(MSG_CR_JOIN_NOTICE) ) ;



	// �ٸ� �����鿡�� ���� ����Ǿ����� �����Ѵ�.
	UpdateInfo_Changed_Current_GuestCount( pRoom ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : Out_Room_Syn
//		 : 
//	DESC : ������ ä�ù��� ������ ó���� �ϴ� �Լ�.
//		 : 
//		 : 1. ��� �� �������� üũ.
//		 : 2. ������ �ޱ�.
//		 : 3. �濡 �����ִ� ���� �� üũ.
//		 :	  3-1. �� ����.
//		 :	  3-2. ������ �� �ε��� ����.
//		 :	  3-3. ��� ����.
//		 : 4. �������� �ƴϸ�,
//		 :    4-1. ä�ù��� �������� �������� ���ҽ�Ų��.
//		 :	  4-2. ������ ������ �� �ε����� �ʱ�ȭ.
//		 :	  4-3. ������ ���� �޽��� ó��.
//		 :    4-4. ����� �����Ѵ�.
//		 :	  4-5. ������ �����ٸ�, ���� �ٲ� ����.
//		 : 
//  DATE : APRIL 4, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Out_Room_Syn( DWORD dwIndex, char* pMsg, DWORD dwLength ) 
{
	// ���� �޽��� ����.
	// 0 = err_CanNot_Found_Room,
	// 1 = err_CanNot_Found_Outer,



	// �Ķ���� üũ.
	ASSERT( pMsg ) ;

	if( !pMsg )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return ;
	}



	// ���� �޽��� ��ȯ.
	MSG_DWORD* pmsg = NULL ;
	pmsg = (MSG_DWORD*)pMsg ;

	ASSERT( pmsg ) ;

	if( !pmsg )
	{
		Throw_Error( err_FCMTO, __FUNCTION__ ) ;
		return ;
	}



	// ���� ���� �ޱ�.
	ST_CR_USER* pUserInfo = m_htUser.GetData( pmsg->dwObjectID ) ;
	if( !pUserInfo )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_OUT_ROOM_NACK, 1 ) ;
		return ;
	}



	// ������ �ޱ�.
	if( pUserInfo->byIdx == 0 )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_OUT_ROOM_NACK, 0 ) ;
		return ;
	}

	ST_CR_ROOM_SRV* pRoom = NULL ;
	pRoom = GetRoomInfo( pUserInfo->byIdx ) ;
	if( !pRoom )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_OUT_ROOM_NACK, 0 ) ;
		return ;
	}



	// ������ �� �ε��� ����.
	pUserInfo->byIdx = 0 ;



	// �˻��� ���� �̸� ����.
	EraseUserName( pUserInfo->name ) ;



	// �濡 �����ִ� ���� �� üũ.
	if( pRoom->byCurGuestCount == 1 )
	{
		// �ٸ� �����鿡�� ���� �����Ǿ����� �����Ѵ�.
		UpdateInfo_Deleted_Room( pRoom ) ;

		// �� ����.
		DeleteRoom( pRoom->byIdx ) ;

		// ���� ����ٰ�, �׳� ���� �޽��� ������ �ʰ�, Ŭ���̾�Ʈ�� ä�ù� ����Ʈ�� ��� ��쵵�� ����.
		if( m_pRoomList.GetCount() == 0 )
		{
			MSGBASE msg ;
			memset(&msg, 0, sizeof(MSGBASE)) ;

			msg.Category	= MP_CHATROOM ;
			msg.Protocol	= MP_CHATROOM_OUT_ROOM_EMPTYROOM ;
			msg.dwObjectID	= pmsg->dwObjectID ;

			g_Network.Send2Server( dwIndex, (char*)&msg, sizeof(MSGBASE) ) ;
			return ;
		}
		else
		{
			// ���� �޽��� ����.
			MSG_CR_ROOMLIST msg ;
			memset( &msg, 0, sizeof(MSG_CR_ROOMLIST) ) ;

			msg.Category	= MP_CHATROOM ;
			msg.Protocol	= MP_CHATROOM_OUT_ROOM_LAST_MAN ;
			msg.dwObjectID	= pmsg->dwObjectID ;

			// ����Ʈ ��� ����.
			ST_CR_ROOM_SRV* pInfo ;
			PTRLISTPOS pos = NULL ;

			pos = m_pRoomList.GetHeadPosition() ;

			// �������� ���� ���� �ε��� ����.
			BYTE byStartIdx = 0 ;
			byStartIdx = byStartIdx * ROOM_COUNT_PER_PAGE ;

			BYTE byIndex = 0 ;

			// ����Ʈ �˻�.
			while( pos )
			{
				// ������ �ޱ�.
				pInfo = NULL ;
				pInfo = (ST_CR_ROOM_SRV*)m_pRoomList.GetNext( pos ) ;

				if( !pInfo ) continue ;

				// �ε��� ��ȿ üũ.
				if( byIndex < byStartIdx ) continue ;

				// ���� �� ���� üũ.
				if( msg.byCount >= ROOM_COUNT_PER_PAGE ) break ;

				// �� ���� ���.
				msg.room[ msg.byCount ].byIdx				= pInfo->byIdx ;
				msg.room[ msg.byCount ].bySecretMode		= pInfo->bySecretMode ;
				msg.room[ msg.byCount ].byRoomType			= pInfo->byRoomType ;
				msg.room[ msg.byCount ].byCurGuestCount		= pInfo->byCurGuestCount ;
				msg.room[ msg.byCount ].byTotalGuestCount	= pInfo->byTotalGuestCount ;

				SafeStrCpy( msg.room[ msg.byCount ].title, pInfo->title, TITLE_SIZE ) ;

				++msg.byCount ;
			}

			// ������ ����.
			msg.byCurPage	= 0 ;
			msg.byTotalPage	= (BYTE)(m_pRoomList.GetCount() / ROOM_COUNT_PER_PAGE) ;

			// ���� ä�ù� �з� ����.
			msg.byRoomType	= e_RTM_AllLooK ;

			// ��� ����.
			g_Network.Send2Server( dwIndex, (char*)&msg, sizeof(MSG_CR_ROOMLIST) ) ;
		}
	}
	else
	{
		// ������ ���� ���� ���Ӿ��� ƨ��ų�, ä���� �����ߴ��� üũ.
		BYTE byChangedOwner = FALSE ;
		if( pRoom->dwOwnerIdx == pUserInfo->dwPlayerID ) byChangedOwner = TRUE ;

		// ���� �ε����� �ӽ� �����Ѵ�.
		BYTE byCheckCount = 0 ;
		DWORD dwPlayerID[ MAX_USER_PER_ROOM ] = {0, } ;
		for( BYTE count = 0 ; count < pRoom->byCurGuestCount ; ++count )
		{
			if( pRoom->dwUser[ count ] == pUserInfo->dwPlayerID ) continue ;

			dwPlayerID[ byCheckCount ] = pRoom->dwUser[ count ] ;
			++byCheckCount ;
		}

		// ä�ù��� ������ �� ����.
		--pRoom->byCurGuestCount ;

		// ���� �ε��� ����
		for( BYTE count = 0 ; count  < pRoom->byCurGuestCount ; ++count )
		{
			pRoom->dwUser[ count ] = dwPlayerID[ count ] ;
		}

		// ���� ���� ����.
		if( byChangedOwner )
		{
			pRoom->dwOwnerIdx = dwPlayerID[ 0 ] ;
		}

		// �ٸ� �����鿡�� ���� ����Ǿ����� �����Ѵ�.
		UpdateInfo_Changed_Current_GuestCount( pRoom ) ;


		// ���� �޽��� ����.
		MSG_CR_ROOMLIST msg ;
		memset( &msg, 0, sizeof(MSG_CR_ROOMLIST) ) ;

		msg.Category	= MP_CHATROOM ;
		msg.Protocol	= MP_CHATROOM_OUT_ROOM_ACK ;
		msg.dwObjectID	= pmsg->dwObjectID ;

		// ����Ʈ ��� ����.
		ST_CR_ROOM_SRV* pInfo ;
		PTRLISTPOS pos = NULL ;

		pos = m_pRoomList.GetHeadPosition() ;

		// �������� ���� ���� �ε��� ����.
		BYTE byStartIdx = 0 ;
		byStartIdx = byStartIdx * ROOM_COUNT_PER_PAGE ;

		BYTE byIndex = 0 ;

		// ����Ʈ �˻�.
		while( pos )
		{
			// ������ �ޱ�.
			pInfo = NULL ;
			pInfo = (ST_CR_ROOM_SRV*)m_pRoomList.GetNext( pos ) ;

			if( !pInfo ) continue ;

			// �ε��� ��ȿ üũ.
			if( byIndex < byStartIdx ) continue ;

			// ���� �� ���� üũ.
			if( msg.byCount >= ROOM_COUNT_PER_PAGE ) break ;

			// �� ���� ���.
			msg.room[ msg.byCount ].byIdx				= pInfo->byIdx ;
			msg.room[ msg.byCount ].bySecretMode		= pInfo->bySecretMode ;
			msg.room[ msg.byCount ].byRoomType			= pInfo->byRoomType ;
			msg.room[ msg.byCount ].byCurGuestCount		= pInfo->byCurGuestCount ;
			msg.room[ msg.byCount ].byTotalGuestCount	= pInfo->byTotalGuestCount ;

			SafeStrCpy( msg.room[ msg.byCount ].title, pInfo->title, TITLE_SIZE ) ;

			++msg.byCount ;
		}

		// ������ ����.
		msg.byCurPage	= 0 ;
		msg.byTotalPage	= (BYTE)(m_pRoomList.GetCount() / ROOM_COUNT_PER_PAGE) ;

		// ���� ä�ù� �з� ����.
		msg.byRoomType	= e_RTM_AllLooK ;

		// ��� ����.
		g_Network.Send2Server( dwIndex, (char*)&msg, sizeof(MSG_CR_ROOMLIST) ) ;



		// ������ ���� ���� ó��.
		MSG_CR_IDNAME msg2 ;
		memset( &msg2, 0, sizeof(MSG_CR_IDNAME) ) ;

		msg2.Category	= MP_CHATROOM ;
		msg2.Protocol	= MP_CHATROOM_OUT_ROOM_NOTICE ;
		msg2.dwObjectID	= pmsg->dwObjectID ;

		msg2.dwID		= pUserInfo->dwPlayerID ;

		SafeStrCpy( msg2.name, pUserInfo->name, MAX_NAME_LENGTH + 1 ) ;

		ST_CR_USER* pMemberInfo = NULL ;
		for( BYTE count = 0 ; count < pRoom->byCurGuestCount ; ++count )
		{
			pMemberInfo = m_htUser.GetData( pRoom->dwUser[ count ] ) ;
			if( !pMemberInfo ) continue ;

			msg2.dwUser[ msg2.byCount ] = pMemberInfo->dwPlayerID ;

			++msg2.byCount ;
		}

		g_Network.Send2Server( dwIndex, (char*)&msg2, sizeof(MSG_CR_IDNAME) ) ;



		// ���� �ٲ� ���� ó��.
		if( !byChangedOwner ) return ;

		MSG_CR_IDNAME msg3 ;
		memset( &msg3, 0, sizeof(MSG_CR_IDNAME) ) ;

		msg3.Category	= MP_CHATROOM ;
		msg3.Protocol	= MP_CHATROOM_OUT_ROOM_CHANGE_OWNER_NOTICE ;
		msg3.dwObjectID	= pmsg->dwObjectID ;

		pMemberInfo = NULL ;
		for( BYTE count = 0 ; count < pRoom->byCurGuestCount ; ++count )
		{
			pMemberInfo = m_htUser.GetData( pRoom->dwUser[ count ] ) ;
			if( !pMemberInfo ) continue ;

			if( pMemberInfo->dwPlayerID == pRoom->dwOwnerIdx )
			{
				msg3.dwID = pMemberInfo->dwPlayerID ;
				SafeStrCpy( msg3.name, pMemberInfo->name, MAX_NAME_LENGTH + 1 ) ;
			}

			msg3.dwUser[ msg3.byCount ] = pMemberInfo->dwPlayerID ;

			++msg3.byCount ;
		}

		g_Network.Send2Server( dwIndex, (char*)&msg3, sizeof(MSG_CR_IDNAME) ) ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : Option_Syn
//		 : 
//	DESC : The function to change option of selected room.
//		 : 
//		 : 1. ��� �� �������� üũ.
//		 : 2. �� ���� �ޱ�.
//		 : 3. �������� �˻�.
//		 : 4. �� �ɼ� ����.
//		 : 5. ��� ����.
//		 : 
//  DATE : APRIL 4, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Option_Syn( DWORD dwIndex, char* pMsg, DWORD dwLength ) 
{
	// ���� �޽��� ����.
	// 0 = err_You_Are_Not_In_ChatRoom,
	// 1 = err_CanNot_Found_Room,
	// 2 = err_You_Are_Not_Owner,



	// �Լ� �Ķ���� üũ.
	ASSERT( pMsg ) ;

	if( !pMsg )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return ;
	}



	// ���� �޽��� ��ȯ.
	MSG_CR_ELEMENT* pmsg = NULL ;
	pmsg = (MSG_CR_ELEMENT*)pMsg ;

	ASSERT( pmsg ) ;

	if( !pmsg )
	{
		Throw_Error( err_FCMTO, __FUNCTION__ ) ;
		return ;
	}



	// ��� �� �������� üũ.
	ST_CR_USER* pUserInfo = m_htUser.GetData( pmsg->dwObjectID ) ;
	if( !pUserInfo )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_CHANGE_OPTION_NACK, 0 ) ;
		return ;
	}



	// ������ �ޱ�.
	ST_CR_ROOM_SRV* pRoom = NULL ;
	pRoom = GetRoomInfo( pUserInfo->byIdx ) ;
	if( !pRoom )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_CHANGE_OPTION_NACK, 1 ) ;
		return ;
	}



    // �������� �˻�.
	if( pUserInfo->dwPlayerID != pRoom->dwOwnerIdx )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_CHANGE_OPTION_NACK, 2 ) ;
		return ;
	}


	
	// ä�ù� ���� ���� ���� ó��.
	// �ٸ� �����鿡�� ���� ����Ǿ����� �����Ѵ�.
	BYTE byChangedSecretMode = FALSE ;
	if( pRoom->bySecretMode != pmsg->bySecretMode )
	{
		byChangedSecretMode = TRUE ;
	}

	BYTE byChangedRoomType = FALSE ;
	if( pRoom->byRoomType != pmsg->byRoomType )
	{
		byChangedRoomType = TRUE ;
	}

	BYTE byChangedGuestCount = FALSE ;
	if( pRoom->byTotalGuestCount != pmsg->byTotalGuestCount )
	{
		byChangedGuestCount = TRUE ;
	}

	BYTE byChangedTitle = FALSE ;
	if( strcmp( pRoom->title, pmsg->title ) != 0 )
	{
		byChangedTitle = TRUE ;
	}



	// �� �ɼ� ����.
	pRoom->bySecretMode			= pmsg->bySecretMode ;
	pRoom->byRoomType			= pmsg->byRoomType ;
	pRoom->byTotalGuestCount	= pmsg->byTotalGuestCount ;

	SafeStrCpy( pRoom->code, pmsg->code, SECRET_CODE_SIZE ) ;

	SafeStrCpy( pRoom->title, pmsg->title, TITLE_SIZE ) ;



	// �˻��� �� ���� ������Ʈ.
	UpdateSearchRoomInfo( pRoom ) ;



	// ��� ����.
	MSG_CR_ROOM_NOTICE msg ;
	memset( &msg, 0, sizeof(MSG_CR_ROOM_NOTICE) ) ;

	msg.Category	= MP_CHATROOM ;
	msg.Protocol	= MP_CHATROOM_CHANGE_OPTION_NOTICE ;
	msg.dwObjectID	= pmsg->dwObjectID ;

	msg.room		= *pRoom ;

	ST_CR_USER* pMemberInfo = NULL ;
	for( BYTE count = 0 ; count < pRoom->byCurGuestCount ; ++count )
	{
		pMemberInfo = m_htUser.GetData( pRoom->dwUser[ count ] ) ;
		if( !pMemberInfo ) continue ;

		msg.dwUser[ msg.byCount ] = pMemberInfo->dwPlayerID ;

		++msg.byCount ;
	}

	g_Network.Send2Server( dwIndex, (char*)&msg, sizeof(MSG_CR_ROOM_NOTICE) ) ;



	// ���� �� ������ ���� ó��.
	if( byChangedSecretMode )
	{
		UpdateInfo_Changed_SecretMode( pRoom ) ;
	}
	
	if( byChangedRoomType )
	{
		UpdateInfo_Changed_RoomType( pRoom ) ;
	}
	
	if( byChangedGuestCount )
	{
		UpdateInfo_Changed_Total_GuestCount( pRoom ) ;
	}
	
	if( byChangedTitle )
	{
		UpdateInfo_Changed_Title( pRoom ) ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : Owner_Syn
//		 : 
//	DESC : The function to change owner of chatroom.
//		 : 
//		 : 1. ���� ���� �ޱ�.
//		 : 2. ���� ���� �ޱ�.
//		 : 3. ������ �ޱ�.
//		 : 4. ���� ������ �������� üũ.
//		 : 5. ���� �������� ���� �� üũ.
//		 : 6. ä�ù��� �����ε��� ����.
//		 : 7. ��� ����.
//		 : 
//  DATE : APRIL 4, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Owner_Syn( DWORD dwIndex, char* pMsg, DWORD dwLength )
{
	// ���� �޽��� ����.
	// 0 = err_Invalid_Current_Owner,
	// 1 = err_Invalid_Next_Owner,
	// 2 = err_CanNot_Found_RoomInfo,
	// 3 = err_You_Are_Not_Owner,
	// 4 = err_You_Alone,



	// �Լ� �Ķ���� üũ.
	ASSERT( pMsg ) ;

	if( !pMsg )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return ;
	}



	// ���� �޽��� ��ȯ.
	MSG_DWORD* pmsg = NULL ;
	pmsg = (MSG_DWORD*)pMsg ;

	ASSERT( pmsg ) ;

	if( !pmsg )
	{
		Throw_Error( err_FCMTO, __FUNCTION__ ) ;
		return ;
	}



	// ���� ���� �ޱ�.
	ST_CR_USER* pOwnerInfo = m_htUser.GetData( pmsg->dwObjectID ) ;
	if( !pOwnerInfo )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_CHANGE_OWNER_NACK, 0 ) ;
		return ;
	}



	// ���� ���� �ޱ�.
	ST_CR_USER* pNewOwnerInfo = m_htUser.GetData( pmsg->dwData ) ;
	if( !pNewOwnerInfo )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_CHANGE_OWNER_NACK, 1 ) ;
		return ;
	}



	// ������ �ޱ�.
	ST_CR_ROOM_SRV* pRoom = NULL ;
	pRoom = GetRoomInfo( pOwnerInfo->byIdx ) ;
	if( !pRoom )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_CHANGE_OWNER_NACK, 2 ) ;
		return ;
	}



	// ���� ������ �������� üũ.
	if( pRoom->dwOwnerIdx != pOwnerInfo->dwPlayerID )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_CHANGE_OWNER_NACK, 3 ) ;
		return ;
	}



	// ���� �������� ���� �� üũ.
	if( pRoom->byCurGuestCount <= 1 )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_CHANGE_OWNER_NACK, 4 ) ;
		return ;
	}



	// ä�ù��� ���� ���̵� ����.
	pRoom->dwOwnerIdx = pNewOwnerInfo->dwPlayerID ;



	// ��� ����.
	MSG_CR_IDNAME msg ;
	memset( &msg, 0, sizeof(MSG_CR_IDNAME) ) ;

	msg.Category	= MP_CHATROOM ;
	msg.Protocol	= MP_CHATROOM_CHANGE_OWNER_NOTICE ;
	msg.dwObjectID	= pmsg->dwObjectID ;

	msg.dwID		= pNewOwnerInfo->dwPlayerID ;

	SafeStrCpy( msg.name, pNewOwnerInfo->name, MAX_NAME_LENGTH + 1 ) ;

	ST_CR_USER* pMemberInfo = NULL ;
	for( BYTE count = 0 ; count < pRoom->byCurGuestCount ; ++count )
	{
		pMemberInfo = m_htUser.GetData( pRoom->dwUser[count] ) ;
		if( !pMemberInfo ) continue ;

		msg.dwUser[ msg.byCount ] = pMemberInfo->dwPlayerID ;

		++msg.byCount ;
	}



	// ��� ����.
	g_Network.Send2Server( dwIndex, (char*)&msg, sizeof(MSG_CR_IDNAME) ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : Kick_Syn
//		 : 
//	DESC : The function to kick selected user from chatroom.
//		 : 
//		 : 1. ���� �ޱ�.
//		 : 2. ������ �ޱ�.
//		 : 3. �������� üũ.
//		 : 4. ���� ���� ��� �ޱ�.
//		 : 5. ����� �� �ε��� �ʱ�ȭ.
//		 : 6. ä�ù��� ���� ������ ����.
//		 : 7. ���� ó��.
//		 : 8. ���� ���� ó��.
//		 : 
//  DATE : APRIL 4, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Kick_Syn( DWORD dwIndex, char* pMsg, DWORD dwLength )
{
	// ���� �޽��� ����.
	// 0 = err_CanNot_Found_RoomInfo,
	// 1 = err_Invalid_OwnerInfo,
	// 2 = err_You_Are_Not_Owner,
	// 3 = err_Invalid_Target,



	// �Լ� �Ķ���� üũ.
	ASSERT( pMsg ) ;

	if( !pMsg )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return ;
	}



	// ���� �޽��� ��ȯ.
	MSG_DWORD* pmsg = NULL ;
	pmsg = (MSG_DWORD*)pMsg ;

	ASSERT( pmsg ) ;

	if( !pmsg )
	{
		Throw_Error( err_FCMTO, __FUNCTION__ ) ;
		return ;
	}



	// ���� �ޱ�.
	ST_CR_USER* pOwnerInfo = m_htUser.GetData( pmsg->dwObjectID ) ;
	if( !pOwnerInfo )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_KICK_GUEST_NACK, 1 ) ;
		return ;
	}



	// ������ �ޱ�.
	ST_CR_ROOM_SRV* pRoom = NULL ;
	pRoom = GetRoomInfo( pOwnerInfo->byIdx ) ;
	if( !pRoom )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_KICK_GUEST_NACK, 0 ) ;
		return ;
	}



	// �������� üũ.
	if( pOwnerInfo->dwPlayerID != pRoom->dwOwnerIdx )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_KICK_GUEST_NACK, 2 ) ;
		return ;
	}



	// ���� ���� ��� �ޱ�.
	ST_CR_USER* pPurgeeInfo = m_htUser.GetData( pmsg->dwData ) ;
	if( !pPurgeeInfo )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_KICK_GUEST_NACK, 3 ) ;
		return ;
	}



	// ����� ���ε��� �ʱ�ȭ.
	pPurgeeInfo->byIdx = 0 ;



	// �˻��� ���� �̸� ����.
	EraseUserName( pPurgeeInfo->name ) ;



	// ���� �ε����� �ӽ� �����Ѵ�.
	BYTE byCheckCount = 0 ;
	DWORD dwPlayerID[ MAX_USER_PER_ROOM ] = {0, } ;
	for( BYTE count = 0 ; count < pRoom->byCurGuestCount ; ++count )
	{
		if( pRoom->dwUser[ count ] == pPurgeeInfo->dwPlayerID ) continue ;

		dwPlayerID[ byCheckCount ] = pRoom->dwUser[ count ] ;
		++byCheckCount ;
	}



	// ä�ù��� ������ �� ����.
	--pRoom->byCurGuestCount ;



	// ���� �ε��� ����
	for( BYTE count = 0 ; count  < pRoom->byCurGuestCount ; ++count )
	{
		pRoom->dwUser[ count ] = dwPlayerID[ count ] ;
	}



	// ���� �޽��� ����.
	MSG_CR_KICK_ACK msg ;
	memset( &msg, 0, sizeof(MSG_CR_KICK_ACK) ) ;

	msg.Category	= MP_CHATROOM ;
	msg.Protocol	= MP_CHATROOM_KICK_GUEST_ACK ;
	msg.dwObjectID	= pmsg->dwObjectID ;

	msg.dwKickPlayer = pmsg->dwData ;



	// �������� ���� ���� �ε��� ����.
	BYTE byIndex = 0 ;
	BYTE byStartIdx = 0 ;
	byStartIdx = byStartIdx * ROOM_COUNT_PER_PAGE ;



	// ����Ʈ ��� ����.
	ST_CR_ROOM_SRV* pInfo ;
	PTRLISTPOS pos = NULL ;
	pos = m_pRoomList.GetHeadPosition() ;

	while( pos )
	{
		// ������ �ޱ�.
		pInfo = NULL ;
		pInfo = (ST_CR_ROOM_SRV*)m_pRoomList.GetNext( pos ) ;

		if( !pInfo ) continue ;

		// �ε��� ��ȿ üũ.
		if( byIndex < byStartIdx ) continue ;

		// ���� �� ���� üũ.
		if( msg.byCount >= ROOM_COUNT_PER_PAGE ) break ;

		// �� ���� ���.
		msg.room[ msg.byCount ].byIdx				= pInfo->byIdx ;
		msg.room[ msg.byCount ].bySecretMode		= pInfo->bySecretMode ;
		msg.room[ msg.byCount ].byRoomType			= pInfo->byRoomType ;
		msg.room[ msg.byCount ].byCurGuestCount		= pInfo->byCurGuestCount ;
		msg.room[ msg.byCount ].byTotalGuestCount	= pInfo->byTotalGuestCount ;

		SafeStrCpy( msg.room[ msg.byCount ].title, pInfo->title, TITLE_SIZE ) ;

		++msg.byCount ;
	}



	// ������ ����.
	msg.byCurPage	= 0 ;
	msg.byTotalPage	= (BYTE)(m_pRoomList.GetCount() / ROOM_COUNT_PER_PAGE) ;



	// ���� ä�ù� �з� ����.
	msg.byRoomType	= e_RTM_AllLooK ;



	// ��� ����.
	g_Network.Send2Server( dwIndex, (char*)&msg, sizeof(MSG_CR_KICK_ACK) ) ;



	// ���� ���� ó��.
	MSG_CR_IDNAME msg2 ;
	memset( &msg2, 0, sizeof(MSG_CR_IDNAME) ) ;

	msg2.Category	= MP_CHATROOM ;
	msg2.Protocol	= MP_CHATROOM_KICK_GUEST_NOTICE ;
	msg2.dwObjectID	= pmsg->dwObjectID ;



	// ���� ����� ���� ����.
	msg2.dwID		= pPurgeeInfo->dwPlayerID ;
	SafeStrCpy( msg2.name, pPurgeeInfo->name, MAX_NAME_LENGTH + 1 ) ;



	// ���� ����� ���.
	ST_CR_USER* pMemberInfo = NULL ;
	for( BYTE count = 0 ; count < MAX_USER_PER_ROOM ; ++count )				
	{
		pMemberInfo = m_htUser.GetData( pRoom->dwUser[ count ] ) ;
		if( !pMemberInfo ) continue ;

		// ���� ��󿡼� �����ڴ� ����.
		if( pMemberInfo->dwPlayerID == pmsg->dwData ) continue ;	

		msg2.dwUser[ msg2.byCount ] = pMemberInfo->dwPlayerID ;

		++msg2.byCount ;
	}



	// ��� ����.
	g_Network.Send2Server( dwIndex, (char*)&msg2, sizeof(MSG_CR_IDNAME) ) ;



	// �ٸ� �����鿡�� ���� ����Ǿ����� �����Ѵ�.
	UpdateInfo_Changed_Current_GuestCount( pRoom ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : Friend_Syn
//		 : 
//	DESC : The function to check user info for add friend.
//		 : 
//		 : 1. ��û�� �ޱ�.
//		 : 2. ��� �ޱ�.
//		 : 3. ���� ä�ù����� üũ.
//		 : 4. ��� ����.
//		 : 
//  DATE : APRIL 4, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Friend_Syn( DWORD dwIndex, char* pMsg, DWORD dwLength )
{
	// ���� �޽��� ����.
	// 0 = err_Invalid_Requester_Info,
	// 1 = err_Invalid_Target_Info.
	// 2 = err_You_Are_Not_In_SameRoom,



	// �Լ� �Ķ���� üũ.
	ASSERT( pMsg ) ;

	if( !pMsg )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return ;
	}



	// ���� �޽��� ��ȯ.
	MSG_DWORD* pmsg = NULL ;
	pmsg = (MSG_DWORD*)pMsg ;

	ASSERT( pmsg ) ;

	if( !pmsg )
	{
		Throw_Error( err_FCMTO, __FUNCTION__ ) ;
		return ;
	}



	// ��û�� �ޱ�.
	ST_CR_USER* pApplicantInfo = m_htUser.GetData( pmsg->dwObjectID ) ;
	if( !pApplicantInfo )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_REQUEST_FRIEND_NACK, 0 ) ;
		return ;
	}



	// ��� �ޱ�.
	ST_CR_USER* pAcceptantInfo = m_htUser.GetData( pmsg->dwData ) ;
	if( !pAcceptantInfo )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_REQUEST_FRIEND_NACK, 1 ) ;
		return ;
	}



	// ���� ä�ù����� üũ.
	if( pApplicantInfo->byIdx != pAcceptantInfo->byIdx )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_REQUEST_FRIEND_NACK, 2 ) ;
		return ;
	}



	// ��� �޽��� ����.
	MSG_NAME msg ;
	memset( &msg, 0, sizeof(MSG_NAME) ) ;

	msg.Category	= MP_CHATROOM ;
	msg.Protocol	= MP_CHATROOM_REQUEST_FRIEND_ACK ;
	msg.dwObjectID	= pmsg->dwObjectID ;

	SafeStrCpy( msg.Name, pAcceptantInfo->name, MAX_NAME_LENGTH+1 ) ;

	g_Network.Send2Server( dwIndex, (char*)&msg, sizeof(MSG_NAME) ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : UpdateUserInfo
//	DESC : ������ ������ ������Ʈ �Ǿ��� ��, ������ ������Ʈ �ϴ� �Լ�.
//  DATE : APRIL 2, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::UpdateUserInfo( DWORD dwIndex, char*pMsg, DWORD dwLength )
{
	// �Լ� �Ķ���� üũ.
	ASSERT( pMsg ) ;

	if( !pMsg )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return ;
	}



	// ���� �޽��� ��ȯ.
	MSG_CR_USER* pmsg = NULL ;
	pmsg = (MSG_CR_USER*)pMsg ;

	ASSERT( pmsg ) ;

	if( !pmsg )
	{
		Throw_Error( err_FCMTO, __FUNCTION__ ) ;
		return ;
	}



	// ���� ���� ������Ʈ.
	ST_CR_USER* pUserInfo = m_htUser.GetData( pmsg->dwObjectID ) ;
	if( pUserInfo )
	{
		// �˻��� �̸� üũ.
		if( strcmp( pUserInfo->name, pmsg->user.name ) != 0 )
		{
			EraseUserName( pUserInfo->name ) ;

			pUserInfo->byLevel	= pmsg->user.byLevel ;
			pUserInfo->byMapNum	= pmsg->user.byMapNum ;

			SafeStrCpy( pUserInfo->name, pmsg->user.name, MAX_NAME_LENGTH + 1 ) ;

			InsertUserName( pUserInfo->name ) ;
			UpdateNameRoomIdx( pUserInfo->name, pUserInfo->byIdx ) ;
		}
		else
		{
			pUserInfo->byLevel  = pmsg->user.byLevel ;
			pUserInfo->byMapNum	= pmsg->user.byMapNum ;

			SafeStrCpy( pUserInfo->name, pmsg->user.name, MAX_NAME_LENGTH + 1 ) ;

			UpdateNameRoomIdx( pUserInfo->name, pUserInfo->byIdx ) ;
		}
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : SearchName_Syn
//	DESC : �̸����� ä�ù� ������ �˻��ϴ� �Լ�.
//  DATE : APRIL 23, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::SearchName_Syn( DWORD dwIndex, char*pMsg, DWORD dwLength ) 
{
	// ���� �޽��� ����.
	// 0 = err_CanNot_Found_User,
	// 1 = err_CanNot_Found_Name,
	// 2 = err_ChatRoom_Destroyed,



	// �Լ� �Ķ���� üũ.
	ASSERT( pMsg ) ;

	if( !pMsg )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return ;
	}



	// ���� �޽��� ��ȯ.
	MSG_CR_SEARCH_NAME* pmsg = NULL ;
	pmsg = (MSG_CR_SEARCH_NAME*)pMsg ;

	ASSERT( pmsg ) ;

	if( !pmsg )
	{
		Throw_Error( err_FCMTO, __FUNCTION__ ) ;
		return ;
	}



	// ���� �˻�.
	ST_CR_USER* pUserInfo = m_htUser.GetData( pmsg->dwObjectID ) ;
	if( !pUserInfo )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_SEARCH_FOR_NAME_NACK, 0 ) ;
		return ;
	}



	// �̸� �˻�.
	const char* name = pmsg->name ;
	ST_SEARCH_NAME* pNameInfo = m_htSearchName.GetData( GetHashCode( name ) ) ;
	if( !pNameInfo )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_SEARCH_FOR_NAME_NACK, 1 ) ;
		return ;
	}



	// �� �˻�.
	ST_CR_ROOM_SRV* pRoom = NULL ;
	pRoom = GetRoomInfo( pNameInfo->byRoomIdx ) ;

	if( !pRoom )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_SEARCH_FOR_NAME_NACK, 2 ) ;
		return ;
	}



	// ��� ����.
	MSG_CR_SEARCH_NAME msg ;
	memset( &msg, 0, sizeof(MSG_CR_SEARCH_NAME) ) ;

	msg.Category	= MP_CHATROOM ;
	msg.Protocol	= MP_CHATROOM_SEARCH_FOR_NAME_ACK ;
	msg.dwObjectID	= pmsg->dwObjectID ;

	msg.byRoomIdx	= pRoom->byIdx ;

	SafeStrCpy( msg.name, pmsg->name, MAX_NAME_LENGTH + 1 ) ;

	g_Network.Send2Server( dwIndex, (char*)&msg, sizeof(MSG_CR_SEARCH_NAME) ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : SearchTitle_Syn
//	DESC : �������� ä�ù� ������ �˻��ϴ� �Լ�.
//  DATE : APRIL 23, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::SearchTitle_Syn( DWORD dwIndex, char*pMsg, DWORD dwLength )
{
	// ���� �޽��� ����.
	// 0 = err_CanNot_Found_Room,



	// �Լ� �Ķ���� üũ.
	ASSERT( pMsg ) ;

	if( !pMsg )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return ;
	}



	// ���� �޽��� ��ȯ.
	MSG_CR_SEARCH_TITLE* pmsg = NULL ;
	pmsg = (MSG_CR_SEARCH_TITLE*)pMsg ;

	ASSERT( pmsg ) ;

	if( !pmsg )
	{
		Throw_Error( err_FCMTO, __FUNCTION__ ) ;
		return ;
	}



	// ��� �޽��� ����.
	MSG_CR_ROOMLIST msg ;
	memset( &msg, 0, sizeof(MSG_CR_ROOMLIST) ) ;

	msg.Category	= MP_CHATROOM ;
	msg.Protocol	= MP_CHATROOM_SEARCH_FOR_TITLE_ACK ;
	msg.dwObjectID	= pmsg->dwObjectID ;



	// �������� ä�ù� �˻�.
	ST_CR_ROOM_CLT* pInfo ;

	PTRLISTPOS pos = NULL ;
	pos = m_pTitleList.GetHeadPosition() ;

	BYTE byCount = 0 ;

	while( pos )
	{
		pInfo = NULL ;
		pInfo = (ST_CR_ROOM_CLT*)m_pTitleList.GetNext( pos ) ;

		if( !pInfo ) continue ;

		if( strstr( pInfo->title, pmsg->title ) == NULL ) continue ;

		++byCount ;

		if( msg.byCount >= MAX_USER_PER_ROOM ) continue ;

		msg.room[ msg.byCount ] = *pInfo ;

		++msg.byCount ;
	}



	// �˻� �� ���� üũ.
	if( msg.byCount == 0 )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_SEARCH_FOR_TITLE_NACK, 0 ) ;
		return ;
	}



	// �˻����� ����.
	msg.byRoomType = e_RTM_Searched ;



	// ������ ����.
	msg.byCurPage	= 0 ;
	msg.byTotalPage = byCount / ROOM_COUNT_PER_PAGE ;



	// ��� ����.
	g_Network.Send2Server( dwIndex, (char*)&msg, sizeof(MSG_CR_ROOMLIST) ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : GetEmptyRoomIdx
//	DESC : ä�ù��� �����ϴ� �ʿ���, �տ��� ���� ����ִ� �� �ε����� ��ȯ�ϴ� �Լ�. 0���� ����.
//  DATE : APRIL 14, 2008 LYW
//-------------------------------------------------------------------------------------------------
BYTE CChatRoomMgr::GetEmptyRoomIdx()
{
	for( BYTE count = 1 ; count < MAX_ROOM_COUNT ; ++count )
	{
		if( m_byRoomIdx[ count ] == 0 ) return count ;
	}

	ASSERT( TRUE ) ;

	Throw_Error( "Cant found empty room index!!", __FUNCTION__ ) ;

	return 0 ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : GetHashCode
//	DESC : ���ڷ� �־��� ��Ʈ���� �ؽ��ʿ� ����� �ڵ� ������ ��ȯ�ϴ� �Լ�.
//  DATE : APRIL 24, 2008 LYW
//-------------------------------------------------------------------------------------------------
DWORD CChatRoomMgr::GetHashCode( const char* pName )
{
	// �Լ� �Ķ���� üũ.
	ASSERT( pName ) ;

	if( !pName || strlen( pName ) == 0 )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return 5381 ;
	}

	DWORD dwCount	= 0 ;
	DWORD dwLength	= 0 ;
	DWORD dwCh		= 0 ;
	DWORD dwResult	= 0 ;

	dwLength = strlen( pName ) ;
	dwResult = 5381 ;

	for( dwCount = 0 ; dwCount < dwLength ; dwCount++ )
	{
		dwCh = (DWORD)pName[ dwCount ] ;
		dwResult = ( (dwResult << 5) + dwResult ) + dwCh ; // hash * 33 + ch
	}	  

	return dwResult ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : InsertUserName
//	DESC : �˻��� �ؽ��ʿ� ���� �̸��� �߰��ϴ� �Լ�.
//  DATE : APRIL 24, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::InsertUserName( char* pName ) 
{
	// �Լ� �Ķ���� üũ.
	ASSERT( pName ) ;

	if( !pName || strlen( pName ) == 0 )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return ;
	}



	// ������ �߰��Ѵ�.
	const char* name = pName ;
	DWORD dwNameCode = GetHashCode( name ) ;

	ST_SEARCH_NAME* pNameInfo = new ST_SEARCH_NAME ;
	ASSERT( pNameInfo ) ;

	if( !pNameInfo ) return ;

	ZeroMemory( pNameInfo, sizeof(pNameInfo) );
	SafeStrCpy( pNameInfo->name, name, MAX_NAME_LENGTH + 1 ) ;

	m_htSearchName.Add( pNameInfo, dwNameCode ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : EraseUserName
//	DESC : �˻��� �ؽ����� ���� �̸��� �����ϴ� �Լ�.
//  DATE : APRIL 24, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::EraseUserName( char* pName )
{
	// �Լ� �Ķ���� üũ.
	ASSERT( pName ) ;

	if( !pName || strlen( pName ) == 0 )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return ;
	}



	// �˻��� �Ѵ�.
	const char* name = pName ;
	DWORD dwNameCode = GetHashCode( name ) ;

	ST_SEARCH_NAME* pNameInfo = m_htSearchName.GetData( dwNameCode ) ;
	if( pNameInfo )
	{
		delete pNameInfo ;
		pNameInfo = NULL ;
	}

	m_htSearchName.Remove( dwNameCode ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : UpdateNameRoomIdx
//	DESC : �˻��� �ؽ����� ������ ������ �� ��ȣ�� ������Ʈ �ϴ� �Լ�.
//  DATE : APRIL 24, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::UpdateNameRoomIdx( char* pName, BYTE byRoomIdx )
{
	// �Լ� �Ķ���� üũ.
	ASSERT( pName ) ;

	if( !pName || strlen( pName ) == 0 )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return ;
	}



	// �� �ε����� ������Ʈ �Ѵ�.
	const char* name = pName ;
	DWORD dwNameCode = GetHashCode( name ) ;

	ST_SEARCH_NAME* pNameInfo = m_htSearchName.GetData( dwNameCode ) ;
	if( pNameInfo )
	{
		pNameInfo->byRoomIdx = byRoomIdx ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : UpdateSearchRoomInfo
//	DESC : �˻��� ���� ������ ������Ʈ �ϴ� �Լ�.
//  DATE : APRIL 24, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::UpdateSearchRoomInfo( ST_CR_ROOM_SRV* pInfo )
{
	// �Լ� �Ķ���� üũ.
	ASSERT( pInfo ) ;

	if( !pInfo )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return ;
	}



	// �˻� �� �� ������Ʈ.
	ST_CR_ROOM_CLT* pTitle = NULL ;
	PTRLISTPOS pos = NULL ;

	pos = m_pTitleList.GetHeadPosition() ;

	while( pos )
	{
		pTitle = NULL ;
		pTitle = (ST_CR_ROOM_CLT*)m_pTitleList.GetNext( pos ) ;

		if( !pInfo ) continue ;

		if( pInfo->byIdx != pTitle->byIdx ) continue ;

		pTitle->byIdx				= pInfo->byIdx ;
		pTitle->bySecretMode		= pInfo->bySecretMode ;
		pTitle->byRoomType			= pInfo->byRoomType ;
		pTitle->byCurGuestCount		= pInfo->byCurGuestCount ;
		pTitle->byTotalGuestCount	= pInfo->byTotalGuestCount ;

		SafeStrCpy( pTitle->title, pInfo->title, TITLE_SIZE ) ;

		return ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : UpdateInfo_Created_Room
//	DESC : ���� ��, ä�ù��� �߰��Ǿ����� Ŭ���̾�Ʈ�� �˸��� �Լ�.
//  DATE : APRIL 2, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::UpdateInfo_Created_Room( ST_CR_ROOM_SRV* pInfo )
{
	// �Լ� �Ķ���� üũ.
	ASSERT( pInfo ) ;

	if( !pInfo )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return ;
	}



	// ä�� �������� ������ �ִ��� Ȯ���Ѵ�.
	if( m_htUser.GetDataNum() == 0 )
	{
		Throw_Error( "Failed to notice add room!!", __FUNCTION__ ) ;
		return ;
	}



	// ���� �޽��� ����.
	MSG_CR_ROOMINFO_TO_NOTICE msg ;
	memset( &msg, 0, sizeof(MSG_CR_ROOMINFO_TO_NOTICE) ) ;

	msg.Category	= MP_CHATROOM ;
	msg.Protocol	= MP_CHATROOM_UPDATEINFO_CREATED_ROOM ;

	msg.room		= *pInfo ;	



	// �������� �޽��� ����.
	ST_CR_USER* pUserInfo = NULL ;
	while( (pUserInfo = m_htUser.GetData()) != NULL )
	{
		msg.dwObjectID = pUserInfo->dwPlayerID ;
		g_Network.Send2Server( pUserInfo->dwConnectionIdx, (char*)&msg, sizeof(MSG_CR_ROOMINFO_TO_NOTICE) ) ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : UpdateInfo_Deleted_Room
//	DESC : ���� ��, ä�ù��� �����Ǿ����� Ŭ���̾�Ʈ�� �˸��� �Լ�.
//  DATE : APRIL 2, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::UpdateInfo_Deleted_Room( ST_CR_ROOM_SRV* pInfo )
{
	// �Լ� �Ķ���� üũ.
	ASSERT( pInfo ) ;

	if( !pInfo )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return ;
	}



	// ä�� �������� ������ �ִ��� Ȯ���Ѵ�.
	if( m_htUser.GetDataNum() == 0 )
	{
		Throw_Error( "Failed to notice add room!!", __FUNCTION__ ) ;
		return ;
	}



	// ���� �޽��� ����.
	MSG_BYTE msg ;
	memset( &msg, 0, sizeof(MSG_BYTE) ) ;

	msg.Category	= MP_CHATROOM ;
	msg.Protocol	= MP_CHATROOM_UPDATEINFO_DELETED_ROOM ;

	msg.bData		= pInfo->byIdx ;



	// �������� �޽��� ����.
	ST_CR_USER* pUserInfo = NULL ;
	while( (pUserInfo = m_htUser.GetData()) != NULL )
	{
		msg.dwObjectID = pUserInfo->dwPlayerID ;
		g_Network.Send2Server( pUserInfo->dwConnectionIdx, (char*)&msg, sizeof(MSG_BYTE) ) ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : UpdateInfo_Changed_SecretMode
//	DESC : ���� ��, ä�ù��� ����/������� ����Ǿ����� �����ϴ� �Լ�.
//  DATE : APRIL 2, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::UpdateInfo_Changed_SecretMode( ST_CR_ROOM_SRV* pInfo )
{
	// �Լ� �Ķ���� üũ.
	ASSERT( pInfo ) ;

	if( !pInfo )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return ;
	}



	// ä�� �������� ������ �ִ��� Ȯ���Ѵ�.
	if( m_htUser.GetDataNum() == 0 )
	{
		Throw_Error( "Failed to notice add room!!", __FUNCTION__ ) ;
		return ;
	}



	// ���� �޽��� ����.
	MSG_BYTE2 msg ;
	memset( &msg, 0, sizeof(MSG_BYTE2) ) ;

	msg.Category	= MP_CHATROOM ;
	msg.Protocol	= MP_CHATROOM_UPDATEINFO_SECRETMODE ;

	msg.bData1		= pInfo->byIdx ;
	msg.bData2		= pInfo->bySecretMode ;	

	

	// �������� �޽��� ����.
	ST_CR_USER* pUserInfo = NULL ;
	while( (pUserInfo = m_htUser.GetData()) != NULL )
	{
		msg.dwObjectID = pUserInfo->dwPlayerID ;
		g_Network.Send2Server( pUserInfo->dwConnectionIdx, (char*)&msg, sizeof(MSG_BYTE2) ) ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : UpdateInfo_Changed_RoomType
//	DESC : ���� ��, ä�ù�з��� ����Ǿ����� �����ϴ� �Լ�.
//  DATE : APRIL 2, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::UpdateInfo_Changed_RoomType( ST_CR_ROOM_SRV* pInfo )
{
	// �Լ� �Ķ���� üũ.
	ASSERT( pInfo ) ;

	if( !pInfo )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return ;
	}



	// ä�� �������� ������ �ִ��� Ȯ���Ѵ�.
	if( m_htUser.GetDataNum() == 0 )
	{
		Throw_Error( "Failed to notice add room!!", __FUNCTION__ ) ;
		return ;
	}



	// ���� �޽��� ����.
	MSG_BYTE2 msg ;
	memset( &msg, 0, sizeof(MSG_BYTE2) ) ;

	msg.Category	= MP_CHATROOM ;
	msg.Protocol	= MP_CHATROOM_UPDATEINFO_ROOMTYPE ;

	msg.bData1		= pInfo->byIdx ;
	msg.bData2		= pInfo->byRoomType ;	

	

	// �������� �޽��� ����.
	ST_CR_USER* pUserInfo = NULL ;
	while( (pUserInfo = m_htUser.GetData()) != NULL )
	{
		msg.dwObjectID = pUserInfo->dwPlayerID ;
		g_Network.Send2Server( pUserInfo->dwConnectionIdx, (char*)&msg, sizeof(MSG_BYTE2) ) ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : UpdateInfo_Changed_Title
//	DESC : ���� ��, ä�ù��� ������ ����Ǿ����� �����ϴ� �Լ�.
//  DATE : APRIL 2, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::UpdateInfo_Changed_Title( ST_CR_ROOM_SRV* pInfo )
{
	// �Լ� �Ķ���� üũ.
	ASSERT( pInfo ) ;

	if( !pInfo )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return ;
	}



	// ä�� �������� ������ �ִ��� Ȯ���Ѵ�.
	if( m_htUser.GetDataNum() == 0 )
	{
		Throw_Error( "Failed to notice add room!!", __FUNCTION__ ) ;
		return ;
	}



	// ���� �޽��� ����.
	MSG_CR_SEARCH_TITLE msg ;
	memset( &msg, 0, sizeof(MSG_CR_SEARCH_TITLE) ) ;

	msg.Category	= MP_CHATROOM ;
	msg.Protocol	= MP_CHATROOM_UPDATEINFO_TITLE ;

	msg.byRoomIdx	= pInfo->byIdx ;
	SafeStrCpy( msg.title, pInfo->title, TITLE_SIZE ) ;	

	

	// �������� �޽��� ����.
	ST_CR_USER* pUserInfo = NULL ;
	while( (pUserInfo = m_htUser.GetData()) != NULL )
	{
		msg.dwObjectID = pUserInfo->dwPlayerID ;
		g_Network.Send2Server( pUserInfo->dwConnectionIdx, (char*)&msg, sizeof(MSG_BYTE2) ) ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : UpdateInfo_Changed_Current_GuestCount
//	DESC : ä�ù��� ������ �ο��� ����Ǿ����� �����ϴ� �Լ�.
//  DATE : APRIL 2, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::UpdateInfo_Changed_Current_GuestCount( ST_CR_ROOM_SRV* pInfo )
{
	// �Լ� �Ķ���� üũ.
	ASSERT( pInfo ) ;

	if( !pInfo )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return ;
	}



	// ä�� �������� ������ �ִ��� Ȯ���Ѵ�.
	if( m_htUser.GetDataNum() == 0 )
	{
		Throw_Error( "Failed to notice add room!!", __FUNCTION__ ) ;
		return ;
	}



	// ���� �޽��� ����.
	MSG_BYTE2 msg ;
	memset( &msg, 0, sizeof(MSG_BYTE2) ) ;

	msg.Category	= MP_CHATROOM ;
	msg.Protocol	= MP_CHATROOM_UPDATEINFO_CUR_GUESTCOUNT ;



	// �������� �޽��� ����.
	ST_CR_USER* pUserInfo = NULL ;
	while( (pUserInfo = m_htUser.GetData()) != NULL )
	{
		msg.bData1		= pInfo->byIdx ;
		msg.bData2		= pInfo->byCurGuestCount ;	

		msg.dwObjectID = pUserInfo->dwPlayerID ;

		g_Network.Send2Server( pUserInfo->dwConnectionIdx, (char*)&msg, sizeof(MSG_BYTE2) ) ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : UpdateInfo_Changed_Total_GuestCount
//	DESC : ä�ù��� �ִ� ������ �ο��� ����Ǿ����� �����ϴ� �Լ�.
//  DATE : APRIL 2, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::UpdateInfo_Changed_Total_GuestCount( ST_CR_ROOM_SRV* pInfo )
{
	// �Լ� �Ķ���� üũ.
	ASSERT( pInfo ) ;

	if( !pInfo )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return ;
	}



	// ä�� �������� ������ �ִ��� Ȯ���Ѵ�.
	if( m_htUser.GetDataNum() == 0 )
	{
		Throw_Error( "Failed to notice add room!!", __FUNCTION__ ) ;
		return ;
	}



	// ���� �޽��� ����.
	MSG_BYTE2 msg ;
	memset( &msg, 0, sizeof(MSG_BYTE2) ) ;

	msg.Category	= MP_CHATROOM ;
	msg.Protocol	= MP_CHATROOM_UPDATEINFO_TOTAL_GUESTCOUNT ;

	msg.bData1		= pInfo->byIdx ;
	msg.bData2		= pInfo->byTotalGuestCount ;	

	

	// �������� �޽��� ����.
	ST_CR_USER* pUserInfo = NULL ;
	while( (pUserInfo = m_htUser.GetData()) != NULL )
	{
		msg.dwObjectID = pUserInfo->dwPlayerID ;
		g_Network.Send2Server( pUserInfo->dwConnectionIdx, (char*)&msg, sizeof(MSG_BYTE2) ) ;
	}
}

void CChatRoomMgr::Throw_Error( BYTE errType, char* szCaption )
{
	// �Լ� �Ķ���� üũ.
	ASSERT( szCaption ) ;

	if( !szCaption || strlen( szCaption ) <= 1 )
	{
#ifdef _USE_ERR_MSGBOX_
		MessageBox( NULL, "Invalid Caption!!", "Throw_Error", MB_OK ) ;
#else
		char tempStr[ 257 ] = {0, } ;

		SafeStrCpy( tempStr, __FUNCTION__, 256 ) ;
		strcat( tempStr, " - " ) ;
		strcat( tempStr, "Invalid Caption!!" ) ;
		WriteLog(tempStr) ;
#endif //_USE_ERR_MSGBOX_
		return ;
	}

	char tempErr[ 257 ] = {0, } ;

	switch(errType)
	{
	case err_IMP :		SafeStrCpy( tempErr, "Invalid a message parameter!!", 256 ) ;					break ;
	case err_FCMTB :	SafeStrCpy( tempErr, "Failed to convert a message to base!!", 256 ) ;			break ;
	case err_FCMTO :	SafeStrCpy( tempErr, "Failed to convert a message to original!!", 256 ) ;		break ;
	default : break ;
	}

	// ���� ���/�α�.
#ifdef _USE_ERR_MSGBOX_
	MessageBox( NULL, tempErr, szCaption, MB_OK ) ;
#else
	char tempStr[ 257 ] = {0, } ;

	SafeStrCpy( tempStr, szCaption, 256 ) ;
	strcat( tempStr, " - " ) ;
	strcat( tempStr, tempErr ) ;
	WriteLog( tempStr ) ;
#endif //_USE_ERR_MSGBOX_
}





//-------------------------------------------------------------------------------------------------
//	NAME : Throw_Error
//	DESC : ä�ù� ��, ���� �޽��� ���� �α׸� ����� �Լ�.
//  DATE : APRIL 14, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Throw_Error( char* szErr, char* szCaption )
{
	// �Լ� �Ķ���� üũ.
	ASSERT( szCaption ) ;

	if( !szErr || strlen( szErr ) <= 1 )
	{
#ifdef _USE_ERR_MSGBOX_
		MessageBox( NULL, "Invalid err message!!", "Throw_Error", MB_OK ) ;
#else
		char tempStr[ 257 ] = {0, } ;

		SafeStrCpy( tempStr, __FUNCTION__, 256 ) ;
		strcat( tempStr, " - " ) ;
		strcat( tempStr, "Invalid err message!!" ) ;
		WriteLog( tempStr ) ;
#endif //_USE_ERR_MSGBOX_
		return ;
	}

	if( !szCaption || strlen( szCaption ) <= 1 )
	{
#ifdef _USE_ERR_MSGBOX_
		MessageBox( NULL, "Invalid Caption!!", "Throw_Error", MB_OK ) ;
#else
		char tempStr[ 257 ] = {0, } ;

		SafeStrCpy( tempStr, __FUNCTION__, 256 ) ;
		strcat (tempStr, " - " ) ;
		strcat( tempStr, "Invalid Caption!!")  ;
		WriteLog( tempStr ) ;
#endif //_USE_ERR_MSGBOX_
		return ;
	}

	// ���� ���/�α�.
#ifdef _USE_ERR_MSGBOX_
	MessageBox( NULL, szErr, szCaption, MB_OK ) ;
#else
	char tempStr[ 257 ] = {0, } ;

	SafeStrCpy( tempStr, szCaption, 256 ) ;
	strcat( tempStr, " - " ) ;
	strcat( tempStr, szErr ) ;
	WriteLog( tempStr ) ;
#endif //_USE_ERR_MSGBOX_
}





//-------------------------------------------------------------------------------------------------
//	NAME : Throw_Error
//	DESC : ä�ù� ��, ���� �޽��� ���� �α׸� ����� �Լ�.
//  DATE : APRIL 14, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Throw_Error( char* szCommonErr, char* szErr, char* szCaption )
{
	// �Լ� �Ķ���� üũ.
	ASSERT( szCaption ) ;

	if( !szErr || strlen( szErr ) <= 1 )
	{
#ifdef _USE_ERR_MSGBOX_
		MessageBox( NULL, "Invalid err message!!", "Throw_Error", MB_OK ) ;
#else
		char tempStr[ 257 ] = {0, } ;

		SafeStrCpy( tempStr, __FUNCTION__, 256 ) ;
		strcat( tempStr, " - " ) ;
		strcat( tempStr, "Invalid err message!!" ) ;
		WriteLog( tempStr ) ;
#endif //_USE_ERR_MSGBOX_
		return ;
	}

	if( !szCaption || strlen( szCaption ) <= 1 )
	{
#ifdef _USE_ERR_MSGBOX_
		MessageBox( NULL, "Invalid Caption!!", "Throw_Error", MB_OK ) ;
#else
		char tempStr[ 257 ] = {0, } ;

		SafeStrCpy( tempStr, __FUNCTION__, 256 ) ;
		strcat( tempStr, " - " ) ;
		strcat( tempStr, "Invalid Caption!!" ) ;
		WriteLog( tempStr ) ;
#endif //_USE_ERR_MSGBOX_
		return ;
	}

	// ���� ���/�α�.
#ifdef _USE_ERR_MSGBOX_
	MessageBox( NULL, szErr, szCaption, MB_OK ) ;
#else
	char tempStr[ 257 ] = {0, } ;

	SafeStrCpy( tempStr, szCaption, 256) ;
	strcat( tempStr, " - " ) ;
	strcat( tempStr, szErr ) ;
	WriteLog( tempStr ) ;
#endif //_USE_ERR_MSGBOX_
}





//-------------------------------------------------------------------------------------------------
//	NAME : WriteLog
//	DESC : ä�ù� ��, ���� �޽��� ���� �α׸� ����� �Լ�.
//  DATE : APRIL 14, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::WriteLog( char* pMsg )
{
	SYSTEMTIME time ;
	GetLocalTime( &time ) ;

	TCHAR szTime[_MAX_PATH ] = {0, } ;
	sprintf( szTime, "%04d-%02d-%02d %02d:%02d:%02d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond ) ;

	FILE *fp = fopen( "Log/Dist-ChatRoomErr.log", "a+" ) ;
	if ( fp )
	{
		fprintf( fp, "%s [%s]\n", pMsg,  szTime ) ;
		fclose( fp ) ;
	}
}












