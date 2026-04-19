// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/Enemy.h"

#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AIPerceptionSystem.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Sight.h"

// Sets default values
AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	SetupStimulusSource();
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	GetWorldTimerManager().SetTimer(NoiseTimerHandle, this, &AEnemy::MakeNoiseEvent, 6.f, true);
	SetGenericTeamId(FGenericTeamId(TeamID));
}

void AEnemy::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (StimulusSource)
	{
		StimulusSource->RegisterForSense(UAISense_Sight::StaticClass());
		StimulusSource->RegisterForSense(UAISense_Hearing::StaticClass());
		StimulusSource->RegisterWithPerceptionSystem();
		
		bool bRegistered = StimulusSource->IsRegistered();
		UE_LOG(LogTemp, Warning, TEXT("Enemy '%s' StimulusSource Registered for Sight/Hearing: %s"), *GetName(), bRegistered ? TEXT("YES") : TEXT("NO"));
		
		if (bRegistered)
		{
			UE_LOG(LogTemp, Warning, TEXT("Enemy '%s' ready for detection! Position: %s"), *GetName(), *GetActorLocation().ToString());
		}
	}
}

void AEnemy::SetupStimulusSource()
{
	StimulusSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("Stimulus"));
}

void AEnemy::MakeNoiseEvent()
{
	if (GetWorld())
	{
		UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), 1.f, this, 0.f, FName("EnemyNoise"));
		UE_LOG(LogTemp, Log, TEXT("Enemy '%s' made noise at %s"), *GetName(), *GetActorLocation().ToString());
		//GEngine->AddOnScreenDebugMessage(-1,10,FColor::Yellow, (TEXT("Enemy '%s' made noise at %s"), *GetName(), *GetActorLocation().ToString()));
		
		GetWorldTimerManager().SetTimer(NoiseTimerHandle, this, &AEnemy::MakeNoiseEvent, 6.f, true); 
	}
}

void AEnemy::Destroyed()
{
	Super::Destroyed();
	//AIController->OnTargetPerceptionForgotten(this);
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector AEnemy::getEnemyLoc()
{
	return GetActorLocation();
}

