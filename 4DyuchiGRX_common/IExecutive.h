#pragma once

#include "stdafx.h"
#include "IExecutive_GUID.h"
#include "IRenderer.h"
#include "IGeometry.h"

#include "typedef.h"

interface I4DyuchiGXExecutive : public IUnknown
{
	virtual I4DyuchiGXRenderer*	__stdcall	GetRenderer() = 0;		// �������̽��� ���� Release()�ʿ����.
	virtual I4DyuchiGXGeometry*	__stdcall	GetGeometry() = 0;		// �������̽��� ���� Release()�ʿ����.

	virtual HRESULT				__stdcall	GetRenderer(I4DyuchiGXRenderer** ppRenderer) = 0;		// Release�ʿ���
	virtual HRESULT				__stdcall	GetGeometry(I4DyuchiGXGeometry** ppGeometry) = 0;		// Release�ʿ���
	virtual	HRESULT				__stdcall	GetFileStorage(I4DyuchiFileStorage** ppFileStorage) = 0;	// Release�ʿ���

	virtual BOOL				__stdcall	InitializeWithoutRegistry(char* szGeometryFileName,char* szRendererFileName,HWND hWnd,DISPLAY_INFO* pInfo,DWORD dwMaxObjectNum,DWORD dwMaxLightNum,DWORD dwMaxTriggerNum,DWORD dwViewportNum, DWORD dwMaxDecalNum, DWORD dwSecretKey, ErrorHandleProc pErrorHandleFunc) = 0;
	virtual BOOL				__stdcall	Initialize(HWND hWnd,DISPLAY_INFO* pInfo,DWORD dwMaxObjectNum,DWORD dwMaxLightNum,DWORD dwMaxTriggerNum,DWORD dwViewportNum, DWORD dwMaxDecalNum,ErrorHandleProc pErrorHandleFunc) = 0;
	// 090831 ShinJS --- Pack���� Create/Load �� Share Mode ���� �߰��Ͽ� ��Ƽ Ŭ���̾�Ʈ ������� ��.
	virtual BOOL				__stdcall	InitializeFileStorageWithoutRegistry(char* szFileName,DWORD dwMaxFileNum,DWORD dwMaxFileHandleNumAtSameTime,DWORD dwMaxFileNameLen,FILE_ACCESS_METHOD accessMethod,PACKFILE_NAME_TABLE* pPackFileList,DWORD dwNum,DWORD dwSecretKey, BOOL bFileShareMode=FALSE) = 0;
	virtual BOOL				__stdcall	InitializeFileStorage(DWORD dwMaxFileNum,DWORD dwMaxFileHandleNumAtSameTime,DWORD dwMaxFileNameLen,FILE_ACCESS_METHOD accessMethod,PACKFILE_NAME_TABLE* pPackFileList,DWORD dwNum,DWORD dwSecretKey, BOOL bFileShareMode=FALSE) = 0;
	
	virtual void				__stdcall	SetViewport(DWORD dwViewportIndex) = 0;

	virtual DWORD				__stdcall	IsValidHandle(GXMAP_OBJECT_HANDLE gxh) = 0;	// ����׿� �Լ�.������ ������.

	virtual DWORD				__stdcall	GXOProcess(GXOBJECT_HANDLE gxo,DWORD dwFrameInc) = 0;
	virtual BOOL				__stdcall	GXORender(GXOBJECT_HANDLE gxo) = 0;

	
	virtual void				__stdcall	GXOSetCurrentFrame(GXOBJECT_HANDLE gxh,DWORD dwFrame) = 0;
	virtual void				__stdcall	GXOSetCurrentMotionIndex(GXOBJECT_HANDLE gxh,DWORD dwMotionIndex) = 0;
	virtual BOOL				__stdcall	GXOGetMotionDesc(GXOBJECT_HANDLE gxh,MOTION_DESC* pMotionDesc,DWORD dwMotionIndex,DWORD dwModelIndex) = 0;
	virtual DWORD				__stdcall	GXOGetMotionNum(GXOBJECT_HANDLE gxh,DWORD dwModelIndex) = 0;			// ��ǵ��� ������ ��´�.
	virtual DWORD				__stdcall	GXOGetCurrentFrame(GXOBJECT_HANDLE gxh) = 0;
	virtual DWORD				__stdcall	GXOGetCurrentMotionIndex(GXOBJECT_HANDLE gxh) = 0;
	
