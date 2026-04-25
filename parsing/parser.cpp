#include "./parser.hpp"
#include "./location.hpp"
#include "../AST/expressions/numberExpression.hpp"
#include "../AST/expressions/variableExpression.hpp"
#include "../AST/expressions/arrayAccessExpression.hpp"
#include "../AST/expressions/unaryExpression.hpp"
#include "../AST/expressions/binaryExpression.hpp"
#include "../AST/expressions/countExpression.hpp"
#include "../AST/expressions/callExpression.hpp"
#include "../AST/statements/copyStatement.hpp"
#include "../AST/statements/implodeStatement.hpp"
#include "../AST/statements/explodeStatement.hpp"
#include "../AST/statements/callStatement.hpp"
#include "../AST/statements/ifStatement.hpp"
#include "../AST/statements/whileStatement.hpp"
#include "../AST/statements/loopStatement.hpp"
#include "../AST/statements/breakStatement.hpp"
#include "../AST/statements/continueStatement.hpp"
#include "../AST/statements/exitStatement.hpp"
#include "../machine/machineLibrary.hpp"
#include "../IO/parseError.hpp"
#include "../IO/generalWarning.hpp"
#include "../IO/format.hpp"

/*!
 * The constructor of Parser.
 * \param token The vector of tokens from the lexical analysis of the input.
 * \param includeResolver An instance of IncludeResolver to use for resolving include and require statements.
 * \param warningIt The destination where to append warnings, if necessary.
 */
Parser::Parser(std::vector<Token> tokens, IncludeResolver includeResolver, const std::back_insert_iterator<std::vector<std::unique_ptr<Warning>>> &warningIt): it(tokens.begin()), warningIt(warningIt), includeResolver(std::move(includeResolver)) {};

/*!
 * \return The type of the next (current) token.
 */
Token::Type Parser::getNextTokenType() const {
	return this->it->getType();
};

/*!
 * Read the current token and increment the token iterator.
 * \return The current token.
 */
const Token &Parser::readToken() {
	return (*(this->it++));
};

/*!
 * Expect a number, throw UNEXPECTED_TOKEN if it is not a number, increment the token iterator.
 * \return A value of the current token, if it is a NUMBER.
 * \throw ParseError If it is not a NUMBER.
 */
size_t Parser::readNumberToken() {
	if(this->getNextTokenType()!=Token::Type::NUMBER)
		throw ParseError(ParseError::Type::UNEXPECTED_TOKEN, this->it);

	return this->readToken().getNumberValue();
};

/*!
 * Expect an identifier, throw UNEXPECTED_TOKEN if it is not an identifier, increment the token iterator.
 * \return A value of the current token, if it is an IDENTIFIER.
 * \throw ParseError If it is not an IDENTIFIER.
 */
const std::wstring &Parser::readIdentifierToken() {
	if(this->getNextTokenType()!=Token::Type::IDENTIFIER)
		throw ParseError(ParseError::Type::UNEXPECTED_TOKEN, this->it);

	return this->readToken().getIdentifierValue();
};

/*!
 * Expect a machine, throw UNEXPECTED_TOKEN if it is not a machine, increment the token iterator.
 * \return A value of the current token, if it is an MACHINE.
 * \throw ParseError If it is not an MACHINE.
 */
const Machine &Parser::readMachineToken() {
	if(this->getNextTokenType()!=Token::Type::MACHINE)
		throw ParseError(ParseError::Type::UNEXPECTED_TOKEN, this->it);

	return this->readToken().getMachineValue();
};

/*!
 * Expect some number of token types, starting from the current position.
 * If one of the tokens is not what is expected, throw UNEXPECTED_TOKEN.
 * The arguments can be either references to capture a token values (see T) or a Token::Type to assert a specific type without capturing (used for valueless types).
 * This is the variant without capturing with the first argument.
 * \tparam T size_t& for NUMBER, std::wstring& for IDENTIFIER, Machine& for MACHINE, Token::Type for any type (especially valueless).
 * \param type The first expected token.
 * \param rest The remaining expected tokens.
 */
template<Token_TypeOrValue... T>
	void Parser::expect(Token::Type type, T &&... rest) {
		if(this->getNextTokenType()!=type)
			throw::ParseError(ParseError::Type::UNEXPECTED_TOKEN, this->it);

		this->readToken();
		this->expect(std::forward<T>(rest)...);
	};

/*!
 * Expect some number of token types, starting from the current position.
 * If one of the tokens is not what is expected, throw UNEXPECTED_TOKEN.
 * The arguments can be either references to capture a token values (see T) or a Token::Type to assert a specific type without capturing (used for valueless types).
 * This is the variant for capturing a number with the first argument.
 * \tparam T size_t& for NUMBER, std::wstring& for IDENTIFIER, Machine& for MACHINE, Token::Type for any type (especially valueless).
 * \param type The first expected token.
 * \param rest The remaining expected tokens.
 */
template<Token_TypeOrValue... T>
	void Parser::expect(size_t &number, T &&... rest) {
		number = this->readNumberToken();
		this->expect(std::forward<T>(rest)...);
	};

/*!
 * Expect some number of token types, starting from the current position.
 * If one of the tokens is not what is expected, throw UNEXPECTED_TOKEN.
 * The arguments can be either references to capture a token values (see T) or a Token::Type to assert a specific type without capturing (used for valueless types).
 * This is the variant for capturing an identifier with the first argument.
 * \tparam T size_t& for NUMBER, std::wstring& for IDENTIFIER, Machine& for MACHINE, Token::Type for any type (especially valueless).
 * \param type The first expected token.
 * \param rest The remaining expected tokens.
 */
