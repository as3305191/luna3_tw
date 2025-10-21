USE [LUNA_GAMEDB_2025]
GO
/****** Object:  StoredProcedure [dbo].[MP_CONSIGNMENT_Insert]    Script Date: 8/14/2025 12:16:38 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
ALTER    PROC [dbo].[MP_CONSIGNMENT_Insert] 
(
	@PlayerIndex int,
	@ItemDBIndex int,
	@ItemIndex int,
	@CharacterName varchar(16),
	@ItemName varchar(32),
	@Category_1 int,
	@Category_2 int,
	@RareLevel tinyint,
	@UseLevel int,
	@Enchant tinyint,
	@Deposit bigint,
	@Commission bigint,
	@Price bigint,
	@FromPos int,
	@Stack int
) 
AS 
SET  NOCOUNT  ON 

declare @ErrorIndex int
declare @CharacterIdx int
declare @Durability int
declare @ConsignmentIdx int
declare @InitDurability int

set @ErrorIndex = 0
set @CharacterIdx = 0

if (select count(*) from [dbo].[TB_CONSIGNMENT] where [ITEM_DBIDX] = @ItemDBIndex) > 0
begin
	set @ErrorIndex = -1
	select @ErrorIndex, @ItemDBIndex, @ItemIndex, @Deposit, @FromPos
	return
end

select	@CharacterIdx = [CHARACTER_IDX],
		@Durability = [ITEM_DURABILITY]
from [dbo].[TB_ITEM] with (nolock)
where [ITEM_DBIDX] = @ItemDBIndex

if(@CharacterIdx <> @PlayerIndex)
begin
	set @ErrorIndex = -1
	select @ErrorIndex, @ItemDBIndex, @ItemIndex, @Deposit, @FromPos
	return
end

if(@Stack = 0)
	begin
	set @InitDurability = 0
	end
else
	begin
	set @InitDurability = @Durability
	end


-- ??? ?? ?? - 10??? ????
if (select count(*) from [dbo].[TB_CONSIGNMENT] where [CHARACTER_IDX] = @CharacterIdx) >= 10
begin
	set @ErrorIndex = -4 --eConsignmentDBResult_RegistedOver
	select @ErrorIndex, @ItemDBIndex, @ItemIndex, 0, 0
	return
end

INSERT [dbo].[TB_CONSIGNMENT]
	([ITEM_DBIDX],
	[ITEM_INDEX],
	[CHARACTER_IDX],
	[CHARACTER_NAME],
	[ITEM_NAME],
	[CATEGORY_1],
	[CATEGORY_2],
	[RARELEVEL],
	[USELEVEL],
	[ENCHANT],
	[REGISTTIME],
	[ENDTIME],
	[INIT_DURABILITY],
	[DEPOSIT],
	[COMMISSION],
	[PRICE] )
values
	( @ItemDBIndex,
	@ItemIndex,
	@PlayerIndex,
	@CharacterName,
	@ItemName,
	@Category_1,
	@Category_2,
	@RareLevel,
	@UseLevel,
	@Enchant,
	getdate(),
	dateadd(hour, 24*7, getdate()),
	@InitDurability,
	@Deposit,
	@Commission,
	@Price )

if @@error<>0 or @@rowcount<>1
	begin
	select -1, @ItemDBIndex, @ItemIndex, @Deposit, @FromPos
	return
	end

-- ????? ???? ??? ????.
select @ConsignmentIdx = [CONSIGNMENT_IDX]
	from [dbo].[TB_CONSIGNMENT] with (nolock)
	where [ITEM_DBIDX] = @ItemDBIndex

if @@error<>0 or @@rowcount<>1 or @ConsignmentIdx=0
	begin
	select -1, @ItemDBIndex, @ItemIndex, @Deposit, @FromPos
	return
	end

-- TB_ITEM ? CHARACTER_IDX=0 ?? ????.
update [dbo].[TB_ITEM]
	set [CHARACTER_IDX] = 0
where [ITEM_DBIDX] = @ItemDBIndex

select @ConsignmentIdx, @ItemDBIndex, @ItemIndex, @Deposit, @FromPos, @Price
return

------------------------------
-- ???? ?? ???? ??
------------------------------

set ANSI_NULLS ON
set QUOTED_IDENTIFIER ON
