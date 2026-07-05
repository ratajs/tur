#pragma once
#include <memory>
#include <vector>
#include "./statement.hpp"
#include "../expressions/expression.hpp"

/*!
 * This statement represents a loop with an optional initial statement, a condition, and at least one step statement.
 * It contains those special statements, a condition as an expression, and the body as a vector of statements.
 * The initial statement will only be executed at the beginning, the step statements will always be executed after the end of the body execution, or after continue, not after break.
 * The body will be executed while the condition holds (zero times if the condition is false in the beginning).
 */
class ForStatement: public Statement {
	private:
		std::unique_ptr<Expression> condition;
		std::unique_ptr<Statement> initStatement;
		std::vector<std::unique_ptr<Statement>> stepStatements, body;

	public:
		ForStatement(std::unique_ptr<Statement> initStatement, std::unique_ptr<Expression> condition, std::vector<std::unique_ptr<Statement>> stepStatements, std::vector<std::unique_ptr<Statement>> body);
		void build(InstructionBuilder &builder) const override;
};