template<Token_TypeOrValue... T>
	void Parser::expect(std::wstring &identifier, T &&... rest) {
		identifier = this->readIdentifierToken();
		this->expect(std::forward<T>(rest)...);
	};

/*!
 * Expect some number of token types, starting from the current position.
 * If one of the tokens is not what is expected, throw UNEXPECTED_TOKEN.
 * The arguments can be either references to capture a token values (see T) or a Token::Type to assert a specific type without capturing (used for valueless types).
 * This is the variant for capturing a machine with the first argument.
 * \tparam T size_t& for NUMBER, std::wstring& for IDENTIFIER, Machine& for MACHINE, Token::Type for any type (especially valueless).
 * \param type The first expected token.
 * \param rest The remaining expected tokens.
 */
template<Token_TypeOrValue... T>
	void Parser::expect(Machine &machine, T &&... rest) {
		machine = this->readMachineToken();
		this->expect(std::forward<T>(rest)...);
	};

/*!
 * An empty variant of expect() as a base case for the templated variants above.
 */
void Parser::expect() {};

/*!
 * \return The location of the token before the current one.
 */
const Location &Parser::getLastTokenLocation() const {
	return std::prev(this->it)->getLocation();
};

void Parser::parseProgram() {
	this->program.addStatements(this->parseStatements());
};

std::vector<std::unique_ptr<Statement>> Parser::parseStatements() {
	std::unique_ptr<Statement> statement;
	std::vector<std::unique_ptr<Statement>> statements;

	while(true) {
		switch(this->getNextTokenType()) {
			case Token::Type::END:
			case Token::Type::ELSE:
			case Token::Type::ENDIF:
			case Token::Type::ENDWHILE:
			case Token::Type::ENDLOOP:
				return statements;

			case Token::Type::LEFT_SQUARE_BRACKET:
			case Token::Type::IDENTIFIER:
			case Token::Type::IF:
			case Token::Type::WHILE:
			case Token::Type::LOOP:
			case Token::Type::BREAK:
			case Token::Type::CONTINUE:
			case Token::Type::EXIT:
			case Token::Type::INCLUDE:
			case Token::Type::REQUIRE:
				statement = this->parseStatement();
				if(statement)
					statements.push_back(std::move(statement));
				this->expect(Token::Type::SEMICOLON);

				break;

			default:
				throw ParseError(ParseError::Type::UNEXPECTED_TOKEN, this->it);
		};
	};
};

std::unique_ptr<Statement> Parser::parseStatement() {
	switch(this->getNextTokenType()) {
		case Token::Type::LEFT_SQUARE_BRACKET:
		case Token::Type::IDENTIFIER:
			return this->parseAssignmentOrSimpleCall();

		case Token::Type::IF:
			return this->parseIfStatement();

		case Token::Type::WHILE:
			return this->parseWhileStatement();

		case Token::Type::LOOP:
			return this->parseLoopStatement();

		case Token::Type::BREAK:
			if(!this->isInLoop)
				throw ParseError(ParseError::Type::STRAY_BREAK, this->it);

			this->expect(Token::Type::BREAK);

			return std::make_unique<BreakStatement>();

		case Token::Type::CONTINUE:
			if(!this->isInLoop)
				throw ParseError(ParseError::Type::STRAY_CONTINUE, this->it);

			this->expect(Token::Type::CONTINUE);

			return std::make_unique<ContinueStatement>();

		case Token::Type::EXIT:
			this->expect(Token::Type::EXIT);

			return std::make_unique<ExitStatement>();

		case Token::Type::INCLUDE:
			this->parseIncludeStatement();

			return {};

		case Token::Type::REQUIRE:
			this->parseRequireStatement();

			return {};

		default:
			throw ParseError(ParseError::Type::UNEXPECTED_TOKEN, this->it);
	};
};

std::unique_ptr<Statement> Parser::parseAssignmentOrSimpleCall() {
	std::wstring identifier;
	std::vector<Token>::const_iterator itA, itB;
	DestinationBundle destinationBundle;
	std::unique_ptr<Expression> expression;

	switch(this->getNextTokenType()) {
		case Token::Type::LEFT_SQUARE_BRACKET:
			itA = this->it;
			destinationBundle = this->parseDestinationBundle();
			itB = std::prev(this->it);
			this->expect(Token::Type::ASSIGN);
			expression = this->parseLogicalExpression();

			return std::make_unique<ExplodeStatement>(std::move(expression), std::move(destinationBundle), Location(itA->getLocation(), itB->getLocation()));

		case Token::Type::IDENTIFIER:
			this->expect(identifier);

			return this->parseRemainingAssignmentOrSimpleCall(std::move(identifier), this->getLastTokenLocation());

		default:
			throw ParseError(ParseError::Type::UNEXPECTED_TOKEN, this->it);
	};
};

