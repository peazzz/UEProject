// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

UCLASS()
class UEPROJECT_API AEnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemySpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SpawnEnemy();

protected:
	// 1. 要生成的敵人 Blueprint 類別
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner")
	TSubclassOf<class AEnemy> EnemyClassToSpawn;

	// 2. 固定頻率（秒）
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner")
	float SpawnInterval = 3.0f;

	// 3. 預計生成的總數量（設為 -1 代表無限生成）
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner")
	int32 MaxEnemyCount = 5;

	// 4. 自訂生成的座標點清單（相對於 Spawner 或世界座標）
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner", meta = (MakeEditWidget = true))
	TArray<FVector> SpawnLocations;

private:
	FTimerHandle SpawnTimerHandle;
	int32 CurrentSpawnedCount = 0;
};
