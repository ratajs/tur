#pragma once
#include <string>
#include "./warning.hpp"

/*!
 * This type of warning is a general warning, with no special data to print.
 */
class GeneralWarning: public Warning {
	private:
		std::wstring message;

	public:
		GeneralWarning(std::wstring message);
		std::wstring getMessage() const override;
};
