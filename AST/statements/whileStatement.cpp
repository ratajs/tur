#include "./whileStatement.hpp"
#include "../../instructions/jumpInstruction.hpp"
#include "../../IO/typeError.hpp"

/*!
 * The constructor of WhileStatement.
 * \param condition The condition.
 * \param body The statemets in the body.
 * \throw TypeError If the type of the condition expression is not CONDITION.
 */
WhileStatement::WhileStatement(std::unique_ptr<Expression> condition, std::vector<std::unique_ptr<Statement>> body): condition(std::move(condition)), body(std::move(body)) {
	if(!this->condition->isCondition())
		throw TypeError(TypeError::Type::NON_CONDITION_IN_A_WHILE_STATEMENT, this->condition->location);
};

void WhileStatement::build(InstructionBuilder &builder) const {
	size_t trueLabel, falseLabel, beginLabel, firstInstruction, lastInstruction;

	beginLabel = builder.createLabel();

	builder.addInstruction(std::make_unique<JumpInstruction>(beginLabel, JumpInstruction::Type::GO_TO));
	firstInstruction = builder.addInstruction(std::make_unique<JumpInstruction>(beginLabel, JumpInstruction::Type::COME_FROM));
	std::tie(trueLabel, falseLabel) = this->condition->buildCondition(builder);
	builder.addInstruction(std::make_unique<JumpInstruction>(trueLabel, JumpInstruction::Type::COME_FROM));
	builder.pushContinueDestination(beginLabel);
	builder.pushBreakDestination(falseLabel);
	std::ranges::for_each(this->body, [&builder](const std::unique_ptr<Statement> &statement) -> void { statement->build(builder); });
	builder.popContinueDestination();
	builder.popBreakDestination();
	lastInstruction = builder.addInstruction(std::make_unique<JumpInstruction>(beginLabel, JumpInstruction::Type::GO_TO));
	builder.addInstruction(std::make_unique<JumpInstruction>(falseLabel, JumpInstruction::Type::COME_FROM));
	builder.postponeLastReference(firstInstruction, lastInstruction);
};
