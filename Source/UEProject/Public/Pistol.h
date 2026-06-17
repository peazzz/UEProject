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
    float FireRate = 0.2f;

    // 開火處理函數，宣告為 BlueprintCallable 才能在藍圖中呼叫
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void StartFire();

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void StopFire();

    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    TSubclassOf<AActor> BulletClass;

protected:
    // C++ 內部的開火邏輯
    void PerformFire();

    // 這是給藍圖實作的事件，用來處理具體的 SpawnActor
    // 在藍圖中你可以建立 "Event K2_PerformFire"
    // Pistol.h
    UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
    void K2_PerformFire();

public:	
    FTimerHandle FireTimerHandle;

};
