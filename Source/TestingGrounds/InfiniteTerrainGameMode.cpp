// Fill out your copyright notice in the Description page of Project Settings.


#include "InfiniteTerrainGameMode.h"
//#include "AI/Navigation/NavMeshBoundsVolumen.h"
#include "GameFramework/Volume.h"
//Para utilizar Pool of Actors
#include "EngineUtils.h"

void AInfiniteTerrainGameMode::PopulateBoundsVolumePool() {
	//pool de actores - se cambia la clase generica del iterator de AActor a ANavMeshBoundsVolume, porque solo queremos ANavMeshBoundsVolume
	auto VolumeIterator = TActorIterator<AVolume>(GetWorld());
	//iterando en la lista de Actors
	while (VolumeIterator) {
		//obtener actor de la lista
		//ANavMeshBoundsVolume* FoundActor = *VolumeIterator;
		//UE_LOG(LogTemp, Warning, TEXT("Actor Encontrado: %s"), *FoundActor->GetName());
		//mover el apuntador al siguiente Actor en la lista
		//++VolumeIterator; //ActorIterator++ no compilara

		AddToPool(*VolumeIterator);
		++VolumeIterator;
	}
}

void AInfiniteTerrainGameMode::AddToPool(AVolume* VolumeToAdd){
	auto VolumeName = VolumeToAdd->GetName();
	if (VolumeName.Contains(FString("NavMesh"), ESearchCase::CaseSensitive, ESearchDir::FromStart)) {
		UE_LOG(LogTemp, Warning, TEXT("Actor Encontrado: %s"), *VolumeToAdd->GetName());
	}	
}

