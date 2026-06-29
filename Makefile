#TODO
BIN	= tur
MAN	= tur.1

PREFIX = ${HOME}/.local
BINDIR = ${PREFIX}/bin/
MANDIR = ${PREFIX}/man/man1/

CXX	= clang++
CXXFLAGS = -std=c++23 -Wall -Wno-deprecated-declarations -pedantic -O3

OBJS = \
	IO/flags.o \
	IO/inputLanguage.o \
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
	IO/irParseError.o \
	IO/warning.o \
	IO/generalWarning.o \
	IO/includeResolver.o \
	machine/tape.o \
	machine/machine.o \
	parsing/location.o \
	parsing/token.o \
	parsing/lexer.o \
	parsing/parser.o \
	parsing/irParser.o \
	parsing/irArguments.o \
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
	AST/statements/assignStatement.o \
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
	machine/machineLibrary.o \
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
	machine/machineLibrary.o \
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
	examples/simpleTests/loop.tm \
	examples/simpleTests/callStatement.tm \
	examples/simpleTests/callExpression.tm

all: ${BIN}

${BIN}: ${OBJS} tur.cpp
	${CXX} ${CXXFLAGS} -o ${BIN} ${OBJS} tur.cpp
	${CXX} ${CXXFLAGS} -MM tur.cpp -MT $@ > ./${BIN}.d

test: ${BIN} ${EXAMPLES} ${EXAMPLES:.tm=.O.tm} test.sh
	./test.sh

algotest: ${TOBJS} algotest.cpp
	${CXX} ${CXXFLAGS} -o ./algotest ${TOBJS} algotest.cpp
	${CXX} ${CXXFLAGS} -MM algotest.cpp -MT $@ > ./algotest.d
	./algotest

doc: ${OBJS:.o=.hpp} ${OBJS:.o=.cpp} tur.cpp algotest.cpp machine/machineLibrary.hpp README.md Doxyfile
	doxygen ./Doxyfile

lint: ${MAN}
	mandoc -Tlint ${MAN}

.SUFFIXES: .cpp .o .tur .tm .cpp .d

.cpp.o:
	${CXX} ${CXXFLAGS} -o $@ -c $<
	${CXX} ${CXXFLAGS} -MM $< -MT $@ > ${@:.o=.d}

.cpp.d:
	${CXX} ${CXXFLAGS} -MM $< -MT ${@:.d=.o} > $@

.tur.tm:
	./${BIN} -c $< $@
	./${BIN} -cO -x .O.tm $< ${@:.tm=.O.tm}

install: ${BIN} ${MAN}
	install -d -m 0755 ${BINDIR} && install -m 0755 ${BIN} ${BINDIR}
	install -d -m 0755 ${MANDIR} && install -m 0755 ${MAN} ${MANDIR}

uninstall:
	rm -rf ${BINDIR}/${BIN}
	rm -rf ${MANDIR}/${MAN}

clean:
	rm -rf ${OBJS} ${TOBJS} ${EXAMPLES} ${EXAMPLES:.tm=.O.tm} ${OBJS:.o=.d} ${TOBJS:.o=.d}
	rm -rf ${BIN} ./doc/ algotest ${BIN}.d algotest.d vgcore.* *.dSYM *.core *~

.PHONY: install uninstall
.PHONY: lint algotest
.PHONY: test
.PHONY: clean

AST/destinationBundle.o: AST/destinationBundle.cpp \
	AST/./destinationBundle.hpp AST/././variable.hpp \
	AST/./../parsing/location.hpp AST/../IO/typeError.hpp \
	AST/../IO/./errorWithLocation.hpp AST/../IO/././error.hpp \
	AST/../IO/./../parsing/location.hpp AST/../IO/../parsing/location.hpp
AST/expressions/arrayAccessExpression.o: \
	AST/expressions/arrayAccessExpression.cpp \
	AST/expressions/./arrayAccessExpression.hpp \
	AST/expressions/././expression.hpp \
	AST/expressions/././../../parsing/location.hpp \
	AST/expressions/././../variable.hpp \
	AST/expressions/././../../building/instructionBuilder.hpp \
	AST/expressions/././../../building/./tapeReference.hpp \
	AST/expressions/././../../building/../instructions/instruction.hpp \
	AST/expressions/././../../building/../instructions/../machine/singleTapeMachineFactory.hpp \
	AST/expressions/././../../building/../instructions/../machine/./machineFactory.hpp \
	AST/expressions/././../../building/../instructions/../machine/././stateGenerator.hpp \
	AST/expressions/././../../building/../instructions/../machine/././machine.hpp \
	AST/expressions/././../../building/../instructions/../machine/./././tape.hpp \
	AST/expressions/././../../building/../instructions/../machine/./machine.hpp \
	AST/expressions/././../../building/../instructions/../machine/multiTapeMachineFactory.hpp \
	AST/expressions/././../../building/./instructionCollection.hpp \
	AST/expressions/././../../building/././tapeReference.hpp \
	AST/expressions/././../../building/./../instructions/instruction.hpp \
	AST/expressions/./../../parsing/location.hpp \
	AST/expressions/../../IO/typeError.hpp \
	AST/expressions/../../IO/./errorWithLocation.hpp \
	AST/expressions/../../IO/././error.hpp \
	AST/expressions/../../IO/./../parsing/location.hpp \
	AST/expressions/../../IO/../parsing/location.hpp
AST/expressions/binaryExpression.o: AST/expressions/binaryExpression.cpp \
	AST/expressions/./binaryExpression.hpp \
	AST/expressions/././expression.hpp \
	AST/expressions/././../../parsing/location.hpp \
	AST/expressions/././../variable.hpp \
	AST/expressions/././../../building/instructionBuilder.hpp \
	AST/expressions/././../../building/./tapeReference.hpp \
	AST/expressions/././../../building/../instructions/instruction.hpp \
	AST/expressions/././../../building/../instructions/../machine/singleTapeMachineFactory.hpp \
	AST/expressions/././../../building/../instructions/../machine/./machineFactory.hpp \
	AST/expressions/././../../building/../instructions/../machine/././stateGenerator.hpp \
	AST/expressions/././../../building/../instructions/../machine/././machine.hpp \
	AST/expressions/././../../building/../instructions/../machine/./././tape.hpp \
	AST/expressions/././../../building/../instructions/../machine/./machine.hpp \
	AST/expressions/././../../building/../instructions/../machine/multiTapeMachineFactory.hpp \
	AST/expressions/././../../building/./instructionCollection.hpp \
	AST/expressions/././../../building/././tapeReference.hpp \
	AST/expressions/././../../building/./../instructions/instruction.hpp \
	AST/expressions/./../../parsing/location.hpp \
	AST/expressions/./../../machine/machine.hpp \
	AST/expressions/../../instructions/clearInstruction.hpp \
	AST/expressions/../../instructions/./instruction.hpp \
	AST/expressions/../../instructions/../machine/singleTapeMachineFactory.hpp \
	AST/expressions/../../instructions/../machine/multiTapeMachineFactory.hpp \
	AST/expressions/../../instructions/../parsing/irArguments.hpp \
	AST/expressions/../../instructions/../parsing/./location.hpp \
	AST/expressions/../../instructions/../parsing/../machine/machine.hpp \
	AST/expressions/../../instructions/copyInstruction.hpp \
	AST/expressions/../../instructions/writeNumberInstruction.hpp \
	AST/expressions/../../instructions/callInstruction.hpp \
	AST/expressions/../../instructions/../machine/machine.hpp \
	AST/expressions/../../instructions/jumpInstruction.hpp \
	AST/expressions/../../instructions/compareInstruction.hpp \
	AST/expressions/../../instructions/compareTapeLengthInstruction.hpp \
	AST/expressions/../../machine/machineLibrary.hpp \
	AST/expressions/../../machine/./machine.hpp \
	AST/expressions/../../IO/typeError.hpp \
	AST/expressions/../../IO/./errorWithLocation.hpp \
	AST/expressions/../../IO/././error.hpp \
	AST/expressions/../../IO/./../parsing/location.hpp \
	AST/expressions/../../IO/../parsing/location.hpp
