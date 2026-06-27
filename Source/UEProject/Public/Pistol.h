// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pistol.generated.h"

UCLASS()
class UEPROJECT_API APistol : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	APistol();

    // 設定開火頻率，在編輯器中可以修改
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float FireRate = 0.67f;

    // 開火處理函數，宣告為 BlueprintCallable 才能在藍圖中呼叫
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void StartFire();

    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    TSubclassOf<AActor> BulletClass;

    // 射線檢測最遠距離（公尺）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float TraceRange = 10000.0f;

    // 直接命中造成的傷害
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float Damage = 20.0f;

    // 每個彈匣的最大子彈數（預設 12）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Ammo")
    int32 MaxAmmo = 12;

    // 當前彈匣內的子彈數（BlueprintReadOnly 讓藍圖可以讀取它來判斷或做 UI）
    UPROPERTY(BlueprintReadOnly, Category = "Combat|Ammo")
    int32 CurrentAmmo;

    // 給藍圖檢查與扣除子彈用。如果成功扣除返回 true，若沒子彈返回 false
    UFUNCTION(BlueprintCallable, Category = "Combat|Ammo")
    bool ConsumeAmmo();

    // 給藍圖換彈結束時呼叫，用來將子彈補滿
    UFUNCTION(BlueprintCallable, Category = "Combat|Ammo")
    void ReloadAmmo();

    void UpdateWeaponAmmoUI();

protected:
    virtual void BeginPlay() override;
    // C++ 內部的開火邏輯
    void PerformFire();
};
