// Copyright Piotr Jeżyna. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

namespace UE::MassFlock::ProcessorGroupNames
{
	const FName Flock = FName(TEXT("Flock"));
}

namespace UE::MassFlock::Utilities
{
	static void FindCloseObstacles(const FVector& Center, const float SearchRadius, const FNavigationObstacleHashGrid2D& AvoidanceObstacleGrid,
									TArray<FMassNavigationObstacleItem, TFixedAllocator<21>>& OutCloseEntities, const int32 MaxResults)
	{
		OutCloseEntities.Reset();
		const FVector Extent(SearchRadius, SearchRadius, 0.f);
		const FBox QueryBox = FBox(Center - Extent, Center + Extent);

		struct FSortingCell
		{
			int32 X;
			int32 Y;
			int32 Level;
			float SqDist;
		};
		TArray<FSortingCell, TInlineAllocator<64>> Cells;
		const FVector QueryCenter = QueryBox.GetCenter();
	
		for (int32 Level = 0; Level < AvoidanceObstacleGrid.NumLevels; Level++)
		{
			const float CellSize = AvoidanceObstacleGrid.GetCellSize(Level);
			const FNavigationObstacleHashGrid2D::FCellRect Rect = AvoidanceObstacleGrid.CalcQueryBounds(QueryBox, Level);
			for (int32 Y = Rect.MinY; Y <= Rect.MaxY; Y++)
			{
				for (int32 X = Rect.MinX; X <= Rect.MaxX; X++)
				{
					const float CenterX = (X + 0.5f) * CellSize;
					const float CenterY = (Y + 0.5f) * CellSize;
					const float DX = CenterX - QueryCenter.X;
					const float DY = CenterY - QueryCenter.Y;
					const float SqDist = DX * DX + DY * DY;
					FSortingCell SortCell;
					SortCell.X = X;
					SortCell.Y = Y;
					SortCell.Level = Level;
					SortCell.SqDist = SqDist;
					Cells.Add(SortCell);
				}
			}
		}

		Cells.Sort([](const FSortingCell& A, const FSortingCell& B) { return A.SqDist < B.SqDist; });

		for (const FSortingCell& SortedCell : Cells)
		{
			if (const FNavigationObstacleHashGrid2D::FCell* Cell = AvoidanceObstacleGrid.FindCell(SortedCell.X, SortedCell.Y, SortedCell.Level))
			{
				const TSparseArray<FNavigationObstacleHashGrid2D::FItem>&  Items = AvoidanceObstacleGrid.GetItems();
				for (int32 Idx = Cell->First; Idx != INDEX_NONE; Idx = Items[Idx].Next)
				{
					OutCloseEntities.Add(Items[Idx].ID);
					if (OutCloseEntities.Num() >= MaxResults)
					{
						return;
					}
				}
			}
		}
	}
}