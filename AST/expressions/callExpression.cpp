#include "./callExpression.hpp"
#include "../../instructions/writeNumberInstruction.hpp"
#include "../../instructions/copyInstruction.hpp"
#include "../../instructions/callInstruction.hpp"
#include "../../instructions/clearInstruction.hpp"
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
	std::optional<size_t> firstIndex1;
	Expression::TapeRange tapeRange;
	std::vector<std::unique_ptr<Expression>>::const_iterator it;

	if(!this->arguments.empty() && !this->arguments.front()->isConstant() && this->arguments.front()->isTapeTemporary()) { // Reusing the tape of the first argument is possible
		std::tie(tape, tapeRange) = this->arguments.front()->buildTape(builder);
		if(tapeRange.index0 > 0)
			builder.addInstruction(std::make_unique<ClearInstruction>(tape, 0, tapeRange.index0));

		if(this->arguments.size()==1 && tapeRange.index1)
			builder.addInstruction(std::make_unique<ClearInstruction>(tape, *tapeRange.index1, std::nullopt));

		firstIndex1 = tapeRange.index1;

		for(it = std::next(this->arguments.begin()); it!=this->arguments.end(); it++) {
			if((*it)->isConstant())
				builder.addInstruction(std::make_unique<WriteNumberInstruction>(tape, (it==std::next(this->arguments.begin())) ? firstIndex1 : std::nullopt, (*it)->buildConstant(builder)));
			else {
				std::tie(argumentTape, tapeRange) = (*it)->buildTape(builder);
				builder.addInstruction(std::make_unique<CopyInstruction>(argumentTape, tape, tapeRange.index0, tapeRange.index1, (it==std::next(this->arguments.begin())) ? firstIndex1 : std::nullopt));
			};
		};
	}
	else {
		tape = builder.createTape();
		for(it = this->arguments.begin(); it!=this->arguments.end(); it++) {
			if((*it)->isConstant())
				builder.addInstruction(std::make_unique<WriteNumberInstruction>(tape, (it==this->arguments.begin()) ? std::optional<size_t>(0) : std::nullopt, (*it)->buildConstant(builder)));
			else {
				std::tie(argumentTape, tapeRange) = (*it)->buildTape(builder);
				builder.addInstruction(std::make_unique<CopyInstruction>(argumentTape, tape, tapeRange.index0, tapeRange.index1, (it==this->arguments.begin()) ? std::optional<size_t>(0) : std::nullopt));
			};
		};
	};

	builder.addInstruction(std::make_unique<CallInstruction>(tape, this->machine));

	return Expression::Result::createTapeRange(tape, Expression::TapeRange(0, 1));
};

std::optional<Expression::TapeRange> CallExpression::getArrayAccesRange() const {
	return {};
};

bool CallExpression::isTapeTemporary() const {
	return true;
};
