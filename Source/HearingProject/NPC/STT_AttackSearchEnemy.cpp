// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/STT_AttackSearchEnemy.h"

#include "AudioMixerBlueprintLibrary.h"
#include "NetworkReplayStreaming.h"
#include "NPC_AIController.h"

EStateTreeRunStatus USTT_AttackSearchEnemy::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	if (Controller->HasVisibleEnemy())
	{
		if (FVector* enLoc = EnemyLocationRef.GetMutablePtr(Context))
		{
			*enLoc = Controller->GetNearestVisibleEnemyLocation();
		}
		return EStateTreeRunStatus::Succeeded;
	}
	else if (Controller->HasHeardEnemy())
	{
		if (FVector* enLoc = EnemyLocationRef.GetMutablePtr(Context))
		{
			*enLoc = Controller->GetNearestHeardEnemyLocation();
		}
		return EStateTreeRunStatus::Succeeded;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, FString::Printf(TEXT("State changed to: %s"), *UEnum::GetValueAsString(EStateTreeEvent::Follow)));
		Controller->CurrentStateTreeState = EStateTreeEvent::Follow;
		return EStateTreeRunStatus::Failed;
	}
}
