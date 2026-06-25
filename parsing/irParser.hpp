#pragma once
#include "../common.hpp"
#include "../building/instructionCollection.hpp"
#include "../IO/warning.hpp"

class IrParser {
	private:
		size_t lineNumber = 1;
		std::wstring::const_iterator it, endIt;
		std::back_insert_iterator<std::vector<std::unique_ptr<Warning>>> warningIt;
		InstructionCollection collection;

		void parseLine(std::wstring_view line);

	public:
		IrParser(const std::wstring &text, const std::back_insert_iterator<std::vector<std::unique_ptr<Warning>>> &warningIt);
		InstructionCollection parse();
};
