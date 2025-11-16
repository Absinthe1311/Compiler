#include<bits/stdc++.h>
using namespace std;

// Enum class to define different types of tokens
enum class TokenType {
    KEYWORD,
    IDENTIFIER,
    INTEGER_LITERAL,
    OPERATOR,
    PUNCTUATOR,
    UNKNOWN,
    END_OF_FILE
};

// Struct to represent a token with its type and value
struct Token {
    TokenType type;
    string value;
    int line;

    Token(TokenType t, const string& v, int l = 0)
        : type(t)
        , value(v)
        , line(l)
        {}
};

// Class that implements the lexical analyzer
class LexicalAnalyzer {
private:
    string input;
    size_t position;
    int currentLine;
    unordered_map<string, TokenType> keywords;

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

    bool isWhitespace(char c)
    {
        return c == ' ' || c == '\t' || c == '\n' || c == '\r';
    }

    bool isAlpha(char c)
    {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }

    bool isDigit(char c)
    {
        return c >= '0' && c <= '9';
    }

    bool isAlphaNumeric(char c)
    {
        return isAlpha(c) || isDigit(c);
    }

    bool isOperator(char c)
    {
        return c == '+' || c == '-' || c == '*' || c == '/' || c == '%'
                || c == '=' || c == '<' || c == '>' || c == '!'
                || c == '&' || c == '|';
    }

    bool isPunctuator(char c)
    {
        return c == '(' || c == ')' || c == '{' || c == '}'
                || c == ';' || c == ',';
    }
    
    bool isUnderscore(char c)
    {
        return c == '_';
    }

    void skipLineComment()
    {
        if(position +1 < input.length() && input[position] == '/' && input[position+1] == '/')
            position += 2;
        while(position < input.length() && input[position] != '\n')
            position++;
        if(position < input.length() && input[position] == '\n')
        {
            position++;
            currentLine++;
        }
    }

    void skipBlockComment()
    {
        if(position+1 < input.length() && input[position] == '/' && input[position+1] == '*')
            position += 2;
        while(position+1 < input.length())
        {
            if(input[position] == '\n')
                currentLine++;
            if(input[position] == '*' && input[position+1] == '/')
            {
                position += 2;
                return;
            }
            position++;
        }
        position = input.length();
    }

    string getNextWord()
    {
        size_t start = position;
        while(position < input.length()
               && (isAlphaNumeric(input[position]) || isUnderscore(input[position])))
        {
            position++;
        }
        return input.substr(start, position - start);
    }

    string getNextNumber()
    {
        size_t start = position;
        while(position < input.length() && isDigit(input[position]))
        {
            position++;
        }
        return input.substr(start, position - start);
    }

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
    LexicalAnalyzer(const string& source)
        : input(source)
        , position(0)
        , currentLine(1)
    {
        initKeywords();
    }

    vector<Token> tokenize()
    {
        vector<Token> tokens;

        while(position < input.length())
        {
            char currentChar = input[position];

            if(isWhitespace(currentChar))
            {
                if(currentChar == '\n')
                    currentLine++;
                position++;
                continue;
            }

            if(isAlpha(currentChar) || isUnderscore(currentChar))
            {
                string word = getNextWord();
                if(keywords.find(word) != keywords.end())
                {
                    tokens.emplace_back(TokenType::KEYWORD, word, currentLine);
                }
                else 
                {
                    tokens.emplace_back(TokenType::IDENTIFIER, word, currentLine);
                }
            }
            else if(isDigit(currentChar))
            {
                string number = getNextNumber();
                tokens.emplace_back(TokenType::INTEGER_LITERAL, number, currentLine);
            }
            else if(currentChar == '/')
            {
                if(position+1 < input.length() && input[position+1] == '/')
                {
                    skipLineComment();
                    continue;
                }
                else if(position + 1 < input.length() && input[position+1] == '*')
                {
                    skipBlockComment();
                    continue;
                }
                else
                {
                    string op = getNextOperator();
                    tokens.emplace_back(TokenType::OPERATOR, op, currentLine);
                }
            }
            else if(isOperator(currentChar))
            {
                string op = getNextOperator(); 
                tokens.emplace_back(TokenType::OPERATOR, op, currentLine);
            }
            else if(isPunctuator(currentChar))
            {
                string punct = getNextPunctuator();
                tokens.emplace_back(TokenType::PUNCTUATOR, punct, currentLine);
            }
            else
            {
                tokens.emplace_back(TokenType::UNKNOWN, string(1, currentChar), currentLine);
                position++;
            }
        }

        tokens.emplace_back(TokenType::END_OF_FILE, "", currentLine);
        return tokens;
    }
};

