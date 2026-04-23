#include "./stateGenerator.hpp"
#include "../IO/unexpectedError.hpp"

/*!
 * \return The state preceding the latest state in order of generation.
 * \throw If such a state does not exist, i.e. the current state is the first one.
 */
std::wstring StateGenerator::getLastState() const {
	if(this->lastState.empty())
		throw UnexpectedError(L"No last state yet.");

	return this->lastState;
};

/*!
 * \return The latest state.
 */
std::wstring StateGenerator::getCurrentState() const {
	return this->currentState;
};

/*!
 * Generate the next state name.
 */
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
