#pragma once
#include "../common.hpp"
#include "./warning.hpp"

class GeneralWarning: public Warning {
	private:
		std::wstring message;

	public:
		GeneralWarning(std::wstring message);
		std::wstring getMessage() const override;
};