AST/expressions/callExpression.o: AST/expressions/callExpression.cpp \
	AST/expressions/./callExpression.hpp \
	AST/expressions/././expression.hpp \
	AST/expressions/././../../parsing/location.hpp \
	AST/expressions/././../variable.hpp \
	AST/expressions/././../../building/instructionBuilder.hpp \
	AST/expressions/././../../building/./tapeReference.hpp \
	AST/expressions/././../../building/../instructions/instruction.hpp \
	AST/expressions/././../../building/../instructions/../machine/singleTapeMachineFactory.hpp \
	AST/expressions/././../../building/../instructions/../machine/./machineFactory.hpp \
	AST/expressions/././../../building/../instructions/../machine/././stateGenerator.hpp \
	AST/expressions/././../../building/../instructions/../machine/././machine.hpp \
	AST/expressions/././../../building/../instructions/../machine/./././tape.hpp \
	AST/expressions/././../../building/../instructions/../machine/./machine.hpp \
	AST/expressions/././../../building/../instructions/../machine/multiTapeMachineFactory.hpp \
	AST/expressions/././../../building/./instructionCollection.hpp \
	AST/expressions/././../../building/././tapeReference.hpp \
	AST/expressions/././../../building/./../instructions/instruction.hpp \
	AST/expressions/./../../parsing/location.hpp \
	AST/expressions/./../../machine/machine.hpp \
	AST/expressions/../../instructions/writeNumberInstruction.hpp \
	AST/expressions/../../instructions/./instruction.hpp \
	AST/expressions/../../instructions/../machine/singleTapeMachineFactory.hpp \
	AST/expressions/../../instructions/../machine/multiTapeMachineFactory.hpp \
	AST/expressions/../../instructions/../parsing/irArguments.hpp \
	AST/expressions/../../instructions/../parsing/./location.hpp \
	AST/expressions/../../instructions/../parsing/../machine/machine.hpp \
	AST/expressions/../../instructions/copyInstruction.hpp \
	AST/expressions/../../instructions/callInstruction.hpp \
	AST/expressions/../../instructions/../machine/machine.hpp \
	AST/expressions/../../instructions/clearInstruction.hpp \
	AST/expressions/../../IO/typeError.hpp \
	AST/expressions/../../IO/./errorWithLocation.hpp \
	AST/expressions/../../IO/././error.hpp \
	AST/expressions/../../IO/./../parsing/location.hpp \
	AST/expressions/../../IO/../parsing/location.hpp
AST/expressions/countExpression.o: AST/expressions/countExpression.cpp \
	AST/expressions/./countExpression.hpp \
	AST/expressions/././expression.hpp \
	AST/expressions/././../../parsing/location.hpp \
	AST/expressions/././../variable.hpp \
	AST/expressions/././../../building/instructionBuilder.hpp \
	AST/expressions/././../../building/./tapeReference.hpp \
	AST/expressions/././../../building/../instructions/instruction.hpp \
	AST/expressions/././../../building/../instructions/../machine/singleTapeMachineFactory.hpp \
	AST/expressions/././../../building/../instructions/../machine/./machineFactory.hpp \
	AST/expressions/././../../building/../instructions/../machine/././stateGenerator.hpp \
	AST/expressions/././../../building/../instructions/../machine/././machine.hpp \
	AST/expressions/././../../building/../instructions/../machine/./././tape.hpp \
	AST/expressions/././../../building/../instructions/../machine/./machine.hpp \
	AST/expressions/././../../building/../instructions/../machine/multiTapeMachineFactory.hpp \
	AST/expressions/././../../building/./instructionCollection.hpp \
	AST/expressions/././../../building/././tapeReference.hpp \
	AST/expressions/././../../building/./../instructions/instruction.hpp \
	AST/expressions/./../../parsing/location.hpp \
	AST/expressions/../../instructions/copyInstruction.hpp \
	AST/expressions/../../instructions/./instruction.hpp \
	AST/expressions/../../instructions/../machine/multiTapeMachineFactory.hpp \
	AST/expressions/../../instructions/../parsing/irArguments.hpp \
	AST/expressions/../../instructions/../parsing/./location.hpp \
	AST/expressions/../../instructions/../parsing/../machine/machine.hpp \
	AST/expressions/../../instructions/callInstruction.hpp \
	AST/expressions/../../instructions/../machine/machine.hpp \
	AST/expressions/../../instructions/../machine/singleTapeMachineFactory.hpp \
	AST/expressions/../../machine/machineLibrary.hpp \
	AST/expressions/../../machine/./machine.hpp
AST/expressions/expression.o: AST/expressions/expression.cpp \
	AST/expressions/./expression.hpp \
	AST/expressions/./../../parsing/location.hpp \
	AST/expressions/./../variable.hpp \
	AST/expressions/./../../building/instructionBuilder.hpp \
	AST/expressions/./../../building/./tapeReference.hpp \
	AST/expressions/./../../building/../instructions/instruction.hpp \
	AST/expressions/./../../building/../instructions/../machine/singleTapeMachineFactory.hpp \
	AST/expressions/./../../building/../instructions/../machine/./machineFactory.hpp \
	AST/expressions/./../../building/../instructions/../machine/././stateGenerator.hpp \
	AST/expressions/./../../building/../instructions/../machine/././machine.hpp \
	AST/expressions/./../../building/../instructions/../machine/./././tape.hpp \
	AST/expressions/./../../building/../instructions/../machine/./machine.hpp \
	AST/expressions/./../../building/../instructions/../machine/multiTapeMachineFactory.hpp \
	AST/expressions/./../../building/./instructionCollection.hpp \
	AST/expressions/./../../building/././tapeReference.hpp \
	AST/expressions/./../../building/./../instructions/instruction.hpp \
	AST/expressions/../../IO/unexpectedError.hpp \
	AST/expressions/../../IO/./error.hpp
AST/expressions/numberExpression.o: AST/expressions/numberExpression.cpp \
	AST/expressions/./numberExpression.hpp \
	AST/expressions/././expression.hpp \
	AST/expressions/././../../parsing/location.hpp \
	AST/expressions/././../variable.hpp \
	AST/expressions/././../../building/instructionBuilder.hpp \
	AST/expressions/././../../building/./tapeReference.hpp \
	AST/expressions/././../../building/../instructions/instruction.hpp \
	AST/expressions/././../../building/../instructions/../machine/singleTapeMachineFactory.hpp \
	AST/expressions/././../../building/../instructions/../machine/./machineFactory.hpp \
	AST/expressions/././../../building/../instructions/../machine/././stateGenerator.hpp \
	AST/expressions/././../../building/../instructions/../machine/././machine.hpp \
	AST/expressions/././../../building/../instructions/../machine/./././tape.hpp \
	AST/expressions/././../../building/../instructions/../machine/./machine.hpp \
	AST/expressions/././../../building/../instructions/../machine/multiTapeMachineFactory.hpp \
	AST/expressions/././../../building/./instructionCollection.hpp \
	AST/expressions/././../../building/././tapeReference.hpp \
	AST/expressions/././../../building/./../instructions/instruction.hpp \
	AST/expressions/./../../parsing/location.hpp
AST/expressions/unaryExpression.o: AST/expressions/unaryExpression.cpp \
	AST/expressions/./unaryExpression.hpp \
	AST/expressions/././expression.hpp \
	AST/expressions/././../../parsing/location.hpp \
	AST/expressions/././../variable.hpp \
	AST/expressions/././../../building/instructionBuilder.hpp \
	AST/expressions/././../../building/./tapeReference.hpp \
	AST/expressions/././../../building/../instructions/instruction.hpp \
	AST/expressions/././../../building/../instructions/../machine/singleTapeMachineFactory.hpp \
	AST/expressions/././../../building/../instructions/../machine/./machineFactory.hpp \
	AST/expressions/././../../building/../instructions/../machine/././stateGenerator.hpp \
	AST/expressions/././../../building/../instructions/../machine/././machine.hpp \
	AST/expressions/././../../building/../instructions/../machine/./././tape.hpp \
	AST/expressions/././../../building/../instructions/../machine/./machine.hpp \
	AST/expressions/././../../building/../instructions/../machine/multiTapeMachineFactory.hpp \
	AST/expressions/././../../building/./instructionCollection.hpp \
	AST/expressions/././../../building/././tapeReference.hpp \
	AST/expressions/././../../building/./../instructions/instruction.hpp \
	AST/expressions/./../../parsing/location.hpp \
	AST/expressions/../../IO/typeError.hpp \
	AST/expressions/../../IO/./errorWithLocation.hpp \
	AST/expressions/../../IO/././error.hpp \
	AST/expressions/../../IO/./../parsing/location.hpp \
	AST/expressions/../../IO/../parsing/location.hpp
AST/expressions/variableExpression.o: \
	AST/expressions/variableExpression.cpp \
	AST/expressions/./variableExpression.hpp \
	AST/expressions/././expression.hpp \
	AST/expressions/././../../parsing/location.hpp \
	AST/expressions/././../variable.hpp \
	AST/expressions/././../../building/instructionBuilder.hpp \
	AST/expressions/././../../building/./tapeReference.hpp \
	AST/expressions/././../../building/../instructions/instruction.hpp \
	AST/expressions/././../../building/../instructions/../machine/singleTapeMachineFactory.hpp \
	AST/expressions/././../../building/../instructions/../machine/./machineFactory.hpp \
	AST/expressions/././../../building/../instructions/../machine/././stateGenerator.hpp \
	AST/expressions/././../../building/../instructions/../machine/././machine.hpp \
	AST/expressions/././../../building/../instructions/../machine/./././tape.hpp \
	AST/expressions/././../../building/../instructions/../machine/./machine.hpp \
	AST/expressions/././../../building/../instructions/../machine/multiTapeMachineFactory.hpp \
	AST/expressions/././../../building/./instructionCollection.hpp \
	AST/expressions/././../../building/././tapeReference.hpp \
	AST/expressions/././../../building/./../instructions/instruction.hpp \
	AST/expressions/./../../parsing/location.hpp \
	AST/expressions/./../variable.hpp