	virtual void				__stdcall	GXOSetCurrentMaterialIndex(GXOBJECT_HANDLE gxh,DWORD dwMaterialIndex) = 0;
	virtual	DWORD				__stdcall	GXOGetMaterialNum(GXOBJECT_HANDLE gxh,DWORD dwModelIndex) = 0;
	virtual DWORD				__stdcall	GXOGetCurrentMaterialIndex(GXOBJECT_HANDLE gxh) = 0;

	virtual void				__stdcall	GXOSetPosition(GXOBJECT_HANDLE gxh,VECTOR3* pv3Pos, BOOL bDoInterpolation) = 0;
	virtual void				__stdcall	GXOMovePosition(GXOBJECT_HANDLE gxh,VECTOR3* pv3Pos) = 0;
	virtual void				__stdcall	GXOGetPosition(GXOBJECT_HANDLE gxh,VECTOR3* pv3Pos) = 0;
	virtual	void				__stdcall	GXOMovePositionWithCollide(GXOBJECT_HANDLE gxh,VECTOR3* pv3Pos)=0;		// 2002/05/22
	virtual void				__stdcall	GXOGetInterpolatedPosition( GXOBJECT_HANDLE gxh,VECTOR3* pv3Pos)=0; // �����Ǿ ������ �Ǵ� �������� ���. ���� GXO�� ��ġ�� �ƴϴ�. ī�޶���ġ ������ �� ���� ������.
	
	virtual void				__stdcall	GXOSetDirection(GXOBJECT_HANDLE gxh,VECTOR3* pv3Axis,float fRad) = 0;
	virtual void				__stdcall	GXOSetDirectionFPSStyle(GXOBJECT_HANDLE gxh, VECTOR3* pv3Angles /* in */) = 0;
	virtual	void				__stdcall	GXOChangeDirection(GXOBJECT_HANDLE gxh,float fRad) = 0;
	virtual void				__stdcall	GXOGetDirection(GXOBJECT_HANDLE gxh,VECTOR3* pv3Axis,float* pfRad) = 0;
	virtual void				__stdcall	GXOGetDirectionFPSStyle(GXOBJECT_HANDLE gxh, VECTOR3* pv3Angle /* out */) = 0;
	virtual void				__stdcall	GXOSetScale(GXOBJECT_HANDLE gxh,VECTOR3* pv3Scale) = 0;
	virtual void				__stdcall	GXOGetScale(GXOBJECT_HANDLE gxh,VECTOR3* pv3Scale) = 0;
	virtual void				__stdcall	GXOMoveForward(GXOBJECT_HANDLE gxh,float fDistance) = 0;
	
	virtual void				__stdcall	GXOEnableHFieldApply(GXOBJECT_HANDLE gxo) = 0;
	virtual void				__stdcall	GXODisableHFieldApply(GXOBJECT_HANDLE gxo) = 0;
	virtual BOOL				__stdcall	GXOIsHFieldApply(GXOBJECT_HANDLE gxo) = 0;

	virtual void				__stdcall	GXOSetZOrder(GXOBJECT_HANDLE gxo,int iZOrder) = 0;

	virtual void				__stdcall	GXOEnableAsEffect(GXOBJECT_HANDLE gxo) = 0;
	virtual void				__stdcall	GXODisableAsEffect(GXOBJECT_HANDLE gxo) = 0;
	virtual BOOL				__stdcall	GXOIsAsEffect(GXOBJECT_HANDLE gxo) = 0;

