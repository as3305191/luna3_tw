#include "StdAfx.h"
#include "common.h"
#include "Manager.h"
#include "Trigger.h"
#include "Message.h"
#include "CheckFunc.h"
#include "Parser.h"
#include "Console.h"
#include "FlagManager.h"
#include "Timer.h"
#include "CristalEmpireBonusProcess.h"
#include "..\..\[CC]ServerModule\MHFile.h"
#include "..\Player.h"
#include "..\ChannelSystem.h"
#include "..\GuildManager.h"
#include "..\PartyManager.h"
#include "..\Guild.h"
#include "..\Party.h"
#include "..\UserTable.h"
#include "..\MapDBMsgParser.h"
#include "..\Dungeon\DungeonMgr.h"
#include "..\PackedData.h"

namespace Trigger
{
	CManager::CManager(void) :
	mMessageProceedCount(10),
	mParser(new CParser(g_pServerSystem->GetMapNum())),
	mIsHideStatistic(TRUE),
	mIsUnablePeriodicMessage(FALSE),
	mFlagManager(new CFlagManager),
	mCristalEmpireBonusProcess(new CCristalEmpireBonusProcess)
	{
		PutLog("Trigger manager is initialized");
	}

	CManager::~CManager(void)
	{
		Release();
		SAFE_DELETE(mParser);
		SAFE_DELETE(mFlagManager);
		SAFE_DELETE(mCristalEmpireBonusProcess);
	}

	CManager& CManager::GetInstance()
	{
		static CManager instance;
		return instance;
	}

	CMessage* const CManager::AllocateMessage(DWORD channelIndex)
	{
		static DWORD messageIndex;		
		CMessage& message = mMessageMap[++messageIndex];
		message.SetIndex(messageIndex);
		message.AddValue(
			eProperty_ChannelID,
			channelIndex);
		++mStatistic.mAllocatedMessageCount;

		return &message;
	}

	void CManager::UpdateDescGroup()
	{
		const CParser::TriggerDescMap& triggerDescMap = mParser->mTriggerDescMap;
		mTriggerDescByGroup.clear();

		for(CParser::TriggerDescMap::const_iterator iter = triggerDescMap.begin();
			triggerDescMap.end() != iter;
			++iter )
		{
			const TriggerDesc& desc = iter->second;
			TriggerIndexList& indexList = mTriggerDescByGroup[desc.mOwner];
			indexList.push_back(desc.mHashCode);
		}
	}

	void CManager::Release()
	{
		PutLog("All trigger is released");

		mMessageMap.clear();
		mChannelTriggerMap.clear();

		while(false == mTimerMap.empty())
		{
			TimerMap::const_iterator iter = mTimerMap.begin();
			const DWORD timerAlias = iter->first;
			const CTimer* timer = iter->second;
			SAFE_DELETE(timer);
			mTimerMap.erase(timerAlias);
		}
	}