AST/program.o: AST/program.cpp AST/./program.hpp \
	AST/./../parsing/location.hpp AST/././variable.hpp \
	AST/./../machine/machine.hpp AST/./../machine/./tape.hpp \
	AST/././statements/statement.hpp \
	AST/././statements/../../building/instructionBuilder.hpp \
	AST/././statements/../../building/./tapeReference.hpp \
	AST/././statements/../../building/../instructions/instruction.hpp \
	AST/././statements/../../building/../instructions/../machine/singleTapeMachineFactory.hpp \
	AST/././statements/../../building/../instructions/../machine/./machineFactory.hpp \
	AST/././statements/../../building/../instructions/../machine/././stateGenerator.hpp \
	AST/././statements/../../building/../instructions/../machine/././machine.hpp \
	AST/././statements/../../building/../instructions/../machine/./machine.hpp \
	AST/././statements/../../building/../instructions/../machine/multiTapeMachineFactory.hpp \
	AST/././statements/../../building/./instructionCollection.hpp \
	AST/././statements/../../building/././tapeReference.hpp \
	AST/././statements/../../building/./../instructions/instruction.hpp \
	AST/./../building/instructionBuilder.hpp AST/./../IO/warning.hpp \
	AST/../instructions/decompressInstruction.hpp \
	AST/../instructions/./instruction.hpp \
	AST/../instructions/../machine/singleTapeMachineFactory.hpp \
	AST/../instructions/../machine/multiTapeMachineFactory.hpp \
	AST/../instructions/../parsing/irArguments.hpp \
	AST/../instructions/../parsing/./location.hpp \
	AST/../instructions/../parsing/../machine/machine.hpp \
	AST/../instructions/compressInstruction.hpp \
	AST/../instructions/jumpInstruction.hpp AST/../IO/unexpectedError.hpp \
	AST/../IO/./error.hpp AST/../IO/symbolError.hpp \
	AST/../IO/./errorWithLocation.hpp AST/../IO/././error.hpp \
	AST/../IO/./../parsing/location.hpp AST/../IO/../parsing/location.hpp \
	AST/../IO/generalWarning.hpp AST/../IO/./warning.hpp \
	AST/../IO/format.hpp
AST/sourceBundle.o: AST/sourceBundle.cpp AST/./sourceBundle.hpp \
	AST/././expressions/expression.hpp \
	AST/././expressions/../../parsing/location.hpp \
	AST/././expressions/../variable.hpp \
	AST/././expressions/../../building/instructionBuilder.hpp \
	AST/././expressions/../../building/./tapeReference.hpp \
	AST/././expressions/../../building/../instructions/instruction.hpp \
	AST/././expressions/../../building/../instructions/../machine/singleTapeMachineFactory.hpp \
	AST/././expressions/../../building/../instructions/../machine/./machineFactory.hpp \
	AST/././expressions/../../building/../instructions/../machine/././stateGenerator.hpp \
	AST/././expressions/../../building/../instructions/../machine/././machine.hpp \
	AST/././expressions/../../building/../instructions/../machine/./././tape.hpp \
	AST/././expressions/../../building/../instructions/../machine/./machine.hpp \
	AST/././expressions/../../building/../instructions/../machine/multiTapeMachineFactory.hpp \
	AST/././expressions/../../building/./instructionCollection.hpp \
	AST/././expressions/../../building/././tapeReference.hpp \
	AST/././expressions/../../building/./../instructions/instruction.hpp \
	AST/../IO/typeError.hpp AST/../IO/./errorWithLocation.hpp \
	AST/../IO/././error.hpp AST/../IO/./../parsing/location.hpp \
	AST/../IO/../parsing/location.hpp
AST/statements/assignStatement.o: AST/statements/assignStatement.cpp \
	AST/statements/./assignStatement.hpp AST/statements/././statement.hpp \
	AST/statements/././../../building/instructionBuilder.hpp \
	AST/statements/././../../building/./tapeReference.hpp \
	AST/statements/././../../building/../instructions/instruction.hpp \
	AST/statements/././../../building/../instructions/../machine/singleTapeMachineFactory.hpp \
	AST/statements/././../../building/../instructions/../machine/./machineFactory.hpp \
	AST/statements/././../../building/../instructions/../machine/././stateGenerator.hpp \
	AST/statements/././../../building/../instructions/../machine/././machine.hpp \
	AST/statements/././../../building/../instructions/../machine/./././tape.hpp \
	AST/statements/././../../building/../instructions/../machine/./machine.hpp \
	AST/statements/././../../building/../instructions/../machine/multiTapeMachineFactory.hpp \
	AST/statements/././../../building/./instructionCollection.hpp \
	AST/statements/././../../building/././tapeReference.hpp \
	AST/statements/././../../building/./../instructions/instruction.hpp \
	AST/statements/./../variable.hpp \
	AST/statements/./../expressions/expression.hpp \
	AST/statements/./../expressions/../../parsing/location.hpp \
	AST/statements/./../expressions/../variable.hpp \
	AST/statements/./../expressions/../../building/instructionBuilder.hpp \
	AST/statements/./../../building/instructionBuilder.hpp \
	AST/statements/../../instructions/writeNumberInstruction.hpp \
	AST/statements/../../instructions/./instruction.hpp \
	AST/statements/../../instructions/../machine/singleTapeMachineFactory.hpp \
	AST/statements/../../instructions/../machine/multiTapeMachineFactory.hpp \
	AST/statements/../../instructions/../parsing/irArguments.hpp \
	AST/statements/../../instructions/../parsing/./location.hpp \
	AST/statements/../../instructions/../parsing/../machine/machine.hpp \
	AST/statements/../../instructions/copyInstruction.hpp \
	AST/statements/../../instructions/clearInstruction.hpp \
	AST/statements/../../IO/typeError.hpp \
	AST/statements/../../IO/./errorWithLocation.hpp \
	AST/statements/../../IO/././error.hpp \
	AST/statements/../../IO/./../parsing/location.hpp \
	AST/statements/../../IO/../parsing/location.hpp
AST/statements/breakStatement.o: AST/statements/breakStatement.cpp \
	AST/statements/./breakStatement.hpp AST/statements/././statement.hpp \
	AST/statements/././../../building/instructionBuilder.hpp \
	AST/statements/././../../building/./tapeReference.hpp \
	AST/statements/././../../building/../instructions/instruction.hpp \
	AST/statements/././../../building/../instructions/../machine/singleTapeMachineFactory.hpp \
	AST/statements/././../../building/../instructions/../machine/./machineFactory.hpp \
	AST/statements/././../../building/../instructions/../machine/././stateGenerator.hpp \
	AST/statements/././../../building/../instructions/../machine/././machine.hpp \
	AST/statements/././../../building/../instructions/../machine/./././tape.hpp \
	AST/statements/././../../building/../instructions/../machine/./machine.hpp \
	AST/statements/././../../building/../instructions/../machine/multiTapeMachineFactory.hpp \
	AST/statements/././../../building/./instructionCollection.hpp \
	AST/statements/././../../building/././tapeReference.hpp \
	AST/statements/././../../building/./../instructions/instruction.hpp \
	AST/statements/../../instructions/jumpInstruction.hpp \
	AST/statements/../../instructions/./instruction.hpp \
	AST/statements/../../instructions/../machine/singleTapeMachineFactory.hpp \
	AST/statements/../../instructions/../machine/multiTapeMachineFactory.hpp \
	AST/statements/../../instructions/../parsing/irArguments.hpp \
	AST/statements/../../instructions/../parsing/./location.hpp \
	AST/statements/../../instructions/../parsing/../machine/machine.hpp \
	AST/statements/../../IO/unexpectedError.hpp \
	AST/statements/../../IO/./error.hpp