std::unique_ptr<Statement> Parser::parseRemainingAssignmentOrSimpleCall(std::wstring identifier, Location identifierLocation) {
	std::wstring argument;
	std::vector<Token>::const_iterator itA, itB;
	std::variant<std::unique_ptr<Expression>, SourceBundle, Machine> expressionOrSourceBundleOrMachine;

	switch(this->getNextTokenType()) {
		case Token::Type::ASSIGN:
			this->expect(Token::Type::ASSIGN);
			itA = this->it;
			expressionOrSourceBundleOrMachine = this->parseLogicalExpressionOrSourceBundleOrMachine();
			itB = std::prev(this->it);

			if(std::holds_alternative<std::unique_ptr<Expression>>(expressionOrSourceBundleOrMachine))
				return std::make_unique<CopyStatement>(std::move(std::get<std::unique_ptr<Expression>>(expressionOrSourceBundleOrMachine)), this->program.findOrAddVariable(identifier), 0);

			if(std::holds_alternative<SourceBundle>(expressionOrSourceBundleOrMachine))
				return std::make_unique<ImplodeStatement>(std::move(std::get<SourceBundle>(expressionOrSourceBundleOrMachine)), this->program.findOrAddVariable(identifier), 0, Location(itA->getLocation(), itB->getLocation()));

			if(std::holds_alternative<Machine>(expressionOrSourceBundleOrMachine)) {
				this->program.addMachine(identifier, std::move(std::get<Machine>(expressionOrSourceBundleOrMachine)), identifierLocation);

				if(this->isInBranch || this->isInLoop)
					this->warningIt = std::make_unique<GeneralWarning>(L"Machine "+Format::blue(identifier)+L" defined inside an if branch or a while loop. Machines are always visible globally after definition.");

				return {};
			};

			std::unreachable();

		case Token::Type::LEFT_SQUARE_BRACKET:
			this->expect(Token::Type::LEFT_SQUARE_BRACKET);

			return this->parseRemainingArrayAccessAssignment(std::move(identifier));

		case Token::Type::LEFT_PARENTHESIS:
			this->expect(Token::Type::LEFT_PARENTHESIS, argument, Token::Type::ELLIPSIS, Token::Type::RIGHT_PARENTHESIS);

			return std::make_unique<CallStatement>(this->program.findMachine(identifier, identifierLocation), this->program.findOrAddVariable(argument));

		case Token::Type::INC:
			this->expect(Token::Type::INC);

			return std::make_unique<CallStatement>(MachineLibrary::INC, this->program.findOrAddVariable(identifier)); //TODO maybe make a special statement

		case Token::Type::DEC:
			this->expect(Token::Type::DEC);

			return std::make_unique<CallStatement>(MachineLibrary::DEC, this->program.findOrAddVariable(identifier)); //TODO maybe make a special statement

		default:
			throw ParseError(ParseError::Type::UNEXPECTED_TOKEN, this->it);
	};
};

std::unique_ptr<Statement> Parser::parseRemainingArrayAccessAssignment(std::wstring identifier) {
	size_t number;
	std::vector<Token>::const_iterator itA, itB;
	std::variant<std::unique_ptr<Expression>, SourceBundle> expressionOrSourceBundle;

	switch(this->getNextTokenType()) {
		case Token::Type::RIGHT_SQUARE_BRACKET:
			this->expect(Token::Type::RIGHT_SQUARE_BRACKET, Token::Type::ASSIGN);
			itA = this->it;
			expressionOrSourceBundle = this->parseLogicalExpressionOrSourceBundle();
			itB = std::prev(this->it);

			if(std::holds_alternative<std::unique_ptr<Expression>>(expressionOrSourceBundle))
				return std::make_unique<CopyStatement>(std::move(std::get<std::unique_ptr<Expression>>(expressionOrSourceBundle)), this->program.findOrAddVariable(identifier), std::nullopt);

			if(std::holds_alternative<SourceBundle>(expressionOrSourceBundle))
				return std::make_unique<ImplodeStatement>(std::move(std::get<SourceBundle>(expressionOrSourceBundle)), this->program.findOrAddVariable(identifier), std::nullopt, Location(itA->getLocation(), itB->getLocation()));

			std::unreachable();

		case Token::Type::NUMBER:
			this->expect(number, Token::Type::COLON, Token::Type::RIGHT_SQUARE_BRACKET, Token::Type::ASSIGN);
			itA = this->it;
			expressionOrSourceBundle = this->parseLogicalExpressionOrSourceBundle();
			itB = std::prev(this->it);

			if(std::holds_alternative<std::unique_ptr<Expression>>(expressionOrSourceBundle))
				return std::make_unique<CopyStatement>(std::move(std::get<std::unique_ptr<Expression>>(expressionOrSourceBundle)), this->program.findOrAddVariable(identifier), number);

			if(std::holds_alternative<SourceBundle>(expressionOrSourceBundle))
				return std::make_unique<ImplodeStatement>(std::move(std::get<SourceBundle>(expressionOrSourceBundle)), this->program.findOrAddVariable(identifier), number, Location(itA->getLocation(), itB->getLocation()));

			std::unreachable();

		default:
			throw ParseError(ParseError::Type::UNEXPECTED_TOKEN, this->it);
	};
};

DestinationBundle Parser::parseDestinationBundle() {
	bool hasEllipsis;
	std::wstring identifier;
	std::vector<Token>::const_iterator itA, itB;
	DestinationBundle destinationBundle;

	this->expect(Token::Type::LEFT_SQUARE_BRACKET, identifier);
	itA = std::prev(this->it);
	hasEllipsis = this->parseOptionalEllipsis();
	itB = std::prev(this->it);
	destinationBundle.add(this->program.findOrAddVariable(identifier), hasEllipsis, Location(itA->getLocation(), itB->getLocation()));
	this->parseRemainingDestinationBundle(destinationBundle);
	this->expect(Token::Type::RIGHT_SQUARE_BRACKET);

	return destinationBundle;
};

