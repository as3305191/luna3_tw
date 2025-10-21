USE [LUNA_LOGDB_2025]
GO

/****** Object:  Table [dbo].[TB_TransmogLog]    Script Date: 31/08/2025 13:51:07 ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO

CREATE TABLE [dbo].[TB_TransmogLog](
	[id] [int] IDENTITY(1,1) NOT NULL,
	[dummy_dbidx] [int] NOT NULL,
	[dummy_itemidx] [int] NOT NULL,
	[item_dbidx] [int] NOT NULL,
	[created_at] [datetime] NULL,
PRIMARY KEY CLUSTERED 
(
	[id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO

ALTER TABLE [dbo].[TB_TransmogLog] ADD  DEFAULT (getdate()) FOR [created_at]
GO


