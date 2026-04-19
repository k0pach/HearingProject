// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/STT_DefenseSearchEnemy.h"

EStateTreeRunStatus USTT_DefenseSearchEnemy::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
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
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Purple, FString::Printf(TEXT("State: %s"), *UEnum::GetValueAsString(EStateTreeEvent::Wait)));
		Controller->CurrentStateTreeState = EStateTreeEvent::Wait;
		return EStateTreeRunStatus::Failed;
	}
}