AST/statements/callStatement.o: AST/statements/callStatement.cpp \
	AST/statements/./callStatement.hpp AST/statements/././statement.hpp \
	AST/statements/././../../building/instructionBuilder.hpp \
	AST/statements/././../../building/./tapeReference.hpp \
	AST/statements/././../../building/../instructions/instruction.hpp \
	AST/statements/././../../building/../instructions/../machine/singleTapeMachineFactory.hpp \
	AST/statements/././../../building/../instructions/../machine/./machineFactory.hpp \
	AST/statements/././../../building/../instructions/../machine/././stateGenerator.hpp \
	AST/statements/././../../building/../instructions/../machine/././machine.hpp \
	AST/statements/././../../building/../instructions/../machine/./././tape.hpp \
	AST/statements/././../../building/../instructions/../machine/./machine.hpp \
	AST/statements/././../../building/../instructions/../machine/multiTapeMachineFactory.hpp \
	AST/statements/././../../building/./instructionCollection.hpp \
	AST/statements/././../../building/././tapeReference.hpp \
	AST/statements/././../../building/./../instructions/instruction.hpp \
	AST/statements/./../variable.hpp \
	AST/statements/./../../machine/machine.hpp \
	AST/statements/../../instructions/callInstruction.hpp \
	AST/statements/../../instructions/./instruction.hpp \
	AST/statements/../../instructions/../machine/machine.hpp \
	AST/statements/../../instructions/../machine/singleTapeMachineFactory.hpp \
	AST/statements/../../instructions/../machine/multiTapeMachineFactory.hpp \
	AST/statements/../../instructions/../parsing/irArguments.hpp \
	AST/statements/../../instructions/../parsing/./location.hpp \
	AST/statements/../../instructions/../parsing/../machine/machine.hpp
AST/statements/continueStatement.o: AST/statements/continueStatement.cpp \
	AST/statements/./continueStatement.hpp \
	AST/statements/././statement.hpp \
	AST/statements/././../../building/instructionBuilder.hpp \
	AST/statements/././../../building/./tapeReference.hpp \
	AST/statements/././../../building/../instructions/instruction.hpp \
	AST/statements/././../../building/../instructions/../machine/singleTapeMachineFactory.hpp \
	AST/statements/././../../building/../instructions/../machine/./machineFactory.hpp \
	AST/statements/././../../building/../instructions/../machine/././stateGenerator.hpp \
	AST/statements/././../../building/../instructions/../machine/././machine.hpp \
	AST/statements/././../../building/../instructions/../machine/./././tape.hpp \
	AST/statements/././../../building/../instructions/../machine/./machine.hpp \
	AST/statements/././../../building/../instructions/../machine/multiTapeMachineFactory.hpp \
	AST/statements/././../../building/./instructionCollection.hpp \
	AST/statements/././../../building/././tapeReference.hpp \
	AST/statements/././../../building/./../instructions/instruction.hpp \
	AST/statements/../../instructions/jumpInstruction.hpp \
	AST/statements/../../instructions/./instruction.hpp \
	AST/statements/../../instructions/../machine/singleTapeMachineFactory.hpp \
	AST/statements/../../instructions/../machine/multiTapeMachineFactory.hpp \
	AST/statements/../../instructions/../parsing/irArguments.hpp \
	AST/statements/../../instructions/../parsing/./location.hpp \
	AST/statements/../../instructions/../parsing/../machine/machine.hpp \
	AST/statements/../../IO/unexpectedError.hpp \
	AST/statements/../../IO/./error.hpp
AST/statements/exitStatement.o: AST/statements/exitStatement.cpp \
	AST/statements/./exitStatement.hpp AST/statements/././statement.hpp \
	AST/statements/././../../building/instructionBuilder.hpp \
	AST/statements/././../../building/./tapeReference.hpp \
	AST/statements/././../../building/../instructions/instruction.hpp \
	AST/statements/././../../building/../instructions/../machine/singleTapeMachineFactory.hpp \
	AST/statements/././../../building/../instructions/../machine/./machineFactory.hpp \
	AST/statements/././../../building/../instructions/../machine/././stateGenerator.hpp \
	AST/statements/././../../building/../instructions/../machine/././machine.hpp \
	AST/statements/././../../building/../instructions/../machine/./././tape.hpp \
	AST/statements/././../../building/../instructions/../machine/./machine.hpp \
	AST/statements/././../../building/../instructions/../machine/multiTapeMachineFactory.hpp \
	AST/statements/././../../building/./instructionCollection.hpp \
	AST/statements/././../../building/././tapeReference.hpp \
	AST/statements/././../../building/./../instructions/instruction.hpp \
	AST/statements/../../instructions/jumpInstruction.hpp \
	AST/statements/../../instructions/./instruction.hpp \
	AST/statements/../../instructions/../machine/singleTapeMachineFactory.hpp \
	AST/statements/../../instructions/../machine/multiTapeMachineFactory.hpp \
	AST/statements/../../instructions/../parsing/irArguments.hpp \
	AST/statements/../../instructions/../parsing/./location.hpp \
	AST/statements/../../instructions/../parsing/../machine/machine.hpp \
	AST/statements/../../IO/unexpectedError.hpp \
	AST/statements/../../IO/./error.hpp
AST/statements/explodeStatement.o: AST/statements/explodeStatement.cpp \
	AST/statements/./explodeStatement.hpp AST/statements/././statement.hpp \
	AST/statements/././../../building/instructionBuilder.hpp \
	AST/statements/././../../building/./tapeReference.hpp \
	AST/statements/././../../building/../instructions/instruction.hpp \
	AST/statements/././../../building/../instructions/../machine/singleTapeMachineFactory.hpp \
	AST/statements/././../../building/../instructions/../machine/./machineFactory.hpp \
	AST/statements/././../../building/../instructions/../machine/././stateGenerator.hpp \
	AST/statements/././../../building/../instructions/../machine/././machine.hpp \
	AST/statements/././../../building/../instructions/../machine/./././tape.hpp \
	AST/statements/././../../building/../instructions/../machine/./machine.hpp \
	AST/statements/././../../building/../instructions/../machine/multiTapeMachineFactory.hpp \
	AST/statements/././../../building/./instructionCollection.hpp \
	AST/statements/././../../building/././tapeReference.hpp \
	AST/statements/././../../building/./../instructions/instruction.hpp \
	AST/statements/./../expressions/expression.hpp \
	AST/statements/./../expressions/../../parsing/location.hpp \
	AST/statements/./../expressions/../variable.hpp \
	AST/statements/./../expressions/../../building/instructionBuilder.hpp \
	AST/statements/./../destinationBundle.hpp \
	AST/statements/./.././variable.hpp \
	AST/statements/./../../parsing/location.hpp \
	AST/statements/../variable.hpp \
	AST/statements/../../instructions/copyInstruction.hpp \
	AST/statements/../../instructions/./instruction.hpp \
	AST/statements/../../instructions/../machine/multiTapeMachineFactory.hpp \
	AST/statements/../../instructions/../parsing/irArguments.hpp \
	AST/statements/../../instructions/../parsing/./location.hpp \
	AST/statements/../../instructions/../parsing/../machine/machine.hpp \
	AST/statements/../../instructions/clearInstruction.hpp \
	AST/statements/../../instructions/../machine/singleTapeMachineFactory.hpp \
	AST/statements/../../IO/typeError.hpp \
	AST/statements/../../IO/./errorWithLocation.hpp \
	AST/statements/../../IO/././error.hpp \
	AST/statements/../../IO/./../parsing/location.hpp \
	AST/statements/../../IO/../parsing/location.hpp
AST/statements/ifStatement.o: AST/statements/ifStatement.cpp \
	AST/statements/./ifStatement.hpp AST/statements/././statement.hpp \
	AST/statements/././../../building/instructionBuilder.hpp \
	AST/statements/././../../building/./tapeReference.hpp \
	AST/statements/././../../building/../instructions/instruction.hpp \
	AST/statements/././../../building/../instructions/../machine/singleTapeMachineFactory.hpp \
	AST/statements/././../../building/../instructions/../machine/./machineFactory.hpp \
	AST/statements/././../../building/../instructions/../machine/././stateGenerator.hpp \
	AST/statements/././../../building/../instructions/../machine/././machine.hpp \
	AST/statements/././../../building/../instructions/../machine/./././tape.hpp \
	AST/statements/././../../building/../instructions/../machine/./machine.hpp \
	AST/statements/././../../building/../instructions/../machine/multiTapeMachineFactory.hpp \
	AST/statements/././../../building/./instructionCollection.hpp \
	AST/statements/././../../building/././tapeReference.hpp \
	AST/statements/././../../building/./../instructions/instruction.hpp \
	AST/statements/./../expressions/expression.hpp \
	AST/statements/./../expressions/../../parsing/location.hpp \
	AST/statements/./../expressions/../variable.hpp \
	AST/statements/./../expressions/../../building/instructionBuilder.hpp \
	AST/statements/../../instructions/jumpInstruction.hpp \
	AST/statements/../../instructions/./instruction.hpp \
	AST/statements/../../instructions/../machine/singleTapeMachineFactory.hpp \
	AST/statements/../../instructions/../machine/multiTapeMachineFactory.hpp \
	AST/statements/../../instructions/../parsing/irArguments.hpp \
	AST/statements/../../instructions/../parsing/./location.hpp \
	AST/statements/../../instructions/../parsing/../machine/machine.hpp \
	AST/statements/../../IO/typeError.hpp \
	AST/statements/../../IO/./errorWithLocation.hpp \
	AST/statements/../../IO/././error.hpp \
	AST/statements/../../IO/./../parsing/location.hpp \
	AST/statements/../../IO/../parsing/location.hpp
