#pragma once
#include "../common.hpp"

class Flags {
	public:
		enum class Flag: int {
			COMPILE = 1,
			INSTRUCTIONS = 2,
			ONLY_INSTRUCTIONS = 4,
			OPTIMIZE = 8,
			BASE = 16,
			RUN = 32,
			STEPS = 64,
			KEEP_BLANKS = 128,
			ENCODE = 256,
			DECODE = 512,
			NO_COLOR = 1024
		};

		int flags = 0;

	public:
		bool isFlagPresent(Flags::Flag flag) const;
		void addFlag(Flags::Flag flag);
};
