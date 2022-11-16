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
    LastSessionSettings->bUseLobbiesIfAvailable = true;     //need for 5.0 and leater
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
    if (!SessionInterface.IsValid())
    {
        return;
    }

     FindSessionsCompleteDelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate);

     LastSessionSearch = MakeShareable(new FOnlineSessionSearch());
     LastSessionSearch->MaxSearchResults = MaxSearchResult;
     LastSessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
     LastSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

     const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
     if (!SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), LastSessionSearch.ToSharedRef()))
     {
         SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);

         MultiplayerOnFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
     }

     if (GEngine)                                                                                       //
     {                                                                                                  //
         GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString(TEXT("TRY FIND SESSION!")));   //
     }                                                                                                  //
}

void UMPSSubsystem::JoinSession(const FOnlineSessionSearchResult& SessionResult) 
{
    if (!SessionInterface.IsValid())
    {
        MultiplayerOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
        return;
    }

    JoinSessionCompleteDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);

    const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
    if (!SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionResult))
    {
        SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);

        MultiplayerOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
    }
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
    if (SessionInterface)
    {
        SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
    }

    if (LastSessionSearch->SearchResults.Num() <= 0)
    {
        MultiplayerOnFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);

        if (GEngine)                                                                                            //
        {                                                                                                       //
            GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString(TEXT("FOUND <= 0 SESSION!")));       //
        }                                                                                                       //

        return;
    }

    MultiplayerOnFindSessionsComplete.Broadcast(LastSessionSearch->SearchResults, bWasSuccessful);
}

void UMPSSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result) 
{
    if (SessionInterface)
    {
        SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
    }

    MultiplayerOnJoinSessionComplete.Broadcast(Result);
}

void UMPSSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful) 
{
}

void UMPSSubsystem::OnStartSessionComplete(FName SessionName, bool bWasSuccessful) 
{
}