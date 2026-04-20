#include "./error.hpp"
#include "./format.hpp"

Error::~Error() {};

std::optional<std::wstring> Error::getTitle() const {
	return {};
};

std::wostream &operator << (std::wostream &stream, const Error &error) {
	stream << Format::boldRed(
		error.getTitle().transform([](std::wstring &&title) -> std::wstring { title+= L" error:"; return title; }).value_or(L"Error:")
	) << L" " << error.getMessage();

	return stream;
};
