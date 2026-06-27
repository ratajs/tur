#include "./irParseError.hpp"
#include <utility>
#include "./format.hpp"

IrParseError::IrParseError(IrParseError::Type type, const Location &location): ErrorWithLocation(location), type(type) {};

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

		case IrParseError::Type::INVALID_RANGE_FOR_CLEAR:
			return L"Invalid range for clear";

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
