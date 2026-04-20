#include "./instruction.hpp"
#include "../IO/unexpectedError.hpp"

Instruction::~Instruction() {};

std::unique_ptr<Instruction> Instruction::tryToMerge(const Instruction &otherInstruction) const {
	return {};
};

std::unique_ptr<Instruction> Instruction::tryToMergeWithCopy(size_t source, size_t destination, size_t sourceIndex0, std::optional<size_t> sourceIndex1, std::optional<size_t> destinationIndex) const {
	return {};
};

std::optional<std::list<std::unique_ptr<Instruction>>> Instruction::tryToUnify() const {
	return {};
};

void Instruction::build(SingleTapeMachineFactory &machineFactory, std::function<size_t (size_t)> getRealTape, std::function<const std::wstring &(size_t)> getState) const {
	throw UnexpectedError(L"This instruction does not support single tape.");
};
