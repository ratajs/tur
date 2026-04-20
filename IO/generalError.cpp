#include "./generalError.hpp"

GeneralError::GeneralError(std::wstring message): message(std::move(message)) {};

std::wstring GeneralError::getMessage() const {
	return this->message;
};
