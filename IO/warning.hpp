#pragma once
#include "../common.hpp"

/*!
 * This is an abstract class for all warning types used by tur.
 */
class Warning {
	public:
		virtual ~Warning();

		/*!
		 * This virtual method should return a general type of the warning, it will be followed by ‘ warning:’ when printing.
		 * The default implementations return {}, so no title.
		 * \return The type of the warning, if available.
		 */
		virtual std::optional<std::wstring> getTitle() const;

		/*!
		 * This virtual method should return the full message.
		 * \return The content of the warning.
		 */
		virtual std::wstring getMessage() const = 0;

	friend std::wostream &operator << (std::wostream &stream, const Warning &error);
};
