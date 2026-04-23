#pragma once
#include "../common.hpp"
#include "./error.hpp"

/*!
 * This type of warning is a general error, with no special data to print.
 */
class GeneralError: public Error {
	private:
		std::wstring message;

	public:
		GeneralError(std::wstring message);
		std::wstring getMessage() const override;
};
