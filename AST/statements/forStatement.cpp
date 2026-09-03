#include "./forStatement.hpp"
#include <cstdlib>
#include <utility>
#include <optional>
#include <tuple>
#include <algorithm>
#include "../../instructions/jumpInstruction.hpp"
#include "../../IO/typeError.hpp"

/*!
 * The constructor of ForStatement.
 * \param initStatement The initial statement, possible nullptr if there is none.
 * \param condition The condition.
 * \param stepStatements The step statements.
 * \param body The statemets in the body.
 * \throw TypeError If the type of the condition expression is not CONDITION.
 */
ForStatement::ForStatement(std::unique_ptr<Statement> initStatement, std::unique_ptr<Expression> condition, std::vector<std::unique_ptr<Statement>> stepStatements, std::vector<std::unique_ptr<Statement>> body): condition(std::move(condition)), initStatement(std::move(initStatement)), stepStatements(std::move(stepStatements)), body(std::move(body)) {
	if(!this->condition->isCondition())
		throw TypeError(TypeError::Type::NON_CONDITION_IN_A_WHILE_STATEMENT, this->condition->location);
};

void ForStatement::build(InstructionBuilder &builder) const {
	size_t trueLabel, falseLabel, beginLabel, stepLabel, lastInstruction;

	beginLabel = builder.createLabel();
	stepLabel = builder.createLabel();

	if(this->initStatement)
		this->initStatement->build(builder);
	builder.getVariableAnalyzer().startLoop();
	builder.addInstruction(std::make_unique<JumpInstruction>(beginLabel, JumpInstruction::Type::GO_TO));
	builder.addInstruction(std::make_unique<JumpInstruction>(beginLabel, JumpInstruction::Type::COME_FROM));
	std::tie(trueLabel, falseLabel) = this->condition->buildCondition(builder);
	builder.addInstruction(std::make_unique<JumpInstruction>(trueLabel, JumpInstruction::Type::COME_FROM));
	builder.pushContinueDestination(stepLabel);
	builder.pushBreakDestination(falseLabel);
	std::ranges::for_each(this->body, [&builder](const std::unique_ptr<Statement> &statement) -> void { statement->build(builder); });
	builder.popContinueDestination();
	builder.popBreakDestination();
	builder.addInstruction(std::make_unique<JumpInstruction>(stepLabel, JumpInstruction::Type::GO_TO));
	builder.addInstruction(std::make_unique<JumpInstruction>(stepLabel, JumpInstruction::Type::COME_FROM));
	std::ranges::for_each(this->stepStatements, [&builder](const std::unique_ptr<Statement> &statement) -> void { statement->build(builder); });
	lastInstruction = builder.addInstruction(std::make_unique<JumpInstruction>(beginLabel, JumpInstruction::Type::GO_TO));
	builder.addInstruction(std::make_unique<JumpInstruction>(falseLabel, JumpInstruction::Type::COME_FROM));
	std::ranges::for_each(builder.getVariableAnalyzer().getUnitialized(),
		[&builder, lastInstruction](const Variable *variable) -> void {
			builder.postponeLastReference(*variable->tape, lastInstruction);
		}
	);
	builder.getVariableAnalyzer().endLoop();
};
