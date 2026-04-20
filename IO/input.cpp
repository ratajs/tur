#include "./input.hpp"
#include "../IO/format.hpp"
#include "../IO/generalError.hpp"
#include "../IO/unexpectedError.hpp"

std::wstring Input::readFile(std::filesystem::path fileName) {
	std::wstring text;
	std::wifstream ifs;

	if(!std::filesystem::exists(fileName))
		throw GeneralError(L"Input file does not exist.");

	if(!std::filesystem::is_regular_file(fileName))
		throw GeneralError(L"Input file is not a regular file.");

	ifs = std::wifstream(fileName);

	if(!ifs)
		throw GeneralError(L"Input file cannot be read.");

	ifs.peek();

	if(ifs.eof()) {
		ifs.close();

		return text;
	};

	std::getline(ifs, text, L'\0');

	if(ifs.fail() || ifs.bad()) {
		ifs.close();

		throw GeneralError(L"An error occurred while reading the input file.");
	};

	ifs.close();

	return text;
};

std::vector<size_t> Input::readNumbers(std::wistream &stream) {
	std::wstring line;
	std::wistringstream iss;
	std::vector<size_t> numbers;

	std::getline(stream, line);
	iss = std::wistringstream(std::move(line));
	std::copy(std::istream_iterator<size_t, wchar_t>(iss), std::istream_iterator<size_t, wchar_t>(), std::back_inserter(numbers));

	if(!iss.eof())
		throw GeneralError(L"Invalid input.");

	return numbers;
};

Input::Input(const std::vector<std::wstring> &arguments, std::wistream &defaultInputStream, std::wostream &defaultOutputStream): defaultInputStream(defaultInputStream), defaultOutputStream(defaultOutputStream) {
	this->parseOptions(arguments);
};

std::filesystem::path Input::getBasePath(const std::filesystem::path &fileName) const {
	std::filesystem::path path;

	if(this->baseIncludePath)
		return (*this->baseIncludePath);

	path = fileName;
	path.remove_filename();

	return path;
};

void Input::parseOptions(const std::vector<std::wstring> &arguments) {
	bool areOptionsAllowed = true;
	std::vector<std::wstring> plainArguments;
	std::optional<std::function<void (const std::wstring&)>> optionValueCallback;

	if(arguments.empty())
		throw GeneralError(L"Argument 0 is missing.");

	this->argument0 = arguments[0];

	std::ranges::for_each(std::next(arguments.begin()), arguments.end(),
		[this, &plainArguments, &areOptionsAllowed, &optionValueCallback](const std::wstring &argument) -> void {
			wchar_t lastOption;

			if(optionValueCallback) {
				(*optionValueCallback)(argument);
				optionValueCallback = {};
			}
			else if(!areOptionsAllowed || argument.empty() || argument[0]!='-' || argument.length() < 2) // Regular argument
				plainArguments.emplace_back(argument);
			else if(argument[1]!='-') { // Short option(s)
				std::ranges::for_each(std::next(argument.begin()), argument.end(), [this, &optionValueCallback, &lastOption](wchar_t option) -> void {
					if(optionValueCallback)
						throw GeneralError(L"Option "+Format::blue(L"-"+(std::wstring { lastOption }))+L" expected a value.");
					optionValueCallback = this->processShortOption(option);
					lastOption = option;
				});
			}
			else if(argument.length()==2) // --
				areOptionsAllowed = false;
			else // Long option
				optionValueCallback = this->processLongOption({ argument.begin() + 2, argument.end() });
		}
	);

	if(optionValueCallback)
		throw GeneralError(L"Option "+Format::blue(arguments.back())+L" expected a value.");

	if(!this->flags.isFlagPresent(Flags::Flag::COMPILE) && !this->flags.isFlagPresent(Flags::Flag::RUN))
		this->flags.addFlag(Flags::Flag::COMPILE); // If neither -c nor -r is provided, assume -c

	if(this->flags.isFlagPresent(Flags::Flag::COMPILE) && this->flags.isFlagPresent(Flags::Flag::RUN))
		this->handlePlainArgumentsForCompilationAndRun(std::move(plainArguments));
	else if(this->flags.isFlagPresent(Flags::Flag::COMPILE))
		this->handlePlainArgumentsForCompilation(std::move(plainArguments));
	else if(this->flags.isFlagPresent(Flags::Flag::RUN))
		this->handlePlainArgumentsForRun(std::move(plainArguments));
};

