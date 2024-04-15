// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Types/ISInventoryTypes.h"
#include <Engine/DataAsset.h>

#include "ISInventoryMappingDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYSYSTEM_API UISInventoryMappingDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintPure, meta=(AutoCreateRefTerm="categoryTag"))
	UDataTable* FindItemsDataTable(const FGameplayTag& categoryTag) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (AutoCreateRefTerm = "categoryTag"))
	FItemRecipeInfo FindRecipesCategory(const FGameplayTag& categoryTag) const;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<FGameplayTag, UDataTable*> ItemsCategoryMap;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<FGameplayTag, FItemRecipeInfo> ItemsCategoryCraftingRecipes;
	
};
