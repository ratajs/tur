#pragma once
#include "../../common.hpp"
#include "./statement.hpp"
#include "../variable.hpp"
#include "../sourceBundle.hpp"
#include "../expressions/expression.hpp"
#include "../../parsing/location.hpp"

/*!
 * This statement represents joining/imploding several expressions together to a variable.
 * It contains a SourceBundle object with the source expressions and a destination variable with its index.
 */
class ImplodeStatement: public Statement {
	private:
		std::optional<size_t> destinationIndex; // Append if {}
		std::optional<Expression::TapeRange> backupRange;
		Variable &destination;
		SourceBundle source;

	public:
		ImplodeStatement(SourceBundle source, Variable &destination, std::optional<size_t> destinationIndex, Location bundleLocation);
		void build(InstructionBuilder &builder) const override;
};
