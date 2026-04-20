#pragma once
#include "../common.hpp"
#include "./flags.hpp"
#include "../machine/tape.hpp"
#include "../machine/machine.hpp"

class Input {
	private:
		static std::wstring readFile(std::filesystem::path fileName);
		static std::vector<size_t> readNumbers(std::wistream &stream);

		Flags flags;
		std::wstring argument0;
		std::optional<std::wstring> includeMachineFileSuffix;
		std::optional<std::filesystem::path> baseIncludePath, inputFilePath, outputFilePath;
		std::wistream &defaultInputStream;
		std::wostream &defaultOutputStream;
		std::optional<std::wofstream> outputFileStream;
		std::vector<std::filesystem::path> providedMachines;

		std::filesystem::path getBasePath(const std::filesystem::path &fileName) const;
		void parseOptions(const std::vector<std::wstring> &arguments);
		void handlePlainArgumentsForCompilation(std::vector<std::wstring> arguments);
		void handlePlainArgumentsForRun(std::vector<std::wstring> arguments);
		void handlePlainArgumentsForCompilationAndRun(std::vector<std::wstring> arguments);
		std::optional<std::function<void (const std::wstring&)>> processShortOption(wchar_t option);
		std::optional<std::function<void (const std::wstring&)>> processLongOption(const std::wstring_view &option);
		void openOutputFileStream(const std::filesystem::path &fileName);

	public:
		Input(const std::vector<std::wstring> &arguments, std::wistream &defaultInputStream = std::wcin, std::wostream &defaultOutputStream = std::wcout);
		Flags getFlags() const;
		std::filesystem::path getBasePath() const;
		std::wstring getMachineFileSuffix() const;
		const std::vector<std::filesystem::path> &getProvidedMachines() const;
		const std::wstring &getArgument0() const;
		std::wostream &getOutputStream();
		std::wstring readSource();
		Tape readTape();
		Machine readMachine();
};
