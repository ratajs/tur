#include "./expression.hpp"
#include "../../IO/unexpectedError.hpp"

/*!
 * Initialize the result of type CONSTANT.
 * \param value The value of the constant.
 */
Expression::Result Expression::Result::createConstant(size_t value) {
	Expression::Result result;

	result.value = value;

	return result;
};

/*!
	* Initialize the result of type TAPE_RANGE.
 * \param tape The index of the returned tape.
 * \param range The range of the tape, this should be { 0, 1 } if unspecified explicitly.
 */
Expression::Result Expression::Result::createTapeRange(size_t tape, Expression::TapeRange range) {
	Expression::Result result;

	result.tapeAndRange = { tape, range };

	return result;
};

/*!
 * Initialize the result of type CONDITION.
 * \param trueLabel The label whither the program will jump if the condition is evaluated as true.
 * \param falseLabel The label whither the program will jump if the condition is evaluated as false.
 */
Expression::Result Expression::Result::createCondition(size_t trueLabel, size_t falseLabel) {
	Expression::Result result;

	result.labels = { trueLabel, falseLabel };

	return result;
};

/*!
 * The default constructor constructs a range which only includes the first number.
 */
Expression::TapeRange::TapeRange(): index0(0), index1(1) {};

/*!
	\param index0 The starting index of the range.
	\param index1 The end index of the range, {} means that the range is rightwise unbounded.
 */
Expression::TapeRange::TapeRange(size_t index0, std::optional<size_t> index1): index0(index0), index1(index1) {};

/*!
 * A delegate constructor to save the location of the expression.
 * \param location The location of this expression.
 */
Expression::Expression(const Location &location): location(location) {};

Expression::~Expression() {};

/*!
 * \retval true If the expression type is CONDITION.
 * \retval false Otherwise.
 */
bool Expression::isCondition() const {
	return (this->getType()==Expression::Type::CONDITION);
};

/*!
 * \retval true If the expression type is CONSTANT.
 * \retval false Otherwise.
 */
bool Expression::isConstant() const {
	return (this->getType()==Expression::Type::CONSTANT);
};

/*!
 * Calls the build function and retrieves the constant variant.
 * \return The built constant value.
 * \throw UnexpectedError If the type is not CONSTANT.
 */
size_t Expression::buildConstant(InstructionBuilder &builder) const {
	if(!this->isConstant())
		throw UnexpectedError(L"Wrong expression type for building.");

	return this->build(builder).value;
};

/*!
 * Calls the build function and retrieves the tape variant.
 * \return The built tape and range.
 * \throw UnexpectedError If the type is not TAPE_RANGE.
 */
std::pair<size_t, Expression::TapeRange> Expression::buildTape(InstructionBuilder &builder) const {
	if(this->isConstant() || this->isCondition())
		throw UnexpectedError(L"Wrong expression type for building.");

	return this->build(builder).tapeAndRange;
};

/*!
 * Calls the build function and retrieves the condition variant.
 * \return The built labels.
 * \throw UnexpectedError If the type is not CONDITION.
 */
std::pair<size_t, size_t> Expression::buildCondition(InstructionBuilder &builder) const {
	if(!this->isCondition())
		throw UnexpectedError(L"Wrong expression type for building.");

	return this->build(builder).labels;
};

/*!
 * Computes the length of the array access from the result of getArrayAccesRange.
 * \return 0, if getArrayAccesRange returns {}, {} if the range is unbounded rightwise, the length (difference between indices) otherwise.
 */
std::optional<size_t> Expression::getArrayAccessLength() const {
	std::optional<Expression::TapeRange> range;

	range = this->getArrayAccesRange();

	if(!range)
		return 0;

	if(!range->index1)
		return {};

	if((*range->index1) <= range->index0)
		throw UnexpectedError(L"Invalid range to assess length.");

	return ((*range->index1) - range->index0);
};

bool Expression::isTapeTemporary() const {
	return false;
};

std::optional<std::reference_wrapper<const Variable>> Expression::getVariable() const {
	return {};
};

std::optional<std::reference_wrapper<const Variable>> Expression::getMeasuredVariable() const {
	return {};
};
