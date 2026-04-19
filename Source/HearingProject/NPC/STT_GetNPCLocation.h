// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NPC_AIController.h"
#include "StateTreePropertyRef.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STT_GetNPCLocation.generated.h"

/**
 * 
 */
UCLASS()
class HEARINGPROJECT_API USTT_GetNPCLocation : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

protected:
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

public:
	UPROPERTY(editAnywhere, Category = "Default")
	TStateTreePropertyRef<FVector> WaitLocationRef;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ANPC_AIController* Controller;
};
