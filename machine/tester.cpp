#include "./tester.hpp"
#include "./machineLibrary.hpp"
#include "../IO/unexpectedError.hpp"

std::vector<std::pair<std::wstring, std::vector<std::wstring>>> Tester::generate1TapeTestContents() {
	std::vector<std::pair<std::wstring, std::vector<std::wstring>>> testContents;

	std::ranges::for_each(Tester::TEST_CONTENTS,
		[&testContents](const std::wstring &content1) -> void {
			std::wstring content;
			std::wstring::const_iterator it1;

			if(content1.empty()) {
				testContents.push_back({ L"10", { L"" } });
				testContents.push_back({ L"1010", { L"" } });

				return;
			};

			for(it1 = content1.begin(); it1 < content1.end(); it1++) {
				content.push_back('1');
				content.push_back(*it1);
			};

			testContents.push_back({ std::move(content), { content1 } });
			testContents.push_back({ L"10"+testContents.back().first, { content1 } });
		}
	);

	return testContents;
};

std::vector<std::pair<std::wstring, std::vector<std::wstring>>> Tester::generate2TapesTestContents() {
	std::vector<std::pair<std::wstring, std::vector<std::wstring>>> testContents;

	std::ranges::for_each(Tester::TEST_CONTENTS,
		[&testContents](const std::wstring &content1) -> void {
			std::ranges::for_each(Tester::TEST_CONTENTS,
				[&testContents, &content1](const std::wstring &content2) -> void {
					std::wstring content;
					std::wstring::const_iterator it1, it2;

					for(it1 = content1.begin(), it2 = content2.begin(); it1 < content1.end() || it2 < content2.end(); it1++, it2++) {
						content.push_back('1');
						content.push_back((it1 < content1.end()) ? (*it1) : '0');
						content.push_back((it2 < content2.end()) ? (*it2) : '0');
					};

					if(content.empty()) {
						testContents.push_back({ L"100", { L"", L"" } });
						testContents.push_back({ L"100100", { L"", L"" } });

						return;
					};

					testContents.push_back({ std::move(content), { content1, content2 } });
					testContents.push_back({ L"100"+testContents.back().first, { content1, content2 } });
				}
			);
		}
	);

	return testContents;
};

std::vector<std::pair<std::wstring, std::vector<std::wstring>>> Tester::generate3TapesTestContents() {
	std::vector<std::pair<std::wstring, std::vector<std::wstring>>> testContents;

	std::ranges::for_each(Tester::TEST_CONTENTS,
		[&testContents](const std::wstring &content1) -> void {
			std::ranges::for_each(Tester::TEST_CONTENTS,
				[&testContents, &content1](const std::wstring &content2) -> void {
					std::ranges::for_each(Tester::TEST_CONTENTS,
						[&testContents, &content1, &content2](const std::wstring &content3) -> void {
							std::wstring content;
							std::wstring::const_iterator it1, it2, it3;

							for(it1 = content1.begin(), it2 = content2.begin(), it3 = content3.begin(); it1 < content1.end() || it2 < content2.end() || it3 < content3.end(); it1++, it2++, it3++) {
								content.push_back('1');
								content.push_back((it1 < content1.end()) ? (*it1) : '0');
								content.push_back((it2 < content2.end()) ? (*it2) : '0');
								content.push_back((it3 < content3.end()) ? (*it3) : '0');
							};

							if(content.empty()) {
								testContents.push_back({ L"1000", { L"", L"", L"" } });
								testContents.push_back({ L"10001000", { L"", L"", L"" } });

								return;
							};

							testContents.push_back({ std::move(content), { content1, content2, content3 } });
							testContents.push_back({ L"1000"+testContents.back().first, { content1, content2, content3 } });
						}
					);
				}
			);
		}
	);

	return testContents;
};

size_t Tester::getNumbersCount(const std::wstring_view &tapeContent) {
	if(tapeContent.empty())
		return 0;

	return (std::ranges::count(tapeContent, '0') + 1);
};

std::vector<std::wstring_view> Tester::explode(std::wstring_view view) {
	size_t zeroPos;
	std::vector<std::wstring_view> numbers;

	while(!view.empty()) {
		zeroPos = view.find_first_of('0');
		if(zeroPos==view.npos) {
			numbers.emplace_back(view);
			view = { view.end(), view.end() };
		}
		else {
			numbers.emplace_back(view.begin(), view.begin() + zeroPos);
			view = { view.begin() + zeroPos + 1, view.end() };
		};
	};

	return numbers;
};

std::vector<size_t> Tester::decode(std::wstring_view view) {
	std::vector<size_t> numbers;

	std::ranges::transform(Tester::explode(view), std::back_inserter(numbers),
		[](const std::wstring_view &number) -> size_t {
			return (number.length() - 1);
		}
	);

	return numbers;
};

std::wstring Tester::encode(std::vector<size_t> numbers) {
	std::wstring tapeContent;

	if(numbers.empty())
		return tapeContent;

	std::ranges::for_each(numbers,
		[&tapeContent](size_t number) -> void {
			size_t x;

			for(x = 0; x <= number; x++)
				tapeContent.push_back('1');
			tapeContent.push_back('0');
		}
	);

	tapeContent.pop_back();

	return tapeContent;
};

void Tester::forEveryPrefix(std::wstring_view view, std::function<void (size_t, const std::wstring_view&)> function) {
	size_t numbersCount, x;

	numbersCount = Tester::getNumbersCount(view);

	for(x = numbersCount; x >= 0; x--) {
		if(x < numbersCount)
			view = view.substr(0, (x==0) ? 0 : view.find_last_of('0'));
		function(x, view);

		if(x==0)
			break;
	};
};

void Tester::forEverySuffix(std::wstring_view view, std::function<void (size_t, const std::wstring_view&)> function) {
	size_t numbersCount, x;

	numbersCount = Tester::getNumbersCount(view);

	for(x = 0; x <= numbersCount; x++) {
		if(x > 0)
			view = view.substr((x==numbersCount) ? view.size() : (view.find_first_of('0') + 1));
		function(x, view);
	};
};

template<MachineFactory_MachineFactory Factory>
	Factory Tester::prepareMachineForComparison(Factory &&machineFactory, std::wstring &lessThanState, std::wstring &equalState, std::wstring &greaterThanState) {
		std::vector<std::wstring> states;

		states = machineFactory.createExtraStates(3);
		lessThanState = std::move(states[0]);
		equalState = std::move(states[1]);
		greaterThanState = std::move(states[2]);

		return machineFactory;
	};

bool Tester::assertValidity(const Tape &tape, const Tester::Fail &fail) {
	size_t firstOnePosition, x;

	firstOnePosition = (std::ranges::find(tape.getContent(), true) - tape.getContent().begin());

	if(tape.getContent()[tape.getPosition()]==false) {
		if(firstOnePosition!=tape.getContent().size()) {
			this->fails.push_back(fail);

			return false;
		};

		return true;
	};

	if(firstOnePosition!=tape.getPosition()) {
		this->fails.push_back(fail);

		return false;
	};

	for(x = (firstOnePosition + 2); x < tape.getContent().size(); x++) {
		if(tape.getContent()[x - 1]==false && tape.getContent()[x]==false)
			break;
	};

	for(x++; x < tape.getContent().size(); x++) {
		if(tape.getContent()[x]==true) {
			this->fails.push_back(fail);

			return false;
		};
	};

	return true;
};

bool Tester::assertValidity(const Tape &tape, size_t tapesCount, const Tester::Fail &fail) {
	size_t firstOnePosition, x, y;

	firstOnePosition = (std::ranges::find(tape.getContent(), true) - tape.getContent().begin());

	if(firstOnePosition!=tape.getPosition()) {
		this->fails.push_back(fail);

		return false;
	};

	tapesCount++;

	for(x = (firstOnePosition + tapesCount); x < tape.getContent().size(); x+= tapesCount) {
		if(tape.getContent()[x]==false)
			break;
	};

	for(x+= tapesCount; x < tape.getContent().size(); x+= tapesCount) {
		if(tape.getContent()[x]==true) {
			this->fails.push_back(fail);

			return false;
		};
	};

	for(x = 1; x <= tapesCount; x++) {
		for(y = (firstOnePosition + x); y < tape.getContent().size(); y+= tapesCount) {
			if(tape.getContent()[y]==true)
				break;
		};

		for(y+= (2 * tapesCount); y < tape.getContent().size(); y+= tapesCount) {
			if(tape.getContent()[y - tapesCount]==false && tape.getContent()[y]==false)
				break;

			if(tape.getContent()[y]==true && tape.getContent()[y - x]==false)
				return false;
		};

		for(y+= tapesCount; y < tape.getContent().size(); y+= tapesCount) {
			if(tape.getContent()[y]==true) {
				this->fails.push_back(fail);

				return false;
			};
		};
	};

	return true;
};

bool Tester::assertContent(const Tape &tape, const std::vector<bool> &content, const Tester::Fail &fail) {
	size_t x;
	std::vector<bool>::const_iterator it;

	for(x = tape.getPosition(), it = content.begin(); x < tape.getContent().size() && it < content.end(); x++, it++) {
		if(tape.getContent()[x]!=(*it)) {
			this->fails.push_back(fail);

			return false;
		};
	};

	if(it!=content.end() || (x < tape.getContent().size() && tape.getContent()[x]==true) || (x + 1 < tape.getContent().size() && tape.getContent()[x + 1]==true)) {
		this->fails.push_back(fail);

		return false;
	};

	return true;
};

bool Tester::assertContent(const Tape &tape, const std::wstring &contentString, const Tester::Fail &fail) {
	std::vector<bool> content;

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

	return this->assertContent(tape, content, fail);
};

bool Tester::assertContent(const Tape &tape, size_t tapesCount, size_t tapeNumber, const std::vector<bool> &content, const Tester::Fail &fail) {
	size_t x;
	std::vector<bool>::const_iterator it;

	tapesCount++;

	for(x = (tape.getPosition() + tapeNumber); x < tape.getContent().size(); x+= tapesCount) {
		if(tape.getContent()[x]==true)
			break;
	};

	for(it = content.begin(); x < tape.getContent().size() && it < content.end(); x+= tapesCount, it++) {
		if(tape.getContent()[x]!=(*it)) {
			this->fails.push_back(fail);

			return false;
		};
	};

	if(it!=content.end() || (x < tape.getContent().size() && tape.getContent()[x]==true) || (x + tapesCount < tape.getContent().size() && tape.getContent()[x + tapesCount]==true)) {
		this->fails.push_back(fail);

		return false;
	};

	return true;
};

