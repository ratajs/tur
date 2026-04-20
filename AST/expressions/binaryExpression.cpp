#include "./binaryExpression.hpp"
#include "../../instructions/clearInstruction.hpp"
#include "../../instructions/copyInstruction.hpp"
#include "../../instructions/writeNumberInstruction.hpp"
#include "../../instructions/callInstruction.hpp"
#include "../../instructions/jumpInstruction.hpp"
#include "../../instructions/compareInstruction.hpp"
#include "../../instructions/compareTapeLengthInstruction.hpp"
#include "../../machine/machineLibrary.hpp"
#include "../../IO/typeError.hpp"

/*!
 * The constructor of BinaryExpression.
 * \param location The location of the expression in code.
 * \param expressionA The first operand.
 * \param expressionB The second operand.
 * \param type The operator to apply.
 * \throw TypeError If the operator is incompatible with the operands.
 */
BinaryExpression::BinaryExpression(const Location &location, std::unique_ptr<Expression> expressionA, std::unique_ptr<Expression> expressionB, BinaryExpression::Type type):
	Expression(location), type(type), expressionA(std::move(expressionA)), expressionB(std::move(expressionB)) {
		this->validate();
		this->swapIfAdvantageous();
	};

/*!
 * An alternative constructor of BinaryExpression.
 * The location is composed from the locations of the operands (beginning at the first operand’s beginning, end at the second operand’s end).
 * \param expressionA The first operand.
 * \param expressionB The second operand.
 * \param type The operator to apply.
 * \throw TypeError If the operator is incompatible with the operands.
 */
BinaryExpression::BinaryExpression(std::unique_ptr<Expression> expressionA, std::unique_ptr<Expression> expressionB, BinaryExpression::Type type):
	Expression({ expressionA->location, expressionB->location }), type(type), expressionA(std::move(expressionA)), expressionB(std::move(expressionB)) {
		this->validate();
		this->swapIfAdvantageous();
	};

/*!
 * A helper function to throw the TypeError for the constructor if the types do not match.
 * \throw TypeError If the operator is incompatible with the operands.
 */
void BinaryExpression::validate() const {
	switch(this->type) {
		case BinaryExpression::Type::PLUS:
		case BinaryExpression::Type::MINUS:
		case BinaryExpression::Type::MUL:
		case BinaryExpression::Type::DIV:
		case BinaryExpression::Type::MOD:
		case BinaryExpression::Type::MIN:
		case BinaryExpression::Type::MAX:
		case BinaryExpression::Type::EQ:
		case BinaryExpression::Type::NE:
		case BinaryExpression::Type::LTE:
		case BinaryExpression::Type::LT:
		case BinaryExpression::Type::GTE:
		case BinaryExpression::Type::GT:
			if(this->expressionA->isCondition())
				throw TypeError(TypeError::Type::CONDITION_IN_AN_ARITHMETIC_OPERATION, this->expressionA->location);

			if(this->expressionB->isCondition())
				throw TypeError(TypeError::Type::CONDITION_IN_AN_ARITHMETIC_OPERATION, this->expressionB->location);

			if(this->expressionA->getArrayAccessLength()!=0 && this->expressionA->getArrayAccessLength()!=1)
				throw TypeError(TypeError::Type::LONG_ARRAY_ACCESS_IN_AN_ARITHMETIC_OPERATION, this->expressionA->location);

			if(this->expressionB->getArrayAccessLength()!=0 && this->expressionB->getArrayAccessLength()!=1)
				throw TypeError(TypeError::Type::LONG_ARRAY_ACCESS_IN_AN_ARITHMETIC_OPERATION, this->expressionB->location);

			break;

		case BinaryExpression::Type::AND:
		case BinaryExpression::Type::OR:
		case BinaryExpression::Type::XOR:
			if(!this->expressionA->isCondition())
				throw TypeError(TypeError::Type::NON_CONDITION_IN_A_LOGICAL_OPERATION, this->expressionA->location);

			if(!this->expressionB->isCondition())
				throw TypeError(TypeError::Type::NON_CONDITION_IN_A_LOGICAL_OPERATION, this->expressionB->location);

			break;
	};
};

/*!
 * Swap the two operands if one of these condition is satisfied:
 * The first operand is a constant, the second not and the operation is commutative.
 * Both operands are of the TAPE_RANGE type, but only the second one returns a temporary tape.
 * The first operand is a constant and the second is a CountExpression, the operator changes if it is not commutative to reflect the swapping.
 */
