#include "./breakStatement.hpp"
#include "../../instructions/jumpInstruction.hpp"
#include "../../IO/unexpectedError.hpp"

void BreakStatement::build(InstructionBuilder &builder) const {
	if(builder.isBreakStackEmpty())
		throw UnexpectedError(L"The break stack is empty.");

	builder.addInstruction(std::make_unique<JumpInstruction>(builder.getBreakDestination(), JumpInstruction::Type::GO_TO));
	builder.addInstruction(std::make_unique<JumpInstruction>(builder.createLabel(), JumpInstruction::Type::COME_FROM)); // Unreachable label
};
