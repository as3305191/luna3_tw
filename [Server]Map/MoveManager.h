/*
081031 LUJ, �÷��̾� ���� ����� �۾��ϸ� �̵� ó���� �Ŵ����� �и���
*/
#pragma once

#define MOVEMGR	( &( CMoveManager::GetInstance() ) )

class CMoveManager  
{
public:
	CMoveManager();
	virtual ~CMoveManager();
	static CMoveManager& GetInstance();
	void NetworkMsgParse( MSGBASE* const, DWORD dwLength );

	// 081031 LUJ, ���� ó��
public:
	// 081031 LUJ, �÷��̾� ��ȯ�� ��û�Ѵ�
	void RequestToRecall( const MSG_RECALL_SYN& );
	// 081031 LUJ, �÷��̾ ��ȯ�Ѵ�
	void Recall( DWORD key, DWORD guildScore = 0, DWORD familyPoint = 0 );
private:
	// 081031 LUJ, ó���� ���� ������ �����Ѵ�
	void SetRecallMessage( DWORD key, const MSG_RECALL_SYN& );
	// 081031 LUJ, ����� ���� ������ ��ȯ�Ѵ�
	const MSG_RECALL_SYN& GetRecallMessage( DWORD key ) const;
	// 081031 LUJ, ���� ������ �ִٸ� ��ȯ�� �����̳ʿ��� �����Ѵ�
	MSG_RECALL_SYN PopRecallMessage( DWORD key );
	// 081031 LUJ, ���� ��û�� ������ �����Ѵ�. Ű: ��ȯ ������ ƽī��Ʈ
	typedef stdext::hash_map< DWORD, MSG_RECALL_SYN >	RecallMessageMap;
	RecallMessageMap									mRecallMessageMap;
};