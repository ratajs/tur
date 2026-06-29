#pragma once
#include <iostream>

/*!
 * This holds the input language, either TUR, or IR.
 */
enum class InputLanguage {
	TUR,
	IR
};

std::wistream &operator >> (std::wistream &stream, InputLanguage &language);
