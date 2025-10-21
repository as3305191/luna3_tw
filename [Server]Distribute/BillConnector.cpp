// ������ ���� ������ ��ũ ������ �߻��Ѵ�
#define BUILDING_LIBCURL

#include "StdAfx.h"
#include "BillConnector.h"
#include "Network.h"
#include "ServerTable.h"
#include "ServerListManager.h"
#include "DistributeDBMsgParser.h"
#include <WinSock2.h>
#include "..\[CC]Header\curl\curl.h"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "wldap32.lib")

#define pgGame_Start	0x00000001	// ���� ����
#define pgGame_End		0x00000002	// ���� ����
#define pgServer_Alive	0x00000003	// ���� Ȯ��
#define pgServer_Reset	0x00000004	// ���� �ʱ�ȭ
#define pgUser_Alert	0x00000005	// ����� �˸�
#define pgUser_Sync		0x00000006	// ����� Ȯ��
#define pgBilling_Auth	0x00000008	// ��������. ��� ���ʿ�: ���� �������� 0x00000001�� ���� �ȴٰ� ��(�ֵ�� ����Ʈ, �纴�� ����)
#define pgClient_Move	0x0000000A	// ����� ���Ӽ��� �̵�
#define pgServer_Conn	0x0000000C	// ��������

extern int g_nServerSetNum;

// curl/docs/examples/sendrecv.c
int wait_on_socket(SOCKET sockfd, int for_recv, long timeout_ms)
{
	struct timeval tv;
	fd_set infd, outfd, errfd;
	int res;

	tv.tv_sec = timeout_ms / 1000;
	tv.tv_usec= (timeout_ms % 1000) * 1000;

	FD_ZERO(&infd);
	FD_ZERO(&outfd);
	FD_ZERO(&errfd);

	FD_SET(sockfd, &errfd); /* always check for error */

	if(for_recv)
	{
		FD_SET(sockfd, &infd);
	}
	else
	{
		FD_SET(sockfd, &outfd);
	}

	/* select() returns the number of signalled sockets or -1 */
	res = select(sockfd + 1, &infd, &outfd, &errfd, &tv);
	return res;
}

CBillConnector::CBillConnector() :
mCurl(curl_easy_init()),
mSocket(0),
mLastConnectTick(0),
mConnectTryCount(0),
mCompanyCode("EYAP"),
mGameNumber(3) // �糪 ���� ��ȣ. ���� �ַ�� ������(�ֵ�� ����Ʈ)�κ��� �Ҵ���� ��ȣ.
{
	ZeroMemory(
		mGameServerName,
		sizeof(mGameServerName));
}

CBillConnector::~CBillConnector(void)
{
	curl_easy_cleanup(
		mCurl);
}

