#pragma once
//-------------------------------------------------------------------------------------------------
//	NAME		: CNpcRecallMgr.h
//	DESC		: NPC�� ��ȯ�ϴ� ������ �ϴ� �Ŵ��� Ŭ����.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//		Define some macroinstruction.
//-------------------------------------------------------------------------------------------------
#define NPCRECALLMGR	CNpcRecallMgr::GetInstance()

//#define _USE_NPCRECALL_ERRBOX_					// �� ��ũ�ΰ� Ȱ��ȭ �Ǿ� ������, ���� �߻��� �޽��� �ڽ��� ����,
												// ��Ȱ��ȭ ��Ű��, Sworking ���� ��, ���� �α׸� �����.
//-------------------------------------------------------------------------------------------------
//		The class CNpcRecallMgr.
//-------------------------------------------------------------------------------------------------
class CNpcRecallMgr
{
public :
	GETINSTANCE(CNpcRecallMgr) ;

	CNpcRecallMgr(void) ;														// ������ �Լ�.
	~CNpcRecallMgr(void) ;														// �Ҹ��� �Լ�.

	void MTOA_ChangeMap_Syn(DWORD dwIndex, char* pMsg, DWORD dwLength) ;
	void CTOA_MapOut(DWORD dwIndex, char* pMsg, DWORD dwLength) ;

	void Throw_Error(char* szErr, char* szCaption) ;							// ���� �޽��� ó���� �ϴ� �Լ�.
	void WriteLog(char* pMsg) ;													// �α׸� ����� ó���� �ϴ� �Լ�.

};
