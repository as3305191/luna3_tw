// AddableInfo.h: interface for the CAddableInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ADDABLEINFO_H__9C670F70_E708_4B9C_8FAA_DE4BF1ADE3CB__INCLUDED_)
#define AFX_ADDABLEINFO_H__9C670F70_E708_4B9C_8FAA_DE4BF1ADE3CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma pack(push,1)

#define MAX_ADDABLEINFO_SIZE	4096*4	//�˳����սô�.

void WriteAssertMsg(char* pStrFileName,int Line,char* pMsg);

struct AddableInfoEntity
{
	BYTE InfoKind;
	WORD InfoSize;
	char InfoData[MAX_ADDABLEINFO_SIZE];
	
	inline WORD GetAddableInfoEntity()
	{
		return sizeof(AddableInfoEntity)-MAX_ADDABLEINFO_SIZE+InfoSize;
	}
};


class CAddableInfoList
{
	WORD m_AddableInfoSize;
	char m_AddInfoBuf[MAX_ADDABLEINFO_SIZE];
	
	
public:	
	enum AddableInfoKind
	{
		None = 0,
		SkillTree,
		MoveInfo,
		StreetStall,
		StreetBuyStall,
		ShowdownInfo,
		GTournament,
		ItemOption,
		PetTotalInfo,
	};
	
	CAddableInfoList(){	Init();}
	~CAddableInfoList(){}
	
	inline void Init()	{	m_AddableInfoSize = 0;	}
	
	void AddInfo(BYTE Kind,WORD InfoLength,void* pInfoData, const char* caller = "" )
	{
		AddableInfoEntity* pNewInfo = (AddableInfoEntity*)&m_AddInfoBuf[m_AddableInfoSize];
		pNewInfo->InfoKind = Kind;
		pNewInfo->InfoSize = InfoLength;
		WORD NewEntityLength = pNewInfo->GetAddableInfoEntity();

		// 090316 LUJ, ���� �����÷ΰ� �߻����� �ʵ��� �˻��Ѵ�
		if( NewEntityLength+m_AddableInfoSize >= MAX_ADDABLEINFO_SIZE )
		{
#ifdef _MAPSERVER_
			char text[ MAX_PATH ] = { 0 };
			sprintf( text, "By %s buffer overflow occured in CAddableInfoList::AddInfo()\n", caller );
			OutputDebugString( text );
#endif
			return;
		}

		memcpy(pNewInfo->InfoData,pInfoData,InfoLength);
		m_AddableInfoSize= m_AddableInfoSize + NewEntityLength;
	}
	
	inline WORD GetInfoLength()	{	return 2+m_AddableInfoSize;	}
	
	friend class CAddableInfoIterator;
};


class CAddableInfoIterator
{
	WORD m_CurPosition;
	AddableInfoEntity* m_pCurInfo;
	CAddableInfoList* m_pAddableInfoList;
	
public:
	CAddableInfoIterator(CAddableInfoList* pInfoList)
	{
		m_pAddableInfoList = pInfoList;
		m_CurPosition = 0;
		m_pCurInfo = (AddableInfoEntity*)&m_pAddableInfoList->m_AddInfoBuf[m_CurPosition];
	}
	
	BYTE GetInfoKind()
	{
		if(m_pAddableInfoList->m_AddableInfoSize <= m_CurPosition)
			return CAddableInfoList::None;
		
		return m_pCurInfo->InfoKind;
	}
	void GetInfoData(void* pOutData)
	{
		memcpy(pOutData,m_pCurInfo->InfoData,m_pCurInfo->InfoSize);
	}
	void ShiftToNextData()
	{
		WORD EntityLength = m_pCurInfo->GetAddableInfoEntity();
		m_CurPosition = m_CurPosition + EntityLength;
		m_pCurInfo = (AddableInfoEntity*)&m_pAddableInfoList->m_AddInfoBuf[m_CurPosition];
	}
};

#pragma pack(pop)

#endif // !defined(AFX_ADDABLEINFO_H__9C670F70_E708_4B9C_8FAA_DE4BF1ADE3CB__INCLUDED_)
