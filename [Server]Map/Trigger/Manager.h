#pragma once
#include "common.h"
#include "message.h"

#define TRIGGERMGR (&Trigger::CManager::GetInstance())

enum EObjectKind;
class CPlayer;

namespace Trigger
{
	class CTrigger;
	class CMessage;
	class CParser;
	class CAction;
	class CCondition;
	class CFlagManager;
	class CTimer;
	class CCristalEmpireBonusProcess;
	typedef BOOL (*CheckFunc)(const CCondition&, const CMessage&);
	typedef CAction* (*ActionFunc)();
	
	class CManager
	{
	public:
		static CManager& GetInstance();
		static void NetworkMsgParser(DWORD connectionIndex, LPSTR packet, DWORD length);
		CMessage* const AllocateMessage(DWORD channelIndex);
		void Release();
		void Process();
		void NetworkMsgParser(DWORD dwConnectionIndex, BYTE Protocol, LPVOID pMsg, DWORD dwLength);
		void LoadTrigger(CPlayer&);
		CTrigger* CreateTrigger(eOwner, DWORD ownerIndex, DWORD dwChannelID, DWORD hashCode, DWORD repeatCount, struct tm& regTime);
		void CreateTrigger(eOwner, LPDBMESSAGE, LPQUERY);
		const TriggerDesc& GetTriggerDesc(DWORD hashCode) const;
		const ConditionDesc& GetConditionDesc(DWORD conditionDescIndex) const;
		const ActionDesc& GetActionDesc(DWORD actionDescIndex) const;
		DWORD GetHashCode(LPCTSTR);
		LPCTSTR GetAliasName(DWORD hashCode) const;
		CTrigger& GetTrigger(DWORD triggerIndex);
		void RestoreRepeatCount(DWORD triggerIndex, DWORD repeatCount, const struct tm&);
		void StopServerTrigger(DWORD channelIndex);
		void StopObjectTrigger(EObjectKind, DWORD objectIndex);
		CheckFunc GetCheckFunc(eProperty) const;
		CAction* CreateAction(eAction) const;
		void ToggleStatistic();
		void TogglePeriodicMessage();
		void SetFlag(eOwner, DWORD ownerIndex, DWORD alias, int value);
		int GetFlag(eOwner, DWORD ownerIndex, DWORD alias) const;
		bool IsFlag(eOwner, DWORD ownerIndex, DWORD alias) const;
		void StartTimer(DWORD alias, DWORD dwChannel, eTimerType type, DWORD dwDuration);
		BOOL SetTimerState(DWORD alias, eTimerState state);
		CTimer* GetTimer(DWORD alias);
		void BatchItemOption(DWORD playerIndex, DWORD itemIndex, ITEM_OPTION::Drop::Key, float power, LEVELTYPE, size_t size);

	private:
		// LUJ, �̱��� ��ü�̹Ƿ� ������/�Ҹ��ڸ� private���� �����Ѵ�
		CManager(void);
		virtual ~CManager(void);
		int GetMessageProceedCount();
		void UpdateDesc();
		void UpdateDescGroup();
		void StopTrigger(eOwner, DWORD ownerIndex);
		void ShowStatistic();
		BOOL IsNoOwner(eOwner, DWORD ownerIndex);
		DWORD GetChannelIndex(eOwner, DWORD ownerIndex);
		void CreateDefaultTrigger(eOwner, DWORD ownerIndex, DWORD channelIndex);
		
	private:
		// Ű: ������ ����, ��: ������ ��ȣ
		typedef std::pair< eOwner, DWORD > OwnerKey;
		// LUJ, Ʈ���� �Ŵ����� ���ʿ� �����ų �ý��� Ʈ���� ��ȣ�� �����ϴ� �����̳�
		typedef DWORD TriggerIndex;
		typedef std::list< TriggerIndex > TriggerIndexList;
		stdext::hash_map< DWORD, TriggerIndexList > mTriggerDescByGroup;
		// �޽��� ť
		typedef std::map< DWORD, CMessage > MessageMap;
		MessageMap mMessageMap;
		// ������ Ʈ���� �ε��� ť
		std::queue< TriggerIndex > mDeleteTriggerQueue;
		// Ʈ���� �ν��Ͻ��� �����Ǵ� �����̳�
		typedef stdext::hash_map< DWORD, CTrigger > TriggerMap;
		TriggerMap mTriggerMap;
		// 091116 LUJ, ä�κ��� �з��� Ʈ���� ��ȣ�� ����ȴ�
		typedef DWORD ChannelIndex;
		typedef stdext::hash_set< ChannelIndex > TriggerIndexSet;
		typedef stdext::hash_map< ChannelIndex, TriggerIndexSet > ChannelTriggerMap;
		ChannelTriggerMap mChannelTriggerMap;
		std::queue< TriggerIndex > mReservedTriggerQueue;
		// 091116 LUJ, �� ���μ��� Ÿ�ӿ� ó���� �޽��� ����
		int mMessageProceedCount;
		CParser* mParser;
		struct Statistic
		{
			DWORD mProceededTriggerCount;
			DWORD mRemovedTriggerCount;
			DWORD mAllocatedTriggerCount;
			DWORD mProceededMessageCount;
			DWORD mAllocatedMessageCount;
			DWORD mRemovedMessageCount;

			Statistic()
			{
				ZeroMemory(this, sizeof(*this));
			}
		}
		mStatistic;
		typedef stdext::hash_set< TriggerIndex > TriggerIndexSet;
		// Ʈ���� �����ֿ� ���� ������ Ʈ���� ��ȣ�� �����Ѵ�
		// ��Ÿ������ üũ�� �� ���� Ÿ�̹��� ��ġ�� �ٽ� �����Ǵ� ��찡 �ִ�.
		// �̸� ���� ���� �̺�Ʈ ������ Ʈ���Ÿ� ����/������ �� �ֵ��� �����̳ʸ� �����Ѵ�
		typedef std::map< OwnerKey, TriggerIndexSet > OwnerCreatedTriggerMap;
		OwnerCreatedTriggerMap mOwnerCreatedTriggerMap;
		BOOL mIsHideStatistic;
		BOOL mIsUnablePeriodicMessage;
		CFlagManager* mFlagManager;
		// Ÿ�̸�
		typedef stdext::hash_map< DWORD, CTimer* > TimerMap;
		TimerMap mTimerMap;
		CCristalEmpireBonusProcess* mCristalEmpireBonusProcess;		
	};
}