bool Tester::assertContent(const Tape &tape, size_t tapesCount, size_t tapeNumber, const std::wstring &contentString, const Tester::Fail &fail) {
	std::vector<bool> content;

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

	return this->assertContent(tape, tapesCount, tapeNumber, content, fail);
};

bool Tester::assertState(const std::wstring &expectedState, const std::wstring &state, const Fail &fail) {
	if(expectedState!=state) {
		this->fails.push_back(fail);

		return false;
	};

	return true;
};

void Tester::testDecompressCompress() {
	std::ranges::for_each(Tester::TEST_CONTENTS,
		[this](const std::wstring &content) -> void {
			size_t x, y, z;
			Tape tape;

			for(x = 1; x < 8; x++) {
				for(y = 1; y <= x; y++) {
					tape.init(content);
					MultiTapeMachineFactory(x).decompress(y).extractMachine().run(tape, true);
					this->assertValidity(tape, x, { L"decompress, validity", content });
					for(z = 1; z <= x; z++) {
						if(z==y)
							this->assertContent(tape, x, z, content, { L"decompress, content (decompressed)", content });
						else
							this->assertContent(tape, x, z, L"", { L"decompress, content (other)", content });
					};
					MultiTapeMachineFactory(x).compress(y).extractMachine().run(tape, true);
					this->assertValidity(tape, { L"compress after decompress, validity", content });
					this->assertContent(tape, content, { L"compress after decompress, content", content });
				};
			};
		}
	);
};

void Tester::testCompress() {
	Tape tape;

	std::ranges::for_each(Tester::generate1TapeTestContents(),
		[this](const std::pair<std::wstring, std::vector<std::wstring>> &testContent) -> void {
			Tape tape;

			tape.init(testContent.first);
			MultiTapeMachineFactory(1).compress(1).extractMachine().run(tape, true);
			this->assertValidity(tape, { L"compress, 1 tape, tape 1, validity", testContent.first });
			this->assertContent(tape, testContent.second[0], { L"compress, 1 tape, tape 1, content", testContent.first });
		}
	);

	std::ranges::for_each(Tester::generate2TapesTestContents(),
		[this](const std::pair<std::wstring, std::vector<std::wstring>> &testContent) -> void {
			Tape tape;

			tape.init(testContent.first);
			MultiTapeMachineFactory(2).compress(1).extractMachine().run(tape, true);
			this->assertValidity(tape, { L"compress, 2 tapes, tape 1, validity", testContent.first });
			this->assertContent(tape, testContent.second[0], { L"compress, 2 tapes, tape 1, content", testContent.first });

			tape.init(testContent.first);
			MultiTapeMachineFactory(2).compress(2).extractMachine().run(tape, true);
			this->assertValidity(tape, { L"compress, 2 tapes, tape 2, validity", testContent.first });
			this->assertContent(tape, testContent.second[1], { L"compress, 2 tapes, tape 2, content", testContent.first });
		}
	);

	std::ranges::for_each(Tester::generate3TapesTestContents(),
		[this](const std::pair<std::wstring, std::vector<std::wstring>> &testContent) -> void {
			Tape tape;

			tape.init(testContent.first);
			MultiTapeMachineFactory(3).compress(1).extractMachine().run(tape, true);
			this->assertValidity(tape, { L"compress, 3 tapes, tape 1, validity", testContent.first });
			this->assertContent(tape, testContent.second[0], { L"compress, 3 tapes, tape 1, content", testContent.first });

			tape.init(testContent.first);
			MultiTapeMachineFactory(3).compress(2).extractMachine().run(tape, true);
			this->assertValidity(tape, { L"compress, 3 tapes, tape 2, validity", testContent.first });
			this->assertContent(tape, testContent.second[1], { L"compress, 3 tapes, tape 2, content", testContent.first });

			tape.init(testContent.first);
			MultiTapeMachineFactory(3).compress(3).extractMachine().run(tape, true);
			this->assertValidity(tape, { L"compress, 3 tapes, tape 3, validity", testContent.first });
			this->assertContent(tape, testContent.second[2], { L"compress, 3 tapes, tape 3, content", testContent.first });
		}
	);
};

void Tester::testClearBeginning() {
	std::ranges::for_each(Tester::TEST_CONTENTS,
		[this](const std::wstring &testContent) -> void {
			Tester::forEverySuffix(testContent,
				[this, &testContent](size_t offset, const std::wstring_view &suffix) -> void {
					Tape tape;

					if(offset==0)
						return;

					tape.init(testContent);
					SingleTapeMachineFactory().clearBeginning(offset).extractMachine().run(tape, true);
					this->assertValidity(tape, { L"clearBeginning, single tape, validity", testContent });
					this->assertContent(tape, std::wstring(suffix), { L"clearBeginning, single tape, content", testContent });
				}
			);
		}
	);

	std::ranges::for_each(Tester::generate1TapeTestContents(),
		[this](const std::pair<std::wstring, std::vector<std::wstring>> &testContent) -> void {
			Tester::forEverySuffix(testContent.second[0],
				[this, &testContent](size_t offset, const std::wstring_view &suffix) -> void {
					Tape tape;

					if(offset==0)
						return;

					tape.init(testContent.first);
					MultiTapeMachineFactory(1).clearBeginning(1, offset).extractMachine().run(tape, true);
					this->assertValidity(tape, 1, { L"clearBeginning, 1 tape, tape 1, validity", testContent.first });
					this->assertContent(tape, 1, 1, std::wstring(suffix), { L"clearBeginning, 1 tape, tape 1, content", testContent.first });
				}
			);
		}
	);

	std::ranges::for_each(Tester::generate2TapesTestContents(),
		[this](const std::pair<std::wstring, std::vector<std::wstring>> &testContent) -> void {
			Tester::forEverySuffix(testContent.second[0],
				[this, &testContent](size_t offset, const std::wstring_view &suffix) -> void {
					Tape tape;

					if(offset==0)
						return;

					tape.init(testContent.first);
					MultiTapeMachineFactory(2).clearBeginning(1, offset).extractMachine().run(tape, true);
					this->assertValidity(tape, 2, { L"clearBeginning, 2 tapes, tape 1, validity", testContent.first });
					this->assertContent(tape, 2, 1, std::wstring(suffix), { L"clearBeginning, 2 tapes, tape 1, content (changed tape)", testContent.first });
					this->assertContent(tape, 2, 2, testContent.second[1], { L"clearBeginning, 2 tapes, tape 1, content (unchanged tape)", testContent.first });
				}
			);

			Tester::forEverySuffix(testContent.second[1],
				[this, &testContent](size_t offset, const std::wstring_view &suffix) -> void {
					Tape tape;

					if(offset==0)
						return;

					tape.init(testContent.first);
					MultiTapeMachineFactory(2).clearBeginning(2, offset).extractMachine().run(tape, true);
					this->assertValidity(tape, 2, { L"clearBeginning, 2 tapes, tape 2, validity", testContent.first });
					this->assertContent(tape, 2, 1, testContent.second[0], { L"clearBeginning, 2 tapes, tape 2, content (unchanged tape)", testContent.first });
					this->assertContent(tape, 2, 2, std::wstring(suffix), { L"clearBeginning, 2 tapes, tape 2, content (changed tape)", testContent.first });
				}
			);
		}
	);

	std::ranges::for_each(Tester::generate3TapesTestContents(),
		[this](const std::pair<std::wstring, std::vector<std::wstring>> &testContent) -> void {
			Tester::forEverySuffix(testContent.second[0],
				[this, &testContent](size_t offset, const std::wstring_view &suffix) -> void {
					Tape tape;

					if(offset==0)
						return;

					tape.init(testContent.first);
					MultiTapeMachineFactory(3).clearBeginning(1, offset).extractMachine().run(tape, true);
					this->assertValidity(tape, 3, { L"clearBeginning, 3 tapes, tape 1, validity", testContent.first });
					this->assertContent(tape, 3, 1, std::wstring(suffix), { L"clearBeginning, 3 tapes, tape 1, content (changed tape)", testContent.first });
					this->assertContent(tape, 3, 2, testContent.second[1], { L"clearBeginning, 3 tapes, tape 1, content (unchanged tape)", testContent.first });
					this->assertContent(tape, 3, 3, testContent.second[2], { L"clearBeginning, 3 tapes, tape 1, content (unchanged tape)", testContent.first });
				}
			);

			Tester::forEverySuffix(testContent.second[1],
				[this, &testContent](size_t offset, const std::wstring_view &suffix) -> void {
					Tape tape;

					if(offset==0)
						return;

					tape.init(testContent.first);
					MultiTapeMachineFactory(3).clearBeginning(2, offset).extractMachine().run(tape, true);
					this->assertValidity(tape, 3, { L"clearBeginning, 3 tapes, tape 2, validity", testContent.first });
					this->assertContent(tape, 3, 1, testContent.second[0], { L"clearBeginning, 3 tapes, tape 2, content (unchanged tape)", testContent.first });
					this->assertContent(tape, 3, 2, std::wstring(suffix), { L"clearBeginning, 3 tapes, tape 2, content (changed tape)", testContent.first });
					this->assertContent(tape, 3, 3, testContent.second[2], { L"clearBeginning, 3 tapes, tape 2, content (unchanged tape)", testContent.first });
				}
			);

			Tester::forEverySuffix(testContent.second[2],
				[this, &testContent](size_t offset, const std::wstring_view &suffix) -> void {
					Tape tape;

					if(offset==0)
						return;

					tape.init(testContent.first);
					MultiTapeMachineFactory(3).clearBeginning(3, offset).extractMachine().run(tape, true);
					this->assertValidity(tape, 3, { L"clearBeginning, 3 tapes, tape 3, validity", testContent.first });
					this->assertContent(tape, 3, 1, testContent.second[0], { L"clearBeginning, 3 tapes, tape 3, content (unchanged tape)", testContent.first });
					this->assertContent(tape, 3, 2, testContent.second[1], { L"clearBeginning, 3 tapes, tape 3, content (unchanged tape)", testContent.first });
					this->assertContent(tape, 3, 3, std::wstring(suffix), { L"clearBeginning, 3 tapes, tape 3, content (changed tape)", testContent.first });
				}
			);
		}
	);
};

