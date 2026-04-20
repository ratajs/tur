#pragma once
#include "../common.hpp"

/*!
 * This is a structor which is created for every variable.
 * It can later store the tape which is assigned to the variable.
 */
struct Variable {
	std::optional<size_t> tape;
};
