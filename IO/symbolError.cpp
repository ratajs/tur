#include "./symbolError.hpp"
#include "../parsing/location.hpp"
#include "./format.hpp"
#include "./unexpectedError.hpp"

/*!
 * The constructor of SymbolError.
 * \param type The type of the error.
 * \param identifierLocation The location to print.
 * \throw UnexpectedError If the type of the error needs a file path to print.
 */
SymbolError::SymbolError(SymbolError::Type type, const Location &identifierLocation): ErrorWithLocation(identifierLocation), type(type) {
	if(type==SymbolError::Type::INCLUDED_MACHINE_FILE_NOT_FOUND || type==SymbolError::Type::INCLUDED_MACHINE_NOT_A_REGULAR_FILE || type==SymbolError::Type::INCLUDED_MACHINE_ERROR_READING || type==SymbolError::Type::INCLUDED_MACHINE_INVALID_MACHINE)
		throw UnexpectedError(L"Not gotten searchedFileName argument for an error which is file-related.");
};

/*!
 * The constructor of SymbolError, a variant which specifies a problematic file.
 * \param type The type of the error.
 * \param identifierLocation The location to print.
 * \param searchedFileName The file which caused the error.
 * \throw UnexpectedError If the type has nothing to do with a file.
 */
SymbolError::SymbolError(SymbolError::Type type, const Location &identifierLocation, std::wstring searchedFileName): ErrorWithLocation(identifierLocation), type(type), searchedFileName(std::move(searchedFileName)) {
	if(type!=SymbolError::Type::INCLUDED_MACHINE_FILE_NOT_FOUND && type!=SymbolError::Type::INCLUDED_MACHINE_NOT_A_REGULAR_FILE && type!=SymbolError::Type::INCLUDED_MACHINE_ERROR_READING && type!=SymbolError::Type::INCLUDED_MACHINE_INVALID_MACHINE)
		throw UnexpectedError(L"Gotten searchedFileName argument for an error which is not file-related.");
};

/*!
 * \return The beginning of the error message, only from the type.
 */
std::wstring SymbolError::getSimpleMessage() const {
	switch(this->type) {
		case SymbolError::Type::UNKNOWN_MACHINE:
			return L"Unknown machine";

		case SymbolError::Type::DUPLICATE_MACHINE:
			return L"Duplicate machine";

		case SymbolError::Type::INCLUDED_MACHINE_FILE_NOT_FOUND:
		case SymbolError::Type::INCLUDED_MACHINE_NOT_A_REGULAR_FILE:
		case SymbolError::Type::INCLUDED_MACHINE_ERROR_READING:
			return L"Unresolved include";

		default:
			std::unreachable();
	};
};

/*!
 * \return The end of the error message, just before the location.
 */
std::wstring SymbolError::getDetailedMessage() const {
	switch(this->type) {
		case SymbolError::Type::UNKNOWN_MACHINE:
		case SymbolError::Type::DUPLICATE_MACHINE:
			return L"";

		case SymbolError::Type::INCLUDED_MACHINE_FILE_NOT_FOUND:
			return (L" (file "+Format::blue(this->searchedFileName)+L" does not exist)");

		case SymbolError::Type::INCLUDED_MACHINE_NOT_A_REGULAR_FILE:
			return (L" (file "+Format::blue(this->searchedFileName)+L" is not a regular file)");

		case SymbolError::Type::INCLUDED_MACHINE_ERROR_READING:
			return (L" (file "+Format::blue(this->searchedFileName)+L" could not be read)");

		case SymbolError::Type::INCLUDED_MACHINE_INVALID_MACHINE:
			return (L" (file "+Format::blue(this->searchedFileName)+L" is not a valid machine file)");

		default:
			std::unreachable();
	};
};

std::optional<std::wstring> SymbolError::getTitle() const {
	return L"Symbol";
};

std::wstring SymbolError::getMessage() const {
	return (
		Format::yellow(this->getSimpleMessage())+L" "+Format::red(std::wstring(this->location))+
		this->getDetailedMessage()+L" "+this->printLocation()
	);
};
