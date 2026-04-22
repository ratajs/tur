#include "./unaryExpression.hpp"
#include "../../IO/typeError.hpp"

/*!
 * The constructor of UnaryExpression.
 * \param location The location of the expression in code.
 * \param expression The inner expression (operand).
 * \param type The operator to apply.
 * \throw TypeError If the operator is incompatible with the operand.
 */
UnaryExpression::UnaryExpression(const Location &location, std::unique_ptr<Expression> expression, UnaryExpression::Type type): Expression(location), type(type), expression(std::move(expression)) {
	this->validate();
};

UnaryExpression::UnaryExpression(std::unique_ptr<Expression> expression, UnaryExpression::Type type): Expression(expression->location), type(type), expression(std::move(expression)) {
	this->validate();
};

/*!
 * A helper function to throw the TypeError for the constructor if the types do not match.
 * \throw TypeError If the operator is incompatible with the operand.
 */
void UnaryExpression::validate() const {
	if(this->expression->getType()!=Expression::Type::CONDITION)
		throw TypeError(TypeError::Type::NON_CONDITION_IN_A_LOGICAL_OPERATION, this->expression->location);
};

Expression::Type UnaryExpression::getType() const {
	switch(this->type) {
		case UnaryExpression::Type::NEG:
			return Expression::Type::CONDITION;

		default:
			std::unreachable();
	};
};

Expression::Result UnaryExpression::build(InstructionBuilder &builder) const {
	size_t trueLabel = 0, falseLabel = 0; // Initialized to prevent compiler warnings

	switch(this->type) {
		case UnaryExpression::Type::NEG:
			std::tie(trueLabel, falseLabel) = this->expression->buildCondition(builder);

			return Expression::Result::createCondition(falseLabel, trueLabel);

		default:
			std::unreachable();
	};
};

std::optional<Expression::TapeRange> UnaryExpression::getArrayAccesRange() const {
	return {};
};

bool UnaryExpression::isTapeTemporary() const {
	switch(this->type) {
		case UnaryExpression::Type::NEG:
			return false;

		default:
			std::unreachable();
	};
};
