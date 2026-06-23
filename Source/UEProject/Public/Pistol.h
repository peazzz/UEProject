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
    float FireRate = 0.7f;

    // 開火處理函數，宣告為 BlueprintCallable 才能在藍圖中呼叫
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void StartFire();

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void StopFire();

    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    TSubclassOf<AActor> BulletClass;

    // 射線檢測最遠距離（公尺）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float TraceRange = 10000.0f;

    // 直接命中造成的傷害
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float Damage = 20.0f;

    // (K2_OnHit 已移除，藍圖可改用其他事件處理視覺或音效)

protected:
    // C++ 內部的開火邏輯
    void PerformFire();

    // (已移除 K2_PerformFire，開火視覺/音效請在其他藍圖事件處理)

public:	
    FTimerHandle FireTimerHandle;

};
