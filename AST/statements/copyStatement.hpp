#pragma once
#include "../../common.hpp"
#include "./statement.hpp"
#include "../variable.hpp"
#include "../expressions/expression.hpp"
#include "../../building/instructionBuilder.hpp"

/*!
 * This statement represents an assignment (copying a tape or writing a constant) to a variable.
 * It contains the destination (variable and index) as well as the source (an expression).
 */
class CopyStatement: public Statement {
	private:
		std::optional<size_t> destinationIndex; // Append if {}
		Variable &destination;
		std::unique_ptr<Expression> source;

	public:
		CopyStatement(std::unique_ptr<Expression> source, Variable &destination, std::optional<size_t> destinationIndex);
		void build(InstructionBuilder &builder) const override;
};
