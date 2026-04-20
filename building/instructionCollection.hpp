#pragma once
#include "../common.hpp"
#include "./tapeReference.hpp"
#include "../instructions/instruction.hpp"
#include "../machine/machineFactory.hpp"
#include "../machine/singleTapeMachineFactory.hpp"
#include "../machine/multiTapeMachineFactory.hpp"
#include "../machine/machine.hpp"

/*!
 * This class stores the list of instructions together with tapes (with data about their first and last reference) and labels (just the number of them).
 */
class InstructionCollection {
	private:
		bool isOptimized = false;
		size_t labelsCount = 0, renamedTapesCount;
		std::vector<size_t> renamedTapes;
		std::vector<TapeReference> tapes;
		std::list<std::unique_ptr<Instruction>> instructions;

	public:
		InstructionCollection();
		InstructionCollection(std::list<std::unique_ptr<Instruction>> instructions, std::vector<TapeReference> tapes, size_t labelsCount);
		void optimize();
		const std::list<std::unique_ptr<Instruction>> &getInstructions() const;
		size_t getLabelsCount() const;
		size_t getTapesCount() const;
		size_t getTape(size_t tape) const;
		void printInstructions(std::wostream &stream) const;
};
