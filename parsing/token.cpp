#include "./token.hpp"
#include "../IO/unexpectedError.hpp"

/*!
 * \param A token type.
 * \return The string representation of the type (uppercase with underscores).
 */
std::wstring Token::typeToString(Token::Type type) {
	switch(type) {
		case Token::Type::IDENTIFIER:
			return L"IDENTIFIER";

		case Token::Type::NUMBER:
			return L"NUMBER";

		case Token::Type::MACHINE:
			return L"MACHINE";

		case Token::Type::SEMICOLON:
			return L"SEMICOLON";

		case Token::Type::COMMA:
			return L"COMMA";

		case Token::Type::COLON:
			return L"COLON";

		case Token::Type::ELLIPSIS:
			return L"ELLIPSIS";

		case Token::Type::ASSIGN:
			return L"ASSIGN";

		case Token::Type::LEFT_PARENTHESIS:
			return L"LEFT_PARENTHESIS";

		case Token::Type::RIGHT_PARENTHESIS:
			return L"RIGHT_PARENTHESIS";

		case Token::Type::LEFT_SQUARE_BRACKET:
			return L"LEFT_SQUARE_BRACKET";

		case Token::Type::RIGHT_SQUARE_BRACKET:
			return L"RIGHT_SQUARE_BRACKET";

		case Token::Type::PLUS:
			return L"PLUS";

		case Token::Type::MINUS:
			return L"MINUS";

		case Token::Type::MUL:
			return L"MUL";

		case Token::Type::DIV:
			return L"DIV";

		case Token::Type::MOD:
			return L"MOD";

		case Token::Type::MIN:
			return L"MIN";

		case Token::Type::MAX:
			return L"MAX";

		case Token::Type::EQ:
			return L"EQ";

		case Token::Type::NE:
			return L"NE";

		case Token::Type::LTE:
			return L"LTE";

		case Token::Type::LT:
			return L"LT";

		case Token::Type::GTE:
			return L"GTE";

		case Token::Type::GT:
			return L"GT";

		case Token::Type::NEG:
			return L"NEG";

		case Token::Type::AND:
			return L"AND";

		case Token::Type::OR:
			return L"OR";

		case Token::Type::XOR:
			return L"XOR";

		case Token::Type::INC:
			return L"INC";

		case Token::Type::DEC:
			return L"DEC";

		case Token::Type::COUNT:
			return L"COUNT";

		case Token::Type::IF:
			return L"IF";

		case Token::Type::ELSE:
			return L"ELSE";

		case Token::Type::ENDIF:
			return L"ENDIF";

		case Token::Type::WHILE:
			return L"WHILE";

		case Token::Type::ENDWHILE:
			return L"ENDWHILE";

		case Token::Type::LOOP:
			return L"LOOP";

		case Token::Type::ENDLOOP:
			return L"ENDLOOP";

		case Token::Type::BREAK:
			return L"BREAK";

		case Token::Type::CONTINUE:
			return L"CONTINUE";

		case Token::Type::EXIT:
			return L"EXIT";

		case Token::Type::INCLUDE:
			return L"INCLUDE";

		case Token::Type::REQUIRE:
			return L"REQUIRE";

		case Token::Type::BEGINNING:
			return L"BEGINNING";

		case Token::Type::END:
			return L"END";

		default:
			std::unreachable();
	};
};

/*!
 * The constructor of a valueless Token.
 * \param location The location of the token in code.
 * \param type The type of the token.
 * \throw UnexpectedError If the token is one of those that need value (IDENTIFIER, NUMBER or MACHINE).
 * \throw UnexpectedError If the condition that the location is empty if and only if the token is either BEGINNING or END is not satisfied.
 */
Token::Token(const Location &location, Token::Type type): type(type), location(location) {
	if(type==Token::Type::IDENTIFIER || type==Token::Type::NUMBER || type==Token::Type::MACHINE)
		throw UnexpectedError(L"Invalid token (missing required value).");

	if(location.isEmpty() xor (type==Token::Type::BEGINNING || type==Token::Type::END))
		throw UnexpectedError(L"Invalid token (location should be missing if and only if token type is BEGINNING or END).");
};

/*!
 * The constructor of a Token of type NUMBER.
 * \param location The location of the token in code.
 * \param value The value of the token.
 * \throw UnexpectedError If the location is empty.
 */
Token::Token(const Location &location, size_t value): type(Token::Type::NUMBER), location(location), value(value) {
	if(location.isEmpty())
		throw UnexpectedError(L"Invalid token (location should be missing if and only if token type is BEGINNING or END).");
};

/*!
 * The constructor of a Token of type IDENTIFIER.
 * \param location The location of the token in code.
 * \param value The value of the token.
 * \throw UnexpectedError If the location is empty.
 */
Token::Token(const Location &location, std::wstring value): type(Token::Type::IDENTIFIER), location(location), value(std::move(value)) {
	if(location.isEmpty())
		throw UnexpectedError(L"Invalid token (location should be missing if and only if token type is BEGINNING or END).");
};

/*!
 * The constructor of a Token of type MACHINE.
 * \param location The location of the token in code.
 * \param value The value of the token.
 * \throw UnexpectedError If the location is empty.
 */
Token::Token(const Location &location, Machine value): type(Token::Type::MACHINE), location(location), value(std::move(value)) {
	if(location.isEmpty())
		throw UnexpectedError(L"Invalid token (location should be missing if and only if token type is BEGINNING or END).");
};

/*!
 * \return The type of the token.
 */
Token::Type Token::getType() const {
	return this->type;
};

/*!
 * \return The value of a NUMBER token.
 * \throw UnexpectedError If the token is not a NUMBER token.
 */
size_t Token::getNumberValue() const {
	if(this->type!=Token::Type::NUMBER)
		throw UnexpectedError(L"Invalid access to a token (not NUMBER).");

	return std::get<size_t>(this->value);
};

/*!
 * \return The value of a IDENTIFIER token.
 * \throw UnexpectedError If the token is not a IDENTIFIER token.
 */
const std::wstring &Token::getIdentifierValue() const {
	if(this->type!=Token::Type::IDENTIFIER)
		throw UnexpectedError(L"Invalid access to a token (not IDENTIFIER).");

	return std::get<std::wstring>(this->value);
};

/*!
 * \return The value of a MACHINE token.
 * \throw UnexpectedError If the token is not a MACHINE token.
 */
const Machine &Token::getMachineValue() const {
	if(this->type!=Token::Type::MACHINE)
		throw UnexpectedError(L"Invalid access to a token (not MACHINE).");

	return std::get<Machine>(this->value);
};

/*!
 * \return The token location in code.
 */
const Location &Token::getLocation() const {
	return this->location;
};
