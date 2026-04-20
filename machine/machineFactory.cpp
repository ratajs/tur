#include "./machineFactory.hpp"
#include "../IO/unexpectedError.hpp"

/*!
 * Links several machines together.
 * The computation would start with the first machine; whenever a machine can stop (except for the last one), the next machine starts.
 * All states will be renamed in alphabetical order.
 * \param machines The machines to be composed.
 * \return The resulting machine.
 */
Machine MachineFactory::compose(const std::vector<Machine> &machines) {
	size_t machineIndex = 0, machinesCount;
	Machine newMachine;
	StateGenerator generator;

	machinesCount = machines.size();
	std::ranges::for_each(machines,
		[&newMachine, &generator, &machineIndex, machinesCount](const Machine &machine) -> void {
			std::vector<std::pair<std::wstring, bool>> missingTransitions;
			std::set<std::wstring> unusedStates;
			std::map<std::wstring, std::wstring> newStatesByOldStates;

			machine.forEachTransition(
				[&newMachine, &generator, &unusedStates, &missingTransitions, &newStatesByOldStates](const std::wstring &state, bool symbol, bool newSymbol, Machine::Direction direction, const std::wstring &newState) -> void {
					if(!newStatesByOldStates.contains(state)) {
						newStatesByOldStates.emplace(state, generator.getCurrentState());
						generator.increment();
					}
					else
						unusedStates.erase(newStatesByOldStates.at(state));

					if(!newStatesByOldStates.contains(newState)) {
						newStatesByOldStates.emplace(newState, generator.getCurrentState());
						unusedStates.insert(generator.getCurrentState());
						generator.increment();
					};

					newMachine.addTransition(newStatesByOldStates.at(state), symbol, newSymbol, direction, newStatesByOldStates.at(newState));

					if(!missingTransitions.empty() && missingTransitions.back().first==newStatesByOldStates.at(state))
						missingTransitions.pop_back();
					else
						missingTransitions.emplace_back(newStatesByOldStates.at(state), !symbol);
				}
			);

			if((++machineIndex)==machinesCount)
				return;

			std::ranges::for_each(missingTransitions,
				[&newMachine, &generator](const std::pair<std::wstring, bool> &missingTransition) -> void {
					newMachine.addTransition(missingTransition.first, missingTransition.second, missingTransition.second, Machine::Direction::N, generator.getCurrentState());
				}
			);

			std::ranges::for_each(unusedStates,
				[&newMachine, &generator](const std::wstring &unusedState) -> void {
					newMachine.addTransition(unusedState, true, true, Machine::Direction::N, generator.getCurrentState());
					newMachine.addTransition(unusedState, false, false, Machine::Direction::N, generator.getCurrentState());
				}
			);
		}
	);

	return newMachine;
};

/*!
 * Add a transition to the machine.
 * The incoming and the outgoing state is the same.
 * \tparam symbol The symbol on which the transition will be.
 * \tparam newSymbol The new symbol.
 * \tparam direction The direction of the transition.
 * \param state The incoming/outgoing state, the current state if {}.
 * \return The outgoing state.
 */
template<bool symbol, bool newSymbol, Machine::Direction direction>
	std::wstring MachineFactory::addTransition(std::optional<std::wstring> state) {
		if(!state)
			state = this->generator.getCurrentState();

		this->machine.addTransition(*state, symbol, newSymbol, direction, *state);

		return (*state);
	};

/*!
 * Add a transition to the machine.
 * \tparam symbol The symbol on which the transition will be.
 * \tparam newSymbol The new symbol.
 * \tparam direction The direction of the transition.
 * \param state The incoming state, the current state if {}.
 * \param newState The outgoing state, a new state if {} (which will then be the current state).
 * \return The outgoing state.
 */
