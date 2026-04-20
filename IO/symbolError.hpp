#pragma once
#include "../common.hpp"
#include "./errorWithLocation.hpp"
#include "../parsing/location.hpp"

class SymbolError: public ErrorWithLocation {
	public:
		enum class Type {
			UNKNOWN_MACHINE,
			DUPLICATE_MACHINE,
			INCLUDED_MACHINE_FILE_NOT_FOUND,
			INCLUDED_MACHINE_NOT_A_REGULAR_FILE,
			INCLUDED_MACHINE_ERROR_READING
		};

	private:
		SymbolError::Type type;
		std::wstring searchedFileName;

		std::wstring getSimpleMessage() const;
		std::wstring getDetailedMessage() const;

	public:
		SymbolError(SymbolError::Type type, const Location &identifierLocation);
		SymbolError(SymbolError::Type type, const Location &identifierLocation, std::wstring searchedFileName);
		std::optional<std::wstring> getTitle() const override;
		std::wstring getMessage() const override;
};
