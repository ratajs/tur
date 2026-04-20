#pragma once
#include "../common.hpp"
#include "./instruction.hpp"
#include "../machine/machine.hpp"
#include "../machine/singleTapeMachineFactory.hpp"
#include "../machine/multiTapeMachineFactory.hpp"

/*!
 * This instruction simulates a given Turing machine on a given tape.
 */
class CallInstruction: public Instruction {
	private:
		size_t tape;
		const Machine &machine;

	public:
		CallInstruction(size_t tape, const Machine &machine);
		std::vector<size_t> listUsedTapes() const override;
		void build(SingleTapeMachineFactory &machineFactory, std::function<size_t (size_t)> getRealTape, std::function<const std::wstring &(size_t)> getState) const override;
		void build(MultiTapeMachineFactory &machineFactory, std::function<size_t (size_t)> getRealTape, std::function<const std::wstring &(size_t)> getState) const override;
		void print(std::wostream &stream, std::function<size_t (size_t)> getRealTape) const override;
};
