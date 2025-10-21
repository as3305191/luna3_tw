#include "stdafx.h"
#include "VehicleManager.h"
#include "ObjectManager.h"
#include "MoveManager.h"
#include "ItemManager.h"
#include "ChatManager.h"
#include "InventoryExDialog.h"
#include "WindowIdEnum.h"
#include "cWindowManager.h"
#include "QuickManager.h"
#include "cMsgBox.h"
#include "GameIn.h"
#include "MHMap.h"

CVehicleManager::CVehicleManager()
{
	m_bCheckTime = TRUE;
}

CVehicleManager::~CVehicleManager()
{
}

void CVehicleManager::NetworkMsgParse( BYTE Protocol, void* pMsg )
{
	switch( Protocol )
	{
	case MP_VEHICLE_ERROR_ACK:
		{
			// 090316 LUJ, ��ȯ ���� ó���� ���ش�. ���޵Ǵ� ���� ����
			const MSG_VEHICLE_ERROR* const message = ( MSG_VEHICLE_ERROR* )pMsg;

			switch( message->mError )
			{
			case MSG_VEHICLE_ERROR::ErrorSummonByDistance:
			case MSG_VEHICLE_ERROR::ErrorSummonByUsingItem:
			case MSG_VEHICLE_ERROR::ErrorSummonByCreateVehicle:

				// 100528 ShinJS --- ��ġ ���н� �������� ����� �����Ѵ�.
				const POSTYPE dwItemPos = POSTYPE( message->dwData );
				CItem* pItem = ITEMMGR->GetItemofTable( eItemTable_Inventory, dwItemPos );
				if( !pItem ||
					pItem->GetItemInfo()->SupplyType != ITEM_KIND_SUMMON_VEHICLE )
					break;

				pItem->SetLock( FALSE );
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 2262 ) );

				break;
			}

#ifndef _GMTOOL_
			switch( message->mError )
			{
			case MSG_VEHICLE_ERROR::ErrorNone:
				{
					_tcprintf("vehicle has no error\n");
					break;
				}
			case MSG_VEHICLE_ERROR::ErrorSummonByItem:
				{
					_tcprintf("Summonining vehicle is failed by invalid item information\n");
					break;
				}
			case MSG_VEHICLE_ERROR::ErrorSummonByDistance:
				{
					_tcprintf("Summonining vehicle is failed by distance\n");
					break;
				}
			case MSG_VEHICLE_ERROR::ErrorSummonByUsingItem:
				{
					_tcprintf("Summonining vehicle is failed by using item\n");
					break;
				}
			case MSG_VEHICLE_ERROR::ErrorSummonByCreateVehicle:
				{
					_tcprintf("Server failed creating vehicle during summonining\n");
					break;
				}
			case MSG_VEHICLE_ERROR::ErrorSummonAlready:
				{
					_tcprintf("You summoned vehicle already\n");
					break;
				}
			case MSG_VEHICLE_ERROR::ErrorSummonByMount:
				{
					_tcprintf("You cannot summon during mounting\n");
					break;
				}
			case MSG_VEHICLE_ERROR::ErrorUnsummonByDistance:
				{
					_tcprintf("Unsummoning vehicle is failed by distance\n");
					break;
				}
			case MSG_VEHICLE_ERROR::ErrorUnsummonByNoMine:
				{
					_tcprintf("It's no yours\n");
					break;
				}
			case MSG_VEHICLE_ERROR::ErrorUnsummonByPassenger:
				{
					_tcprintf("You should unmount all passenger include you\n");
					break;
				}
			case MSG_VEHICLE_ERROR::ErrorMountByDistance:
				{
					_tcprintf("Mounting vehicle is failed by distance\n");
					break;
				}
			case MSG_VEHICLE_ERROR::ErrorMountByFullSeat:
				{
					_tcprintf("There is no seat on vehicle\n");
					break;
				}
			case MSG_VEHICLE_ERROR::ErrorMountBySitAlready:
				{
					_tcprintf("You sit already on vehicle\n");
					break;
				}
			case MSG_VEHICLE_ERROR::ErrorMountByNoVehicle:
				{
					_tcprintf("There is no vehicle\n");
					break;
				}
			case MSG_VEHICLE_ERROR::ErrorMountByOverSeatIndex:
				{
					_tcprintf("Requested seat index is out of range\n");
					break;
				}
			case MSG_VEHICLE_ERROR::ErrorMountByNoOwner:
				{
					_tcprintf("There is no owner\n");
					break;
				}
			case MSG_VEHICLE_ERROR::ErrorMountByPlayerLevel:
				{
					_tcprintf("You need to require more level\n");
					break;
				}
			case MSG_VEHICLE_ERROR::ErrorMountBySummonedAlready:
				{
					_tcprintf("You cannot mount to other vehicle by yours\n");
					break;
				}
			case MSG_VEHICLE_ERROR::ErrorDismountbyDistance:
				{
					_tcprintf("You should come closer to dismount\n");
					break;
				}
			case MSG_VEHICLE_ERROR::ErrorDismountByNotSit:
				{
					_tcprintf("You not sit on vehicle\n");
					break;
				}
			case MSG_VEHICLE_ERROR::ErrorMoveByNoOwner:
				{
					_tcprintf("You are not owner\n");
					break;
				}
			case MSG_VEHICLE_ERROR::ErrorTollByFullSeat:
				{
					_tcprintf("Seat is full\n");
					break;
				}
			case MSG_VEHICLE_ERROR::ErrorTollByNoOwner:
				{
					_tcprintf("There is no owner\n");
					break;
				}
			case MSG_VEHICLE_ERROR::ErrorTollByNoPassenger:
				{
					_tcprintf("There is no passenger\n");
					break;
				}
			case MSG_VEHICLE_ERROR::ErrorTollByOwnerHasFullMoney:
				{
					_tcprintf("Owner has too many money. He should decrease it\n");
					break;
				}
			case MSG_VEHICLE_ERROR::ErrorTollByPassengerHasInsufficientMoney:
				{
					_tcprintf("You has insufficent money\n");
					break;
				}
			}
#endif
		}
		break;
	case MP_VEHICLE_SUMMON_ACK:
		{
			// 090316 LUJ, �������� ���� ��ƾ���� ó���Ѵ�.
			const MSG_DWORD4* const message = ( MSG_DWORD4* )pMsg;
			const DWORD dwVehicleID = message->dwData1;
			const DWORD dwPlayerID = message->dwData2;
			const POSTYPE dwItemPos = POSTYPE( message->dwData3 );
			const BOOL summonByPlayer = message->dwData4;

			Summon( dwVehicleID, dwPlayerID, dwItemPos, summonByPlayer );
		}
		break;		

		// 090316 LUJ, ��ȯ ����
	case MP_VEHICLE_UNSUMMON_ACK:
		{
			const MSG_DWORD* const message = ( MSG_DWORD* )pMsg;
			const DWORD playerIndex = message->dwObjectID;
			const DWORD vehicleIndex = message->dwData;

			UnSummon( playerIndex, vehicleIndex );
		}
		break;

		// 090316 LUJ, �ٸ� �÷��̾ ������� Ż�Ϳ� ������ ��û��
	case MP_VEHICLE_MOUNT_REQUEST_SYN:
		{
			const MSG_DWORD4_VEHICLE* const message = ( MSG_DWORD4_VEHICLE* )pMsg;
			const DWORD askedPlayerIndex = message->dwData1;
			const DWORD vehicleIndex = message->dwData2;
			const MONEYTYPE chargedMoney = message->dwData3;
			const DWORD key = message->dwData4;

			RequestGetOn(
				askedPlayerIndex,
				vehicleIndex,
				chargedMoney,
				key);
		}
		break;

		// 090316 LUJ, ���� ó��
	case MP_VEHICLE_MOUNT_ALLOW_ACK:
		{
			const MSG_VEHICLE_ALLOW* const message = ( MSG_VEHICLE_ALLOW* )pMsg;			
			
			// 090422 ShinJS --- ž��ó��
			GetOn( message->mMountedObjectIndex,
					message->mVehicleObjectIndex,
					message->mMountedSeatIndex,
					message->mOwnerObjectIndex,
					message->mTollMoney,
					message->mVehicleObjectName,
					message->mMountedObjectName );
		}
		break;

		// 090316 LUJ, ������ �ź� ����
	case MP_VEHICLE_MOUNT_REJECT_ACK:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1828 ) );
		}
		break;

		// 090316 LUJ, ���� ó��
	case MP_VEHICLE_DISMOUNT_ACK:
		{
			const MSG_DWORD3* const message = ( MSG_DWORD3* )pMsg;
			const DWORD playerIndex = message->dwData1;
			const DWORD vehicleIndex = message->dwData2;
			const DWORD seatIndex = message->dwData3;
			
			GetOff( playerIndex, vehicleIndex, seatIndex );
		}
		break;
	case MP_VEHICLE_GET_OPTION_ACK:
		{
			const MSG_DWORD3_VEHICLE* const message = (MSG_DWORD3_VEHICLE*)pMsg;
			const DWORD vehicleIndex = message->dwData1;
			const MONEYTYPE chargedMoney = message->dwData2;
			const DWORD key = message->dwData3;

			cMsgBox* const messageBox = WINDOWMGR->MsgBox(
				MBI_VEHICLE_GETON_ASK_PAY_TOLL,
				MBT_YESNO,
				CHATMGR->GetChatMsg( 1826 ),
				AddComma(chargedMoney));

			if(0 == messageBox)
			{
				break;
			}

			messageBox->SetParam(
				key);
			SetRidingVehicle(
				vehicleIndex);
		}
		break;
	}
}

