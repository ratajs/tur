#include "./exitStatement.hpp"
#include "../../instructions/jumpInstruction.hpp"
#include "../../IO/unexpectedError.hpp"

void ExitStatement::build(InstructionBuilder &builder) const {
	if(!builder.isExitDestinationSet())
		throw UnexpectedError(L"The exit destination is not set.");

	builder.addInstruction(std::make_unique<JumpInstruction>(builder.getExitDestination(), JumpInstruction::Type::GO_TO));
	builder.addInstruction(std::make_unique<JumpInstruction>(builder.createLabel(), JumpInstruction::Type::COME_FROM)); // Unreachable label
};
