#pragma once
#include "../common.hpp"
#include "./instruction.hpp"
#include "../machine/singleTapeMachineFactory.hpp"
#include "../machine/multiTapeMachineFactory.hpp"

/*!
 * This instruction compares two numbers and jumps to different labels depending on the result of the comparison.
 * Those numbers can either be constants or indices on tapes (a combination is possible).
 * The next instruction should be a JumpInstruction of the COME_FROM type.
 */
class CompareInstruction: public Instruction {
	public:
		enum class Type {
			EQ, NE, LT, LTE, GT, GTE
		};

	private:
		CompareInstruction::Type type;
		size_t trueLabel, falseLabel;
		std::variant<std::pair<size_t, size_t>, size_t> argumentA, argumentB;

		CompareInstruction::Type getInvertedType() const;
		bool isConstantConditionSatisfied() const;
		void buildSemiConstantComparison(SingleTapeMachineFactory &machineFactory, size_t index, size_t number, const std::wstring &trueState, const std::wstring &falseState, bool isInverted = false) const;
		void buildSemiConstantComparison(MultiTapeMachineFactory &machineFactory, size_t tape, size_t index, size_t number, const std::wstring &trueState, const std::wstring &falseState, bool isInverted = false) const;
		void buildTapeComparison(SingleTapeMachineFactory &machineFactory, size_t indexA, size_t indexB, const std::wstring &trueState, const std::wstring &falseState) const;
		void buildTapeComparison(MultiTapeMachineFactory &machineFactory, size_t tapeA, size_t indexA, size_t tapeB, size_t indexB, const std::wstring &trueState, const std::wstring &falseState) const;

	public:
		CompareInstruction(std::variant<std::pair<size_t, size_t>, size_t> argumentA, std::variant<std::pair<size_t, size_t>, size_t> argumentB, size_t trueLabel, size_t falseLabel, CompareInstruction::Type type);
		std::vector<size_t> listUsedTapes() const override;
		void build(SingleTapeMachineFactory &machineFactory, std::function<size_t (size_t)> getRealTape, std::function<const std::wstring &(size_t)> getState) const override;
		void build(MultiTapeMachineFactory &machineFactory, std::function<size_t (size_t)> getRealTape, std::function<const std::wstring &(size_t)> getState) const override;
		void print(std::wostream &stream, std::function<size_t (size_t)> getRealTape) const override;
};
