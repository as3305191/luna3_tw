USE LUNA_GAMEDB_2025
GO
/****** Object:  StoredProcedure [dbo].[MP_ITEM_TRANSMOG]    Script Date: 30/08/2025 03:03:02 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE [dbo].[MP_ITEM_TRANSMOG]
(
	@ITEM_DBIDX int,
	@DUMMY_DBIDX int,
	@DUMMY_3D int
)

AS
	SET NOCOUNT ON
	
	UPDATE TB_ITEM 
	SET TRANSMOG_IDX = @DUMMY_3D
	WHERE ITEM_DBIDX = @ITEM_DBIDX
	
	insert into luna_logdb_v2.dbo.TB_TransmogLog(item_dbidx,dummy_dbidx , dummy_itemidx)
	values(@ITEM_DBIDX , @DUMMY_DBIDX , @DUMMY_3D)
	RETURN

