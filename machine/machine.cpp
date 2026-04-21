#include "./machine.hpp"
#include "../IO/unexpectedError.hpp"

/*!
 * Repeat those steps while applicable and while the stream is good:
 * If the character is a space, tabulator or newline, consume it from the stream.
 * If the character is a number sign (#), skip the rest of the line.
 * \param stream The input stream.
 */
void Machine::skipWhiteSpaceAndComments(std::wistream &stream) {
	while(stream && (stream.peek()==' ' || stream.peek()=='\t' || stream.peek()=='\n' || stream.peek()=='#')) {
		if(stream.get()=='#') {
			while(stream) {
				if(stream.get()=='\n')
					break;
			};
		};
	};
};

/*!
 * Read a state name from the stream, read from a given input stream while possible.
 * State name can be eny non-empty string not containing digits, whitespaces (space, tabulator, newline) and #.
 * \param stream The input stream.
 * \param state Reference to a string to store the state name in.
 * \param isEndAllowed Whether to allow end of stream directly after the state name.
 * \return Whether the reading was successful.
 */
bool Machine::readState(std::wistream &stream, std::wstring &state, bool isEndAllowed) {
	if(!stream || (stream.peek() >= '0' && stream.peek() <= '9') || !stream) {
		stream.setstate(std::ios::failbit);

		return false;
	};

	state.clear();

	do {
		if(isEndAllowed && stream.eof() && !state.empty()) {
			stream.clear();

			return true;
		};

		state.push_back(stream.get());
	} while(stream && stream.peek()!=' ' && stream.peek()!='\t' && stream.peek()!='\n' && stream.peek()!='#' && (stream.peek() < '0' || stream.peek() > '9'));

	return (stream ? true : false);
};

/*!
 * Read a single 0/1 symbol from a given input stream.
 * \param The input stream.
 * \param symbol Reference to a variable to store the symbol in (false for 0, true for 1).
 * \return Whether the reading was successful.
 */
bool Machine::readSymbol(std::wistream &stream, bool &symbol) {
	if(!stream || (stream.peek()!='0' && stream.peek()!='1') || !stream) {
		stream.setstate(std::ios::failbit);

		return false;
	};

	symbol = (stream.get()=='1');

	return true;
};

/*!
 * Read the direction/move of a transition (L/R/N) from a given input stream.
 * It is case-insensitive and S is allowed as an alias of N.
 * \param stream The input stream.
 * \param direction Reference to a variable to store the direction in.
 * \return Whether the reading was successful.
 */
bool Machine::readDirection(std::wistream &stream, Machine::Direction &direction) {
	if(!stream)
		return false;

	switch(stream.get()) {
		case 'L':
		case 'l':
			direction = Machine::Direction::L;

			break;

		case 'N':
		case 'n':
		case 'S':
		case 's':
			direction = Machine::Direction::N;

			break;

		case 'R':
		case 'r':
			direction = Machine::Direction::R;

			break;

		default:
			stream.setstate(std::ios::failbit);

			return false;
	};

	return true;
};

/*!
 * Read the whole transition (old state, old symbol, new symbol, direction, new state) from a given input stream.
 * \param stream The input stream.
 * \param state Reference to a string to store the name of the old state in.
 * \param symbol Reference to a variable to store the old symbol in (false for 0, true for 1).
 * \param newSymbol Reference to a variable to store the new symbol in (false for 0, true for 1).
 * \param direction Reference to a variable to store the direction in.
 * \param newState Reference to a string to store the name of the new state in.
 * \return Whether the reading was successful.
 */
bool Machine::readTransition(std::wistream &stream, std::wstring &state, bool &symbol, bool &newSymbol, Machine::Direction &direction, std::wstring &newState) {
	if(!stream)
		return false;

	Machine::skipWhiteSpaceAndComments(stream);

	if(!stream) {
		if(stream.eof())
			stream.clear();

		return false;
	};

	if(!Machine::readState(stream, state, false))
		return false;

	Machine::skipWhiteSpaceAndComments(stream);

	if(!Machine::readSymbol(stream, symbol))
		return false;

	Machine::skipWhiteSpaceAndComments(stream);

	if(!Machine::readSymbol(stream, newSymbol))
		return false;

	Machine::skipWhiteSpaceAndComments(stream);

	if(!Machine::readDirection(stream, direction))
		return false;

	Machine::skipWhiteSpaceAndComments(stream);

	if(!Machine::readState(stream, newState, true))
		return false;

	return true;
};

