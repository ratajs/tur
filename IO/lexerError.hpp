#pragma once
#include "../common.hpp"
#include "./errorWithLocation.hpp"
#include "../parsing/token.hpp"

/*!
 * This type of error represents a problem during the lexical analysis.
 */
class LexerError: public ErrorWithLocation {
	public:
		/*!
		 * These are all types of LexerError.
		 */
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
