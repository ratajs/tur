#pragma once
#include "../../common.hpp"
#include "./expression.hpp"
#include "../../parsing/location.hpp"

/*!
 * This expression represents the result of a unary operator applied to an inner expression.
 * It contains the the operator and the inner expression.
 */
class UnaryExpression: public Expression {
	public:
		/*!
		 * These are the supported unary operators:
		 * NEG: A logical operator inverting the logical outcome.
		 */
		enum class Type {
			NEG
		};

	private:
		UnaryExpression::Type type;
		std::unique_ptr<Expression> expression;

		void validate() const;

	public:
		UnaryExpression(const Location &location, std::unique_ptr<Expression> expression, UnaryExpression::Type type);
		UnaryExpression(std::unique_ptr<Expression> expression, UnaryExpression::Type type);
		Expression::Type getType() const override;
		Expression::Result build(InstructionBuilder &builder) const override;
		std::optional<Expression::TapeRange> getArrayAccesRange() const override;
		bool isTapeTemporary() const override;
};
