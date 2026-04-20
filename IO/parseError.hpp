#pragma once
#include "../common.hpp"
#include "./errorWithLocation.hpp"
#include "../parsing/token.hpp"

class ParseError: public ErrorWithLocation {
	public:
		enum class Type {
			UNEXPECTED_TOKEN, STRAY_BREAK, STRAY_CONTINUE
		};

	private:
		ParseError::Type type;
		Token::Type tokenType;

	public:
		ParseError(ParseError::Type type, std::vector<Token>::const_iterator token);
		std::optional<std::wstring> getTitle() const override;
		std::wstring getMessage() const override;
};
