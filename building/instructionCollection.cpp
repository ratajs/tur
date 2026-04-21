#include "./instructionCollection.hpp"
#include "../IO/unexpectedError.hpp"

/*!
 * Creates an empty InstructionCollection, with no instruction, no tapes and no labels.
 */
InstructionCollection::InstructionCollection() {};

/*!
 * Create an InstructionCollection with provided instructions, tapes and labels.
 * The caller is responsible for the correct number of tapes and labels.
 * The first and last references of tapes have to be correct (no tape can be used before the first execution of the first reference instruction and after the first execution of the last reference instruction).
 * \param instructions The list of instructions.
 * \param tapes The list of tape references, long enough for all tape indices mentioned in the instructions.
 * \param labelsCount The number of labels, it has to be greater than all label indices mentioned in the instructions.
 */
InstructionCollection::InstructionCollection(std::list<std::unique_ptr<Instruction>> instructions, std::vector<TapeReference> tapes, size_t labelsCount):
	labelsCount(labelsCount), tapes(std::move(tapes)), instructions(std::move(instructions)) {};

/*!
 * Optimize the instructions by reassigning tapes.
 * The functionality of the resulting program is dependent on the correctness of first and last references of the tapes.
 * Can only be called once.
 * \throw UnexpectedError If the instructions were already optimized with this function.
 * \throw UnexpectedError If there is a tape in the original tapes vector which is never used by any instruction.
 */
void InstructionCollection::optimize() {
	size_t tapesCount = 0, instructionIndex;
	std::vector<size_t> newFreeTapes, result;
	std::vector<std::optional<size_t>> tapes;
	std::optional<std::list<std::unique_ptr<Instruction>>> replacementInstructions;
	std::list<std::unique_ptr<Instruction>>::const_iterator it;
	std::queue<size_t> freeTapes;

	if(this->isOptimized)
		throw UnexpectedError(L"Instructions are already optimized.");

	tapes.resize(this->tapes.size());

	for(it = this->instructions.begin(), instructionIndex = 0; it!=this->instructions.end(); instructionIndex++) {
		newFreeTapes.clear();

		if(
			(*it)->listUsedTapes().size()==2 &&
			(*this->tapes[(*it)->listUsedTapes()[0]].lastReference)==instructionIndex &&
			tapes[(*it)->listUsedTapes()[0]] &&
			!tapes[(*it)->listUsedTapes()[1]] &&
			(replacementInstructions = (*it)->tryToUnify())
		) {
			tapes[(*it)->listUsedTapes()[1]] = (*tapes[(*it)->listUsedTapes()[0]]);
			this->instructions.splice(it, std::move(*replacementInstructions));
			it = this->instructions.erase(it);

			continue;
		}
		else {
			std::ranges::for_each((*it)->listUsedTapes(),
				[this, &tapes, &tapesCount, &freeTapes, &newFreeTapes, instructionIndex](size_t tape) -> void {
					if(!tapes[tape]) {
						if(!freeTapes.empty()) {
							tapes[tape] = freeTapes.front();
							freeTapes.pop();
						}
						else
							tapes[tape] = (++tapesCount);
					};
					if(this->tapes[tape].lastReference==instructionIndex)
						newFreeTapes.push_back(*tapes[tape]);
				}
			);
		};

		std::ranges::for_each(newFreeTapes,
			[&freeTapes](size_t tape) -> void {
				freeTapes.push(tape);
			}
		);

		it++;
	};

	std::ranges::for_each(tapes,
		[](const std::optional<size_t> &tape) -> void {
			if(!tape)
				throw UnexpectedError(L"Unused tape detected.");
		}
	);

	std::ranges::transform(tapes, std::back_inserter(result), [](std::optional<size_t> tape) -> size_t { return (*tape); });

	this->renamedTapesCount = tapesCount;
	this->renamedTapes = std::move(result);
	this->isOptimized = true;
};

/*!
 * Print all the instructions (optimized if optimize has been called).
 * The output will start with TAPES n, where n is the number of tapes.
 * Then, on separate lines, every instruction will print itself (it is expected that the instructions print the end of line themselves).
 * \param stream The output stream.
 */
void InstructionCollection::printInstructions(std::wostream &stream) const {
	stream << L"TAPES " << this->getTapesCount() << std::endl;
	std::ranges::for_each(this->instructions,
		[this, &stream](const std::unique_ptr<Instruction> &instruction) -> void {
			instruction->print(stream, [this](size_t tape) -> size_t { return this->getTape(tape); });
		}
	);
};

/*!
 * \return A constant reference to the list of instructions.
 */
const std::list<std::unique_ptr<Instruction>> &InstructionCollection::getInstructions() const {
	return this->instructions;
};

/*!
 * \return The number of labels.
 */
size_t InstructionCollection::getLabelsCount() const {
	return this->labelsCount;
};

/*!
 * \return The number of tapes.
 */
size_t InstructionCollection::getTapesCount() const {
	return (this->isOptimized ? this->renamedTapesCount : this->tapes.size());
};

/*!
 * Convert the tape index stored in the instruction to the final tape index for the machine generation.
 * 0 is now reserved for the special zeroth tape, it starts with 1.
 * \param The tape index stored in the instruction.
 * \return The ‘real’ tape index.
 */
size_t InstructionCollection::getTape(size_t tape) const {
	if(tape >= this->tapes.size())
		throw UnexpectedError(L"The tape doesn’t exist.");

	return (this->isOptimized ? this->renamedTapes[tape] : (tape + 1));
};
