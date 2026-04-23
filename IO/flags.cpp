#include "./flags.hpp"

/*!
 * Detect whether this flag is in the collection (the option is on).
 * \param flag Which flag to look for.
 * \return Whether it is present.
 */
bool Flags::isFlagPresent(Flags::Flag flag) const {
	return (this->flags & std::to_underlying(flag));
};

/*!
 * Add a flag into the collection (turn on).
 * \param flag Which flag to add.
 */
void Flags::addFlag(Flags::Flag flag) {
	this->flags|= std::to_underlying(flag);
};

