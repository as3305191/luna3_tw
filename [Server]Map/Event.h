/*
080616 LUJ, ���� �߿� �߻��ϴ� �پ��� �̺�Ʈ�� ó���ϱ� ���� �߻� Ŭ����
*/
#pragma once

class CObject;

class CEvent
{
public:
	enum Type
	{
		TypeNone,
		TypeTakeDamage,
		TypeGiveDamage,
		TypeDie,
		TypeKill,
	};

	inline Type GetType() const	{ return mType; }
	inline eStatusKind GetTargetStatus() const { return mTargetStatus; }

protected:
	// 080616 LUJ, ���� �������� ���ϵ��� ������/�Ҹ��ڸ� protected�� �����Ѵ�
	CEvent( Type, eStatusKind );
	virtual ~CEvent(void) {};

	Type mType;
	// 100310 ShinJS --- Ư�� eStatusKind �� ó���Ҽ� �ֵ��� ���� �߰�
	eStatusKind			mTargetStatus;
};

// 080616 LUJ, �ٸ� ������Ʈ���� ���ظ� ���� �� �ߵ��Ǵ� �̺�Ʈ
class CGiveDamageEvent : public CEvent
{
public:
	CGiveDamageEvent( CObject* attacker, const RESULTINFO& );
	CGiveDamageEvent( CObject* attacker, const RESULTINFO&, eStatusKind );

	inline const RESULTINFO& GetResult() const	{ return mResult; }
	inline CObject* GetTarget() const			{ return mTarget; }

private:
	const RESULTINFO	mResult;
	CObject*			mTarget;
};

// 080616 LUJ, �ٸ� ������Ʈ�� ���ظ� �޾��� �� �ߵ��Ǵ� �̺�Ʈ
// 100219 ShinJS --- RESULTINFO �� �����Ҽ� �ֵ��� ����
class CTakeDamageEvent : public CEvent
{
public:
	CTakeDamageEvent( CObject* attacker, RESULTINFO& );
	CTakeDamageEvent( CObject* attacker, RESULTINFO&, eStatusKind );

	inline RESULTINFO& GetResult() const	{ return mResult; }
	inline CObject* GetAttacker() const			{ return mAttacker; }

private:
	RESULTINFO&			mResult;
	CObject*			mAttacker;
};

// 080616 LUJ, �ٸ� ������Ʈ���� �ǻ�� �� �߻��ϴ� �̺�Ʈ
// 080708 LUJ, �����ڸ� ���ڷ� �޵��� ��
class CDieEvent : public CEvent
{
public:
	CDieEvent( CObject* killer, CObject* victim );

	inline CObject* GetKiller() const	{ return mKiller; }
	inline CObject* GetVictim() const	{ return mVictim; }

protected:
	CObject* mVictim;
	CObject* mKiller;
};

// 080616 LUJ, �ٸ� ������Ʈ�� ������ �� �߻��ϴ� �̺�Ʈ
// 080708 LUJ, CDieEvent�� ���� ������ ������ ��
class CKillEvent : public CDieEvent
{
public:
	CKillEvent( CObject* killer, CObject* victim );
};