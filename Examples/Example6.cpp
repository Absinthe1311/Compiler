
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
    int line; // 行号信息

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
    int currentLine; //当前的行号
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
                || c == ';' || c == ',';
    }
    
    // Function to check if a character is a underscore
    bool isUnderscore(char c)
    {
        return c == '_';
    }

    // Function to skip LineComment
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

    // Function to skip BlockComment
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
                return ;
            }
            position++;
        }
        position = input.length(); // 当最后没有终结*/的时候，到了程序结尾
    }

    // Function to get the next word
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
               || two == ">=" || two == "&&" || two == "||"
              )
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
        , currentLine(1)
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
                if(currentChar == '\n')
                    currentLine++;
                position++;
                continue;
            }

            // Identify keywords or identifiers 还有下划线开头
            if(isAlpha(currentChar) || isUnderscore(currentChar))
            {
                string word = getNextWord();
                if(keywords.find(word) != keywords.end()) //identify keywords
                {
                    tokens.emplace_back(TokenType::KEYWORD, word, currentLine);
                }
                else 
                {
                    tokens.emplace_back(TokenType::IDENTIFIER, word, currentLine);
                }
            }
            else if(isDigit(currentChar)) // identify integer
            {
                string number = getNextNumber();
                tokens.emplace_back(TokenType::INTEGER_LITERAL, number, currentLine);
            }
            else if(currentChar == '/') //遇到/的时候判断是注释还是运算符
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
                    tokens.emplace_back(TokenType::OPERATOR,op, currentLine);
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
            else // unknown character
            {
                tokens.emplace_back(TokenType::UNKNOWN, string(1, currentChar), currentLine);
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
        <<":"<<"\"" << token.value << "\"" <<token.line<< endl;
    }
}
class Parser {
public:
    vector<Token> tokens;
    int pos = 0;
    vector<int> errors;

    Parser(const vector<Token>& t) : tokens(t) {}

    // ---------------- 工具函数 ----------------

    bool isEnd() const {
        return pos >= (int)tokens.size();
    }

    const Token& peek(int offset = 0) const {
        static Token dummy(TokenType::UNKNOWN, "", 0);
        int index = pos + offset;
        return (index < tokens.size() ? tokens[index] : dummy);
    }

    bool match(TokenType type, const string& val = "") {
        if (isEnd()) return false;
        if (tokens[pos].type != type) return false;
        if (!val.empty() && tokens[pos].value != val) return false;
        pos++;
        return true;
    }

    const Token& advance() {
        if (!isEnd()) return tokens[pos++];
        return peek(); // dummy
    }

    void error(const string& msg) {
        if (!isEnd()) {
            errors.push_back(peek().line);
        } else if (!tokens.empty()) {
            // 若在末尾报错，就使用最后一个 token 的行号
            errors.push_back(tokens.back().line);
        }
    }

    bool checkOp(const string& op) {
        return !isEnd() && tokens[pos].type == TokenType::OPERATOR && tokens[pos].value == op;
    }

    bool checkPunc(const string& p) {
        return !isEnd() && tokens[pos].type == TokenType::PUNCTUATOR && tokens[pos].value == p;
    }

    bool checkKW(const string& kw) {
        return !isEnd() && tokens[pos].type == TokenType::KEYWORD && tokens[pos].value == kw;
    }

    bool checkID() {
        return !isEnd() && tokens[pos].type == TokenType::IDENTIFIER;
    }

    bool checkNumber() {
        return !isEnd() && tokens[pos].type == TokenType::INTEGER_LITERAL;
    }

    // ---------------- 入口 ----------------
    void parseCompUnit() {
        while (!isEnd()) {
            parseFuncDef();
        }
    }

    // ---------------- 函数定义 ----------------
    void parseFuncDef() {
        // ("int" | "void")
        if (!(checkKW("int") || checkKW("void"))) {
            error("Expected type in function definition");
            advance();
            return;
        }
        advance();

        // ID
        if (!checkID()) {
            error("Expected function name");
            advance();
            return;
        }
        advance();

        // "("
        if (!match(TokenType::PUNCTUATOR, "(")) {
            error("Expected '('");
        }

        // 参数表
        if (!checkPunc(")")) {
            parseParam();
            while (match(TokenType::PUNCTUATOR, ",")) {
                parseParam();
            }
        }

        // ")"
        if (!match(TokenType::PUNCTUATOR, ")")) {
            error("Expected ')'");
        }

        // Block
        parseBlock();
    }

    void parseParam() {
        if (!checkKW("int")) {
            error("Expected 'int' in parameter");
            advance();
            return;
        }
        advance();
        if (!checkID()) {
            error("Expected identifier in parameter");
            advance();
        } else {
            advance();
        }
    }

