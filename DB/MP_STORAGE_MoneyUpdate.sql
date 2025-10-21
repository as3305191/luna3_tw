USE [LUNA_GAMEDB_2025]
GO
/****** Object:  StoredProcedure [dbo].[MP_STORAGE_MoneyUpdate]    Script Date: 8/14/2025 12:20:34 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
ALTER proc [dbo].[MP_STORAGE_MoneyUpdate] (@User_idx int, @PyogukMoney bigint )
as
	Set Nocount On

update tb_pyoguk set pyogukmoney = @PyogukMoney where User_IDX  = @User_idx