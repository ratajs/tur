#!/usr/bin/env bash

BINARY=$(dirname "$0")/tur

for INPUT_FILE in "$(dirname "$0")"/examples/simpleTests/*.in "$(dirname "$0")"/examples/*.in ; do
	MACHINE_FILE=$(sed -E 's/(.*)\.in/\1.tm/' <<< "$INPUT_FILE")
	OUTPUT_FILE=$(sed -E 's/(.*)\.in/\1.out/' <<< "$INPUT_FILE")
	LINES_COUNT=$(wc -l < "$INPUT_FILE")
	printf 'Machine: %s\n' "$MACHINE_FILE" >&2

	if [ \! -f "$INPUT_FILE" ] ; then
		echo "Input file isn’t a regular file." >&2
		exit 1
	fi

	if [ \! -r "$INPUT_FILE" ] ; then
		echo "Input file cannot be read." >&2
		exit 1
	fi

	if [ \! -e "$OUTPUT_FILE" ] ; then
		echo "Output file doesn’t exist." >&2
		exit 1
	fi

	if [ \! -f "$OUTPUT_FILE" ] ; then
		echo "Output file isn’t a regular file." >&2
		exit 1
	fi

	if [ \! -r "$OUTPUT_FILE" ] ; then
		echo "Output file cannot be read." >&2
		exit 1
	fi

	if [ \! -e "$MACHINE_FILE" ] ; then
		echo "Machine file doesn’t exist." >&2
		exit 1
	fi

	if [ \! -f "$MACHINE_FILE" ] ; then
		echo "Machine file isn’t a regular file." >&2
		exit 1
	fi

	if [ \! -r "$MACHINE_FILE" ] ; then
		echo "Machine file cannot be read." >&2
		exit 1
	fi

	if (($(wc -l < "$OUTPUT_FILE")!=LINES_COUNT)) ; then
		echo "Input and output lines don’t match." >&2
		exit 1
	fi

	for ((X = 1; X <= LINES_COUNT; X++)) ; do
		INPUT=$(head -n "$X" "$INPUT_FILE" | tail -n 1)
		REF_OUTPUT=$(head -n "$X" "$OUTPUT_FILE" | tail -n 1)

		OUTPUT=$("$BINARY" -rn --no-color "$MACHINE_FILE" 2> /dev/null <<< "$INPUT")

		if (($?!=0)) ; then
			printf 'Input: %s\nInvalid output.\n' "$INPUT" >&2
			exit 2
		fi

		if [ "$OUTPUT" != "$REF_OUTPUT" ] ; then
			printf 'Input: %s\nReference output: %s\nOutput: %s\n' "$INPUT" "$REF_OUTPUT" "$OUTPUT" >&2
			exit 2
		fi
	done

	echo "OK" >&2
done
