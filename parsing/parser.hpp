#pragma once
#include "../common.hpp"
#include "../machine/machine.hpp"
#include "./token.hpp"
#include "./location.hpp"
#include "../AST/program.hpp"
#include "../AST/statements/statement.hpp"
#include "../AST/expressions/expression.hpp"
#include "../AST/sourceBundle.hpp"
#include "../AST/destinationBundle.hpp"
#include "../IO/includeResolver.hpp"
#include "../IO/warning.hpp"

class Parser {
	private:
		bool isInBranch = false, isInLoop = false;
		std::vector<Token>::const_iterator it;
		std::back_insert_iterator<std::vector<std::unique_ptr<Warning>>> warningIt;
		IncludeResolver includeResolver;
		Program program;

		Token::Type getNextTokenType() const;
		const Token &readToken();
		size_t readNumberToken();
		const std::wstring &readIdentifierToken();
		const Machine &readMachineToken();
		void expect(Token::Type type, Token_TypeOrValue auto &&... rest);
		void expect(size_t &number, Token_TypeOrValue auto &&... rest);
		void expect(std::wstring &identifier, Token_TypeOrValue auto &&... rest);
		void expect(Machine &machine, Token_TypeOrValue auto &&... rest);
		void expect();
		const Location &getLastTokenLocation() const;
		void parseProgram();
		std::vector<std::unique_ptr<Statement>> parseStatements();
		std::unique_ptr<Statement> parseStatement();
		std::unique_ptr<Statement> parseAssignmentOrSimpleCall();
		std::unique_ptr<Statement> parseRemainingAssignmentOrSimpleCall(std::wstring identifier, Location identifierLocation);
		std::unique_ptr<Statement> parseRemainingArrayAccessAssignment(std::wstring identifier);
		DestinationBundle parseDestinationBundle();
		void parseRemainingDestinationBundle(DestinationBundle &destinationBundle);
		bool parseOptionalEllipsis();
		std::variant<std::unique_ptr<Expression>, SourceBundle, Machine> parseLogicalExpressionOrSourceBundleOrMachine();
		std::variant<std::unique_ptr<Expression>, SourceBundle> parseLogicalExpressionOrSourceBundle();
		SourceBundle parseSourceBundle();
		void parseSourceBundleContent(SourceBundle &sourceBundle);
		void parseRemainingSourceBundleContent(SourceBundle &sourceBundle);
		std::unique_ptr<Expression> parseLogicalExpression();
		std::unique_ptr<Expression> parseRemainingLogicalExpression(std::unique_ptr<Expression>);
		std::unique_ptr<Expression> parseLogicalTerm();
		std::unique_ptr<Expression> parseRemainingLogicalTerm(std::unique_ptr<Expression>);
		std::unique_ptr<Expression> parseLogicalFactor();
		std::unique_ptr<Expression> parseRemainingLogicalFactor(std::unique_ptr<Expression>);
		std::unique_ptr<Expression> parseExpression();
		std::unique_ptr<Expression> parseRemainingExpression(std::unique_ptr<Expression>);
		std::unique_ptr<Expression> parseTerm();
		std::unique_ptr<Expression> parseRemainingTerm(std::unique_ptr<Expression>);
		std::unique_ptr<Expression> parseFactor();
		std::unique_ptr<Expression> parseRemainingFactor(std::unique_ptr<Expression>);
		std::unique_ptr<Expression> parseMaxArgument();
		std::unique_ptr<Expression> parseRemainingMaxArgument(std::unique_ptr<Expression>);
		std::unique_ptr<Expression> parseMinArgument();
		std::unique_ptr<Expression> parseOptionalCall(std::wstring identifier, Location identifierLocation);
		std::unique_ptr<Expression> parseOptionalEllipsisOrArrayAccess(std::unique_ptr<Expression>);
		std::unique_ptr<Expression> parseArrayAccess(std::unique_ptr<Expression>);
		std::unique_ptr<Expression> parseRemainingArrayAccess(std::unique_ptr<Expression> expression, size_t indexA);
		std::optional<size_t> parseOptionalNumber();
		std::vector<std::unique_ptr<Expression>> parseLogicalExpressions();
		void parseRemainingLogicalExpressions(std::vector<std::unique_ptr<Expression>> &expressions);
		std::unique_ptr<Statement> parseIfStatement();
		std::vector<std::unique_ptr<Statement>> parseOptionalElse();
		std::unique_ptr<Statement> parseWhileStatement();
		std::unique_ptr<Statement> parseLoopStatement();
		void parseIncludeStatement();
		void parseRequireStatement();

	public:
		Parser(std::vector<Token> tokens, IncludeResolver includeResolver, const std::back_insert_iterator<std::vector<std::unique_ptr<Warning>>> &warningIt);
		Parser &parse();
		Program extractProgram();
};
