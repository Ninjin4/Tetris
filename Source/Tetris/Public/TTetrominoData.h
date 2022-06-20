 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "TTetrominoRotationRule.h"
#include "TTetrominoData.generated.h"

USTRUCT(BlueprintType)
struct FTetrominoData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<bool> Blocks;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FLinearColor Color;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ERotationRule RotationRule;
};
