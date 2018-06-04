#define BOOST_TEST_MODULE INTERPRETER_TEST
#include <boost/test/included/unit_test.hpp>
#include <exception>
#include "../Interpreter.h"

BOOST_AUTO_TEST_CASE( separateTokens1 )
{
    Lexer lexer;

    std::string line = "\'Test\'";

    std::vector<Token> tokens;
    tokens.emplace_back(QUOTATION, "Test");
    tokens.emplace_back(END, "end");

    BOOST_CHECK(lexer.separateTokens(line) == tokens);
}

BOOST_AUTO_TEST_CASE( separateTokens2 )
{
    Lexer lexer;

    std::string line = "pwd;";

    std::vector<Token> tokens;
    tokens.emplace_back(WORD, "pwd");
    tokens.emplace_back(SEMICOLON, ";");
    BOOST_CHECK(lexer.separateTokens(line) == tokens);
}

BOOST_AUTO_TEST_CASE( separateTokens3 )
{
    Lexer lexer;

    std::string line = "pwd > a;";

    std::vector<Token> tokens;
    tokens.emplace_back(WORD, "pwd");
    tokens.emplace_back(SPACE, " ");
    tokens.emplace_back(REDIRECTION, ">");
    tokens.emplace_back(SPACE, " ");
    tokens.emplace_back(WORD, "a");
    tokens.emplace_back(SEMICOLON, ";");

    BOOST_CHECK(lexer.separateTokens(line) == tokens);
}

BOOST_AUTO_TEST_CASE( separateTokens4 )
{
    Lexer lexer;

    std::string line = "cd 'a' |b";

    std::vector<Token> tokens;
    tokens.emplace_back(WORD, "cd");
    tokens.emplace_back(SPACE, " ");
    tokens.emplace_back(QUOTATION, "a");
    tokens.emplace_back(SPACE, " ");
    tokens.emplace_back(PIPE, "|");
    tokens.emplace_back(WORD, "b");
    tokens.emplace_back(END, "end");

    BOOST_CHECK(lexer.separateTokens(line) == tokens);
}


//BOOST_CHECK_THROW(env.searchPath(programName), std::exception);