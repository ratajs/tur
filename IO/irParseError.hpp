#pragma once
#include "../common.hpp"
#include "./errorWithLocation.hpp"

class IrParseError: public ErrorWithLocation {
	public:
		enum class Type {
			WRONG_LINE_FORMAT,
			NUMBER_OF_TAPES_ALREADY_SET,
			UNKNOWN_INSTRUCTION,
			INVALID_ARGUMENTS,
			INVALID_TAPE_NUMBER,
			INVALID_RANGE_FOR_CLEAR
		};

	private:
		IrParseError::Type type;

		std::wstring getSimpleMessage() const;

	public:
		IrParseError(IrParseError::Type type, const Location &location);
		std::optional<std::wstring> getTitle() const override;
		std::wstring getMessage() const override;
};