void Parser::parseRemainingDestinationBundle(DestinationBundle &destinationBundle) {
	bool hasEllipsis;
	std::wstring identifier;
	std::vector<Token>::const_iterator itA, itB;

	while(true) {
		switch(this->getNextTokenType()) {
			case Token::Type::RIGHT_SQUARE_BRACKET:
				return;

			case Token::Type::COMMA:
				this->expect(Token::Type::COMMA, identifier);
				itA = std::prev(this->it);
				hasEllipsis = this->parseOptionalEllipsis();
				itB = std::prev(this->it);
				destinationBundle.add(this->program.findOrAddVariable(identifier), hasEllipsis, Location(itA->getLocation(), itB->getLocation()));

				break;

			default:
				throw ParseError(ParseError::Type::UNEXPECTED_TOKEN, this->it);
		};
	};
};

bool Parser::parseOptionalEllipsis() {
	switch(this->getNextTokenType()) {
		case Token::Type::COMMA:
		case Token::Type::RIGHT_SQUARE_BRACKET:
			return false;

		case Token::Type::ELLIPSIS:
			this->expect(Token::Type::ELLIPSIS);

			return true;

		default:
			throw ParseError(ParseError::Type::UNEXPECTED_TOKEN, this->it);
	};
};

std::variant<std::unique_ptr<Expression>, SourceBundle, Machine> Parser::parseLogicalExpressionOrSourceBundleOrMachine() {
	Machine machine;

	switch(this->getNextTokenType()) {
		case Token::Type::NEG:
		case Token::Type::IDENTIFIER:
		case Token::Type::LEFT_PARENTHESIS:
		case Token::Type::NUMBER:
		case Token::Type::COUNT:
			return this->parseLogicalExpression();

		case Token::Type::LEFT_SQUARE_BRACKET:
			return this->parseSourceBundle();

		case Token::Type::MACHINE:
			this->expect(machine);

			return machine;

		default:
			throw ParseError(ParseError::Type::UNEXPECTED_TOKEN, this->it);
	};
};

std::variant<std::unique_ptr<Expression>, SourceBundle> Parser::parseLogicalExpressionOrSourceBundle() {
	switch(this->getNextTokenType()) {
		case Token::Type::NEG:
		case Token::Type::IDENTIFIER:
		case Token::Type::LEFT_PARENTHESIS:
		case Token::Type::NUMBER:
		case Token::Type::COUNT:
			return this->parseLogicalExpression();

		case Token::Type::LEFT_SQUARE_BRACKET:
			return this->parseSourceBundle();

		default:
			throw ParseError(ParseError::Type::UNEXPECTED_TOKEN, this->it);
	};
};

SourceBundle Parser::parseSourceBundle() {
	SourceBundle sourceBundle;

	this->expect(Token::Type::LEFT_SQUARE_BRACKET);
	this->parseSourceBundleContent(sourceBundle);
	this->expect(Token::Type::RIGHT_SQUARE_BRACKET);

	return sourceBundle;
};

void Parser::parseSourceBundleContent(SourceBundle &sourceBundle) {
	switch(this->getNextTokenType()) {
		case Token::Type::RIGHT_SQUARE_BRACKET:
			return;

		case Token::Type::NEG:
		case Token::Type::IDENTIFIER:
		case Token::Type::LEFT_PARENTHESIS:
		case Token::Type::NUMBER:
		case Token::Type::COUNT:
			sourceBundle.add(this->parseLogicalExpression());
			this->parseRemainingSourceBundleContent(sourceBundle);

			return;

		default:
			throw ParseError(ParseError::Type::UNEXPECTED_TOKEN, this->it);
	};
};

void Parser::parseRemainingSourceBundleContent(SourceBundle &sourceBundle) {
	while(true) {
		switch(this->getNextTokenType()) {
			case Token::Type::RIGHT_SQUARE_BRACKET:
				return;

			case Token::Type::COMMA:
				this->expect(Token::Type::COMMA);
				sourceBundle.add(this->parseLogicalExpression());

				break;

			default:
				throw ParseError(ParseError::Type::UNEXPECTED_TOKEN, this->it);
		};
	};
};

std::unique_ptr<Expression> Parser::parseLogicalExpression() {
	std::unique_ptr<Expression> expression;

	expression = this->parseLogicalTerm();

	return this->parseRemainingLogicalExpression(std::move(expression));
};

std::unique_ptr<Expression> Parser::parseRemainingLogicalExpression(std::unique_ptr<Expression> expression) {
	while(true) {
		switch(this->getNextTokenType()) {
			case Token::Type::SEMICOLON:
			case Token::Type::COMMA:
			case Token::Type::RIGHT_SQUARE_BRACKET:
			case Token::Type::RIGHT_PARENTHESIS:
			case Token::Type::COLON:
				return expression;

			case Token::Type::OR:
				this->expect(Token::Type::OR);
				expression = std::make_unique<BinaryExpression>(std::move(expression), this->parseLogicalTerm(), BinaryExpression::Type::OR);

				break;

			case Token::Type::XOR:
				this->expect(Token::Type::XOR);
				expression = std::make_unique<BinaryExpression>(std::move(expression), this->parseLogicalTerm(), BinaryExpression::Type::XOR);

				break;

			default:
				throw ParseError(ParseError::Type::UNEXPECTED_TOKEN, this->it);
		};
	};
};

std::unique_ptr<Expression> Parser::parseLogicalTerm() {
	std::unique_ptr<Expression> expression;

	expression = this->parseLogicalFactor();

	return this->parseRemainingLogicalTerm(std::move(expression));
};