void BinaryExpression::swapIfAdvantageous() {
	if(this->isCommutative()) {
		if(this->expressionA->isConstant() && !this->expressionB->isConstant())
			std::swap(this->expressionA, this->expressionB);
		if(this->expressionA->getType()==Expression::Type::TAPE_RANGE && this->expressionB->getType()==Expression::Type::TAPE_RANGE && !this->expressionA->isTapeTemporary() && this->expressionB->isTapeTemporary())
			std::swap(this->expressionA, this->expressionB);

		return;
	};

	if(this->expressionA->isConstant() && this->expressionB->getMeasuredVariable()) {
		std::swap(this->expressionA, this->expressionB);
		switch(this->type) {
			case BinaryExpression::Type::LT:
				this->type = BinaryExpression::Type::GT;

				return;

			case BinaryExpression::Type::LTE:
				this->type = BinaryExpression::Type::GTE;

				return;

			case BinaryExpression::Type::GT:
				this->type = BinaryExpression::Type::LT;

				return;

			case BinaryExpression::Type::GTE:
				this->type = BinaryExpression::Type::LTE;

				return;

			default:
				std::unreachable();
		};
	};
};

/*!
 * \return Whether the used operator is commutative.
 */
bool BinaryExpression::isCommutative() const {
	switch(this->type) {
		case BinaryExpression::Type::PLUS:
		case BinaryExpression::Type::MUL:
		case BinaryExpression::Type::MIN:
		case BinaryExpression::Type::MAX:
		case BinaryExpression::Type::EQ:
		case BinaryExpression::Type::NE:
		case BinaryExpression::Type::AND:
		case BinaryExpression::Type::OR:
		case BinaryExpression::Type::XOR:
			return true;

		case BinaryExpression::Type::MINUS:
		case BinaryExpression::Type::DIV:
		case BinaryExpression::Type::MOD:
		case BinaryExpression::Type::LTE:
		case BinaryExpression::Type::LT:
		case BinaryExpression::Type::GTE:
		case BinaryExpression::Type::GT:
			return false;

		default:
			std::unreachable();
	};
};

/*!
 * This is called if both of the arguments are constant values.
 * \param argumentA The value of the first operand.
 * \param argumentB The value of the second operand.
 * \return The result of the operation.
 */
size_t BinaryExpression::computeConstant(size_t argumentA, size_t argumentB) const { // Overflow?
	switch(this->type) {
		case BinaryExpression::Type::PLUS:
			return (argumentA + argumentB);

		case BinaryExpression::Type::MINUS:
			return ((argumentA > argumentB) ? (argumentA - argumentB) : 0);

		case BinaryExpression::Type::MUL:
			return (argumentA * argumentB);

		case BinaryExpression::Type::DIV:
			return (argumentA / argumentB);

		case BinaryExpression::Type::MOD:
			return (argumentA % argumentB);

		case BinaryExpression::Type::MIN:
			return ((argumentA > argumentB) ? argumentB : argumentA);

		case BinaryExpression::Type::MAX:
			return ((argumentA > argumentB) ? argumentA : argumentB);

		default:
			std::unreachable();
	};
};

/*!
 * This is called if the result of the operator is of the type CONDITION.
 * \param builder The InstructionBuilder object used for building.
 * \return The result of the expression.
 */
