#include "./common.hpp"
#include "./machine/tester.hpp"
#include "./IO/error.hpp"

int main() {
	Tester tester;

	std::locale::global(std::locale("C.UTF-8"));

	try {
		tester.runAllTests();
		std::ranges::for_each(tester.getFails(),
			[](const Tester::Fail &fail) -> void {
				std::wcerr << fail.testName << L": " << fail.input << std::endl;
			}
		);
	} catch(const Error &error) {
		std::wcerr << error << std::endl;

		std::locale::global(std::locale::classic());

		return 1;
	};

	std::locale::global(std::locale::classic());

	return 0;
};
