#include "./forStatement.hpp"
#include <cstdlib>
#include <utility>
#include <tuple>
#include <algorithm>
#include "../../instructions/jumpInstruction.hpp"
#include "../../IO/typeError.hpp"

ForStatement::ForStatement(std::unique_ptr<Statement> initStatement, std::unique_ptr<Expression> condition, std::unique_ptr<Statement> stepStatement, std::vector<std::unique_ptr<Statement>> body): condition(std::move(condition)), initStatement(std::move(initStatement)), stepStatement(std::move(stepStatement)), body(std::move(body)) {
	if(!this->condition->isCondition())
		throw TypeError(TypeError::Type::NON_CONDITION_IN_A_WHILE_STATEMENT, this->condition->location);
};

void ForStatement::build(InstructionBuilder &builder) const {
	size_t trueLabel, falseLabel, beginLabel, stepLabel, firstInstruction, lastInstruction;

	beginLabel = builder.createLabel();
	stepLabel = builder.createLabel();

	this->initStatement->build(builder);
	builder.addInstruction(std::make_unique<JumpInstruction>(beginLabel, JumpInstruction::Type::GO_TO));
	firstInstruction = builder.addInstruction(std::make_unique<JumpInstruction>(beginLabel, JumpInstruction::Type::COME_FROM));
	std::tie(trueLabel, falseLabel) = this->condition->buildCondition(builder);
	builder.addInstruction(std::make_unique<JumpInstruction>(trueLabel, JumpInstruction::Type::COME_FROM));
	builder.pushContinueDestination(stepLabel);
	builder.pushBreakDestination(falseLabel);
	std::ranges::for_each(this->body, [&builder](const std::unique_ptr<Statement> &statement) -> void { statement->build(builder); });
	builder.popContinueDestination();
	builder.popBreakDestination();
	builder.addInstruction(std::make_unique<JumpInstruction>(stepLabel, JumpInstruction::Type::GO_TO));
	builder.addInstruction(std::make_unique<JumpInstruction>(stepLabel, JumpInstruction::Type::COME_FROM));
	this->stepStatement->build(builder);
	lastInstruction = builder.addInstruction(std::make_unique<JumpInstruction>(beginLabel, JumpInstruction::Type::GO_TO));
	builder.addInstruction(std::make_unique<JumpInstruction>(falseLabel, JumpInstruction::Type::COME_FROM));
	builder.postponeLastReference(firstInstruction, lastInstruction);
};
