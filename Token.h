#ifndef BASH_CLONE_TOKEN_H
#define BASH_CLONE_TOKEN_H
#include <vector>
#include <string>
#include <ostream>

/**
 *	@enum TokenType
 *
 *	@brief Represents type of token
 */
enum TokenType {
    QUOTATION, PIPE, AMPERSAND, SEMICOLON, END, REDIRECTION, SPACE, ASSIGNMENT, WITH$, WORD, NONE
};

/**
 *	@struct Token
 *
 *	@brief Defines token, holds type od token and value
 */
struct Token {
    Token(TokenType type, const std::string &value) : type(type), value(value) {}

    Token() {
        type = NONE;
        value = "";
    }

    bool operator==(const Token &rhs) const {
        return type == rhs.type &&
               value == rhs.value;
    }

    bool operator!=(const Token &rhs) const {
        return !(rhs == *this);
    }

    friend std::ostream &operator<<(std::ostream &os, const Token &token) {
        os << "type: " << token.type << " value: " << token.value;
        return os;
    }

    TokenType type;
    std::string value;
};

#endif //BASH_CLONE_TOKEN_H
