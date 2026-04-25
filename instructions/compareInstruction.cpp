#include "./compareInstruction.hpp"
#include "../IO/unexpectedError.hpp"

/*!
 * The constructor of CompareInstruction.
 * \param argumentA The first argument for the comparison. A pair means the tape number and index. A single numbers means the constant of this value.
 * \param argumentB The second argument for the comparison. A pair means the tape number and index. A single numbers means the constant of this value.
 * \param trueLabel The label for the positive outcome.
 * \param trueLabel The label for the negative outcome.
 * \param type The binary logical operator to use.
 */
CompareInstruction::CompareInstruction(std::variant<std::pair<size_t, size_t>, size_t> argumentA, std::variant<std::pair<size_t, size_t>, size_t> argumentB, size_t trueLabel, size_t falseLabel, CompareInstruction::Type type):
	type(type), trueLabel(trueLabel), falseLabel(falseLabel), argumentA(argumentA), argumentB(argumentB) {};

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
			return (std::get<size_t>(this->argumentA)==std::get<size_t>(this->argumentB));

		case CompareInstruction::Type::NE:
			return (std::get<size_t>(this->argumentA)!=std::get<size_t>(this->argumentB));

		case CompareInstruction::Type::LT:
			return (std::get<size_t>(this->argumentA) < std::get<size_t>(this->argumentB));

		case CompareInstruction::Type::LTE:
			return (std::get<size_t>(this->argumentA) <= std::get<size_t>(this->argumentB));

		case CompareInstruction::Type::GT:
			return (std::get<size_t>(this->argumentA) > std::get<size_t>(this->argumentB));

		case CompareInstruction::Type::GTE:
			return (std::get<size_t>(this->argumentA) >= std::get<size_t>(this->argumentB));

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
			machineFactory.compare(indexA, indexB, falseState, trueState, falseState);

			break;

		case CompareInstruction::Type::NE:
			machineFactory.compare(indexA, indexB, trueState, falseState, trueState);

			break;

		case CompareInstruction::Type::LT:
			machineFactory.compare(indexA, indexB, trueState, falseState, falseState);

			break;

		case CompareInstruction::Type::LTE:
			machineFactory.compare(indexA, indexB, trueState, trueState, falseState);

			break;

		case CompareInstruction::Type::GT:
			machineFactory.compare(indexA, indexB, falseState, falseState, trueState);

			break;

		case CompareInstruction::Type::GTE:
			machineFactory.compare(indexA, indexB, falseState, trueState, trueState);

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
			machineFactory.compare(tapeA, indexA, tapeB, indexB, falseState, trueState, falseState);

			break;

		case CompareInstruction::Type::NE:
			machineFactory.compare(tapeA, indexA, tapeB, indexB, trueState, falseState, trueState);

			break;

		case CompareInstruction::Type::LT:
			machineFactory.compare(tapeA, indexA, tapeB, indexB, trueState, falseState, falseState);

			break;

		case CompareInstruction::Type::LTE:
			machineFactory.compare(tapeA, indexA, tapeB, indexB, trueState, trueState, falseState);

			break;

		case CompareInstruction::Type::GT:
			machineFactory.compare(tapeA, indexA, tapeB, indexB, falseState, falseState, trueState);

			break;

		case CompareInstruction::Type::GTE:
			machineFactory.compare(tapeA, indexA, tapeB, indexB, falseState, trueState, trueState);

			break;
	};
};

std::vector<size_t> CompareInstruction::listUsedTapes() const {
	std::vector<size_t> tapes;

	if(std::holds_alternative<std::pair<size_t, size_t>>(this->argumentA))
		tapes.push_back(std::get<std::pair<size_t, size_t>>(this->argumentA).first);
	if(std::holds_alternative<std::pair<size_t, size_t>>(this->argumentB))
		tapes.push_back(std::get<std::pair<size_t, size_t>>(this->argumentB).first);

	return tapes;
};

