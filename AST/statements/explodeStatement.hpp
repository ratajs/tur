#pragma once
#include "../../common.hpp"
#include "./statement.hpp"
#include "../expressions/expression.hpp"
#include "../destinationBundle.hpp"
#include "../../parsing/location.hpp"

/*!
 * This statement represents splitting/exploding a rightwise unbounded variable range into variables.
 * Normally, one number goes in order to one variable, but it is possible to specify that the last one gets the rest (possibly empty).
 * It contains the source expresion and the variables in a DestinationBundle object.
 */
class ExplodeStatement: public Statement {
	private:
		std::unique_ptr<Expression> source;
		DestinationBundle destination;

	public:
		ExplodeStatement(std::unique_ptr<Expression> source, DestinationBundle destination, Location bundleLocation);
		void build(InstructionBuilder &builder) const override;
};