	void CManager::Process()
	{
		// 091116 LUJ, Ÿ�̸� ó���� ���� ��ü�� �ű� �� ������ �� ��. ������ ���������� ���������� ���δ�
		for(TimerMap::iterator iter = mTimerMap.begin();
			iter != mTimerMap.end();
			)
		{
			CTimer* pTimer = iter->second;
			pTimer->Process();

			if(pTimer->GetWaitDie())
			{
				DungeonMGR->RemoveTimer(pTimer->GetChannel());
				SAFE_DELETE(pTimer);
				mTimerMap.erase(iter++);
			}
			else
			{
				++iter;
			}
		}

		int messageProceedCount = GetMessageProceedCount();

		// 091116 LUJ, Ʈ���� ó���� �׸��庰�� �и�����
		while(mMessageMap.end() != mMessageMap.begin())
		{
			const MessageMap::iterator messageIter = mMessageMap.begin();
			const CMessage& message = messageIter->second;
			const BOOL isPeriocMessage(eEvent_CheckSelf == message.GetValue(eProperty_Event));

			if(isPeriocMessage &&
				mIsUnablePeriodicMessage)
			{
				mMessageMap.erase(messageIter);
				continue;
			}

			const DWORD messageChannelIndex = message.GetValue(eProperty_ChannelID);
			const TriggerIndexSet& triggerIndexSet = mChannelTriggerMap[messageChannelIndex];

			for(TriggerIndexSet::const_iterator indexIter = triggerIndexSet.begin();
				triggerIndexSet.end() != indexIter;
				++indexIter)
			{
				const DWORD triggerIndex = *indexIter;
				CTrigger& trigger = GetTrigger(triggerIndex);

				if(eState_Stop == trigger.GetState())
				{
					mDeleteTriggerQueue.push(triggerIndex);
					continue;
				}

				++mStatistic.mProceededTriggerCount;
				trigger.Process(message);
			}

			mMessageMap.erase(messageIter);
			++mStatistic.mRemovedMessageCount;

			if(0 > --messageProceedCount)
			{
				break;
			}
		}

		// 091116 LUJ, ������ Ʈ���Ÿ� �����Ѵ�
		while(false == mDeleteTriggerQueue.empty())
		{
			const DWORD triggerIndex  = mDeleteTriggerQueue.front();
			mDeleteTriggerQueue.pop();
			CTrigger& trigger = GetTrigger(triggerIndex);

			if(0 == trigger.GetIndex())
			{
				continue;
			}

			PutLog(
				"%s(%d), trigger stopped",
				TRIGGERMGR->GetAliasName(trigger.GetHashCode()),
				triggerIndex);

			mChannelTriggerMap[trigger.GetChannel()].erase(triggerIndex);
			mTriggerMap.erase(triggerIndex);

			++mStatistic.mRemovedTriggerCount;
		}

		// ���μ��� ���߿� ������ Ʈ���Ÿ� ���� ���μ�����
		// ó���� �� �ֵ��� �����̳ʿ� �����Ѵ�
		while(false == mReservedTriggerQueue.empty())
		{
			const TriggerIndex triggerIndex = mReservedTriggerQueue.front();
			mReservedTriggerQueue.pop();

			const CTrigger& trigger = GetTrigger(triggerIndex);

			if(trigger.GetIndex())
			{
				mChannelTriggerMap[trigger.GetChannel()].insert(
					trigger.GetIndex());
			}
		}

		mCristalEmpireBonusProcess->Idle();
		UpdateDesc();
	}

	void CManager::NetworkMsgParser(DWORD dwConnectionIndex, BYTE Protocol, LPVOID pMsg, DWORD dwLength)
	{
		// 091014 LUJ, �ƹ� �ϵ� ���� �ʴ� switch���� ��� �����ϹǷ� ������
	}

	void CManager::UpdateDesc()
	{
		if(FALSE == mParser->Update())
		{
			return;
		}

		UpdateDescGroup();

		// 091116 LUJ, �������� ��� Ʈ���ŵ��� ��������� üũ�Ͽ� Ŭ���� �Һ��ߴ�.
		//			������ �Ŵ����� �ϰ������� ������Ʈ�ϵ��� �Ͽ� ���ϸ� ���ҽ��״�
		for(TriggerMap::iterator iter = mTriggerMap.begin();
			mTriggerMap.end() != iter;
			++iter)
		{
			CTrigger& trigger = iter->second;
			trigger.UpdateCondition();
		}
	}

	DWORD CManager::GetHashCode(LPCTSTR text)
	{
		return mParser->GetHashCode(text);
	}

	LPCTSTR CManager::GetAliasName(DWORD hashCode) const
	{
		return mParser->GetAliasName(hashCode);
	}

