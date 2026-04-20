#pragma once
#include "../common.hpp"
#include "./tape.hpp"
#include "./machine.hpp"
#include "./singleTapeMachineFactory.hpp"
#include "./multiTapeMachineFactory.hpp"

class Tester {
	private:
		static constexpr std::array<const wchar_t*, 13> TEST_CONTENTS = { L"", L"1", L"11", L"111", L"101", L"1101", L"1011", L"11011", L"10101", L"110101", L"1011011", L"101110101", L"1010111011" };
		static std::vector<std::pair<std::wstring, std::vector<std::wstring>>> generate1TapeTestContents();
		static std::vector<std::pair<std::wstring, std::vector<std::wstring>>> generate2TapesTestContents();
		static std::vector<std::pair<std::wstring, std::vector<std::wstring>>> generate3TapesTestContents();
		static size_t getNumbersCount(const std::wstring_view &tapeContent);
		static std::vector<std::wstring_view> explode(std::wstring_view view);
		static std::vector<size_t> decode(std::wstring_view view);
		static std::wstring encode(std::vector<size_t> numbers);
		static void forEveryPrefix(std::wstring_view view, std::function<void (size_t, const std::wstring_view&)> function);
		static void forEverySuffix(std::wstring_view view, std::function<void (size_t, const std::wstring_view&)> function);
		template<MachineFactory_MachineFactory Factory>
			static Factory prepareMachineForComparison(Factory &&machineFactory, std::wstring &lessThanState, std::wstring &equalState, std::wstring &greaterThanState);

	public:
		struct Fail {
			std::wstring testName, input;
		};

	private:
		std::vector<Tester::Fail> fails;

		bool assertValidity(const Tape &tape, const Fail &fail);
		bool assertValidity(const Tape &tape, size_t tapesCount, const Fail &fail);
		bool assertContent(const Tape &tape, const std::vector<bool> &content, const Fail &fail);
		bool assertContent(const Tape &tape, const std::wstring &contentString, const Fail &fail);
		bool assertContent(const Tape &tape, size_t tapesCount, size_t tapeNumber, const std::vector<bool> &content, const Fail &fail);
		bool assertContent(const Tape &tape, size_t tapesCount, size_t tapeNumber, const std::wstring &contentString, const Fail &fail);
		bool assertState(const std::wstring &expectedState, const std::wstring &state, const Fail &fail);

	public:
		void testDecompressCompress();
		void testCompress();
		void testClearBeginning();
		void testClearEnd();
		void testWriteNumber();
		void testAppendNumber();
		void testCopy();
		void testAppend();
		void testCopyAll();
		void testAppendAll();
		void testCompare();
		void testCompareWithConstant();
		void testCompareTapeLength();
		void testAdd();
		void testSub();
		void testMul();
		void testDiv();
		void testMod();
		void testMin();
		void testMax();
		void testCount();
		void runAllTests();
		const std::vector<Tester::Fail> &getFails();
};
