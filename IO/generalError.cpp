#include "./generalError.hpp"

/*!
 * The constructor of GeneralWarning.
 * \param message The message.
 */
GeneralError::GeneralError(std::wstring message): message(std::move(message)) {};

std::wstring GeneralError::getMessage() const {
	return this->message;
};
