#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <sstream>
#include <set>
#include <memory>

using namespace std;

// Enum class to define different types of tokens
enum class TokenType {
    KEYWORD,
    IDENTIFIER,
    INTEGER_LITERAL,
    OPERATOR,
    PUNCTUATOR,
    UNKNOWN,
    END
};

// Struct to represent a token with its type and value
struct Token {
    TokenType type;
    string value;
    int line;

    Token(TokenType t, const string& v, int l = 0)
        : type(t), value(v), line(l) {}
};

// Class that implements the lexical analyzer
class LexicalAnalyzer {
private:
    string input;
    size_t position;
    int currentLine;
    unordered_map<string, TokenType> keywords;

    // Function to initialize the keywords map
    void initKeywords() {
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
    bool isWhitespace(char c) {
        return c == ' ' || c == '\t' || c == '\n' || c == '\r';
    }

    // Function to check if a character is alphabetic
    bool isAlpha(char c) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }

    // Function to check if a character is digit
    bool isDigit(char c) {
        return c >= '0' && c <= '9';
    }

    // Function to check if a character is alphanumeric
    bool isAlphaNumeric(char c) {
        return isAlpha(c) || isDigit(c);
    }

    // Function to check if a character is a operator
    bool isOperator(char c) {
        return c == '+' || c == '-' || c == '*' || c == '/' || c == '%' ||
               c == '=' || c == '<' || c == '>' || c == '!' ||
               c == '&' || c == '|';
    }

    // Function to check if a character is a punctuator
    bool isPunctuator(char c) {
        return c == '(' || c == ')' || c == '{' || c == '}' ||
               c == ';' || c == ',';
    }
    
    // Function to check if a character is a underscore
    bool isUnderscore(char c) {
        return c == '_';
    }

    // Function to skip LineComment
    void skipLineComment() {
        if (position + 1 < input.length() && input[position] == '/' && input[position + 1] == '/') {
            position += 2;
        }
        while (position < input.length() && input[position] != '\n') {
            position++;
        }
        if (position < input.length() && input[position] == '\n') {
            currentLine++;
            position++;
        }
    }

    // Function to skip BlockComment
    void skipBlockComment() {
        if (position + 1 < input.length() && input[position] == '/' && input[position + 1] == '*') {
            position += 2;
        }
        while (position + 1 < input.length()) {
            if (input[position] == '\n') {
                currentLine++;
            }
            if (input[position] == '*' && input[position + 1] == '/') {
                position += 2;
                return;
            }
            position++;
        }
        position = input.length();
    }

    // Function to get the next word
    string getNextWord() {
        size_t start = position;
        while (position < input.length() &&
               (isAlphaNumeric(input[position]) || isUnderscore(input[position]))) {
            position++;
        }
        return input.substr(start, position - start);
    }

    // Function to get the next number
    string getNextNumber() {
        size_t start = position;
        while (position < input.length() && isDigit(input[position])) {
            position++;
        }
        return input.substr(start, position - start);
    }

    // Function to get the next operator
    string getNextOperator() {
        size_t start = position;
        string two;
        if (start + 1 < input.length()) {
            two = input.substr(start, 2);
            if (two == "==" || two == "!=" || two == "<=" ||
                two == ">=" || two == "&&" || two == "||") {
                position += 2;
                return two;
            }
        }
        position++;
        return input.substr(start, 1);
    }

    string getNextPunctuator() {
        size_t start = position;
        position++;
        return input.substr(start, 1);
    }

public: 
    // Constructor for LexicalAnalyzer
    LexicalAnalyzer(const string& source)
        : input(source), position(0), currentLine(1) {
        initKeywords();
    }

