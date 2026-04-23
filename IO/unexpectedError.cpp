#include "./unexpectedError.hpp"

/*!
 * The constructor of UnexpectedError.
 * \param message The message.
 */
UnexpectedError::UnexpectedError(std::wstring message): message(std::move(message)) {};

std::optional<std::wstring> UnexpectedError::getTitle() const {
	return L"Unexpected";
};

std::wstring UnexpectedError::getMessage() const {
	return this->message;
};
