#include "./copyStatement.hpp"
#include "../../instructions/writeNumberInstruction.hpp"
#include "../../instructions/copyInstruction.hpp"
#include "../../instructions/clearInstruction.hpp"
#include "../../IO/typeError.hpp"

/*!
 * The constructor of CopyStatement.
 * \param source The expression from which to copy/assign. It cannot represent directly the (possibly indexed) destination variable.
 * \param destination The destination variable.
 * \param destinationIndex The index on the variable from where on to write. {} (std::nullopt) means appending.
 * \throw TypeError If the same variable (as VariableExpression, possibly indexed) is used as both source and destination.
 */
CopyStatement::CopyStatement(std::unique_ptr<Expression> source, Variable &destination, std::optional<size_t> destinationIndex):
	destinationIndex(destinationIndex), destination(destination), source(std::move(source)) {
		if(this->source->isCondition())
			throw TypeError(TypeError::Type::CONDITION_AS_AN_ASSIGNMENT_SOURCE, this->source->location);
	};

void CopyStatement::build(InstructionBuilder &builder) const {
	size_t sourceTape, tmpTape;
	Expression::TapeRange sourceRange;

	if(this->source->isConstant())
		builder.addInstruction(std::make_unique<WriteNumberInstruction>(*this->destination.tape, this->destinationIndex, this->source->buildConstant(builder)));
	else if(this->source->getVariable() && (&this->source->getVariable()->get())==(&this->destination)) { // The same variable used both in source and destination
		if(destinationIndex==0) { // Assigning to the beginning, so just cleaning is enough
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
			builder.addInstruction(std::make_unique<CopyInstruction>(tmpTape, *this->destination.tape, 0, std::nullopt, this->destinationIndex));
		};
	}
	else {
		std::tie(sourceTape, sourceRange) = this->source->buildTape(builder);
		builder.addInstruction(std::make_unique<CopyInstruction>(sourceTape, *this->destination.tape, sourceRange.index0, sourceRange.index1, this->destinationIndex));
	};
};