void Tester::testClearEnd() {
	std::ranges::for_each(Tester::TEST_CONTENTS,
		[this](const std::wstring &testContent) -> void {
			Tester::forEveryPrefix(testContent,
				[this, &testContent](size_t length, const std::wstring_view &prefix) -> void {
					Tape tape;

					tape.init(testContent);
					SingleTapeMachineFactory().clearEnd(length).extractMachine().run(tape, true);
					this->assertValidity(tape, { L"clearEnd, single tape, validity", testContent });
					this->assertContent(tape, std::wstring(prefix), { L"clearEnd, single tape, content", testContent });
				}
			);
		}
	);

	std::ranges::for_each(Tester::generate1TapeTestContents(),
		[this](const std::pair<std::wstring, std::vector<std::wstring>> &testContent) -> void {
			Tester::forEveryPrefix(testContent.second[0],
				[this, &testContent](size_t length, const std::wstring_view &prefix) -> void {
					Tape tape;

					tape.init(testContent.first);
					MultiTapeMachineFactory(1).clearEnd(1, length).extractMachine().run(tape, true);
					this->assertValidity(tape, 1, { L"clearEnd, 1 tape, tape 1, validity", testContent.first });
					this->assertContent(tape, 1, 1, std::wstring(prefix), { L"clearEnd, 1 tape, tape 1, content", testContent.first });
				}
			);
		}
	);

	std::ranges::for_each(Tester::generate2TapesTestContents(),
		[this](const std::pair<std::wstring, std::vector<std::wstring>> &testContent) -> void {
			Tester::forEveryPrefix(testContent.second[0],
				[this, &testContent](size_t length, const std::wstring_view &prefix) -> void {
					Tape tape;

					tape.init(testContent.first);
					MultiTapeMachineFactory(2).clearEnd(1, length).extractMachine().run(tape, true);
					this->assertValidity(tape, 2, { L"clearEnd, 2 tapes, tape 1, validity", testContent.first });
					this->assertContent(tape, 2, 1, std::wstring(prefix), { L"clearEnd, 2 tapes, tape 1, content (changed tape)", testContent.first });
					this->assertContent(tape, 2, 2, testContent.second[1], { L"clearEnd, 2 tapes, tape 1, content (unchanged tape)", testContent.first });
				}
			);

			Tester::forEveryPrefix(testContent.second[1],
				[this, &testContent](size_t length, const std::wstring_view &prefix) -> void {
					Tape tape;

					tape.init(testContent.first);
					MultiTapeMachineFactory(2).clearEnd(2, length).extractMachine().run(tape, true);
					this->assertValidity(tape, 2, { L"clearEnd, 2 tapes, tape 2, validity", testContent.first });
					this->assertContent(tape, 2, 1, testContent.second[0], { L"clearEnd, 2 tapes, tape 2, content (unchanged tape)", testContent.first });
					this->assertContent(tape, 2, 2, std::wstring(prefix), { L"clearEnd, 2 tapes, tape 2, content (changed tape)", testContent.first });

				}
			);
		}
	);

	std::ranges::for_each(Tester::generate3TapesTestContents(),
		[this](const std::pair<std::wstring, std::vector<std::wstring>> &testContent) -> void {
			Tester::forEveryPrefix(testContent.second[0],
				[this, &testContent](size_t length, const std::wstring_view &prefix) -> void {
					Tape tape;

					tape.init(testContent.first);
					MultiTapeMachineFactory(3).clearEnd(1, length).extractMachine().run(tape, true);
					this->assertValidity(tape, 3, { L"clearEnd, 3 tapes, tape 1, validity", testContent.first });
					this->assertContent(tape, 3, 1, std::wstring(prefix), { L"clearEnd, 3 tapes, tape 1, content (changed tape)", testContent.first });
					this->assertContent(tape, 3, 2, testContent.second[1], { L"clearEnd, 3 tapes, tape 1, content (unchanged tape)", testContent.first });
					this->assertContent(tape, 3, 3, testContent.second[2], { L"clearEnd, 3 tapes, tape 1, content (unchanged tape)", testContent.first });
				}
			);

			Tester::forEveryPrefix(testContent.second[1],
				[this, &testContent](size_t length, const std::wstring_view &prefix) -> void {
					Tape tape;

					tape.init(testContent.first);
					MultiTapeMachineFactory(3).clearEnd(2, length).extractMachine().run(tape, true);
					this->assertValidity(tape, 3, { L"clearEnd, 3 tapes, tape 2, validity", testContent.first });
					this->assertContent(tape, 3, 1, testContent.second[0], { L"clearEnd, 3 tapes, tape 2, content (unchanged tape)", testContent.first });
					this->assertContent(tape, 3, 2, std::wstring(prefix), { L"clearEnd, 3 tapes, tape 2, content (changed tape)", testContent.first });
					this->assertContent(tape, 3, 3, testContent.second[2], { L"clearEnd, 3 tapes, tape 2, content (unchanged tape)", testContent.first });
				}
			);

			Tester::forEveryPrefix(testContent.second[2],
				[this, &testContent](size_t length, const std::wstring_view &prefix) -> void {
					Tape tape;

					tape.init(testContent.first);
					MultiTapeMachineFactory(3).clearEnd(3, length).extractMachine().run(tape, true);
					this->assertValidity(tape, 3, { L"clearEnd, 3 tapes, tape 3, validity", testContent.first });
					this->assertContent(tape, 3, 1, testContent.second[0], { L"clearEnd, 3 tapes, tape 3, content (unchanged tape)", testContent.first });
					this->assertContent(tape, 3, 2, testContent.second[1], { L"clearEnd, 3 tapes, tape 3, content (unchanged tape)", testContent.first });
					this->assertContent(tape, 3, 3, std::wstring(prefix), { L"clearEnd, 3 tapes, tape 3, content (changed tape)", testContent.first });
				}
			);
		}
	);
};

void Tester::testWriteNumber() {
	std::ranges::for_each(Tester::TEST_CONTENTS,
		[this](const std::wstring &testContent) -> void {
			Tester::forEveryPrefix(testContent,
				[this, &testContent](size_t length, const std::wstring_view &prefix) -> void {
					size_t x;
					Tape tape;

					for(x = 0; x < 8; x++) {
						tape.init(testContent);
						SingleTapeMachineFactory().writeNumber(length, x).extractMachine().run(tape, true);
						this->assertValidity(tape, { L"writeNumber, single tape, validity", testContent });
						this->assertContent(tape, std::wstring(prefix)+(prefix.empty() ? L"" : L"0")+Tester::encode({ x }), { L"writeNumber, single tape, content", testContent });
					};
				}
			);
		}
	);

	std::ranges::for_each(Tester::generate1TapeTestContents(),
		[this](const std::pair<std::wstring, std::vector<std::wstring>> &testContent) -> void {
			Tester::forEveryPrefix(testContent.second[0],
				[this, &testContent](size_t length, const std::wstring_view &prefix) -> void {
					size_t x;
					Tape tape;

					for(x = 0; x < 8; x++) {
						tape.init(testContent.first);
						MultiTapeMachineFactory(1).writeNumber(1, length, x).extractMachine().run(tape, true);
						this->assertValidity(tape, 1, { L"writeNumber, 1 tape, tape 1, validity", testContent.first });
						this->assertContent(tape, 1, 1, std::wstring(prefix)+(prefix.empty() ? L"" : L"0")+Tester::encode({ x }), { L"writeNumber, 1 tape, tape 1, content", testContent.first });
					};
				}
			);
		}
	);

	std::ranges::for_each(Tester::generate2TapesTestContents(),
		[this](const std::pair<std::wstring, std::vector<std::wstring>> &testContent) -> void {
			size_t targetTape;

			for(targetTape = 1; targetTape <= 2; targetTape++) {
				Tester::forEveryPrefix(testContent.second[targetTape - 1],
					[this, &testContent, targetTape](size_t length, const std::wstring_view &prefix) -> void {
						size_t x, y;
						Tape tape;

						for(x = 0; x < 8; x++) {
							tape.init(testContent.first);
							MultiTapeMachineFactory(2).writeNumber(targetTape, length, x).extractMachine().run(tape, true);
							this->assertValidity(tape, 2, { L"writeNumber, 2 tapes, tape "+std::to_wstring(targetTape)+L", validity", testContent.first });
							for(y = 1; y <= 2; y++) {
								if(y==targetTape)
									this->assertContent(tape, 2, y, std::wstring(prefix)+(prefix.empty() ? L"" : L"0")+Tester::encode({ x }), { L"writeNumber, 2 tapes, tape "+std::to_wstring(targetTape)+L", target tape, content", testContent.first });
								else
									this->assertContent(tape, 2, y, testContent.second[y - 1], { L"writeNumber, 2 tapes, tape "+std::to_wstring(targetTape)+L", other tape, content", testContent.first });
							};
						};
					}
				);
			};
		}
	);

	std::ranges::for_each(Tester::generate3TapesTestContents(),
		[this](const std::pair<std::wstring, std::vector<std::wstring>> &testContent) -> void {
			size_t targetTape;

			for(targetTape = 1; targetTape <= 3; targetTape++) {
				Tester::forEveryPrefix(testContent.second[targetTape - 1],
					[this, &testContent, targetTape](size_t length, const std::wstring_view &prefix) -> void {
						size_t x, y;
						Tape tape;

						for(x = 0; x < 8; x++) {
							tape.init(testContent.first);
							MultiTapeMachineFactory(3).writeNumber(targetTape, length, x).extractMachine().run(tape, true);
							this->assertValidity(tape, 3, { L"writeNumber, 3 tapes, tape "+std::to_wstring(targetTape)+L", validity", testContent.first });
							for(y = 1; y <= 3; y++) {
								if(y==targetTape)
									this->assertContent(tape, 3, y, std::wstring(prefix)+(prefix.empty() ? L"" : L"0")+Tester::encode({ x }), { L"writeNumber, 3 tapes, tape "+std::to_wstring(targetTape)+L", target tape, content", testContent.first });
								else
									this->assertContent(tape, 3, y, testContent.second[y - 1], { L"writeNumber, 3 tapes, tape "+std::to_wstring(targetTape)+L", other tape, content", testContent.first });
							};
						};
					}
				);
			};
		}
	);
};

