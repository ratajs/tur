#pragma once
#include "../../common.hpp"
#include "./statement.hpp"

/*!
 * This statement represents a jump to a destination at the beginning of the innermost loop.
 * The destination of the jump is determined by the InstructionBuilder.
 */
class ContinueStatement: public Statement {
	public:
		void build(InstructionBuilder &builder) const override;
};