void CVehicleManager::Summon( DWORD dwVehicleID, DWORD dwPlayerID, POSTYPE dwItemPos, BOOL summonByPlayer ) const
{
	CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject( dwPlayerID );
	if( !pPlayer || pPlayer->GetObjectKind() != eObjectKind_Player )
		return;

	// Player�� VehicleID�� �����Ѵ�
	pPlayer->SetVehicleID( dwVehicleID );
	// ��ȯ �ð��� �����Ѵ�
	pPlayer->SetLastVehicleSummonTime( gCurTime );


	// ��ȯ�ڰ� HERO�� ��� ������ �Ҹ� ó���� �Ѵ�
	if( dwPlayerID != HEROID )
		return;

	// 090316 LUJ, ������ �Ҹ� ó��
	{
		const ITEMBASE* const itemBase = ITEMMGR->GetItemInfoAbsIn( 0, dwItemPos );

		if( 0 == itemBase )
		{
			return;
		}

		CItem* const item = ITEMMGR->GetItem( itemBase->dwDBIdx );

		// 090316 LUJ, ������ �Ҹ� ó��
		if( 0 == item )
		{
			return;
		}

		item->SetLock(
			TRUE);

		// 090316 LUJ, �� �̵� �ÿ��� ������ ���� ��ȯ�ǹǷ� ������ �Ҹ� �Ǿ�� �ȵȴ�
		if( 0 < item->GetDurability() &&
			TRUE == summonByPlayer )
		{
			const ITEM_INFO* const itemInfo = item->GetItemInfo();
			// �Ҹ� �������� ���� �޽��� ǥ��
			CHATMGR->AddMsg( CTC_EXPENDEDITEM, CHATMGR->GetChatMsg( 1111 ), itemInfo ? itemInfo->ItemName : "?" );

			item->SetDurability( item->GetDurability() - 1 );

			if( 0 == item->GetDurability() )
			{
				CItem* pOutItem = 0;
				ITEMMGR->DeleteItem( dwItemPos, &pOutItem );
			}

			QUICKMGR->RefreshQickItem();
		}

		ICONBASE usedItem = { 0 };
		usedItem.wIconIdx = itemBase->wIconIdx;
		usedItem.Position = dwItemPos;
		usedItem.dwDBIdx = itemBase->dwDBIdx;

		CVehicle* const vehicle = ( CVehicle* )OBJECTMGR->GetObject( dwVehicleID );

		if( vehicle &&
			vehicle->GetObjectKind() == eObjectKind_Vehicle )
		{
			vehicle->SetUsedItem( usedItem );
		}
	}
}

