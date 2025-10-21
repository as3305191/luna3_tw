#include "stdafx.h"
#include "BodyChangeDialog.h"
#include "WindowIDEnum.h"
#include "ObjectManager.h"
#include "ItemManager.h"
#include "Item.h"
#include "interface/cStatic.h"
#include "cResourceManager.h"
#include "ExitManager.h"
#include "input/UserInput.h"
#include "MoveManager.h"
#include "ObjectStateManager.h"
#include "cMsgBox.h"
#include "ChatManager.h"
#include "cWindowManager.h"


CBodyChangeDialog::CBodyChangeDialog() :
mStopTime( 0 ),
mAutoRotataionDelayTime( 5000 )
{
	ZeroMemory( &mCharacterChangeInfo,		sizeof( mCharacterChangeInfo ) );
	ZeroMemory( &mUsedItem,					sizeof( mUsedItem ) );
	ZeroMemory( &mStoredCameraDescription,	sizeof( mStoredCameraDescription ) );

	// 080414 LUJ, �޴� ���ڿ� ����Ʈ�� �����Ѵ�. ���� ������ ��� ���� ����
	{
		CMHFile file;


        char filename[100];
        sprintf(filename, "System/Resource/%s", GAMERESRCMNGR->GetTranslationPath("CharMake_List.bin"));
        
		if( ! file.Init( filename, "rb" ) )
		{
			return;
		}

		char line[ MAX_PATH * 5 ] = { 0 };

		while( ! file.IsEOF() )
		{
			file.GetLine( line, sizeof( line ) );

			MenuText menuText;
			menuText.mRace		= RaceType_Max;
			menuText.mPart		= ePartType_None;
			menuText.mGender	= UINT_MAX;
			menuText.mValue		= UINT_MAX;
			
			const char* separator = " \t";

			// 080414 LUJ, ù��°�� ����
			const char* token = strtok( line, separator );

			// 080414 LUJ, �ι�°�� Ÿ��
			token = strtok( 0, separator );

			if( ! token )
			{
				continue;
			}
			else if( ! stricmp( "CM_ST_HMFACE", token ) )
			{
				menuText.mRace		= RaceType_Human;
				menuText.mPart		= ePartType_Face;
				menuText.mGender	= GENDER_MALE;
			}
			else if( ! stricmp( "CM_ST_HMHAIR", token ) )
			{
				menuText.mRace		= RaceType_Human;
				menuText.mPart		= ePartType_Hair;
				menuText.mGender	= GENDER_MALE;
			}
			else if( ! stricmp( "CM_ST_HWMFACE", token ) )
			{
				menuText.mRace		= RaceType_Human;
				menuText.mPart		= ePartType_Face;
				menuText.mGender	= GENDER_FEMALE;
			}
			else if( ! stricmp( "CM_ST_HWMHAIR", token ) )
			{
				menuText.mRace		= RaceType_Human;
				menuText.mPart		= ePartType_Hair;
				menuText.mGender	= GENDER_FEMALE;
			}
			else if( ! stricmp( "CM_ST_EMFACE", token ) )
			{
				menuText.mRace		= RaceType_Elf;
				menuText.mPart		= ePartType_Face;
				menuText.mGender	= GENDER_MALE;
			}
			else if( ! stricmp( "CM_ST_EMHAIR", token ) )
			{
				menuText.mRace		= RaceType_Elf;
				menuText.mPart		= ePartType_Hair;
				menuText.mGender	= GENDER_MALE;
			}
			else if( ! stricmp( "CM_ST_EWMFACE", token ) )
			{
				menuText.mRace		= RaceType_Elf;
				menuText.mPart		= ePartType_Face;
				menuText.mGender	= GENDER_FEMALE;
			}
			else if( ! stricmp( "CM_ST_EWMHAIR", token ) )
			{
				menuText.mRace		= RaceType_Elf;
				menuText.mPart		= ePartType_Hair;
				menuText.mGender	= GENDER_FEMALE;
			}
			// 090504 ONS �ű��������� ó�� �߰�
			else if( ! stricmp( "CM_ST_DMFACE", token ) )
			{
				menuText.mRace		= RaceType_Devil;
				menuText.mPart		= ePartType_Face;
				menuText.mGender	= GENDER_MALE;
			}
			else if( ! stricmp( "CM_ST_DMHAIR", token ) )
			{
				menuText.mRace		= RaceType_Devil;
				menuText.mPart		= ePartType_Hair;
				menuText.mGender	= GENDER_MALE;
			}
			else if( ! stricmp( "CM_ST_DWMFACE", token ) )
			{
				menuText.mRace		= RaceType_Devil;
				menuText.mPart		= ePartType_Face;
				menuText.mGender	= GENDER_FEMALE;
			}
			else if( ! stricmp( "CM_ST_DWMHAIR", token ) )
			{
				menuText.mRace		= RaceType_Devil;
				menuText.mPart		= ePartType_Hair;
				menuText.mGender	= GENDER_FEMALE;
			}

			// 080414 LUJ, �������� ���� �����
			else
			{
				continue;
			}

			for(
				token = strtok( 0, separator );
				token;
				token = strtok( 0, separator ) )
			{

				menuText.mText	= token;

				const char* value1 = strtok( 0, separator );

				// 080414 LUJ, �� ���������� �ǹ̾��� ��
				const char* value2 = strtok( 0, separator );

				if( ! value1 || ! value2 )
				{
					break;
				}

				menuText.mValue	= atoi( value1 );

				mMenuTextList.push_back( menuText );
			}
		}
	}
}

