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
                while (!checkSign(line[it], {'|', '&', ';', '\'', '$', '<', '>', '=', '/', ':', '{', '}', '@'}) && !isspace(line[it]) &&
                       it < line.size()) {
                    bufor.push_back(line[it++]);
                }
                tokens.emplace_back(VARIABLE, bufor);
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
/*
Token getNextToken(std::string &line, int it){
    Token token;
    if(tryWhitespace(line, it, token))
        return token;
    if(tryQuotation(line, it, token))
        return token;

}

bool tryQuotation(std::string &line, int it, Token &token){
    if (isspace(line[it])) {
        token.type = SPACE;
        it++;
        while (isspace(line[it])) {
            it++;
        }
        return true;
    }
    return false;
}

bool tryQuotation(std::string &line, int it, Token &token){
    if (line[it] == '\'') {
        std::string bufor;
        it++;
        while (line[it] != '\'') {
            bufor.push_back(line[it]);
            it++;
        }
        it++;
        token.type = QUOTATION;
        token.value = bufor;
        return true;
    }
    return false;
}

bool tryCommandEnd(std::string &line, int it, Token &token){
    if (line[it] == '|') {
        token.type = PIPE;
        it++;
        return true;
    }
    if (line[it] == '&') {
        token.type = AMPERSAND;
        it++;
        return true;
    }
    if (line[it] == ';') {
        token.type = SEMICOLON;
        it++;
        return true;
    }
    return false;
}


bool tryStream(std::string &line, int it, Token &token){
    if (line[it] == '<') {
        token.type = PIPE;
        token.value =
        it++;
        return true;
    } else if (line[it] == '>') {
        it++;
        if (line[it] == '>') {
            tokens.emplace_back(REDIRECTION, ">>");
            it++;
        } else
            tokens.emplace_back(REDIRECTION, ">");
    }
}*/

bool Lexer::checkSign(char sign, const std::initializer_list<char> &acceptable) const {
    for (auto &it: acceptable) {
        if (it == sign) {
            return true;
        }
    }
    return false;
}
