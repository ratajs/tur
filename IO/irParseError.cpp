#include "./irParseError.hpp"
#include <utility>
#include "./format.hpp"

/*!
 * The constructor of IrParseError.
 * \param type The type of the error.
 * \param location The location to print.
 */
IrParseError::IrParseError(IrParseError::Type type, const Location &location): ErrorWithLocation(location), type(type) {};

/*!
 * \return The beginning of the error message, only from the type.
 */
std::wstring IrParseError::getSimpleMessage() const {
	switch(this->type) {
		case IrParseError::Type::WRONG_LINE_FORMAT:
			return L"Invalid line format";

		case IrParseError::Type::NUMBER_OF_TAPES_ALREADY_SET:
			return L"Number of tapes already set";

		case IrParseError::Type::UNKNOWN_INSTRUCTION:
			return L"Unknown instruction";

		case IrParseError::Type::INVALID_ARGUMENTS:
			return L"Invalid arguments";

		case IrParseError::Type::INVALID_TAPE_NUMBER:
			return L"Invalid tape number";

		case IrParseError::Type::INVALID_RANGE:
			return L"Invalid range";

		case IrParseError::Type::INVALID_INDEX:
			return L"Invalid index";

		case IrParseError::Type::INVALID_RANGE_FOR_CLEAR:
			return L"Invalid range for clear";

		case IrParseError::Type::SAME_TAPE_WITH_COPY:
			return L"A copy instruction must copy from one tape to another";

		case IrParseError::Type::COME_FROM_EXPECTED:
			return L"A COMEFROM instruction expected";

		case IrParseError::Type::COME_FROM_NOT_EXPECTED:
			return L"A COMEFROM instruction not expected";

		case IrParseError::Type::MULTIPLE_LABEL_ENDPOINTS:
			return L"Multiple COMEFROM instructions with the same label";

		case IrParseError::Type::LABEL_REVERSAL_MISMATCH:
			return L"Label reversal mismatch";

		default:
			std::unreachable();
	};
};

std::optional<std::wstring> IrParseError::getTitle() const {
	return L"IR parse";
};

std::wstring IrParseError::getMessage() const {
	return (Format::yellow(this->getSimpleMessage())+L" "+this->printLocation());
};
