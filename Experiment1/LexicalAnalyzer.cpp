#include<bits/stdc++.h>
using namespace std;

// Enum class to define different types of tokens
enum class TokenType {
    KEYWORD,
    IDENTIFIER,
    INTEGER_LITERAL,
    OPERATOR,
    PUNCTUATOR,
    UNKNOWN
};

// Struct to represent a token with its type and value
struct Token {
    TokenType type;
    string value;

    Token(TokenType t, const string& v)
        : type(t)
        , value(v)
        {}
};

// Class that implements the lexical analyzer
class LexicalAnalyzer {
private:
    string input;
    size_t position;
    unordered_map<string, TokenType> keywords;

    // Function to initialize the keywords map
    void initKeywords()
    {
        keywords["int"] = TokenType::KEYWORD;
        keywords["if"] = TokenType::KEYWORD;
        keywords["else"] = TokenType::KEYWORD;
        keywords["while"] = TokenType::KEYWORD;
        keywords["break"] = TokenType::KEYWORD;
        keywords["continue"] = TokenType::KEYWORD;
        keywords["return"] = TokenType::KEYWORD;
        keywords["void"] = TokenType::KEYWORD;
    }

    // Function to check if a character is whitespace
    bool isWhitespace(char c)
    {
        return c == ' ' || c == '\t' || c == '\n'
                || c == '\r';
    }

    // Function to check if a character is alphabetic
    bool isAlpha(char c)
    {
        return (c >= 'a' && c <= 'z')
                || (c >= 'A' && c <= 'Z');
    }

    // Function to check if a character is digit
    bool isDigit(char c)
    {
        return c >= '0' && c <= '9';
    }

    // Function to check if a character is alphanumeric
    bool isAlphaNumeric(char c)
    {
        return isAlpha(c) || isDigit(c);
    }

    // Function to check if a character is a operator
    bool isOperator(char c)
    {
        return c == '+' || c == '-' || c == '*' || c == '/' || c == '%'
                || c == '=' || c == '<' || c == '>' || c == '!'
                || c == '&' || c == '|';
    }

    // Function to check if a character is a punctuator
    bool isPunctuator(char c)
    {
        return c == '(' || c == ')' || c == '{' || c == '}'
                || c == ';';
    }

    // Function to get the next word
    string getNextWord()
    {
        size_t start = position;
        while(position < input.length()
               && isAlphaNumeric(input[position]))
        {
            position++;
        }
        return input.substr(start, position - start);
    }

    // Function to get the next number
    string getNextNumber()
    {
        size_t start = position;
        while(position < input.length() && isDigit(input[position]))
        {
            position++;
        }
        return input.substr(start, position - start);
    }

    // Function to get the next operator
    string getNextOperator()
    {
        size_t start = position;
        string two;
        if(start + 1 < input.length())
        {
            two = input.substr(start, 2);
            if(two == "==" || two == "!=" || two == "<="
               || two == ">=" || two == "&&" || two == "||")
               {
                position += 2;
                return two;
               }
        }
        position++;
        return input.substr(start, 1);
    }

    string getNextPunctuator()
    {
        size_t start = position;
        position++;
        return input.substr(start, 1);
    }

public: 
    // Constructor for LexicalAnalyzer
    LexicalAnalyzer(const string& source)
        : input(source)
        , position(0)
    {
        initKeywords();
    }

    // Function to tokenize the input string
    vector<Token> tokenize()
    {
        vector<Token> tokens;

        while(position < input.length())
        {
            char currentChar = input[position];

            // Skip whitespace
            if(isWhitespace(currentChar))
            {
                position++;
                continue;
            }

            // Identify keywords or identifiers
            if(isAlpha(currentChar))
            {
                string word = getNextWord();
                if(keywords.find(word) != keywords.end()) //identify keywords
                {
                    tokens.emplace_back(TokenType::KEYWORD, word);
                }
                else 
                {
                    tokens.emplace_back(TokenType::IDENTIFIER, word);
                }
            }
            else if(isDigit(currentChar)) // identify integer
            {
                string number = getNextNumber();
                tokens.emplace_back(TokenType::INTEGER_LITERAL, number);
            }
            else if(isOperator(currentChar))
            {
                string op = getNextOperator();
                tokens.emplace_back(TokenType::OPERATOR, op);
            }
            else if(isPunctuator(currentChar))
            {
                string punct = getNextPunctuator();
                tokens.emplace_back(TokenType::PUNCTUATOR, punct);
            }
            else // unknown character
            {
                tokens.emplace_back(TokenType::UNKNOWN, string(1, currentChar));
                position++;
            }
        }

        return tokens;
    }
};

// all functions below for print name
string getKeyWordName(const Token& token)
{
    return string("'") + token.value + "'";
}


string getOperatorName(const Token& token)
{
    return string("'") + token.value + "'";
}

string getPunctuatorName(const Token& token)
{
    return string("'") + token.value + "'";
}

// Function to convert TokenType to string for printing 
string getTokenTypeName(TokenType type, const Token& token)
{
    switch(type)
    {
        case TokenType::KEYWORD:
            return getKeyWordName(token);
        case TokenType::IDENTIFIER:
            return "Ident";
        case TokenType::INTEGER_LITERAL:
            return "IntConst";
        case TokenType::OPERATOR:
            return getOperatorName(token);
        case TokenType::PUNCTUATOR:
            return getPunctuatorName(token);
        case TokenType::UNKNOWN:
            return "Unknown";
        default:
            return "UNDEFINED";
    }
}

// Function to print all tokens
void printTokens(const vector<Token>& tokens)
{
    int count = 0;
    for(const auto& token : tokens)
    {
        cout << count++ <<":" << getTokenTypeName(token.type, token)
        <<":"<<"\"" << token.value << "\"" << endl;
    }
}

int main()
{
    string sourceCode;
    std::ostringstream ss;
    ss << std::cin.rdbuf();
    sourceCode = ss.str();

    LexicalAnalyzer lexer(sourceCode);

    vector<Token> tokens = lexer.tokenize();

    //cout << "Tokens Generate by Lexical Analyzer:" << endl;
    printTokens(tokens);

    return 0;
}