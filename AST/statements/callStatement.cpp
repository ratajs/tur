#include "./callStatement.hpp"
#include "../../instructions/callInstruction.hpp"

/*!
 * The constructor of CallStatement.
 * \param machine The Turing machine to run.
 * \param variable The variable to run it on.
 */
CallStatement::CallStatement(const Machine &machine, Variable &variable): variable(variable), machine(machine) {};

void CallStatement::build(InstructionBuilder &builder) const {
	builder.addInstruction(std::make_unique<CallInstruction>(*this->variable.tape, this->machine));
};
