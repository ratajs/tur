#pragma once
#include "../common.hpp"
#include "./instruction.hpp"
#include "../machine/singleTapeMachineFactory.hpp"
#include "../machine/multiTapeMachineFactory.hpp"

/*!
 * This instruction compares the length of a given tape (number of numbers on it) and jumps to different labels depending on the result of the comparison.
 * The next instruction should be a JumpInstruction of the COME_FROM type.
 */
class CompareTapeLengthInstruction: public Instruction {
	public:
		enum class Type {
			EQ, NE, LT, LTE, GT, GTE
		};

	private:
		CompareTapeLengthInstruction::Type type;
		size_t tape, number, trueLabel, falseLabel;

	public:
		CompareTapeLengthInstruction(size_t tape, size_t number, size_t trueLabel, size_t falseLabel, CompareTapeLengthInstruction::Type type);
		std::vector<size_t> listUsedTapes() const override;
		void build(SingleTapeMachineFactory &machineFactory, std::function<size_t (size_t)> getRealTape, std::function<const std::wstring &(size_t)> getState) const override;
		void build(MultiTapeMachineFactory &machineFactory, std::function<size_t (size_t)> getRealTape, std::function<const std::wstring &(size_t)> getState) const override;
		void print(std::wostream &stream, std::function<size_t (size_t)> getRealTape) const override;
};
