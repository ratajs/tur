#pragma once
#include <cstdlib>
#include <optional>
#include <vector>
#include <string>
#include <iostream>
#include <functional>
#include "./instruction.hpp"
#include "../machine/singleTapeMachineFactory.hpp"
#include "../machine/multiTapeMachineFactory.hpp"
#include "../parsing/irArguments.hpp"

/*!
 * This instruction either makes an unconditional jump (the GO_TO type) to a label or represents a jump from a label (the COME_FROM type).
 * In the first case, the next instruction should be a JumpInstruction of the COME_FROM type.
 * In the second case, the previous instruction should be a JumpInstruction of the GO_TO type, a CompareInstruction or a CompareTapeLengthInstruction.
 */
class JumpInstruction: public Instruction {
	public:
		enum class Type {
			GO_TO, COME_FROM
		};

	private:
		JumpInstruction::Type type;
		size_t label;

	public:
		JumpInstruction(size_t label, JumpInstruction::Type type);
		JumpInstruction(IrArguments &arguments);
		std::vector<size_t> listUsedTapes() const override;
		std::optional<size_t> getComeFromOrigin() const override;
		bool isGoToInstruction() const override;
		void build(SingleTapeMachineFactory &machineFactory, std::function<size_t (size_t)> getRealTape, std::function<const std::wstring &(size_t)> getState) const override;
		void build(MultiTapeMachineFactory &machineFactory, std::function<size_t (size_t)> getRealTape, std::function<const std::wstring &(size_t)> getState) const override;
		void print(std::wostream &stream, std::function<size_t (size_t)> getRealTape) const override;
};
