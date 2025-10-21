#pragma once
#include "Action.h"

namespace Trigger
{
	class CActionMonsterChat :
		public CAction
	{
	public:
		virtual void DoAction();
		static CAction* Clone()
		{
			return new CActionMonsterChat;
		}
	};
}