void CVehicleManager::UnSummon( DWORD dwPlayerID, DWORD dwVehicleID ) const
{
	CPlayer* const player = ( CPlayer* )OBJECTMGR->GetObject( dwPlayerID );

	if( player && player->GetObjectKind() == eObjectKind_Player )
	{
		player->SetVehicleID( 0 );
		// ���� �ð��� �����Ѵ�
		player->SetLastVehicleSummonTime( gCurTime );
	}

	// 090316 LUJ, Ÿ�� �ִ� ����ڸ� ��� ������ �ؾ��Ѵ�
	CVehicle* const vehicle = ( CVehicle* )OBJECTMGR->GetObject( dwVehicleID );

	if( vehicle && vehicle->GetObjectKind() == eObjectKind_Vehicle )
	{
		vehicle->GetOffAll();

		// 100708 ShinJS �ڽż����� ��� ������ Lock ����
		if( vehicle->GetOwnerIndex() == HEROID )
		{
			if(CItem* const item = ITEMMGR->GetItem(vehicle->GetUsedItem().dwDBIdx))
			{
				item->SetLock(
					FALSE);
			}
		}
	}

	if( OBJECTMGR->GetSelectedObjectID() == dwVehicleID )
	{
		OBJECTMGR->SetSelectedObjectID( 0 );
	}
}

