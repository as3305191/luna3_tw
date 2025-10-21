USE [LUNA_GAMEDB_2025]
GO
/****** Object:  StoredProcedure [dbo].[MP_GUILDFIELDWAR_AddMoney]    Script Date: 8/14/2025 12:21:10 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
ALTER	proc [dbo].[MP_GUILDFIELDWAR_AddMoney]
(
	@CharIdx	int,
	@Money		bigint
)
as
	SET NOCOUNT ON
--	SET xact_abort on

	DECLARE @Curmap	bigint
	DECLARE @RMoney	bigint
	SET @Curmap = 0
	SET @RMoney = 0

	SELECT @Curmap = character_currentmap, @RMoney = character_money 
	FROM tb_character  WITH(READUNCOMMITTED) 
	WHERE character_idx = @CharIdx
	
	IF( @Curmap = 0 )
		BEGIN
			UPDATE tb_character SET character_money = @RMoney + @Money  WHERE character_idx = @CharIdx
			SELECT 1		-- not login
			RETURN
		END
	ELSE
		BEGIN
			SELECT 0, @Curmap, @CharIdx, @Money
		END
