#pragma once
#include "../common.hpp"
#include "./expressions/expression.hpp"

class SourceBundle {
	private:
		std::vector<std::unique_ptr<Expression>> expressions;

	public:
		bool isEmpty() const;
		void add(std::unique_ptr<Expression> expression);
		void forEachExpression(const std::function<void (const std::unique_ptr<Expression>&)> function) const;
};
