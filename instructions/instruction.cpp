#include "./instruction.hpp"
#include "../IO/unexpectedError.hpp"

std::optional<size_t> Instruction::getComeFromOrigin() const {
	return {};
};

bool Instruction::isGoToInstruction() const {
	return false;
};

std::vector<size_t> Instruction::getGoToDestinations() const {
	return {};
};

std::unique_ptr<Instruction> Instruction::tryToMerge(const Instruction &otherInstruction) const {
	return {};
};

std::optional<std::list<std::unique_ptr<Instruction>>> Instruction::tryToUnify() const {
	return {};
};

void Instruction::build(SingleTapeMachineFactory &machineFactory, std::function<size_t (size_t)> getRealTape, std::function<const std::wstring &(size_t)> getState) const {
	throw UnexpectedError(L"This instruction does not support single tape.");
};
