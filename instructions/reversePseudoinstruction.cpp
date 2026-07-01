#include "./reversePseudoinstruction.hpp"

std::vector<size_t> ReversePseudoinstruction::listUsedTapes() const {
	return {};
};

void ReversePseudoinstruction::build(SingleTapeMachineFactory &machineFactory, std::function<size_t (size_t)> getRealTape, std::function<const std::wstring &(size_t)> getState) const {
	machineFactory.reverse();
};

void ReversePseudoinstruction::build(MultiTapeMachineFactory &machineFactory, std::function<size_t (size_t)> getRealTape, std::function<const std::wstring &(size_t)> getState) const {
	machineFactory.reverse();
};

void ReversePseudoinstruction::print(std::wostream &stream, std::function<size_t (size_t)> getRealTape) const {
	stream << L"REVERSE" << std::endl;
};