void Tester::testAppendNumber() {
	std::ranges::for_each(Tester::TEST_CONTENTS,
		[this](const std::wstring &testContent) -> void {
			size_t x;
			Tape tape;

			for(x = 0; x < 8; x++) {
				tape.init(testContent);
				SingleTapeMachineFactory().appendNumber(x).extractMachine().run(tape, true);
				this->assertValidity(tape, { L"appendNumber, single tape, validity", testContent });
				this->assertContent(tape, testContent+(testContent.empty() ? L"" : L"0")+Tester::encode({ x }), { L"appendNumber, single tape, content", testContent });
			};
		}
	);

	std::ranges::for_each(Tester::generate1TapeTestContents(),
		[this](const std::pair<std::wstring, std::vector<std::wstring>> &testContent) -> void {
			size_t x;
			Tape tape;

			for(x = 0; x < 8; x++) {
				tape.init(testContent.first);
				MultiTapeMachineFactory(1).appendNumber(1, x).extractMachine().run(tape, true);
				this->assertValidity(tape, 1, { L"appendNumber, 1 tape, tape 1, validity", testContent.first });
				this->assertContent(tape, 1, 1, testContent.second[0]+(testContent.second[0].empty() ? L"" : L"0")+Tester::encode({ x }), { L"appendNumber, 1 tape, tape 1, content", testContent.first });
			};
		}
	);

	std::ranges::for_each(Tester::generate2TapesTestContents(),
		[this](const std::pair<std::wstring, std::vector<std::wstring>> &testContent) -> void {
			size_t x, y, z;
			Tape tape;

			for(x = 1; x <= 2; x++) {
				for(y = 0; y < 8; y++) {
					tape.init(testContent.first);
					MultiTapeMachineFactory(2).appendNumber(x, y).extractMachine().run(tape, true);
					this->assertValidity(tape, 2, { L"appendNumber, 2 tapes, tape "+std::to_wstring(x)+L", validity", testContent.first });
					for(z = 1; z <= 2; z++) {
						if(z==x)
							this->assertContent(tape, 2, z, testContent.second[z - 1]+(testContent.second[z - 1].empty() ? L"" : L"0")+Tester::encode({ y }), { L"appendNumber, 2 tapes, tape "+std::to_wstring(x)+L", target tape, content", testContent.first });
						else
							this->assertContent(tape, 2, z, testContent.second[z - 1], { L"appendNumber, 2 tapes, tape "+std::to_wstring(x)+L", other tape, content", testContent.first });
					};
				};
			};
		}
	);

	std::ranges::for_each(Tester::generate3TapesTestContents(),
		[this](const std::pair<std::wstring, std::vector<std::wstring>> &testContent) -> void {
			size_t x, y, z;
			Tape tape;

			for(x = 1; x <= 3; x++) {
				for(y = 0; y < 8; y++) {
					tape.init(testContent.first);
					MultiTapeMachineFactory(3).appendNumber(x, y).extractMachine().run(tape, true);
					this->assertValidity(tape, 3, { L"appendNumber, 3 tapes, tape "+std::to_wstring(x)+L", validity", testContent.first });
					for(z = 1; z <= 3; z++) {
						if(z==x)
							this->assertContent(tape, 3, z, testContent.second[z - 1]+(testContent.second[z - 1].empty() ? L"" : L"0")+Tester::encode({ y }), { L"appendNumber, 3 tapes, tape "+std::to_wstring(x)+L", target tape, content", testContent.first });
						else
							this->assertContent(tape, 3, z, testContent.second[z - 1], { L"appendNumber, 3 tapes, tape "+std::to_wstring(x)+L", other tape, content", testContent.first });
					};
				};
			};
		}
	);
};

void Tester::testCopy() {
	std::ranges::for_each(Tester::generate2TapesTestContents(),
		[this](const std::pair<std::wstring, std::vector<std::wstring>> &testContent) -> void {
			Tester::forEverySuffix(testContent.second[0],
				[this, &testContent](size_t sourceOffset, const std::wstring_view &sourceSuffix) -> void {
					Tester::forEveryPrefix(testContent.second[1],
						[this, &testContent, sourceOffset, &sourceSuffix](size_t destinationLength, const std::wstring_view &destinationPrefix) -> void {
							Tester::forEveryPrefix(sourceSuffix,
								[this, &testContent, sourceOffset, destinationLength, &destinationPrefix](size_t sourceLength, const std::wstring_view &sourcePrefix) -> void {
									Tape tape;

									if(sourceLength==0)
										return;

									tape.init(testContent.first);
									MultiTapeMachineFactory(2).copy(1, sourceOffset, 2, destinationLength, sourceLength).extractMachine().run(tape, true);
									this->assertValidity(tape, 2, { L"copy, 2 tapes, 1["+std::to_wstring(sourceOffset)+L":"+std::to_wstring(sourceOffset + sourceLength)+L"] → 2["+std::to_wstring(destinationLength)+L"], validity", testContent.first });
									this->assertContent(tape, 2, 1, testContent.second[0], { L"copy, 2 tapes, 1["+std::to_wstring(sourceOffset)+L":"+std::to_wstring(sourceOffset + sourceLength)+L"] → 2["+std::to_wstring(destinationLength)+L"], content (source tape)", testContent.first });
									this->assertContent(tape, 2, 2, std::wstring(destinationPrefix)+(destinationPrefix.empty() ? L"" : L"0")+std::wstring(sourcePrefix), { L"copy, 2 tapes, 1["+std::to_wstring(sourceOffset)+L":"+std::to_wstring(sourceOffset + sourceLength)+L"] → 2["+std::to_wstring(destinationLength)+L"], content (target tape)", testContent.first });
								}
							);
						}
					);
				}
			);

			Tester::forEverySuffix(testContent.second[1],
				[this, &testContent](size_t sourceOffset, const std::wstring_view &sourceSuffix) -> void {
					Tester::forEveryPrefix(testContent.second[0],
						[this, &testContent, sourceOffset, &sourceSuffix](size_t destinationLength, const std::wstring_view &destinationPrefix) -> void {
							Tester::forEveryPrefix(sourceSuffix,
								[this, &testContent, sourceOffset, destinationLength, &destinationPrefix](size_t sourceLength, const std::wstring_view &sourcePrefix) -> void {
									Tape tape;

									if(sourceLength==0)
										return;

									tape.init(testContent.first);
									MultiTapeMachineFactory(2).copy(2, sourceOffset, 1, destinationLength, sourceLength).extractMachine().run(tape, true);
									this->assertValidity(tape, 2, { L"copy, 2 tapes, 2["+std::to_wstring(sourceOffset)+L":"+std::to_wstring(sourceOffset + sourceLength)+L"] → 1["+std::to_wstring(destinationLength)+L"], validity", testContent.first });
									this->assertContent(tape, 2, 1, std::wstring(destinationPrefix)+(destinationPrefix.empty() ? L"" : L"0")+std::wstring(sourcePrefix), { L"copy, 2 tapes, 2["+std::to_wstring(sourceOffset)+L":"+std::to_wstring(sourceOffset + sourceLength)+L"] → 1["+std::to_wstring(destinationLength)+L"], content (target tape)", testContent.first });
									this->assertContent(tape, 2, 2, testContent.second[1], { L"copy, 2 tapes, 2["+std::to_wstring(sourceOffset)+L":"+std::to_wstring(sourceOffset + sourceLength)+L"] → 1["+std::to_wstring(destinationLength)+L"], content (source tape)", testContent.first });
								}
							);
						}
					);
				}
			);
		}
	);

	std::ranges::for_each(Tester::generate3TapesTestContents(),
		[this](const std::pair<std::wstring, std::vector<std::wstring>> &testContent) -> void {
			Tester::forEverySuffix(testContent.second[0],
				[this, &testContent](size_t sourceOffset, const std::wstring_view &sourceSuffix) -> void {
					Tester::forEveryPrefix(testContent.second[1],
						[this, &testContent, sourceOffset, &sourceSuffix](size_t destinationLength, const std::wstring_view &destinationPrefix) -> void {
							Tester::forEveryPrefix(sourceSuffix,
								[this, &testContent, sourceOffset, destinationLength, &destinationPrefix](size_t sourceLength, const std::wstring_view &sourcePrefix) -> void {
									Tape tape;

									if(sourceLength==0)
										return;

									tape.init(testContent.first);
									MultiTapeMachineFactory(3).copy(1, sourceOffset, 2, destinationLength, sourceLength).extractMachine().run(tape, true);
									this->assertValidity(tape, 3, { L"copy, 3 tapes, 1["+std::to_wstring(sourceOffset)+L":"+std::to_wstring(sourceOffset + sourceLength)+L"] → 2["+std::to_wstring(destinationLength)+L"], validity", testContent.first });
									this->assertContent(tape, 3, 1, testContent.second[0], { L"copy, 3 tapes, 1["+std::to_wstring(sourceOffset)+L":"+std::to_wstring(sourceOffset + sourceLength)+L"] → 2["+std::to_wstring(destinationLength)+L"], content (source tape)", testContent.first });
									this->assertContent(tape, 3, 2, std::wstring(destinationPrefix)+(destinationPrefix.empty() ? L"" : L"0")+std::wstring(sourcePrefix), { L"copy, 3 tapes, 1["+std::to_wstring(sourceOffset)+L":"+std::to_wstring(sourceOffset + sourceLength)+L"] → 2["+std::to_wstring(destinationLength)+L"], content (target tape)", testContent.first });
									this->assertContent(tape, 3, 3, testContent.second[2], { L"copy, 3 tapes, 1["+std::to_wstring(sourceOffset)+L":"+std::to_wstring(sourceOffset + sourceLength)+L"] → 2["+std::to_wstring(destinationLength)+L"], content (other tape)", testContent.first });
								}
							);
						}
					);
				}
			);

			Tester::forEverySuffix(testContent.second[2],
				[this, &testContent](size_t sourceOffset, const std::wstring_view &sourceSuffix) -> void {
					Tester::forEveryPrefix(testContent.second[0],
						[this, &testContent, sourceOffset, &sourceSuffix](size_t destinationLength, const std::wstring_view &destinationPrefix) -> void {
							Tester::forEveryPrefix(sourceSuffix,
								[this, &testContent, sourceOffset, destinationLength, &destinationPrefix](size_t sourceLength, const std::wstring_view &sourcePrefix) -> void {
									Tape tape;

									if(sourceLength==0)
										return;

									tape.init(testContent.first);
									MultiTapeMachineFactory(3).copy(3, sourceOffset, 1, destinationLength, sourceLength).extractMachine().run(tape, true);
									this->assertValidity(tape, 3, { L"copy, 3 tapes, 3["+std::to_wstring(sourceOffset)+L":"+std::to_wstring(sourceOffset + sourceLength)+L"] → 1["+std::to_wstring(destinationLength)+L"], validity", testContent.first });
									this->assertContent(tape, 3, 1, std::wstring(destinationPrefix)+(destinationPrefix.empty() ? L"" : L"0")+std::wstring(sourcePrefix), { L"copy, 3 tapes, 3["+std::to_wstring(sourceOffset)+L":"+std::to_wstring(sourceOffset + sourceLength)+L"] → 1["+std::to_wstring(destinationLength)+L"], content (target tape)", testContent.first });
									this->assertContent(tape, 3, 2, testContent.second[1], { L"copy, 3 tapes, 3["+std::to_wstring(sourceOffset)+L":"+std::to_wstring(sourceOffset + sourceLength)+L"] → 1["+std::to_wstring(destinationLength)+L"], content (other tape)", testContent.first });
									this->assertContent(tape, 3, 3, testContent.second[2], { L"copy, 3 tapes, 3["+std::to_wstring(sourceOffset)+L":"+std::to_wstring(sourceOffset + sourceLength)+L"] → 1["+std::to_wstring(destinationLength)+L"], content (source tape)", testContent.first });
								}
							);
						}
					);
				}
			);
		}
	);
};