template<bool symbol, bool newSymbol, Machine::Direction direction>
	std::wstring MachineFactory::addTransition(std::optional<std::wstring> state, std::optional<std::wstring> newState) {
		if(!state)
			state = this->generator.getCurrentState();

		if(!newState) {
			this->generator.increment();
			newState = this->generator.getCurrentState();
		};

		this->machine.addTransition(*state, symbol, newSymbol, direction, *newState);

		return (*newState);
	};

/*!
 * Add a transition to the machine.
 * The incoming and the outgoing state is the same.
 * The transition will not manipulate with the symbols (so it will in fact be two transitions, 0/0 and 1/1).
 * \tparam direction The direction of the transition.
 * \param state The incoming/outgoing state, the current state if {}.
 * \return The outgoing state.
 */
template<Machine::Direction direction>
	std::wstring MachineFactory::addTransition(std::optional<std::wstring> state) {
		if(!state)
			state = this->generator.getCurrentState();

		this->machine.addTransition(*state, false, false, direction, *state);
		this->machine.addTransition(*state, true, true, direction, *state);

		return (*state);
	};

/*!
 * Add a transition to the machine.
 * The transition will not manipulate with the symbols (so it will in fact be two transitions, 0/0 and 1/1).
 * \tparam direction The direction of the transition.
 * \param state The incoming state, the current state if {}.
 * \param newState The outgoing state, a new state if {} (which will then be the current state).
 * \return The outgoing state.
 */
template<Machine::Direction direction>
	std::wstring MachineFactory::addTransition(std::optional<std::wstring> state, std::optional<std::wstring> newState) {
		if(!state)
			state = this->generator.getCurrentState();

		if(!newState) {
			this->generator.increment();
			newState = this->generator.getCurrentState();
		};

		this->machine.addTransition(*state, false, false, direction, *newState);
		this->machine.addTransition(*state, true, true, direction, *newState);

		return (*newState);
	};

/*!
 * Retrieve the machine.
 * The MachineFactory object will be reset to contain an empty machine.
 * \return The finished machine.
 */
Machine MachineFactory::extractMachine() {
	Machine machine;

	machine = std::move(this->machine);
	this->machine = {};
	this->generator = {};

	return machine;
};

/*!
 * Creates special states while still allowing the use of the current state.
 * \param count Number of new states.
 * \return Vector of those states.
 */
std::vector<std::wstring> MachineFactory::createExtraStates(size_t count) {
	size_t x;
	std::wstring currentState;
	std::vector<std::wstring> extraStates;

	if(count==0)
		return extraStates;

	currentState = this->generator.getCurrentState();
	for(x = 0; x < count; x++) {
		this->generator.increment();
		extraStates.push_back(this->generator.getCurrentState());
		this->extraStates.insert(this->generator.getCurrentState());
	};
	this->addTransition<Machine::Direction::N>(currentState, {});

	return extraStates;
};

/*!
 * A public method to create an N-transition.
 * The incoming state has to be either the current state or a state created with createExtraStates().
 * The outgoing state has to be either a new state or a state created with createExtraStates().
 * If the outgoing state is an extra state, the current state will be blocked.
 * \param stateA The incoming state, {} for the current state; otherwise, it has to be an extra state.
 * \param stateB The outgoing state, {} for a new state (with will then be the current state); otherwise, it has to be an extra state.
 */
void MachineFactory::addNTransition(std::optional<std::wstring> stateA, std::optional<std::wstring> stateB) {
	if((stateA && !this->extraStates.contains(*stateA)) || (stateB && !this->extraStates.contains(*stateB)))
		throw UnexpectedError(L"States given to the machine factory must be extra states.");

	if(!stateA && this->isCurrentStateDisabled)
		throw UnexpectedError(L"Current state is disabled.");

	this->addTransition<Machine::Direction::N>(stateA, stateB);

	this->isCurrentStateDisabled = stateB.has_value();
};