AST/statements/implodeStatement.o: AST/statements/implodeStatement.cpp \
	AST/statements/./implodeStatement.hpp AST/statements/././statement.hpp \
	AST/statements/././../../building/instructionBuilder.hpp \
	AST/statements/././../../building/./tapeReference.hpp \
	AST/statements/././../../building/../instructions/instruction.hpp \
	AST/statements/././../../building/../instructions/../machine/singleTapeMachineFactory.hpp \
	AST/statements/././../../building/../instructions/../machine/./machineFactory.hpp \
	AST/statements/././../../building/../instructions/../machine/././stateGenerator.hpp \
	AST/statements/././../../building/../instructions/../machine/././machine.hpp \
	AST/statements/././../../building/../instructions/../machine/./././tape.hpp \
	AST/statements/././../../building/../instructions/../machine/./machine.hpp \
	AST/statements/././../../building/../instructions/../machine/multiTapeMachineFactory.hpp \
	AST/statements/././../../building/./instructionCollection.hpp \
	AST/statements/././../../building/././tapeReference.hpp \
	AST/statements/././../../building/./../instructions/instruction.hpp \
	AST/statements/./../variable.hpp AST/statements/./../sourceBundle.hpp \
	AST/statements/./.././expressions/expression.hpp \
	AST/statements/./.././expressions/../../parsing/location.hpp \
	AST/statements/./.././expressions/../variable.hpp \
	AST/statements/./.././expressions/../../building/instructionBuilder.hpp \
	AST/statements/./../expressions/expression.hpp \
	AST/statements/./../../parsing/location.hpp \
	AST/statements/../../instructions/clearInstruction.hpp \
	AST/statements/../../instructions/./instruction.hpp \
	AST/statements/../../instructions/../machine/singleTapeMachineFactory.hpp \
	AST/statements/../../instructions/../machine/multiTapeMachineFactory.hpp \
	AST/statements/../../instructions/../parsing/irArguments.hpp \
	AST/statements/../../instructions/../parsing/./location.hpp \
	AST/statements/../../instructions/../parsing/../machine/machine.hpp \
	AST/statements/../../instructions/writeNumberInstruction.hpp \
	AST/statements/../../instructions/copyInstruction.hpp
AST/statements/loopStatement.o: AST/statements/loopStatement.cpp \
	AST/statements/./loopStatement.hpp AST/statements/././statement.hpp \
	AST/statements/././../../building/instructionBuilder.hpp \
	AST/statements/././../../building/./tapeReference.hpp \
	AST/statements/././../../building/../instructions/instruction.hpp \
	AST/statements/././../../building/../instructions/../machine/singleTapeMachineFactory.hpp \
	AST/statements/././../../building/../instructions/../machine/./machineFactory.hpp \
	AST/statements/././../../building/../instructions/../machine/././stateGenerator.hpp \
	AST/statements/././../../building/../instructions/../machine/././machine.hpp \
	AST/statements/././../../building/../instructions/../machine/./././tape.hpp \
	AST/statements/././../../building/../instructions/../machine/./machine.hpp \
	AST/statements/././../../building/../instructions/../machine/multiTapeMachineFactory.hpp \
	AST/statements/././../../building/./instructionCollection.hpp \
	AST/statements/././../../building/././tapeReference.hpp \
	AST/statements/././../../building/./../instructions/instruction.hpp \
	AST/statements/../../instructions/jumpInstruction.hpp \
	AST/statements/../../instructions/./instruction.hpp \
	AST/statements/../../instructions/../machine/singleTapeMachineFactory.hpp \
	AST/statements/../../instructions/../machine/multiTapeMachineFactory.hpp \
	AST/statements/../../instructions/../parsing/irArguments.hpp \
	AST/statements/../../instructions/../parsing/./location.hpp \
	AST/statements/../../instructions/../parsing/../machine/machine.hpp
AST/statements/statement.o: AST/statements/statement.cpp \
	AST/statements/./statement.hpp \
	AST/statements/./../../building/instructionBuilder.hpp \
	AST/statements/./../../building/./tapeReference.hpp \
	AST/statements/./../../building/../instructions/instruction.hpp \
	AST/statements/./../../building/../instructions/../machine/singleTapeMachineFactory.hpp \
	AST/statements/./../../building/../instructions/../machine/./machineFactory.hpp \
	AST/statements/./../../building/../instructions/../machine/././stateGenerator.hpp \
	AST/statements/./../../building/../instructions/../machine/././machine.hpp \
	AST/statements/./../../building/../instructions/../machine/./././tape.hpp \
	AST/statements/./../../building/../instructions/../machine/./machine.hpp \
	AST/statements/./../../building/../instructions/../machine/multiTapeMachineFactory.hpp \
	AST/statements/./../../building/./instructionCollection.hpp \
	AST/statements/./../../building/././tapeReference.hpp \
	AST/statements/./../../building/./../instructions/instruction.hpp
AST/statements/whileStatement.o: AST/statements/whileStatement.cpp \
	AST/statements/./whileStatement.hpp AST/statements/././statement.hpp \
	AST/statements/././../../building/instructionBuilder.hpp \
	AST/statements/././../../building/./tapeReference.hpp \
	AST/statements/././../../building/../instructions/instruction.hpp \
	AST/statements/././../../building/../instructions/../machine/singleTapeMachineFactory.hpp \
	AST/statements/././../../building/../instructions/../machine/./machineFactory.hpp \
	AST/statements/././../../building/../instructions/../machine/././stateGenerator.hpp \
	AST/statements/././../../building/../instructions/../machine/././machine.hpp \
	AST/statements/././../../building/../instructions/../machine/./././tape.hpp \
	AST/statements/././../../building/../instructions/../machine/./machine.hpp \
	AST/statements/././../../building/../instructions/../machine/multiTapeMachineFactory.hpp \
	AST/statements/././../../building/./instructionCollection.hpp \
	AST/statements/././../../building/././tapeReference.hpp \
	AST/statements/././../../building/./../instructions/instruction.hpp \
	AST/statements/./../expressions/expression.hpp \
	AST/statements/./../expressions/../../parsing/location.hpp \
	AST/statements/./../expressions/../variable.hpp \
	AST/statements/./../expressions/../../building/instructionBuilder.hpp \
	AST/statements/../../instructions/jumpInstruction.hpp \
	AST/statements/../../instructions/./instruction.hpp \
	AST/statements/../../instructions/../machine/singleTapeMachineFactory.hpp \
	AST/statements/../../instructions/../machine/multiTapeMachineFactory.hpp \
	AST/statements/../../instructions/../parsing/irArguments.hpp \
	AST/statements/../../instructions/../parsing/./location.hpp \
	AST/statements/../../instructions/../parsing/../machine/machine.hpp \
	AST/statements/../../IO/typeError.hpp \
	AST/statements/../../IO/./errorWithLocation.hpp \
	AST/statements/../../IO/././error.hpp \
	AST/statements/../../IO/./../parsing/location.hpp \
	AST/statements/../../IO/../parsing/location.hpp
AST/variable.o: AST/variable.cpp AST/./variable.hpp
building/instructionBuilder.o: building/instructionBuilder.cpp \
	building/./instructionBuilder.hpp building/././tapeReference.hpp \
	building/./../instructions/instruction.hpp \
	building/./../instructions/../machine/singleTapeMachineFactory.hpp \
	building/./../instructions/../machine/./machineFactory.hpp \
	building/./../instructions/../machine/././stateGenerator.hpp \
	building/./../instructions/../machine/././machine.hpp \
	building/./../instructions/../machine/./././tape.hpp \
	building/./../instructions/../machine/./machine.hpp \
	building/./../instructions/../machine/multiTapeMachineFactory.hpp \
	building/././instructionCollection.hpp \
	building/./././tapeReference.hpp \
	building/././../instructions/instruction.hpp \
	building/../IO/unexpectedError.hpp building/../IO/./error.hpp
building/instructionCollection.o: building/instructionCollection.cpp \
	building/./instructionCollection.hpp building/././tapeReference.hpp \
	building/./../instructions/instruction.hpp \
	building/./../instructions/../machine/singleTapeMachineFactory.hpp \
	building/./../instructions/../machine/./machineFactory.hpp \
	building/./../instructions/../machine/././stateGenerator.hpp \
	building/./../instructions/../machine/././machine.hpp \
	building/./../instructions/../machine/./././tape.hpp \
	building/./../instructions/../machine/./machine.hpp \
	building/./../instructions/../machine/multiTapeMachineFactory.hpp \
	building/../IO/unexpectedError.hpp building/../IO/./error.hpp
