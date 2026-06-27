// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelManager.generated.h"

UCLASS()
class UEPROJECT_API ALevelManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 開始新的一關
	void StartNextLevel();

	// 固定頻率被 Timer 呼叫，用來分配生怪
	void TriggerSpawnWave();

public:
	// 被敵人呼叫：當有敵人死亡時
	void OnEnemyKilled();

protected:
	// 要生成的敵人藍圖類別
	UPROPERTY(EditAnywhere, Category = "Level System")
	TSubclassOf<class AEnemy> EnemyClassToSpawn;

	// 每一隻怪生成的間隔時間
	UPROPERTY(EditAnywhere, Category = "Level System")
	float SpawnInterval = 2.0f;

	// 關卡之間的休息/準備時間（秒）
	UPROPERTY(EditAnywhere, Category = "Level System")
	float DelayBetweenLevels = 5.0f;

	UFUNCTION(BlueprintImplementableEvent, Category = "Level System")
	void BP_ShowWaveUI(int32 WaveNumber);

private:
	int32 CurrentLevel = 0;         // 當前關卡 n
	int32 TotalEnemiesThisLevel = 0;// 本關總共要生成的數量 (n*n - n)
	int32 EnemiesSpawnedSoFar = 0;  // 本關目前已生成的數量
	int32 ActiveEnemyCount = 0;     // 目前場上還活著的數量

	FTimerHandle SpawnTimerHandle;
	FTimerHandle NextLevelTimerHandle;

	// 儲存場上所有的 Spawner 參照
	UPROPERTY()
	TArray<class AEnemySpawner*> FoundSpawners;

};
