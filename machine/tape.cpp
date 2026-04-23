#include "./tape.hpp"
#include "../IO/unexpectedError.hpp"
#include "../IO/format.hpp"

/*!
 * Initialize this tape with a given content and position.
 * \param content The content of the tape.
 * \param position Postion on the tape.
 * \throw UnexpectedError If the position is out of range (greater or equal to the length of the content).
 */
void Tape::init(std::deque<bool> content, size_t position) {
	if(content.empty())
		content.push_back(false);

	if(position >= content.size())
		throw UnexpectedError(L"Position is out of range.");

	this->tape = std::move(content);
	this->position = position;
};

/*!
 * Initialize this tape with a given content and position.
 * \param contentString A string representation of the content of the tape. It must only contain 0 and 1 characters.
 * \param position Postion on the tape.
 * \throw UnexpectedError If contentString contains other characters than 0 and 1.
 * \throw UnexpectedError If the position is out of range (greater or equal to the length of the content).
 */
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

/*!
 * Initialize the tape with numbers.
 * The numbers will be encoded (unary encoding, 0 is 1, 1 is 11 etc.).
 * The position will be at the beginning (the first 1 symbol if there is at least one number).
 * \param numbers The numbers to encode on the tape.
 */
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

/*!
 * Read a symbol from the tape, as a machine.
 * \return The symbol at the current position (false is 0, true is 1).
 */
bool Tape::readSymbol() const {
	return this->tape[this->position];
};

/*!
 * Write a symbol onto the tape, as a machine.
 * \param The symbol to write to the current position (false is 0, true is 1).
 */
void Tape::writeSymbol(bool symbol) {
	this->tape[this->position] = symbol;
};

/*!
 * Move left, as a machine.
 * The position is decremented, a 0 symbol is inserted if necessary.
 */
void Tape::moveLeft() {
	if(this->position==0)
		this->tape.push_front(false);
	else
		this->position--;
};

/*!
 * Move right, as a machine.
 * The position is incremented, a 0 symbol is inserted if necessary.
 */
void Tape::moveRight() {
	this->position++;
	if(this->position >= this->tape.size())
		this->tape.push_back(false);
};

/*!
 * Clear leading and trailing blank (0) symbols.
 * For a symbol to be cleared, it has to be both left to the leftmost 1 symbol and left to the current position. Or the same with right.
 */
void Tape::clearBlanks() {
	while(this->position > 0 && !this->tape.front()) {
		this->tape.pop_front();
		this->position--;
	};

	while(this->position < this->tape.size() - 1 && !this->tape.back())
		this->tape.pop_back();
};

/*!
 * \return The current tape content.
 */
const std::deque<bool> &Tape::getContent() const {
	return this->tape;
};

/*!
 * \return The current position.
 */
size_t Tape::getPosition() const {
	return this->position;
};

/*!
 * Try to decode numbers written on the tape using unary encoding (1 for 0, 11 for 1 etc.).
 * The numbers have to start at the current position and there cannot be any 1 symbol before the position.
 * An empty tape is valid, it means no numbers.
 * \return The vector of numbers. Or {} (std::nullopt) if the decoding fails.
 */
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

/*!
 * Print the content of the tape to a given output stream.
 * No newline character is added.
 * Characters for 0 and 1 are used.
 * \param stream The output stream.
 * \param isFormattingEnabled If true, the current position is highlighted using formating sequences.
 */
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

/*!
 * Try to read the tape content from a given input stream.
 * The initial position will be the first occurence of 1 (or the beginning if the tape is empty).
 * \param stream The input stream.
 * \param tape The destination (reference to the tape where the content will be written).
 */
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

/*!
 * Print the content of the tape to a given output stream.
 * No newline character is added.
 * Characters for 0 and 1 are used.
 * Formatting sequences are used to highlight the current position.
 * \param stream The output stream.
 */
std::wostream &operator << (std::wostream &stream, const Tape &tape) {
	tape.print(stream, true);

	return stream;
};
