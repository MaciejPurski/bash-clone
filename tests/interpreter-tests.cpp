#define BOOST_TEST_MODULE INTERPRETER_TEST
#include <boost/test/included/unit_test.hpp>
#include <exception>
#include "../Interpreter.h"
#include "../BashExceptions.h"

BOOST_AUTO_TEST_SUITE( INTERPRETER_TEST_SUITE)
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

BOOST_AUTO_TEST_CASE( sepatateTokens5 )
{
    Lexer lexer;

    std::string line = "export a=b c";

    std::vector<Token> tokens;
    //export a=b c
    tokens.emplace_back(WORD, "export");
    tokens.emplace_back(SPACE, " ");
    tokens.emplace_back(ASSIGNMENT, "a");
    tokens.emplace_back(WORD, "b");
    tokens.emplace_back(SPACE, " ");
    tokens.emplace_back(WORD, "c");
    tokens.emplace_back(END, "end");

    BOOST_CHECK(lexer.separateTokens(line) == tokens);
}

BOOST_AUTO_TEST_CASE( parser1 )
{
    Environment env;
    Parser parser(env);

    std::vector<Token> tokens;
    tokens.emplace_back(WORD, "cd");
    tokens.emplace_back(SPACE, " ");
    tokens.emplace_back(QUOTATION, "a");
    tokens.emplace_back(SPACE, " ");
    tokens.emplace_back(PIPE, "|");
    tokens.emplace_back(WORD, "b");
    tokens.emplace_back(END, "end");

    Command command1;
    command1.command = "cd";
    command1.args.emplace_back("a");
    command1.term = Command::PIPE;
    Command command2;
    command2.command = "b";
    std::vector<Command> commands;
    commands.push_back(command1);
    commands.push_back(command2);

    BOOST_CHECK(parser.separateCommands(tokens) == commands);
}

BOOST_AUTO_TEST_CASE( parser2 )
{
    Environment env;
    Parser parser(env);

    std::vector<Token> tokens;
    //export a=b c
    tokens.emplace_back(WORD, "export");
    tokens.emplace_back(SPACE, " ");
    tokens.emplace_back(ASSIGNMENT, "a");
    tokens.emplace_back(WORD, "b");
    tokens.emplace_back(SPACE, " ");
    tokens.emplace_back(WORD, "c");
    tokens.emplace_back(END, "end");

    Command command1;
    command1.command = "export";
    command1.args.emplace_back("a");
    command1.args.emplace_back("c");
    std::vector<Command> commands;
    commands.push_back(command1);
    BOOST_CHECK(parser.separateCommands(tokens) == commands);
}

BOOST_AUTO_TEST_CASE( parser3 )
{
    Environment env;
    Parser parser(env);

    std::vector<Token> tokens;
    tokens.emplace_back(WORD, "cd");
    tokens.emplace_back(SEMICOLON, ";");
    tokens.emplace_back(AMPERSAND, "&");

    BOOST_CHECK_THROW(parser.separateCommands(tokens), InterpreterException);
}

BOOST_AUTO_TEST_CASE( parser4 )
{
    Environment env;
    Parser parser(env);

    std::vector<Token> tokens;
    tokens.emplace_back(WORD, "ls");
    tokens.emplace_back(REDIRECTION, ">>");
    tokens.emplace_back(SPACE, " ");
    tokens.emplace_back(REDIRECTION, "2>");
    tokens.emplace_back(WORD, "a");

    BOOST_CHECK_THROW(parser.separateCommands(tokens), InterpreterException);
}

BOOST_AUTO_TEST_CASE( parser5 )
{
    Environment env;
    Parser parser(env);

    std::vector<Token> tokens;

    tokens.emplace_back(WORD, "ls");
    tokens.emplace_back(REDIRECTION, ">>");
    tokens.emplace_back(SPACE, " ");
    tokens.emplace_back(WORD, "a");
    tokens.emplace_back(END, "end");

    Command command1;
    command1.command = "ls";
    command1.redirections.emplace_back(1, false, true, "a");
    std::vector<Command> commands;
    commands.push_back(command1);
    BOOST_CHECK(parser.separateCommands(tokens) == commands);
}

BOOST_AUTO_TEST_SUITE_END()