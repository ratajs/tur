#pragma once
#include "../common.hpp"

/*!
 * This class generates state names for MachineFactory.
 * The first state is A, next states follow alphabetically.
 * After Z, the next states are AA, AB, AC and so on, after AZ goes BA, after ZZ goes AAA.
 */
class StateGenerator {
	private:
		std::wstring lastState;
		std::wstring currentState = L"A";

	public:
		std::wstring getLastState() const;
		std::wstring getCurrentState() const;
		void increment();
};
