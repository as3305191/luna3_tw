USE [LUNA_GAMEDB_2025]
GO
/****** Object:  StoredProcedure [dbo].[MP_STORAGE_Info]    Script Date: 8/14/2025 12:59:03 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
ALTER   proc [dbo].[MP_STORAGE_Info]
(
	@User_IDX	int
)

as

	Set Nocount On

	declare @pyogukmoney bigint
	declare @pyoguknum tinyint

	select @pyoguknum = PyogukNum, @pyogukmoney = PyogukMoney from tb_pyoguk where user_idx = @User_IDX
	if( @pyogukmoney ) < 0
		set @pyogukmoney = 0
	select @pyoguknum , @pyogukmoney
