#pragma once

#include "../4DyuchiGRX_Common/IFileStorage.h"

#define CURRENT_VERSION		0x00000001
class CPackFile;


struct FSFILE_DESC
{
	DWORD			dwTotalSize;
	DWORD			dwPositionInPackFile;

	DWORD			dwRealFileSize;
			
	BOOL			bSourceFromPackFile;	// TRUE�̸� �����Ϸκ��� ������ ��ũ����, FALSE�̸� ������ ���Ϸκ��� ������ ��ũ����.
	FSFILE_DESC*	pPrvInPackFile;
	FSFILE_DESC*	pNextInPackFile;

	char*			pEntryInMemory;
	DWORD			dwOpenCount;


	void*			pHashHandle;
	CPackFile*		pOwnerPackFile;

	char*			pszFileName;				// full path ���ϸ� 
	DWORD			dwFileNameLen;			// full path ���ϸ� ����
};
struct FSFILE_POINTER
{
	BOOL			bFromPack;
	FILE*			fp;
	DWORD			dwSize;
	DWORD			dwOffset;
	FSFILE_DESC*	pFSDesc;
	
};

struct FSFILE_HEADER
{
	DWORD	dwTotalSize;
	DWORD	dwRealFileSize;
	DWORD	dwFileNameLen;
	DWORD	dwFileDataOffset;
	DWORD	dwFlag1;
	DWORD	dwFlag2;
	DWORD	dwFlag3;
	DWORD	dwFlag4;

	char	szFileName[4];
};

#define FSFILE_HEADER_SIZE	32

struct PACK_FILE_HEADER
{
	DWORD	dwVersion;
	DWORD	dwFileItemNum;
	DWORD	dwFlag;
	DWORD	dwCRC[4];
	DWORD	dwReserved[16];

	DWORD	dwType;		// fileType 
	DWORD	dwDataSize;	// data size
	DWORD	dwSecretKey; // Leeching Protection
};


