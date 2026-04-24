BIN	= tur
MAN	= tur.1

PREFIX = ${HOME}/.local
BINDIR = ${PREFIX}/bin/
MANDIR = ${PREFIX}/man/man1/

CXX	= clang++
CXXFLAGS = -std=c++23 -Wall -Wno-deprecated-declarations -pedantic -O3

OBJS = \
	IO/flags.o \
	IO/input.o \
	IO/format.o \
	IO/error.o \
	IO/generalError.o \
	IO/unexpectedError.o \
	IO/errorWithLocation.o \
	IO/lexerError.o \
	IO/parseError.o \
	IO/typeError.o \
	IO/symbolError.o \
	IO/warning.o \
	IO/generalWarning.o \
	IO/includeResolver.o \
	machine/tape.o \
	machine/machine.o \
	parsing/location.o \
	parsing/token.o \
	parsing/lexer.o \
	parsing/parser.o \
	AST/program.o \
	AST/variable.o \
	AST/expressions/expression.o \
	AST/expressions/numberExpression.o \
	AST/expressions/variableExpression.o \
	AST/expressions/arrayAccessExpression.o \
	AST/expressions/unaryExpression.o \
	AST/expressions/binaryExpression.o \
	AST/expressions/countExpression.o \
	AST/expressions/callExpression.o \
	AST/statements/statement.o \
	AST/statements/copyStatement.o \
	AST/statements/implodeStatement.o \
	AST/statements/explodeStatement.o \
	AST/statements/callStatement.o \
	AST/statements/ifStatement.o \
	AST/statements/whileStatement.o \
	AST/statements/loopStatement.o \
	AST/statements/breakStatement.o \
	AST/statements/continueStatement.o \
	AST/statements/exitStatement.o \
	AST/sourceBundle.o \
	AST/destinationBundle.o \
	building/instructionBuilder.o \
	building/instructionCollection.o \
	building/tapeReference.o \
	building/machineBuilder.o \
	instructions/instruction.o \
	instructions/decompressInstruction.o \
	instructions/compressInstruction.o \
	instructions/writeNumberInstruction.o \
	instructions/copyInstruction.o \
	instructions/clearInstruction.o \
	instructions/callInstruction.o \
	instructions/jumpInstruction.o \
	instructions/compareInstruction.o \
	instructions/compareTapeLengthInstruction.o \
	machine/stateGenerator.o \
	machine/machineFactory.o \
	machine/singleTapeMachineFactory.o \
	machine/multiTapeMachineFactory.o

TOBJS	=	\
	IO/format.o \
	IO/error.o \
	IO/generalError.o \
	IO/unexpectedError.o \
	IO/warning.o \
	IO/generalWarning.o \
	machine/tape.o \
	machine/machine.o \
	machine/stateGenerator.o \
	machine/machineFactory.o \
	machine/singleTapeMachineFactory.o \
	machine/multiTapeMachineFactory.o \
	machine/tester.o

EXAMPLES = \
	examples/gcd.tm \
	examples/primes.tm \
	examples/utm.tm \
	examples/ww.tm \
	examples/id.tm \
	examples/count.tm \
	examples/fib.tm \
	examples/bubbleSort.tm \
	examples/primeDecomposition.tm \
	examples/BFS.tm \
	examples/simpleTests/simpleAssignment.tm \
	examples/simpleTests/boundedArrayAccessAssignment.tm \
	examples/simpleTests/unboundedArrayAccessAssignment.tm \
	examples/simpleTests/assignmentToArrayAccess.tm \
	examples/simpleTests/arithmeticOperations.tm \
	examples/simpleTests/implosion.tm \
	examples/simpleTests/explosion.tm \
	examples/simpleTests/explosionWithSourceVariable.tm \
	examples/simpleTests/branchingWithoutElse.tm \
	examples/simpleTests/branchingWithElse.tm \
	examples/simpleTests/while.tm \
	examples/simpleTests/break.tm \
	examples/simpleTests/continue.tm \
	examples/simpleTests/loop.tm

all: ${BIN}

${BIN}: ${OBJS} tur.cpp
	${CXX} ${CXXFLAGS} -o ${BIN} ${OBJS} tur.cpp
	${CXX} ${CXXFLAGS} -MM tur.cpp -MT ${BIN} > ./${BIN}.d

test: ${BIN} ${EXAMPLES} test.sh
	./test.sh

algotest: ${TOBJS} algotest.cpp
	${CXX} ${CXXFLAGS} -o ./algotest ${TOBJS} algotest.cpp && ./algotest
	${CXX} ${CXXFLAGS} -MM algotest.cpp > ./algotest.d

doc: ${OBJS:.o=.hpp} ${OBJS:.o=.cpp} tur.cpp algotest.cpp common.hpp machine/machineLibrary.hpp Doxyfile
	doxygen ./Doxyfile

lint: ${MAN}
	mandoc -Tlint ${MAN}

.SUFFIXES: .cpp .o .tur .tm .cpp .d

include ${OBJS:.o=.d}
include ${TOBJS:.o=.d}
include tur.d
include algotest.d

.cpp.o:
	${CXX} ${CXXFLAGS} -o $@ -c $<
	${CXX} ${CXXFLAGS} -MM $< -MT $@ > ${@:.o=.d}

.cpp.d:
	${CXX} ${CXXFLAGS} -MM $< -MT $@ > ${@:.o=.d}

${EXAMPLES}: %.tm: %.tur ${BIN}
	./${BIN} -cO $< $@

install: ${BIN} ${MAN}
	install -d -m 0755 ${BINDIR} && install -m 0755 ${BIN} ${BINDIR}
	install -d -m 0755 ${MANDIR} && install -m 0755 ${MAN} ${MANDIR}

uninstall:
	rm -rf ${BINDIR}/${BIN}
	rm -rf ${MANDIR}/${MAN}

clean:
	rm -rf ${OBJS} ${TOBJS} ${EXAMPLES} ${OBJS:.o=.d} ${TOBJS:.o=.d}
	rm -rf ${BIN} ./doc/ algotest ${BIN}.d algotest.d vgcore.* *.dSYM *.core *~

.PHONY: install uninstall
.PHONY: lint algotest
.PHONY: clean
