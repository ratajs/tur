#pragma once
#include "../../common.hpp"
#include "../../building/instructionBuilder.hpp"

/*!
 * This is the abstract class from which all statements inherit.
 */
class Statement {
	public:
		virtual ~Statement();

		/*!
		 * This virtual function should generate the instructions of the statement.
		 * \param builder The instance of InstructionBuilder used for generating the instructions.
		 */
		virtual void build(InstructionBuilder &builder) const = 0;
};
