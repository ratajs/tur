#include "./sourceBundle.hpp"
#include "../IO/typeError.hpp"

bool SourceBundle::isEmpty() const {
	return this->expressions.empty();
};

void SourceBundle::add(std::unique_ptr<Expression> expression) {
	if(expression->isCondition())
		throw TypeError(TypeError::Type::CONDITION_IN_A_SOURCE_BUNDLE, expression->location);

	this->expressions.push_back(std::move(expression));
};

void SourceBundle::forEachExpression(const std::function<void (const std::unique_ptr<Expression>&)> function) const {
	std::ranges::for_each(this->expressions, function);
};
