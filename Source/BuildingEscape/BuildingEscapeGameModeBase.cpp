// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.


#include "BuildingEscapeGameModeBase.h"
#include "UObject/ConstructorHelpers.h"

ABuildingEscapeGameModeBase::ABuildingEscapeGameModeBase()
{
    static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/BP_FirstPersonCharacter"));

    DefaultPawnClass = PlayerPawnClassFinder.Class;
}