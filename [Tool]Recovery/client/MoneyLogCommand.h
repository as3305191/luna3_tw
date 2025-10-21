/*
080403 LUJ, ����Ʈ �α׸� ó���ϴ� Ŀ�ǵ� Ŭ����
*/
#pragma once


#include "Command.h"


class CMoneyLogCommand : public CCommand
{
public:
	struct Configuration
	{
		Configuration() :
		mPlayerIndex( 0 )
		{}

		DWORD mPlayerIndex;
	};

public:
	// 090122 LUJ, ������ ����
	CMoneyLogCommand( CclientApp&, const TCHAR* title, const Configuration& );

	virtual void SaveToExcel( DWORD serverIndex, const CListCtrl&  ) const;
	virtual void Initialize( CListCtrl& ) const;
	virtual void Stop( DWORD serverIndex ) const;
	virtual void Find( DWORD serverIndex, const TCHAR* beginTime, const TCHAR* endTime );
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