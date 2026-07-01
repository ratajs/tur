#pragma once
#include <memory>
#include <vector>
#include "./statement.hpp"
#include "../expressions/expression.hpp"

class ForStatement: public Statement {
	private:
		std::unique_ptr<Expression> condition;
		std::unique_ptr<Statement> initStatement, stepStatement;
		std::vector<std::unique_ptr<Statement>> body;

	public:
		ForStatement(std::unique_ptr<Statement> initStatement, std::unique_ptr<Expression> condition, std::unique_ptr<Statement> stepStatement, std::vector<std::unique_ptr<Statement>> body);
		void build(InstructionBuilder &builder) const override;
};
