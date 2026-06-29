#include "./explodeStatement.hpp"
#include <cstdlib>
#include <utility>
#include <memory>
#include <optional>
#include <tuple>
#include <functional>
#include "../variable.hpp"
#include "../../instructions/copyInstruction.hpp"
#include "../../instructions/clearInstruction.hpp"
#include "../../instructions/reversePseudoinstruction.hpp"
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
	/*
	if(this->destination.hasNonFinalEllipsis())
		throw TypeError(TypeError::Type::NON_FINAL_ELLIPSIS_IN_A_DESTINATION_BUNDLE, bundleLocation);
	*/
};

void ExplodeStatement::build(InstructionBuilder &builder) const {
	size_t index, sourceTape;
	Expression::TapeRange sourceRange;
	std::optional<std::function<void ()>> clearSourceVariable;

	std::tie(sourceTape, sourceRange) = this->source->buildTape(builder);

	index = 0;
	this->destination.forEachVariableFromStartUntilEllipsis(
		[this, &builder, sourceTape, &sourceRange, &index, &clearSourceVariable](const Variable &variable, bool hasEllipsis) -> void {
			if(this->source->getVariable() && (&this->source->getVariable()->get())==(&variable)) { // Source variable, no copying
				clearSourceVariable = ([&builder, sourceTape, &sourceRange, index, hasEllipsis]() -> void {
					if(sourceRange.index0 + index > 0)
						builder.addInstruction(std::make_unique<ClearInstruction>(sourceTape, 0, sourceRange.index0 + index));
					if(!hasEllipsis)
						builder.addInstruction(std::make_unique<ClearInstruction>(sourceTape, 1, std::nullopt));
				});
			}
			else {
				if(hasEllipsis)
					builder.addInstruction(std::make_unique<CopyInstruction>(sourceTape, *variable.tape, sourceRange.index0 + index, std::nullopt, 0));
				else
					builder.addInstruction(std::make_unique<CopyInstruction>(sourceTape, *variable.tape, sourceRange.index0 + index, sourceRange.index0 + index + 1, 0));
			};

			index++;
		}
	);
	if(clearSourceVariable)
		(*clearSourceVariable)();

	if(this->destination.hasNonFinalEllipsis()) {
		clearSourceVariable.reset();
		builder.addInstruction(std::make_unique<ReversePseudoinstruction>());
		index = 0;
		this->destination.forEachVariableFromEndUntilEllipsis(
			[this, &builder, sourceTape, &index, &clearSourceVariable](const Variable &variable, bool hasEllipsis) -> void {
				if(hasEllipsis) {
					builder.addInstruction(std::make_unique<ClearInstruction>(*variable.tape, 0, index)); //FIXME for source variable after non-final ellipsis

					return;
				};

				if(this->source->getVariable() && (&this->source->getVariable()->get())==(&variable)) { // Source variable, no copying
					clearSourceVariable = ([&builder, sourceTape, index, hasEllipsis]() -> void {
						if(index > 0)
							builder.addInstruction(std::make_unique<ClearInstruction>(sourceTape, 0, index));
						if(!hasEllipsis)
							builder.addInstruction(std::make_unique<ClearInstruction>(sourceTape, 1, std::nullopt));
					});
				}
				else
					builder.addInstruction(std::make_unique<CopyInstruction>(sourceTape, *variable.tape, index, index + 1, 0));

				index++;
			}
		);
		if(clearSourceVariable)
			(*clearSourceVariable)();
		builder.addInstruction(std::make_unique<ReversePseudoinstruction>());
	};
};
