// MapNameManager.h: interface for the CMapNameManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAPNAMEMANAGER_H__E7197EE0_38FD_4C3D_B592_83AB8F7CF490__INCLUDED_)
#define AFX_MAPNAMEMANAGER_H__E7197EE0_38FD_4C3D_B592_83AB8F7CF490__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAPNAME USINGTON(CMapNameManager)

class CMapNameManager  
{
protected:
	// 080820 LUJ, �����ڵ� �������� �۾�. Ű: �� ��ȣ
	typedef stdext::hash_map< DWORD, CString >	MapNameTable;
	MapNameTable							m_MapNameTable;

public:
	CMapNameManager();
	virtual ~CMapNameManager();

	const TCHAR*	GetMapName( DWORD dwMapNum );
};

EXTERNGLOBALTON(CMapNameManager)

#endif // !defined(AFX_MAPNAMEMANAGER_H__E7197EE0_38FD_4C3D_B592_83AB8F7CF490__INCLUDED_)
