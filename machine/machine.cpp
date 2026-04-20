#include "./machine.hpp"
#include "../IO/unexpectedError.hpp"

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

bool Machine::readSymbol(std::wistream &stream, bool &symbol) {
	if(!stream || (stream.peek()!='0' && stream.peek()!='1') || !stream) {
		stream.setstate(std::ios::failbit);

		return false;
	};

	symbol = (stream.get()=='1');

	return true;
};

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

Machine::Machine(std::wstring text) {
	std::wistringstream iss;

	iss.str(std::move(text));
	iss >> (*this);

	if(iss.fail())
		(*this) = {};
};

bool Machine::addState(const std::wstring &name) {
	if(this->statesByNames.contains(name))
		return false;

	this->statesByNames.emplace(name, this->transitions.size());
	this->stateNames.push_back(name);
	this->transitions.emplace_back();

	return true;
};

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

Machine::operator bool() const {
	return (!this->transitions.empty());
};

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

std::wostream &operator << (std::wostream &stream, const Machine &machine) {
	machine.print(stream);

	return stream;
};

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
