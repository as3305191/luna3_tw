#include "StdAfx.h"
#include "ActionCamera.h"
#include "common.h"
#include "..\Object.h"
#include "..\Player.h"
#include "..\Vehicle.h"
#include "..\VehicleManager.h"
#include "..\UserTable.h"
#include "..\PackedData.h"
#include "..\CharMove.h"

namespace Trigger
{
	void CActionCameraFilterAttach::DoAction()
	{
        MSG_DWORD4 message;
		ZeroMemory(&message, sizeof(message));
		const DWORD dwFilterIndex = DWORD(GetValue(eProperty_CameraFilterIndex));
		// 100119 ShinJS --- ī�޶� ���� ������ �߰�(�ݺ�����, FadeIn ����, Fade ���[���ذ�=100])
		BOOL bNoRepeat = GetValue(eProperty_CameraFilterNoRepeat);
		BOOL bFadeIn = GetValue(eProperty_CameraFilterFadeIn);
		DWORD dwFadeRatio = (DWORD)GetValue(eProperty_CameraFilterFadeTime);

		message.Category = MP_TRIGGER;
		message.Protocol = MP_TRIGGER_CAMERA_FILTER_ATTACH;
		message.dwData1 = dwFilterIndex;		// CameraFilterList.bin�� ���ǵ� �ε���
		message.dwData2 = DWORD( bNoRepeat );
		message.dwData3 = DWORD( bFadeIn );
		message.dwData4 = dwFadeRatio;

		for(DWORD objectIndex = GetHeadTarget();
			objectIndex > 0;
			objectIndex = GetNextTarget())
		{
			CObject* const object = g_pUserTable->FindUser(objectIndex);

			if(0 == object)
			{
				continue;
			}
			else if(eObjectKind_Player != object->GetObjectKind())
			{
				continue;
			}

			message.dwObjectID = objectIndex;
			object->SendMsg(&message, sizeof(message));
		}
	}

	void CActionCameraFilterDetach::DoAction()
	{
        MSG_DWORD3 message;
		ZeroMemory(&message, sizeof(message));
		// CameraFilterList.bin�� ���ǵ� �ε���
		const DWORD dwFilterIndex = DWORD(GetValue(eProperty_CameraFilterIndex));
		const BOOL bFadeOut = GetValue(eProperty_CameraFilterFadeOut);
		const DWORD dwFadeRatio = GetValue(eProperty_CameraFilterFadeTime);

		message.Category = MP_TRIGGER;
		message.Protocol = MP_TRIGGER_CAMERA_FILTER_DETACH;
		message.dwData1 = dwFilterIndex; 
		message.dwData2 = bFadeOut;
		message.dwData3 = dwFadeRatio;

		for(DWORD objectIndex = GetHeadTarget();
			objectIndex > 0;
			objectIndex = GetNextTarget())
		{
			CObject* const object = g_pUserTable->FindUser(objectIndex);

			if(0 == object)
			{
				continue;
			}
			else if(eObjectKind_Player != object->GetObjectKind())
			{
				continue;
			}

			message.dwObjectID = objectIndex;
			object->SendMsg(&message, sizeof(message));
		}
	}

	void CActionCameraFilterMove::DoAction()
	{
		MSG_DWORD6 message;
		ZeroMemory(&message, sizeof(message));
		const DWORD dwFilterIndex = DWORD(GetValue(eProperty_CameraFilterIndex));
		// 100119 ShinJS --- ī�޶� ���� ������ �߰�(�ݺ�����, FadeIn ����, Fade ���[���ذ�=100])
		BOOL bNoRepeat = GetValue(eProperty_CameraFilterNoRepeat);
		BOOL bFadeIn = GetValue(eProperty_CameraFilterFadeIn);
		DWORD dwFadeRatio = (DWORD)GetValue(eProperty_CameraFilterFadeTime);
		VECTOR3 vecPos={0,};
		vecPos.x = (float)GetValue(eProperty_PosX);
		vecPos.z = (float)GetValue(eProperty_PosZ);

		message.Category = MP_TRIGGER;
		message.Protocol = MP_TRIGGER_CAMERA_FILTER_MOVE_ACK;
		message.dwData1 = dwFilterIndex;		// CameraFilterList.bin�� ���ǵ� �ε���
		message.dwData2 = DWORD( bNoRepeat );
		message.dwData3 = DWORD( bFadeIn );
		message.dwData4 = dwFadeRatio;
		message.dwData5 = (DWORD)vecPos.x;
		message.dwData6 = (DWORD)vecPos.z;

		for(DWORD objectIndex = GetHeadTarget();
			objectIndex > 0;
			objectIndex = GetNextTarget())
		{
			CObject* const object = g_pUserTable->FindUser(objectIndex);

			if(0 == object)
			{
				continue;
			}
			else if(eObjectKind_Player != object->GetObjectKind())
			{
				continue;
			}

			message.dwObjectID = objectIndex;
			object->SendMsg(&message, sizeof(message));
			
			CPlayer* pPlayer = (CPlayer*)object;
			// Ż�Ϳ� ž���� ���
			if( pPlayer->GetMountedVehicle() != 0 )
			{
				CVehicle* pVehicle = (CVehicle*)g_pUserTable->FindUser( pPlayer->GetMountedVehicle() );
				if( !pVehicle || pVehicle->GetObjectKind() != eObjectKind_Vehicle )
					continue;

				// �ڽ��� �������� ���� Ż���̳� �������� ž���� ��� ������ �Ѵ�.
				if( pPlayer->GetMountedVehicle() != pPlayer->GetSummonedVehicle() ||
					pVehicle->GetMoveSpeed() <= 0.0f )
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

					PACKEDDATA_OBJ->QuickSendExceptObjectSelf( pVehicle, &msg, sizeof(msg) );
				}
			}

			CCharMove::SetPosition( object, &vecPos );

			MOVE_POS msg;
			msg.Category = MP_MOVE;
			msg.Protocol = MP_MOVE_WARP;
			msg.dwMoverID = object->GetID();
			msg.cpos.Compress( &vecPos );

			PACKEDDATA_OBJ->QuickSendExceptObjectSelf( object, &msg, sizeof(msg) );
		}
	}

	void CActionCameraShake::DoAction()
	{
		const WORD wShakeNum = WORD(GetValue(eProperty_CameraShakeNum));
		const WORD wShakeAmp = WORD(GetValue(eProperty_CameraShakeAmp));
		const WORD wShakeTimes = WORD(max(1, GetValue(eProperty_CameraShakeTimes)));

		MSG_WORD3 message;
		message.Category = MP_TRIGGER;
		message.Protocol = MP_TRIGGER_CAMERA_SHAKE_ACK;
		message.wData1 = wShakeNum;				// 1:Earthquake 2:Vibration
		message.wData2 = wShakeAmp;				// ����
		message.wData3 = wShakeTimes;			// Ƚ��

		for(DWORD objectIndex = GetHeadTarget();
			objectIndex > 0;
			objectIndex = GetNextTarget())
		{
			CObject* const object = g_pUserTable->FindUser(objectIndex);

			if(0 == object)
			{
				continue;
			}
			else if(eObjectKind_Player != object->GetObjectKind())
			{
				continue;
			}

			message.dwObjectID = objectIndex;
			PACKEDDATA_OBJ->QuickSend(
				object,
				&message,
				sizeof(message));
		}
	}
}