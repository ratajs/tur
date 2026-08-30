#include "./clearInstruction.hpp"
#include <tuple>
#include <initializer_list>
#include "../IO/unexpectedError.hpp"
#include "../IO/irParseError.hpp"

/*!
 * The constructor of ClearInstruction.
 * \param tape The tape to clear.
 * \param index0 The start index of the range to clear (inclusive).
 * \param index1 The end index of the range to clear (exclusive; {} if the range is rightwise unbounded).
 * \throw UnexpectedError If the range represents neither the beginning (index0 is 0) nor the end (index1 is {}) of the tape.
 */
ClearInstruction::ClearInstruction(size_t tape, size_t index0, std::optional<size_t> index1, bool isIndex0FromEnd):
	isIndex0FromEnd(isIndex0FromEnd), tape(tape), index0(index0), index1(index1) {
		if((index0 > 0 && index1) || (isIndex0FromEnd && index1))
			throw UnexpectedError(L"Invalid clear (neither beginning nor end).");
	};

/*!
 * An alternative constructor of ClearInstruction.
 * The arguments are extracted from an instance of IrArguments.
 * The tape and range (the first index followed by a colon and an optional second index, all in square brackets) is expected in the arguments.
 * \param arguments The arguments of the instruction from the IR input.
 * \throw IrParseError If the arguments do not match the expected format.
 */
ClearInstruction::ClearInstruction(IrArguments &arguments): tape(arguments.readTape()) {
	std::tie(this->index0, this->index1, this->isIndex0FromEnd) = arguments.readRange(true);
	arguments.end();

	if((this->index0 > 0 && this->index1) || (this->isIndex0FromEnd && this->index1))
		throw IrParseError(IrParseError::Type::INVALID_RANGE_FOR_CLEAR, arguments.getLocation()); //TODO more precise location?
};

std::vector<size_t> ClearInstruction::listUsedTapes() const {
	return { this->tape };
};

void ClearInstruction::build(SingleTapeMachineFactory &machineFactory, std::function<size_t (size_t)> getRealTape, std::function<const std::wstring &(size_t)> getState) const {
	if(getRealTape(this->tape)!=1)
		throw UnexpectedError(L"Other real tape than 1 appeared in a single tape machine instruction.");

	if(this->index0==0 && this->index1)
		machineFactory.clearBeginning(*this->index1);
	else
		machineFactory.clearEnd(this->index0, this->isIndex0FromEnd);
};

void ClearInstruction::build(MultiTapeMachineFactory &machineFactory, std::function<size_t (size_t)> getRealTape, std::function<const std::wstring &(size_t)> getState) const {
	if(this->index0==0 && this->index1)
		machineFactory.clearBeginning(getRealTape(this->tape), *this->index1);
	else
		machineFactory.clearEnd(getRealTape(this->tape), this->index0, this->isIndex0FromEnd);
};

void ClearInstruction::print(std::wostream &stream, std::function<size_t (size_t)> getRealTape) const {
	stream << L"clear(" << getRealTape(this->tape) << L"[" << (this->isIndex0FromEnd ? L"−" : L"") << this->index0 << L":" << (this->index1 ? std::to_wstring(*this->index1) : L"") << L"])" << std::endl;
};

//TODO merge with appending
