#include "./generalWarning.hpp"

/*!
 * The constructor of GeneralWarning.
 * \param message The message.
 */
GeneralWarning::GeneralWarning(std::wstring message): message(std::move(message)) {};

std::wstring GeneralWarning::getMessage() const {
	return this->message;
};
