#include "stdafx.h"
#include "ExperienceLogCommand.h"
#include "client.h"


CExperienceLogCommand::CExperienceLogCommand( CclientApp& application, const TCHAR* title, DWORD playerIndex ) :
CCommand( application, title ),
mPlayerIndex( playerIndex )
{}


void CExperienceLogCommand::SaveToExcel( DWORD serverIndex, const CListCtrl& listCtrl ) const
{
	CString textHead;
	textHead.LoadString( IDS_STRING9 );

	CString textLog;
	textLog.LoadString( IDS_STRING10 );

	CclientApp::SaveToExcel(
		textHead + textLog + _T( "-" ) + mApplication.GetServerName( serverIndex ),
		textLog,
		listCtrl );
}


void CExperienceLogCommand::Initialize( CListCtrl& listCtrl ) const
{
	CRect	rect;
	int		step = -1;

	listCtrl.GetWindowRect( rect );

	listCtrl.InsertColumn( ++step,  _T( "log index" ), LVCFMT_LEFT, 0 );

	CString textDate;
	textDate.LoadString( IDS_STRING3 );
	listCtrl.InsertColumn( ++step, textDate, LVCFMT_LEFT, int( rect.Width() * 0.29f ) );

	CString textCategory;
	textCategory.LoadString( IDS_STRING4 );
	listCtrl.InsertColumn( ++step, textCategory, LVCFMT_LEFT, int( rect.Width() * 0.1f ) );

	CString textLevel;
	textLevel.LoadString( IDS_STRING5 );			
	listCtrl.InsertColumn( ++step, textLevel, LVCFMT_RIGHT, int( rect.Width() * 0.1f ) );

	CString textVariety;
	textVariety.LoadString( IDS_STRING6 );
	listCtrl.InsertColumn( ++step, textVariety, LVCFMT_LEFT, int( rect.Width() * 0.1f ) );

	CString textKillerType;
	textKillerType.LoadString( IDS_STRING7 );
	// 080716 LUJ, ų�� ������ ��ȣ�� ��� ���� �ʵ带 �߰�
	listCtrl.InsertColumn( ++step, _T( "Killer kind" ), LVCFMT_LEFT, 0 );
	listCtrl.InsertColumn( ++step, textKillerType, LVCFMT_LEFT, int( rect.Width() * 0.25f ) );

	CString textKillerIndex;
	textKillerIndex.LoadString( IDS_STRING8 );
	listCtrl.InsertColumn( ++step, textKillerIndex, LVCFMT_RIGHT, int( rect.Width() * 0.25f ) );

	CString textExperience;
	textExperience.LoadString( IDS_STRING9 );
	listCtrl.InsertColumn( ++step, textExperience, LVCFMT_RIGHT, int( rect.Width() * 0.2f ) );

	// 080716 LUJ, ��ȣ ȸ�� ǥ���ϱ� ���� �ʵ� �߰�
	CString textMemo;
	textExperience.LoadString( IDS_STRING17 );
	listCtrl.InsertColumn( ++step, textExperience, LVCFMT_LEFT, int( rect.Width() * 0.3f ) );
}


void CExperienceLogCommand::Find( DWORD serverIndex, const TCHAR* beginTime, const TCHAR* endTime )
{
	MSG_RM_LOG_REQUEST message;
	ZeroMemory( &message, sizeof( message ) );

	message.Category		= MP_RM_EXP_LOG;
	message.Protocol		= MP_RM_EXP_LOG_GET_SIZE_SYN;
	message.mRequestTick	= mTickCount = GetTickCount();
	message.dwObjectID		= mPlayerIndex;

	strncpy( message.mBeginDate,	CW2AEX< MAX_PATH >( beginTime ),	sizeof( message.mBeginDate ) );
	strncpy( message.mEndDate,		CW2AEX< MAX_PATH >( endTime ),		sizeof( message.mEndDate ) );

	mApplication.Send( serverIndex, message, sizeof( message ) );
}


