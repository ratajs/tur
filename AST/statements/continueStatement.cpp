#include "./continueStatement.hpp"
#include "../../instructions/jumpInstruction.hpp"
#include "../../IO/unexpectedError.hpp"

void ContinueStatement::build(InstructionBuilder &builder) const {
	if(builder.isContinueStackEmpty())
		throw UnexpectedError(L"The continue stack is empty.");

	builder.addInstruction(std::make_unique<JumpInstruction>(builder.getContinueDestination(), JumpInstruction::Type::GO_TO));
	builder.addInstruction(std::make_unique<JumpInstruction>(builder.createLabel(), JumpInstruction::Type::COME_FROM)); // Unreachable label
};
