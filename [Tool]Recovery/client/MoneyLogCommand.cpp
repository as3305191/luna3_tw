#include "stdafx.h"
#include "MoneyLogCommand.h"
#include "client.h"

CMoneyLogCommand::CMoneyLogCommand( CclientApp& application, const TCHAR* title, const CMoneyLogCommand::Configuration& configuration ) :
CCommand( application, title ),
mConfiguration( configuration )
{}

void CMoneyLogCommand::SaveToExcel( DWORD serverIndex, const CListCtrl& listCtrl ) const
{
	CString textHead;
	textHead.LoadString( IDS_STRING20 );
	CString textLog;
	textLog.LoadString( IDS_STRING10 );

	CclientApp::SaveToExcel(
		textHead + textLog + _T( "-" ) + mApplication.GetServerName( serverIndex ),
		textLog,
		listCtrl );
}

void CMoneyLogCommand::Initialize( CListCtrl& listCtrl ) const
{
	CRect rect;
	int	step = -1;
	listCtrl.GetClientRect( rect );

	listCtrl.InsertColumn( ++step, L"log index", LVCFMT_RIGHT, 0 );
	CString textDate;
	textDate.LoadString( IDS_STRING3 );
	listCtrl.InsertColumn( ++step, textDate, LVCFMT_LEFT, int( rect.Width() * 0.25f ) );
	CString textType;
	textType.LoadString( IDS_STRING4 );
	listCtrl.InsertColumn( ++step, textType, LVCFMT_LEFT, int( rect.Width() * 0.1f ) );
	CString textMarginMoney;
	textMarginMoney.LoadString( IDS_STRING124 );
	listCtrl.InsertColumn( ++step, textMarginMoney, LVCFMT_RIGHT, int( rect.Width() * 0.2f ) );
	CString textSentPlayerIndex;
	textSentPlayerIndex.LoadString( IDS_STRING119 );
	listCtrl.InsertColumn( ++step, textSentPlayerIndex, LVCFMT_RIGHT, int( rect.Width() * 0.3f ) );
	CString textSentPlayerMoney;
	textSentPlayerMoney.LoadString( IDS_STRING120 );
	listCtrl.InsertColumn( ++step, textSentPlayerMoney, LVCFMT_RIGHT, int( rect.Width() * 0.4f ) );
	CString textReceivedPlayerIndex;
	textReceivedPlayerIndex.LoadString( IDS_STRING122 );
	listCtrl.InsertColumn( ++step, textReceivedPlayerIndex, LVCFMT_RIGHT, int( rect.Width() * 0.3f ) );
	CString textReceivedPlayerMoney;
	textReceivedPlayerMoney.LoadString( IDS_STRING123 );
	listCtrl.InsertColumn( ++step, textReceivedPlayerMoney, LVCFMT_RIGHT, int( rect.Width() * 0.4f ) );
}

void CMoneyLogCommand::Find( DWORD serverIndex, const TCHAR* beginTime, const TCHAR* endTime )
{
	MSG_RM_LOG_REQUEST message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category = MP_RM_MONEY_LOG;
	message.Protocol = MP_RM_MONEY_LOG_SIZE_SYN;
	message.mRequestTick = mTickCount = GetTickCount();
	message.dwObjectID = mConfiguration.mPlayerIndex;
	strncpy( message.mBeginDate, CW2AEX< MAX_PATH >( beginTime ), sizeof( message.mBeginDate ) );
	strncpy( message.mEndDate, CW2AEX< MAX_PATH >( endTime ), sizeof( message.mEndDate ) );
	mApplication.Send( serverIndex, message, sizeof( message ) );
}

void CMoneyLogCommand::Stop( DWORD serverIndex ) const
{
	MSGROOT message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category = MP_RM_MONEY_LOG;
	message.Protocol = MP_RM_MONEY_LOG_STOP_SYN;
	mApplication.Send( serverIndex, message, sizeof( message ) );
}

