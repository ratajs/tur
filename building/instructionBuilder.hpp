#pragma once
#include "../common.hpp"
#include "./tapeReference.hpp"
#include "../instructions/instruction.hpp"
#include "./instructionCollection.hpp"

/*!
 * This class holds a sequence of instructions together with extra data, like number of tapes and labels.
 * By passing a reference to an instance of InstructionBuilder, statements and expressions can add generate code.
 * It remembers important labels for exit, break and continue.
 * For tapes, it keeps track of the first and last place the tape is used.
 * In the beginning, the sequence of instructions is empty, no tape is registered (the zeroth tape is not taken into account here) and no labels exist.
 */
class InstructionBuilder {
	private:
		bool isInstructionMergingEnabled = false;
		size_t labelsCount = 0;
		std::optional<size_t> exitDestination;
		std::vector<TapeReference> tapes;
		std::stack<size_t> breakStack, continueStack;
		std::list<std::unique_ptr<Instruction>> instructions;
		std::vector<std::set<size_t>> tapesByLastReference;

	public:
		size_t createTape();
		size_t createLabel();
		bool isExitDestinationSet() const;
		bool isBreakStackEmpty() const;
		bool isContinueStackEmpty() const;
		size_t getExitDestination() const;
		size_t getBreakDestination() const;
		size_t getContinueDestination() const;
		void pushBreakDestination(size_t label);
		void pushContinueDestination(size_t label);
		void setExitDestination(size_t label);
		void popBreakDestination();
		void popContinueDestination();
		void unsetExitDestination();
		void allowInstructionMerging();
		size_t addInstruction(std::unique_ptr<Instruction> instruction);
		void postponeLastReference(size_t firstInstruction, size_t lastInstruction);
		InstructionCollection extractInstructions();
};