	// ���ο� Ʈ���Ÿ� �����Ѵ�
	CTrigger* CManager::CreateTrigger(eOwner ownerType, DWORD ownerIndex, DWORD dwChannelID, DWORD hashCode, DWORD repeatCount, struct tm& regTime)
	{
		const TriggerDesc& triggerDesc = GetTriggerDesc(hashCode);
		const BOOL itCreatesAlways = (0 == triggerDesc.mRepeatCount);
		const BOOL itCannotCreateByRepeatCount = (triggerDesc.mRepeatCount <= repeatCount);

		// ������ �ݺ�ȸ�� ��ŭ ����� ���
		if( ! itCreatesAlways && 
			itCannotCreateByRepeatCount)
		{
			PutLog(
				"Trigger(%s) could not create. It reached limit.",
				triggerDesc.mName);
			return 0;
		}

		static DWORD triggerIndex;
		CTrigger& trigger = mTriggerMap[++triggerIndex];

		CTrigger::Parameter parameter = {0};
		parameter.mIndex = triggerIndex;
		parameter.mOwnerType = ownerType;
		parameter.mOwnerIndex = ownerIndex;
		parameter.mChannelID = dwChannelID;
		parameter.mHashCode = hashCode;
		parameter.mRepeatCount = repeatCount;
		parameter.mRegTime = regTime;
		SafeStrCpy(
			parameter.mName,
			GetAliasName(hashCode),
			sizeof(parameter.mName) / sizeof(*parameter.mName));
		trigger.UpdateParamter(parameter);

		TCHAR textCount[MAX_PATH] = {0};
		LPCTSTR textInfinite = "infinite";
		LPCTSTR textFormat = "%d/%d";
		sprintf(
			textCount,
			itCreatesAlways ? textInfinite : textFormat,
			trigger.GetRepeatCount(),
			triggerDesc.mRepeatCount);
		PutLog(
			"Trigger(%s, %d) is created(count:%s)",
			trigger.GetName(),
			trigger.GetIndex(),
			textCount);

		// 091116 LUJ, �����ϱ� �����ϵ��� �����ֿ� ���� �з��� �����̳ʿ� �ִ´�
		TriggerIndexSet& triggerIndexSet = mOwnerCreatedTriggerMap[OwnerKey(ownerType, ownerIndex)];
		triggerIndexSet.insert(trigger.GetIndex());
		// Ʈ���Ÿ� �����ϴ� ���� Ʈ���� ���μ��� �߿� �̷�����.
		// Ʈ���� ��ȣ �����̳ʿ� ���� ������ ��� �ݺ��ڰ� ������
		// ���� ������ ���� �ʰ� �ȴ�. ���� �ӽ÷� ������ �صΰ�
		// ���μ����� �Ϸ�� �� ó�� �����̳ʿ� �̵����Ѿ��Ѵ�
		mReservedTriggerQueue.push(
			triggerIndex);
		++mStatistic.mAllocatedTriggerCount;

		return &trigger;
	}

	void CManager::LoadTrigger(CPlayer& player)
	{
		// 100712 ShinJS ������������ Load���� �ʵ��� �Ѵ�
		if( player.IsDungeonObserver() )
			return;

		// �÷��̾� Ʈ���Ŵ� �����ǵ��
		LoadTriggerPlayerFromDb(player.GetID(), 0);

		// ä�ο� ���ʷ� ���������� ����Ʈ���ŷε�
		if(1 == CHANNELSYSTEM->GetPlayerNumInChannel(player.GetGridID()))
		{
			LoadTriggerServerFromDb(player.GetGridID(), 0);
		}
	}

	CTrigger& CManager::GetTrigger(DWORD triggerIndex)
	{
		// 091123 LUJ, ���� ������ ������ �Ѱܼ� �������� ������ �־���
		//			�� ������ ������ �����Ͽ� �����ϰ� ó����
		static CTrigger emptyTrigger;
		TriggerMap::iterator iter = mTriggerMap.find(triggerIndex);

		return mTriggerMap.end() == iter ? emptyTrigger : iter->second;
	}

