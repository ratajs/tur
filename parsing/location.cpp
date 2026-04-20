#include "./location.hpp"
#include "../IO/unexpectedError.hpp"

/*!
 * The constructor of Location.
 * \param view The view where the specific range is found.
 * \param beginningLineNumber The line number where the range begins.
 * \param wholeSource A view of the whole source.
 * \throws UnexpectedError when the range is not inside the whole source.
 */
Location::Location(const std::wstring_view &view, size_t beginningLineNumber, const std::wstring_view &wholeSource): beginningLineNumber(beginningLineNumber), view(view), wholeSource(wholeSource) {
	if(view.begin() < wholeSource.begin() || view.end() > wholeSource.end())
		throw UnexpectedError(L"Invalid location (view not a subview of wholeSource).");
};

/*!
 * An alternative constructor of Location.
 * It merges two existing locations.
 * The beginning will be the beginning of the first location.
 * The end will be the end of the second location.
 * \throw UnexpectedError If the locationA does not lie (with possible overlap) before locationB in the same source.
 */
Location::Location(const Location &locationA, const Location &locationB): beginningLineNumber(locationA.beginningLineNumber), wholeSource(locationA.wholeSource) {
	if(locationB.wholeSource!=locationA.wholeSource)
		throw UnexpectedError(L"Invalid location (locationA and locationB do not share the same source).");

	if(locationB.beginningLineNumber < locationA.beginningLineNumber || locationA.view.begin() > locationB.view.begin() || locationA.view.end() > locationB.view.end())
		throw UnexpectedError(L"Invalid location (locationB is not after locationA and they are not the same location).");

	this->view = { locationA.view.begin(), locationB.view.end() };
};

/*!
 * \return The view of the range.
 */
const std::wstring_view &Location::getView() const {
	return this->view;
};

/*!
 * \return The line number of the beginning of the range.
 */
size_t Location::getBeginningLineNumber() const {
	return this->beginningLineNumber;
};

/*!
 * \return A view of the whole source.
 */
const std::wstring_view &Location::getWholeSource() const {
	return this->wholeSource;
};

/*!
 * \return Whether the range is empty.
 */
bool Location::isEmpty() const {
	return this->getView().empty();
};

/*!
 * Gets a view of a block of whole lines surrounding the range (without the last line break).
 * \param precedingLinesCount The number of lines before (0 means that no extra line is added, the surrounding starts at the beginning of the first line of the range).
 * \param succedingLinesCount The number of lines after (0 means that no extra line is added, the surrounding ends at the end of the last line of the range).
 * \return A view of the surroundings.
 */
std::wstring_view Location::getSurroundings(size_t precedingLinesCount, size_t succedingLinesCount) const {
	std::wstring_view surroundings;
	std::wstring_view::const_iterator firstCharacter, lastCharacter;

	if(this->wholeSource.empty())
		return this->wholeSource;

	firstCharacter = this->view.begin();
	if(firstCharacter==this->wholeSource.end())
		firstCharacter--;

	while(firstCharacter!=this->wholeSource.begin() && ((*std::prev(firstCharacter))!='\n' || (precedingLinesCount--) > 0))
		firstCharacter--;

	lastCharacter = this->view.end();
	if(lastCharacter!=this->wholeSource.begin())
		lastCharacter--;

	while(std::next(lastCharacter)!=this->wholeSource.end() && ((*std::next(lastCharacter))!='\n' || (succedingLinesCount--) > 0))
		lastCharacter++;

	surroundings = { firstCharacter, std::next(lastCharacter) };

	return surroundings;
};

/*!
 * \return Whether both locations represent the same range in the same source (with the same line numbering).
 */
bool Location::operator == (const Location &location) const {
	return (
		this->view.begin()==location.view.begin() &&
		this->view.end()==location.view.end() &&
		this->beginningLineNumber==location.beginningLineNumber &&
		this->wholeSource==location.wholeSource
	);
};
