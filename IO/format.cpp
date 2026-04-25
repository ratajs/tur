#include "./format.hpp"

/*!
 * Enclose the string with escape sequences, so that it is red in the terminal.
 * \param string The string to format.
 * \return The same string with the escape sentences.
 */
std::wstring Format::red(std::wstring string) {
	return (L"\033[31m"+string+L"\033[0m");
};

/*!
 * Enclose the string with escape sequences, so that it is yellow in the terminal.
 * \param string The string to format.
 * \return The same string with the escape sentences.
 */
std::wstring Format::yellow(std::wstring string) {
	return (L"\033[33m"+string+L"\033[0m");
};

/*!
 * Enclose the string with escape sequences, so that it is blue in the terminal.
 * \param string The string to format.
 * \return The same string with the escape sentences.
 */
std::wstring Format::blue(std::wstring string) {
	return (L"\033[34m"+string+L"\033[0m");
};

/*!
 * Enclose the string with escape sequences, so that it is red and bold in the terminal.
 * \param string The string to format.
 * \return The same string with the escape sentences.
 */
std::wstring Format::boldRed(std::wstring string) {
	return (L"\033[1;31m"+string+L"\033[0m");
};

/*!
 * Enclose the string with escape sequences, so that it is yellow and bold in the terminal.
 * \param string The string to format.
 * \return The same string with the escape sentences.
 */
std::wstring Format::boldYellow(std::wstring string) {
	return (L"\033[1;33m"+string+L"\033[0m");
};

/*!
 * Enclose the string with escape sequences, so that it is blue and bold in the terminal.
 * \param string The string to format.
 * \return The same string with the escape sentences.
 */
std::wstring Format::boldBlue(std::wstring string) {
	return (L"\033[1;34m"+string+L"\033[0m");
};

/*!
 * Enclose the string with escape sequences, so that it is red and underscored in the terminal.
 * \param string The string to format.
 * \return The same string with the escape sentences.
 */
std::wstring Format::underscoredRed(std::wstring string) {
	return (L"\033[4;31m"+string+L"\033[0m");
};

/*!
 * Enclose the string with escape sequences, so that it is yellow and underscored in the terminal.
 * \param string The string to format.
 * \return The same string with the escape sentences.
 */
std::wstring Format::underscoredYellow(std::wstring string) {
	return (L"\033[4;33m"+string+L"\033[0m");
};

/*!
 * Enclose the string with escape sequences, so that it is blue and underscored in the terminal.
 * \param string The string to format.
 * \return The same string with the escape sentences.
 */
std::wstring Format::underscoredBlue(std::wstring string) {
	return (L"\033[4;34m"+string+L"\033[0m");
};
