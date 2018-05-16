#include "Interpreter.h"

void Interpreter::nextCommandLine(std::string &line) {
    std::vector<Token> tokens = separateTokens(line);
    for(auto &it: tokens){
        std::cout<<it.type<<"  "<<it.value<<std::endl;
    }
}

std::vector<Interpreter::Token> Interpreter::separateTokens(std::string &line) {
    std::vector<Token> tokens;
    int it = 0;
    while (it < line.size()) {
        int tmp = it;
        while (isspace(line[it])) {
            it++;
        }
        if (tmp != it) {
            tokens.emplace_back(SPACE, " ");
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
                bufor.push_back(line[it]);
                it++;
            }
            if (!bufor.empty() && line[it] == '>') {
                bufor.push_back(line[it]);
                it++;
                if (line[it] == '>') {
                    bufor.push_back(line[it]);
                    it++;
                }
                tokens.emplace_back(STREAM, bufor);
            } else if (bufor.empty() && line[it] == '$') {
                it++;
                while (!is(line[it], {'|', '&', ';', '\'', '$', '<', '>', '='}) && !isspace(line[it]) &&
                       it < line.size()) {
                    bufor.push_back(line[it]);
                    it++;
                }
                tokens.emplace_back(WITH$, bufor);
            } else {
                while (!is(line[it], {'|', '&', ';', '\'', '$', '<', '>'}) && !isspace(line[it]) && it < line.size()) {
                    if (line[it] == '=')
                        break;
                    bufor.push_back(line[it]);
                    it++;
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
        if(tokens[it].type == SEMICOLON || tokens[it].type == PIPE || tokens[it].type == AMPERSAND){
            throw("Nieoczekiwany znacznik:" + tokens[it].value);
        }
        while(tokens[it].type != SEMICOLON && tokens[it].type != PIPE && tokens[it].type != AMPERSAND && tokens[it].type != END){
            if(tokens[it].type == STREAM){
                instruction.push_back(tokens[it]);
                it++;
                if(tokens[it].type == SPACE)
                    it++;
                if(!is(tokens[it].type, {WORD, QUOTATION, SPACE, ASSIGNMENT, WITH$})){
                    throw("Nieoczekiwany znacznik:" + tokens[it].value);
                }
            }
            instruction.push_back(tokens[it]);
            it++;
        }

        instructions.push_back(instruction);
    }
    return instructions;
}

void Interpreter::interpretInstruction(std::vector<Interpreter::Token> &instruction) {

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