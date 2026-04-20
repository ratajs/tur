#pragma once
#include "../common.hpp"
#include "./stateGenerator.hpp"
#include "./machine.hpp"

class MachineFactory {
	public:
		static Machine compose(const std::vector<Machine> &machines);

	protected:
		bool isCurrentStateDisabled = false;
		std::set<std::wstring> extraStates;
		Machine machine;
		StateGenerator generator;

		template<bool symbol, bool newSymbol, Machine::Direction direction>
			std::wstring addTransition(std::optional<std::wstring> state);
		template<bool symbol, bool newSymbol, Machine::Direction direction>
			std::wstring addTransition(std::optional<std::wstring> state, std::optional<std::wstring> newState);
		template<Machine::Direction direction>
			std::wstring addTransition(std::optional<std::wstring> state);
		template<Machine::Direction direction>
			std::wstring addTransition(std::optional<std::wstring> state, std::optional<std::wstring> newState);

	public:
		Machine extractMachine();
		std::vector<std::wstring> createExtraStates(size_t count);
		void addNTransition(std::optional<std::wstring> stateA, std::optional<std::wstring> stateB);
};

class SingleTapeMachineFactory;
class MultiTapeMachineFactory;

template<class T>
	concept MachineFactory_MachineFactory = (std::same_as<T, SingleTapeMachineFactory> || std::same_as<T, MultiTapeMachineFactory>);
