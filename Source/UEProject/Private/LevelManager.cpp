// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelManager.h"
#include "Enemy.h"
#include "EnemySpawner.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

// Sets default values
ALevelManager::ALevelManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ALevelManager::BeginPlay()
{
	Super::BeginPlay();

	// 1. 自動搜尋場上所有的 EnemySpawner
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(this, AEnemySpawner::StaticClass(), OutActors);
	for (AActor* Actor : OutActors)
	{
		if (AEnemySpawner* Spawner = Cast<AEnemySpawner>(Actor))
		{
			FoundSpawners.Add(Spawner);
		}
	}

	if (FoundSpawners.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("LevelManager: 場上沒有放置任何 EnemySpawner！"));
		return;
	}

	// 2. 遊戲開始，延遲一下後進入第 1 關
	CurrentLevel = 0;
	BP_ShowWaveUI(CurrentLevel + 1);
	GetWorldTimerManager().SetTimer(NextLevelTimerHandle, this, &ALevelManager::StartNextLevel, DelayBetweenLevels, false);
}

void ALevelManager::StartNextLevel()
{
	CurrentLevel++; // 進入第 n 關

	// 公式：n * n - n
	TotalEnemiesThisLevel = (CurrentLevel * CurrentLevel) - CurrentLevel;

	// 為了防呆：如果第 1 關計算出來是 0 隻（1*1 - 1 = 0），我們可以強制給它 1 隻或跳過
	if (TotalEnemiesThisLevel <= 0)
	{
		TotalEnemiesThisLevel = 1; // 或者維持 0 直接進下一關，這裡防呆設為 2 隻
	}

	EnemiesSpawnedSoFar = 0;
	ActiveEnemyCount = 0;

	UE_LOG(LogTemp, Log, TEXT("======== 關卡 %d 開始！目標敵人總數: %d ========"), CurrentLevel, TotalEnemiesThisLevel);

	// 啟動計時器，固定頻率呼叫分配生怪的邏輯
	GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ALevelManager::TriggerSpawnWave, SpawnInterval, true);
}

void ALevelManager::TriggerSpawnWave()
{
	if (EnemiesSpawnedSoFar >= TotalEnemiesThisLevel)
	{
		// 已經把這一關該生成的數量派發完畢，關閉生成計時器
		GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
		return;
	}

	UWorld* World = GetWorld();
	if (!World || FoundSpawners.Num() == 0) return;

	// 【平均分配的核心邏輯】
	// 每次觸發時，讓「所有的 Spawner 同時生一隻怪」，直到總數達到上限為止
	for (AEnemySpawner* Spawner : FoundSpawners)
	{
		if (EnemiesSpawnedSoFar < TotalEnemiesThisLevel)
		{
			// 從 Spawner 取得它身上的自訂座標（利用 Spawner 的輪流機制或隨機點）
			// 為了精簡程式碼，我們可以直接在 Manager 裡調用 World->SpawnActor，但座標用 Spawner 的位置

			// 取得該 Spawner 的世界座標
			FVector SpawnLocation = Spawner->GetActorLocation();
			FRotator SpawnRotation = Spawner->GetActorRotation();

			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			AEnemy* NewEnemy = World->SpawnActor<AEnemy>(EnemyClassToSpawn, SpawnLocation, SpawnRotation, SpawnParams);
			if (NewEnemy)
			{
				NewEnemy->LevelManagerRef = this; // 註冊管理器參照
				EnemiesSpawnedSoFar++;
				ActiveEnemyCount++;
			}
		}
		else
		{
			break; // 數量滿了，停止生成
		}
	}
}

void ALevelManager::OnEnemyKilled()
{
	ActiveEnemyCount--;
	UE_LOG(LogTemp, Log, TEXT("敵人死亡！剩餘活著的敵人: %d"), ActiveEnemyCount);

	// 檢查是否「全部生完」而且「全部死光」
	if (EnemiesSpawnedSoFar >= TotalEnemiesThisLevel && ActiveEnemyCount <= 0)
	{
		UE_LOG(LogTemp, Log, TEXT("======== 關卡 %d 通關！ ========"), CurrentLevel);

		BP_ShowWaveUI(CurrentLevel + 1);

		// 倒數計時進入下一關
		GetWorldTimerManager().SetTimer(NextLevelTimerHandle, this, &ALevelManager::StartNextLevel, DelayBetweenLevels, false);
	}
}

