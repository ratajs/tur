#include "./variableAnalyzer.hpp"
#include <utility>
#include <algorithm>
#include <iterator>
#include <string>
#include "../IO/unexpectedError.hpp"

/*!
 * This function combines results from one branch with all preceding branches.
 * \param branching The whole branching.
 * \param previousBranch The just analyzed branch.
 */
void VariableAnalyzer::endBranch(VariableAnalyzer::Branching &branching, VariableAnalyzer::Scope &&previousBranch) {
	if(branching.isInFirstBranch) {
		branching.scope.initializedVariables = std::move(previousBranch.initializedVariables);
		branching.scope.uninitializedVariables = std::move(previousBranch.uninitializedVariables);
		branching.isInFirstBranch = false;

		return;
	};

	// Variables initialized in all previous branches, but not this one. ⇒ Not initialized.
	std::erase_if(branching.scope.initializedVariables,
		[&previousBranch](const Variable *variable) -> bool {
			return (!previousBranch.initializedVariables.contains(variable));
		}
	);

	// Variables initialized in this branch, but not seen before. ⇒ Neither initialized nor uninitialized.

	// Used uninitialized in this branch.
	std::ranges::copy(previousBranch.uninitializedVariables, std::inserter(branching.scope.uninitializedVariables, branching.scope.uninitializedVariables.end()));
};

/*!
 * This function combines results from an ending scope with the scope higher in hierarchy.
 * \param higherScope The higher scope.
 * \param lowerScope The just ending scope.
 * \param isScopeGuaranteedToRun Whether the scope will always run, so it is possible to consider here initialized variables as always initialized (true for branching as a whole, false for loops).
 */
void VariableAnalyzer::endScope(VariableAnalyzer::Scope &higherScope, Scope &lowerScope, bool isScopeGuaranteedToRun) {
	std::ranges::set_difference(lowerScope.uninitializedVariables, higherScope.initializedVariables, std::inserter(higherScope.uninitializedVariables, higherScope.uninitializedVariables.end()));
	if(isScopeGuaranteedToRun)
		std::ranges::set_difference(lowerScope.initializedVariables, higherScope.uninitializedVariables, std::inserter(higherScope.initializedVariables, higherScope.initializedVariables.end()));
};

/*!
 * The constructor of VariableAnalyzer.
 * An all-enwraping scope (code block) is created
 */
VariableAnalyzer::VariableAnalyzer() {
	this->stack.push(VariableAnalyzer::Scope());
};

/*!
 * This method creates a scope for branching and a scope for the first branch.
 */
void VariableAnalyzer::startBranching() {
	this->stack.push(VariableAnalyzer::Branching());
	this->stack.push(VariableAnalyzer::Scope());
};

/*!
 * This method ends the current branch and starts a new one.
 * \throw UnexpectedError If the top does not match with a branch.
 */
void VariableAnalyzer::switchBranch() {
	VariableAnalyzer::Scope previousBranch;

	if(this->stack.size() < 2 || !std::holds_alternative<VariableAnalyzer::Scope>(this->stack.top()) || (previousBranch = std::move(std::get<VariableAnalyzer::Scope>(this->stack.top())), this->stack.pop(), !std::holds_alternative<VariableAnalyzer::Branching>(this->stack.top())))
		throw UnexpectedError(L"Unexpected stack contents in Variable::switchBranch().");

	VariableAnalyzer::endBranch(std::get<VariableAnalyzer::Branching>(this->stack.top()), std::move(previousBranch));

	this->stack.push(VariableAnalyzer::Scope());
};

/*!
 * This method ends the current branch and the whole branching with it.
 * \throw UnexpectedError If the top of the scope stack does not match with a started branching.
 */
void VariableAnalyzer::endBranching() {
	VariableAnalyzer::Scope previousBranch;
	VariableAnalyzer::Branching branching;

	if(
		this->stack.size() < 3 ||
		!std::holds_alternative<VariableAnalyzer::Scope>(this->stack.top()) ||
		(previousBranch = std::move(std::get<VariableAnalyzer::Scope>(this->stack.top())), this->stack.pop(), !std::holds_alternative<VariableAnalyzer::Branching>(this->stack.top())) ||
		(branching = std::move(std::get<VariableAnalyzer::Branching>(this->stack.top())), this->stack.pop(), !std::holds_alternative<VariableAnalyzer::Scope>(this->stack.top()))
	)
		throw UnexpectedError(L"Unexpected stack contents in Variable::endBranching().");

	VariableAnalyzer::endBranch(branching, std::move(previousBranch));

	/* This should not be necessary.
	std::erase_if(branching.initializedVariables,
		[&branching](const Variable *variable) -> bool {
			return branching.uninitializedVariables.contains(variable);
		}
	);
	*/

	VariableAnalyzer::endScope(std::get<VariableAnalyzer::Scope>(this->stack.top()), branching.scope, true);
};

/*!
 * This method creates a scope for a loop.
 */
void VariableAnalyzer::startLoop() {
	this->stack.push(VariableAnalyzer::Loop());
	this->stack.push(VariableAnalyzer::Scope());
};

/*!
 * This methods ends a loop.
 * \throw UnexpectedError If the top of the scope stack does not match with a started loop.
 */
void VariableAnalyzer::endLoop() {
	VariableAnalyzer::Scope innerScope;

	if(
		this->stack.size() < 3 ||
		!std::holds_alternative<VariableAnalyzer::Scope>(this->stack.top()) ||
		(innerScope = std::move(std::get<VariableAnalyzer::Scope>(this->stack.top())), this->stack.pop(), !std::holds_alternative<VariableAnalyzer::Loop>(this->stack.top())) ||
		(this->stack.pop(), !std::holds_alternative<VariableAnalyzer::Scope>(this->stack.top()))
	)
		throw UnexpectedError(L"Unexpected stack contents in Variable::endLoop().");

	VariableAnalyzer::endScope(std::get<VariableAnalyzer::Scope>(this->stack.top()), innerScope, false);
};

/*!
 * This method is for reporting a use of a variable for anything except for rewriting the whole of it (copying from, copying onto an offset of it, compressing the output…).
 * \param variable The variable.
 */
void VariableAnalyzer::reportVariableUsage(const Variable &variable) {
	if(!std::get<VariableAnalyzer::Scope>(this->stack.top()).initializedVariables.contains(&variable))
		std::get<VariableAnalyzer::Scope>(this->stack.top()).uninitializedVariables.insert(&variable);
};

/*!
 * This method is for reporting an initialization of a variable, i.e. rewriting the whole contents of it.
 * \param variable The variable.
 */
void VariableAnalyzer::reportVariableAssignment(const Variable &variable) {
	if(!std::get<VariableAnalyzer::Scope>(this->stack.top()).uninitializedVariables.contains(&variable))
		std::get<VariableAnalyzer::Scope>(this->stack.top()).initializedVariables.insert(&variable);
};

/*!
 * This method is used to list variables with contents that could originate from before this block.
 * When in branching, the current branch is used.
 * \return The current list of variables used without initializing in the current scope (possibly initialized before).
 */
const std::set<const Variable*> &VariableAnalyzer::getUnitialized() const {
	return std::get<VariableAnalyzer::Scope>(this->stack.top()).uninitializedVariables;
};
