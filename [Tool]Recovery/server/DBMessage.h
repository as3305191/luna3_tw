#pragma once

// �α���
void RLoginOperator( LPMIDDLEQUERY, LPDBMESSAGE );

// �α��� ID�� Ű����� ���� �ε����� ����
void RGetUserList( LPMIDDLEQUERY, LPDBMESSAGE );

// ������ �ε����� �����´�
void RGetUserIndex( LPMIDDLEQUERY, LPDBMESSAGE );

// ������ ������ �����´�
void RGetUserData( LPMIDDLEQUERY, LPDBMESSAGE );

// �������� �÷��̾� ����� �����´�
void RGetUserPlayerList( LPQUERY, LPDBMESSAGE );

// ����ڸ� �߰��Ѵ�
void UserAdd( DWORD connectionIndex, const CHARACTERMAKEINFO& );
void RUserAdd( LPMIDDLEQUERY, LPDBMESSAGE );

// ����ڰ� ������ Ư�� �÷��̾ �����Ѵ�
void RestoreUser( DWORD connectionIndex, DWORD playerIndex, const char* playerName );
void RRestoreUser( LPMIDDLEQUERY, LPDBMESSAGE );

// ����ڰ� ������ Ư�� �÷��̾ �����Ѵ�
void DeleteUser( DWORD connectionIndex, DWORD playerIndex );
void RDeleteUser( LPMIDDLEQUERY, LPDBMESSAGE );

// Ű����� �÷��̾� �ε����� �̸��� ����
void GetPlayerList( DWORD connectionIndex, DWORD userIndex, const char* userName, const char* keyword, DWORD beginPlayerIndex );
void RGetPlayerList( LPQUERY, LPDBMESSAGE );

// �÷��̾��� ���� ID�� �����´�
void GetPlayerLoginId( DWORD connectionIndex, DWORD userIndex, DWORD playerIndex );
void RGetPlayerLoginId( LPMIDDLEQUERY, LPDBMESSAGE );

// �÷��̾� �� ������ ����
void RGetPlayerData( LPQUERY, LPDBMESSAGE );

// ���ڿ� �� ū �ʵ尪�� ���⼭ ����
void RGetPlayerExtendedData( LPMIDDLEQUERY, LPDBMESSAGE );

// 080328 LUJ, �÷��̾ �Ҽӵ� �йи� �̸��� ����
void RGetPlayerFamilyData( LPMIDDLEQUERY, LPDBMESSAGE );

// ĳ���� Ȯ�� ������ �����Ѵ�
//void SetPlayerExtendedData( DWORD connectionIndex, const MSG_RM_PLAYER_EXTENDED_DATA& );
void RSetPlayerExtendedData( LPMIDDLEQUERY, LPDBMESSAGE );

// �ֹ��� ������ �д´�
void RGetPlayerLicense( LPMIDDLEQUERY, LPDBMESSAGE );

// �ֹ��� ������ �����Ѵ�
//void PlayerSetLicense( DWORD connectionIndex, DWORD playerIndex, const DATE_MATCHING_INFO& );
void RPlayerSetLicense( LPMIDDLEQUERY, LPDBMESSAGE );

// ������ ������ �д´�
void GetItemListOnInventory( DWORD connectionIndex, DWORD playerIndex, DWORD beginItemDbIndex );
void GetItemListOnStorage( DWORD connectionIndex, DWORD userIndex, DWORD beginItemDbIndex );
void GetItemListOnShop( DWORD connectionIndex, DWORD userIndex, DWORD beginItemDbIndex );
// 080716 LUJ, ���� ���� �������� �����Ѵ�
// 080730 LUJ, ����� ��ȣ ��� �� ��ȣ�� ���ڷ� ����
void GetItemListOnPet( DWORD connectionIndex, DWORD petIndex, DWORD beginItemDbIndex );

void RGetItemList( LPQUERY, LPDBMESSAGE );

// ������ �����ڸ� ã�´�
void FindItemOwner( DWORD connectionIndex, DWORD itemIndex, DWORD playerIndex );
void RFindItemOwner( LPQUERY, LPDBMESSAGE );

// ������ �߰�
void RAddItem( LPMIDDLEQUERY, LPDBMESSAGE );

// ������ ��� ���� ���� ����
void RSetItemEndTime( LPMIDDLEQUERY, LPDBMESSAGE );

// ��ų ������ ȹ���Ѵ�
void GetSkillList( DWORD connectionIndex, DWORD playerIndex );
void RGetSkillList( LPQUERY, LPDBMESSAGE );

