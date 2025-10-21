#include "StdAfx.h"
#include ".\Event.h"

CEvent::CEvent( CEvent::Type type, eStatusKind targetStatus ) :
mType( type ),
mTargetStatus( targetStatus )
{}

// 080616 LUJ, �ٸ� ������Ʈ���� ���ظ� ���� �� �ߵ��Ǵ� �̺�Ʈ
CGiveDamageEvent::CGiveDamageEvent( CObject* target, const RESULTINFO& result ) :
CEvent( TypeGiveDamage, eStatusKind_None ),
mResult( result ),
mTarget( target )
{}

CGiveDamageEvent::CGiveDamageEvent( CObject* target, const RESULTINFO& result, eStatusKind targetStatus ) :
CEvent( TypeGiveDamage, targetStatus ),
mResult( result ),
mTarget( target )
{}

// 080616 LUJ, �ٸ� ������Ʈ�� ���ظ� �޾��� �� �ߵ��Ǵ� �̺�Ʈ
CTakeDamageEvent::CTakeDamageEvent( CObject* attacker, RESULTINFO& result ) :
CEvent( TypeTakeDamage, eStatusKind_None ),
mResult( result ),
mAttacker( attacker )
{}

CTakeDamageEvent::CTakeDamageEvent( CObject* attacker, RESULTINFO& result, eStatusKind targetStatus ) :
CEvent( TypeTakeDamage, targetStatus ),
mResult( result ),
mAttacker( attacker )
{}

// 080616 LUJ, �ٸ� ������Ʈ���� �ǻ�� �� �߻��ϴ� �̺�Ʈ
// 080708 LUJ, �����ڸ� ���ڷ� �޵��� ��
CDieEvent::CDieEvent( CObject* killer, CObject* victim ) :
CEvent( TypeDie, eStatusKind_None ),
mKiller( killer ),
mVictim( victim )
{}

// 080616 LUJ, �ٸ� ������Ʈ�� ������ �� �߻��ϴ� �̺�Ʈ
// 080708 LUJ, CDieEvent�� ���� ������ ������ ��
CKillEvent::CKillEvent( CObject* killer, CObject* victim ) :
CDieEvent( killer, victim )
{
	mType = TypeKill;
}