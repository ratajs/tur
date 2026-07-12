#include "./unexpectedError.hpp"
#include <utility>
#include <locale>
#include <codecvt>
#include "./format.hpp"

/*!
 * The constructor of UnexpectedError.
 * \param message The message.
 * \param location The location where the error is thrown, the default value should be kept to use the location of the called constructor.
 */
UnexpectedError::UnexpectedError(std::wstring message, std::source_location location): message(std::move(message)), location(std::move(location)) {};

std::optional<std::wstring> UnexpectedError::getTitle() const {
	return L"Unexpected";
};

std::wstring UnexpectedError::getMessage() const {
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

	return (this->message+L"\nThrown in "+Format::boldYellow(converter.from_bytes(this->location.file_name()))+L" at line "+Format::boldBlue(std::to_wstring(this->location.line()))+L".");
};
