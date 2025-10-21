#include "stdafx.h"
#include "MHFile.h"
#include "SHFamilyManager.h"
#include <io.h>

#if defined(_AGENTSERVER)
	#include "Network.h"
	#include "AgentDBMsgParser.h"
	#include "FilteringTable.h"
	#include "../Farm/SHFarmManager.h"
#elif defined(_MAPSERVER_)
	#include "UserTable.h"
	#include "Player.h"
	#include "PackedData.h"
	#include "Network.h"
	#include "../[CC]Header/GameResourceManager.h"
	#include "MapDBMsgParser.h"
	#include "../Farm/SHFarmManager.h"
	#include "CharMove.h"
	#include "../[CC]Skill/Server/Object/BuffSkillObject.h"
	#include "../[cc]skill/Server/Info/ActiveSkillInfo.h"
	#include "../[cc]skill/Server/Manager/SkillManager.h"
#else
	#include "../[CC]Header/FilteringTable.h"
	#include "Player.h"
	#include "ChatManager.h"
	#include "GameIn.h"
	#include "ObjectManager.h"
	#include "WindowIDEnum.h"
	#include "cMsgBox.h"
	#include "interface/cWindowManager.h"
	#include "FamilyCreateDialog.h"
	#include "FamilyDialog.h"
	#include "ObjectBalloon.h"
	#include "OBalloonName.h"
	#include "../FreeImage/FreeImage.h"
	#include "cImageSelf.h"
	#include "../Monstermeter/SHMonstermeterDlg.h"
	#include "../Farm/SHFarmManageDlg.h"
	#include "MiniMapDlg.h"
	#include "BigMapDlg.h"
	#include "../Farm/SHFarmManager.h"
#endif

extern void ClearDirectory( LPCTSTR pStrName );
extern int g_nServerSetNum;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// class CSHFamilyManager
//
CSHFamilyManager g_csFamilyManager;

// �⺻��(���Ͽ��� �о �ٽ� ������)
DWORD	CSHFamilyManager::HONOR_POINT_CHECK_TIME		= 60*60*1000;  	
int		CSHFamilyManager::HONOR_POINT_CHECK_MEMBER_NUM	= 2;
int		CSHFamilyManager::HONOR_POINT_CHECK_POINT		= 10;
float	CSHFamilyManager::MONKILL_HONOR_POINT_EXP_RATE	= 0.01f;
float	CSHFamilyManager::LEAVE_PENALTY_EXP_DOWN_RATE[FLK_MAX]	= {0.03f,};
int		CSHFamilyManager::LEAVE_PENALTY_REJOIN_TIME[FLK_MAX] = {24,};

