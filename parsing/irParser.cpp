#include "./irParser.hpp"
#include <utility>
#include <iterator>
#include <string>
#include <iostream>
#include <sstream>
#include "../IO/irParseError.hpp"
#include "../instructions/decompressInstruction.hpp"
#include "../instructions/compressInstruction.hpp"
#include "../instructions/clearInstruction.hpp"
#include "../instructions/writeNumberInstruction.hpp"
#include "../instructions/copyInstruction.hpp"
#include "../instructions/callInstruction.hpp"
#include "../instructions/jumpInstruction.hpp"
#include "../instructions/compareInstruction.hpp"
#include "../instructions/compareTapeLengthInstruction.hpp"

/*!
 * The constructor of IrParser.
 * \param text The view of the source. It must remain valid as long as the IrParser is used.
 * \param warningIt The destination where to append warnings, if necessary.
 */
IrParser::IrParser(std::wstring_view text, const std::back_insert_iterator<std::vector<std::unique_ptr<Warning>>> &warningIt): text(text), it(text.begin()), warningIt(warningIt) {};
//TODO implement some warnings

/*!
 * Parser a single line.
 * \param line The line to parse, without trailing whitespace or commments.
 * \throw IrParseError If the line is invalid (wrong format, TAPES pseudoinstruction if the number of tapes already set, unknown instruction, invalid arguments).
 */
void IrParser::parseLine(std::wstring_view line) {
	size_t parenthesisPos, tapesCount;
	std::wstring_view instructionName, instructionArguments;
	std::wistringstream iss;

	if(line.empty())
		return;

	if(line.starts_with(L"TAPES ")) {
		if(this->tapesCount)
			throw IrParseError(IrParseError::Type::NUMBER_OF_TAPES_ALREADY_SET, { line, this->lineNumber, this->text });

		iss.str(std::wstring(line.substr(6)));

		if(!(iss >> tapesCount) || !iss.eof())
			throw IrParseError(IrParseError::Type::WRONG_LINE_FORMAT, { line, this->lineNumber, this->text });

		this->tapesCount = tapesCount;

		return;
	};

	if(!this->tapesCount)
		this->tapesCount = 1;

	parenthesisPos = line.find('(');

	if(!line.ends_with(')') || parenthesisPos==line.npos || parenthesisPos==0)
		throw IrParseError(IrParseError::Type::WRONG_LINE_FORMAT, { line, this->lineNumber, this->text });

	//TODO do something with leading whitespace

	instructionName = line.substr(0, parenthesisPos);
	instructionArguments = line.substr(parenthesisPos + 1, line.size() - parenthesisPos - 2);

	this->instructions.push_back(this->resolveInstrucion(instructionName, { instructionArguments, *this->tapesCount, this->labels, { instructionArguments, this->lineNumber, this->text } }));
};

/*!
 * Resolve an instruction with its arguments.
 * \param instructionName The name of the instruction.
 * \param arguments An instance of IrArguments representing the arguments of the instruction.
 * \return The parsed instruction.
 * \throw IrParseError If the instruction is invalid (unknown instruction, invalid arguments).
 */
std::unique_ptr<Instruction> IrParser::resolveInstrucion(std::wstring_view instructionName, IrArguments arguments) {
	if(instructionName==L"decompress")
		return std::make_unique<DecompressInstruction>(arguments);

	if(instructionName==L"compress")
		return std::make_unique<CompressInstruction>(arguments);

	if(instructionName==L"clear")
		return std::make_unique<ClearInstruction>(arguments);

	if(instructionName==L"writeNumber")
		return std::make_unique<WriteNumberInstruction>(arguments);

	if(instructionName==L"copy")
		return std::make_unique<CopyInstruction>(arguments);

	if(instructionName==L"call")
		return std::make_unique<CallInstruction>(arguments);

	if(instructionName==L"jump")
		return std::make_unique<JumpInstruction>(arguments);

	if(instructionName==L"compare")
		return std::make_unique<CompareInstruction>(arguments);

	if(instructionName==L"compareTapeLength")
		return std::make_unique<CompareTapeLengthInstruction>(arguments);

	throw IrParseError(IrParseError::Type::UNKNOWN_INSTRUCTION, { instructionName, this->lineNumber, this->text });
};

/*!
 * Parse the source.
 * \return An instance of InstructionCollection with the parsed IR.
 * \throw IrParseError If any line is invalid (wrong format, TAPES pseudoinstruction if the number of tapes already set, unknown instruction, invalid arguments).
 */
InstructionCollection IrParser::parse() {
	size_t pos;
	std::wstring_view line;
	std::wstring_view::const_iterator lineStartIt;

	for(lineStartIt = this->it; lineStartIt!=this->text.end(); lineStartIt = std::next(this->it)) {
		for(this->it = lineStartIt; this->it!=this->text.end() && (*this->it)!='\n'; this->it++);
		line = std::wstring_view(lineStartIt, this->it);
		pos = line.find('#');
		if(pos!=line.npos)
			line.remove_suffix(line.size() - pos);
		pos = line.find_last_not_of(L" \t");
		if(pos==line.npos)
			line.remove_suffix(line.size());
		else
			line.remove_suffix(line.size() - pos - 1);

		this->parseLine(line);
		this->lineNumber++;
	};

	if(!this->tapesCount)
		this->tapesCount = 1;

	return { std::move(this->instructions), std::vector<TapeReference>(*this->tapesCount, TapeReference()), this->labels.size() };
};
