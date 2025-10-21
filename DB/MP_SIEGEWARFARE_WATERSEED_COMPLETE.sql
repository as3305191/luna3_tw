
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		<Author,,Name>
-- Create date: <Create Date,,>
-- Description:	<Description,,>
-- =============================================
CREATE PROCEDURE [dbo].[MP_SIEGEWARFARE_WATERSEED_COMPLETE](
        @P_ID	int,
        @MAP_TYPE	int,
        @GUILD_IDX	int
        )
    AS
    SET NOCOUNT ON
    SET XACT_ABORT ON
    DELETE FROM SIEGEWARFARE_WATERSEED_COMPLETE where MAPTYPE=@MAP_TYPE;
    INSERT INTO SIEGEWARFARE_WATERSEED_COMPLETE VALUES (@P_ID, @MAP_TYPE, @GUILD_IDX);
    SELECT ID_IDX, P_IDX, MAPTYPE, GUILDIDX FROM SIEGEWARFARE_WATERSEED_COMPLETE WHERE MAPTYPE = @MAP_TYPE;