void Tester::testAppend() {
	std::ranges::for_each(Tester::generate2TapesTestContents(),
		[this](const std::pair<std::wstring, std::vector<std::wstring>> &testContent) -> void {
			Tester::forEverySuffix(testContent.second[0],
				[this, &testContent](size_t offset, const std::wstring_view &suffix) -> void {
					Tester::forEveryPrefix(suffix,
						[this, &testContent, offset](size_t length, const std::wstring_view &prefix) -> void {
							Tape tape;

							if(length==0)
								return;

							tape.init(testContent.first);
							MultiTapeMachineFactory(2).append(1, offset, 2, length).extractMachine().run(tape, true);
							this->assertValidity(tape, 2, { L"append, 2 tapes, 1["+std::to_wstring(offset)+L":"+std::to_wstring(offset + length)+L"] → 2[], validity", testContent.first });
							this->assertContent(tape, 2, 1, testContent.second[0], { L"append, 2 tapes, 1["+std::to_wstring(offset)+L":"+std::to_wstring(offset + length)+L"] → 2[], content (source tape)", testContent.first });
							this->assertContent(tape, 2, 2, std::wstring(testContent.second[1])+(testContent.second[1].empty() ? L"" : L"0")+std::wstring(prefix), { L"append, 2 tapes, 1["+std::to_wstring(offset)+L":"+std::to_wstring(offset + length)+L"] → 2[], content (target tape)", testContent.first });
						}
					);
				}
			);

			Tester::forEverySuffix(testContent.second[1],
				[this, &testContent](size_t offset, const std::wstring_view &suffix) -> void {
					Tester::forEveryPrefix(suffix,
						[this, &testContent, offset](size_t length, const std::wstring_view &prefix) -> void {
							Tape tape;

							if(length==0)
								return;

							tape.init(testContent.first);
							MultiTapeMachineFactory(2).append(2, offset, 1, length).extractMachine().run(tape, true);
							this->assertValidity(tape, 2, { L"append, 2 tapes, 2["+std::to_wstring(offset)+L":"+std::to_wstring(offset + length)+L"] → 1[], validity", testContent.first });
							this->assertContent(tape, 2, 1, std::wstring(testContent.second[0])+(testContent.second[0].empty() ? L"" : L"0")+std::wstring(prefix), { L"append, 2 tapes, 2["+std::to_wstring(offset)+L":"+std::to_wstring(offset + length)+L"] → 1[], content (target tape)", testContent.first });
							this->assertContent(tape, 2, 2, testContent.second[1], { L"append, 2 tapes, 2["+std::to_wstring(offset)+L":"+std::to_wstring(offset + length)+L"] → 1[], content (source tape)", testContent.first });
						}
					);
				}
			);
		}
	);

	std::ranges::for_each(Tester::generate3TapesTestContents(),
		[this](const std::pair<std::wstring, std::vector<std::wstring>> &testContent) -> void {
			Tester::forEverySuffix(testContent.second[0],
				[this, &testContent](size_t offset, const std::wstring_view &suffix) -> void {
					Tester::forEveryPrefix(suffix,
						[this, &testContent, offset](size_t length, const std::wstring_view &prefix) -> void {
							Tape tape;

							if(length==0)
								return;

							tape.init(testContent.first);
							MultiTapeMachineFactory(3).append(1, offset, 2, length).extractMachine().run(tape, true);
							this->assertValidity(tape, 3, { L"append, 3 tapes, 1["+std::to_wstring(offset)+L":"+std::to_wstring(offset + length)+L"] → 2[], validity", testContent.first });
							this->assertContent(tape, 3, 1, testContent.second[0], { L"append, 3 tapes, 1["+std::to_wstring(offset)+L":"+std::to_wstring(offset + length)+L"] → 2[], content (source tape)", testContent.first });
							this->assertContent(tape, 3, 2, std::wstring(testContent.second[1])+(testContent.second[1].empty() ? L"" : L"0")+std::wstring(prefix), { L"append, 3 tapes, 1["+std::to_wstring(offset)+L":"+std::to_wstring(offset + length)+L"] → 2[], content (target tape)", testContent.first });
							this->assertContent(tape, 3, 3, testContent.second[2], { L"append, 3 tapes, 1["+std::to_wstring(offset)+L":"+std::to_wstring(offset + length)+L"] → 2[], content (other tape)", testContent.first });
						}
					);
				}
			);

			Tester::forEverySuffix(testContent.second[2],
				[this, &testContent](size_t offset, const std::wstring_view &suffix) -> void {
					Tester::forEveryPrefix(suffix,
						[this, &testContent, offset](size_t length, const std::wstring_view &prefix) -> void {
							Tape tape;

							if(length==0)
								return;

							tape.init(testContent.first);
							MultiTapeMachineFactory(3).append(3, offset, 1, length).extractMachine().run(tape, true);
							this->assertValidity(tape, 3, { L"append, 3 tapes, 3["+std::to_wstring(offset)+L":"+std::to_wstring(offset + length)+L"] → 1[], validity", testContent.first });
							this->assertContent(tape, 3, 1, std::wstring(testContent.second[0])+(testContent.second[0].empty() ? L"" : L"0")+std::wstring(prefix), { L"append, 3 tapes, 3["+std::to_wstring(offset)+L":"+std::to_wstring(offset + length)+L"] → 1[], content (target tape)", testContent.first });
							this->assertContent(tape, 3, 2, testContent.second[1], { L"append, 3 tapes, 3["+std::to_wstring(offset)+L":"+std::to_wstring(offset + length)+L"] → 1[], content (other tape)", testContent.first });
							this->assertContent(tape, 3, 3, testContent.second[2], { L"append, 3 tapes, 3["+std::to_wstring(offset)+L":"+std::to_wstring(offset + length)+L"] → 1[], content (source tape)", testContent.first });
						}
					);
				}
			);
		}
	);
};

