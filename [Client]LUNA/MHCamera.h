// MHCamera.h: interface for the CMHCamera class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MHCAMERA_H__3EE48679_085F_4E12_AF97_C06D8AB29A00__INCLUDED_)
#define AFX_MHCAMERA_H__3EE48679_085F_4E12_AF97_C06D8AB29A00__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Angle.h"
#include ".\Engine\EngineCamera.h"
#include ".\Engine\EngineObject.h"

class CObject;
class CEngineObject;


struct MHCAMERASHAKE;


#define MAX_CAMERA_DESC	8
#define CAMERA_FILTER_DISTANCE	200
#define CAMERA_FILTER_FADE_INOUT_DEFAULT_SPEED	0.001f
#define CAMERA	USINGTON(CMHCamera)

enum eCAMERAMODE
{
	eCM_Free = 0,
	eCM_QuarterView,
	eCM_FollowView,
	eCM_LOGIN,
	eCM_EyeView,
};

enum eCAMERASTATE
{
	eCS_Normal,
	eCS_Sit,
	eCS_Die,
};

enum
{
	eCP_CHARMAKE = 0,
	eCP_CHARSEL,
	eCP_INTERPOLATE,

	eCP_MAX,
};

enum eCAMERA_FILTER_ACTION
{
	eCAMERA_FILTER_ACTION_NONE,
	eCAMERA_FILTER_ACTION_MOVE,
};

struct MHCAMERAPOS
{
	float m_AngleX;
	float m_AngleY;
	float m_Dist;

	VECTOR3 m_Pivot;
};


struct MHCAMERADESC
{
	CAngle m_AngleX;
	CAngle m_AngleY;

	float m_fDistance;

	VECTOR3 m_CameraPos;
	VECTOR3 m_CameraDir;
	
	VECTOR3 m_CameraDownVector;
	VECTOR3 m_CameraDownPositon;

	BOOL m_bCameraAngleMovingX;
	DWORD m_CameraAngleMoveStartX;
	DWORD m_CameraAngleMoveDurationX;
	
	BOOL m_bCameraAngleMovingY;
	DWORD m_CameraAngleMoveStartY;
	DWORD m_CameraAngleMoveDurationY;

	BOOL m_bCameraDistanceMoving;
	DWORD m_CameraDistanceMoveStart;
	DWORD m_CameraDistanceMoveDuration;
	float m_fDistanceStart;
	float m_fDistanceChange;

	float m_CharHeight;			//����߿��� ī�޶� ������ ����������.
	float m_StartCharHeight;
	float m_TargetCharHeight;
	BOOL  m_bHeightChanging;
	DWORD m_dwHeightStartTime;
	DWORD m_dwHeightChangeDuration;

	CObject* m_pPivotObject;	// ���߿� �ϳ��� �� �־�� �Ѵ�.
	VECTOR3 m_PivotPos;

	BOOL m_bPivotObject;		// TRUE �� PivotObject FALSE �� PivotPos
};

struct stCameraFilterInfo
{
	DWORD	dwFilterIdx;
	char	szModelFileName[MAX_PATH];
	DWORD	dwDefaultWidth;
	DWORD	dwDefaultHeight;
	BOOL	bShowInterface;
	int		nMotionIdxToAction;
	std::vector<int> motionIdxVector;

	stCameraFilterInfo()
	{
		dwFilterIdx = 0;
		ZeroMemory(szModelFileName, MAX_PATH);
		dwDefaultWidth = 800;
		dwDefaultHeight = 600;
		nMotionIdxToAction = 0;
	}
};

// 100125 ShinJS --- CameraFilter Action�� ���� CObjectBase
class CCameraFilterObject : public CObjectBase
{
	DWORD m_dwObjectIndex;
	DWORD m_dwFilterIndex;
	CEngineObject m_FilterEngineObject;

	BOOL m_bCameraFilterNoRepeat;						// Camera Filter Index Repeat ����
	BOOL m_bCameraFilterFaded;
	BOOL m_bCameraFilterFadeIn;
	BOOL m_bCameraFilterFadeOut;
	float m_fCameraFilteFadeAlpha;						// Camera Filter Index Fade Alpha ��
	float m_fCameraFilteFadeAlphaValue;					// Camera Filter Index Fade Alpha ó���� ���� ����
	DWORD m_dwRemainTime;								// ���� �ð�

	std::deque<int> m_FilterMotionQue;					// Camera Filter Animaion Motion Index�� ���� ������� ����
	eCAMERA_FILTER_ACTION m_CameraFilterAction;			// Filter Motion ����� ������ �׼�
	VECTOR3 m_FilterActionMovePos;						// eCAMERA_FILTER_ACTION_MOVE ����� �ʿ� ����

public:
	CCameraFilterObject();
	~CCameraFilterObject();
	BOOL Init( DWORD dwObjectIndex, const stCameraFilterInfo& filterInfo, BOOL bNoRepeat, BOOL bFadeIn, float fFadeTime, DWORD dwRemainTime, eCAMERA_FILTER_ACTION eAction );
	void Release();
	BOOL IsInited();
	const DWORD GetObjectIndex() const { return m_dwObjectIndex; }