std::unique_ptr<Expression> Parser::parseRemainingLogicalTerm(std::unique_ptr<Expression> expression) {
	while(true) {
		switch(this->getNextTokenType()) {
			case Token::Type::OR:
			case Token::Type::XOR:
			case Token::Type::SEMICOLON:
			case Token::Type::COMMA:
			case Token::Type::RIGHT_SQUARE_BRACKET:
			case Token::Type::RIGHT_PARENTHESIS:
			case Token::Type::COLON:
				return expression;

			case Token::Type::AND:
				this->expect(Token::Type::AND);
				expression = std::make_unique<BinaryExpression>(std::move(expression), this->parseLogicalFactor(), BinaryExpression::Type::AND);

				break;

			default:
				throw ParseError(ParseError::Type::UNEXPECTED_TOKEN, this->it);
		};
	};
};

std::unique_ptr<Expression> Parser::parseLogicalFactor() {
	bool isNegated = false;
	std::unique_ptr<Expression> expression;

	while(true) {
		switch(this->getNextTokenType()) {
			case Token::Type::NEG:
				this->expect(Token::Type::NEG);
				isNegated = (!isNegated);

				break;

			case Token::Type::IDENTIFIER:
			case Token::Type::LEFT_PARENTHESIS:
			case Token::Type::NUMBER:
			case Token::Type::COUNT:
				expression = this->parseExpression();
				expression = this->parseRemainingLogicalFactor(std::move(expression));
				if(isNegated)
					expression = std::make_unique<UnaryExpression>(std::move(expression), UnaryExpression::Type::NEG);

				return expression;

			default:
				throw ParseError(ParseError::Type::UNEXPECTED_TOKEN, this->it);
		};
	};
};

std::unique_ptr<Expression> Parser::parseRemainingLogicalFactor(std::unique_ptr<Expression> expression) {
	switch(this->getNextTokenType()) {
		case Token::Type::AND:
		case Token::Type::OR:
		case Token::Type::XOR:
		case Token::Type::SEMICOLON:
		case Token::Type::COMMA:
		case Token::Type::RIGHT_SQUARE_BRACKET:
		case Token::Type::RIGHT_PARENTHESIS:
		case Token::Type::COLON:
			break;

		case Token::Type::EQ:
			this->expect(Token::Type::EQ);
			expression = std::make_unique<BinaryExpression>(std::move(expression), this->parseExpression(), BinaryExpression::Type::EQ);

			break;

		case Token::Type::NE:
			this->expect(Token::Type::NE);
			expression = std::make_unique<BinaryExpression>(std::move(expression), this->parseExpression(), BinaryExpression::Type::NE);

			break;

		case Token::Type::LT:
			this->expect(Token::Type::LT);
			expression = std::make_unique<BinaryExpression>(std::move(expression), this->parseExpression(), BinaryExpression::Type::LT);

			break;

		case Token::Type::LTE:
			this->expect(Token::Type::LTE);
			expression = std::make_unique<BinaryExpression>(std::move(expression), this->parseExpression(), BinaryExpression::Type::LTE);

			break;

		case Token::Type::GT:
			this->expect(Token::Type::GT);
			expression = std::make_unique<BinaryExpression>(std::move(expression), this->parseExpression(), BinaryExpression::Type::GT);

			break;

		case Token::Type::GTE:
			this->expect(Token::Type::GTE);
			expression = std::make_unique<BinaryExpression>(std::move(expression), this->parseExpression(), BinaryExpression::Type::GTE);

			break;

		default:
			throw ParseError(ParseError::Type::UNEXPECTED_TOKEN, this->it);
	};

	return expression;
};

std::unique_ptr<Expression> Parser::parseExpression() {
	std::unique_ptr<Expression> expression;

	expression = this->parseTerm();

	return this->parseRemainingExpression(std::move(expression));
};

std::unique_ptr<Expression> Parser::parseRemainingExpression(std::unique_ptr<Expression> expression) {
	while(true) {
		switch(this->getNextTokenType()) {
			case Token::Type::EQ:
			case Token::Type::NE:
			case Token::Type::LT:
			case Token::Type::LTE:
			case Token::Type::GT:
			case Token::Type::GTE:
			case Token::Type::AND:
			case Token::Type::OR:
			case Token::Type::XOR:
			case Token::Type::SEMICOLON:
			case Token::Type::COMMA:
			case Token::Type::RIGHT_SQUARE_BRACKET:
			case Token::Type::RIGHT_PARENTHESIS:
			case Token::Type::COLON:
				return expression;

			case Token::Type::PLUS:
				this->expect(Token::Type::PLUS);
				expression = std::make_unique<BinaryExpression>(std::move(expression), this->parseTerm(), BinaryExpression::Type::PLUS);

				break;

			case Token::Type::MINUS:
				this->expect(Token::Type::MINUS);
				expression = std::make_unique<BinaryExpression>(std::move(expression), this->parseTerm(), BinaryExpression::Type::MINUS);

				break;

			default:
				throw ParseError(ParseError::Type::UNEXPECTED_TOKEN, this->it);
		};
	};
};

std::unique_ptr<Expression> Parser::parseTerm() {
	std::unique_ptr<Expression> expression;

	expression = this->parseFactor();

	return this->parseRemainingTerm(std::move(expression));
};

