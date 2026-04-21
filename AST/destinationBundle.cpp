#include "./destinationBundle.hpp"
#include "../IO/typeError.hpp"

/*!
 * Add a new variable to the bundle.
 * \param variable The variable to insert.
 * \param hasEllipsis Whether the variable is marked with the ellipsis.
 * \param variableLocation Location of the variable in the source.
 * \throw TypeError If the variable has already been added or if it is the second variable with ellipsis.
 */
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

/*!
 * Run a closure for each variable in the bundle.
 * \param function The function to run.
 */
void DestinationBundle::forEachVariable(const std::function<void (const Variable&, bool)> &function) const {
	size_t index = 0;

	std::ranges::for_each(this->variables,
		[this, &function, &index](const Variable &variable) -> void {
			function(variable, (index==this->ellipsisVariable));
			index++;
		}
	);
};

/*!
 * Only final ellipsis (at the final variable) is supported by tur for now.
 * \return Whether it contains a variable with ellipsis which is not the last one.
 */
bool DestinationBundle::hasNonFinalEllipsis() const {
	return (this->ellipsisVariable && this->ellipsisVariable!=this->variables.size() - 1);
};
