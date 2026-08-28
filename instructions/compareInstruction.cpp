#include "./compareInstruction.hpp"
#include <optional>
#include <tuple>
#include <initializer_list>
#include "../IO/unexpectedError.hpp"

/*!
 * The constructor of CompareInstruction.
 * \param argumentA The first argument for the comparison. A pair means the tape number and index. A single numbers means the constant of this value.
 * \param argumentB The second argument for the comparison. A pair means the tape number and index. A single numbers means the constant of this value.
 * \param trueLabel The label for the positive outcome.
 * \param trueLabel The label for the negative outcome.
 * \param type The binary logical operator to use.
 * \param isIndexBFromEnd Whether the index of the second number is indexed from the end (it must only be true if both arguments are tapes with indices).
 * \throw UnexpectedError If isIndexBFromEnd is true, but one of the arguments is a constant.
 */
CompareInstruction::CompareInstruction(std::variant<std::pair<size_t, size_t>, size_t> argumentA, std::variant<std::pair<size_t, size_t>, size_t> argumentB, size_t trueLabel, size_t falseLabel, CompareInstruction::Type type, bool isIndexBFromEnd):
	isArgumentAConstant(std::holds_alternative<size_t>(argumentA)), isArgumentBConstant(std::holds_alternative<size_t>(argumentB)), isIndexBFromEnd(isIndexBFromEnd), type(type), trueLabel(trueLabel), falseLabel(falseLabel) {
		if(isIndexBFromEnd && (this->isArgumentAConstant || this->isArgumentBConstant))
			throw UnexpectedError(L"isIndexBFromEnd is true, but one of the arguments of a comparison is a constant.");

		if(isArgumentAConstant)
			this->numberA = std::get<size_t>(argumentA);
		else
			std::tie(this->tapeA, this->indexA) = std::get<std::pair<size_t, size_t>>(argumentA);

		if(isArgumentBConstant)
			this->numberB = std::get<size_t>(argumentB);
		else
			std::tie(this->tapeB, this->indexB) = std::get<std::pair<size_t, size_t>>(argumentB);
	};

/*!
 * An alternative constructor of CompareInstruction.
 * The arguments are extracted from an instance of IrArguments.
 * The first argument, type, the second argument, comma, the true label, comma, and the false label is expected in the arguments.
 * An argument is either a tape number followed by an index in square brackets or just a number.
 * Type can be one of the following: =, ≠, <, ≤, >, ≥
 * \param arguments The arguments of the instruction from the IR input.
 * \throw IrParseError If the arguments do not match the expected format.
 */
CompareInstruction::CompareInstruction(IrArguments &arguments) {
	std::variant<std::tuple<size_t, size_t, bool>, size_t> argument;

	argument = arguments.readTapeAndIndexOrNumber();
	this->isArgumentAConstant = std::holds_alternative<size_t>(argument);
	if(this->isArgumentAConstant)
		this->numberA = std::get<size_t>(argument);
	else
		std::tie(this->tapeA, this->indexA, std::ignore) = std::get<std::tuple<size_t, size_t, bool>>(argument);

	switch(arguments.readString({ L"=", L"≠", L"<", L"≤", L">", L"≥" })) {
		case 0:
			this->type = CompareInstruction::Type::EQ;

			break;

		case 1:
			this->type = CompareInstruction::Type::NE;

			break;

		case 2:
			this->type = CompareInstruction::Type::LT;

			break;

		case 3:
			this->type = CompareInstruction::Type::LTE;

			break;

		case 4:
			this->type = CompareInstruction::Type::GT;

			break;

		case 5:
			this->type = CompareInstruction::Type::GTE;

			break;
	};

	argument = arguments.readTapeAndIndexOrNumber(true);
	this->isArgumentBConstant = std::holds_alternative<size_t>(argument);
	if(this->isArgumentBConstant)
		this->numberB = std::get<size_t>(argument);
	else
		std::tie(this->tapeB, this->indexB, this->isIndexBFromEnd) = std::get<std::tuple<size_t, size_t, bool>>(argument);

	if(this->isIndexBFromEnd)
		this->indexB--; // Index from zero instead of from 1

	arguments.readComma();
	this->trueLabel = arguments.readLabel();
	arguments.readComma();
	this->falseLabel = arguments.readLabel();
	arguments.end();
};

