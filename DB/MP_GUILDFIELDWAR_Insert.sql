USE [LUNA_GAMEDB_2025]
GO
/****** Object:  StoredProcedure [dbo].[MP_GUILDFIELDWAR_Insert]    Script Date: 8/14/2025 12:21:30 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
ALTER PROC [dbo].[MP_GUILDFIELDWAR_Insert]
(
		@GuildIdx1	int,
		@GuildIdx2	int,
		@GFWMoney	BIGINT
)
AS	
	SET NOCOUNT ON
	
	-- ??? ?? ???? ??? ??
	IF EXISTS ( SELECT  *
				FROM    TB_GUILDFIELDWAR
				WHERE   (GuildIdx1 = @GuildIdx1) AND (GuildIdx2 = @GuildIdx2) )
	BEGIN
		RETURN
	END 
	ELSE IF EXISTS (	SELECT  *
						FROM    TB_GUILDFIELDWAR
						WHERE   (GuildIdx1 = @GuildIdx2) AND (GuildIdx2 = @GuildIdx1) )
	BEGIN
		RETURN
	END
	
	INSERT TB_GUILDFIELDWAR 
	(
	GuildIdx1,
	GuildIdx2,
	GFWMoney
	) 
	VALUES 
	(
	@GuildIdx1,
	@GuildIdx2, 
	@GFWMoney
	)
