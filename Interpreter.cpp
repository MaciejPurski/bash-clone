#include "Interpreter.h"
std::vector<Command> Interpreter::processCommandLine(std::string &line) {
	std::vector<Token> tokens = lexer.separateTokens(line);
	return parser.separateCommands(tokens);
}