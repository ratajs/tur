#pragma once
#include <iostream>
#include "./instructionCollection.hpp"
#include "../machine/machineFactory.hpp"
#include "../machine/machine.hpp"

/*!
 * Objects of this class are created with an InstructionCollection.
 * The responsibility of MachineBuilder is to create a machine using a SingleTapeMachineFactory or MultiTapeMachineFactory from the instructions.
 */
class MachineBuilder {
	private:
		InstructionCollection instructions;

		Machine buildMachine(MachineFactory_MachineFactory auto machineFactory);

	public:
		MachineBuilder(InstructionCollection instructions);
		void printInstructions(std::wostream &stream) const;
		Machine buildMachine();
};