building/machineBuilder.o: building/machineBuilder.cpp \
	building/./machineBuilder.hpp building/././instructionCollection.hpp \
	building/./././tapeReference.hpp \
	building/././../instructions/instruction.hpp \
	building/././../instructions/../machine/singleTapeMachineFactory.hpp \
	building/././../instructions/../machine/./machineFactory.hpp \
	building/././../instructions/../machine/././stateGenerator.hpp \
	building/././../instructions/../machine/././machine.hpp \
	building/././../instructions/../machine/./././tape.hpp \
	building/././../instructions/../machine/./machine.hpp \
	building/././../instructions/../machine/multiTapeMachineFactory.hpp \
	building/./../machine/machineFactory.hpp \
	building/./../machine/machine.hpp \
	building/../machine/machineFactory.hpp \
	building/../IO/unexpectedError.hpp building/../IO/./error.hpp
building/tapeReference.o: building/tapeReference.cpp \
	building/./tapeReference.hpp
instructions/callInstruction.o: instructions/callInstruction.cpp \
	instructions/./callInstruction.hpp instructions/././instruction.hpp \
	instructions/././../machine/singleTapeMachineFactory.hpp \
	instructions/././../machine/./machineFactory.hpp \
	instructions/././../machine/././stateGenerator.hpp \
	instructions/././../machine/././machine.hpp \
	instructions/././../machine/./././tape.hpp \
	instructions/././../machine/./machine.hpp \
	instructions/././../machine/multiTapeMachineFactory.hpp \
	instructions/./../machine/machine.hpp \
	instructions/./../machine/singleTapeMachineFactory.hpp \
	instructions/./../machine/multiTapeMachineFactory.hpp \
	instructions/./../parsing/irArguments.hpp \
	instructions/./../parsing/./location.hpp \
	instructions/./../parsing/../machine/machine.hpp \
	instructions/../IO/unexpectedError.hpp instructions/../IO/./error.hpp
instructions/clearInstruction.o: instructions/clearInstruction.cpp \
	instructions/./clearInstruction.hpp instructions/././instruction.hpp \
	instructions/././../machine/singleTapeMachineFactory.hpp \
	instructions/././../machine/./machineFactory.hpp \
	instructions/././../machine/././stateGenerator.hpp \
	instructions/././../machine/././machine.hpp \
	instructions/././../machine/./././tape.hpp \
	instructions/././../machine/./machine.hpp \
	instructions/././../machine/multiTapeMachineFactory.hpp \
	instructions/./../machine/singleTapeMachineFactory.hpp \
	instructions/./../machine/multiTapeMachineFactory.hpp \
	instructions/./../parsing/irArguments.hpp \
	instructions/./../parsing/./location.hpp \
	instructions/./../parsing/../machine/machine.hpp \
	instructions/../IO/unexpectedError.hpp instructions/../IO/./error.hpp \
	instructions/../IO/irParseError.hpp \
	instructions/../IO/./errorWithLocation.hpp \
	instructions/../IO/././error.hpp \
	instructions/../IO/./../parsing/location.hpp
instructions/compareInstruction.o: instructions/compareInstruction.cpp \
	instructions/./compareInstruction.hpp instructions/././instruction.hpp \
	instructions/././../machine/singleTapeMachineFactory.hpp \
	instructions/././../machine/./machineFactory.hpp \
	instructions/././../machine/././stateGenerator.hpp \
	instructions/././../machine/././machine.hpp \
	instructions/././../machine/./././tape.hpp \
	instructions/././../machine/./machine.hpp \
	instructions/././../machine/multiTapeMachineFactory.hpp \
	instructions/./../machine/singleTapeMachineFactory.hpp \
	instructions/./../machine/multiTapeMachineFactory.hpp \
	instructions/./../parsing/irArguments.hpp \
	instructions/./../parsing/./location.hpp \
	instructions/./../parsing/../machine/machine.hpp \
	instructions/../IO/unexpectedError.hpp instructions/../IO/./error.hpp
instructions/compareTapeLengthInstruction.o: \
	instructions/compareTapeLengthInstruction.cpp \
	instructions/./compareTapeLengthInstruction.hpp \
	instructions/././instruction.hpp \
	instructions/././../machine/singleTapeMachineFactory.hpp \
	instructions/././../machine/./machineFactory.hpp \
	instructions/././../machine/././stateGenerator.hpp \
	instructions/././../machine/././machine.hpp \
	instructions/././../machine/./././tape.hpp \
	instructions/././../machine/./machine.hpp \
	instructions/././../machine/multiTapeMachineFactory.hpp \
	instructions/./../machine/singleTapeMachineFactory.hpp \
	instructions/./../machine/multiTapeMachineFactory.hpp \
	instructions/./../parsing/irArguments.hpp \
	instructions/./../parsing/./location.hpp \
	instructions/./../parsing/../machine/machine.hpp \
	instructions/../IO/unexpectedError.hpp instructions/../IO/./error.hpp
instructions/compressInstruction.o: instructions/compressInstruction.cpp \
	instructions/./compressInstruction.hpp \
	instructions/././instruction.hpp \
	instructions/././../machine/singleTapeMachineFactory.hpp \
	instructions/././../machine/./machineFactory.hpp \
	instructions/././../machine/././stateGenerator.hpp \
	instructions/././../machine/././machine.hpp \
	instructions/././../machine/./././tape.hpp \
	instructions/././../machine/./machine.hpp \
	instructions/././../machine/multiTapeMachineFactory.hpp \
	instructions/./../machine/singleTapeMachineFactory.hpp \
	instructions/./../machine/multiTapeMachineFactory.hpp \
	instructions/./../parsing/irArguments.hpp \
	instructions/./../parsing/./location.hpp \
	instructions/./../parsing/../machine/machine.hpp \
	instructions/../IO/unexpectedError.hpp instructions/../IO/./error.hpp
instructions/copyInstruction.o: instructions/copyInstruction.cpp \
	instructions/./copyInstruction.hpp instructions/././instruction.hpp \
	instructions/././../machine/singleTapeMachineFactory.hpp \
	instructions/././../machine/./machineFactory.hpp \
	instructions/././../machine/././stateGenerator.hpp \
	instructions/././../machine/././machine.hpp \
	instructions/././../machine/./././tape.hpp \
	instructions/././../machine/./machine.hpp \
	instructions/././../machine/multiTapeMachineFactory.hpp \
	instructions/./../machine/multiTapeMachineFactory.hpp \
	instructions/./../parsing/irArguments.hpp \
	instructions/./../parsing/./location.hpp \
	instructions/./../parsing/../machine/machine.hpp \
	instructions/./clearInstruction.hpp \
	instructions/./../machine/singleTapeMachineFactory.hpp \
	instructions/../IO/unexpectedError.hpp instructions/../IO/./error.hpp
instructions/decompressInstruction.o: \
	instructions/decompressInstruction.cpp \
	instructions/./decompressInstruction.hpp \
	instructions/././instruction.hpp \
	instructions/././../machine/singleTapeMachineFactory.hpp \
	instructions/././../machine/./machineFactory.hpp \
	instructions/././../machine/././stateGenerator.hpp \
	instructions/././../machine/././machine.hpp \
	instructions/././../machine/./././tape.hpp \
	instructions/././../machine/./machine.hpp \
	instructions/././../machine/multiTapeMachineFactory.hpp \
	instructions/./../machine/singleTapeMachineFactory.hpp \
	instructions/./../machine/multiTapeMachineFactory.hpp \
	instructions/./../parsing/irArguments.hpp \
	instructions/./../parsing/./location.hpp \
	instructions/./../parsing/../machine/machine.hpp \
	instructions/../IO/unexpectedError.hpp instructions/../IO/./error.hpp
instructions/instruction.o: instructions/instruction.cpp \
	instructions/./instruction.hpp \
	instructions/./../machine/singleTapeMachineFactory.hpp \
	instructions/./../machine/./machineFactory.hpp \
	instructions/./../machine/././stateGenerator.hpp \
	instructions/./../machine/././machine.hpp \
	instructions/./../machine/./././tape.hpp \
	instructions/./../machine/./machine.hpp \
	instructions/./../machine/multiTapeMachineFactory.hpp \
	instructions/../IO/unexpectedError.hpp instructions/../IO/./error.hpp