void Input::handlePlainArgumentsForCompilation(std::vector<std::wstring> arguments) {
	switch(arguments.size()) {
		case 0:
			break;

		case 1:
			this->inputFilePath = std::move(arguments[0]);

			break;

		case 2:
			this->inputFilePath = std::move(arguments[0]);
			this->outputFilePath = std::move(arguments[1]);

			break;

		default:
			throw GeneralError(L"Too many arguments.");
	};
};

void Input::handlePlainArgumentsForRun(std::vector<std::wstring> arguments) {
	switch(arguments.size()) {
		case 0:
			throw GeneralError(L"One argument required.");

		case 1:
			this->inputFilePath = std::move(arguments[0]);

			break;

		default:
			throw GeneralError(L"Too many arguments.");
	};
};

void Input::handlePlainArgumentsForCompilationAndRun(std::vector<std::wstring> arguments) {
	if(this->getFlags().isFlagPresent(Flags::Flag::ONLY_INSTRUCTIONS))
		throw GeneralError(L"--only-instructions and --run cannot be used at the same time.");

	switch(arguments.size()) {
		case 0:
			throw GeneralError(L"One argument required.");

		case 1:
			this->inputFilePath = std::move(arguments[0]);

			break;

		default:
			throw GeneralError(L"Too many arguments.");
	};
};

std::optional<std::function<void (const std::wstring&)>> Input::processShortOption(wchar_t option) {
	switch(option) {
		case 'c': // compile
			this->flags.addFlag(Flags::Flag::COMPILE);

			return {};

		case 'i': // instructions
			this->flags.addFlag(Flags::Flag::INSTRUCTIONS);

			return {};

		case 'I': // only-instructions
			this->flags.addFlag(Flags::Flag::ONLY_INSTRUCTIONS);

			return {};

		case 'O': // optimize
			this->flags.addFlag(Flags::Flag::OPTIMIZE);

			return {};

		case 'b': // include-base
			return ([this](const std::wstring &baseIncludePath) -> void { this->baseIncludePath = baseIncludePath; });

		case 'x': // include-suffix
			return ([this](const std::wstring &includeMachineFileSuffix) -> void { this->includeMachineFileSuffix = includeMachineFileSuffix; });

		case 'm': // machine
			return ([this](const std::wstring &machinePath) -> void { this->providedMachines.emplace_back(machinePath); });

		case 'r': // run
			this->flags.addFlag(Flags::Flag::RUN);

			return {};

		case 's': // steps
			this->flags.addFlag(Flags::Flag::STEPS);

			return {};

		case 'k': // keep-blanks
			this->flags.addFlag(Flags::Flag::KEEP_BLANKS);

			return {};

		case 'e': // encode
			this->flags.addFlag(Flags::Flag::ENCODE);

			return {};

		case 'd': // decode
			this->flags.addFlag(Flags::Flag::DECODE);

			return {};

		case 'n': // numbers
			this->flags.addFlag(Flags::Flag::ENCODE);
			this->flags.addFlag(Flags::Flag::DECODE);

			return {};

		case 'C': // no-color
			this->flags.addFlag(Flags::Flag::NO_COLOR);

			return {};

		default:
			throw GeneralError(L"Unknown short option: "+Format::red(L"-"+std::wstring({ option })));
	};
};

