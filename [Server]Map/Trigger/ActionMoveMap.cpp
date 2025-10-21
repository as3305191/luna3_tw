#include "StdAfx.h"
#include "ActionMoveMap.h"
#include "common.h"
#include "..\MapNetworkMsgParser.h"
#include "..\UserTable.h"
#include "..\Object.h"
#include "..\Player.h"
#include "..\Vehicle.h"
#include "..\Pet.h"
#include "..\VehicleManager.h"
#include "..\PackedData.h"
#include "..\CharMove.h"

namespace Trigger
{
	void CActionMoveMap::DoAction()
	{
		const DWORD maintainCurrentChannel = WORD( -1 );
		MSG_NAME_DWORD2 message;
		ZeroMemory( &message, sizeof( message ) );
		message.Category = MP_CHEAT;
		message.Protocol = MP_CHEAT_CHANGEMAP_SYN;
		message.dwData1	= GetValue(eProperty_MapIndex);
		message.dwData2	= maintainCurrentChannel;

		for(DWORD objectIndex = GetHeadTarget();
			objectIndex > 0;
			objectIndex = GetNextTarget())
		{
			message.dwObjectID = objectIndex;
			Cheat_ChangeMap_Syn( ( char* )&message );
		}
	}

	void CActionMoveObject::DoAction()
	{
		VECTOR3 vecPos={0,};
		vecPos.x = (float)GetValue(eProperty_PosX);
		vecPos.z = (float)GetValue(eProperty_PosZ);

		for(DWORD objectIndex = GetHeadTarget();
			objectIndex > 0;
			objectIndex = GetNextTarget())
		{
			CObject* const object = g_pUserTable->FindUser(objectIndex);

			if(0 == object)
			{
				continue;
			}

			if( object->GetObjectKind() == eObjectKind_Player )
			{
				CPlayer* pPlayer = (CPlayer*)object;
				// Ż�Ϳ� ž���� ���
				if( pPlayer->GetMountedVehicle() != 0 )
				{
					CVehicle* pVehicle = (CVehicle*)g_pUserTable->FindUser( pPlayer->GetMountedVehicle() );
					if( !pVehicle || pVehicle->GetObjectKind() != eObjectKind_Vehicle )
						continue;

					// ������ �Ѵ�.
					VEHICLEMGR->Dismount( pPlayer->GetID(), FALSE );
				}

				// Ż���� ������ ���
				if( pPlayer->GetSummonedVehicle() != 0 )
				{
					CVehicle* pVehicle = ( CVehicle* )g_pUserTable->FindUser( pPlayer->GetSummonedVehicle() );
					if( !pVehicle || pVehicle->GetObjectKind() != eObjectKind_Vehicle )
						continue;

					// �̵���
					if( pVehicle->GetMoveSpeed() > 0.0f )
					{
						// �ڽ� �̿��� ž���ڵ��� ������ �Ѵ�.
						pVehicle->DismountAllExceptOne( pPlayer->GetID() );

						// Ż���� �̵���Ų��.
						CCharMove::SetPosition( pVehicle, &vecPos );

						MOVE_POS msg;
						msg.Category = MP_MOVE;
						msg.Protocol = MP_MOVE_WARP;
						msg.dwMoverID = pVehicle->GetID();
						msg.cpos.Compress( &vecPos );

						PACKEDDATA_OBJ->QuickSend( pVehicle, &msg, sizeof(msg) );
					}
				}
			}

			CCharMove::SetPosition( object, &vecPos );

			MOVE_POS msg;
			msg.Category = MP_MOVE;
			msg.Protocol = MP_MOVE_WARP;
			msg.dwMoverID = object->GetID();
			msg.cpos.Compress( &vecPos );

			PACKEDDATA_OBJ->QuickSend( object, &msg, sizeof(msg) );
		}
	}

	void CActionAddShockedTime::DoAction()
	{
		const DWORD dwShockedTime = GetValue(eProperty_ElapsedSecond) * 1000;

		for(DWORD objectIndex = GetHeadTarget();
			objectIndex > 0;
			objectIndex = GetNextTarget())
		{
			CObject* const object = g_pUserTable->FindUser(objectIndex);

			if(0 == object)
			{
				continue;
			}

			object->SetShockTime( dwShockedTime );

			MSG_DWORD2	msg;
			ZeroMemory( &msg, sizeof(msg) );
			msg.Category = MP_TRIGGER;
			msg.Protocol = MP_TRIGGER_ADD_SHOCKED_TIME_ACK;
			msg.dwObjectID = object->GetID();
			msg.dwData1 = objectIndex;
			msg.dwData2 = dwShockedTime;
			

			switch( object->GetObjectKind() )
			{
			case eObjectKind_Vehicle:
			case eObjectKind_Pet:
				{
					CObject* const ownerPlayer = g_pUserTable->FindUser( object->GetOwnerIndex() );
					if( ownerPlayer == NULL )
						break;

					ownerPlayer->SendMsg(&msg, sizeof(msg));
				}
				break;

			default:
				{
					object->SendMsg(&msg, sizeof(msg));
				}
				break;
			}
		}
	}
}