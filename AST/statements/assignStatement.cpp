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
 * \param isDestinationIndexFromEnd Whether the destination is indexed from the end (0 means the end, 1 is the place before the last number).
 * \throw TypeError If the same variable (as VariableExpression, possibly indexed) is used as both source and destination.
 * \throw UnexpectedError If destinationIndex is {}, but isReversed or isDestinationIndexFromEnd is true.
 */
AssignStatement::AssignStatement(std::unique_ptr<Expression> source, Variable &destination, std::optional<size_t> destinationIndex, bool isReversed, bool isDestinationIndexFromEnd):
	isReversed(isReversed), isDestinationIndexFromEnd(isDestinationIndexFromEnd), destinationIndex(destinationIndex), destination(destination), source(std::move(source)) {
		if(isReversed && !destinationIndex)
			throw UnexpectedError(L"Reversed assignment requires a destination index.");

		if(isDestinationIndexFromEnd && !destinationIndex)
			throw UnexpectedError(L"Destination indexed from end without a destination index.");

		if(this->source->isCondition())
			throw TypeError(TypeError::Type::CONDITION_AS_AN_ASSIGNMENT_SOURCE, this->source->location);
	};

void AssignStatement::build(InstructionBuilder &builder) const {
	size_t sourceTape, tmpTape;
	Expression::TapeRange sourceRange;

	if(this->source->isConstant()) {
		if(this->isReversed) {
			builder.addInstruction(std::make_unique<ReversePseudoinstruction>());
			builder.addInstruction(std::make_unique<WriteNumberInstruction>(*this->destination.tape, *this->destinationIndex, this->source->buildConstant(builder), !this->isDestinationIndexFromEnd));
			builder.addInstruction(std::make_unique<ReversePseudoinstruction>());
		}
		else
			builder.addInstruction(std::make_unique<WriteNumberInstruction>(*this->destination.tape, this->destinationIndex, this->source->buildConstant(builder), this->isDestinationIndexFromEnd));
	}
	else if(this->source->getVariable() && (&this->source->getVariable()->get())==(&this->destination)) { // The same variable used both in source and destination
		if(!this->isReversed && !this->isDestinationIndexFromEnd && destinationIndex==0) { // Assigning to the beginning, so just cleaning is enough
			std::tie(sourceTape, sourceRange) = this->source->buildTape(builder);

			if(!sourceRange.isIndex0FromEnd && sourceRange.index0 > 0)
				builder.addInstruction(std::make_unique<ClearInstruction>(sourceTape, 0, sourceRange.index0));

			if(sourceRange.isIndex0FromEnd) {
				builder.addInstruction(std::make_unique<ReversePseudoinstruction>());
				builder.addInstruction(std::make_unique<ClearInstruction>(sourceTape, sourceRange.index0, std::nullopt, false));
				builder.addInstruction(std::make_unique<ReversePseudoinstruction>());
			};

			if(!sourceRange.isIndex1FromEnd) {
				if(!sourceRange.isIndex0FromEnd)
					builder.addInstruction(std::make_unique<ClearInstruction>(sourceTape, sourceRange.index1 - sourceRange.index0, std::nullopt));
				else
					throw UnexpectedError(L"Tape indices of type [−x:y] encountered.");
			};

			if(sourceRange.isIndex1FromEnd && sourceRange.index1 > 0) {
				builder.addInstruction(std::make_unique<ReversePseudoinstruction>());
				builder.addInstruction(std::make_unique<ClearInstruction>(sourceTape, 0, sourceRange.index1, false));
				builder.addInstruction(std::make_unique<ReversePseudoinstruction>());
			};
		}
		else { // An extra tape is necessary
			std::tie(sourceTape, sourceRange) = this->source->buildTape(builder);
			tmpTape = builder.createTape();

			if(sourceRange.isIndex0FromEnd==sourceRange.isIndex1FromEnd)
				builder.addInstruction(std::make_unique<CopyInstruction>(sourceTape, tmpTape, sourceRange.index0, sourceRange.index1, 0, sourceRange.isIndex0FromEnd));
			else if(sourceRange.isIndex1FromEnd) {
				builder.addInstruction(std::make_unique<CopyInstruction>(sourceTape, tmpTape, sourceRange.index0, std::nullopt, 0));
				if(sourceRange.index1 > 0)
					builder.addInstruction(std::make_unique<ClearInstruction>(tmpTape, sourceRange.index1, std::nullopt, true));
			}
			else
				throw UnexpectedError(L"Tape indices of type [−x:y] encountered.");

			if(this->isReversed) {
				builder.addInstruction(std::make_unique<ReversePseudoinstruction>());
				builder.addInstruction(std::make_unique<CopyInstruction>(tmpTape, *this->destination.tape, 0, std::nullopt, *this->destinationIndex, false, !this->isDestinationIndexFromEnd));
				builder.addInstruction(std::make_unique<ReversePseudoinstruction>());
			}
			else
				builder.addInstruction(std::make_unique<CopyInstruction>(tmpTape, *this->destination.tape, 0, std::nullopt, this->destinationIndex, false, this->isDestinationIndexFromEnd));
		};
	}
	else {
		std::tie(sourceTape, sourceRange) = this->source->buildTape(builder);

		if(this->isReversed) {
			builder.addInstruction(std::make_unique<ReversePseudoinstruction>());
			if(sourceRange.isIndex0FromEnd==sourceRange.isIndex1FromEnd) {
				builder.addInstruction(std::make_unique<CopyInstruction>(sourceTape, *this->destination.tape, sourceRange.index1, sourceRange.index0, *this->destinationIndex, !sourceRange.isIndex0FromEnd, !this->isDestinationIndexFromEnd));
				builder.addInstruction(std::make_unique<ReversePseudoinstruction>());
			}
			else if(sourceRange.isIndex1FromEnd) {
				builder.addInstruction(std::make_unique<CopyInstruction>(sourceTape, *this->destination.tape, sourceRange.index1, std::nullopt, this->destinationIndex, false, !this->isDestinationIndexFromEnd));
				builder.addInstruction(std::make_unique<ReversePseudoinstruction>());
				if(sourceRange.index0 > 0)
					builder.addInstruction(std::make_unique<ClearInstruction>(*this->destination.tape, 0, sourceRange.index0));
			}
			else
				throw UnexpectedError(L"Tape indices of type [−x:y] encountered.");
		}
		else {
			if(sourceRange.isIndex0FromEnd==sourceRange.isIndex1FromEnd)
				builder.addInstruction(std::make_unique<CopyInstruction>(sourceTape, *this->destination.tape, sourceRange.index0, sourceRange.index1, this->destinationIndex, sourceRange.isIndex0FromEnd, this->isDestinationIndexFromEnd));
			else if(sourceRange.isIndex1FromEnd) {
				builder.addInstruction(std::make_unique<CopyInstruction>(sourceTape, *this->destination.tape, sourceRange.index0, std::nullopt, this->destinationIndex, false, this->isDestinationIndexFromEnd));
				if(sourceRange.index1 > 0)
					builder.addInstruction(std::make_unique<ClearInstruction>(*this->destination.tape, sourceRange.index1, std::nullopt, true));
			}
			else
				throw UnexpectedError(L"Tape indices of type [−x:y] encountered.");
		};
	};

	if(this->destinationIndex!=0)
		builder.tapeInitializationAnalyzer.reportTapeUsage(*this->destination.tape);
	builder.tapeInitializationAnalyzer.reportTapeInitialization(*this->destination.tape);
};
