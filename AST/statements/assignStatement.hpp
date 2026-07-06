#pragma once
#include <cstdlib>
#include <memory>
#include <optional>
#include "./statement.hpp"
#include "../variable.hpp"
#include "../expressions/expression.hpp"
#include "../../building/instructionBuilder.hpp"

/*!
 * This statement represents an assignment (copying a tape or writing a constant) to a variable.
 * The assignment can be reversed, which means that it will not replace the end, but the beginning of the destination variable.
 * It contains the destination (variable and index) as well as the source (an expression).
 */
class AssignStatement: public Statement {
	private:
		bool isReversed, isDestinationIndexFromEnd;
		std::optional<size_t> destinationIndex; // Append if {}
		Variable &destination;
		std::unique_ptr<Expression> source;

	public:
		AssignStatement(std::unique_ptr<Expression> source, Variable &destination, std::optional<size_t> destinationIndex, bool isReversed, bool isDestinationIndexFromEnd);
		void build(InstructionBuilder &builder) const override;
};
