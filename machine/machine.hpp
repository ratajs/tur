#pragma once
#include "../common.hpp"
#include "./tape.hpp"

class Machine {
	public:
		enum class Direction { L, N, R };

	private:
		static void skipWhiteSpaceAndComments(std::wistream &stream);
		static bool readState(std::wistream &stream, std::wstring &state, bool isEndAllowed);
		static bool readSymbol(std::wistream &stream, bool &symbol);
		static bool readDirection(std::wistream &stream, Machine::Direction &direction);

	public:
		static bool readTransition(std::wistream &stream, std::wstring &state, bool &symbol, bool &newSymbol, Machine::Direction &direction, std::wstring &newState);

	private:
		struct Transition {
			bool newSymbol;
			Machine::Direction direction;
			size_t newState;
		};

		std::vector<std::wstring> stateNames;
		std::vector<std::map<bool, Machine::Transition>> transitions;
		std::map<std::wstring, size_t> statesByNames;

		bool addState(const std::wstring &name);
		bool step(Tape &tape, size_t &state, std::optional<std::reference_wrapper<std::wostream>> stream, bool isFormattingEnabled) const;

	public:
		Machine() = default;
		Machine(std::wstring text);
		bool addTransition(const std::wstring &state, bool symbol, bool newSymbol, Machine::Direction direction, const std::wstring &newState);
		void forEachTransition(const std::function<void (const std::wstring&, bool, bool, Machine::Direction, const std::wstring&)> &function) const;
		Machine &removeNTransitions();
		Machine &removeInaccessibleStates();
		bool run(Tape &tape, bool shouldKeepBlanks = false, std::optional<std::reference_wrapper<std::wstring>> lastState = {}, std::optional<std::reference_wrapper<std::wostream>> stream = {}, bool isFormattingEnabled = true) const;
		explicit operator bool() const;
		void print(std::wostream &stream, std::wstring_view columnDelimiter = L" ", std::wstring_view rowDelimiter = L"\n") const;
};

std::wostream &operator << (std::wostream &stream, const Machine &machine);
std::wistream &operator >> (std::wistream &stream, Machine &machine);