// Parser class
class Parser {
private:
    vector<Token> tokens;
    size_t pos;
    set<int> errorLines;
    int loopDepth;
    bool hasError;

    Token getCurrentToken() {
        if (pos >= tokens.size()) return tokens.back();
        return tokens[pos];
    }


    void advance() {
        if (pos < tokens.size()) pos++;
    }

    void error() {
        hasError = true;
        int line = getCurrentToken().line;
        errorLines.insert(line);
    }

    bool match(TokenType type, const string& value = "") {
        if (getCurrentToken().type != type) return false;
        if (!value.empty() && getCurrentToken().value != value) return false;
        return true;
    }

    bool consume(TokenType type, const string& value) {
        if (match(type, value)) {
            advance();
            return true;
        }
        error();
        return false;
    }

    bool consume(TokenType type) {
        if (current().type == type) {
            advance();
            return true;
        }
        error();
        return false;
    }

    void sync() {
        while (current().type != TokenType::END_OF_FILE && 
               !(current().type == TokenType::PUNCTUATOR && current().value == ";") &&
               !(current().type == TokenType::PUNCTUATOR && current().value == "}")) {
            advance();
        }
        if (current().type == TokenType::PUNCTUATOR && current().value == ";") 
            advance();
    }

    void parseCompUnit() {
        while (current().type != TokenType::END_OF_FILE) {
            parseFuncDef();
        }
    }

    void parseFuncDef() {
        if (!match(TokenType::KEYWORD, "int") && !match(TokenType::KEYWORD, "void")) {
            error();
            sync();
            if (current().type == TokenType::PUNCTUATOR && current().value == "}") 
                advance();
            return;
        }
        advance();

        if (!consume(TokenType::IDENTIFIER)) {
            sync();
            if (current().type == TokenType::PUNCTUATOR && current().value == "}") 
                advance();
            return;
        }

        consume(TokenType::PUNCTUATOR, "(");

        if (match(TokenType::KEYWORD, "int")) {
            parseParam();
            while (current().type == TokenType::PUNCTUATOR && current().value == ",") {
                advance();
                parseParam();
            }
        }

        consume(TokenType::PUNCTUATOR, ")");
        parseBlock();
    }

    void parseParam() {
        consume(TokenType::KEYWORD, "int");
        consume(TokenType::IDENTIFIER);
    }

    void parseBlock() {
        if (!consume(TokenType::PUNCTUATOR, "{")) {
            return;
        }

        while (!(current().type == TokenType::PUNCTUATOR && current().value == "}") && 
               current().type != TokenType::END_OF_FILE) {
            parseStmt();
        }

        consume(TokenType::PUNCTUATOR, "}");
    }

    void parseStmt() {
        if (match(TokenType::KEYWORD, "int")) {
            advance();
            consume(TokenType::IDENTIFIER);
            if (current().type == TokenType::OPERATOR && current().value == "=") {
                advance();
                parseExpr();
            }
            while (current().type == TokenType::PUNCTUATOR && current().value == ",") {
                advance();
                consume(TokenType::IDENTIFIER);
                if (current().type == TokenType::OPERATOR && current().value == "=") {
                    advance();
                    parseExpr();
                }
            }
            consume(TokenType::PUNCTUATOR, ";");
        } else if (match(TokenType::KEYWORD, "if")) {
            advance();
            consume(TokenType::PUNCTUATOR, "(");
            parseExpr();
            consume(TokenType::PUNCTUATOR, ")");
            parseStmt();
            if (match(TokenType::KEYWORD, "else")) {
                advance();
                parseStmt();
            }
        } else if (match(TokenType::KEYWORD, "while")) {
            advance();
            consume(TokenType::PUNCTUATOR, "(");
            parseExpr();
            consume(TokenType::PUNCTUATOR, ")");
            loopDepth++;
            parseStmt();
            loopDepth--;
        } else if (match(TokenType::KEYWORD, "break")) {
            advance();
            consume(TokenType::PUNCTUATOR, ";");
        } else if (match(TokenType::KEYWORD, "continue")) {
            advance();
            consume(TokenType::PUNCTUATOR, ";");
        } else if (match(TokenType::KEYWORD, "return")) {
            advance();
            if (!(current().type == TokenType::PUNCTUATOR && current().value == ";")) {
                parseExpr();
            }
            consume(TokenType::PUNCTUATOR, ";");
        } else if (current().type == TokenType::PUNCTUATOR && current().value == "{") {
            parseBlock();
        } else if (current().type == TokenType::IDENTIFIER) {
            advance();

            if (current().type == TokenType::OPERATOR && current().value == "=") {
                advance();
                parseExpr();
                consume(TokenType::PUNCTUATOR, ";");
            } else if (current().type == TokenType::PUNCTUATOR && current().value == "(") {
                advance();
                
                if (!(current().type == TokenType::PUNCTUATOR && current().value == ")")) {
                    parseExpr();
                    while (current().type == TokenType::PUNCTUATOR && current().value == ",") {
                        advance();
                        parseExpr();
                    }
                }
                consume(TokenType::PUNCTUATOR, ")");
                consume(TokenType::PUNCTUATOR, ";");
            } else {
                consume(TokenType::PUNCTUATOR, ";");
            }
        } else if (current().type == TokenType::PUNCTUATOR && current().value == ";") {
            advance();
        } else {
            error();
            advance();
        }
    }

