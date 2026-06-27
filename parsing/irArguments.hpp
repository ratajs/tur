#pragma once
#include "../common.hpp"
#include "../machine/machine.hpp"

class IrArguments {
	private:
		size_t tapesCount, lineNumber;
		std::wistringstream iss; //TODO use std::wispanstream when it’s supported by libc++
		std::map<size_t, size_t> &labels;

	public:
		IrArguments(std::wstring_view view, size_t tapesCount, size_t lineNumber, std::map<size_t, size_t> &labels);
		void readCharacter(wchar_t character);
		size_t readString(std::vector<std::wstring> allowedStrings);
		void readComma();
		size_t readNumber();
		size_t readTape();
		size_t readLabel();
		std::pair<size_t, std::optional<size_t>> readRange();
		std::optional<size_t> readRightwiseUnboundedRange();
		std::variant<std::pair<size_t, size_t>, size_t> readTapeAndIndexOrNumber();
		Machine readMachine();
		void end();
		size_t getLineNumber() const; //TODO replace with location
};
