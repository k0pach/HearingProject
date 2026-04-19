#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/Character.h"
#include "NPC.generated.h"

class ANPC_AIController;

UCLASS()
class HEARINGPROJECT_API ANPC : public ACharacter, public IGenericTeamAgentInterface 
{
	GENERATED_BODY()

public:
	ANPC();
	
	virtual FGenericTeamId GetGenericTeamId() const override {return TeamID;}
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
private:
	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
	ANPC_AIController* AIController;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Team")
	uint8 TeamID = 1; 
	
};
