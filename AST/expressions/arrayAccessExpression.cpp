#include "./arrayAccessExpression.hpp"
#include "../../IO/typeError.hpp"

/*!
 * The constructor of ArrayAccessExpression.
 * \param location The location of the expression in code.
 * \param expression The inner expression, it must not be indexed (no explicit array access, expressions which only yield one number, like count or arithmetics are also invalid).
 * \param index0 The index where the range starts (inclusive).
 * \param index1 The index where the range starts (exclusive). {} (std::nullopt) means that the range is rightwise unbounded.
 * \throw TypeError If the range end is not after the range start, the inner expression if not of type TAPE_RANGE or if it is already indexed.
 */
ArrayAccessExpression::ArrayAccessExpression(const Location &location, std::unique_ptr<Expression> expression, size_t index0, std::optional<size_t> index1):
	Expression(location), index0(index0), index1(index1), expression(std::move(expression)) {
		this->validate();
	};

/*!
 * An alternative constructor of ArrayAccessExpression.
 * This constructor uses the location of the inner expression and extends it to the left.
 * The location of the new expression will span from beginning of the location of the inner expression to the end of the endLocation argument.
 * \param expression The inner expression.
 * \param endLocation A location used for the expression location.
 * \param index0 The index where the range starts (inclusive).
 * \param index1 The index where the range starts (exclusive). {} (std::nullopt) means that the range is rightwise unbounded.
 * \throw TypeError If the range end is not after the range start, the inner expression if not of type TAPE_RANGE or if it is already indexed.
 */
ArrayAccessExpression::ArrayAccessExpression(std::unique_ptr<Expression> expression, const Location &endLocation, size_t index0, std::optional<size_t> index1):
	Expression({ expression->location, endLocation }), index0(index0), index1(index1), expression(std::move(expression)) {
		this->validate();
	};

/*!
 * A helper function to throw the TypeError for the constructor if the inner expression or the indices are invalid.
 * \throw TypeError If the range end is not after the range start, the inner expression if not of type TAPE_RANGE or if it is already indexed.
 */
void ArrayAccessExpression::validate() const {
	if(this->index1 && (*this->index1) <= index0)
		throw TypeError(TypeError::Type::INVALID_ARRAY_ACCESS_INDICES, this->location);

	if(this->expression->isConstant())
		throw TypeError(TypeError::Type::CONSTANT_WITH_AN_ARRAY_ACCESS, this->expression->location);

	if(this->expression->isCondition())
		throw TypeError(TypeError::Type::CONDITION_WITH_AN_ARRAY_ACCESS, this->expression->location);

	if(this->expression->getArrayAccessLength()!=0)
		throw TypeError(TypeError::Type::DOUBLE_ARRAY_ACCESS, this->expression->location);
};

Expression::Type ArrayAccessExpression::getType() const {
	return this->expression->getType();
};

Expression::Result ArrayAccessExpression::build(InstructionBuilder &builder) const {
	return Expression::Result::createTapeRange(this->expression->buildTape(builder).first, Expression::TapeRange(this->index0, this->index1));
};

std::optional<std::reference_wrapper<const Variable>> ArrayAccessExpression::getVariable() const {
	return this->expression->getVariable();
};

std::optional<Expression::TapeRange> ArrayAccessExpression::getArrayAccesRange() const {
	return Expression::TapeRange(this->index0, this->index1);
};

bool ArrayAccessExpression::isTapeTemporary() const {
	return this->expression->isTapeTemporary();
};