void CBillConnector::Connect(DWORD serverIndex)
{
	// ���� ���̹Ƿ� ��õ� ���� �ʴ´�
	if(mSocket)
	{
		return;
	}
	else if(0 == mCurl)
	{
		SetInvalid();
		return;
	}

	char uniformResourceLocator[MAX_PATH] = {0};
	sprintf(
		uniformResourceLocator,
		"%s:%u",
		SERVERLISTMGR->GetBillingInfo().szIPForServer,
		SERVERLISTMGR->GetBillingInfo().wPortForServer);

	char errorBuffer[MAX_PATH] = {0};
	curl_easy_setopt(
		mCurl,
		CURLOPT_ERRORBUFFER,
		errorBuffer);
	curl_easy_setopt(
		mCurl,
		CURLOPT_URL,
		uniformResourceLocator);
	curl_easy_setopt(
		mCurl,
		CURLOPT_CONNECT_ONLY,
		1); 

	CURLcode curlCode = curl_easy_perform(
		mCurl);

	if(CURLE_OK != curlCode)
	{
		return;
	}

	curlCode = curl_easy_getinfo(
		mCurl,
		CURLINFO_LASTSOCKET,
		&mSocket);

	if(FAILED(mSocket))
	{
		return;
	}

	g_Console.LOG(
		0,
		"Connected to the billing server: %s",
		uniformResourceLocator);

	while(false == mSendPacketQueue.empty())
	{
		mSendPacketQueue.pop();
	}

	mUserCheckContainer.clear();

	_stprintf(
		mGameServerName,
		"%s%02d",
		"LUNA",
		g_nServerSetNum);

	{
		_PACKET_GAME packetGame = {0};
		packetGame.Packet_Type = pgServer_Conn;		
		Send(
			packetGame);
	}

	{
		_PACKET_GAME packetGame = {0};
		packetGame.Packet_Type = pgServer_Reset;		
		Send(
			packetGame);
	}

	OutputDebug(
		"Billing: auto authorization process: %u case",
		mUserPacketContainer.size());

	// ���� �����ڰ� ���� ��� ������ ��û�� �Ѵ�
	while(false == mUserPacketContainer.empty())
	{
		const UserPacketContainer::iterator iterator = mUserPacketContainer.begin();
		const _PACKET_GAME& packetGame = iterator->second;

		GameStart(
			_ttoi(packetGame.User_ID),
			packetGame.User_IP);

		mUserPacketContainer.erase(
			iterator);
	}
}

