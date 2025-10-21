USE [LUNA_LOGDB_2025]
GO
/****** Object:  StoredProcedure [dbo].[up_ItemMoneyLog]    Script Date: 8/14/2025 12:25:00 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
/*
080114 웅주, 오늘날짜에 해당하는 테이블에 로그를 저장한다
*/
ALTER  PROCEDURE [dbo].[up_ItemMoneyLog] (
	@FROMCHRIDX		INT, 
	@FROMCHRNAME	VARCHAR(16), 
	@TOCHRIDX		INT, 
	@TOCHRNAME		VARCHAR(16), 
	@LOGTYPE		SMALLINT,
	@FROMTOTALMONEY BIGINT,
	@TOTOTALMONEY	BIGINT,
	@CHANGEMONEY	BIGINT,
	@ITEMIDX		INT,
	@ITEMDBIDX		INT,
	@ITEMFROMPOS	INT,
	@ITEMTOPOS		INT,
	@ITEMDUR		BIGINT,
	@CHREXPPOINT	BIGINT
)
AS
	SET NOCOUNT ON
	SET XACT_ABORT ON

	--RETURN

	DECLARE @TABLE VARCHAR( 20 )
	SET @TABLE = 'TB_ITEM_LOG_' + CONVERT( VARCHAR( 8 ), GETDATE(), 112 )
	--firman fix
	--If table not exists, create it
	DECLARE @SQL NVARCHAR( max )
	SET @SQL = 
	'IF OBJECT_ID(''dbo.['+@TABLE+']'',''U'') IS NOT NULL
	BEGIN
		insert ' + @TABLE +
				' ( LogType, FromChrName, FromChrIdx, ToChrName, ToChrIdx, FromTotalMoney, ToTotalMoney, ChangeMoney,' +
				'	ItemIdx, ItemDBIdx, ItemFromPos, ItemToPos, ItemDur, ChrExpPoint ) VALUES (' +
				CAST( @LogType			AS NVARCHAR( MAX ) ) + ',' +
				'''' + @FromChrName	+ ''',' +
				CAST( @FromChrIdx		AS NVARCHAR( MAX ) ) + ',' +
				'''' + @ToChrName	+ ''',' +
				CAST( @ToChrIdx			AS NVARCHAR( MAX ) ) + ',' +
				CAST( @FromTotalMoney	AS NVARCHAR( MAX ) ) + ',' +
				CAST( @ToTotalMoney		AS NVARCHAR( MAX ) ) + ',' +
				CAST( @ChangeMoney		AS NVARCHAR( MAX ) ) + ',' +
				CAST( @ItemIdx			AS NVARCHAR( MAX ) ) + ',' +
				CAST( @ItemDBIdx		AS NVARCHAR( MAX ) ) + ',' +
				CAST( @ItemFromPos		AS NVARCHAR( MAX ) ) + ',' +
				CAST( @ItemToPos		AS NVARCHAR( MAX ) ) + ',' +
				CAST( @ItemDur			AS NVARCHAR( MAX ) ) + ',' +
				CAST( @ChrExpPoint		AS NVARCHAR( MAX ) ) + ')
	END
	ELSE
	BEGIN 
		CREATE TABLE [DBO].['+@TABLE+']('+'
		[LOGIDX] [int] IDENTITY(1,1) NOT NULL,'+'
		[LOGTYPE] [smallint] NOT NULL,'+'
		[FROMCHRIDX] [int] NOT NULL,'+'
		[FROMCHRNAME] [varchar](16) NOT NULL,'+'
		[TOCHRIDX] [int] NOT NULL,'+'
		[TOCHRNAME] [varchar](16) NOT NULL,'+'
		[FROMTOTALMONEY] [bigint] NOT NULL,'+'
		[TOTOTALMONEY] [bigint] NULL,'+'
		[CHANGEMONEY] [bigint] NULL,'+'
		[ITEMIDX] [int] NULL,'+'
		[ITEMDBIDX] [int] NULL,'+'
		[ITEMDUR] [bigint] NULL,'+'
		[ITEMFROMPOS] [int] NULL,'+'
		[ITEMTOPOS] [int] NULL,'+'
		[REGDATE] [datetime] NOT NULL DEFAULT GETDATE(),'+'
		[CHREXPPOINT] [bigint] NOT NULL)
		
	insert ' + @TABLE +
				' ( LogType, FromChrName, FromChrIdx, ToChrName, ToChrIdx, FromTotalMoney, ToTotalMoney, ChangeMoney,' +
				'	ItemIdx, ItemDBIdx, ItemFromPos, ItemToPos, ItemDur, ChrExpPoint ) VALUES (' +
				CAST( @LogType			AS NVARCHAR( MAX ) ) + ',' +
				'''' + @FromChrName	+ ''',' +
				CAST( @FromChrIdx		AS NVARCHAR( MAX ) ) + ',' +
				'''' + @ToChrName	+ ''',' +
				CAST( @ToChrIdx			AS NVARCHAR( MAX ) ) + ',' +
				CAST( @FromTotalMoney	AS NVARCHAR( MAX ) ) + ',' +
				CAST( @ToTotalMoney		AS NVARCHAR( MAX ) ) + ',' +
				CAST( @ChangeMoney		AS NVARCHAR( MAX ) ) + ',' +
				CAST( @ItemIdx			AS NVARCHAR( MAX ) ) + ',' +
				CAST( @ItemDBIdx		AS NVARCHAR( MAX ) ) + ',' +
				CAST( @ItemFromPos		AS NVARCHAR( MAX ) ) + ',' +
				CAST( @ItemToPos		AS NVARCHAR( MAX ) ) + ',' +
				CAST( @ItemDur			AS NVARCHAR( MAX ) ) + ',' +
				CAST( @ChrExpPoint		AS NVARCHAR( MAX ) ) + ')
	END'
--	PRINT( @SQL )
	EXEC( @SQL )
	RETURN


