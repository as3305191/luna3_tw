#include "stdafx.h"
#include "ItemDropOptionLogCommand.h"
#include "client.h"


CItemDropOptionLogCommand::CItemDropOptionLogCommand( CclientApp& application, const TCHAR* title, DWORD itemDbIndex ) :
CCommand( application, title ),
mItemDbIndex( itemDbIndex )
{}


void CItemDropOptionLogCommand::SaveToExcel( DWORD serverIndex, const CListCtrl& listCtrl ) const
{
	CString textHead;
	textHead.LoadString( IDS_STRING354 );

	CString textLog;
	textLog.LoadString( IDS_STRING10 );

	CclientApp::SaveToExcel(
		textHead + textLog + _T( "-" ) + mApplication.GetServerName( serverIndex ),
		textLog,
		listCtrl );
}


void CItemDropOptionLogCommand::Initialize( CListCtrl& listCtrl ) const
{
	CRect	rect;
	int		step = -1;

	listCtrl.GetClientRect( rect );

	listCtrl.InsertColumn( ++step,  _T( "log index" ), LVCFMT_LEFT, 0 );

	CString textDate;
	textDate.LoadString( IDS_STRING3 );
	listCtrl.InsertColumn( ++step, textDate, LVCFMT_LEFT, int( rect.Width() * 0.15f ) );

	CString textType;
	textType.LoadString( IDS_STRING4 );
	listCtrl.InsertColumn( ++step, textType, LVCFMT_LEFT, int( rect.Width() * 0.15f ) );

	CString textItemDbIndex;
	textItemDbIndex.LoadString( IDS_STRING13 );
	listCtrl.InsertColumn( ++step, textItemDbIndex, LVCFMT_RIGHT, int( rect.Width() * 0.15f ) );

	CString textKey;
	textKey.LoadString( IDS_STRING151 );
	CString textValue;
	textValue.LoadString( IDS_STRING152 );

	listCtrl.InsertColumn( ++step, textKey		+ _T( " 1" ), LVCFMT_LEFT, int( rect.Width() * 0.15f ) );
	listCtrl.InsertColumn( ++step, textValue	+ _T( " 1" ), LVCFMT_RIGHT, int( rect.Width() * 0.15f ) );
	listCtrl.InsertColumn( ++step, textKey		+ _T( " 2" ), LVCFMT_LEFT, int( rect.Width() * 0.15f ) );
	listCtrl.InsertColumn( ++step, textValue	+ _T( " 2" ), LVCFMT_RIGHT, int( rect.Width() * 0.15f ) );
	listCtrl.InsertColumn( ++step, textKey		+ _T( " 3" ), LVCFMT_LEFT, int( rect.Width() * 0.15f ) );
	listCtrl.InsertColumn( ++step, textValue	+ _T( " 3" ), LVCFMT_RIGHT, int( rect.Width() * 0.15f ) );
	listCtrl.InsertColumn( ++step, textKey		+ _T( " 4" ), LVCFMT_LEFT, int( rect.Width() * 0.15f ) );
	listCtrl.InsertColumn( ++step, textValue	+ _T( " 4" ), LVCFMT_RIGHT, int( rect.Width() * 0.15f ) );
	listCtrl.InsertColumn( ++step, textKey		+ _T( " 5" ), LVCFMT_LEFT, int( rect.Width() * 0.15f ) );
	listCtrl.InsertColumn( ++step, textValue	+ _T( " 5" ), LVCFMT_RIGHT, int( rect.Width() * 0.15f ) );

	CString textMemo;
	textMemo.LoadString( IDS_STRING17 );
	listCtrl.InsertColumn( ++step, textMemo, LVCFMT_LEFT, int( rect.Width() * 0.15f ) );
}


void CItemDropOptionLogCommand::Find( DWORD serverIndex, const TCHAR* beginTime, const TCHAR* endTime )
{
	MSG_RM_LOG_REQUEST message;
	ZeroMemory( &message, sizeof( message ) );

	message.Category		= MP_RM_ITEM_DROP_OPTION_LOG;
	message.Protocol		= MP_RM_ITEM_DROP_OPTION_LOG_SIZE_SYN;
	message.mRequestTick	= mTickCount = GetTickCount();
	message.dwObjectID		= mItemDbIndex;

	strncpy( message.mBeginDate,	CW2AEX< MAX_PATH >( beginTime ),	sizeof( message.mBeginDate ) );
	strncpy( message.mEndDate,		CW2AEX< MAX_PATH >( endTime ),		sizeof( message.mEndDate ) );

	mApplication.Send( serverIndex, message, sizeof( message ) );
}


void CItemDropOptionLogCommand::Stop( DWORD serverIndex ) const
{
	MSGROOT message;
	ZeroMemory( &message, sizeof( message ) );

	message.Category	= MP_RM_ITEM_DROP_OPTION_LOG;
	message.Protocol	= MP_RM_ITEM_DROP_OPTION_LOG_STOP_SYN;

	mApplication.Send( serverIndex, message, sizeof( message ) );
}


