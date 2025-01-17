using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using UnrealBuildTool;

public class Mono : ModuleRules
{
	public Mono(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;

		PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "src"));

		var LibraryPath = Path.Combine(ModuleDirectory, "lib");

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, Target.Platform.ToString(), "coreclr.import.lib"));

			PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, Target.Platform.ToString(),
				"mono-component-debugger-static.lib"));

			PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, Target.Platform.ToString(),
				"mono-component-diagnostics_tracing-static.lib"));

			PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, Target.Platform.ToString(),
				"mono-component-hot_reload-static.lib"));

			PublicAdditionalLibraries.Add(
				Path.Combine(LibraryPath, Target.Platform.ToString(), "mono-profiler-aot.lib"));

			PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, Target.Platform.ToString(), "monosgen-2.0.lib"));

			RuntimeDependencies.Add("$(BinaryOutputDir)/coreclr.dll",
				Path.Combine(LibraryPath, Target.Platform.ToString(), "coreclr.dll"));

			RuntimeDependencies.Add("$(BinaryOutputDir)/mono-sgen.pdb",
				Path.Combine(LibraryPath, Target.Platform.ToString(), "mono-sgen.pdb"));

			var Files = GetFiles(Path.Combine(LibraryPath, Target.Platform.ToString(), "net7.0"));

			foreach (var File in Files)
			{
				var ModuleLastDirectory = Path.GetFullPath(Path.Combine(ModuleDirectory, ".."));

				var DestPath = File.Substring(ModuleLastDirectory.Length + 1,
					File.Length - ModuleLastDirectory.Length - 1);

				RuntimeDependencies.Add("$(BinaryOutputDir)/" + DestPath, File);
			}
		}
		else if (Target.Platform == UnrealTargetPlatform.Android)
		{
			PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, Target.Platform.ToString(), "libmonosgen-2.0.a"));

			var Files = GetFiles(Path.Combine(LibraryPath, Target.Platform.ToString(), "net7.0"));

			foreach (var File in Files)
			{
				var ModuleLastDirectory = Path.GetFullPath(Path.Combine(ModuleDirectory, ".."));

				var DestPath = File.Substring(ModuleLastDirectory.Length + 1,
					File.Length - ModuleLastDirectory.Length - 1);

				RuntimeDependencies.Add("$(BinaryOutputDir)/" + DestPath, File);
			}

			string APLName = "Mono_APL.xml";

			string RelativeAPLPath = Utils.MakePathRelativeTo(Path.Combine(ModuleDirectory, "lib", "Android"),
				Target.RelativeEnginePath);

			AdditionalPropertiesForReceipt.Add("AndroidPlugin", Path.Combine(RelativeAPLPath, APLName));
		}
		else if (Target.Platform == UnrealTargetPlatform.Linux)
		{
			PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, Target.Platform.ToString(), "libmonosgen-2.0.a"));

			PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, Target.Platform.ToString(),
				"libmono-component-debugger-static.a"));

			PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, Target.Platform.ToString(),
				"libmono-component-diagnostics_tracing-static.a"));

			PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, Target.Platform.ToString(),
				"libmono-component-hot_reload-static.a"));

			RuntimeDependencies.Add("$(BinaryOutputDir)/libcoreclr.so",
				Path.Combine(LibraryPath, Target.Platform.ToString(), "libcoreclr.so"));

			RuntimeDependencies.Add("$(BinaryOutputDir)/libcoreclr.so.dbg",
				Path.Combine(LibraryPath, Target.Platform.ToString(), "libcoreclr.so.dbg"));

			RuntimeDependencies.Add("$(BinaryOutputDir)/libSystem.Native.so",
				Path.Combine(LibraryPath, Target.Platform.ToString(), "libSystem.Native.so"));

			RuntimeDependencies.Add("$(BinaryOutputDir)/libSystem.Native.so.dbg",
				Path.Combine(LibraryPath, Target.Platform.ToString(), "libSystem.Native.so.dbg"));

			var Files = GetFiles(Path.Combine(LibraryPath, Target.Platform.ToString(), "net7.0"));

			foreach (var File in Files)
			{
				var ModuleLastDirectory = Path.GetFullPath(Path.Combine(ModuleDirectory, ".."));

				var DestPath = File.Substring(ModuleLastDirectory.Length + 1,
					File.Length - ModuleLastDirectory.Length - 1);

				RuntimeDependencies.Add("$(BinaryOutputDir)/" + DestPath, File);
			}
		}
		else if (Target.Platform == UnrealTargetPlatform.Mac)
		{
			var Platform = String.Format("macOS_{0}", base.Target.Architecture);

			PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, Platform, "libmonosgen-2.0.a"));

			PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, Platform,
				"libmono-component-debugger-static.a"));

			PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, Platform,
				"libmono-component-diagnostics_tracing-static.a"));

			PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, Platform,
				"libmono-component-hot_reload-static.a"));

			RuntimeDependencies.Add("$(BinaryOutputDir)/libcoreclr.dylib",
				Path.Combine(LibraryPath, Platform, "libcoreclr.dylib"));

			RuntimeDependencies.Add("$(BinaryOutputDir)/libcoreclr.dylib.dwarf",
				Path.Combine(LibraryPath, Platform, "libcoreclr.dylib.dwarf"));

			RuntimeDependencies.Add("$(BinaryOutputDir)/libSystem.Native.dylib",
				Path.Combine(LibraryPath, Platform, "libSystem.Native.dylib"));

			RuntimeDependencies.Add("$(BinaryOutputDir)/libSystem.IO.Compression.Native.dylib",
				Path.Combine(LibraryPath, Platform, "libSystem.IO.Compression.Native.dylib"));

			RuntimeDependencies.Add("$(BinaryOutputDir)/libSystem.Security.Cryptography.Native.Apple.dylib",
				Path.Combine(LibraryPath, Platform, "libSystem.Security.Cryptography.Native.Apple.dylib"));

			RuntimeDependencies.Add("$(BinaryOutputDir)/libSystem.Security.Cryptography.Native.OpenSsl.dylib",
				Path.Combine(LibraryPath, Platform, "libSystem.Security.Cryptography.Native.OpenSsl.dylib"));

			RuntimeDependencies.Add("$(BinaryOutputDir)/libSystem.Net.Security.Native.dylib",
				Path.Combine(LibraryPath, Platform, "libSystem.Net.Security.Native.dylib"));

			RuntimeDependencies.Add("$(BinaryOutputDir)/libSystem.Globalization.Native.dylib",
				Path.Combine(LibraryPath, Platform, "libSystem.Globalization.Native.dylib"));

			RuntimeDependencies.Add("$(BinaryOutputDir)/libSystem.IO.Ports.Native.dylib",
				Path.Combine(LibraryPath, Platform, "libSystem.IO.Ports.Native.dylib"));

			var Files = GetFiles(Path.Combine(LibraryPath, Platform, "net7.0"));

			foreach (var File in Files)
			{
				var ModuleLastDirectory = Path.GetFullPath(Path.Combine(ModuleDirectory, ".."));

				var DestPath = File.Substring(ModuleLastDirectory.Length + 1,
					File.Length - ModuleLastDirectory.Length - 1);

				RuntimeDependencies.Add("$(BinaryOutputDir)/" + DestPath, File);
			}
		}
		else if (Target.Platform == UnrealTargetPlatform.IOS)
		{
			PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, Target.Platform.ToString(),
				"System.Private.CoreLib.dll.a"));

			PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, Target.Platform.ToString(), "libmonosgen-2.0.a"));

			PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, Target.Platform.ToString(),
				"libmono-component-debugger-static.a"));

			PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, Target.Platform.ToString(),
				"libmono-component-diagnostics_tracing-static.a"));

			PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, Target.Platform.ToString(),
				"libmono-component-hot_reload-static.a"));

			PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, Target.Platform.ToString(),
				"libicudata.a"));

			PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, Target.Platform.ToString(),
				"libicui18n.a"));

			PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, Target.Platform.ToString(),
				"libicuuc.a"));

			PublicAdditionalFrameworks.Add(
				new Framework(
					"Mono",
					Path.Combine(LibraryPath, Target.Platform.ToString(), "Mono.embeddendframework.zip"),
					null,
					true
				)
			);

			var Files = GetFiles(Path.Combine(LibraryPath, Target.Platform.ToString(), "net7.0"));

			foreach (var File in Files)
			{
				var ModuleLastDirectory = Path.GetFullPath(Path.Combine(ModuleDirectory, ".."));

				var DestPath = File.Substring(ModuleLastDirectory.Length + 1,
					File.Length - ModuleLastDirectory.Length - 1);

				RuntimeDependencies.Add("$(BinaryOutputDir)/" + DestPath, File);
			}
		}
	}

	private static IEnumerable<string> GetFiles(string InDirectory, string InPattern = "*.*")
	{
		var Files = new List<string>();

		var Strings = Directory.GetFiles(InDirectory, InPattern);

		foreach (var File in Strings)
		{
			Files.Add(File);
		}

		foreach (var File in Directory.GetDirectories(InDirectory))
		{
			Files.AddRange(GetFiles(File, InPattern));
		}

		return Files;
	}
}