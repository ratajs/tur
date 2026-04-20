#include "./tape.hpp"
#include "../IO/unexpectedError.hpp"
#include "../IO/format.hpp"

void Tape::init(std::deque<bool> content, size_t position) {
	if(content.empty())
		content.push_back(false);

	if(position >= content.size())
		throw UnexpectedError(L"Position is out of range.");

	this->tape = std::move(content);
	this->position = position;
};

void Tape::init(std::wstring contentString, size_t position) {
	std::deque<bool> content;

	std::ranges::transform(contentString, std::back_inserter(content),
		[](wchar_t character) -> bool {
			switch(character) {
				case '0':
					return false;

				case '1':
					return true;

				default:
					throw UnexpectedError(L"Invalid character.");
			};
		}
	);

	this->init(std::move(content), position);
};

void Tape::init(std::vector<size_t> numbers) {
	this->position = 0;

	if(numbers.empty()) {
		this->tape = { false };

		return;
	};

	this->tape.clear();
	std::ranges::for_each(numbers,
		[this](size_t number) -> void {
			std::ranges::fill_n(std::back_inserter(this->tape), number + 1, true);
			this->tape.push_back(false);
		}
	);
	this->tape.pop_back();
};

bool Tape::readSymbol() const {
	return this->tape[this->position];
};

void Tape::writeSymbol(bool symbol) {
	this->tape[this->position] = symbol;
};

void Tape::moveLeft() {
	if(this->position==0)
		this->tape.push_front(false);
	else
		this->position--;
};

void Tape::moveRight() {
	this->position++;
	if(this->position >= this->tape.size())
		this->tape.push_back(false);
};

void Tape::clearBlanks() {
	while(this->position > 0 && !this->tape.front()) {
		this->tape.pop_front();
		this->position--;
	};

	while(this->position < this->tape.size() - 1 && !this->tape.back())
		this->tape.pop_back();
};

const std::deque<bool> &Tape::getContent() const {
	return this->tape;
};

size_t Tape::getPosition() const {
	return this->position;
};

std::optional<std::vector<size_t>> Tape::getNumbers() const {
	std::vector<size_t> numbers;
	std::deque<bool>::const_iterator it;

	if(!this->tape[this->position])
		return (std::ranges::contains(this->tape, true) ? std::nullopt : std::optional<std::vector<size_t>>(numbers));

	if(std::ranges::contains(this->tape.begin(), this->tape.begin() + this->position, true))
		return {};

	numbers.push_back(0);

	for(it = (this->tape.begin() + this->position + 1); it!=this->tape.end(); it++) {
		if(*it)
			numbers.back()++;
		else if(std::next(it)!=this->tape.end() && (*std::next(it))) {
			it++;
			numbers.push_back(0);
		}
		else
			return (std::ranges::contains(it, this->tape.end(), true) ? std::nullopt : std::optional<std::vector<size_t>>(numbers));
	};

	return numbers;
};

void Tape::print(std::wostream &stream, bool isFormattingEnabled) const {
	size_t index = 0;

	std::ranges::for_each(this->getContent(),
		[this, &stream, &index, isFormattingEnabled](bool symbol) -> void {
			if(isFormattingEnabled && this->getPosition()==index)
				stream << Format::underscoredYellow({ symbol ? '1' : '0' });
			else
				stream << (symbol ? '1' : '0');
			index++;
		}
	);
};

std::wistream &operator >> (std::wistream &stream, Tape &tape) {
	wchar_t character;
	size_t position = 0;
	std::optional<size_t> firstOnePosition;

	tape.tape = { 0 };
	tape.position = 0;

	character = stream.peek();
	while(character=='0' || character=='1') {
		stream.get();
		tape.writeSymbol((character=='1') ? true : false);
		tape.moveRight();
		if(!firstOnePosition && character=='1')
			firstOnePosition = position;
		position++;
		character = stream.peek();
	};

	if(tape.tape.size() > 1)
		tape.tape.pop_back();

	tape.position = firstOnePosition.value_or(0);

	return stream;
};

std::wostream &operator << (std::wostream &stream, const Tape &tape) {
	tape.print(stream, true);

	return stream;
};
