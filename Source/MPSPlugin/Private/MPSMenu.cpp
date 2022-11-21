// MPSPlugin by Pheniex


#include "MPSMenu.h"
#include "Components/Button.h"
#include "MPSSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"

void UMPSMenu::MenuSetup(int32 PublicConnections, FString TypeOfMatch, FString PathToLobby)
{
    LobbyPath = FString::Printf(TEXT("%s?listen"), *PathToLobby);
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
        MPSSubsystem->MultiplayerOnFindSessionsComplete.AddUObject(this, &ThisClass::OnFindSessions);
        MPSSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);
        MPSSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);
        MPSSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
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
            World->ServerTravel(LobbyPath);
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

void UMPSMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessfeul) 
{
    if (MPSSubsystem == nullptr)
    {
        return;
    }

    for (auto Res : SessionResults)
    {
        FString SettingsValue;
        Res.Session.SessionSettings.Get(FName("MatchType"), SettingsValue);
        if (SettingsValue == MatchType)
        {
            MPSSubsystem->JoinSession(Res);
            return;
        }
    }
}

void UMPSMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result) 
{
    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
    if (Subsystem)
    {
        IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
        if (SessionInterface.IsValid())
        {
            FString Adr;
            SessionInterface->GetResolvedConnectString(NAME_GameSession, Adr);

            APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
            if (PlayerController)
            {
                PlayerController->ClientTravel(Adr, ETravelType::TRAVEL_Absolute);
            }
        }
    }
}

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
    if (MPSSubsystem)
    {
        MPSSubsystem->FindSessions(10000);
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
