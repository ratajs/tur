#pragma once
#include "../common.hpp"
#include "./error.hpp"

class GeneralError: public Error {
	private:
		std::wstring message;

	public:
		GeneralError(std::wstring message);
		std::wstring getMessage() const override;
};