	BOOL CManager::IsNoOwner(eOwner ownerType, DWORD ownerIndex)
	{
		switch(ownerType)
		{
		case eOwner_Server:
			{
				if(0 < CHANNELSYSTEM->GetPlayerNumInChannel(ownerIndex))
				{
					return FALSE;
				}

				break;
			}
		case eOwner_Player:
			{
				if(g_pUserTable->FindUser(ownerIndex))
				{
					return FALSE;
				}

				break;
			}
		}

		return TRUE;
	}

	DWORD CManager::GetChannelIndex(eOwner ownerType, DWORD ownerIndex)
	{
		switch(ownerType)
		{
		case eOwner_Server:
			{
				return ownerIndex;
			}
		case eOwner_Player:
			{
				CObject* const object = g_pUserTable->FindUser(ownerIndex);

				if(object)
				{
					return object->GetGridID();
				}

				break;
			}
		}

		return UINT_MAX;
	}

	void CManager::CreateDefaultTrigger(eOwner ownerType, DWORD ownerIndex, DWORD channelIndex)
	{
		const OwnerKey ownerKey(ownerType, ownerIndex);
		const TriggerIndexSet& triggerIndexSet = mOwnerCreatedTriggerMap[ownerKey];
		// ������ Ʈ������ ��ũ��Ʈ �ε����� �����صд�. ĳ��ó�� ����ؼ� ��˻��� �ִ��� ���Ѵ�
		std::set< DWORD > createdTriggerDescSet;

		// ������ Ʈ������ ��ũ��Ʈ ��ȣ�� �ִ´�
		for(TriggerIndexSet::const_iterator triggerIter = triggerIndexSet.begin();
			triggerIndexSet.end() != triggerIter;
			++triggerIter)
		{
			const DWORD triggerIndex = *triggerIter;
			const CTrigger& trigger = GetTrigger(triggerIndex);

			if(0 == trigger.GetIndex())
			{
				continue;
			}

			createdTriggerDescSet.insert(triggerIndex);
		}

		const TriggerIndexList& triggerIndexList = mTriggerDescByGroup[ownerType];

		__time64_t time = 0;
		_time64(&time);
		struct tm currentTime = *_localtime64(&time);

		// 091016 LUJ, �⺻ Ʈ���Ÿ� �����Ѵ�
		for(TriggerIndexList::const_iterator iter = triggerIndexList.begin();
			triggerIndexList.end() != iter;
			++iter)
		{
			const DWORD hashCode = *iter;
			const BOOL isAlreadyCreated(createdTriggerDescSet.end() != createdTriggerDescSet.find(hashCode));

			if(isAlreadyCreated)
			{
				continue;
			}

			CreateTrigger(
				ownerType,
				ownerIndex,
				channelIndex,
				hashCode,
				0,
				currentTime);
		}
	}

	void CManager::CreateTrigger(eOwner ownerType, LPDBMESSAGE dbMessage, LPQUERY query)
	{
		const DWORD ownerIndex = dbMessage->dwID;

		if(IsNoOwner(ownerType, ownerIndex))
		{
			return;
		}

		const DWORD channelIndex = GetChannelIndex(ownerType, ownerIndex);

		// 091016 LUJ, Ʈ���Ÿ� �����Ѵ�
		for(DWORD i = 0; i < dbMessage->dwResult; ++i)
		{
			const QUERYST& record = query[i];
			const DWORD hashCode = atoi(LPTSTR(record.Data[0]));
			const DWORD repeatCount = atoi(LPTSTR(record.Data[1]));
			const TriggerDesc& desc = GetTriggerDesc(hashCode);

			if(FALSE == desc.mAutoStart)
			{
				continue;
			}

			// MSDN 2005 �̻󿡼��� ����(00:00:00)���� ���� ������ ����(09.10.28) �׽�Ʈ(.Net) ��� 09:00:00 ����.
			__time64_t time = _atoi64( (LPCTSTR)record.Data[2] );
			struct tm regTime = *_localtime64(&time);

			CreateTrigger(
				ownerType,
				ownerIndex,
				channelIndex,
				hashCode,
				repeatCount,
				regTime);
		}

		// 091016 LUJ, �о���� Ʈ���Ű� ���� ��� �� �д´�
		if(MAX_ROW_NUM == dbMessage->dwResult)
		{
			const QUERYST& record = query[MAX_ROW_NUM - 1];
			const DWORD startindex = atoi(LPTSTR(record.Data[0]));

			switch(ownerType)
			{
			case eOwner_Player:
				LoadTriggerPlayerFromDb(ownerIndex, startindex);
				break;
			case eOwner_Server:
				LoadTriggerServerFromDb(ownerIndex, startindex);
				break;
			}
		}
		// 091016 LUJ, ���� ��� �⺻ Ʈ���Ÿ� �����Ѵ�
		else
		{
			CreateDefaultTrigger(ownerType, ownerIndex, channelIndex);
		}
	}