/*!
 * The constructor of Machine.
 * If the machine is invalid, an empty machine with no states or transitions is returned.
 * \param text The definition of the machine as a string.
 */
Machine::Machine(std::wstring text) {
	std::wistringstream iss;

	iss.str(std::move(text));
	iss >> (*this);

	if(iss.fail())
		(*this) = {};
};

/*!
 * Add a new state to the machine if it does not exist yet.
 * \param name The name of the state.
 * \retval true The state was added.
 * \retval false The state already exists.
 */
bool Machine::addState(const std::wstring &name) {
	if(this->statesByNames.contains(name))
		return false;

	this->statesByNames.emplace(name, this->transitions.size());
	this->stateNames.push_back(name);
	this->transitions.emplace_back();

	return true;
};

/*!
 * Simulate one step of this machine on a given tape from a given state.
 * \param tape The tape to work with.
 * \param state Reference to a state identifier (numbers are used, not names), old state is read, new state is written.
 * \param stream If an output stream is provided, the current content of the tape with the current state (before the transition) is printed to it.
 * \param isFormattingEnabled If true (and if the stream is provided), the position on the tape is highlighted with formating sequences.
 * \retval true A transition was found and performed.
 * \retval false No transition was found and the machine halts.
 */
bool Machine::step(Tape &tape, size_t &state, std::optional<std::reference_wrapper<std::wostream>> stream, bool isFormattingEnabled) const {
	bool symbol;
	Machine::Transition transition;

	symbol = tape.readSymbol();

	if(stream) {
		tape.print(*stream, isFormattingEnabled);
		stream->get() << L" " << symbol << L" " << this->stateNames[state] << std::endl;
	};

	if(!this->transitions[state].contains(symbol))
		return false;

	transition = this->transitions[state].at(symbol);
	tape.writeSymbol(transition.newSymbol);
	if(transition.direction==Machine::Direction::L)
		tape.moveLeft();
	else if(transition.direction==Machine::Direction::R)
		tape.moveRight();
	state = transition.newState;

	return true;
};

/*!
 * Add a new transition to the machine.
 * If the states do not exist, they are created.
 * \param state The name of the old state.
 * \param symbol The old symbol (0/1).
 * \param newState The new symbol (0/1).
 * \param direction The direction/move of the machine (L/R/N).
 * \param newState The name of the new state.
 * \retval true The transition was added.
 * \retval false A transition with the same old state and old symbol already exists.
 */
bool Machine::addTransition(const std::wstring &state, bool symbol, bool newSymbol, Machine::Direction direction, const std::wstring &newState) {
	this->addState(state);
	this->addState(newState);

	if(state.empty() || newState.empty())
		throw UnexpectedError(L"Empty string is an invalid state name.");

	if(this->transitions.at(this->statesByNames.at(state)).contains(symbol))
		return false;

	this->transitions.at(this->statesByNames.at(state)).emplace(symbol, Machine::Transition(newSymbol, direction, this->statesByNames.at(newState)));

	return true;
};

/*!
 * Call a closure for each transition of the machine.
 * The function receives: old state, old symbol, new symbol, direction, new state
 * \param function The function to call.
 */
void Machine::forEachTransition(const std::function<void (const std::wstring&, bool, bool, Machine::Direction, const std::wstring&)> &function) const {
	size_t stateIndex = 0;
	std::ranges::for_each(this->transitions,
		[this, &stateIndex, &function](const std::map<bool, Machine::Transition> &transitions) -> void {
			std::ranges::for_each(transitions,
				[this, &stateIndex, &function](const std::pair<bool, Machine::Transition> &transition) -> void {
					function(this->stateNames[stateIndex], transition.first, transition.second.newSymbol, transition.second.direction, this->stateNames[transition.second.newState]);
				}
			);
			stateIndex++;
		}
	);
};

