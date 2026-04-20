#pragma once
#include "../common.hpp"
#include "../machine/singleTapeMachineFactory.hpp"
#include "../machine/multiTapeMachineFactory.hpp"

/*!
 * This is the abstract class from which all instructions inherit.
 */
class Instruction {
	public:
		virtual ~Instruction();

		/*!
		 * This virtual method should return all tapes used by the instruction.
		 * \return Vector with numbers of all used tapes (numbers according to the InstructionBuilder).
		 */
		virtual std::vector<size_t> listUsedTapes() const = 0;

		/*!
		 * This virtual method can be implemented by instructions using exactly two tapes if the tapes can be merged.
		 * It can be presumed that the first used tape is never used again and that the second tape has not been used before.
		 * The default implementation always returns {} (std::nullopt), which means that no unification should happen.
		 * \return A list of instructions doing the same actions if the two tapes are later converted to a single tape. Or {} for no change.
		 */
		virtual std::optional<std::list<std::unique_ptr<Instruction>>> tryToUnify() const;

		/*!
		 * If this virtual method is implemented and returns an instruction, this and an other instruction can be merged into one.
		 * The default implementation always returns {} (nullptr), which means that no unification should happen.
		 * \param otherInstruction The second instruction which comes directly after this.
		 * \return The merged instruction. Or {} (nullptr) if no merge should happen.
		 */
		virtual std::unique_ptr<Instruction> tryToMerge(const Instruction &otherInstruction) const;

		/*!
		 * This virtual method is called when the previous instruction is a CopyInstruction.
		 * If this virtual method is implemented and returns an instruction, the CopyInstruction and this instruction can be merged into one.
		 * The default implementation always returns {} (nullptr), which means that no unification should happen.
		 * \param source The number of the source tape of the CopyInstruction.
		 * \param destination The number of the destination tape of the CopyInstruction.
		 * \param sourceIndex0 The index where the source of the CopyInstruction starts.
		 * \param sourceIndex1 The index where the source of the CopyInstruction ends ({} if the source is rightwise unbounded).
		 * \param destinationIndex The index where the destination of the CopyInstruction is ({} if the destination is the end of the destination tape).
		 * \return The merged instruction. Or {} (nullptr) if no merge should happen.
		 */
		virtual std::unique_ptr<Instruction> tryToMergeWithCopy(size_t source, size_t destination, size_t sourceIndex0, std::optional<size_t> sourceIndex1, std::optional<size_t> destinationIndex) const;

		/*!
		 * This virtual method should build the instruction using the SingleTapeMachineFactory.
		 * This variant should only be called if the instruction uses a single tape only.
		 * The default implementation is to be used if the instruction does not support the single-tape mode.
		 * \param machineFactory The SingleTapeMachineFactory object which the instruction should use for building.
		 * \param getRealTape A function for translation from the tape numbers with which the instructions were created to tape numbers which should be used with the machine factory. It should always return 1 in this case.
		 * \param getState A function for translation from the label numbers with which the instructions were created to state names known to the machine factory as extra states.
		 */
		virtual void build(SingleTapeMachineFactory &machineFactory, std::function<size_t (size_t)> getRealTape, std::function<const std::wstring &(size_t)> getState) const;

		/*!
		 * This virtual method should build the instruction using the MultiTapeMachineFactory.
		 * This variant should only be called if the instruction uses a single tape only.
		 * \param machineFactory The MultiTapeMachineFactory object which the instruction should use for building.
		 * \param getRealTape A function for translation from the tape numbers with which the instructions were created to tape numbers which should be used with the machine factory.
		 * \param getState A function for translation from the label numbers with which the instructions were created to state names known to the machine factory as extra states.
		 */
		virtual void build(MultiTapeMachineFactory &machineFactory, std::function<size_t (size_t)> getRealTape, std::function<const std::wstring &(size_t)> getState) const = 0;

		/*!
		 * This virtual method should print a string representation of the instruction to a given output stream.
		 * \param stream The output stream.
		 * \param getRealTape A function for translation from the tape numbers with which the instructions were created to tape numbers which should be used for printing.
		 * \return The type of the expression.
		 */
		virtual void print(std::wostream &stream, std::function<size_t (size_t)> getRealTape) const = 0;
};
