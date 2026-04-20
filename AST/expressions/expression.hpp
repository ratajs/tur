#pragma once
#include "../../common.hpp"
#include "../../parsing/location.hpp"
#include "../variable.hpp"
#include "../../building/instructionBuilder.hpp"

/*!
 * This is the abstract class from which all expressions inherit.
 */
class Expression {
	public:
		/*!
		 * Every expression has to have one of these types.
		 * CONSTANT means that the expression returns a number known at compile time.
		 * TAPE_RANGE means that the expresion returns a tape number with a range on it.
		 */
		enum class Type {
			CONSTANT, TAPE_RANGE, CONDITION
		};

		/*!
		 * This structure holds the start index of the range (inclusive) and optionally, the end index of the range (exclusive; {} means that the range is unbounded rightwise).
		 */
		struct TapeRange {
			size_t index0;
			std::optional<size_t> index1;
			TapeRange();
			TapeRange(size_t index0, std::optional<size_t> index1);
		};

		/*!
		 * This union holds the result of an expresion.
		 */
		union Result {
			template<Expression::Type type> requires (type==Expression::Type::CONSTANT)
				static Expression::Result create(size_t);
			template<Expression::Type type> requires (type==Expression::Type::TAPE_RANGE)
				static Expression::Result create(size_t, Expression::TapeRange);
			template<Expression::Type type> requires (type==Expression::Type::CONDITION)
				static Expression::Result create(size_t, size_t);

			template<>
				Expression::Result create<Expression::Type::CONSTANT>(size_t value);
			template<>
				Expression::Result create<Expression::Type::TAPE_RANGE>(size_t tape, Expression::TapeRange range);
			template<>
				Expression::Result create<Expression::Type::CONDITION>(size_t trueLabel, size_t falseLabel);

			size_t value = 0, measuredTape;
			std::pair<size_t, Expression::TapeRange> tapeAndRange;
			std::pair<size_t, size_t> labels;
		};

		Location location;

		Expression(const Location &location);
		virtual ~Expression();
		bool isCondition() const;
		bool isConstant() const;
		size_t buildConstant(InstructionBuilder &builder) const;
		std::pair<size_t, Expression::TapeRange> buildTape(InstructionBuilder &builder) const;
		std::pair<size_t, size_t> buildCondition(InstructionBuilder &builder) const;
		std::optional<size_t> getArrayAccessLength() const; // 0 for no array access, {} for unlimited length

		/*!
		 * This virtual method should return the type of the expression.
		 * The type has to correspond to the variant which the build method will return.
		 * \return The type of the expression.
		 */
		virtual Expression::Type getType() const = 0;

		/*!
		 * This virtual method should generate the instructions needed to evaluate the expression.
		 * The variant has to correspond to the type returned by the getType method.
		 * \param builder The instance of InstructionBuilder used for generating the instructions.
		 * \return For CONSTANT type, the value of the constant.
		 * \return For TAPE_RANGE, the index of the tape (the index used by InstructionBuilder) and the range.
		 * \return For CONDITION, the true label and the false label (label numbers according to InstructionBuilder), the expression should create jumps to at least one of these two labels.
		 */
		virtual Expression::Result build(InstructionBuilder &builder) const = 0;

		/*!
		 * This virtual method informs about array access of this expression.
		 * \return The array access indices, if the expression is explicitly indexed, otherwise {}.
		 */
		virtual std::optional<Expression::TapeRange> getArrayAccesRange() const = 0;

		/*!
		 * If this virtual method is implemented and returns true, the tape it returns can be freely reused, it is not used anywhere else (unlike a variable band, which can be used anytime later).
		 * Returning true only makes sense if the expresion is of type TAPE_RANGE.
		 * The default implementation returns false.
		 * \return Whether the tape is temporary.
		 */
		virtual bool isTapeTemporary() const;

		/*!
		 * This virtual method says whether the expression is directly bound to a variable tape.
		 * The default implementation returns {}.
		 * \return The reference of the variable to which the tape returned by the expression belongs, or {} if it is bound to no variable.
		 */
		virtual std::optional<std::reference_wrapper<const Variable>> getVariable() const;

		/*!
		 * This virutal says whether the expression is an expresion which counts the length of a variable.
		 * The default implementation returns {}.
		 * \return The reference of the variable the length of which the expression computes, or {} if it isn’t a count expression.
		 */
		virtual std::optional<std::reference_wrapper<const Variable>> getMeasuredVariable() const;
};
