#include "./decompressInstruction.hpp"
#include "../IO/unexpectedError.hpp"

/*!
 * The constructor of DecompressInstruction.
 * \param destination The destination where to put the input (the number of the input tape).
 */
DecompressInstruction::DecompressInstruction(size_t destination): destination(destination) {};

std::vector<size_t> DecompressInstruction::listUsedTapes() const {
	return { this->destination };
};

void DecompressInstruction::build(SingleTapeMachineFactory &machineFactory, std::function<size_t (size_t)> getRealTape, std::function<const std::wstring &(size_t)> getState) const {
	if(getRealTape(this->destination)!=1)
		throw UnexpectedError(L"Other real tape than 1 appeared in a single tape machine instruction.");
};

void DecompressInstruction::build(MultiTapeMachineFactory &machineFactory, std::function<size_t (size_t)> getRealTape, std::function<const std::wstring &(size_t)> getState) const {
	machineFactory.decompress(getRealTape(this->destination));
};

void DecompressInstruction::print(std::wostream &stream, std::function<size_t (size_t)> getRealTape) const {
	stream << L"decompress(" << getRealTape(this->destination) << L")" << std::endl;
};
