#include "./compressInstruction.hpp"
#include "../IO/unexpectedError.hpp"

/*!
 * The constructor of CompressInstruction.
 * \param source The tape with the content which should be preserved (the output tape number).
 */
CompressInstruction::CompressInstruction(size_t source): source(source) {};

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
