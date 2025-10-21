#include "stdafx.h"
#include "FileName.h"

CFileName::CFileName(CFileName* pParent,CYHFileFind* pFileFind) :
m_pParent( pParent ),
m_bIsDirectory( FALSE )
{
	if( pFileFind )
	{
		m_FileName = pFileFind->GetFileName();
	}
}

CFileName::~CFileName()
{}

BOOL CFileName::IsPak() const {

	CString text;
	text = GetFileName().Right(GetFileName().GetLength() - (GetFileName().Find(_T('.')) +1)); 
	return text == _T( "pak" );
}
CString CFileName::GetFullPathName() const
{
	CString path;

	if( m_pParent )
	{
		path += m_pParent->GetFullPathName();
	}

	return path + m_FileName;
}