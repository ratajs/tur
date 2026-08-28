#pragma once
#include <cstdlib>
#include <optional>
#include <string>
#include "./machineFactory.hpp"
#include "./machine.hpp"

/*!
 * This class generates machines without virtual tapes.
 * It implements several algorithms which can be called subsequently to generate machines doing complex programs.
 */
class SingleTapeMachineFactory: public MachineFactory {
	private:
		std::wstring goHome(std::optional<std::wstring> returningState, std::optional<std::wstring> endState);
		std::wstring findEnd(std::optional<std::wstring> findingEndState, std::optional<std::wstring> endState);
		template<Machine::Direction direction = Machine::Direction::R>
			std::wstring skipNumber(std::optional<std::wstring> skippingState, std::optional<std::wstring> endState);
		std::wstring clearNumber(std::optional<std::wstring> clearingState, std::optional<std::wstring> endState);
		std::wstring copyNumberFromEndToTheBeginning(size_t indexFromEnd, std::optional<std::wstring> startState, std::optional<std::wstring> endState);
		void compareAdjacentNumbers(std::wstring lessThanState, std::wstring equalState, std::wstring greaterThanState);
		void compare(size_t distance, std::optional<std::wstring> startState, std::wstring lessThanState, std::wstring equalState, std::wstring greaterThanState);

	public:
		SingleTapeMachineFactory &clearBeginning(size_t count);
		SingleTapeMachineFactory &clearEnd(size_t begin, bool isBeginFromEnd = false);
		SingleTapeMachineFactory &writeNumber(size_t begin, size_t number, bool isBeginFromEnd = false);
		SingleTapeMachineFactory &appendNumber(size_t number);
		SingleTapeMachineFactory &simulate(const Machine &machine);
		SingleTapeMachineFactory &compareWithConstant(size_t index, size_t number, std::wstring lessThanState, std::wstring equalState, std::wstring greaterThanState);
		SingleTapeMachineFactory &compare(size_t indexA, size_t indexB, std::wstring lessThanState, std::wstring equalState, std::wstring greaterThanState, bool isIndexBFromEnd = false);
		SingleTapeMachineFactory &compareTapeLength(size_t number, std::wstring lessThanState, std::wstring equalState, std::wstring greaterThanState);
		SingleTapeMachineFactory &reverse();
};
