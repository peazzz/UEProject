// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameType.generated.h" // UENUM

UENUM(BlueprintType)
enum class EUpgradeType : uint8
{
    None                    UMETA(DisplayName = "None"),
    Pistol_FireRate        UMETA(DisplayName = "Increase Fire Rate"),
    Pistol_Damage          UMETA(DisplayName = "Increase Damage"),
    Pistol_MaxAmmo         UMETA(DisplayName = "Increase Max Ammo"),
    Character_MaxHealth    UMETA(DisplayName = "Increase Max Health"),
    Character_MaxWalkSpeed UMETA(DisplayName = "Increase Movement Speed")
};
