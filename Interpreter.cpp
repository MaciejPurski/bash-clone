#include "Interpreter.h"

std::vector<Command> Interpreter::nextCommandLine(std::string &line) {
    std::vector<Token> tokens = separateTokens(line);
    std::vector<std::vector<Token>> instructions = separateInstruction(tokens);
    std::vector<Command> commands;
    for(auto &it:instructions){
        commands.push_back(createCommand(it));
    }
    return commands;
}

std::vector<Interpreter::Token> Interpreter::separateTokens(std::string &line) {
    std::vector<Token> tokens;
    int it = 0;
    while (it < line.size()) {
        if(isspace(line[it])){
            tokens.emplace_back(SPACE, " ");
            it++;
            while (isspace(line[it])) {
                it++;
            }
        }

        if (line[it] == '\'') {
            std::string bufor;
            it++;
            while (line[it] != '\'') {
                bufor.push_back(line[it]);
                it++;
            }
            tokens.emplace_back(QUOTATION, bufor);
            it++;
        } else if (line[it] == '|') {
            tokens.emplace_back(PIPE, "|");
            it++;
        } else if (line[it] == '&') {
            tokens.emplace_back(AMPERSAND, "&");
            it++;
        } else if (line[it] == ';') {
            tokens.emplace_back(SEMICOLON, ";");
            it++;
        } else if (line[it] == '<') {
            tokens.emplace_back(STREAM, "<");
            it++;
        } else if (line[it] == '>') {
            it++;
            if (line[it] == '>') {
                tokens.emplace_back(STREAM, ">>");
                it++;
            } else
                tokens.emplace_back(STREAM, ">");
        } else {
            std::string bufor;
            while (line[it] > '0' && line[it] < '9') {
                bufor.push_back(line[it++]);
            }
            if (!bufor.empty() && line[it] == '>') {
                bufor.push_back(line[it++]);
                if (line[it] == '>') {
                    bufor.push_back(line[it++]);
                }
                tokens.emplace_back(STREAM, bufor);
            } else if (!bufor.empty() && line[it] == '<') {
                bufor.push_back(line[it++]);
                tokens.emplace_back(STREAM, bufor);
            } else if (bufor.empty() && line[it] == '$') {
                it++;
                while (!is(line[it], {'|', '&', ';', '\'', '$', '<', '>', '='}) && !isspace(line[it]) &&
                       it < line.size()) {
                    bufor.push_back(line[it++]);
                }
                tokens.emplace_back(WITH$, bufor);
            } else {
                while (!is(line[it], {'|', '&', ';', '\'', '$', '<', '>'}) && !isspace(line[it]) && it < line.size()) {
                    if (line[it] == '=')
                        break;
                    bufor.push_back(line[it++]);
                }
                if (line[it] == '=') {
                    tokens.emplace_back(ASSIGNMENT, bufor);
                    it++;
                }
                else
                    tokens.emplace_back(WORD, bufor);
            }
        }
    }

    tokens.emplace_back(END, "end");

    return tokens;
}

std::vector<std::vector<Interpreter::Token>> Interpreter::separateInstruction(std::vector<Interpreter::Token> &tokens) {
    std::vector<std::vector<Interpreter::Token>> instructions;
    int it=0;
    while (it<tokens.size()){
        std::vector<Interpreter::Token> instruction;
        if(tokens[it].type == SPACE)
            it++;
        if(is(tokens[it].type, {SEMICOLON, PIPE, AMPERSAND})){
            throw(parserException(tokens[it].value));
        }
        while(!is(tokens[it].type, {SEMICOLON, PIPE, AMPERSAND, END})){
            if(tokens[it].type == STREAM){
                instruction.push_back(tokens[it++]);
                if(tokens[it].type == SPACE)
                    it++;
                if(!is(tokens[it].type, {WORD, QUOTATION, SPACE, ASSIGNMENT, WITH$})){
                    throw(parserException(tokens[it].value));
                }
            }
            instruction.push_back(tokens[it++]);
        }

        instruction.push_back(tokens[it++]);
        instructions.push_back(instruction);
    }
    return instructions;
}

Command::Command Interpreter::createCommand(std::vector<Interpreter::Token> &instruction) {
    Command command;
    int it=0;
    while(!is(instruction[it].type, {PIPE, AMPERSAND, SEMICOLON, END})){
        if (instruction[it].type == SPACE){
            it++;
        }
        else if(instruction[it].type == STREAM){
            std::string::size_type tmp;
            int number;
            std::string sign;
            try {
                number = std::stoi(instruction[it].value, &tmp);
                sign = instruction[it].value.substr(tmp);
            }
            catch (std::invalid_argument &){
                sign = instruction[it].value;
                if(sign != "<")
                    number = 1;
                else
                    number = 0;
            }
            it++;
            command.redirections.emplace_back(number, sign == "<", sign == ">>", concatenation(instruction, it));
        }
        else if(command.command.empty()){
            if (instruction[it].type == ASSIGNMENT){
                std::string tmp =instruction[it++].value;
                env.setVariable(tmp, concatenation(instruction, it));
            } else
                command.command = concatenation(instruction, it);
        } else if(command.command == "export"){
            if (instruction[it].type == ASSIGNMENT){
                command.args.push_back(instruction[it].value);
                env.setVariable(instruction[it++].value, concatenation(instruction, it));
            } else
                command.command = concatenation(instruction, it);
        }
        else {
            command.args.push_back(concatenation(instruction, it));
        }

    }
    if(instruction[it].type == PIPE){
        command.term = Command::PIPE;
    }
    else if(instruction[it].type == AMPERSAND){
        command.term = Command::AMPER;
    }
    return command;

}

Interpreter::Token::Token(Interpreter::TokenType type, const std::string &value) : type(type), value(value) {}

bool Interpreter::is(char sign, const std::initializer_list<char> &acceptable) const {
    for (auto &it: acceptable) {
        if (it == sign) {
            return true;
        }
    }
    return false;
}

bool Interpreter::is(TokenType type, const std::initializer_list<TokenType> &acceptable) const {
    for (auto &it: acceptable) {
        if (it == type) {
            return true;
        }
    }
    return false;
}

std::string Interpreter::concatenation(std::vector<Token> &instruction, int &it) {
    std::string tmp;
    while (is(instruction[it].type, {WORD, QUOTATION, ASSIGNMENT, WITH$})) {
        if (instruction[it].type == WITH$) {
            tmp += env.getValue(instruction[it].value);
        }else if (instruction[it].type == ASSIGNMENT) {
            tmp += instruction[it].value + "=";
        }         else {
            tmp += instruction[it].value;
        }
        it++;
    }
    return tmp;
}