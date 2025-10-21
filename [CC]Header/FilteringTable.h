#pragma once

#define FILTERTABLE	CFilteringTable::GetInstance()

// ..���͸����� ���ܷ� ó���� ��� �� Ÿ��
enum FILTER_EXCEPTION_TYPE
{
	FET_NONE = 0,
	FET_WHISPER_CHARNAME, // �Ӹ� ĳ���͸�
};

class CFilteringTable
{	
protected:
#ifdef _UNICODE
	typedef std::wstring Text;
#else
	typedef std::string Text;
#endif
	friend BOOL FindPartially(const Text&, const Text&);
	typedef std::set< Text > KeywordContainer;
	enum Type
	{
		TypeNone,
		TypePartiallyBlockedName,
		TypeEntirelyBlockedName,
		TypePartiallyBlockedKeyword,
	};
	typedef std::map< Type, KeywordContainer > FilterContainer;
	FilterContainer mFilterContainer;
	FILTER_EXCEPTION_TYPE m_eFilterExceptionType;

protected:
	CFilteringTable();
	virtual ~CFilteringTable();
	void LoadScript();
	BOOL IsInvalidText(Type, LPCTSTR) const;
	BOOL IsInDBCRange(WORD DBChar) const;
	BOOL IsExceptionInvalidCharInclude(unsigned char* pszText);
	void RemoveSpace(LPCTSTR, LPTSTR, size_t) const;

public:
	static CFilteringTable* GetInstance();
	BOOL IsInvalidCharInclude(unsigned char* pStr, BOOL AllowAllDBC = FALSE);
	BOOL IsUsableName(LPCTSTR);
	BOOL FilterChat(LPCTSTR);
	BOOL IsCharInString(const char* pStr, const char* pNoChar);
	void SetExceptionInvalidCharIncludeType(FILTER_EXCEPTION_TYPE eFilterExceptionType) { m_eFilterExceptionType = eFilterExceptionType; }
};