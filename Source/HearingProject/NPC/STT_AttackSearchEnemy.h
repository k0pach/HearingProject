// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreePropertyRef.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STT_AttackSearchEnemy.generated.h"

class ANPC_AIController;
/**
 * 
 */
UCLASS()
class HEARINGPROJECT_API USTT_AttackSearchEnemy : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

protected:
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition) override;

public:
	UPROPERTY(editAnywhere, Category = "Default")
	TStateTreePropertyRef<FVector> EnemyLocationRef;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ANPC_AIController* Controller;
};
