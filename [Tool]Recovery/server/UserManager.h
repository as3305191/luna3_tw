#pragma once


class CUserManager  
{
public:	
	CUserManager();
	virtual ~CUserManager();

	static CUserManager& GetInstance();

	bool IsConnect( DWORD connectionIndex ) const;
	bool IsConnectedUser( const char* id ) const;
	bool IsConnectedIp( const char* ip ) const;


public:
	struct User
	{
		eGM_POWER	mPower;
		std::string	mId;
		std::string	mName;
		std::string mIp;
		std::string	mRegistedDate;
	};

	bool		AddUser( DWORD connectionIndex, const char* id, const char* name, const char* ip, const char* registedDate, eGM_POWER );
	void		RemoveUser( DWORD connectionIndex );
	const User& GetUser( DWORD connectionIndex )	const;
	const User& GetUser( const char* id )			const;

    
protected:
	// Ű: ���� �ε���
	typedef stdext::hash_map< DWORD, User >	UserMap;
	UserMap									mUserMap;


	// ��� ���� ����
public:
	typedef std::list< MSG_RM_AUTHORITY::Authority > AuthorityList;
	void SetAuthority( eGM_POWER, const AuthorityList& );
	void SetAuthority( eGM_POWER, const MSG_RM_AUTHORITY& );
	bool IsAuthorize( RecoveryCategory, BYTE protocol, DWORD connectionIndex ) const;	
private:	
	typedef	LONGLONG	MessageKey;
	// �������ݿ� ���� ��밡���� ������ ����������. �������� ��� ���������� ����������.
	typedef stdext::hash_set< MessageKey >					MessageKeySet;
	typedef stdext::hash_map< eGM_POWER, MessageKeySet >	AuthorityMap;
	AuthorityMap mAuthorityMap;
protected:
	// �� ���� �޾� ������ Ű�� �����Ѵ�
	MessageKey GetMessageKey( RecoveryCategory, BYTE protocol ) const;
};