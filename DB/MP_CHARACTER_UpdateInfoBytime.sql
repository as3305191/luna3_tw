USE [LUNA_GAMEDB_2025]
GO
/****** Object:  StoredProcedure [dbo].[MP_CHARACTER_UpdateInfoBytime]    Script Date: 8/14/2025 12:17:32 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER OFF
GO
-- ???, ?????, ? (???? ????)

ALTER    proc [dbo].[MP_CHARACTER_UpdateInfoBytime]
(
		@charno	int

-- 080605 LYW : ??? ??(__int32 -> __int64) ? ?? ??? ?? ??.
--	,	@exp		int
	,	@exp		bigint

	,	@abilityexp	int
	,	@money		bigint
	,	@time		int
	,	@lastpkendtime int
)
as
	Set Nocount on
	Set Xact_Abort on
-----------------------------------------------------------------
-- 080605 LYW : ??? ??(__int32 -> __int64) ? ?? ??? ?? ??.
--	declare @chrexp int
	declare @chrexp bigint

select @chrexp = character_expoint from tb_character where character_idx = @charno
if ( @chrexp > @exp ) 
	return
-----------------------------------------------------------------
-- 080610 LYW --- ?? ????? ???? ?? ??.
--	update tb_character set character_expoint = @exp, character_abilityexp = @abilityexp, character_money = @money, character_playtime = @time, character_lastpkendtime = @lastpkendtime

	update tb_character 
	set character_expoint = @exp, 
--	character_abilityexp = @abilityexp, 
	character_money = @money, 
	character_playtime = @time, 
	character_lastpkendtime = @lastpkendtime

	where character_idx = @charno
