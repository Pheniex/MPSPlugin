// MPSPlugin by Pheniex

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "MPSSubsystem.generated.h"

//Declaring custom delegfates for Menu class to bind callbacks
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnCreateSessionComplete, bool, bWasSuccessfeul);
DECLARE_MULTICAST_DELEGATE_TwoParams(
    FMultiplayerOnFindSessionsComplete, const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessfeul);
DECLARE_MULTICAST_DELEGATE_OneParam(FMultiplayerOnJoinSessionComplete, EOnJoinSessionCompleteResult::Type Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnStartSessionComplete, bool, bWasSuccessfeul);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnDestroySessionComplete, bool, bWasSuccessfeul);

UCLASS()
class MPSPLUGIN_API UMPSSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UMPSSubsystem();

    void CreateSession(int32 NumPublicConnections, FString MatchType);
    void FindSessions(int32 MaxSearchResult);
    void JoinSession(const FOnlineSessionSearchResult& SessionResult);
    void DestroySession();
    void StartSession();

    //Custom delegates
    FMultiplayerOnCreateSessionComplete MultiplayerOnCreateSessionComplete;
    FMultiplayerOnFindSessionsComplete MultiplayerOnFindSessionsComplete;
    FMultiplayerOnJoinSessionComplete MultiplayerOnJoinSessionComplete;
    FMultiplayerOnStartSessionComplete MultiplayerOnStartSessionComplete;
    FMultiplayerOnDestroySessionComplete MultiplayerOnDestroySessionComplete;

protected:
    //callbacks for delegates
    void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
    void OnFindSessionsComlete(bool bWasSuccessful);
    void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
    void OnStartSessionComplete(FName SessionName, bool bWasSuccessful);
    void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

private:
    IOnlineSessionPtr SessionInterface;
    TSharedPtr<FOnlineSessionSettings> LastSessionSettings;
    TSharedPtr<FOnlineSessionSearch> LastSessionSearch;

    //OnlineSessionInterfaceDelegate list
    FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
    FDelegateHandle CreateSessionCompleteDelegateHandle;

    FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
    FDelegateHandle FindSessionsCompleteDelegateHandle;

    FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
    FDelegateHandle JoinSessionCompleteDelegateHandle;

    FOnStartSessionCompleteDelegate StartSessionCompleteDelegate;
    FDelegateHandle StartSessionCompleteDelegateHandle;

    FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;
    FDelegateHandle DestroySessionCompleteDelegateHandle;

    bool bCreateSessionOnDestroy{false};
    int32 LastNumOfPublicConnections;
    FString LastMatchType;
};
