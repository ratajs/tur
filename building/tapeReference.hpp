#pragma once
#include "../common.hpp"

/*!
 * For each tape used in the program, this structure knows when it is used for the first time and when for the last time.
 * More precisely, no instruction may use the tape before the first execution of the instruction with the index of the first reference.
 * And no instruction may use the tape after the first execution of the instruction with the index of the last reference.
 */
struct TapeReference {
	std::optional<size_t> firstReference, lastReference;
};
