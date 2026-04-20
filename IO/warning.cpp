#include "./warning.hpp"
#include "./format.hpp"

Warning::~Warning() {};

std::optional<std::wstring> Warning::getTitle() const {
	return {};
};

std::wostream &operator << (std::wostream &stream, const Warning &warning) {
	stream << Format::boldYellow(
		warning.getTitle().transform([](std::wstring &&title) -> std::wstring { title+= L" warning:"; return title; }).value_or(L"Warning:")
	) << L" " << warning.getMessage();

	return stream;
};
