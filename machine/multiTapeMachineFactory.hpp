#pragma once
#include <cstdlib>
#include <optional>
#include <string>
#include "./machineFactory.hpp"
#include "./machine.hpp"

/*!
 * This class generates machines with virtual tapes.
 * It implements several algorithms which can be called subsequently to generate machines doing complex programs.
 */
class MultiTapeMachineFactory: public MachineFactory {
	private:
		size_t tapesCount;

		// Super-Transitions
		template<bool symbol, bool newSymbol, Machine::Direction direction> requires (direction!=Machine::Direction::N)
			std::wstring addSuperTransition(std::optional<std::wstring> state, std::optional<std::wstring> newState, size_t tape = 0);
		template<Machine::Direction direction> requires (direction!=Machine::Direction::N)
			std::wstring addSuperTransition(std::optional<std::wstring> state, std::optional<std::wstring> newState, size_t tape = 0);

		// Tape management
		void invertTapeNumberIfNecessary(size_t &tapeNumber) const;

		// General
		std::wstring goHome(std::optional<std::wstring> returningState, std::optional<std::wstring> endState, size_t tape = 0); // Mark first symbol if tape ≠ 0
		std::wstring findTape(size_t tape, std::optional<std::wstring> startState, std::optional<std::wstring> endState, bool isEmptyTapeAllowed = true);
		std::wstring skipNumber(std::optional<std::wstring> skippingState, std::optional<std::wstring> endState, size_t tape = 0); // Mark tape 0 if tape ≠ 0
		std::wstring skipNumberFromEnd(std::optional<std::wstring> startState, std::optional<std::wstring> endState);
		std::wstring clearNumber(std::optional<std::wstring> clearingState, std::optional<std::wstring> endState);
		std::wstring findEnd(std::optional<std::wstring> findingEndState, std::optional<std::wstring> endState, size_t tape = 0); // Mark tape 0 if tape ≠ 0
		// For copying
		std::wstring placeSymbol(std::optional<std::wstring> goingToTheEndWith1State, std::optional<std::wstring> goingToTheEndWith0State, std::optional<std::wstring> endState, size_t tapeB);
		std::wstring returnToMark(std::optional<std::wstring> returningToAState, std::optional<std::wstring> endState, size_t tapeA, size_t tapeB);
		std::wstring copySingleNumber(std::optional<std::wstring> startState, std::optional<std::wstring> endState, size_t tapeA, size_t tapeB);
		std::wstring copySingleNumberAndZero(std::optional<std::wstring> startState, std::optional<std::wstring> endState, size_t tapeA, size_t tapeB);
		std::wstring copyFromHere(std::optional<std::wstring> startState, std::optional<std::wstring> endState, size_t tapeA, size_t tapeB);
		std::wstring copyFromHere(std::optional<std::wstring> startState, std::optional<std::wstring> endState, size_t tapeA, size_t tapeB, size_t count);
		std::wstring copyIfNonEmpty(std::optional<std::wstring> readyState, std::optional<std::wstring> endState, size_t tapeA, size_t tapeB, size_t beginA, bool isBeginAFromEnd = false);
		// For comparing
		std::wstring addDummyZero(size_t tape, std::optional<std::wstring> startState, std::optional<std::wstring> endString);
		std::wstring removeDummyZero(size_t tape, std::optional<std::wstring> startState, std::optional<std::wstring> endString);


	public:
		MultiTapeMachineFactory(size_t tapesCount);
		MultiTapeMachineFactory &decompress(size_t inputTape);
		MultiTapeMachineFactory &compress(size_t outputTape);
		MultiTapeMachineFactory &clearBeginning(size_t tape, size_t count);
		MultiTapeMachineFactory &clearEnd(size_t tape, size_t begin, bool isBeginFromEnd = false);
		MultiTapeMachineFactory &writeNumber(size_t tape, size_t begin, size_t number, bool isBeginFromEnd = false);
		MultiTapeMachineFactory &appendNumber(size_t tape, size_t number);
		MultiTapeMachineFactory &copyAll(size_t tapeA, size_t beginA, size_t tapeB, size_t beginB, bool isBeginAFromEnd = false, bool isBeginBFromEnd = false);
		MultiTapeMachineFactory &appendAll(size_t tapeA, size_t beginA, size_t tapeB, bool isBeginAFromEnd = false);
		MultiTapeMachineFactory &copy(size_t tapeA, size_t beginA, size_t tapeB, size_t beginB, size_t count, bool isBeginAFromEnd = false, bool isBeginBFromEnd = false);
		MultiTapeMachineFactory &append(size_t tapeA, size_t beginA, size_t tapeB, size_t count, bool isBeginAFromEnd = false);
		MultiTapeMachineFactory &simulate(size_t tape, const Machine &machine);
		MultiTapeMachineFactory &compareWithConstant(size_t tape, size_t index, size_t number, std::wstring lessThanState, std::wstring equalState, std::wstring greaterThanState);
		MultiTapeMachineFactory &compare(size_t tapeA, size_t indexA, size_t tapeB, size_t indexB, std::wstring lessThanState, std::wstring equalState, std::wstring greaterThanState, bool isIndexBFromEnd = false);
		MultiTapeMachineFactory &compare(size_t tape, size_t indexA, size_t indexB, std::wstring lessThanState, std::wstring equalState, std::wstring greaterThanState, bool isIndexBFromEnd = false);
		MultiTapeMachineFactory &compareTapeLength(size_t tape, size_t number, std::wstring lessThanState, std::wstring equalState, std::wstring greaterThanState);
		MultiTapeMachineFactory &reverse();
};