void CVehicleManager::RequestGetOn(DWORD dwAskPlayerID, DWORD dwVehicleID, MONEYTYPE chargedMoney, DWORD key) const
{
	CObject* pAskPlayer = OBJECTMGR->GetObject( dwAskPlayerID );
	CVehicle* pVehicle = (CVehicle*)OBJECTMGR->GetObject( dwVehicleID );
	if( !pAskPlayer || !pVehicle || pVehicle->GetObjectKind() != eObjectKind_Vehicle )		return;

	cMsgBox* const messageBox = WINDOWMGR->MsgBox(
		MBI_VEHICLE_ALLOW_GETON, 
		MBT_YESNO, 
		CHATMGR->GetChatMsg( 1827 ),
		pAskPlayer->GetObjectName(),
		HERO->GetObjectName(),
		pVehicle->GetObjectName());

	if(0 == messageBox)
	{
		MSG_DWORD2 msg;
		msg.Category = MP_VEHICLE;
		msg.Protocol = MP_VEHICLE_MOUNT_REQUEST_NACK;
		msg.dwObjectID = gHeroID;
		msg.dwData1 = dwAskPlayerID;
		msg.dwData2 = dwVehicleID;
		NETWORK->Send( &msg, sizeof( msg ) );
		return;
	}

	messageBox->SetParam(
		key);
	pVehicle->SetRequestGetOnPlayerID( dwAskPlayerID );
}

void CVehicleManager::GetOn( DWORD dwPlayerID, DWORD dwVehicleID, DWORD dwSeatPos, DWORD dwMasterID, DWORDEX dwTollMoney, const char* szVehicleName, const char* szPlayerName ) const
{
	// 090422 ShinJS --- ž�� ����� �ִ� ���(�����ְ� Ż�Ͱ� �Ÿ��� �� ��� Ż�Ͱ� ž���ڰ� �������� �ʴ� ��찡 �߻��Ͽ� ž��ó���� ���� �������[�̸�, ������ID]�� �޾Ƽ� ó���Ѵ�)
	if( dwTollMoney )
	{
		// ž���� ž�¿�� ���� �޼��� ���
		if( HEROID == dwPlayerID &&			// ž���ڰ� HERO�̰� 			
			dwMasterID != dwPlayerID )		// Ż���� ������ �ƴѰ��
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1830 ), AddComma( dwTollMoney ) );
		}
		// ������ ž�¿�� ���� �޼��� ���
		else if( HEROID == dwMasterID &&	// Ż���� HERO�� Ż���̰�			
				HEROID != dwPlayerID )		// HERO�� ž���ڰ� �ƴѰ��(�ڽ��� Ż���� ž�·Ḧ �������� �ʴ´�)
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1831 ), szPlayerName, szVehicleName );
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1832 ), AddComma( dwTollMoney ) );
		}
	}

	CPlayer* const player = ( CPlayer* )OBJECTMGR->GetObject( dwPlayerID );
	CVehicle* const vehicle = ( CVehicle* )OBJECTMGR->GetObject( dwVehicleID );

	if( 0 == player || eObjectKind_Player != player->GetObjectKind() ||
		0 == vehicle || eObjectKind_Vehicle != vehicle->GetObjectKind() )
	{
		return;
	}

	// ž�½� Ż���� ��ġ�� ž���ڸ� �̵���Ų��
	MOVEMGR->MoveStop(
		player,
		&vehicle->GetCurPosition());
	vehicle->GetOn(
		player,
		dwSeatPos );
}

void CVehicleManager::GetOff( DWORD dwPlayerID, DWORD dwVehicleID, DWORD dwSeatPos ) const
{
	CPlayer* const player = ( CPlayer* )OBJECTMGR->GetObject( dwPlayerID );
	CVehicle* const vehicle = ( CVehicle* )OBJECTMGR->GetObject( dwVehicleID );

	if( ! player || player->GetObjectKind() != eObjectKind_Player ||
		! vehicle || vehicle->GetObjectKind() != eObjectKind_Vehicle )
	{
		return;
	}

	// ���� ó��
	vehicle->GetOff( player, dwSeatPos );
	player->SetLastVehicleGetOffTime( gCurTime );
}


