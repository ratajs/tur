#pragma once
#include "../common.hpp"
#include "./instruction.hpp"
#include "../machine/singleTapeMachineFactory.hpp"
#include "../machine/multiTapeMachineFactory.hpp"

/*!
 * This instruction clears the given tape on a given range.
 * The range must either start at the beginning or be rightwise unbounded.
 */
class ClearInstruction: public Instruction {
	private:
		size_t tape, index0;
		std::optional<size_t> index1;

	public:
		ClearInstruction(size_t tape, size_t index0, std::optional<size_t> index1);
		std::vector<size_t> listUsedTapes() const override;
		void build(SingleTapeMachineFactory &machineFactory, std::function<size_t (size_t)> getRealTape, std::function<const std::wstring &(size_t)> getState) const override;
		void build(MultiTapeMachineFactory &machineFactory, std::function<size_t (size_t)> getRealTape, std::function<const std::wstring &(size_t)> getState) const override;
		void print(std::wostream &stream, std::function<size_t (size_t)> getRealTape) const override;
};
