#include "./irParser.hpp"
#include "../IO/generalError.hpp"
#include "../IO/format.hpp"
#include "../instructions/decompressInstruction.hpp"
#include "../instructions/compressInstruction.hpp"
#include "../instructions/clearInstruction.hpp"
#include "../instructions/writeNumberInstruction.hpp"
#include "../instructions/copyInstruction.hpp"
#include "../instructions/callInstruction.hpp"
#include "../instructions/jumpInstruction.hpp"
#include "../instructions/compareInstruction.hpp"

std::unique_ptr<Instruction> IrParser::resolveInstrucion(std::wstring_view instructionName, IrArguments arguments) {
	if(instructionName==L"decompress")
		return std::make_unique<DecompressInstruction>(arguments);
	else if(instructionName==L"compress")
		return std::make_unique<CompressInstruction>(arguments);
	else if(instructionName==L"clear")
		return std::make_unique<ClearInstruction>(arguments);
	else if(instructionName==L"writeNumber")
		return std::make_unique<WriteNumberInstruction>(arguments);
	else if(instructionName==L"copy")
		return std::make_unique<CopyInstruction>(arguments);
	else if(instructionName==L"call")
		return std::make_unique<CallInstruction>(arguments);
	else if(instructionName==L"jump")
		return std::make_unique<JumpInstruction>(arguments);
	else if(instructionName==L"compare")
		return std::make_unique<CompareInstruction>(arguments);
	else
		throw GeneralError(L"Unknown instruction on line"+Format::blue(std::to_wstring(arguments.getLineNumber()))+L": "+Format::red(std::wstring(instructionName)));
};

void IrParser::parseLine(std::wstring_view line) {
	size_t parenthesisPos, tapesCount;
	std::wstring_view instructionName, instructionArguments;
	std::wistringstream iss;

	if(line.empty())
		return;

	if(line.starts_with(L"TAPES ")) {
		if(this->tapesCount)
			throw GeneralError(L"The number of tapes must be set at the beginning and only once.");

		iss.str(std::wstring(line.substr(6)));

		if(!(iss >> tapesCount) || !iss.eof())
			throw GeneralError(L"Invalid line "+Format::blue(std::to_wstring(this->lineNumber))+L": "+Format::red(std::wstring(line)));

		this->tapesCount = tapesCount;

		return;
	};

	if(!this->tapesCount)
		this->tapesCount = 1;

	parenthesisPos = line.find('(');

	if(!line.ends_with(')') || parenthesisPos==line.npos || parenthesisPos==0)
		throw GeneralError(L"Invalid line "+Format::blue(std::to_wstring(this->lineNumber))+L": "+Format::red(std::wstring(line)));

	instructionName = line.substr(0, parenthesisPos);
	instructionArguments = line.substr(parenthesisPos + 1, line.size() - parenthesisPos - 2);

	//TODO replace lineNumber with Location
	this->instructions.push_back(IrParser::resolveInstrucion(instructionName, { instructionArguments, *this->tapesCount, lineNumber, this->labels }));
};

IrParser::IrParser(const std::wstring &text, const std::back_insert_iterator<std::vector<std::unique_ptr<Warning>>> &warningIt): it(text.begin()), endIt(text.end()), warningIt(warningIt) {};

InstructionCollection IrParser::parse() {
	size_t pos;
	std::wstring_view line;
	std::wstring::const_iterator lineStartIt;

	for(lineStartIt = this->it; lineStartIt!=this->endIt; lineStartIt = std::next(this->it)) {
		for(this->it = lineStartIt; this->it!=endIt && (*this->it)!='\n'; this->it++);
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
