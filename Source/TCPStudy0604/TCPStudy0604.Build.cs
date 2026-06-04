// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class TCPStudy0604 : ModuleRules
{
	public TCPStudy0604(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { 
            "Core", 
            "CoreUObject", 
            "Engine", 
            "InputCore", 
            "EnhancedInput",
            "Sockets",
            "Networking"
        });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true

        //장비 추가나 라이브러리 추가 할때
        string IncludePath = Path.Combine(ModuleDirectory,"..", "Include");
        PublicIncludePaths.Add(IncludePath);

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            // 1. 윈도우 전용 사전처리기(매크로) 정의
            //PublicDefinitions.Add("WITH_MY_WIN64_LIBRARY=1");

            //// 2. 라이브러리 파일(.lib)이 있는 경로 지정
            //string LibPath = Path.Combine(ModuleDirectory, "..", "ThirdParty", "Lib", "Win64");
            //PublicAdditionalLibraries.Add(Path.Combine(LibPath, "ThirdPartyLib.lib"));

            //// 3. 실행 시 필요한 DLL 파일이 있다면 바이너리 폴더에 복사하도록 설정
            //string DllPath = Path.Combine(ModuleDirectory, "..", "ThirdParty", "Bin", "Win64", "ThirdPartyLib.dll");
            //RuntimeDependencies.Add("$(BinaryOutputDir)/ThirdPartyLib.dll", DllPath);

            //[강사님 코드]
            //windows
            //string LibraryPath = Path.Combine(ModuleDirectory, "..", "ThirdParty", "lib");
            ////library 파일 추가 
            //PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, "Win64", "MySQL.lib"));
        }
        else if (Target.Platform == UnrealTargetPlatform.Android)
        {
        }
        else if (Target.Platform == UnrealTargetPlatform.IOS)
        {
        }

    }
}
