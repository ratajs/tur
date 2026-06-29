#include "./compressInstruction.hpp"
#include <initializer_list>
#include "../IO/unexpectedError.hpp"

/*!
 * The constructor of CompressInstruction.
 * \param source The tape with the content which should be preserved (the output tape number).
 */
CompressInstruction::CompressInstruction(size_t source): source(source) {};

/*!
 * An alternative constructor of CompressInstruction.
 * The arguments are extracted from an instance of IrArguments.
 * Only the source tape number is expected in the arguments.
 * \param arguments The arguments of the instruction from the IR input.
 * \throw IrParseError If the arguments do not match the expected format.
 */
CompressInstruction::CompressInstruction(IrArguments &arguments): source(arguments.readTape()) {
	arguments.end();
};

std::vector<size_t> CompressInstruction::listUsedTapes() const {
	return { this->source };
};

void CompressInstruction::build(SingleTapeMachineFactory &machineFactory, std::function<size_t (size_t)> getRealTape, std::function<const std::wstring &(size_t)> getState) const {
	if(getRealTape(this->source)!=1)
		throw UnexpectedError(L"Other real tape than 1 appeared in a single tape machine instruction.");
};

void CompressInstruction::build(MultiTapeMachineFactory &machineFactory, std::function<size_t (size_t)> getRealTape, std::function<const std::wstring &(size_t)> getState) const {
	machineFactory.compress(getRealTape(this->source));
};

void CompressInstruction::print(std::wostream &stream, std::function<size_t (size_t)> getRealTape) const {
	stream << L"compress(" << getRealTape(this->source) << L")" << std::endl;
};