void CVehicleManager::Follow( void* pMsg )
{
	// ���󰡱� ������ Vehicle�� ��´�
	MOVE_ONETARGETPOS_FROMSERVER* pmsg = (MOVE_ONETARGETPOS_FROMSERVER*)pMsg;

	CVehicle* pVehicle = (CVehicle*)OBJECTMGR->GetObject( pmsg->dwObjectID );
	if( !pVehicle || pVehicle->GetObjectKind() != eObjectKind_Vehicle )
		return;
	
	// Ż�Ͱ� ž���ڸ� �̵���Ų��
	pVehicle->Move_OneTarget( pmsg );
}

void CVehicleManager::InstallVehicle( CObject* pObject )
{
	if( pObject == NULL ||
		pObject->GetObjectKind() != eObjectKind_Vehicle ||
		pObject->GetID() != VEHICLE_INSTALL_ID )
		return;

	CVehicle* pVehicle = (CVehicle*)pObject;

	// ��ġ�� �ȵ� ���
	if( !pVehicle->IsInstalled() )
	{
		VECTOR3 vecVehiclePos, vecHeroPos;
		vecVehiclePos = pVehicle->GetCurPosition();
		vecHeroPos = HERO->GetCurPosition();
		float fDist = CalcDistanceXZ(&vecHeroPos, &vecVehiclePos);
		WORD wMonsterKind = pVehicle->GetMonsterKind();

		const VehicleScript& script = GAMERESRCMNGR->GetVehicleScript( wMonsterKind );
		if( script.mMonsterKind != wMonsterKind )		return;

		// ��ġ ��ȿ������ ��� ���
		if( fDist > script.mCondition.mSummonRange )
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1819 ) );
			return;
		}

		// ��ġ �Ҽ� ���� ����� ���
		if( !pVehicle->IsValidPosForInstall() )
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1820 ) );
			return;
		}

		// ��۽��Ҽ� ������ ��ġObject�� �����Ѵ�
		OBJECTMGR->RemoveVehicle( VEHICLE_INSTALL_ID );

		CInventoryExDialog* pDlg = GAMEIN->GetInventoryDialog();
		if( !pDlg ) return;

		// ��Ÿ�Ӽ����� Progress ������ ���� �Լ� ����
		pDlg->StartVehicleSummon( vecVehiclePos );
	}
}

BOOL CVehicleManager::InstallVehicleToNearPosition( const WORD wMonsterKind )
{
	VECTOR3 installPos={0}, heroPos={0};

	// Ư�� ����
	installPos.x = (float)(rand() % 100);
	installPos.y = (float)(rand() % 100);
	installPos.z = (float)(rand() % 100);
	Normalize( &installPos, &installPos );

	const VehicleScript& script = GAMERESRCMNGR->GetVehicleScript( wMonsterKind );
	const int randomDistLimit = max( (int)script.mCondition.mSummonRange, 1000 );

	// ���� �Ÿ�
	installPos = installPos * (float)(rand() % randomDistLimit);

	HERO->GetPosition( &heroPos );
	installPos = heroPos + installPos;

	// �̵����� Ÿ�Ϸ� ����
	MAP->CollisionLine( &heroPos, &installPos, &installPos, MAP->GetMapNum(), HERO );

	CInventoryExDialog* pDlg = GAMEIN->GetInventoryDialog();
	if( !pDlg ) return FALSE;

	// ��Ÿ�Ӽ����� Progress ������ ���� �Լ� ����
	pDlg->StartVehicleSummon( installPos );

	return TRUE;
}

BOOL CVehicleManager::IsMaster( DWORD dwPlayerID )
{
	CPlayer* const pPlayer = ( CPlayer* )OBJECTMGR->GetObject( dwPlayerID );
	CVehicle* const pVehicle = ( CVehicle* )OBJECTMGR->GetObject( pPlayer->GetVehicleID() );

	if( !pPlayer || pPlayer->GetObjectKind() != eObjectKind_Player || 
		!pVehicle || pVehicle->GetObjectKind() != eObjectKind_Vehicle )
		return FALSE;

    return (pVehicle->GetOwnerIndex() == dwPlayerID);
}