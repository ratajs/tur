#include "./irArguments.hpp"
#include "../IO/generalError.hpp"
#include "../IO/format.hpp"

IrArguments::IrArguments(std::wstring_view view, size_t tapesCount, size_t lineNumber, std::map<size_t, size_t> &labels): tapesCount(tapesCount), lineNumber(lineNumber), iss(std::wstring(view)), labels(labels) {};

void IrArguments::readCharacter(wchar_t character) {
	wchar_t readCharacter;

	if(!(this->iss >> readCharacter) || readCharacter!=character)
		throw GeneralError(L"Invalid arguments on line "+Format::blue(std::to_wstring(this->lineNumber))+L".");
};

size_t IrArguments::readString(std::vector<std::wstring> allowedStrings) {
	std::wstring string;
	std::vector<std::wstring>::const_iterator index;

	if(!(this->iss >> string))
		throw GeneralError(L"Invalid arguments on line "+Format::blue(std::to_wstring(this->lineNumber))+L".");

	index = std::ranges::find(allowedStrings, string);

	if(index==allowedStrings.end())
		throw GeneralError(L"Invalid arguments on line "+Format::blue(std::to_wstring(this->lineNumber))+L".");

	return (index - allowedStrings.begin());
};

void IrArguments::readComma() {
	this->readCharacter(',');
};

size_t IrArguments::readNumber() {
	size_t number;

	if(!(this->iss >> number))
		throw GeneralError(L"Invalid arguments on line "+Format::blue(std::to_wstring(this->lineNumber))+L".");

	return number;
};

size_t IrArguments::readTape() {
	size_t tape;

	tape = this->readNumber();

	if(tape==0 || tape > this->tapesCount)
		throw GeneralError(L"Invalid tape number on line "+Format::blue(std::to_wstring(this->lineNumber))+L".");

	return (tape - 1);
};

size_t IrArguments::readLabel() {
	size_t label;

	label = this->readNumber();

	if(this->labels.contains(label))
		return this->labels.at(label);

	return this->labels.emplace(label, this->labels.size()).first->second;
};

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

std::variant<std::pair<size_t, size_t>, size_t> IrArguments::readTapeAndIndexOrNumber() {
	size_t firstNumber, index;

	firstNumber = this->readNumber();
	if(this->iss.peek()=='[') {
		this->iss.get();
		index = this->readNumber();
		this->readCharacter(']');

		if(firstNumber==0 || firstNumber > this->tapesCount)
			throw GeneralError(L"Invalid tape number on line "+Format::blue(std::to_wstring(this->lineNumber))+L".");

		return std::pair(firstNumber - 1, index);
	}
	else
		return firstNumber;
};

Machine IrArguments::readMachine() {
	std::wstring machineString;
	Machine machine;

	this->readCharacter('{');

	std::getline(iss, machineString, L'}');

	if(!iss || iss.eof() || (std::wistringstream(std::move(machineString)) >> machine).fail() || !machine)
		throw GeneralError(L"Invalid arguments on line "+Format::blue(std::to_wstring(this->lineNumber))+L".");

	return machine;
};

void IrArguments::end() {
	this->iss.peek();
	if(!this->iss.eof())
		throw GeneralError(L"Invalid arguments on line "+Format::blue(std::to_wstring(this->lineNumber))+L".");
};

size_t IrArguments::getLineNumber() const {
	return this->lineNumber;
};
