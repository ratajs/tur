#include "./includeResolver.hpp"
#include "../IO/format.hpp"
#include "../IO/generalError.hpp"
#include "../IO/symbolError.hpp"

/*!
 * The contructor of IncludeResolver.
 * \param basePath The path where to look for included machine files.
 * \param fileSuffix The suffix of included machine files
 * \param providedMachines The complete paths of machines which should be used for require statements.
 */
IncludeResolver::IncludeResolver(std::filesystem::path basePath, std::wstring fileSuffix, std::vector<std::filesystem::path> providedMachines):
	fileSuffix(std::move(fileSuffix)), basePath(std::move(basePath)), providedMachines(std::move(providedMachines)), providedMachinesIterator(this->providedMachines.begin()) {};

/*!
 * Resolve an include statement.
 * It looks in the base directory for a file named as like the machine name, with the suffix.
 * \param name The machine name.
 * \param location The location to print in case of an error.
 * \return The machine, if successful.
 * \throw SymbolError If the machine could not be loaded.
 */
Machine IncludeResolver::include(const std::wstring &name, Location location) const {
	std::wstring text;
	std::filesystem::path fileName;
	std::wifstream ifs;
	Machine machine;

	fileName = (basePath / std::filesystem::path(name+this->fileSuffix));

	if(!std::filesystem::exists(fileName))
		throw SymbolError(SymbolError::Type::INCLUDED_MACHINE_FILE_NOT_FOUND, location, fileName.wstring());

	if(!std::filesystem::is_regular_file(fileName))
		throw SymbolError(SymbolError::Type::INCLUDED_MACHINE_NOT_A_REGULAR_FILE, location, fileName.wstring());

	ifs = std::wifstream(std::filesystem::path(fileName));

	if(!ifs)
		throw SymbolError(SymbolError::Type::INCLUDED_MACHINE_ERROR_READING, location, fileName.wstring());

	if(!(ifs >> machine))
		throw SymbolError(SymbolError::Type::INCLUDED_MACHINE_INVALID_MACHINE, location, fileName.wstring());

	return machine;
};

/*!
 * \retval false All provided machine files have already been used.
 * \retval true There is at least one unused provided machine files.
 */
bool IncludeResolver::isRequiredMachineAvailable() const {
	return (this->providedMachinesIterator!=this->providedMachines.end());
};

/*!
 * Resolve a requrie statement.
 * It tries to load the next provided file.
 * \return The machine, if successful.
 * \throw SymbolError If the machine could not be loaded.
 */
Machine IncludeResolver::require() {
	std::wstring text;
	std::filesystem::path fileName;
	std::wifstream ifs;
	Machine machine;

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

	if(!(ifs >> machine))
		throw GeneralError(L"Required machine file "+Format::blue(fileName.wstring())+L" is not a valid machine file.");

	return machine;
};
