#pragma once
#include "../common.hpp"
#include "../parsing/location.hpp"
#include "./variable.hpp"
#include "../machine/machine.hpp"
#include "./statements/statement.hpp"
#include "../building/instructionBuilder.hpp"
#include "../IO/warning.hpp"

/*!
 * This is the root of the AST.
 * It serves also as the symbol table.
 * It contains every variable and Turing machine together with all statements.
 */
class Program {
	private:
		std::set<std::wstring> usedVariableNames, usedMachineNames, unusedMachines;
		std::map<std::wstring, std::unique_ptr<Variable>> variables;
		std::map<std::wstring, std::unique_ptr<Machine>> machines;
		std::vector<std::unique_ptr<Statement>> statements;

	public:
		Program();
		Variable &findOrAddVariable(const std::wstring &name);
		const Machine &addMachine(const std::wstring &name, Machine machine, Location location);
		const Machine &findMachine(const std::wstring &name, Location location); // Mark as used
		void addStatements(std::vector<std::unique_ptr<Statement>> statements);
		void build(InstructionBuilder &builder);
		void checkForWarnings(std::back_insert_iterator<std::vector<std::unique_ptr<Warning>>> warningIt) const;
};
