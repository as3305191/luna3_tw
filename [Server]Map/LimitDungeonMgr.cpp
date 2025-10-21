#include "StdAfx.h"
#include <sys/stat.h>
#include "LimitDungeonMgr.h"
#include "AIGroupManager.h"
#include "RegenManager.h"
#include "UserTable.h"
#include "AISystem.h"
#include "Player.h"
#include "Monster.h"
#include "LootingManager.h"
#include "ChannelSystem.h"
#include "ItemManager.h"
#include "MapDBMsgParser.h"
#include "..\[CC]ServerModule\Network.h"
#include "..\[CC]Header\GameResourceManager.h"

CLimitDungeonManager::CLimitDungeonManager() :
mMapType(MAPTYPE(g_pServerSystem->GetMapNum()))
{
	ZeroMemory(&mFileStatus, sizeof(mFileStatus));

	// 091106 LUJ, ũ�Ⱑ 0���� �˻縦 ���ϱ� ���� ���ǹ��� ���� �ִ´�
	const Event emptyEvent = {Event::TypeNone, 0, 0, 0};
	__time64_t todayTime = 0;
	_time64(&todayTime);
	mScheduleMap.insert(std::make_pair(todayTime, emptyEvent));
}

CLimitDungeonManager::~CLimitDungeonManager(void)
{}

CLimitDungeonManager* CLimitDungeonManager::GetInstance()
{
	static CLimitDungeonManager instance ;

	return &instance;
}

CLimitDungeonManager::Room& CLimitDungeonManager::GetRoom(MAPTYPE mapType, DWORD channelIndex)
{
	const RoomKey key(mapType, channelIndex);
	const RoomMap::iterator iter = mRoomMap.find(key);

	if(mRoomMap.end() == iter)
	{
		static Room emptyRoom;
		emptyRoom.mCloseTime = 0;
		emptyRoom.mPlayerIndexSet.clear();

		return emptyRoom;
	}

	return iter->second;
}

void CLimitDungeonManager::AddPlayer(CPlayer& player)
{
	const LimitDungeonScript& script = g_CGameResourceManager.GetLimitDungeonScript(
		mMapType,
		player.GetChannelID());

	if(0 == script.mMapType)
	{
		return;
	}

	Room& room = GetRoom(
		script.mMapType,
		script.mChannel);
	room.mPlayerIndexSet.insert(
		player.GetID());
	mPlayerQueue.push(
		player.GetID());

	// 091106 LUJ, ���ư� ���� �̸� �����س��´�
	const LimitDungeonScript::Home& home = script.mHome;
	player.SetMapMoveInfo(
		home.mMapType,
		DWORD(home.mPosition.x),
		DWORD(home.mPosition.z));
	__time64_t todayTime = 0;
	_time64(&todayTime);
	MSG_DWORDEX2 message;
	ZeroMemory(&message, sizeof(message));
	message.Category = MP_LIMITDUNGEON;
	message.Protocol = MP_LIMITDUNGEON_JOIN_ACK;
	message.dweData1 = DWORDEX(g_pServerSystem->GetMonsterCount(script.mChannel));
	message.dweData2 = room.mCloseTime - todayTime;
	player.SendMsg(
		&message,
		sizeof(message));

	TCHAR text[MAX_PATH] = {0};
	_sntprintf(
		text,
		_countof(text),
		"%map:%d(%d)",
		script.mMapType,
		script.mChannel);
	LogItemMoney(
		player.GetID(),
		player.GetObjectName(),
		0,
		text,
		eLog_LimitDungeonBegin,
		player.GetMoney(),
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0);
}

