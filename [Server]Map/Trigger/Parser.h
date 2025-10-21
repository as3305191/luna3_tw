#pragma once
#include "common.h"

namespace Trigger
{	
	struct PropertyText;
	struct ValueText;
	class CCondition;
	class CMessage;
	class CAction;
	typedef BOOL (*CheckFunc)(const CCondition&, const CMessage&);
	typedef CAction* (*ActionFunc)();

	class CParser
	{
		friend class CManager;
	public:
		BOOL Update();
		const TriggerDesc& GetTriggerDesc(DWORD hashCode) const;
		const ActionDesc& GetActionDesc(DWORD actionDescIndex) const;
		const ConditionDesc& GetConditionDesc(DWORD conditionDescIndex) const;
		CheckFunc GetCheckFunc(eProperty) const;
		CAction* CreateAction(eAction) const;

	protected:
		CParser(MAPTYPE);
		virtual ~CParser();
		void Load(LPCTSTR fileName);
		void Release();
		eProperty ParseKeyword(LPCTSTR);
		int ParseValue(eProperty, LPCTSTR);
		eOperation ParseOperation(LPCTSTR) const;
		LPCTSTR GetAliasName(DWORD hashCode) const;
		DWORD GetHashCode(LPCTSTR);
		LPCTSTR GetDescName() const;
		// �ߺ��� ���� ��ϵǾ� �ִ��� �˻��Ѵ�
		void CheckData(const PropertyText*, size_t);
		void CheckData(const ValueText*, size_t);
		void UpdateFuncMap();
		
	private:
		// Ʈ���� ��ũ��Ʈ
		typedef stdext::hash_map<DWORD, TriggerDesc> TriggerDescMap;
		TriggerDescMap mTriggerDescMap;
		// ����� ��ũ��Ʈ
		typedef stdext::hash_map<DWORD, ConditionDesc>	ConditionDescMap;
		ConditionDescMap mConditionDescMap;
		// �׼� ��ũ��Ʈ
		typedef stdext::hash_map<DWORD, ActionDesc> ActionDescMap;
		ActionDescMap mActionDescMap;
		// ��Ī ���̺�. Ű: ��Ī���� ����� �ؽ��ڵ�. GetHashCode() ����
		typedef stdext::hash_map< DWORD,std::string > AliasMap;
		AliasMap mAliasMap;
		// 100111 LUJ, �˻��ߴ� ���� �� �̻� ������� �ʱ� ���� ����� �����̳�
		typedef stdext::hash_map< std::string, DWORD > HashCodeMap;
		HashCodeMap mHashCodeMap;
		struct stat mFileStatus;
		MAPTYPE mMapType;
		// ����ǿ��� �˻��ϴ� �Ӽ��� ���� ��� ����Ǿ� �ִ�
		typedef stdext::hash_map< eProperty, CheckFunc > CheckFuncMap;
		CheckFuncMap mCheckFuncMap;
		// �Ӽ����� ������ �׼��� ����Ǿ� �ִ�
		typedef stdext::hash_map< eAction, ActionFunc > ActionFuncMap;
		ActionFuncMap mActionFuncMap;
	};
}