    // Function to tokenize the input string
    vector<Token> tokenize() {
        vector<Token> tokens;

        while (position < input.length()) {
            char currentChar = input[position];

            // Skip whitespace
            if (isWhitespace(currentChar)) {
                if (currentChar == '\n') {
                    currentLine++;
                }
                position++;
                continue;
            }

            // Identify keywords or identifiers
            if (isAlpha(currentChar) || isUnderscore(currentChar)) {
                string word = getNextWord();
                if (keywords.find(word) != keywords.end()) {
                    tokens.emplace_back(TokenType::KEYWORD, word, currentLine);
                } else {
                    tokens.emplace_back(TokenType::IDENTIFIER, word, currentLine);
                }
            } else if (isDigit(currentChar)) {
                string number = getNextNumber();
                tokens.emplace_back(TokenType::INTEGER_LITERAL, number, currentLine);
            } else if (currentChar == '/') {
                if (position + 1 < input.length() && input[position + 1] == '/') {
                    skipLineComment();
                    continue;
                } else if (position + 1 < input.length() && input[position + 1] == '*') {
                    skipBlockComment();
                    continue;
                } else {
                    string op = getNextOperator();
                    tokens.emplace_back(TokenType::OPERATOR, op, currentLine);
                }
            } else if (isOperator(currentChar)) {
                string op = getNextOperator(); 
                tokens.emplace_back(TokenType::OPERATOR, op, currentLine);
            } else if (isPunctuator(currentChar)) {
                string punct = getNextPunctuator();
                tokens.emplace_back(TokenType::PUNCTUATOR, punct, currentLine);
            } else {
                tokens.emplace_back(TokenType::UNKNOWN, string(1, currentChar), currentLine);
                position++;
            }
        }

        tokens.emplace_back(TokenType::END, "", currentLine);
        return tokens;
    }
};

// Syntax Analyzer (Parser)
class Parser {
private:
    vector<Token> tokens;
    size_t current;
    set<int> errorLines;
    bool hasError;

    Token& currentToken() {
        return tokens[current];
    }

    Token& peek() {
        if (current + 1 < tokens.size()) {
            return tokens[current + 1];
        }
        return tokens.back(); // END token
    }

    bool match(TokenType type, const string& value = "") {
        if (currentToken().type == type && 
            (value.empty() || currentToken().value == value)) {
            current++;
            return true;
        }
        return false;
    }

    bool check(TokenType type, const string& value = "") {
        return currentToken().type == type && 
               (value.empty() || currentToken().value == value);
    }

    void error(const string& message) {
        errorLines.insert(currentToken().line);
        hasError = true;
    }

    void synchronize() {
        // Skip tokens until we find a statement boundary
        while (!isAtEnd()) {
            if (previous().value == ";") return;
            if (check(TokenType::KEYWORD, "int") || 
                check(TokenType::KEYWORD, "void") ||
                check(TokenType::KEYWORD, "if") ||
                check(TokenType::KEYWORD, "while") ||
                check(TokenType::KEYWORD, "return") ||
                check(TokenType::KEYWORD, "break") ||
                check(TokenType::KEYWORD, "continue") ||
                check(TokenType::PUNCTUATOR, "}")) {
                return;
            }
            advance();
        }
    }

    Token advance() {
        if (!isAtEnd()) current++;
        return previous();
    }

    Token previous() {
        return tokens[current - 1];
    }

    bool isAtEnd() {
        return currentToken().type == TokenType::END;
    }

public:
    Parser(const vector<Token>& t) : tokens(t), current(0), hasError(false) {}

    // CompUnit → FuncDef+
    void compUnit() {
        if (!funcDef()) {
            error("Expected function definition");
            return;
        }
        
        while (funcDef()) {
            // Continue parsing function definitions
        }
        
        if (!isAtEnd()) {
            error("Unexpected tokens after compilation unit");
        }
    }

    // FuncDef → ("int" | "void") ID "(" (Param ("," Param)*)? ")" Block
    bool funcDef() {
        if (!check(TokenType::KEYWORD, "int") && !check(TokenType::KEYWORD, "void")) {
            return false;
        }

        advance(); // consume type
        
        if (!match(TokenType::IDENTIFIER)) {
            error("Expected function name");
            synchronize();
            return false;
        }
        
        if (!match(TokenType::PUNCTUATOR, "(")) {
            error("Expected '(' after function name");
            synchronize();
            return false;
        }
        
        // Parse parameters
        if (!check(TokenType::PUNCTUATOR, ")")) {
            if (!param()) {
                error("Expected parameter");
                synchronize();
                return false;
            }
            
            while (match(TokenType::PUNCTUATOR, ",")) {
                if (!param()) {
                    error("Expected parameter after ','");
                    synchronize();
                    return false;
                }
            }
        }
        
        if (!match(TokenType::PUNCTUATOR, ")")) {
            error("Expected ')' after parameters");
            synchronize();
            return false;
        }
        
        if (!block()) {
            error("Expected function body");
            synchronize();
            return false;
        }
        
        return true;
    }

    // Param → "int" ID
    bool param() {
        if (!match(TokenType::KEYWORD, "int")) {
            return false;
        }
        
        if (!match(TokenType::IDENTIFIER)) {
            error("Expected parameter name");
            return false;
        }
        
        return true;
    }