instructions/jumpInstruction.o: instructions/jumpInstruction.cpp \
	instructions/./jumpInstruction.hpp instructions/././instruction.hpp \
	instructions/././../machine/singleTapeMachineFactory.hpp \
	instructions/././../machine/./machineFactory.hpp \
	instructions/././../machine/././stateGenerator.hpp \
	instructions/././../machine/././machine.hpp \
	instructions/././../machine/./././tape.hpp \
	instructions/././../machine/./machine.hpp \
	instructions/././../machine/multiTapeMachineFactory.hpp \
	instructions/./../machine/singleTapeMachineFactory.hpp \
	instructions/./../machine/multiTapeMachineFactory.hpp \
	instructions/./../parsing/irArguments.hpp \
	instructions/./../parsing/./location.hpp \
	instructions/./../parsing/../machine/machine.hpp
instructions/writeNumberInstruction.o: \
	instructions/writeNumberInstruction.cpp \
	instructions/./writeNumberInstruction.hpp \
	instructions/././instruction.hpp \
	instructions/././../machine/singleTapeMachineFactory.hpp \
	instructions/././../machine/./machineFactory.hpp \
	instructions/././../machine/././stateGenerator.hpp \
	instructions/././../machine/././machine.hpp \
	instructions/././../machine/./././tape.hpp \
	instructions/././../machine/./machine.hpp \
	instructions/././../machine/multiTapeMachineFactory.hpp \
	instructions/./../machine/singleTapeMachineFactory.hpp \
	instructions/./../machine/multiTapeMachineFactory.hpp \
	instructions/./../parsing/irArguments.hpp \
	instructions/./../parsing/./location.hpp \
	instructions/./../parsing/../machine/machine.hpp \
	instructions/../IO/unexpectedError.hpp instructions/../IO/./error.hpp
IO/error.o: IO/error.cpp IO/./error.hpp IO/./format.hpp
IO/errorWithLocation.o: IO/errorWithLocation.cpp \
	IO/./errorWithLocation.hpp IO/././error.hpp \
	IO/./../parsing/location.hpp IO/../parsing/location.hpp \
	IO/./format.hpp
IO/flags.o: IO/flags.cpp IO/./flags.hpp
IO/format.o: IO/format.cpp IO/./format.hpp
IO/generalError.o: IO/generalError.cpp IO/./generalError.hpp \
	IO/././error.hpp
IO/generalWarning.o: IO/generalWarning.cpp IO/./generalWarning.hpp \
	IO/././warning.hpp
IO/includeResolver.o: IO/includeResolver.cpp IO/./includeResolver.hpp \
	IO/./../machine/machine.hpp IO/./../machine/./tape.hpp \
	IO/./../parsing/location.hpp IO/../IO/format.hpp \
	IO/../IO/generalError.hpp IO/../IO/./error.hpp \
	IO/../IO/symbolError.hpp IO/../IO/./errorWithLocation.hpp \
	IO/../IO/././error.hpp IO/../IO/./../parsing/location.hpp \
	IO/../IO/../parsing/location.hpp
IO/input.o: IO/input.cpp IO/./input.hpp IO/././flags.hpp \
	IO/././inputLanguage.hpp IO/./../machine/tape.hpp \
	IO/./../machine/machine.hpp IO/./../machine/./tape.hpp \
	IO/../IO/format.hpp IO/../IO/generalError.hpp IO/../IO/./error.hpp \
	IO/../IO/unexpectedError.hpp
IO/inputLanguage.o: IO/inputLanguage.cpp IO/./inputLanguage.hpp
IO/irParseError.o: IO/irParseError.cpp IO/./irParseError.hpp \
	IO/././errorWithLocation.hpp IO/./././error.hpp \
	IO/././../parsing/location.hpp IO/./format.hpp
IO/lexerError.o: IO/lexerError.cpp IO/./lexerError.hpp \
	IO/././errorWithLocation.hpp IO/./././error.hpp \
	IO/././../parsing/location.hpp IO/./../parsing/location.hpp \
	IO/./unexpectedError.hpp IO/././error.hpp IO/./format.hpp
IO/parseError.o: IO/parseError.cpp IO/./parseError.hpp \
	IO/././errorWithLocation.hpp IO/./././error.hpp \
	IO/././../parsing/location.hpp IO/./../parsing/token.hpp \
	IO/./../parsing/./location.hpp IO/./../parsing/../machine/machine.hpp \
	IO/./../parsing/../machine/./tape.hpp IO/./format.hpp
IO/symbolError.o: IO/symbolError.cpp IO/./symbolError.hpp \
	IO/././errorWithLocation.hpp IO/./././error.hpp \
	IO/././../parsing/location.hpp IO/./../parsing/location.hpp \
	IO/../parsing/location.hpp IO/./format.hpp IO/./unexpectedError.hpp \
	IO/././error.hpp
IO/typeError.o: IO/typeError.cpp IO/./typeError.hpp \
	IO/././errorWithLocation.hpp IO/./././error.hpp \
	IO/././../parsing/location.hpp IO/./../parsing/location.hpp \
	IO/./format.hpp
IO/unexpectedError.o: IO/unexpectedError.cpp IO/./unexpectedError.hpp \
	IO/././error.hpp
IO/warning.o: IO/warning.cpp IO/./warning.hpp IO/./format.hpp
machine/machine.o: machine/machine.cpp machine/./machine.hpp \
	machine/././tape.hpp machine/../IO/unexpectedError.hpp \
	machine/../IO/./error.hpp
machine/machineFactory.o: machine/machineFactory.cpp \
	machine/./machineFactory.hpp machine/././stateGenerator.hpp \
	machine/././machine.hpp machine/./././tape.hpp \
	machine/../IO/unexpectedError.hpp machine/../IO/./error.hpp
machine/machineLibrary.o: machine/machineLibrary.cpp \
	machine/./machineLibrary.hpp machine/././machine.hpp \
	machine/./././tape.hpp
machine/multiTapeMachineFactory.o: machine/multiTapeMachineFactory.cpp \
	machine/./multiTapeMachineFactory.hpp machine/././machineFactory.hpp \
	machine/./././stateGenerator.hpp machine/./././machine.hpp \
	machine/././././tape.hpp machine/././machine.hpp \
	machine/./stateGenerator.hpp machine/./singleTapeMachineFactory.hpp \
	machine/../IO/unexpectedError.hpp machine/../IO/./error.hpp
machine/singleTapeMachineFactory.o: machine/singleTapeMachineFactory.cpp \
	machine/./singleTapeMachineFactory.hpp machine/././machineFactory.hpp \
	machine/./././stateGenerator.hpp machine/./././machine.hpp \
	machine/././././tape.hpp machine/././machine.hpp \
	machine/../IO/unexpectedError.hpp machine/../IO/./error.hpp
machine/stateGenerator.o: machine/stateGenerator.cpp \
	machine/./stateGenerator.hpp machine/../IO/unexpectedError.hpp \
	machine/../IO/./error.hpp
machine/tape.o: machine/tape.cpp machine/./tape.hpp \
	machine/../IO/unexpectedError.hpp machine/../IO/./error.hpp \
	machine/../IO/format.hpp
parsing/irArguments.o: parsing/irArguments.cpp parsing/./irArguments.hpp \
	parsing/././location.hpp parsing/./../machine/machine.hpp \
	parsing/./../machine/./tape.hpp parsing/../IO/irParseError.hpp \
	parsing/../IO/./errorWithLocation.hpp parsing/../IO/././error.hpp \
	parsing/../IO/./../parsing/location.hpp
parsing/irParser.o: parsing/irParser.cpp parsing/./irParser.hpp \
	parsing/./../instructions/instruction.hpp \
	parsing/./../instructions/../machine/singleTapeMachineFactory.hpp \
	parsing/./../instructions/../machine/./machineFactory.hpp \
	parsing/./../instructions/../machine/././stateGenerator.hpp \
	parsing/./../instructions/../machine/././machine.hpp \
	parsing/./../instructions/../machine/./././tape.hpp \
	parsing/./../instructions/../machine/./machine.hpp \
	parsing/./../instructions/../machine/multiTapeMachineFactory.hpp \
	parsing/./../building/instructionCollection.hpp \
	parsing/./../building/./tapeReference.hpp \
	parsing/./../building/../instructions/instruction.hpp \
	parsing/././irArguments.hpp parsing/./././location.hpp \
	parsing/././../machine/machine.hpp parsing/./../IO/warning.hpp \
	parsing/../IO/irParseError.hpp parsing/../IO/./errorWithLocation.hpp \
	parsing/../IO/././error.hpp parsing/../IO/./../parsing/location.hpp \
	parsing/../instructions/decompressInstruction.hpp \
	parsing/../instructions/./instruction.hpp \
	parsing/../instructions/../machine/singleTapeMachineFactory.hpp \
	parsing/../instructions/../machine/multiTapeMachineFactory.hpp \
	parsing/../instructions/../parsing/irArguments.hpp \
	parsing/../instructions/compressInstruction.hpp \
	parsing/../instructions/clearInstruction.hpp \
	parsing/../instructions/writeNumberInstruction.hpp \
	parsing/../instructions/copyInstruction.hpp \
	parsing/../instructions/callInstruction.hpp \
	parsing/../instructions/../machine/machine.hpp \
	parsing/../instructions/jumpInstruction.hpp \
	parsing/../instructions/compareInstruction.hpp
