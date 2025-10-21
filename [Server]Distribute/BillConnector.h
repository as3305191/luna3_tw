/*
����: �ֵ�� ����Ʈ���� �����ϴ� ���� ������ ����Ͽ�, ���� �������� Ȯ���Ѵ�
�۾���: �̿���
����: ��ö�� ����

���ӿ��� ����ϴ� inetwork.dll�� ����� ����ؼ� ������ �Ѵ�. �̷��� ���� ������
�߸��� ��Ŷ�� �����Ѵ�. �̸� ���ϱ� ���� curl 7.20.1 ���̺귯���� �̿��ؼ� ���
�Ͽ� �����ߴ�.
*/
#pragma once
#include <queue>

class CBillConnector
{
private:
	const int mGameNumber;
	DWORD mLastConnectTick;
	DWORD mConnectTryCount;
	LPCTSTR mCompanyCode;
	char mGameServerName[32];
	LPVOID mCurl;
	SOCKET mSocket;

	typedef DWORD UserIndex;
	typedef std::map< UserIndex, _PACKET_GAME > UserPacketContainer;
	UserPacketContainer mUserPacketContainer;

	// ������ �α��� ���� �˻��ϱ� ���� ����Ѵ�. �ش� ������Ʈ�� ������ ���ٰ�
	// ������Ʈ���� �뺸�� �� ������ UserCheckFailedCount�� 1�� ���ҽ�Ų��.
	// ������Ʈ ������ ���� ���� �� �ִ�. ���� ���а� �Ǵ��� UserCheckFailedCount = 0��
	// �������� ���� ������ �������� �ʴ´�
	typedef size_t AgentSize;
	typedef std::map< UserIndex, AgentSize > UserCheckContainer;
	UserCheckContainer mUserCheckContainer;

	std::queue< _PACKET_GAME > mSendPacketQueue;

private:
	void PacketResultAuth(const _PACKET_GAME&) const;
	void PacketResultAlert(const _PACKET_GAME&) const;
	void Send(_PACKET_GAME&);
	const _PACKET_GAME& GetPacket(UserIndex) const;
	void SetGUID(_PACKET_GAME&) const;
	void Connect(DWORD serverIndex);
	void Receive(const _PACKET_GAME&);
	BOOL IsInvalid() const { return UINT_MAX == mSocket; }
	void SetInvalid() { mSocket = UINT_MAX; }

public:
	CBillConnector();
	virtual ~CBillConnector();
	void GameStart(UserIndex, LPCTSTR userIpAddress);
	// ����� ���� ���� �� ȣ��ȴ�
	void GameEnd(UserIndex);
	void Process();
	void SucceedToCheck(UserIndex);
	void FailToCheck(UserIndex);
	void Reset();
};