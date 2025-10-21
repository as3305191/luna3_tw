USE [LUNA_GAMEDB_2025]
GO
/****** Object:  StoredProcedure [dbo].[MP_OPTIONLOAD]    Script Date: 26/06/2022 03:57:31 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
ALTER  PROCEDURE [dbo].[MP_OPTIONLOAD]
	(
		@dwPlayerID	int
	)
AS
	Set	Nocount	on
	Set	Xact_abort on

	declare	@HideFlag	int
	declare @MoveType	tinyint
	if not exists ( select * from tb_option where character_idx = @dwPlayerID )
	begin
		set @HideFlag = 0
		set @MoveType = 1
		insert tb_option(character_idx, hideflag, movetype) values(@dwPlayerID, @HideFlag, @MoveType)
	end

	select hideflag, movetype from tb_option where character_idx = @dwPlayerID