void CBillConnector::PacketResultAuth(const _PACKET_GAME& packetGame) const
{
	switch(packetGame.Packet_Result)
	{
	case 0:		// ��������
		OutputDebug(
			"Billing: Packet_Result(%d)-Billing Success. remained seconds: %u. user: %s",
			packetGame.Packet_Result,
			packetGame.Bill_Remain,
			packetGame.User_ID);
		break;
	case 2:		// DB�� ���� �� �� ����.
		OutputDebug(
			"Billing: Packet_Result(%d)-Billing Error. user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	case 3:		// ��Ʈ��ũ�� ���� �߻�.
		OutputDebug(
			"Billing: Packet_Result(%d)-Network Error has occurred. user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	case 10:	// �������� ����. ���� ������ DB�� ����.
		OutputDebug(
			"Billing: Packet_Result(%d)-You have insufficient points to play. Please top up.. user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	case 11:	// �ܾ� ����.
		OutputDebug(
			"Billing: Packet_Result(%d)-You have insufficient points to play. Please top up.. user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	case 12:	// ������ ����.
		OutputDebug(
			"Billing: Packet_Result(%d)-Billing Error. user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	case 17:	// �ߺ����� �����.
		OutputDebug(
			"Billing: Packet_Result(%d)-You have already connected another game.. user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	case 18:	// �ҹ� �����.
		OutputDebug(
			"Billing: Packet_Result(%d)-You are illegal user.. user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	case 20:	// ���� ���� ���� ��Ŷ.
		OutputDebug(
			"Billing: Packet_Result(%d)-Billing Error. user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	case 25:	// COM ����
		OutputDebug(
			"Billing: Packet_Result(%d)-Billing Error. user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	case 26:	// ������ �ð�����(30��) ������ ����.
		OutputDebug(
			"Billing: Packet_Result(%d)-Billing Error. user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	case 27:	// ���� DB�������� Connect ����.
		OutputDebug(
			"Billing: Packet_Result(%d)-Billing Error. user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	case 30:	// ��Ŷ�� �ִ� ���� ��ȣ�� ���� ������ ��ϵ� ���� ��ȣ�� �ٸ�.
		OutputDebug(
			"Billing: Packet_Result(%d)-Billing Error. user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	case 31:	// ���� ��ī��Ʈ�� ���� ��� ����.
		OutputDebug(
			"Billing: Packet_Result(%d)-Billing Error. user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	case 33:	// �ش������ �������� ����.
		OutputDebug(
			"Billing: Packet_Result(%d)-You have insufficient points to play. Please top up.. user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	case 35:	// ������ ���� ó�� �䱸�� ���� ������ ���� ó�� �Ұ�.
		OutputDebug(
			"Billing: Packet_Result(%d)-Billing Error. user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	case 100:	// ���� ���� ���� ����.
		OutputDebug(
			"Billing: Packet_Result(%d)-Billing Error. user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	case 200:	// IP�� Ȯ�� �ϴ� �� COM ������ �߻�.
		OutputDebug(
			"Billing: Packet_Result(%d)-Billing Error. user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	case 201:	// �ش� IP�� ������ �������� ������ ����.
		OutputDebug(
			"Billing: Packet_Result(%d)-Billing Error. user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	case 210:	// ���� Ű�� Ȯ���ϴ� �� COM ������ �߻�.
		OutputDebug(
			"Billing: Packet_Result(%d)-Billing Error. user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	case 211:	// ���� Ű�� ã�� ����.
		OutputDebug(
			"Billing: Packet_Result(%d)-Billing Error. user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	case 301:	// ���� �̿���
		OutputDebug(
			"Billing: Packet_Result(%d)-Free user. user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	case 401:	// ���� ���� �޽����� ���޵��� ���� ���¿��� �ٸ� ó���� ��û�Ǿ���
		OutputDebug(
			"Billing: Packet_Result(%d)-Billing Error. user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	default:	// �˼����� ����
		OutputDebug(
			"Billing: Packet_Result(%d)-Unknown Error. user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	}
}

void CBillConnector::PacketResultAlert(const _PACKET_GAME& packetGame) const
{
	switch(packetGame.Packet_Result)
	{
	case 1:		// ������� ���� �ð�(�� ����)
		OutputDebug(
			"Billing: remain seconds: %u. user: %s",
			packetGame.Bill_Remain);
		break;
	case 0:		// �����ð� ����
		OutputDebug(
			"Billing: game stop. user: %s",
			packetGame.User_ID);
		break;
	case -1:	// ȯ��
		OutputDebug(
			"Billing:Packet_Result(%d). user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	case -2:	// �ߺ� �α���
		OutputDebug(
			"Billing:Packet_Result(%d)-You are disconnected due to double login from other PC. user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	case -3:	// ����Ǿ� �ִ� ���� ���� ��� ����
		OutputDebug(
			"Billing:Packet_Result(%d). user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	case -4:	// ��Ÿ ���� ����
		OutputDebug(
			"Billing:Packet_Result(%d). user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	default:	// �˼����� ����
		OutputDebug(
			"Billing:Packet_Result(%d)-Unknown Error. user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	}
}

void CBillConnector::Receive(const _PACKET_GAME& packetGame)
{
	switch (packetGame.Packet_Type)
	{
		/*-- ����
		����ڰ� ���� ������ �����Ͽ��� �� ������ �����ص� �Ǵ��� ���������� Ȯ��
		���� ����� �����̸� ������������ ��뷮 ����� ���� (���� �̺�Ʈ����)
		*/
		/*-- ���� ���� �۾�
		������� �����̸� ������ ����, ������ ��� Ŭ���̾�Ʈ���� ����� ������ ���� ����
		*/
	case pgGame_Start:
		{
			PacketResultAuth(
				packetGame);

			const UserIndex userIndex = _ttoi(
				packetGame.User_ID);
            const GameRoomType gameRoomType = GetGameRoomType(
				packetGame);

			if(GameRoomIsInvalid == gameRoomType)
			{
				mUserCheckContainer.erase(
					userIndex);

				// �����ߴ� ������ ������ ������ �ٸ� IP�� �����ص� ������ �ް� �ȴ�
				MSGBASE message;
				ZeroMemory(
					&message,
					sizeof(message));
				message.Category = MP_USERCONN;
				message.Protocol = MP_USERCONN_BILLING_START_NACK;
				message.dwObjectID = userIndex;

				g_Network.Broadcast2AgentServer(
					(char*)&message,
					sizeof(message));
				break;
			}

			OutputDebug(
				"User(%u) logged in at %s game room(%u)",
				userIndex,
				GameRoomIsPremium == gameRoomType ? "premium" : "free",
				packetGame.User_Age);

			mUserPacketContainer[userIndex] = packetGame;

			MSG_PACKET_GAME message;
			ZeroMemory(
				&message,
				sizeof(message));
			message.Category = MP_USERCONN;
			message.Protocol = MP_USERCONN_BILLING_START_ACK;
			message.dwObjectID = userIndex;
			message.mPacketGame = packetGame;

			g_Network.Broadcast2AgentServer(
				(char*)&message,
				sizeof(message));
			break;
		}
	case pgServer_Alive:
		{
			_PACKET_GAME sendMsg = {0};
			sendMsg.Packet_Type = pgServer_Alive;
			Send(
				sendMsg);

			OutputDebug(
				"RT->ACCT::Packet_Type=Server_Alive\n");
			break;
		}
		/*-- ����
		Bill_Method = FM/FW/FD/TX �� ��� �������� 24�ð� ������ �� 1�ð����� ���� ���� 1�ð� ������ �� 10�и��� ����, ���� 10�������� �� 1�и��� ����
		Bill_Method = DB/HB �� ��� ������ ����Ǳ� 5�������� 1�п� �ѹ��� ����
		���� ���Ÿ� �Ͽ��� �޼�¡�� ��ӵǸ�, ���౸�� ���θ� Ȯ���� �������� ���� ���ŵ� ���������� �޼�¡
		*/
		/*-- ���� ���� �۾�
		������������ ���� ������ �������� ����ڿ��� �˸�
		���Ӽ������� Packet_Result�� 1�� -3�� �ƴ� ��� ����ڸ� ��������(0x00000002 ���� ��Ŷ ���� ����)
		*/
	case pgUser_Alert:			// ��������
		{
			PacketResultAlert(
				packetGame);

			MSG_PACKET_GAME message;
			ZeroMemory(
				&message,
				sizeof(message));
			message.Category = MP_USERCONN;
			message.Protocol = MP_USERCONN_BILLING_ALERT_ACK;
			message.mPacketGame = packetGame;
			message.dwObjectID = _ttoi(
				packetGame.User_ID);

			g_Network.Broadcast2AgentServer(
				(char*)&message,
				sizeof(message));
			break;
		}
		/*-- ����
		��Ʈ��ũ ��ֿ� ���� ������ �������� �̺�Ʈ�� ���� �ý����� ó�� ���� ���Ѱ�� ����ڰ� ������ ���� ���������� Ȯ��
		���� ���ӵǾ� �ִ� ����� ���� Ȯ��
		���������� ����ڰ� "OFF" �̸� ���� �ð��� �������� ��������
		����ð��� ���� ���ϸ� �˻� �ֱ⸸ŭ �ð��� ���� ���
		*/
		/*-- ���� ���� �۾�
		���� ���ǰ��� ���� ����ڰ� �ִ��� Ȯ���ؼ�, ��� ����
		���� ���ǰ��� ����ڰ� ������ Packet_Result�� 1, ������ 0 ��ȯ
		*/
	case pgUser_Sync:
		{
			if(0 != packetGame.Packet_Result)
			{
				break;
			}

			const UserIndex userIndex = _ttoi(
				packetGame.User_ID);

			// �̹� ������ Ȯ�� �߿� �ִ�
			if(mUserCheckContainer.end() != mUserCheckContainer.find(userIndex))
			{
				break;
			}

			const AgentSize agentSize = g_pServerTable->GetDataNum();
			
			mUserCheckContainer.insert(
				std::make_pair(userIndex, agentSize));

			MSGBASE message;
			ZeroMemory(
				&message,
				sizeof(message));
			message.Category = MP_USERCONN;
			message.Protocol = MP_USERCONN_BILLING_CHECK_SYN;
			message.dwObjectID = userIndex;

			g_Network.Send2AgentServer(
				(char*)&message,
				sizeof(message));

			OutputDebug(
				"RT->ACCT::Packet_Type=User_Sync(%s, %s)\n",
				packetGame.User_ID,
				packetGame.User_Status);
			break;
		}
	default:
		{
			OutputDebug(
				"RT->ACCT::Packet_Type=Unknown(%d)\n",
				packetGame.Packet_Type);
			break;
		}        
	}
}

// GameStart Method - ���� ���� �� ���� ���� ������ �˸�.
void CBillConnector::GameStart(DWORD userIndex, LPCTSTR User_IP)
{
	if(0 == _tcsicmp(SERVERLISTMGR->GetBillingInfo().szIPForServer, "database"))
	{
		g_DB.LoginMiddleQuery(
			RGameRoomData,
			userIndex,
			"EXEC dbo.MP_GAMEROOM_SELECT \'%s\'",
			User_IP);
		return;
	}
	else if(IsInvalid())
	{
		return;
	}

	_PACKET_GAME packetGame = {0};
	ZeroMemory(
		&packetGame,
		sizeof(packetGame));
	packetGame.Packet_Type = pgGame_Start;

	SetGUID(
		packetGame);
	_stprintf(
		packetGame.User_ID,
		"%d",
		userIndex);
	SafeStrCpy(
		packetGame.User_IP,
		User_IP,
		_countof(packetGame.User_IP));
	
	Send(
		packetGame);
}

// GameEnd Method - ���� ���� �� ���� ���� ��û ����.
void CBillConnector::GameEnd(DWORD userIndex)
{
	if(mUserPacketContainer.end() == mUserPacketContainer.find(userIndex))
	{
		return;
	}

	_PACKET_GAME packet = mUserPacketContainer[userIndex];
	packet.Packet_Type = pgGame_End;
	Send(
		packet);

	mUserPacketContainer.erase(
		userIndex);
	mUserCheckContainer.erase(
		userIndex);
}

void CBillConnector::Send(_PACKET_GAME& packetGame)
{
	memcpy(
		packetGame.User_CC,
		mCompanyCode,
		_tcslen(mCompanyCode));
	SafeStrCpy(
		packetGame.Game_Server,
		mGameServerName,
		_countof(packetGame.Game_Server));

	{
		TCHAR uniqueIndex[MAX_PATH] = {0};
		memcpy(
			uniqueIndex,
			packetGame.Session,
			32);
	}

	packetGame.Packet_Type = htonl(packetGame.Packet_Type);
	packetGame.Packet_Result = htonl(packetGame.Packet_Result);
	packetGame.User_Age = htonl(packetGame.User_Age);
	packetGame.Game_No = htonl(mGameNumber);
	packetGame.Bill_Remain = htonl(packetGame.Bill_Remain);

	mSendPacketQueue.push(
		packetGame);
}

const _PACKET_GAME& CBillConnector::GetPacket(UserIndex userIndex) const
{
	static const _PACKET_GAME emptyPacket = {0};
	const UserPacketContainer::const_iterator iterator = mUserPacketContainer.find(
		userIndex);	

	return mUserPacketContainer.end() == iterator ? emptyPacket : iterator->second;
}

void CBillConnector::Process()
{
	if(IsInvalid())
	{
		return;
	}
	else if(0 == mSocket)
	{
		if(gCurTime < mLastConnectTick)
		{
			return;
		}

		Connect(
			g_nServerSetNum);

		if(0 == mSocket)
		{
			OutputDebug(
				"Billing server don't response(%d)",
				mConnectTryCount);
		}

		const DWORD stepTick = 5000;
		mLastConnectTick = gCurTime + stepTick;
		const DWORD maxTryCount = 10;

		if(maxTryCount < ++mConnectTryCount)
		{
			OutputDebug(
				"Distribute server don't try to connect any more");
			SetInvalid();
		}

		return;
	}
	else if(false == mSendPacketQueue.empty())
	{
		if(false == wait_on_socket(mSocket, 0, 1))
		{
			return;
		}

		_PACKET_GAME& packet = mSendPacketQueue.front();
		size_t sendLength = 0;

		const CURLcode curlCode = curl_easy_send(
			mCurl,
			LPVOID(&packet),
			sizeof(packet),
			&sendLength);

		if(CURLE_OK != curlCode)
		{
			mSocket = 0;

			g_Console.LOG(
				0,
				"Billing Server is disconnected");
			return;
		}

		mSendPacketQueue.pop();
	}

	wait_on_socket(
		mSocket,
		1,
		1);

	char buffer[1024] = {0};
	size_t receiveLength = 0;
	const CURLcode curlCode = curl_easy_recv(
		mCurl,
		buffer,
		_countof(buffer), 
		&receiveLength);

	if(CURLE_OK != curlCode)
	{
		return;
	}

	_PACKET_GAME* const packetGame = (_PACKET_GAME*)buffer;
	packetGame->Packet_Type = ntohl(packetGame->Packet_Type);
	packetGame->Packet_Result = ntohl(packetGame->Packet_Result);
	packetGame->User_Age = ntohl(packetGame->User_Age);
	packetGame->Game_No = ntohl(packetGame->Game_No);
	packetGame->Bill_Remain = ntohl(packetGame->Bill_Remain);

	Receive(
		*packetGame);
}

void CBillConnector::SucceedToCheck(UserIndex userIndex)
{
	if(mUserCheckContainer.end() == mUserCheckContainer.find(userIndex))
	{
		return;
	}

	mUserCheckContainer.erase(
		userIndex);

	const int isExist = 1;

	_PACKET_GAME packetGame = GetPacket(
		userIndex);
	packetGame.Packet_Type = pgUser_Sync;
	packetGame.Packet_Result = isExist;
	
	Send(
		packetGame);
}

void CBillConnector::FailToCheck(UserIndex userIndex)
{
	if(mUserCheckContainer.end() == mUserCheckContainer.find(userIndex))
	{
		return;
	}

	AgentSize& agentSize = mUserCheckContainer[userIndex];

	// 0 ���� ���� �����̴�
	if(0 == agentSize--)
	{
		mUserCheckContainer.erase(
			userIndex);
		return;
	}
	// �ٸ� ������Ʈ�κ��� ������ ��ٷ����Ѵ�.
	// ������Ʈ�� �ٿ�� ��� üũ�� �ٽ� �ϵ��� �����̳ʿ� �� ���� �ϰ� 1�� ���δ�
	else if(0 < agentSize)
	{
		return;
	}

	mUserCheckContainer.erase(
		userIndex);

	const int isNotExist = 0;

	_PACKET_GAME packetGame = GetPacket(
		userIndex);
	packetGame.Packet_Type = pgUser_Sync;
	packetGame.Packet_Result = isNotExist;

	Send(
		packetGame);
}

void CBillConnector::SetGUID(_PACKET_GAME& packetGame) const
{
	char uniqueIndex[MAX_PATH] = {0};
	GUID guid;
	CoCreateGuid(
		&guid);
	sprintf(
		uniqueIndex,
		"%04X%04X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X",
		HIWORD(guid.Data1),
		LOWORD(guid.Data1),
		guid.Data2,
		guid.Data3,
		guid.Data4[0],
		guid.Data4[1],
		guid.Data4[2],
		guid.Data4[3],
		guid.Data4[4],
		guid.Data4[5],
		guid.Data4[6],
		guid.Data4[7]);
	memcpy(
		packetGame.Session,
		uniqueIndex,
		_countof(packetGame.Session));
}

void CBillConnector::Reset()
{
	mConnectTryCount = 0;
	mSocket = 0;
	mLastConnectTick = 0;
}