// 080731 LUJ, ���� ��ų ������ ȹ���Ѵ�
void GetBuffList( DWORD connectionIndex, DWORD playerIndex, DWORD lastSkillIndex );
void RGetBuffList( LPMIDDLEQUERY, LPDBMESSAGE );

// ��ų ������ �ٲ۴�
void SetSkillLevel( DWORD connectionIndex, DWORD playerIndex, DWORD skillDbIndex, DWORD level );
void RSetSkillLevel( LPMIDDLEQUERY, LPDBMESSAGE );

// ��ų�� �߰��Ѵ�
void AddSkillData( DWORD connectionIndex, DWORD playerIndex, DWORD skillIndex );
void RAddSkillData( LPMIDDLEQUERY, LPDBMESSAGE );

// ��ų�� �����Ѵ�
void RemoveSkillData( DWORD connectionIndex, DWORD skillDbIndex );
void RRemoveSkillData( LPMIDDLEQUERY, LPDBMESSAGE );

// �йи��� �˻��Ѵ�
void RGetFamilyList( LPQUERY, LPDBMESSAGE );
// �йи� �Ϲ� ������ �д´�
void RGetFamilyData( LPMIDDLEQUERY, LPDBMESSAGE );
// �йи� ��� ������ �д´�
void RGetFamilyMember( LPQUERY, LPDBMESSAGE );
// �йи� ���� ������ �д´�
void RGetFamilyFarm( LPMIDDLEQUERY, LPDBMESSAGE );
// ���۹� ������ �д´�
void RGetFamilyCrop( LPMIDDLEQUERY, LPDBMESSAGE );
// 080731 LUJ, ���� ������ �д´�
void RGetFamilyLivestock( LPMIDDLEQUERY, LPDBMESSAGE );

// ���� ����Ʈ ������ �д´�
void GetQuestMainData( DWORD connectionIndex, DWORD playerIndex, DWORD questIndex );
void RGetQuestMainData( LPQUERY, LPDBMESSAGE );

// ���� ����Ʈ ������ �д´�
void GetQuestSubData( DWORD connectionIndex, DWORD playerIndex, DWORD questIndex );
void RGetQuestSubData( LPQUERY, LPDBMESSAGE );

// ����Ʈ ������ ������ �д´�
void GetQuestItemList( DWORD connectionIndex, DWORD playerIndex, DWORD questIndex );
void RGetQuestItemList( LPQUERY, LPDBMESSAGE );

// ��������Ʈ�� �Ϸ��Ѵ�
void EndSubQuest( DWORD connectionIndex, DWORD playerIndex, DWORD mainQuestIndex, DWORD subQuestIndex, QSTATETYPE state );
void REndSubQuest( LPMIDDLEQUERY, LPDBMESSAGE );

// ����Ʈ�� �����Ѵ�(�ٽ� ������ �� �ֵ���)
void RemoveQuestData( DWORD connectionIndex, DWORD playerIndex, DWORD mainQuestIndex );
void RRemoveQuestData( LPMIDDLEQUERY, LPDBMESSAGE );

// ����Ʈ ������ ������ �����Ѵ�
void RUpdateQuestItem( LPMIDDLEQUERY, LPDBMESSAGE );

// ����Ʈ �������� �����Ѵ�
void DeleteQuestItem( DWORD connectionIndex, DWORD playerIndex, DWORD questMainIndex, DWORD itemIndex );
void RDeleteQuestItem( LPMIDDLEQUERY, LPDBMESSAGE );

// ����Ʈ �α׸� �����´�
void RGetQuestLogSize( LPMIDDLEQUERY, LPDBMESSAGE );
void RGetQuestLog( LPQUERY, LPDBMESSAGE );

// ��带 �˻��Ѵ�
void GetGuildList( DWORD connectionIndex, const char* keyword, bool isDismissed );
void RGetGuildList( LPQUERY, LPDBMESSAGE );

// ��� �⺻ ������ ��´�
void RGetGuildData( LPMIDDLEQUERY, LPDBMESSAGE );

// 080425 LUJ, ��� ��ų ������ ��´�
void RGetGuildSkill( LPMIDDLEQUERY, LPDBMESSAGE );

// â�� ������ ��´�
void GetGuildStore( DWORD connectionIndex, DWORD guildIndex );
void RGetGuildStore( LPQUERY, LPDBMESSAGE );

