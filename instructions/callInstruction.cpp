#include "./callInstruction.hpp"
#include "../IO/unexpectedError.hpp"

/*!
 * The constructor of CallInstruction.
 * \param tape The tape on which the machine shall be simulated.
 * \param machine The Turing machine to simulate.
 */
CallInstruction::CallInstruction(size_t tape, const Machine &machine): tape(tape), machine(machine) {};

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
