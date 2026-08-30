#pragma once
#include <optional>
#include <string>
#include <source_location>
#include "./error.hpp"

/*!
 * This type of error represents something that should never happen, like a failed internal constraint.
 */
class UnexpectedError: public Error {
	private:
		std::wstring message;
		std::source_location location;

	public:
		UnexpectedError(std::wstring message, std::source_location location = std::source_location::current());
		std::optional<std::wstring> getTitle() const override;
		std::wstring getMessage() const override;
};
