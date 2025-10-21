USE [LUNA_GAMEDB_2025]
GO
/****** Object:  StoredProcedure [dbo].[MP_SKILLTREE_SkillMoney_Update]    Script Date: 8/14/2025 12:22:06 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author		: Firman Akbar
-- Create date	: <Create Date,,>
-- Description	: SKILL RESET RETURN MONEY
-- =============================================
CREATE PROCEDURE [dbo].[MP_SKILLTREE_SkillMoney_Update] 
(
	@characteridx	int,
	@money			bigint
)	
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;
	
	DECLARE @checkeksis int 
	DECLARE @curentskillmoney int

	select @checkeksis = COUNT(1) from dbo.TB_SKILLMONEY where character_idx = @characteridx
	IF (@checkeksis = 0)
	begin
		insert into TB_SKILLMONEY (character_idx, skillmoney) values (@characteridx, '0')
	end 
		
	select @curentskillmoney = skillmoney from dbo.TB_SKILLMONEY where character_idx = @characteridx
		
	select @curentskillmoney += @money;
		
	update TB_SKILLMONEY set skillmoney = @curentskillmoney where character_idx = @characteridx
END