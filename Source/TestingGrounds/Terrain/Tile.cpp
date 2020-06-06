// Fill out your copyright notice in the Description page of Project Settings.


#include "Tile.h"
#include "Engine/World.h"
//para dibujar una esfera que nos permita debuguear el Sweep de CastSphere
#include "DrawDebugHelpers.h"
//Para utilizar Pool of Actors
//#include "EngineUtils.h"
#include "ActorPool.h"

// Sets default values
ATile::ATile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//inicializando valores por defecto
	MinExtent = FVector(0, -2000, 0);
	MaxExtent = FVector(4000, 2000, 0);

}

void ATile::SetPool(UActorPool* InPool)
{
	UE_LOG(LogTemp, Warning, TEXT("[%s] Setting Pool %s"), *(this->GetName()), *(InPool->GetName()));
	Pool = InPool;

	PositionNavMeshBoundsVolume();
}

void ATile::PositionNavMeshBoundsVolume()
{
	//se obtiene Actor de la lista
	NavMeshBoundsVolume = Pool->Checkout();
	if (NavMeshBoundsVolume == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] Not enough actors in pool."), *GetName());
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("[%s] Checked out: {%s}"), *GetName(), *NavMeshBoundsVolume->GetName());
	NavMeshBoundsVolume->SetActorLocation(GetActorLocation());
}

//Metodo para generar(Spawn) los Actores, estos parametros se definen en el Blueprint
void ATile::PlaceActors(TSubclassOf<AActor> ToSpawn, int MinSpawn, int MaxSpawn, float Radius, float MinScale, float MaxScale){
/*	FVector Min(0, -2000, 0);
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
*/
	int NumberToSpawn = FMath::RandRange(MinSpawn, MaxSpawn);
	for (size_t i = 0; i < NumberToSpawn; i++)
	{	

		FVector SpawnPoint;
		float RandomScale = FMath::RandRange(MinScale, MaxScale);
		bool found = FindEmptyLocation(SpawnPoint, Radius * RandomScale); //el radio se ve afectado por el tamaño del objeto
		if (found) {
			//valor random para la rotacion, entre -180º y 180º
			float RandomRotation = FMath::RandRange(-180.f, 180.f);
			PlaceActor(ToSpawn, SpawnPoint, RandomRotation, RandomScale);
		}

		
	}
}

bool ATile::FindEmptyLocation(FVector& OutLocation, float Radius) {
	//FVector Min(0, -2000, 0);
	//FVector Max(4000, 2000, 0);
	//FBox Bounds(Min, Max);
	FBox Bounds(MinExtent, MaxExtent);

	//limite de intentos para buscar un lugar disponible
	const int MAX_ATTEMPTS = 50;
	for (size_t i = 0; i < MAX_ATTEMPTS; i++)
	{
		FVector CandidatePoint = FMath::RandPointInBox(Bounds);
		if (CanSpawnAtLocation(CandidatePoint, Radius))
		{
			OutLocation = CandidatePoint;
			return true;
		}
	}

	return false;
}

void ATile::PlaceActor(TSubclassOf<AActor> ToSpawn, FVector SpawnPoint, float Rotation, float Scale) {
	//genera el Actor indicado en la posicion origen del mundo
	AActor* Spawned = GetWorld()->SpawnActor<AActor>(ToSpawn);
	//se mueve el Actor a la posicion random generada
	Spawned->SetActorRelativeLocation(SpawnPoint);
	//se enlaza el Actor Spawned a Tile						(//como se enlazara con tile, //si se quedara pegado fijo a tile (true) o se vera influenciado por la fisica (false ))
	Spawned->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
	//asignando la rotacion al Actor - FRotator(Pitch, Yaw, Row)
	Spawned->SetActorRotation(FRotator(0, Rotation, 0));
	//asignando el tamaño random
	Spawned->SetActorScale3D(FVector(Scale));
}

// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();

	//para probar CastSphere
	//CanSpawnAtLocation(GetActorLocation(), 300);
	//CanSpawnAtLocation(GetActorLocation() + FVector(0,0,1000), 300);
	
	/*
	//pool de actores
	TActorIterator<AActor> ActorIterator = TActorIterator<AActor>(GetWorld());
	//iterando en la lista de Actors
	while (ActorIterator) {
		//obtener actor de la lista
		AActor* FoundActor = *ActorIterator;
		UE_LOG(LogTemp, Warning, TEXT("Actor Encontrado: %s"), *FoundActor->GetName());
		//mover el apuntador al siguiente Actor en la lista
		++ActorIterator; //ActorIterator++ no compilara
	}
	*/
}

void ATile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	Pool->Return(NavMeshBoundsVolume);
}

// Called every frame
void ATile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ATile::CanSpawnAtLocation(FVector Location, float Radius)
{
	FHitResult HitResult;

	FVector GlobalLocation = ActorToWorld().TransformPosition(Location);

	//en la ubicacion definida, hace un Sweep dentro del radio definido, virificando si hizo Hit con algo
	//se usa ByChannel por que no nos interesa el tipo de Objeto con el que se hace contacto
	bool HasHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		GlobalLocation,				//posicion inicial del Sweep
		GlobalLocation,				//posicion final del Sweep
		FQuat::Identity,		//se puede hacer Sweep con otros objetos, en esos casos una rotacion definida puede tener sentido, pero en el caso deuna esfera no, FQuat::Identity es basicamente Rotacion 0
		ECollisionChannel::ECC_GameTraceChannel2, //canal en el que se hara el Sweep, en este caso es todo lo que la camara pueda ver
		FCollisionShape::MakeSphere(Radius)	//definir la forma con la que se hara el Sweep
	);

	//para deicidir el color final se usa: Ternary Operator (A ? B : C)
//	FColor ResultColor = HasHit ? FColor::Red : FColor::Green;
	/*DrawDebugSphere(
		GetWorld(),
		Location, 
		Radius, 
		100, 
		ResultColor,
		true,				//para que no se borre la esfera despues de dibujar
		100					//tiempo de vida del dibujo 100segundos
	);*/
	/*DrawDebugCapsule(
		GetWorld(),
		GlobalLocation,
		0,					//HalfHeight
		Radius,
		FQuat::Identity,
		ResultColor,				
		true,				//para que no se borre la esfera despues de dibujar
		100
	);*/

	//se invierte porque queremos cuando no haga Hit, significa que el lugar esta disponible
	return !HasHit;
}