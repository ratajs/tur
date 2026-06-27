#include "./compareTapeLengthInstruction.hpp"
#include <initializer_list>
#include <functional>
#include "../IO/unexpectedError.hpp"

/*!
 * The constructor of CompareTapeLengthInstruction.
 * \param tape The tape where the compared number is.
 * \param number The constant value which is the second argument of the comparison.
 * \param trueLabel The label for the positive outcome.
 * \param trueLabel The label for the negative outcome.
 * \param type The binary logical operator to use.
 */
CompareTapeLengthInstruction::CompareTapeLengthInstruction(size_t tape, size_t number, size_t trueLabel, size_t falseLabel, CompareTapeLengthInstruction::Type type): type(type), tape(tape), number(number), trueLabel(trueLabel), falseLabel(falseLabel) {};

CompareTapeLengthInstruction::CompareTapeLengthInstruction(IrArguments &arguments) {
	arguments.readCharacter(L'|');
	this->tape = arguments.readTape();
	arguments.readCharacter(L'|');

	switch(arguments.readString({ L"=", L"≠", L"<", L"≤", L">", L"≥" })) {
		case 0:
			this->type = CompareTapeLengthInstruction::Type::EQ;

			break;

		case 1:
			this->type = CompareTapeLengthInstruction::Type::NE;

			break;

		case 2:
			this->type = CompareTapeLengthInstruction::Type::LT;

			break;

		case 3:
			this->type = CompareTapeLengthInstruction::Type::LTE;

			break;

		case 4:
			this->type = CompareTapeLengthInstruction::Type::GT;

			break;

		case 5:
			this->type = CompareTapeLengthInstruction::Type::GTE;

			break;
	};

	this->number = arguments.readNumber();
	arguments.readComma();
	this->trueLabel = arguments.readLabel();
	arguments.readComma();
	this->falseLabel = arguments.readLabel();
	arguments.end();
};

std::vector<size_t> CompareTapeLengthInstruction::listUsedTapes() const {
	return { this->tape };
};

void CompareTapeLengthInstruction::build(SingleTapeMachineFactory &machineFactory, std::function<size_t (size_t)> getRealTape, std::function<const std::wstring &(size_t)> getState) const {
	std::wstring trueState, falseState;

	if(getRealTape(this->tape)!=1)
		throw UnexpectedError(L"Other real tape than 1 appeared in a single tape machine instruction.");

	trueState = getState(this->trueLabel);
	falseState = getState(this->falseLabel);

	switch(this->type) {
		case CompareTapeLengthInstruction::Type::EQ:
			machineFactory.compareTapeLength(this->number, falseState, trueState, falseState);

			break;

		case CompareTapeLengthInstruction::Type::NE:
			machineFactory.compareTapeLength(this->number, trueState, falseState, trueState);

			break;

		case CompareTapeLengthInstruction::Type::LT:
			machineFactory.compareTapeLength(this->number, trueState, falseState, falseState);

			break;

		case CompareTapeLengthInstruction::Type::LTE:
			machineFactory.compareTapeLength(this->number, trueState, trueState, falseState);

			break;

		case CompareTapeLengthInstruction::Type::GT:
			machineFactory.compareTapeLength(this->number, falseState, falseState, trueState);

			break;

		case CompareTapeLengthInstruction::Type::GTE:
			machineFactory.compareTapeLength(this->number, falseState, trueState, trueState);

			break;
	};
};

void CompareTapeLengthInstruction::build(MultiTapeMachineFactory &machineFactory, std::function<size_t (size_t)> getRealTape, std::function<const std::wstring &(size_t)> getState) const {
	size_t realTape;
	std::wstring trueState, falseState;

	realTape = getRealTape(this->tape);
	trueState = getState(this->trueLabel);
	falseState = getState(this->falseLabel);

	switch(this->type) {
		case CompareTapeLengthInstruction::Type::EQ:
			machineFactory.compareTapeLength(realTape, this->number, falseState, trueState, falseState);

			break;

		case CompareTapeLengthInstruction::Type::NE:
			machineFactory.compareTapeLength(realTape, this->number, trueState, falseState, trueState);

			break;

		case CompareTapeLengthInstruction::Type::LT:
			machineFactory.compareTapeLength(realTape, this->number, trueState, falseState, falseState);

			break;

		case CompareTapeLengthInstruction::Type::LTE:
			machineFactory.compareTapeLength(realTape, this->number, trueState, trueState, falseState);

			break;

		case CompareTapeLengthInstruction::Type::GT:
			machineFactory.compareTapeLength(realTape, this->number, falseState, falseState, trueState);

			break;

		case CompareTapeLengthInstruction::Type::GTE:
			machineFactory.compareTapeLength(realTape, this->number, falseState, trueState, trueState);

			break;
	};
};

void CompareTapeLengthInstruction::print(std::wostream &stream, std::function<size_t (size_t)> getRealTape) const {
	stream << L"compareTapeLength(|";
	stream << getRealTape(this->tape);
	switch(this->type) {
		case CompareTapeLengthInstruction::Type::EQ:
			stream << L"| = ";

			break;

		case CompareTapeLengthInstruction::Type::NE:
			stream << L"| ≠ ";

			break;

		case CompareTapeLengthInstruction::Type::LT:
			stream << L"| < ";

			break;

		case CompareTapeLengthInstruction::Type::LTE:
			stream << L"| ≤ ";

			break;

		case CompareTapeLengthInstruction::Type::GT:
			stream << L"| > ";

			break;

		case CompareTapeLengthInstruction::Type::GTE:
			stream << L"| ≥ ";

			break;

	};
	stream << this->number;
	stream << L", " << this->trueLabel << L", " << this->falseLabel << L")" << std::endl;
};