Expression::Result BinaryExpression::buildCondition(InstructionBuilder &builder) const {
	size_t trueLabelA, falseLabelA, trueLabelB, falseLabelB, trueLabel, falseLabel;

	trueLabel = builder.createLabel();
	falseLabel = builder.createLabel();

	switch(this->type) {
		case BinaryExpression::Type::AND:
			std::tie(trueLabelA, falseLabelA) = this->expressionA->buildCondition(builder);
			builder.addInstruction(std::make_unique<JumpInstruction>(falseLabelA, JumpInstruction::Type::COME_FROM)); // false and ?
			builder.addInstruction(std::make_unique<JumpInstruction>(falseLabel, JumpInstruction::Type::GO_TO)); // outcome: false
			builder.addInstruction(std::make_unique<JumpInstruction>(trueLabelA, JumpInstruction::Type::COME_FROM)); // true and ?
			std::tie(trueLabelB, falseLabelB) = this->expressionB->buildCondition(builder);
			builder.addInstruction(std::make_unique<JumpInstruction>(falseLabelB, JumpInstruction::Type::COME_FROM)); // true and false
			builder.addInstruction(std::make_unique<JumpInstruction>(falseLabel, JumpInstruction::Type::GO_TO)); // outcome: false
			builder.addInstruction(std::make_unique<JumpInstruction>(trueLabelB, JumpInstruction::Type::COME_FROM)); // true and true
			builder.addInstruction(std::make_unique<JumpInstruction>(trueLabel, JumpInstruction::Type::GO_TO)); // outcome: true

			break;

		case BinaryExpression::Type::OR:
			std::tie(trueLabelA, falseLabelA) = this->expressionA->buildCondition(builder);
			builder.addInstruction(std::make_unique<JumpInstruction>(trueLabelA, JumpInstruction::Type::COME_FROM)); // true or ?
			builder.addInstruction(std::make_unique<JumpInstruction>(trueLabel, JumpInstruction::Type::GO_TO)); // outcome: true
			builder.addInstruction(std::make_unique<JumpInstruction>(falseLabelA, JumpInstruction::Type::COME_FROM)); // false or ?
			std::tie(trueLabelB, falseLabelB) = this->expressionB->buildCondition(builder);
			builder.addInstruction(std::make_unique<JumpInstruction>(trueLabelB, JumpInstruction::Type::COME_FROM)); // false or true
			builder.addInstruction(std::make_unique<JumpInstruction>(trueLabel, JumpInstruction::Type::GO_TO)); // outcome: true
			builder.addInstruction(std::make_unique<JumpInstruction>(falseLabelB, JumpInstruction::Type::COME_FROM)); // false or false
			builder.addInstruction(std::make_unique<JumpInstruction>(falseLabel, JumpInstruction::Type::GO_TO)); // outcome: false

			break;

		case BinaryExpression::Type::XOR:
			std::tie(trueLabelA, falseLabelA) = this->expressionA->buildCondition(builder);
			builder.addInstruction(std::make_unique<JumpInstruction>(trueLabelA, JumpInstruction::Type::COME_FROM)); // true xor ?
			std::tie(trueLabelB, falseLabelB) = this->expressionB->buildCondition(builder);
			builder.addInstruction(std::make_unique<JumpInstruction>(trueLabelB, JumpInstruction::Type::COME_FROM)); // true xor true
			builder.addInstruction(std::make_unique<JumpInstruction>(falseLabel, JumpInstruction::Type::GO_TO)); // outcome: false
			builder.addInstruction(std::make_unique<JumpInstruction>(falseLabelB, JumpInstruction::Type::COME_FROM)); // true xor false
			builder.addInstruction(std::make_unique<JumpInstruction>(trueLabel, JumpInstruction::Type::GO_TO)); // outcome: true
			builder.addInstruction(std::make_unique<JumpInstruction>(falseLabelA, JumpInstruction::Type::COME_FROM)); // false xor ?
			std::tie(trueLabelB, falseLabelB) = this->expressionB->buildCondition(builder);
			builder.addInstruction(std::make_unique<JumpInstruction>(trueLabelB, JumpInstruction::Type::COME_FROM)); // false xor true
			builder.addInstruction(std::make_unique<JumpInstruction>(trueLabel, JumpInstruction::Type::GO_TO)); // outcome: true
			builder.addInstruction(std::make_unique<JumpInstruction>(falseLabelB, JumpInstruction::Type::COME_FROM)); // false xor false
			builder.addInstruction(std::make_unique<JumpInstruction>(falseLabel, JumpInstruction::Type::GO_TO)); // outcome: false

			break;

		default:
			if(this->expressionA->getMeasuredVariable() && this->expressionB->isConstant())
				this->buildTapeLengthComparison(builder, trueLabel, falseLabel);
			else
				this->buildComparison(builder, trueLabel, falseLabel);
	};

	return Expression::Result::create<Expression::Type::CONDITION>(trueLabel, falseLabel);
};

/*!
 * This is called if the operator is a comparison between two non-conditions where at least one of the operands is not constant and it is not a comparison between a tape length and a constant.
 * \param builder The InstructionBuilder object used for building.
 * \param trueLabel Label used to signal a positive outcome.
 * \param falseLabel Label used to signal a negative outcome.
 */
