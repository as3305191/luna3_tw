USE [LUNA_LOGDB_2025]
GO
/****** Object:  StoredProcedure [dbo].[UP_CHEATE_Log]    Script Date: 8/14/2025 12:25:17 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER OFF
GO
ALTER  proc [dbo].[UP_CHEATE_Log]
(
	@Character_Idx	int,
	@LogType	smallint,
	@Param		bigint,
	@ItemIdx		int,
	@ItemDBIdx	int
)

as
	Set Nocount on
	Set Xact_Abort on

-- 080511 LUJ, ???? ???? ?? ?? ?? ???? ??? ?
return

insert tb_logcheat
(
character_idx,
logtype,
Param,
Item_DBIdx,
Item_Idx
)
values
(
@Character_Idx,
@LogType,
@Param,
@ItemDBIdx,
@ItemIdx
)