	virtual void				__stdcall	GXOEnableSelfIllumin(GXOBJECT_HANDLE gxo) = 0;
	virtual void				__stdcall	GXODisableSelfIllumin(GXOBJECT_HANDLE gxo) = 0;
	virtual BOOL				__stdcall	GXOIsEnableSelfIllumin(GXOBJECT_HANDLE gxo) = 0;

	virtual void				__stdcall	EstimatedMoveForward(GXOBJECT_HANDLE gxh,VECTOR3* pv3Pos,float fDistance) = 0;

	//////////////////////////
	virtual	void				__stdcall	GXOSetBoundingVolume( GXOBJECT_HANDLE gxh, BOUNDING_VOLUME* pBV)=0;
	virtual	BOOL				__stdcall	GXOIsLanding( GXOBJECT_HANDLE gxh)=0;

	virtual BOOL				__stdcall	GXOAttach(GXOBJECT_HANDLE gxh,GXOBJECT_HANDLE to_gxh,char* szObjName) = 0;
	virtual BOOL				__stdcall	GXOAttachWithModelIndex(GXOBJECT_HANDLE gxh,GXOBJECT_HANDLE to_gxh,DWORD dwModelIndex,char* szObjName) = 0;
	virtual BOOL				__stdcall	GXODetach(GXOBJECT_HANDLE gxh,GXOBJECT_HANDLE child_gxh) = 0;
	virtual BOOL				__stdcall	GXOAttachLight(GXOBJECT_HANDLE gxoh,char* szObjName,GXLIGHT_HANDLE gxlh,VECTOR3* pv3Offset,DWORD dwAttachType) = 0;
	virtual BOOL				__stdcall	GXODetachLight(GXOBJECT_HANDLE gxoh,GXLIGHT_HANDLE child_gxh) = 0;
	virtual BOOL				__stdcall	GXOAttachCameraFront(GXOBJECT_HANDLE gxo,float fDist) =0;
	virtual BOOL				__stdcall	GXODetachCameraFront(GXOBJECT_HANDLE gxo) = 0;
	virtual ULONG				__stdcall	GXOGetModel(GXOBJECT_HANDLE gxh,I3DModel** ppModel,DWORD* pdwRefIndex,DWORD dwModelIndex) = 0;// Release()�ʿ�
	virtual	GXOBJECT_HANDLE		__stdcall	GXOGetObjectWithScreenCoord(VECTOR3* pv3IntersectPoint,DWORD* pdwModelIndex,DWORD* pdwObjIndex,float* pfDist,POINT* ptCursor,DWORD dwViewportIndex,DWORD dwFlag) = 0;
	virtual DWORD				__stdcall	GXOGetMultipleObjectWithScreenCoordRect(PICK_GXOBJECT_DESC* pPickDesc,DWORD dwMaxDescNum,RECT* pRect,DWORD dwViewportIndex,DWORD dwFlag) = 0;
	virtual DWORD				__stdcall	GXOGetMultipleObjectWithScreenCoord(PICK_GXOBJECT_DESC* pPickDesc,DWORD dwMaxDescNum,POINT* ptCursor,DWORD dwViewportIndex,DWORD dwFlag) = 0;
	virtual BOOL				__stdcall	GXOReplaceModel(GXOBJECT_HANDLE gxh,DWORD dwModelIndex,char* szFileName) = 0;
	virtual void				__stdcall	GXOGetCollisionMesh(GXOBJECT_HANDLE gxh,COLLISION_MESH_OBJECT_DESC* pColMeshDesc) = 0;
	virtual DWORD				__stdcall	GXOGetObjectIndex(GXOBJECT_HANDLE gxh,char* szObjName,DWORD dwModelIndex) = 0;
	virtual BOOL				__stdcall	GXOGetWorldMatrixPerObject(GXOBJECT_HANDLE gxh,MATRIX4* pMatWorld,DWORD dwModelIndex,DWORD dwObjIndex) = 0;
	virtual DWORD				__stdcall	GXOGetAllObjectsWitLoadMapScript(GXOBJECT_HANDLE* pObjArray,DWORD dwMaxNum) = 0;
	