    // Block → "{" Stmt* "}"
    bool block() {
        if (!match(TokenType::PUNCTUATOR, "{")) {
            return false;
        }
        
        while (stmt()) {
            // Parse statements
        }
        
        if (!match(TokenType::PUNCTUATOR, "}")) {
            error("Expected '}' to close block");
            synchronize();
            return false;
        }
        
        return true;
    }

    // Stmt → Block | ";" | Expr ";" | ID "=" Expr ";" 
    //        | "int" ID "=" Expr ";"
    //        | "if" "(" Expr ")" Stmt ("else" Stmt)?
    //        | "while" "(" Expr ")" Stmt
    //        | "break" ";" | "continue" ";" | "return" Expr ";"
    bool stmt() {
        // Block
        if (check(TokenType::PUNCTUATOR, "{")) {
            return block();
        }
        
        // Empty statement
        if (match(TokenType::PUNCTUATOR, ";")) {
            return true;
        }
        
        // Variable declaration
        if (match(TokenType::KEYWORD, "int")) {
            if (!match(TokenType::IDENTIFIER)) {
                error("Expected variable name");
                synchronize();
                return false;
            }
            
            if (match(TokenType::OPERATOR, "=")) {
                if (!expr()) {
                    error("Expected expression after '='");
                    synchronize();
                    return false;
                }
            }
            
            if (!match(TokenType::PUNCTUATOR, ";")) {
                error("Expected ';' after variable declaration");
                synchronize();
                return false;
            }
            
            return true;
        }
        
        // Control statements
        if (match(TokenType::KEYWORD, "if")) {
            if (!match(TokenType::PUNCTUATOR, "(")) {
                error("Expected '(' after 'if'");
                synchronize();
                return false;
            }
            
            if (!expr()) {
                error("Expected condition expression");
                synchronize();
                return false;
            }
            
            if (!match(TokenType::PUNCTUATOR, ")")) {
                error("Expected ')' after condition");
                synchronize();
                return false;
            }
            
            if (!stmt()) {
                error("Expected statement after if condition");
                synchronize();
                return false;
            }
            
            if (match(TokenType::KEYWORD, "else")) {
                if (!stmt()) {
                    error("Expected statement after 'else'");
                    synchronize();
                    return false;
                }
            }
            
            return true;
        }
        
        if (match(TokenType::KEYWORD, "while")) {
            if (!match(TokenType::PUNCTUATOR, "(")) {
                error("Expected '(' after 'while'");
                synchronize();
                return false;
            }
            
            if (!expr()) {
                error("Expected condition expression");
                synchronize();
                return false;
            }
            
            if (!match(TokenType::PUNCTUATOR, ")")) {
                error("Expected ')' after condition");
                synchronize();
                return false;
            }
            
            if (!stmt()) {
                error("Expected statement after while condition");
                synchronize();
                return false;
            }
            
            return true;
        }
        
        if (match(TokenType::KEYWORD, "break") || match(TokenType::KEYWORD, "continue")) {
            if (!match(TokenType::PUNCTUATOR, ";")) {
                error("Expected ';' after break/continue");
                synchronize();
                return false;
            }
            return true;
        }
        
        if (match(TokenType::KEYWORD, "return")) {
            if (!expr()) {
                error("Expected expression after 'return'");
                synchronize();
                return false;
            }
            
            if (!match(TokenType::PUNCTUATOR, ";")) {
                error("Expected ';' after return expression");
                synchronize();
                return false;
            }
            return true;
        }
        
        // Assignment or expression statement
        if (check(TokenType::IDENTIFIER)) {
            Token id = advance();
            
            // Assignment
            if (match(TokenType::OPERATOR, "=")) {
                if (!expr()) {
                    error("Expected expression after '='");
                    synchronize();
                    return false;
                }
            } 
            // Function call (part of primary expression)
            else if (match(TokenType::PUNCTUATOR, "(")) {
                current--; // Put back '(' for primaryExpr to handle
                if (!expr()) {
                    error("Expected expression");
                    synchronize();
                    return false;
                }
            }
            // Just identifier
            else {
                // This is handled by expr() below
                current--; // Put back identifier for expr() to handle
            }
            
            if (!expr()) {
                error("Expected expression");
                synchronize();
                return false;
            }
            
            if (!match(TokenType::PUNCTUATOR, ";")) {
                error("Expected ';' after expression");
                synchronize();
                return false;
            }
            return true;
        }
        
        // Expression statement
        if (expr()) {
            if (!match(TokenType::PUNCTUATOR, ";")) {
                error("Expected ';' after expression");
                synchronize();
                return false;
            }
            return true;
        }
        
        return false;
    }

