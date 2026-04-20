#pragma once
#include "../../common.hpp"
#include "./statement.hpp"
#include "../expressions/expression.hpp"

/*!
 * This statement represents a loop without a condition.
 * It contains the body as a vector of statements.
 * The body will be executed indefinetely (until a break or exit statement is called).
 */
class LoopStatement: public Statement {
	private:
		std::vector<std::unique_ptr<Statement>> body;

	public:
		LoopStatement(std::vector<std::unique_ptr<Statement>> body);
		void build(InstructionBuilder &builder) const override;
};
