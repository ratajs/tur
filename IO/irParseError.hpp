#pragma once
#include <optional>
#include <string>
#include "./errorWithLocation.hpp"

/*!
 * This type of error signifies an invalid IR program.
 */
class IrParseError: public ErrorWithLocation {
	public:
		/*!
		 * These are all types of IrParseError.
		 */
		enum class Type {
			WRONG_LINE_FORMAT,
			NUMBER_OF_TAPES_ALREADY_SET,
			UNKNOWN_INSTRUCTION,
			INVALID_ARGUMENTS,
			INVALID_TAPE_NUMBER,
			INVALID_RANGE_FOR_CLEAR,
			COME_FROM_EXPECTED,
			COME_FROM_NOT_EXPECTED,
			MULTIPLE_LABEL_ENDPOINTS
		};

	private:
		IrParseError::Type type;

		std::wstring getSimpleMessage() const;

	public:
		IrParseError(IrParseError::Type type, const Location &location);
		std::optional<std::wstring> getTitle() const override;
		std::wstring getMessage() const override;
};