	virtual void				__stdcall 	DisableRender(GXMAP_OBJECT_HANDLE gxh) = 0;
	virtual void				__stdcall 	EnableRender(GXMAP_OBJECT_HANDLE gxh) = 0;
	virtual void				__stdcall	DisableSchedule(GXMAP_OBJECT_HANDLE gxh) = 0;
	virtual	BOOL				__stdcall	IsRenderable(GXMAP_OBJECT_HANDLE gxh) = 0;
	virtual void				__stdcall	EnableSchedule(GXMAP_OBJECT_HANDLE gxh) = 0;
	
	virtual void				__stdcall	LockTransform(GXMAP_OBJECT_HANDLE gxh) = 0;
	virtual void				__stdcall	UnLockTransform(GXMAP_OBJECT_HANDLE gxh) = 0;
	virtual	BOOL				__stdcall	IsLockTransform(GXMAP_OBJECT_HANDLE gxh) = 0;


	virtual void				__stdcall	SetPickType(GXMAP_OBJECT_HANDLE	gxh,PICK_TYPE type) = 0;
	virtual PICK_TYPE			__stdcall	GetPickType(GXMAP_OBJECT_HANDLE	gxh) = 0;

	virtual void				__stdcall	SetRenderFlag(GXMAP_OBJECT_HANDLE gxh,DWORD dwFlag) = 0;
	virtual DWORD				__stdcall	GetRenderFlag(GXMAP_OBJECT_HANDLE gxh) = 0;
	virtual void				__stdcall	SetAlphaFlag(GXMAP_OBJECT_HANDLE gxh,DWORD dwFlag) = 0;
	virtual DWORD				__stdcall	GetAlphaFlag(GXMAP_OBJECT_HANDLE gxh) = 0;
	virtual void*				__stdcall	GetData(GXMAP_OBJECT_HANDLE gxh) = 0;
	virtual void				__stdcall	SetData(GXMAP_OBJECT_HANDLE gxh,void* pData) = 0;
	virtual DWORD				__stdcall	GetID(GXMAP_OBJECT_HANDLE gxh) = 0;
	virtual BOOL				__stdcall	SetID(GXMAP_OBJECT_HANDLE gxh,DWORD dwID) = 0;
	virtual void 				__stdcall	ReleaseID(GXMAP_OBJECT_HANDLE gxh) = 0;
	virtual BOOL				__stdcall	IsInViewVolume(GXMAP_OBJECT_HANDLE gxh) = 0;
	virtual BOOL				__stdcall	IsCollisionWithScreenCoord(GXOBJECT_HANDLE gxh,VECTOR3* pv3IntersectPoint,DWORD* pdwModelIndex,DWORD* pdwObjIndex,float* pfDist,POINT* ptCursor,DWORD dwViewportIndex,DWORD dwFlag) = 0;

	virtual GXMAP_OBJECT_HANDLE __stdcall	GetGXMapObjectWithID(DWORD dwID) = 0;
	
	virtual void				__stdcall	SetPropertyFlag(GXMAP_OBJECT_HANDLE gxh,DWORD dwFlag) = 0;
	virtual DWORD				__stdcall	GetPropertyFlag(GXMAP_OBJECT_HANDLE gxh) = 0;

	virtual void				__stdcall	EnablePick(GXMAP_OBJECT_HANDLE gxh) = 0;
	virtual void				__stdcall	DisablePick(GXMAP_OBJECT_HANDLE gxh) = 0;
	virtual void				__stdcall	EnableUpdateShading(GXMAP_OBJECT_HANDLE gxh) = 0;
	virtual void				__stdcall	DisableUpdateShading(GXMAP_OBJECT_HANDLE gxh) = 0;
	virtual	void				__stdcall	EnableSendShadow(GXMAP_OBJECT_HANDLE gxh) = 0;
	virtual void				__stdcall	DisableSendShadow(GXMAP_OBJECT_HANDLE gxh) = 0;

	
	
