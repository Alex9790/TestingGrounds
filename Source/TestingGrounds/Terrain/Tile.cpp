// Fill out your copyright notice in the Description page of Project Settings.


#include "Tile.h"
#include "Engine/World.h"

// Sets default values
ATile::ATile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

//Metodo para generar(Spawn) los Actores, estos parametros se definen en el Blueprint
void ATile::PlaceActors(TSubclassOf<AActor> ToSpawn, int MinSpawn, int MaxSpawn){
	FVector Min(0, -2000, 0);
	FVector Max(4000, 2000, 0);
	FBox Bounds(Min, Max);
	int NumberToSpawn = FMath::RandRange(MinSpawn, MaxSpawn);
	for (size_t i = 0; i < NumberToSpawn; i++)
	{
		//Valor random dentro del espacio definido
		FVector SpawnPoint = FMath::RandPointInBox(Bounds);
		//UE_LOG(LogTemp, Warning, TEXT("SpawnPoint: %s"), *SpawnPoint.ToCompactString());
		//genera el Actor indicado en la posicion origen del mundo
		AActor* Spawned = GetWorld()->SpawnActor<AActor>(ToSpawn);
		//se mueve el Actor a la posicion random generada
		Spawned->SetActorRelativeLocation(SpawnPoint);
		//se enlaza el Actor Spawned a Tile						(//como se enlazara con tile, //si se quedara pegado fijo a tile (true) o se vera influenciado por la fisica (false ))
		Spawned->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
	}
}

// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