    void parseExpr() {
        parseLOrExpr();
    }

    void parseLOrExpr() {
        parseLAndExpr();
        while (current().type == TokenType::OPERATOR && current().value == "||") {
            advance();
            parseLAndExpr();
        }
    }

    void parseLAndExpr() {
        parseRelExpr();
        while (current().type == TokenType::OPERATOR && current().value == "&&") {
            advance();
            parseRelExpr();
        }
    }

    void parseRelExpr() {
        parseAddExpr();
        while (current().type == TokenType::OPERATOR && 
               (current().value == "<" || current().value == "<=" || 
                current().value == ">" || current().value == ">=" || 
                current().value == "==" || current().value == "!=")) {
            advance();
            parseAddExpr();
        }
    }

    void parseAddExpr() {
        parseMulExpr();
        while (current().type == TokenType::OPERATOR && 
               (current().value == "+" || current().value == "-")) {
            advance();
            parseMulExpr();
        }
    }

    void parseMulExpr() {
        parseUnaryExpr();
        while (current().type == TokenType::OPERATOR && 
               (current().value == "*" || current().value == "/" || current().value == "%")) {
            advance();
            parseUnaryExpr();
        }
    }

    void parseUnaryExpr() {
        if (current().type == TokenType::OPERATOR && 
            (current().value == "+" || current().value == "-" || current().value == "!")) {
            advance();
            parseUnaryExpr();
        } else {
            parsePrimaryExpr();
        }
    }

    void parsePrimaryExpr() {
        if (current().type == TokenType::IDENTIFIER) {
            advance();
            if (current().type == TokenType::PUNCTUATOR && current().value == "(") {
                advance();
                if (!(current().type == TokenType::PUNCTUATOR && current().value == ")")) {
                    parseExpr();
                    while (current().type == TokenType::PUNCTUATOR && current().value == ",") {
                        advance();
                        parseExpr();
                    }
                }
                consume(TokenType::PUNCTUATOR, ")", "Lack of ')'");
            }
        } else if (current().type == TokenType::INTEGER_LITERAL) {
            advance();
        } else if (current().type == TokenType::PUNCTUATOR && current().value == "(") {
            advance();
            parseExpr();
            consume(TokenType::PUNCTUATOR, ")", "Lack of ')'");
        } else {
            error("Expected expression");
            if (current().type != TokenType::END_OF_FILE && 
                !(current().type == TokenType::PUNCTUATOR && current().value == ";")) {
                advance();
            }
        }
    }

public:
    Parser(const vector<Token>& toks) : tokens(toks), pos(0), loopDepth(0), hasError(false) {}

    bool parse() {
        parseCompUnit();
        return !hasError;
    }

    map<int, string> getErrors() { return errors; }
};

int main() {
    string input, line;
    while (getline(cin, line)) {
        input += line + "\n";
    }

    LexicalAnalyzer lexer(input);
    vector<Token> tokens = lexer.tokenize();

    Parser parser(tokens);
    bool success = parser.parse();
    auto parseErrors = parser.getErrors();

    if (parseErrors.empty()) {
        cout << "accept" << endl;
    } else {
        cout << "reject" << endl;
        for (const auto& e : parseErrors) {
            cout << e.first << " " << e.second << endl;
        }
    }

    return 0;
}