	const TriggerDesc& CManager::GetTriggerDesc(DWORD hashCode) const
	{
		return mParser->GetTriggerDesc(hashCode);
	}

	const ConditionDesc& CManager::GetConditionDesc(DWORD conditionDescIndex) const
	{
		return mParser->GetConditionDesc(conditionDescIndex);
	}

	const ActionDesc& CManager::GetActionDesc(DWORD actionDescIndex) const
	{
		return mParser->GetActionDesc(actionDescIndex);
	}

	int CManager::GetMessageProceedCount()
	{
		static DWORD lastTick;

		if(gCurTime < lastTick)
		{
			return mMessageProceedCount;
		}

		const DWORD tickStep = 1000;
		lastTick = gCurTime + tickStep;
		mMessageProceedCount = mMessageMap.size() / 2;

		ShowStatistic();

		// ��ȯ�Ǵ� ���� �ּ� 1�̾�� �Ѵ�. �׷��� ������ Ʈ���Ű� ������ �޽����� �����ȴ�
		return mMessageProceedCount;
	}

	void CManager::RestoreRepeatCount(DWORD triggerIndex, DWORD repeatCount, const struct tm& regDate)
	{
		const TriggerMap::iterator iter = mTriggerMap.find(triggerIndex);

		if(mTriggerMap.end() == iter)
		{
			return;
		}

		CTrigger& trigger = iter->second;
		trigger.RestoreRepeatCount(repeatCount);
		trigger.SetRegTime(regDate);
	}

	void CManager::StopServerTrigger(DWORD channelIndex)
	{
		StopTrigger(eOwner_Server, channelIndex);
	}

	void CManager::StopObjectTrigger(EObjectKind objectKind, DWORD objectIndex)
	{
		switch(objectKind)
		{
		case eObjectKind_Player:
			{
				StopTrigger(eOwner_Player, objectIndex);
				break;
			}
		case eObjectKind_Monster:
		case eObjectKind_BossMonster:
		case eObjectKind_FieldBossMonster:
		case eObjectKind_FieldSubMonster:
			{
				StopTrigger(eOwner_Monster, objectIndex);
				break;
			}
		}

		mCristalEmpireBonusProcess->Remove(
			objectIndex);
	}

	void CManager::StopTrigger(eOwner ownerType, DWORD ownerIndex)
	{
		const OwnerKey ownerKey(ownerType, ownerIndex);
		const TriggerIndexSet& triggerIndexSet = mOwnerCreatedTriggerMap[ownerKey];

		for(TriggerIndexSet::const_iterator iter = triggerIndexSet.begin();
			triggerIndexSet.end() != iter;
			++iter)
		{
			const DWORD triggerIndex = *iter;
			CTrigger& trigger = GetTrigger(triggerIndex);
			trigger.SetState(eState_Stop);
			mDeleteTriggerQueue.push(trigger.GetIndex());
		}

		mOwnerCreatedTriggerMap.erase(ownerKey);
		mFlagManager->RemoveFlag(
			ownerType,
			ownerIndex);

		if(ownerType == eOwner_Server)
		{
			// ����Ʈ���Ű� ����Ǹ� �ش� ä�ο� ��ϵ� Ÿ�̸Ӹ� ������Ų�� - �����Ͽ� �Ҹ�ȴ�.
			for(TimerMap::iterator iter = mTimerMap.begin();
				iter != mTimerMap.end();
				iter++)
			{
				CTimer* pTimer = iter->second;

				if(pTimer->GetChannel() == ownerIndex)
					pTimer->SetState(eTimerState_Stop);
			}
		}
	}