std::unique_ptr<Expression> Parser::parseRemainingTerm(std::unique_ptr<Expression> expression) {
	while(true) {
		switch(this->getNextTokenType()) {
			case Token::Type::PLUS:
			case Token::Type::MINUS:
			case Token::Type::EQ:
			case Token::Type::NE:
			case Token::Type::LT:
			case Token::Type::LTE:
			case Token::Type::GT:
			case Token::Type::GTE:
			case Token::Type::AND:
			case Token::Type::OR:
			case Token::Type::XOR:
			case Token::Type::SEMICOLON:
			case Token::Type::COMMA:
			case Token::Type::RIGHT_SQUARE_BRACKET:
			case Token::Type::RIGHT_PARENTHESIS:
			case Token::Type::COLON:
				return expression;

			case Token::Type::MUL:
				this->expect(Token::Type::MUL);
				expression = std::make_unique<BinaryExpression>(std::move(expression), this->parseFactor(), BinaryExpression::Type::MUL);

				break;

			case Token::Type::DIV:
				this->expect(Token::Type::DIV);
				expression = std::make_unique<BinaryExpression>(std::move(expression), this->parseFactor(), BinaryExpression::Type::DIV);

				break;

			case Token::Type::MOD:
				this->expect(Token::Type::MOD);
				expression = std::make_unique<BinaryExpression>(std::move(expression), this->parseFactor(), BinaryExpression::Type::MOD);


				break;

			default:
				throw ParseError(ParseError::Type::UNEXPECTED_TOKEN, this->it);
		};
	};
};

std::unique_ptr<Expression> Parser::parseFactor() {
	std::unique_ptr<Expression> expression;

	expression = this->parseMaxArgument();

	return this->parseRemainingFactor(std::move(expression));
};

std::unique_ptr<Expression> Parser::parseRemainingFactor(std::unique_ptr<Expression> expression) {
	while(true) {
		switch(this->getNextTokenType()) {
			case Token::Type::MUL:
			case Token::Type::DIV:
			case Token::Type::MOD:
			case Token::Type::PLUS:
			case Token::Type::MINUS:
			case Token::Type::EQ:
			case Token::Type::NE:
			case Token::Type::LT:
			case Token::Type::LTE:
			case Token::Type::GT:
			case Token::Type::GTE:
			case Token::Type::AND:
			case Token::Type::OR:
			case Token::Type::XOR:
			case Token::Type::SEMICOLON:
			case Token::Type::COMMA:
			case Token::Type::RIGHT_SQUARE_BRACKET:
			case Token::Type::RIGHT_PARENTHESIS:
			case Token::Type::COLON:
				return expression;

			case Token::Type::MAX:
				this->expect(Token::Type::MAX);
				expression = std::make_unique<BinaryExpression>(std::move(expression), this->parseMaxArgument(), BinaryExpression::Type::MAX);

				break;

			default:
				throw ParseError(ParseError::Type::UNEXPECTED_TOKEN, this->it);
		};
	};
};

std::unique_ptr<Expression> Parser::parseMaxArgument() {
	std::unique_ptr<Expression> expression;

	expression = this->parseMinArgument();

	return this->parseRemainingMaxArgument(std::move(expression));
};

std::unique_ptr<Expression> Parser::parseRemainingMaxArgument(std::unique_ptr<Expression> expression) {
	while(true) {
		switch(this->getNextTokenType()) {
			case Token::Type::MAX:
			case Token::Type::MUL:
			case Token::Type::DIV:
			case Token::Type::MOD:
			case Token::Type::PLUS:
			case Token::Type::MINUS:
			case Token::Type::EQ:
			case Token::Type::NE:
			case Token::Type::LT:
			case Token::Type::LTE:
			case Token::Type::GT:
			case Token::Type::GTE:
			case Token::Type::AND:
			case Token::Type::OR:
			case Token::Type::XOR:
			case Token::Type::SEMICOLON:
			case Token::Type::COMMA:
			case Token::Type::RIGHT_SQUARE_BRACKET:
			case Token::Type::RIGHT_PARENTHESIS:
			case Token::Type::COLON:
				return expression;

			case Token::Type::MIN:
				this->expect(Token::Type::MIN);
				expression = std::make_unique<BinaryExpression>(std::move(expression), this->parseMinArgument(), BinaryExpression::Type::MIN);

				break;

			default:
				throw ParseError(ParseError::Type::UNEXPECTED_TOKEN, this->it);
		};
	};
};

std::unique_ptr<Expression> Parser::parseMinArgument() {
	size_t number, beginningLineNumber;
	std::wstring identifier;
	std::unique_ptr<Expression> expression;

	switch(this->getNextTokenType()) {
		case Token::Type::IDENTIFIER:
			this->expect(identifier);
			expression = this->parseOptionalCall(std::move(identifier), this->getLastTokenLocation());
			expression = this->parseOptionalEllipsisOrArrayAccess(std::move(expression));

			break;

		case Token::Type::LEFT_PARENTHESIS:
			this->expect(Token::Type::LEFT_PARENTHESIS);
			beginningLineNumber = this->getLastTokenLocation().getBeginningLineNumber();
			expression = this->parseLogicalExpression();
			this->expect(Token::Type::RIGHT_PARENTHESIS);
			expression->location = { { std::prev(expression->location.getView().begin()), std::next(expression->location.getView().end()) }, beginningLineNumber, expression->location.getWholeSource() };
			expression = this->parseOptionalEllipsisOrArrayAccess(std::move(expression));

			break;

		case Token::Type::NUMBER:
			this->expect(number);
			expression = std::make_unique<NumberExpression>(this->getLastTokenLocation(), number);

			break;

		case Token::Type::COUNT:
			this->expect(Token::Type::COUNT, identifier);
			expression = std::make_unique<CountExpression>(Location(std::prev(std::prev(this->it))->getLocation(), this->getLastTokenLocation()), this->program.findOrAddVariable(identifier));

			break;

		default:
			throw ParseError(ParseError::Type::UNEXPECTED_TOKEN, this->it);
	};

	return expression;
};