void CLimitDungeonManager::UpdateSchedule()
{
	mRoomMap.clear();
	mScheduleMap.clear();

	__time64_t todayTime = 0;
	_time64(&todayTime);
	const struct tm todayDate = *_localtime64(&todayTime);
	const Event emptyEvent = {Event::TypeNone, 0, 0, 0};
	mScheduleMap.insert(
		std::make_pair(todayTime, emptyEvent));

	typedef CGameResourceManager::LimitDungeonScriptMap ScriptMap;
	const ScriptMap& scriptMap = g_CGameResourceManager.GetLimitDungeonScriptMap();

	// 091106 LUJ, ������ �����Ѵ�
	for(ScriptMap::const_iterator scriptIter = scriptMap.begin();
		scriptMap.end() != scriptIter;
		++scriptIter)
	{
		const LimitDungeonScript& script = scriptIter->second;
		const BOOL isNoHomeMap(script.mHome.mMapType != mMapType);
		const BOOL isNoPlayMap(script.mMapType != mMapType);

		if(isNoPlayMap && isNoHomeMap)
		{
			continue;
		}

		// 091106 LUJ, ������ ����Ѵ�
		for(LimitDungeonScript::DateList::const_iterator dateIter = script.mDateList.begin();
			script.mDateList.end() != dateIter;
			++dateIter)
		{
			const LimitDungeonScript::Date& date = *dateIter;
			const WORD dayOnWeek = 7;
			// 091106 LUJ: ���� ���ϰ� �� ���� ���̿� ��ĥ �������� ��Ÿ���� ��� ���̺�
			//			��: ���� ����(��~��), ��: �� ����(��~��)
			const int howManyDayBetweenDayOfWeek[][dayOnWeek] =
			{
				{0, 1, 2, 3, 4, 5, 6},
				{6, 0, 1, 2, 3, 4, 5},
				{5, 6, 0, 1, 2, 3, 4},
				{4, 5, 6, 0, 1, 2, 3},
				{3, 4, 5, 6, 0, 1, 2},
				{2, 3, 4, 5, 6, 0, 1},
				{1, 2, 3, 4, 5, 6, 0},
			};

			// 091106 LUJ, �������� ���۵� �ð��� ��´�
			struct tm futureDate(todayDate);
			const int howManyDay = howManyDayBetweenDayOfWeek[todayDate.tm_wday][date.mDayOfWeek];
			futureDate.tm_mday = futureDate.tm_mday + howManyDay;
			mktime(&futureDate);
			futureDate.tm_hour = date.mStartHour;
			futureDate.tm_min = date.mStartmMinute;
			futureDate.tm_sec = 0;

			__time64_t openTime = _mktime64(&futureDate);
			const BOOL isPassed(openTime < todayTime);

			if(isPassed)
			{
				const __time64_t remainTime = openTime + date.mPlayTime - todayTime;

				// 091106 LUJ, �ð��� ��������, ���� ������ ��� ��� ����.
				//			���������� ����̹Ƿ� ���� ������ ���� �ʴ´�
				if(0 < remainTime)
				{
					const Event openEvent = {
						Event::TypeOpenIlregulary,
						script.mMapType,
						script.mChannel,
						remainTime};
					CloseRoom(
						openEvent);
					OpenRoom(
						openEvent);

					const Event closeEvent = {
						Event::TypeCloseIlregulary,
						script.mMapType,
						script.mChannel,
						0};
					mScheduleMap.insert(
						std::make_pair(todayTime + remainTime, closeEvent));
				}

				openTime = openTime + GetSecondForOneWeek();
			}

			const Event openEvent = {
				Event::TypeOpen,
				script.mMapType,
				script.mChannel,
				date.mPlayTime};
			const Event closeEvent = {
				Event::TypeClose,
				script.mMapType,
				script.mChannel,
				0};
			const __time64_t closeTime = openTime + date.mPlayTime;

			mScheduleMap.insert(
				std::make_pair(openTime, openEvent));
			mScheduleMap.insert(
				std::make_pair(closeTime, closeEvent));
		}
	}
}

__time64_t CLimitDungeonManager::GetSecondForOneWeek() const
{
	const __time64_t oneHour(60 * 60);
	const __time64_t oneDay(oneHour * 24);
	return oneDay * 7;
}

void CLimitDungeonManager::NetworkMsgParser(MSGBASE* receivedMessage) 
{
	switch(receivedMessage->Protocol)
	{
	case MP_LIMITDUNGEON_JOIN_SYN :
		{
			const MSG_DWORD3* const message = (MSG_DWORD3*)receivedMessage;
			const DWORD playerIndex = message->dwObjectID;
			const MAPTYPE mapType = MAPTYPE(message->dwData1);
			const DWORD channelIndex = message->dwData2;
			const POSTYPE itemPosition = POSTYPE(message->dwData3);
			AskJoin(
				playerIndex,
				mapType,
				channelIndex,
				itemPosition);
			break ;
		}
	}
}

