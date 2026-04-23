#pragma once
#include "../common.hpp"
#include "./error.hpp"

/*!
 * This type of error represents something that should never happen, like a failed internal constraint.
 */
class UnexpectedError: public Error {
	private:
		std::wstring message;

	public:
		UnexpectedError(std::wstring message);
		std::optional<std::wstring> getTitle() const override;
		std::wstring getMessage() const override;
};
