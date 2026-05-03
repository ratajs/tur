#pragma once
#include "./machine.hpp"

/*!
 * These are predefined machines for arithmetical operations:
 * - INC: Increment the first number on the tape. Undefined behavior if the tape is empty.
 * - DEC: Decrement the first number on the tape. Undefined behavior if the tape is empty.
 * - ADD: Add two numbers. Undefined behavior if the tape does not contain exactly two numbers.
 * - SUB: Subtract two numbers, 0 is returned if the second number is greater. Undefined behavior if the tape does not contain exactly two numbers.
 * - MUL: Multiply two numbers. Undefined behavior if the tape does not contain exactly two numbers.
 * - DIV: Divide two numbers (integer division). Undefined behavior if the tape does not contain exactly two numbers or if the second number is zero.
 * - MOD: Divide two numbers (integer division), get the remainder. Undefined behavior if the tape does not contain exactly two numbers or if the second number is zero.
 * - MIN: Get the smaller of the numbers. Undefined behavior if the tape does not contain exactly two numbers or if the second number is zero.
 * - MAX: Get the greater of the numbers. Undefined behavior if the tape does not contain exactly two numbers or if the second number is zero.
 * - COUNT: Count the numbers on the tape.
 */
namespace MachineLibrary {
	extern const Machine INC;
	extern const Machine DEC;
	extern const Machine ADD;
	extern const Machine SUB;
	extern const Machine MUL;
	extern const Machine DIV;
	extern const Machine MOD;
	extern const Machine MIN;
	extern const Machine MAX;
	extern const Machine COUNT;
};
