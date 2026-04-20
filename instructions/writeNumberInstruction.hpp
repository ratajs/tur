#pragma once
#include "../common.hpp"
#include "./instruction.hpp"
#include "../machine/singleTapeMachineFactory.hpp"
#include "../machine/multiTapeMachineFactory.hpp"

/*!
 * This instruction writes a number to a given place on a given tape.
 */
class WriteNumberInstruction: public Instruction {
	private:
		size_t tape, number;
		std::optional<size_t> index; // Append if {}

	public:
		WriteNumberInstruction(size_t tape, std::optional<size_t> index, size_t number);
		std::vector<size_t> listUsedTapes() const override;
		void build(SingleTapeMachineFactory &machineFactory, std::function<size_t (size_t)> getRealTape, std::function<const std::wstring &(size_t)> getState) const override;
		void build(MultiTapeMachineFactory &machineFactory, std::function<size_t (size_t)> getRealTape, std::function<const std::wstring &(size_t)> getState) const override;
		void print(std::wostream &stream, std::function<size_t (size_t)> getRealTape) const override;
};
