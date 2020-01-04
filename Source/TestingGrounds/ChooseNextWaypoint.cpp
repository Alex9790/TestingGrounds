// Fill out your copyright notice in the Description page of Project Settings.


#include "BehaviorTree/BlackboardComponent.h"
#include "ChooseNextWaypoint.h"
#include "AIController.h"
#include "PatrolRoute.h" 
EBTNodeResult::Type UChooseNextWaypoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{	
//////PRIMER NODO GET PATROL POINTS
	//se obtienen los patrol route
	auto AIController = OwnerComp.GetAIOwner();
	auto ControlledPawn = AIController->GetPawn();
	auto PatrolRoute = ControlledPawn->FindComponentByClass<UPatrolRoute>();

	//se protege en caso de no tener PatrolRoute Cmponent
	if(!ensure(PatrolRoute)){ return EBTNodeResult::Failed; }

	//se castea el Pawn a Patrolling Guard para obtener referencia a los Patrol Points
	//auto PatrollingGuard = Cast<APatrollingGuard>(ControlledPawn);
	//auto PatrolPoints = PatrollingGuard->GetPatrolPointsCPP();

	auto PatrolPoints = PatrolRoute->GetPatrolPointsCPP();
	//se alerta en caso de no tener Patrol Points asignados
	if (PatrolPoints.Num() == 0) {
		UE_LOG(LogTemp, Warning, TEXT("No se han asignado Patrol Points."))
		return EBTNodeResult::Failed;
	}

//////SEGUNDO NODO SET NEXT WAYPOINT
	//se toma referecia del blackboard relacionado
	auto BlackboardComp = OwnerComp.GetBlackboardComponent();
	auto Index = BlackboardComp->GetValueAsInt(IndexKey.SelectedKeyName);	
	BlackboardComp->SetValueAsObject(WaypointKey.SelectedKeyName, PatrolPoints[Index]);

//////TERCER NODO CYCLE INDEX
	auto NextIndex = (Index + 1) % PatrolPoints.Num();
	BlackboardComp->SetValueAsInt(IndexKey.SelectedKeyName, NextIndex);

	UE_LOG(LogTemp, Warning, TEXT("Index = %i"), Index)
	return EBTNodeResult::Succeeded;
}
