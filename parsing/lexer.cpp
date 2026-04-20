#include "./lexer.hpp"
#include "../IO/lexerError.hpp"
#include "../IO/generalWarning.hpp"

Lexer::Lexer(const std::wstring &text, const std::back_insert_iterator<std::vector<std::unique_ptr<Warning>>> &warningIt): it(text.begin()), endIt(text.end()), warningIt(warningIt) {};

bool Lexer::isAtEnd() const {
	return (this->it==this->endIt);
};

wchar_t Lexer::getCharacter() const {
	return (*this->it);
};

wchar_t Lexer::readCharacter() {
	return (*(this->it++));
};

size_t Lexer::readNumber() {
	size_t value = 0;

	while(!this->isAtEnd() && this->getCharacter() >= '0' && this->getCharacter() <= '9')
		value = (value * 10 + (this->readCharacter() - '0'));

	return value;
};

void Lexer::skipLine() {
	while(!this->isAtEnd()) {
		if(this->readCharacter()=='\n') {
			this->lineNumber++;

			break;
		};
	};
};

bool Lexer::isCharacterSpecial() const {
	switch(this->getCharacter()) {
		case ' ':
		case '\t':
		case '\n':
		case '#':
		case ';':
		case ',':
		case ':':
		case '.':
		case L'…':
		case '(':
		case ')':
		case '[':
		case ']':
		case '{':
		case '}':
		case '+':
		case L'−':
		case '-':
		case L'⋅':
		case L'×':
		case '*':
		case L'∶':
		case L'∕':
		case '/':
		case '%':
		case L'ρ':
		case '=':
		case '!':
		case L'≠':
		case '<':
		case '>':
		case L'≤':
		case L'≥':
		case L'¬':
		case L'∧':
		case L'∨':
			return true;

		default:
			return false;
	};
};

std::variant<std::wstring, Token::Type> Lexer::readIdentifierOrKeyword() {
	std::wstring value;

	while(!this->isAtEnd() && !this->isCharacterSpecial())
		value+= this->readCharacter();

	if(value==L"if")
		return Token::Type::IF;

	if(value==L"else")
		return Token::Type::ELSE;

	if(value==L"endif")
		return Token::Type::ENDIF;

	if(value==L"while")
		return Token::Type::WHILE;

	if(value==L"endwhile")
		return Token::Type::ENDWHILE;

	if(value==L"loop")
		return Token::Type::LOOP;

	if(value==L"endloop")
		return Token::Type::ENDLOOP;

	if(value==L"break")
		return Token::Type::BREAK;

	if(value==L"continue")
		return Token::Type::CONTINUE;

	if(value==L"exit")
		return Token::Type::EXIT;

	if(value==L"include")
		return Token::Type::INCLUDE;

	if(value==L"require")
		return Token::Type::REQUIRE;

	if(value==L"mod")
		return Token::Type::MOD;

	if(value==L"not")
		return Token::Type::NEG;

	if(value==L"and")
		return Token::Type::AND;

	if(value==L"or")
		return Token::Type::OR;

	if(value==L"xor")
		return Token::Type::XOR;

	if(value==L"min")
		return Token::Type::MIN;

	if(value==L"max")
		return Token::Type::MAX;

	if(value==L"count")
		return Token::Type::COUNT;

	return value;
};

Machine Lexer::readMachine() {
	size_t lineNumber;
	std::wstring machineString;
	std::wstring::const_iterator tokenStart;
	std::wistringstream iss;
	Machine machine;

	tokenStart = this->it;
	lineNumber = this->lineNumber;

	this->readCharacter(); // {
	while(!this->isAtEnd() && this->getCharacter()!='}') {
		if(this->getCharacter()=='\n')
			this->lineNumber++;
		machineString+= this->readCharacter();
	};

	if(this->isAtEnd())
		throw LexerError(LexerError::Type::UNEXPECTED_END, { { this->it, this->it }, this->lineNumber, this->tokens.front().getLocation().getWholeSource() });

	this->readCharacter(); // }

	iss.str(machineString);
	iss >> machine;

	if(iss.fail() || !machine)
		throw LexerError(LexerError::Type::INVALID_MACHINE, { { tokenStart, this->it }, lineNumber, this->tokens.front().getLocation().getWholeSource() });

	return machine;
};

