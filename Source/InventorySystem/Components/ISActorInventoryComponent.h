// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "Types/ISInventoryTypes.h"

#include "ISActorInventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnItemGranted, FGameplayTag, itemName, float, itemStack, AActor*, ownerReference);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnItemUsed, FGameplayTag, itemName, float, itemStack, AActor*, ownerReference);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnItemRemoved, FGameplayTag, itemName, float, itemStack, AActor*, ownerReference);

/**
 *  Inventory component used to manage the inventory of players during the game.
 *  It covers the basic functions for items and requires that the player implements the GCInventoryInterface,
 *  to apply the desired effects of the Items.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class INVENTORYSYSTEM_API UISActorInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UISActorInventoryComponent(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Function called to add an item to the inventory with a specific stack
	UFUNCTION(BlueprintCallable, Category = "InventoryComponent")
	void AddItemToInventory(FGameplayTag itemTag, float itemStack);

	// Function called to use a specific stack of items from the inventory
	UFUNCTION(BlueprintCallable, Category = "InventoryComponent")
	void UseItemFromInventory(FGameplayTag itemTag, float itemStack);

	// Function called to drop a specific stack of items from the inventory
	UFUNCTION(BlueprintCallable, Category = "InventoryComponent")
	void DropItemFromInventory(FGameplayTag itemTag, float itemStack);

	// Function called to drop a specific stack of items from the inventory
	UFUNCTION(BlueprintCallable, Category = "InventoryComponent")
	void DropAllItemsFromInventory();

	// Function called to remove a specific stack of items from the inventory
	UFUNCTION(BlueprintCallable, Category = "InventoryComponent")
	void RemoveItemFromInventory(FGameplayTag itemTag, float itemStack);

	// Function to remove all the items from the inventory making sure that the owner perceives the changes
	UFUNCTION(BlueprintCallable, Category = "InventoryComponent")
	void RemoveAllItemsFromInventory();

	// Fast function to remove all elements in the inventory and empty it in a fast way.
	UFUNCTION(BlueprintCallable, Category = "InventoryComponent")
	void ClearInventory();

	// Returns true if the input item is in the inventory
	UFUNCTION(BlueprintCallable, Category = "InventoryComponent")
	bool IsItemInInventory(FGameplayTag itemTag) const;

	// Returns the current stack of the input item from the inventory. 0 if the player doesn't have the item.
	UFUNCTION(BlueprintCallable, Category = "InventoryComponent")
	float GetItemStack(FGameplayTag itemTag) const;

	// Returns the current stack of the input item from the inventory. 0 if the player doesn't have the item.
	UFUNCTION(BlueprintCallable, Category = "InventoryComponent")
	TMap<FGameplayTag, float> GetAllItemsOnInventory() const;

	//~ Crafting related functions

	// Function called to craft the desired item.
	UFUNCTION(BlueprintCallable, Category = "InventoryComponent|Crafting")
	void CraftItem(FGameplayTag itemTag);

	// Function called to craft the desired item.
	UFUNCTION(BlueprintCallable, Category = "InventoryComponent|Crafting")
	bool CanItemBeCrafted(FGameplayTag itemTag);

protected:

	bool IsItemCraftable(FItemRecipeElements recipe);

public:

	UPROPERTY(BlueprintAssignable)
	FOnItemGranted OnItemGranted;

	UPROPERTY(BlueprintAssignable)
	FOnItemUsed OnItemUsed;

	UPROPERTY(BlueprintAssignable)
	FOnItemRemoved OnItemRemoved;

protected:

	// Gameplay tags of the items that the player holds
	UPROPERTY(Replicated)
	FISGameplayTagStackContainer HeldItemTags;

	UPROPERTY(EditDefaultsOnly, Category = "InventoryComponent|Defaults")
	TMap<FGameplayTag, float> StartUpItems;
};
