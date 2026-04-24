#include "./program.hpp"
#include "../instructions/decompressInstruction.hpp"
#include "../instructions/compressInstruction.hpp"
#include "../instructions/jumpInstruction.hpp"
#include "../IO/unexpectedError.hpp"
#include "../IO/symbolError.hpp"
#include "../IO/generalWarning.hpp"
#include "../IO/format.hpp"

/*!
 * The constructor of Program.
 * It starts automatically with the special variables called input and output registered.
 */
Program::Program() {
	this->variables.emplace(L"input", std::make_unique<Variable>());
	this->variables.emplace(L"output", std::make_unique<Variable>());
};

/*!
 * Find a variable in the symbol table, create it if it does not exist.
 * The variable is marked as used.
 * \param name The name of the variable.
 * \return A reference to the variable.
 */
Variable &Program::findOrAddVariable(const std::wstring &name) {
	this->usedVariableNames.emplace(name);

	if(this->variables.contains(name))
		return (*this->variables.at(name));

	return (*this->variables.emplace(name, std::make_unique<Variable>()).first->second);
};

/*!
 * Store a Turing machine in the symbol table.
 * The machine is marked as unused.
 * \param name The name of the machine.
 * \param machine The machine itself.
 * \param location The location of the identifier (machine name) in code.
 * \throw SymbolError If a machine of this name is already registered.
 * \return A reference to the machine.
 */
const Machine &Program::addMachine(const std::wstring &name, Machine machine, Location location) {
	if(this->machines.contains(name))
		throw SymbolError(SymbolError::Type::DUPLICATE_MACHINE, location);

	this->usedMachineNames.emplace(name);
	this->unusedMachines.emplace(name);

	return (*this->machines.emplace(name, std::make_unique<Machine>(std::move(machine))).first->second);
};

/*!
 * Find a Turing machine in the symbol table by name, mark the machine as used.
 * \param name The name of the machine.
 * \param location The location of the identifier (machine name) in code.
 * \return A reference to the machine.
 */
const Machine &Program::findMachine(const std::wstring &name, Location location) {
	if(!this->machines.contains(name))
		throw SymbolError(SymbolError::Type::UNKNOWN_MACHINE, location);

	this->unusedMachines.erase(name);

	return (*this->machines.at(name));
};

/*!
 * Add statements to the program.
 * This should be called only once.
 * \throw UnexpectedError If the program already has some statements.
 */
void Program::addStatements(std::vector<std::unique_ptr<Statement>> statements) {
	if(!this->statements.empty())
		throw UnexpectedError(L"Program already has statements.");

	this->statements = std::move(statements);
};

/*!
 * Build the whole program using a given InstructionBuilder object.
 * The builder should be unused (no tapes, no labels, no instructions).
 * \param builder The builder to use.
 */
void Program::build(InstructionBuilder &builder) {
	size_t exitDestination;

	exitDestination = builder.createLabel();
	builder.setExitDestination(exitDestination);
	std::ranges::for_each(this->variables | std::views::values,
		[&builder](std::unique_ptr<Variable> &variable) -> void {
			variable->tape = builder.createTape();
		}
	);
	builder.addInstruction(std::make_unique<DecompressInstruction>(*this->variables.at(L"input")->tape));
	std::ranges::for_each(this->statements, [&builder](std::unique_ptr<Statement> &statement) -> void { statement->build(builder); });
	builder.addInstruction(std::make_unique<JumpInstruction>(exitDestination, JumpInstruction::Type::GO_TO));
	builder.addInstruction(std::make_unique<JumpInstruction>(exitDestination, JumpInstruction::Type::COME_FROM));
	builder.addInstruction(std::make_unique<CompressInstruction>(*this->variables.at(L"output")->tape));
};

/*!
 * Check following:
 * Program should have at least one statement.
 * Every machine and variable registered in the symbol table should be used.
 * No name should refer both to a machine and to a variable.
 * \param warningIt An output iterator where warnings are sent.
 */
void Program::checkForWarnings(std::back_insert_iterator<std::vector<std::unique_ptr<Warning>>> warningIt) const {
	std::vector<std::wstring> commonNames;

	if(this->statements.empty())
		warningIt = std::make_unique<GeneralWarning>(L"The program does not contain any statement.");

	std::ranges::set_intersection(this->usedVariableNames, this->usedMachineNames, std::back_inserter(commonNames));
	std::ranges::for_each(commonNames,
		[&warningIt](const std::wstring &name) -> void {
			warningIt = std::make_unique<GeneralWarning>(L"This name refers to a variable as well as to a machine: "+Format::blue(name));
		}
	);

	if(!this->usedVariableNames.contains(L"input"))
		warningIt = std::make_unique<GeneralWarning>(L"The "+Format::blue(L"input")+L" variable is not used.");
	if(!this->usedVariableNames.contains(L"output"))
		warningIt = std::make_unique<GeneralWarning>(L"The "+Format::blue(L"output")+L" variable is not used.");

	std::ranges::for_each(unusedMachines,
		[&warningIt](const std::wstring &name) -> void {
			warningIt = std::make_unique<GeneralWarning>(L"This machine was defined but not used: "+Format::blue(name));
		}
	);
};
