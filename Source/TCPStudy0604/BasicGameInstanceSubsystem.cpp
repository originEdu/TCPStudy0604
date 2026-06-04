// Fill out your copyright notice in the Description page of Project Settings.

#include "BasicGameInstanceSubsystem.h"
#include "SocketSubsystem.h"
#include "Sockets.h"
#include "IPAddress.h"
#include "Interfaces/IPv4/IPv4Address.h"

bool UBasicGameInstanceSubsystem::Connect(const FString& Host, int32 Port)
{
	//OS별 소켓 생성
	ISocketSubsystem* SocketSubSystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	
	//dns이름주면 Ip 받음
	FAddressInfoResult AddInfo = SocketSubSystem->GetAddressInfo(
		*Host,                                     // 호스트 이름 (FString)
		nullptr,                                   // 서비스 이름 또는 포트 (보통 nullptr이나 "" 문자열)
		EAddressInfoFlags::Default,					// 플래그
		NAME_None
	);

	//소켓주소
	TSharedRef<FInternetAddr> ServerAddr = AddInfo.Results[0].Address;
	//포트연결
	ServerAddr->SetPort(Port);
	//socket()
	ServerSocket = SocketSubSystem->CreateSocket(NAME_Stream,TEXT("TCPClient"),ServerAddr->GetProtocolType());
	//connect()
	if (!ServerSocket->Connect(*ServerAddr))
	{
		UE_LOG(LogTemp, Warning, TEXT("Connect fail to: %s"), *Host);
		return false;
	}

	RecvBuffer.Reset();
	OnTCPConnected.Broadcast();

	return false;
}

bool UBasicGameInstanceSubsystem::Disconnect()
{
	//소켓
	ISocketSubsystem* SocketSubSystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	//closesocket
	ServerSocket->Close();
	SocketSubSystem->DestroySocket(ServerSocket);

	ServerSocket = nullptr;

	OnTCPDisconnected.Broadcast();
	return false;
}

bool UBasicGameInstanceSubsystem::IsConnected() const
{
	return false;
}

void UBasicGameInstanceSubsystem::RecvAll()
{
	//헤더 받기
	if (!ServerSocket)
	{
		return;
	}



	DispatchPacket();

}

bool UBasicGameInstanceSubsystem::SendAll(const uint8* Body, uint32 BodyLength)
{
	return false;
}

void UBasicGameInstanceSubsystem::DispatchPacket()
{
	//플렛버퍼만들어서 구조화
}
