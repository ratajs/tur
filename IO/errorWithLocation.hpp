#pragma once
#include "../common.hpp"
#include "./error.hpp"
#include "../parsing/location.hpp"

/*!
 * This abstract class is used as a base by errors that need to print a part of the source code.
 */
class ErrorWithLocation: public Error {
	protected:
		size_t lineNumber;
		std::wstring surroundings;
		std::wstring_view location;

		std::wstring printLocation() const;

	public:
		ErrorWithLocation(const Location &location);
};