std::unique_ptr<Expression> Parser::parseOptionalCall(std::wstring identifier, Location identifierLocation) {
	std::vector<std::unique_ptr<Expression>> arguments;

	switch(this->getNextTokenType()) {
		case Token::Type::ELLIPSIS:
		case Token::Type::LEFT_SQUARE_BRACKET:
		case Token::Type::MIN:
		case Token::Type::MAX:
		case Token::Type::MUL:
		case Token::Type::DIV:
		case Token::Type::MOD:
		case Token::Type::PLUS:
		case Token::Type::MINUS:
		case Token::Type::EQ:
		case Token::Type::NE:
		case Token::Type::LT:
		case Token::Type::LTE:
		case Token::Type::GT:
		case Token::Type::GTE:
		case Token::Type::AND:
		case Token::Type::OR:
		case Token::Type::XOR:
		case Token::Type::SEMICOLON:
		case Token::Type::COMMA:
		case Token::Type::RIGHT_SQUARE_BRACKET:
		case Token::Type::RIGHT_PARENTHESIS:
		case Token::Type::COLON:
			return std::make_unique<VariableExpression>(identifierLocation, this->program.findOrAddVariable(identifier));

		case Token::Type::LEFT_PARENTHESIS:
			this->expect(Token::Type::LEFT_PARENTHESIS);
			arguments = this->parseLogicalExpressions();
			this->expect(Token::Type::RIGHT_PARENTHESIS);

			return std::make_unique<CallExpression>(Location(identifierLocation, this->getLastTokenLocation()), this->program.findMachine(identifier, identifierLocation), std::move(arguments));

		default:
			throw ParseError(ParseError::Type::UNEXPECTED_TOKEN, this->it);
	};
};

std::unique_ptr<Expression> Parser::parseOptionalEllipsisOrArrayAccess(std::unique_ptr<Expression> expression) {
	switch(this->getNextTokenType()) {
		case Token::Type::MIN:
		case Token::Type::MAX:
		case Token::Type::MUL:
		case Token::Type::DIV:
		case Token::Type::MOD:
		case Token::Type::PLUS:
		case Token::Type::MINUS:
		case Token::Type::EQ:
		case Token::Type::NE:
		case Token::Type::LT:
		case Token::Type::LTE:
		case Token::Type::GT:
		case Token::Type::GTE:
		case Token::Type::AND:
		case Token::Type::OR:
		case Token::Type::XOR:
		case Token::Type::SEMICOLON:
		case Token::Type::COMMA:
		case Token::Type::RIGHT_SQUARE_BRACKET:
		case Token::Type::RIGHT_PARENTHESIS:
		case Token::Type::COLON:
			return expression;

		case Token::Type::ELLIPSIS:
			this->expect(Token::Type::ELLIPSIS);

			return std::make_unique<ArrayAccessExpression>(std::move(expression), this->getLastTokenLocation(), 0, std::nullopt);

		case Token::Type::LEFT_SQUARE_BRACKET:
			this->expect(Token::Type::LEFT_SQUARE_BRACKET);

			return this->parseArrayAccess(std::move(expression));

		default:
			throw ParseError(ParseError::Type::UNEXPECTED_TOKEN, this->it);
	};
};

std::unique_ptr<Expression> Parser::parseArrayAccess(std::unique_ptr<Expression> expression) {
	size_t indexA;
	std::optional<size_t> indexB;

	switch(this->getNextTokenType()) {
		case Token::Type::NUMBER:
			this->expect(indexA);

			return this->parseRemainingArrayAccess(std::move(expression), indexA);

		case Token::Type::COLON:
			this->expect(Token::Type::COLON);
			indexB = this->parseOptionalNumber();
			this->expect(Token::Type::RIGHT_SQUARE_BRACKET);

			return std::make_unique<ArrayAccessExpression>(std::move(expression), this->getLastTokenLocation(), 0, indexB);

		default:
			throw ParseError(ParseError::Type::UNEXPECTED_TOKEN, this->it);
	};
};

std::unique_ptr<Expression> Parser::parseRemainingArrayAccess(std::unique_ptr<Expression> expression, size_t indexA) {
	std::optional<size_t> indexB;

	switch(this->getNextTokenType()) {
		case Token::Type::RIGHT_SQUARE_BRACKET:
			this->expect(Token::Type::RIGHT_SQUARE_BRACKET);

			return std::make_unique<ArrayAccessExpression>(std::move(expression), this->getLastTokenLocation(), indexA, indexA + 1);

		case Token::Type::COLON:
			this->expect(Token::Type::COLON);
			indexB = this->parseOptionalNumber();
			this->expect(Token::Type::RIGHT_SQUARE_BRACKET);

			return std::make_unique<ArrayAccessExpression>(std::move(expression), this->getLastTokenLocation(), indexA, indexB);

		default:
			throw ParseError(ParseError::Type::UNEXPECTED_TOKEN, this->it);
	};
};

std::optional<size_t> Parser::parseOptionalNumber() {
	size_t number;

	switch(this->getNextTokenType()) {
		case Token::Type::COLON:
		case Token::Type::RIGHT_SQUARE_BRACKET:
			return {};

		case Token::Type::NUMBER:
			this->expect(number);

			return number;

		default:
			throw ParseError(ParseError::Type::UNEXPECTED_TOKEN, this->it);
	};
};

