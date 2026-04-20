#pragma once
#include "../../common.hpp"
#include "./expression.hpp"
#include "../../parsing/location.hpp"
#include "../variable.hpp"

/*!
 * This expression represents the tape of a variable with no explicit array access.
 */
class VariableExpression: public Expression {
	private:
		Variable &variable;

	public:
		VariableExpression(const Location &location, Variable &variable);
		Expression::Type getType() const override;
		Expression::Result build(InstructionBuilder &builder) const override;
		std::optional<std::reference_wrapper<const Variable>> getVariable() const override;
		std::optional<Expression::TapeRange> getArrayAccesRange() const override;
};
