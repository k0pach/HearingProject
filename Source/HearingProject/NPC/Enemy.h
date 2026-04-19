// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NPC_AIController.h"
#include "GameFramework/Actor.h"
#include "GenericTeamAgentInterface.h"
#include "Enemy.generated.h"

UCLASS()
class HEARINGPROJECT_API AEnemy :  public AActor, public IGenericTeamAgentInterface 
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemy();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;
	
	virtual void Destroyed() override;	
	FVector getEnemyLoc();
	
	virtual FGenericTeamId GetGenericTeamId() const override {return TeamID;}
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	

private:
	class UAIPerceptionStimuliSourceComponent* StimulusSource;

	void SetupStimulusSource();

	FTimerHandle NoiseTimerHandle;  
	void MakeNoiseEvent();

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
	ANPC_AIController* AIController;
	
	//new
	UPROPERTY(EditDefaultsOnly, Category = "AI|Team")
	uint8 TeamID = 2; 

	
};
