#pragma once
#include "../common.hpp"
#include "./location.hpp"
#include "../machine/machine.hpp"

/*!
 * This class holds the type of a token together with its location in the sourced code.
 * In case of IDENTIFIER, NUMBER and MACHINE, it also holds the value of the token.
 */
class Token {
	public:
		/*!
		 * The types of tokens recognized by lexer.
		 * IDENTIFIER, NUMBER and MACHINE must have values, others are valueless.
		 */
		enum class Type {
			IDENTIFIER, NUMBER, MACHINE,
			SEMICOLON, COMMA, COLON, ELLIPSIS, ASSIGN,
			LEFT_PARENTHESIS, RIGHT_PARENTHESIS, LEFT_SQUARE_BRACKET, RIGHT_SQUARE_BRACKET,
			PLUS, MINUS, MUL, DIV, MOD, MIN, MAX, EQ, NE, LTE, LT, GTE, GT, NEG, AND, OR, XOR, INC, DEC, COUNT,
			IF, ELSE, ENDIF, WHILE, ENDWHILE, LOOP, ENDLOOP, BREAK, CONTINUE, EXIT, INCLUDE, REQUIRE,
			BEGINNING, END
		};

		static std::wstring typeToString(Token::Type type);

	private:
		Token::Type type;
		Location location;
		std::variant<size_t, std::wstring, Machine> value;

	public:
		Token(const Location &location, Token::Type type);
		Token(const Location &location, size_t value);
		Token(const Location &location, std::wstring value);
		Token(const Location &location, Machine value);
		Token::Type getType() const;
		size_t getNumberValue() const;
		const std::wstring &getIdentifierValue() const;
		const Machine &getMachineValue() const;
		const Location &getLocation() const;
};

/*!
 * This concept is used for pattern matching on tokens.
 * The type can either be directly Token::Type, or it can be a lvalue reference of a value type to match a IDENTIFIER/NUMBER/MACHINE token while capturing the value.
 */
template<typename T>
	concept Token_TypeOrValue = (std::same_as<T, Token::Type> || std::same_as<T, size_t&> || std::same_as<T, std::wstring&> || std::same_as<T, Machine&>);
