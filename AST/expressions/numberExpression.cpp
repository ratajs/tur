#include "./numberExpression.hpp"

/*!
 * The constructor of NumberExpression.
 * \param location The location of the expression in code.
 * \param number The value of the constant.
 */
NumberExpression::NumberExpression(const Location &location, size_t number): Expression(location), number(number) {};

Expression::Type NumberExpression::getType() const {
	return Expression::Type::CONSTANT;
};

Expression::Result NumberExpression::build(InstructionBuilder &builder) const {
	return Expression::Result::createConstant(this->number);
};

std::optional<Expression::TapeRange> NumberExpression::getArrayAccesRange() const {
	return {};
};