	void Process();
	void DoCameraFilterAction();						// Camera Filter Action ����

	void Hide() { m_FilterEngineObject.Hide(); }
	void SetFadeOut( float fFadeTime );
	void SetFilterActionMovePos(const VECTOR3& vecPos) { m_FilterActionMovePos = vecPos; }
};

class CMHCamera  
{
	VECTOR3 m_DownVector;
	DWORD m_CurCameraMode;
	CEngineCamera m_EngineCamera;

	MHCAMERADESC m_CameraDesc[MAX_CAMERA_DESC];
	int m_CurCamera;

	float m_fAddHeight;

	// 100607 ONS ������ ���̵��� ī�޶���ġ �ʱ�ȭ�� �������� ����
	BOOL  m_bIsFirstTime;

//---sight distance option
	float m_fDistanceRate;	//50~100% 
	float m_fMaxSightDistance;
	float m_fFov;


	//////////////////////////////////////////////////////////////////////////
	// ���� ��� ������ ���� ������
	VECTOR3 m_VibratePos;
	VECTOR3 m_VibrateAngle;
	VECTOR3 m_PushedVector;
	DWORD m_VibrateDuration;
	DWORD m_VibrateStartTime;
	BOOL m_bIsVibrating;


	void CalcVibration();
	//////////////////////////////////////////////////////////////////////////	

	
	//////////////////////////////////////////////////////////////////////////
	// ����ũ ����� ���� ������
	MHCAMERASHAKE** m_ppCameraShakeDesc;		// ����ŷ �� ���� Desc ��..
	VECTOR3 m_ShakingPos;
	BOOL m_bIsShaking;
	DWORD m_nCurShakingNum;						// ���� �ϰ� �ִ� ����ŷ Desc Index (-1�̸� �ϰ� ���� �ʴ�)
	DWORD m_CurShakingPos;						// ���� �������� ����ŷ Desc�������� ��ġ
	DWORD m_LastShakingPosChangedTime;			// ���������� m_CurShakingPos�� �ٲ� �ð�.
	float m_Amplitude;
	WORD  m_ShakingTimes;						// ����ŷ Ƚ��

	void CalcShaking();
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	// �̵������� ���� ������
	MHCAMERAPOS m_MovePos[eCP_MAX];
	int m_CurPosition;
	int m_GotoPosition;
	
	float m_MoveTime;
	float m_MoveStartTime;
	float m_MoveSpeed;
	
	BOOL m_bMoved;
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// ī�޶������̵��� ���� ������
	DWORD m_LastMoveNoticeTime;			// ĳ���Ͱ� �̵��ߴٰ� ������ �˷��� �ð�
	// 090422 ShinJS --- FollowView�� �ѹ��� �����ϴ� ��� (�����̵��� �Ϸ�Ǹ� Follow ������ eCM_Free�� ��ȯ)
	BOOL m_bExecuteOnceFollowView;
	
	void CalcCameraPosAngle();
	void CalcFreeCamera();
	void ProcessCameraMode();
	void ProcessCameraMoving();

	void RestrictCameraAngleDistance();
	
	void FollowCamera();

	//---KES Map GXObject Alpha 071020
	cPtrList m_AlphaHandleList;
	//-----------------------------

public:
	CMHCamera();
//	//MAKESINGLETON(CMHCamera);
	~CMHCamera();

	void Release();

	//---KES Map GXObject Alpha 071020
	void GXOAlphaProcess();
	//-----------------------------

	void Init(CObject* pObject,float angleX,float angleY,float fDistance);
	void InitCamera(int CameraNum,float angleX,float angleY,float fDistance,CObject* pObject);
	void InitCamera(int CameraNum,float angleX,float angleY,float fDistance,VECTOR3* pPivotPos);

	void SetToEngineCamera();

	void MouseRotate(int x,int y);

	void Process();

	void SetCurCamera(int Camera);
	void SetCameraPivotObject(int Camera,CObject* pObject);
	void SetCameraPivotPos(int Camera,VECTOR3* pPos);

	void SetTargetAngleX(int Camera,float fAngleX,DWORD Duration);
	void SetTargetAngleY(int Camera,float fAngleY,DWORD Duration);
	void SetDistance(int Camera,float fDistance,DWORD Duration);
	float GetDistance(int Camera) { return m_CameraDesc[Camera].m_fDistance; }

