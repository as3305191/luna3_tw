/*
080401 LUJ, �α� �� ����Ǵ� â ��ɿ� ���� �߻� Ŭ����
*/
#pragma once


class CListCtrl;
class CProgressCtrl;
class CStatic;
class CButton;
class CclientApp;


class CCommand
{
	// 080401 LUJ, �߻� Ŭ������ ������ �� ������ ������/�Ҹ��ڸ� protected�� �����
protected:
	inline CCommand( CclientApp& application, const TCHAR* title )	:
	mApplication( application ),
		mTickCount( 0 ),
		mTitle( title )
	{}

	inline virtual ~CCommand() {};

public:
	virtual void Find(
		DWORD			serverIndex,
		const TCHAR*	beginTime,
		const TCHAR*	endTime )	= 0;

	virtual void Parse(
		const MSGROOT*,
		CListCtrl&,
		CProgressCtrl&,
		CStatic&,
		CButton&		findButton,
		CButton&		stopButton )	const = 0;

	virtual void Stop		( DWORD serverIndex )					const = 0;
	virtual void Initialize	( CListCtrl& )							const = 0;
	virtual void SaveToExcel( DWORD serverIndex, const CListCtrl& )	const = 0;

protected:
	CclientApp& mApplication;
	CString		mTitle;
	DWORD		mTickCount;	

public:
	inline const TCHAR* GetTitle() const { return mTitle; }
};
