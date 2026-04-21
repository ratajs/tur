#include "./common.hpp"
#include "./IO/input.hpp"
#include "./IO/flags.hpp"
#include "./IO/includeResolver.hpp"
#include "./IO/error.hpp"
#include "./IO/warning.hpp"
#include "./IO/generalError.hpp"
#include "./machine/tape.hpp"
#include "./machine/machine.hpp"
#include "./parsing/token.hpp"
#include "./parsing/lexer.hpp"
#include "./parsing/parser.hpp"
#include "./AST/program.hpp"
#include "./building/instructionBuilder.hpp"
#include "./building/instructionCollection.hpp"
#include "./building/machineBuilder.hpp"

static Machine compile(Input &input) {
	std::wstring source;
	std::vector<Token> tokens;
	Machine machine;
	Program program;
	InstructionBuilder instructionBuilder;
	InstructionCollection instructions;
	std::vector<std::unique_ptr<Warning>> warnings;

	source = input.readSource();

	tokens = Lexer(source, std::back_inserter(warnings)).analyze();
	program = Parser(tokens, { input.getBasePath(), input.getMachineFileSuffix(), input.getProvidedMachines() }, std::back_inserter(warnings)).parse().extractProgram();
	program.checkForWarnings(std::back_inserter(warnings));

	if(input.getFlags().isFlagPresent(Flags::Flag::OPTIMIZE))
		instructionBuilder.allowInstructionMerging();

	program.build(instructionBuilder);
	instructions = instructionBuilder.extractInstructions();

	if(input.getFlags().isFlagPresent(Flags::Flag::OPTIMIZE))
		instructions.optimize();

	if(input.getFlags().isFlagPresent(Flags::Flag::ONLY_INSTRUCTIONS))
		instructions.printInstructions(input.getOutputStream());
	else if(input.getFlags().isFlagPresent(Flags::Flag::INSTRUCTIONS))
		instructions.printInstructions(std::wcerr);

	if(!input.getFlags().isFlagPresent(Flags::Flag::ONLY_INSTRUCTIONS)) {
		machine = MachineBuilder(std::move(instructions)).buildMachine();
		if(!input.getFlags().isFlagPresent(Flags::Flag::RUN))
			input.getOutputStream() << machine << std::endl;
	};

	std::ranges::for_each(warnings,
		[](const std::unique_ptr<Warning> &warning) -> void {
			std::wcerr << (*warning) << std::endl;
		}
	);

	return machine;
};

static void run(Input &input, Machine machine) {
	std::optional<std::vector<size_t>> numbers;
	std::vector<size_t>::const_iterator it;
	Tape tape;

	tape = input.readTape();

	if(input.getFlags().isFlagPresent(Flags::Flag::STEPS))
		machine.run(tape, input.getFlags().isFlagPresent(Flags::Flag::KEEP_BLANKS), {}, std::wcerr, !input.getFlags().isFlagPresent(Flags::Flag::NO_COLOR));
	else
		machine.run(tape, input.getFlags().isFlagPresent(Flags::Flag::KEEP_BLANKS));

	if(input.getFlags().isFlagPresent(Flags::Flag::DECODE)) {
		numbers = tape.getNumbers();

		if(!numbers)
			throw GeneralError(L"Output tape content is invalid.");

		for(it = numbers->begin(); it!=numbers->end(); it++) {
			input.getOutputStream() << (*it);
			if(std::next(it)!=numbers->end())
				input.getOutputStream() << L" ";
		};
		input.getOutputStream() << std::endl;
	}
	else {
		tape.print(input.getOutputStream(), !input.getFlags().isFlagPresent(Flags::Flag::NO_COLOR));
		input.getOutputStream() << std::endl;
	};
};

static void doWork(Input input) {
	if(input.getFlags().isFlagPresent(Flags::Flag::COMPILE) && input.getFlags().isFlagPresent(Flags::Flag::RUN))
		run(input, compile(input));
	else if(input.getFlags().isFlagPresent(Flags::Flag::COMPILE))
		compile(input);
	else if(input.getFlags().isFlagPresent(Flags::Flag::RUN))
		run(input, input.readMachine());
};

int main(int argc, char **argv) {
	std::vector<std::wstring> arguments;
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

	std::locale::global(std::locale("C.UTF-8"));

	std::ranges::transform(argv, argv + argc, std::back_inserter(arguments), [&converter](const char *argument) -> std::wstring { return converter.from_bytes(argument); });

	try {
		doWork(Input(arguments));
	} catch(const Error &error) {
		std::wcerr << error << std::endl;

		std::locale::global(std::locale::classic());

		return 1;
	};

	std::locale::global(std::locale::classic());

	return 0;
};
