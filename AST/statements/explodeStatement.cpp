#include "./explodeStatement.hpp"
#include "../variable.hpp"
#include "../../instructions/copyInstruction.hpp"
#include "../../instructions/clearInstruction.hpp"
#include "../../IO/typeError.hpp"

/*!
 * The constructor of ExplodeStatement.
 * \param source The source expression. It has to be bear a TAPE_RANGE type and must be rightwise unbounded (unindexed is allowed).
 * \param destination The DestinationBundle object with the variables used as destinations. Ellipsis elsewhere from the final variable is not supported.
 * \param bundleLocation The location of the DestinationBundle, used for type errors.
 * \throw TypeError if the expression has other type than TAPE_RANGE, if it has a rightwise bounded array access or if the DestinationBundle has an ellipsis for another variable than the last one.
 */
ExplodeStatement::ExplodeStatement(std::unique_ptr<Expression> source, DestinationBundle destination, Location bundleLocation): source(std::move(source)), destination(std::move(destination)) {
	if(this->source->isConstant())
		throw TypeError(TypeError::Type::CONSTANT_IN_AN_EXPLOSION_STATEMENT, this->source->location);
	if(this->source->isCondition())
		throw TypeError(TypeError::Type::CONDITION_IN_AN_EXPLOSION_STATEMENT, this->source->location);

	if(this->source->getArrayAccessLength() && (*this->source->getArrayAccessLength())!=0)
		throw TypeError(TypeError::Type::BOUNDED_EXPRESSION_AN_EXPLOSION_STATEMENT, this->source->location);

	if(this->destination.hasNonFinalEllipsis())
		throw TypeError(TypeError::Type::NON_FINAL_ELLIPSIS_IN_A_DESTINATION_BUNDLE, bundleLocation);
};

void ExplodeStatement::build(InstructionBuilder &builder) const {
	size_t index, sourceTape;
	std::optional<std::pair<size_t, std::optional<size_t>>> sourceVariablePosition;
	Expression::TapeRange sourceRange;

	std::tie(sourceTape, sourceRange) = this->source->buildTape(builder);

	index = 0;
	this->destination.forEachVariable(
		[this, &builder, sourceTape, &sourceRange, &index, &sourceVariablePosition](const Variable &variable, bool hasEllipsis) -> void {
			if(this->source->getVariable() && (&this->source->getVariable()->get())==(&variable)) // Source variable, no copying
				sourceVariablePosition = std::pair(index, hasEllipsis ? std::nullopt : std::optional(index + 1));
			else {
				if(hasEllipsis)
					builder.addInstruction(std::make_unique<CopyInstruction>(sourceTape, *variable.tape, sourceRange.index0 + index, std::nullopt, 0));
				else
					builder.addInstruction(std::make_unique<CopyInstruction>(sourceTape, *variable.tape, sourceRange.index0 + index, sourceRange.index0 + index + 1, 0));
			};

			index++;
		}
	);

	if(!sourceVariablePosition)
		return;

	if(sourceVariablePosition->first > 0)
		builder.addInstruction(std::make_unique<ClearInstruction>(*this->source->getVariable()->get().tape, 0, sourceVariablePosition->first));

	if(sourceVariablePosition->second)
		builder.addInstruction(std::make_unique<ClearInstruction>(*this->source->getVariable()->get().tape, (*sourceVariablePosition->second) - sourceVariablePosition->first, std::nullopt));
};
