#include "./callExpression.hpp"
#include <cstdlib>
#include <utility>
#include <tuple>
#include <algorithm>
#include <iterator>
#include <string>
#include "../../instructions/writeNumberInstruction.hpp"
#include "../../instructions/copyInstruction.hpp"
#include "../../instructions/callInstruction.hpp"
#include "../../instructions/clearInstruction.hpp"
#include "../../instructions/reversePseudoinstruction.hpp"
#include "../../IO/unexpectedError.hpp"
#include "../../IO/typeError.hpp"

/*!
 * The constructor of CallExpression.
 * \param location The location of the expression in code.
 * \param machine The machine to run.
 * \param arguments The arguments to be joined on the tape for the machine, can be empty for empty tape.
 * \throw TypeError If the arguments contains an expression of type CONDITION.
 */
CallExpression::CallExpression(const Location &location, const Machine &machine, std::vector<std::unique_ptr<Expression>> arguments): Expression(location), machine(machine), arguments(std::move(arguments)) {
	std::ranges::for_each(this->arguments,
		[](const std::unique_ptr<Expression> &argument) -> void {
			if(argument->isCondition())
				throw TypeError(TypeError::Type::CONDITION_AS_A_CALL_ARGUMENT, argument->location);
		}
	);
};

Expression::Type CallExpression::getType() const {
	return Expression::Type::TAPE_RANGE;
};

Expression::Result CallExpression::build(InstructionBuilder &builder) const {
	size_t tape, argumentTape;
	Expression::TapeRange tapeRange;
	std::vector<std::unique_ptr<Expression>>::const_iterator it;

	if(!this->arguments.empty() && !this->arguments.front()->isConstant() && this->arguments.front()->isTapeTemporary()) { // Reusing the tape of the first argument is possible
		std::tie(tape, tapeRange) = this->arguments.front()->buildTape(builder);
		if(tapeRange.index0 > 0 && !tapeRange.isIndex0FromEnd)
			builder.addInstruction(std::make_unique<ClearInstruction>(tape, 0, tapeRange.index0));
		if(tapeRange.isIndex0FromEnd) {
			builder.addInstruction(std::make_unique<ReversePseudoinstruction>());
			builder.addInstruction(std::make_unique<ClearInstruction>(tape, tapeRange.index0, std::nullopt));
			builder.addInstruction(std::make_unique<ReversePseudoinstruction>());
		};

		if(!tapeRange.isIndex1FromEnd) {
			if(!tapeRange.isIndex1FromEnd)
				builder.addInstruction(std::make_unique<ClearInstruction>(tape, tapeRange.index1 - tapeRange.index0, std::nullopt));
			else
				throw UnexpectedError(L"Tape indices of type [−x:y] encountered.");
		}
		if(tapeRange.isIndex1FromEnd && tapeRange.index1 > 0)
			builder.addInstruction(std::make_unique<ClearInstruction>(tape, tapeRange.index1, std::nullopt, true));

		for(it = std::next(this->arguments.begin()); it!=this->arguments.end(); it++) {
			if((*it)->isConstant())
				builder.addInstruction(std::make_unique<WriteNumberInstruction>(tape, std::nullopt, (*it)->buildConstant(builder)));
			else {
				std::tie(argumentTape, tapeRange) = (*it)->buildTape(builder);
				if(tapeRange.isIndex0FromEnd==tapeRange.isIndex1FromEnd) // [x:y], [−x:−y]
					builder.addInstruction(std::make_unique<CopyInstruction>(argumentTape, tape, tapeRange.index0, tapeRange.index1, std::nullopt, tapeRange.isIndex0FromEnd));
				else if(tapeRange.isIndex1FromEnd) { // [x:−y]
					builder.addInstruction(std::make_unique<CopyInstruction>(argumentTape, tape, tapeRange.index0, std::nullopt, std::nullopt));
					if(tapeRange.index1 > 0)
						builder.addInstruction(std::make_unique<ClearInstruction>(tape, tapeRange.index1, std::nullopt, true));
				}
				else // [−x:y]
					throw UnexpectedError(L"Tape indices of type [−x:y] encountered.");
			};
		};
	}
	else {
		tape = builder.createTape();
		if(this->arguments.empty())
			builder.addInstruction(std::make_unique<ClearInstruction>(tape, 0, std::nullopt)); // Necessary for optimization (so that the original content is cleared if the tape is reused)
		for(it = this->arguments.begin(); it!=this->arguments.end(); it++) {
			if((*it)->isConstant())
				builder.addInstruction(std::make_unique<WriteNumberInstruction>(tape, (it==this->arguments.begin()) ? std::optional<size_t>(0) : std::nullopt, (*it)->buildConstant(builder)));
			else {
				std::tie(argumentTape, tapeRange) = (*it)->buildTape(builder);
				if(tapeRange.isIndex0FromEnd==tapeRange.isIndex1FromEnd)
					builder.addInstruction(std::make_unique<CopyInstruction>(argumentTape, tape, tapeRange.index0, tapeRange.index1, (it==this->arguments.begin()) ? std::optional<size_t>(0) : std::nullopt));
				else if(tapeRange.isIndex1FromEnd) {
					builder.addInstruction(std::make_unique<CopyInstruction>(argumentTape, tape, tapeRange.index0, std::nullopt, (it==this->arguments.begin()) ? std::optional<size_t>(0) : std::nullopt));
					if(tapeRange.index1 > 0)
						builder.addInstruction(std::make_unique<ClearInstruction>(tape, tapeRange.index1, std::nullopt, true));
				}
				else
					throw UnexpectedError(L"Tape indices of type [−x:y] encountered.");
			};
		};
	};

	builder.addInstruction(std::make_unique<CallInstruction>(tape, this->machine));

	return Expression::Result::createTapeRange(tape, Expression::TapeRange(0, 1, false, false));
};

std::optional<Expression::TapeRange> CallExpression::getArrayAccesRange() const {
	return {};
};

bool CallExpression::isTapeTemporary() const {
	return true;
};
