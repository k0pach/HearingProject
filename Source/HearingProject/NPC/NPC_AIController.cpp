// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/NPC_AIController.h"
#include "Enemy.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISense_Hearing.h"
#include "Components/StateTreeComponent.h"
#include "StateTreeExecutionContext.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Sight.h"



ANPC_AIController::ANPC_AIController(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
	StateTreeComp = CreateDefaultSubobject<UStateTreeComponent>(TEXT("StateTreeComp"));
	StateTreeComp->SetStartLogicAutomatically(false);
	
	StateTreeComp->PrimaryComponentTick.bCanEverTick = true;
	StateTreeComp->PrimaryComponentTick.bStartWithTickEnabled = true;
	StateTreeComp->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	SetupPerceptionSystem();
	isEnemyHear = false;
	isEnemySee = false;
}

UStateTreeComponent* ANPC_AIController::GetStateTreeComp()
{
	return StateTreeComp;
}

bool ANPC_AIController::getIsEnemySee()
{
	return isEnemySee;
}

bool ANPC_AIController::HasVisibleEnemy() const
{
	if (!PerceptionComponent) return false;

	TArray<AActor*> PerceivedActors;
	PerceptionComponent->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), PerceivedActors);

	for (AActor* Actor : PerceivedActors)
	{
		/*if (IsValid(Actor) && Cast<AEnemy>(Actor))
		{
			return true;
		}*/
		if (IsValid(Actor) && GetTeamAttitudeTowards(*Actor) == ETeamAttitude::Hostile)
		{
			return true;
		}
	}
	return false;
}

FVector ANPC_AIController::GetNearestVisibleEnemyLocation() const
{
	if (!PerceptionComponent) return FVector::ZeroVector;

	TArray<AActor*> PerceivedActors;
	PerceptionComponent->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), PerceivedActors);

	FVector NearestLoc;
	float NearestDist = TNumericLimits<float>::Max();
	APawn* ZPawn = GetPawn();
	FVector PawnPos = ZPawn ? ZPawn->GetActorLocation() : FVector::ZeroVector;

	for (AActor* Actor : PerceivedActors)
	{
		/*if (!IsValid(Actor)) continue;
		if (Cast<AEnemy>(Actor))
		{
			AEnemy* EnemyActor = Cast<AEnemy>(Actor);
			if (!IsValid(EnemyActor)) continue;

			float Dist = FVector::Dist(PawnPos, Actor->GetActorLocation());
			if (Dist < NearestDist)
			{
				NearestDist = Dist;
				NearestLoc = Actor->GetActorLocation();
			}
		}*/
		if (IsValid(Actor) && GetTeamAttitudeTowards(*Actor) == ETeamAttitude::Hostile)
		{
			float Dist = FVector::Dist(PawnPos, Actor->GetActorLocation());
			if (Dist < NearestDist)
			{
				NearestDist = Dist;
				NearestLoc = Actor->GetActorLocation();
			}
		}
	}
	return NearestLoc;
}

bool ANPC_AIController::HasHeardEnemy() const
{
	if (!PerceptionComponent) return false;

	TArray<AActor*> PerceivedActors;
	PerceptionComponent->GetCurrentlyPerceivedActors(UAISense_Hearing::StaticClass(), PerceivedActors);

	for (AActor* Actor : PerceivedActors)
	{
		/*if (IsValid(Actor) && Cast<AEnemy>(Actor))
		{
			return true;
		}*/
		if (IsValid(Actor) && GetTeamAttitudeTowards(*Actor) == ETeamAttitude::Hostile)
		{
			return true;
		}
	}
	return false;
}

FVector ANPC_AIController::getSeenEnemyLocation()
{
	return seenEnemyLocation;
}

FVector ANPC_AIController::getHeardEnemyLocation()
{
	return heardEnemyLocation;
}

