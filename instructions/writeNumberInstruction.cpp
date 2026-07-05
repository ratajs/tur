#include "./writeNumberInstruction.hpp"
#include <initializer_list>
#include "../IO/unexpectedError.hpp"

/*!
 * The constructor of WriteNumberInstruction.
 * \param tape The destination tape.
 * \param index The destination index, {} (std::nullopt) if the number should be appended to the tape.
 * \param number The constant to write on the tape.
 * \param isIndexFromEnd Whether the destination is indexed from the end.
 * \throw UnexpectedError If no index is provided but indexing from the is requested.
 */
WriteNumberInstruction::WriteNumberInstruction(size_t tape, std::optional<size_t> index, size_t number, bool isIndexFromEnd): isIndexFromEnd(isIndexFromEnd), tape(tape), number(number), index(index) {
	if(isIndexFromEnd && !index)
		throw UnexpectedError(L"Indexing from the end should only be used with a specific index.");
};

/*!
 * An alternative constructor of WriteNumberInstruction.
 * The arguments are extracted from an instance of IrArguments.
 * The target tape, destination (an index followed by a colon, all in square brackets, or empty square brackets), a comma, and the number is expected in the arguments.
 * \param arguments The arguments of the instruction from the IR input.
 * \throw IrParseError If the arguments do not match the expected format.
 */
WriteNumberInstruction::WriteNumberInstruction(IrArguments &arguments): tape(arguments.readTape()) { //TODO indexed from the end
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
		machineFactory.writeNumber(*this->index, this->number, this->isIndexFromEnd);
	else
		machineFactory.appendNumber(this->number);
};

void WriteNumberInstruction::build(MultiTapeMachineFactory &machineFactory, std::function<size_t (size_t)> getRealTape, std::function<const std::wstring &(size_t)> getState) const {
	if(this->index)
		machineFactory.writeNumber(getRealTape(this->tape), *this->index, this->number, this->isIndexFromEnd);
	else
		machineFactory.appendNumber(getRealTape(this->tape), this->number);
};

void WriteNumberInstruction::print(std::wostream &stream, std::function<size_t (size_t)> getRealTape) const {
	stream << L"writeNumber(" << getRealTape(this->tape) << L"[" << (this->isIndexFromEnd ? L"−" : L"") << (this->index ? (std::to_wstring(*this->index)+L":") : L"") << "], " << this->number << L")" << std::endl;
};
