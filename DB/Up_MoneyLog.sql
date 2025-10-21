USE [LUNA_LOGDB_2025]
GO
/****** Object:  StoredProcedure [dbo].[Up_MoneyLog]    Script Date: 8/14/2025 12:25:34 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
ALTER  proc [dbo].[Up_MoneyLog] 
	( @LogType smallint, @CharNo int, @InvenMoney bigint, @ChangeValue bigint,
	 @TargetID int, @TargetMoney bigint )
as
	Set Nocount on
--	Set Xact_Abort on

insert tb_moneylog
(
LogType,
CharNo,
InvenMoney,
ChangeValue,
TargetIdx,
TargetMoney
)
values
(
@LogType,
@CharNo,
@InvenMoney,
@ChangeValue,
@TargetID,
@TargetMoney
)