	CheckFunc CManager::GetCheckFunc(eProperty property) const
	{
		return mParser->GetCheckFunc(property);
	}

	CAction* CManager::CreateAction(eAction action) const
	{
		return mParser->CreateAction(action);
	}

	void CManager::ShowStatistic()
	{
		if(mIsHideStatistic)
		{
			return;
		}

		TCHAR textAllocatedTrigger[MAX_PATH] = {0};
		_stprintf(
			textAllocatedTrigger,
			mStatistic.mAllocatedTriggerCount ? " +%u" : "",
			mStatistic.mAllocatedTriggerCount);
		TCHAR textRemoveTrigger[MAX_PATH] = {0};
		_stprintf(
			textRemoveTrigger,
			mStatistic.mRemovedTriggerCount ? " -%u" : "",
			mStatistic.mRemovedTriggerCount);
		TCHAR textAllocatedMessage[MAX_PATH] = {0};
		_stprintf(
			textAllocatedMessage,
			mStatistic.mAllocatedMessageCount ? " +%u" : "",
			mStatistic.mAllocatedMessageCount);
		TCHAR textRemovedMessage[MAX_PATH] = {0};
		_stprintf(
			textRemovedMessage,
			mStatistic.mRemovedMessageCount ? " -%u" : "",
			mStatistic.mRemovedMessageCount);

		PutLog(
			"Rate(msg/trg:%u): trigger(%u%s%s) %0.1f, message(%u%s%s) %0.1f",
			mMessageProceedCount,
			mTriggerMap.size(),
			textAllocatedTrigger,
			textRemoveTrigger,
			mTriggerMap.empty() ? 0 : float(mStatistic.mProceededTriggerCount) / mTriggerMap.size(),
			mMessageMap.size(),
			textAllocatedMessage,
			textRemovedMessage,
			mMessageMap.empty() ? 0 : float(mStatistic.mProceededMessageCount) / mMessageMap.size());		
		mStatistic = Statistic();
	}

	void CManager::ToggleStatistic()
	{
		mIsHideStatistic = (!mIsHideStatistic);

		PutLog(
			"Trigger statistic is %s",
			mIsHideStatistic ? "off" : "on");
	}

	void CManager::TogglePeriodicMessage()
	{
		mIsUnablePeriodicMessage = (!mIsUnablePeriodicMessage);

		PutLog(
			"Trigger manager'll be %s periodic message from CObject.",
			mIsUnablePeriodicMessage ? "not proceed" : "proceed");
	}

	void CManager::SetFlag(eOwner ownerType, DWORD ownerIndex, DWORD alias, int value)
	{
		mFlagManager->SetFlag(
			ownerType,
			ownerIndex,
			alias,
			value);
		PutLog(
			"flag(owner: %u, index: %u): %s = %d",
			ownerType,
			ownerIndex,
			GetAliasName(alias),
			value);
	}

	int CManager::GetFlag(eOwner ownerType, DWORD ownerIndex, DWORD alias) const
	{
		return mFlagManager->GetFlag(
			ownerType,
			ownerIndex,
			alias);
	}

	bool CManager::IsFlag(eOwner ownerType, DWORD ownerIndex, DWORD alias) const
	{
		return mFlagManager->IsExist(
			ownerType,
			ownerIndex,
			alias);
	}

	void CManager::StartTimer(DWORD alias, DWORD dwChannel, eTimerType type, DWORD dwDuration)
	{
		CTimer* pTimer = new CTimer;
		pTimer->Init(alias, dwChannel, type, dwDuration);

		mTimerMap[alias] = pTimer;

		if(DungeonMGR->IsDungeon(g_pServerSystem->GetMapNum()))
		{
			// �ش� ������ Ÿ�̸������� ������ �˷��ش�.
			DungeonMGR->AddTimer(dwChannel, alias);
		}
	}

