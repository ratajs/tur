#include "./jumpInstruction.hpp"
#include <initializer_list>

/*!
 * The constructor of JumpInstruction.
 * \param label The number of the label, as given by the InstructionBuilder.
 * \param type The type of the jump.
 */
JumpInstruction::JumpInstruction(size_t label, JumpInstruction::Type type): type(type), label(label) {};

/*!
 * An alternative constructor of JumpInstruction.
 * The arguments are extracted from an instance of IrArguments.
 * The jump type (GOTO or COMEFROM) and the label is expected in the arguments.
 * \param arguments The arguments of the instruction from the IR input.
 * \throw IrParseError If the arguments do not match the expected format.
 */
JumpInstruction::JumpInstruction(IrArguments &arguments): type((arguments.readString({ L"GOTO", L"COMEFROM" })==0) ? JumpInstruction::Type::GO_TO : JumpInstruction::Type::COME_FROM), label(arguments.readLabel()) {
	arguments.end();
};

std::vector<size_t> JumpInstruction::listUsedTapes() const {
	return {};
};

std::optional<size_t> JumpInstruction::getComeFromOrigin() const {
	return ((this->type==JumpInstruction::Type::COME_FROM) ? std::optional(this->label) : std::nullopt);
};

bool JumpInstruction::isGoToInstruction() const {
	return (this->type==JumpInstruction::Type::GO_TO);
};

std::vector<size_t> JumpInstruction::getGoToDestinations() const {
	if(this->type==JumpInstruction::Type::GO_TO)
		return { this->label };
	else
		return {};
};

void JumpInstruction::build(SingleTapeMachineFactory &machineFactory, std::function<size_t (size_t)> getRealTape, std::function<const std::wstring &(size_t)> getState) const {
	switch(this->type) {
		case JumpInstruction::Type::GO_TO:
			machineFactory.addNTransition({}, getState(this->label));

			break;

		case JumpInstruction::Type::COME_FROM:
			machineFactory.addNTransition(getState(this->label), {});

			break;
	};
};

void JumpInstruction::build(MultiTapeMachineFactory &machineFactory, std::function<size_t (size_t)> getRealTape, std::function<const std::wstring &(size_t)> getState) const {
	switch(this->type) {
		case JumpInstruction::Type::GO_TO:
			machineFactory.addNTransition({}, getState(this->label));

			break;

		case JumpInstruction::Type::COME_FROM:
			machineFactory.addNTransition(getState(this->label), {});

			break;
	};
};

void JumpInstruction::print(std::wostream &stream, std::function<size_t (size_t)> getRealTape) const {
	stream << L"jump(";
	switch(this->type) {
		case JumpInstruction::Type::GO_TO:
			stream << L"GOTO ";

			break;

		case JumpInstruction::Type::COME_FROM:
			stream << L"COMEFROM ";

			break;
	};
	stream << this->label << L")" << std::endl;
};
