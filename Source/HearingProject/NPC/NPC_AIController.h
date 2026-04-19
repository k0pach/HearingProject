#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "GenericTeamAgentInterface.h"
#include "NPC_AIController.generated.h"

class UStateTreeComponent;
class UStateTree;

UENUM(BlueprintType)
enum class EStateTreeEvent : uint8
{
	Unknown UMETA(DisplayName = "Unknown"),
	Idle UMETA(DisplayName = "Idle"),
	Follow UMETA(DisplayName = "Follow"),
	CombatSee UMETA(DisplayName = "Combat See"),
	CombatHear UMETA(DisplayName = "Combat Hear"),
	Wait UMETA(DisplayName = "Wait"),
	MoveToPoint UMETA(DisplayName = "Move To Point"),
	Attack UMETA(DisplayName = "Attack"),
	ChoosePoint UMETA(DisplayName = "Choose Point")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FNPCStimulusEventSignature, AActor*, Actor, const FAIStimulus&, Stimulus);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNPCStimulusForgottenSignature, AActor*, Actor);

UCLASS()
class HEARINGPROJECT_API ANPC_AIController : public AAIController
{
	GENERATED_BODY()

public:
	ANPC_AIController(const FObjectInitializer& ObjectInitializer);
	UStateTreeComponent* GetStateTreeComp();
	bool getIsEnemySee();
	bool getIsEnemyHear();
	bool HasVisibleEnemy() const;
	bool HasHeardEnemy() const;
	FVector getSeenEnemyLocation();
	FVector getHeardEnemyLocation();
	FVector GetNearestVisibleEnemyLocation() const;
	FVector GetNearestHeardEnemyLocation() const;
	
	bool isDefense = false;
	
	/** Broadcast delegates */
	UPROPERTY(BlueprintAssignable, Category = "AI|Events", meta = (DisplayName = "On Sight Stimulus Detected"))
	FNPCStimulusEventSignature OnSightStimulusDetected;

	UPROPERTY(BlueprintAssignable, Category = "AI|Events", meta = (DisplayName = "On Sight Stimulus Forgotten"))
	FNPCStimulusForgottenSignature OnSightStimulusForgotten;

	UPROPERTY(BlueprintAssignable, Category = "AI|Events", meta = (DisplayName = "On Hearing Stimulus Detected"))
	FNPCStimulusEventSignature OnHearingStimulusDetected;

	UPROPERTY(BlueprintAssignable, Category = "AI|Events", meta = (DisplayName = "On Hearing Stimulus Forgotten"))
	FNPCStimulusForgottenSignature OnHearingStimulusForgotten;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI|State")
	EStateTreeEvent CurrentStateTreeState = EStateTreeEvent::Unknown;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI|Perception")
	TObjectPtr<AActor> AcquiredTarget = nullptr;

	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	UStateTreeComponent* StateTreeComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	UStateTree* StateTreeAsset;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	bool isEnemySee;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	bool isEnemyHear;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FVector heardEnemyLocation;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FVector seenEnemyLocation;
	
	class UAISenseConfig_Sight* SightConfig;
	class UAISenseConfig_Hearing* HearingConfig;

	void SetupPerceptionSystem();

	UFUNCTION()
	void OnTargetPerceptionForgotten(AActor* Actor);
	
	UFUNCTION()
	void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);
	
	
};
