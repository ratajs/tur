#pragma once
#include "../common.hpp"
#include "./instruction.hpp"
#include "../machine/singleTapeMachineFactory.hpp"
#include "../machine/multiTapeMachineFactory.hpp"

/*!
 * This instruction should be the first instruction in every program.
 * It creates a multi-tape system with the original input preserved on one of the tapes.
 * If the machine is built in the single-tape mode, this instruction does nothing.
 */
class DecompressInstruction: public Instruction {
	private:
		size_t destination;

	public:
		DecompressInstruction(size_t destination);
		std::vector<size_t> listUsedTapes() const override;
		void build(SingleTapeMachineFactory &machineFactory, std::function<size_t (size_t)> getRealTape, std::function<const std::wstring &(size_t)> getState) const override;
		void build(MultiTapeMachineFactory &machineFactory, std::function<size_t (size_t)> getRealTape, std::function<const std::wstring &(size_t)> getState) const override;
		void print(std::wostream &stream, std::function<size_t (size_t)> getRealTape) const override;
};
