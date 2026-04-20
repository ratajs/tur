#pragma once
#include "../common.hpp"
#include "./error.hpp"
#include "../parsing/location.hpp"

class ErrorWithLocation: public Error {
	protected:
		size_t lineNumber;
		std::wstring surroundings;
		std::wstring_view location;

		std::wstring printLocation() const;

	public:
		ErrorWithLocation(const Location &location);
};