BOOL CLimitDungeonManager::IsReady() const
{
	__time64_t todayTime = 0;
	_time64(&todayTime);

	const ScheduleMap::const_iterator iter = mScheduleMap.begin();
	const __time64_t startSecond = iter->first;

	return todayTime > startSecond;
}

void CLimitDungeonManager::PostponeSchedule()
{
	const ScheduleMap::iterator iterator = mScheduleMap.begin();
	const __time64_t second = iterator->first;
	const Event event = iterator->second;
	mScheduleMap.erase(iterator);

	// 091106 LUJ, �Ͻ��� �̺�Ʈ�� �����Ѵ�
	if(Event::TypeOpenIlregulary == event.mType)
	{
		return;
	}
	else if(Event::TypeCloseIlregulary == event.mType)
	{
		return;
	}

	mScheduleMap.insert(
		std::make_pair(second + GetSecondForOneWeek(), event));
}

void CLimitDungeonManager::Run()
{
	const ScheduleMap::const_iterator scheduleIterator = mScheduleMap.begin();
	const Event& event = scheduleIterator->second;

	switch(event.mType)
	{
	case Event::TypeOpen:
	case Event::TypeOpenIlregulary:
		{
			OpenRoom(event);
			break;
		}
	case Event::TypeClose:
	case Event::TypeCloseIlregulary:
		{
			CloseRoom(event);
			break;
		}
	}
}

void CLimitDungeonManager::OpenRoom(const CLimitDungeonManager::Event& event)
{
	const LimitDungeonScript& script = g_CGameResourceManager.GetLimitDungeonScript(
		event.mMapType,
		event.mChannelIndex);
	const RoomKey roomKey(event.mMapType, event.mChannelIndex);
	const RoomMap::const_iterator roomIterator = mRoomMap.find(roomKey);
	const BOOL isAlreadyPlay(mRoomMap.end() != roomIterator);

	if(isAlreadyPlay)
	{
		return;
	}

	__time64_t todayTime = 0;
	_time64(&todayTime);
	Room& room = mRoomMap[roomKey];
	room.mCloseTime = todayTime + event.mPlayTime;
	
	if(mMapType == script.mMapType)
	{
		g_pAISystem.Load(
			script.mMonsterScript);
		g_pAISystem.Summon(
			script.mChannel);
	}
}

void CLimitDungeonManager::CloseRoom(const CLimitDungeonManager::Event& event)
{
	mRoomMap.erase(
		RoomKey(event.mMapType, event.mChannelIndex));

	const LimitDungeonScript& script = g_CGameResourceManager.GetLimitDungeonScript(
		event.mMapType,
		event.mChannelIndex);

	// 100104 LUJ, �˻縦 ���ϸ� �Ա��� �ִ� �� ���� ���� ����ó�� �۵��Ѵ�
	if(mMapType == script.mMapType)
	{
		g_pServerSystem->RemoveMonsterInGrid(event.mChannelIndex);
	}
}

void CLimitDungeonManager::RefreshScript()
{
	LPCTSTR filePath = "system\\resource\\LimitDungeon.bin";
	struct stat fileStatus = {0};

	if(stat(filePath, &fileStatus))
	{
		return;
	}
	else if(0 == memcmp(&fileStatus, &mFileStatus, sizeof(fileStatus)))
	{
		return;
	}

	mFileStatus	= fileStatus;
	g_CGameResourceManager.LoadLimitDungeonScript();
	UpdateSchedule();
}

void CLimitDungeonManager::Process()
{
	if(IsReady())
	{
        Run();
		PostponeSchedule();
		return;
	}

	RefreshScript();
	UpdatePlayer();
}