// ȸ�� ������ ��´�
void GetGuildMember( DWORD connectionIndex, DWORD guildIndex );
void RGetGuildMember( LPQUERY, LPDBMESSAGE );

// �Ʊ� ������ ��´�
void GetGuildFriendly( DWORD connectionIndex, DWORD guildIndex );
void RGetGuildFriendly( LPQUERY, LPDBMESSAGE );

// ���� ������ ��´�
void GetGuildEnemy( DWORD connectionIndex, DWORD guildIndex );
void RGetGuildEnemy( LPQUERY, LPDBMESSAGE );

// ��� ������ �����Ѵ�
void RGuildUpdateData( LPMIDDLEQUERY, LPDBMESSAGE );

// ��� ������ �����Ѵ�
void RGuildSetRank( LPMIDDLEQUERY, LPDBMESSAGE );

// ��忡�� Ż���Ų��
void RGuildKickMember( LPMIDDLEQUERY, LPDBMESSAGE );

// ����ġ �α� ������ ��ȸ�Ѵ�
void RGetExperienceLogSize( LPMIDDLEQUERY, LPDBMESSAGE );
void RGetExperienceLog( LPQUERY, LPDBMESSAGE );

// ������ �α� ��ȸ
void RGetItemLogSize( LPMIDDLEQUERY, LPDBMESSAGE );
void RGetItemLog( LPQUERY, LPDBMESSAGE );

// ���� �α׸� �����´�
void RGetStatLogSize( LPMIDDLEQUERY, LPDBMESSAGE );
void RGetStatLog( LPQUERY, LPDBMESSAGE );

// ��ų �α׸� �����´�
void RGetSkillLogSize( LPMIDDLEQUERY, LPDBMESSAGE );
void RGetSkillLog( LPQUERY, LPDBMESSAGE );

// ������ �ɼ� �α׸� �����´�
void RGetItemOptionLogSize( LPMIDDLEQUERY, LPDBMESSAGE );
void RGetItemOptionLog( LPQUERY, LPDBMESSAGE );

// 080320 LUJ, ��� �ɼ� �α׸� �����´�
void RGetItemDropOptionLogSize( LPMIDDLEQUERY, LPDBMESSAGE );
void RGetItemDropOptionLog( LPQUERY, LPDBMESSAGE );

// ��� �α� �� �����´�
void RGetGuildLogSize( LPMIDDLEQUERY, LPDBMESSAGE );
void RGetGuildLog( LPQUERY, LPDBMESSAGE );

// ��� â�� �α׸� �����´�
void RGetGuildWarehouseLogSize( LPMIDDLEQUERY, LPDBMESSAGE );
void RGetGuildWarehouseLog( LPQUERY, LPDBMESSAGE );

// Ŭ���� �α׸� �����´�
void RGetJobLogSize( LPMIDDLEQUERY, LPDBMESSAGE );
void RGetJobLog( LPQUERY, LPDBMESSAGE );

// �йи� ���۹� �α� �����´�
void RGetFarmCropLogSize( LPMIDDLEQUERY, LPDBMESSAGE );
void RGetFarmCropLog( LPQUERY, LPDBMESSAGE );

// �йи� ����Ʈ ũ�⸦ �����´�
void RGetFamilyPointLogSize( LPMIDDLEQUERY, LPDBMESSAGE );
void RGetFamilyPointLog( LPQUERY, LPDBMESSAGE );

// ��� ���� �α׸� �����´�
void RGetGuildScoreLogSize( LPMIDDLEQUERY, LPDBMESSAGE );
void RGetGuildScoreLog( LPQUERY, LPDBMESSAGE );

// IP�� ���� ��� ��Ͽ� �߰��Ѵ�
void AddPermission( DWORD connectionIndex, const char* ipAddress );
void RAddPermission( LPMIDDLEQUERY, LPDBMESSAGE );

// IP�� ���� ��� ��Ͽ��� �����Ѵ�
void RemovePermission( DWORD connectionIndex, const char* ipAddress );
void RRemovePermission( LPMIDDLEQUERY, LPDBMESSAGE );

// 창고
void RGetStorageData( LPMIDDLEQUERY, LPDBMESSAGE );
void RSetStorageData( LPMIDDLEQUERY, LPDBMESSAGE );

