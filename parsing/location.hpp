#pragma once
#include "../common.hpp"

/*!
 * This class represents a range in the source code.
 * It holds a view of the range, line numbers where it begins, and also a view of the whole source.
 */
class Location {
	private:
		size_t beginningLineNumber;
		std::wstring_view view, wholeSource;

	public:
		Location(const std::wstring_view &view, size_t beginningLineNumber, const std::wstring_view &wholeSource);
		Location(const Location &locationA, const Location &locationB);
		const std::wstring_view &getView() const;
		size_t getBeginningLineNumber() const;
		const std::wstring_view &getWholeSource() const;
		bool isEmpty() const;
		std::wstring_view getSurroundings(size_t precedingLinesCount = 0, size_t succedingLinesCount = 0) const;
		bool operator == (const Location &location) const;
};
