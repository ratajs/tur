#pragma once
#include "../common.hpp"
#include "./machineFactory.hpp"
#include "./stateGenerator.hpp"
#include "./machine.hpp"

class MultiTapeMachineFactory: public MachineFactory {
	private:
		size_t tapesCount;

		// Super-Transitions
		template<bool symbol, bool newSymbol, Machine::Direction direction> requires (direction!=Machine::Direction::N)
			std::wstring addSuperTransition(std::optional<std::wstring> state, std::optional<std::wstring> newState, size_t tape = 0);
		template<Machine::Direction direction> requires (direction!=Machine::Direction::N)
			std::wstring addSuperTransition(std::optional<std::wstring> state, std::optional<std::wstring> newState, size_t tape = 0);

		// General
		std::wstring goHome(std::optional<std::wstring> returningState, std::optional<std::wstring> endState, size_t tape = 0); // Mark first symbol if tape ≠ 0
		std::wstring findTape(size_t tape, std::optional<std::wstring> startState, std::optional<std::wstring> endState, bool isEmptyTapeAllowed = true);
		std::wstring skipNumber(std::optional<std::wstring> skippingState, std::optional<std::wstring> endState, size_t tape = 0); // Mark tape 0 if tape ≠ 0
		std::wstring clearNumber(std::optional<std::wstring> clearingState, std::optional<std::wstring> endState);
		std::wstring findEnd(std::optional<std::wstring> findingEndState, std::optional<std::wstring> endState, size_t tape = 0); // Mark tape 0 if tape ≠ 0
		// For copying
		std::wstring placeSymbol(std::optional<std::wstring> goingToTheEndWith1State, std::optional<std::wstring> goingToTheEndWith0State, std::optional<std::wstring> endState, size_t tapeB);
		std::wstring returnToMark(std::optional<std::wstring> returningToAState, std::optional<std::wstring> endState, size_t tapeA, size_t tapeB);
		std::wstring copySingleNumber(std::optional<std::wstring> startState, std::optional<std::wstring> endState, size_t tapeA, size_t tapeB);
		std::wstring copySingleNumberAndZero(std::optional<std::wstring> startState, std::optional<std::wstring> endState, size_t tapeA, size_t tapeB);
		std::wstring copyFromHere(std::optional<std::wstring> startState, std::optional<std::wstring> endState, size_t tapeA, size_t tapeB);
		std::wstring copyFromHere(std::optional<std::wstring> startState, std::optional<std::wstring> endState, size_t tapeA, size_t tapeB, size_t count);
		std::wstring copyIfNonEmpty(std::optional<std::wstring> readyState, std::optional<std::wstring> endState, size_t tapeA, size_t tapeB, size_t beginA);
		// For comparing
		std::wstring addDummyZero(size_t tape, std::optional<std::wstring> startState, std::optional<std::wstring> endString);
		std::wstring removeDummyZero(size_t tape, std::optional<std::wstring> startState, std::optional<std::wstring> endString);


	public:
		MultiTapeMachineFactory(size_t tapesCount);
		MultiTapeMachineFactory &decompress(size_t inputTape);
		MultiTapeMachineFactory &compress(size_t outputTape);
		MultiTapeMachineFactory &clearBeginning(size_t tape, size_t count);
		MultiTapeMachineFactory &clearEnd(size_t tape, size_t begin);
		MultiTapeMachineFactory &writeNumber(size_t tape, size_t begin, size_t number);
		MultiTapeMachineFactory &appendNumber(size_t tape, size_t number);
		MultiTapeMachineFactory &copyAll(size_t tapeA, size_t beginA, size_t tapeB, size_t beginB);
		MultiTapeMachineFactory &appendAll(size_t tapeA, size_t beginA, size_t tapeB);
		MultiTapeMachineFactory &copy(size_t tapeA, size_t beginA, size_t tapeB, size_t beginB, size_t count);
		MultiTapeMachineFactory &append(size_t tapeA, size_t beginA, size_t tapeB, size_t count);
		MultiTapeMachineFactory &simulate(size_t tape, const Machine &machine);
		MultiTapeMachineFactory &compareWithConstant(size_t tape, size_t index, size_t number, std::wstring lessThanState, std::wstring equalState, std::wstring greaterThanState);
		MultiTapeMachineFactory &compare(size_t tapeA, size_t indexA, size_t tapeB, size_t indexB, std::wstring lessThanState, std::wstring equalState, std::wstring greaterThanState);
		MultiTapeMachineFactory &compare(size_t tape, size_t indexA, size_t indexB, std::wstring lessThanState, std::wstring equalState, std::wstring greaterThanState);
		MultiTapeMachineFactory &compareTapeLength(size_t tape, size_t number, std::wstring lessThanState, std::wstring equalState, std::wstring greaterThanState);
};
