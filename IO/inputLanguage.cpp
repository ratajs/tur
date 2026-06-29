#include "./inputLanguage.hpp"
#include <algorithm>
#include <string>
#include <cwctype>

std::wistream &operator >> (std::wistream &stream, InputLanguage &language) {
	std::wstring languageString;

	stream >> languageString;

	std::ranges::transform(languageString, languageString.begin(), std::towlower);

	if(languageString==L"tur") {
		language = InputLanguage::TUR;

		return stream;
	};

	if(languageString==L"ir") {
		language = InputLanguage::IR;

		return stream;
	};

	stream.setstate(std::ios::failbit);

	return stream;
};
