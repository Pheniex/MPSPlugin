// MPSPlugin by Pheniex


#include "MPSMenu.h"
#include "Components/Button.h"
#include "MPSSubsystem.h"

void UMPSMenu::MenuSetup(int32 PublicConnections, FString TypeOfMatch)
{
    NumPublicConnections = PublicConnections;
    MatchType = TypeOfMatch;

    AddToViewport();
    SetVisibility(ESlateVisibility::Visible);
    bIsFocusable = true;

    UWorld* World = GetWorld();
    if (World)
    {
        APlayerController* PlayerController = World->GetFirstPlayerController();
        if (PlayerController)
        {
            FInputModeUIOnly InputModeData;
            InputModeData.SetWidgetToFocus(TakeWidget());
            InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            PlayerController->SetInputMode(InputModeData);
            PlayerController->SetShowMouseCursor(true);
        }
    }

    UGameInstance* GameInstance = GetGameInstance();
    if (GameInstance)
    {
        MPSSubsystem = GameInstance->GetSubsystem<UMPSSubsystem>();
    }

    if (MPSSubsystem)
    {
        MPSSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
    }
}

bool UMPSMenu::Initialize()
{
    if (!Super::Initialize())
    {
        return false;
    }

    if (HostButton)
    {
        HostButton->OnClicked.AddDynamic(this, &ThisClass::HostButtonClicked);
    }
    if (JoinButton)
    {
        JoinButton->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);
    }

    return true;
}

void UMPSMenu::OnLevelRemovedFromWorld(ULevel* OnLevel, UWorld* OnWorld) 
{
    MenuTearDown();
    Super::OnLevelRemovedFromWorld(OnLevel, OnWorld);
}

void UMPSMenu::OnCreateSession(bool bWasSuccessful) 
{
    if (bWasSuccessful)
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow, FString(TEXT("Session created successful")));
        }

        UWorld* World = GetWorld();
        if (World)
        {
            World->ServerTravel("/Game/ThirdPerson/Maps/Lobby?listen");
        }
    }
    else
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString(TEXT("Session created FAILED")));
        }
    }
}

void UMPSMenu::OnFindSession(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessfeul) {}

void UMPSMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result) {}

void UMPSMenu::OnStartSession(bool bWasSuccessfeul) {}

void UMPSMenu::OnDestroySession(bool bWasSuccessfeul) {}

void UMPSMenu::HostButtonClicked() 
{
    if (MPSSubsystem)
    {
        MPSSubsystem->CreateSession(NumPublicConnections, MatchType);
    }
}

void UMPSMenu::JoinButtonClicked() 
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow, FString(TEXT("Join Button Clicked")));
    }
}

void UMPSMenu::MenuTearDown() 
{
    RemoveFromParent();
    UWorld* World = GetWorld();
    if (World)
    {
        APlayerController* PlayerController = World->GetFirstPlayerController();
        if (PlayerController)
        {
            FInputModeGameOnly InputMode;
            PlayerController->SetInputMode(InputMode);
            PlayerController->SetShowMouseCursor(false);
        }
    }
}
