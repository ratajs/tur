#pragma once
#include <cstdlib>
#include <variant>
#include <set>
#include <stack>

/*!
 * This class analyzes which variable tapes are initialized (completelly rewritten) and which variable tapes are used before initializing in the current code block (branch, loop).
 */
class TapeInitializationAnalyzer {
	private:
		struct Scope {
			// initializedTapes – Definitelly initialized (previous content can never be relevant after this block).
			// uninitializedTapes – Potentially used without initializing in this block before.
			std::set<size_t> uninitializedTapes, initializedTapes;
		};

		struct Branching {
			bool isInFirstBranch = true;
			TapeInitializationAnalyzer::Scope scope;
		};

		struct Loop {};

		static void endBranch(TapeInitializationAnalyzer::Branching &branching, TapeInitializationAnalyzer::Scope &&previousBranch);
		static void endScope(TapeInitializationAnalyzer::Scope &higherScope, Scope &lowerScope, bool isScopeGuaranteedToRun);

		std::stack<std::variant<TapeInitializationAnalyzer::Scope, TapeInitializationAnalyzer::Branching, TapeInitializationAnalyzer::Loop>> stack;

	public:
		TapeInitializationAnalyzer();
		void startBranching();
		void switchBranch();
		void endBranching();
		void startLoop();
		void endLoop();
		void reportTapeUsage(size_t tape);
		void reportTapeInitialization(size_t tape);
		const std::set<size_t> &getUnitialized() const;
};
