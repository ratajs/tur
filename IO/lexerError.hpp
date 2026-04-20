#pragma once
#include "../common.hpp"
#include "./errorWithLocation.hpp"
#include "../parsing/token.hpp"

class LexerError: public ErrorWithLocation {
	public:
		enum class Type {
			UNEXPECTED_END, UNEXPECTED_CHARACTER, INVALID_MACHINE
		};

	private:
		LexerError::Type type;
		wchar_t character;

	public:
		LexerError(LexerError::Type type, const Location &location);
		LexerError(LexerError::Type type, wchar_t character, const Location &location);
		std::optional<std::wstring> getTitle() const override;
		std::wstring getMessage() const override;
};