void CItemDropOptionLogCommand::Parse( const MSGROOT* message, CListCtrl& listCtrl, CProgressCtrl& progressCtrl, CStatic& resultStatic, CButton& findButton, CButton& stopButton ) const
{
	switch( message->Protocol )
	{
	case MP_RM_ITEM_DROP_OPTION_LOG_GET_ACK:
		{
			const MSG_RM_ITEM_OPTION_LOG* m = ( MSG_RM_ITEM_OPTION_LOG* )message;

			// 080401 LUJ, ���� �α׿� ���� �ε��� ��Ʈ�� ����� �ߺ����� �ʵ��� üũ�Ѵ�
			std::set< DWORD > indexSet;
			{
				for( int row = 0; row < listCtrl.GetItemCount(); ++row )
				{
					indexSet.insert( _ttoi( listCtrl.GetItemText( row, 0 ) ) );
				}
			}

			for( DWORD i = 0; i < m->mSize; ++i )
			{
				const MSG_RM_ITEM_OPTION_LOG::Log& data = m->mLog[ i ];

				const DWORD row		= listCtrl.GetItemCount();
				int			step	= 0;

				if( indexSet.end() != indexSet.find( data.mLogIndex ) )
				{
					continue;
				}

				CString text;
				text.Format( _T( "%d" ), data.mLogIndex );
				listCtrl.InsertItem( row, text, 0 );

				listCtrl.SetItemText( row, ++step, CA2WEX< sizeof( data.mDate ) >( data.mDate ) );

				listCtrl.SetItemText( row, ++step, mApplication.GetText( data.mType ) );

				text.Format( _T( "%d" ), data.mItemDbIndex );
				listCtrl.SetItemText( row, ++step, text );

				{
					const ITEM_OPTION::Drop& drop = data.mOption.mDrop;

					text.Format( _T( "%d" ), drop.mValue[ 0 ].mKey );
					listCtrl.SetItemText( row, ++step, text );

					text.Format( _T( "%0.3f" ), drop.mValue[ 0 ].mValue );
					listCtrl.SetItemText( row, ++step, text );

					text.Format( _T( "%d" ), drop.mValue[ 1 ].mKey );
					listCtrl.SetItemText( row, ++step, text );

					text.Format( _T( "%0.3f" ), drop.mValue[ 1 ].mValue );
					listCtrl.SetItemText( row, ++step, text );

					text.Format( _T( "%d" ), drop.mValue[ 2 ].mKey );
					listCtrl.SetItemText( row, ++step, text );

					text.Format( _T( "%0.3f" ), drop.mValue[ 2 ].mValue );
					listCtrl.SetItemText( row, ++step, text );

					text.Format( _T( "%d" ), drop.mValue[ 3 ].mKey );
					listCtrl.SetItemText( row, ++step, text );

					text.Format( _T( "%0.3f" ), drop.mValue[ 3 ].mValue );
					listCtrl.SetItemText( row, ++step, text );

					text.Format( _T( "%d" ), drop.mValue[ 4 ].mKey );
					listCtrl.SetItemText( row, ++step, text );

					text.Format( _T( "%0.3f" ), drop.mValue[ 4 ].mValue );
					listCtrl.SetItemText( row, ++step, text );
				}

				listCtrl.SetItemText( row, ++step, CA2WEX< sizeof( data.mMemo ) >( data.mMemo ) );
			}

			{
				int minRange;
				int maxRange;
				progressCtrl.GetRange( minRange, maxRange );

				progressCtrl.SetPos( progressCtrl.GetPos() + int( m->mSize ) );

				CString text;
				text.Format( _T( "%d/%d" ), progressCtrl.GetPos(), maxRange );
				resultStatic.SetWindowText( text );

				// 080523 LUJ, ��ư Ȱ��ȭ üũ�� ���������� �ǵ��� ������
				if( progressCtrl.GetPos() == maxRange )
				{
					findButton.EnableWindow( TRUE );
					findButton.EnableWindow( FALSE );
				}
			}

			break;
		}
	case MP_RM_ITEM_DROP_OPTION_LOG_SIZE_ACK:
		{
			const MSG_DWORD* m = ( MSG_DWORD* )message;

			const DWORD size = m->dwData;

			CString text;
			text.Format( _T( "0/%d" ), size );

			resultStatic.SetWindowText( text );
			progressCtrl.SetRange32( 0, size );
			progressCtrl.SetPos( 0 );
			findButton.EnableWindow( FALSE );
			findButton.EnableWindow( TRUE );

			listCtrl.DeleteAllItems();
			break;
		}
	case MP_RM_ITEM_DROP_OPTION_LOG_SIZE_NACK:
		{
			CString textThereIsNoResult;
			textThereIsNoResult.LoadString( IDS_STRING1 );

			MessageBox( 0, textThereIsNoResult, _T( "" ), MB_ICONERROR | MB_OK );
			break;
		}
	case MP_RM_ITEM_DROP_OPTION_LOG_STOP_ACK:
		{
			findButton.EnableWindow( TRUE );
			findButton.EnableWindow( FALSE );

			CString textSearchWasStopped;
			textSearchWasStopped.LoadString( IDS_STRING2 );

			MessageBox( 0, textSearchWasStopped, _T( "" ), MB_ICONERROR | MB_OK );
			break;
		}
	case MP_RM_ITEM_DROP_OPTION_LOG_NACK_BY_AUTH:
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
