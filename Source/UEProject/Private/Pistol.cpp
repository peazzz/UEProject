// Fill out your copyright notice in the Description page of Project Settings.


#include "Pistol.h"
#include "TimerManager.h"

// Sets default values
APistol::APistol()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void APistol::StartFire()
{
    // 防止計時器重複啟動
    if (GetWorldTimerManager().IsTimerActive(FireTimerHandle)) return;

    // 立即執行第一發
    PerformFire();

    // 啟動循環計時器
    GetWorldTimerManager().SetTimer(FireTimerHandle, this, &APistol::PerformFire, FireRate, true);
}

void APistol::StopFire()
{
    // 清除計時器
    GetWorldTimerManager().ClearTimer(FireTimerHandle);
}

void APistol::PerformFire()
{
    // 檢查是否有設定子彈類別，且世界是否有效
    if (BulletClass && GetWorld())
    {
        // 取得槍口的座標與旋轉 (假設你已經有射擊點，這裡以 Actor 自身作為基準)
        FVector SpawnLocation = GetActorLocation();
        FRotator SpawnRotation = GetActorRotation();
        SpawnRotation.Yaw += 90.0f;

        // 設定生成參數
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.Instigator = GetInstigator();

        // 生成子彈
        GetWorld()->SpawnActor<AActor>(BulletClass, SpawnLocation, SpawnRotation, SpawnParams);
    }
    // 呼叫藍圖事件，去執行 SpawnActor 等視覺行為
    K2_PerformFire();
}

