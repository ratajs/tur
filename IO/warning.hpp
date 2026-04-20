#pragma once
#include "../common.hpp"

class Warning {
	public:
		virtual ~Warning();
		virtual std::optional<std::wstring> getTitle() const;
		virtual std::wstring getMessage() const = 0;

	friend std::wostream &operator << (std::wostream &stream, const Warning &error);
};