void CMoneyLogCommand::Parse( const MSGROOT* receivedMessage, CListCtrl& listCtrl, CProgressCtrl& progressCtrl, CStatic& resultStatic, CButton& findButton, CButton& stopButton ) const
{
	switch( receivedMessage->Protocol )
	{
	case MP_RM_MONEY_LOG_ACK:
		{
			const MSG_RM_MONEY_LOG* const message = ( MSG_RM_MONEY_LOG* )receivedMessage;

			if( message->mRequestTick != mTickCount )
			{
				break;
			}

			// 080401 LUJ, ���� �α׿� ���� �ε��� ��Ʈ�� ����� �ߺ����� �ʵ��� üũ�Ѵ�
			std::set< DWORD > indexSet;
			{
				for( int row = 0; row < listCtrl.GetItemCount(); ++row )
				{
					indexSet.insert( _ttoi( listCtrl.GetItemText( row, 0 ) ) );
				}
			}

			CString textLogRegular;
			textLogRegular.LoadString( IDS_STRING256 );

			for( DWORD i = 0; i < message->mSize; ++i )
			{
				const MSG_RM_MONEY_LOG::Log& data = message->mLog[ i ];
				const DWORD row	= listCtrl.GetItemCount();
				int	step = 0;

				if( indexSet.end() != indexSet.find( data.mIndex ) )
				{
					continue;
				}

				CString text;
				text.Format( _T( "%d" ), data.mIndex );
				listCtrl.InsertItem( row, text, 0 );
				listCtrl.SetItemText( row, ++step, CA2WEX< sizeof( data.mDate ) >( data.mDate ) );
				listCtrl.SetItemText( row, ++step, eLog_ItemNone == data.mType ? textLogRegular : mApplication.GetText( data.mType ) );

				listCtrl.SetItemText(
					row,
					++step,
					mApplication.ConvertToText(data.mMarginMoney));

				text.Format( _T( "%d" ), data.mSourcePlayer.mIndex );
				listCtrl.SetItemText( row, ++step, text );

				listCtrl.SetItemText(
					row,
					++step,
					mApplication.ConvertToText(data.mSourcePlayer.mInventoryMoney));

				text.Format( _T( "%d" ), data.mTargetPlayer.mIndex );
				listCtrl.SetItemText( row, ++step, text );

				listCtrl.SetItemText( row, ++step, 
					mApplication.ConvertToText(data.mTargetPlayer.mInventoryMoney));
			}

			{
				int minRange;
				int maxRange;
				progressCtrl.GetRange( minRange, maxRange );

				progressCtrl.SetPos( progressCtrl.GetPos() + int( message->mSize ) );

				CString text;
				text.Format( _T( "%d/%d" ), progressCtrl.GetPos(), maxRange );
				resultStatic.SetWindowText( text );

				// 080523 LUJ, ��ư Ȱ��ȭ üũ�� ���������� �ǵ��� ������
				if( progressCtrl.GetPos() == maxRange )
				{
					findButton.EnableWindow( TRUE );
					stopButton.EnableWindow( FALSE );
				}
			}

			break;
		}
	case MP_RM_MONEY_LOG_SIZE_ACK:
		{
			const MSG_DWORD* const message = ( MSG_DWORD* )receivedMessage;
			const DWORD size = message->dwData;

			CString text;
			text.Format( _T( "0/%d" ), size );

			resultStatic.SetWindowText( text );
			progressCtrl.SetRange32( 0, size );
			progressCtrl.SetPos( 0 );
			findButton.EnableWindow( FALSE );
			stopButton.EnableWindow( TRUE );

			listCtrl.DeleteAllItems();
			break;
		}
	case MP_RM_MONEY_LOG_SIZE_NACK:
		{
			CString textThereIsNoResult;
			textThereIsNoResult.LoadString( IDS_STRING1 );

			MessageBox( 0, textThereIsNoResult, _T( "" ), MB_ICONERROR | MB_OK );
			break;
		}
	case MP_RM_MONEY_LOG_STOP_ACK:
		{
			findButton.EnableWindow( TRUE );
			stopButton.EnableWindow( FALSE );

			CString textSearchWasStopped;
			textSearchWasStopped.LoadString( IDS_STRING2 );

			MessageBox( 0, textSearchWasStopped, _T( "" ), MB_ICONERROR | MB_OK );
			break;
		}
	case MP_RM_MONEY_LOG_NACK_BY_AUTH:
		{
			CString textYouHaveNoAuthority;
			textYouHaveNoAuthority.LoadString( IDS_STRING18 );

			MessageBox( 0, textYouHaveNoAuthority, _T( "" ), MB_OK | MB_ICONERROR );
			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}
}