    // ---------------- 语句块 ----------------
    void parseBlock() {
        if (!match(TokenType::PUNCTUATOR, "{")) {
            error("Expected '{'");
            return;
        }

        while (!isEnd() && !checkPunc("}")) {
            parseStmt();
        }

        if (!match(TokenType::PUNCTUATOR, "}")) {
            error("Expected '}'");
        }
    }

    // ---------------- 语句 ----------------
    void parseStmt() {
        if (checkPunc("{")) {
            parseBlock();
            return;
        }

        if (checkPunc(";")) {
            advance();
            return;
        }

        // int 声明
        if (checkKW("int")) {
            advance();
            if (!checkID()) error("Expected identifier after int");
            else advance();

            if (!match(TokenType::OPERATOR, "=")) error("Expected '='");
            parseExpr();

            if (!match(TokenType::PUNCTUATOR, ";")) error("Expected ';'");
            return;
        }

        // if
        if (checkKW("if")) {
            advance();
            if (!match(TokenType::PUNCTUATOR, "(")) error("Expected '('");
            parseExpr();
            if (!match(TokenType::PUNCTUATOR, ")")) error("Expected ')'");
            parseStmt();
            if (checkKW("else")) {
                advance();
                parseStmt();
            }
            return;
        }

        // while
        if (checkKW("while")) {
            advance();
            if (!match(TokenType::PUNCTUATOR, "(")) error("Expected '('");
            parseExpr();
            if (!match(TokenType::PUNCTUATOR, ")")) error("Expected ')'");
            parseStmt();
            return;
        }

        // break;
        if (checkKW("break")) {
            advance();
            if (!match(TokenType::PUNCTUATOR, ";")) error("Expected ';'");
            return;
        }

        // continue;
        if (checkKW("continue")) {
            advance();
            if (!match(TokenType::PUNCTUATOR, ";")) error("Expected ';'");
            return;
        }

        // return expr;
        if (checkKW("return")) {
            advance();
            parseExpr();
            if (!match(TokenType::PUNCTUATOR, ";")) error("Expected ';'");
            return;
        }

        // ID = Expr;
        if (checkID()) {
            advance();
            if (match(TokenType::OPERATOR, "=")) {
                parseExpr();
                if (!match(TokenType::PUNCTUATOR, ";")) error("Expected ';'");
                return;
            }
            else {
                error("Expected '='");
                return;
            }
        }

        // Expr ;
        parseExpr();
        if (!match(TokenType::PUNCTUATOR, ";")) error("Expected ';'");
    }

    // ---------------- 表达式 ----------------
    void parseExpr() {
        parseLOr();
    }

    void parseLOr() {
        parseLAnd();
        while (checkOp("||")) {
            advance();
            parseLAnd();
        }
    }

    void parseLAnd() {
        parseRel();
        while (checkOp("&&")) {
            advance();
            parseRel();
        }
    }

    void parseRel() {
        parseAdd();
        while (checkOp("<") || checkOp(">") || checkOp("<=") || checkOp(">=") ||
               checkOp("==") || checkOp("!=")) {
            advance();
            parseAdd();
        }
    }

    void parseAdd() {
        parseMul();
        while (checkOp("+") || checkOp("-")) {
            advance();
            parseMul();
        }
    }

    void parseMul() {
        parseUnary();
        while (checkOp("*") || checkOp("/") || checkOp("%")) {
            advance();
            parseUnary();
        }
    }

    void parseUnary() {
        if (checkOp("+") || checkOp("-") || checkOp("!")) {
            advance();
            parseUnary();
        } else {
            parsePrimary();
        }
    }

    void parsePrimary() {
        if (checkNumber()) {
            advance();
            return;
        }
        if (checkID()) {
            advance();
            // 函数调用
            if (match(TokenType::PUNCTUATOR, "(")) {
                if (!checkPunc(")")) {
                    parseExpr();
                    while (match(TokenType::PUNCTUATOR, ",")) {
                        parseExpr();
                    }
                }
                if (!match(TokenType::PUNCTUATOR, ")")) error("Expected ')'");
            }
            return;
        }
        if (match(TokenType::PUNCTUATOR, "(")) {
            parseExpr();
            if (!match(TokenType::PUNCTUATOR, ")"))
                error("Expected ')'");
            return;
        }
        error("Expected primary expression");
        advance();
    }
};


int main()
{
    string line;
    string input;

    while (getline(cin, line)) {
        input += line + "\n";
    }

    LexicalAnalyzer lexer(input);
    vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    parser.parseCompUnit();

    if (parser.errors.empty()) {
        cout << "accept\n";
    } else {
        for (int line : parser.errors) cout << line << "\n";
    }

}