#include "./symbolError.hpp"
#include "../parsing/location.hpp"
#include "./format.hpp"
#include "./unexpectedError.hpp"

SymbolError::SymbolError(SymbolError::Type type, const Location &identifierLocation): ErrorWithLocation(identifierLocation), type(type) {};

SymbolError::SymbolError(SymbolError::Type type, const Location &identifierLocation, std::wstring searchedFileName): SymbolError(type, identifierLocation) {
	if(type!=SymbolError::Type::INCLUDED_MACHINE_FILE_NOT_FOUND && type!=SymbolError::Type::INCLUDED_MACHINE_NOT_A_REGULAR_FILE && type!=SymbolError::Type::INCLUDED_MACHINE_ERROR_READING)
		throw UnexpectedError(L"Gotten searchedFileName argument for an error which is not file-related.");

	this->searchedFileName = std::move(searchedFileName);
};

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
