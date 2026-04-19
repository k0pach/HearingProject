// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "HearingProjectCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;
class UInputMappingContext;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCommandWaitSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCommandFollowSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCommandAttackSignature, AActor*, Target);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCommandMoveToPointSignature, const FVector&, PointLocation);

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A simple player-controllable third person character
 *  Implements a controllable orbiting camera
 */
UCLASS(abstract)
class AHearingProjectCharacter : public ACharacter, public IGenericTeamAgentInterface 
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
protected:

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;
	
	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MouseLookAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* WaitCommand;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* FollowCommand;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* AttackCommand;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* MoveToPointCommand;
	
	/** Initialize input action bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
	
	void WaitEvent(const FInputActionValue& Value);
	
	void FollowEvent(const FInputActionValue& Value);
	
	void MoveToPointEvent(const FInputActionValue& Value);
	
	void AttackEvent(const FInputActionValue& Value);
	
	void ChoosePointEvent(const FInputActionValue& Value);
	
public:

	/** Constructor */
	AHearingProjectCharacter();
	
	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles look inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();
	
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	virtual void BeginPlay() override;
	
	UPROPERTY(BlueprintAssignable, Category = "Command", meta = (DisplayName = "On Wait Command Delegate"))
	FCommandWaitSignature OnWaitCommand;
	
	UPROPERTY(BlueprintAssignable, Category = "Command", meta = (DisplayName = "On Follow Command Delegate"))
	FCommandFollowSignature OnFollowCommand;
	
	UPROPERTY(BlueprintAssignable, Category = "Command", meta = (DisplayName = "On Attack Command Delegate"))
	FCommandAttackSignature OnAttackCommand;
	
	UPROPERTY(BlueprintAssignable, Category = "Command", meta = (DisplayName = "On Move To Point Command Delegate"))
	FCommandMoveToPointSignature OnMoveToPointCommand;
	
	virtual FGenericTeamId GetGenericTeamId() const override {return TeamID;}
private:
	UPROPERTY(EditDefaultsOnly, Category = "AI|Team")
	uint8 TeamID = 1;
};