std::optional<std::function<void (const std::wstring&)>> Input::processLongOption(const std::wstring_view &option) {
	if(option==L"compile") {
		this->flags.addFlag(Flags::Flag::COMPILE);

		return {};
	}
	else if(option==L"instructions") {
		this->flags.addFlag(Flags::Flag::INSTRUCTIONS);

		return {};
	}
	else if(option==L"only-instructions") {
		this->flags.addFlag(Flags::Flag::ONLY_INSTRUCTIONS);

		return {};
	}
	else if(option==L"optimize") {
		this->flags.addFlag(Flags::Flag::OPTIMIZE);

		return {};
	}
	else if(option==L"include-base")
		return ([this](const std::wstring &baseIncludePath) -> void { this->baseIncludePath = baseIncludePath; });
	else if(option==L"include-suffix")
		return ([this](const std::wstring &includeMachineFileSuffix) -> void { this->includeMachineFileSuffix = includeMachineFileSuffix; });
	else if(option==L"machine")
		return ([this](const std::wstring &machinePath) -> void { this->providedMachines.emplace_back(machinePath); });
	else if(option==L"run") {
		this->flags.addFlag(Flags::Flag::RUN);

		return {};
	}
	else if(option==L"steps") {
		this->flags.addFlag(Flags::Flag::STEPS);

		return {};
	}
	else if(option==L"keep-blanks") {
		this->flags.addFlag(Flags::Flag::KEEP_BLANKS);

		return {};
	}
	else if(option==L"encode") {
		this->flags.addFlag(Flags::Flag::ENCODE);

		return {};
	}
	else if(option==L"decode") {
		this->flags.addFlag(Flags::Flag::DECODE);

		return {};
	}
	else if(option==L"numbers") {
		this->flags.addFlag(Flags::Flag::ENCODE);
		this->flags.addFlag(Flags::Flag::DECODE);

		return {};
	}
	else if(option==L"no-color") {
		this->flags.addFlag(Flags::Flag::NO_COLOR);

		return {};
	};

	throw GeneralError(L"Unknown long option: "+Format::red(L"--"+std::wstring(option)));
};

void Input::openOutputFileStream(const std::filesystem::path &fileName) {
	std::wstring text;
	std::wofstream ofs;

	if(std::filesystem::exists(fileName) && !std::filesystem::is_regular_file(fileName))
		throw GeneralError(L"Output file exists and it is not a regular file.");

	ofs = std::wofstream(fileName);

	if(!ofs)
		throw GeneralError(L"Output file cannot be opened for writing.");

	this->outputFileStream = std::move(ofs);
};


Flags Input::getFlags() const {
	return this->flags;
};

std::filesystem::path Input::getBasePath() const {
	return this->getBasePath(this->inputFilePath ? (*this->inputFilePath) : "./");
};

std::wstring Input::getMachineFileSuffix() const {
	return this->includeMachineFileSuffix.value_or(L".tm");
};

const std::vector<std::filesystem::path> &Input::getProvidedMachines() const {
	return this->providedMachines;
};

const std::wstring &Input::getArgument0() const {
	return this->argument0;
};

std::wostream &Input::getOutputStream() {
	if(this->outputFilePath && !this->outputFileStream)
		this->openOutputFileStream(*this->outputFilePath);

	return (this->outputFileStream ? (*this->outputFileStream) : this->defaultOutputStream);
};

std::wstring Input::readSource() {
	std::wstring source;

	if(!this->flags.isFlagPresent(Flags::Flag::COMPILE))
		throw UnexpectedError(L"Cannot read source without the --compile flag.");

	if(this->inputFilePath)
		source = Input::readFile(*this->inputFilePath);
	else
		std::getline(this->defaultInputStream, source, L'\0');

	return source;
};

Tape Input::readTape() {
	Tape tape;

	if(!this->flags.isFlagPresent(Flags::Flag::RUN))
		throw UnexpectedError(L"Cannot read tape without the --run flag.");

	if(this->flags.isFlagPresent(Flags::Flag::ENCODE))
		tape.init(Input::readNumbers(this->defaultInputStream));
	else {
		if(!(this->defaultInputStream >> tape) || this->defaultInputStream.peek()!='\n')
			throw GeneralError(L"Invalid tape.");
	};

	return tape;
};

Machine Input::readMachine() {
	std::wstring machineDefinition;
	Machine machine;

	if(!this->flags.isFlagPresent(Flags::Flag::RUN))
		throw UnexpectedError(L"Cannot read machine without the --run flag.");

	if(this->flags.isFlagPresent(Flags::Flag::COMPILE))
		throw UnexpectedError(L"Cannot read machine with the --compile flag.");

	if(!this->inputFilePath)
		throw UnexpectedError(L"No input file for the machine.");

	machineDefinition = Input::readFile(*this->inputFilePath);
	machine = { machineDefinition };

	if(!machine)
		throw GeneralError(L"Invalid machine.");

	return machine;
};
