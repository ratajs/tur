#pragma once
#include <cstdlib>
#include <optional>
#include "./statement.hpp"
#include "../variable.hpp"
#include "../sourceBundle.hpp"
#include "../expressions/expression.hpp"
#include "../../parsing/location.hpp"

/*!
 * This statement represents joining/imploding several expressions together to a variable.
 * The implosion can be reversed, which means that it will not replace the end, but the beginning of the destination variable.
 * It contains a SourceBundle object with the source expressions and a destination variable with its index.
 */
class ImplodeStatement: public Statement {
	private:
		bool isReversed, isDestinationIndexFromEnd;
		std::optional<size_t> destinationIndex; // Append if {}
		std::optional<Expression::TapeRange> backupRange;
		Variable &destination;
		SourceBundle source;

	public:
		ImplodeStatement(SourceBundle source, Variable &destination, std::optional<size_t> destinationIndex, bool isReversed, bool isDestinationIndexFromEnd, Location bundleLocation);
		void build(InstructionBuilder &builder) const override;
};
