#include "Parser.h"
#include "BashExceptions.h"

Parser::Parser(Environment &env) : env(env) {}


std::vector<Command> Parser::separateCommands(std::vector<Token> &tokens) {

    std::vector<std::vector<Token>> instructions = syntaxAnalyse(tokens);
    std::vector<Command> commands;
    for (auto &it:instructions) {
        commands.push_back(createCommand(it));
    }
    return commands;
}


std::vector<std::vector<Token>> Parser::syntaxAnalyse(std::vector<Token> &tokens) {
    std::vector<std::vector<Token>> instructions;
    int it = 0;
    while (it < tokens.size()) {
        std::vector<Token> instruction;
        if (tokens[it].type == SPACE)
            it++;
        if (checkType(tokens[it].type, {SEMICOLON, PIPE, AMPERSAND})) {
            throw (InterpreterException("Syntax error:" + tokens[it].value + "unexpected"));
        }
        while (!checkType(tokens[it].type, {SEMICOLON, PIPE, AMPERSAND, END})) {
            if (tokens[it].type == REDIRECTION) {
                instruction.push_back(tokens[it++]);
                if (tokens[it].type == SPACE)
                    it++;
                if (!checkType(tokens[it].type, {WORD, QUOTATION, SPACE, ASSIGNMENT, WITH$})) {
                    throw (InterpreterException("Syntax error:" + tokens[it].value + "unexpected"));
                }
            }
            instruction.push_back(tokens[it++]);
        }

        instruction.push_back(tokens[it++]);
        instructions.push_back(instruction);
    }
    return instructions;
}

Command Parser::createCommand(std::vector<Token> &tokens) {
    Command command;
    int it = 0;
    while (!checkType(tokens[it].type, {PIPE, AMPERSAND, SEMICOLON, END})) {
        if (tokens[it].type == SPACE) {
            it++;
        } else if (tokens[it].type == REDIRECTION) {
            command.redirections.push_back(parseRedirection(tokens, it));
        } else if (command.command.empty()) {
            if (tokens[it].type == ASSIGNMENT) {
                parseAssignment(tokens, it);
            } else
                command.command = concatenation(tokens, it);
        } else if (command.command == "export") {
            if (tokens[it].type == ASSIGNMENT) {
                command.args.push_back(parseAssignment(tokens, it));
            } else
                command.args.push_back(concatenation(tokens, it));
        } else {
            command.args.push_back(concatenation(tokens, it));
        }

    }
    if (tokens[it].type == PIPE) {
        command.term = Command::PIPE;
    } else if (tokens[it].type == AMPERSAND) {
        command.term = Command::AMPER;
    }
    return command;
}

bool Parser::checkType(TokenType type, const std::initializer_list<TokenType> &acceptable) const {
    for (auto &it: acceptable) {
        if (it == type) {
            return true;
        }
    }
    return false;
}

Command::Redirection Parser::parseRedirection(std::vector<Token> &tokens, int &it){
    std::string::size_type tmp;
    int number;
    std::string sign;
    try {
        number = std::stoi(tokens[it].value, &tmp);
        sign = tokens[it].value.substr(tmp);
    }
    catch (std::invalid_argument &) {
        sign = tokens[it].value;
        if (sign != "<")
            number = 1;
        else
            number = 0;
    }
    it++;
    return Command::Redirection(number, sign == "<", sign == ">>", concatenation(tokens, it));
}

std::string Parser::parseAssignment(std::vector<Token> &tokens, int &it) {
    std::string tmp = tokens[it++].value;
    env.setVariable(tmp, concatenation(tokens, it));
    return tmp;
}

std::string Parser::concatenation(std::vector<Token> &tokens, int &it) {
    std::string tmp;
    while (checkType(tokens[it].type, {WORD, QUOTATION, ASSIGNMENT, WITH$})) {
        if (tokens[it].type == WITH$) {
            tmp += env.getValue(tokens[it].value);
        } else if (tokens[it].type == ASSIGNMENT) {
            tmp += tokens[it].value + "=";
        } else {
            tmp += tokens[it].value;
        }
        it++;
    }
    return tmp;
}




