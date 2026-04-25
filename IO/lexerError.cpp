#include "./lexerError.hpp"
#include "./unexpectedError.hpp"
#include "./format.hpp"

/*!
 * The constructor of LexerError.
 * \param type The type of the error.
 * \param location The location to print.
 * \throw UnexpectedError If the type of the error needs a character to print.
 */
LexerError::LexerError(LexerError::Type type, const Location &location): ErrorWithLocation(location), type(type) {
	if(type==LexerError::Type::UNEXPECTED_CHARACTER)
		throw UnexpectedError(L"Unexpected character lexer error constructed without the character argument.");
};

/*!
 * The constructor of LexerError, a variant with character.
 * \param type The type of the error.
 * \param character A character which caused the problem.
 * \param location The location to print.
 * \throw UnexpectedError If the type of the error has nothing to do with a character.
 */
LexerError::LexerError(LexerError::Type type, wchar_t character, const Location &location): ErrorWithLocation(location), type(type), character(character) {
	if(type!=LexerError::Type::UNEXPECTED_CHARACTER)
		throw UnexpectedError(L"Character argument provided for a different error that unexpected character.");
};

std::optional<std::wstring> LexerError::getTitle() const {
	return L"Lexer";
};

std::wstring LexerError::getMessage() const {
	switch(this->type) {
		case LexerError::Type::UNEXPECTED_END:
			return (L"Unexpected end of input "+this->printLocation());

		case LexerError::Type::UNEXPECTED_CHARACTER:
			return (
				L"Unexpected character "+
				Format::blue(std::wstring { this->character })+ //TODO Do not just print it if it’s e.g. newline
				L" "+this->printLocation()
			);

		case LexerError::Type::INVALID_MACHINE:
			return (L"Invalid machine "+this->printLocation());

		default:
			std::unreachable();
	};
};
