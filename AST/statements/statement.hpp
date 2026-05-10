#pragma once
#include "../../building/instructionBuilder.hpp"

/*!
 * This is the abstract class from which all statements inherit.
 */
class Statement {
	public:
		virtual ~Statement() noexcept = default;

		/*!
		 * This virtual function should generate the instructions of the statement.
		 * \param builder The instance of InstructionBuilder used for generating the instructions.
		 */
		virtual void build(InstructionBuilder &builder) const = 0;
};
