#pragma once
#include "../common.hpp"

/*!
 * This is an abstract class for all error types used by tur.
 */
class Error {
	public:
		virtual ~Error();

		/*!
		 * This virtual method should return a general type of the error, it will be followed by ‘ error:’ when printing.
		 * The default implementations return {}, so no title.
		 * \return The type of the error, if available.
		 */
		virtual std::optional<std::wstring> getTitle() const;

		/*!
		 * This virtual method should return the full message.
		 * \return The content of the error.
		 */
		virtual std::wstring getMessage() const = 0;

	friend std::wostream &operator << (std::wostream &stream, const Error &error);
};
