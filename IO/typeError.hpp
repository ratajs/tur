#pragma once
#include "../common.hpp"
#include "./errorWithLocation.hpp"
#include "../parsing/location.hpp"

/*!
 * This type of error means that the AST could not be constructed even though no parse error has been found.
 * All types are connected with expressions.
 */
class TypeError: public ErrorWithLocation {
	public:
		/*!
		 * These are all types of TypeError.
		 */
		enum class Type {
			MULTIPLE_VARIABLES_WITH_ELLIPSIS_IN_A_DESTINATION_BUNDLE,
			CONDITION_IN_A_SOURCE_BUNDLE,
			CONDITION_IN_AN_EXPLOSION_STATEMENT,
			CONSTANT_IN_AN_EXPLOSION_STATEMENT,
			BOUNDED_EXPRESSION_AN_EXPLOSION_STATEMENT,
			VARIABLE_MULTIPLE_TIMES_IN_A_DESTINATION_BUNDLE,
			NON_FINAL_ELLIPSIS_IN_A_DESTINATION_BUNDLE,
			NON_CONDITION_IN_A_WHILE_STATEMENT,
			CONDITION_AS_AN_ASSIGNMENT_SOURCE,
			NON_CONDITION_IN_AN_IF_STATEMENT,
			CONDITION_AS_A_CALL_ARGUMENT,
			CONSTANT_WITH_AN_ARRAY_ACCESS,
			CONDITION_WITH_AN_ARRAY_ACCESS,
			DOUBLE_ARRAY_ACCESS,
			CONDITION_IN_AN_ARITHMETIC_OPERATION,
			LONG_ARRAY_ACCESS_IN_AN_ARITHMETIC_OPERATION,
			NON_CONDITION_IN_A_LOGICAL_OPERATION,
			INVALID_ARRAY_ACCESS_INDICES
		};

	private:
		TypeError::Type type;

		std::wstring getSimpleMessage() const;

	public:
		TypeError(TypeError::Type type, const Location &expressionLocation);
		std::optional<std::wstring> getTitle() const override;
		std::wstring getMessage() const override;
};
