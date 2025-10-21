USE [LUNA_GAMEDB_2025]
GO

/****** Object:  Table [dbo].[TB_SKILLMONEY]    Script Date: 8/14/2025 12:24:07 AM ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO

CREATE TABLE [dbo].[TB_SKILLMONEY](
	[character_idx] [int] NOT NULL,
	[skillmoney] [bigint] NOT NULL
) ON [PRIMARY]
GO