void CExperienceLogCommand::Stop( DWORD serverIndex ) const
{
	MSGROOT message;
	ZeroMemory( &message, sizeof( message ) );

	message.Category	= MP_RM_EXP_LOG;
	message.Protocol	= MP_RM_EXP_LOG_GET_STOP_SYN;

	mApplication.Send( serverIndex, message, sizeof( message ) );
}


void CExperienceLogCommand::Parse( const MSGROOT* message, CListCtrl& listCtrl, CProgressCtrl& progressCtrl, CStatic& resultStatic, CButton& findButton, CButton& stopButton ) const
{
	switch( message->Protocol )
	{
	case MP_RM_EXP_LOG_GET_SIZE_ACK:
		{
			const MSG_DWORD* m = ( MSG_DWORD* )message;

			const DWORD size = m->dwData;

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
	case MP_RM_EXP_LOG_GET_SIZE_NACK:
		{			
			CString textThereIsNoResult;

			textThereIsNoResult.LoadString( IDS_STRING1 );

			MessageBox( 0, textThereIsNoResult, _T( "" ), MB_ICONERROR | MB_OK );
			break;
		}
	case MP_RM_EXP_LOG_GET_ACK:
		{
			const MSG_RM_EXP_LOG* m = ( MSG_RM_EXP_LOG* )message;
			
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
				const MSG_RM_EXP_LOG::Log&	data	= m->mLog[ i ];
				const DWORD					row		= listCtrl.GetItemCount();
				int							step	= 0;

				if( indexSet.end() != indexSet.find( data.mLogIndex ) )
				{
					continue;
				}

				CString text;
				text.Format( _T( "%d" ), data.mLogIndex );
				listCtrl.InsertItem( row, text, 0 );

				listCtrl.SetItemText( row, ++step, CA2WEX< sizeof( data.mDate ) >( data.mDate ) );

				// 080416 LUJ, �α� ������ ���ڷ� ǥ���ϵ��� ��
				listCtrl.SetItemText( row, ++step, mApplication.GetText( data.mType ) );

				text.Format( _T( "%d" ), data.mGrade );
				listCtrl.SetItemText( row, ++step, text );

				text.Format( _T( "%d" ), data.mVariation );
				listCtrl.SetItemText( row, ++step, text );

				// 080716 LUJ, ų�� ������ ��ȣ�� ǥ���Ѵ�
				text.Format( _T( "%d" ), data.mKillerKind );
				listCtrl.SetItemText( row, ++step, text );

				listCtrl.SetItemText( row, ++step, mApplication.GetMonsterName( data.mKillerKind ) );
				
				text.Format( _T( "%d" ), data.mKillerIndex );
				listCtrl.SetItemText( row, ++step, text );

				// 080716 LUJ, ����ġ ���� Ŀ���� �Ǽ������� ����
				text.Format( _T( "%0.0f" ), data.mExperience );
				listCtrl.SetItemText( row, ++step, text );

				// 080716 LUJ,  �α� ���¿� ���� �ٸ� �������� ǥ��
				switch( data.mType )
				{
				case eExpLog_ProtectExp:
					{
						CString textProtect;
						textProtect.LoadString( IDS_STRING402 );

						text.Format( _T( "%s: %d" ), textProtect, data.mCount );
						listCtrl.SetItemText( row, ++step, text );
						break;
					}
				}
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
					stopButton.EnableWindow( FALSE );
				}
			}

			break;
		}
	case MP_RM_EXP_LOG_GET_STOP_ACK:
		{
			findButton.EnableWindow( TRUE );
			stopButton.EnableWindow( FALSE );

			CString textSearchWasStopped;
			textSearchWasStopped.LoadString( IDS_STRING2 );

			MessageBox( 0, textSearchWasStopped, _T( "" ), MB_ICONERROR | MB_OK );
			break;
		}
	case MP_RM_EXP_LOG_GET_STOP_NACK:
		{
			ASSERT( 0 );
			break;
		}
	default:
		{
			break;
		}
	}
}
