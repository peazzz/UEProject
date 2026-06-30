// Copyright Epic Games, Inc. All Rights Reserved.

#include "UEProjectCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "UEProject.h"
#include "Blueprint/UserWidget.h"
#include "Misc/OutputDeviceNull.h"
#include "GameType.h"
#include "Pistol.h"

AUEProjectCharacter::AUEProjectCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AUEProjectCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AUEProjectCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AUEProjectCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AUEProjectCharacter::Look);
	}
	else
	{
		UE_LOG(LogUEProject, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AUEProjectCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void AUEProjectCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AUEProjectCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void AUEProjectCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AUEProjectCharacter::DoJumpStart()
{
	// signal the character to jump
	Jump();
}

void AUEProjectCharacter::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}

void AUEProjectCharacter::BeginPlay()
{
	Super::BeginPlay();

	// store initial transform for respawn
	InitialLocation = GetActorLocation();
	InitialRotation = GetActorRotation();

	// initialize health
	Health = MaxHealth;
	bIsDead = false;

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC && PlayerHUDClass)
	{
		// 建立 Widget
		PlayerHUDInstance = CreateWidget<UUserWidget>(PC, PlayerHUDClass);
		if (PlayerHUDInstance)
		{
			// 放到畫面上
			PlayerHUDInstance->AddToViewport();
		}
	}

	// 初始化 HUD 顯示
	UpdateHUDHealth();
}

float AUEProjectCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsDead) return 0.f;

	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage <= 0.f) return 0.f;

	Health -= ActualDamage;
	UE_LOG(LogTemp, Log, TEXT("AUEProjectCharacter::TakeDamage %s took %f damage, health now %f"), *GetNameSafe(this), ActualDamage, Health);
	UpdateHUDHealth();

	if (Health <= 0.f && !bIsDead)
	{
		Die();
	}

	return ActualDamage;
}

void AUEProjectCharacter::Die()
{
	bIsDead = true;
	UE_LOG(LogTemp, Log, TEXT("AUEProjectCharacter::Die %s died"), *GetNameSafe(this));

	UpdateHUDHealth();

	//SetActorLocation(InitialLocation, false, nullptr, ETeleportType::TeleportPhysics);
	//SetActorRotation(InitialRotation);

	// disable movement and input
	GetCharacterMovement()->DisableMovement();
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		DisableInput(PC);
	}

	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		// 讓網格體開啟物理模擬
		MeshComp->SetSimulatePhysics(true);

		// 改變碰撞模式，讓屍體可以跟地面碰撞，但不會阻擋敵人或彈藥
		MeshComp->SetCollisionProfileName(TEXT("Ragdoll"));
	}

	// Blueprint hook
	OnPlayerDied();

	// schedule respawn
	//FTimerHandle RespawnHandle;
	//GetWorldTimerManager().SetTimer(RespawnHandle, this, &AUEProjectCharacter::Respawn, RespawnDelay, false);
}


void AUEProjectCharacter::Respawn()
{
	// restore health and position
	Health = MaxHealth;
	bIsDead = false;

	// teleport to initial location
	SetActorLocation(InitialLocation, false, nullptr, ETeleportType::TeleportPhysics);
	SetActorRotation(InitialRotation);

	// re-enable movement and input
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		EnableInput(PC);
	}

	UE_LOG(LogTemp, Log, TEXT("AUEProjectCharacter::Respawn %s respawned"), *GetNameSafe(this));
	OnPlayerRespawned();
}

// 實作反射呼叫 UMG 函數的邏輯
void AUEProjectCharacter::UpdateHUDHealth()
{
	if (!PlayerHUDInstance) return;

	// 計算血量比例
	float Percent = FMath::Clamp(Health / MaxHealth, 0.f, 1.f);

	// 透過反射呼叫你在 WBP_PlayerHUD 裡寫的 UpdatePlayerHealth 函數
	FOutputDeviceNull Ar;
	FString Cmd = FString::Printf(TEXT("UpdatePlayerHealth %f"), Percent);
	PlayerHUDInstance->CallFunctionByNameWithArguments(*Cmd, Ar, nullptr, true);
}

void AUEProjectCharacter::ApplyUpgrade(EUpgradeType UpgradeType)
{
	switch (UpgradeType)
	{
	case EUpgradeType::Character_MaxHealth:
		MaxHealth += 100; // 每次升級提升100血量上限
		Health = MaxHealth;    // 同步幫玩家回滿
		UpdateHUDHealth();  // 呼叫你現有的 UI 更新邏輯
		break;

	case EUpgradeType::Character_MaxWalkSpeed:
		// 直接修改 CharacterMovement 的最大走路速度
		if (GetCharacterMovement())
		{
			GetCharacterMovement()->MaxWalkSpeed *= 1.5f;
		}
		break;

	case EUpgradeType::Pistol_FireRate:
		if (CurrentWeapon)
		{
			// 注意：FireRate 通常代表射擊間隔，所以「升級」應該是讓間隔「變短」
			CurrentWeapon->FireRate = FMath::Max(0.1f, CurrentWeapon->FireRate * 0.5f);
		}
		break;

	case EUpgradeType::Pistol_Damage:
		if (CurrentWeapon)
		{
			CurrentWeapon->Damage *= 1.5f;
		}
		break;

	case EUpgradeType::Pistol_MaxAmmo:
		if (CurrentWeapon)
		{
			CurrentWeapon->MaxAmmo *= 2;
			CurrentWeapon->ReloadAmmo(); // 順便幫玩家把子彈補滿並刷新 UI
		}
		break;

	default:
		break;
	}
}
