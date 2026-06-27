#pragma once
#include "../common.hpp"
#include "../instructions/instruction.hpp"
#include "../building/instructionCollection.hpp"
#include "./irArguments.hpp"
#include "../IO/warning.hpp"

class IrParser {
	private:
		static std::unique_ptr<Instruction> resolveInstrucion(std::wstring_view instructionName, IrArguments arguments);

		size_t lineNumber = 1;
		std::optional<size_t> tapesCount;
		std::wstring::const_iterator it, endIt;
		std::map<size_t, size_t> labels; //TODO consider named labels
		std::back_insert_iterator<std::vector<std::unique_ptr<Warning>>> warningIt;
		std::list<std::unique_ptr<Instruction>> instructions;

		void parseLine(std::wstring_view line);

	public:
		IrParser(const std::wstring &text, const std::back_insert_iterator<std::vector<std::unique_ptr<Warning>>> &warningIt);
		InstructionCollection parse();
};
