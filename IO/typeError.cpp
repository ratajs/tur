#include "./typeError.hpp"
#include "../parsing/location.hpp"
#include "./format.hpp"

TypeError::TypeError(TypeError::Type type, const Location &expressionLocation): ErrorWithLocation(expressionLocation), type(type) {};

std::wstring TypeError::getSimpleMessage() const {
	switch(this->type) {
		case TypeError::Type::MULTIPLE_VARIABLES_WITH_ELLIPSIS_IN_A_DESTINATION_BUNDLE:
			return L"Invalid destination bundle: Multiple variables with ellipsis";

		case TypeError::Type::CONDITION_IN_A_SOURCE_BUNDLE:
			return L"Invalid expression for a source bundle: A condition";

		case TypeError::Type::CONDITION_IN_AN_EXPLOSION_STATEMENT:
			return L"Invalid source expression for explosion: A condition";

		case TypeError::Type::CONSTANT_IN_AN_EXPLOSION_STATEMENT:
			return L"Invalid source expression for explosion: A constant";

		case TypeError::Type::BOUNDED_EXPRESSION_AN_EXPLOSION_STATEMENT:
			return L"Invalid source expression for explosion: A bounded expression";

		case TypeError::Type::VARIABLE_MULTIPLE_TIMES_IN_A_DESTINATION_BUNDLE:
			return L"Invalid explosion: The same variable multiple times in a destination bundle";

		case TypeError::Type::NON_FINAL_ELLIPSIS_IN_A_DESTINATION_BUNDLE:
			return L"Invalid destination bundle for explosion: Ellipsis not at the end";

		case TypeError::Type::NON_CONDITION_IN_A_WHILE_STATEMENT:
			return L"Invalid while condition: Not a condition";

		case TypeError::Type::CONDITION_AS_AN_ASSIGNMENT_SOURCE:
			return L"Invalid assignment source: Condition";

		case TypeError::Type::NON_CONDITION_IN_AN_IF_STATEMENT:
			return L"Invalid if condition: Not a condition";

		case TypeError::Type::CONDITION_AS_A_CALL_ARGUMENT:
			return L"Invalid argument call argument: Condition";

		case TypeError::Type::CONSTANT_WITH_AN_ARRAY_ACCESS:
			return L"Invalid expression for array access or ellipsis: A constant";

		case TypeError::Type::CONDITION_WITH_AN_ARRAY_ACCESS:
			return L"Invalid expression for array access or ellipsis: A condition";

		case TypeError::Type::DOUBLE_ARRAY_ACCESS:
			return L"Invalid expression for array access or ellipsis: Already sliced expression or an expression with only one number";

		case TypeError::Type::CONDITION_IN_AN_ARITHMETIC_OPERATION:
			return L"Invalid operand for an arithmetic operator: Condition";

		case TypeError::Type::LONG_ARRAY_ACCESS_IN_AN_ARITHMETIC_OPERATION:
			return L"Invalid operand for an arithmetic operator: Variable with a long array access or ellipsis";

		case TypeError::Type::NON_CONDITION_IN_A_LOGICAL_OPERATION:
			return L"Invalid operand for a logical operator: Not a condition";

		case TypeError::Type::INVALID_ARRAY_ACCESS_INDICES:
			return L"Invalid indices for array access.";

		default:
			std::unreachable();
	};
};

std::optional<std::wstring> TypeError::getTitle() const {
	return L"Type";
};

std::wstring TypeError::getMessage() const {
	return (Format::yellow(this->getSimpleMessage())+L" "+this->printLocation());
};