void BinaryExpression::buildComparison(InstructionBuilder &builder, size_t trueLabel, size_t falseLabel) const {
	std::variant<std::pair<size_t, size_t>, size_t> argumentA, argumentB;
	Expression::TapeRange range;

	if(this->expressionA->isConstant())
		argumentA = this->expressionA->buildConstant(builder);
	else {
		argumentA = std::pair<size_t, size_t>();
		std::tie(std::get<std::pair<size_t, size_t>>(argumentA).first, range) = this->expressionA->buildTape(builder);
		std::get<std::pair<size_t, size_t>>(argumentA).second = range.index0;
	};
	if(this->expressionB->isConstant())
		argumentB = this->expressionB->buildConstant(builder);
	else {
		argumentB = std::pair<size_t, size_t>();
		std::tie(std::get<std::pair<size_t, size_t>>(argumentB).first, range) = this->expressionB->buildTape(builder);
		std::get<std::pair<size_t, size_t>>(argumentB).second = range.index0;
	};
	switch(this->type) {
		case BinaryExpression::Type::EQ:
			builder.addInstruction(std::make_unique<CompareInstruction>(argumentA, argumentB, trueLabel, falseLabel, CompareInstruction::Type::EQ));

			break;

		case BinaryExpression::Type::NE:
			builder.addInstruction(std::make_unique<CompareInstruction>(argumentA, argumentB, trueLabel, falseLabel, CompareInstruction::Type::NE));

			break;

		case BinaryExpression::Type::LTE:
			builder.addInstruction(std::make_unique<CompareInstruction>(argumentA, argumentB, trueLabel, falseLabel, CompareInstruction::Type::LTE));

			break;

		case BinaryExpression::Type::LT:
			builder.addInstruction(std::make_unique<CompareInstruction>(argumentA, argumentB, trueLabel, falseLabel, CompareInstruction::Type::LT));

			break;

		case BinaryExpression::Type::GTE:
			builder.addInstruction(std::make_unique<CompareInstruction>(argumentA, argumentB, trueLabel, falseLabel, CompareInstruction::Type::GTE));

			break;

		case BinaryExpression::Type::GT:
			builder.addInstruction(std::make_unique<CompareInstruction>(argumentA, argumentB, trueLabel, falseLabel, CompareInstruction::Type::GT));

			break;

		default:
			std::unreachable();
	};
};

/*!
 * This is called if the operation is a comparison between a tape length and a constant.
 * The tape length is always the first operand (it has been swapped if it had not been the case).
 * Running of the machine for counting is not needed, CompareTapeLengthInstruction instruction is simpler, it does not involve any copying, so it is preferred.
 * \param builder The InstructionBuilder object used for building.
 * \param trueLabel Label used to signal a positive outcome.
 * \param falseLabel Label used to signal a negative outcome.
 */
void BinaryExpression::buildTapeLengthComparison(InstructionBuilder &builder, size_t trueLabel, size_t falseLabel) const {
	size_t tape, constant;

	tape = (*this->expressionA->getMeasuredVariable()->get().tape);
	constant = this->expressionB->buildConstant(builder);

	switch(this->type) {
		case BinaryExpression::Type::EQ:
			builder.addInstruction(std::make_unique<CompareTapeLengthInstruction>(tape, constant, trueLabel, falseLabel, CompareTapeLengthInstruction::Type::EQ));

			break;

		case BinaryExpression::Type::NE:
			builder.addInstruction(std::make_unique<CompareTapeLengthInstruction>(tape, constant, trueLabel, falseLabel, CompareTapeLengthInstruction::Type::NE));

			break;

		case BinaryExpression::Type::LTE:
			builder.addInstruction(std::make_unique<CompareTapeLengthInstruction>(tape, constant, trueLabel, falseLabel, CompareTapeLengthInstruction::Type::LTE));

			break;

		case BinaryExpression::Type::LT:
			builder.addInstruction(std::make_unique<CompareTapeLengthInstruction>(tape, constant, trueLabel, falseLabel, CompareTapeLengthInstruction::Type::LT));

			break;

		case BinaryExpression::Type::GTE:
			builder.addInstruction(std::make_unique<CompareTapeLengthInstruction>(tape, constant, trueLabel, falseLabel, CompareTapeLengthInstruction::Type::GTE));

			break;

		case BinaryExpression::Type::GT:
			builder.addInstruction(std::make_unique<CompareTapeLengthInstruction>(tape, constant, trueLabel, falseLabel, CompareTapeLengthInstruction::Type::GT));

			break;

		default:
			std::unreachable();
	};
};

/*!
 * A helper function to get the correct machine to perform an arithmetical operation.
 * \return The machine for the used operation.
 */
