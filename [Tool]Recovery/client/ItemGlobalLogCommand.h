/*
080716 LUJ, �Ϻ��� �и��� ������ �α� ���̺��� �ϰ��� �˻��� �� �ִ� Ŀ�ǵ� Ŭ����
*/
#pragma once


#include "command.h"


class CItemGlobalLogCommand : public CCommand
{
public:
	struct Configuration
	{
		eLogitemmoney	mLogType;
		DWORD			mPlayerIndex;
		DWORD			mItemDbIndex;
		DWORD			mItemIndex;
	};

public:
	CItemGlobalLogCommand( CclientApp&, const TCHAR* title, const Configuration& );
	
	virtual void SaveToExcel( DWORD serverIndex, const CListCtrl&  )	const;
	virtual void Initialize	( CListCtrl& )								const;
	virtual void Stop		( DWORD serverIndex )						const;

	virtual void Find(
		DWORD			serverIndex,
		const TCHAR*	beginTime,
		const TCHAR*	endTime );

	virtual void Parse(
		const MSGROOT*,
		CListCtrl&,
		CProgressCtrl&,
		CStatic&,
		CButton&	findButton,
		CButton&	stopButton ) const;

private:
	const Configuration mConfiguration;
};
