USE [LUNA_GAMEDB_2025]
GO
/****** Object:  StoredProcedure [dbo].[MP_CHARACTER_UPDATE_ON_MAPOUT_WITHOUT_MONEY]    Script Date: 8/14/2025 12:23:10 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
/*
081218 LUJ, ? ?? ? ???? ?? ??? ????
081219 LUJ, ???? ??? ?? ????? ??? ? ??. ?? ????? ???
*/
CREATE    proc [dbo].[MP_CHARACTER_UPDATE_ON_MAPOUT_WITHOUT_MONEY]
(
	@CHAR_IDX		INT,
	@USER_IDX		INT,
	@MAP			INT,
	@CHANNEL		INT
)
as
	SET NOCOUNT ON
	SET XACT_ABORT ON

	SELECT	1,
			@MAP,
			@CHANNEL
	RETURN


--------------------
-- ?? ???? ??
--------------------
SET ANSI_NULLS ON