parsing/lexer.o: parsing/lexer.cpp parsing/./lexer.hpp \
	parsing/././token.hpp parsing/./././location.hpp \
	parsing/././../machine/machine.hpp parsing/././../machine/./tape.hpp \
	parsing/./../IO/warning.hpp parsing/../IO/lexerError.hpp \
	parsing/../IO/./errorWithLocation.hpp parsing/../IO/././error.hpp \
	parsing/../IO/./../parsing/location.hpp \
	parsing/../IO/../parsing/location.hpp parsing/../IO/generalWarning.hpp \
	parsing/../IO/./warning.hpp
parsing/location.o: parsing/location.cpp parsing/./location.hpp \
	parsing/../IO/unexpectedError.hpp parsing/../IO/./error.hpp
parsing/parser.o: parsing/parser.cpp parsing/./parser.hpp \
	parsing/./../machine/machine.hpp parsing/./../machine/./tape.hpp \
	parsing/././token.hpp parsing/./././location.hpp \
	parsing/././../machine/machine.hpp parsing/././location.hpp \
	parsing/./../AST/program.hpp parsing/./../AST/../parsing/location.hpp \
	parsing/./../AST/./variable.hpp \
	parsing/./../AST/../machine/machine.hpp \
	parsing/./../AST/./statements/statement.hpp \
	parsing/./../AST/./statements/../../building/instructionBuilder.hpp \
	parsing/./../AST/./statements/../../building/./tapeReference.hpp \
	parsing/./../AST/./statements/../../building/../instructions/instruction.hpp \
	parsing/./../AST/./statements/../../building/../instructions/../machine/singleTapeMachineFactory.hpp \
	parsing/./../AST/./statements/../../building/../instructions/../machine/./machineFactory.hpp \
	parsing/./../AST/./statements/../../building/../instructions/../machine/././stateGenerator.hpp \
	parsing/./../AST/./statements/../../building/../instructions/../machine/././machine.hpp \
	parsing/./../AST/./statements/../../building/../instructions/../machine/./machine.hpp \
	parsing/./../AST/./statements/../../building/../instructions/../machine/multiTapeMachineFactory.hpp \
	parsing/./../AST/./statements/../../building/./instructionCollection.hpp \
	parsing/./../AST/./statements/../../building/././tapeReference.hpp \
	parsing/./../AST/./statements/../../building/./../instructions/instruction.hpp \
	parsing/./../AST/../building/instructionBuilder.hpp \
	parsing/./../AST/../IO/warning.hpp \
	parsing/./../AST/statements/statement.hpp \
	parsing/./../AST/expressions/expression.hpp \
	parsing/./../AST/expressions/../../parsing/location.hpp \
	parsing/./../AST/expressions/../variable.hpp \
	parsing/./../AST/expressions/../../building/instructionBuilder.hpp \
	parsing/./../AST/sourceBundle.hpp \
	parsing/./../AST/./expressions/expression.hpp \
	parsing/./../AST/destinationBundle.hpp \
	parsing/./../IO/includeResolver.hpp \
	parsing/./../IO/../machine/machine.hpp \
	parsing/./../IO/../parsing/location.hpp parsing/./../IO/warning.hpp \
	parsing/./location.hpp parsing/../AST/expressions/numberExpression.hpp \
	parsing/../AST/expressions/./expression.hpp \
	parsing/../AST/expressions/../../parsing/location.hpp \
	parsing/../AST/expressions/variableExpression.hpp \
	parsing/../AST/expressions/../variable.hpp \
	parsing/../AST/expressions/arrayAccessExpression.hpp \
	parsing/../AST/expressions/unaryExpression.hpp \
	parsing/../AST/expressions/binaryExpression.hpp \
	parsing/../AST/expressions/../../machine/machine.hpp \
	parsing/../AST/expressions/countExpression.hpp \
	parsing/../AST/expressions/callExpression.hpp \
	parsing/../AST/statements/assignStatement.hpp \
	parsing/../AST/statements/./statement.hpp \
	parsing/../AST/statements/../variable.hpp \
	parsing/../AST/statements/../expressions/expression.hpp \
	parsing/../AST/statements/../../building/instructionBuilder.hpp \
	parsing/../AST/statements/implodeStatement.hpp \
	parsing/../AST/statements/../sourceBundle.hpp \
	parsing/../AST/statements/../../parsing/location.hpp \
	parsing/../AST/statements/explodeStatement.hpp \
	parsing/../AST/statements/../destinationBundle.hpp \
	parsing/../AST/statements/callStatement.hpp \
	parsing/../AST/statements/../../machine/machine.hpp \
	parsing/../AST/statements/ifStatement.hpp \
	parsing/../AST/statements/whileStatement.hpp \
	parsing/../AST/statements/loopStatement.hpp \
	parsing/../AST/statements/breakStatement.hpp \
	parsing/../AST/statements/continueStatement.hpp \
	parsing/../AST/statements/exitStatement.hpp \
	parsing/../machine/machineLibrary.hpp parsing/../machine/./machine.hpp \
	parsing/../IO/parseError.hpp parsing/../IO/./errorWithLocation.hpp \
	parsing/../IO/././error.hpp parsing/../IO/./../parsing/location.hpp \
	parsing/../IO/../parsing/token.hpp parsing/../IO/generalWarning.hpp \
	parsing/../IO/./warning.hpp parsing/../IO/format.hpp
parsing/token.o: parsing/token.cpp parsing/./token.hpp \
	parsing/././location.hpp parsing/./../machine/machine.hpp \
	parsing/./../machine/./tape.hpp parsing/../IO/unexpectedError.hpp \
	parsing/../IO/./error.hpp
tur: tur.cpp IO/input.hpp IO/./flags.hpp IO/./inputLanguage.hpp \
	IO/../machine/tape.hpp IO/../machine/machine.hpp \
	IO/../machine/./tape.hpp IO/flags.hpp IO/inputLanguage.hpp \
	IO/includeResolver.hpp IO/../parsing/location.hpp IO/error.hpp \
	IO/warning.hpp IO/generalError.hpp IO/./error.hpp machine/tape.hpp \
	machine/machine.hpp parsing/token.hpp parsing/./location.hpp \
	parsing/../machine/machine.hpp parsing/lexer.hpp parsing/./token.hpp \
	parsing/../IO/warning.hpp parsing/parser.hpp \
	parsing/../AST/program.hpp parsing/../AST/../parsing/location.hpp \
	parsing/../AST/./variable.hpp parsing/../AST/../machine/machine.hpp \
	parsing/../AST/./statements/statement.hpp \
	parsing/../AST/./statements/../../building/instructionBuilder.hpp \
	parsing/../AST/./statements/../../building/./tapeReference.hpp \
	parsing/../AST/./statements/../../building/../instructions/instruction.hpp \
	parsing/../AST/./statements/../../building/../instructions/../machine/singleTapeMachineFactory.hpp \
	parsing/../AST/./statements/../../building/../instructions/../machine/./machineFactory.hpp \
	parsing/../AST/./statements/../../building/../instructions/../machine/././stateGenerator.hpp \
	parsing/../AST/./statements/../../building/../instructions/../machine/././machine.hpp \
	parsing/../AST/./statements/../../building/../instructions/../machine/./machine.hpp \
	parsing/../AST/./statements/../../building/../instructions/../machine/multiTapeMachineFactory.hpp \
	parsing/../AST/./statements/../../building/./instructionCollection.hpp \
	parsing/../AST/./statements/../../building/././tapeReference.hpp \
	parsing/../AST/./statements/../../building/./../instructions/instruction.hpp \
	parsing/../AST/../building/instructionBuilder.hpp \
	parsing/../AST/../IO/warning.hpp \
	parsing/../AST/statements/statement.hpp \
	parsing/../AST/expressions/expression.hpp \
	parsing/../AST/expressions/../../parsing/location.hpp \
	parsing/../AST/expressions/../variable.hpp \
	parsing/../AST/expressions/../../building/instructionBuilder.hpp \
	parsing/../AST/sourceBundle.hpp \
	parsing/../AST/./expressions/expression.hpp \
	parsing/../AST/destinationBundle.hpp parsing/../IO/includeResolver.hpp \
	parsing/irParser.hpp parsing/../instructions/instruction.hpp \
	parsing/../building/instructionCollection.hpp \
	parsing/./irArguments.hpp parsing/././location.hpp \
	parsing/./../machine/machine.hpp AST/program.hpp \
	building/instructionBuilder.hpp building/instructionCollection.hpp \
	building/machineBuilder.hpp building/./instructionCollection.hpp \
	building/../machine/machineFactory.hpp building/../machine/machine.hpp
