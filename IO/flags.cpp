#include "./flags.hpp"

bool Flags::isFlagPresent(Flags::Flag flag) const {
	return (this->flags & std::to_underlying(flag));
};

void Flags::addFlag(Flags::Flag flag) {
	this->flags|= std::to_underlying(flag);
};

