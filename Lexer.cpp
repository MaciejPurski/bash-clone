#include "Lexer.h"

std::vector<Token> Lexer::separateTokens(std::string &line) {
    std::vector<Token> tokens;
    int it = 0;
    while (it < line.size()) {
        if (isspace(line[it])) {
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
            tokens.emplace_back(REDIRECTION, "<");
            it++;
        } else if (line[it] == '>') {
            it++;
            if (line[it] == '>') {
                tokens.emplace_back(REDIRECTION, ">>");
                it++;
            } else
                tokens.emplace_back(REDIRECTION, ">");
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
                tokens.emplace_back(REDIRECTION, bufor);
            } else if (!bufor.empty() && line[it] == '<') {
                bufor.push_back(line[it++]);
                tokens.emplace_back(REDIRECTION, bufor);
            } else if (bufor.empty() && line[it] == '$') {
                it++;
                while (!checkSign(line[it], {'|', '&', ';', '\'', '$', '<', '>', '='}) && !isspace(line[it]) &&
                       it < line.size()) {
                    bufor.push_back(line[it++]);
                }
                tokens.emplace_back(WITH$, bufor);
            } else {
                while (!checkSign(line[it], {'|', '&', ';', '\'', '$', '<', '>'}) && !isspace(line[it]) && it < line.size()) {
                    if (line[it] == '=')
                        break;
                    bufor.push_back(line[it++]);
                }
                if (line[it] == '=') {
                    tokens.emplace_back(ASSIGNMENT, bufor);
                    it++;
                } else
                    tokens.emplace_back(WORD, bufor);
            }
        }
    }
    if (!(tokens.back().type == AMPERSAND || tokens.back().type == SEMICOLON))
        tokens.emplace_back(END, "end");

    return tokens;
}

bool Lexer::checkSign(char sign, const std::initializer_list<char> &acceptable) const {
    for (auto &it: acceptable) {
        if (it == sign) {
            return true;
        }
    }
    return false;
}
