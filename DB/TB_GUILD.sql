USE [luna_gamedb_2025]
GO

/****** Object:  Table [dbo].[TB_GUILD] ******/
/****** Set BigInt For Money Column ******/
ALTER TABLE [dbo].[TB_GUILD] DROP  CONSTRAINT [DF_TB_GUILD_GuildMoney]
GO

ALTER TABLE [dbo].[TB_GUILD] ALTER  COLUMN [GuildMoney]  [bigint] NOT NULL
GO

ALTER TABLE [dbo].[TB_GUILD] ADD  CONSTRAINT [DF_TB_GUILD_GuildMoney]  DEFAULT ((0)) FOR [GuildMoney]
GO