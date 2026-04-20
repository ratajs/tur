#include "./variableExpression.hpp"

/*!
 * The constructor of VariableExpression.
 * \param location The location of the expression in code.
 * \param variable The variable.
 */
VariableExpression::VariableExpression(const Location &location, Variable &variable): Expression(location), variable(variable) {};

Expression::Type VariableExpression::getType() const {
	return Expression::Type::TAPE_RANGE;
};

Expression::Result VariableExpression::build(InstructionBuilder &builder) const {
	return Expression::Result::create<Expression::Type::TAPE_RANGE>(*this->variable.tape, Expression::TapeRange(0, 1));
};

std::optional<std::reference_wrapper<const Variable>> VariableExpression::getVariable() const {
	return this->variable;
};

std::optional<Expression::TapeRange> VariableExpression::getArrayAccesRange() const {
	return {};
};
