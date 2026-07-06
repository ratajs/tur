#include "./assignStatement.hpp"
#include <utility>
#include <tuple>
#include <string>
#include <functional>
#include "../../instructions/writeNumberInstruction.hpp"
#include "../../instructions/copyInstruction.hpp"
#include "../../instructions/clearInstruction.hpp"
#include "../../instructions/reversePseudoinstruction.hpp"
#include "../../IO/typeError.hpp"
#include "../../IO/unexpectedError.hpp"

/*!
 * The constructor of AssignStatement.
 * \param source The expression from which to copy/assign. It cannot represent directly the (possibly indexed) destination variable.
 * \param destination The destination variable.
 * \param destinationIndex The index on the variable from where (to to where, if reversed) on to write. {} (std::nullopt) means appending.
 * \param isReversed Whether the assignment should rewrite the beginning, instead of the end.
 * \throw TypeError If the same variable (as VariableExpression, possibly indexed) is used as both source and destination.
 */
AssignStatement::AssignStatement(std::unique_ptr<Expression> source, Variable &destination, std::optional<size_t> destinationIndex, bool isReversed):
	isReversed(isReversed), destinationIndex(destinationIndex), destination(destination), source(std::move(source)) {
		if(isReversed && !destinationIndex)
			throw UnexpectedError(L"Reversed assignment requires a destination index.");

		if(this->source->isCondition())
			throw TypeError(TypeError::Type::CONDITION_AS_AN_ASSIGNMENT_SOURCE, this->source->location);
	};

void AssignStatement::build(InstructionBuilder &builder) const {
	size_t sourceTape, tmpTape;
	Expression::TapeRange sourceRange;

	if(this->source->isConstant()) {
		if(this->isReversed) {
			builder.addInstruction(std::make_unique<ReversePseudoinstruction>());
			builder.addInstruction(std::make_unique<WriteNumberInstruction>(*this->destination.tape, *this->destinationIndex, this->source->buildConstant(builder), true));
			builder.addInstruction(std::make_unique<ReversePseudoinstruction>());
		}
		else
			builder.addInstruction(std::make_unique<WriteNumberInstruction>(*this->destination.tape, this->destinationIndex, this->source->buildConstant(builder)));
	}
	else if(this->source->getVariable() && (&this->source->getVariable()->get())==(&this->destination)) { // The same variable used both in source and destination
		if(!this->isReversed && destinationIndex==0) { // Assigning to the beginning, so just cleaning is enough
			std::tie(sourceTape, sourceRange) = this->source->buildTape(builder);
			if(sourceRange.index0 > 0)
				builder.addInstruction(std::make_unique<ClearInstruction>(sourceTape, 0, sourceRange.index0));
			if(sourceRange.index1)
				builder.addInstruction(std::make_unique<ClearInstruction>(sourceTape, (*sourceRange.index1) - sourceRange.index0, std::nullopt));
		}
		else { // An extra tape is necessary
			std::tie(sourceTape, sourceRange) = this->source->buildTape(builder);
			tmpTape = builder.createTape();
			builder.addInstruction(std::make_unique<CopyInstruction>(sourceTape, tmpTape, sourceRange.index0, sourceRange.index1, 0));
			if(this->isReversed) {
				builder.addInstruction(std::make_unique<ReversePseudoinstruction>());
				builder.addInstruction(std::make_unique<CopyInstruction>(tmpTape, *this->destination.tape, 0, std::nullopt, *this->destinationIndex, false, true));
				builder.addInstruction(std::make_unique<ReversePseudoinstruction>());
			}
			else
				builder.addInstruction(std::make_unique<CopyInstruction>(tmpTape, *this->destination.tape, 0, std::nullopt, this->destinationIndex));
		};
	}
	else {
		std::tie(sourceTape, sourceRange) = this->source->buildTape(builder);
		if(this->isReversed) {
			builder.addInstruction(std::make_unique<ReversePseudoinstruction>());
			if(sourceRange.index1) {
				builder.addInstruction(std::make_unique<CopyInstruction>(sourceTape, *this->destination.tape, *sourceRange.index1, sourceRange.index0, *this->destinationIndex, true, true));
				builder.addInstruction(std::make_unique<ReversePseudoinstruction>());
			}
			else {
				builder.addInstruction(std::make_unique<CopyInstruction>(sourceTape, *this->destination.tape, 0, std::nullopt, this->destinationIndex, false, true));
				builder.addInstruction(std::make_unique<ReversePseudoinstruction>());
				if(sourceRange.index0 > 0)
					builder.addInstruction(std::make_unique<ClearInstruction>(*this->destination.tape, 0, sourceRange.index0));
			};
		}
		else
			builder.addInstruction(std::make_unique<CopyInstruction>(sourceTape, *this->destination.tape, sourceRange.index0, sourceRange.index1, this->destinationIndex));
	};
};