CBodyChangeDialog::~CBodyChangeDialog()
{}


void CBodyChangeDialog::Linking()
{}


void CBodyChangeDialog::OnActionEvent( LONG id, void* p, DWORD we )
{
	CHero* hero = OBJECTMGR->GetHero();

	if( ! hero )
	{
		return;
	}

	// 080414 LUJ, �̺�Ʈ�� ������ �ڵ� ȸ�� �ð��� ������Ų��
	mStopTime = gCurTime + mAutoRotataionDelayTime;

	CHARACTERCHANGE_INFO info( mCharacterChangeInfo );

	switch( id )
	{
	case BODY_CHANGE_HAIR_LEFT:
		{
			mHairMenu.Decrease();
			info.HairType	= mHairMenu.GetValue();
			info.FaceType	= mFaceMenu.GetValue();

			hero->SetCharChangeInfo( &info );

			{
				cStatic* textStatic = ( cStatic* )GetWindowForID( BODY_CHANGE_HAIR_TEXT );

				if( textStatic )
				{
					textStatic->SetStaticText( mHairMenu.GetText() );
				}
			}

			break;
		}
	case BODY_CHANGE_HAIR_RIGHT:
		{
			mHairMenu.Increase();
			info.HairType	= mHairMenu.GetValue();
			info.FaceType	= mFaceMenu.GetValue();

			hero->SetCharChangeInfo( &info );

			{
				cStatic* textStatic = ( cStatic* )GetWindowForID( BODY_CHANGE_HAIR_TEXT );

				if( textStatic )
				{
					textStatic->SetStaticText( mHairMenu.GetText() );
				}
			}

			break;
		}
	case BODY_CHANGE_FACE_LEFT:
		{
			mFaceMenu.Decrease();
			info.HairType	= mHairMenu.GetValue();
			info.FaceType	= mFaceMenu.GetValue();

			hero->SetCharChangeInfo( &info );

			{
				cStatic* textStatic = ( cStatic* )GetWindowForID( BODY_CHANGE_FACE_TEXT );

				if( textStatic )
				{
					textStatic->SetStaticText( mFaceMenu.GetText() );
				}
			}

			break;
		}
	case BODY_CHANGE_FACE_RIGHT:
		{
			mFaceMenu.Increase();
			info.HairType	= mHairMenu.GetValue();
			info.FaceType	= mFaceMenu.GetValue();

			hero->SetCharChangeInfo( &info );

			{
				cStatic* textStatic = ( cStatic* )GetWindowForID( BODY_CHANGE_FACE_TEXT );

				if( textStatic )
				{
					textStatic->SetStaticText( mFaceMenu.GetText() );
				}
			}

			break;
		}
	case BODY_CHANGE_SUBMIT:
		{
			const ITEM_INFO* info = ITEMMGR->GetItemInfo( mUsedItem	.wIconIdx );

			if( ! info )
			{
				break;
			}

			char text[ MAX_PATH ] = { 0 };

			sprintf(
				text,
				CHATMGR->GetChatMsg( 1244 ),
				info->ItemName );

			WINDOWMGR->MsgBox( MBI_USEBODYCHANGEITEM, MBT_YESNO, text );

			// 080930 LUJ, �޽��� �ڽ� ���� ������ â�� ��ٴ�
			SetDisable( TRUE );
			break;
		}			
	case MBI_USEBODYCHANGEITEM:
		{
			// 080930 LUJ, ���� �Ŀ��� â ����� �����Ѵ�
			SetDisable( FALSE );

			if( MBI_NO == we )
			{
				break;
			}

			info.HairType	= mHairMenu.GetValue();
			info.FaceType	= mFaceMenu.GetValue();

			// 080414 LUJ, ���� ���� �ٸ��� �ʴٸ� ó���� ���� ����
			if( ! memcmp( &info, &mCharacterChangeInfo, sizeof( info ) ) )
			{
				return;
			}

			// 080414 LUJ, ������ �޽��� ����
			{
				MSG_CHARACTER_CHANGE message;
				ZeroMemory( &message, sizeof( message ) );

				message.dwObjectID	= hero->GetID();
				message.Category	= MP_ITEM;
				message.Protocol	= MP_ITEM_SHOPITEM_CHARCHANGE_SYN;
				message.mItem		= mUsedItem;
				message.mInfo		= info;

				NETWORK->Send( &message, sizeof( message ) );
			}

			SetDisable( TRUE );
			EXITMGR->SendExitMsg( eEK_CHARSEL );

			// 080414 LUJ, �ڵ� ���� ����Ǵ� ���� ȸ���ϵ��� �Ѵ�
			mStopTime = gCurTime;
			break;
		}
	case CMI_CLOSEBTN:
		{
			SetActive( FALSE );
			break;
		}
	}

	// 080414 LUJ, ���簪�� ������ Ȯ�� ��ư ��Ȱ��ȭ
	// 080930 LUJ, �޽��� �ڽ� ���� �� ó���ϸ� ��ư�� �����������.
	if( MBI_USEBODYCHANGEITEM != id )
	{
		cWindow* window = GetWindowForID( BODY_CHANGE_SUBMIT );

		if( window )
		{
			// 080930 LUJ, ���õ� �޴� �׸��� ��ȿ���� ��ȯ�ϴ� ���� Ŭ����
			class 
			{
			public:
				BOOL operator()( const CMenu& faceMenu, const CMenu& hairMenu, const ITEMBASE& usedItem ) const
				{
					const CItemManager::BodyChangeScriptListMap&				scriptListMap	= ITEMMGR->GetBodyChangeScriptListMap();
					const CItemManager::BodyChangeScriptListMap::const_iterator	map_it			= scriptListMap.find( usedItem.wIconIdx );
	
					if( scriptListMap.end() == map_it )
					{
						return FALSE;
					}
	
					BOOL isValidHair	= FALSE;
					BOOL isValidFace	= FALSE;
	
					const CItemManager::BodyChangeScriptList& scriptList = map_it->second;
	
					for(	CItemManager::BodyChangeScriptList::const_iterator list_it = scriptList.begin();
							scriptList.end() != list_it;
							++list_it )
					{
						const CItemManager::BodyChangeScript& script = *list_it;
	
						if( script.mPart	== ePartType_Hair	&&
							script.mValue	== hairMenu.GetValue() )
						{
							isValidHair = TRUE;
						}
						else if(	script.mPart	== ePartType_Face	&&
									script.mValue	== faceMenu.GetValue() )
						{
							isValidFace = TRUE;
						}
					}
	
					return isValidFace && isValidHair;
				}
			}
			IsValid;
	
			const BOOL	isValid		= IsValid( mFaceMenu, mHairMenu, mUsedItem );
			const BOOL	isDifferent	= memcmp( &info, &mCharacterChangeInfo, sizeof( info ) );
			
			window->SetActive( isValid && isDifferent );
		}
	}

	cDialog::OnActionEvent( id, p, we );
}


