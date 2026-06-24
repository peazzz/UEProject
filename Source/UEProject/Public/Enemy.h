// Simple enemy character that chases and attacks the player. Can receive damage and die.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

class AEnemyAIController;

UCLASS()
class UEPROJECT_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemy();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	// Handle incoming damage
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	// Attack logic
	void StartAttack();
	void StopAttack();
	void DoAttack();

	void Die();

protected:
	// Health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	float Health = 100.0f;

	// Damage dealt to player when in attack range
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	float Damage = 10.0f;

	// Detection and movement ranges
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	float AggroRange = 2000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	float AttackRange = 150.0f;

	// Attack rate in seconds
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	float AttackRate = 1.0f;

	// 儲存你的攻擊動畫蒙太奇
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Animation")
	UAnimMontage* AttackMontage = nullptr;

	// Internal
	UPROPERTY()
	AActor* CurrentTarget = nullptr;

	FTimerHandle AttackTimerHandle;

	AEnemyAIController* EnemyController = nullptr;

public:
	// Expose a simple getter
	UFUNCTION(BlueprintCallable, Category = "Enemy")
	bool IsAlive() const { return Health > 0.f; }

	// Death animation montage
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* DeathMontage = nullptr;

	// Optional blueprint event called when death animation finishes (for VFX, sounds, spawn)
	UFUNCTION(BlueprintImplementableEvent, Category = "Enemy")
	void OnDeathAnimationFinished();
};
