#include "./machineBuilder.hpp"
#include "../machine/machineFactory.hpp"
#include "../IO/unexpectedError.hpp"

/*!
 * Build the final machine.
 * \param machineFactory The SingleTapeMachineFactory or MultiTapeMachineFactory object used for the building.
 * \return The final machine.
 */
Machine MachineBuilder::buildMachine(MachineFactory_MachineFactory auto machineFactory) {
	std::vector<std::wstring> labels;
	Machine machine;

	labels = machineFactory.createExtraStates(this->instructions.getLabelsCount());

	std::ranges::for_each(this->instructions.getInstructions(),
		[this, &machineFactory, &labels](const std::unique_ptr<Instruction> &instruction) -> void {
			instruction->build(machineFactory,
				[this](size_t tape) -> size_t {
					return this->instructions.getTape(tape);
				},
				[&labels](size_t label) -> const std::wstring& {
					if(label >= labels.size())
						throw UnexpectedError(L"The label doesn’t exist.");

					return labels[label];
				}
			);
		}
	);

	machine = machineFactory.extractMachine();
	machine.removeNTransitions();
	machine.removeInaccessibleStates();

	// MachineFactory::compose called to order state names alphabetically.
	return MachineFactory::compose({ machine });
};

/*!
 * The constructor of MachineBuilder.
 * \param instructions The InstructionCollection object containing the instructions (and labels etc.).
 */
MachineBuilder::MachineBuilder(InstructionCollection instructions): instructions(std::move(instructions)) {};

/*!
 * Print the string representation of the instructions to an output stream.
 * \param stream The output stream.
 */
void MachineBuilder::printInstructions(std::wostream &stream) const {
	this->instructions.printInstructions(stream);
};

/*!
 * Build the final machine.
 * It chooses whether to create a SingleTapeMachineFactory or a MultiTapeMachineFactory, depending on the number of tapes.
 * \return The final machine.
 */
Machine MachineBuilder::buildMachine() {
	if(this->instructions.getTapesCount()==1)
		return this->buildMachine(SingleTapeMachineFactory());
	else
		return this->buildMachine(MultiTapeMachineFactory(this->instructions.getTapesCount()));
};
