#pragma once
#include <cstdlib>
#include <memory>
#include <optional>
#include <vector>
#include <list>
#include <string>
#include <iostream>
#include <functional>
#include "./instruction.hpp"
#include "../machine/multiTapeMachineFactory.hpp"
#include "../parsing/irArguments.hpp"

/*!
 * This instruction copies a range of numbers from one tape to another tape.
 */
class CopyInstruction: public Instruction {
	private:
		bool areSourceIndicesFromEnd, isDestinationIndexFromEnd;
		size_t source, destination, sourceIndex0;
		std::optional<size_t> sourceIndex1, destinationIndex;

	public:
		CopyInstruction(size_t source, size_t destination, size_t sourceIndex0, std::optional<size_t> sourceIndex1, std::optional<size_t> destinationIndex, bool areSourceIndicesFromEnd = false, bool isDestinationIndexFromEnd = false);
		CopyInstruction(IrArguments &arguments);
		std::vector<size_t> listUsedTapes() const override;
		std::optional<std::list<std::unique_ptr<Instruction>>> tryToUnify() const override;
		std::unique_ptr<Instruction> tryToMerge(const Instruction &otherInstruction) const override;
		std::unique_ptr<Instruction> tryToMergeWithCopy(size_t source, size_t destination, size_t sourceIndex0, std::optional<size_t> sourceIndex1, std::optional<size_t> destinationIndex) const override;
		void build(MultiTapeMachineFactory &machineFactory, std::function<size_t (size_t)> getRealTape, std::function<const std::wstring &(size_t)> getState) const override;
		void print(std::wostream &stream, std::function<size_t (size_t)> getRealTape) const override;
};
