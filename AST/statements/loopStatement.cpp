#include "./loopStatement.hpp"
#include "../../instructions/jumpInstruction.hpp"

/*!
 * The constructor of LoopStatement.
 * \param body The statemets in the body.
 */
LoopStatement::LoopStatement(std::vector<std::unique_ptr<Statement>> body): body(std::move(body)) {};

void LoopStatement::build(InstructionBuilder &builder) const {
	size_t beginLabel, endLabel, firstInstruction, lastInstruction;

	beginLabel = builder.createLabel();
	endLabel = builder.createLabel();

	builder.addInstruction(std::make_unique<JumpInstruction>(beginLabel, JumpInstruction::Type::GO_TO));
	firstInstruction = builder.addInstruction(std::make_unique<JumpInstruction>(beginLabel, JumpInstruction::Type::COME_FROM));
	builder.pushContinueDestination(beginLabel);
	builder.pushBreakDestination(endLabel);
	std::ranges::for_each(this->body, [&builder](const std::unique_ptr<Statement> &statement) -> void { statement->build(builder); });
	builder.popContinueDestination();
	builder.popBreakDestination();
	lastInstruction = builder.addInstruction(std::make_unique<JumpInstruction>(beginLabel, JumpInstruction::Type::GO_TO));
	builder.addInstruction(std::make_unique<JumpInstruction>(endLabel, JumpInstruction::Type::COME_FROM));
	builder.postponeLastReference(firstInstruction, lastInstruction);
};

