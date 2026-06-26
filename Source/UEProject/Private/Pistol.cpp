// Fill out your copyright notice in the Description page of Project Settings.


#include "Pistol.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

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
}

bool APistol::ConsumeAmmo()
{
    if (CurrentAmmo > 0)
    {
        CurrentAmmo--;
        return true;
    }
    return false;
}

void APistol::ReloadAmmo()
{
    CurrentAmmo = MaxAmmo;
}

