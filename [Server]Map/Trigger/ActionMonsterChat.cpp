#include "StdAfx.h"
#include "ActionMonsterChat.h"
#include "common.h"
#include "..\Object.h"
#include "..\UserTable.h"
#include "..\PackedData.h"

namespace Trigger
{
	void CActionMonsterChat::DoAction()
	{
		// MSG_DWORD4 message;
		// ZeroMemory(&message, sizeof(message));
		// message.Category = MP_CHAT;
		// message.Protocol = MP_CHAT_MONSTERSPEECH;
		// // 0: ��ǳ���� ǥ��, 1: ��ǳ�� + ä��â ǥ��.
		// // 1������ ������Ű�� �߰� �䱸�� ������ Ȯ���Ѵ�
		// message.dwData1 = 1;
		// // ���� ��ȭ ��ȣ
		// message.dwData2 = GetValue(eProperty_MonsterSpeech);

		// for(DWORD objectIndex = GetHeadTarget();
		// 	objectIndex > 0;
		// 	objectIndex = GetNextTarget())
		// {
		// 	CObject* const object = g_pUserTable->FindUser(objectIndex);

		// 	if(0 == object)
		// 	{
		// 		continue;
		// 	}
		// 	else if(FALSE == (object->GetObjectKind() & eObjectKind_Monster))
		// 	{
		// 		continue;
		// 	}

		// 	message.dwObjectID = objectIndex;
		// 	PACKEDDATA_OBJ->QuickSend(
		// 		object,
		// 		&message,
		// 		sizeof(message));
		// }
	}
}