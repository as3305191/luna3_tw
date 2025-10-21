/*
	Ŭ������ : class AESFile

	��    �� : ������ Bin������ Decoding����� AES��ȣȭ ������� �����ϱ� ���� Ŭ����
*/
#pragma once
#ifdef _AES_FILE_

#define MAX_KEY_LENGTH		512


struct AESFILE_HEADER
{
	DWORD	dwVersion;	// version
	DWORD	dwType;		// fileType 
	DWORD	dwDataSize;	// data size
	DWORD	dwSecretKey; // Leeching Protection
};


// Bin���� ������������� �����ϴ� �迭
const DWORD dwVersion[] = 
{
	20040308,
	20090506,
	20101117,
	20250813,
};

const DWORD dwSecretKey[] = 
{
	20040803,
	20050506,
	20092007,
	20251308,
};

// ��������� �����ϴ� Ű���� �����ϴ� �迭
const unsigned char aKey[][MAX_KEY_LENGTH] = 
{
	"",
	"E76B2413958B00E193A1",
	"A15H3825287T19G082K8",
	"CBALUNA20250813ABCDAX",
};



class cAESFile
{
	/* ������ ����(Ű) ī���� */ 
	int	m_nVersionCount;

	DWORD m_dwCurVersion;
	DWORD m_dwLastVersion;
	
	DWORD m_dwCurSecretKey;
	DWORD m_dwLastSecretKey;

	unsigned char m_aCurKey[MAX_KEY_LENGTH];
	unsigned char m_aLastKey[MAX_KEY_LENGTH];

public:
	/*
		Bin��������� ���������� �̿��Ͽ� �ش��ϴ� Ű�� �ִ��� �˻��Ѵ�.
		Ű�� �����ϸ� ������ Ű���� �����Ѵ�.
	*/
	BOOL SetBinVersion( DWORD BinVersion );

	/*
		Ű���� ���̸� ��ȯ�Ѵ�.
	*/
	BOOL IsExistKey();

public:
	cAESFile();
	virtual ~cAESFile();

	void Init();
	BOOL AESEncryptData(FILE* fp, char* pFileName, char* pData, DWORD dwDataSize);
	BOOL AESDecryptData(FILE* fp, char* pData, DWORD dwDataSize, DWORD dwType);

};

#endif