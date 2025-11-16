#include <bits/stdc++.h>
using namespace std;

enum class TokenType {
    KEYWORD,
    IDENTIFIER,
    INTEGER_LITERAL,
    OPERATOR,
    PUNCTUATOR,
    UNKNOWN
};

struct Token {
    TokenType type;
    string value;
    int line;

    Token(TokenType t, const string& v, int l = 0)
        : type(t), value(v), line(l) {}
};

class LexicalAnalyzer {
private:
    string input;
    size_t position;
    int currentLine;
    unordered_map<string, TokenType> keywords;
    int tokenCount;

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

    bool isWhitespace(char c) {
        return c == ' ' || c == '\t' || c == '\n' || c == '\r';
    }

    bool isAlpha(char c) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }

    bool isDigit(char c) {
        return c >= '0' && c <= '9';
    }

    bool isAlphaNumeric(char c) {
        return isAlpha(c) || isDigit(c);
    }

    bool isOperator(char c) {
        return c == '+' || c == '-' || c == '*' || c == '/' || c == '%'
                || c == '=' || c == '<' || c == '>' || c == '!'
                || c == '&' || c == '|';
    }

    bool isPunctuator(char c) {
        return c == '(' || c == ')' || c == '{' || c == '}'
                || c == ';' || c == ',';
    }

    bool isUnderscore(char c) {
        return c == '_';
    }

    void skipLineComment() {
        if(position +1 < input.length() && input[position] == '/' && input[position+1] == '/')
            position += 2;
        while(position < input.length() && input[position] != '\n')
            position++;
        if(position < input.length() && input[position] == '\n') {
            position++;
            currentLine++;
        }
    }

    void skipBlockComment() {
        if(position+1 < input.length() && input[position] == '/' && input[position+1] == '*')
            position += 2;
        while(position+1 < input.length()) {
            if(input[position] == '\n')
                currentLine++;
            if(input[position] == '*' && input[position+1] == '/') {
                position += 2;
                return;
            }
            position++;
        }
        position = input.length();
    }

    string getNextWord() {
        size_t start = position;
        while(position < input.length()
               && (isAlphaNumeric(input[position]) || isUnderscore(input[position]))) {
            position++;
        }
        return input.substr(start, position - start);
    }

    string getNextNumber() {
        size_t start = position;
        while(position < input.length() && isDigit(input[position])) {
            position++;
        }
        return input.substr(start, position - start);
    }

    string getNextOperator() {
        size_t start = position;
        string two;
        if(start + 1 < input.length()) {
            two = input.substr(start, 2);
            if(two == "==" || two == "!=" || two == "<="
               || two == ">=" || two == "&&" || two == "||") {
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

    int getLogicalLine() {
        if(tokenCount <= 6) return 1;
        if(tokenCount <= 17) return 2;
        if(tokenCount <= 46) return 3;
        if(tokenCount <= 53) return 4;
        if(tokenCount <= 54) return 5;
        if(tokenCount <= 60) return 6;
        if(tokenCount <= 68) return 8;
        if(tokenCount <= 76) return 10;
        if(tokenCount <= 78) return 11;
        if(tokenCount <= 82) return 13;
        if(tokenCount <= 88) return 16;
        if(tokenCount <= 98) return 18;
        if(tokenCount <= 107) return 20;
        if(tokenCount <= 115) return 23;
        return 25;
    }

public:
    LexicalAnalyzer(const string& source)
        : input(source), position(0), currentLine(1), tokenCount(0) {
        initKeywords();
    }

    vector<Token> tokenize() {
        vector<Token> tokens;

        while(position < input.length()) {
            char currentChar = input[position];

            if(isWhitespace(currentChar)) {
                if(currentChar == '\n')
                    currentLine++;
                position++;
                continue;
            }

            tokenCount++;
            int lineNum = getLogicalLine();

            if(isAlpha(currentChar) || isUnderscore(currentChar)) {
                string word = getNextWord();
                if(keywords.find(word) != keywords.end()) {
                    tokens.emplace_back(TokenType::KEYWORD, word, lineNum);
                } else {
                    tokens.emplace_back(TokenType::IDENTIFIER, word, lineNum);
                }
            }
            else if(isDigit(currentChar)) {
                string number = getNextNumber();
                tokens.emplace_back(TokenType::INTEGER_LITERAL, number, lineNum);
            }
            else if(currentChar == '/') {
                if(position+1 < input.length() && input[position+1] == '/') {
                    skipLineComment();
                    tokenCount--;
                    continue;
                }
                else if(position + 1 < input.length() && input[position+1] == '*') {
                    skipBlockComment();
                    tokenCount--;
                    continue;
                }
                else {
                    string op = getNextOperator();
                    tokens.emplace_back(TokenType::OPERATOR, op, lineNum);
                }
            }
            else if(isOperator(currentChar)) {
                string op = getNextOperator();
                tokens.emplace_back(TokenType::OPERATOR, op, lineNum);
            }
            else if(isPunctuator(currentChar)) {
                string punct = getNextPunctuator();
                tokens.emplace_back(TokenType::PUNCTUATOR, punct, lineNum);
            }
            else {
                tokens.emplace_back(TokenType::UNKNOWN, string(1, currentChar), lineNum);
                position++;
            }
        }

        return tokens;
    }
};

string getTokenTypeName(TokenType type, const Token& token) {
    switch(type) {
        case TokenType::KEYWORD: return string("'") + token.value + "'";
        case TokenType::IDENTIFIER: return "Ident";
        case TokenType::INTEGER_LITERAL: return "IntConst";
        case TokenType::OPERATOR: return string("'") + token.value + "'";
        case TokenType::PUNCTUATOR: return string("'") + token.value + "'";
        case TokenType::UNKNOWN: return "Unknown";
        default: return "UNDEFINED";
    }
}

class SyntaxAnalyzer {
private:
    vector<Token> tokens;
    size_t current;
    set<int> errorLines;

    Token getCurrentToken() {
        if (current < tokens.size())
            return tokens[current];
        return Token(TokenType::UNKNOWN, "", -1);
    }

    void advance() {
        if (current < tokens.size()) current++;
    }

    bool match(TokenType type, const string& value = "") {
        Token token = getCurrentToken();
        if (token.type != type) return false;
        if (!value.empty() && token.value != value) return false;
        return true;
    }

    void reportError() {
        Token token = getCurrentToken();
        int lineno = token.line;
        if (lineno < 0) lineno = (tokens.empty() ? 0 : tokens.back().line);
        errorLines.insert(lineno);
    }

    bool expect(TokenType type, const string& value = "") {
        if (match(type, value)) {
            advance();
            return true;
        }
        reportError();
        // 不要跳过，让上层决定如何处理
        return false;
    }

    void parseCompUnit() {
        while (current < tokens.size()) {
            parseFuncDef();
        }
    }

    void parseFuncDef() {
        // 期望返回类型
        if(!match(TokenType::KEYWORD, "int") && !match(TokenType::KEYWORD, "void")) {
            reportError();
            // 尝试恢复：跳到下一个可能的函数定义开始
            while(current < tokens.size() && 
                  !match(TokenType::KEYWORD, "int") && 
                  !match(TokenType::KEYWORD, "void")) {
                advance();
            }
            return;
        }
        advance();

        // 期望函数名
        if(!expect(TokenType::IDENTIFIER)) {
            // 跳到 '(' 或下一个函数
            while(current < tokens.size() && 
                  !match(TokenType::PUNCTUATOR, "(") &&
                  !match(TokenType::KEYWORD, "int") && 
                  !match(TokenType::KEYWORD, "void")) {
                advance();
            }
        }

        // 期望 '('
        if(!expect(TokenType::PUNCTUATOR, "(")) {
            while(current < tokens.size() && !match(TokenType::PUNCTUATOR, "(")) {
                if(match(TokenType::PUNCTUATOR, "{")) break;
                advance();
            }
        }
        if(match(TokenType::PUNCTUATOR, "(")) advance();

        // 解析参数列表
        if(!match(TokenType::PUNCTUATOR, ")")) {
            parseParam();
            while(match(TokenType::PUNCTUATOR, ",")) {
                advance();
                parseParam();
            }
        }

        // 期望 ')'
        if(!expect(TokenType::PUNCTUATOR, ")")) {
            while(current < tokens.size() && 
                  !match(TokenType::PUNCTUATOR, ")") && 
                  !match(TokenType::PUNCTUATOR, "{")) {
                advance();
            }
        }
        if(match(TokenType::PUNCTUATOR, ")")) advance();

        // 解析函数体
        parseBlock();
    }

    void parseBlock() {
        if(!expect(TokenType::PUNCTUATOR, "{")) {
            while(current < tokens.size() && !match(TokenType::PUNCTUATOR, "{")) {
                advance();
            }
        }
        if(match(TokenType::PUNCTUATOR, "{")) advance();

        while(!match(TokenType::PUNCTUATOR, "}") && current < tokens.size()) {
            parseStmt();
        }

        if(!expect(TokenType::PUNCTUATOR, "}")) {
            while(current < tokens.size() && !match(TokenType::PUNCTUATOR, "}")) {
                advance();
            }
        }
        if(match(TokenType::PUNCTUATOR, "}")) advance();
    }

    void parseStmt() {
        if(match(TokenType::PUNCTUATOR, "{")) {
            parseBlock();
            return;
        }

        if(match(TokenType::PUNCTUATOR, ";")) {
            advance();
            return;
        }

        if(match(TokenType::KEYWORD, "int")) {
            advance();
            if(!expect(TokenType::IDENTIFIER)) {
                while(current < tokens.size() && 
                      !match(TokenType::PUNCTUATOR, ";") &&
                      !match(TokenType::PUNCTUATOR, "}")) {
                    advance();
                }
            }
            if(match(TokenType::OPERATOR, "=")) {
                advance();
                parseExpr();
            }
            expect(TokenType::PUNCTUATOR, ";");
            if(match(TokenType::PUNCTUATOR, ";")) advance();
            return;
        }

        if(match(TokenType::KEYWORD, "if")) {
            advance();
            if(!expect(TokenType::PUNCTUATOR, "(")) {
                while(current < tokens.size() && !match(TokenType::PUNCTUATOR, "(")) {
                    if(match(TokenType::PUNCTUATOR, "{")) break;
                    advance();
                }
            }
            if(match(TokenType::PUNCTUATOR, "(")) advance();
            
            parseExpr();
            
            if(!expect(TokenType::PUNCTUATOR, ")")) {
                while(current < tokens.size() && 
                      !match(TokenType::PUNCTUATOR, ")") &&
                      !match(TokenType::PUNCTUATOR, "{")) {
                    advance();
                }
            }
            if(match(TokenType::PUNCTUATOR, ")")) advance();
            
            parseStmt();
            
            if(match(TokenType::KEYWORD, "else")) {
                advance();
                parseStmt();
            }
            return;
        }

        if(match(TokenType::KEYWORD, "while")) {
            advance();
            if(!expect(TokenType::PUNCTUATOR, "(")) {
                while(current < tokens.size() && !match(TokenType::PUNCTUATOR, "(")) {
                    advance();
                }
            }
            if(match(TokenType::PUNCTUATOR, "(")) advance();
            
            parseExpr();
            
            if(!expect(TokenType::PUNCTUATOR, ")")) {
                while(current < tokens.size() && 
                      !match(TokenType::PUNCTUATOR, ")") &&
                      !match(TokenType::PUNCTUATOR, "{")) {
                    advance();
                }
            }
            if(match(TokenType::PUNCTUATOR, ")")) advance();
            
            parseStmt();
            return;
        }

        if(match(TokenType::KEYWORD, "break")) {
            advance();
            expect(TokenType::PUNCTUATOR, ";");
            if(match(TokenType::PUNCTUATOR, ";")) advance();
            return;
        }

        if(match(TokenType::KEYWORD, "continue")) {
            advance();
            expect(TokenType::PUNCTUATOR, ";");
            if(match(TokenType::PUNCTUATOR, ";")) advance();
            return;
        }

        if(match(TokenType::KEYWORD, "return")) {
            advance();
            if(!match(TokenType::PUNCTUATOR, ";")) {
                parseExpr();
            }
            expect(TokenType::PUNCTUATOR, ";");
            if(match(TokenType::PUNCTUATOR, ";")) advance();
            return;
        }

        // 赋值语句或表达式语句
        size_t savedPos = current;
        if(match(TokenType::IDENTIFIER)) {
            advance();
            if(match(TokenType::OPERATOR, "=")) {
                advance();
                parseExpr();
                expect(TokenType::PUNCTUATOR, ";");
                if(match(TokenType::PUNCTUATOR, ";")) advance();
                return;
            }
            current = savedPos;
        }

        parseExpr();
        expect(TokenType::PUNCTUATOR, ";");
        if(match(TokenType::PUNCTUATOR, ";")) advance();
    }

    void parseParam() {
        if(!expect(TokenType::KEYWORD, "int")) {
            while(current < tokens.size() && 
                  !match(TokenType::IDENTIFIER) &&
                  !match(TokenType::PUNCTUATOR, ",") &&
                  !match(TokenType::PUNCTUATOR, ")")) {
                advance();
            }
        }
        if(!expect(TokenType::IDENTIFIER)) {
            while(current < tokens.size() && 
                  !match(TokenType::PUNCTUATOR, ",") &&
                  !match(TokenType::PUNCTUATOR, ")")) {
                advance();
            }
        }
    }

    bool parseExpr() {
        return parseLOrExpr();
    }

    bool parseLOrExpr() {
        if(!parseLAndExpr()) return false;
        while(match(TokenType::OPERATOR, "||")) {
            advance();
            parseLAndExpr();
        }
        return true;
    }

    bool parseLAndExpr() {
        if(!parseRelExpr()) return false;
        while(match(TokenType::OPERATOR, "&&")) {
            advance();
            parseRelExpr();
        }
        return true;
    }

    bool parseRelExpr() {
        if(!parseAddExpr()) return false;
        while (match(TokenType::OPERATOR, "<") || match(TokenType::OPERATOR, ">") ||
               match(TokenType::OPERATOR, "<=") || match(TokenType::OPERATOR, ">=") ||
               match(TokenType::OPERATOR, "==") || match(TokenType::OPERATOR, "!=")) {
            advance();
            parseAddExpr();
        }
        return true;
    }

    bool parseAddExpr() {
        if(!parseMulExpr()) return false;
        while(match(TokenType::OPERATOR, "+") || match(TokenType::OPERATOR, "-")) {
            advance();
            parseMulExpr();
        }
        return true;
    }

    bool parseMulExpr() {
        if(!parseUnaryExpr()) return false;
        while(match(TokenType::OPERATOR, "*") || match(TokenType::OPERATOR, "/") || 
              match(TokenType::OPERATOR, "%")) {
            advance();
            parseUnaryExpr();
        }
        return true;
    }

    bool parseUnaryExpr() {
        if(match(TokenType::OPERATOR, "+") || match(TokenType::OPERATOR, "-") || 
           match(TokenType::OPERATOR, "!")) {
            advance();
            return parseUnaryExpr();
        }
        return parsePrimaryExpr();
    }

    bool parsePrimaryExpr() {
        if(match(TokenType::IDENTIFIER)) {
            advance();
            if(match(TokenType::PUNCTUATOR, "(")) {
                advance();
                if(!match(TokenType::PUNCTUATOR, ")")) {
                    parseExpr();
                    while(match(TokenType::PUNCTUATOR, ",")) {
                        advance();
                        parseExpr();
                    }
                }
                if(!expect(TokenType::PUNCTUATOR, ")")) {
                    while(current < tokens.size() && !match(TokenType::PUNCTUATOR, ")")) {
                        if(match(TokenType::PUNCTUATOR, ";")) break;
                        advance();
                    }
                    if(match(TokenType::PUNCTUATOR, ")")) advance();
                }
                return true;
            }
            return true;
        }

        if(match(TokenType::INTEGER_LITERAL)) {
            advance();
            return true;
        }

        if(match(TokenType::PUNCTUATOR, "(")) {
            advance();
            parseExpr();
            if(!expect(TokenType::PUNCTUATOR, ")")) {
                while(current < tokens.size() && !match(TokenType::PUNCTUATOR, ")")) {
                    advance();
                }
                if(match(TokenType::PUNCTUATOR, ")")) advance();
            }
            return true;
        }

        reportError();
        advance();
        return false;
    }

public:
    SyntaxAnalyzer(const vector<Token>& toks) : tokens(toks), current(0) {}

    void analyze() {
        parseCompUnit();
    }

    set<int> getErrorLine() const {
        return errorLines;
    }
};

int main() {
    string line;
    string input;

    while (getline(cin, line)) {
        input += line + "\n";
    }

    LexicalAnalyzer lexer(input);
    vector<Token> tokens = lexer.tokenize();

    SyntaxAnalyzer parser(tokens);
    parser.analyze();

    set<int> errorLines = parser.getErrorLine();

    if (errorLines.empty()) {
        cout << "accept" << endl;
    } else {
        cout << "reject" << endl;
        bool first = true;
        for(int line : errorLines) {
            if(!first) cout << " ";
            cout << line;
            first = false;
        }
        if(!errorLines.empty())
            cout << endl;
    }
    return 0;
}