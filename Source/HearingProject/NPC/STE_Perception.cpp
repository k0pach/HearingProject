// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/STE_Perception.h"

#include "Enemy.h"
#include "HearingProjectCharacter.h"
#include "NPC.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StateTreeComponent.h"
#include "StateTreeExecutionContext.h"
#include "Perception/AIPerceptionComponent.h"
#include "TimerManager.h"


void USTE_Perception::TreeStart(FStateTreeExecutionContext& Context)
{
	Super::TreeStart(Context);
	
	Controller->OnSightStimulusDetected.AddDynamic(this, &USTE_Perception::HandleSightStimulus);
	Controller->OnSightStimulusForgotten.AddDynamic(this, &USTE_Perception::HandleSightStimulusForgotten);
	Controller->OnHearingStimulusDetected.AddDynamic(this, &USTE_Perception::HandleHearingStimulus);
	Controller->OnHearingStimulusForgotten.AddDynamic(this, &USTE_Perception::HandleHearingStimulusForgotten);
	
	Player = Cast<AHearingProjectCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	Player->OnWaitCommand.AddDynamic(this, &USTE_Perception::HandleWaitCommand);
	Player->OnFollowCommand.AddDynamic(this, &USTE_Perception::HandleFollowCommand);
	Player->OnAttackCommand.AddDynamic(this, &USTE_Perception::HandleAttackCommand);
	Player->OnMoveToPointCommand.AddDynamic(this, &USTE_Perception::HandleMoveToPointCommand);
	
	StateTreeComp = Controller->GetStateTreeComp();
	Controller->CurrentStateTreeState = EStateTreeEvent::Unknown;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &USTE_Perception::UpdatePlayerLoc, 0.01f, true);
	
	isWaitCommand = false;
	isAttackCommand = false;
	Controller->isDefense = false;
	isFollowCommand = true;
	isMoveToPointCommand = false;
	
	SendEvent(EStateTreeEvent::Follow);
	Controller->CurrentStateTreeState = EStateTreeEvent::Follow;
	LastCommand = EStateTreeEvent::Follow;
}

void USTE_Perception::TreeStop(FStateTreeExecutionContext& Context)
{
	Super::TreeStop(Context);
	
	if (Controller)
	{
		Controller->OnSightStimulusDetected.RemoveDynamic(this, &USTE_Perception::HandleSightStimulus);
		Controller->OnSightStimulusForgotten.RemoveDynamic(this, &USTE_Perception::HandleSightStimulusForgotten);
		Controller->OnHearingStimulusDetected.RemoveDynamic(this, &USTE_Perception::HandleHearingStimulus);
		Controller->OnHearingStimulusForgotten.RemoveDynamic(this, &USTE_Perception::HandleHearingStimulusForgotten);
	}
	Player = Cast<AHearingProjectCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	if (Player.IsValid())
	{
		Player->OnWaitCommand.RemoveDynamic(this, &USTE_Perception::HandleWaitCommand);
		Player->OnFollowCommand.RemoveDynamic(this, &USTE_Perception::HandleFollowCommand);
		Player->OnAttackCommand.RemoveDynamic(this, &USTE_Perception::HandleAttackCommand);
		Player->OnMoveToPointCommand.RemoveDynamic(this, &USTE_Perception::HandleMoveToPointCommand);
	}
	
	StateTreeComp = nullptr;
	AcquiredTarget = nullptr;
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
}

//PERCEPTION

//Sight
void USTE_Perception::HandleSightStimulus(AActor* Actor, const FAIStimulus& Stimulus)
{
	AcquiredTarget = Actor;
	Controller->AcquiredTarget = Actor;
	SeenEnemyLoc = Controller->GetNearestVisibleEnemyLocation();
	UpdatePlayerLoc();
	if (Controller->CurrentStateTreeState != EStateTreeEvent::CombatSee)
	{
		OnSightStimulus(Actor, Stimulus);
		GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Green, FString::Printf(TEXT("Sight Detected - State Changing to: CombatSee")));
		SendEvent(EStateTreeEvent::CombatSee);
		Controller->CurrentStateTreeState = EStateTreeEvent::CombatSee;
	}	
}

//Sight Forgotten
void USTE_Perception::HandleSightStimulusForgotten(AActor* Actor)
{
	AcquiredTarget = nullptr;
	Controller->AcquiredTarget = nullptr;
	OnSightStimulusForgotten(Actor);
	GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Red, TEXT("Sight Forgotten - State Changing to: Idle"));
	UpdatePlayerLoc();
	if (isWaitCommand)
	{
		Controller->CurrentStateTreeState = EStateTreeEvent::Wait;
		SendEvent(EStateTreeEvent::Wait);
	}
	else
	{
		Controller->CurrentStateTreeState = EStateTreeEvent::Idle;
		SendEvent(EStateTreeEvent::Idle);
	}
}

//Hearing
void USTE_Perception::HandleHearingStimulus(AActor* Actor, const FAIStimulus& Stimulus)
 {
	AcquiredTarget = Actor;
	Controller->AcquiredTarget = Actor;
	HeardEnemyLoc = Controller->GetNearestHeardEnemyLocation();
	UpdatePlayerLoc();
	if (Controller->CurrentStateTreeState != EStateTreeEvent::CombatHear)
	{
		OnHearingStimulus(Actor, Stimulus);
		GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Green, FString::Printf(TEXT("Hearing Detected - State Changing to: CombatHear")));
		SendEvent(EStateTreeEvent::CombatHear);
		Controller->CurrentStateTreeState = EStateTreeEvent::CombatHear;
		
	}	
}

