// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h" 

#include "BasicClientGameInstanceSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTCPConnected);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTCPDisconnected);

class FSocket;
/**
 *
 */
UCLASS()
class TCPSTUDY0604_API UBasicClientGameInstanceSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "TCP")
	bool Connect(const FString& Host, int32 Port);

	UFUNCTION(BlueprintCallable, Category = "TCP")
	bool Disconnect();

	UFUNCTION(BlueprintCallable, Category = "TCP")
	bool IsConnected() const;

	UPROPERTY(BlueprintAssignable, Category = "TCP")
	FOnTCPConnected OnTCPConnected;
	UPROPERTY(BlueprintAssignable, Category = "TCP")
	FOnTCPDisconnected OnTCPDisconnected;

	void SendLogin(const FString& UserID, const FString& Password);

private:
	TArray<uint8> RecvBuffer;

	void RecvAll();
	bool SendAll(const uint8* Body, uint32 BodyLength);

	FSocket* ServerSocket = nullptr;

	void DispatchPacket();

	//FTickableGameObject 상속시 반드시 추가해야하는 함수
	virtual void Tick(float DeltaTime) override;
	TStatId GetStatId() const override;

};
