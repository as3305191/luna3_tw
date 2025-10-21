#include "stdafx.h"
#include "ExchangeManager.h"
#include "ExchangeRoom.h"
#include "UserTable.h"
#include "Player.h"
#include "objectstatemanager.h"
#include "itemmanager.h"
#include "CheckRoutine.h"
#include "..\[CC]Header\GameResourceStruct.h"
#include "Battle.h"
#include "HousingMgr.h"

#define MAX_EXCHANGEROOM	100


CExchangeManager::CExchangeManager()
{
	m_mpExchangeRoom = new CPool<CExchangeRoom>;
	m_mpExchangeRoom->Init( MAX_EXCHANGEROOM, 50, "ExchangeRoomPool" );
}

CExchangeManager::~CExchangeManager()
{
	m_PtrList.RemoveAll();
	
	m_mpExchangeRoom->Release();
	SAFE_DELETE( m_mpExchangeRoom );
}

BOOL CExchangeManager::CanApplyExchange( CPlayer* pPlayer, CPlayer* pOpPlayer )
{
/////////

	if( pPlayer->GetState() != eObjectState_None && pPlayer->GetState() != eObjectState_Move
		&& pPlayer->GetState() != eObjectState_Immortal &&
		pPlayer->GetState() != eObjectState_Housing)
		return FALSE;
	if( pPlayer->IsShowdown() )
		return FALSE;

	if( pPlayer->GetBattle()->GetBattleKind() == eBATTLE_KIND_GTOURNAMENT )
	{
		if( pPlayer->GetBattleTeam() == 2 )			// Oberver
			return FALSE;
	}
	
//	ASSERT( pPlayer->GetExchangeContainer()->pRoom == NULL );	//Confirm
	if( pPlayer->GetExchangeContainer()->pRoom != NULL )
		return FALSE;

///////////////


	if( pOpPlayer == NULL )
		return FALSE;
	else if( pOpPlayer->GetGameOption()->bNoDeal )
		return FALSE;
	else if( pOpPlayer->GetExchangeContainer()->pRoom != NULL || pOpPlayer->IsShowdown() )
		return FALSE;
	else if( pOpPlayer->GetState() != eObjectState_None && pOpPlayer->GetState() != eObjectState_Move
		&& pOpPlayer->GetState() != eObjectState_Immortal && pOpPlayer->GetState() != eObjectState_Fishing &&
		pOpPlayer->GetState() != eObjectState_Housing)
		return FALSE;

	return TRUE;
}

CExchangeRoom* CExchangeManager::CreateExchangeRoom( CPlayer* p1, CPlayer* p2 )
{
	CExchangeRoom* pExchangeRoom = m_mpExchangeRoom->Alloc();

	ASSERT( pExchangeRoom );
	if( pExchangeRoom == NULL )
		return NULL;
	
	pExchangeRoom->Init( p1, p2 );
	pExchangeRoom->SetExchangeState( EXCS_WAITING );
	
	m_PtrList.AddTail( pExchangeRoom );

	return pExchangeRoom;
}

void CExchangeManager::DestroyExchangeRoom( CExchangeRoom* pRoom )
{
	if( pRoom == NULL ) return;
	pRoom->Exit();
	m_mpExchangeRoom->Free( pRoom );	
	m_PtrList.Remove( pRoom );
}

void CExchangeManager::UserLogOut( CPlayer* pPlayer )
{
	sEXCHANGECONTAINER* pEC = pPlayer->GetExchangeContainer();
	if( !pEC->pRoom ) return;
	
	if( pEC->pOpPlayer )
	{
		MSG_BYTE CancelMsg;
		CancelMsg.Category	= MP_EXCHANGE;
		CancelMsg.Protocol	= MP_EXCHANGE_CANCEL; //confirm
		CancelMsg.bData		= eEE_USERLOGOUT;

		pEC->pOpPlayer->SendMsg( &CancelMsg, sizeof( MSG_BYTE ) );
		if( pEC->pOpPlayer->GetState() == eObjectState_Exchange )
			OBJECTSTATEMGR_OBJ->EndObjectState(pEC->pOpPlayer, eObjectState_Exchange);

		pEC->pOpPlayer = NULL;
	}

	DestroyExchangeRoom( pEC->pRoom );
	pEC->pRoom = NULL;
}


