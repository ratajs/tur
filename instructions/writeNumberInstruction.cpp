#include "./writeNumberInstruction.hpp"
#include <initializer_list>
#include "../IO/unexpectedError.hpp"

/*!
 * The constructor of WriteNumberInstruction.
 * \param tape The destination tape.
 * \param index The destination index, {} (std::nullopt) if the number should be appended to the tape.
 * \param number The constant to write on the tape.
 */
WriteNumberInstruction::WriteNumberInstruction(size_t tape, std::optional<size_t> index, size_t number): tape(tape), number(number), index(index) {};

WriteNumberInstruction::WriteNumberInstruction(IrArguments &arguments): tape(arguments.readTape()) {
//std::optional<size_t> index1;

	/*
	std::tie(this->index, index1) = arguments.readRange(); //TODO destination can be []

	if(index1)
		throw GeneralError(L"Invalid range for writeNumber on line "+Format::blue(std::to_wstring(arguments.getLineNumber()))+L".");
	*/

	this->index = arguments.readRightwiseUnboundedRange();

	arguments.readComma();
	this->number = arguments.readNumber();
	arguments.end();
};

std::vector<size_t> WriteNumberInstruction::listUsedTapes() const {
	return { this->tape };
};

void WriteNumberInstruction::build(SingleTapeMachineFactory &machineFactory, std::function<size_t (size_t)> getRealTape, std::function<const std::wstring &(size_t)> getState) const {
	if(getRealTape(this->tape)!=1)
		throw UnexpectedError(L"Other real tape than 1 appeared in a single tape machine instruction.");

	if(this->index)
		machineFactory.writeNumber(*this->index, this->number);
	else
		machineFactory.appendNumber(this->number);
};

void WriteNumberInstruction::build(MultiTapeMachineFactory &machineFactory, std::function<size_t (size_t)> getRealTape, std::function<const std::wstring &(size_t)> getState) const {
	if(this->index)
		machineFactory.writeNumber(getRealTape(this->tape), *this->index, this->number);
	else
		machineFactory.appendNumber(getRealTape(this->tape), this->number);
};

void WriteNumberInstruction::print(std::wostream &stream, std::function<size_t (size_t)> getRealTape) const {
	stream << L"writeNumber(" << getRealTape(this->tape) << L"[" << (this->index ? (std::to_wstring(*this->index)+L":") : L"") << "], " << this->number << L")" << std::endl;
};