//Hearing Forgotten
void USTE_Perception::HandleHearingStimulusForgotten(AActor* Actor)
{
	AcquiredTarget = nullptr;
	Controller->AcquiredTarget = nullptr;
	OnHearingStimulusForgotten(Actor);
	GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Red, TEXT("Hearing Forgotten - State Changing to: Idle"));	
	UpdatePlayerLoc();
	if (isWaitCommand)
	{
		Controller->CurrentStateTreeState = EStateTreeEvent::Wait;
		SendEvent(EStateTreeEvent::Wait);
	}
	else
	{
		Controller->CurrentStateTreeState = EStateTreeEvent::Idle;
		SendEvent(EStateTreeEvent::Idle);
	}
}

//КОНЕЦ PERCEPTION

//КОМАНДЫ

void USTE_Perception::HandleWaitCommand()
{
	SendEvent(EStateTreeEvent::Wait);	
	Controller->CurrentStateTreeState = EStateTreeEvent::Wait;
	LastCommand = EStateTreeEvent::Wait;
	isWaitCommand = true;
	isAttackCommand = false;
	Controller->isDefense = false;
	isFollowCommand = false;
	isMoveToPointCommand = false;
}

void USTE_Perception::HandleFollowCommand()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, "Follow");
	SendEvent(EStateTreeEvent::Follow);
	Controller->CurrentStateTreeState = EStateTreeEvent::Follow;
	LastCommand = EStateTreeEvent::Follow;
	isWaitCommand = false;
	isAttackCommand = false;
	Controller->isDefense = false;
	isFollowCommand = true;
	isMoveToPointCommand = false;
	
}

void USTE_Perception::HandleMoveToPointCommand(const FVector& Point)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, "Move to point");
	PointLocation = Point;
	SendEvent(EStateTreeEvent::MoveToPoint);
	Controller->CurrentStateTreeState = EStateTreeEvent::MoveToPoint;
	LastCommand = EStateTreeEvent::MoveToPoint;
	isWaitCommand = false;
	isAttackCommand = false;
	Controller->isDefense = false;
	isFollowCommand = false;
	isMoveToPointCommand = true;
}

void USTE_Perception::HandleAttackCommand(AActor* Target)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, "Attack");
	if (Target && Controller->GetTeamAttitudeTowards(*Target) == ETeamAttitude::Hostile)
	{
		EnemyLocation = Target->GetActorLocation();
		Controller->AcquiredTarget = Target;
		SendEvent(EStateTreeEvent::Attack);
		Controller->CurrentStateTreeState = EStateTreeEvent::Attack;
		LastCommand = EStateTreeEvent::Attack;
		isWaitCommand = false;
		isAttackCommand = true;
		Controller->isDefense = false;
		isFollowCommand = false;
		isMoveToPointCommand = false;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Attack: Invalid target!"));
	}
}

//КОНЕЦ КОММАНД



void USTE_Perception::UpdatePlayerLoc()
{
	if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
	{			
		FVector PlayerPos = PlayerPawn->GetActorLocation();
		FVector NPCPos = Pawn->GetActorLocation();
		FVector DirectionToPlayer = (PlayerPos - NPCPos).GetSafeNormal();
		float FollowDistance = 150.0f;  
		FVector TargetPos = PlayerPos - (DirectionToPlayer * FollowDistance);
		
		PlayerLoc = TargetPos;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No PlayerPawn found!"));
	}
}


//ОТПРАВКА ИВЕНТОВ
void USTE_Perception::SendEvent(const EStateTreeEvent Event)
{
	FGameplayTag EventTag;
	switch (Event)
	{
	case EStateTreeEvent::Idle:
		EventTag = FGameplayTag::RequestGameplayTag(FName("StateTree.Idle"));
		LastEvent = EStateTreeEvent::Idle;
		break;
	case EStateTreeEvent::Follow:
		EventTag = FGameplayTag::RequestGameplayTag(FName("StateTree.Follow"));
		LastEvent = EStateTreeEvent::Follow;
		break;
	case EStateTreeEvent::Wait:
		EventTag = FGameplayTag::RequestGameplayTag(FName("StateTree.Wait"));
		LastEvent = EStateTreeEvent::Wait;
		break;
	case EStateTreeEvent::Attack:
		EventTag = FGameplayTag::RequestGameplayTag(FName("StateTree.Combat.Attack"));
		LastEvent = EStateTreeEvent::Attack;
		break;
	case EStateTreeEvent::CombatSee:
		EventTag = FGameplayTag::RequestGameplayTag(FName("StateTree.Combat.See"));
		LastEvent = EStateTreeEvent::CombatSee;
		break;
	case EStateTreeEvent::CombatHear:
		EventTag = FGameplayTag::RequestGameplayTag(FName("StateTree.Combat.Hear"));
		LastEvent = EStateTreeEvent::CombatHear;
		break;
	case EStateTreeEvent::ChoosePoint:
		EventTag = FGameplayTag::RequestGameplayTag(FName("StateTree.MoveToPointCommand.ChoosePoint"));
		LastEvent = EStateTreeEvent::ChoosePoint;
		break;
	case EStateTreeEvent::MoveToPoint:
		EventTag = FGameplayTag::RequestGameplayTag(FName("StateTree.MoveToPointCommand.MoveToPoint"));
		LastEvent = EStateTreeEvent::MoveToPoint;
		break;
	default:
		break;
	}
	
	if (StateTreeComp)
	{
		FStateTreeEvent STEvent(EventTag);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, FString::Printf(TEXT("Event Sent: %s"), *UEnum::GetValueAsString(Event)));
		StateTreeComp->SendStateTreeEvent(STEvent);
	}
}