void CompareInstruction::build(SingleTapeMachineFactory &machineFactory, std::function<size_t (size_t)> getRealTape, std::function<const std::wstring &(size_t)> getState) const {
	if((std::holds_alternative<std::pair<size_t, size_t>>(this->argumentA) && getRealTape(std::get<std::pair<size_t, size_t>>(this->argumentA).first)!=1) || (std::holds_alternative<std::pair<size_t, size_t>>(this->argumentB) && getRealTape(std::get<std::pair<size_t, size_t>>(this->argumentB).first)!=1))
		throw UnexpectedError(L"Other real tape than 1 appeared in a single tape machine instruction.");

	if(std::holds_alternative<size_t>(this->argumentA) && std::holds_alternative<size_t>(this->argumentB))
		machineFactory.addNTransition({}, this->isConstantConditionSatisfied() ? getState(this->trueLabel) : getState(this->falseLabel));
	else if(std::holds_alternative<size_t>(this->argumentB))
		this->buildSemiConstantComparison(machineFactory, std::get<std::pair<size_t, size_t>>(this->argumentA).second, std::get<size_t>(this->argumentB), getState(this->trueLabel), getState(this->falseLabel));
	else if(std::holds_alternative<size_t>(this->argumentA))
		this->buildSemiConstantComparison(machineFactory, std::get<std::pair<size_t, size_t>>(this->argumentB).second, std::get<size_t>(this->argumentA), getState(this->trueLabel), getState(this->falseLabel), true);
	else
		this->buildTapeComparison(machineFactory, std::get<std::pair<size_t, size_t>>(this->argumentA).second, std::get<std::pair<size_t, size_t>>(this->argumentB).second, getState(this->trueLabel), getState(this->falseLabel));
};

void CompareInstruction::build(MultiTapeMachineFactory &machineFactory, std::function<size_t (size_t)> getRealTape, std::function<const std::wstring &(size_t)> getState) const {
	if(std::holds_alternative<size_t>(this->argumentA) && std::holds_alternative<size_t>(this->argumentB))
		machineFactory.addNTransition({}, this->isConstantConditionSatisfied() ? getState(this->trueLabel) : getState(this->falseLabel));
	else if(std::holds_alternative<size_t>(this->argumentB))
		this->buildSemiConstantComparison(machineFactory, getRealTape(std::get<std::pair<size_t, size_t>>(this->argumentA).first), std::get<std::pair<size_t, size_t>>(this->argumentA).second, std::get<size_t>(this->argumentB), getState(this->trueLabel), getState(this->falseLabel));
	else if(std::holds_alternative<size_t>(this->argumentA))
		this->buildSemiConstantComparison(machineFactory, getRealTape(std::get<std::pair<size_t, size_t>>(this->argumentB).first), std::get<std::pair<size_t, size_t>>(this->argumentB).second, std::get<size_t>(this->argumentA), getState(this->trueLabel), getState(this->falseLabel), true);
	else
		this->buildTapeComparison(machineFactory, getRealTape(std::get<std::pair<size_t, size_t>>(this->argumentA).first), std::get<std::pair<size_t, size_t>>(this->argumentA).second, getRealTape(std::get<std::pair<size_t, size_t>>(this->argumentB).first), std::get<std::pair<size_t, size_t>>(this->argumentB).second, getState(this->trueLabel), getState(this->falseLabel));
};

void CompareInstruction::print(std::wostream &stream, std::function<size_t (size_t)> getRealTape) const {
	stream << L"compare(";
	if(std::holds_alternative<size_t>(this->argumentA))
		stream << std::get<size_t>(this->argumentA);
	else {
		stream <<
			getRealTape(std::get<std::pair<size_t, size_t>>(this->argumentA).first) <<
			L"[" << std::get<std::pair<size_t, size_t>>(this->argumentA).second << L"]";
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
	if(std::holds_alternative<size_t>(this->argumentB))
		stream << std::get<size_t>(this->argumentB);
	else {
		stream <<
			getRealTape(std::get<std::pair<size_t, size_t>>(this->argumentB).first) <<
			L"[" << std::get<std::pair<size_t, size_t>>(this->argumentB).second << L"]";
	};
	stream << L", " << this->trueLabel << L", " << this->falseLabel << L")" << std::endl;
};
