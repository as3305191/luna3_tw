#pragma once
#include "skillobject.h"

class cBuffSkillInfo;
class CTargetList;
class cDelay;
class CEvent;

class cBuffSkillObject :
	public cSkillObject
{
	/// ���ӽð�
	cDelay*		mpDelay;
	cDelay*		mpInterval;
	cDelay*		mpUpdate;
	cDelay*		mpConsumption;
	const cBuffSkillInfo* const mSkillInfo;
	BOOL		mDelayCheck;
	BOOL		mActive;
	BOOL		mDestroy;
	// 090204 LUJ, ���ǿ� ���� ��ų ȿ���� ����/�����Ѵ�
	BOOL		mIsTurnOn;
	BOOL		mIsUpdate;
	// 090204 LUJ, ���� ǥ�� ������ Ȯ��
	int			mCount;
	// 090204 LUJ, Ÿ���� ��Ȯ�� ��
	eBuffSkillCountType mCountType;

protected:
	void Consumption();
	void ProcessStart();
	void ProcessState();
	void SetActive(BOOL);
	void ResetDelay();
	DWORD GetDelayTime(CObject*, const BUFF_SKILL_INFO*) const;
	DWORD GetDelayTime() const;
	virtual void Execute(const CEvent&);
	void AddAggro(CObject&);

public:
	cBuffSkillObject(const cBuffSkillInfo&);
	virtual ~cBuffSkillObject();
	virtual void Init(sSKILL_CREATE_INFO*);
	virtual SKILL_STATE	Update();
	virtual void SetEndState();
	virtual void EndState();
	virtual DWORD SetAddMsg(DWORD dwReceiverID, BOOL bLogin, MSGBASE*&);
	void RemoveBuffCount(eBuffSkillCountType, int count);
	virtual void SetMultiTarget(CTargetList*) {}
	const BUFF_SKILL_INFO& GetInfo() const;
};
