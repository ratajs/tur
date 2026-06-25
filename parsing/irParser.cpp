#include "./irParser.hpp"

void IrParser::parseLine(std::wstring_view line) {
	//TODO

	std::wcerr << L"Line " << this->lineNumber << L": " << line << std::endl;
};

IrParser::IrParser(const std::wstring &text, const std::back_insert_iterator<std::vector<std::unique_ptr<Warning>>> &warningIt): it(text.begin()), endIt(text.end()), warningIt(warningIt) {};

InstructionCollection IrParser::parse() {
	size_t commentSymbolPos;
	std::wstring_view line;
	std::wstring::const_iterator lineStartIt;

	for(lineStartIt = this->it; lineStartIt!=this->endIt; lineStartIt = std::next(this->it)) {
		for(this->it = lineStartIt; this->it!=endIt && (*this->it)!='\n'; this->it++);
		line = std::wstring_view(lineStartIt, this->it);
		commentSymbolPos = line.find('#');
		if(commentSymbolPos!=line.npos)
			line.remove_suffix(line.size() - commentSymbolPos);

		this->parseLine(line);
		this->lineNumber++;
	};

	return std::move(this->collection);
};
