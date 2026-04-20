#pragma once
#include "../../common.hpp"
#include "./expression.hpp"
#include "../../parsing/location.hpp"

/*!
 * This expression represents a constant.
 */
class NumberExpression: public Expression {
	private:
		size_t number;

	public:
		NumberExpression(const Location &location, size_t number);
		Expression::Type getType() const override;
		Expression::Result build(InstructionBuilder &builder) const override;
		std::optional<Expression::TapeRange> getArrayAccesRange() const override;
};
