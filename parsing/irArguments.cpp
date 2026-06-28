#include "./irArguments.hpp"
#include <algorithm>
#include <iostream>
#include "../IO/irParseError.hpp"

/*!
 * The constructor of IrArguments.
 * \param view A view of the arguments (the inside of the parentheses). It must remain valid as long as this object is used.
 * \param tapesCount The number of virtual tapes the IR specifies.
 * \param labels A mapping from labels in the IR to labels used further. The new labels should always be generated sequentially, if an unknown label is encountered, IrArguments maps it to a new label the number of which is the size of the map.
 * \param location The location in the source code, used for errors.
 */
IrArguments::IrArguments(std::wstring_view view, size_t tapesCount, std::map<size_t, size_t> &labels, Location location): tapesCount(tapesCount), iss(std::wstring(view)), location(location), labels(labels) {};

/*!
 * Expect a specific character (whitespace is ignored).
 * \param character The character to read.
 * \throw IrParseError If the character is not there.
 */
void IrArguments::readCharacter(wchar_t character) {
	wchar_t readCharacter;

	if(!(this->iss >> readCharacter) || readCharacter!=character)
		throw IrParseError(IrParseError::Type::INVALID_ARGUMENTS, this->location);
};

/*!
 * Expect one of specified strings.
 * One of the strings has to follow (possibly after whitespace).
 * The string is read from the input all the way until the end or the next whitespace, being a prefix is not enough.
 * \param allowedStrings All strings that can follow.
 * \return Index of the recognized string.
 * \throw IrParseError Ir none of the provided strings was there.
 */
size_t IrArguments::readString(std::vector<std::wstring> allowedStrings) {
	std::wstring string;
	std::vector<std::wstring>::const_iterator index;

	if(!(this->iss >> string))
		throw IrParseError(IrParseError::Type::INVALID_ARGUMENTS, this->location);

	index = std::ranges::find(allowedStrings, string);

	if(index==allowedStrings.end())
		throw IrParseError(IrParseError::Type::INVALID_ARGUMENTS, this->location);

	return (index - allowedStrings.begin());
};

/*!
 * Expect a comma.
 * \throw IrParseError If the comma was not there.
 */
void IrArguments::readComma() {
	this->readCharacter(',');
};

/*!
 * Expect a number.
 * \return The read number.
 * \throw IrParseError If the number was not there.
 */
size_t IrArguments::readNumber() {
	size_t number;

	if(!(this->iss >> number))
		throw IrParseError(IrParseError::Type::INVALID_ARGUMENTS, this->location);

	return number;
};

/*!
 * Expect a number of a tape, at least 1, at most the number of tapes.
 * \return The number, but decremented by one, so that the first tape is not 1, but 0.
 * \throw IrParseError If no number was there or it was not a valid tape.
 */
size_t IrArguments::readTape() {
	size_t tape;

	tape = this->readNumber();

	if(tape==0 || tape > this->tapesCount)
		throw IrParseError(IrParseError::Type::INVALID_TAPE_NUMBER, this->location); //TODO more precise location?

	return (tape - 1);
};

/*!
 * Expect a label (a number).
 * If the label is known by the mapping, return it, if it is unknown, create it as the size of the map.
 * \return The label, renamed by the mapping.
 * \throw IrParseError If there was no number.
 */
size_t IrArguments::readLabel() {
	size_t label;

	label = this->readNumber();

	if(this->labels.contains(label))
		return this->labels.at(label);

	return this->labels.emplace(label, this->labels.size()).first->second;
};

/*!
 * Expect a range in square brackets.
 * The first index is required, followed by a colon and an optional second index.
 * \return The range (two indices, the second is optional).
 * \throw IrParseError If no range was there.
 */
std::pair<size_t, std::optional<size_t>> IrArguments::readRange() {
	size_t index0;
	std::optional<size_t> index1;

	this->readCharacter('[');
	index0 = this->readNumber();
	this->readCharacter(':');
	if(this->iss.peek()!=']')
		index1 = this->readNumber();
	this->readCharacter(']');

	return { index0, index1 };
};

/*!
 * Expect a rightwise unbounded range (used as a destination) in square brackets.
 * It is either an index followed by a colon or the square brackets are empty.
 * \return The range (an optional index).
 * \throw IrParseError If no such range was there.
 */
std::optional<size_t> IrArguments::readRightwiseUnboundedRange() {
	size_t index;

	this->readCharacter('[');
	if(this->iss.peek()==']') {
		this->iss.get();

		return {};
	}
	else {
		index = this->readNumber();
		this->readCharacter(':');
		this->readCharacter(']');

		return index;
	};
};

/*!
 * Expect either a tape with an index in square brackets or a number.
 * \return The read value (either tape and index or the immediate value).
 * \throw IrParseError If no such value was there.
 */
std::variant<std::pair<size_t, size_t>, size_t> IrArguments::readTapeAndIndexOrNumber() {
	size_t firstNumber, index;

	firstNumber = this->readNumber();

	if(this->iss.peek()=='[') {
		this->iss.get();
		index = this->readNumber();
		this->readCharacter(']');

		if(firstNumber==0 || firstNumber > this->tapesCount)
			throw IrParseError(IrParseError::Type::INVALID_TAPE_NUMBER, this->location); //TODO more precise location?

		return std::pair(firstNumber - 1, index);
	};

	return firstNumber;
};

/*!
 * Expect a Turing machine in braces.
 * \return The machine.
 * \throw IrParseError If the machine was not there or was invalid.
 */
Machine IrArguments::readMachine() {
	std::wstring machineString;
	Machine machine;

	this->readCharacter('{');

	std::getline(this->iss, machineString, L'}');

	if(!this->iss || this->iss.eof() || (std::wistringstream(std::move(machineString)) >> machine).fail() || !machine)
		throw IrParseError(IrParseError::Type::INVALID_ARGUMENTS, this->location);

	return machine;
};

/*!
 * Expect the end of the arguments.
 * \throw IrParseError If the arguments are not yet consumed.
 */
void IrArguments::end() {
	this->iss.peek(); //FIXME whitespaces before EOF

	if(!this->iss.eof())
		throw IrParseError(IrParseError::Type::INVALID_ARGUMENTS, this->location);
};

/*!
 * \return The location the object was initialized with.
 */
Location IrArguments::getLocation() const {
	return this->location;
};
