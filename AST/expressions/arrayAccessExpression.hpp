#pragma once
#include "../../common.hpp"
#include "./expression.hpp"
#include "../../parsing/location.hpp"

/*!
 * This expression represents indexing (array access) of another expression.
 * It contains the inner expression and the range (the index where it begins and optionally the index where it ends).
 */
class ArrayAccessExpression: public Expression {
	private:
		size_t index0;
		std::optional<size_t> index1; // End if {}
		std::unique_ptr<Expression> expression;

		void validate() const;

	public:
		ArrayAccessExpression(const Location &location, std::unique_ptr<Expression> expression, size_t index0, std::optional<size_t> index1);
		ArrayAccessExpression(std::unique_ptr<Expression> expression, const Location &endLocation, size_t index0, std::optional<size_t> index1);
		Expression::Type getType() const override;
		Expression::Result build(InstructionBuilder &builder) const override;
		std::optional<std::reference_wrapper<const Variable>> getVariable() const override;
		std::optional<Expression::TapeRange> getArrayAccesRange() const override;
		bool isTapeTemporary() const override;
};
