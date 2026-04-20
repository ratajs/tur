#pragma once
#include "../common.hpp"
#include "./variable.hpp"
#include "../parsing/location.hpp"

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
