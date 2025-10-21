/*
090316 LUJ, Ż�� Ŭ����
*/
#pragma once
#include "monster.h"

class CVehicle : public CMonster
{
public:
	CVehicle(void);
	virtual ~CVehicle(void);
	virtual BOOL Init( EObjectKind, DWORD agentNum, BASEOBJECT_INFO* );
	virtual void DoStand();
	virtual void DoRest( BOOL bStart );
	virtual BOOL DoWalkAround();
	virtual BOOL DoPursuit( CObject* = 0 );
	virtual BOOL DoAttack( DWORD attackNum );
	// 090316 LUJ, ���μ��� ó��
	virtual void StateProcess();
	virtual void DoDie( CObject* attacker );
	virtual DWORD SetAddMsg(DWORD dwReceiverID, BOOL isLogin, MSGBASE*&);
	// 091212 ShinJS --- �ȱ�/�ٱ� �ӵ� �з�
	virtual float DoGetMoveSpeed() { return (m_MoveInfo.MoveMode == eMoveMode_Run ? mRunSpeed : mWalkSpeed); }
	virtual DWORD Damage( CObject*, RESULTINFO* );
	DWORD GetEmptySeatForPassenger() const;
	DWORD GetEmptySeatForDriver();
	inline DWORD GetMountedSeatSize() const { return mMountedSeatSize; }
	// 090316 LUJ, ����� �������� �����Ѵ�
	inline void SetUseItem( const ICONBASE& iconBase ) { mUsedItemToSummon = iconBase; }
	inline const ICONBASE& GetUseItem() const { return mUsedItemToSummon; }
	// 091212 ShinJS --- �ȱ�/�ٱ� �ӵ� �з�
	inline void SetMoveSpeed( float runSpeed, float walkSpeed ) { mRunSpeed = runSpeed;		mWalkSpeed = walkSpeed; }
	MSG_VEHICLE_ERROR::Error Mount( DWORD playerIndex );
	MSG_VEHICLE_ERROR::Error Mount( DWORD playerIndex, DWORD seatIndex );
	// 090316 LUJ, �¼����� ������. ���� �¼� ��ȣ�� ��ȯ�ȴ�
	MSG_VEHICLE_ERROR::Error Dismount( const DWORD playerIndex, const BOOL needBuff );
	// 090316 LUJ, �ش� �÷��̾ ž���� �¼� ��ȣ�� ��´�
	DWORD GetMountedSeat( const DWORD playerIndex ) const;
	const VehicleScript& GetScript() const;
	// 090316 LUJ, �ش� �¼��� �ִ� �÷��̾� ��ȣ�� ���Ѵ�
	DWORD GetPlayerInSeat( const DWORD seatIndex ) const;
	MSG_VEHICLE_ERROR::Error Move( const DWORD askedPlayerIndex, VECTOR3& startPosition, VECTOR3& targetPosition );
	MSG_VEHICLE_ERROR::Error EndMove( const DWORD askedPlayerIndex, VECTOR3& position );	
	BOOL Teleport( const DWORD askedPlayerIndex, const MAPCHANGE_INFO* const mapChangeInfo );
	// 090316 LUJ, Ư�� �÷��̾�� ��ũ��Ʈ�� ���ǵ� ������ �����Ѵ�
	void AddBuff( const DWORD playerIndex );
	// 090408 ShinJS, Owner���� �Ÿ��� �־��� ��� �����̵� ��Ų��
	BOOL Recall( const VECTOR3& );
	// 100121 ShinJS --- Ư�� �÷��̾� �̿��� ��� �÷��̾ ������Ų��
	void DismountAllExceptOne( DWORD dwExceptIndex );
	BOOL IsInfiniteSummon() const { return UINT_MAX == m_MonsterInfo.OwnedObjectIndex; }

private:
	// 090316 LUJ, �¼� ��ȣ�� �������� �˻��Ѵ�
	BOOL IsInvalidSeatIndex( const DWORD seatIndex ) const;
	// 090316 LUJ, �¼��� �÷��̾ �����Ѵ�
	void SetSeatMountedPlayer( const DWORD playerIndex, const DWORD seatIndex );
	void SetSeatSize( const VehicleScript& );
	// 090316 LUJ, ž���� ��� �÷��̾ ������Ų��
	void DismountAll();
	BOOL Teleport(DWORD playerIndex, const MAPCHANGE_INFO&);

private:
	// 090316 LUJ, �¼� ��ȣ���� ž�� ���� �÷��̾� ��ȣ�� ����ִ�. [�ε��� = �¼� ��ȣ - 1]
	DWORD mSeat[VehicleScript::Seat::MaxSize];
	ICONBASE mUsedItemToSummon;
	// 090316 LUJ, 
	DWORD mMountedSeatSize;
	// 091212 ShinJS --- Run/Walk ����
	float mRunSpeed;
	float mWalkSpeed;
	// 090316 LUJ, �̵� ������ ����ȭ�� �¼� ��ȣ. �� ���μ��� Ÿ�ӿ� �ϳ��� ����ȭ��Ų��
	DWORD mMoveSyncedSeatIndex;
};