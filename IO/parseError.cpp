#include "./parseError.hpp"
#include "./format.hpp"

/*!
 * The constructor of ParseError.
 * \param type The type of the error.
 * \param token The token where the error occurred, used for printing location (and saying what type of token was unexpected).
 */
ParseError::ParseError(ParseError::Type type, std::vector<Token>::const_iterator token): ErrorWithLocation(token->getLocation()), type(type), tokenType(token->getType()) {};

std::optional<std::wstring> ParseError::getTitle() const {
	return L"Parse";
};

std::wstring ParseError::getMessage() const {
	switch(this->type) {
		case ParseError::Type::UNEXPECTED_TOKEN:
			return (L"Unexpected token "+Format::blue(Token::typeToString(this->tokenType))+L" "+this->printLocation());

		case ParseError::Type::STRAY_BREAK:
			return (L"A break statement was found outside of a loop "+this->printLocation());

		case ParseError::Type::STRAY_CONTINUE:
			return (L"A continue statement was found outside of a loop "+this->printLocation());

		default:
			std::unreachable();
	};
};
