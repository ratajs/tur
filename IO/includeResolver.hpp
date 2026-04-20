#pragma once
#include "../common.hpp"
#include "../machine/machine.hpp"
#include "../parsing/location.hpp"

class IncludeResolver {
	private:
		std::wstring fileSuffix;
		std::filesystem::path basePath;
		std::vector<std::filesystem::path> providedMachines;
		std::vector<std::filesystem::path>::const_iterator providedMachinesIterator;

	public:
		IncludeResolver(std::filesystem::path basePath, std::wstring fileSuffix, std::vector<std::filesystem::path> providedMachines);
		Machine include(const std::wstring &name, Location location) const;
		bool isRequiredMachineAvailable() const;
		Machine require();
};