void CLimitDungeonManager::UpdatePlayer()
{
	if(mPlayerQueue.empty())
	{
		return;
	}

	const DWORD playerIndex = mPlayerQueue.front();
	mPlayerQueue.pop();

	CPlayer* const player = (CPlayer*)g_pUserTable->FindUser(playerIndex);

	if(0 == player)
	{
		return;
	}
	else if(eObjectKind_Player != player->GetObjectKind())
	{
		return;
	}

	const Room& room = GetRoom(mMapType, player->GetChannelID());
	const BOOL isClosedRoom(0 == room.mCloseTime);

	if(isClosedRoom)
	{
		const LimitDungeonScript& script = g_CGameResourceManager.GetLimitDungeonScript(
			mMapType,
			player->GetChannelID());				
		player->UpdateLogoutToDB(FALSE);

		MSG_WORD2 message;
		ZeroMemory(&message, sizeof(message));
		message.Category = MP_USERCONN;
		message.Protocol = MP_USERCONN_MAP_OUT_WITHMAPNUM;
		message.dwObjectID = playerIndex;
		message.wData1 = script.mHome.mMapType;
		message.wData2 = WORD(-1);
		g_Network.Broadcast2AgentServer(
			LPTSTR(&message),
			sizeof(message));
		g_pServerSystem->RemovePlayer(playerIndex);
		return;
	}

	// 091106 LUJ, �ٽ� ���������� ������Ʈ�� �� �ֵ��� ť�� �־�д�
	mPlayerQueue.push(playerIndex);

	MSG_DWORD message;
	ZeroMemory(&message, sizeof(message));
	message.Category = MP_LIMITDUNGEON;
	message.Protocol = MP_LIMITDUNGEON_SET_STATUS_ACK;
	message.dwData = g_pServerSystem->GetMonsterCount(player->GetChannelID());
	player->SendMsg(
		&message,
		sizeof(message));
}

BOOL CLimitDungeonManager::IsNoRoom(MAPTYPE mapType, DWORD channelIndex)
{
	const Room& room = GetRoom(mapType, channelIndex);

	return 0 == room.mCloseTime;
}

void CLimitDungeonManager::AskJoin(DWORD playerIndex, MAPTYPE mapType, DWORD channelIndex, POSTYPE itemPosition)
{
	const LimitDungeonScript& script = g_CGameResourceManager.GetLimitDungeonScript(
		mapType,
		channelIndex);
	CPlayer* const player = (CPlayer*)g_pUserTable->FindUser(playerIndex);

	if(0 == player)
	{
		return;
	}
	// 091106 LUJ, ����� �� ���� �������� �õ���
	else if(script.mHome.mMapType != mMapType)
	{		
		MSG_LIMITDUNGEON_ERROR message;
		ZeroMemory(&message, sizeof(message));
		message.Category = MP_LIMITDUNGEON;
		message.Protocol = MP_LIMITDUNGEON_JOIN_NACK;
		message.mError = MSG_LIMITDUNGEON_ERROR::ErrorMapMove;
		player->SendMsg(
			&message,
			sizeof(message));
		return;
	}
	else if(eEXITCODE_OK != player->CanExitStart())
	{
		MSG_LIMITDUNGEON_ERROR message;
		ZeroMemory(&message, sizeof(message));
		message.Category = MP_LIMITDUNGEON;
		message.Protocol = MP_LIMITDUNGEON_JOIN_NACK;
		message.mError = MSG_LIMITDUNGEON_ERROR::ErrorMapMove;
		player->SendMsg(
			&message,
			sizeof(message));
		return;
	}
	else if(eObjectState_Die == player->GetState())
	{
		MSG_LIMITDUNGEON_ERROR message;
		ZeroMemory(&message, sizeof(message));
		message.Category = MP_LIMITDUNGEON;
		message.Protocol = MP_LIMITDUNGEON_JOIN_NACK;
		message.mError = MSG_LIMITDUNGEON_ERROR::ErrorMapMove;
		player->SendMsg(
			&message,
			sizeof(message));
		return;
	}
	else if(script.mLevel.mMin > player->GetLevel())
	{
		MSG_LIMITDUNGEON_ERROR message;
		ZeroMemory(&message, sizeof(message));
		message.Category = MP_LIMITDUNGEON;
		message.Protocol = MP_LIMITDUNGEON_JOIN_NACK;
		message.mError = MSG_LIMITDUNGEON_ERROR::ErrorLevel;
		player->SendMsg(
			&message,
			sizeof(message));
		return;
	}
	else if(script.mLevel.mMax < player->GetLevel())
	{
		MSG_LIMITDUNGEON_ERROR message;
		ZeroMemory(&message, sizeof(message));
		message.Category = MP_LIMITDUNGEON;
		message.Protocol = MP_LIMITDUNGEON_JOIN_NACK;
		message.mError = MSG_LIMITDUNGEON_ERROR::ErrorLevel;
		player->SendMsg(
			&message,
			sizeof(message));
		return;
	}
	else if(IsNoRoom(mapType, channelIndex))
	{
		MSG_LIMITDUNGEON_ERROR message;
		ZeroMemory(&message, sizeof(message));
		message.Category = MP_LIMITDUNGEON;
		message.Protocol = MP_LIMITDUNGEON_JOIN_NACK;
		message.mError = MSG_LIMITDUNGEON_ERROR::ErrorRoom;
		player->SendMsg(
			&message,
			sizeof(message));
		return;
	}
	else if(FALSE == UseItem(script, *player, itemPosition))
	{
		MSG_LIMITDUNGEON_ERROR message;
		ZeroMemory(&message, sizeof(message));
		message.Category = MP_LIMITDUNGEON;
		message.Protocol = MP_LIMITDUNGEON_JOIN_NACK;
		message.mError = MSG_LIMITDUNGEON_ERROR::ErrorItem;
		player->SendMsg(
			&message,
			sizeof(message));
		return;
	}

	// 091106 LUJ, ������Ʈ�� ä���� Ŭ���̾�Ʈ ������ ������ �÷��׸� �����Ѵ�
	{
		MSGBASE message;
		ZeroMemory(&message, sizeof(message));
		message.Category = MP_USERCONN;
		message.Protocol = MP_USERCONN_FORCED_CHANNEL_SYN;
		player->SendMsg(
			&message,
			sizeof(message));
	}

	// 091106 LUJ, �� �̵� ������ �����Ѵ�
	{
		const MAPCHANGE_INFO* const mapChangeInfo = g_CGameResourceManager.GetMapChangeInfoFromToMapNum(
			mMapType,
			mapType);

		if(mapChangeInfo)
		{
			player->SetMapMoveInfo(
				mapType,
				DWORD(mapChangeInfo->MovePoint.x),
				DWORD(mapChangeInfo->MovePoint.z));
		}
	}

	// 091106 LUJ, �� �̵���Ų��
	{
		MSG_WORD2 message;
		ZeroMemory(&message, sizeof(message));
		message.Category = MP_USERCONN;
		message.Protocol = MP_USERCONN_MAP_OUT_WITHMAPNUM;
		message.dwObjectID = playerIndex;
		message.wData1 = mapType;
		message.wData2 = WORD(channelIndex - 1);
		g_Network.Broadcast2AgentServer(
			LPTSTR(&message),
			sizeof(message));
		player->UpdateLogoutToDB(FALSE);
	}

	g_pServerSystem->RemovePlayer(playerIndex);
}

