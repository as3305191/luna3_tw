#pragma once
//-------------------------------------------------------------------------------------------------
//	NAME		: CNpcRecallBase.h
//	DESC		: NPC�� ��ȯ�ϴ� �⺻ ������ ��� �ִ� Ŭ����.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------------------------
//		Include part.
//-------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------------------------
//		Define some macroinstruction.
//-------------------------------------------------------------------------------------------------
//#define _USE_NPCRECALL_ERRBOX_					// �� ��ũ�ΰ� Ȱ��ȭ �Ǿ� ������, ���� �߻��� �޽��� �ڽ��� ����,
												// ��Ȱ��ȭ ��Ű��, Sworking ���� ��, ���� �α׸� �����.





//-------------------------------------------------------------------------------------------------
//		The class CNpcRecallBase.
//-------------------------------------------------------------------------------------------------
class CNpcRecallBase
{
	BYTE		m_byRecallCodition ;									// ��ȯ ����.

	DWORD		m_dwNpcRecallIdx ;										// NpcRecall �ε��� 
	DWORD		m_dwNpcIdx ;											// Npc �ε���. StaticNpcList.bin�� Unique index�� ����Ѵ�.
	WORD		m_wNpcKind ;											// NpcList.bin���� ������ Kind �ε���.

	MAPTYPE		m_RecallMapNum ;										// ��ȯ �� ��ȣ.

	float		m_fXpos ;												// ��ǥ X.
	float		m_fZpos ;												// ��ǥ Z.
	WORD		m_wDir;													// ����.

	BYTE		m_byActive ;											// Ȱ��ȭ ���θ� ��� ����.

	DWORD		m_dwCreatedIdx ;										// �� �������� ���� �� npc ���� �ε���.

	BYTE		m_byReadyToDelete ;										// NPC�� ������ �غ� �Ǿ����� ���θ� ���� ����.

	DWORD		m_dwRecallTime ;										// Npc�� ��ȯ �� �ð�.

	WORD		m_wRecallFailedMsg ;									// ��ȯ ���� �� ����� ���� �޽��� ��ȣ.

	char		m_NpcName[MAX_NPC_NAME_LENGTH+1];						// Npc�̸� 

	WORD		m_wRecalledChenel;										// ��ȯ�� ü��

	DWORD		m_dwParentNpcIdx;										// �θ� NPC�̸� 

	BYTE		m_byParentType;											// �θ� NPC Type

public:
	enum		eParentNpcType	{	eParentNpcType_UnKnown ,			// �θ� NPC Type ������ 
									eParentNpcType_Static,				
									eParentNpcType_Recall,	
									eParentNpcType_OnlySelf,
									eParentNpcType_Max		} ;


public:
	CNpcRecallBase();
	virtual ~CNpcRecallBase();
	void	Set_RecallCondition(BYTE byCondition) ;						// ��ȯ ������ �����ϴ� �Լ�.
	BYTE	Get_RecallCondition() { return m_byRecallCodition ; }		// ��ȯ ������ ��ȯ�ϴ� �Լ�.

	void	Set_NpcRecallIndex(DWORD m_dwNpcRecallIdx) ;						// NpcRecall �ε����� �����ϴ� �Լ�.
	DWORD	Get_NpcRecallIndex() { return m_dwNpcRecallIdx ; }				// NpcRecall �ε����� ��ȯ�ϴ� �Լ�.

	void	Set_NpcIndex(DWORD dwNpcIdx) ;								// Npc �ε����� �����ϴ� �Լ�.
	DWORD	Get_NpcIndex() { return m_dwNpcIdx ; }						// Npc �ε����� ��ȯ�ϴ� �Լ�.

	void	Set_NpcKind(WORD wNpcKind) ;								// Npc ������ �����ϴ� �Լ�.
	WORD	Get_NpcKind() { return m_wNpcKind ; }						// Npc ������ ��ȯ�ϴ� �Լ�.

	void	Set_RecallMap(MAPTYPE mapNum) ;								// Npc�� ��ȯ �� ��ȣ�� �����ϴ� �Լ�.
	MAPTYPE Get_RecallMap() { return m_RecallMapNum ; }					// Npc�� ��ȯ �� ��ȣ�� ��ȯ�ϴ� �Լ�.

	void	Set_RecallPosX(float fXpos) ;								// Npc�� ��ȯ X��ǥ�� �����ϴ� �Լ�.
	void	Set_RecallPosZ(float fZpos) ;								// Npc�� ��ȯ Z��ǥ�� �����ϴ� �Լ�.
	float	Get_RecallposX() { return m_fXpos ; }						// Npc�� ��ȯ ��ǥ X�� ��ȯ�ϴ� �Լ�.
	float	Get_RecallposZ() { return m_fZpos ; }						// Npc�� ��ȯ ��ǥ Z�� ��ȯ�ϴ� �Լ�.
	void Get_RecallPos(VECTOR3* vOutPut);							// Npc�� ��ȯ ��ǥ�� ��ȯ�ϴ� �Լ�.

	void	Set_RecallDir(WORD wDir) ;									// Npc�� ��ȯ ������ �����ϴ� �Լ�.
	WORD	Get_RecallDir() { return m_wDir ; }							// Npc�� ��ȯ ������ ��ȯ�ϴ� �Լ�.

	// �� �������� ������ npc ���̵� ����/��ȯ�ϴ� �Լ�.
	void	Set_CreatedIdx(DWORD dwCreatedIdx) { m_dwCreatedIdx = dwCreatedIdx ; }
	DWORD	Get_CreatedIdx() { return m_dwCreatedIdx ; }

	// npc�� ������ �غ� �Ǿ����� ���θ� ����/��ȯ�ϴ� �Լ�.
	void	Set_ReadyToDelete(BYTE byReady) { m_byReadyToDelete = byReady ; }
	BYTE	Is_ReadyToDelete() { return m_byReadyToDelete ; }

	// Npc�� ��ȯ �� �ð��� ����/��ȯ �� �Լ�.
	void	Set_RecallTime(DWORD dwRecallTime) { m_dwRecallTime = dwRecallTime ; }
	DWORD	Get_RecallTime() { return m_dwRecallTime ; }

	// Npc ��ȯ ���� �� ����� ���� �޽����� ����/��ȯ�ϴ� �Լ�.
	void	Set_RecallFailedMsg(WORD wMsgNum) { m_wRecallFailedMsg = wMsgNum ; }
	WORD	Get_RecallFailedMsg() { return m_wRecallFailedMsg ; }

	void	Set_NpcName(char* pNpcName) ;
	char*	Get_NpcName() {return m_NpcName;}	

	void	Set_RecalledChenel(WORD wChenel );
	WORD	Get_RecalledChenel(){return m_wRecalledChenel;}

	void	Set_ParentNpcIndex(DWORD dwIndex);
	DWORD	Get_ParentNpcIndex(){return m_dwParentNpcIdx;}

	void	Set_ParentNpcType(BYTE byType);
	DWORD	Get_ParentNpcType(){return m_byParentType;}



	
	////////////////////////////////
	// [ ���� �޽��� ó�� ��Ʈ. ] //
	////////////////////////////////

	void Throw_Error(char* szErr, char* szCaption) ;					// ���� �޽��� ó���� �ϴ� �Լ�.
	void WriteLog(char* pMsg) ;											// �α׸� ����� ó���� �ϴ� �Լ�.
};