	virtual	BOOL				__stdcall	PreCreateLight(char* szFileName,DWORD dwFlag) = 0;
	virtual DWORD				__stdcall	PreLoadGXObject(char* szFileName) = 0;
	virtual GXOBJECT_HANDLE 	__stdcall	CreateGXObject(char* szFileName,GXSchedulePROC pProc,void* pData,DWORD dwFlag) = 0;
	virtual GXLIGHT_HANDLE		__stdcall	CreateGXLight(LIGHT_DESC* pDesc,GXLightSchedulePROC pProc,void* pData,DWORD dwProjTexIndex,MATERIAL* pMtl,DWORD dwFlag) = 0;
	virtual GXTRIGGER_HANDLE	__stdcall	CreateGXEventTrigger(GXSchedulePROC pProc,void* pData,DWORD dwFlag) = 0;
	
	virtual GXMAP_HANDLE		__stdcall	CreateGXMap(GXMapSchedulePROC pProc,void* pData,DWORD dwFlag) = 0;
	virtual void				__stdcall	UnloadAllPreLoadedGXObject(DWORD dwFlag) = 0;
	virtual void				__stdcall	UnloadPreLoadedGXObject(GXOBJECT_HANDLE gxo) = 0;


	virtual BOOL				__stdcall	BeginBuildMap(DWORD dwFlag) = 0;
	virtual BOOL				__stdcall	InsertHFieldToGXMap(IHeightField* pHField) = 0;
	virtual BOOL				__stdcall	InsertStaticModelTOGXMap(I3DStaticModel* pModel) = 0;
	virtual BOOL				__stdcall	EndBuildMap(float fTop,float fBottom) = 0;
	virtual	BOOL				__stdcall	RebuildMap(float fTop,float fBottom)=0;
	virtual BOOL				__stdcall	GetWorldBoundingBox( MAABB* pWorldBox) = 0;

	virtual BOOL				__stdcall	DeleteGXMap(GXMAP_HANDLE gxm) = 0;
	virtual BOOL				__stdcall	DeleteAllGXObjects() = 0;
	virtual BOOL				__stdcall	DeleteAllGXLights() = 0;
	virtual BOOL				__stdcall	DeleteAllGXEventTriggers() = 0;
	virtual BOOL				__stdcall	DeleteGXObject(GXOBJECT_HANDLE gxh) = 0;
	virtual BOOL				__stdcall	DeleteGXLight(GXLIGHT_HANDLE gxh) = 0;
	virtual BOOL				__stdcall	DeleteGXEventTrigger(GXTRIGGER_HANDLE gxh) = 0;

	virtual DWORD				__stdcall	GetFramePerSec() = 0;
	virtual	void				__stdcall	SetFramePerSec(DWORD dwFrame) = 0;
	
	virtual	void				__stdcall	GXLSetLightDesc(GXLIGHT_HANDLE gxh,LIGHT_DESC* pLightDesc) = 0;
	virtual	void				__stdcall	GXLGetLightDesc(GXLIGHT_HANDLE gxh,LIGHT_DESC* pLightDesc) = 0;
	virtual void				__stdcall	GXLSetPosition(GXLIGHT_HANDLE gxh,VECTOR3* pv3Pos) = 0;
	virtual void				__stdcall	GXLGetPosition(GXLIGHT_HANDLE gxh,VECTOR3* pv3Pos) = 0;
	virtual	void				__stdcall	GXLMovePosition(GXLIGHT_HANDLE gxh,VECTOR3* pv3Pos) = 0;
	virtual GXLIGHT_HANDLE		__stdcall	GXLGetLightWithScreenCoord(VECTOR3* pv3IntersectPoint,float* pfDist,POINT* ptCursor,DWORD dwViewportIndex,DWORD dwFlag) = 0;
	virtual	void				__stdcall	GXLDisableStaticShadow(GXLIGHT_HANDLE gxl) = 0;
	virtual void				__stdcall	GXLEnableStaticShadow(GXLIGHT_HANDLE gxl) = 0;
	virtual BOOL				__stdcall	GXLIsDisableStaticShadow(GXLIGHT_HANDLE gxl) = 0;
		
