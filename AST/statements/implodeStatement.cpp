#include "./implodeStatement.hpp"
#include "../../instructions/clearInstruction.hpp"
#include "../../instructions/writeNumberInstruction.hpp"
#include "../../instructions/copyInstruction.hpp"

/*!
 * The constructor of ImplodeStatement.
 * \param source The bundle with the source expressions.
 * \param destination The variable where the joined expressions will be written.
 * \param destination The index of the variable.
	* \param bundleLocation The location of the SourceBundle (not used for now, but it would be used for type errors).
 */
ImplodeStatement::ImplodeStatement(SourceBundle source, Variable &destination, std::optional<size_t> destinationIndex, Location bundleLocation):
	destinationIndex(destinationIndex), destination(destination), source(std::move(source)) {
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
	std::optional<size_t> index;

	if(this->source.isEmpty()) {
		if(!this->destinationIndex)
			return;

		builder.addInstruction(std::make_unique<ClearInstruction>(*this->destination.tape, *this->destinationIndex, std::nullopt));
	};

	if(this->backupRange) {
		backupTape = builder.createTape();
		builder.addInstruction(std::make_unique<CopyInstruction>(*this->destination.tape, backupTape, this->backupRange->index0, this->backupRange->index1, 0));
	};

	index = this->destinationIndex;
	this->source.forEachExpression(
		[this, &builder, &index, backupTape](const std::unique_ptr<Expression> &expression) -> void {
			size_t sourceTape;
			Expression::TapeRange sourceRange;

			if(expression->isConstant()) {
				builder.addInstruction(std::make_unique<WriteNumberInstruction>(*this->destination.tape, index, expression->buildConstant(builder)));
				if(index)
					(*index)++;
			}
			else if(expression->getVariable() && (&expression->getVariable()->get())==(&this->destination)) { // The destination variable, we need to use the backup
				std::tie(sourceTape, sourceRange) = expression->buildTape(builder);
				builder.addInstruction(std::make_unique<CopyInstruction>(backupTape, *this->destination.tape, sourceRange.index0 - this->backupRange->index0, sourceRange.index1.transform([this](size_t index) -> size_t { return (index - this->backupRange->index0); }), index));
				if(!sourceRange.index1)
					index = {};
				else
					(*index)+= ((*sourceRange.index1) - sourceRange.index0);
			}
			else {
				std::tie(sourceTape, sourceRange) = expression->buildTape(builder);
				builder.addInstruction(std::make_unique<CopyInstruction>(sourceTape, *this->destination.tape, sourceRange.index0, sourceRange.index1, index));
				if(!sourceRange.index1)
					index = {};
				else if(index)
					(*index)+= ((*sourceRange.index1) - sourceRange.index0);
			};
		}
	);
};
