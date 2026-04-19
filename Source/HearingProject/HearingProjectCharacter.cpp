// Copyright Epic Games, Inc. All Rights Reserved.

#include "HearingProjectCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "HearingProject.h"
#include "NPC/Enemy.h"

AHearingProjectCharacter::AHearingProjectCharacter()
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
	
	PrimaryActorTick.bCanEverTick = true; 

	//SetupStimulusSource();
	
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AHearingProjectCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}


void AHearingProjectCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AHearingProjectCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AHearingProjectCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AHearingProjectCharacter::Look);
		
		//NPC Commands
		EnhancedInputComponent->BindAction(WaitCommand, ETriggerEvent::Started, this, &AHearingProjectCharacter::WaitEvent);
		EnhancedInputComponent->BindAction(FollowCommand, ETriggerEvent::Started, this, &AHearingProjectCharacter::FollowEvent);
		EnhancedInputComponent->BindAction(MoveToPointCommand, ETriggerEvent::Started, this, &AHearingProjectCharacter::MoveToPointEvent);
		EnhancedInputComponent->BindAction(AttackCommand, ETriggerEvent::Started, this, &AHearingProjectCharacter::AttackEvent);
	}
	else
	{
		UE_LOG(LogHearingProject, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AHearingProjectCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void AHearingProjectCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AHearingProjectCharacter::WaitEvent(const FInputActionValue& Value)
{
	OnWaitCommand.Broadcast();
	
}

void AHearingProjectCharacter::FollowEvent(const FInputActionValue& Value)
{
	OnFollowCommand.Broadcast();
	
}

void AHearingProjectCharacter::MoveToPointEvent(const FInputActionValue& Value)
{	
	FHitResult Hit;	
	const FVector TraceStart = GetFollowCamera()->GetComponentLocation() + FVector(0.0f, 0.0f, 150.0f);
	const FVector TraceEnd = TraceStart + GetFollowCamera()->GetForwardVector() * 10000.f;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	
	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, Params);
	if (bHit && Hit.bBlockingHit)
	{
		FVector SelectedPoint = Hit.Location;
		OnMoveToPointCommand.Broadcast(SelectedPoint);
		
		DrawDebugSphere(GetWorld(), SelectedPoint, 50.f, 12, FColor::Green, false, 5.f);
		UE_LOG(LogTemp, Log, TEXT("MoveToPoint: выбрана точка %s"), *SelectedPoint.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("MoveToPoint: Нет попадания по поверхности."));
	}
}

void AHearingProjectCharacter::AttackEvent(const FInputActionValue& Value)
{
	FHitResult Hit;	
	const FVector TraceStart = GetFollowCamera()->GetComponentLocation() + FVector(0.0f, 0.0f, 150.0f);
	const FVector TraceEnd = TraceStart + GetFollowCamera()->GetForwardVector() * 10000.f;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	
	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, Params);
	if (bHit && Hit.bBlockingHit)
	{
		AActor* HitActor = Hit.GetActor();
		AEnemy* TargetEnemy = Cast<AEnemy>(HitActor);
		if (TargetEnemy)
		{
			OnAttackCommand.Broadcast(TargetEnemy);
			DrawDebugSphere(GetWorld(), Hit.Location, 50.f, 12, FColor::Red, false, 5.f);
			UE_LOG(LogTemp, Log, TEXT("Attack: Selected enemy %s at %s"), *TargetEnemy->GetName(), *Hit.Location.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Attack: Hit non-enemy actor!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Attack: No hit!"));
	}
}


void AHearingProjectCharacter::DoMove(float Right, float Forward)
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

void AHearingProjectCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AHearingProjectCharacter::DoJumpStart()
{
	// signal the character to jump
	Jump();
}

void AHearingProjectCharacter::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}