/*!
 * \return The operator which can be used after swapping arguments.
 */
CompareInstruction::Type CompareInstruction::getInvertedType() const {
	switch(this->type) {
		case CompareInstruction::Type::EQ:
		case CompareInstruction::Type::NE:
			return this->type;

		case CompareInstruction::Type::LT:
			return CompareInstruction::Type::GT;

		case CompareInstruction::Type::LTE:
			return CompareInstruction::Type::GTE;

		case CompareInstruction::Type::GT:
			return CompareInstruction::Type::LT;

		case CompareInstruction::Type::GTE:
			return CompareInstruction::Type::LTE;

		default:
			std::unreachable();
	};
};

/*!
 * This is called if both arguments are constant values.
 * \return The truth value of the comparison.
 */
bool CompareInstruction::isConstantConditionSatisfied() const {
	switch(this->type) {
		case CompareInstruction::Type::EQ:
			return (this->numberA==this->numberB);

		case CompareInstruction::Type::NE:
			return (this->numberA!=this->numberB);

		case CompareInstruction::Type::LT:
			return (this->numberA < this->numberB);

		case CompareInstruction::Type::LTE:
			return (this->numberA <= this->numberB);

		case CompareInstruction::Type::GT:
			return (this->numberA > this->numberB);

		case CompareInstruction::Type::GTE:
			return (this->numberA >= this->numberB);

		default:
			std::unreachable();
	};
};

/*!
 * This is called if one of the arguments is a number on a tape, while the other argument is a constant value.
 * This is the single-tape variant.
 * \param machineFactory The SingleTapeMachineFactory object for building.
 * \param index The index on the tape to compare with a constant.
 * \param number The constant.
 * \param trueState The name of the state for the positive outcome.
 * \param falseState The name of the state for the negative outcome.
 * \param isInverted This is true when the first argument of the comparison should be the constant, otherwise false.
 */
void CompareInstruction::buildSemiConstantComparison(SingleTapeMachineFactory &machineFactory, size_t index, size_t number, const std::wstring &trueState, const std::wstring &falseState, bool isInverted) const {
	switch(isInverted ? this->getInvertedType() : this->type) {
		case CompareInstruction::Type::EQ:
			machineFactory.compareWithConstant(index, number, falseState, trueState, falseState);

			break;

		case CompareInstruction::Type::NE:
			machineFactory.compareWithConstant(index, number, trueState, falseState, trueState);

			break;

		case CompareInstruction::Type::LT:
			machineFactory.compareWithConstant(index, number, trueState, falseState, falseState);

			break;

		case CompareInstruction::Type::LTE:
			machineFactory.compareWithConstant(index, number, trueState, trueState, falseState);

			break;

		case CompareInstruction::Type::GT:
			machineFactory.compareWithConstant(index, number, falseState, falseState, trueState);

			break;

		case CompareInstruction::Type::GTE:
			machineFactory.compareWithConstant(index, number, falseState, trueState, trueState);

			break;
	};
};

/*!
 * This is called if one of the arguments is a number on a tape, while the other argument is a constant value.
 * This is the multi-tape variant.
 * \param machineFactory The MultiTapeMachineFactory object for building.
 * \param tape The tape where the compared number is found.
 * \param index The index on the tape to compare with a constant.
 * \param number The constant.
 * \param trueState The name of the state for the positive outcome.
 * \param falseState The name of the state for the negative outcome.
 * \param isInverted This is true when the first argument of the comparison should be the constant, otherwise false.
 */