	virtual void				__stdcall	GXLEnableDynamicLight(GXLIGHT_HANDLE gxl) = 0;
	virtual void				__stdcall	GXLDisableDynamicLight(GXLIGHT_HANDLE gxl) = 0;
	virtual	BOOL				__stdcall	GXLIsEnableDynamicLight(GXLIGHT_HANDLE gxl) = 0;

	virtual void				__stdcall	GXTSetPosition(GXTRIGGER_HANDLE gxh,VECTOR3* pv3Pos) = 0;
	virtual void				__stdcall	GXTSetScale(GXTRIGGER_HANDLE gxh,VECTOR3* pv3PScale) = 0;
	virtual void				__stdcall	GXTSetRotation(GXTRIGGER_HANDLE gxh,VECTOR3* pv3Rot) = 0;
	virtual void				__stdcall	GXTGetEventTriggerDesc(GXTRIGGER_HANDLE gxh,EVENT_TRIGGER_DESC* pEVDesc) = 0;
	virtual GXTRIGGER_HANDLE	__stdcall	GXTGetObjectWithScreenCoord(VECTOR3* pv3IntersectPoint,float* pfDist,POINT* ptCursor,DWORD dwViewportIndex,DWORD dwFlag) = 0;
	
	virtual BOOL				__stdcall	GXMGetCollisionPointWithScreenCoord(DWORD* pdwObjIndex,VECTOR3* pv3IntersectPoint,float* pfDist,POINT* ptCursor,DWORD dwFlag) = 0;
	virtual BOOL				__stdcall	GXMGetHFieldCollisionPointWithRay(VECTOR3* pv3IntersectPoint,float* pfDist,VECTOR3* pv3Orig,VECTOR3* pv3Dir) = 0;
	virtual	BOOL				__stdcall	GXMGetHFieldCollisionPointWithScreenCoord(VECTOR3* pv3IntersectPoint,float* pfDist,POINT* ptCursor) = 0;
	virtual BOOL				__stdcall	GXMGetHFieldHeight(float* py,float x,float z) = 0;
	virtual BOOL				__stdcall	GXMStaticShadeMap(DWORD dwAmbientColor,LIGHT_DESC* pLight,DWORD dwLightNum,DWORD dwFlag,SHADE_FUNC pFunc) = 0;
	virtual void				__stdcall	GXMSetHFieldAlphaFlag(DWORD dwAlpha) = 0;
	virtual void				__stdcall	GXMSetSTMAlphaFlag(DWORD dwAlpha) = 0;;
	
	
	
	virtual	BOOL				__stdcall	SetSymbol(SYMBOL_TYPE type,char* szFileName) = 0;
	virtual	void				__stdcall	SetRenderMode(DWORD dwRenderMode) = 0;
	virtual DWORD				__stdcall	GetRenderMode() = 0;

	//trustpak 2005/06/09
	virtual VOID				__stdcall	SetPickingMode(DWORD dwPickingMode) = 0;

	virtual	BOOL				__stdcall	DisableRenderAllStaticModels(VOID) = 0;
	virtual BOOL				__stdcall	EnableRenderAllStaticModels(VOID) = 0;

	///

	virtual	DWORD				__stdcall	GetGXObjectsNum() = 0;
	virtual	GXOBJECT_HANDLE		__stdcall	GetGXObjectWithSeqIndex(DWORD dwIndex) = 0;

	virtual DWORD				__stdcall	GetGXLightsNum() = 0;
	virtual GXLIGHT_HANDLE		__stdcall	GetGXLightWithSeqIndex(DWORD dwIndex) = 0;

