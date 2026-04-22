#include "./countExpression.hpp"
#include "../../instructions/copyInstruction.hpp"
#include "../../instructions/callInstruction.hpp"
#include "../../machine/machineLibrary.hpp"

/*!
 * The constructor or CountExpression.
 * \param location The location of the expression in code.
 * \param variable The measured variable.
 */
CountExpression::CountExpression(const Location &location, Variable &variable): Expression(location), variable(variable) {};

Expression::Type CountExpression::getType() const {
	return Expression::Type::TAPE_RANGE;
};

Expression::Result CountExpression::build(InstructionBuilder &builder) const {
	size_t tape;

	tape = builder.createTape();
	builder.addInstruction(std::make_unique<CopyInstruction>(*this->variable.tape, tape, 0, std::nullopt, 0));
	builder.addInstruction(std::make_unique<CallInstruction>(tape, MachineLibrary::COUNT));

	return Expression::Result::createTapeRange(tape, Expression::TapeRange(0, 1));
};

std::optional<Expression::TapeRange> CountExpression::getArrayAccesRange() const {
	return Expression::TapeRange(0, 1);
};

std::optional<std::reference_wrapper<const Variable>> CountExpression::getMeasuredVariable() const {
	return this->variable;
};

bool CountExpression::isTapeTemporary() const {
	return true;
};