void CBodyChangeDialog::SetActive( BOOL isActive, CItem& item )
{
	CHero* hero = OBJECTMGR->GetHero();

	if( ! hero )
	{
		return;
	}

	typedef CItemManager::BodyChangeScriptListMap BodyChangeScriptListMap;

	const BodyChangeScriptListMap&			scriptListMap	= ITEMMGR->GetBodyChangeScriptListMap();
	BodyChangeScriptListMap::const_iterator	map_it			= scriptListMap.find( item.GetItemIdx() );

	// 080414 LUJ, �����ۿ� �ش��ϴ� �ܾ� ���� ��ũ��Ʈ�� �ִ��� ����
	if( scriptListMap.end() == map_it )
	{
		return;
	}

	// 080728 LUJ, �� �̿�� �������� �׸��� ���������Ƿ� �ʱ�ȭ�ؾ��Ѵ�.
	mFaceMenu.Clear();
	mHairMenu.Clear();

	// 080414 LUJ, ����Ʈ�� �������� �÷��̾ �ش��ϴ� ������ �����Ѵ�
	{
		typedef CItemManager::BodyChangeScriptList	BodyChangeScriptList;
		typedef CItemManager::BodyChangeScript		BodyChangeScript;

		const BodyChangeScriptList& scriptList = map_it->second;

		for(
			BodyChangeScriptList::const_iterator list_it = scriptList.begin();
			scriptList.end() != list_it;
			++list_it )
		{
			const BodyChangeScript& script = *list_it;

			if( script.mRace	!= hero->GetRace() ||
				script.mGender	!= hero->GetGender() )
			{
				continue;
			}

			const char* text = GetText(
				script.mRace,
				script.mPart,
				script.mGender,
				script.mValue );

			// 080414 LUJ, ���ڿ��� ������ ���ҽ� �Ŵ������� �о�´�
			// 080428 LUJ, �� ���ڿ��� �߸� üũ�ϴ� ���� ����
			if( ! text || ! *text )
			{
				text = RESRCMGR->GetMsg( script.mText );
			}

			switch( script.mPart )
			{
			case ePartType_Face:
				{
					mFaceMenu.Add(
						BYTE( script.mValue ),
						text );
					break;
				}
			case ePartType_Hair:
				{
					mHairMenu.Add(
						BYTE( script.mValue ),
						text );
					break;
				}
			}
		}
	}

	if( ! mHairMenu.GetSize() &&
		! mFaceMenu.GetSize() )
	{
		return;
	}
    
	// 080414 LUJ, ���� ������ �����ϰ�, �⺻���� ���� �������� �س��´�
	{
		const CHARACTER_TOTALINFO* info = hero->GetCharacterTotalInfo();

		if( ! info )
		{
			return;
		}

		ZeroMemory( &mCharacterChangeInfo, sizeof( mCharacterChangeInfo ) );

		mCharacterChangeInfo.FaceType	= info->FaceType;
		mCharacterChangeInfo.HairType	= info->HairType;
		mCharacterChangeInfo.Gender		= info->Gender;
		mCharacterChangeInfo.Height		= info->Height;
		mCharacterChangeInfo.Width		= info->Width;

		mHairMenu.Add( info->HairType, "..." );
		mFaceMenu.Add( info->FaceType, "..." );

		mHairMenu.SetDefault( info->HairType );
		mFaceMenu.SetDefault( info->FaceType );
	}

	// 080414 LUJ, ������ ���� ���� �� ��Ȱ��ȭ
	{
		item.SetLock( TRUE );

		mUsedItem = item.GetItemBaseInfo();
	}

	// 080414 LUJ, �ʱ� ���� �����ϱ� ���� �̺�Ʈ�� ���Ƿ� �߻���Ų��. �������� ���� �ٽ� ���������� ���� ���� �ʱⰪ
	{
		OnActionEvent( BODY_CHANGE_HAIR_LEFT,	0, 0 );
		OnActionEvent( BODY_CHANGE_HAIR_RIGHT,	0, 0 );

		OnActionEvent( BODY_CHANGE_FACE_LEFT,	0, 0 );
		OnActionEvent( BODY_CHANGE_FACE_RIGHT,	0, 0 );

		// 080414 LUJ, �̺�Ʈ�� ���� ������ �ڵ� ȸ���� ������Ų��. ���ʿ��� �ڵ� ȸ���ǵ��� ����
		mStopTime = gCurTime;
	}	

	// 080414 LUJ, �÷��̾ ������ �� ������ �Ѵ�
	{
		hero->DisableAutoAttack();
		hero->SetNextAction( 0 );

		MOVEMGR->HeroMoveStop();
		OBJECTSTATEMGR->StartObjectState( hero, eObjectState_Exit );
	}

	// 080414 LUJ, â�� ��ġ�� �ڵ� ��ġ�Ѵ�
	{
		const DISPLAY_INFO& screen	= GAMERESRCMNGR->m_GameDesc.dispInfo;
		const LONG			space	= 100;
		const LONG			x		= screen.dwWidth - GetWidth() - space;
		const LONG			y		= ( screen.dwHeight - GetHeight() ) / 2;

		SetAbsXY( x, y );
	}

	// 080414 LUJ, ī�޶� ������ �����صΰ� â�� ���� �� �����Ѵ�
	{
		const MHCAMERADESC* description = CAMERA->GetCameraDesc();

		if( description )
		{
			mStoredCameraDescription = *description;

			const VECTOR3& vector1 = description->m_CameraPos;

			VECTOR3 vector2 = { 0 };
			hero->GetPosition( &vector2 );

			// 080414 LUJ, 3���� ���� ���� �� �� ���� �Ÿ��� ���Ѵ�
			const double distance =
				sqrt( pow( vector1.x, 2.0f ) + pow( vector1.y, 2.0f ) + pow( vector1.z, 2.0f ) ) -
				sqrt( pow( vector2.x, 2.0f ) + pow( vector2.y, 2.0f ) + pow( vector2.z, 2.0f ) );

			// 080414 LUJ, ī�޶� �ִ��� ����
			CAMERA->ZoomDistance( 0, -1.0f * abs( float( distance ) ), 1000 );
		}
	}

	// 080414 LUJ, ����/���� ǥ��
	{
		std::string race;
		std::string gender;

		switch( hero->GetRace() )
		{
		case RaceType_Human:
			{
				race = RESRCMGR->GetMsg( 247 );
				break;
			}
		case RaceType_Elf:
			{
				race = RESRCMGR->GetMsg( 248 );
				break;
			}
		// 090504 ONS �ű��������� ó�� �߰�
		case RaceType_Devil:
			{
				race = RESRCMGR->GetMsg( 1129 );
				break;
			}
		}

		switch( hero->GetGender() )
		{
		case GENDER_FEMALE:
			{
				gender = RESRCMGR->GetMsg( 30 );
				break;
			}
		case GENDER_MALE:
			{
				gender = RESRCMGR->GetMsg( 29 );
				break;
			}
		}

		cStatic* titleStatic = ( cStatic* )GetWindowForID( BODY_CHANGE_TITLE_TEXT );

		if( titleStatic )
		{
			titleStatic->SetStaticText( ( race + " " + gender ).c_str() );
		}
	}

	SetActive( TRUE );
}


