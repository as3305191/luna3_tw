#include "StdAfx.h"
#include "common.h"

namespace Trigger
{
	void PutLog(LPCTSTR text, ...)
	{
		const HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);

		// 091116 LUJ, �ܼ� â�� ���� ������ �α׸� ������� �ʵ��� �Ͽ� ���ϸ� �ּ�ȭ��
		if(0 == handle)
		{
			return;
		}

		SYSTEMTIME time = { 0 };
		::GetLocalTime( &time );

		TCHAR buffer[ 2048 ] = { 0 };
		va_list argument;
		va_start( argument, text );
		_vstprintf( buffer, text, argument );
		va_end( argument);

		TCHAR log[ 2048 ] = { 0 };
		_stprintf(
			log,
			_T( "%02d:%02d:%02d> %s\n" ),
			time.wHour,
			time.wMinute,
			time.wSecond,
			buffer );
		
		ULONG character = 0;
		WriteConsole(
			handle,
			log,
			_tcslen(log),
			&character,
			0);
	}
}