	virtual DWORD				__stdcall	GetGXEventTriggersNum() = 0;
	virtual GXTRIGGER_HANDLE	__stdcall	GetGXEventTriggerWithSeqIndex(DWORD dwIndex) = 0;
	virtual BOOL				__stdcall	LoadMapScript(char* szFileName,LOAD_CALLBACK_FUNC pFunc,DWORD dwFlag) = 0;
	virtual void				__stdcall	DeleteAllGXMapObjectsWitLoadMapScript() = 0;
	
	virtual void				__stdcall	Render() = 0;
	virtual BOOL				__stdcall	RenderCameraFrontObject() = 0;
	// �������� �Ǵ�.
	// �� �Լ��� Process()�� ¦�� �̷� ���°��� ����. Run()���� ��Ÿ��.
	virtual DWORD				__stdcall	Process() = 0;
	// ���� �������� �׽�Ʈ �ؼ� ������Ű��, ������ �����Ǹ� GXObject�� ���ν����� ��ȸ�Ѵ�.
	// ���ϰ��� ������ �������̴�.
	
	virtual DWORD				__stdcall	Run(DWORD dwBackColor,GX_FUNC pfBeforeRenderFunc,GX_FUNC pfAfterRenderFunc,DWORD dwFlag) = 0;
	// ���� ���ν����� ������ �������� �Ǵ�.
	// ���������� Process()�� Render()�� ���� ���̴�.
	// ���ӿ��� �� �Լ��� �� ���, Render()�� Process()�� ���� ȣ�� �ؼ��� �ȵȴ�.
	// ���ϰ��� ������ �������̴�.
	virtual BOOL				__stdcall	SetCameraFitGXObject(GXOBJECT_HANDLE gxo,float fNear,float fFar,float fFov,DWORD dwViewportIndex) = 0;
	virtual void				__stdcall	SetHFieldDetail(DWORD dwDetail) = 0;

	// ��Į 2003.01.15
	virtual	GXDECAL_HANDLE		__stdcall	CreateGXDecal( DECAL_DESC* pDesc, GXDecalSchedulePROC pProc, void* pData, DWORD dwFlag) = 0;
	virtual	BOOL				__stdcall	DeleteGXDecal( GXDECAL_HANDLE gxd) = 0;
//	virtual	BOOL				__stdcall	DeleteAllGXDecal() = 0;
	
	virtual BOOL				__stdcall	GXOInitializeIllusionEffect(GXOBJECT_HANDLE gxh,DWORD dwMaxIllusionFrameNum,char* szObjName,void* pMtlHandle,DWORD dwFlag) = 0;
		
	virtual void				__stdcall	GXOBeginIllusionEffect(GXOBJECT_HANDLE gxh) = 0;
	virtual void				__stdcall	GXOEndIllusionEffect(GXOBJECT_HANDLE gxh) = 0;
	virtual DWORD				__stdcall	GXOGetAttachedGXObjects(GXOBJECT_HANDLE gxh,GXOBJECT_HANDLE* pGXOList,DWORD dwMaxNum) = 0;

	virtual void				__stdcall	GXOSetScheduleProc(GXOBJECT_HANDLE gxo,GXSchedulePROC pProc) = 0;
	virtual GXSchedulePROC		__stdcall	GXOGetScheduleProc(GXOBJECT_HANDLE gxo) = 0;

	virtual	void				__stdcall	GXOSetEffectIndex(GXOBJECT_HANDLE gxo,DWORD dwEffectIndex) = 0;
	virtual	DWORD				__stdcall	GXOGetEffectIndex(GXOBJECT_HANDLE gxo) = 0;

	virtual	void				__stdcall	SetAfterInterpolation( AfterInterpolationCallBack pfCallBack)=0;
	// �ͽ�ťƼ��::Render()���� ��ġ������ ������ ���̿� ȣ��Ǵ� �ݹ�.
	// ī�޶� ĳ���Ϳ� ���̰ų��� �� ���� �����ϴ�. ���� �Ϸ��� NULL�� �����Ѵ�.

