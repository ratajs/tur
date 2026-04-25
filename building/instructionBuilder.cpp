#include "./instructionBuilder.hpp"
#include "../IO/unexpectedError.hpp"

/*!
 * Register a new numbered tape.
 * \return The number of the tape, from 0 (not necessarily the tape index in the final machine).
 */
size_t InstructionBuilder::createTape() {
	this->tapes.emplace_back();

	return (tapes.size() - 1);
};

/*!
 * Register a new numbered label.
 * \return The number of the label, from 0.
 */
size_t InstructionBuilder::createLabel() {
	return (this->labelsCount++);
};

/*!
 * \return If the destination for the exit statement is set, then true, false otherwise.
 */
bool InstructionBuilder::isExitDestinationSet() const {
	return this->exitDestination.has_value();
};

/*!
 * \return If the stack for break destinations of loops is empty, then true, false otherwise.
 */
bool InstructionBuilder::isBreakStackEmpty() const {
	return this->breakStack.empty();
};

/*!
 * \return If the stack for continue destinations of loops is empty, then true, false otherwise.
 */
bool InstructionBuilder::isContinueStackEmpty() const {
	return this->continueStack.empty();
};

/*!
 * \return The label registered as the exit destination.
 * \throw UnexpectedError If the exit destination has not been registered.
 */
size_t InstructionBuilder::getExitDestination() const {
	if(!this->isExitDestinationSet())
		throw UnexpectedError(L"The exit destination is not set.");

	return (*this->exitDestination);
};

/*!
 * \return The label on top of the break stack.
 * \throw UnexpectedError If the break stack is empty.
 */
size_t InstructionBuilder::getBreakDestination() const {
	if(this->breakStack.empty())
		throw UnexpectedError(L"The break stack is empty.");

	return this->breakStack.top();
};

/*!
 * \return The label on top of the continue stack.
 * \throw UnexpectedError If the continue stack is empty.
 */
size_t InstructionBuilder::getContinueDestination() const {
	if(this->continueStack.empty())
		throw UnexpectedError(L"The continue stack is empty.");

	return this->continueStack.top();
};

/*!
 * Register a new exit destination.
 * \param label The label of the exit destination.
 * \throw UnexpectedError If the label doesn’t exit.
 */
void InstructionBuilder::setExitDestination(size_t label) {
	if(label >= this->labelsCount)
		throw UnexpectedError(L"The label doesn’t exist.");

	this->exitDestination = label;
};

/*!
 * Register a new break destination, pushes it on the break stack.
 * \param label The label of the break destination.
 * \throw UnexpectedError If the label doesn’t exit.
 */
void InstructionBuilder::pushBreakDestination(size_t label) {
	if(label >= this->labelsCount)
		throw UnexpectedError(L"The label doesn’t exist.");

	this->breakStack.push(label);
};

/*!
 * Register a new continue destination, pushes it on the break stack.
 * \param label The label of the continue destination.
 * \throw UnexpectedError If the label doesn’t exit.
 */
void InstructionBuilder::pushContinueDestination(size_t label) {
	if(label >= this->labelsCount)
		throw UnexpectedError(L"The label doesn’t exist.");

	this->continueStack.push(label);
};

/*!
 * Unset the exit destination, if there is one, no effect otherwise.
 */
void InstructionBuilder::unsetExitDestination() {
	this->exitDestination.reset();
};

/*!
 * Pop the label on top of the break stack, if there is one, no effect if the stack is empty.
 */
void InstructionBuilder::popBreakDestination() {
	this->breakStack.pop();
};

/*!
 * Pop the label on top of the continue stack, if there is one, no effect if the stack is empty.
 */
void InstructionBuilder::popContinueDestination() {
	this->continueStack.pop();
};

/*!
 * Allow instructions to merge with the previous instruction directly after adding.
 */
void InstructionBuilder::allowInstructionMerging() {
	this->isInstructionMergingEnabled = true;
};

/*!
 * Add a new instructions to the sequence.
 * If the last instruction can be merged with the newly added one, they are merged.
 * Inspects the new (or updated/merged) instruction for tape usage.
 * \return The index of the new (or updated/merged) instruction.
 */
size_t InstructionBuilder::addInstruction(std::unique_ptr<Instruction> instruction) {
	std::unique_ptr<Instruction> replacementInstruction;

	if(this->isInstructionMergingEnabled && !this->instructions.empty() && (replacementInstruction = this->instructions.back()->tryToMerge(*instruction))) {
		this->instructions.pop_back();
		this->instructions.push_back(std::move(replacementInstruction));
	}
	else {
		this->instructions.push_back(std::move(instruction));
		this->tapesByLastReference.emplace_back();
	};

	std::ranges::for_each(this->instructions.back()->listUsedTapes(),
		[this](size_t tape) -> void {
			size_t instructionIndex;

			if(tape >= this->tapes.size())
			 throw UnexpectedError(L"Tape used by an instruction doesn’t exist.");

			instructionIndex = (this->instructions.size() - 1);

			if(!this->tapes[tape].firstReference)
				this->tapes[tape].firstReference = instructionIndex;

			if(!this->tapes[tape].lastReference) {
				this->tapes[tape].lastReference = instructionIndex;
				this->tapesByLastReference[instructionIndex].insert(tape);
			}
			else if((*this->tapes[tape].lastReference) < instructionIndex) {
				this->tapesByLastReference[*this->tapes[tape].lastReference].erase(instructionIndex);
				this->tapes[tape].lastReference = instructionIndex;
				this->tapesByLastReference[instructionIndex].insert(tape);
			};
		}
	);

	return (this->instructions.size() - 1);
};

/*!
 * Rewrite the last references of tapes used by instructions in a particular range.
 * The new last reference will be the end of the range.
 * The last reference is the index of the last instruction which is expected to change it.
 * Used if jumping up takes place, which causes that a tape can be accessed after the last instruction which uses it is executed.
 * \param firstReference The beginning of the range (inclusive).
 * \param lastReference The end of the range and the new last reference.
 */
void InstructionBuilder::postponeLastReference(size_t firstInstruction, size_t lastInstruction) {
	if(lastInstruction <= firstInstruction || lastInstruction >= this->instructions.size())
		throw UnexpectedError(L"Invalid instruction indices.");

	std::ranges::for_each(this->tapesByLastReference.begin() + firstInstruction, this->tapesByLastReference.begin() + lastInstruction,
		[this, lastInstruction](const std::set<size_t> &tapes) -> void {
			std::ranges::for_each(tapes,
				[this, lastInstruction](size_t tape) -> void {
					this->tapes[tape].lastReference = lastInstruction;
				}
			);
		}
	);
};

/*!
 * Extract all the collected instructions.
 * The instance itself will lose all data.
 * \return The collection of all instructions from this instance.
 */
InstructionCollection InstructionBuilder::extractInstructions() {
	size_t labelsCount;
	std::vector<TapeReference> tapes;
	std::list<std::unique_ptr<Instruction>> instructions;

	instructions = std::move(this->instructions);
	tapes = std::move(this->tapes);
	labelsCount = this->labelsCount;

	(*this) = {};

	return { std::move(instructions), std::move(tapes), labelsCount };
};
