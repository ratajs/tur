#pragma once
#include "../../common.hpp"
#include "./expression.hpp"
#include "../../parsing/location.hpp"
#include "../../machine/machine.hpp"

/*!
 * This expression represents the result of a given Turing machine run on a result of the inner expresisons (joined together).
 * It contains the machine and the inner expressions.
 */
class CallExpression: public Expression {
	private:
		const Machine &machine;
		std::vector<std::unique_ptr<Expression>> arguments;

	public:
		CallExpression(const Location &location, const Machine &machine, std::vector<std::unique_ptr<Expression>> arguments);
		Expression::Type getType() const override;
		Expression::Result build(InstructionBuilder &builder) const override;
		std::optional<Expression::TapeRange> getArrayAccesRange() const override;
		bool isTapeTemporary() const override;
};