/*!
 * Simplify the machine by joining subsequent N-transitions with the first L- or R-transition.
 * \return Reference to this machine.
 */
Machine &Machine::removeNTransitions() {
	size_t stateIndex = 0;

	std::ranges::for_each(this->transitions,
		[this, &stateIndex](std::map<bool, Machine::Transition> &transitions) -> void {
			std::set<bool> toDelete;

			std::ranges::for_each(transitions,
				[this, stateIndex, &toDelete](const std::pair<bool, Machine::Transition> &transition) -> void {
					size_t state, symbol;
					std::set<std::pair<bool, size_t>> visited;

					if(transition.second.direction!=Machine::Direction::N)
						return;

					state = stateIndex;
					symbol = transition.first;
					do {
						if(!visited.emplace(symbol, state).second) { // A loop from N-transitions detected
							this->transitions[stateIndex].at(transition.first) = Machine::Transition(symbol, Machine::Direction::N, stateIndex);

							return;
						};

						std::tie(state, symbol) = std::pair(this->transitions[state].at(symbol).newState, this->transitions[state].at(symbol).newSymbol);

						if(!this->transitions[state].contains(symbol)) {
							if(transition.first!=symbol) // The machine should change the symbol before it exits
								this->transitions[stateIndex].at(transition.first) = Machine::Transition(symbol, Machine::Direction::N, state);
							else // The N-transitions are a dead-end and do not do anything
								toDelete.insert(transition.first);

							return;
						};
					} while(this->transitions[state].at(symbol).direction==Machine::Direction::N);

					this->transitions[stateIndex].at(transition.first) = this->transitions[state].at(symbol);
				}
			);

			if(toDelete.contains(true))
				transitions.erase(true);

			if(toDelete.contains(false))
				transitions.erase(false);

			stateIndex++;
		}
	);

	return (*this);
};

/*!
 * Simplify the machine by removing all inaccessible states.
 */
Machine &Machine::removeInaccessibleStates() {
	size_t state, statesCount;
	std::vector<std::optional<size_t>> newStatesByOldStates;
	std::vector<std::wstring> stateNames;
	std::vector<std::map<bool, Machine::Transition>> transitions;
	std::queue<size_t> queue;
	std::map<std::wstring, size_t> statesByNames;

	if(this->transitions.empty())
		return (*this);

	newStatesByOldStates.resize(this->transitions.size());
	newStatesByOldStates[0] = 0;

	queue.push(0);

	statesCount = 1;
	while(!queue.empty()) {
		state = queue.front();
		queue.pop();

		if(this->transitions[state].contains(true) && !newStatesByOldStates[this->transitions[state].at(true).newState]) {
			newStatesByOldStates[this->transitions[state].at(true).newState] = (statesCount++);
			queue.push(this->transitions[state].at(true).newState);
		};

		if(this->transitions[state].contains(false) && !newStatesByOldStates[this->transitions[state].at(false).newState]) {
			newStatesByOldStates[this->transitions[state].at(false).newState] = (statesCount++);
			queue.push(this->transitions[state].at(false).newState);
		};
	};

	transitions.resize(statesCount);
	state = 0;
	std::ranges::for_each(this->transitions,
		[&state, &newStatesByOldStates, &transitions](std::map<bool, Transition> transition) -> void {
			if(newStatesByOldStates[state]) {
				if(transition.contains(true))
					transition.at(true).newState = (*newStatesByOldStates[transition.at(true).newState]);
				if(transition.contains(false))
					transition.at(false).newState = (*newStatesByOldStates[transition.at(false).newState]);

				transitions[*newStatesByOldStates[state]] = (std::move(transition));
			};
			state++;
		}
	);

	stateNames.resize(statesCount);
	state = 0;
	std::ranges::for_each(this->stateNames,
		[&state, &newStatesByOldStates, &stateNames, &statesByNames](const std::wstring &stateName) -> void {
			if(newStatesByOldStates[state]) {
				stateNames[*newStatesByOldStates[state]] = stateName;
				statesByNames.emplace(stateName, *newStatesByOldStates[state]);
			};
			state++;
		}
	);

	this->transitions = std::move(transitions);
	this->stateNames = std::move(stateNames);
	this->statesByNames = std::move(statesByNames);

	return (*this);
};

