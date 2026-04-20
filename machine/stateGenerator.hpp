#pragma once
#include "../common.hpp"

class StateGenerator {
	private:
		std::wstring lastState;
		std::wstring currentState = L"A";

	public:
		std::wstring getLastState() const;
		std::wstring getCurrentState() const;
		void increment();
};