void CompareInstruction::buildSemiConstantComparison(MultiTapeMachineFactory &machineFactory, size_t tape, size_t index, size_t number, const std::wstring &trueState, const std::wstring &falseState, bool isInverted) const {
	switch(isInverted ? this->getInvertedType() : this->type) {
		case CompareInstruction::Type::EQ:
			machineFactory.compareWithConstant(tape, index, number, falseState, trueState, falseState);

			break;

		case CompareInstruction::Type::NE:
			machineFactory.compareWithConstant(tape, index, number, trueState, falseState, trueState);

			break;

		case CompareInstruction::Type::LT:
			machineFactory.compareWithConstant(tape, index, number, trueState, falseState, falseState);

			break;

		case CompareInstruction::Type::LTE:
			machineFactory.compareWithConstant(tape, index, number, trueState, trueState, falseState);

			break;

		case CompareInstruction::Type::GT:
			machineFactory.compareWithConstant(tape, index, number, falseState, falseState, trueState);

			break;

		case CompareInstruction::Type::GTE:
			machineFactory.compareWithConstant(tape, index, number, falseState, trueState, trueState);

			break;
	};
};

/*!
 * This is called if both arguments are numbers on tapes.
 * This is the single-tape variant.
 * \param machineFactory The SingleTapeMachineFactory object for building.
 * \param indexA The index on the tape representing the first argument.
 * \param indexB The index on the tape representing the second argument.
 * \param trueState The name of the state for the positive outcome.
 * \param falseState The name of the state for the negative outcome.
 */
void CompareInstruction::buildTapeComparison(SingleTapeMachineFactory &machineFactory, size_t indexA, size_t indexB, const std::wstring &trueState, const std::wstring &falseState) const {
	switch(this->type) {
		case CompareInstruction::Type::EQ:
			machineFactory.compare(indexA, indexB, falseState, trueState, falseState, this->isIndexBFromEnd);

			break;

		case CompareInstruction::Type::NE:
			machineFactory.compare(indexA, indexB, trueState, falseState, trueState, this->isIndexBFromEnd);

			break;

		case CompareInstruction::Type::LT:
			machineFactory.compare(indexA, indexB, trueState, falseState, falseState, this->isIndexBFromEnd);

			break;

		case CompareInstruction::Type::LTE:
			machineFactory.compare(indexA, indexB, trueState, trueState, falseState, this->isIndexBFromEnd);

			break;

		case CompareInstruction::Type::GT:
			machineFactory.compare(indexA, indexB, falseState, falseState, trueState, this->isIndexBFromEnd);

			break;

		case CompareInstruction::Type::GTE:
			machineFactory.compare(indexA, indexB, falseState, trueState, trueState, this->isIndexBFromEnd);

			break;
	};
};

/*!
 * This is called if both arguments are numbers on tapes.
 * This is the multi-tape variant.
 * \param machineFactory The MultiTapeMachineFactory object for building.
 * \param tapeA The tape where the first argument is found.
 * \param indexA The index on the tape representing the first argument.
 * \param tapeB The tape where the first argument is found.
 * \param indexB The index on the tape representing the second argument.
 * \param trueState The name of the state for the positive outcome.
 * \param falseState The name of the state for the negative outcome.
 */
void CompareInstruction::buildTapeComparison(MultiTapeMachineFactory &machineFactory, size_t tapeA, size_t indexA, size_t tapeB, size_t indexB, const std::wstring &trueState, const std::wstring &falseState) const {
	switch(this->type) {
		case CompareInstruction::Type::EQ:
			machineFactory.compare(tapeA, indexA, tapeB, indexB, falseState, trueState, falseState, this->isIndexBFromEnd);

			break;

		case CompareInstruction::Type::NE:
			machineFactory.compare(tapeA, indexA, tapeB, indexB, trueState, falseState, trueState, this->isIndexBFromEnd);

			break;

		case CompareInstruction::Type::LT:
			machineFactory.compare(tapeA, indexA, tapeB, indexB, trueState, falseState, falseState, this->isIndexBFromEnd);

			break;

		case CompareInstruction::Type::LTE:
			machineFactory.compare(tapeA, indexA, tapeB, indexB, trueState, trueState, falseState, this->isIndexBFromEnd);

			break;

		case CompareInstruction::Type::GT:
			machineFactory.compare(tapeA, indexA, tapeB, indexB, falseState, falseState, trueState, this->isIndexBFromEnd);

			break;

		case CompareInstruction::Type::GTE:
			machineFactory.compare(tapeA, indexA, tapeB, indexB, falseState, trueState, trueState, this->isIndexBFromEnd);

			break;
	};
};

