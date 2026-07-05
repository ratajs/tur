#include "./implodeStatement.hpp"
#include <utility>
#include <memory>
#include <tuple>
#include <string>
#include <functional>
#include "../../instructions/clearInstruction.hpp"
#include "../../instructions/writeNumberInstruction.hpp"
#include "../../instructions/copyInstruction.hpp"
#include "../../instructions/reversePseudoinstruction.hpp"
#include "../../IO/unexpectedError.hpp"

/*!
 * The constructor of ImplodeStatement.
 * \param source The bundle with the source expressions.
 * \param destination The variable where the joined expressions will be written.
 * \param destinationIndex The index on the variable from where (to to where, if reversed) on to write. {} (std::nullopt) means appending.
 * \param isReversed Whether the implosion should rewrite the beginning, instead of the end.
 * \param bundleLocation The location of the SourceBundle (not used for now, but it would be used for type errors).
 */
ImplodeStatement::ImplodeStatement(SourceBundle source, Variable &destination, std::optional<size_t> destinationIndex, bool isReversed, Location bundleLocation):
	isReversed(isReversed), destinationIndex(destinationIndex), destination(destination), source(std::move(source)) {
		if(isReversed && !destinationIndex)
			throw UnexpectedError(L"Reversed implosion requires a destination index.");

		this->source.forEachExpression(
			[this](const std::unique_ptr<Expression> &expression) -> void {
				Expression::TapeRange range;

				if(expression->getVariable() && (&expression->getVariable()->get())==(&this->destination)) { // The destination tape is in the bundle, backup is necessary
					range = expression->getArrayAccesRange().value_or(Expression::TapeRange(0, 1));
					if(!this->backupRange)
						this->backupRange = range;
					else {
						if(range.index0 < this->backupRange->index0)
							this->backupRange->index0 = range.index0;
						if(this->backupRange->index1 && (!range.index1 || (*range.index1) > (*this->backupRange->index1)))
							this->backupRange->index1 = range.index1;
					};
				};
			}
		);
	};

void ImplodeStatement::build(InstructionBuilder &builder) const {
	size_t backupTape = 0;

	if(this->backupRange) {
		backupTape = builder.createTape();
		builder.addInstruction(std::make_unique<CopyInstruction>(*this->destination.tape, backupTape, this->backupRange->index0, this->backupRange->index1, 0));
	};

	if(this->isReversed && (*this->destinationIndex)!=0)
		builder.addInstruction(std::make_unique<ClearInstruction>(*this->destination.tape, 0, *this->destinationIndex));
	if(!this->isReversed && this->destinationIndex)
		builder.addInstruction(std::make_unique<ClearInstruction>(*this->destination.tape, *this->destinationIndex, std::nullopt, this->isReversed));

	if(this->source.isEmpty())
		return;

	this->source.forEachExpression(
		[this, &builder, backupTape](const std::unique_ptr<Expression> &expression) -> void {
			size_t sourceTape;
			Expression::TapeRange sourceRange;

			if(expression->isConstant()) {
				if(this->isReversed)
					builder.addInstruction(std::make_unique<ReversePseudoinstruction>());
				builder.addInstruction(std::make_unique<WriteNumberInstruction>(*this->destination.tape, std::nullopt, expression->buildConstant(builder)));
				if(this->isReversed)
					builder.addInstruction(std::make_unique<ReversePseudoinstruction>());
			}
			else if(expression->getVariable() && (&expression->getVariable()->get())==(&this->destination)) { // The destination variable, we need to use the backup
				std::tie(sourceTape, sourceRange) = expression->buildTape(builder);

				if(this->isReversed) {
					builder.addInstruction(std::make_unique<ReversePseudoinstruction>());
					if(sourceRange.index1) {
						builder.addInstruction(std::make_unique<CopyInstruction>(backupTape, *this->destination.tape, *sourceRange.index1 - this->backupRange->index0, sourceRange.index0 - this->backupRange->index0, std::nullopt, true));
						builder.addInstruction(std::make_unique<ReversePseudoinstruction>());
					}
					else {
						builder.addInstruction(std::make_unique<CopyInstruction>(backupTape, *this->destination.tape, 0, std::nullopt, std::nullopt));
						builder.addInstruction(std::make_unique<ReversePseudoinstruction>());
						builder.addInstruction(std::make_unique<ClearInstruction>(*this->destination.tape, 0, sourceRange.index0 - this->backupRange->index0));
					};
				}
				else
					builder.addInstruction(std::make_unique<CopyInstruction>(backupTape, *this->destination.tape, sourceRange.index0 - this->backupRange->index0, sourceRange.index1.transform([this](size_t index) -> size_t { return (index - this->backupRange->index0); }), std::nullopt));
			}
			else {
				std::tie(sourceTape, sourceRange) = expression->buildTape(builder);

				if(this->isReversed) {
					builder.addInstruction(std::make_unique<ReversePseudoinstruction>());
					if(sourceRange.index1) {
						builder.addInstruction(std::make_unique<CopyInstruction>(sourceTape, *this->destination.tape, *sourceRange.index1, sourceRange.index0, std::nullopt, true));
						builder.addInstruction(std::make_unique<ReversePseudoinstruction>());
					}
					else {
						builder.addInstruction(std::make_unique<CopyInstruction>(sourceTape, *this->destination.tape, 0, std::nullopt, std::nullopt));
						builder.addInstruction(std::make_unique<ReversePseudoinstruction>());
						builder.addInstruction(std::make_unique<ClearInstruction>(*this->destination.tape, 0, sourceRange.index0));
					};
				}
				else
					builder.addInstruction(std::make_unique<CopyInstruction>(sourceTape, *this->destination.tape, sourceRange.index0, sourceRange.index1, std::nullopt));
			};
		}, this->isReversed
	);
};
