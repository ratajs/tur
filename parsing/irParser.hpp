#pragma once
#include <cstdlib>
#include <memory>
#include <optional>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <iterator>
#include <string_view>
#include "../instructions/instruction.hpp"
#include "../building/instructionCollection.hpp"
#include "./irArguments.hpp"
#include "../IO/warning.hpp"

/*!
 * This class is for parsing the IR input.
 * It is initialized with a view of the source.
 * The output is an InstructionCollection.
 */
class IrParser {
	private:
		bool isComeFromExpected = false, isReversed = false;
		size_t lineNumber = 1;
		std::optional<size_t> tapesCount;
		std::wstring_view text;
		std::wstring_view::const_iterator it;
		std::set<size_t> comeFromOrigins;
		std::map<size_t, bool> labelReversals;
		std::map<size_t, size_t> labels; //TODO consider named labels
		std::back_insert_iterator<std::vector<std::unique_ptr<Warning>>> warningIt;
		std::list<std::unique_ptr<Instruction>> instructions;

		void parseLine(std::wstring_view line);
		std::unique_ptr<Instruction> resolveInstrucion(std::wstring_view instructionName, IrArguments arguments);

	public:
		IrParser(std::wstring_view text, const std::back_insert_iterator<std::vector<std::unique_ptr<Warning>>> &warningIt);
		InstructionCollection parse();
};
