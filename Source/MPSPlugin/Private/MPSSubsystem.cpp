// MPSPlugin by Pheniex

#include "MPSSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"

UMPSSubsystem::UMPSSubsystem()
    : CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete)),
      FindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsComlete)),
      JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete)),
      StartSessionCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnStartSessionComplete)),
      DestroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionComplete))
{
    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
    if (Subsystem)
    {
        SessionInterface = Subsystem->GetSessionInterface();
    }
}

void UMPSSubsystem::CreateSession(int32 NumPublicConnections, FString MatchType) 
{
    if (!SessionInterface.IsValid())
    {
        return;
    }

    auto ExsistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
    if (ExsistingSession != nullptr)
    {
        SessionInterface->DestroySession(NAME_GameSession);
    }

    //Store delegate in FDelegateHandle
    CreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

    LastSessionSettings = MakeShareable(new FOnlineSessionSettings());
    LastSessionSettings->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
    LastSessionSettings->NumPublicConnections = NumPublicConnections;
    LastSessionSettings->bAllowJoinInProgress = true;
    LastSessionSettings->bAllowJoinViaPresence = true;
    LastSessionSettings->bShouldAdvertise = true;
    LastSessionSettings->bUsesPresence = true;
    //SessionSettings->bUseLobbiesIfAvailable = true;
    LastSessionSettings->Set(FName("MatchType"), MatchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

    const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
    if (!SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *LastSessionSettings))
    {
        SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);

        //Broadcast custom delegates
        MultiplayerOnCreateSessionComplete.Broadcast(false);
    }
}

void UMPSSubsystem::FindSessions(int32 MaxSearchResult) 
{
}

void UMPSSubsystem::JoinSession(const FOnlineSessionSearchResult& SessionResult) 
{
}

void UMPSSubsystem::StartSession() 
{
}

void UMPSSubsystem::DestroySession() 
{
}

void UMPSSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful) 
{
    if (SessionInterface)
    {
        SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
    }

    MultiplayerOnCreateSessionComplete.Broadcast(bWasSuccessful);
}

void UMPSSubsystem::OnFindSessionsComlete(bool bWasSuccessful) 
{
}

void UMPSSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result) 
{
}

void UMPSSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful) 
{
}

void UMPSSubsystem::OnStartSessionComplete(FName SessionName, bool bWasSuccessful) 
{
}