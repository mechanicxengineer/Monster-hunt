// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyController.generated.h"

UCLASS()
class AAAMECHANICS_API AEnemyController : public AAIController
{
	GENERATED_BODY()
	
	/** Blackboard component for the enemy */
	UPROPERTY(BlueprintReadWrite, Category = "AI Behavior", meta = (AllowPrivateAccess = "true"))
	class UBlackboardComponent* BlackboardComponent;
	
	/** Behavior tree component for this enemy */
	UPROPERTY(BlueprintReadWrite, Category = "AI Behavior", meta = (AllowPrivateAccess = "true"))
	class UBehaviorTreeComponent* BehaviorTreeComponent;

public:
	AEnemyController();
	virtual void OnPossess(APawn* InPawn) override;



	/********* GETTER **********/
	FORCEINLINE UBlackboardComponent* GetBlackboardComponent() { return BlackboardComponent; }

};