	// ������ ���� �������� �����Ѵ�.
	virtual	DWORD				__stdcall	GetCurrentGameFrame(void)=0;

	// ���� �ö���ִ� stm�� �����Ѵ�. ������� NULL
	virtual ULONG				__stdcall	GXMGetStaticModel( I3DStaticModel** ppStaticModel) = 0; //���� COM��ü�� ���� Release�ʿ�.�����ϸ� ���ϰ�0,�����ϸ� 0xffffffff
	virtual ULONG				__stdcall	GXMGetHField(IHeightField** ppHField) = 0;			// ���� COM��ü�� ���� Release�ʿ�.�����ϸ� ���ϰ�0,�����ϸ� 0xffffffff

	virtual void				__stdcall	GXODisableUnloadPreLoaded(GXOBJECT_HANDLE gxh) = 0;
	virtual void				__stdcall	GXOEnableUnloadPreLoaded(GXOBJECT_HANDLE gxh) = 0;

	virtual void				__stdcall	GXOEnableScaleOfAttachedModel(GXOBJECT_HANDLE gxo) = 0;
	virtual void				__stdcall	GXODisableScaleOfAttachedModel(GXOBJECT_HANDLE gxo) = 0;

	//trustpak 2005/06/24

	virtual		void			__stdcall	SetModelLODUsingMode(DWORD dwLodUsingMode, DWORD dwLevel) = 0;
	virtual		DWORD			__stdcall	GetModelLODUsingMode(VOID) = 0;	
	virtual		DWORD			__stdcall	GetModelLodUsingLevel(VOID) = 0;

	// 090422 ShinJS --- GXObject�� Parent Matrix ��ȯ
	virtual		MATRIX4			__stdcall	GXOGetParentMatrix(GXOBJECT_HANDLE gxh) = 0;

	// NYJ 090313 - �Ͽ�¡
	virtual BOOL				__stdcall	GXOIsCollisionMeshWithBoundingBox(GXOBJECT_HANDLE gxh, VECTOR3* pv3IntersectPoint,DWORD* pdwModelIndex,DWORD* pdwObjIndex,float* pfDist,COLLISION_MESH_OBJECT_DESC* pSrcColDesc) = 0;
	virtual BOOL				__stdcall	GXOIsCollisionMeshWithObjectBoundingBox(GXOBJECT_HANDLE gxhDest, VECTOR3* pv3IntersectPoint,DWORD* pdwModelIndex,DWORD* pdwObjIndex,float* pfDist,GXOBJECT_HANDLE gxhSrc) = 0;
	virtual BOOL				__stdcall	GXOIsCollisionBoungingBoxWithScreenCoord(GXOBJECT_HANDLE gxh, VECTOR3* pv3IntersectPoint, VECTOR3* pv3ColTri, POINT* ptCursor,DWORD dwViewportIndex) = 0;
	virtual DWORD				__stdcall	GXOGetModelNum(GXOBJECT_HANDLE gxh) = 0;
	virtual DWORD				__stdcall	GXOGetObjectNum(GXOBJECT_HANDLE gxh, DWORD dwModelIndex) = 0;
	virtual BOOL				__stdcall	GXOGetCollisononObjectDesc(GXOBJECT_HANDLE gxh, COLLISION_MESH_OBJECT_DESC* pDesc, DWORD dwModelIndex, DWORD dwObjectIndex) = 0;
	virtual BOOL				__stdcall	GXOGetCollisionModelDesc(GXOBJECT_HANDLE gxh, COLLISION_MODEL_DESC* pDesc, DWORD dwMaxModelNum) = 0;

	// pdy 090519 - �Ͽ�¡ 
	virtual BOOL				__stdcall	GXOGetWorldMatrixByName(GXOBJECT_HANDLE gxh , MATRIX4* pMatrix4 ,char* szObjName ) = 0;

	///
};
