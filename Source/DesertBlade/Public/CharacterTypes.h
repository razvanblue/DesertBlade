#pragma once

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	Unequipped UMETA(DisplayName = "Unequipped"),
	Equipped1HWeapon UMETA(DisplayName = "Equipped 1H Weapon"),
	Equipped2HWeapon UMETA(DisplayName = "Equipped 2H Weapon")
};

UENUM(BlueprintType)
enum class EActionState : uint8
{
	Unoccupied UMETA(DisplayName = "Unoccupied"),
	Attacking UMETA(DisplayName = "Attacking")
};