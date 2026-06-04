// Fill out your copyright notice in the Description page of Project Settings.

#include "BasicClientGameInstanceSubsystem.h"
#include "SocketSubsystem.h"
#include "Sockets.h"
#include "IPAddress.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "UserPacket_generated.h"
bool UBasicClientGameInstanceSubsystem::Connect(const FString& Host, int32 Port)
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

bool UBasicClientGameInstanceSubsystem::Disconnect()
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

bool UBasicClientGameInstanceSubsystem::IsConnected() const
{
	return ServerSocket != nullptr && ServerSocket->GetConnectionState() == SCS_Connected;
}

void UBasicClientGameInstanceSubsystem::SendLogin(const FString& UserID, const FString& Password)
{
	flatbuffers::FlatBufferBuilder Builder;

	const FTCHARToUTF8 UserIDUTF8(UserID);
	const FTCHARToUTF8 PasswordUTF8(Password);

	//패킷 만들어서 서버에 보내기
	auto LoginData = UserPacket::CreateC2S_LoginDirect(
		Builder,
		UserIDUTF8.Get(),
		PasswordUTF8.Get()
	);

	auto PacketData = UserPacket::CreatePacketData(
		Builder,
		UserPacket::PacketType_C2S_Login,
		LoginData.Union()
	);

	Builder.Finish(PacketData);
	
	SendAll(Builder.GetBufferPointer(), Builder.GetSize());
}

void UBasicClientGameInstanceSubsystem::RecvAll()
{
	if (!ServerSocket)
	{
		return;
	}

	//2byte 헤더 받기
	uint32 Pending = 0;

	int32 RecvBytes = 0;
	uint16 PacketSize = 0;
	while (ServerSocket->HasPendingData(Pending))
	{
		if (ServerSocket->Recv((uint8*)&PacketSize, sizeof(PacketSize), RecvBytes) || RecvBytes == 0)
		{
			Disconnect();
			break;
		}

		if (RecvBytes == 2)
		{
			break;
		}
	}

	//데이터 받기
	while (ServerSocket->HasPendingData(Pending))
	{
		if (ServerSocket->Recv(RecvBuffer.GetData(), PacketSize, RecvBytes) || RecvBytes == 0)
		{
			Disconnect();
			break;
		}

		if (RecvBytes == PacketSize)
		{
			break;
		}
	}

	if (RecvBytes > 0)
	{
		RecvBuffer.SetNum(RecvBytes);
		DispatchPacket();
		RecvBuffer.Reset();
	}
}

bool UBasicClientGameInstanceSubsystem::SendAll(const uint8* Body, uint32 BodyLength)
{
	TArray<uint8> Packet;
	Packet.Reserve(2+ BodyLength); //헤더크기 + 데이터크기
	/*Packet.Add((uint8)(BodyLength>>8)& 0xFF);
	Packet.Add((uint8)(BodyLength & 0xFF));*/
	//또는
	FMemory::Memcpy(Packet.GetData(),&BodyLength,2);
	Packet.SetNum(2); //위치를 헤더뒤로 움직임
	Packet.Append(Body, BodyLength);

	int32 SentTotalBytes = 0;
	while (SentTotalBytes < Packet.Num())
	{
		int32 SentBytes = 0;
		if (ServerSocket->Send(Packet.GetData() + SentTotalBytes, Packet.Num() - SentTotalBytes, SentBytes) || SentBytes < 0)
		{
			return false;
		}
		SentTotalBytes += SentBytes;
	}

	return true;
}

void UBasicClientGameInstanceSubsystem::DispatchPacket()
{
	//플렛버퍼 -> 데이터 추출
	const auto UserPacketData = UserPacket::GetPacketData(RecvBuffer.GetData());
	switch (UserPacketData->data_type())
	{
	case UserPacket::PacketType_S2C_Login:
	{
		//Delegate로 바꿈
		const auto* LoginData = UserPacketData->data_as_S2C_Login();

		//메시지 비어있다면 빈값으로
		FString Message = LoginData->message() ? UTF8_TO_TCHAR(LoginData->message()) : FString();

		UE_LOG(LogTemp, Warning, TEXT("Login %d %s"), LoginData->client_socket_id(), *Message);
	}
	break;
	case UserPacket::PacketType_S2C_Spawn:
	{
	}
	break;
	case UserPacket::PacketType_S2C_Move:
	{

	}
	break;
	case UserPacket::PacketType_S2C_Destroy:
	{
	}
	break;

	case UserPacket::PacketType_S2C_ChangeColor:
	{
	}
	break;

	case UserPacket::PacketType_S2C_Signup:
	{
		auto SignupPacket = UserPacketData->data_as_S2C_Signup();

		{

		}
	}
	break;
	}
}

void UBasicClientGameInstanceSubsystem::Tick(float DeltaTime)
{
	RecvAll();
}

TStatId UBasicClientGameInstanceSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UBasicClientGameInstanceSubsystem, STATGROUP_Tickables);
}
