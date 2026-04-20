#pragma once
#include "../../common.hpp"
#include "./expression.hpp"
#include "../../parsing/location.hpp"
#include "../../machine/machine.hpp"

/*!
 * This expression represents the result of a binary operator applied to two inner expressions.
 * It contains the operator and the inner expressionsanan.
 */
class BinaryExpression: public Expression {
	public:
		/*!
		 * These are the supported binary operators:
		 * PLUS: Addition.
		 * MINUS: Subtraction.
		 * MUL: Multiplication.
		 * DIV: Integer division (undefined behavior for division by zero).
		 * MOD: Division remainder (undefined behavior for division by zero).
		 * MIN: Minimum of two values.
		 * MAX: Maximum of two values.
		 * EQ, NE, LT, LTE, GT, GTE: Equality and inequalities.
		 */
		enum class Type {
			PLUS, MINUS, MUL, DIV, MOD, MIN, MAX, EQ, NE, LTE, LT, GTE, GT, AND, OR, XOR
		};

	private:
		BinaryExpression::Type type;
		std::unique_ptr<Expression> expressionA, expressionB;

		void validate() const;
		void swapIfAdvantageous();
		bool isCommutative() const;
		size_t computeConstant(size_t argumentA, size_t argumentB) const;
		Expression::Result buildCondition(InstructionBuilder &builder) const;
		void buildComparison(InstructionBuilder &builder, size_t trueLabel, size_t falseLabel) const;
		void buildTapeLengthComparison(InstructionBuilder &builder, size_t trueLabel, size_t falseLabel) const;
		const Machine &getMachine() const;

	public:
		BinaryExpression(const Location &location, std::unique_ptr<Expression> expressionA, std::unique_ptr<Expression> expressionB, BinaryExpression::Type type);
		BinaryExpression(std::unique_ptr<Expression> expressionA, std::unique_ptr<Expression> expressionB, BinaryExpression::Type type);
		Expression::Type getType() const override;
		Expression::Result build(InstructionBuilder &builder) const override;
		std::optional<Expression::TapeRange> getArrayAccesRange() const override;
		bool isTapeTemporary() const override;
};
