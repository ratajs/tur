#include "./callInstruction.hpp"
#include <utility>
#include <initializer_list>
#include <string_view>
#include "../IO/unexpectedError.hpp"

/*!
 * The constructor of CallInstruction.
 * \param tape The tape on which the machine shall be simulated.
 * \param machine The Turing machine to simulate.
 */
CallInstruction::CallInstruction(size_t tape, Machine machine): tape(tape), machine(std::move(machine)) {};

/*!
 * An alternative constructor of CallInstruction.
 * The arguments are extracted from an instance of IrArguments.
 * The tape, comma, and the machine in braces is expected in the arguments.
 * \param arguments The arguments of the instruction from the IR input.
 * \throw IrParseError If the arguments do not match the expected format.
 */
CallInstruction::CallInstruction(IrArguments &arguments): tape(arguments.readTape()), machine((arguments.readComma(), arguments.readMachine())) {
	arguments.end();
};

std::vector<size_t> CallInstruction::listUsedTapes() const {
	return { this->tape };
};

void CallInstruction::build(SingleTapeMachineFactory &machineFactory, std::function<size_t (size_t)> getRealTape, std::function<const std::wstring &(size_t)> getState) const {
	if(getRealTape(this->tape)!=1)
		throw UnexpectedError(L"Other real tape than 1 appeared in a single tape machine instruction.");

	machineFactory.simulate(this->machine);
};

void CallInstruction::build(MultiTapeMachineFactory &machineFactory, std::function<size_t (size_t)> getRealTape, std::function<const std::wstring &(size_t)> getState) const {
	machineFactory.simulate(getRealTape(this->tape), this->machine);
};

void CallInstruction::print(std::wostream &stream, std::function<size_t (size_t)> getRealTape) const {
	stream << L"call(" << getRealTape(this->tape) << L", { ";
	this->machine.print(stream, L"", L" ");
	stream << L" })" << std::endl;
};