// operator
void RGetOperator( LPQUERY, LPDBMESSAGE );
void RAddOperator( LPMIDDLEQUERY, LPDBMESSAGE );
void RUpdateOperator( LPMIDDLEQUERY, LPDBMESSAGE );
void RGetOperatorIp( LPQUERY, LPDBMESSAGE );
void RAddOperatorIp( LPMIDDLEQUERY, LPDBMESSAGE );
void RRemoveOperatorIp( LPMIDDLEQUERY, LPDBMESSAGE );

// password
void RSetPassword( LPMIDDLEQUERY, LPDBMESSAGE );

// operator log
void RGetOperatorLogSize( LPMIDDLEQUERY, LPDBMESSAGE );
void RGetOperatorLog( LPQUERY, LPDBMESSAGE );

// name log
void RGetNameLog( LPQUERY, LPDBMESSAGE );

// 080401 LUJ, ���� �α�
void RGetFarmLogSize( LPMIDDLEQUERY, LPDBMESSAGE );
void RGetFarmLog( LPQUERY, LPDBMESSAGE );

// 080630 LUJ, ���� ���� �α�
void RGetLivestockLogSize( LPMIDDLEQUERY, LPDBMESSAGE );
void RGetLivestockLog( LPQUERY, LPDBMESSAGE );

// 080716 LUJ, �� �α�
void RGetPetLogSize( LPMIDDLEQUERY, LPDBMESSAGE );
void RGetPetLog( LPQUERY, LPDBMESSAGE );

// 080403 LUJ, ���� ������ ���� �α�
void RGetItemShopLogSize( LPMIDDLEQUERY, LPDBMESSAGE );
void RGetItemShopLog( LPQUERY, LPDBMESSAGE );

// 080523 LUJ, ���� �α�
void RGetFishLogSize( LPMIDDLEQUERY, LPDBMESSAGE );
void RGetFishLog( LPQUERY, LPDBMESSAGE );

// 080526 LUJ, ���� ������ �����´�
void RGetFishingData( LPMIDDLEQUERY, LPDBMESSAGE );
void RGetCookData(LPMIDDLEQUERY, LPDBMESSAGE);
void RGetCookRecipeData(LPMIDDLEQUERY, LPDBMESSAGE);
void RGetHouseData(LPMIDDLEQUERY, LPDBMESSAGE);
void RGetPetData(LPMIDDLEQUERY, LPDBMESSAGE);
void RGetItemGlobalLog(LPQUERY, LPDBMESSAGE);
void RAddPet(LPMIDDLEQUERY, LPDBMESSAGE);

// 081021 LUJ, ���� ������ �����´�
void GetAuthorityToServer( eGM_POWER );
void RGetAuthorityToServer( LPQUERY, LPDBMESSAGE );
void GetAuthorityToUser( DWORD connectionIndex, eGM_POWER, AuthorityType );
void RGetAuthorityToUser( LPQUERY, LPDBMESSAGE );

// 081027 LUJ, ��ʸ�Ʈ ������ �����Ѵ�
void RGetTournamentCount( LPMIDDLEQUERY, LPDBMESSAGE );
// 081027 LUJ, Ư�� ȸ���� ������ �����Ѵ�
void GetTournamentGuild( DWORD connectionIndex, DWORD count, DWORD lastGuildIndex );
void RGetTournamentGuild( LPQUERY, LPDBMESSAGE );
// 081027 LUJ, �÷��̾� ������ �����Ѵ�
void RGetTournamentPlayer( LPMIDDLEQUERY, LPDBMESSAGE );

