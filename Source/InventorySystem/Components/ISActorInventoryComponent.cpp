// Fill out your copyright notice in the Description page of Project Settings.

#include "ISActorInventoryComponent.h"
#include "Interfaces/ISInventoryInterface.h"
#include "Subsystems/ISInventoryGISSubsystems.h"
#include <Net/UnrealNetwork.h>

UISActorInventoryComponent::UISActorInventoryComponent(const FObjectInitializer& ObjectInitializer)
{
	HeldItemTags = FISGameplayTagStackContainer();
	StartUpItems.Empty();

	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UISActorInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	if (StartUpItems.Num() > 0)
	{
		for (const auto& currentItem : StartUpItems)
		{
			AddItemToInventory(currentItem.Key, currentItem.Value);
		}
	}
}

void UISActorInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, HeldItemTags);
}

void UISActorInventoryComponent::AddItemToInventory(FGameplayTag itemTag, float itemStack)
{
	const auto ownerActor = GetOwner();

	if (ownerActor && ownerActor->GetClass()->ImplementsInterface(UISInventoryInterface::StaticClass()))
	{
		HeldItemTags.AddStack(itemTag, itemStack);

		IISInventoryInterface::Execute_ItemGranted(ownerActor, itemTag, itemStack);

		OnItemGranted.Broadcast(itemTag, itemStack, ownerActor);
	}
}

void UISActorInventoryComponent::UseItemFromInventory(FGameplayTag itemTag, float itemStack)
{
	const auto ownerActor = GetOwner();

	if (ownerActor && ownerActor->GetClass()->ImplementsInterface(UISInventoryInterface::StaticClass()) && IsItemInInventory(itemTag))
	{
		IISInventoryInterface::Execute_ItemUsed(ownerActor, itemTag, itemStack);

		OnItemUsed.Broadcast(itemTag, itemStack, ownerActor);
	}
}

void UISActorInventoryComponent::DropItemFromInventory(FGameplayTag itemTag, float itemStack)
{
	const auto ownerActor = GetOwner();

	if (ownerActor && ownerActor->GetClass()->ImplementsInterface(UISInventoryInterface::StaticClass()) && IsItemInInventory(itemTag))
	{
		HeldItemTags.RemoveStack(itemTag, itemStack);

		IISInventoryInterface::Execute_ItemDropped(ownerActor, itemTag, itemStack);

		OnItemRemoved.Broadcast(itemTag, itemStack, ownerActor);
	}
}

void UISActorInventoryComponent::DropAllItemsFromInventory()
{
	const auto heldItems = HeldItemTags.GetGameplayTagStackList();

	if (heldItems.Num() > 0)
	{
		for (const auto& itemStack : heldItems)
		{
			DropItemFromInventory(itemStack.GetGameplayTag(), itemStack.GetStackCount());
		}
	}
}

void UISActorInventoryComponent::RemoveItemFromInventory(FGameplayTag itemTag, float itemStack)
{
	const auto ownerActor = GetOwner();

	if (ownerActor && ownerActor->GetClass()->ImplementsInterface(UISInventoryInterface::StaticClass()) && IsItemInInventory(itemTag))
	{
		HeldItemTags.RemoveStack(itemTag, itemStack);

		IISInventoryInterface::Execute_ItemRemoved(ownerActor, itemTag, itemStack);

		OnItemRemoved.Broadcast(itemTag, itemStack, ownerActor);
	}
}

void UISActorInventoryComponent::RemoveAllItemsFromInventory()
{
	const auto heldItems = HeldItemTags.GetGameplayTagStackList();

	if (heldItems.Num() > 0)
	{
		for (const auto& itemStack : heldItems)
		{
			RemoveItemFromInventory(itemStack.GetGameplayTag(), itemStack.GetStackCount());
		}
	}
}

void UISActorInventoryComponent::ClearInventory()
{
	HeldItemTags.ClearStack();
}

bool UISActorInventoryComponent::IsItemInInventory(FGameplayTag itemTag) const
{
	return HeldItemTags.ContainsTag(itemTag);
}

float UISActorInventoryComponent::GetItemStack(FGameplayTag itemTag) const
{
	return HeldItemTags.GetStackCount(itemTag);
}

TMap<FGameplayTag, float> UISActorInventoryComponent::GetAllItemsOnInventory() const
{
	TMap<FGameplayTag, float> currentItemsMap;

	const auto currentItemsArray = HeldItemTags.GetGameplayTagStackList();

	if (currentItemsArray.Num() > 0)
	{
		for (const auto& itemStack : currentItemsArray)
		{
			currentItemsMap.Add(itemStack.GetGameplayTag(), itemStack.GetStackCount());
		}
	}

	return currentItemsMap;
}

void UISActorInventoryComponent::CraftItem(FGameplayTag itemTag)
{
	const auto ownerActor = GetOwner();

	if (auto inventorySubsystem = UISInventoryGISSubsystems::Get(ownerActor))
	{
		const auto itemRecipe = inventorySubsystem->GetItemRecipe(itemTag);

		if (IsItemCraftable(itemRecipe))
		{
			for (const auto& recipeElement : itemRecipe.RecipeElements)
			{
				RemoveItemFromInventory(recipeElement.Key, recipeElement.Value);
			}

			AddItemToInventory(itemTag, 1.f);
		}
	}
}

bool UISActorInventoryComponent::CanItemBeCrafted(FGameplayTag itemTag)
{
	const auto ownerActor = GetOwner();

	if (auto inventorySubsystem = UISInventoryGISSubsystems::Get(ownerActor))
	{
		const auto itemRecipe = inventorySubsystem->GetItemRecipe(itemTag);

		return IsItemCraftable(itemRecipe);
	}

	return false;
}

bool UISActorInventoryComponent::IsItemCraftable(FItemRecipeElements recipe)
{
	bool bHasMaterials = true;

	if (recipe.RecipeElements.Num() > 0)
	{
		for (const auto& recipeElement : recipe.RecipeElements)
		{
			if (GetItemStack(recipeElement.Key) < recipeElement.Value)
			{
				bHasMaterials = false;
				break;
			}
		}
	}
	else
	{
		bHasMaterials = false;
	}

	return bHasMaterials;
}