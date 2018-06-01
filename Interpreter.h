#ifndef BASH_CLONE_INTERPRETER_H
#define BASH_CLONE_INTERPRETER_H
#include <iostream>
#include <vector>
#include "Command.h"
#include "Environment.h"
#include "ExecutionEngine.h"

/**
 * @class Interpreter
 * @brief Parses input and calls ExecutionEngine to execute command
 *
 * By default it works on stdin, but it might work on file
 * input as well.
 */
class Interpreter {
public:

    class parserException: public std::exception {
    public:
        parserException(const std::string &value)  {
            this->value = "Nieoczekiwany znacznik: '" + value + "\'";
        }

        const char* what() const throw() final
        {
            return value.c_str();
        }

    private:
        std::string value;
    };


    Interpreter(Environment &nenv, ExecutionEngine &engine) : env(nenv), engine(engine) { };
	std::vector<Command> nextCommandLine(std::string &line);

private:
    Environment &env;
	ExecutionEngine &engine;

    enum TokenType {QUOTATION, PIPE, AMPERSAND, SEMICOLON, END, STREAM, SPACE, ASSIGNMENT, WITH$, WORD};

    struct Token{
		Token(TokenType type, const std::string &value);
		TokenType type;
		std::string value;
	};

	std::vector<Token> separateTokens(std::string &line);
	std::vector<std::vector<Token>> separateInstruction(std::vector<Token> &tokens);
    Command createCommand(std::vector<Token> &instruction);
    bool is(char sign, const std::initializer_list<char> &acceptable) const;
    bool is(TokenType type, const std::initializer_list<TokenType> &acceptable) const;
    std::string concatenation(std::vector<Token> &instruction, int &it);


};


#endif //BASH_CLONE_INTERPRETER_H
