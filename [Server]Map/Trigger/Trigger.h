#pragma once

#include "Pool.h"

namespace Trigger
{
	enum eState;
	enum eOwner;
	enum eProperty;
	class CCondition;
	class CMessage;

	class CTrigger : public CPoolObject
	{
		friend class CManager;
	public:
		struct Parameter
		{
			DWORD mIndex;
			eOwner mOwnerType;
			DWORD mOwnerIndex;
			DWORD mChannelID;
			DWORD mHashCode;
			DWORD mRepeatCount;
			TCHAR mName[MAX_PATH];
			struct tm mRegTime;
		};

	public:
		CTrigger();
		virtual ~CTrigger();
		eState GetState() const	{ return mState; }
		DWORD GetIndex() const { return mParameter.mIndex; }
		// DB�� ���� ���¸� �޾ƿ� �� ��� (������ ȣ��)
		void Process(const CMessage&);
		int GetActionValue(DWORD descIndex, eProperty) const;
		// Ʈ������ ���� �̸��� �����´�
		DWORD GetHashCode() const;
		// �־��� �ؽ� �ڵ带 Ʈ������ ������ ��ȣ, ���¿� ���� �����ؼ� ��ȯ�Ѵ�
		DWORD GetHashCode(DWORD aliasHashCode) const;
		DWORD GetRepeatCount() const { return mParameter.mRepeatCount; }
		DWORD GetSuccessObjectIndex(DWORD conditionHashcode) const;
		DWORD GetChannel() const { return mParameter.mChannelID; }
		void RestoreRepeatCount(DWORD repeatCount);
		LPCTSTR GetName() const { return mParameter.mName; }
		const struct tm& GetRegTime() const { return mParameter.mRegTime; }
		void SetRegTime(const struct tm&);
		inline eOwner GetOwnerType() const { return mParameter.mOwnerType; }
		inline DWORD GetOwnerIndex() const { return mParameter.mOwnerIndex; }
		void UpdateParamter(const Parameter&);
		void NetworkMsgParser(DWORD connectionIndex, BYTE protocol, LPVOID packet, DWORD size);

	private:
		void SetState(eState state)	{ mState = state; }
		void DoAction() const;
		void UpdateCondition();
		void ReleaseCondition();
		BOOL FailCondition(const CMessage&) const;
		void SetDescTimeStamp();
		void SaveToDb();
		void LoadFromDb();
		BOOL IsLoop() const;
		void ResetCondition();

	private:
		Parameter mParameter;
		// ��: �ؽ��ڵ�
		typedef stdext::hash_map< DWORD, CCondition* > ConditionMap;
		ConditionMap mConditionMap;
		eState mState;
	};
}