// Fill out your copyright notice in the Description page of Project Settings.

using System;
using System.IO;
using System.Configuration;
using Tools.DotNETCommon;
using UnrealBuildTool;

public class GSDKLibrary : ModuleRules
{
	public GSDKLibrary(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;

		PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "include"));

        string OutputDirectory = Path.Combine(ModuleDirectory, "x64", "Release");

        PublicAdditionalLibraries.Add(Path.Combine(OutputDirectory, "gsdklib.lib"));
        
        if (Target.Type != global::UnrealBuildTool.TargetType.Server) return;

        // Ensure that the DLL is staged along with the executable
		RuntimeDependencies.Add("$(TargetOutputDir)/libcrypto-1_1-x64.dll", Path.Combine(OutputDirectory, "libcrypto-1_1-x64.dll"), StagedFileType.SystemNonUFS);

        RuntimeDependencies.Add("$(TargetOutputDir)/libcurl.dll", Path.Combine(OutputDirectory, "libcurl.dll"), StagedFileType.SystemNonUFS);

        RuntimeDependencies.Add("$(TargetOutputDir)/libssl-1_1-x64.dll", Path.Combine(OutputDirectory, "libssl-1_1-x64.dll"), StagedFileType.SystemNonUFS);

        // Add dynamic dlls required by all dedicated servers
        RuntimeDependencies.Add("$(TargetOutputDir)/xinput1_3.dll", "$(EngineDir)/Binaries/ThirdParty/AppLocalDependencies/Win64/DirectX/xinput1_3.dll", StagedFileType.SystemNonUFS);
        RuntimeDependencies.Add("$(TargetOutputDir)/concrt140.dll", "$(EngineDir)/Binaries/ThirdParty/AppLocalDependencies/Win64/Microsoft.VC.CRT/concrt140.dll", StagedFileType.SystemNonUFS);
        RuntimeDependencies.Add("$(TargetOutputDir)/msvcp140.dll", "$(EngineDir)/Binaries/ThirdParty/AppLocalDependencies/Win64/Microsoft.VC.CRT/msvcp140.dll", StagedFileType.SystemNonUFS);
        RuntimeDependencies.Add("$(TargetOutputDir)/msvcp140_1.dll", "$(EngineDir)/Binaries/ThirdParty/AppLocalDependencies/Win64/Microsoft.VC.CRT/msvcp140_1.dll", StagedFileType.SystemNonUFS);
        RuntimeDependencies.Add("$(TargetOutputDir)/msvcp140_2.dll", "$(EngineDir)/Binaries/ThirdParty/AppLocalDependencies/Win64/Microsoft.VC.CRT/msvcp140_2.dll", StagedFileType.SystemNonUFS);
        RuntimeDependencies.Add("$(TargetOutputDir)/vccorlib140.dll", "$(EngineDir)/Binaries/ThirdParty/AppLocalDependencies/Win64/Microsoft.VC.CRT/vccorlib140.dll", StagedFileType.SystemNonUFS);
        RuntimeDependencies.Add("$(TargetOutputDir)/vcruntime140.dll", "$(EngineDir)/Binaries/ThirdParty/AppLocalDependencies/Win64/Microsoft.VC.CRT/vcruntime140.dll", StagedFileType.SystemNonUFS);
        
        var Sys32Path = Path.GetPathRoot(Environment.SystemDirectory) + "Windows/System32"; //Assumed windows
        RuntimeDependencies.Add("$(TargetOutputDir)/D3DCompiler_43.dll", Sys32Path + "/D3DCompiler_43.dll", StagedFileType.SystemNonUFS);
        RuntimeDependencies.Add("$(TargetOutputDir)/DXCore.dll", Sys32Path + "/DXCore.dll", StagedFileType.SystemNonUFS);
        RuntimeDependencies.Add("$(TargetOutputDir)/nvapi64.dll", Sys32Path + "/nvapi64.dll", StagedFileType.SystemNonUFS);
        RuntimeDependencies.Add("$(TargetOutputDir)/pnrpnsp.dll", Sys32Path + "/pnrpnsp.dll", StagedFileType.SystemNonUFS);
        RuntimeDependencies.Add("$(TargetOutputDir)/umpdc.dll", Sys32Path + "/umpdc.dll", StagedFileType.SystemNonUFS);
        RuntimeDependencies.Add("$(TargetOutputDir)/vcruntime140_1.dll", Sys32Path + "/vcruntime140_1.dll", StagedFileType.SystemNonUFS);
        RuntimeDependencies.Add("$(TargetOutputDir)/winrnr.dll", Sys32Path + "/winrnr.dll", StagedFileType.SystemNonUFS);
        RuntimeDependencies.Add("$(TargetOutputDir)/wshbth.dll", Sys32Path + "/wshbth.dll", StagedFileType.SystemNonUFS);
	}
}