std::vector<size_t> CompareInstruction::listUsedTapes() const {
	std::vector<size_t> tapes;

	if(!this->isArgumentAConstant)
		tapes.push_back(this->tapeA);
	if(!this->isArgumentBConstant)
		tapes.push_back(this->tapeB);

	return tapes;
};

bool CompareInstruction::isGoToInstruction() const {
	return true;
};

std::vector<size_t> CompareInstruction::getGoToDestinations() const {
	return { this->trueLabel, this->falseLabel };
};

void CompareInstruction::build(SingleTapeMachineFactory &machineFactory, std::function<size_t (size_t)> getRealTape, std::function<const std::wstring &(size_t)> getState) const {
	if((!this->isArgumentAConstant && getRealTape(this->tapeA)!=1) || (!this->isArgumentBConstant && getRealTape(this->tapeB)!=1))
		throw UnexpectedError(L"Other real tape than 1 appeared in a single tape machine instruction.");

	if(this->isArgumentAConstant && this->isArgumentBConstant)
		machineFactory.addNTransition({}, this->isConstantConditionSatisfied() ? getState(this->trueLabel) : getState(this->falseLabel));
	else if(this->isArgumentBConstant)
		this->buildSemiConstantComparison(machineFactory, this->indexA, this->numberB, getState(this->trueLabel), getState(this->falseLabel));
	else if(this->isArgumentAConstant)
		this->buildSemiConstantComparison(machineFactory, this->tapeB, this->numberA, getState(this->trueLabel), getState(this->falseLabel), true);
	else
		this->buildTapeComparison(machineFactory, this->indexA, this->indexB, getState(this->trueLabel), getState(this->falseLabel));
};

void CompareInstruction::build(MultiTapeMachineFactory &machineFactory, std::function<size_t (size_t)> getRealTape, std::function<const std::wstring &(size_t)> getState) const {
	if(this->isArgumentAConstant && this->isArgumentBConstant)
		machineFactory.addNTransition({}, this->isConstantConditionSatisfied() ? getState(this->trueLabel) : getState(this->falseLabel));
	else if(this->isArgumentBConstant)
		this->buildSemiConstantComparison(machineFactory, getRealTape(this->tapeA), this->indexA, this->numberB, getState(this->trueLabel), getState(this->falseLabel));
	else if(this->isArgumentAConstant)
		this->buildSemiConstantComparison(machineFactory, getRealTape(this->tapeB), this->indexB, this->numberA, getState(this->trueLabel), getState(this->falseLabel), true);
	else
		this->buildTapeComparison(machineFactory, getRealTape(this->tapeA), this->indexA, getRealTape(this->tapeB), this->indexB, getState(this->trueLabel), getState(this->falseLabel));
};

void CompareInstruction::print(std::wostream &stream, std::function<size_t (size_t)> getRealTape) const {
	stream << L"compare(";
	if(this->isArgumentAConstant)
		stream << this->numberA;
	else {
		stream <<
			getRealTape(this->tapeA) <<
			L"[" << this->indexA << L"]";
	};
	switch(this->type) {
		case CompareInstruction::Type::EQ:
			stream << L" = ";

			break;

		case CompareInstruction::Type::NE:
			stream << L" ≠ ";

			break;

		case CompareInstruction::Type::LT:
			stream << L" < ";

			break;

		case CompareInstruction::Type::LTE:
			stream << L" ≤ ";

			break;

		case CompareInstruction::Type::GT:
			stream << L" > ";

			break;

		case CompareInstruction::Type::GTE:
			stream << L" ≥ ";

			break;
	};
	if(this->isArgumentBConstant)
		stream << this->numberB;
	else {
		stream <<
			getRealTape(this->tapeB) <<
			L"[" << (this->isIndexBFromEnd ? L"−" : L"") << (this->isIndexBFromEnd ? (this->indexB + 1) : this->indexB) << L"]";
	};
	stream << L", " << this->trueLabel << L", " << this->falseLabel << L")" << std::endl;
};
