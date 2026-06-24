// Enemy implementation: simple chase and attack player
#include "Enemy.h"
#include "EnemyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	// Default movement settings (can be tweaked in Blueprint)
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	EnemyController = Cast<AEnemyAIController>(GetController());
}

void AEnemy::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!IsAlive()) return;

	// Find player pawn (player index 0)
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (!PlayerPawn) return;

	float Dist = FVector::Dist(GetActorLocation(), PlayerPawn->GetActorLocation());
	if (Dist <= AggroRange)
	{
		// Move towards player
		if (EnemyController)
		{
			EnemyController->MoveToActor(PlayerPawn, 5.0f);
		}

		CurrentTarget = PlayerPawn;

		if (Dist <= AttackRange)
		{
			// within attack distance
			if (EnemyController)
			{
				EnemyController->StopMovement();
			}
			StartAttack();
		}
		else
		{
			StopAttack();
		}
	}
	else
	{
		// out of aggro range: stop moving and stop attacking
		if (EnemyController)
		{
			EnemyController->StopMovement();
		}
		CurrentTarget = nullptr;
		StopAttack();
	}
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage <= 0.f) return 0.f;

	Health -= ActualDamage;
	UE_LOG(LogTemp, Log, TEXT("AEnemy::TakeDamage %s took %f damage, health now %f"), *GetNameSafe(this), ActualDamage, Health);

	if (Health <= 0.f)
	{
		Die();
	}

	return ActualDamage;
}

void AEnemy::StartAttack()
{
	if (GetWorldTimerManager().IsTimerActive(AttackTimerHandle)) return;
	// immediately attack then set looping timer
	DoAttack();
	GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &AEnemy::DoAttack, AttackRate, true);
}

void AEnemy::StopAttack()
{
	if (GetWorldTimerManager().IsTimerActive(AttackTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(AttackTimerHandle);
	}
}

void AEnemy::DoAttack()
{
	if (!CurrentTarget || !IsAlive())
	{
		StopAttack();
		return;
	}

	if (AttackMontage)
	{
		// 取得當前的動畫實例並播放蒙太奇
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(AttackMontage);
		}
	}

	// Apply damage to target (simple implementation)
	UE_LOG(LogTemp, Log, TEXT("AEnemy::DoAttack %s attacks %s for %f"), *GetNameSafe(this), *GetNameSafe(CurrentTarget), Damage);
	UGameplayStatics::ApplyDamage(CurrentTarget, Damage, GetController(), this, UDamageType::StaticClass());
}

void AEnemy::Die()
{
	UE_LOG(LogTemp, Log, TEXT("AEnemy::Die %s died"), *GetNameSafe(this));
	StopAttack();
	// disable movement
	GetCharacterMovement()->DisableMovement();

	// Play death montage if assigned, otherwise just destroy after short delay
	if (DeathMontage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		float Duration = GetMesh()->GetAnimInstance()->Montage_Play(DeathMontage, 0.65f);
		if (Duration <= 0.f)
		{
			Duration = DeathMontage->GetPlayLength();
		}

		// allow a small buffer then destroy
		SetLifeSpan(Duration + 0.25f);

		// Optionally notify blueprint after montage finishes (Blueprint can spawn VFX)
		// We'll schedule a timer to call the BlueprintImplementableEvent after Duration
		FTimerHandle TmpHandle;
		GetWorldTimerManager().SetTimer(TmpHandle, [this]() {
			OnDeathAnimationFinished();
		}, Duration, false);
	}
	else
	{
		// no montage: fallback
		SetLifeSpan(2.0f);
		OnDeathAnimationFinished();
	}
}
