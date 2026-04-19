// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NPC_AIController.h"
#include "StateTreePropertyRef.h"
#include "Blueprint/StateTreeEvaluatorBlueprintBase.h"
#include "STE_Perception.generated.h"


class AEnemy;
class AHearingProjectCharacter;
/**
 * 
 */
UCLASS()
class HEARINGPROJECT_API USTE_Perception : public UStateTreeEvaluatorBlueprintBase
{
	GENERATED_BODY()

protected:
	virtual void TreeStart(FStateTreeExecutionContext& Context) override;
	virtual void TreeStop(FStateTreeExecutionContext& Context) override;
	
	/** Called when sight stimulus is detected */
	UFUNCTION(BlueprintImplementableEvent, Category = "Perception")
	void OnSightStimulus(AActor* Actor, const FAIStimulus& Stimulus);

	/** Called when sight stimulus is forgotten */
	UFUNCTION(BlueprintImplementableEvent, Category = "Perception")
	void OnSightStimulusForgotten(AActor* Actor);

	/** Called when hearing stimulus is detected */
	UFUNCTION(BlueprintImplementableEvent, Category = "Perception")
	void OnHearingStimulus(AActor* Actor, const FAIStimulus& Stimulus);

	/** Called when hearing stimulus is forgotten */
	UFUNCTION(BlueprintImplementableEvent, Category = "Perception")
	void OnHearingStimulusForgotten(AActor* Actor);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ANPC_AIController* Controller;
	
	/*UPROPERTY(EditAnywhere, Category = "Default")
	TStateTreePropertyRef<FVector> PlayerLocRef;
	
	UPROPERTY(EditAnywhere, Category="Default")
	TStateTreePropertyRef<bool> isSeeEnemyRef;
	
	UPROPERTY(EditAnywhere, Category = "Default")
	TStateTreePropertyRef<FVector> EnemyLocRef;

	UPROPERTY(EditAnywhere, Category="Default")
	TStateTreePropertyRef<bool> isHearEnemyRef;

	UPROPERTY(EditAnywhere, Category = "Default")
	TStateTreePropertyRef<FVector> HeardLocRef;*/
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Output")
	FVector SeenEnemyLoc = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Output")
	FVector HeardEnemyLoc = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Output")
	FVector PlayerLoc = FVector::ZeroVector;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	APawn* Pawn;
	
	UPROPERTY(EditAnywhere, Category = "Default")
	TStateTreePropertyRef<FVector> WaitLocationRef;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Output", meta = (AllowPrivateAccess = "true"))
	FVector WaitLocation = FVector::ZeroVector;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Output", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AActor> AcquiredTarget = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Default")
	FVector PointLocation = FVector::ZeroVector;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Output")
	FVector EnemyLocation = FVector::ZeroVector;
	
private:
	
	UFUNCTION()
	void HandleSightStimulus(AActor* Actor, const FAIStimulus& Stimulus);

	UFUNCTION()
	void HandleSightStimulusForgotten(AActor* Actor);

	UFUNCTION()
	void HandleHearingStimulus(AActor* Actor, const FAIStimulus& Stimulus);

	UFUNCTION()
	void HandleHearingStimulusForgotten(AActor* Actor);
	
	UFUNCTION()
	void HandleWaitCommand();
	
	UFUNCTION()
	void HandleFollowCommand();
	
	UFUNCTION()
	void HandleAttackCommand(AActor* Target);
	
	UFUNCTION()
	void HandleMoveToPointCommand(const FVector& Point);
	
	void SendEvent(const EStateTreeEvent Event);
	
	void UpdatePlayerLoc();
	
	UPROPERTY()
	TObjectPtr<UStateTreeComponent> StateTreeComp;
	
	bool isSeeEnemy = false;
	bool isHearEnemy = false;
	bool NeedToUpdate = false;
	EStateTreeEvent LastEvent = EStateTreeEvent::Idle;
	EStateTreeEvent LastCommand = EStateTreeEvent::Follow;
	FTimerHandle TimerHandle;
	TWeakObjectPtr<AHearingProjectCharacter> Player;
	
	
	//Приоритет комманд
	bool isAttackCommand = false;
	bool isFollowCommand = true;
	bool isMoveToPointCommand = false;
	bool isWaitCommand = false;
};
