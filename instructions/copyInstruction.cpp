#include "./copyInstruction.hpp"
#include "./clearInstruction.hpp"
#include "../IO/unexpectedError.hpp"

/*!
 * The constructor for CopyInstruction.
 * \param source The source tape.
 * \param destination The destination tape. It must be a different tape from the source tape.
 * \param sourceIndex0 The index where the source start (inclusive).
 * \param sourceIndex1 The index where the source ends (exclusive; {} if the source is rightwise unbounded).
 * \param destinationIndex The index where the destination is ({} if the destination is the end of the destination tape).
 * \throw UnexpectedError If the source and destination tape is the same.
 */
CopyInstruction::CopyInstruction(size_t source, size_t destination, size_t sourceIndex0, std::optional<size_t> sourceIndex1, std::optional<size_t> destinationIndex):
	source(source), destination(destination), sourceIndex0(sourceIndex0), sourceIndex1(sourceIndex1), destinationIndex(destinationIndex) {
		if(source==destination)
			throw UnexpectedError(L"Invalid copy (source is destination).");
	};

std::vector<size_t> CopyInstruction::listUsedTapes() const {
	return { this->source, this->destination };
};

std::optional<std::list<std::unique_ptr<Instruction>>> CopyInstruction::tryToUnify() const {
	std::list<std::unique_ptr<Instruction>> instructions;

	if(this->destinationIndex!=0)
		return {};

	if(this->sourceIndex0!=0)
		instructions.push_back(std::make_unique<ClearInstruction>(this->source, 0, this->sourceIndex0));

	if(this->sourceIndex1)
		instructions.push_back(std::make_unique<ClearInstruction>(this->source, (*this->sourceIndex1) - this->sourceIndex0, std::nullopt));

	return { std::move(instructions) };
};

std::unique_ptr<Instruction> CopyInstruction::tryToMerge(const Instruction &otherInstruction) const {
	return otherInstruction.tryToMergeWithCopy(this->source, this->destination, this->sourceIndex0, this->sourceIndex1, this->destinationIndex);
};

std::unique_ptr<Instruction> CopyInstruction::tryToMergeWithCopy(size_t source, size_t destination, size_t sourceIndex0, std::optional<size_t> sourceIndex1, std::optional<size_t> destinationIndex) const {
	// Merge if this and the previous instruction copy together a continuous slice.
	if(this->source==source && this->destination==destination && sourceIndex1 && this->sourceIndex0==(*sourceIndex1) && (!this->destinationIndex || (destinationIndex && (*this->destinationIndex)==(*destinationIndex) + ((*sourceIndex1) - sourceIndex0))))
		return std::make_unique<CopyInstruction>(source, destination, sourceIndex0, this->sourceIndex1, destinationIndex);

	return {};
};

void CopyInstruction::build(MultiTapeMachineFactory &machineFactory, std::function<size_t (size_t)> getRealTape, std::function<const std::wstring &(size_t)> getState) const {
	if(this->destinationIndex) {
		if(this->sourceIndex1)
			machineFactory.copy(getRealTape(this->source), this->sourceIndex0, getRealTape(this->destination), *this->destinationIndex, (*this->sourceIndex1) - this->sourceIndex0);
		else
			machineFactory.copyAll(getRealTape(this->source), this->sourceIndex0, getRealTape(this->destination), *this->destinationIndex);
	}
	else {
		if(this->sourceIndex1)
			machineFactory.append(getRealTape(this->source), this->sourceIndex0, getRealTape(this->destination), (*this->sourceIndex1) - this->sourceIndex0);
		else
			machineFactory.appendAll(getRealTape(this->source), this->sourceIndex0, getRealTape(this->destination));
	};
};

void CopyInstruction::print(std::wostream &stream, std::function<size_t (size_t)> getRealTape) const {
	stream <<
		L"copy(" <<
		getRealTape(this->source) <<
		L"[" << this->sourceIndex0 << L":" << (this->sourceIndex1 ? std::to_wstring(*this->sourceIndex1) : L"") << L"], " <<
		getRealTape(this->destination) <<
		L"[" << (this->destinationIndex ? (std::to_wstring(*this->destinationIndex)+L":") : L"") << L"]" <<
		L")" << std::endl;
};
