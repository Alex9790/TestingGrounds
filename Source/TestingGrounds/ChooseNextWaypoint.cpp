// Fill out your copyright notice in the Description page of Project Settings.


#include "BehaviorTree/BlackboardComponent.h"
#include "ChooseNextWaypoint.h"
#include "AIController.h"
#include "PatrollingGuard.h" //TODO: remove coupling, elimina cuando se aplique solucion mas general

EBTNodeResult::Type UChooseNextWaypoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{	
//////PRIMER NODO GET PATROL POINTS
	//se obtienen los patrol points
	auto AIController = OwnerComp.GetAIOwner();
	auto ControlledPawn = AIController->GetPawn();
	//se castea el Pawn a Patrolling Guard para obtener referencia a los Patrol Points
	auto PatrollingGuard = Cast<APatrollingGuard>(ControlledPawn);
	auto PatrolPoints = PatrollingGuard->GetPatrolPointsCPP();

//////SEGUNDO NODO SET NEXT WAYPOINT
	//se toma referecia del blackboard relacionado
	auto BlackboardComp = OwnerComp.GetBlackboardComponent();
	auto Index = BlackboardComp->GetValueAsInt(IndexKey.SelectedKeyName);
	//TODO: proteger en caso de PatrolPoins == null
	BlackboardComp->SetValueAsObject(WaypointKey.SelectedKeyName, PatrolPoints[Index]);

//////TERCER NODO CYCLE INDEX
	auto NextIndex = (Index + 1) % PatrolPoints.Num();
	BlackboardComp->SetValueAsInt(IndexKey.SelectedKeyName, NextIndex);

	UE_LOG(LogTemp, Warning, TEXT("Index = %i"), Index)
	return EBTNodeResult::Succeeded;
}
