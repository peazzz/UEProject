// Fill out your copyright notice in the Description page of Project Settings.


#include "Pistol.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "UEProjectCharacter.h"
#include "Misc/OutputDeviceNull.h"
#include "Blueprint/UserWidget.h"

// Sets default values
APistol::APistol()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

void APistol::StartFire()
{
    // 射線檢測：從畫面正中央發出一條射線，直接處理命中（不生成子彈 Actor）
    if (GetWorld())
    {
        APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
        if (PC)
        {
            int32 ViewX = 0, ViewY = 0;
            PC->GetViewportSize(ViewX, ViewY);
            float ScreenX = ViewX * 0.5f;
            float ScreenY = ViewY * 0.5f;

            FVector WorldLocation;
            FVector WorldDirection;
            // 將畫面中心反投影為世界空間射線
            if (PC->DeprojectScreenPositionToWorld(ScreenX, ScreenY, WorldLocation, WorldDirection))
            {
                const float Range = TraceRange > 0.f ? TraceRange : 10000.f;
                FVector TraceStart = WorldLocation;
                FVector TraceEnd = TraceStart + WorldDirection * Range;

                FHitResult Hit;
                FCollisionQueryParams Params;
                Params.AddIgnoredActor(this);
                if (GetInstigator()) Params.AddIgnoredActor(GetInstigator());

                bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, Params);

#if WITH_EDITOR
                // 除錯用，可在編輯器中看到射線（可移除或以條件編譯包覆）
                DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 1.0f, 0, 1.0f);
                if (bHit)
                {
                    DrawDebugPoint(GetWorld(), Hit.ImpactPoint, 8.0f, FColor::Yellow, false, 2.0f);
                }
#endif

                // 處理命中：若擊中角色或物件，套用傷害
                if (bHit && Hit.GetActor())
                {
                    // 套用點傷害
                    AController* InstigatorController = GetInstigatorController();
                    UGameplayStatics::ApplyPointDamage(Hit.GetActor(), Damage, WorldDirection, Hit, InstigatorController, this, UDamageType::StaticClass());
                }
            }
        }
    }
}

void APistol::BeginPlay()
{
    Super::BeginPlay();
    // 遊戲開始時，自動將子彈補滿
    CurrentAmmo = MaxAmmo;

    // === 改用計時器延遲 0.2 秒再初始化 UI，確保角色的 HUD 已經生成完畢 ===
    FTimerHandle UIInitHandle;
    GetWorldTimerManager().SetTimer(UIInitHandle, this, &APistol::UpdateWeaponAmmoUI, 0.2f, false);
}

bool APistol::ConsumeAmmo()
{
    if (CurrentAmmo > 0)
    {
        CurrentAmmo--;
        UpdateWeaponAmmoUI();
        return true;
    }
    return false;
}

void APistol::ReloadAmmo()
{
    CurrentAmmo = MaxAmmo;
    UpdateWeaponAmmoUI();
}

void APistol::UpdateWeaponAmmoUI()
{
    // 1. 取得持有這把槍的角色
    AUEProjectCharacter* MyCharacter = Cast<AUEProjectCharacter>(GetInstigator());

    // 2. 改用你剛剛寫的 GetPlayerHUDInstance() 函數來取得 UI 實例
    if (!MyCharacter || !MyCharacter->GetPlayerHUDInstance()) return;

    // 3. 計算子彈百分比
    float Percent = MaxAmmo > 0 ? (float)CurrentAmmo / (float)MaxAmmo : 0.f;

    // 4. 反射呼叫藍圖
    FOutputDeviceNull Ar;
    FString Cmd = FString::Printf(TEXT("UpdatePlayerAmmoUI %f %d %d"), Percent, CurrentAmmo, MaxAmmo);

    // 這裡同樣改用函數獲取
    MyCharacter->GetPlayerHUDInstance()->CallFunctionByNameWithArguments(*Cmd, Ar, nullptr, true);
}
