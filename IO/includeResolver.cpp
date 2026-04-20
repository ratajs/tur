#include "./includeResolver.hpp"
#include "../IO/format.hpp"
#include "../IO/generalError.hpp"
#include "../IO/symbolError.hpp"

IncludeResolver::IncludeResolver(std::filesystem::path basePath, std::wstring fileSuffix, std::vector<std::filesystem::path> providedMachines): fileSuffix(std::move(fileSuffix)), basePath(std::move(basePath)), providedMachines(std::move(providedMachines)), providedMachinesIterator(this->providedMachines.begin()) {};

Machine IncludeResolver::include(const std::wstring &name, Location location) const {
	std::wstring text;
	std::filesystem::path fileName;
	std::wifstream ifs;

	fileName = (basePath / std::filesystem::path(name+this->fileSuffix));

	if(!std::filesystem::exists(fileName))
		throw SymbolError(SymbolError::Type::INCLUDED_MACHINE_FILE_NOT_FOUND, location, fileName.wstring());

	if(!std::filesystem::is_regular_file(fileName))
		throw SymbolError(SymbolError::Type::INCLUDED_MACHINE_NOT_A_REGULAR_FILE, location, fileName.wstring());

	ifs = std::wifstream(std::filesystem::path(fileName));

	if(!ifs)
		throw SymbolError(SymbolError::Type::INCLUDED_MACHINE_ERROR_READING, location, fileName.wstring());

	std::getline(ifs, text, L'\0');

	ifs.close();

	return { text };
};

bool IncludeResolver::isRequiredMachineAvailable() const {
	return (this->providedMachinesIterator!=this->providedMachines.end());
};

Machine IncludeResolver::require() {
	std::wstring text;
	std::filesystem::path fileName;
	std::wifstream ifs;

	if(!this->isRequiredMachineAvailable())
		throw GeneralError(L"More require statements than provided machines.");

	fileName = (*(this->providedMachinesIterator++));

	if(!std::filesystem::exists(fileName))
		throw GeneralError(L"Required machine file "+Format::blue(fileName.wstring())+L" does not exist.");

	if(!std::filesystem::is_regular_file(fileName))
		throw GeneralError(L"Required machine file "+Format::blue(fileName.wstring())+L" is not a regular file.");

	ifs = std::wifstream(std::filesystem::path(fileName));

	if(!ifs)
		throw GeneralError(L"Required machine file "+Format::blue(fileName.wstring())+L" could not be read.");

	std::getline(ifs, text, L'\0');

	ifs.close();

	return { text };
};
