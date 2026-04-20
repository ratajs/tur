#include "./format.hpp"

std::wstring Format::red(std::wstring string) {
	return (L"\033[31m"+string+L"\033[0m");
};

std::wstring Format::yellow(std::wstring string) {
	return (L"\033[33m"+string+L"\033[0m");
};

std::wstring Format::blue(std::wstring string) {
	return (L"\033[34m"+string+L"\033[0m");
};

std::wstring Format::boldRed(std::wstring string) {
	return (L"\033[1;31m"+string+L"\033[0m");
};

std::wstring Format::boldYellow(std::wstring string) {
	return (L"\033[1;33m"+string+L"\033[0m");
};

std::wstring Format::boldBlue(std::wstring string) {
	return (L"\033[1;34m"+string+L"\033[0m");
};

std::wstring Format::underscoredRed(std::wstring string) {
	return (L"\033[4;31m"+string+L"\033[0m");
};

std::wstring Format::underscoredYellow(std::wstring string) {
	return (L"\033[4;33m"+string+L"\033[0m");
};

std::wstring Format::underscoredBlue(std::wstring string) {
	return (L"\033[4;34m"+string+L"\033[0m");
};
