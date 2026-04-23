#pragma once
#include "../common.hpp"

/*!
 * This class represents the tape of a Turing machine.
 * Apart from the content (0/1 symbols), it also stores the current position.
 */
class Tape {
	private:
		size_t position = 0;
		std::deque<bool> tape = { false };

	public:
		void init(std::deque<bool> content, size_t position = 0);
		void init(std::wstring contentString, size_t position = 0);
		void init(std::vector<size_t> numbers);
		bool readSymbol() const;
		void writeSymbol(bool symbol);
		void moveLeft();
		void moveRight();
		void clearBlanks();
		const std::deque<bool> &getContent() const;
		size_t getPosition() const;
		std::optional<std::vector<size_t>> getNumbers() const;
		void print(std::wostream &stream, bool isFormattingEnabled) const;

		friend std::wistream &operator >> (std::wistream &stream, Tape &tape);
};

std::wostream &operator << (std::wostream &stream, const Tape &tape);