std::vector<std::unique_ptr<Expression>> Parser::parseLogicalExpressions() {
	std::vector<std::unique_ptr<Expression>> expressions;

	switch(this->getNextTokenType()) {
		case Token::Type::RIGHT_PARENTHESIS:
			break;

		case Token::Type::NEG:
		case Token::Type::IDENTIFIER:
		case Token::Type::LEFT_PARENTHESIS:
		case Token::Type::NUMBER:
			expressions.push_back(this->parseLogicalExpression());
			this->parseRemainingLogicalExpressions(expressions);

			break;

		default:
			throw ParseError(ParseError::Type::UNEXPECTED_TOKEN, this->it);
	};

	return expressions;
};

void Parser::parseRemainingLogicalExpressions(std::vector<std::unique_ptr<Expression>> &expressions) {
	while(true) {
		switch(this->getNextTokenType()) {
			case Token::Type::RIGHT_PARENTHESIS:
				return;

			case Token::Type::COMMA:
				this->expect(Token::Type::COMMA);
				expressions.push_back(this->parseLogicalExpression());

				break;

			default:
				throw ParseError(ParseError::Type::UNEXPECTED_TOKEN, this->it);
		};
	};
};

std::unique_ptr<Statement> Parser::parseIfStatement() {
	bool wasInBranch;
	std::unique_ptr<Expression> condition;
	std::vector<std::unique_ptr<Statement>> trueBranch, falseBranch;

	this->expect(Token::Type::IF);
	condition = this->parseLogicalExpression();
	this->expect(Token::Type::COLON);
	wasInBranch = std::exchange(this->isInBranch, true);
	trueBranch = this->parseStatements();
	falseBranch = this->parseOptionalElse();
	this->isInBranch = wasInBranch;
	this->expect(Token::Type::ENDIF);

	return std::make_unique<IfStatement>(std::move(condition), std::move(trueBranch), std::move(falseBranch));
};

std::vector<std::unique_ptr<Statement>> Parser::parseOptionalElse() {
	switch(this->getNextTokenType()) {
		case Token::Type::ENDIF:
			return {};

		case Token::Type::ELSE:
			this->expect(Token::Type::ELSE, Token::Type::COLON);

			return this->parseStatements();

		default:
			throw ParseError(ParseError::Type::UNEXPECTED_TOKEN, this->it);
	};
};

std::unique_ptr<Statement> Parser::parseWhileStatement() {
	bool wasInLoop;
	std::unique_ptr<Expression> condition;
	std::vector<std::unique_ptr<Statement>> body;

	this->expect(Token::Type::WHILE);
	condition = this->parseLogicalExpression();
	this->expect(Token::Type::COLON);
	wasInLoop = std::exchange(this->isInLoop, true);
	body = this->parseStatements();
	this->isInLoop = wasInLoop;
	this->expect(Token::Type::ENDWHILE);

	return std::make_unique<WhileStatement>(std::move(condition), std::move(body));
};

std::unique_ptr<Statement> Parser::parseLoopStatement() {
	bool wasInLoop;
	std::vector<std::unique_ptr<Statement>> body;

	this->expect(Token::Type::LOOP, Token::Type::COLON);
	wasInLoop = std::exchange(this->isInLoop, true);
	body = this->parseStatements();
	this->isInLoop = wasInLoop;
	this->expect(Token::Type::ENDLOOP);

	return std::make_unique<LoopStatement>(std::move(body));
};

void Parser::parseIncludeStatement() {
	std::wstring machineName;

	this->expect(Token::Type::INCLUDE, machineName);
	this->program.addMachine(machineName, this->includeResolver.include(machineName, this->getLastTokenLocation()), this->getLastTokenLocation());

	if(this->isInBranch || this->isInLoop)
		this->warningIt = std::make_unique<GeneralWarning>(L"Machine "+Format::blue(machineName)+L" included inside an if branch or a while loop. Machines are always visible globally after definition.");
};

void Parser::parseRequireStatement() {
	std::wstring machineName;

	this->expect(Token::Type::REQUIRE, machineName);
	this->program.addMachine(machineName, this->includeResolver.require(), this->getLastTokenLocation());

	if(this->isInBranch || this->isInLoop)
		this->warningIt = std::make_unique<GeneralWarning>(L"Machine "+Format::blue(machineName)+L" required inside an if branch or a while loop. Machines are always visible globally after definition.");
};

/*!
 * Performs the syntactic and semantic analysis.
 * The AST is stored and can be retrieved by extractProgram().
 * Warnings are appended using the iterator provided to the constructor.
 * \return This instance.
 * \throw ParseError If the LL(1) analysis fails (UNEXPECTED_TOKEN) or if a break or continue statement is outside a loop (STRAY_BREAK, STRAY_CONTINUE).
 * \throw SymbolError If the symbol table (the Program class) detected an error (unknown or duplicate machine) or IncludeResolver cannot resolve an include.
 * \throw GeneralError If the IncludeResolver cannot read one of the provided machine.
 * \throw TypeError If the AST cannot be constructed because of properties/incompatibility of expressions.
 */
Parser &Parser::parse() {
	this->expect(Token::Type::BEGINNING);
	this->parseProgram();
	this->expect(Token::Type::END);

	if(this->includeResolver.isRequiredMachineAvailable())
		this->warningIt = std::make_unique<GeneralWarning>(L"Not all provided machines have been used by require statements.");

	return (*this);
};

/*!
 * Retrieve the AST.
 * The AST will no longer be in the parser.
 * \return The root of the AST (a unique pointer to an instance of Program).
 */
Program Parser::extractProgram() {
	return std::move(this->program);
};