template std::wstring MachineFactory::addTransition<false, false, Machine::Direction::L>(std::optional<std::wstring> state);
template std::wstring MachineFactory::addTransition<false, false, Machine::Direction::R>(std::optional<std::wstring> state);
template std::wstring MachineFactory::addTransition<false, false, Machine::Direction::N>(std::optional<std::wstring> state);
template std::wstring MachineFactory::addTransition<false, true, Machine::Direction::L>(std::optional<std::wstring> state);
template std::wstring MachineFactory::addTransition<false, true, Machine::Direction::R>(std::optional<std::wstring> state);
template std::wstring MachineFactory::addTransition<false, true, Machine::Direction::N>(std::optional<std::wstring> state);
template std::wstring MachineFactory::addTransition<true, false, Machine::Direction::L>(std::optional<std::wstring> state);
template std::wstring MachineFactory::addTransition<true, false, Machine::Direction::R>(std::optional<std::wstring> state);
template std::wstring MachineFactory::addTransition<true, false, Machine::Direction::N>(std::optional<std::wstring> state);
template std::wstring MachineFactory::addTransition<true, true, Machine::Direction::L>(std::optional<std::wstring> state);
template std::wstring MachineFactory::addTransition<true, true, Machine::Direction::R>(std::optional<std::wstring> state);
template std::wstring MachineFactory::addTransition<true, true, Machine::Direction::N>(std::optional<std::wstring> state);
template std::wstring MachineFactory::addTransition<false, false, Machine::Direction::L>(std::optional<std::wstring> state, std::optional<std::wstring> newState);
template std::wstring MachineFactory::addTransition<false, false, Machine::Direction::R>(std::optional<std::wstring> state, std::optional<std::wstring> newState);
template std::wstring MachineFactory::addTransition<false, false, Machine::Direction::N>(std::optional<std::wstring> state, std::optional<std::wstring> newState);
template std::wstring MachineFactory::addTransition<false, true, Machine::Direction::L>(std::optional<std::wstring> state, std::optional<std::wstring> newState);
template std::wstring MachineFactory::addTransition<false, true, Machine::Direction::R>(std::optional<std::wstring> state, std::optional<std::wstring> newState);
template std::wstring MachineFactory::addTransition<false, true, Machine::Direction::N>(std::optional<std::wstring> state, std::optional<std::wstring> newState);
template std::wstring MachineFactory::addTransition<true, false, Machine::Direction::L>(std::optional<std::wstring> state, std::optional<std::wstring> newState);
template std::wstring MachineFactory::addTransition<true, false, Machine::Direction::R>(std::optional<std::wstring> state, std::optional<std::wstring> newState);
template std::wstring MachineFactory::addTransition<true, false, Machine::Direction::N>(std::optional<std::wstring> state, std::optional<std::wstring> newState);
template std::wstring MachineFactory::addTransition<true, true, Machine::Direction::L>(std::optional<std::wstring> state, std::optional<std::wstring> newState);
template std::wstring MachineFactory::addTransition<true, true, Machine::Direction::R>(std::optional<std::wstring> state, std::optional<std::wstring> newState);
template std::wstring MachineFactory::addTransition<true, true, Machine::Direction::N>(std::optional<std::wstring> state, std::optional<std::wstring> newState);
template std::wstring MachineFactory::addTransition<Machine::Direction::L>(std::optional<std::wstring> state);
template std::wstring MachineFactory::addTransition<Machine::Direction::R>(std::optional<std::wstring> state);
template std::wstring MachineFactory::addTransition<Machine::Direction::N>(std::optional<std::wstring> state);
template std::wstring MachineFactory::addTransition<Machine::Direction::L>(std::optional<std::wstring> state, std::optional<std::wstring> newState);
template std::wstring MachineFactory::addTransition<Machine::Direction::R>(std::optional<std::wstring> state, std::optional<std::wstring> newState);
template std::wstring MachineFactory::addTransition<Machine::Direction::N>(std::optional<std::wstring> state, std::optional<std::wstring> newState);