BOOL CLimitDungeonManager::UseItem(const LimitDungeonScript& script, CPlayer& player, POSTYPE itemPosition)
{
	if(0 == script.mItem.mItemIndex)
	{
		return TRUE;
	}

	CItemSlot* const itemSlot = player.GetSlot(eItemTable_Inventory);

	if(0 == itemSlot)
	{
		return FALSE;
	}

	const ITEMBASE* const itemBase = itemSlot->GetItemInfoAbs(itemPosition);

	if(0 == itemBase)
	{
		return FALSE;
	}
	else if(script.mItem.mItemIndex != itemBase->wIconIdx)
	{
		return FALSE;
	}
	else if(script.mItem.mQuantity > itemBase->Durability)
	{
		return FALSE;
	}

	const ITEMBASE itemBaseForLog(*itemBase);

	if(0 != ITEMMGR->DiscardItem(
		&player,
		itemPosition,
		script.mItem.mItemIndex,
		script.mItem.mQuantity))
	{
		return FALSE;
	}

	TCHAR text[MAX_PATH] = {0};
	_stprintf(
		text,
		"*limit.%u.%u",
		script.mMapType,
		script.mChannel);
	LogItemMoney(
		player.GetID(),
		player.GetObjectName(),
		0,
		text,
		eLog_ItemDiscard,
		player.GetMoney(),
		0,
		0,
		itemBaseForLog.wIconIdx,
		itemBaseForLog.dwDBIdx,
		itemBaseForLog.Position,
		0,
		itemBaseForLog.Durability - script.mItem.mQuantity,
		0);
	return TRUE;
}

BOOL CLimitDungeonManager::IsOpen(DWORD channelIndex) const
{
	const RoomKey roomKey(mMapType, channelIndex);

	return mRoomMap.end() != mRoomMap.find(roomKey);
}