bool Lexer::readToken() {
	size_t number, beginningLineNumber;
	std::wstring::const_iterator tokenStart;
	std::variant<std::wstring, Token::Type> identifierOrKeyword;
	Machine machine;

	while(!this->isAtEnd() && (this->getCharacter()==' ' || this->getCharacter()=='\t' || this->getCharacter()=='\n' || this->getCharacter()=='#')) {
		switch(this->readCharacter()) {
			case '\n':
				this->lineNumber++;

				break;

			case '#':
				this->skipLine();

				break;
		};
	};

	if(this->isAtEnd())
		return false;

	tokenStart = this->it;

	switch(this->getCharacter()) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			number = this->readNumber();
			this->tokens.push_back({ { { tokenStart, this->it }, this->lineNumber, this->tokens.front().getLocation().getWholeSource() }, number });

			return true;

		case ';':
			this->readCharacter();
			this->tokens.push_back({ { { tokenStart, this->it }, this->lineNumber, this->tokens.front().getLocation().getWholeSource() }, Token::Type::SEMICOLON, });

			return true;

		case ',':
			this->readCharacter();
			this->tokens.push_back({ { { tokenStart, this->it }, this->lineNumber, this->tokens.front().getLocation().getWholeSource() }, Token::Type::COMMA });

			return true;

		case ':':
			this->readCharacter();
			this->tokens.push_back({ { { tokenStart, this->it }, this->lineNumber, this->tokens.front().getLocation().getWholeSource() }, Token::Type::COLON });

			return true;

		case L'…':
			this->readCharacter();
			this->tokens.push_back({ { { tokenStart, this->it }, this->lineNumber, this->tokens.front().getLocation().getWholeSource() }, Token::Type::ELLIPSIS });

			return true;

		case '.':
			this->readCharacter();

			if(this->isAtEnd())
				throw LexerError(LexerError::Type::UNEXPECTED_END, { { this->it, this->it }, this->lineNumber, this->tokens.front().getLocation().getWholeSource() });

			if(this->getCharacter()!='.')
				throw LexerError(LexerError::Type::UNEXPECTED_CHARACTER, '.', { { tokenStart, this->it }, this->lineNumber, this->tokens.front().getLocation().getWholeSource() });
			else
				this->readCharacter();

			if(this->isAtEnd())
				throw LexerError(LexerError::Type::UNEXPECTED_END, { { this->it, this->it }, this->lineNumber, this->tokens.front().getLocation().getWholeSource() });

			if(this->getCharacter()!='.')
				throw LexerError(LexerError::Type::UNEXPECTED_CHARACTER, '.', { { tokenStart, this->it }, this->lineNumber, this->tokens.front().getLocation().getWholeSource() });
			else
				this->readCharacter();

			this->tokens.push_back({ { { tokenStart, this->it }, this->lineNumber, this->tokens.front().getLocation().getWholeSource() }, Token::Type::ELLIPSIS });

			return true;

		case '(':
			this->readCharacter();
			this->tokens.push_back({ { { tokenStart, this->it }, this->lineNumber, this->tokens.front().getLocation().getWholeSource() }, Token::Type::LEFT_PARENTHESIS });

			return true;

		case ')':
			this->readCharacter();
			this->tokens.push_back({ { { tokenStart, this->it }, this->lineNumber, this->tokens.front().getLocation().getWholeSource() }, Token::Type::RIGHT_PARENTHESIS });

			return true;

		case '[':
			this->readCharacter();
			this->tokens.push_back({ { { tokenStart, this->it }, this->lineNumber, this->tokens.front().getLocation().getWholeSource() }, Token::Type::LEFT_SQUARE_BRACKET });

			return true;

		case ']':
			this->readCharacter();
			this->tokens.push_back({ { { tokenStart, this->it }, this->lineNumber, this->tokens.front().getLocation().getWholeSource() }, Token::Type::RIGHT_SQUARE_BRACKET });

			return true;

		case '+':
			this->readCharacter();
			if(!this->isAtEnd() && this->getCharacter()=='+') {
				this->readCharacter();
				this->tokens.push_back({ { { tokenStart, this->it }, this->lineNumber, this->tokens.front().getLocation().getWholeSource() }, Token::Type::INC });
			}
			else
				this->tokens.push_back({ { { tokenStart, this->it }, this->lineNumber, this->tokens.front().getLocation().getWholeSource() }, Token::Type::PLUS });

			return true;

		case L'−':
			this->readCharacter();
			if(!this->isAtEnd() && this->getCharacter()==L'−') {
				this->readCharacter();
				this->tokens.push_back({ { { tokenStart, this->it }, this->lineNumber, this->tokens.front().getLocation().getWholeSource() }, Token::Type::DEC });
			}
			else
				this->tokens.push_back({ { { tokenStart, this->it }, this->lineNumber, this->tokens.front().getLocation().getWholeSource() }, Token::Type::MINUS });

			return true;

		case '-':
			this->readCharacter();
			if(!this->isAtEnd() && this->getCharacter()=='-') {
				this->readCharacter();
				this->tokens.push_back({ { { tokenStart, this->it }, this->lineNumber, this->tokens.front().getLocation().getWholeSource() }, Token::Type::DEC });
			}
			else
				this->tokens.push_back({ { { tokenStart, this->it }, this->lineNumber, this->tokens.front().getLocation().getWholeSource() }, Token::Type::MINUS });

			return true;

		case L'⋅':
		case L'×':
		case '*':
			this->readCharacter();
			this->tokens.push_back({ { { tokenStart, this->it }, this->lineNumber, this->tokens.front().getLocation().getWholeSource() }, Token::Type::MUL });

			return true;

		case L'∶':
		case L'∕':
		case '/':
			this->readCharacter();
			this->tokens.push_back({ { { tokenStart, this->it }, this->lineNumber, this->tokens.front().getLocation().getWholeSource() }, Token::Type::DIV });

			return true;

		case '%':
			this->readCharacter();
			this->tokens.push_back({ { { tokenStart, this->it }, this->lineNumber, this->tokens.front().getLocation().getWholeSource() }, Token::Type::MOD });

			return true;

		case L'ρ':
			this->readCharacter();
			this->tokens.push_back({ { { tokenStart, this->it }, this->lineNumber, this->tokens.front().getLocation().getWholeSource() }, Token::Type::COUNT });

			return true;

		case '=':
			this->readCharacter();
			if(!this->isAtEnd() && this->getCharacter()=='=') {
				this->readCharacter();
				this->tokens.push_back({ { { tokenStart, this->it }, this->lineNumber, this->tokens.front().getLocation().getWholeSource() }, Token::Type::EQ });
			}
			else
				this->tokens.push_back({ { { tokenStart, this->it }, this->lineNumber, this->tokens.front().getLocation().getWholeSource() }, Token::Type::ASSIGN });

			return true;

		case L'≠':
			this->readCharacter();
			this->tokens.push_back({ { { tokenStart, this->it }, this->lineNumber, this->tokens.front().getLocation().getWholeSource() }, Token::Type::NE });

			return true;

		case '<':
			this->readCharacter();
			if(!this->isAtEnd() && this->getCharacter()=='=') {
				this->readCharacter();
				this->tokens.push_back({ { { tokenStart, this->it }, this->lineNumber, this->tokens.front().getLocation().getWholeSource() }, Token::Type::LTE });
			}
			else
				this->tokens.push_back({ { { tokenStart, this->it }, this->lineNumber, this->tokens.front().getLocation().getWholeSource() }, Token::Type::LT });

			return true;

		case L'≤':
			this->readCharacter();
			this->tokens.push_back({ { { tokenStart, this->it }, this->lineNumber, this->tokens.front().getLocation().getWholeSource() }, Token::Type::LTE });

			return true;

		case '>':
			this->readCharacter();
			if(!this->isAtEnd() && this->getCharacter()=='=') {
				this->readCharacter();
				this->tokens.push_back({ { { tokenStart, this->it }, this->lineNumber, this->tokens.front().getLocation().getWholeSource() }, Token::Type::GTE });
			}
			else
				this->tokens.push_back({ { { tokenStart, this->it }, this->lineNumber, this->tokens.front().getLocation().getWholeSource() }, Token::Type::GT });

			return true;

		case L'≥':
			this->readCharacter();
			this->tokens.push_back({ { { tokenStart, this->it }, this->lineNumber, this->tokens.front().getLocation().getWholeSource() }, Token::Type::GTE });

			return true;

		case '!':
			this->readCharacter();
			if(!this->isAtEnd() && this->getCharacter()=='=') {
				this->readCharacter();
				this->tokens.push_back({ { { tokenStart, this->it }, this->lineNumber, this->tokens.front().getLocation().getWholeSource() }, Token::Type::NE });
			}
			else
				this->tokens.push_back({ { { tokenStart, this->it }, this->lineNumber, this->tokens.front().getLocation().getWholeSource() }, Token::Type::NEG });

			return true;

		case L'¬':
			this->readCharacter();
			this->tokens.push_back({ { { tokenStart, this->it }, this->lineNumber, this->tokens.front().getLocation().getWholeSource() }, Token::Type::NEG });

			return true;

		case L'∧':
			this->readCharacter();
			this->tokens.push_back({ { { tokenStart, this->it }, this->lineNumber, this->tokens.front().getLocation().getWholeSource() }, Token::Type::AND });

			return true;

		case L'∨':
			this->readCharacter();
			this->tokens.push_back({ { { tokenStart, this->it }, this->lineNumber, this->tokens.front().getLocation().getWholeSource() }, Token::Type::OR });

			return true;

		case L'&':
			this->readCharacter();
			if(!this->isAtEnd() && this->getCharacter()=='&') {
				this->readCharacter();
				this->tokens.push_back({ { { tokenStart, this->it }, this->lineNumber, this->tokens.front().getLocation().getWholeSource() }, Token::Type::AND });
			}
			else
				this->tokens.push_back({ { { tokenStart, this->it }, this->lineNumber, this->tokens.front().getLocation().getWholeSource() }, Token::Type::MIN });

			return true;

		case L'|':
			this->readCharacter();
			if(!this->isAtEnd() && this->getCharacter()=='|') {
				this->readCharacter();
				this->tokens.push_back({ { { tokenStart, this->it }, this->lineNumber, this->tokens.front().getLocation().getWholeSource() }, Token::Type::OR });
			}
			else
				this->tokens.push_back({ { { tokenStart, this->it }, this->lineNumber, this->tokens.front().getLocation().getWholeSource() }, Token::Type::MAX });

			return true;

		case '{':
			beginningLineNumber = this->lineNumber;
			machine = this->readMachine();
			this->tokens.push_back({ { { tokenStart, this->it }, beginningLineNumber, this->tokens.front().getLocation().getWholeSource() }, std::move(machine) });

			return true;

		default:
			identifierOrKeyword = this->readIdentifierOrKeyword();
			if(std::holds_alternative<Token::Type>(identifierOrKeyword)) // Keyword
				this->tokens.push_back({ { { tokenStart, this->it }, this->lineNumber, this->tokens.front().getLocation().getWholeSource() }, std::get<Token::Type>(identifierOrKeyword) });
			else // Identifier
				this->tokens.push_back({ { { tokenStart, this->it }, this->lineNumber, this->tokens.front().getLocation().getWholeSource() }, std::get<std::wstring>(identifierOrKeyword) });

			return true;
	};
};

std::vector<Token> Lexer::analyze() {
	if(this->isAtEnd()) {
		this->warningIt = std::make_unique<GeneralWarning>(L"The source is empty.");

		return { { { { this->it, this->it }, 1, { this->it, this->endIt } }, Token::Type::BEGINNING }, { { { this->it, this->it }, 1, { this->it, this->endIt } }, Token::Type::END } };
	};

	this->tokens.push_back({ { { this->it, this->it }, 1, { this->it, this->endIt } }, Token::Type::BEGINNING });

	while(this->readToken());

	this->tokens.push_back({ { { this->it, this->it }, this->lineNumber, this->tokens.front().getLocation().getWholeSource() }, Token::Type::END });

	return std::move(this->tokens);
};