	BOOL CManager::SetTimerState(DWORD alias, eTimerState state)
	{
		TimerMap::iterator iter;
		iter = mTimerMap.find(alias);
		if(iter!=mTimerMap.end() && iter->second)
		{
			return iter->second->SetState(state);
		}

		return FALSE;
	}

	CTimer* CManager::GetTimer(DWORD alias)
	{
		const TimerMap::iterator iter = mTimerMap.find(alias);

		return mTimerMap.end() == iter ? 0 : iter->second;
	}

	void CManager::NetworkMsgParser(DWORD connectionIndex, LPSTR packet, DWORD length)
	{
		const MSGBASE* const baseMessage = (MSGBASE*)packet;

		switch(baseMessage->Protocol)
		{
		case MP_TRIGGER_SET_FLAG_SYN:
			{
				const MSG_DWORD4* receivedMessage = (MSG_DWORD4*)baseMessage;
				const DWORD hashCode = receivedMessage->dwData1;
				const DWORD value = receivedMessage->dwData2;
				const eOwner ownerType = eOwner(receivedMessage->dwData3);
				const DWORD ownerIndex = receivedMessage->dwData4;

				TCHAR textDebug[MAX_PATH] = {0};
				_stprintf(
					textDebug,
					"hash code: %u, %s\n",
					hashCode,
					TRIGGERMGR->GetAliasName(hashCode));
				OutputDebugString(textDebug);

				TRIGGERMGR->SetFlag(
					ownerType,
					ownerIndex,
					hashCode,
					value);
				break;
			}
		case MP_TRIGGER_ITEM_OPTION_APPLY_SYN:
			{
				const MSG_ITEM_MIX_ACK* receivedMessage = (MSG_ITEM_MIX_ACK*)baseMessage;
				const DWORD playerIndex = receivedMessage->dwObjectID;
				const size_t maxSize = _countof(receivedMessage->mUpdateResult.mItem);

				MSG_ITEM_OPTION resultMessage;
				ZeroMemory(
					&resultMessage,
					sizeof(resultMessage));
				resultMessage.Category = MP_TRIGGER;
				resultMessage.Protocol = MP_TRIGGER_ITEM_OPTION_APPLY_ACK;

				for(WORD i = WORD(min(maxSize, receivedMessage->mUpdateResult.mSize));
					0 < i--;)
				{
					const ITEMBASE& itemBase = receivedMessage->mUpdateResult.mItem[i];

					CManager::GetInstance().mCristalEmpireBonusProcess->Run(
						playerIndex,
						itemBase.wIconIdx,
						itemBase.dwDBIdx,
						itemBase.Position,
						resultMessage.mOption[resultMessage.mSize++]);
				}

				CManager::GetInstance().mCristalEmpireBonusProcess->Remove(
					playerIndex);

				CObject* const object = g_pUserTable->FindUser(
					playerIndex);

				if(object)
				{
					object->SendMsg(
						&resultMessage,
						resultMessage.GetSize());
				}

				MSG_DWORD effeectMessage;
				ZeroMemory(
					&effeectMessage,
					sizeof(effeectMessage));
				effeectMessage.Category = MP_ITEM;
				effeectMessage.Protocol = MP_ITEM_WORKING_SUCCESS;
				effeectMessage.dwData  = playerIndex;
				PACKEDDATA_OBJ->QuickSend(
					object,
					&effeectMessage,
					sizeof(effeectMessage));
				break;
			}
		}
	}

	void CManager::BatchItemOption(DWORD playerIndex, DWORD itemIndex, ITEM_OPTION::Drop::Key key, float power, LEVELTYPE level, size_t size)
	{
		mCristalEmpireBonusProcess->Batch(
			playerIndex,
			itemIndex,
			key,
			power,
			level,
			size);
	}
}
