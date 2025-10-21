#pragma once
#include <queue>

#define LIMITDUNGEONMGR	CLimitDungeonManager::GetInstance()

class CPlayer;

class CLimitDungeonManager
{
private:
	// 091106 LUJ, ������ ���������� ����Ǿ� �ִ�
	struct Event
	{
		enum Type
		{
			TypeNone,
			TypeOpen,
			TypeOpenIlregulary,
			TypeClose,
			TypeCloseIlregulary,
		}
		mType;
		MAPTYPE mMapType;
		DWORD mChannelIndex;
		__time64_t mPlayTime;
	};
	typedef std::multimap< __time64_t, Event > ScheduleMap;
	ScheduleMap mScheduleMap;
	typedef DWORD ChannelIndex;
	// 091106 LUJ, ������ ���� ���� ������ ����Ǿ� �ִ�
	typedef std::pair< MAPTYPE, ChannelIndex > RoomKey;
	struct Room
	{
		typedef stdext::hash_set< DWORD > IndexSet;
		IndexSet mPlayerIndexSet;
		__time64_t mCloseTime;
	};
	typedef std::map< RoomKey, Room > RoomMap;
	RoomMap mRoomMap;
	// 091106 LUJ, ���� ������ ������Ʈ�� ���. ť�� ����Ǿ� ���������� ó���ȴ�
	typedef std::queue< DWORD > IndexQueue;
	IndexQueue mPlayerQueue;
	// 091106 LUJ, ���� �� ��ȣ
	MAPTYPE mMapType;
	// 091106 LUJ, ��ũ��Ʈ ���� ���� ����
	struct stat mFileStatus;
	
public:
	CLimitDungeonManager(void);
	virtual ~CLimitDungeonManager(void);
	static CLimitDungeonManager* GetInstance();
	void NetworkMsgParser(MSGBASE*);
	void Process();
	void AddPlayer(CPlayer&);

private:
	void UpdateSchedule();
	void RefreshScript();
	BOOL IsReady() const;
	void PostponeSchedule();
	__time64_t GetSecondForOneWeek() const;
	void Run();
	void AskJoin(DWORD playerIndex, MAPTYPE, DWORD channelIndex, POSTYPE);
	void OpenRoom(const Event&);
	void CloseRoom(const Event&);
	BOOL IsOpen(DWORD channelIndex) const;
	Room& GetRoom(MAPTYPE, DWORD channelIndex);
	BOOL IsNoRoom(MAPTYPE, DWORD channelIndex);
	void UpdatePlayer();
	BOOL UseItem(const LimitDungeonScript&, CPlayer&, POSTYPE);
};