FVector ANPC_AIController::GetNearestHeardEnemyLocation() const
{
	if (!PerceptionComponent) return FVector::ZeroVector;

	TArray<AActor*> PerceivedActors;
	PerceptionComponent->GetCurrentlyPerceivedActors(UAISense_Hearing::StaticClass(), PerceivedActors);

	FVector HeardLoc;
	float NearestDist = TNumericLimits<float>::Max();
	APawn* ZPawn = GetPawn();
	FVector PawnPos = ZPawn ? ZPawn->GetActorLocation() : FVector::ZeroVector;

	for (AActor* Actor : PerceivedActors)
	{
		/*if (!IsValid(Actor)) continue;
		if (Cast<AEnemy>(Actor))
		{
			AEnemy* EnemyActor = Cast<AEnemy>(Actor);
			if (!IsValid(Actor)) continue;
			
			float Dist = FVector::Dist(PawnPos, Actor->GetActorLocation());
			if (Dist < NearestDist)
			{
				NearestDist = Dist;
				HeardLoc = Actor->GetActorLocation();
			}
		}*/
		if (IsValid(Actor) && GetTeamAttitudeTowards(*Actor) == ETeamAttitude::Hostile)
		{
			float Dist = FVector::Dist(PawnPos, Actor->GetActorLocation());
			if (Dist < NearestDist)
			{
				NearestDist = Dist;
				HeardLoc = Actor->GetActorLocation();
			}
		}
	}
	return HeardLoc;
}

bool ANPC_AIController::getIsEnemyHear()
{
	return isEnemyHear;
}

void ANPC_AIController::BeginPlay()
{
	Super::BeginPlay();
	SetGenericTeamId(FGenericTeamId(1));
	StateTreeComp->StartLogic();
	GetPerceptionComponent()->OnPerceptionUpdated.AddDynamic(this, &ANPC_AIController::OnPerceptionUpdated);
	
	UE_LOG(LogTemp, Log, TEXT("ANPC_AIController::BeginPlay - CurrentStateTreeState: %s"), *UEnum::GetValueAsString(CurrentStateTreeState));
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("Initial State: %s"), *UEnum::GetValueAsString(CurrentStateTreeState)));
	}
}

void ANPC_AIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	if (StateTreeAsset && StateTreeComp)
	{
		StateTreeComp->SetStateTree(StateTreeAsset);
	}
}

void ANPC_AIController::SetupPerceptionSystem()
{
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	SightConfig->SightRadius=1000.f;
	SightConfig->LoseSightRadius = SightConfig->SightRadius + 25.f;
	SightConfig->PeripheralVisionAngleDegrees = 180.f;
	SightConfig->SetMaxAge(5.f);
	SightConfig->AutoSuccessRangeFromLastSeenLocation = 3000.f;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("Hearing Config"));
	HearingConfig->HearingRange = 2000.f;  
	HearingConfig->LoSHearingRange = HearingConfig->HearingRange + 50.f;  
	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
	HearingConfig->SetMaxAge(5.f);
	
	GetPerceptionComponent()->SetDominantSense(SightConfig->GetSenseImplementation());
	GetPerceptionComponent()->ConfigureSense(*SightConfig);
	GetPerceptionComponent()->ConfigureSense(*HearingConfig);
	
}


