#pragma once
#include "skillobject.h"

class cBuffSkillInfo;

class cBuffSkillObject :
	public cSkillObject
{
	/// ����ȿ�� ����Ʈ
	cPtrList	mStatusList;
	BOOL		mIsTurnOn;

public:
	cBuffSkillObject(cBuffSkillInfo*);
	virtual ~cBuffSkillObject();
	virtual void Init(const SKILLOBJECT_INFO&);
	virtual void Update();
	virtual void OnReceiveSkillResult(MSG_SKILL_RESULT*);
	void ResetDelay();
	void SetCount(int count);

private:
	void ReleaseSkillObject();
	BOOL IsHiddenCount();

	// 080318 LUJ,	��ų�� ������ �ð� ����.	
private:
	DWORD			mBeginTime;
public:
	inline DWORD GetBeginTime() const { return mBeginTime; }
	// 0901222 NYJ - ��ų�� ���۵� �ð� ����
	void SetBeginTime(DWORD dwTime) {mBeginTime = dwTime;}
};
