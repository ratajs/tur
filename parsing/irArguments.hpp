#pragma once
#include <cstdlib>
#include <utility>
#include <optional>
#include <variant>
#include <vector>
#include <map>
#include <string>
#include <string_view>
#include <sstream>
#include "./location.hpp"
#include "../machine/machine.hpp"

/*!
 * An instance of this class represents the inside of parentheses in IR, together with extra information about tapes, labels and the current location in the source.
 * Various types of arguments can be read from it.
 */
class IrArguments {
	private:
		size_t tapesCount;
		std::wistringstream iss; //TODO use std::wispanstream when it’s supported by libc++
		Location location;
		std::map<size_t, size_t> &labels;

	public:
		IrArguments(std::wstring_view view, size_t tapesCount, std::map<size_t, size_t> &labels, Location location);
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
		Location getLocation() const;
};