void ANPC_AIController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	if (isDefense)
	{
		for (AActor* Actor : UpdatedActors)
		{
			if (!Actor || !Cast<AEnemy>(Actor)) continue; 

			FActorPerceptionBlueprintInfo Info;
			GetPerceptionComponent()->GetActorsPerception(Actor, Info);

			for (const FAIStimulus& Stimulus : Info.LastSensedStimuli)
			{
				if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
				{
					if (Stimulus.WasSuccessfullySensed())
					{
						/*isEnemySee = true;
						seenEnemyLocation = Stimulus.StimulusLocation;
						OnSightStimulusDetected.Broadcast(Actor, Stimulus); 
						
						UE_LOG(LogTemp, Log, TEXT("Sight Detected - Actor: %s, Location: %s"), *GetNameSafe(Actor), *Stimulus.StimulusLocation.ToString());
						if (GEngine)
						{
							GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString::Printf(TEXT("Sight Detected: %s"), *GetNameSafe(Actor)));
						}*/
						if (GetTeamAttitudeTowards(*Actor) == ETeamAttitude::Hostile)
						{
							isEnemySee = true;
							seenEnemyLocation = Stimulus.StimulusLocation;
							OnSightStimulusDetected.Broadcast(Actor, Stimulus);
						
							UE_LOG(LogTemp, Log, TEXT("Sight Detected - Actor: %s, Location: %s"), *GetNameSafe(Actor), *Stimulus.StimulusLocation.ToString());
							if (GEngine)
							{
								GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString::Printf(TEXT("Sight Detected: %s"), *GetNameSafe(Actor)));
							}
						}
					}
					else
					{
						isEnemySee = false;
						OnSightStimulusForgotten.Broadcast(Actor);
						
						UE_LOG(LogTemp, Log, TEXT("Sight Forgotten - Actor: %s"), *GetNameSafe(Actor));
						if (GEngine)
						{
							GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("Sight Forgotten: %s"), *GetNameSafe(Actor)));
						}
					}
				}
				else if (Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>())
				{
					if (Stimulus.WasSuccessfullySensed())
					{
						/*isEnemyHear = true;
						heardEnemyLocation = Stimulus.StimulusLocation;
						OnHearingStimulusDetected.Broadcast(Actor, Stimulus);
						
						UE_LOG(LogTemp, Log, TEXT("Hearing Detected - Actor: %s, Location: %s"), *GetNameSafe(Actor), *Stimulus.StimulusLocation.ToString());
						if (GEngine)
						{
							GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString::Printf(TEXT("Hearing Detected: %s"), *GetNameSafe(Actor)));
						}*/
						if (GetTeamAttitudeTowards(*Actor) == ETeamAttitude::Hostile)
						{
							isEnemyHear = true;
							heardEnemyLocation = Stimulus.StimulusLocation;
							OnHearingStimulusDetected.Broadcast(Actor, Stimulus);
						
							UE_LOG(LogTemp, Log, TEXT("Hearing Detected - Actor: %s, Location: %s"), *GetNameSafe(Actor), *Stimulus.StimulusLocation.ToString());
							if (GEngine)
							{
								GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString::Printf(TEXT("Hearing Detected: %s"), *GetNameSafe(Actor)));
							}
						}
					}
					else
					{
						isEnemyHear = false;
						OnHearingStimulusForgotten.Broadcast(Actor);
						
						UE_LOG(LogTemp, Log, TEXT("Hearing Forgotten - Actor: %s"), *GetNameSafe(Actor));
						if (GEngine)
						{
							GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("Hearing Forgotten: %s"), *GetNameSafe(Actor)));
						}
					}
				}
			}
		}
	}
}

ETeamAttitude::Type ANPC_AIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	// Check if the other actor implements IGenericTeamAgentInterface
	if (const IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(&Other))
	{
		FGenericTeamId OtherTeamId = TeamAgent->GetGenericTeamId();

		// Player team = 1, Enemy team = 2
		if (OtherTeamId == FGenericTeamId(1))
		{
			return ETeamAttitude::Friendly;
		}
		else if (OtherTeamId == FGenericTeamId(2))
		{
			return ETeamAttitude::Hostile; 
		}
	}

	return ETeamAttitude::Neutral; // Everything else is ignored
}

void ANPC_AIController::OnTargetPerceptionForgotten(AActor* Actor)
{
	//if (Cast<AEnemy>(Actor))
	if (GetTeamAttitudeTowards(*Actor) == ETeamAttitude::Hostile)
	{
		isEnemySee = false;
		isEnemyHear = false;
		seenEnemyLocation = FVector::ZeroVector;
		heardEnemyLocation = FVector::ZeroVector;
	}
	
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, 
			FString::Printf(TEXT("Forgotten: %s | See: %s | Hear: %s | Loc: %s"), 
				*GetNameSafe(Actor), 
				isEnemySee ? TEXT("TRUE") : TEXT("FALSE"),
				isEnemyHear ? TEXT("TRUE") : TEXT("FALSE"),
				*seenEnemyLocation.ToCompactString()));
	}
}


