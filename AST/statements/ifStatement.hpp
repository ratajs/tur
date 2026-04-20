#pragma once
#include "../../common.hpp"
#include "./statement.hpp"
#include "../expressions/expression.hpp"

/*!
 * This statement represents branching.
 * It contains a condition as an expression and two branches.
 * Both of the branches are vectors of statements.
 */
class IfStatement: public Statement {
	private:
		std::unique_ptr<Expression> condition;
		std::vector<std::unique_ptr<Statement>> trueBranch, falseBranch;

	public:
		IfStatement(std::unique_ptr<Expression> condition, std::vector<std::unique_ptr<Statement>> trueBranch, std::vector<std::unique_ptr<Statement>> falseBranch);
		void build(InstructionBuilder &builder) const override;
};
