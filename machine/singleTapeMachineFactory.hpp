#pragma once
#include "../common.hpp"
#include "./machineFactory.hpp"
#include "./stateGenerator.hpp"
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
		void compareAdjacentNumbers(std::wstring lessThanState, std::wstring equalState, std::wstring greaterThanState);
		void compare(size_t distance, std::wstring lessThanState, std::wstring equalState, std::wstring greaterThanState);

	public:
		SingleTapeMachineFactory &clearBeginning(size_t count);
		SingleTapeMachineFactory &clearEnd(size_t begin);
		SingleTapeMachineFactory &writeNumber(size_t begin, size_t number);
		SingleTapeMachineFactory &appendNumber(size_t number);
		SingleTapeMachineFactory &simulate(const Machine &machine);
		SingleTapeMachineFactory &compareWithConstant(size_t index, size_t number, std::wstring lessThanState, std::wstring equalState, std::wstring greaterThanState);
		SingleTapeMachineFactory &compare(size_t indexA, size_t indexB, std::wstring lessThanState, std::wstring equalState, std::wstring greaterThanState);
		SingleTapeMachineFactory &compareTapeLength(size_t number, std::wstring lessThanState, std::wstring equalState, std::wstring greaterThanState);
};