void Tester::testCopyAll() {
	std::ranges::for_each(Tester::generate2TapesTestContents(),
		[this](const std::pair<std::wstring, std::vector<std::wstring>> &testContent) -> void {
			Tester::forEverySuffix(testContent.second[0],
				[this, &testContent](size_t sourceOffset, const std::wstring_view &sourceSuffix) -> void {
					Tester::forEveryPrefix(testContent.second[1],
						[this, &testContent, sourceOffset, &sourceSuffix](size_t destinationLength, const std::wstring_view &destinationPrefix) -> void {
							Tape tape;

							tape.init(testContent.first);
							MultiTapeMachineFactory(2).copyAll(1, sourceOffset, 2, destinationLength).extractMachine().run(tape, true);
							this->assertValidity(tape, 2, { L"copyAll, 2 tapes, 1["+std::to_wstring(sourceOffset)+L":] → 2["+std::to_wstring(destinationLength)+L"], validity", testContent.first });
							this->assertContent(tape, 2, 1, testContent.second[0], { L"copyAll, 2 tapes, 1["+std::to_wstring(sourceOffset)+L":] → 2["+std::to_wstring(destinationLength)+L"], content (source tape)", testContent.first });
							this->assertContent(tape, 2, 2, std::wstring(destinationPrefix)+(destinationPrefix.empty() ? L"" : L"0")+std::wstring(sourceSuffix), { L"copyAll, 2 tapes, 1["+std::to_wstring(sourceOffset)+L":] → 2["+std::to_wstring(destinationLength)+L"], content (target tape)", testContent.first });
						}
					);
				}
			);

			Tester::forEverySuffix(testContent.second[1],
				[this, &testContent](size_t sourceOffset, const std::wstring_view &sourceSuffix) -> void {
					Tester::forEveryPrefix(testContent.second[0],
						[this, &testContent, sourceOffset, &sourceSuffix](size_t destinationLength, const std::wstring_view &destinationPrefix) -> void {
							Tape tape;

							tape.init(testContent.first);
							MultiTapeMachineFactory(2).copyAll(2, sourceOffset, 1, destinationLength).extractMachine().run(tape, true);
							this->assertValidity(tape, 2, { L"copyAll, 2 tapes, 2["+std::to_wstring(sourceOffset)+L":] → 1["+std::to_wstring(destinationLength)+L"], validity", testContent.first });
							this->assertContent(tape, 2, 1, std::wstring(destinationPrefix)+(destinationPrefix.empty() ? L"" : L"0")+std::wstring(sourceSuffix), { L"copyAll, 2 tapes, 2["+std::to_wstring(sourceOffset)+L":] → 1["+std::to_wstring(destinationLength)+L"], content (target tape)", testContent.first });
							this->assertContent(tape, 2, 2, testContent.second[1], { L"copyAll, 2 tapes, 2["+std::to_wstring(sourceOffset)+L":] → 1["+std::to_wstring(destinationLength)+L"], content (source tape)", testContent.first });
						}
					);
				}
			);
		}
	);

	std::ranges::for_each(Tester::generate3TapesTestContents(),
		[this](const std::pair<std::wstring, std::vector<std::wstring>> &testContent) -> void {
			Tester::forEverySuffix(testContent.second[0],
				[this, &testContent](size_t sourceOffset, const std::wstring_view &sourceSuffix) -> void {
					Tester::forEveryPrefix(testContent.second[1],
						[this, &testContent, sourceOffset, &sourceSuffix](size_t destinationLength, const std::wstring_view &destinationPrefix) -> void {
							Tape tape;

							tape.init(testContent.first);
							MultiTapeMachineFactory(3).copyAll(1, sourceOffset, 2, destinationLength).extractMachine().run(tape, true);
							this->assertValidity(tape, 3, { L"copyAll, 3 tapes, 1["+std::to_wstring(sourceOffset)+L":] → 2["+std::to_wstring(destinationLength)+L"], validity", testContent.first });
							this->assertContent(tape, 3, 1, testContent.second[0], { L"copyAll, 3 tapes, 1["+std::to_wstring(sourceOffset)+L":] → 2["+std::to_wstring(destinationLength)+L"], content (source tape)", testContent.first });
							this->assertContent(tape, 3, 2, std::wstring(destinationPrefix)+(destinationPrefix.empty() ? L"" : L"0")+std::wstring(sourceSuffix), { L"copyAll, 3 tapes, 1["+std::to_wstring(sourceOffset)+L":] → 2["+std::to_wstring(destinationLength)+L"], content (target tape)", testContent.first });
							this->assertContent(tape, 3, 3, testContent.second[2], { L"copyAll, 3 tapes, 1["+std::to_wstring(sourceOffset)+L":] → 2["+std::to_wstring(destinationLength)+L"], content (other tape)", testContent.first });
						}
					);
				}
			);

			Tester::forEverySuffix(testContent.second[2],
				[this, &testContent](size_t sourceOffset, const std::wstring_view &sourceSuffix) -> void {
					Tester::forEveryPrefix(testContent.second[0],
						[this, &testContent, sourceOffset, &sourceSuffix](size_t destinationLength, const std::wstring_view &destinationPrefix) -> void {
							Tape tape;

							tape.init(testContent.first);
							MultiTapeMachineFactory(3).copyAll(3, sourceOffset, 1, destinationLength).extractMachine().run(tape, true);
							this->assertValidity(tape, 3, { L"copyAll, 3 tapes, 3["+std::to_wstring(sourceOffset)+L":] → 1["+std::to_wstring(destinationLength)+L"], validity", testContent.first });
							this->assertContent(tape, 3, 1, std::wstring(destinationPrefix)+(destinationPrefix.empty() ? L"" : L"0")+std::wstring(sourceSuffix), { L"copyAll, 3 tapes, 3["+std::to_wstring(sourceOffset)+L":] → 1["+std::to_wstring(destinationLength)+L"], content (target tape)", testContent.first });
							this->assertContent(tape, 3, 2, testContent.second[1], { L"copyAll, 3 tapes, 3["+std::to_wstring(sourceOffset)+L":] → 1["+std::to_wstring(destinationLength)+L"], content (other tape)", testContent.first });
							this->assertContent(tape, 3, 3, testContent.second[2], { L"copyAll, 3 tapes, 3["+std::to_wstring(sourceOffset)+L":] → 1["+std::to_wstring(destinationLength)+L"], content (source tape)", testContent.first });
						}
					);
				}
			);
		}
	);
};

void Tester::testAppendAll() {
	std::ranges::for_each(Tester::generate2TapesTestContents(),
		[this](const std::pair<std::wstring, std::vector<std::wstring>> &testContent) -> void {
			Tester::forEverySuffix(testContent.second[0],
				[this, &testContent](size_t offset, const std::wstring_view &suffix) -> void {
					Tape tape;

					tape.init(testContent.first);
					MultiTapeMachineFactory(2).appendAll(1, offset, 2).extractMachine().run(tape, true);
					this->assertValidity(tape, 2, { L"appendAll, 2 tapes, 1["+std::to_wstring(offset)+L":] → 2[], validity", testContent.first });
					this->assertContent(tape, 2, 1, testContent.second[0], { L"appendAll, 2 tapes, 1["+std::to_wstring(offset)+L":] → 2[], content (source tape)", testContent.first });
					this->assertContent(tape, 2, 2, std::wstring(testContent.second[1])+(testContent.second[1].empty() ? L"" : L"0")+std::wstring(suffix), { L"appendAll, 2 tapes, 1["+std::to_wstring(offset)+L":] → 2[], content (target tape)", testContent.first });
				}
			);

			Tester::forEverySuffix(testContent.second[1],
				[this, &testContent](size_t offset, const std::wstring_view &suffix) -> void {
					Tape tape;

					tape.init(testContent.first);
					MultiTapeMachineFactory(2).appendAll(2, offset, 1).extractMachine().run(tape, true);
					this->assertValidity(tape, 2, { L"appendAll, 2 tapes, 2["+std::to_wstring(offset)+L":] → 1[], validity", testContent.first });
					this->assertContent(tape, 2, 1, std::wstring(testContent.second[0])+(testContent.second[0].empty() ? L"" : L"0")+std::wstring(suffix), { L"appendAll, 2 tapes, 2["+std::to_wstring(offset)+L":] → 1[], content (target tape)", testContent.first });
					this->assertContent(tape, 2, 2, testContent.second[1], { L"appendAll, 2 tapes, 2["+std::to_wstring(offset)+L":] → 1[], content (source tape)", testContent.first });
				}
			);
		}
	);

	std::ranges::for_each(Tester::generate3TapesTestContents(),
		[this](const std::pair<std::wstring, std::vector<std::wstring>> &testContent) -> void {
			Tester::forEverySuffix(testContent.second[0],
				[this, &testContent](size_t offset, const std::wstring_view &suffix) -> void {
					Tape tape;

					tape.init(testContent.first);
					MultiTapeMachineFactory(3).appendAll(1, offset, 2).extractMachine().run(tape, true);
					this->assertValidity(tape, 3, { L"appendAll, 3 tapes, 1["+std::to_wstring(offset)+L":] → 2[], validity", testContent.first });
					this->assertContent(tape, 3, 1, testContent.second[0], { L"appendAll, 3 tapes, 1["+std::to_wstring(offset)+L":] → 2[], content (source tape)", testContent.first });
					this->assertContent(tape, 3, 2, std::wstring(testContent.second[1])+(testContent.second[1].empty() ? L"" : L"0")+std::wstring(suffix), { L"appendAll, 3 tapes, 1["+std::to_wstring(offset)+L":] → 2[], content (target tape)", testContent.first });
					this->assertContent(tape, 3, 3, testContent.second[2], { L"appendAll, 3 tapes, 1["+std::to_wstring(offset)+L":] → 2[], content (other tape)", testContent.first });
				}
			);

			Tester::forEverySuffix(testContent.second[2],
				[this, &testContent](size_t offset, const std::wstring_view &suffix) -> void {
					Tape tape;

					tape.init(testContent.first);
					MultiTapeMachineFactory(3).appendAll(3, offset, 1).extractMachine().run(tape, true);
					this->assertValidity(tape, 3, { L"appendAll, 3 tapes, 3["+std::to_wstring(offset)+L":] → 1[], validity", testContent.first });
					this->assertContent(tape, 3, 1, std::wstring(testContent.second[0])+(testContent.second[0].empty() ? L"" : L"0")+std::wstring(suffix), { L"appendAll, 3 tapes, 3["+std::to_wstring(offset)+L":] → 1[], content (target tape)", testContent.first });
					this->assertContent(tape, 3, 2, testContent.second[1], { L"appendAll, 3 tapes, 3["+std::to_wstring(offset)+L":] → 1[], content (other tape)", testContent.first });
					this->assertContent(tape, 3, 3, testContent.second[2], { L"appendAll, 3 tapes, 3["+std::to_wstring(offset)+L":] → 1[], content (source tape)", testContent.first });
				}
			);
		}
	);
};

