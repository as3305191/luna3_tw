USE [LUNA_GAMEDB_2025]
GO
/****** Object:  StoredProcedure [dbo].[MP_OPTIONUPDATE]    Script Date: 26/06/2022 03:57:33 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
ALTER PROCEDURE [dbo].[MP_OPTIONUPDATE]
	(
		@dwPlayerID	int,
		@dwHideFlag	int,
		@wMoveType tinyint
	)
AS
	SET NOCOUNT ON
	
	
	delete from TB_OPTION where (CHARACTER_IDX = @dwPlayerID)

	insert TB_OPTION
	(
		CHARACTER_IDX,
		HIDEFLAG,
		MOVETYPE
	)
	values
	(
		@dwPlayerID,
		@dwHideFlag,
		@wMoveType
	)
