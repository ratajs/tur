#include "./ifStatement.hpp"
#include "../../instructions/jumpInstruction.hpp"
#include "../../IO/typeError.hpp"

/*!
 * The constructor of IfStatement.
 * \param condition The condition.
 * \param trueBranch The statemets in the branch for the positive outcome of the condition.
 * \param trueBranch The statemets in the branch for the negative outcome of the condition.
 * \throw TypeError If the type of the condition expression is not CONDITION.
 */
IfStatement::IfStatement(std::unique_ptr<Expression> condition, std::vector<std::unique_ptr<Statement>> trueBranch, std::vector<std::unique_ptr<Statement>> falseBranch):
	condition(std::move(condition)), trueBranch(std::move(trueBranch)), falseBranch(std::move(falseBranch)) {
		if(!this->condition->isCondition())
			throw TypeError(TypeError::Type::NON_CONDITION_IN_AN_IF_STATEMENT, this->condition->location);
	};

void IfStatement::build(InstructionBuilder &builder) const {
	size_t trueLabel, falseLabel, endLabel;

	endLabel = builder.createLabel();

	std::tie(trueLabel, falseLabel) = this->condition->buildCondition(builder);
	builder.addInstruction(std::make_unique<JumpInstruction>(trueLabel, JumpInstruction::Type::COME_FROM));
	std::ranges::for_each(this->trueBranch, [&builder](const std::unique_ptr<Statement> &statement) -> void { statement->build(builder); });
	builder.addInstruction(std::make_unique<JumpInstruction>(endLabel, JumpInstruction::Type::GO_TO));
	builder.addInstruction(std::make_unique<JumpInstruction>(falseLabel, JumpInstruction::Type::COME_FROM));
	std::ranges::for_each(this->falseBranch, [&builder](const std::unique_ptr<Statement> &statement) -> void { statement->build(builder); });
	builder.addInstruction(std::make_unique<JumpInstruction>(endLabel, JumpInstruction::Type::GO_TO));
	builder.addInstruction(std::make_unique<JumpInstruction>(endLabel, JumpInstruction::Type::COME_FROM));
};