// -------------------------------------------------------------------------------------------------------------------------------------
// CSHFamilyManager Method																										  ������
//
CSHFamilyManager::CSHFamilyManager()
{
	ZeroMemory(m_pcsFamily, sizeof(m_pcsFamily));
	m_nFamilyNum = 0;
	m_nInviteMasterID = NULL;


// 090213 LYW --- SHFamilyManager : �йи� �ɹ� ���� ���� ���� �߰� ��� ó��.
#ifdef _MAPSERVER_	
	m_byMinMemberCnt	= 0 ;
	m_wRewardExprate	= 0 ;
	m_dwRewardExpValue	= 0 ;

	memset( m_dwRewardBuffIdxs, 0, sizeof(DWORD)*CSHFamily::MAX_MEMBER_NUM ) ;
#endif //_MAPSERVER_


	LoadFamilyInfo();
#if !defined(_AGENTSERVER) && !defined(_MAPSERVER_)
	ClearDirectory("Data/Interface/2DImage/FamilyEmblem");
	CreateDirectory("Data/Interface/2DImage/FamilyEmblem", NULL);
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
// ~CSHFamilyManager Method																										  �ı���
//
CSHFamilyManager::~CSHFamilyManager()
{
	for(int i=0; i<MAX_FAMILY_UNIT_NUM; i++)
	{
		SAFE_DELETE_ARRAY(m_pcsFamily[i]);
	}
}

// -------------------------------------------------------------------------------------------------------------------------------------
// LoadFamilyInfo Method																								�йи� ���� �б�
//
BOOL CSHFamilyManager::LoadFamilyInfo()
{
	char szLine[MAX_PATH], szFile[MAX_PATH];
	int	 nKind = 0, nCnt = 0;
	CMHFile fp;

	sprintf(szFile, "./System/Resource/Family.bin");
	fp.Init(szFile, "rb");
	if(!fp.IsInited()) return FALSE;

	while(FALSE == fp.IsEOF())
	{
		fp.GetLine(szLine, sizeof(szLine));
		if (strstr(szLine, "//") ||
			IsEmptyLine(szLine))
		{
			continue;			
		}
		else if (strstr(szLine, "END_KIND")) 
		{
			nCnt = 0;
			nKind++;
			continue;
		}

		switch(nKind)
		{
		case 0:
			sscanf(szLine, "%d %d %d %f",		&HONOR_POINT_CHECK_TIME,
												&HONOR_POINT_CHECK_MEMBER_NUM,
												&HONOR_POINT_CHECK_POINT,
												&MONKILL_HONOR_POINT_EXP_RATE);
			HONOR_POINT_CHECK_TIME = HONOR_POINT_CHECK_TIME*60*1000;
			break;
		case 1:
			sscanf(szLine, "%f %f %f %d %d %d",			&LEAVE_PENALTY_EXP_DOWN_RATE[FLK_LEAVE],
														&LEAVE_PENALTY_EXP_DOWN_RATE[FLK_EXPEL],
														&LEAVE_PENALTY_EXP_DOWN_RATE[FLK_BREAKUP],
														&LEAVE_PENALTY_REJOIN_TIME[FLK_LEAVE],
														&LEAVE_PENALTY_REJOIN_TIME[FLK_EXPEL],
														&LEAVE_PENALTY_REJOIN_TIME[FLK_BREAKUP]);
			break;

		// 090213 LYW --- SHFamilyManager : �йи� �ɹ� ���� ���� ���� �߰� ��� ó��.
#ifdef _MAPSERVER_
		// ����ġ�� ������ ������ �ּ� �α��� ��� �� �ε�.
		case 2 :
			{
				m_byMinMemberCnt = 	(BYTE)atoi(szLine) ;
			}
			break ;

		// ���� �� ��� ���� ���� �߰��Ǵ� ����ġ ����� ���� ��ġ �ε�.
		case 3 :
			{
				sscanf( szLine, "%hu %d", &m_wRewardExprate, &m_dwRewardExpValue ) ;
			}
			break ;

		// ���� �� ��� ���� ���� �߰��Ǵ� ���� ó���� ���� �����ε��� �ε�.
		case 4 :
			{
				sscanf( szLine, "%d %d %d %d %d",	&m_dwRewardBuffIdxs[0], 
													&m_dwRewardBuffIdxs[1], 
													&m_dwRewardBuffIdxs[2], 
													&m_dwRewardBuffIdxs[3], 
													&m_dwRewardBuffIdxs[4] ) ;
			}
			break ;
#else 
			// ����ġ�� ������ ������ �ּ� �α��� ��� �� �ε�.
		case 2 :
			{
				BYTE byTempVal = 0 ;
				byTempVal = (BYTE)atoi(szLine) ;
			}
			break ;

			// ���� �� ��� ���� ���� �߰��Ǵ� ����ġ ����� ���� ��ġ �ε�.
		case 3 :
			{
				WORD  wTempVal  = 0 ;
				DWORD dwTempVal = 0 ;
				sscanf( szLine, "%hu %d", &wTempVal, &dwTempVal ) ;
			}
			break ;

			// ���� �� ��� ���� ���� �߰��Ǵ� ���� ó���� ���� �����ε��� �ε�.
		case 4 :
			{
				DWORD dwTempArray[CSHFamily::MAX_MEMBER_NUM] = {0, } ;
				sscanf( szLine, "%d %d %d %d %d",	
					&dwTempArray[0], 
					&dwTempArray[1], 
					&dwTempArray[2], 
					&dwTempArray[3], 
					&dwTempArray[4] ) ;
			}
			break ;
#endif // _MAPSERVER_
		}
		
		nCnt++;
	}
	fp.Release();

	return TRUE;
}

// -------------------------------------------------------------------------------------------------------------------------------------
// AddFamilyToTbl Method																							���̺� �йи� �߰�
//
CSHFamily* CSHFamilyManager::AddFamilyToTbl(CSHFamily* pcsFamily)
{
	// ���̺� �̹� �����ϴ��� üũ
	CSHFamily* pstAleadyFamily = GetFamily(pcsFamily->Get()->nID);
	if (pstAleadyFamily)
	{
		return pstAleadyFamily;
	}

	// �йи� �߰�
	//
	// ..�йи� ������ �ִ밪�� �Ѵ´ٸ�...
	if (m_nFamilyNum >= MAX_FAMILY_UNIT_NUM*MAX_FAMILY_1U_NUM)
	{
		return NULL;
	}

	// �����Ǿ� �� ������ ������ �� ���� �߰�
	for(int i=0; i<m_nFamilyNum; i++)
	{
		int nUnitIndex = i/MAX_FAMILY_1U_NUM;
		int n1UIndex = i%MAX_FAMILY_1U_NUM;

		if (m_pcsFamily[nUnitIndex][n1UIndex].Get()->nID == NULL)
		{
			m_pcsFamily[nUnitIndex][n1UIndex].Set(pcsFamily);
			m_pcsFamily[nUnitIndex][n1UIndex].SetIndexAtTbl(i);
			return &m_pcsFamily[nUnitIndex][n1UIndex];
		}
	}

	int nUnitIndex = m_nFamilyNum/MAX_FAMILY_1U_NUM;
	int n1UIndex = m_nFamilyNum%MAX_FAMILY_1U_NUM;
	// ..������ �Ѿ��(Ȥ�� ó���̸�) �޸� �Ҵ�
	if (n1UIndex == 0)
	{
		m_pcsFamily[nUnitIndex] = new CSHFamily[MAX_FAMILY_1U_NUM];
	}

	m_pcsFamily[nUnitIndex][n1UIndex].Set(pcsFamily);
	m_pcsFamily[nUnitIndex][n1UIndex].SetIndexAtTbl(m_nFamilyNum);
	m_nFamilyNum++;

	return &m_pcsFamily[nUnitIndex][n1UIndex];
}

// -------------------------------------------------------------------------------------------------------------------------------------
// DelFamilyToTbl Method																						  ���̺��� �йи� ����
//
BOOL CSHFamilyManager::DelFamilyToTbl(DWORD nPlayerID, CSHFamily* pcsFamily)
{
	// ���̺� �̹� �����ϴ��� üũ
	if (pcsFamily)
	{
		CSHFamily* pstAleadyFamily = GetFamily(pcsFamily->Get()->nID);
		if (pstAleadyFamily)
		{
			// �α׾ƿ� ó��
			int	nLogoutCnt = 0;
			// ..�α׾ƿ� �ϴ� ����� ���� ���� ����
			for(UINT i=0; i<pcsFamily->Get()->nMemberNum; i++)
			{
				if (nPlayerID == pcsFamily->GetMember(i)->Get()->nID)
				{
					CSHFamilyMember::stINFO stMemberInfo = *pcsFamily->GetMember(i)->Get();

					// ���� ���� ����
					stMemberInfo.eConState = CSHFamilyMember::MEMBER_CONSTATE_LOGOFF;
					pcsFamily->GetMember(i)->Set(&stMemberInfo);
				}

				// �α׾ƿ��� ��� �� üũ
				if (pcsFamily->GetMember(i)->Get()->eConState == CSHFamilyMember::MEMBER_CONSTATE_LOGOFF)
				{
					nLogoutCnt++;
				}
			}

			// ..���� ������ �ִ� ����� HONOR_POINT_CHECK_MEMBER_NUM�� �̸��� �Ǹ� �� ����Ʈ �ð� �ʱ�ȭ
			if (pcsFamily->Get()->nMemberNum-nLogoutCnt < (UINT)CSHFamilyManager::HONOR_POINT_CHECK_MEMBER_NUM)
			{
				CSHFamily::stINFO_EX stInfoEx = *pcsFamily->GetEx();
				stInfoEx.nHonorPointTimeTick = 0;
				pcsFamily->SetEx(&stInfoEx);
			}

			// ..��� ����� �α׾ƿ� ������ ���̺��� �йи� ����
			if ((UINT)nLogoutCnt == pcsFamily->Get()->nMemberNum)
			{
				int nUnitIndex = pcsFamily->GetIndexAtTbl()/MAX_FAMILY_1U_NUM;
				int n1UIndex = pcsFamily->GetIndexAtTbl()%MAX_FAMILY_1U_NUM;

				m_pcsFamily[nUnitIndex][n1UIndex].Set(pcsFamily);
				m_pcsFamily[nUnitIndex][n1UIndex].SetIndexAtTbl(NULL);
				return TRUE;
			}
		}
	}

	return FALSE;
}

// -------------------------------------------------------------------------------------------------------------------------------------
// DelFamilyToTbl Method																						  ���̺��� �йи� ����
//
BOOL CSHFamilyManager::DelFamilyToTbl(CSHFamily* pcsFamily)
{
	// ���̺� �̹� �����ϴ��� üũ
	if (pcsFamily)
	{
		CSHFamily* pstAleadyFamily = GetFamily(pcsFamily->Get()->nID);
		if (pstAleadyFamily)
		{
			int nUnitIndex = pcsFamily->GetIndexAtTbl()/MAX_FAMILY_1U_NUM;
			int n1UIndex = pcsFamily->GetIndexAtTbl()%MAX_FAMILY_1U_NUM;

			CSHFamily csFamily;
			m_pcsFamily[nUnitIndex][n1UIndex].Set(&csFamily);
			m_pcsFamily[nUnitIndex][n1UIndex].SetIndexAtTbl(NULL);
			return TRUE;
		}
	}
	return FALSE;
}

// -------------------------------------------------------------------------------------------------------------------------------------
// SetFamily Method																											 �йи� ����
//
VOID CSHFamilyManager::SetFamily(CSHFamily* pcsFamily)
{
	int nUnitIndex = pcsFamily->GetIndexAtTbl()/MAX_FAMILY_1U_NUM;
	int n1UIndex = pcsFamily->GetIndexAtTbl()%MAX_FAMILY_1U_NUM;

	m_pcsFamily[nUnitIndex][n1UIndex].Set(pcsFamily);
}

// -------------------------------------------------------------------------------------------------------------------------------------
// GetFamily Method																											 �йи� ���
//
CSHFamily* CSHFamilyManager::GetFamily(DWORD nFamilyID)
{
	if(0 == nFamilyID)
	{
		return 0;
	}

	int nUnitIndex = 0, n1UIndex = 0;

	for(int i=0; i<m_nFamilyNum; i++)
	{
		nUnitIndex = i/MAX_FAMILY_1U_NUM;
		n1UIndex = i%MAX_FAMILY_1U_NUM;

		if (m_pcsFamily[nUnitIndex][n1UIndex].Get()->nID == nFamilyID)
		{
			return &m_pcsFamily[nUnitIndex][n1UIndex];
		}
	}

	return NULL;
}

// -------------------------------------------------------------------------------------------------------------------------------------
// GetFamily Method																											 �йи� ���
//
CSHFamily* CSHFamilyManager::GetFamily(char* pszName)
{
	int nUnitIndex = 0, n1UIndex = 0;
	for(int i=0; i<m_nFamilyNum; i++)
	{
		nUnitIndex = i/MAX_FAMILY_1U_NUM;
		n1UIndex = i%MAX_FAMILY_1U_NUM;

		if (m_pcsFamily[nUnitIndex][n1UIndex].Get()->nID)
		{
			if (stricmp(m_pcsFamily[nUnitIndex][n1UIndex].Get()->szName, pszName) == 0)
			{
				return &m_pcsFamily[nUnitIndex][n1UIndex];
			}
		}
	}

	return NULL;
}

// -------------------------------------------------------------------------------------------------------------------------------------
// GetFamilyByMasterID Method																								 �йи� ���
//
CSHFamily* CSHFamilyManager::GetFamilyByMasterID(DWORD nMasterID)
{
	int nUnitIndex = 0, n1UIndex = 0;
	for(int i=0; i<m_nFamilyNum; i++)
	{
		nUnitIndex = i/MAX_FAMILY_1U_NUM;
		n1UIndex = i%MAX_FAMILY_1U_NUM;

		if (m_pcsFamily[nUnitIndex][n1UIndex].Get()->nID)
		{
			if (m_pcsFamily[nUnitIndex][n1UIndex].Get()->nMasterID == nMasterID)
			{
				return &m_pcsFamily[nUnitIndex][n1UIndex];
			}
		}
	}

	return NULL;
}

// -------------------------------------------------------------------------------------------------------------------------------------
// Process Method																											
//
VOID CSHFamilyManager::Process()
{
#if defined(_AGENTSERVER)
	static DWORD nextCheckTick;

	if (gCurTime > nextCheckTick)
	{
		// 090915 LUJ, 30�� �������� �˻�
		const DWORD space = 30 * 60 * 1000;
		nextCheckTick = gCurTime + space;
		int nUnitIndex = 0, n1UIndex = 0;

		for(int i=0; i<m_nFamilyNum; i++)
		{
			nUnitIndex = i/MAX_FAMILY_1U_NUM;
			n1UIndex = i%MAX_FAMILY_1U_NUM;

			if (m_pcsFamily[nUnitIndex][n1UIndex].Get()->nID)
			{
				if (m_pcsFamily[nUnitIndex][n1UIndex].GetEx()->nHonorPointTimeTick)
				{
					if (gCurTime - m_pcsFamily[nUnitIndex][n1UIndex].GetEx()->nHonorPointTimeTick > HONOR_POINT_CHECK_TIME)
					{
						// �� ����Ʈ ����
						CSHFamily::stINFO_EX	stInfoEx = *m_pcsFamily[nUnitIndex][n1UIndex].GetEx();
						stInfoEx.nHonorPoint += HONOR_POINT_CHECK_POINT;
						stInfoEx.nHonorPointTimeTick = gCurTime;

						if (stInfoEx.nHonorPoint > 4200000000) stInfoEx.nHonorPoint = 4200000000;
						m_pcsFamily[nUnitIndex][n1UIndex].SetEx(&stInfoEx);

						// DB�� ����
						Family_SaveInfo(m_pcsFamily[nUnitIndex][n1UIndex].Get()->nMasterID, 
										m_pcsFamily[nUnitIndex][n1UIndex].Get()->szName, 
										m_pcsFamily[nUnitIndex][n1UIndex].GetEx()->nHonorPoint, 
										m_pcsFamily[nUnitIndex][n1UIndex].GetEx()->nNicknameON,
										1);

						// 071114 ����, �α�
						{
							CSHFamily*	family	= &( m_pcsFamily[nUnitIndex][n1UIndex] );
							ASSERT( family );

							InsertLogFamilyPoint( family, eLog_FamilyPointAdd );
						}

						// Ŭ���̾�Ʈ�� �˸���
						{
							MSG_DWORD stPacket;
							for(UINT i=0; i<m_pcsFamily[nUnitIndex][n1UIndex].Get()->nMemberNum; i++)
							{
								USERINFO* pMemberUserInfo = g_pUserTableForObjectID->FindUser(m_pcsFamily[nUnitIndex][n1UIndex].GetMember(i)->Get()->nID);
								if (pMemberUserInfo)
								{
									stPacket.Category			= MP_FAMILY;
									stPacket.Protocol			= MP_FAMILY_HONOR_POINT;
									stPacket.dwObjectID			= pMemberUserInfo->dwCharacterID;
									stPacket.dwData				= HONOR_POINT_CHECK_POINT;

									g_Network.Send2User( pMemberUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket) );
								}
							}
						}

						// �ٸ� ������Ʈ�� �˸���
						MSG_NAME_DWORD3 stPacket;
						ZeroMemory(&stPacket, sizeof(stPacket));

						stPacket.Category			= MP_FAMILY;
						stPacket.Protocol			= MP_FAMILY_HONOR_POINT_TO_OTHER_AGENT;
						stPacket.dwData1			= m_pcsFamily[nUnitIndex][n1UIndex].Get()->nID;
						stPacket.dwData2			= HONOR_POINT_CHECK_POINT;
						stPacket.dwData3			= 1;
						g_Network.Broadcast2AgentServerExceptSelf((char*)&stPacket, sizeof(stPacket));	
					}
				}
			}
		}
	}

	if(false == mReservedTransferContainer.empty())
	{
		const TransferredFamily& transferredFamily = mReservedTransferContainer.front();
		const TickCount registeredTime = transferredFamily.first;
		const FamilyIndex familyIndex = transferredFamily.second;

		if(gCurTime > registeredTime)
		{
			ASRV_SendFamilyInfoToMapServerReal(
				GetFamily(familyIndex));

			mReservedTransferContainer.pop();
		}
	}
#endif
}
// -------------------------------------------------------------------------------------------------------------------------------------
// ASRV_CreateFamily Method																									 �йи� ����
//
VOID CSHFamilyManager::ASRV_CreateFamily(USERINFO* pUserInfo, char* szFamilyName, int nCheckState, CSHFamily* pcsFamily)
{
#if defined(_AGENTSERVER)
	RESULT eResult = RESULT_OK;
	int nPastTime = 0;

	if(GetFamily(pUserInfo->mFamilyIndex))
	{
		eResult = RESULT_FAIL_01;
		goto FAILED;
	}
	else if(0 == szFamilyName)
	{
		eResult = RESULT_FAIL_02;
		goto FAILED;
	}

	// �̸��� �ִ� ���̸� �ʰ��߳�?
	if (strlen(szFamilyName) > MAX_NAME_LENGTH)
	{
		eResult = RESULT_FAIL_02;
		goto FAILED;
	}

	// ����� �� ���� �̸��ΰ�?
	if((FILTERTABLE->IsInvalidCharInclude((unsigned char*)szFamilyName)))
	{
		eResult = RESULT_FAIL_02;
		goto FAILED;
	}

	// Ż�� ���Ƽ ������ ������?
	// ..DB�� int �� 0710111524(����Ͻð���)�� ���� �����. ���� int �� ���� ��¥������ ���游ŭ ���� ���� ����...��
	if (pUserInfo->nFamilyLeaveDate)
	{
		nPastTime = GetPastTime(pUserInfo->nFamilyLeaveDate);
		// ���� �ð� üũ
		if (nPastTime < LEAVE_PENALTY_REJOIN_TIME[pUserInfo->nFamilyLeaveKind])
		{
			eResult = RESULT_FAIL_03;
			switch(pUserInfo->nFamilyLeaveKind)
			{
			case FLK_LEAVE:		eResult = RESULT_FAIL_03;	break;
			case FLK_EXPEL:		eResult = RESULT_FAIL_04;	break;
			case FLK_BREAKUP:	eResult = RESULT_FAIL_05;	break;
			}
			goto FAILED;
		}
		// ���� �ʱ�ȭ
		Family_Leave_SaveInfo(pUserInfo->dwCharacterID, pUserInfo->nFamilyLeaveKind, TRUE);
	}

	switch(nCheckState)
	{
	// �̸� üũ1
	case FCS_NAME_CHECK1:
		{
			// �̹� DB�� �����ϴ� �йи� �̸����� üũ ������ DB�� ������.
			Family_CheckName(pUserInfo->dwCharacterID, szFamilyName);
			return;
		}
		break;

	// �̸� üũ1 ����
	case FCS_NAME_ERR:
		{
			eResult = RESULT_FAIL_02;
			goto FAILED;
		}
		break;

	// �̸� üũ2
	case FCS_NAME_CHECK2:
		{
			// DB���� ������, Ȥ�� ������ ���̺� �����ϴ� �̸��ΰ�?
			// �̸� üũ ������ DB�� ������ �� ������ �ʾ����� ���� �߻��� ��쿡�� DB üũ�����δ� ��Ȯ���� ������ �� ����. Ư���� ��Ƽ�������� ���.
			// �׷��� ������ ���̺� üũ�Ѵ�.
			if (GetFamily(szFamilyName))
			{
				eResult = RESULT_FAIL_02;
				goto FAILED;
			}

			// �ʼ����� ������ �̿��� ���� üũ�� ���� �ʼ����� �йи� ���� ��Ŷ�� ������.
			MSG_NAME stPacket;

			stPacket.Category			= MP_FAMILY;
			stPacket.Protocol			= MP_FAMILY_CREATE;
			stPacket.dwObjectID			= pUserInfo->dwCharacterID;
			SafeStrCpy(stPacket.Name, szFamilyName, MAX_NAME_LENGTH+1);
			g_Network.Send2Server(pUserInfo->dwMapServerConnectionIndex, (char*)&stPacket, sizeof(stPacket) );
			return;
		}
		break;

	// ����
	case FCS_CREATE:
		{
			// �ٷ� �����ϴ� �� �ƴϰ�, ������ ������ �̿��ؼ� DB�� ������ �õ��Ѵ�.
			CSHFamily csFamily(pUserInfo->dwCharacterID, szFamilyName);
			Family_SaveInfo(pUserInfo->dwCharacterID, szFamilyName, csFamily.GetEx()->nHonorPoint, csFamily.GetEx()->nNicknameON);
			return;
		}
		break;

	// �����Ϸ�
	case FCS_COMPLETE:
		{
			if(CSHFamily* const family = AddFamilyToTbl(pcsFamily))
			{
				pUserInfo->mFamilyIndex = family->Get()->nID;

				ASRV_SendInfoToClient(
					family);
			}
		}
		break;
	}

FAILED:
	// Ŭ���̾�Ʈ�� �˸���
	MSG_DWORD2 stPacket;

	stPacket.Category			= MP_FAMILY;
	stPacket.Protocol			= MP_FAMILY_CREATE;
	stPacket.dwObjectID			= pUserInfo->dwCharacterID;
	stPacket.dwData1			= eResult;
	stPacket.dwData2			= nPastTime;
	g_Network.Send2User( pUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket) );
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  ASRV_ParseRequestFromClient Method																 Ŭ���̾�Ʈ ��û �м�/ó��(������Ʈ)
//
VOID CSHFamilyManager::ASRV_ParseRequestFromClient(DWORD dwConnectionID, char* pMsg, DWORD dwLength)
{
#if defined(_AGENTSERVER)
	MSGBASE* pTempMsg = (MSGBASE*)pMsg;
	
	// Ŭ���̾�Ʈ���� ��Ŷ �������� ���� ĳ���� ID �� ���� �� �ֱ� ������
	// g_pUserTableForObjectID->FindUser �� UserInfo �� ���ϸ� �� ��!!
	USERINFO* pUserInfo = g_pUserTable->FindUser( dwConnectionID );
	if( pUserInfo == NULL ) return;
	pTempMsg->dwObjectID = pUserInfo->dwCharacterID;

	switch( pTempMsg->Protocol )
	{
	// �йи� ����
	case MP_FAMILY_CREATE:
		{
			MSG_DWORD_NAME* pstPacket = (MSG_DWORD_NAME*)pMsg;

			ASRV_CreateFamily(pUserInfo, pstPacket->Name, CSHFamilyManager::FCS_NAME_CHECK1);
		}
		break;
	// ����� �ʴ�
	case MP_FAMILY_INVITE:
		{
			MSG_DWORD* pstPacket = (MSG_DWORD*)pMsg;

			ASRV_Invite(pUserInfo, pstPacket->dwData);
		}
		break;
	// ȣĪ ����
	case MP_FAMILY_CHANGE_NICKNAME:
		{
			MSG_DWORD_NAME* pstPacket = (MSG_DWORD_NAME*)pMsg;

			ASRV_ChangeNickname(pUserInfo, pstPacket->dwData, pstPacket->Name);
		}
		break;
	// ���� ���
	case MP_FAMILY_REGIST_EMBLEM:
		{
			PACKET_FAMILY_EMBLEM* pstPacket = (PACKET_FAMILY_EMBLEM*)pMsg;

			ASRV_RegistEmblem(pUserInfo, pstPacket->nFamilyID, pstPacket->cImgData);
		}
		break;
	// 091111 ONS ���� ����
	case MP_FAMILY_DEL_EMBLEM:
		{
			MSG_DWORD* pstPacket = (MSG_DWORD*)pMsg;

			ASRV_DeleteEmblem(pUserInfo, pstPacket->dwData);
		}
		break;
	// Ż��
	case MP_FAMILY_LEAVE:
		{
			ASRV_Leave(pUserInfo);
		}
		break;
	// �߹�
	case MP_FAMILY_EXPEL:
		{
			MSG_DWORD* pstPacket = (MSG_DWORD*)pMsg;

			ASRV_Expel(pUserInfo, pstPacket->dwData);
		}
		break;
	// ��ü
	case MP_FAMILY_BREAKUP:
		{
			ASRV_BreakUp(pUserInfo);
		}
		break;
	// 091126 ONS �йи��� �̾� ��� �߰�
	case MP_FAMILY_TRANSFER:
		{
			MSG_DWORD* pstPacket = (MSG_DWORD*)pMsg;

			ASRV_Transfer(pUserInfo, pstPacket->dwData);
		}
		break;
	default:
		g_Network.Send2Server( pUserInfo->dwMapServerConnectionIndex, pMsg, dwLength );
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  ASRV_ParseRequestFromServer Method																	   ���� ��û �м�/ó��(������Ʈ)
//
VOID CSHFamilyManager::ASRV_ParseRequestFromServer(DWORD dwConnectionID, char* pMsg, DWORD dwLength)
{
#if defined(_AGENTSERVER)
	MSGBASE* pTempMsg = (MSGBASE*)pMsg;
	
	// USERINFO �� �������� �ʴ� ���
	//
	switch( pTempMsg->Protocol )
	{
	// �ٸ� ������Ʈ�κ����� ��� ���� ó��
	case MP_FAMILY_MEMBER_CONNECTION_TO_OTHER_AGENT:
		{
			MSG_DWORD4* pstPacket = (MSG_DWORD4*)pMsg;

			ASRV_MemberConFromOtherAgent(pstPacket->dwData1, pstPacket->dwData2, pstPacket->dwData3, pstPacket->dwData4);
		}
		return;
	// �ٸ� ������Ʈ�κ����� ȣĪ ���� �˸�
	case MP_FAMILY_CHANGE_NICKNAME:
		{
			MSG_NAME_DWORD3* pstPacket = (MSG_NAME_DWORD3*)pMsg;

			ASRV_ChangeNicknameFromOtherAgent(pstPacket->dwData1, pstPacket->dwData2, pstPacket->dwData3, pstPacket->Name);
		}
		return;
	// �ٸ� ������Ʈ�κ����� ���� �÷��̾� ���� ������Ʈ
	case MP_FAMILY_UPDATE_MAP_PLAYER_INFO_TO_OTHER_AGENT:
		{
			MSG_NAME_DWORD3* pstPacket = (MSG_NAME_DWORD3*)pMsg;

			ASRV_UpdateMapPlayerInfoFromOtherAgent(pstPacket->dwData1, pstPacket->dwData2, pstPacket->dwData3, pstPacket->Name);
		}
		return;
	// �ٸ� ������Ʈ�κ����� �� ����Ʈ
	case MP_FAMILY_HONOR_POINT_TO_OTHER_AGENT:
		{
			MSG_DWORD3* pstPacket = (MSG_DWORD3*)pMsg;

			ASRV_UpdateHonorPointFromOtherAgent(pstPacket->dwData1, pstPacket->dwData2, pstPacket->dwData3);
		}
		return;
	// �ٸ� ������Ʈ�κ����� �ʴ� ���
	case MP_FAMILY_INVITE_RESULT_TO_OTHER_AGENT:
		{
			PACKET_ADD_MEMBER* pstPacket = (PACKET_ADD_MEMBER*)pMsg;

			ASRV_InviteResultFromOtherAgent(pstPacket);
		}
		return;
	// �ٸ� ������Ʈ�κ����� �ʴ� ���
	case MP_FAMILY_INVITE_RESULT_TO_OTHER_AGENT_3:
		{
			PACKET_FAMILY_TOTAL* pstPacket = (PACKET_FAMILY_TOTAL*)pMsg;

			ASRV_InviteResultFromOtherAgent2(pstPacket);
		}
		return;
	// �ٸ� ������Ʈ�κ����� ���� ���
	case MP_FAMILY_REGIST_EMBLEM_TO_OTHER_AGENT:
		{
			PACKET_FAMILY_EMBLEM* pstPacket = (PACKET_FAMILY_EMBLEM*)pMsg;

			ASRV_RegistEmblemFromOtherAgent(pstPacket->nFamilyID, pstPacket->cImgData);
		}
		return;
	// 091111 ONS �ٸ� ������Ʈ�κ����� ���� ����
	case MP_FAMILY_DEL_EMBLEM_TO_OTHER_AGENT:
		{
			MSG_DWORD* pPacket = (MSG_DWORD*)pMsg;
			ASRV_DeleteEmblemFromOtherAgent(pPacket->dwData);
		}
		return;
	// �ٸ� ������Ʈ�κ����� Ż��
	case MP_FAMILY_LEAVE_TO_OTHER_AGENT:
		{
			MSG_DWORD* pstPacket = (MSG_DWORD*)pMsg;

			ASRV_LeaveFromOtherAgent(pstPacket->dwData, pstPacket->dwObjectID);
		}
		return;
	// �ٸ� ������Ʈ�κ����� �߹�
	case MP_FAMILY_EXPEL_TO_OTHER_AGENT:
		{
			MSG_DWORD2* pstPacket = (MSG_DWORD2*)pMsg;

			ASRV_ExpelFromOtherAgent(pstPacket->dwData1, pstPacket->dwObjectID);
		}
		return;
	// �ٸ� ������Ʈ�κ����� ��ü
	case MP_FAMILY_BREAKUP_TO_OTHER_AGENT:
		{
			MSG_DWORD2* pstPacket = (MSG_DWORD2*)pMsg;

			ASRV_BreakUpFromOtherAgent(pstPacket->dwData1);
		}
		return;

	// 090213 LYW --- SHFamilyManager : �йи� ��� ���� ���� ���� �߰� ��� ó��.
	case MP_FAMILY_REFRESH_FAMILYINFO_MAPSERVER :
		{
			ASRV_RefreshFamilyInfo( pMsg ) ;
		}
		return ;

	// 091126 ONS �йи��� �̾� ����߰�
	case MP_FAMILY_TRANSFER_TO_OTHER_AGENT:
		{
			// 100218 ONS �йи��� �̾� �ٸ� ������Ʈ ó�� ���� ����.
			ASRV_TransferFromOtherAgent(pMsg);
		}
		return ;
		// 100809 ONS Ŭ�����ʱ�ȭ ó���� �йи� ������ ����ó�� �߰�
	case MP_FAMILY_CHANGE_CLASS:
		{
			MSG_DWORD3* pstPacket = (MSG_DWORD3*)pMsg;

			ASRV_ChangeClassInfo(pstPacket->dwObjectID, pstPacket->dwData1, pstPacket->dwData2, pstPacket->dwData3);
		}
		return;
	}


	USERINFO* pUserInfo = g_pUserTableForObjectID->FindUser(pTempMsg->dwObjectID);
	if (pUserInfo == NULL) return;


	switch( pTempMsg->Protocol )
	{
	// �йи� ����
	case MP_FAMILY_CREATE:
		{
			MSG_DWORD_NAME* pstPacket = (MSG_DWORD_NAME*)pMsg;

			if (pstPacket->dwData == RESULT_OK)
			{
				// ��� ������ �����ؼ� �йи� ����!
				ASRV_CreateFamily(pUserInfo, pstPacket->Name, CSHFamilyManager::FCS_CREATE);
			}
		}
		break;
	// �ʴ� ���
	case MP_FAMILY_INVITE_RESULT:
		{
			PACKET_ADD_MEMBER* pstPacket = (PACKET_ADD_MEMBER*)pMsg;

			ASRV_InviteResult(pUserInfo, pstPacket);
		}
		break;
	// �� ����Ʈ
	case MP_FAMILY_HONOR_POINT:
		{
			MSG_DWORD* pstPacket = (MSG_DWORD*)pMsg;

			ASRV_UpdateHonorPoint(pUserInfo, pstPacket->dwData);
		}
		break;
	// ���� �÷��̾� ���� ������Ʈ
	case MP_FAMILY_UPDATE_MAP_PLAYER_INFO:
		{
			MSG_DWORD_NAME* pstPacket = (MSG_DWORD_NAME*)pMsg;

			ASRV_UpdateMapPlayerInfo(pUserInfo, pstPacket->dwData, pstPacket->Name);

		}
		break;
	// �ٸ� ������Ʈ�κ����� �ʴ�
	case MP_FAMILY_INVITED:
		{
			MSG_DWORD_NAME* pstPacket = (MSG_DWORD_NAME*)pMsg;

			ASRV_InviteFromOtherAgent(pUserInfo, pstPacket->dwData, pstPacket->Name);
		}
		break;

	default:
		g_Network.Send2User( pUserInfo->dwConnectionIndex, (char*)pMsg, dwLength );
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
// ASRV_Invite Method																							  �йи��� �ʴ�(������Ʈ)
//
VOID CSHFamilyManager::ASRV_Invite(USERINFO* pUserInfo, DWORD nTargetPlayerID)
{
#if defined(_AGENTSERVER)
	RESULT eResult = RESULT_OK;
	int nPastTime = 0;

	USERINFO* pTargetUserInfo = g_pUserTableForObjectID->FindUser(nTargetPlayerID);
	CSHFamily* const family = g_csFamilyManager.GetFamily(
		pUserInfo->mFamilyIndex);

	if(0 == family)
	{
		eResult = RESULT_FAIL_01;
		goto FAILED;
	}
	else if(pUserInfo->dwCharacterID != family->Get()->nMasterID)
	{
		eResult = RESULT_FAIL_02;
		goto FAILED;
	}
	else if(family->Get()->nMemberNum == family->GetMemberNumMax())
	{
		eResult = RESULT_FAIL_03;
		goto FAILED;
	}

	// �߹� ���Ƽ ������ ������?
	// ..DB�� int �� 0710111524(����Ͻð���)�� ���� �����. ���� int �� ���� ��¥������ ���游ŭ ���� ���� ����...��
	if (pUserInfo->nFamilyLeaveDate)
	{
		nPastTime = GetPastTime(pUserInfo->nFamilyLeaveDate);
		// ���� �ð� üũ
		if (nPastTime < LEAVE_PENALTY_REJOIN_TIME[pUserInfo->nFamilyLeaveKind])
		{
			eResult = RESULT_FAIL_05;
			goto FAILED;
		}
		// ���� �ʱ�ȭ
		Family_Leave_SaveInfo(pUserInfo->dwCharacterID, pUserInfo->nFamilyLeaveKind, TRUE);
	}
	
	if (pTargetUserInfo)
	{
		if(GetFamily(pTargetUserInfo->mFamilyIndex))
		{
			eResult = RESULT_FAIL_04;
			goto FAILED;
		}

		// Ż�� ���Ƽ ������ ������?
		// ..DB�� int �� 2007101115(����Ͻð�)�� ���� �����. ���� int �� ���� ��¥������ ���游ŭ ���� ���� ����...��
		if (pTargetUserInfo->nFamilyLeaveDate)
		{
			nPastTime = GetPastTime(pTargetUserInfo->nFamilyLeaveDate);
			// ���� �ð� üũ
			if (nPastTime < LEAVE_PENALTY_REJOIN_TIME[pTargetUserInfo->nFamilyLeaveKind])
			{
				eResult = RESULT_FAIL_06;
				goto FAILED;
			}
			// ���� �ʱ�ȭ
			Family_Leave_SaveInfo(pTargetUserInfo->dwCharacterID, pTargetUserInfo->nFamilyLeaveKind, TRUE);
		}
	}
	else
	{
		// �ٸ� ������Ʈ�� �����ϴ� �÷��̾��. �ٸ� ������Ʈ�� �˸���
		MSG_DWORD_NAME stPacket;
		stPacket.Category			= MP_FAMILY;
		stPacket.Protocol			= MP_FAMILY_INVITED;
		stPacket.dwObjectID			= nTargetPlayerID;
		stPacket.dwData				= pUserInfo->dwCharacterID;
		SafeStrCpy(
			stPacket.Name,
			family->Get()->szName,
			_countof(stPacket.Name));
		g_Network.Broadcast2AgentServerExceptSelf((char*)&stPacket, sizeof(stPacket));
		return;
	}

FAILED:
	if (eResult == RESULT_OK)
	{
		MSG_DWORD_NAME stPacket;
		stPacket.Category			= MP_FAMILY;
		stPacket.Protocol			= MP_FAMILY_INVITED;
		stPacket.dwObjectID			= nTargetPlayerID;
		stPacket.dwData				= pUserInfo->dwCharacterID;
		SafeStrCpy(
			stPacket.Name,
			family->Get()->szName,
			_countof(stPacket.Name));
		g_Network.Send2User( pTargetUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket) );
	}

	MSG_DWORD2 stPacket;

	stPacket.Category			= MP_FAMILY;
	stPacket.Protocol			= MP_FAMILY_INVITE;
	stPacket.dwObjectID			= pUserInfo->dwCharacterID;
	stPacket.dwData1			= eResult;
	stPacket.dwData2			= nPastTime;
	g_Network.Send2User( pUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket) );
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
// ASRV_Invite Method																						�йи��� �ʴ� ���(������Ʈ)
//
VOID CSHFamilyManager::ASRV_InviteResult(USERINFO* pUserInfo, PACKET_ADD_MEMBER* pstInfo)
{
#if defined(_AGENTSERVER)
	if(g_csFamilyManager.GetFamily(pUserInfo->mFamilyIndex))
	{
		return;
	}

	// �����Ͱ� ��ȿ�Ѱ�?
	USERINFO* pMasterUserInfo = g_pUserTableForObjectID->FindUser(pstInfo->nMasterID);
	if (pMasterUserInfo == NULL)
	{
		// �ٸ� ������Ʈ���� �����ϴ� �÷��̾��� �� �ִ�.
		pstInfo->Protocol = MP_FAMILY_INVITE_RESULT_TO_OTHER_AGENT;
		g_Network.Broadcast2AgentServerExceptSelf((char*)pstInfo, sizeof(*pstInfo));
		return;
	}

	CSHFamily* const family = g_csFamilyManager.GetFamily(
		pMasterUserInfo->mFamilyIndex);

	if(0 == family ||
		pstInfo->nMasterID != family->Get()->nMasterID)
	{
		return;
	}

	// ��� ������
	MSG_NAME_DWORD2 stPacket;
	// ..�����Ϳ��� ��� ������
	stPacket.Category			= MP_FAMILY;
	stPacket.Protocol			= MP_FAMILY_INVITE_RESULT;
	stPacket.dwObjectID			= pMasterUserInfo->dwCharacterID;
	stPacket.dwData1			= pUserInfo->dwCharacterID;
	if (pstInfo->nResult == CSHFamilyManager::FIR_ACCEPT_PLAYER)	stPacket.dwData2 = FIR_ACCEPT_MASTER;
	else															stPacket.dwData2 = FIR_DENY_MASTER;
	g_Network.Send2User( pMasterUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket) );

	// ..Ÿ�� �÷��̾�� ��� ������
	stPacket.Category			= MP_FAMILY;
	stPacket.Protocol			= MP_FAMILY_INVITE_RESULT;
	stPacket.dwObjectID			= pUserInfo->dwCharacterID;
	stPacket.dwData1			= pMasterUserInfo->dwCharacterID;
	stPacket.dwData2			= pstInfo->nResult;
	SafeStrCpy(
		stPacket.Name,
		family->Get()->szName,
		_countof(stPacket.Name));
	g_Network.Send2User(
		pUserInfo->dwConnectionIndex,
		(char*)&stPacket,
		sizeof(stPacket));

	ASRV_SetFamilyIdxToMapServer(
		pUserInfo,
		family);

	// �¶�
	if (pstInfo->nResult == FIR_ACCEPT_PLAYER)
	{
		if(family->Get()->nMemberNum == family->GetMemberNumMax())
		{
			return;
		}

		CSHFamilyMember csMember;
		CSHFamilyMember::stINFO stInfo;

		ZeroMemory(&stInfo, sizeof(stInfo));
		stInfo.nID			= pUserInfo->dwCharacterID;
		stInfo.nRace		= pstInfo->nRace;
		stInfo.nSex			= pstInfo->nSex;
		stInfo.nJobFirst	= pstInfo->nJobFirst;
		stInfo.nJobCur		= pstInfo->nJobCur;
		stInfo.nJobGrade	= pstInfo->nJobGrade;
		stInfo.nLV			= pstInfo->nLV;
		stInfo.eConState	= CSHFamilyMember::MEMBER_CONSTATE_LOGIN;
		SafeStrCpy(stInfo.szName, pstInfo->szName, MAX_NAME_LENGTH+1);

		csMember.Set(&stInfo);
		family->AddMember(&csMember);
		pUserInfo->mFamilyIndex = pMasterUserInfo->mFamilyIndex; 

		if(family->Get()->nMemberNum == family->GetMemberNumMax())
		{
			// for(UINT cnt = 0; cnt < family->Get()->nMemberNum; ++cnt)
			// {
			// 	WebEvent(
			// 		family->GetMember(cnt)->Get()->nID,
			// 		5);
			// }
		}

		Family_Member_SaveInfo(
			family->Get()->nID,
			pUserInfo->dwCharacterID,
			NULL);
		InsertLogFamily(
			eLog_FamilyAddMember,
			family->Get()->nID,
			pUserInfo->dwCharacterID,
			family->Get()->szName);
		Family_LoadEmblemInfo(
			pUserInfo->dwCharacterID,
			family->Get()->nID);
		ASRV_SendInfoToClient(
			family);

		// �ٸ� ������Ʈ�� ����鿡�Ե� ������..
		{
			PACKET_FAMILY_TOTAL stPacket;
			int nLoginCnt = 0;

			ZeroMemory(
				&stPacket,
				sizeof(stPacket));
			stPacket.Category = MP_FAMILY;
			stPacket.Protocol = MP_FAMILY_INVITE_RESULT_TO_OTHER_AGENT_3;
			stPacket.dwObjectID	= pstInfo->dwObjectID;
			stPacket.stFamily.stInfo = *family->Get();
			stPacket.stFamily.stInfoEx = *family->GetEx();

			for(UINT i = 0; i < family->Get()->nMemberNum; ++i)
			{
				stPacket.stMember[i].stInfo	= *family->GetMember(i)->Get();
				stPacket.stMember[i].stInfoEx = *family->GetMember(i)->GetEx();
				stPacket.stMember[i].nIndex = i;

				if(stPacket.stMember[i].stInfo.eConState == CSHFamilyMember::MEMBER_CONSTATE_LOGIN)
				{
					nLoginCnt++;
				}
			}

			// ���� ������ �ִ� ����� HONOR_POINT_CHECK_MEMBER_NUM���� �Ǹ� �� ����Ʈ �ð��� �����Ѵ�.
			if (nLoginCnt == CSHFamilyManager::HONOR_POINT_CHECK_MEMBER_NUM)
			{
				family->GetEx()->nHonorPointTimeTick = gCurTime;
			}

			g_Network.Broadcast2AgentServerExceptSelf((char*)&stPacket, sizeof(stPacket));
		}

		g_csFarmManager.ASRV_RequestFarmUIInfoToMap(
			pUserInfo->dwCharacterID,
			family);
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  ASRV_ChangeNickname Method																								   ȣĪ ����
//
VOID CSHFamilyManager::ASRV_ChangeNickname(USERINFO* pUserInfo, DWORD nTargetPlayerID, char* pszNickname)
{
#if defined(_AGENTSERVER)
	RESULT eResult = RESULT_OK;
	USERINFO* const pTargetUserInfo = g_pUserTableForObjectID->FindUser(
		nTargetPlayerID);
	CSHFamily* const family = g_csFamilyManager.GetFamily(
		pUserInfo->mFamilyIndex);

	if(0 == family)
	{
		return;
	}

	// �̸��� �ִ� ���̸� �ʰ��߳�?
	if (strlen(pszNickname) > MAX_NAME_LENGTH)
	{
		eResult = RESULT_FAIL_01;
		goto FAILED;
	}
	else if(TRUE == FILTERTABLE->IsCharInString(pszNickname, "'") ||
		FALSE == FILTERTABLE->IsUsableName(pszNickname))
	{
		MSG_DWORD msg;
		msg.Category = MP_FAMILY;
		msg.Protocol = MP_FAMILY_CHANGE_NICKNAME_NACK;
		msg.dwObjectID = pUserInfo->dwCharacterID;
		g_Network.Send2User(pUserInfo->dwConnectionIndex, (char*)&msg, sizeof(msg));
		return;
	}

	BOOL bMember = FALSE;

	for(UINT i = 0; i < family->Get()->nMemberNum; ++i)
	{
		if(nTargetPlayerID == family->GetMember(i)->Get()->nID)
		{
			CSHFamilyMember::stINFO* stInfo = family->GetMember(i)->Get();

			SafeStrCpy(
				stInfo->szNickname,
				pszNickname,
				_countof(stInfo->szNickname));
			bMember = TRUE;
			break;
		}
	}

	// ���� �йи� ����ΰ�?
	if (bMember == FALSE)
	{
		return;
	}

	ASRV_SendInfoToClient(
		family,
		2);
	Family_Member_SaveInfo(
		family->Get()->nID,
		nTargetPlayerID,
		pszNickname);
	InsertLogFamily(
		eLog_FamilyChangeNick,
		family->Get()->nID,
		nTargetPlayerID,
		pszNickname );

	if (pTargetUserInfo)
	{
		// �ʼ����� �˸���
		ASRV_SendNicknameToMap(pTargetUserInfo, pszNickname);

		// �ٸ� ������Ʈ�� �˸���
		MSG_NAME_DWORD3 stPacket;
		ZeroMemory(&stPacket, sizeof(stPacket));

		stPacket.Category			= MP_FAMILY;
		stPacket.Protocol			= MP_FAMILY_CHANGE_NICKNAME;
		stPacket.dwData1			= family->Get()->nID;
		stPacket.dwData2			= NULL;
		stPacket.dwData3			= nTargetPlayerID;
		SafeStrCpy(stPacket.Name, pszNickname, MAX_NAME_LENGTH+1);

		g_Network.Broadcast2AgentServerExceptSelf((char*)&stPacket, sizeof(stPacket));	
	}
	
FAILED:
	// Ŭ���̾�Ʈ�� �˸���
	MSG_DWORD_NAME stPacket;
	// ..��û�ڿ��� �˸���
	ZeroMemory(&stPacket, sizeof(stPacket));
	stPacket.Category			= MP_FAMILY;
	stPacket.Protocol			= MP_FAMILY_CHANGE_NICKNAME;
	stPacket.dwObjectID			= pUserInfo->dwCharacterID;
	stPacket.dwData				= eResult;
	SafeStrCpy(
		stPacket.Name,
		pszNickname,
		_countof(stPacket.Name));
	g_Network.Send2User(
		pUserInfo->dwConnectionIndex,
		(char*)&stPacket, sizeof(stPacket));

	// ..����ڿ��� �˸���
	if (eResult == RESULT_OK)
	{
		if (pTargetUserInfo)
		{
			if (pUserInfo != pTargetUserInfo)
			{
				ZeroMemory(&stPacket, sizeof(stPacket));
				stPacket.Category			= MP_FAMILY;
				stPacket.Protocol			= MP_FAMILY_CHANGE_NICKNAME;
				stPacket.dwObjectID			= pTargetUserInfo->dwCharacterID;
				stPacket.dwData				= RESULT_OK_02;
				SafeStrCpy(
					stPacket.Name,
					pszNickname,
					_countof(stPacket.Name));
				g_Network.Send2User( pTargetUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket) );
			}
		}
		else
		{
			// �ٸ� ������Ʈ�� �����ϴ� �÷��̾��� �ű⼭ ó���ؾ� �ϴϱ�..
			MSG_NAME_DWORD3 stPacket;
			ZeroMemory(&stPacket, sizeof(stPacket));

			stPacket.Category			= MP_FAMILY;
			stPacket.Protocol			= MP_FAMILY_CHANGE_NICKNAME;
			stPacket.dwData1			= family->Get()->nID;
			stPacket.dwData2			= pUserInfo->dwCharacterID;
			stPacket.dwData3			= nTargetPlayerID;
			SafeStrCpy(stPacket.Name, pszNickname, MAX_NAME_LENGTH+1);

			g_Network.Broadcast2AgentServerExceptSelf((char*)&stPacket, sizeof(stPacket));
		}
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  ASRV_UpdateHonorPoint Method														  �� ����Ʈ ó�� (���� ��ɽ� ����ġ�� ����)
//
VOID CSHFamilyManager::ASRV_UpdateHonorPoint(USERINFO* pUserInfo, DWORD nHonorPoint)
{
#if defined(_AGENTSERVER)
	CSHFamily* const family = GetFamily(
		pUserInfo->mFamilyIndex);

	if(0 == family)
	{
		return;
	}

	CSHFamily::stINFO_EX& stInfoEx = *family->GetEx();

	// ���� �����̳� ���׷��̵��� ���� ���� ����Ʈ���� - int �� �ѱ��. �׷��� int �� ĳ�����ؼ� - �� ���� üũ�ؼ�
	// �׷� ��쿡�� -= �ϵ��� �Ѵ�. ���� ����Ʈ���� int ������� ����...
	if ((int)nHonorPoint < 0)
	{
		if (stInfoEx.nHonorPoint >= (DWORD)(-1*(int)nHonorPoint))
		{
			stInfoEx.nHonorPoint -= (DWORD)(-1*(int)nHonorPoint);
		}
		else
		{
			return;
		}
	}
	else
	{
		stInfoEx.nHonorPoint += nHonorPoint;
		if (stInfoEx.nHonorPoint > 4200000000) stInfoEx.nHonorPoint = 4200000000;
	}

	MSG_DWORD stPacket;
	
	for(UINT i = 0; i < family->Get()->nMemberNum; ++i)
	{
		if(USERINFO* pMemberUserInfo = g_pUserTableForObjectID->FindUser(family->GetMember(i)->Get()->nID))
		{
			stPacket.Category			= MP_FAMILY;
			stPacket.Protocol			= MP_FAMILY_HONOR_POINT;
			stPacket.dwObjectID			= pMemberUserInfo->dwCharacterID;
			stPacket.dwData				= nHonorPoint;

			g_Network.Send2User( pMemberUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket) );
		}
	}

	Family_SaveInfo(
		family->Get()->nMasterID, 
		family->Get()->szName, 
		family->GetEx()->nHonorPoint, 
		family->GetEx()->nNicknameON,
		1);

	// �ٸ� ������Ʈ�� �˸���
	{
		MSG_DWORD3 stPacket;
		ZeroMemory(&stPacket, sizeof(stPacket));

		stPacket.Category			= MP_FAMILY;
		stPacket.Protocol			= MP_FAMILY_HONOR_POINT_TO_OTHER_AGENT;
		stPacket.dwData1			= family->Get()->nID;
		stPacket.dwData2			= nHonorPoint;
		stPacket.dwData3			= 0;
		g_Network.Broadcast2AgentServerExceptSelf((char*)&stPacket, sizeof(stPacket));
	}
#endif
}

// 100809 ONS Ŭ�����ʱ�ȭ ó���� �йи� ������ �����Ѵ�.
VOID CSHFamilyManager::ASRV_ChangeClassInfo( DWORD CharacterID, DWORD FamilyIdx, DWORD Job, DWORD Grade )
{
#if defined(_AGENTSERVER)
	CSHFamily* const family = GetFamily(FamilyIdx);
	if(0 == family)
	{
		return;
	}

	for(UINT i = 0; i < family->Get()->nMemberNum; ++i)
	{
		if (CharacterID == family->GetMember(i)->Get()->nID)
		{
			CSHFamilyMember::stINFO	stInfo = *family->GetMember(i)->Get();
			stInfo.nJobCur		= Job;
			stInfo.nJobGrade	= Grade;
			family->GetMember(i)->Set(&stInfo);
		}
	}

	ASRV_SendInfoToClient(family, 2);
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  ASRV_UpdateMapPlayerInfo Method																			 ���� �÷��̾� ���� ������Ʈ
//
VOID CSHFamilyManager::ASRV_UpdateMapPlayerInfo(USERINFO* pUserInfo, DWORD nLevel, char* pszGuildName)
{
#if defined(_AGENTSERVER)
	CSHFamily* const family = GetFamily(
		pUserInfo->mFamilyIndex);

	if(0 == family)
	{
		return;
	}

	for(UINT i = 0; i < family->Get()->nMemberNum; ++i)
	{
		if(pUserInfo->dwCharacterID == family->GetMember(i)->Get()->nID)
		{
			family->GetMember(i)->Get()->nLV = nLevel;

			if (pszGuildName)
			{
				CSHFamilyMember::stINFO_EX* const info = family->GetMember(i)->GetEx();

				SafeStrCpy(
					info->szGuild,
					pszGuildName,
					_countof(info->szGuild));
			}

			break;
		}
	}

	ASRV_SendInfoToClient(
		family,
		2);

	// �ٸ� ������Ʈ�� �˸���
	MSG_NAME_DWORD3 stPacket;

	stPacket.Category	= MP_FAMILY;
	stPacket.Protocol	= MP_FAMILY_UPDATE_MAP_PLAYER_INFO_TO_OTHER_AGENT;
	stPacket.dwData1	= family->Get()->nID;
	stPacket.dwData2	= pUserInfo->dwCharacterID;
	stPacket.dwData3	= nLevel;
	if (pszGuildName)
	{
		SafeStrCpy(stPacket.Name, pszGuildName, MAX_NAME_LENGTH+1);
	}

	g_Network.Broadcast2AgentServerExceptSelf((char*)&stPacket, sizeof(stPacket));
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  ASRV_SendNicknameToMap Method																				  ȣĪ�� �ʼ����� ������
//
VOID CSHFamilyManager::ASRV_SendNicknameToMap(USERINFO* pUserInfo, char* pszNickname)
{
#if defined(_AGENTSERVER)
	MSG_NAME stPacket;

	stPacket.Category			= MP_FAMILY;
	stPacket.Protocol			= MP_FAMILY_SHOW_NICKNAME;
	stPacket.dwObjectID			= pUserInfo->dwCharacterID;
	SafeStrCpy(stPacket.Name, pszNickname, MAX_NAME_LENGTH+1);

	g_Network.Send2Server( pUserInfo->dwMapServerConnectionIndex, (char*)&stPacket, sizeof(stPacket) );
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  ASRV_RegistEmblem Method																								   ���� ���
//
VOID CSHFamilyManager::ASRV_RegistEmblem(USERINFO* pUserInfo, DWORD nFamilyID, char* pImgData, int nState)
{
#if defined(_AGENTSERVER)
	CSHFamily* const family = GetFamily(
		pUserInfo->mFamilyIndex);

	if(0 == family)
	{
		return;
	}
	else if(family->Get()->nMasterID != pUserInfo->dwCharacterID &&
		 nState == 0)
	{
		return;
	}

	PACKET_FAMILY_EMBLEM stPacket;

	// ���
	if (nState == 0)
	{
		family->IncreaseEmblemChangedNum();
		
		for(UINT i = 0; i < family->Get()->nMemberNum; ++i)
		{
			if(USERINFO* const pMemberUserInfo = g_pUserTableForObjectID->FindUser(family->GetMember(i)->Get()->nID))
			{
				stPacket.Category		= MP_FAMILY;
				stPacket.Protocol		= MP_FAMILY_REGIST_EMBLEM;
				stPacket.dwObjectID		= pMemberUserInfo->dwCharacterID;
				stPacket.nFamilyID		= nFamilyID;
				stPacket.nChangedFreq	= family->GetEmblemChangedNum();
				// 091111 ONS ���ӷα��� ������ ó���ϴ� �޼����� ����Ҷ��� �����ϱ� ���� 
				// �йи� ������ ���/�ε带 �����ϴ� ����߰�
				stPacket.bIsLoad		= FALSE;
				memcpy(stPacket.cImgData, pImgData, EMBLEM_BUFSIZE);
				g_Network.Send2Server( pMemberUserInfo->dwMapServerConnectionIndex, (char*)&stPacket, sizeof(stPacket) );
			}
		}
		// DB�� ����
		Family_SaveEmblemInfo(pUserInfo->dwCharacterID, nFamilyID, pImgData);
		// 081205 LUJ, �α�
		InsertLogFamily(
			eLog_FamilyRegistEmblem,
			nFamilyID,
			pUserInfo->dwCharacterID );
		// �ٸ� ������Ʈ�� �˸���
		stPacket.Category		= MP_FAMILY;
		stPacket.Protocol		= MP_FAMILY_REGIST_EMBLEM_TO_OTHER_AGENT;
		stPacket.nFamilyID		= nFamilyID;
		stPacket.bIsLoad		= FALSE;
		memcpy(stPacket.cImgData, pImgData, EMBLEM_BUFSIZE);
		g_Network.Broadcast2AgentServerExceptSelf((char*)&stPacket, sizeof(stPacket));
	}
	// �ε�
	else
	{
		stPacket.Category		= MP_FAMILY;
		stPacket.Protocol		= MP_FAMILY_SHOW_EMBLEM;
		stPacket.dwObjectID		= pUserInfo->dwCharacterID;
		stPacket.nFamilyID		= nFamilyID;
		stPacket.nChangedFreq	= family->GetEmblemChangedNum();
		stPacket.bIsLoad		= TRUE;
		memcpy(stPacket.cImgData, pImgData, EMBLEM_BUFSIZE);
		g_Network.Send2Server( pUserInfo->dwMapServerConnectionIndex, (char*)&stPacket, sizeof(stPacket) );
	}
#endif
}

// 091111 ONS �йи� ���� ���� ó�� �߰�
// -------------------------------------------------------------------------------------------------------------------------------------
//  ASRV_DeleteEmblem Method																 ������Ʈ�κ����� ���� ����
//
VOID CSHFamilyManager::ASRV_DeleteEmblem(USERINFO* pUserInfo, DWORD nFamilyID)
{
#if defined(_AGENTSERVER)

	CSHFamily* family = GetFamily(nFamilyID);

	if(0 == family)
	{
		return;
	}

	family->IncreaseEmblemChangedNum();

	for(UINT i = 0; i < family->Get()->nMemberNum; ++i)
	{
		if (USERINFO* const pMemberUserInfo = g_pUserTableForObjectID->FindUser(family->GetMember(i)->Get()->nID))
		{
			MSG_DWORD2 stPacket;
			ZeroMemory(&stPacket, sizeof(MSG_DWORD2));
			stPacket.Category		= MP_FAMILY;
			stPacket.Protocol		= MP_FAMILY_DEL_EMBLEM;
			stPacket.dwObjectID		= pMemberUserInfo->dwCharacterID;
			stPacket.dwData1		= nFamilyID;
			stPacket.dwData2		= family->GetEmblemChangedNum();
			g_Network.Send2Server( pMemberUserInfo->dwMapServerConnectionIndex, (char*)&stPacket, sizeof(stPacket) );
		}
	}

	// DB�� ����.
	Family_DeleteEmblem(pUserInfo->dwCharacterID, nFamilyID);
	
	InsertLogFamily(
		eLog_FamilyDelEmelem,
		nFamilyID,
		pUserInfo->dwCharacterID );

	// �ٸ� ������Ʈ�� �˸���
	MSG_DWORD message;
	ZeroMemory(&message, sizeof(MSG_DWORD));
	message.Category		= MP_FAMILY;
	message.Protocol		= MP_FAMILY_DEL_EMBLEM_TO_OTHER_AGENT;
	message.dwData			= nFamilyID;
	g_Network.Broadcast2AgentServerExceptSelf((char*)&message, sizeof(message));

#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  ASRV_RegistEmblemFromOtherAgent Method																 �ٸ� ������Ʈ�κ����� ���� ���
//
VOID CSHFamilyManager::ASRV_RegistEmblemFromOtherAgent(DWORD nFamilyID, char* pImgData)
{
#if defined(_AGENTSERVER)
	// ���̺� �̹� �����ϴ��� üũ
	CSHFamily* pcsFamily = GetFamily(nFamilyID);
	if (pcsFamily == NULL)
	{
		return;
	}

	pcsFamily->IncreaseEmblemChangedNum();

	PACKET_FAMILY_EMBLEM stPacket;

	// ����鿡�� �˸���
	for(UINT i=0; i<pcsFamily->Get()->nMemberNum; i++)
	{
		USERINFO* pMemberUserInfo = g_pUserTableForObjectID->FindUser(pcsFamily->GetMember(i)->Get()->nID);
		if (pMemberUserInfo)
		{
			stPacket.Category		= MP_FAMILY;
			stPacket.Protocol		= MP_FAMILY_REGIST_EMBLEM;
			stPacket.dwObjectID		= pMemberUserInfo->dwCharacterID;
			stPacket.nFamilyID		= nFamilyID;
			stPacket.nChangedFreq	= pcsFamily->GetEmblemChangedNum();
			stPacket.bIsLoad		= FALSE;
			memcpy(stPacket.cImgData, pImgData, EMBLEM_BUFSIZE);
			g_Network.Send2Server( pMemberUserInfo->dwMapServerConnectionIndex, (char*)&stPacket, sizeof(stPacket) );
		}
	}
#endif
}

// 091111 ONS �йи� ���� ���� �޼����� �ٸ� ������Ʈ�� ����
// -------------------------------------------------------------------------------------------------------------------------------------
//  ASRV_DeleteEmblemFromOtherAgent Method																 �ٸ� ������Ʈ�κ����� ���� ����
//
VOID CSHFamilyManager::ASRV_DeleteEmblemFromOtherAgent(DWORD nFamilyID)
{
#if defined(_AGENTSERVER)
	// ���̺� �̹� �����ϴ��� üũ
	CSHFamily* pcsFamily = GetFamily(nFamilyID);
	if (pcsFamily == NULL)
	{
		return;
	}

	pcsFamily->IncreaseEmblemChangedNum();

	// ����鿡�� �˸���
	for(UINT i=0; i<pcsFamily->Get()->nMemberNum; i++)
	{
		USERINFO* pMemberUserInfo = g_pUserTableForObjectID->FindUser(pcsFamily->GetMember(i)->Get()->nID);
		if (pMemberUserInfo)
		{
			MSG_DWORD2 stPacket;
			ZeroMemory(&stPacket, sizeof(MSG_DWORD2));
			stPacket.Category		= MP_FAMILY;
			stPacket.Protocol		= MP_FAMILY_DEL_EMBLEM;
			stPacket.dwObjectID		= pMemberUserInfo->dwCharacterID;
			stPacket.dwData1		= nFamilyID;
			stPacket.dwData2		= pcsFamily->GetEmblemChangedNum();
			g_Network.Send2Server( pMemberUserInfo->dwMapServerConnectionIndex, (char*)&stPacket, sizeof(stPacket) );
		}
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  ASRV_Leave Method																												Ż��
//
VOID CSHFamilyManager::ASRV_Leave(USERINFO* pUserInfo)
{
#if defined(_AGENTSERVER)
	MSG_DWORD stPacket;
	CSHFamily* const family = GetFamily(
		pUserInfo->mFamilyIndex);

	if(0 == family)
	{
		return;
	}
	else if (pUserInfo->dwCharacterID == family->Get()->nMasterID)
	{
		return;
	}

	// ����鿡�� �˸���
	int nLoginCnt = 0;

	for(UINT i = 0; i < family->Get()->nMemberNum; ++i)
	{
		if(USERINFO* const pMemberUserInfo = g_pUserTableForObjectID->FindUser(family->GetMember(i)->Get()->nID))
		{
			stPacket.Category	= MP_FAMILY;
			stPacket.Protocol	= MP_FAMILY_LEAVE;
			stPacket.dwObjectID	= pMemberUserInfo->dwCharacterID;
			stPacket.dwData		= pUserInfo->dwCharacterID;
			g_Network.Send2User( pMemberUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket) );
		}

		if(family->GetMember(i)->Get()->eConState == CSHFamilyMember::MEMBER_CONSTATE_LOGIN)
		{
			nLoginCnt++;
		}
	}

	// ���� ������ �ִ� ����� HONOR_POINT_CHECK_MEMBER_NUM�� �̸��� �Ǹ� �� ����Ʈ �ð� �ʱ�ȭ
	if (nLoginCnt-1 < CSHFamilyManager::HONOR_POINT_CHECK_MEMBER_NUM)
	{
		family->GetEx()->nHonorPointTimeTick = 0;
	}

	// �ʼ����� �˸���
	// ..���� ����, �ش� ĳ���Ͱ� �ɾ���� ���۹� ����
	stPacket.Category	= MP_FAMILY;
	stPacket.Protocol	= MP_FAMILY_LEAVE;
	stPacket.dwObjectID = pUserInfo->dwCharacterID;
	g_Network.Broadcast2MapServer((char*)&stPacket, sizeof(stPacket));

	// ���� ����
	SYSTEMTIME t;
	GetLocalTime(&t);
	pUserInfo->nFamilyLeaveDate = (t.wYear-2000)*100000000 + t.wMonth*1000000 + t.wDay*10000 + t.wHour*100 + t.wMinute;
	pUserInfo->nFamilyLeaveKind = FLK_LEAVE;

	// �ٸ� ������Ʈ�� �˸���
	stPacket.Category	= MP_FAMILY;
	stPacket.Protocol	= MP_FAMILY_LEAVE_TO_OTHER_AGENT;
	stPacket.dwObjectID	= pUserInfo->dwCharacterID;
	stPacket.dwData		= family->Get()->nID;
	g_Network.Broadcast2AgentServerExceptSelf((char*)&stPacket, sizeof(stPacket));

	family->DelMember(
		pUserInfo->dwCharacterID);
	Family_Leave_SaveInfo(
		pUserInfo->dwCharacterID,
		FLK_LEAVE,
		FALSE);
	InsertLogFamily(
		eLog_FamilyLeaveMember,
		family->Get()->nID,
		pUserInfo->dwCharacterID ) ;

	// �α��� ���� Ż���� 1���̸� �йи� ����
	if (nLoginCnt == 1)
	{
		// 081205 LUJ, �α�. ���� �ʱ�ȭ��Ű�Ƿ� ���� �α׸� ������
		InsertLogFamily(
			eLog_FamilyBreakup,
			family->Get()->nID,
			pUserInfo->dwCharacterID,
			"auto" );
		DelFamilyToTbl(
			family);
	}

	ASRV_SendFamilyInfoToMapServer(
		family->Get()->nID);

	pUserInfo->mFamilyIndex = 0;

	// 090219 LYW --- SHFamilyManager : Ż�� ĳ������ �йи� �ε����� �ʱ�ȭ�Ѵ�.
	ASRV_SetFamilyIdxToMapServer( pUserInfo, NULL ) ;
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  ASRV_Expel Method																												�߹�
//
VOID CSHFamilyManager::ASRV_Expel(USERINFO* pUserInfo, DWORD nMemberID)
{
#if defined(_AGENTSERVER)
	RESULT eResult = RESULT_OK;

	MSG_DWORD2 stPacket;
	CSHFamily* const family = GetFamily(
		pUserInfo->mFamilyIndex);

	if(0 == family)
	{
		return;
	}
	else if(pUserInfo->dwCharacterID != family->Get()->nMasterID)
	{
		return;
	}

	// �߹� ����� �������ΰ�?
	if (pUserInfo->dwCharacterID == nMemberID)
	{
		eResult = RESULT_FAIL_01;
		goto FAILED;
		return;
	}


	// ����鿡�� �˸���
	int nLoginCnt = 0;

	for(UINT i = 0; i < family->Get()->nMemberNum; ++i)
	{
		if(USERINFO* const pMemberUserInfo = g_pUserTableForObjectID->FindUser(family->GetMember(i)->Get()->nID))
		{
			stPacket.Category	= MP_FAMILY;
			stPacket.Protocol	= MP_FAMILY_EXPEL;
			stPacket.dwObjectID	= pMemberUserInfo->dwCharacterID;
			stPacket.dwData1	= nMemberID;
			stPacket.dwData2	= eResult;
			g_Network.Send2User( pMemberUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket) );
		}

		if(family->GetMember(i)->Get()->eConState == CSHFamilyMember::MEMBER_CONSTATE_LOGIN)
		{
			nLoginCnt++;
		}
	}

	// ���� ������ �ִ� ����� HONOR_POINT_CHECK_MEMBER_NUM�� �̸��� �Ǹ� �� ����Ʈ �ð� �ʱ�ȭ
	if (nLoginCnt-1 < CSHFamilyManager::HONOR_POINT_CHECK_MEMBER_NUM)
	{
		family->GetEx()->nHonorPointTimeTick = 0;
	}

	// �ʼ����� �˸���
	// ..���� ����, �ش� ĳ���Ͱ� �ɾ���� ���۹� ����
	stPacket.Category	= MP_FAMILY;
	stPacket.Protocol	= MP_FAMILY_EXPEL;
	stPacket.dwObjectID = nMemberID;
	stPacket.dwData1	= nMemberID;
	stPacket.dwData2	= eResult;
	g_Network.Broadcast2MapServer((char*)&stPacket, sizeof(stPacket));

	// ..�������� �߹� ���Ƽ�� ����..
	stPacket.Category	= MP_FAMILY;
	stPacket.Protocol	= MP_FAMILY_EXPEL;
	stPacket.dwObjectID = pUserInfo->dwCharacterID;
	stPacket.dwData1	= nMemberID;
	stPacket.dwData2	= eResult;
	g_Network.Send2Server(pUserInfo->dwMapServerConnectionIndex, (char*)&stPacket, sizeof(stPacket));

	// ���� ����
	SYSTEMTIME t;
	GetLocalTime(&t);
	int nDate = (t.wYear-2000)*100000000 + t.wMonth*1000000 + t.wDay*10000 + t.wHour*100 + t.wMinute;
	pUserInfo->nFamilyLeaveDate = nDate;
	pUserInfo->nFamilyLeaveKind = FLK_EXPEL;

	// �ٸ� ������Ʈ�� �˸���
	stPacket.Category	= MP_FAMILY;
	stPacket.Protocol	= MP_FAMILY_EXPEL_TO_OTHER_AGENT;
	stPacket.dwObjectID	= nMemberID;
	stPacket.dwData1	= family->Get()->nID;
	stPacket.dwData2	= eResult;
	g_Network.Broadcast2AgentServerExceptSelf((char*)&stPacket, sizeof(stPacket));

	family->DelMember(
		nMemberID);
	Family_Leave_SaveInfo(nMemberID, FLK_EXPEL, FALSE);
	Family_Leave_SaveInfo(pUserInfo->dwCharacterID, FLK_EXPEL, FALSE);
	// 081205 LUJ, �α�
	InsertLogFamily(
		eLog_FamilyExpelMember,
		family->Get()->nID,
		pUserInfo->dwCharacterID);
	ASRV_SendFamilyInfoToMapServer(
		family->Get()->nID);

	// �߹�� ����� �������� ����
	USERINFO* pExpelUserInfo = g_pUserTableForObjectID->FindUser(nMemberID);
	if (pExpelUserInfo)
	{
		pExpelUserInfo->mFamilyIndex = 0;
		pExpelUserInfo->nFamilyLeaveDate = nDate;
		pExpelUserInfo->nFamilyLeaveKind = FLK_EXPEL;

		// 090219 LYW --- SHFamilyManager : �߹� ĳ������ �йи� �ε����� �ʱ�ȭ�Ѵ�.
		ASRV_SetFamilyIdxToMapServer( pExpelUserInfo, NULL ) ;
	}

FAILED:
	if (eResult == RESULT_OK)
	{
	}
	else
	{
		stPacket.Category	= MP_FAMILY;
		stPacket.Protocol	= MP_FAMILY_EXPEL;
		stPacket.dwObjectID	= pUserInfo->dwCharacterID;
		stPacket.dwData2	= eResult;
		g_Network.Send2User( pUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket) );
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  ASRV_BreakUp Method																												��ü
//
VOID CSHFamilyManager::ASRV_BreakUp(USERINFO* pUserInfo)
{
#if defined(_AGENTSERVER)
	RESULT eResult = RESULT_OK;

	MSG_DWORD2 stPacket;
	CSHFamily* const family = GetFamily(
		pUserInfo->mFamilyIndex);

	if(0 == family)
	{
		eResult = RESULT_FAIL_01;
		goto FAILED;
		return;
	}
	else if(pUserInfo->dwCharacterID != family->Get()->nMasterID)
	{
		eResult = RESULT_FAIL_02;
		goto FAILED;
		return;
	}
	else if(family->Get()->nMemberNum > 1)
	{
		eResult = RESULT_FAIL_03;
		goto FAILED;
		return;
	}

	for(UINT i = 0; i < family->Get()->nMemberNum; ++i)
	{
		if(USERINFO* const pMemberUserInfo = g_pUserTableForObjectID->FindUser(family->GetMember(i)->Get()->nID))
		{
			// Ŭ���̾�Ʈ�� �˸���
			stPacket.Category	= MP_FAMILY;
			stPacket.Protocol	= MP_FAMILY_BREAKUP;
			stPacket.dwObjectID	= pMemberUserInfo->dwCharacterID;
			stPacket.dwData1	= pUserInfo->dwCharacterID;
			stPacket.dwData2	= eResult;
			g_Network.Send2User( pMemberUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket) );
			// �ʼ����� �˸���
			// ..���� ����, �ش� ĳ���Ͱ� �ɾ���� ���۹� ����
			stPacket.Category	= MP_FAMILY;
			stPacket.Protocol	= MP_FAMILY_BREAKUP;
			stPacket.dwObjectID = pMemberUserInfo->dwCharacterID;
			stPacket.dwData1	= pUserInfo->dwCharacterID;
			stPacket.dwData2	= eResult;
			g_Network.Broadcast2MapServer((char*)&stPacket, sizeof(stPacket));
			// ����� �йи� �ʱ�ȭ
			if (pMemberUserInfo->dwCharacterID != pUserInfo->dwCharacterID) 
			{
				pMemberUserInfo->mFamilyIndex = 0;
			}
		}
	}

	// ���� ����
	SYSTEMTIME t;
	GetLocalTime(&t);
	pUserInfo->nFamilyLeaveDate = (t.wYear-2000)*100000000 + t.wMonth*1000000 + t.wDay*10000 + t.wHour*100 + t.wMinute;
	pUserInfo->nFamilyLeaveKind = FLK_BREAKUP;

	// �ٸ� ������Ʈ�� �˸���
	stPacket.Category	= MP_FAMILY;
	stPacket.Protocol	= MP_FAMILY_BREAKUP_TO_OTHER_AGENT;
	stPacket.dwObjectID	= pUserInfo->dwCharacterID;
	stPacket.dwData1	= family->Get()->nID;
	stPacket.dwData2	= eResult;
	g_Network.Broadcast2AgentServerExceptSelf((char*)&stPacket, sizeof(stPacket));

	// 081205 LUJ, �α�. ���� �� �޸𸮸� �ʱ�ȭ�ϹǷ� ���� �α׸� ������
	InsertLogFamily(
		eLog_FamilyBreakup,
		family->Get()->nID,
		pUserInfo->dwCharacterID );
	DelFamilyToTbl(
		family);
	Family_Leave_SaveInfo(
		pUserInfo->dwCharacterID,
		FLK_BREAKUP,
		FALSE,
		TRUE);
	pUserInfo->mFamilyIndex = 0;

FAILED:
	if (eResult != RESULT_OK)
	{
		stPacket.Category	= MP_FAMILY;
		stPacket.Protocol	= MP_FAMILY_BREAKUP;
		stPacket.dwObjectID	= pUserInfo->dwCharacterID;
		stPacket.dwData2	= eResult;
		g_Network.Send2User( pUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket) );
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  ASRV_LeaveFromOtherAgent Method																		  	  �ٸ� ������Ʈ�κ����� Ż��
//
VOID CSHFamilyManager::ASRV_LeaveFromOtherAgent(DWORD nFamilyID, DWORD nMemberID)
{
#if defined(_AGENTSERVER)
	// ���̺� �̹� �����ϴ��� üũ
	CSHFamily* pcsFamily = GetFamily(nFamilyID);
	if (pcsFamily == NULL)
	{
		return;
	}

	MSG_DWORD stPacket;

	// ����鿡�� �˸���
	int nLoginCnt = 0;
	for(UINT i=0; i<pcsFamily->Get()->nMemberNum; i++)
	{
		USERINFO* pMemberUserInfo = g_pUserTableForObjectID->FindUser(pcsFamily->GetMember(i)->Get()->nID);
		if (pMemberUserInfo)
		{
			stPacket.Category		= MP_FAMILY;
			stPacket.Protocol		= MP_FAMILY_LEAVE;
			stPacket.dwObjectID		= pMemberUserInfo->dwCharacterID;
			stPacket.dwData			= nMemberID;
			g_Network.Send2User( pMemberUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket) );
		}

		if (pcsFamily->GetMember(i)->Get()->eConState == CSHFamilyMember::MEMBER_CONSTATE_LOGIN)
		{
			nLoginCnt++;
		}
	}

	// ���� ������ �ִ� ����� HONOR_POINT_CHECK_MEMBER_NUM�� �̸��� �Ǹ� �� ����Ʈ �ð� �ʱ�ȭ
	if (nLoginCnt-1 < CSHFamilyManager::HONOR_POINT_CHECK_MEMBER_NUM)
	{
		pcsFamily->GetEx()->nHonorPointTimeTick = 0;
	}

	// ��� ����
	pcsFamily->DelMember(nMemberID);

	// �α��� ���� Ż���� 1���̸� �йи� ����
	if (nLoginCnt == 1)
	{
		DelFamilyToTbl(pcsFamily);
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  ASRV_ExpelFromOtherAgent Method																		  	  �ٸ� ������Ʈ�κ����� �߹�
//
VOID CSHFamilyManager::ASRV_ExpelFromOtherAgent(DWORD nFamilyID, DWORD nMemberID)
{
#if defined(_AGENTSERVER)
	// ���̺� �̹� �����ϴ��� üũ
	CSHFamily* pcsFamily = GetFamily(nFamilyID);
	if (pcsFamily == NULL)
	{
		return;
	}

	MSG_DWORD2 stPacket;

	// ����鿡�� �˸���
	int nLoginCnt = 0;
	for(UINT i=0; i<pcsFamily->Get()->nMemberNum; i++)
	{
		USERINFO* pMemberUserInfo = g_pUserTableForObjectID->FindUser(pcsFamily->GetMember(i)->Get()->nID);
		if (pMemberUserInfo)
		{
			stPacket.Category		= MP_FAMILY;
			stPacket.Protocol		= MP_FAMILY_EXPEL;
			stPacket.dwObjectID		= pMemberUserInfo->dwCharacterID;
			stPacket.dwData1		= nMemberID;
			stPacket.dwData2		= RESULT_OK;
			g_Network.Send2User( pMemberUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket) );
		}

		if (pcsFamily->GetMember(i)->Get()->eConState == CSHFamilyMember::MEMBER_CONSTATE_LOGIN)
		{
			nLoginCnt++;
		}
	}

	// ���� ������ �ִ� ����� HONOR_POINT_CHECK_MEMBER_NUM�� �̸��� �Ǹ� �� ����Ʈ �ð� �ʱ�ȭ
	if (nLoginCnt-1 < CSHFamilyManager::HONOR_POINT_CHECK_MEMBER_NUM)
	{
		CSHFamily::stINFO_EX stInfoEx = *pcsFamily->GetEx();
		stInfoEx.nHonorPointTimeTick = 0;
		pcsFamily->SetEx(&stInfoEx);
	}

	// ��� ����
	pcsFamily->DelMember(nMemberID);

	// ..�߹�� ����� ���� ���� ����
	USERINFO* pMemberUserInfo = g_pUserTableForObjectID->FindUser(nMemberID);
	if (pMemberUserInfo)
	{
		SYSTEMTIME t;
		GetLocalTime(&t);
		int nDate = (t.wYear-2000)*100000000 + t.wMonth*1000000 + t.wDay*10000 + t.wHour*100 + t.wMinute;
		pMemberUserInfo->nFamilyLeaveDate = nDate;
		pMemberUserInfo->nFamilyLeaveKind = FLK_EXPEL;
		pMemberUserInfo->mFamilyIndex = 0;
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  ASRV_BreakUpFromOtherAgent Method																	  	  �ٸ� ������Ʈ�κ����� ��ü
//
VOID CSHFamilyManager::ASRV_BreakUpFromOtherAgent(DWORD nFamilyID)
{
#if defined(_AGENTSERVER)
	// ���̺� �̹� �����ϴ��� üũ
	CSHFamily* pcsFamily = GetFamily(nFamilyID);
	if (pcsFamily == NULL)
	{
		return;
	}

	// �йи� ����
	DelFamilyToTbl(pcsFamily);
#endif
}

// 091126 ONS �йи��� �̾� ��� �߰�
VOID CSHFamilyManager::ASRV_Transfer(USERINFO* pUserInfo, DWORD dwNewMaster)
{
#if defined(_AGENTSERVER)
	CSHFamily* const family = GetFamily(
		pUserInfo->mFamilyIndex);

	if(0 == family)
	{
		return;
	}

	const DWORD dwMaster = family->Get()->nMasterID;
	
	if(pUserInfo->dwCharacterID != dwMaster)
	{
		return;
	}

	// �������� ���������Ͱ� �ƴϾ���Ѵ�.
	if (dwNewMaster == dwMaster)
	{
		return;
	}

	g_DB.FreeMiddleQuery(
		RFamily_ChangeMaster,
		dwMaster,
		"EXEC DBO.MP_FAMILY_CHANGEMASTER %d, %d, %d",
		dwMaster,
		dwNewMaster,
		family->Get()->nID);
#endif
}

// 091126 ONS �йи��� �̾� ��� �߰� : �ٸ� ������Ʈ�� �˸���
VOID CSHFamilyManager::ASRV_TransferFromOtherAgent(void* pMsg)
{
#if defined(_AGENTSERVER)
	const MSG_DWORD3_NAME* const pmsg = (MSG_DWORD3_NAME*)pMsg;
	const DWORD familyIndex = pmsg->dwObjectID;
	const DWORD masterPlayerIndex = pmsg->dwData1;
	const DWORD leaveDate = pmsg->dwData2;
	const DWORD leaveKind = pmsg->dwData3;

	CSHFamily* const family = GetFamily(
		familyIndex);

	if(0 == family)
	{
		return;
	}

	family->Get()->nMasterID = masterPlayerIndex;
	SafeStrCpy(
		family->Get()->szMasterName,
		pmsg->Name,
		_countof(family->Get()->szMasterName));

	for(UINT i = 0; i < family->Get()->nMemberNum; ++i)
	{
		if(USERINFO* const pMemberUserInfo = g_pUserTableForObjectID->FindUser(family->GetMember(i)->Get()->nID))
		{
			
			MSG_DWORD stPacket;
			ZeroMemory(&stPacket, sizeof(MSG_DWORD));
			stPacket.Category		= MP_FAMILY;
			stPacket.Protocol		= MP_FAMILY_TRANSFER;
			stPacket.dwObjectID		= pMemberUserInfo->dwCharacterID;
			stPacket.dwData			= masterPlayerIndex;
			g_Network.Send2User( pMemberUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket) );

			if(pMemberUserInfo->dwCharacterID == masterPlayerIndex)
			{
				pMemberUserInfo->nFamilyLeaveDate = leaveDate;
				pMemberUserInfo->nFamilyLeaveKind = leaveKind;
			}
		}
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
// 090213 LYW --- SHFamilyManager :												�ٸ� ������Ʈ�� ���� ����� �α��� �ƿ� ������ ���� �Լ�.
//
VOID CSHFamilyManager::ASRV_RefreshFamilyInfo(void* pMsg)
{
#ifdef _AGENTSERVER
	// �Լ� ���� Ȯ��.
	if( !pMsg ) return ;


	// ���� �޽��� ��ȯ.
	MSGBASE* pmsg = (MSGBASE*)pMsg ;
	if( !pmsg ) return ;


	// �Լ� �Ű����� Ȯ��.
	CSHFamily* pFamily = g_csFamilyManager.GetFamily( pmsg->dwObjectID ) ;
	if( !pFamily ) return ;


	// �� ������ ������ �йи� �ε����� �޴´�.
	DWORD dwFamilyIDX = pFamily->Get()->nID ;


	// �� ������ ������ �α��� ���� ��� ���� ���Ѵ�.
	DWORD dwMemberCount = 0 ;
	CSHFamilyMember* pMember ;
	CSHFamilyMember::stINFO* pInfo = NULL;
	for( BYTE byCount = 0 ; byCount < CSHFamily::MAX_MEMBER_NUM ; ++byCount )
	{
		// �йи� ��� ������ Ȯ���Ѵ�.
		pMember =  pFamily->GetMember( byCount ) ;
		if( !pMember ) continue ;

		pInfo = pMember->Get() ;
		if( !pInfo ) continue ;

		if( pInfo->eConState != CSHFamilyMember::MEMBER_CONSTATE_LOGIN ) continue ;

		++dwMemberCount ;
	}


	// �ش�  ������ �˻��� �� ������ �йи� ������ �����Ѵ�.
	USERINFO* pUserInfo ;

	MSG_DWORD2 msg ;
	memset( &msg, 0, sizeof(MSG_DWORD2) ) ;

	msg.Category				= MP_FAMILY ;
	msg.Protocol				= MP_FAMILY_REFRESH_FAMILYINFO_MAPSERVER ;
	msg.dwObjectID				= pInfo->nID ;
	msg.dwData1					= dwFamilyIDX ;
	msg.dwData2					= dwMemberCount ;

	for( BYTE byCount = 0 ; byCount < CSHFamily::MAX_MEMBER_NUM ; ++byCount )
	{
		// �йи� ��� ������ Ȯ���Ѵ�.
		pMember =  pFamily->GetMember( byCount ) ;
		if( !pMember ) continue ;

		pInfo = pMember->Get() ;
		if( !pInfo ) continue ;

		if( pInfo->eConState != CSHFamilyMember::MEMBER_CONSTATE_LOGIN ) continue ;

		pUserInfo = g_pUserTableForObjectID->FindUser( pInfo->nID ) ;
		if( !pUserInfo ) continue ;

		msg.dwObjectID				= pInfo->nID ;

		g_Network.Broadcast2MapServer( (char*)&msg, sizeof(MSG_DWORD2) ) ;
	}
#endif // _AGENTSERVER
}

#ifdef _AGENTSERVER
// -------------------------------------------------------------------------------------------------------------------------------------
//															  Agent������ ���� �� �� ������ ĳ������ �йи� �ε����� �����ϴ� �Լ� �߰�.
//
VOID CSHFamilyManager::ASRV_SetFamilyIdxToMapServer( USERINFO* pUserInfo, CSHFamily* pFamily )
{
	// �Լ� �Ű� ���� Ȯ��.
	if( !pUserInfo ) return ;


	// �� ������ ������ �޽��� ����.
	MSG_DWORD msg ;
	memset( &msg, 0, sizeof(MSG_DWORD) ) ;

	msg.Category	= MP_FAMILY ;
	msg.Protocol	= MP_FAMILY_SETFAMILYID_MAPSERVER ;
	msg.dwObjectID	= pUserInfo->dwCharacterID ;

	// �йи� ������ ��ȿ���� Ȯ���Ѵ�.
	if( pFamily )
	{
		msg.dwData		= pFamily->Get()->nID ;
	}

	// ���� Agent������ ���� �� �� ������ �޽����� �����Ѵ�.
	g_Network.Broadcast2MapServer( (char*)&msg, sizeof(MSG_DWORD) ) ;
}

VOID CSHFamilyManager::ASRV_SendFamilyInfoToMapServer(DWORD familyIndex)
{
	mReservedTransferContainer.push(
		TransferredFamily(gCurTime + 5000, familyIndex));
}

VOID CSHFamilyManager::ASRV_SendFamilyInfoToMapServerReal( CSHFamily* pFamily )
{
	// �Լ� �Ű����� Ȯ��.
	if( !pFamily ) return ;


	// �� ������ ������ �йи� �ε����� �޴´�.
	DWORD dwFamilyIDX = pFamily->Get()->nID ;


	// �� ������ ������ �α��� ���� ��� ���� ���Ѵ�.
	DWORD dwMemberCount = 0 ;
	CSHFamilyMember* pMember ;
	CSHFamilyMember::stINFO* pInfo ;
	for( BYTE byCount = 0 ; byCount < CSHFamily::MAX_MEMBER_NUM ; ++byCount )
	{
		// �йи� ��� ������ Ȯ���Ѵ�.
		pMember =  pFamily->GetMember( byCount ) ;
		if( !pMember ) continue ;

		pInfo = pMember->Get() ;
		if( !pInfo ) continue ;

		if( pInfo->eConState != CSHFamilyMember::MEMBER_CONSTATE_LOGIN ) continue ;

		++dwMemberCount ;
	}


	// �ش�  ������ �˻��� �� ������ �йи� ������ �����Ѵ�.
	USERINFO* pUserInfo ;

	MSG_DWORD2 msg ;
	memset( &msg, 0, sizeof(MSG_DWORD2) ) ;

	msg.Category				= MP_FAMILY ;
	msg.Protocol				= MP_FAMILY_REFRESH_FAMILYINFO_MAPSERVER ;
	msg.dwData1					= dwFamilyIDX ;
	msg.dwData2					= dwMemberCount ;

	for( BYTE byCount = 0 ; byCount < CSHFamily::MAX_MEMBER_NUM ; ++byCount )
	{
		// �йи� ��� ������ Ȯ���Ѵ�.
		pMember =  pFamily->GetMember( byCount ) ;
		if( !pMember ) continue ;

		pInfo = pMember->Get() ;
		if( !pInfo ) continue ;

		if( pInfo->eConState != CSHFamilyMember::MEMBER_CONSTATE_LOGIN ) continue ;

		pUserInfo = g_pUserTableForObjectID->FindUser( pInfo->nID ) ;
		if( !pUserInfo ) continue ;

		msg.dwObjectID				= pInfo->nID ;

		g_Network.Broadcast2MapServer( (char*)&msg, sizeof(MSG_DWORD2) ) ;
	}


	// �ٸ� Agent���� �޽��� ���� ó���� �Ѵ�.
	MSGBASE msgOther ;
	memset( &msgOther, 0, sizeof(MSGBASE) ) ;

	msgOther.Category				= MP_FAMILY ;
	msgOther.Protocol				= MP_FAMILY_REFRESH_FAMILYINFO_MAPSERVER_OTHER ;
	msgOther.dwObjectID				= dwFamilyIDX ;

	g_Network.Broadcast2AgentServerExceptSelf( (char*)&msgOther, sizeof(MSGBASE) ) ;
}
//
#endif //_AGENTSERVER

// -------------------------------------------------------------------------------------------------------------------------------------
//  ASRV_SendInfoToClient Method																		 Ŭ���̾�Ʈ�� �йи� ���� ������
//
VOID CSHFamilyManager::ASRV_SendInfoToClient(CSHFamily* pcsFamily, int nType)
{
#if defined(_AGENTSERVER)
	if(0 == pcsFamily)
	{
		return;
	}

	PACKET_FAMILY stPacket;
	PACKET_MEMBER_INFO stMemberPacket;

	for(UINT i = 0; i < pcsFamily->Get()->nMemberNum; ++i)
	{
		if(USERINFO* pMemberUserInfo = g_pUserTableForObjectID->FindUser(pcsFamily->GetMember(i)->Get()->nID))
		{
			if (nType == 0 || nType == 1)
			{
				stPacket.Category = MP_FAMILY;
				stPacket.Protocol = MP_FAMILY_DATA_FAMILY;
				stPacket.dwObjectID = pMemberUserInfo->dwCharacterID;
				stPacket.stInfo	= *pcsFamily->Get();
				stPacket.stInfoEx = *pcsFamily->GetEx();
				g_Network.Send2User(pMemberUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket) );
			}

			if (nType == 0 || nType == 2)
			{
				for(UINT j = 0; j < pcsFamily->Get()->nMemberNum; ++j)
				{
					stMemberPacket.Category = MP_FAMILY;
					stMemberPacket.Protocol	= MP_FAMILY_DATA_MEMBER;
					stMemberPacket.dwObjectID = pMemberUserInfo->dwCharacterID;
					stMemberPacket.nIndex = j;
					stMemberPacket.stInfo = *pcsFamily->GetMember(j)->Get();
					stMemberPacket.stInfoEx = *pcsFamily->GetMember(j)->GetEx();
					g_Network.Send2User( pMemberUserInfo->dwConnectionIndex, (char*)&stMemberPacket, sizeof(stMemberPacket) );
				}
			}
		}
	}

	ASRV_SendFamilyInfoToMapServer(
		pcsFamily->Get()->nID);
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  ASRV_SendMemberConToOtherAgent Method																�ٸ� ������Ʈ�� ��� ���� �˸���
//
VOID CSHFamilyManager::ASRV_SendMemberConToOtherAgent(CSHFamily* pcsFamily, DWORD nMemberID, int nConState, int nType)
{
#if defined(_AGENTSERVER)
	if (pcsFamily == NULL)
	{
		return;
	}

	MSG_DWORD4 stPacket;

	stPacket.Category	= MP_FAMILY;
	stPacket.Protocol	= MP_FAMILY_MEMBER_CONNECTION_TO_OTHER_AGENT;
	stPacket.dwData1	= pcsFamily->Get()->nID;
	stPacket.dwData2	= nMemberID;
	stPacket.dwData3	= nConState;
	stPacket.dwData4	= nType;
	g_Network.Broadcast2AgentServerExceptSelf((char*)&stPacket, sizeof(stPacket));
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  ASRV_MemberConFromOtherAgent Method																�ٸ� ������Ʈ�κ����� ��� ���� ó��
//
VOID CSHFamilyManager::ASRV_MemberConFromOtherAgent(DWORD nFamilyID, DWORD nMemberID, int nConState, int nType)
{
#if defined(_AGENTSERVER)
	// ���̺� �̹� �����ϴ��� üũ
	CSHFamily* pcsFamily = GetFamily(nFamilyID);
	if (pcsFamily == NULL)
	{
		return;
	}

	// �ش� ����� ���� ���� ����
	int nLoginCnt = 0;
	for(UINT i=0; i<pcsFamily->Get()->nMemberNum; i++)
	{
		if (pcsFamily->GetMember(i)->Get()->nID == nMemberID)
		{
			CSHFamilyMember::stINFO stInfo = *pcsFamily->GetMember(i)->Get();
			stInfo.eConState = (CSHFamilyMember::MEMBER_CONSTATE)nConState;
			pcsFamily->GetMember(i)->Set(&stInfo);
		}

		if (pcsFamily->GetMember(i)->Get()->eConState == CSHFamilyMember::MEMBER_CONSTATE_LOGIN)
		{
			nLoginCnt++;
		}
	}

	// ���� ������ �ִ� ����� HONOR_POINT_CHECK_MEMBER_NUM���� �Ǹ� �� ����Ʈ �ð��� �����Ѵ�.
	if (nLoginCnt == CSHFamilyManager::HONOR_POINT_CHECK_MEMBER_NUM && nConState == CSHFamilyMember::MEMBER_CONSTATE_LOGIN)
	{
		CSHFamily::stINFO_EX stInfoEx = *pcsFamily->GetEx();
		stInfoEx.nHonorPointTimeTick = gCurTime;
		pcsFamily->SetEx(&stInfoEx);
	}

	// ���� ������ �ִ� ����� HONOR_POINT_CHECK_MEMBER_NUM�� �̸��� �Ǹ� �� ����Ʈ �ð� �ʱ�ȭ
	if (nLoginCnt < CSHFamilyManager::HONOR_POINT_CHECK_MEMBER_NUM)
	{
		CSHFamily::stINFO_EX stInfoEx = *pcsFamily->GetEx();
		stInfoEx.nHonorPointTimeTick = 0;
		pcsFamily->SetEx(&stInfoEx);
	}

	// �ٸ� ������Ʈ�� �÷��̾ �����ؼ� ��� ���� ������ �޾��� ��쿡��..
	// �Ʒ����� ���� ������Ʈ���� ���� ������Ʈ�� ������ ������ ������.. üũ ���ϸ� ������ �ְ� �ް� �Ѵ�.
	if (nType == 0)
	{
		// ..��� ����� �α׾ƿ� ������ ���̺��� �йи� ����
		if (nConState == CSHFamilyMember::MEMBER_CONSTATE_LOGOFF)
		{
			if (nLoginCnt == 0)
			{
				int nUnitIndex = pcsFamily->GetIndexAtTbl()/MAX_FAMILY_1U_NUM;
				int n1UIndex = pcsFamily->GetIndexAtTbl()%MAX_FAMILY_1U_NUM;

				CSHFamily csFamily;
				m_pcsFamily[nUnitIndex][n1UIndex].Set(&csFamily);
				m_pcsFamily[nUnitIndex][n1UIndex].SetIndexAtTbl(NULL);
			}
		}

		// ������ ���� ������Ʈ���� ���� ������Ʈ�� �����ϴ� ����� ������ ������.
		if (nConState == CSHFamilyMember::MEMBER_CONSTATE_LOGIN)
		{
			for(UINT i=0; i<pcsFamily->Get()->nMemberNum; i++)
			{
				USERINFO* pMemberUserInfo = g_pUserTableForObjectID->FindUser(pcsFamily->GetMember(i)->Get()->nID);
				if (pMemberUserInfo && pcsFamily->GetMember(i)->Get()->eConState == CSHFamilyMember::MEMBER_CONSTATE_LOGIN)
				{
					ASRV_SendMemberConToOtherAgent(pcsFamily, pcsFamily->GetMember(i)->Get()->nID, pcsFamily->GetMember(i)->Get()->eConState, 1);
				}
			}
		}
	}

	// Ŭ���̾�Ʈ�� �˸���
	ASRV_SendInfoToClient(pcsFamily, 2);
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  ASRV_ChangeNicknameFromOtherAgent Method														�ٸ� ������Ʈ�κ����� ȣĪ ���� ó��
//
VOID CSHFamilyManager::ASRV_ChangeNicknameFromOtherAgent(DWORD nFamilyID, DWORD nBehavePlayerID, DWORD nTargetPlayerID, char* pszNickname)
{
#if defined(_AGENTSERVER)
	// ���̺� �̹� �����ϴ��� üũ
	CSHFamily* pcsFamily = GetFamily(nFamilyID);
	if (pcsFamily == NULL)
	{
		return;
	}

	// ����� ȣĪ�� �� ������Ʈ�� ���̺��� ����
	for(UINT i=0; i<pcsFamily->Get()->nMemberNum; i++)
	{
		if (nTargetPlayerID == pcsFamily->GetMember(i)->Get()->nID)
		{
			CSHFamilyMember::stINFO	stInfo = *pcsFamily->GetMember(i)->Get();
			
			SafeStrCpy(stInfo.szNickname, pszNickname, MAX_NAME_LENGTH+1);
			pcsFamily->GetMember(i)->Set(&stInfo);
			break;
		}
	}

	// Ŭ���̾�Ʈ�� �˸���
	ASRV_SendInfoToClient(pcsFamily, 2);

	USERINFO* pTargetUserInfo = g_pUserTableForObjectID->FindUser(nTargetPlayerID);
	if (pTargetUserInfo)
	{
		// �ʼ����� �˸���
		ASRV_SendNicknameToMap(pTargetUserInfo, pszNickname);

		// Ŭ���̾�Ʈ�� ���� �˸���
		MSG_DWORD_NAME stPacket;
		ZeroMemory(&stPacket, sizeof(stPacket));

		stPacket.Category			= MP_FAMILY;
		stPacket.Protocol			= MP_FAMILY_CHANGE_NICKNAME;
		stPacket.dwObjectID			= pTargetUserInfo->dwCharacterID;
		stPacket.dwData				= RESULT_OK_02;
		SafeStrCpy(
			stPacket.Name,
			pszNickname,
			_countof(stPacket.Name));
		g_Network.Send2User( pTargetUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket) );
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  ASRV_UpdateMapPlayerInfoFromOtherAgent Method									   �ٸ� ������Ʈ�κ����� ���� �÷��̾� ���� ������Ʈ
//
VOID CSHFamilyManager::ASRV_UpdateMapPlayerInfoFromOtherAgent(DWORD nFamilyID, DWORD nPlayerID, DWORD nLevel, char* pszGuildName)
{
#if defined(_AGENTSERVER)
	// ���̺� �̹� �����ϴ��� üũ
	CSHFamily* pcsFamily = GetFamily(nFamilyID);
	if (pcsFamily == NULL)
	{
		return;
	}

	// ������Ʈ �� ���� �ҷ�����
	for(UINT i=0; i<pcsFamily->Get()->nMemberNum; i++)
	{
		if (nPlayerID == pcsFamily->GetMember(i)->Get()->nID)
		{
			CSHFamilyMember::stINFO		stInfo		= *pcsFamily->GetMember(i)->Get();
			CSHFamilyMember::stINFO_EX	stInfoEx	= *pcsFamily->GetMember(i)->GetEx();

			stInfo.nLV = nLevel;
			if (pszGuildName)
			{
				SafeStrCpy(stInfoEx.szGuild, pszGuildName, MAX_NAME_LENGTH+1);
			}
			pcsFamily->GetMember(i)->Set(&stInfo, &stInfoEx);
			break;
		}
	}

	// Ŭ���̾�Ʈ�� �˸���
	ASRV_SendInfoToClient(pcsFamily, 2);
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  ASRV_UpdateHonorPointFromOtherAgent Method													  �ٸ� ������Ʈ�κ����� �� ����Ʈ ó��
//
VOID CSHFamilyManager::ASRV_UpdateHonorPointFromOtherAgent(DWORD nFamilyID, DWORD nHonorPoint, int nType)
{
#if defined(_AGENTSERVER)
	// ���̺� �̹� �����ϴ��� üũ
	CSHFamily* pcsFamily = GetFamily(nFamilyID);
	if (pcsFamily == NULL)
	{
		return;
	}

	// ���� �����̳� ���׷��̵��� ���� ���� ����Ʈ���� - int �� �ѱ��. �׷��� int �� ĳ�����ؼ� - �� ���� üũ�ؼ�
	// �׷� ��쿡�� -= �ϵ��� �Ѵ�. ���� ����Ʈ���� int ������� ����...
	CSHFamily::stINFO_EX stInfoEx = *pcsFamily->GetEx();
	if ((int)nHonorPoint < 0)
	{
		if (stInfoEx.nHonorPoint >= (DWORD)(-1*(int)nHonorPoint))
		{
			stInfoEx.nHonorPoint -= (DWORD)(-1*(int)nHonorPoint);
		}
		else
		{
			return;
		}
	}
	else
	{
		stInfoEx.nHonorPoint += nHonorPoint;
		if (stInfoEx.nHonorPoint > 4200000000) stInfoEx.nHonorPoint = 4200000000;
	}

	// �� ����Ʈ ���� �ð� �ʱ�ȭ
	if (nType == 1)
	{
		stInfoEx.nHonorPointTimeTick = gCurTime;
	}
	pcsFamily->SetEx(&stInfoEx);

	MSG_DWORD stPacket;
	// Ŭ���̾�Ʈ�� �˸���
	for(UINT i=0; i<pcsFamily->Get()->nMemberNum; i++)
	{
		USERINFO* pMemberUserInfo = g_pUserTableForObjectID->FindUser(pcsFamily->GetMember(i)->Get()->nID);
		if (pMemberUserInfo)
		{
			stPacket.Category			= MP_FAMILY;
			stPacket.Protocol			= MP_FAMILY_HONOR_POINT;
			stPacket.dwObjectID			= pMemberUserInfo->dwCharacterID;
			stPacket.dwData				= nHonorPoint;

			g_Network.Send2User( pMemberUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket) );
		}
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  ASRV_InviteFromOtherAgent Method																 �ٸ� ������Ʈ�κ����� �йи��� �ʴ�
//
VOID CSHFamilyManager::ASRV_InviteFromOtherAgent(USERINFO* pUserInfo, DWORD nMasterID, char* pszFamilyName)
{
#if defined(_AGENTSERVER)
	RESULT eResult = RESULT_OK;
	int nPastTime = 0;

	if(GetFamily(pUserInfo->mFamilyIndex))
	{
		eResult = RESULT_FAIL_04;
		goto FAILED;
	}

	// Ż�� ���Ƽ ������ ������?
	// ..DB�� int �� 0710111524(����Ͻð���)�� ���� �����. ���� int �� ���� ��¥������ ���游ŭ ���� ���� ����...��
	if (pUserInfo->nFamilyLeaveDate)
	{
		nPastTime = GetPastTime(pUserInfo->nFamilyLeaveDate);
		// ���� �ð� üũ
		if (nPastTime < LEAVE_PENALTY_REJOIN_TIME[pUserInfo->nFamilyLeaveKind])
		{
			eResult = RESULT_FAIL_06;
			goto FAILED;
		}
		// ���� �ʱ�ȭ
		Family_Leave_SaveInfo(pUserInfo->dwCharacterID, pUserInfo->nFamilyLeaveKind, TRUE);
	}

FAILED:
	if (eResult == RESULT_OK)
	{
		MSG_DWORD_NAME stPacket;
		stPacket.Category			= MP_FAMILY;
		stPacket.Protocol			= MP_FAMILY_INVITED;
		stPacket.dwObjectID			= pUserInfo->dwCharacterID;
		stPacket.dwData				= nMasterID;
		SafeStrCpy(stPacket.Name, pszFamilyName, MAX_NAME_LENGTH+1);
		g_Network.Send2User( pUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket) );
	}

	MSG_DWORD2 stPacket;
	stPacket.Category			= MP_FAMILY;
	stPacket.Protocol			= MP_FAMILY_INVITE;
	stPacket.dwObjectID			= nMasterID;
	stPacket.dwData1			= eResult;
	stPacket.dwData2			= nPastTime;
	g_Network.Broadcast2AgentServerExceptSelf((char*)&stPacket, sizeof(stPacket));
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  ASRV_InviteResultFromOtherAgent Method														�ٸ� ������Ʈ�κ����� �йи��� �ʴ� ���
//
VOID CSHFamilyManager::ASRV_InviteResultFromOtherAgent(PACKET_ADD_MEMBER* pstInfo)
{
#if defined(_AGENTSERVER)
	USERINFO* pMasterUserInfo = g_pUserTableForObjectID->FindUser(pstInfo->nMasterID);
	
	// �����Ͱ� ��ȿ�Ѱ�?
	if (pMasterUserInfo == NULL)
	{
		return;
	}

	CSHFamily* const family = GetFamily(
		pMasterUserInfo->mFamilyIndex);

	if(0 == family)
	{
		return;
	}
	else if(pstInfo->nMasterID != family->Get()->nMasterID)
	{
		return;
	}

	// ��� ������
	MSG_NAME_DWORD2 stPacket;
	// ..�����Ϳ��� ��� ������
	stPacket.Category			= MP_FAMILY;
	stPacket.Protocol			= MP_FAMILY_INVITE_RESULT;
	stPacket.dwObjectID			= pMasterUserInfo->dwCharacterID;
	stPacket.dwData1			= pstInfo->dwObjectID;
	if (pstInfo->nResult == CSHFamilyManager::FIR_ACCEPT_PLAYER)	stPacket.dwData2 = FIR_ACCEPT_MASTER;
	else															stPacket.dwData2 = FIR_DENY_MASTER;
	g_Network.Send2User( pMasterUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket) );

	// ..Ÿ�� �÷��̾�� ��� ������
	// ....�ٸ� ������Ʈ�� ���ؾ� �Ѵ�..
	stPacket.Category			= MP_FAMILY;
	stPacket.Protocol			= MP_FAMILY_INVITE_RESULT_TO_OTHER_AGENT_2;
	stPacket.dwObjectID			= pstInfo->dwObjectID;
	stPacket.dwData1			= pMasterUserInfo->dwCharacterID;
	stPacket.dwData2			= pstInfo->nResult;
	SafeStrCpy(
		stPacket.Name,
		family->Get()->szName,
		_countof(stPacket.Name));
	g_Network.Broadcast2AgentServerExceptSelf((char*)&stPacket, sizeof(stPacket));

	// �¶�
	if (pstInfo->nResult == FIR_ACCEPT_PLAYER)
	{
		if (family->Get()->nMemberNum == family->GetMemberNumMax())
		{
			return;
		}

		CSHFamilyMember csMember;
		CSHFamilyMember::stINFO stInfo;

		ZeroMemory(&stInfo, sizeof(stInfo));
		stInfo.nID			= pstInfo->dwObjectID;
		stInfo.nRace		= pstInfo->nRace;
		stInfo.nSex			= pstInfo->nSex;
		stInfo.nJobFirst	= pstInfo->nJobFirst;
		stInfo.nJobCur		= pstInfo->nJobCur;
		stInfo.nJobGrade	= pstInfo->nJobGrade;
		stInfo.nLV			= pstInfo->nLV;
		stInfo.eConState	= CSHFamilyMember::MEMBER_CONSTATE_LOGIN;
		SafeStrCpy(stInfo.szName, pstInfo->szName, MAX_NAME_LENGTH+1);

		csMember.Set(
			&stInfo);
		family->AddMember(
			&csMember);
		Family_Member_SaveInfo(
			family->Get()->nID,
			pstInfo->dwObjectID,
			NULL);

		// �ٸ� ������Ʈ�� �˸���
		PACKET_FAMILY_TOTAL stPacket;
		int nLoginCnt = 0;

		ZeroMemory(&stPacket, sizeof(stPacket));
		stPacket.Category			= MP_FAMILY;
		stPacket.Protocol			= MP_FAMILY_INVITE_RESULT_TO_OTHER_AGENT_3;
		stPacket.dwObjectID			= pstInfo->dwObjectID;
		stPacket.stFamily.stInfo	= *family->Get();
		stPacket.stFamily.stInfoEx	= *family->GetEx();

		for(UINT i = 0; i < family->Get()->nMemberNum; ++i)
		{
			stPacket.stMember[i].stInfo	= *family->GetMember(i)->Get();
			stPacket.stMember[i].stInfoEx = *family->GetMember(i)->GetEx();
			stPacket.stMember[i].nIndex	= i;

			if (stPacket.stMember[i].stInfo.eConState == CSHFamilyMember::MEMBER_CONSTATE_LOGIN)
			{
				nLoginCnt++;
			}
		}

		// ���� ������ �ִ� ����� HONOR_POINT_CHECK_MEMBER_NUM���� �Ǹ� �� ����Ʈ �ð��� �����Ѵ�.
		if (nLoginCnt == CSHFamilyManager::HONOR_POINT_CHECK_MEMBER_NUM)
		{
			family->GetEx()->nHonorPointTimeTick = gCurTime;
		}

		g_Network.Broadcast2AgentServerExceptSelf(
			(char*)&stPacket,
			sizeof(stPacket));
		ASRV_SendInfoToClient(
			family);
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  ASRV_InviteResultFromOtherAgent2 Method														�ٸ� ������Ʈ�κ����� �йи��� �ʴ� ���
//
VOID CSHFamilyManager::ASRV_InviteResultFromOtherAgent2(PACKET_FAMILY_TOTAL* pstInfo)
{
#if defined(_AGENTSERVER)
	CSHFamily* pcsFamily = GetFamily(pstInfo->stFamily.stInfo.nID);
	// �̹� ���̺� �йи��� ������ ��� �߰��� �Ѵ�.
	if (pcsFamily)
	{
		// �߰��� ������ �йи� ������ ����
		USERINFO* pUserInfo = g_pUserTableForObjectID->FindUser(pstInfo->dwObjectID);
		// ������ ���� ���̸� �йи� ������ ����. �ʴ� �����ϰ� ��ٷ� ������ ������ ������ ���� ���� �ִ�. 
		// ������ ������, ���̺��� �йи� ������ ����
		if (pUserInfo)
		{
			pUserInfo->mFamilyIndex = pcsFamily->Get()->nID;
		}

		// Ȥ�� �𸣴ϱ� ��� �� üũ
		if (pcsFamily->Get()->nMemberNum >= pcsFamily->GetMemberNumMax()) return;

		// ���� �������� �߰��� �����ϱ�..
		int nAddIndex = pstInfo->stFamily.stInfo.nMemberNum-1;

		// ��� �߰�
		CSHFamilyMember csFamilyMember;
		CSHFamilyMember::stINFO		stInfo		= pstInfo->stMember[nAddIndex].stInfo;
		CSHFamilyMember::stINFO_EX	stInfoEx	= pstInfo->stMember[nAddIndex].stInfoEx;

		csFamilyMember.Set(&stInfo, &stInfoEx);
		pcsFamily->AddMember(&csFamilyMember);

		int nLoginCnt = 0;
		for(UINT i=0; i<pcsFamily->Get()->nMemberNum; i++)
		{
			if (pcsFamily->GetMember(i)->Get()->eConState == CSHFamilyMember::MEMBER_CONSTATE_LOGIN)
			{
				nLoginCnt++;
			}
		}

		// ���� ������ �ִ� ����� HONOR_POINT_CHECK_MEMBER_NUM���� �Ǹ� �� ����Ʈ �ð��� �����Ѵ�.
		if (nLoginCnt == CSHFamilyManager::HONOR_POINT_CHECK_MEMBER_NUM)
		{
			pcsFamily->GetEx()->nHonorPointTimeTick = gCurTime;
		}

		// Ŭ���̾�Ʈ�� �йи� ���� ������
		ASRV_SendInfoToClient(pcsFamily);

		// ����� �� �÷��̾�� ���� ����
		if (pUserInfo)
		{
			Family_LoadEmblemInfo(
				pUserInfo->dwCharacterID,
				pcsFamily->Get()->nID);
			g_csFarmManager.ASRV_RequestFarmUIInfoToMap(
				pUserInfo->dwCharacterID,
				pcsFamily);
		}
	}
	// ���̺� ���� �߰��� �÷��̾ �����ϴ� ������Ʈ��� �йи��� ���̺� �߰��Ѵ�.
	else
	{
		USERINFO* pUserInfo = g_pUserTableForObjectID->FindUser(pstInfo->dwObjectID);
		if (pUserInfo == NULL)
		{
			return;
		}

		// �йи� ���� ����
		CSHFamily::stINFO			stInfo				= pstInfo->stFamily.stInfo;
		CSHFamily::stINFO_EX		stInfoEx			= pstInfo->stFamily.stInfoEx;
		CSHFamily csNewFamily;
		stInfoEx.nHonorPointTimeTick = 0;
		csNewFamily.Set(&stInfo, &stInfoEx);

		// ��� ���� ����
		CSHFamilyMember::stINFO		stMemberInfo;
		CSHFamilyMember::stINFO_EX	stMemberInfoEx;
		CSHFamilyMember csMember;
		int nLoginCnt = 0;
		for(UINT i=0; i<pstInfo->stFamily.stInfo.nMemberNum; i++)
		{
			stMemberInfo	= pstInfo->stMember[i].stInfo;
			stMemberInfoEx	= pstInfo->stMember[i].stInfoEx;
			csMember.Set(&stMemberInfo, &stMemberInfoEx);
			csNewFamily.SetMember(&csMember, i);
			if (pstInfo->stMember[i].stInfo.eConState == CSHFamilyMember::MEMBER_CONSTATE_LOGIN)
			{
				nLoginCnt++;
			}
		}

		pcsFamily = AddFamilyToTbl(
			&csNewFamily);

		// ���� ������ �ִ� ����� HONOR_POINT_CHECK_MEMBER_NUM���� �Ǹ� �� ����Ʈ �ð��� �����Ѵ�.
		// ..���̺� �߰��� �Ǵ� ��Ȳ�̹Ƿ� == �� �ƴ϶� >= ���� ��. ������ ����Ǿ� �ִ� ����� HONOR_POINT_CHECK_MEMBER_NUM �̻��� ���� �����Ƿ�..
		// ..�� �ڼ��ϰ� �������ڸ�, ������Ʈ1 �� �̹� 2���� �����ϰ� �ִ� �йи��� �־���. ������Ʈ2���� �� �йи� ����� ������ ���� �ʾƼ�
		// ..������Ʈ2 �� ���̺��� �� �йи��� ������. �� ��Ȳ���� ������Ʈ2 �� � �÷��̾ �� �йи��� ����� �� �� �� ��ƾ�� ����ȴ�.
		// ..�׷��� 3���� �α��� �� �����ε�, == �� ó���ϸ�, �Ʒ��� �ڵ尡 ������ ���� �ʴ� ���� �߻��Ѵ�. ����...
		// ..�� ��ƾ�� ���� �йи��� ���̺� �߰��� �Ŀ��� ����~�� ���� ��ƾ���� == �� üũ�Ѵ�.
		if (nLoginCnt >= CSHFamilyManager::HONOR_POINT_CHECK_MEMBER_NUM)
		{
			pcsFamily->GetEx()->nHonorPointTimeTick = gCurTime;
		}

		ASRV_SendInfoToClient(
			pcsFamily);
		Family_LoadEmblemInfo(
			pUserInfo->dwCharacterID,
			pcsFamily->Get()->nID);
		g_csFarmManager.ASRV_RequestFarmUIInfoToMap(
			pUserInfo->dwCharacterID,
			pcsFamily);
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_ParseRequestFromClient Method																		   Ŭ���̾�Ʈ ��û �м�/ó��
//
VOID CSHFamilyManager::SRV_ParseRequestFromClient(DWORD dwConnectionID, char* pMsg, DWORD dwLength)
{
#if defined(_MAPSERVER_)
	MSGBASE* pPacket = (MSGBASE*)pMsg;

	// �߸��� �÷��̾� ó��
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser( pPacket->dwObjectID );
	if( !pPlayer )	
	{
		switch(pPacket->Protocol)
		{
		// Ż�� (���� ����)
		case MP_FAMILY_LEAVE:
			{
				SRV_Leave(NULL, pPacket->dwObjectID, 1);
			}
			break;
		// �߹� (���� ����)
		case MP_FAMILY_EXPEL:
			{
				SRV_Expel(NULL, pPacket->dwObjectID, 1);
			}
			break;
		// ��ü (���� ����)
		case MP_FAMILY_BREAKUP:
			{
				SRV_BreakUp(NULL, pPacket->dwObjectID, 1);
			}
			break;
		}
		return;
	}

	switch(pPacket->Protocol)
	{
	// ����
	case MP_FAMILY_CREATE:
		{
			MSG_NAME* pPacket = (MSG_NAME*)pMsg;
			SRV_CreateFamily(pPlayer, pPacket->Name);
		}
		break;
	// �ʴ� ���
	case MP_FAMILY_INVITE_RESULT:
		{
			MSG_DWORD2* pPacket = (MSG_DWORD2*)pMsg;
			SRV_InviteResult(pPlayer, pPacket->dwData1, pPacket->dwData2);
		}
		break;
	// ȣĪ
	case MP_FAMILY_SHOW_NICKNAME:
		{
			MSG_NAME* pPacket = (MSG_NAME*)pMsg;
			pPlayer->SetFamilyNickName(
				pPacket->Name);

			MSG_NAME stPacket;
			stPacket.Category = MP_FAMILY;
			stPacket.Protocol = MP_FAMILY_SHOW_NICKNAME;
			stPacket.dwObjectID = pPlayer->GetID();
			SafeStrCpy(
				stPacket.Name,
				pPacket->Name,
				_countof(stPacket.Name));
			PACKEDDATA_OBJ->QuickSend(
				pPlayer,
				&stPacket,
				sizeof(stPacket));
		}
		break;
	// ���� ���
	case MP_FAMILY_REGIST_EMBLEM:
		{
			PACKET_FAMILY_EMBLEM* pstPacket = (PACKET_FAMILY_EMBLEM*)pMsg;

			SRV_RegistEmblem(pPlayer, pstPacket->nFamilyID, pstPacket->cImgData, pstPacket->nChangedFreq);
		}
		break;
	// 091111 ONS ���� ����
	case MP_FAMILY_DEL_EMBLEM:
		{
			MSG_DWORD2* pPacket = (MSG_DWORD2*)pMsg;
			SRV_DeleteEmblem(pPlayer, pPacket->dwData1, pPacket->dwData2);
		}
		break;
	// ���� ǥ��
	case MP_FAMILY_SHOW_EMBLEM:
		{
			PACKET_FAMILY_EMBLEM* pstPacket = (PACKET_FAMILY_EMBLEM*)pMsg;

			SRV_ShowEmblem(pPlayer, pstPacket->nFamilyID, pstPacket->cImgData, pstPacket->nChangedFreq);
		}
		break;
	// ���� ǥ��2
	case MP_FAMILY_SHOW_EMBLEM_2:
		{
			MSG_DWORD* pstPacket = (MSG_DWORD*)pMsg;

			SRV_ShowEmblem2(pPlayer, pstPacket->dwData);
		}
		break;
	// Ż��
	case MP_FAMILY_LEAVE:
		{
			SRV_Leave(pPlayer, pPlayer->GetID());
		}
		break;
	// �߹�
	case MP_FAMILY_EXPEL:
		{
			MSG_DWORD* pstPacket = (MSG_DWORD*)pMsg;

			SRV_Expel(pPlayer, pstPacket->dwData);
		}
		break;
	// ��ä
	case MP_FAMILY_BREAKUP:
		{
			MSG_DWORD* pstPacket = (MSG_DWORD*)pMsg;

			SRV_BreakUp(pPlayer, pstPacket->dwData);
		}
		break;
	// 090213 LYW --- SHFamilyManager : �йи� ��� ���� ���� ���� �߰� ó��.
	case MP_FAMILY_REFRESH_FAMILYINFO_MAPSERVER :
		{
			// ���� �޽��� ��ȯ.
			MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg ;
			if( !pmsg ) return ;

			// Player ������ �޴´�.
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser( pmsg->dwObjectID ) ;
			if( !pPlayer ) return ;

			// Player�� ���� ���� ���� ��� ���� �����Ѵ�.
			pPlayer->SetCurFamilyMemCnt( static_cast<BYTE>(pmsg->dwData2) ) ;
			// 100219 ONS �÷��̾��� �йи� �ε����� �����Ѵ�.
			pPlayer->SetFamilyIdx( pmsg->dwData1 );

			SRV_SetFamilyExp(
				*pPlayer);
			SRV_ActiveFamilyBuff(
				*pPlayer);
		}
		break ;

	// 090219 LYW --- SHFamilyManager : �ش� Player�� ã�Ƽ� �йи� �ε����� �����Ѵ�.
	case MP_FAMILY_SETFAMILYID_MAPSERVER :
		{
			// ���� �޽��� ��ȯ.
			MSG_DWORD* pmsg = (MSG_DWORD*)pMsg ;
			if( !pmsg ) return ;

			// Player ������ �޴´�.
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser( pmsg->dwObjectID ) ;
			if( !pPlayer ) return ;

			// �йи� ���� �ɷ��� ȸ���Ѵ�.
			if( pmsg->dwData == 0 )
			{
				// Player�� ���� ���� ���� ��� ���� �����Ѵ�.
				pPlayer->SetCurFamilyMemCnt( 0 ) ;

				SRV_SetFamilyExp(
					*pPlayer);
				SRV_ActiveFamilyBuff(
					*pPlayer);
			}

			// Player�� �йи� �ε����� �����Ѵ�.
			pPlayer->SetFamilyIdx( pmsg->dwData ) ;
		}
		break ;
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_CreateFamily Method																									 �йи� ����
//
VOID CSHFamilyManager::SRV_CreateFamily(CPlayer* pPlayer, char* szFamilyName)
{
#if defined(_MAPSERVER_)
	MSG_DWORD_NAME stPacket;

	// Ŭ���̾�Ʈ�� �˸���
	// ..Ŭ���̾�Ʈ�� �ٷ� ���� �ʰ� Agent���� ó���� ���� ó���ϰ� ����
	stPacket.Category			= MP_FAMILY;
	stPacket.Protocol			= MP_FAMILY_CREATE;
	stPacket.dwObjectID			= pPlayer->GetID();
	stPacket.dwData				= RESULT_OK;
	SafeStrCpy(stPacket.Name, szFamilyName, MAX_NAME_LENGTH+1);
	pPlayer->SendMsg(&stPacket, sizeof(stPacket));
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_InviteResult Method																							  �йи��� �ʴ� ���
//
VOID CSHFamilyManager::SRV_InviteResult(CPlayer* pPlayer, DWORD nMasterID, DWORD nResult)
{
#if defined(_MAPSERVER_)
	CPlayer* pMaster = (CPlayer*)g_pUserTable->FindUser( nMasterID );
	if( !pMaster ) return;

	PACKET_ADD_MEMBER stPacket;
	// Ŭ���̾�Ʈ�� �˸���
	// ..Ŭ���̾�Ʈ�� �ٷ� ���� �ʰ� Agent���� ó���� ���� ó���ϰ� ����
	stPacket.Category			= MP_FAMILY;
	stPacket.Protocol			= MP_FAMILY_INVITE_RESULT;
	stPacket.dwObjectID			= pPlayer->GetID();
	stPacket.nMasterID			= nMasterID;
	stPacket.nRace				= pPlayer->GetRacial();
	stPacket.nSex				= pPlayer->GetGender();
	stPacket.nJobFirst			= pPlayer->GetJop(0);
	stPacket.nJobCur			= pPlayer->GetJop(pPlayer->GetJopGrade()-1);
	stPacket.nJobGrade			= pPlayer->GetJopGrade();
	stPacket.nLV				= pPlayer->GetLevel();
	SafeStrCpy(stPacket.szName, pPlayer->GetObjectName(), MAX_NAME_LENGTH+1);
	stPacket.nResult			= nResult;

	pPlayer->SendMsg(&stPacket, sizeof(stPacket));
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_ProcessHonorPoint Method														  �� ����Ʈ ó�� (���� ��ɽ� ����ġ�� ����)
//
VOID CSHFamilyManager::SRV_ProcessHonorPoint(CPlayer* pPlayer, DWORD nExp)
{
#if defined(_MAPSERVER_)
	 nExp = (DWORD)(nExp*MONKILL_HONOR_POINT_EXP_RATE);

	if (nExp)
	{
		MSG_DWORD stPacket;
		// Ŭ���̾�Ʈ�� �˸���
		// ..Ŭ���̾�Ʈ�� �ٷ� ���� �ʰ� Agent���� ó���� ���� ó���ϰ� ����
		stPacket.Category			= MP_FAMILY;
		stPacket.Protocol			= MP_FAMILY_HONOR_POINT;
		stPacket.dwObjectID			= pPlayer->GetID();
		stPacket.dwData				= nExp;

		pPlayer->SendMsg(&stPacket, sizeof(stPacket));
	}
#endif
}


// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_ProcessHonorPoint Method
//
VOID CSHFamilyManager::SRV_ProcessHonorPointAdd(CPlayer* pPlayer, DWORD nExp)
{
#if defined(_MAPSERVER_)
	if (nExp)
	{
		MSG_DWORD stPacket;
		// Ŭ���̾�Ʈ�� �˸���
		// ..Ŭ���̾�Ʈ�� �ٷ� ���� �ʰ� Agent���� ó���� ���� ó���ϰ� ����
		stPacket.Category			= MP_FAMILY;
		stPacket.Protocol			= MP_FAMILY_HONOR_POINT;
		stPacket.dwObjectID			= pPlayer->GetID();
		stPacket.dwData				= nExp;

		pPlayer->SendMsg(&stPacket, sizeof(stPacket));
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_UpdateMapPlayerInfo Method																			 ���� �÷��̾� ���� ������Ʈ
//
VOID CSHFamilyManager::SRV_UpdateMapPlayerInfo(CPlayer* pPlayer, char* pszGuildName)
{
#if defined(_MAPSERVER_)
	MSG_DWORD_NAME stPacket;
	ZeroMemory(&stPacket, sizeof(stPacket));

	// Ŭ���̾�Ʈ�� �˸���
	// ..Ŭ���̾�Ʈ�� �ٷ� ���� �ʰ� Agent���� ó���� ���� ó���ϰ� ����
	stPacket.Category			= MP_FAMILY;
	stPacket.Protocol			= MP_FAMILY_UPDATE_MAP_PLAYER_INFO;
	stPacket.dwObjectID			= pPlayer->GetID();
	stPacket.dwData				= pPlayer->GetLevel();
	if (pszGuildName)
	{
		SafeStrCpy(stPacket.Name, pszGuildName, MAX_NAME_LENGTH+1);
	}

	pPlayer->SendMsg(&stPacket, sizeof(stPacket));
#endif
}

VOID CSHFamilyManager::SRV_RegistEmblem(CPlayer* pPlayer, DWORD nFamilyID, char* pImgData, DWORD nChangedNum)
{
#if defined(_MAPSERVER_)
	if (pPlayer->GetFamilyEmblem() == NULL)
	{
		char* pcEmblem = new char[EMBLEM_BUFSIZE];
		pPlayer->SetFamilyEmblem(pcEmblem);
	}
	memcpy(pPlayer->GetFamilyEmblem(), pImgData, EMBLEM_BUFSIZE);
	pPlayer->SetFamilyEmblemChangedFreq(nChangedNum);

	PACKET_FAMILY_EMBLEM stPacket;

	stPacket.Category			= MP_FAMILY;
	stPacket.Protocol			= MP_FAMILY_REGIST_EMBLEM;
	stPacket.dwObjectID			= pPlayer->GetID();
	stPacket.nFamilyID			= nFamilyID;
	stPacket.nChangedFreq		= pPlayer->GetFamilyEmblemChangedFreq();
	stPacket.nTargetPlayerID	= pPlayer->GetID();
	stPacket.bIsLoad			= FALSE;
	memcpy(stPacket.cImgData, pImgData, EMBLEM_BUFSIZE);

	PACKEDDATA_OBJ->QuickSend(pPlayer, &stPacket, sizeof(stPacket));
#endif
}

// 091111 ONS �йи� ���� ���� ó��
// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_DeleteEmblem Method																									   ���� ����
//
VOID CSHFamilyManager::SRV_DeleteEmblem(CPlayer* pPlayer, DWORD nFamilyID, DWORD nChangedNum)
{
#if defined(_MAPSERVER_)
	
	if (pPlayer == NULL) 
		return;

	if (pPlayer->GetFamilyEmblem())
	{
		char* pcEmblem = pPlayer->GetFamilyEmblem();
		delete[] pcEmblem;
		pPlayer->SetFamilyEmblem(NULL);

		pPlayer->SetFamilyEmblemChangedFreq(nChangedNum);

		MSG_DWORD stPacket;
		ZeroMemory(&stPacket, sizeof(MSG_DWORD));
		stPacket.Category			= MP_FAMILY;
		stPacket.Protocol			= MP_FAMILY_DEL_EMBLEM;
		stPacket.dwData				= pPlayer->GetID();
		PACKEDDATA_OBJ->QuickSend(pPlayer, &stPacket, sizeof(stPacket));
	}

#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_ShowEmblem Method																									   ���� ǥ��
//
VOID CSHFamilyManager::SRV_ShowEmblem(CPlayer* pPlayer, DWORD nFamilyID, char* pImgData, DWORD nChangedNum)
{
#if defined(_MAPSERVER_)
	// �ʼ����� �йи�ID, ���� ���� ����
	pPlayer->SetFamilyIdx(nFamilyID);
	if (pPlayer->GetFamilyEmblem() == NULL)
	{
		char* pcEmblem = new char[EMBLEM_BUFSIZE];
		pPlayer->SetFamilyEmblem(pcEmblem);
	}
	memcpy(pPlayer->GetFamilyEmblem(), pImgData, EMBLEM_BUFSIZE);
	pPlayer->SetFamilyEmblemChangedFreq(nChangedNum);

	MSG_DWORD3 stPacket;

	stPacket.Category			= MP_FAMILY;
	stPacket.Protocol			= MP_FAMILY_SHOW_EMBLEM;
	stPacket.dwObjectID			= pPlayer->GetID();
	stPacket.dwData1			= nFamilyID;
	stPacket.dwData2			= pPlayer->GetID();
	stPacket.dwData3			= pPlayer->GetFamilyEmblemChangedFreq();
	// QuickSend �δ� pPlayer ���� ������ ���� �ʴ´�.. ������Ʈ���� ������ �ε��ؼ� ���� �����͸�
	// ����(��)���� ���� �ǵ�, �� ���������� pPlayer �� �׸��忡 �߰��Ǿ� ���� ���� ��...??
	// �׷��� �Ʒ�ó�� 2���� ������ ����
	PACKEDDATA_OBJ->QuickSendExceptObjectSelf(pPlayer, &stPacket, sizeof(stPacket));
	pPlayer->SendMsg(&stPacket, sizeof(stPacket));
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_ShowEmblem2 Method																									  ���� ǥ��2
//
VOID CSHFamilyManager::SRV_ShowEmblem2(CPlayer* pPlayer, DWORD nTargetPlayerID)
{
#if defined(_MAPSERVER_)
	CPlayer* pTargetPlayer = (CPlayer*)g_pUserTable->FindUser( nTargetPlayerID );
	if( !pTargetPlayer ) return;
	
	if (pTargetPlayer->GetFamilyEmblem())
	{
		PACKET_FAMILY_EMBLEM stPacket;

		stPacket.Category			= MP_FAMILY;
		stPacket.Protocol			= MP_FAMILY_REGIST_EMBLEM;
		stPacket.dwObjectID			= pPlayer->GetID();
		stPacket.nFamilyID			= pTargetPlayer->GetFamilyIdx();
		stPacket.nTargetPlayerID	= nTargetPlayerID;
		stPacket.nChangedFreq		= pTargetPlayer->GetFamilyEmblemChangedFreq();
		stPacket.bIsLoad			= TRUE;
		memcpy(stPacket.cImgData, pTargetPlayer->GetFamilyEmblem(), EMBLEM_BUFSIZE);

		pPlayer->SendMsg(&stPacket, sizeof(stPacket));
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_RegistEmblem Method																											Ż��
//
VOID CSHFamilyManager::SRV_Leave(CPlayer* pPlayer, DWORD nPlayerID, int nKind)
{
#if defined(_MAPSERVER_)
	switch(nKind)
	{
	// ���� ����, ����ġ �ٿ�
	case 0:
		{
			if (pPlayer == NULL) return;
			// ���� ����
			if (pPlayer->GetFamilyEmblem())
			{
				char* pcEmblem = pPlayer->GetFamilyEmblem();
				delete[] pcEmblem;
				pPlayer->SetFamilyEmblem(NULL);
			}

			char szName[MAX_NAME_LENGTH+1] = "";
			pPlayer->SetFamilyNickName(szName);

			MSG_DWORD stPacket;

			stPacket.Category			= MP_FAMILY;
			stPacket.Protocol			= MP_FAMILY_LEAVE_EMBLEM;
			stPacket.dwData				= pPlayer->GetID();

			PACKEDDATA_OBJ->QuickSend(pPlayer, &stPacket, sizeof(stPacket));

			// ����ġ �ٿ�
			if (LEAVE_PENALTY_EXP_DOWN_RATE[FLK_LEAVE])
			{
				LEVELTYPE nOldLevel = pPlayer->GetLevel();
				EXPTYPE pointForLevel = GAMERESRCMNGR->GetMaxExpPoint(pPlayer->GetLevel());
				EXPTYPE minusExp = (EXPTYPE)(pointForLevel * LEAVE_PENALTY_EXP_DOWN_RATE[FLK_LEAVE]);
				pPlayer->ReduceExpPoint(minusExp, eExpLog_LosebyLeaveFamily);
				// ..�޼���
				MSG_DWORD3 msg;													
				msg.Category	= MP_FAMILY ;										
				msg.Protocol	= MP_FAMILY_PENALTY_MSG;			
				msg.dwObjectID	= pPlayer->GetID();
				msg.dwData1		= 0;
				msg.dwData2		= (DWORD)(LEAVE_PENALTY_EXP_DOWN_RATE[FLK_LEAVE]*100+0.001f);
				msg.dwData3		= 0;
				if (nOldLevel > pPlayer->GetLevel()) msg.dwData3 = 1;
				pPlayer->SendMsg((MSGBASE*)&msg, sizeof(msg));
			}

			// Ż���ϴ� ����� ���� ���۹��� �ִ��� Ȯ���ϰ� ����..
			g_csFarmManager.SRV_DelCrop(nPlayerID);
			// 080423 KTH -- ������ ������ ����
			g_csFarmManager.SRV_DelAnimal(nPlayerID);
			pPlayer->SetFarmCropPlantRetryTime(0);
			pPlayer->SetFarmCropManureRetryTime(0);
			// DB�� ����
			Farm_SetTimeDelay(pPlayer->GetID(), CSHFarmManager::FARM_TIMEDELAY_KIND_PLANT, 0);
			Farm_SetTimeDelay(pPlayer->GetID(), CSHFarmManager::FARM_TIMEDELAY_KIND_MANURE, 0);
			// 080430 KTH -- Animal Delay Add
		}
		break;
	// ���۹� ����
	case 1:
		{
			// Ż���ϴ� ����� ���� ���۹��� �ִ��� Ȯ���ϰ� ����..
			g_csFarmManager.SRV_DelCrop(nPlayerID);
			// 080423 KTH
			g_csFarmManager.SRV_DelAnimal(nPlayerID);
		}
		break;
	}
	
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_Expel Method																												�߹�
//
VOID CSHFamilyManager::SRV_Expel(CPlayer* pPlayer, DWORD nMemberID, int nKind)
{
#if defined(_MAPSERVER_)
	switch(nKind)
	{
	// ���� ����
	case 0:
		{
			if (pPlayer == NULL) return;
			// ����� ���
			if (pPlayer->GetID() == nMemberID)
			{
				// ���� ����
				if (pPlayer->GetFamilyEmblem())
				{
					char* pcEmblem = pPlayer->GetFamilyEmblem();
					delete[] pcEmblem;
					pPlayer->SetFamilyEmblem(NULL);
				}

				char szName[MAX_NAME_LENGTH+1] = "";
				pPlayer->SetFamilyNickName(szName);

				MSG_DWORD stPacket;

				stPacket.Category			= MP_FAMILY;
				stPacket.Protocol			= MP_FAMILY_LEAVE_EMBLEM;
				stPacket.dwData				= pPlayer->GetID();

				PACKEDDATA_OBJ->QuickSend(pPlayer, &stPacket, sizeof(stPacket));

				// Ż���ϴ� ����� ���� ���۹��� �ִ��� Ȯ���ϰ� ����..
				g_csFarmManager.SRV_DelCrop(nMemberID);
				// 080423 KTH
				g_csFarmManager.SRV_DelAnimal(nMemberID);
				pPlayer->SetFarmCropPlantRetryTime(0);
				pPlayer->SetFarmCropManureRetryTime(0);
				// DB�� ����
				Farm_SetTimeDelay(pPlayer->GetID(), CSHFarmManager::FARM_TIMEDELAY_KIND_PLANT, 0);
				Farm_SetTimeDelay(pPlayer->GetID(), CSHFarmManager::FARM_TIMEDELAY_KIND_MANURE, 0);
			}
			// �������� ���
			else
			{
				// ����ġ �ٿ�
				if (LEAVE_PENALTY_EXP_DOWN_RATE[FLK_EXPEL])
				{
					LEVELTYPE nOldLevel = pPlayer->GetLevel();
					EXPTYPE pointForLevel = GAMERESRCMNGR->GetMaxExpPoint(pPlayer->GetLevel());
					EXPTYPE minusExp = (EXPTYPE)(pointForLevel * LEAVE_PENALTY_EXP_DOWN_RATE[FLK_EXPEL]);
					pPlayer->ReduceExpPoint(minusExp, eExpLog_LosebyExpleFamilyMember);
					// ..�޼���
					MSG_DWORD3 msg ;													
					msg.Category	= MP_FAMILY ;										
					msg.Protocol	= MP_FAMILY_PENALTY_MSG;			
					msg.dwObjectID	= pPlayer->GetID();
					msg.dwData1		= 1;
					msg.dwData2		= (DWORD)(LEAVE_PENALTY_EXP_DOWN_RATE[FLK_EXPEL]*100+0.001f);
					msg.dwData3		= 0;
					if (nOldLevel > pPlayer->GetLevel()) msg.dwData3 = 1;
					pPlayer->SendMsg((MSGBASE*)&msg, sizeof(msg));
				}
			}
		}
		break;
	// ���۹� ����
	case 1:
		{
			// Ż���ϴ� ����� ���� ���۹��� �ִ��� Ȯ���ϰ� ����..
			g_csFarmManager.SRV_DelCrop(nMemberID);
			// 080423 KTH
			g_csFarmManager.SRV_DelAnimal(nMemberID);
		}
		break;
	}
	
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_BreakUp Method																												��ü
//
VOID CSHFamilyManager::SRV_BreakUp(CPlayer* pPlayer, DWORD nPlayerID, int nKind)
{
#if defined(_MAPSERVER_)
	switch(nKind)
	{
	// ���� ����
	case 0:
		{
			if (pPlayer == NULL) return;
			
			// ���� ����
			if (pPlayer->GetFamilyEmblem())
			{
				char* pcEmblem = pPlayer->GetFamilyEmblem();
				delete[] pcEmblem;
				pPlayer->SetFamilyEmblem(NULL);
			}

			char szName[MAX_NAME_LENGTH+1] = "";
			pPlayer->SetFamilyNickName(szName);

			MSG_DWORD stPacket;

			stPacket.Category			= MP_FAMILY;
			stPacket.Protocol			= MP_FAMILY_LEAVE_EMBLEM;
			stPacket.dwData				= pPlayer->GetID();

			PACKEDDATA_OBJ->QuickSend(pPlayer, &stPacket, sizeof(stPacket));

			pPlayer->SetFarmCropPlantRetryTime(0);
			pPlayer->SetFarmCropManureRetryTime(0);
			// DB�� ����
			Farm_SetTimeDelay(pPlayer->GetID(), CSHFarmManager::FARM_TIMEDELAY_KIND_PLANT, 0);
			Farm_SetTimeDelay(pPlayer->GetID(), CSHFarmManager::FARM_TIMEDELAY_KIND_MANURE, 0);

			// 080430 KTH -- Animal Delay Add

			// �������� ���
			if (pPlayer->GetID() == nPlayerID)
			{
				// ����ġ �ٿ�
				if (LEAVE_PENALTY_EXP_DOWN_RATE[FLK_BREAKUP])
				{
					LEVELTYPE nOldLevel = pPlayer->GetLevel();
					EXPTYPE pointForLevel = GAMERESRCMNGR->GetMaxExpPoint(pPlayer->GetLevel());
					EXPTYPE minusExp = (EXPTYPE)(pointForLevel * LEAVE_PENALTY_EXP_DOWN_RATE[FLK_BREAKUP]);
					pPlayer->ReduceExpPoint(minusExp, eExpLog_LosebyBreakupFamily);
					// ..�޼���
					MSG_DWORD3 msg ;													
					msg.Category	= MP_FAMILY ;										
					msg.Protocol	= MP_FAMILY_PENALTY_MSG;			
					msg.dwObjectID	= pPlayer->GetID();
					msg.dwData1		= 2;
					msg.dwData2		= (DWORD)(LEAVE_PENALTY_EXP_DOWN_RATE[FLK_BREAKUP]*100+0.001f);
					msg.dwData3		= 0;
					if (nOldLevel > pPlayer->GetLevel()) msg.dwData3 = 1;
					pPlayer->SendMsg((MSGBASE*)&msg, sizeof(msg));
				}
				// ���� ����
				g_csFarmManager.SRV_DelFarm(pPlayer->GetID(), CSHFarmManager::FARM_DEL_KIND_BREAKUP_FAMILY);
			}
		}
		break;
	// ���۹� ����
	case 1:
		{
			// ���� ����
			g_csFarmManager.SRV_DelFarm(nPlayerID, CSHFarmManager::FARM_DEL_KIND_BREAKUP_FAMILY);
		}
		break;
	}
	
#endif
}




//----------------------------------------------------------------------------------------------------------------
// 090212 LYW --- SHFamilyManager : �йи� �ɹ� ���� ���� ���� �߰� ��� ó��.
#ifdef _MAPSERVER_
VOID CSHFamilyManager::SRV_SetFamilyExp(CPlayer& pPlayer)
{
	const DWORD dwMemberCount = pPlayer.GetCurFamilyMemCnt();

	if( dwMemberCount >= m_byMinMemberCnt )
	{
		pPlayer.SetFamilyRewardExp(
			dwMemberCount * (m_wRewardExprate + m_dwRewardExpValue));
	}

	pPlayer.SetCurFamilyMemCnt(
		BYTE(dwMemberCount));
}

VOID CSHFamilyManager::SRV_ActiveFamilyBuff(CPlayer& pPlayer)
{
	if(0 == pPlayer.GetFamilyIdx())
	{
		return;
	}

	const DWORD dwMemberCount = pPlayer.GetCurFamilyMemCnt() ;

	for( BYTE byCount = 0 ; byCount < _countof(m_dwRewardBuffIdxs); ++byCount )
	{
		if(byCount < dwMemberCount)
		{
			cSkillInfo* const pSkillInfo = SKILLMGR->GetSkillInfo(
				m_dwRewardBuffIdxs[byCount]);

			if(0 == pSkillInfo)
			{
				continue;
			}
			else if(cBuffSkillObject* skillObject = pPlayer.GetBuffList().GetData(pSkillInfo->GetIndex() / 100))
			{
				continue;
			}

			sSKILL_CREATE_INFO stCreateInfo;
			stCreateInfo.level = pSkillInfo->GetLevel();
			stCreateInfo.operatorId	= pPlayer.GetID();
			stCreateInfo.mainTarget.SetMainTarget(
				pPlayer.GetID());
			stCreateInfo.pos = *CCharMove::GetPosition(
				&pPlayer);
			stCreateInfo.count = 1;

			SKILLMGR->OnBuffSkillStart(
				pSkillInfo,
				&stCreateInfo);
		}
		else
		{
			for(DWORD i = dwMemberCount; i < MAX_BUFF_COUNT; ++i)
			{
				if(cBuffSkillObject* const skillObject = pPlayer.GetBuffList().GetData(m_dwRewardBuffIdxs[i] / 100))
				{
					skillObject->SetEndState();
				}
			}
		}
	}
}

BOOL CSHFamilyManager::IsFamilyRewardBuff(DWORD dwIndex)
{
	for(WORD i=0; i<CSHFamily::MAX_MEMBER_NUM; i++)
	{
		if(m_dwRewardBuffIdxs[i] == dwIndex)
			return TRUE;
	}

	return FALSE;
}
//
//
#endif // _MAPSERVER_

// -------------------------------------------------------------------------------------------------------------------------------------
// CLI_RequestCreateFamily Method																					    �йи� ���� ��û
//
VOID CSHFamilyManager::CLI_RequestCreateFamily(char* szFamilyName)
{
#if !defined(_AGENTSERVER) && !defined(_MAPSERVER_)
	/*
	if( ( FILTERTABLE->IsInvalidCharInclude((unsigned char *)FamilyName) == TRUE ) || 
		( FILTERTABLE->IsUsableName(FamilyName) == FALSE ) )
	{
		CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(698));
		return;
	}
	*/

	MSG_DWORD_NAME stPacket;
	stPacket.Category = MP_FAMILY;
	stPacket.Protocol = MP_FAMILY_CREATE;
	SafeStrCpy(stPacket.Name, szFamilyName, MAX_NAME_LENGTH+1);
	stPacket.dwObjectID = HEROID;

	NETWORK->Send(&stPacket, sizeof(stPacket));
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
// CLI_RequestInvite Method																						  �йи������� �ʴ� ��û
//
VOID CSHFamilyManager::CLI_RequestInvite(CPlayer* pPlayer)
{
#if !defined(_AGENTSERVER) && !defined(_MAPSERVER_)
	MSG_DWORD stPacket;
	stPacket.Category 		= MP_FAMILY;
	stPacket.Protocol 		= MP_FAMILY_INVITE;
	stPacket.dwObjectID		= HEROID;
	stPacket.dwData			= pPlayer->GetID();

	NETWORK->Send( (MSGROOT*)&stPacket, sizeof(stPacket) );
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
// CLI_RequestAnswerToInvite Method																				�йи��� �ʴ뿡 ���� ��û
//
VOID CSHFamilyManager::CLI_RequestAnswerToInvite(FAMILY_INVITE_RESULT eResult)
{
#if !defined(_AGENTSERVER) && !defined(_MAPSERVER_)
	if (m_nInviteMasterID)
	{
		MSG_DWORD2 stPacket;
		stPacket.Category 		= MP_FAMILY;
		stPacket.Protocol 		= MP_FAMILY_INVITE_RESULT;
		stPacket.dwObjectID		= HEROID;
		stPacket.dwData1		= m_nInviteMasterID;
		stPacket.dwData2		= eResult;

		NETWORK->Send( (MSGROOT*)&stPacket, sizeof(stPacket) );
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
// CLI_RequestRegistEmblem Method																						   ���� ��� ��û
//
BOOL CSHFamilyManager::CLI_RequestRegistEmblem(char* pszBmpName)
{
#if !defined(_AGENTSERVER) && !defined(_MAPSERVER_)
	char cImgData[EMBLEM_BUFSIZE];

 	FIBITMAP* pBitMap = FreeImage_Load(FIF_BMP, pszBmpName, BMP_DEFAULT);
	BITMAPINFO* pBitInfo = FreeImage_GetInfo(pBitMap);

	if( (pBitMap == 0) || (pBitInfo == 0) )
	{
		char szTmp[256];
		sprintf(szTmp, "%s.bmp", pszBmpName);
		pBitMap = FreeImage_Load(FIF_BMP, szTmp, BMP_DEFAULT);
		pBitInfo = FreeImage_GetInfo(pBitMap);
		if( (pBitMap == 0) || (pBitInfo == 0) )
			return FALSE;
	}
	//����� Ʋ��
	if( pBitInfo->bmiHeader.biHeight != EMBLEM_HEIGHT ||
		pBitInfo->bmiHeader.biWidth != EMBLEM_WIDTH )
		return FALSE;

	FIBITMAP* pConvertedBitmap = FreeImage_ConvertTo16Bits565(pBitMap);

	int bytesPerLine = EMBLEM_WIDTH*EMBLEM_BPP;
	BYTE* pData = FreeImage_GetBits(pConvertedBitmap);
	for(int y=0;y<EMBLEM_HEIGHT;++y)
	{
		memcpy(
			&cImgData[(EMBLEM_HEIGHT-y-1)*bytesPerLine],
			&pData[y*bytesPerLine],
			bytesPerLine);
	}
	
	FreeImage_Unload(pBitMap);
	FreeImage_Unload(pConvertedBitmap);

	PACKET_FAMILY_EMBLEM msg;

	SetProtocol(&msg,MP_FAMILY,MP_FAMILY_REGIST_EMBLEM);
	msg.dwObjectID = HEROID;
	msg.nFamilyID = HERO->GetFamily()->Get()->nID;
	msg.nTargetPlayerID = 0;
	msg.bIsLoad = FALSE;
	memcpy(msg.cImgData,cImgData,EMBLEM_BUFSIZE);
	NETWORK->Send(&msg,sizeof(msg));

#endif
	return TRUE;
}

// -------------------------------------------------------------------------------------------------------------------------------------
// CLI_RequestShowEmblem Method															 ���� ǥ�� ��û (�׸������ �ٸ� �÷��̾�鿡��)
//
VOID CSHFamilyManager::CLI_RequestShowEmblem(DWORD nPlayerID, DWORD nTargetPlayerID)
{
#if !defined(_AGENTSERVER) && !defined(_MAPSERVER_)
	MSG_DWORD stPacket;

	stPacket.Category	= MP_FAMILY;
	stPacket.Protocol	= MP_FAMILY_SHOW_EMBLEM_2;
	stPacket.dwObjectID = nPlayerID;
	stPacket.dwData		= nTargetPlayerID;
	NETWORK->Send(&stPacket, sizeof(stPacket));
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
// CLI_ShowEmblem Method																									   ���� ǥ��
//
VOID CSHFamilyManager::CLI_ShowEmblem(CPlayer* pPlayer, DWORD nFamilyID, DWORD nChangedFreq)
{
#if !defined(_AGENTSERVER) && !defined(_MAPSERVER_)
	// �̹� ���۹޾Ƽ� �����ϰ� �ִ� �����̶��, �ش� ������ ������ ����Ѵ�.
	char szEmblem[256];
	DWORD nOldChangedFreq = 0;
	sprintf(szEmblem, "Data/Interface/2DImage/FamilyEmblem/%d_%d.txt", g_nServerSetNum, nFamilyID);
	FILE* fp = fopen(szEmblem, "rt");
	if (fp)
	{
		// �ٸ� �׸���(�йи� ����� �ƴ� ���), ĳ���� ����ȭ��, ���̵� �� ��� ��쿡�� ����Ǵ� ������ ���۹��� ���Ѵ�.
		// �׷���, ������ ���� ȸ���� ���ؼ� Ŭ���̾�Ʈ ���� ���� ȸ���� ���� ���� ȸ�� ���� ������ 
		// ����� ������ ��û�Ѵ�. 
		// ������ ���� �׸��� ������ ����� �ٸ� �йи��� ������ ���� �� �׸��� ������ ���� ��Ȳ���� �� �йи��� ������ ��
		// ����Ǿ��� ���, �� �йи� ����� �α׾ƿ��� ���� �ʰ�, �ٽ� �׸��� ������ ���� ���� ����� ������ ���� �� ������
		// �α׾ƿ� �� �ٽ� �������� ��쿡�� (��� �йи� �����.. �׷��� ���� ���� ȸ���� �ʱ�ȭ�� �Ǹ�) ���� ȸ���� �ʱ�ȭ�Ǿ
		// ����� ������ ���� �� ����, ���������� ������ �������� ǥ���ϰ� �ȴ�. ���� ȸ���� DB�� �������� �ʴ� �̻� ��¿ �� ����.
		// Ŭ���̾�Ʈ ���� �� ó�� ���� �ÿ� ���� ������ �ʱ�ȭ�ϱ� ������ Ŭ���̾�Ʈ�� �ٽ� �����ϸ� �ذ�Ǵ� ������.
		fscanf(fp, "%d", &nOldChangedFreq);
		if (nOldChangedFreq < nChangedFreq)
		{
			CLI_RequestShowEmblem(HERO->GetID(), pPlayer->GetID());
		}
		else
		{
			sprintf(szEmblem, "Data/Interface/2DImage/FamilyEmblem/%d_%d_%d.tga", g_nServerSetNum, nFamilyID, nOldChangedFreq);
			if (_access(szEmblem, 0) != -1)
			{
				cImageSelf* pEmblem = pPlayer->GetFamilyEmblem();
				if (pEmblem)
				{
					pEmblem->Release();
					delete pEmblem;
				}

				pEmblem = new cImageSelf;
				pEmblem->LoadSprite(szEmblem, EMBLEM_WIDTH, EMBLEM_HEIGHT);
				pPlayer->SetFamilyEmblem(pEmblem);
			}
		}
		fclose(fp);
	}
	// ������ �ٽ� ������ ��û
	else
	{
		CLI_RequestShowEmblem(HERO->GetID(), pPlayer->GetID());
	}
#endif
}

// 091111 ONS �йи� ���� ���� 
// -------------------------------------------------------------------------------------------------------------------------------------
// CLI_DeleteEmblem Method																									   ���� ǥ��
//
VOID CSHFamilyManager::CLI_RequestDeleteEmblem()
{
#if !defined(_AGENTSERVER) && !defined(_MAPSERVER_)

	MSG_DWORD message;
	ZeroMemory(&message, sizeof(MSG_DWORD));
	message.Category	= MP_FAMILY;
	message.Protocol	= MP_FAMILY_DEL_EMBLEM;
	message.dwObjectID	= HERO->GetID();
	message.dwData		= HERO->GetFamilyIdx();
	NETWORK->Send( &message, sizeof( message ) );

#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
// CLI_RequestChangeNickname Method																						  ȣĪ ���� ��û
//
VOID CSHFamilyManager::CLI_RequestChangeNickname(DWORD nTargetPlayerID, char* szNickname)
{
#if !defined(_AGENTSERVER) && !defined(_MAPSERVER_)
	MSG_DWORD_NAME stPacket;
	stPacket.Category 		= MP_FAMILY;
	stPacket.Protocol 		= MP_FAMILY_CHANGE_NICKNAME;
	stPacket.dwObjectID		= HEROID;								// ȣĪ�� �����ϴ� ĳ����ID
	stPacket.dwData			= nTargetPlayerID;						// ȣĪ�� ����� ĳ����ID
	SafeStrCpy(stPacket.Name, szNickname, MAX_NAME_LENGTH+1);

	// ' �� ���ڿ��� ��Ÿ���� ǥ���̹Ƿ� ���ڿ��� ����� �� ����. " �� ��ü�ϵ��� ��.
	for(int i=0; i<MAX_NAME_LENGTH+1; i++)
	{
		if (stPacket.Name[i] == '\'')
		{
			stPacket.Name[i] = '"';
		}
	}

	NETWORK->Send( (MSGROOT*)&stPacket, sizeof(stPacket) );
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
// CLI_RequestRegistEmblem Method																							  ȣĪON ��û
//
VOID CSHFamilyManager::CLI_RequestChangeNicknameON()
{
#if !defined(_AGENTSERVER) && !defined(_MAPSERVER_)
	MSGBASE stPacket;
	stPacket.Category 		= MP_FAMILY;
	stPacket.Protocol 		= MP_FAMILY_CHANGE_NICKNAMEON;
	stPacket.dwObjectID		= HEROID;

	NETWORK->Send( (MSGROOT*)&stPacket, sizeof(stPacket) );
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
// CLI_RequestLeave Method																									   Ż�� ��û
//
VOID CSHFamilyManager::CLI_RequestLeave()
{
#if !defined(_AGENTSERVER) && !defined(_MAPSERVER_)
	MSGBASE stPacket;
	stPacket.Category 		= MP_FAMILY;
	stPacket.Protocol 		= MP_FAMILY_LEAVE;
	stPacket.dwObjectID		= HEROID;

	NETWORK->Send( (MSGROOT*)&stPacket, sizeof(stPacket) );
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
// CLI_RequestExpel Method																									   �߹� ��û
//
VOID CSHFamilyManager::CLI_RequestExpel(int nMemberID)
{
#if !defined(_AGENTSERVER) && !defined(_MAPSERVER_)
	MSG_DWORD stPacket;
	stPacket.Category 		= MP_FAMILY;
	stPacket.Protocol 		= MP_FAMILY_EXPEL;
	stPacket.dwObjectID		= HEROID;
	stPacket.dwData			= nMemberID;

	NETWORK->Send( (MSGROOT*)&stPacket, sizeof(stPacket) );
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
// CLI_RequestBreakUp Method																								   ��ü ��û
//
VOID CSHFamilyManager::CLI_RequestBreakUp()
{
#if !defined(_AGENTSERVER) && !defined(_MAPSERVER_)
	MSG_DWORD stPacket;
	stPacket.Category 		= MP_FAMILY;
	stPacket.Protocol 		= MP_FAMILY_BREAKUP;
	stPacket.dwObjectID		= HEROID;

	NETWORK->Send( (MSGROOT*)&stPacket, sizeof(stPacket) );
#endif
}

// 091204 ONS �йи� ���̾�α׿� ���弼�� ���� ��� �߰� : ���� ��û
VOID CSHFamilyManager::CLI_RequestPayTax( int nKind )
{
#if !defined(_AGENTSERVER) && !defined(_MAPSERVER_)

	g_csFarmManager.CLI_RequestPayTax(2);

#endif
}


// 091126 ONS �йи��� �̾� ��� �߰� : ������ �̾� ��û
VOID CSHFamilyManager::CLI_RequestTransfer( DWORD dwIndex )
{
#if !defined(_AGENTSERVER) && !defined(_MAPSERVER_)
	MSG_DWORD stPacket;
	stPacket.Category 		= MP_FAMILY;
	stPacket.Protocol 		= MP_FAMILY_TRANSFER;
	stPacket.dwObjectID		= HEROID;
	stPacket.dwData			= dwIndex;

	NETWORK->Send( (MSGROOT*)&stPacket, sizeof(stPacket) );
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  CLI_ParseAnswerFromSrv Method																			  �����κ����� ���� �м�/ó��
//
VOID CSHFamilyManager::CLI_ParseAnswerFromSrv(void* pMsg)
{
#if !defined(_MAPSERVER_) && !defined(_AGENTSERVER)														// �ʼ�����, ������Ʈ ������ �ƴ϶��,

	char szTxt[256];																					// �ӽ� ���۸� �����Ѵ�.

	MSGBASE* pTmp = (MSGBASE*)pMsg;																		// ��Ŷ �޽����� �ӽ� ��Ŷ�� �޴´�.

	switch(pTmp->Protocol)																				// ��Ŷ�� ���������� Ȯ���Ѵ�.
	{
	// ���� ó��
	case MP_FAMILY_CREATE:																				// �йи� ���� ��� ���������̸�,
		{
			MSG_DWORD2* pstPacket = (MSG_DWORD2*)pMsg;													// ��Ŷ ������ �޴´�.

			switch(pstPacket->dwData1)																	// ��Ŷ ����Ÿ�� Ȯ���Ѵ�.
			{
			case RESULT_OK:																				// ���� ���� �޽������,
				{
					WINDOWMGR->MsgBox( MBI_FAMILY_CREATE_SUCESS, MBT_OK, CHATMGR->GetChatMsg( 729 ) );	// �йи� ���� ���� �޽����� ����Ѵ�.

					CFamilyCreateDialog* pDlg = GAMEIN->GetFamilyCreateDlg() ;							// �йи� ����â ������ �޾ƿ´�.

					if( pDlg )																			// �йи� ����â ������ ��ȿ���� üũ�Ѵ�.
					{
						pDlg->SetActive( FALSE ) ;														// �йи� ����â�� �ݴ´�.
					}
				}
				break;

			case RESULT_FAIL_01:																		// �̹� �йи��� �����ִٴ� �޽��� ���,
				{
					WINDOWMGR->MsgBox( MBI_FAMILY_ALREADY_HAVE_FAMILY, MBT_OK, CHATMGR->GetChatMsg( 697 ) );// �̹� �йи��� �����ִٴ� �˸�â�� ����Ѵ�.

					CFamilyCreateDialog* pDlg = GAMEIN->GetFamilyCreateDlg() ;							// �йи� ����â ������ �޾ƿ´�.

					if( pDlg )																			// �йи� ����â ������ ��ȿ���� üũ�Ѵ�.
					{
						pDlg->SetActive( FALSE ) ;														// �йи� ����â�� �ݴ´�.
					}
				}
				break;

			case RESULT_FAIL_02:																		// ���� �̸��� �̹� �����ϰų� ����� �� ���� �޽��� ���,
				{				
					WINDOWMGR->MsgBox( MBI_FAMILY_SAME_FAMILYNAME, MBT_OK, CHATMGR->GetChatMsg( 698 ) );	// ���� ó���� �Ѵ�.
				}
				break;
			// Ż��
			case RESULT_FAIL_03:
				{
					CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg( 1145 ), LEAVE_PENALTY_REJOIN_TIME[FLK_LEAVE], pstPacket->dwData2);
				}
				break;
			// �߹�
			case RESULT_FAIL_04:
				{
					CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg( 1146 ), LEAVE_PENALTY_REJOIN_TIME[FLK_EXPEL], pstPacket->dwData2);
				}
				break;
			// ��ü
			case RESULT_FAIL_05:
				{
					CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg( 1147 ), LEAVE_PENALTY_REJOIN_TIME[FLK_BREAKUP], pstPacket->dwData2);
				}
				break;
			}
		}
		break;
	// �ʴ�
	case MP_FAMILY_INVITE:																				// �йи� �ʴ� ���������̸�,
		{
			MSG_DWORD2* pstPacket = (MSG_DWORD2*)pMsg;													// ��Ŷ ������ �޴´�.

			switch(pstPacket->dwData1)																	// �����͸� Ȯ���Ѵ�.
			{
			case RESULT_OK:																				// �ʴ� ���� �޽������,
				{
					char inviteBuf[128] = {0, } ;																// �ʴ� �޽����� ���� �ӽ� ���۸� �����Ѵ�.

					CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetSelectedObject() ;								// ������ �÷��̾� ������ �޴´�.
					sprintf( inviteBuf, CHATMGR->GetChatMsg( 1153 ), pPlayer->GetObjectName() ) ;		// �ʴ� �޽����� �����.
					WINDOWMGR->MsgBox( MBI_FAMILY_INVITE, MBT_OK, inviteBuf );									// �ʴ� �޽����� ���´ٴ� �˸�â�� ����.
				}
				break;
			case RESULT_FAIL_01:																		// �йи��� �������� �ʴٴ� �޽������,
				//MessageBox(NULL, "�йи��� ���� ���� ����!", NULL, NULL);
				//WINDOWMGR->MsgBox( MBI_FAMILY_NO_FAMILY, MBT_OK, CHATMGR->GetChatMsg( 698 ) );		// ���� ó���� �Ѵ�.
				break;
			case RESULT_FAIL_02:																		// �����͸� �ʴ� �����ϴٴ� �޽������,
				WINDOWMGR->MsgBox( MBI_FAMILY_ABLE_MASTER, MBT_OK, CHATMGR->GetChatMsg( 732 ) );		// ���� ó���� �Ѵ�.
				break;
			case RESULT_FAIL_03:																		// ������ �ʰ� �ߴٴ� �޽������,
				WINDOWMGR->MsgBox( MBI_FAMILY_FULL_MEMBER, MBT_OK, CHATMGR->GetChatMsg( 738 ) );		// ���� ó���� �Ѵ�.
				break;
			case RESULT_FAIL_04:																		// �ٸ� �йи��� �����ִ� �÷��̾��� �޽������,
				WINDOWMGR->MsgBox( MBI_FAMILY_OTHER_FAMILY, MBT_OK, CHATMGR->GetChatMsg( 735 ) );		// ���� ó���� �Ѵ�.
				break;
			// �߹�
			case RESULT_FAIL_05:
 				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg( 1148 ), LEAVE_PENALTY_REJOIN_TIME[FLK_EXPEL], pstPacket->dwData2);
				break;
			// Ż��
			case RESULT_FAIL_06:
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg( 1149 ), LEAVE_PENALTY_REJOIN_TIME[FLK_LEAVE], pstPacket->dwData2) ;
				break;
			}
		}
		break;
	// �ʴ��
	case MP_FAMILY_INVITED:																				// �ʴ븦 �޴� ���������̸�,
		{
			MSG_DWORD_NAME* pstPacket = (MSG_DWORD_NAME*)pMsg;											// ��Ŷ�� �޴´�.

			CPlayer* pMaster = (CPlayer*)OBJECTMGR->GetObject( pstPacket->dwData );						// ������ ������ �޴´�.

			if (pMaster == NULL)																		// ������ ������ ��ȿ���� �ʴٸ�,
			{
				//WINDOWMGR->MsgBox( MBI_FAMILY_NO_MASTER_POSITION, MBT_OK, "������ �Ǵ� �������� ��ġ�� �Һи�!!" );	// ���� ó���� �Ѵ�.
				return;																					// ���� ó���� �Ѵ�.
			}

			sprintf(szTxt, CHATMGR->GetChatMsg( 1154 ),													// �ʴ� �޴� �޽����� �����Ѵ�.
					pstPacket->Name, pMaster->GetObjectName(), HERO->GetObjectName());
			WINDOWMGR->MsgBox( MBI_FAMILY_INVITE_YOU, MBT_YESNO, szTxt );								// �˸� â�� ����.

			m_nInviteMasterID = pstPacket->dwData;														// �ʴ븦 ��û�� �������� ���̵� �޴´�.
		}
		break;
	// �ʴ� ���
	case MP_FAMILY_INVITE_RESULT:																		// �ʴ� ����� �޴� ���������̸�,
	case MP_FAMILY_INVITE_RESULT_TO_OTHER_AGENT_2:
		{
			MSG_NAME_DWORD2* pstPacket = (MSG_NAME_DWORD2*)pMsg;										// ��Ŷ�� �޴´�.

 			CPlayer* pTargetPlayer = (CPlayer*)OBJECTMGR->GetObject( pstPacket->dwData1 );				// �ʴ븦 �� Ÿ���� ������ �޴´�.

  			if (pTargetPlayer == NULL)																	// Ÿ�� ������ ��ȿ���� üũ�� �Ѵ�.
			{
				return;																					// ���� ó���� �Ѵ�.
			}

			switch(pstPacket->dwData2)																	// ��Ŷ�� �����͸� Ȯ���Ѵ�.
			{
			case FIR_ACCEPT_MASTER:																		// �ʴ븦 �������� �� �йи� �����Ϳ��� ���� �޽������,
				{
					cDialog* const dialog = WINDOWMGR->GetWindowForID(
						MBI_FAMILY_INVITE);

					if(dialog)
					{
						dialog->SetActive(
							FALSE);
					}

					sprintf(szTxt, CHATMGR->GetChatMsg( 700 ), pTargetPlayer->GetObjectName());			// ��밡 �йи� ����� �Ǿ��ٴ� �޽����� �����Ѵ�.
					WINDOWMGR->MsgBox( MBI_FAMILY_ACCEPT_MASTER, MBT_OK, szTxt );						// �˸� �޽����� ����.
					CHATMGR->AddMsg(CTC_SYSMSG, szTxt) ;												// ä��â�� �޽����� ����Ѵ�.

					CFamilyDialog* pDlg = GAMEIN->GetFamilyDlg() ;										// �йи� ���̾�α� ������ �޾ƿ´�.

					if( pDlg )																			// �йи� ����â ������ ��ȿ���� üũ�Ѵ�.
					{
						pDlg->UpdateAllInfo() ;															// �йи� ������ ������Ʈ �Ѵ�.
						pDlg->SetActive( TRUE ) ;														// �йи� ����â�� �ݴ´�.
					}
				}
				break;
			case FIR_DENY_MASTER:																		// �ʴ븦 �������� �� �����Ϳ��� ���� �޽������,
				{
					cDialog* const dialog = WINDOWMGR->GetWindowForID(
						MBI_FAMILY_INVITE);

					if(dialog)
					{
						dialog->SetActive(
							FALSE);
					}

					sprintf(szTxt, CHATMGR->GetChatMsg( 1155 ),											// �ʴ븦 �ź��ߴٴ� �޽����� �����Ѵ�.
							pTargetPlayer->GetObjectName(), HERO->GetObjectName());
					WINDOWMGR->MsgBox( MBI_FAMILY_DENY_MASTER, MBT_OK, szTxt );							// �˸� �޽����� ����.
					CHATMGR->AddMsg(CTC_SYSMSG, szTxt) ;												// ä��â�� �޽����� ����Ѵ�.
				}
				break;
			case FIR_ACCEPT_PLAYER:																		// �ʴ븦 �������� �� ������ ������� ���� �޽������,
				{
					sprintf(szTxt,CHATMGR->GetChatMsg( 1156 ), pstPacket->Name);						// �йи� ����� �Ǿ��ٴ� �޽����� �����Ѵ�.
					WINDOWMGR->MsgBox( MBI_FAMILY_ACCEPT_PLAYER, MBT_OK, szTxt );						// �˸� �޽����� ����.
					CHATMGR->AddMsg(CTC_SYSMSG, szTxt) ;												// ä��â�� �޽����� ����Ѵ�.

					CFamilyDialog* pDlg = GAMEIN->GetFamilyDlg() ;										// �йи� ���̾�α� ������ �޾ƿ´�.

					if( pDlg )																			// �йи� ����â ������ ��ȿ���� üũ�Ѵ�.
					{
						pDlg->UpdateAllInfo() ;															// �йи� ������ ������Ʈ �Ѵ�.
						pDlg->SetActive( TRUE ) ;														// �йи� ����â�� �ݴ´�.
					}
				}
				break;
			case FIR_DENY_PLAYER:																		// �ʴ븦 �������� �� ������ ������� ���� �޽������,
				{
					sprintf(szTxt,CHATMGR->GetChatMsg( 1157 ),pTargetPlayer->GetObjectName());			// �ʴ븦 �����ߴٴ� �޽����� �����Ѵ�.
					WINDOWMGR->MsgBox( MBI_FAMILY_DENY_PLAYER, MBT_OK, szTxt );							// �˸� �޽����� ����.
					CHATMGR->AddMsg(CTC_SYSMSG, szTxt) ;												// ä��â�� �޽����� ����Ѵ�.
				}
				break;
			}
		}
		break;
	// �йи� ������ �ޱ�
	case MP_FAMILY_DATA_FAMILY:																			// �йи� ������ �޴� ���������̸�,
		{
			if (HERO == NULL)
			{
				return;
			}

			PACKET_FAMILY* pstPacket = (PACKET_FAMILY*)pMsg;											// ��Ŷ�� �޴´�.

			CSHFamily csFamily;																			// �йи� ������ ���� Ŭ������ �����Ѵ�.
			csFamily.Set(&pstPacket->stInfo, &pstPacket->stInfoEx);										// �йи��� �⺻, �߰� ������ �����Ѵ�.

			HERO->SetFamily(&csFamily);																	// ���� �� �йи��� ���� �йи��� �����Ѵ�.
			// 100219 ONS �÷��̾��� �йи� �ε����� �����Ѵ�
			HERO->SetFamilyIdx(pstPacket->stInfo.nID);
		}
		break;
	// �йи� ��� ������ �ޱ�
	case MP_FAMILY_DATA_MEMBER:																			// �йи� ��� ������ �޴� ���������̸�,
		{
			if (HERO == NULL)
			{
				return;
			}

			PACKET_MEMBER_INFO* pstMemberPacket = (PACKET_MEMBER_INFO*)pMsg;							// ��Ŷ�� �޴´�.

			CSHFamilyMember csMember;																	// �йи� ��� ������ ���� Ŭ������ �����Ѵ�.
			csMember.Set(&pstMemberPacket->stInfo, &pstMemberPacket->stInfoEx);							// �йи� ����� �⺻, �߰� ������ �����Ѵ�.

			HERO->GetFamily()->SetMember(&csMember, pstMemberPacket->nIndex);							// ���� �� ����� ���� �йи� ����� �����Ѵ�.

			// �ڱ� ȣĪ ����
			// ���� MP_FAMILY_SHOW_NICKNAME �� �޾ƾ� �ϴµ� �׸������ �ٸ� �÷��̾�� �޴µ� �ڽ��� �� �޴´�.
			// Ŭ���̾�Ʈ�� ���ӵǱ� ���� �������� ������ ��.. ��Ƽ������ ����?
			if (csMember.Get()->nID == HERO->GetID())
			{
				HERO->SetFamilyNickName(csMember.Get()->szNickname);
				HERO->GetObjectBalloon()->SetFamilyNickName(csMember.Get()->szNickname);
			}

			CFamilyDialog* pDlg = GAMEIN->GetFamilyDlg() ;												// �йи� ���̾�α� ������ �޴´�.

			if( pDlg )																					// ���̾�α� ������ ��ȿ���� üũ�Ѵ�.
			{
				pDlg->UpdateAllInfo() ;																	// �йи� â�� ������Ʈ �Ѵ�.
			}
		}
		break;
	// ȣĪ ����
	case MP_FAMILY_CHANGE_NICKNAME_NACK:
		{
			MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
			if(HERO->GetID() == pmsg->dwObjectID)
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1813));
			}
		}
		break;
	case MP_FAMILY_CHANGE_NICKNAME:																		// ȣĪ ���� �������� �̶��,
		{
			if( HERO )
			{
				MSG_DWORD_NAME* pstPacket = (MSG_DWORD_NAME*)pMsg;											// ��Ŷ�� �޴´�.

				switch(pstPacket->dwData)																	// ��Ŷ �����͸� Ȯ���Ѵ�.
				{
				case RESULT_FAIL_01:																		// ����� �� ���ٴ� �޽��� ���,
					{
						LPCTSTR textMessage = CHATMGR->GetChatMsg(551);

						WINDOWMGR->MsgBox(
							MBI_FAMILY_CHANGENAME_FAIL,
							MBT_OK,
							textMessage);
						CHATMGR->AddMsg(
							CTC_SYSMSG,
							textMessage);
					}
					break;
				case RESULT_OK_02:																			// ���� ���� �޽������,
					{								
						LPCTSTR textMessage = CHATMGR->GetChatMsg(714);
						WINDOWMGR->MsgBox(
							MBI_FAMILY_CHANGENAME_COMPLETE2,
							MBT_OK,
							textMessage);
						CHATMGR->AddMsg(
							CTC_SYSMSG,
							textMessage);
					}
					break;
				}
			}
		}
		break;
	// �� ����Ʈ
	case MP_FAMILY_HONOR_POINT:																			// �йи� �� ����Ʈ�� �޴� �޽������,
		{
			if( HERO )																					// ������� ���� ���� Ȯ��.
			{
				CSHFamily* pFamily = HERO->GetFamily() ;												// ����ΰ� �йи��� �ִ��� Ȯ���Ѵ�.

				if( pFamily )																			// �йи��� �ִٸ�,
				{
					MSG_DWORD* pstPacket = (MSG_DWORD*)pMsg;											// ��Ŷ ������ �޴´�.

					CSHFamily::stINFO_EX stInfoEx = *pFamily->GetEx();									// ������� �йи� �߰� ������ �����Ѵ�.

					// ���� �����̳� ���׷��̵��� ���� ���� ����Ʈ���� - int �� �ѱ��. 
					// �׷��� int �� ĳ�����ؼ� - �� ���� üũ�ؼ�
					// �׷� ��쿡�� -= �ϵ��� �Ѵ�. ���� ����Ʈ���� int ������� ����...
					if ((int)pstPacket->dwData < 0)
					{
						stInfoEx.nHonorPoint -= (DWORD)(-1*(int)pstPacket->dwData);
					}
					else
					{
						stInfoEx.nHonorPoint += pstPacket->dwData;										// �߰� ������ �� ����Ʈ��, ��Ŷ���� �Ѿ�� ����Ʈ�� �����Ѵ�.
					}

					pFamily->SetEx(&stInfoEx);															// ������� �йи��� �� ����Ʈ�� �����Ѵ�.

					CFamilyDialog* pDlg = GAMEIN->GetFamilyDlg() ;										// �йи� ���̾�α� ������ �޴´�.

					if( pDlg )																			// ���̾�α� ������ ��ȿ���� üũ�Ѵ�.
					{
						pDlg->SetHonorPoint( pstPacket->dwData ) ;										// �� ����Ʈ�� �����Ѵ�.
					}
				}
			}
		}
		break;
	// ȣĪ ǥ��
	case MP_FAMILY_SHOW_NICKNAME:
		{
			if (HERO == NULL)
			{
				return;
			}

			MSG_NAME* pstPacket = (MSG_NAME*)pMsg;
	
			if (HERO->GetID() == pstPacket->dwObjectID)
			{
				HERO->SetFamilyNickName(pstPacket->Name);
				HERO->GetObjectBalloon()->SetFamilyNickName(pstPacket->Name);
			}
			else
			{
				CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject(pstPacket->dwObjectID);
				if( !pPlayer )	return;

				pPlayer->SetFamilyNickName(pstPacket->Name);
				pPlayer->GetObjectBalloon()->SetFamilyNickName(pstPacket->Name);
			}
		}
		break;
	// ���� ���/����
	case MP_FAMILY_REGIST_EMBLEM:
		{
 			if (HERO == NULL)
			{
				return;
			}

   			PACKET_FAMILY_EMBLEM* pstPacket = (PACKET_FAMILY_EMBLEM*)pMsg;

			CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject(pstPacket->nTargetPlayerID);
			if( !pPlayer )	return;


			// tga ���̺귯������ �� �� �ε��� ������ �ε�� ��� ������ �������� �ʴ� �̻� ���� �̸����� tga�� �����ϴٰ� �ص�
			// tga �ε� �� �� ���� ������ tga ������ �����ϱ� ������ ������ ����Ǿ��� ��� �� ������ �����ϴ� ��� �ڿ��� �����ϰ�
			// ���� �̸����� �����ϴ���, �ƴϸ� ���ο� �����̸����� �����ؼ� �װ��� �ε��ϴ��� �ϴ� ������ �ؾ� �Ѵ�. 
			// ������ ���� ���������� ������ ���� ������ �ʿ���� ������ ����� ����Ѵ�. 
			// �׷��� ���/����� ������ ���Ͽ� ���/���� ȸ���� �йи�ID�� �����ؼ� ������ ���/����� ��
			// ���ο� �����̸����� tga�� ���� �ε��ϵ��� �Ѵ�. 
			char szEmblemInfo[256];
			char szEmblemImg[256];
			DWORD nOldChangedFreq = 0;
			sprintf(szEmblemInfo, "Data/Interface/2DImage/FamilyEmblem/%d_%d.txt", g_nServerSetNum, pstPacket->nFamilyID);

			FILE* fp = fopen(szEmblemInfo, "rt");
			if (fp)
			{
				fscanf(fp, "%d", &nOldChangedFreq);
				fclose(fp);
			}

			// �����Ͱ� �ƴ� �Ϲݸ�� ������ ���� ���..
			if (nOldChangedFreq > pstPacket->nChangedFreq) pstPacket->nChangedFreq = nOldChangedFreq;

			// ���ο� ���� ����
			sprintf( szEmblemImg, "Data/Interface/2DImage/FamilyEmblem/%d_%d_%d.tga", g_nServerSetNum, pstPacket->nFamilyID, pstPacket->nChangedFreq);
			// ���� ������ ���� ���� ������ �����ϰ� ������ �ִ� �� �ε�. �ٸ� �йи� ����� ���ؼ� ���� ������ �Ծ��� ��쿡 �ش�ȴ�.
			if (access(szEmblemImg, 0) == -1)
			{
				char tempBuf[EMBLEM_BPP*EMBLEM_WIDTH*EMBLEM_WIDTH];
				memset( tempBuf, 0, EMBLEM_BPP*EMBLEM_WIDTH*EMBLEM_WIDTH );
				memcpy( tempBuf, pstPacket->cImgData, EMBLEM_BPP*EMBLEM_WIDTH*EMBLEM_WIDTH );
				WriteTGA( szEmblemImg, tempBuf, EMBLEM_WIDTH, EMBLEM_WIDTH, EMBLEM_BPP*EMBLEM_WIDTH, EMBLEM_BPP*8 );
			}
			// ���� ȸ�� ����
			fp = fopen(szEmblemInfo, "wt");
			fprintf(fp, "%d", pstPacket->nChangedFreq);
			fclose(fp);

			// ����� ������ �޸𸮿� �ε�
			cImageSelf* pEmblem = pPlayer->GetFamilyEmblem();
			if (pEmblem)
			{
				pEmblem->Release();
				delete pEmblem;
			}

			pEmblem = new cImageSelf;
			pEmblem->LoadSprite(szEmblemImg, EMBLEM_WIDTH, EMBLEM_HEIGHT);
			pPlayer->SetFamilyEmblem(pEmblem);

			// 091111 ONS �йи� ���� ��Ͻ� ä��â�޼����� �˸��޼����� �����ش�.
			CSHFamily* pFamily = HERO->GetFamily() ;
			if( pFamily && 
				( pPlayer->GetID() == HEROID ) &&
				( pPlayer->GetFamilyIdx() == HERO->GetFamilyIdx()) && 
				( !pstPacket->bIsLoad ) )
			{
				WINDOWMGR->MsgBox( MBI_NOTICE, MBT_OK, CHATMGR->GetChatMsg( 713 ) );						// �˸�â�� ����.
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg( 713 )) ;							// ä��â�� �޽����� ����Ѵ�.

				CFamilyDialog* pDlg = GAMEIN->GetFamilyDlg() ;										// �йи� ���̾�α� ������ �޾ƿ´�.
				if( pDlg )																			// �йи� ����â ������ ��ȿ���� üũ�Ѵ�.
				{
					pDlg->UpdateAllInfo() ;															// �йи� ������ ������Ʈ �Ѵ�.
					pDlg->SetActive( TRUE ) ;														// �йи� ����â�� �ݴ´�.
				}
			}
		}
		break;
	// ���� ǥ��
	case MP_FAMILY_SHOW_EMBLEM:
		{
 			if (HERO == NULL)
			{
				return;
			}

   			MSG_DWORD3* pstPacket = (MSG_DWORD3*)pMsg;

			CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject(pstPacket->dwData2);
			if( !pPlayer )	return;

			CLI_ShowEmblem(pPlayer, pstPacket->dwData1, pstPacket->dwData3);
		}
		break;
	// 091111 ONS ���� ����(����)
	case MP_FAMILY_DEL_EMBLEM:
		{
 			if (HERO == NULL) return;

			MSG_DWORD* pstPacket = (MSG_DWORD*)pMsg;

			CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject( pstPacket->dwData );
			if( !pPlayer ) return;

			cImageSelf* pEmblem = pPlayer->GetFamilyEmblem();
			if (pEmblem)
			{
				pEmblem->Release();
				delete pEmblem;
				pPlayer->SetFamilyEmblem(NULL);
			}

			CSHFamily* pFamily = HERO->GetFamily() ;
			if( pFamily && 
				( pPlayer->GetID() == HEROID ) &&
				( pPlayer->GetFamilyIdx() == HERO->GetFamilyIdx()) )					
			{
				WINDOWMGR->MsgBox( MBI_NOTICE, MBT_OK, CHATMGR->GetChatMsg( 1974 ));
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg( 1974 )) ;	

				CFamilyDialog* pDlg = GAMEIN->GetFamilyDlg() ;				
				if( pDlg )													
				{
					pDlg->UpdateAllInfo() ;									
					pDlg->SetActive( TRUE ) ;								
				}
			}
		}
		break;
	// Ż��
	case MP_FAMILY_LEAVE:
		{
 			if (HERO == NULL) return;

			CSHFamily* pFamily = HERO->GetFamily() ;
			if( pFamily )							
			{
   				MSG_DWORD* pstPacket = (MSG_DWORD*)pMsg;
	
				char szName[256];
				for(UINT i=0; i<HERO->GetFamily()->Get()->nMemberNum; i++)
				{
					if (HERO->GetFamily()->GetMember(i)->Get()->nID == pstPacket->dwData)
					{
						SafeStrCpy(szName, HERO->GetFamily()->GetMember(i)->Get()->szName, MAX_NAME_LENGTH+1);
						break;
					}
				}

				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg( 1150 ), szName);

				// ��� ����
				pFamily->DelMember(pstPacket->dwData);
				CFamilyDialog* pDlg = GAMEIN->GetFamilyDlg() ;	
				if( pDlg ) pDlg->UpdateAllInfo() ;						

				// Ż���� ����ڶ�� �йи� ������ �����.
				if (HERO->GetID() == pstPacket->dwData)
				{
					CSHFamily csFamily;
					pFamily->Set(&csFamily);
					if( pDlg )										
					{
						pDlg->SetActive(FALSE);
					}

					// ���� ������ Ÿ�� �ʱ�ȭ
					CSHMonstermeterDlg* pDlg = GAMEIN->GetMonstermeterDlg() ;
					if (pDlg == NULL) return;
					pDlg->GetFarmRePlantTimeBar()->CLI_End();
					pDlg->GetFarmReManureTimeBar()->CLI_End();

					// ������� â �ݱ�
					CSHFarmManageDlg* pFarmDlg = GAMEIN->GetFarmManageDlg();
					if (pFarmDlg)
					{
						pFarmDlg->SetActive(FALSE);
						// �̴ϸ��� ���� ������ ����
						CMiniMapDlg* pMiniMapDlg = GAMEIN->GetMiniMapDialog();
						if (pMiniMapDlg)
						{
							pMiniMapDlg->RemoveIcon(pFarmDlg->GetFarm()->GetID()+FARM_RENDER_OBJ_ID_START);
						}
						CBigMapDlg* pBigMapDlg = GAMEIN->GetBigMapDialog();
						if (pBigMapDlg)
						{
							pBigMapDlg->RemoveIcon(pFarmDlg->GetFarm()->GetID()+FARM_RENDER_OBJ_ID_START);
						}
					}
				}
			}
		}
		break;
	// �߹�
	case MP_FAMILY_EXPEL:
		{
 			if (HERO == NULL) return;

			CSHFamily* pFamily = HERO->GetFamily() ;
			if( pFamily )							
			{
   				MSG_DWORD2* pstPacket = (MSG_DWORD2*)pMsg;
	
				switch(pstPacket->dwData2)
				{
				case RESULT_OK:
					{
						char szName[256];
						for(UINT i=0; i<HERO->GetFamily()->Get()->nMemberNum; i++)
						{
							if (HERO->GetFamily()->GetMember(i)->Get()->nID == pstPacket->dwData1)
							{
								SafeStrCpy(szName, HERO->GetFamily()->GetMember(i)->Get()->szName, MAX_NAME_LENGTH+1);
								break;
							}
						}

						CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg( 1151 ), szName) ;

						// ��� ����
						pFamily->DelMember(pstPacket->dwData1);
						CFamilyDialog* pDlg = GAMEIN->GetFamilyDlg() ;	
						if( pDlg ) pDlg->UpdateAllInfo() ;						

						// �߹�� ����ڶ�� �йи� ������ �����.
						if (HERO->GetID() == pstPacket->dwData1)
						{
							CSHFamily csFamily;
							pFamily->Set(&csFamily);
							if( pDlg ) pDlg->SetActive(FALSE);

							// ���� ������ Ÿ�� �ʱ�ȭ
							CSHMonstermeterDlg* pDlg = GAMEIN->GetMonstermeterDlg() ;
							if (pDlg == NULL) return;
							pDlg->GetFarmRePlantTimeBar()->CLI_End();
							pDlg->GetFarmReManureTimeBar()->CLI_End();

							// ������� â �ݱ�
							CSHFarmManageDlg* pFarmDlg = GAMEIN->GetFarmManageDlg();
							if (pFarmDlg)
							{
								pFarmDlg->SetActive(FALSE);
								// �̴ϸ��� ���� ������ ����
								CMiniMapDlg* pMiniMapDlg = GAMEIN->GetMiniMapDialog();
								if (pMiniMapDlg)
								{
									pMiniMapDlg->RemoveIcon(pFarmDlg->GetFarm()->GetID()+FARM_RENDER_OBJ_ID_START);
								}
								CBigMapDlg* pBigMapDlg = GAMEIN->GetBigMapDialog();
								if (pBigMapDlg)
								{
									pBigMapDlg->RemoveIcon(pFarmDlg->GetFarm()->GetID()+FARM_RENDER_OBJ_ID_START);
								}
							}
						}
					}
					break;
				case RESULT_FAIL_01:
					CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg( 1163 ));
					break;
				}
			}
		}
		break;
	// ��ü
	case MP_FAMILY_BREAKUP:
		{
 			if (HERO == NULL) return;

			CSHFamily* pFamily = HERO->GetFamily() ;
			if( pFamily )							
			{
   				MSG_DWORD2* pstPacket = (MSG_DWORD2*)pMsg;
	
				switch(pstPacket->dwData2)
				{
				case RESULT_OK:
					{
						CSHFamily csFamily;
						pFamily->Set(&csFamily);

						CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg( 1152 )) ;

						CFamilyDialog* pFamilyDlg = GAMEIN->GetFamilyDlg() ;	
						if( pFamilyDlg ) pFamilyDlg->SetActive(FALSE);

						// ���� ������ Ÿ�� �ʱ�ȭ
						CSHMonstermeterDlg* pDlg = GAMEIN->GetMonstermeterDlg() ;
						if (pDlg == NULL) return;
						pDlg->GetFarmRePlantTimeBar()->CLI_End();
						pDlg->GetFarmReManureTimeBar()->CLI_End();

						// ������� â �ݱ�
						CSHFarmManageDlg* pFarmDlg = GAMEIN->GetFarmManageDlg();
						if (pFarmDlg)
						{
							pFarmDlg->SetActive(FALSE);
							// �̴ϸ��� ���� ������ ����
							CMiniMapDlg* pMiniMapDlg = GAMEIN->GetMiniMapDialog();
							if (pMiniMapDlg)
							{
								pMiniMapDlg->RemoveIcon(pFarmDlg->GetFarm()->GetID()+FARM_RENDER_OBJ_ID_START);
							}
							CBigMapDlg* pBigMapDlg = GAMEIN->GetBigMapDialog();
							if (pBigMapDlg)
							{
								pBigMapDlg->RemoveIcon(pFarmDlg->GetFarm()->GetID()+FARM_RENDER_OBJ_ID_START);
							}
						}
					}
					break;
				case RESULT_FAIL_01:
				case RESULT_FAIL_02:
					CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg( 1162 )) ;
					break;
				case RESULT_FAIL_03:
					CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg( 1161 )) ;
					break;
				}
			}
		}
		break;
	// Ż�� ���� ����
	case MP_FAMILY_LEAVE_EMBLEM:
		{
 			if (HERO == NULL) return;

			MSG_DWORD* pstPacket = (MSG_DWORD*)pMsg;

			CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject( pstPacket->dwData );
			if( !pPlayer ) return;

			cImageSelf* pEmblem = pPlayer->GetFamilyEmblem();
			if (pEmblem)
			{
				pEmblem->Release();
				delete pEmblem;
				pPlayer->SetFamilyEmblem(NULL);
			}

			char szName[MAX_NAME_LENGTH+1] = "";
			pPlayer->SetFamilyNickName(szName);
			pPlayer->GetObjectBalloon()->SetFamilyNickName(szName);
		}
		break;
	// Ż��,�߹�,��ü ���Ƽ �޼���
	case MP_FAMILY_PENALTY_MSG:
		{
 			if (HERO == NULL) return;

			MSG_DWORD3* pstPacket = (MSG_DWORD3*)pMsg;

			CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject( pstPacket->dwObjectID );
			if( !pPlayer ) return;

			char szText[256];

			// ����ġ ����
   			switch(pstPacket->dwData1)
			{
			case 0:	strcpy(szText, CHATMGR->GetChatMsg( 1165 )); break;
			case 1: strcpy(szText, CHATMGR->GetChatMsg( 1166 )); break;
			case 2: strcpy(szText, CHATMGR->GetChatMsg( 1167 )); break;
			}
			// ���� ����
			if (pstPacket->dwData3)
			{
				strcat(szText, " ");
				strcat(szText, CHATMGR->GetChatMsg( 657 ));
			}

			if (pstPacket->dwData2)
			{
				CHATMGR->AddMsg(CTC_SYSMSG, szText, pstPacket->dwData2);
			}
		}
		break;
	// 091126 ONS �йи��� �̾� ����߰� : �йи��� �������� ����Ѵ�.
	case MP_FAMILY_TRANSFER:
		{
 			if (HERO == NULL) return;

			CSHFamily* pFamily = HERO->GetFamily() ;
			if( pFamily )							
			{
   				MSG_DWORD* pstPacket = (MSG_DWORD*)pMsg;

				char szName[256]={ 0, };
				for(UINT i=0; i<HERO->GetFamily()->Get()->nMemberNum; i++)
				{
					if (HERO->GetFamily()->GetMember(i)->Get()->nID == pstPacket->dwData)
					{
						SafeStrCpy(szName, HERO->GetFamily()->GetMember(i)->Get()->szName, MAX_NAME_LENGTH+1);
						break;
					}
				}
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg( 1991 ), szName);

				// �йи����� ����ʿ� ���� UI�� ������Ʈ�Ѵ�.
				CSHGroup::stINFO* stInfo = pFamily->Get();
				stInfo->nMasterID = pstPacket->dwData;
				SafeStrCpy(stInfo->szMasterName, szName, MAX_NAME_LENGTH+1);
				pFamily->Set(stInfo);

                CFamilyDialog* pDlg = GAMEIN->GetFamilyDlg() ;	
				if( pDlg )
				{
					pDlg->UpdateAllInfo() ;						
				}
			}
		}
		break;

	}
#endif
}