const Machine &BinaryExpression::getMachine() const {
	switch(this->type) {
		case BinaryExpression::Type::PLUS:
			return MachineLibrary::ADD;

		case BinaryExpression::Type::MINUS:
			return MachineLibrary::SUB;

		case BinaryExpression::Type::MUL:
			return MachineLibrary::MUL;

		case BinaryExpression::Type::DIV:
			return MachineLibrary::DIV;

		case BinaryExpression::Type::MOD:
			return MachineLibrary::MOD;

		case BinaryExpression::Type::MIN:
			return MachineLibrary::MIN;

		case BinaryExpression::Type::MAX:
			return MachineLibrary::MAX;

		default:
			std::unreachable();
	};
};

Expression::Type BinaryExpression::getType() const {
	switch(type) {
		case BinaryExpression::Type::PLUS:
		case BinaryExpression::Type::MINUS:
		case BinaryExpression::Type::MUL:
		case BinaryExpression::Type::DIV:
		case BinaryExpression::Type::MOD:
		case BinaryExpression::Type::MIN:
		case BinaryExpression::Type::MAX:
			if(this->expressionA->isConstant() && this->expressionB->isConstant())
				return Expression::Type::CONSTANT;
			else
				return Expression::Type::TAPE_RANGE;

		case BinaryExpression::Type::EQ:
		case BinaryExpression::Type::NE:
		case BinaryExpression::Type::LTE:
		case BinaryExpression::Type::LT:
		case BinaryExpression::Type::GTE:
		case BinaryExpression::Type::GT:
		case BinaryExpression::Type::AND:
		case BinaryExpression::Type::OR:
		case BinaryExpression::Type::XOR:
			return Expression::Type::CONDITION;

		default:
			std::unreachable();
	};
};

Expression::Result BinaryExpression::build(InstructionBuilder &builder) const {
	size_t tape, newTape;
	Expression::TapeRange range;

	if(this->isConstant())
		return Expression::Result::create<Expression::Type::CONSTANT>(this->computeConstant(this->expressionA->buildConstant(builder), this->expressionB->buildConstant(builder)));

	if(this->isCondition())
		return this->buildCondition(builder);

	if(this->expressionA->getType()==Expression::Type::TAPE_RANGE && this->expressionA->isTapeTemporary()) {
		std::tie(tape, range) = this->expressionA->buildTape(builder);
		if(range.index0 > 0)
			builder.addInstruction(std::make_unique<ClearInstruction>(tape, 0, range.index0));
		newTape = tape;
	}
	else {
		newTape = builder.createTape();
		if(this->expressionA->isConstant())
			builder.addInstruction(std::make_unique<WriteNumberInstruction>(newTape, 0, this->expressionA->buildConstant(builder)));
		else {
			std::tie(tape, range) = this->expressionA->buildTape(builder);
			builder.addInstruction(std::make_unique<CopyInstruction>(tape, newTape, range.index0, range.index0 + 1, 0));
		};
	};

	if(this->expressionB->isConstant())
		builder.addInstruction(std::make_unique<WriteNumberInstruction>(newTape, 1, this->expressionB->buildConstant(builder)));
	else {
		std::tie(tape, range) = this->expressionB->buildTape(builder);
		builder.addInstruction(std::make_unique<CopyInstruction>(tape, newTape, range.index0, range.index0 + 1, 1));
	};

	builder.addInstruction(std::make_unique<CallInstruction>(newTape, this->getMachine()));

	return Expression::Result::create<Expression::Type::TAPE_RANGE>(newTape, Expression::TapeRange(0, 1));
};

std::optional<Expression::TapeRange> BinaryExpression::getArrayAccesRange() const {
	switch(type) {
		case BinaryExpression::Type::PLUS:
		case BinaryExpression::Type::MINUS:
		case BinaryExpression::Type::MUL:
		case BinaryExpression::Type::DIV:
		case BinaryExpression::Type::MOD:
		case BinaryExpression::Type::MIN:
		case BinaryExpression::Type::MAX:
			return Expression::TapeRange(0, 1);

		case BinaryExpression::Type::EQ:
		case BinaryExpression::Type::NE:
		case BinaryExpression::Type::LTE:
		case BinaryExpression::Type::LT:
		case BinaryExpression::Type::GTE:
		case BinaryExpression::Type::GT:
		case BinaryExpression::Type::AND:
		case BinaryExpression::Type::OR:
		case BinaryExpression::Type::XOR:
			return {};

		default:
			std::unreachable();
	};
};

bool BinaryExpression::isTapeTemporary() const {
	return (this->getType()==Expression::Type::TAPE_RANGE);
};