void CBodyChangeDialog::SetActive( BOOL isActive )
{
	CHero* hero = OBJECTMGR->GetHero();

	if( ! hero )
	{
		return;
	}
	else if( ! isActive )
	{
		hero->SetCharChangeInfo( &mCharacterChangeInfo );

		// 080414 LUJ, ������ ��� ����
		{
			CItem* item = ITEMMGR->GetItem( mUsedItem.dwDBIdx );

			if( item )
			{
				item->SetLock( FALSE );
			}
		}

		// 080414 LUJ, �÷��̾ ������ �� �ֵ��� �Ѵ�
		OBJECTSTATEMGR->StartObjectState( hero, eObjectState_None );
		
		// 080414 LUJ, ����� ī�޶�� ���� ī�޶��� ������ �˾Ƴ��� ����ġ��Ų��
		{
			const MHCAMERADESC* description = CAMERA->GetCameraDesc();

			if( description )
			{
				VECTOR3 vector1 = { 0 };
				hero->GetPosition( &vector1 );

				const VECTOR3& vector2 = mStoredCameraDescription.m_CameraPos;

				// 080414 LUJ, 3���� ���� ���� �� �� ���� �Ÿ��� ���Ѵ�
				const double distance =
					sqrt( pow( vector1.x, 2.0f ) + pow( vector1.y, 2.0f ) + pow( vector1.z, 2.0f ) ) -
					sqrt( pow( vector2.x, 2.0f ) + pow( vector2.y, 2.0f ) + pow( vector2.z, 2.0f ) );

				// 080414 LUJ, ī�޶� �� ��ġ��Ų��
				CAMERA->ZoomDistance( 0, abs( float( distance ) ), 1000 );
			}
		}

		ITEMMGR->SetDisableDialog( FALSE, eItemTable_Inventory );
	}

	cDialog::SetActive( isActive );
}


void CBodyChangeDialog::Render()
{
	if( ! IsActive() )
	{
		return;
	}

	// 080414 LUJ, ������ ���콺 ��ư�� ���������� ���� �ð� �������� �ʵ��� �Ѵ�
	if( g_UserInput.GetMouse()->RButtonPressed() )
	{
		mStopTime = gCurTime + mAutoRotataionDelayTime;
	}
	else if( gCurTime > mStopTime  )
	{
		CAMERA->MouseRotate( -1, 0 );
	}

	cDialog::Render();
}


const char* CBodyChangeDialog::GetText( RaceType race, PART_TYPE part, DWORD gender, DWORD value ) const
{
	for(
		MenuTextList::const_iterator it = mMenuTextList.begin();
		mMenuTextList.end() != it;
		++it )
	{
		const MenuText& menuText = *it;

		if( menuText.mGender	!= gender	||
			menuText.mRace		!= race		||
			menuText.mPart		!= part		||
			menuText.mValue		!= value )
		{
			continue;
		}

		return menuText.mText.c_str();
	}

	return "";
}