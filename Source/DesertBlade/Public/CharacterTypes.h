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
	Attacking UMETA(DisplayName = "Attacking"),
	Equipping UMETA(DisplayName = "Equipping")
};

UENUM(BlueprintType)
enum class EDeathPose : uint8
{
	DeadRight UMETA(DisplayName = "Dead Right"),
	DeadLeft UMETA(DisplayName = "Dead Left"),
	DeadForward UMETA(DisplayName = "Dead Forward"),
	DeadBack UMETA(DisplayName = "Dead Back")
};


UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	Dead UMETA(DisplayName = "Dead"),
	Patrolling UMETA(DisplayName = "Patrolling"),
	Chasing UMETA(DisplayName = "Chasing"),
	Engaged UMETA(DisplayName = "Engaged"),
	Attacking UMETA(DisplayName = "Attacking")
};