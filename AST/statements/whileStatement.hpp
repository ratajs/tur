#pragma once
#include "../../common.hpp"
#include "./statement.hpp"
#include "../expressions/expression.hpp"

/*!
 * This statement represents a loop with a condition.
 * It contains a condition as an expression and the body as a vector of statements.
 * The body will be executed while the condition holds (zero times if the condition is false in the beginning).
 */
class WhileStatement: public Statement {
	private:
		std::unique_ptr<Expression> condition;
		std::vector<std::unique_ptr<Statement>> body;

	public:
		WhileStatement(std::unique_ptr<Expression> condition, std::vector<std::unique_ptr<Statement>> body);
		void build(InstructionBuilder &builder) const override;
};