    // Expr → LOrExpr
    bool expr() {
        return lOrExpr();
    }

    // LOrExpr → LAndExpr | LOrExpr "||" LAndExpr
    bool lOrExpr() {
        if (!lAndExpr()) {
            return false;
        }
        
        while (match(TokenType::OPERATOR, "||")) {
            if (!lAndExpr()) {
                error("Expected expression after '||'");
                return false;
            }
        }
        
        return true;
    }

    // LAndExpr → RelExpr | LAndExpr "&&" RelExpr
    bool lAndExpr() {
        if (!relExpr()) {
            return false;
        }
        
        while (match(TokenType::OPERATOR, "&&")) {
            if (!relExpr()) {
                error("Expected expression after '&&'");
                return false;
            }
        }
        
        return true;
    }

    // RelExpr → AddExpr | RelExpr ("<" | ">" | "<=" | ">=" | "==" | "!=") AddExpr
    bool relExpr() {
        if (!addExpr()) {
            return false;
        }
        
        while (check(TokenType::OPERATOR)) {
            string op = currentToken().value;
            if (op == "<" || op == ">" || op == "<=" || op == ">=" || op == "==" || op == "!=") {
                advance();
                if (!addExpr()) {
                    error("Expected expression after relational operator");
                    return false;
                }
            } else {
                break;
            }
        }
        
        return true;
    }

    // AddExpr → MulExpr | AddExpr ("+" | "-") MulExpr
    bool addExpr() {
        if (!mulExpr()) {
            return false;
        }
        
        while (match(TokenType::OPERATOR, "+") || match(TokenType::OPERATOR, "-")) {
            if (!mulExpr()) {
                error("Expected expression after additive operator");
                return false;
            }
        }
        
        return true;
    }

    // MulExpr → UnaryExpr | MulExpr ("*" | "/" | "%") UnaryExpr
    bool mulExpr() {
        if (!unaryExpr()) {
            return false;
        }
        
        while (match(TokenType::OPERATOR, "*") || 
               match(TokenType::OPERATOR, "/") || 
               match(TokenType::OPERATOR, "%")) {
            if (!unaryExpr()) {
                error("Expected expression after multiplicative operator");
                return false;
            }
        }
        
        return true;
    }

    // UnaryExpr → PrimaryExpr | ("+" | "-" | "!") UnaryExpr
    bool unaryExpr() {
        if (match(TokenType::OPERATOR, "+") || 
            match(TokenType::OPERATOR, "-") || 
            match(TokenType::OPERATOR, "!")) {
            return unaryExpr();
        }
        
        return primaryExpr();
    }

    // PrimaryExpr → ID | NUMBER | "(" Expr ")" | ID "(" (Expr ("," Expr)*)? ")"
    bool primaryExpr() {
        // Identifier
        if (match(TokenType::IDENTIFIER)) {
            // Function call
            if (match(TokenType::PUNCTUATOR, "(")) {
                // Parse arguments
                if (!check(TokenType::PUNCTUATOR, ")")) {
                    if (!expr()) {
                        error("Expected expression in function call");
                        return false;
                    }
                    
                    while (match(TokenType::PUNCTUATOR, ",")) {
                        if (!expr()) {
                            error("Expected expression after ',' in function call");
                            return false;
                        }
                    }
                }
                
                if (!match(TokenType::PUNCTUATOR, ")")) {
                    error("Expected ')' after function call");
                    return false;
                }
            }
            // Just identifier - already consumed
            return true;
        }
        
        // Number
        if (match(TokenType::INTEGER_LITERAL)) {
            return true;
        }
        
        // Parenthesized expression
        if (match(TokenType::PUNCTUATOR, "(")) {
            if (!expr()) {
                error("Expected expression after '('");
                return false;
            }
            
            if (!match(TokenType::PUNCTUATOR, ")")) {
                error("Expected ')' after expression");
                return false;
            }
            return true;
        }
        
        return false;
    }

    bool parse() {
        compUnit();
        return !hasError;
    }

    void printErrors() {
        if (hasError) {
            cout << "reject" << endl;
            for (int line : errorLines) {
                cout << line << endl;
            }
        } else {
            cout << "accept" << endl;
        }
    }
};

int main() {
    string sourceCode;
    ostringstream ss;
    ss << cin.rdbuf();
    sourceCode = ss.str();

    LexicalAnalyzer lexer(sourceCode);
    vector<Token> tokens = lexer.tokenize();

    Parser parser(tokens);
    bool success = parser.parse();
    parser.printErrors();

    return 0;
}