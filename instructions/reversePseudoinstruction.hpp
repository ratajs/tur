#pragma once
#include <cstdlib>
#include <vector>
#include <string>
#include <iostream>
#include <functional>
#include "./instruction.hpp"

/*!
 * This is a special instruction for mirroring of all computation.
 * The content of all tapes will be interpreted as mirrored until the next occurence of this instruction.
 * The placement in the list of instruction is important (other instructions being between REVERSE pseudoinstructions), as well as the execution of REVERSE itself.
 */
class ReversePseudoinstruction: public Instruction {
	public:
		std::vector<size_t> listUsedTapes() const override;
		void build(SingleTapeMachineFactory &machineFactory, std::function<size_t (size_t)> getRealTape, std::function<const std::wstring &(size_t)> getState) const override;
		void build(MultiTapeMachineFactory &machineFactory, std::function<size_t (size_t)> getRealTape, std::function<const std::wstring &(size_t)> getState) const override;
		void print(std::wostream &stream, std::function<size_t (size_t)> getRealTape) const override;
	//TODO merge with reverse
};
