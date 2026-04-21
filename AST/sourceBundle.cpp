#include "./sourceBundle.hpp"
#include "../IO/typeError.hpp"

/*!
 * \return Whether the source bundle is empty (i.e. contains no expressions).
 */
bool SourceBundle::isEmpty() const {
	return this->expressions.empty();
};

/*!
 * Add a new expression to the bundle.
 * \param expression The expression to insert.
 * \throw CONDITION_IN_A_SOURCE_BUNDLE If the expression is a condition.
 */
void SourceBundle::add(std::unique_ptr<Expression> expression) {
	if(expression->isCondition())
		throw TypeError(TypeError::Type::CONDITION_IN_A_SOURCE_BUNDLE, expression->location);

	this->expressions.push_back(std::move(expression));
};

/*!
 * Run a closure for each expression in the bundle.
 * \param function The function to run.
 */
void SourceBundle::forEachExpression(const std::function<void (const std::unique_ptr<Expression>&)> function) const {
	std::ranges::for_each(this->expressions, function);
};
