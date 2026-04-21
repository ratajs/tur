#pragma once
#include "../common.hpp"
#include "./variable.hpp"
#include "../parsing/location.hpp"

/*!
 * This class represents in AST the left side of ExplodeStatement.
 * It contains all variables in the bundle as well as the information which variable (if any) has ellipsis (marking that the variable gobbles up the rest).
 */
class DestinationBundle {
	private:
		std::optional<size_t> ellipsisVariable;
		std::vector<std::reference_wrapper<const Variable>> variables;
		std::set<const Variable*> variableSet;

	public:
		void add(Variable &variable, bool hasEllipsis, Location variableLocation);
		void forEachVariable(const std::function<void (const Variable&, bool)> &function) const;
		bool hasNonFinalEllipsis() const;
};