void Tester::testCompare() {
	std::ranges::for_each(Tester::TEST_CONTENTS,
		[this](const std::wstring &testContent) -> void {
			size_t x, y;
			std::wstring lessThanState, equalState, greaterThanState, state;
			std::vector<size_t> numbers;
			Tape tape;

			numbers = Tester::decode(testContent);

			for(x = 0; x < numbers.size(); x++) {
				for(y = 0; y < numbers.size(); y++) {
					tape.init(testContent);
					Tester::prepareMachineForComparison(SingleTapeMachineFactory(), lessThanState, equalState, greaterThanState).compare(x, y, lessThanState, equalState, greaterThanState).extractMachine().run(tape, true, state);
					this->assertValidity(tape, { L"compare, single tape, validity", testContent });
					this->assertContent(tape, testContent, { L"compare, 1 tape, content", testContent });
					this->assertState(state, (
						(numbers[x] < numbers[y]) ? lessThanState : (
							(numbers[x]==numbers[y]) ? equalState : greaterThanState
						)
					), { L"compare, 1 tape, state", testContent });
				};
			};
		}
	);

	std::ranges::for_each(Tester::generate1TapeTestContents(),
		[this](const std::pair<std::wstring, std::vector<std::wstring>> &testContent) -> void {
			size_t x, y;
			std::wstring lessThanState, equalState, greaterThanState, state;
			std::vector<size_t> numbers;
			Tape tape;

			numbers = Tester::decode(testContent.second[0]);

			for(x = 0; x < numbers.size(); x++) {
				for(y = 0; y < numbers.size(); y++) {
					tape.init(testContent.first);
					Tester::prepareMachineForComparison(MultiTapeMachineFactory(1), lessThanState, equalState, greaterThanState).compare(1, x, y, lessThanState, equalState, greaterThanState).extractMachine().run(tape, true, state);
					this->assertValidity(tape, 1, { L"compare, 1 tape, validity", testContent.first });
					this->assertContent(tape, 1, 1, testContent.second[0], { L"compare, 1 tape, content", testContent.first });
					this->assertState(state, (
						(numbers[x] < numbers[y]) ? lessThanState : (
							(numbers[x]==numbers[y]) ? equalState : greaterThanState
						)
					), { L"compare, 1 tape, state", testContent.first });
				};
			};
		}
	);

	std::ranges::for_each(Tester::generate2TapesTestContents(),
		[this](const std::pair<std::wstring, std::vector<std::wstring>> &testContent) -> void {
			size_t x, y, z, w;
			std::wstring lessThanState, equalState, greaterThanState, state;
			std::vector<std::vector<size_t>> numbers;
			Tape tape;

			std::ranges::transform(testContent.second, std::back_inserter(numbers), Tester::decode);

			for(x = 1; x <= 2; x++) {
				for(y = 0; y < numbers[x - 1].size(); y++) {
					for(z = 1; z <= 2; z++) {
						for(w = 0; w < numbers[z - 1].size(); w++) {
							tape.init(testContent.first);
							Tester::prepareMachineForComparison(MultiTapeMachineFactory(2), lessThanState, equalState, greaterThanState).compare(x, y, z, w, lessThanState, equalState, greaterThanState).extractMachine().run(tape, true, state);
							this->assertValidity(tape, 2, { L"compare, 2 tapes, validity", testContent.first });
							this->assertContent(tape, 2, 1, testContent.second[0], { L"compare, 2 tapes, content", testContent.first });
							this->assertContent(tape, 2, 2, testContent.second[1], { L"compare, 2 tapes, content", testContent.first });
							this->assertState(state, (
								(numbers[x - 1][y] < numbers[z - 1][w]) ? lessThanState : (
									(numbers[x - 1][y]==numbers[z - 1][w]) ? equalState : greaterThanState
								)
							), { L"compare, 2 tapes, state", testContent.first });
						};
					};
				};
			};
		}
	);

	std::ranges::for_each(Tester::generate3TapesTestContents(),
		[this](const std::pair<std::wstring, std::vector<std::wstring>> &testContent) -> void {
			size_t x, y;
			std::wstring lessThanState, equalState, greaterThanState, state;
			std::vector<std::vector<size_t>> numbers;
			Tape tape;

			std::ranges::transform(testContent.second, std::back_inserter(numbers), Tester::decode);

			for(x = 0; x < numbers[0].size(); x++) {
				for(y = 0; y < numbers[1].size(); y++) {
					tape.init(testContent.first);
					Tester::prepareMachineForComparison(MultiTapeMachineFactory(3), lessThanState, equalState, greaterThanState).compare(1, x, 2, y, lessThanState, equalState, greaterThanState).extractMachine().run(tape, true, state);
					this->assertValidity(tape, 3, { L"compare, 3 tapes, validity", testContent.first });
					this->assertContent(tape, 3, 1, testContent.second[0], { L"compare, 3 tapes, content", testContent.first });
					this->assertContent(tape, 3, 2, testContent.second[1], { L"compare, 3 tapes, content", testContent.first });
					this->assertContent(tape, 3, 3, testContent.second[2], { L"compare, 3 tapes, content", testContent.first });
					this->assertState(state, (
						(numbers[0][x] < numbers[1][y]) ? lessThanState : (
							(numbers[0][x]==numbers[1][y]) ? equalState : greaterThanState
						)
					), { L"compare, 3 tapes, state", testContent.first });
				};
			};

			for(x = 0; x < numbers[2].size(); x++) {
				for(y = 0; y < numbers[0].size(); y++) {
					tape.init(testContent.first);
					Tester::prepareMachineForComparison(MultiTapeMachineFactory(3), lessThanState, equalState, greaterThanState).compare(3, x, 1, y, lessThanState, equalState, greaterThanState).extractMachine().run(tape, true, state);
					this->assertValidity(tape, 3, { L"compare, 3 tapes, validity", testContent.first });
					this->assertContent(tape, 3, 1, testContent.second[0], { L"compare, 3 tapes, content", testContent.first });
					this->assertContent(tape, 3, 2, testContent.second[1], { L"compare, 3 tapes, content", testContent.first });
					this->assertContent(tape, 3, 3, testContent.second[2], { L"compare, 3 tapes, content", testContent.first });
					this->assertState(state, (
						(numbers[2][x] < numbers[0][y]) ? lessThanState : (
							(numbers[2][x]==numbers[0][y]) ? equalState : greaterThanState
						)
					), { L"compare, 3 tapes, state", testContent.first });
				};
			};
		}
	);
};

void Tester::testCompareWithConstant() {
	std::ranges::for_each(Tester::TEST_CONTENTS,
		[this](const std::wstring &testContent) -> void {
			size_t x, y;
			std::wstring lessThanState, equalState, greaterThanState, state;
			std::vector<size_t> numbers;
			Tape tape;

			numbers = Tester::decode(testContent);

			for(x = 0; x < numbers.size(); x++) {
				for(y = 0; y < 5; y++) {
					tape.init(testContent);
					Tester::prepareMachineForComparison(SingleTapeMachineFactory(), lessThanState, equalState, greaterThanState).compareWithConstant(x, y, lessThanState, equalState, greaterThanState).extractMachine().run(tape, true, state);
					this->assertValidity(tape, { L"compareWithConstant, single tape, validity", testContent });
					this->assertContent(tape, testContent, { L"compareWithConstant, single tape, content", testContent });
					this->assertState(state, (
						(numbers[x] < y) ? lessThanState : (
							(numbers[x]==y) ? equalState : greaterThanState
						)
					), { L"compareWithConstant, single tape, state", testContent });
				};
			};
		}
	);

	std::ranges::for_each(Tester::generate1TapeTestContents(),
		[this](const std::pair<std::wstring, std::vector<std::wstring>> &testContent) -> void {
			size_t x, y;
			std::wstring lessThanState, equalState, greaterThanState, state;
			std::vector<size_t> numbers;
			Tape tape;

			numbers = Tester::decode(testContent.second[0]);

			for(x = 0; x < numbers.size(); x++) {
				for(y = 0; y < 5; y++) {
					tape.init(testContent.first);
					Tester::prepareMachineForComparison(MultiTapeMachineFactory(1), lessThanState, equalState, greaterThanState).compareWithConstant(1, x, y, lessThanState, equalState, greaterThanState).extractMachine().run(tape, true, state);
					this->assertValidity(tape, 1, { L"compareWithConstant, 1 tape, validity", testContent.first });
					this->assertContent(tape, 1, 1, testContent.second[0], { L"compareWithConstant, 1 tape, content", testContent.first });
					this->assertState(state, (
						(numbers[x] < y) ? lessThanState : (
							(numbers[x]==y) ? equalState : greaterThanState
						)
					), { L"compareWithConstant, 1 tape, state", testContent.first });
				};
			};
		}
	);

	std::ranges::for_each(Tester::generate2TapesTestContents(),
		[this](const std::pair<std::wstring, std::vector<std::wstring>> &testContent) -> void {
			size_t x, y, z;
			std::wstring lessThanState, equalState, greaterThanState, state;
			std::vector<std::vector<size_t>> numbers;
			Tape tape;

			std::ranges::transform(testContent.second, std::back_inserter(numbers), Tester::decode);

			for(x = 1; x <= 2; x++) {
				for(y = 0; y < numbers[x - 1].size(); y++) {
					for(z = 0; z < 5; z++) {
						tape.init(testContent.first);
						Tester::prepareMachineForComparison(MultiTapeMachineFactory(2), lessThanState, equalState, greaterThanState).compareWithConstant(x, y, z, lessThanState, equalState, greaterThanState).extractMachine().run(tape, true, state);
						this->assertValidity(tape, 2, { L"compareWithConstant, 2 tapes, validity", testContent.first });
						this->assertContent(tape, 2, 1, testContent.second[0], { L"compareWithConstant, 2 tapes, content", testContent.first });
						this->assertContent(tape, 2, 2, testContent.second[1], { L"compareWithConstant, 2 tapes, content", testContent.first });
						this->assertState(state, (
							(numbers[x - 1][y] < z) ? lessThanState : (
								(numbers[x - 1][y]==z) ? equalState : greaterThanState
							)
						), { L"compareWithConstant, 2 tapes, state", testContent.first });
					};
				};
			};
		}
	);

	std::ranges::for_each(Tester::generate3TapesTestContents(),
		[this](const std::pair<std::wstring, std::vector<std::wstring>> &testContent) -> void {
			size_t x, y, z;
			std::wstring lessThanState, equalState, greaterThanState, state;
			std::vector<std::vector<size_t>> numbers;
			Tape tape;

			std::ranges::transform(testContent.second, std::back_inserter(numbers), Tester::decode);

			for(x = 1; x <= 3; x++) {
				for(y = 0; y < numbers[x - 1].size(); y++) {
					for(z = 0; z < 5; z++) {
						tape.init(testContent.first);
						Tester::prepareMachineForComparison(MultiTapeMachineFactory(3), lessThanState, equalState, greaterThanState).compareWithConstant(x, y, z, lessThanState, equalState, greaterThanState).extractMachine().run(tape, true, state);
						this->assertValidity(tape, 3, { L"compareWithConstant, 3 tapes, validity", testContent.first });
						this->assertContent(tape, 3, 1, testContent.second[0], { L"compareWithConstant, 3 tapes, content", testContent.first });
						this->assertContent(tape, 3, 2, testContent.second[1], { L"compareWithConstant, 3 tapes, content", testContent.first });
						this->assertContent(tape, 3, 3, testContent.second[2], { L"compareWithConstant, 3 tapes, content", testContent.first });
						this->assertState(state, (
							(numbers[x - 1][y] < z) ? lessThanState : (
								(numbers[x - 1][y]==z) ? equalState : greaterThanState
							)
						), { L"compareWithConstant, 3 tapes, state", testContent.first });
					};
				};
			};
		}
	);
};

