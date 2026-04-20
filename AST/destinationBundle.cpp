#include "./destinationBundle.hpp"
#include "../IO/typeError.hpp"

void DestinationBundle::add(Variable &variable, bool hasEllipsis, Location variableLocation) {
	if(this->variableSet.contains(&variable))
		throw TypeError(TypeError::Type::VARIABLE_MULTIPLE_TIMES_IN_A_DESTINATION_BUNDLE, variableLocation);
	if(hasEllipsis && this->ellipsisVariable)
		throw TypeError(TypeError::Type::MULTIPLE_VARIABLES_WITH_ELLIPSIS_IN_A_DESTINATION_BUNDLE, variableLocation);
	else if(hasEllipsis)
		this->ellipsisVariable = this->variables.size();

	this->variables.emplace_back(variable);
	this->variableSet.insert(&variable);
};

void DestinationBundle::forEachVariable(const std::function<void (const Variable&, bool)> &function) const {
	size_t index = 0;

	std::ranges::for_each(this->variables,
		[this, &function, &index](const Variable &variable) -> void {
			function(variable, (index==this->ellipsisVariable));
			index++;
		}
	);
};

bool DestinationBundle::hasNonFinalEllipsis() const {
	return (this->ellipsisVariable && this->ellipsisVariable!=this->variables.size() - 1);
};
