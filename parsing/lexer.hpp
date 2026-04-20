#pragma once
#include "../common.hpp"
#include "./token.hpp"
#include "../IO/warning.hpp"

class Lexer {
	private:
		size_t lineNumber = 1;
		std::vector<Token> tokens;
		std::wstring::const_iterator it, endIt;
		std::back_insert_iterator<std::vector<std::unique_ptr<Warning>>> warningIt;

		bool isAtEnd() const;
		wchar_t getCharacter() const;
		wchar_t readCharacter();
		size_t readNumber();
		void skipLine();
		bool isCharacterSpecial() const;
		std::variant<std::wstring, Token::Type> readIdentifierOrKeyword();
		Machine readMachine();
		bool readToken();

	public:
		Lexer(const std::wstring &text, const std::back_insert_iterator<std::vector<std::unique_ptr<Warning>>> &warningIt);
		std::vector<Token> analyze();
};