	void Rotate(int Camera,float RotateAngle,DWORD Duration);
	void ZoomDistance(int Camera,float fDistanceChange,DWORD Duration);

	void ChangeCameraSmooth(int Camera,DWORD Duration);	// ���� ī�޶󿡼� �ٸ� ī�޶�� �ε巴�� �̵��Ѵ�.
	void ChangeToDefaultCamera();	// ���� ī�޶��� �������� �״�� 0�� ī�޶� �����Ѵ�.
	void StopAllMoving(int Camera);	// ȸ���̳� �ܵ��� �����.

	void SetMaxSightDistance( float fMaxDistance )		{ m_fMaxSightDistance = fMaxDistance; m_fDistanceRate = 0.0f;  }
	void SetCameraFov( float fFov )						{ m_fFov = fFov; }

	void SetSightDistanceRate( float fDistanceRate );


	//////////////////////////////////////////////////////////////////////////
	// ��������� ���� �Լ���
	void SetPushedVibration(VECTOR3* PushedVector,DWORD Duration);
	//////////////////////////////////////////////////////////////////////////
	
	//////////////////////////////////////////////////////////////////////////
	// ����ŷ ����� ���� �Լ���
	void SetShaking(int ShakeNum, float amp, int Times = 1 );
	//////////////////////////////////////////////////////////////////////////
	// �̵������� ���� �Լ���
	void SetMovePos(int Index, MHCAMERAPOS* pCameraPos);
	void MoveStart(int GotoIndex);
	void Interpolation();
	BOOL IsMoved()				{	return m_bMoved;	}
	int GetGotoPosition()		{	return m_GotoPosition;	}
	void ResetInterPolation();
	//////////////////////////////////////////////////////////////////////////


	int GetCameraMode() { return m_CurCameraMode; }
	void SetCameraMode(int CameraMode);	
	void ToggleCameraViewMode();
	
	void SetCharState( int eCharState, DWORD dwDuration );
//	BOOL CanChangeCameraAngleTo( float xRad, float yRad );
	void Fly( float fFly );
	
	// ���� ī�޶� ������ �������� �Լ�
	MHCAMERADESC* GetCameraDesc() { return &m_CameraDesc[m_CurCamera]; }
	// 090422 ShinJS --- FollowView�� �ѹ��� ����(�����̵��� �Ϸ�Ǹ� Follow ������ eCM_Free�� ��ȯ)
	void ExecuteOnceFollowView();
	VECTOR3 GetForwardVector();
	VECTOR3 GetRightVector();

	// 091005 pdy ī�޶� ���� �۾�
private:
	CYHHashTable< stCameraFilterInfo > m_CameraFilterHash;

	// 100607 ShinJS --- ���� ���͸� ������ �����Ҽ� �ֵ��� Index�� List�� �����Ҽ� �ֵ��� �Ѵ�.
	typedef std::map< DWORD, CCameraFilterObject > CameraFilterObjectMap;
	std::map< DWORD, CameraFilterObjectMap > m_mapCameraFilter;
	CIndexGenerator m_CameraFilterIndexGenerator;

	typedef std::list< std::pair< DWORD, DWORD > > DeletedFilterContainer;
	DeletedFilterContainer m_DeleteFilterIndexList;
	DWORD m_HideInterfaceCount;
	void ProcessCameraFilter();

public:
	void LoadCameraFilterList();
	void ReleaseFilterList();
	stCameraFilterInfo* GetFilterInfo(DWORD dwKey) { return m_CameraFilterHash.GetData( dwKey ) ;}
	
	// ī�޶� ���͸� Attach �Ѵ�.
	CCameraFilterObject& AttachCameraFilter( DWORD dwIndex, BOOL bNoRepeat=FALSE, BOOL bFadeIn=FALSE, float fFadeRatio=1.f, DWORD dwRemainTime=0, eCAMERA_FILTER_ACTION eAction=eCAMERA_FILTER_ACTION_NONE );
	// �ش� ����Index�� ���͸� ��� �����Ѵ�.
	void DetachCameraFilter( DWORD dwFilterIndex, BOOL bFadeOut=FALSE, float fFadeRatio=1.f );
	// �ش� ����Index�� ����Object�� �����Ѵ�.
	void DetachCameraFilter( DWORD dwFilterIndex, DWORD dwObjectIndex, BOOL bFadeOut=FALSE, float fFadeTime=1.f );
	void ReleaseCameraFilter();
	void AddCameraFilterDeleteList( DWORD dwFilterIndex, DWORD dwObjectIndex );
	void DeleteCameraFilterObject( DWORD dwFilterIndex, DWORD dwObjectIndex );
};
EXTERNGLOBALTON(CMHCamera)
#endif // !defined(AFX_MHCAMERA_H__3EE48679_085F_4E12_AF97_C06D8AB29A00__INCLUDED_)