void CExchangeManager::CancelExchange( CPlayer* pPlayer )
{
	sEXCHANGECONTAINER* pEC = pPlayer->GetExchangeContainer();

	if( !pEC->pRoom ) return;

	MSG_BYTE CancelMsg;
	CancelMsg.Category	= MP_EXCHANGE;
	CancelMsg.Protocol	= MP_EXCHANGE_CANCEL; //confirm
	CancelMsg.bData		= eEE_DIE;

	pPlayer->SendMsg( &CancelMsg, sizeof( MSG_BYTE ) );

	if( pEC->pOpPlayer )
	{
		CancelMsg.bData	= eEE_USERDIE;
		pEC->pOpPlayer->SendMsg( &CancelMsg, sizeof( MSG_BYTE ) );

		if( pEC->pOpPlayer->GetState() == eObjectState_Exchange )
			OBJECTSTATEMGR_OBJ->EndObjectState(pEC->pOpPlayer, eObjectState_Exchange);

		pEC->pOpPlayer = NULL;
	}

	DestroyExchangeRoom( pEC->pRoom );
	pEC->pRoom = NULL;
}


void CExchangeManager::NetworkMsgParse( BYTE Protocol, void* pMsg )
{
	switch( Protocol )
	{
	case MP_EXCHANGE_APPLY_SYN:
		{
			MSG_DWORD* msg = (MSG_DWORD*)pMsg;

			CPlayer* pPlayer	= (CPlayer*)g_pUserTable->FindUser(msg->dwObjectID);
			CPlayer* pOpPlayer	= (CPlayer*)g_pUserTable->FindUser(msg->dwData);

			ASSERT( pPlayer );
			if( pPlayer == NULL ) break;

			MSG_DWORD msgApplySyn;
			msgApplySyn.Category	= MP_EXCHANGE;
			msgApplySyn.dwData		= msg->dwData;

			if( CanApplyExchange( pPlayer, pOpPlayer ) )
			{
				msgApplySyn.Protocol	= MP_EXCHANGE_APPLY_ACK;
				pPlayer->SendMsg( &msgApplySyn, sizeof( msgApplySyn ) );
			}
			else
			{
				msgApplySyn.Protocol	= MP_EXCHANGE_APPLY_NACK;
				pPlayer->SendMsg( &msgApplySyn, sizeof( msgApplySyn ) );
				break;
			}

			CreateExchangeRoom( pPlayer, pOpPlayer );
	
			MSG_DWORD msgApply;
			msgApply.Category	= MP_EXCHANGE;
			msgApply.Protocol	= MP_EXCHANGE_APPLY;
			msgApply.dwData		= msg->dwObjectID;

			pOpPlayer->SendMsg( &msgApply, sizeof( msgApply ) );
			
			OBJECTSTATEMGR_OBJ->StartObjectState(pPlayer, eObjectState_Exchange, 0);
		}
		break;

	case MP_EXCHANGE_ACCEPT_SYN:
		{
			MSGBASE* msg = (MSGBASE*)pMsg;

			CPlayer* pPlayer1		= (CPlayer*)g_pUserTable->FindUser( msg->dwObjectID );
			if( pPlayer1 == NULL ) break;
			
			CPlayer* pPlayer2		= pPlayer1->GetExchangeContainer()->pOpPlayer;
			CExchangeRoom* pRoom	= pPlayer1->GetExchangeContainer()->pRoom;

			if( pPlayer2 == NULL )
			{
				MSGBASE AcceptNackMsg;
				AcceptNackMsg.Category	= MP_EXCHANGE;
				AcceptNackMsg.Protocol	= MP_EXCHANGE_ACCEPT_NACK;
				pPlayer1->SendMsg( &AcceptNackMsg, sizeof( MSGBASE ) );

				if( pRoom )
					DestroyExchangeRoom( pRoom );

				if( pPlayer1->GetState() == eObjectState_Exchange )
					OBJECTSTATEMGR_OBJ->EndObjectState(pPlayer1, eObjectState_Exchange);

				break;
			}

			if( pRoom == NULL )
			{
				MSGBASE AcceptNackMsg;
				AcceptNackMsg.Category	= MP_EXCHANGE;
				AcceptNackMsg.Protocol	= MP_EXCHANGE_ACCEPT_NACK;
				pPlayer1->SendMsg( &AcceptNackMsg, sizeof( MSGBASE ) );

				AcceptNackMsg.Protocol	= MP_EXCHANGE_CANTAPPLY;
				pPlayer2->SendMsg( &AcceptNackMsg, sizeof( MSGBASE ) );

				if( pPlayer1->GetState() == eObjectState_Exchange )
					OBJECTSTATEMGR_OBJ->EndObjectState(pPlayer1, eObjectState_Exchange);
				if( pPlayer2->GetState() == eObjectState_Exchange )
					OBJECTSTATEMGR_OBJ->EndObjectState(pPlayer2, eObjectState_Exchange);

				break;
			}			


			VECTOR3 vPosApplyer, vPosAccepter;
			pPlayer2->GetPosition( &vPosApplyer );
			pPlayer1->GetPosition( &vPosAccepter );
			float fDist = CalcDistanceXZ( &vPosApplyer, &vPosAccepter );	//confirm

			if( fDist > 1500.0f )
			{
				MSGBASE msgError;
				msgError.Category	= MP_EXCHANGE;
				msgError.Protocol	= MP_EXCHANGE_CANTAPPLY;
				pPlayer1->SendMsg( &msgError, sizeof( MSGBASE ) );
				pPlayer2->SendMsg( &msgError, sizeof( MSGBASE ) );

				if( pPlayer1->GetState() == eObjectState_Exchange )
					OBJECTSTATEMGR_OBJ->EndObjectState(pPlayer1, eObjectState_Exchange);
				if( pPlayer2->GetState() == eObjectState_Exchange )
					OBJECTSTATEMGR_OBJ->EndObjectState(pPlayer2, eObjectState_Exchange);

				DestroyExchangeRoom( pRoom );
				break;
			}
			
			MSG_DWORD AcceptMsg;
			AcceptMsg.Category	= MP_EXCHANGE;
			AcceptMsg.Protocol	= MP_EXCHANGE_ACCEPT;
			AcceptMsg.dwData	= msg->dwObjectID;	//
			pPlayer2->SendMsg( &AcceptMsg, sizeof( MSG_DWORD) );

			MSGBASE StartMsg;
			StartMsg.Category	= MP_EXCHANGE;
			StartMsg.Protocol	= MP_EXCHANGE_START;

			pPlayer1->SendMsg( &StartMsg, sizeof( MSGBASE ) );
			pPlayer2->SendMsg( &StartMsg, sizeof( MSGBASE ) );
			
			pPlayer1->GetExchangeContainer()->pRoom->SetExchangeState( EXCS_DOING );

			OBJECTSTATEMGR_OBJ->StartObjectState(pPlayer1, eObjectState_Exchange, 0);
		}
		break;

	case MP_EXCHANGE_REJECT_SYN:
		{
			MSGBASE* msg = (MSGBASE*)pMsg;

			CPlayer* pPlayer1 = (CPlayer*)g_pUserTable->FindUser( msg->dwObjectID );
			if( pPlayer1 == NULL ) break;

			CExchangeRoom* pRoom = pPlayer1->GetExchangeContainer()->pRoom;

			if( pRoom ) 
			if( pRoom->GetExchangeState() == EXCS_DOING )
				break;

			if( pPlayer1->GetState() == eObjectState_Exchange )
				OBJECTSTATEMGR_OBJ->EndObjectState(pPlayer1, eObjectState_Exchange);

			CPlayer* pPlayer2 = pPlayer1->GetExchangeContainer()->pOpPlayer;
			DestroyExchangeRoom( pRoom );

			if( pPlayer2 == NULL ) break;

			MSGBASE RejectMsg;
			RejectMsg.Category	= MP_EXCHANGE;
			RejectMsg.Protocol	= MP_EXCHANGE_REJECT;
			pPlayer2->SendMsg( &RejectMsg, sizeof( MSGBASE ) );

			if( pPlayer2->GetState() == eObjectState_Exchange )
				OBJECTSTATEMGR_OBJ->EndObjectState(pPlayer2, eObjectState_Exchange);
		}
		break;

	case MP_EXCHANGE_CANTAPPLY:
		{
			MSGBASE* msg = (MSGBASE*)pMsg;

			CPlayer* pPlayer1 = (CPlayer*)g_pUserTable->FindUser( msg->dwObjectID );
			if( pPlayer1 == NULL ) break;

			if( pPlayer1->GetState() == eObjectState_Exchange )
				OBJECTSTATEMGR_OBJ->EndObjectState(pPlayer1, eObjectState_Exchange);

			CPlayer* pPlayer2		= pPlayer1->GetExchangeContainer()->pOpPlayer;
			CExchangeRoom* pRoom	= pPlayer1->GetExchangeContainer()->pRoom;
			DestroyExchangeRoom( pRoom );

			if( pPlayer2 == NULL ) break;

			MSGBASE CantApplyMsg;
			CantApplyMsg.Category	= MP_EXCHANGE;
			CantApplyMsg.Protocol	= MP_EXCHANGE_CANTAPPLY;
			pPlayer2->SendMsg( &CantApplyMsg, sizeof( MSGBASE ) );

			if( pPlayer2->GetState() == eObjectState_Exchange )
				OBJECTSTATEMGR_OBJ->EndObjectState(pPlayer2, eObjectState_Exchange);
		}
		break;

	case MP_EXCHANGE_WAITING_CANCEL_SYN:
		{
			MSGBASE* msg = (MSGBASE*)pMsg;

			CPlayer* pPlayer1		= (CPlayer*)g_pUserTable->FindUser( msg->dwObjectID );
			if( pPlayer1 == NULL ) break;
			
			CExchangeRoom* pRoom = pPlayer1->GetExchangeContainer()->pRoom;

			if( pRoom != NULL )
			{
				if( pRoom->GetExchangeState() == EXCS_DOING )
				{
					MSGBASE CancelMsg = *msg;

					CancelMsg.Protocol = MP_EXCHANGE_WAITING_CANCEL_NACK;
					pPlayer1->SendMsg( &CancelMsg, sizeof( MSGBASE ) );
					break;
				}
			}
			else
			{
				break;
			}

			MSGBASE CancelMsg = *msg;

			CancelMsg.Protocol = MP_EXCHANGE_WAITING_CANCEL_ACK;
			pPlayer1->SendMsg( &CancelMsg, sizeof( MSGBASE ) );

			if( pPlayer1->GetState() == eObjectState_Exchange )
				OBJECTSTATEMGR_OBJ->EndObjectState(pPlayer1, eObjectState_Exchange);

			CPlayer* pPlayer2		= pPlayer1->GetExchangeContainer()->pOpPlayer;

			DestroyExchangeRoom( pRoom );

			if( pPlayer2 == NULL ) break;	//
			CancelMsg.Protocol = MP_EXCHANGE_WAITING_CANCEL;
			pPlayer2->SendMsg( &CancelMsg, sizeof( MSGBASE ) );
			
			if( pPlayer2->GetState() == eObjectState_Exchange )
				OBJECTSTATEMGR_OBJ->EndObjectState(pPlayer2, eObjectState_Exchange);
		}
		break;

	case MP_EXCHANGE_ADDITEM_SYN:
		{
			MSG_LINKITEM* msg = (MSG_LINKITEM*)pMsg;

			CPlayer* pPlayer1 = (CPlayer*)g_pUserTable->FindUser( msg->dwObjectID );
			if( pPlayer1 == NULL ) break;
			CPlayer* pPlayer2 = pPlayer1->GetExchangeContainer()->pOpPlayer;
			if( pPlayer2 == NULL ) break;
		
			CExchangeRoom* pRoom = pPlayer1->GetExchangeContainer()->pRoom;
			if( pRoom == NULL ) break;	

			MSG_LINKITEM ItemMsg = *msg;
			ITEM_INFO* pInfo = ITEMMGR->GetItemInfo( msg->ItemInfo.wIconIdx );
			if(!pInfo)		return;

			CItemSlot* pInventory = (CInventorySlot*)pPlayer1->GetSlot( eItemTable_Inventory );

			if( pInventory == NULL ) return;
			const ITEMBASE* pItemBase = pInventory->GetItemInfoAbs( msg->ItemInfo.Position );
			// 071125 KTH --- 봉인???��??�이?��? 교환??불�??�하?? 080204 KTH -- 교환 불�??�한 ?�이?�도 교환??불�??�하??
			if( pItemBase == NULL || pItemBase->nSealed == eITEM_TYPE_UNSEAL || !pInfo->Trade )
			{
				ItemMsg.Protocol = MP_EXCHANGE_ADDITEM_NACK;
				pPlayer1->SendMsg( &ItemMsg, sizeof( MSG_LINKITEM ) );
				return;
			}

			if( pItemBase->dwDBIdx != msg->ItemInfo.dwDBIdx ||
				pItemBase->wIconIdx != msg->ItemInfo.wIconIdx ||
				pItemBase->Durability != msg->ItemInfo.Durability ||
				pItemBase->Position != msg->ItemInfo.Position)
			{
				ItemMsg.Protocol = MP_EXCHANGE_ADDITEM_NACK;
				pPlayer1->SendMsg( &ItemMsg, sizeof( MSG_LINKITEM ) );
				return;
			}

			if( ITEMMGR->IsDupItem(pItemBase->wIconIdx) )
			{
				// 091215 ONS ?�이?�Stack최�??��? ?�이?�별�?지?�된 값으�?처리?�다.
				const WORD wItemStackNum = ITEMMGR->GetItemStackNum( pItemBase->wIconIdx );
				if( pItemBase->Durability > wItemStackNum )
				{
					ItemMsg.Protocol = MP_EXCHANGE_ADDITEM_NACK;
					pPlayer1->SendMsg( &ItemMsg, sizeof( MSG_LINKITEM ) );
					return;
				}
			}

			if( pRoom->IsLocked( pPlayer1->GetExchangeContainer()->nMyIndex ) || pRoom->IsLocked( pPlayer2->GetExchangeContainer()->nMyIndex ) )
			{
				ItemMsg.Protocol = MP_EXCHANGE_ADDITEM_NACK;
				pPlayer1->SendMsg( &ItemMsg, sizeof( MSG_LINKITEM ) );
				return;
			}

			if( !pRoom->AddItem( pPlayer1->GetExchangeContainer()->nMyIndex, &msg->ItemInfo ) )
			{
				ItemMsg.Protocol = MP_EXCHANGE_ADDITEM_NACK;
				pPlayer1->SendMsg( &ItemMsg, sizeof( MSG_LINKITEM ) );
				return;
			}
			if( !CHKRT->ItemOf( pPlayer1, msg->ItemInfo.Position, msg->ItemInfo.wIconIdx,
				msg->ItemInfo.Durability, msg->ItemInfo.QuickPosition ) )
			{
				ItemMsg.Protocol = MP_EXCHANGE_ADDITEM_NACK;
				pPlayer1->SendMsg( &ItemMsg, sizeof( MSG_LINKITEM ) );

				return;
			}

			ItemMsg.Protocol = MP_EXCHANGE_ADDITEM_ACK;
			pPlayer1->SendMsg( &ItemMsg, sizeof( MSG_LINKITEM ) );
			
			//ItemMsg.Protocol = MP_EXCHANGE_ADDITEM;
			//pPlayer2->SendMsg( &ItemMsg, sizeof( MSG_LINKITEM ) );

			MSG_LINKITEM_ADD ItemOptionMsg;
			ZeroMemory( &ItemOptionMsg, sizeof( ItemOptionMsg ) );

			ItemOptionMsg.Category		= MP_EXCHANGE;
			ItemOptionMsg.Protocol		= MP_EXCHANGE_ADDITEM;
			ItemOptionMsg.dwObjectID	= msg->dwObjectID;
			ItemOptionMsg.ItemInfo		= msg->ItemInfo;
			ItemOptionMsg.wAbsPosition	= msg->wAbsPosition;

			ItemOptionMsg.sOptionInfo	= ITEMMGR->GetOption( msg->ItemInfo );

			pPlayer2->SendMsg( &ItemOptionMsg, sizeof( ItemOptionMsg ) );
		}
		break;
	case MP_EXCHANGE_LOCK_SYN:
		{
			MSGBASE* msg = (MSGBASE*)pMsg;
			CPlayer* pPlayer1 = (CPlayer*)g_pUserTable->FindUser( msg->dwObjectID );
			if( pPlayer1 == NULL ) break;
			CPlayer* pPlayer2 = pPlayer1->GetExchangeContainer()->pOpPlayer;
			if( pPlayer2 == NULL ) break;
			//CHECK

			CExchangeRoom* pRoom = pPlayer1->GetExchangeContainer()->pRoom;
			if( !pRoom ) return;		//error

			pRoom->Lock( pPlayer1->GetExchangeContainer()->nMyIndex, TRUE );

			MSGBASE LockMsg = *msg;
			LockMsg.Protocol = MP_EXCHANGE_LOCK_ACK;
			pPlayer1->SendMsg( &LockMsg, sizeof( MSGBASE ) );

			LockMsg.Protocol	= MP_EXCHANGE_LOCK;
			pPlayer2->SendMsg( &LockMsg, sizeof( MSGBASE ) );


		}
		break;

	case MP_EXCHANGE_INPUTMONEY_SYN:
		{
			MSG_DWORDEX* msg = (MSG_DWORDEX*)pMsg;
			CPlayer* pPlayer1 = (CPlayer*)g_pUserTable->FindUser( msg->dwObjectID );
			if( pPlayer1 == NULL ) break;
			CPlayer* pPlayer2 = pPlayer1->GetExchangeContainer()->pOpPlayer;
			if( pPlayer2 == NULL ) break;

			CExchangeRoom* pRoom = pPlayer1->GetExchangeContainer()->pRoom;
			if( pRoom == NULL ) break;	//error
			
			//KES
			if( pRoom->IsLocked( pPlayer1->GetExchangeContainer()->nMyIndex ) || pRoom->IsLocked( pPlayer2->GetExchangeContainer()->nMyIndex )
				|| pRoom->GetExchangeMoney( pPlayer1->GetExchangeContainer()->nMyIndex ) > 0 )  //?�쪽?�라???�을 ?�거?? ?�을 ?�번?�라???�금?�을 경우
			{
				MSGBASE NackMsg;
				NackMsg.Category = MP_EXCHANGE;
				NackMsg.Protocol = MP_EXCHANGE_INPUTMONEY_NACK;
				NackMsg.dwObjectID = pPlayer1->GetID();
				
				pPlayer1->SendMsg( &NackMsg, sizeof(MSGBASE) );
				return;
			}
			
			DWORDEX dwMoney = pRoom->InputMoney( pPlayer1->GetExchangeContainer()->nMyIndex, msg->dweData );

			MSG_DWORDEX MoneyMsg = *msg;
			MoneyMsg.dweData = dwMoney;

			MoneyMsg.Protocol = MP_EXCHANGE_INPUTMONEY_ACK;
			pPlayer1->SendMsg( &MoneyMsg, sizeof( MSG_DWORD ) );

			MoneyMsg.Protocol = MP_EXCHANGE_INPUTMONEY;
			pPlayer2->SendMsg( &MoneyMsg, sizeof( MSG_DWORD ) );
		}
		break;
	case MP_EXCHANGE_CANCEL_SYN:
		{
			MSGBASE* msg = (MSGBASE*)pMsg;

			CPlayer* pPlayer1 = (CPlayer*)g_pUserTable->FindUser( msg->dwObjectID );
			if( pPlayer1 == NULL ) break;
			CPlayer* pPlayer2 = pPlayer1->GetExchangeContainer()->pOpPlayer;
			if( pPlayer2 == NULL ) break;
			
			CExchangeRoom* pRoom = pPlayer1->GetExchangeContainer()->pRoom;
			if( pRoom == NULL ) break;
			
			DestroyExchangeRoom( pRoom );

			MSGBASE CancelMsg = *msg;
			CancelMsg.Protocol = MP_EXCHANGE_CANCEL_ACK;
			pPlayer1->SendMsg( &CancelMsg, sizeof( MSGBASE ) );

			MSG_BYTE ErrorMsg;
			ErrorMsg.Category	= MP_EXCHANGE;
			ErrorMsg.Protocol	= MP_EXCHANGE_CANCEL;
			ErrorMsg.bData		= eEE_USERCANCEL;
			pPlayer2->SendMsg( &ErrorMsg, sizeof( MSG_BYTE ) );

			if( pPlayer1->GetState() == eObjectState_Exchange )
				OBJECTSTATEMGR_OBJ->EndObjectState(pPlayer1, eObjectState_Exchange);
			if( pPlayer2->GetState() == eObjectState_Exchange )
				OBJECTSTATEMGR_OBJ->EndObjectState(pPlayer2, eObjectState_Exchange);
		}
		break;
	
	case MP_EXCHANGE_EXCHANGE_SYN:
		{
			MSGBASE* msg = (MSGBASE*)pMsg;

			CPlayer* pPlayer1 = (CPlayer*)g_pUserTable->FindUser( msg->dwObjectID );
			if( pPlayer1 == NULL ) break;
			CPlayer* pPlayer2 = pPlayer1->GetExchangeContainer()->pOpPlayer;
			if( pPlayer2 == NULL ) break;

			CExchangeRoom* pRoom = pPlayer1->GetExchangeContainer()->pRoom;

			if( pRoom == NULL ) break;	//confirm
			if( pRoom->IsLocked( pPlayer1->GetExchangeContainer()->nMyIndex ) == FALSE || pRoom->IsLocked( pPlayer2->GetExchangeContainer()->nMyIndex ) == FALSE )
			{

				MSGBASE ExchangeNackMsg;
				ExchangeNackMsg.Category = MP_EXCHANGE;
				ExchangeNackMsg.Protocol = MP_EXCHANGE_EXCHANGE_NACK;
				pPlayer1->SendMsg( &ExchangeNackMsg, sizeof( MSGBASE ) );
				break;
			}

			pRoom->SetExchange( pPlayer1->GetExchangeContainer()->nMyIndex );

			if( pRoom->IsAllExchange() )
			{
				int rt;
				if( ( rt = pRoom->DoExchange() ) == eEE_OK )
				{
					MSGBASE ExchangeMsg;
					ExchangeMsg.Category	= MP_EXCHANGE;
					ExchangeMsg.Protocol	= MP_EXCHANGE_EXCHANGE;
					pPlayer1->SendMsg( &ExchangeMsg, sizeof( MSGBASE ) );
					pPlayer2->SendMsg( &ExchangeMsg, sizeof( MSGBASE ) );
				}
				else
				{

					MSG_BYTE ErrorMsg;
					ErrorMsg.Category	= MP_EXCHANGE;
					ErrorMsg.Protocol	= MP_EXCHANGE_CANCEL;
					ErrorMsg.bData		= BYTE( rt );
					pPlayer1->SendMsg( &ErrorMsg, sizeof( MSG_BYTE ) );
					pPlayer2->SendMsg( &ErrorMsg, sizeof( MSG_BYTE ) );
				}

				DestroyExchangeRoom( pRoom );

				OBJECTSTATEMGR_OBJ->EndObjectState( pPlayer1, eObjectState_Exchange );
				OBJECTSTATEMGR_OBJ->EndObjectState( pPlayer2, eObjectState_Exchange );
			}
		}
		break;
	}
}

CExchangeRoom* CExchangeManager::GetRoomInfo( DWORD dwPlayer1ID, CPlayer** ppPlayer1, CPlayer** ppPlayer2 )
{
	*ppPlayer1 = (CPlayer*)g_pUserTable->FindUser( dwPlayer1ID );
	if( *ppPlayer1 == NULL ) return NULL;

	*ppPlayer2 = (*ppPlayer1)->GetExchangeContainer()->pOpPlayer;

	return (*ppPlayer1)->GetExchangeContainer()->pRoom;
}


