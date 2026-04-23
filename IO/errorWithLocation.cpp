#include "./errorWithLocation.hpp"
#include "../parsing/location.hpp"
#include "./format.hpp"

/*!
 * The constructor of ErrorWithLocation.
 * \param location The location to print.
 */
ErrorWithLocation::ErrorWithLocation(const Location &expressionLocation): lineNumber(expressionLocation.getBeginningLineNumber()) {
	std::wstring_view surroundings;

	surroundings = expressionLocation.getSurroundings();
	this->surroundings = surroundings;
	this->location = {
		this->surroundings.begin() + (expressionLocation.getView().begin() - surroundings.begin()),
		this->surroundings.begin() + (expressionLocation.getView().end() - surroundings.begin())
	};
};

/*!
 * This method should be used by derived errors to print the location.
 * \return A string representation of the surrounding of the location.
 */
std::wstring ErrorWithLocation::printLocation() const {
	return (
		L"at line "+Format::blue(std::to_wstring(this->lineNumber))+L".\n"+
		std::wstring(std::wstring_view(this->surroundings).begin(), this->location.begin())+
		Format::yellow(std::wstring(this->location))+
		std::wstring(this->location.end(), std::wstring_view(this->surroundings).end())
	);
};
