#pragma once
#include "../../common.hpp"
#include "./statement.hpp"
#include "../variable.hpp"
#include "../../machine/machine.hpp"

/*!
 * This statement represents running a custom Turing machine on the whole variable tape.
 * It contains the variable and the machine.
 */
class CallStatement: public Statement {
	private:
		Variable &variable;
		const Machine &machine;

	public:
		CallStatement(const Machine &machine, Variable &variable);
		void build(InstructionBuilder &builder) const override;
};
