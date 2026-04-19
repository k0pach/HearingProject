#include "NPC/NPC.h"

#include "NPC/NPC_AIController.h"

ANPC::ANPC()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ANPC::BeginPlay()
{
	Super::BeginPlay();
	
	AIController = Cast<ANPC_AIController>(GetController());
	
	if (!AIController)
	{
		SpawnDefaultController();
		AIController = Cast<ANPC_AIController>(GetController());
	}
}

void ANPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ANPC::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
