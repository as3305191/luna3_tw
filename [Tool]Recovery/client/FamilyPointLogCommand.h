/*
080403 LUJ, ����Ʈ �α׸� ó���ϴ� Ŀ�ǵ� Ŭ����
*/
#pragma once


#include "Command.h"


class CFamilyPointLogCommand : public CCommand
{
public:
	CFamilyPointLogCommand( CclientApp&, const TCHAR* title, DWORD familyIndex );

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
	DWORD mFamilyIndex;
};