void Tester::testCompareTapeLength() {
	std::ranges::for_each(Tester::TEST_CONTENTS,
		[this](const std::wstring &testContent) -> void {
			size_t x;
			std::wstring lessThanState, equalState, greaterThanState, state;
			std::vector<size_t> numbers;
			Tape tape;

			numbers = Tester::decode(testContent);

			for(x = 0; x < 5; x++) {
				tape.init(testContent);
				Tester::prepareMachineForComparison(SingleTapeMachineFactory(), lessThanState, equalState, greaterThanState).compareTapeLength(x, lessThanState, equalState, greaterThanState).extractMachine().run(tape, true, state);
				this->assertValidity(tape, { L"compareTapeLength, single tape, validity", testContent });
				this->assertContent(tape, testContent, { L"compareTapeLength, single tape, content", testContent });
				this->assertState(state, (
					(numbers.size() < x) ? lessThanState : (
						(numbers.size()==x) ? equalState : greaterThanState
					)
				), { L"compareTapeLength, single tape, state", testContent });
			};
		}
	);

	std::ranges::for_each(Tester::generate1TapeTestContents(),
		[this](const std::pair<std::wstring, std::vector<std::wstring>> &testContent) -> void {
			size_t x;
			std::wstring lessThanState, equalState, greaterThanState, state;
			std::vector<size_t> numbers;
			Tape tape;

			numbers = Tester::decode(testContent.second[0]);

			for(x = 0; x < 5; x++) {
				tape.init(testContent.first);
				Tester::prepareMachineForComparison(MultiTapeMachineFactory(1), lessThanState, equalState, greaterThanState).compareTapeLength(1, x, lessThanState, equalState, greaterThanState).extractMachine().run(tape, true, state);
				this->assertValidity(tape, 1, { L"compareTapeLength, 1 tape, validity", testContent.first });
				this->assertContent(tape, 1, 1, testContent.second[0], { L"compareTapeLength, 1 tape, content", testContent.first });
				this->assertState(state, (
					(numbers.size() < x) ? lessThanState : (
						(numbers.size()==x) ? equalState : greaterThanState
					)
				), { L"compareTapeLength, 1 tape, state", testContent.first });
			};
		}
	);

	std::ranges::for_each(Tester::generate2TapesTestContents(),
		[this](const std::pair<std::wstring, std::vector<std::wstring>> &testContent) -> void {
			size_t x, y;
			std::wstring lessThanState, equalState, greaterThanState, state;
			std::vector<std::vector<size_t>> numbers;
			Tape tape;

			std::ranges::transform(testContent.second, std::back_inserter(numbers), Tester::decode);

			for(x = 1; x <= 2; x++) {
				for(y = 0; y < 5; y++) {
					tape.init(testContent.first);
					Tester::prepareMachineForComparison(MultiTapeMachineFactory(2), lessThanState, equalState, greaterThanState).compareTapeLength(x, y, lessThanState, equalState, greaterThanState).extractMachine().run(tape, true, state);
					this->assertValidity(tape, 2, { L"compareTapeLength, 2 tapes, validity", testContent.first });
					this->assertContent(tape, 2, 1, testContent.second[0], { L"compareTapeLength, 2 tapes, content", testContent.first });
					this->assertContent(tape, 2, 2, testContent.second[1], { L"compareTapeLength, 2 tapes, content", testContent.first });
					this->assertState(state, (
						(numbers[x - 1].size() < y) ? lessThanState : (
							(numbers[x - 1].size()==y) ? equalState : greaterThanState
						)
					), { L"compareTapeLength, 2 tapes, state", testContent.first });
				};
			};
		}
	);

	std::ranges::for_each(Tester::generate3TapesTestContents(),
		[this](const std::pair<std::wstring, std::vector<std::wstring>> &testContent) -> void {
			size_t x, y;
			std::wstring lessThanState, equalState, greaterThanState, state;
			std::vector<std::vector<size_t>> numbers;
			Tape tape;

			std::ranges::transform(testContent.second, std::back_inserter(numbers), Tester::decode);

			for(x = 1; x <= 3; x++) {
				for(y = 0; y < 5; y++) {
					tape.init(testContent.first);
					Tester::prepareMachineForComparison(MultiTapeMachineFactory(3), lessThanState, equalState, greaterThanState).compareTapeLength(x, y, lessThanState, equalState, greaterThanState).extractMachine().run(tape, true, state);
					this->assertValidity(tape, 3, { L"compareTapeLength, 3 tapes, validity", testContent.first });
					this->assertContent(tape, 3, 1, testContent.second[0], { L"compareTapeLength, 3 tapes, content", testContent.first });
					this->assertContent(tape, 3, 2, testContent.second[1], { L"compareTapeLength, 3 tapes, content", testContent.first });
					this->assertContent(tape, 3, 3, testContent.second[2], { L"compareTapeLength, 3 tapes, content", testContent.first });
					this->assertState(state, (
						(numbers[x - 1].size() < y) ? lessThanState : (
							(numbers[x - 1].size()==y) ? equalState : greaterThanState
						)
					), { L"compareTapeLength, 3 tapes, state", testContent.first });
				};
			};
		}
	);
};

void Tester::testAdd() {
	size_t x, y;
	Tape tape;

	for(x = 0; x < 8; x++) {
		for(y = 0; y < 8; y++) {
			tape.init(Tester::encode({ x, y }));
			MachineLibrary::ADD.run(tape, true);
			this->assertValidity(tape, { L"ADD, single tape, validity", std::to_wstring(x)+L" + "+std::to_wstring(y) });
			this->assertContent(tape, Tester::encode({ x + y }), { L"ADD, single tape, content", std::to_wstring(x)+L" + "+std::to_wstring(y) });
		};
	};
};

void Tester::testSub() {
	size_t x, y;
	Tape tape;

	for(x = 0; x < 8; x++) {
		for(y = 0; y < 8; y++) {
			tape.init(Tester::encode({ x, y }));
			MachineLibrary::SUB.run(tape, true);
			this->assertValidity(tape, { L"SUB, single tape, validity", std::to_wstring(x)+L" − "+std::to_wstring(y) });
			this->assertContent(tape, Tester::encode({ (y >= x) ? 0 : (x - y) }), { L"SUB, single tape, content", std::to_wstring(x)+L" − "+std::to_wstring(y) });
		};
	};
};

void Tester::testMul() {
	size_t x, y;
	Tape tape;

	for(x = 0; x < 8; x++) {
		for(y = 0; y < 8; y++) {
			tape.init(Tester::encode({ x, y }));
			MachineLibrary::MUL.run(tape, true);
			this->assertValidity(tape, { L"MUL, single tape, validity", std::to_wstring(x)+L" ⋅ "+std::to_wstring(y) });
			this->assertContent(tape, Tester::encode({ x * y }), { L"MUL, single tape, content", std::to_wstring(x)+L" ⋅ "+std::to_wstring(y) });
		};
	};
};

void Tester::testDiv() {
	size_t x, y;
	Tape tape;

	for(x = 0; x < 8; x++) {
		for(y = 1; y < 8; y++) {
			tape.init(Tester::encode({ x, y }));
			MachineLibrary::DIV.run(tape, true);
			this->assertValidity(tape, { L"DIV, single tape, validity", std::to_wstring(x)+L" ∶ "+std::to_wstring(y) });
			this->assertContent(tape, Tester::encode({ x / y }), { L"DIV, single tape, content", std::to_wstring(x)+L" ∶ "+std::to_wstring(y) });
		};
	};
};

void Tester::testMod() {
	size_t x, y;
	Tape tape;

	for(x = 0; x < 8; x++) {
		for(y = 1; y < 8; y++) {
			tape.init(Tester::encode({ x, y }));
			MachineLibrary::MOD.run(tape, true);
			this->assertValidity(tape, { L"MOD, single tape, validity", std::to_wstring(x)+L" mod "+std::to_wstring(y) });
			this->assertContent(tape, Tester::encode({ x % y }), { L"MOD, single tape, content", std::to_wstring(x)+L" mod "+std::to_wstring(y) });
		};
	};
};

void Tester::testMin() {
	size_t x, y;
	Tape tape;

	for(x = 0; x < 8; x++) {
		for(y = 0; y < 8; y++) {
			tape.init(Tester::encode({ x, y }));
			MachineLibrary::MIN.run(tape, true);
			this->assertValidity(tape, { L"MIN, single tape, validity", L"min("+std::to_wstring(x)+L"; "+std::to_wstring(y)+L")" });
			this->assertContent(tape, Tester::encode({ std::min(x, y) }), { L"MIN, single tape, content", L"min("+std::to_wstring(x)+L"; "+std::to_wstring(y)+L")" });
		};
	};
};

void Tester::testMax() {
	size_t x, y;
	Tape tape;

	for(x = 0; x < 8; x++) {
		for(y = 0; y < 8; y++) {
			tape.init(Tester::encode({ x, y }));
			MachineLibrary::MAX.run(tape, true);
			this->assertValidity(tape, { L"MAX, single tape, validity", L"max("+std::to_wstring(x)+L"; "+std::to_wstring(y)+L")" });
			this->assertContent(tape, Tester::encode({ std::max(x, y) }), { L"MAX, single tape, content", L"max("+std::to_wstring(x)+L"; "+std::to_wstring(y)+L")" });
		};
	};
};

void Tester::testCount() {
	std::ranges::for_each(Tester::TEST_CONTENTS,
		[this](const std::wstring &testContent) -> void {
			std::wstring lessThanState, equalState, greaterThanState, state;
			std::vector<size_t> numbers;
			Tape tape;

			numbers = Tester::decode(testContent);

			tape.init(testContent);
			MachineLibrary::COUNT.run(tape, true);
			this->assertValidity(tape, { L"COUNT, single tape, validity", testContent });
			this->assertContent(tape, Tester::encode({ numbers.size() }), { L"MAX, single tape, content", testContent });
		}
	);
};

void Tester::runAllTests() {
	this->testDecompressCompress();
	this->testCompress();
	this->testClearBeginning();
	this->testClearEnd();
	this->testWriteNumber();
	this->testAppendNumber();
	this->testCopy();
	this->testAppend();
	this->testCopyAll();
	this->testAppendAll();
	this->testCompare();
	this->testCompareWithConstant();
	this->testCompareTapeLength();
	this->testAdd();
	this->testSub();
	this->testMul();
	this->testDiv();
	this->testMod();
	this->testMin();
	this->testMax();
	this->testCount();
};

const std::vector<Tester::Fail> &Tester::getFails() {
	return this->fails;
};
