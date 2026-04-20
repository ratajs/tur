#pragma once
#include "../../common.hpp"
#include "./statement.hpp"

/*!
 * This statement represents a jump to a destination at the end of the program.
 * The destination of the jump is determined by the InstructionBuilder.
 */
class ExitStatement: public Statement {
	public:
		void build(InstructionBuilder &builder) const override;
};
