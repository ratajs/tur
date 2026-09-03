#include "./tapeInitializationAnalyzer.hpp"
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
void TapeInitializationAnalyzer::endBranch(TapeInitializationAnalyzer::Branching &branching, TapeInitializationAnalyzer::Scope &&previousBranch) {
	if(branching.isInFirstBranch) {
		branching.scope.initializedTapes = std::move(previousBranch.initializedTapes);
		branching.scope.uninitializedTapes = std::move(previousBranch.uninitializedTapes);
		branching.isInFirstBranch = false;

		return;
	};

	// Tapes initialized in all previous branches, but not this one. ⇒ Not initialized.
	std::erase_if(branching.scope.initializedTapes,
		[&previousBranch](size_t tape) -> bool {
			return (!previousBranch.initializedTapes.contains(tape));
		}
	);

	// Tapes initialized in this branch, but not seen before. ⇒ Neither initialized nor uninitialized.

	// Used uninitialized in this branch.
	std::ranges::copy(previousBranch.uninitializedTapes, std::inserter(branching.scope.uninitializedTapes, branching.scope.uninitializedTapes.end()));
};

/*!
 * This function combines results from an ending scope with the scope higher in hierarchy.
 * \param higherScope The higher scope.
 * \param lowerScope The just ending scope.
 * \param isScopeGuaranteedToRun Whether the scope will always run, so it is possible to consider here initialized variables as always initialized (true for branching as a whole, false for loops).
 */
void TapeInitializationAnalyzer::endScope(TapeInitializationAnalyzer::Scope &higherScope, Scope &lowerScope, bool isScopeGuaranteedToRun) {
	std::ranges::set_difference(lowerScope.uninitializedTapes, higherScope.initializedTapes, std::inserter(higherScope.uninitializedTapes, higherScope.uninitializedTapes.end()));
	if(isScopeGuaranteedToRun)
		std::ranges::set_difference(lowerScope.initializedTapes, higherScope.uninitializedTapes, std::inserter(higherScope.initializedTapes, higherScope.initializedTapes.end()));
};

/*!
 * The constructor of TapeInitializationAnalyzer.
 * An all-enwraping scope (code block) is created
 */
TapeInitializationAnalyzer::TapeInitializationAnalyzer() {
	this->stack.push(TapeInitializationAnalyzer::Scope());
};

/*!
 * This method creates a scope for branching and a scope for the first branch.
 */
void TapeInitializationAnalyzer::startBranching() {
	this->stack.push(TapeInitializationAnalyzer::Branching());
	this->stack.push(TapeInitializationAnalyzer::Scope());
};

/*!
 * This method ends the current branch and starts a new one.
 * \throw UnexpectedError If the top does not match with a branch.
 */
void TapeInitializationAnalyzer::switchBranch() {
	TapeInitializationAnalyzer::Scope previousBranch;

	if(this->stack.size() < 2 || !std::holds_alternative<TapeInitializationAnalyzer::Scope>(this->stack.top()) || (previousBranch = std::move(std::get<TapeInitializationAnalyzer::Scope>(this->stack.top())), this->stack.pop(), !std::holds_alternative<TapeInitializationAnalyzer::Branching>(this->stack.top())))
		throw UnexpectedError(L"Unexpected stack contents in Variable::switchBranch().");

	TapeInitializationAnalyzer::endBranch(std::get<TapeInitializationAnalyzer::Branching>(this->stack.top()), std::move(previousBranch));

	this->stack.push(TapeInitializationAnalyzer::Scope());
};

/*!
 * This method ends the current branch and the whole branching with it.
 * \throw UnexpectedError If the top of the scope stack does not match with a started branching.
 */
void TapeInitializationAnalyzer::endBranching() {
	TapeInitializationAnalyzer::Scope previousBranch;
	TapeInitializationAnalyzer::Branching branching;

	if(
		this->stack.size() < 3 ||
		!std::holds_alternative<TapeInitializationAnalyzer::Scope>(this->stack.top()) ||
		(previousBranch = std::move(std::get<TapeInitializationAnalyzer::Scope>(this->stack.top())), this->stack.pop(), !std::holds_alternative<TapeInitializationAnalyzer::Branching>(this->stack.top())) ||
		(branching = std::move(std::get<TapeInitializationAnalyzer::Branching>(this->stack.top())), this->stack.pop(), !std::holds_alternative<TapeInitializationAnalyzer::Scope>(this->stack.top()))
	)
		throw UnexpectedError(L"Unexpected stack contents in Variable::endBranching().");

	TapeInitializationAnalyzer::endBranch(branching, std::move(previousBranch));
	TapeInitializationAnalyzer::endScope(std::get<TapeInitializationAnalyzer::Scope>(this->stack.top()), branching.scope, true);
};

/*!
 * This method creates a scope for a loop.
 */
void TapeInitializationAnalyzer::startLoop() {
	this->stack.push(TapeInitializationAnalyzer::Loop());
	this->stack.push(TapeInitializationAnalyzer::Scope());
};

/*!
 * This methods ends a loop.
 * \throw UnexpectedError If the top of the scope stack does not match with a started loop.
 */
void TapeInitializationAnalyzer::endLoop() {
	TapeInitializationAnalyzer::Scope innerScope;

	if(
		this->stack.size() < 3 ||
		!std::holds_alternative<TapeInitializationAnalyzer::Scope>(this->stack.top()) ||
		(innerScope = std::move(std::get<TapeInitializationAnalyzer::Scope>(this->stack.top())), this->stack.pop(), !std::holds_alternative<TapeInitializationAnalyzer::Loop>(this->stack.top())) ||
		(this->stack.pop(), !std::holds_alternative<TapeInitializationAnalyzer::Scope>(this->stack.top()))
	)
		throw UnexpectedError(L"Unexpected stack contents in Variable::endLoop().");

	TapeInitializationAnalyzer::endScope(std::get<TapeInitializationAnalyzer::Scope>(this->stack.top()), innerScope, false);
};

/*!
 * This method is for reporting a use of a tape for anything except for rewriting the whole of it (copying from, copying onto an offset of it, compressing the output…).
 * \param tape The number of the tape.
 */
void TapeInitializationAnalyzer::reportTapeUsage(size_t tape) {
	if(!std::get<TapeInitializationAnalyzer::Scope>(this->stack.top()).initializedTapes.contains(tape))
		std::get<TapeInitializationAnalyzer::Scope>(this->stack.top()).uninitializedTapes.insert(tape);
};

/*!
 * This method is for reporting an initialization of a tape, i.e. rewriting the whole contents of it.
 * \param tape The number of the tape.
 */
void TapeInitializationAnalyzer::reportTapeInitialization(size_t tape) {
	if(!std::get<TapeInitializationAnalyzer::Scope>(this->stack.top()).uninitializedTapes.contains(tape))
		std::get<TapeInitializationAnalyzer::Scope>(this->stack.top()).initializedTapes.insert(tape);
};

/*!
 * This method is used to list variables with contents that could originate from before this block.
 * When in branching, the current branch is used.
 * \return The current list of variables used without initializing in the current scope (possibly initialized before).
 */
const std::set<size_t> &TapeInitializationAnalyzer::getUnitialized() const {
	return std::get<TapeInitializationAnalyzer::Scope>(this->stack.top()).uninitializedTapes;
};
