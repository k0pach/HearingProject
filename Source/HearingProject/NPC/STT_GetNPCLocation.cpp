// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/STT_GetNPCLocation.h"

EStateTreeRunStatus USTT_GetNPCLocation::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	if (FVector* waitLoc = WaitLocationRef.GetMutablePtr(Context))
	{
		*waitLoc = Controller->GetPawn()->GetActorLocation();
		
		return EStateTreeRunStatus::Succeeded;
	}
	return EStateTreeRunStatus::Failed;
}
