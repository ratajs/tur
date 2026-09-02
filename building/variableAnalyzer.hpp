#pragma once
#include <variant>
#include <set>
#include <stack>
#include "../AST/variable.hpp"

/*!
 * This class analyzes which variables are initialized (completelly rewritten) and which variables are used before initializing in the current code block (branch, loop).
 */
class VariableAnalyzer {
	private:
		struct Scope {
			// initializedVariables – Definitelly initialized (previous content can never be relevant after this block).
			// uninitializedVariables – Potentially used without initializing in this block before.
			std::set<const Variable*> uninitializedVariables, initializedVariables;
		};

		struct Branching {
			bool isInFirstBranch = true;
			VariableAnalyzer::Scope scope;
		};

		struct Loop {};

		static void endBranch(VariableAnalyzer::Branching &branching, VariableAnalyzer::Scope &&previousBranch);
		static void endScope(VariableAnalyzer::Scope &higherScope, Scope &lowerScope, bool isScopeGuaranteedToRun);

		std::stack<std::variant<VariableAnalyzer::Scope, VariableAnalyzer::Branching, VariableAnalyzer::Loop>> stack;

	public:
		VariableAnalyzer();
		void startBranching();
		void switchBranch();
		void endBranching();
		void startLoop();
		std::set<const Variable*> endLoop();
		void reportVariableUsage(const Variable &variable);
		void reportVariableAssignment(const Variable &variable);
};
