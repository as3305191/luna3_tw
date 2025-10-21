#include "StdAfx.h"
#include "common.h"
#include "ActionCancelQuest.h"
#include "..\UserTable.h"
#include "..\Player.h"
#include "..\QuestManager.h"

namespace Trigger
{
	void CActionCancelQuest::DoAction()
	{
		const DWORD mainQuestIndex = GetValue(eProperty_Quest);

		SetHeadTarget();

		for(DWORD objectIndex = GetNextTarget();
			0 < objectIndex;
			objectIndex = GetNextTarget())
		{
			CPlayer* const player = (CPlayer*)g_pUserTable->FindUser(objectIndex);

			if(0 == player)
			{
				PutLog("Player is not found");
				continue;
			}
			else if(eObjectKind_Player != player->GetObjectKind())
			{
				PutLog("Target is not player");
				continue;
			}

			// 091019 LUJ, ���� ����Ʈ ��ȣ�� ���� ��� �׻� �����ȴ�
			QUESTMGR->EndQuest(
				player,
				mainQuestIndex,
				1);
		}
	}
}