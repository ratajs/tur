#pragma once
#include "../common.hpp"
#include "../machine/machine.hpp"

class IrArguments {
	private:
		size_t tapesCount, lineNumber;
		std::wistringstream iss; //TODO use std::wispanstream when it’s supported by libc++

	public:
		IrArguments();
		IrArguments(std::wstring_view view, size_t tapesCount, size_t lineNumber);
		void readCharacter(wchar_t character);
	//size_t readString(std::same_as<std::wstring_view> auto... allowedStrings);
		size_t readString(std::vector<std::wstring> allowedStrings);
		void readComma();
		size_t readNumber();
		size_t readTape();
		std::pair<size_t, std::optional<size_t>> readRange();
		std::optional<size_t> readRightwiseUnboundedRange();
		std::variant<std::pair<size_t, size_t>, size_t> readTapeAndIndexOrNumber();
		Machine readMachine();
		void end();
		size_t getLineNumber() const; //TODO replace with location
};
