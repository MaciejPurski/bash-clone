#include "Interpreter.h"

void Interpreter::nextCommandLine(std::string &line) {
	//
}

std::vector<Interpreter::Token> Interpreter::separateTokens(std::string &line) {
    std::vector<Token> tokens;
    int it=0;
    while(it<line.size()){
        int tmp = it;
        while (isspace(line[it]))
        {
            it++;
        }
        if(tmp != it){
            tokens.emplace_back(SPACE, " ");
        }

        if(line[it] == '\''){
            std::string bufor;
            while (line[it] != '\'')
            {
                bufor.push_back(line[it]);
                it++;
            }
            tokens.emplace_back(QUOTATION, bufor);
            it++;
        }
        else  if(line[it] == '|') {
            tokens.emplace_back(PIPE, "|");
            it++;
        }
        else  if(line[it] == '&') {
            tokens.emplace_back(AMPERSAND, "&");
            it++;
        }
        else  if(line[it] == ';') {
            tokens.emplace_back(SEMICOLON, ";");
            it++;
        }
        else if (line[it] == '<'){
            tokens.emplace_back(STREAM, "<");
            it++;
        }
        else if (line[it] == '>'){
            it++;
            if(line[it] == '>'){
                tokens.emplace_back(STREAM, ">>");
                it++;
            }
            else
                tokens.emplace_back(STREAM, ">");
        }
        else{
            std::string bufor;
            while(line[it] > '0' && line[it] < '9'){
                bufor.push_back(line[it]);
                it++;
            }
            if(!bufor.empty() && line[it] == '>'){
                bufor.push_back(line[it]);
                it++;
                if(line[it] == '>'){
                bufor.push_back(line[it]);
                    it++;
                }
                tokens.emplace_back(STREAM, bufor);
            }
            else if(bufor.empty() && line[it] == '$'){

            }


        }
    }



    return std::vector<Interpreter::Token>();
}

std::vector<std::vector<Interpreter::Token>> Interpreter::separateInstruction(std::vector<Interpreter::Token> &tokens) {
    return std::vector<std::vector<Interpreter::Token>>();
}

void Interpreter::interpretInstruction(std::vector<Interpreter::Token> &instruction) {

}

Interpreter::Token::Token(Interpreter::TokenType type, const std::string &value) : type(type), value(value) {}
