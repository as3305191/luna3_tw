#pragma once

#include "Pool.h"

class CObject;

namespace Trigger
{
	enum eOwner;
	enum eProperty;
	class CTrigger;

	class CAction : public CPoolObject
	{
	public:
		struct Parameter
		{
			DWORD mDescIndex;
			DWORD mChannelID;
			eOwner mOwnerType;
			DWORD mOwnerIndex;
			DWORD mTriggerIndex;

			Parameter() :
			mDescIndex(0),
			mChannelID(0),
			mOwnerType(eOwner(0)),
			mOwnerIndex(0),
			mTriggerIndex(0)
			{}
		};

	public:
		CAction();
		virtual ~CAction(void);
		// ����, �׼� ��ũ��Ʈ�� ���� �޾� �����ϵ��� �Ѵ�. �Լ��� Run�� ���?
		virtual void DoAction() = 0;
		void SetParameter(const Parameter&);

	protected:
		Parameter mParameter;
		int GetValue(eProperty) const;
		const CTrigger& GetTrigger() const;
		DWORD GetChannelID() const;
		DWORD GetHeadTarget() const;
		DWORD GetNextTarget() const;
		LPCTSTR GetAliasName(DWORD hashCode) const;

	private:
		typedef std::list< DWORD > ObjectIndexList;
		mutable ObjectIndexList mObjectIndexList;
		mutable ObjectIndexList::const_iterator mObjectIndexIterator;
	};
}