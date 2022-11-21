// MPSPlugin by Pheniex

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "MPSMenu.generated.h"

class UButton;
class UMPSSubsystem;

UCLASS()
class MPSPLUGIN_API UMPSMenu : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void MenuSetup(int32 PublicConnections = 4, FString TypeOfMatch = FString(TEXT("FreeForAll")),
        FString PathToLobby = FString(TEXT("/Game/ThirdPerson/Maps/Lobby")));

protected:
    virtual bool Initialize() override;
    virtual void OnLevelRemovedFromWorld(ULevel* OnLevel, UWorld* OnWorld) override;

    // Callbacks for custom delegates on MPSSubsystem
    UFUNCTION()
    void OnCreateSession(bool bWasSuccessful);
    void OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessfeul);
    void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);
    UFUNCTION()
    void OnStartSession(bool bWasSuccessfeul);
    UFUNCTION()
    void OnDestroySession(bool bWasSuccessfeul);

private:
    UPROPERTY(meta = (BindWidget))
    UButton* HostButton;

    UPROPERTY(meta = (BindWidget))
    UButton* JoinButton;

    UFUNCTION()
    void HostButtonClicked();

    UFUNCTION()
    void JoinButtonClicked();

    void MenuTearDown();

    // subsystem to handle all online functionality
    UMPSSubsystem* MPSSubsystem;

    int32 NumPublicConnections{4};
    FString MatchType{TEXT("FreeForAll")};
    FString LobbyPath{TEXT("")};
};
