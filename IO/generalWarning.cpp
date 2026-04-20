#include "./generalWarning.hpp"

GeneralWarning::GeneralWarning(std::wstring message): message(std::move(message)) {};

std::wstring GeneralWarning::getMessage() const {
	return this->message;
};
