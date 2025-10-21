#pragma once
//-------------------------------------------------------------------------------------------------
//	NAME		: CNpcRecallMoveable.h
//	DESC		: ��ȯ �� ��, ���� �ð��� ������, ���� �� ������ �־��� ��ǥ�� �̵��ϴ� npc Ŭ����.
//				: 1ȸ �̵��� �����ϸ�, ����� �����ʿ��� ���� ����Ʈ�� �̿��Ѵ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------------------------
//		Include part.
//-------------------------------------------------------------------------------------------------
#include "./NpcRecallBase.h"





//-------------------------------------------------------------------------------------------------
//		Define some macroinstruction.
//-------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------------------------
//		The class CNpcRecallMoveable.
//-------------------------------------------------------------------------------------------------
class CNpcRecallMoveable : public CNpcRecallBase
{
	DWORD	m_dwRemainTime ;									// Npc�� ��ȯ ���� �ð�.

	DWORD	m_dwMoveTime ;										// Npc�� �̵��� ���� �� �ð�.

	float	m_fMoveXpos ;										// �̵� �� X ��ǥ.
	float	m_fMoveZpos ;										// �̵� �� Z ��ǥ.

	MAPTYPE m_ChangeMapNum ;									// �̵��� �� ��ȣ.

	float	m_fChangeMapXPos ;									// �̵��� ���� X��ǥ.
	float	m_fChangeMapZPos ;									// �̵��� ���� Z��ǥ.

	BYTE	m_byMoved ;											// �̵��� �ߴ��� ���θ� ���� ����.

public:
	CNpcRecallMoveable(void);
	virtual ~CNpcRecallMoveable(void);
	// Npc�� ��ȯ�� ���� �� �ð��� ����/��ȯ �� �Լ�.
	void	Set_RemainTime(DWORD dwRemainTime) ;
	DWORD	Get_RemainTime() { return m_dwRemainTime ; }


	// Npc�� �̵��� �� �ð��� ����/��ȯ �� �Լ�.
	void	Set_MoveTime(DWORD dwMoveTime) ;
	DWORD	Get_MoveTime() { return m_dwMoveTime ; }


	// Npc�� �̵��� X/Z��ǥ�� ����/��ȯ �� �Լ�.
	void	Set_MoveposX(float fXpos) ;
	void	Set_MoveposZ(float fZpos) ;
	float	Get_MovePosX() { return m_fMoveXpos ; }
	float	Get_MovePosZ() { return m_fMoveZpos ; }


	// �̵��� ���� �� �������� ��ȯ�ϴ� �Լ�.
	BYTE	IsMoveable(DWORD dwCurTime) ;


	// ��ȯ ���� �ð��� �������� ��ȯ�ϴ� �Լ�.
	BYTE	IsEndRemainTime(DWORD dwCurTime) ;


	// npc�� ���� �̵��� ���� �� ��ȣ�� ����/��ȯ�ϴ� �Լ�.
	void	Set_ChangeMapNum(MAPTYPE mapNum) ;
	MAPTYPE Get_ChangeMapNum() { return m_ChangeMapNum ; }


	// npc�� ���� �̵��� ���� X/Z��ǥ�� ����/��ȯ�ϴ� �Լ�.
	void	Set_ChangeMapXPos(float fXpos) ;
	void	Set_ChangeMapZPos(float fZpos) ;
	float	Get_ChangeMapXPos() { return m_fChangeMapXPos ; }
	float	Get_ChangeMapZPos() { return m_fChangeMapZPos ; }


	// npc�� �̵��� �ߴ���, �ƴ��� ���θ� ����/��ȯ�ϴ� �Լ�.
	void	Set_Moved(BYTE byMoved) { m_byMoved = byMoved ; }
	BYTE	Is_Moved() { return m_byMoved ; }


	// ��ȯ�� ���� �� �ð��� �ʱ�ȭ �ϴ� �Լ�.
	void ClearRemainTime() { m_dwRemainTime = 0 ; }
};
