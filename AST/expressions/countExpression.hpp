#pragma once
#include "../../common.hpp"
#include "./expression.hpp"
#include "../../parsing/location.hpp"

/*!
 * This expression represents the length of a variable (number of numbers of the variable).
 */
class CountExpression: public Expression {
	private:
		Variable &variable;

	public:
		CountExpression(const Location &location, Variable &variable);
		Expression::Type getType() const override;
		Expression::Result build(InstructionBuilder &builder) const override;
		std::optional<Expression::TapeRange> getArrayAccesRange() const override;
		std::optional<std::reference_wrapper<const Variable>> getMeasuredVariable() const override;
		bool isTapeTemporary() const override;
};