// 081027 LUJ, 공성 로그
void RGetSiegeLogSize( LPMIDDLEQUERY, LPDBMESSAGE );
void RGetSiegeLog( LPQUERY, LPDBMESSAGE );
// 081127 LUJ, ���� ������ �����Ѵ�. ����ȭ�� ���� DB���� ó���Ѵ�
void IncreaseAuthorityVersion( DWORD connectionIndex, eGM_POWER = eGM_POWER_MASTER, AuthorityType = AuthorityTypeVersion );
void RIncreaseAuthorityVersion( LPMIDDLEQUERY, LPDBMESSAGE );
// 081205 LUJ, 채팅 로그
void RGetChatLogSize( LPMIDDLEQUERY, LPDBMESSAGE );
void RGetChatLog( LPMIDDLEQUERY, LPDBMESSAGE );
// 081205 LUJ, �йи� �α�
void RGetFamilyLogSize( LPMIDDLEQUERY, LPDBMESSAGE );
void RGetFamilyLog( LPQUERY, LPDBMESSAGE );
// 081224 LUJ, ���۷����� �α�
void LogOperator( RecoveryLog, const TCHAR* const user, const TCHAR* const key1, const TCHAR* const value1, const TCHAR* const key2 = _T( "" ), const TCHAR* const value2 = _T( "" ), const TCHAR* const key3 = _T( "" ), const TCHAR* const value3 = _T( "" ) );
// 090122 LUJ, ��ũ��Ʈ ���� �α� ������ �����´�
void RGetScriptHackLogSize( LPMIDDLEQUERY, LPDBMESSAGE );
// 090122 LUJ, ��ũ��Ʈ ���� �α� ������ ��û�Ѵ�
void GetScriptHackLog( DWORD connectionIndex, LPCTSTR beginDate, LPCTSTR endDate, DWORD requestTick, DWORD queriedSize );
// 090122 LUJ, ��ũ��Ʈ ���� �α� ����� ó���Ѵ�
void RGetScriptHackLog( LPQUERY, LPDBMESSAGE );
// 090406 LUJ, �� �α׸� ó���Ѵ�
void RGetMoneyLogSize( LPMIDDLEQUERY, LPDBMESSAGE );
void GetMoneyLog( DWORD connectionIndex, LPCTSTR beginDate, LPCTSTR endDate, DWORD playerIndex, DWORD requestTick, DWORD queriedSize );
void RGetMoneyLog( LPQUERY, LPDBMESSAGE );
// 090609 ShinJS, AutoNote �α׸� ó���Ѵ�
void RGetAutoNoteLogSize( LPMIDDLEQUERY, LPDBMESSAGE );
void GetAutoNoteLog( DWORD connectionIndex, LPCTSTR beginDate, LPCTSTR endDate, DWORD playerIndex, DWORD requestTick, DWORD queriedSize );
void RGetAutoNoteLog( LPQUERY, LPDBMESSAGE );
// 090618 ShinJS, User Punish Data Update��
void RUserDataUpdate( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage );
void RGetHouseLogSize(LPMIDDLEQUERY, LPDBMESSAGE);
void GetHouseLog(DWORD connectionIndex, LPCTSTR beginDate, LPCTSTR endDate, DWORD userIndex, DWORD requestTick, DWORD queriedSize);
void RGetHouseLog(LPQUERY, LPDBMESSAGE);
void RGetCookLogSize(LPMIDDLEQUERY, LPDBMESSAGE);
void GetCookLog(DWORD connectionIndex, LPCTSTR beginDate, LPCTSTR endDate, DWORD userIndex, DWORD requestTick, DWORD queriedSize);
void RGetCookLog(LPQUERY, LPDBMESSAGE);
void RGetRecipeLogSize(LPMIDDLEQUERY, LPDBMESSAGE);
void GetRecipeLog(DWORD connectionIndex, LPCTSTR beginDate, LPCTSTR endDate, DWORD userIndex, DWORD requestTick, DWORD queriedSize);
void RGetRecipeLog(LPQUERY, LPDBMESSAGE);
void RGetFurniture(LPQUERY, LPDBMESSAGE);
void RGetNoteList(LPQUERY, LPDBMESSAGE);
void RGetNote(LPLARGEQUERY, LPDBMESSAGE);
void RGetConsignLogSize(LPMIDDLEQUERY, LPDBMESSAGE);
void GetConsignLog(DWORD connectionIndex, LPCTSTR beginDate, LPCTSTR endDate, DWORD playerIndex, DWORD itemDBIndex, DWORD consignIndex, DWORD requestTick, DWORD queriedSize);
void RGetConsignLog(LPQUERY, LPDBMESSAGE);
void RGetGameRoomPointLogSize(LPMIDDLEQUERY, LPDBMESSAGE);
void GetGameRoomPointLog(DWORD connectionIndex, LPCTSTR beginDate, LPCTSTR endDate, DWORD playerIndex, DWORD requestTick, DWORD queriedSize);
void RGetGameRoomPointLog(LPQUERY, LPDBMESSAGE);
void RGetGameRoomData(LPMIDDLEQUERY, LPDBMESSAGE);
void RGetPartyList(LPMIDDLEQUERY, LPDBMESSAGE);
void RGetForbidChatLogSize(LPMIDDLEQUERY, LPDBMESSAGE);
void GetForbidChatLog(DWORD connectionIndex, LPCTSTR beginDate, LPCTSTR endDate, DWORD playerIndex, DWORD requestTick, DWORD queriedSize);
void RGetForbidChatLog(LPMIDDLEQUERY, LPDBMESSAGE);
