USE [LUNA_GAMEDB_2025]
GO
/****** Object:  StoredProcedure [dbo].[MP_CHARACTER_MAPCHANGEPOINTRETURN]    Script Date: 8/14/2025 12:19:50 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
/*
081218 LUJ, ???? ??? ?? ??
*/
ALTER     proc [dbo].[MP_CHARACTER_MAPCHANGEPOINTRETURN]
(
	@character_idx int
,	@character_mapchangepoint int
, 	@dwConnectionIdx int
,	@level int

-- 080605 LYW : ??? ??(__int32 -> __int64) ? ?? ??? ?? ??.
--, 	@exppoint int
, 	@exppoint bigint
	-- 081218 LUJ, ???? ???
,	@inventoryMoney	bigint
,	@pyogukmoney bigint 

)
as
	Set Nocount on
	Set Xact_abort on

	declare  @UserIdx int 

Update TB_CHARACTER 
set CHARACTER_MAPCHANGEPOINT = @character_mapchangepoint ,
CHARACTER_GRADE = @level,
CHARACTER_EXPOINT = @exppoint,
-- 081218 LUJ, ???? ??? ?? ??
CHARACTER_MONEY = @inventoryMoney

where @character_idx = CHARACTER_IDX

select @UserIdx = USER_IDX 
from TB_CHARACTER with(nolock) 
where CHARACTER_IDX = @character_idx

update TB_PYOGUK
set PYOGUKMONEY = @pyogukmoney
where USER_IDX = @UserIdx 

select @dwConnectionIdx , @character_mapchangepoint
