// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemySpawner.h"
#include "Enemy.h" // 引入你的敵人標頭檔
#include "TimerManager.h"
#include "Engine/World.h"

// Sets default values
AEnemySpawner::AEnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	
	// 如果沒有指定敵人藍圖，或者沒有設定座標，就不啟動計時器
	if (!EnemyClassToSpawn || SpawnLocations.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("EnemySpawner: Missing EnemyClass or SpawnLocations!"));
		return;
	}

	// 啟動固定頻率的計時器 (Loop = true)
	GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &AEnemySpawner::SpawnEnemy, SpawnInterval, true);
}

void AEnemySpawner::SpawnEnemy()
{
	// 檢查是否達到了最大生成數量
	if (MaxEnemyCount >= 0 && CurrentSpawnedCount >= MaxEnemyCount)
	{
		// 達到上限，關閉計時器
		GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
		UE_LOG(LogTemp, Log, TEXT("EnemySpawner: Reached MaxEnemyCount. Stopping spawner."));
		return;
	}

	UWorld* World = GetWorld();
	if (World)
	{
		// 輪流或隨機從你的「自訂座標清單」中選一個點
		// 這裡示範用「當前數量 % 座標總數」來輪流在不同點生成
		int32 LocationIndex = CurrentSpawnedCount % SpawnLocations.Num();

		// 將相對座標轉換為世界座標（加上 Spawner 本身的位置）
		FVector RelativeLocation = SpawnLocations[LocationIndex];
		FVector TargetWorldLocation = GetActorLocation() + GetTransform().TransformVector(RelativeLocation);

		// 設定生成的旋轉（讓敵人面向跟 Spawner 一致）
		FRotator TargetRotation = GetActorRotation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		// 生成敵人
		AEnemy* SpawnedEnemy = World->SpawnActor<AEnemy>(EnemyClassToSpawn, TargetWorldLocation, TargetRotation, SpawnParams);

		if (SpawnedEnemy)
		{
			CurrentSpawnedCount++;
			UE_LOG(LogTemp, Log, TEXT("EnemySpawner: Spawned enemy %d/%d"), CurrentSpawnedCount, MaxEnemyCount);
		}
	}
}

