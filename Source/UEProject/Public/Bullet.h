// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet.generated.h"

//元件
class UProjectileMovementComponent;
class USphereComponent;

//向藍圖公開
UCLASS(BlueprintType, Blueprintable)
class UEPROJECT_API ABullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABullet();

	// 擊中對象力道
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile | Physics")
	float PhysicsForce = 100.0f;

	// Called when the projectile collides with an object
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// Mesh of the projectile in the world 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile | Mesh")
	TObjectPtr<UStaticMeshComponent> ProjectileMesh;

	// Despawn after 5 seconds by default
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile | Lifespan")
	float ProjectileLifespan = 5.0f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Sphere collision component 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile | Components")
	TObjectPtr<USphereComponent> CollisionComponent;

	// Projectile movement component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile | Components")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