/*!
 * Simulate this machine on a given tape.
 * \param tape The tape to work with.
 * \param shouldKeepBlanks If false, the leading and trailing zeros on the tape are disregarded (not printed and cleared at the end), otherwise, visiting a place on the tape ensures that the place remains.
 * \param lastState If a reference wrapper is provided, the name of the state in which the machine halts is written there.
 * \param stream If an output stream is provided, the current content of the tape and the current state is printed to it between every two subsequent transitions.
 * \param isFormattingEnabled If true (and if the stream is provided), the position on the tape is highlighted with formating sequences.
 * \return False, if the machine is empty, true otherwise (no return if the machine does not halt).
 */
bool Machine::run(Tape &tape, bool shouldKeepBlanks, std::optional<std::reference_wrapper<std::wstring>> lastState, std::optional<std::reference_wrapper<std::wostream>> stream, bool isFormattingEnabled) const {
	size_t state = 0;

	if(this->transitions.empty())
		return false;

	if(!shouldKeepBlanks && stream) {
		do
			tape.clearBlanks();
		while(this->step(tape, state, stream, isFormattingEnabled));
	}
	else
		while(this->step(tape, state, stream, isFormattingEnabled));

	if(!shouldKeepBlanks)
		tape.clearBlanks();

	if(lastState)
		lastState->get() = this->stateNames[state];

	return true;
};

/*!
 * \return Whether the machine is non-empty (freshly created machine or a machine after a failed reading is empty).
 */
Machine::operator bool() const {
	return (!this->transitions.empty());
};

/*!
 * Prints all the transitions of the machine to a given output stream.
 * No delimiter is printed at the beginning nor at the end.
 * \param stream The output stream.
 * \param columnDelimiter A string to use as the column delimiter.
 * \param rowDelimiter A string to use as the row delimiter.
 */
void Machine::print(std::wostream &stream, std::wstring_view columnDelimiter, std::wstring_view rowDelimiter) const {
	bool isFirstTransition = true;
	size_t stateIndex = 0;

	if(!(*this)) { // Empty machine are not always considered valid
		stream
			<< L"A" << columnDelimiter
			<< L"0" << columnDelimiter
			<< L"0" << columnDelimiter
			<< L"N" << columnDelimiter
			<< L"_";
	};

	std::ranges::for_each(this->transitions,
		[this, &stream, &stateIndex, &isFirstTransition, columnDelimiter, rowDelimiter](const std::map<bool, Machine::Transition> &transitions) -> void {
			std::ranges::for_each(transitions,
				[this, &stream, stateIndex, &isFirstTransition, columnDelimiter, rowDelimiter](const std::pair<bool, Machine::Transition> &transition) -> void {
					if(!std::exchange(isFirstTransition, false))
						stream << rowDelimiter;
					stream << this->stateNames[stateIndex] << columnDelimiter << transition.first << columnDelimiter << transition.second.newSymbol << columnDelimiter;
					switch(transition.second.direction) {
						case Machine::Direction::L:
							stream << L"L";

							break;

						case Machine::Direction::N:
							stream << L"N";

							break;

						case Machine::Direction::R:
							stream << L"R";

							break;
					};
					stream << columnDelimiter << this->stateNames[transition.second.newState];
				}
			);
			stateIndex++;
		}
	);
};

/*!
 * Print the machine to a stream.
 * \param stream The output stream.
 * \param machine The machine.
 * \return The same stream.
 */
std::wostream &operator << (std::wostream &stream, const Machine &machine) {
	machine.print(stream);

	return stream;
};

/*!
 * Read a machine from a stream.
 * \param stream The input stream.
 * \param machine The machine (destination).
 * \return The same stream.
*/
std::wistream &operator >> (std::wistream &stream, Machine &machine) {
	bool symbol, newSymbol;
	Machine::Direction direction;
	std::wstring state, newState;

	if(!stream)
		return stream;

	while(Machine::readTransition(stream, state, symbol, newSymbol, direction, newState))
		machine.addTransition(state, symbol, newSymbol, direction, newState);

	return stream;
};
