#include "./stateGenerator.hpp"
#include "../IO/unexpectedError.hpp"

std::wstring StateGenerator::getLastState() const {
	if(this->lastState.empty())
		throw UnexpectedError(L"No last state yet.");

	return this->lastState;
};

std::wstring StateGenerator::getCurrentState() const {
	return this->currentState;
};

void StateGenerator::increment() {
	std::wstring::reverse_iterator it;

	this->lastState = this->currentState;

	for(it = this->currentState.rbegin(); it < this->currentState.rend(); it++) {
		if((*it)=='Z')
			(*it) = 'A';
		else {
			(*it)++;

			return;
		};
	};

	this->currentState.insert(this->currentState.begin(), 'A');
};
