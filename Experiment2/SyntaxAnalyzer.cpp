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

    // Function to check if a character is a OPERATOR
    bool isOPERATOR(char c)
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

    // Function to get the next OPERATOR
    string getNextOPERATOR()
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
                    string op = getNextOPERATOR();
                    tokens.emplace_back(TokenType::OPERATOR,op, currentLine);
                }
            }
            else if(isOPERATOR(currentChar))
            {
                string op = getNextOPERATOR(); 
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

        tokens.emplace_back(TokenType::END_OF_FILE, "", currentLine);
        return tokens;
    }
};

// all functions below for print name
string getKeyWordName(const Token& token)
{
    return string("'") + token.value + "'";
}


string getOPERATORName(const Token& token)
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
            return getOPERATORName(token);
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

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

class SyntaxAnalyzer{
private:
    vector<Token> tokens; 
    int pos; // 当前的位置
    set<int> errorLines;

    Token getCurrentToken() {
        if(pos >= tokens.size()) return tokens.back();
        return tokens[pos];
    }

    void advance() {
        if (pos < tokens.size()) pos ++;
    }

    void error() {
        int line = getCurrentToken().line;
        errorLines.insert(line);
    }

    bool match(TokenType type, const string& value = "") {
        if(getCurrentToken().type != type) return false;
        if(!value.empty() && getCurrentToken().value != value) return false;
        return true;
    }

    bool consume(TokenType type, const string&value) {
        if(match(type, value)) {
            advance();
            return true;
        }
        error();
        return false;
    }

    bool consume(TokenType type) {
        if(getCurrentToken().type == type) {
            advance();
            return true;
        }
        error();
        return false;
    }

    void sync() {
        while(getCurrentToken().type != TokenType::END_OF_FILE &&
            !(match(TokenType::PUNCTUATOR, ";"))&&
            !(match(TokenType::PUNCTUATOR,"}"))) {
                advance();
            }
            if(match(TokenType::PUNCTUATOR, ";"))
                advance();
    }

    void parseCompUnit() {
        while (getCurrentToken().type != TokenType::END_OF_FILE) {
            parseFuncDef();
        }
    }

    // 函数定义 FuncDef → (“int” | “void”) ID “(” (Param (“,” Param)*)? “)” Block
    void parseFuncDef() {
        if (!match(TokenType::KEYWORD, "int") && !match(TokenType::KEYWORD, "void")) {
            error();
            sync();
            if(match(TokenType::PUNCTUATOR,"}"))
                advance();
            return;
        }
        advance();

        if(!consume(TokenType::IDENTIFIER)) {
            sync();
            if(match(TokenType::PUNCTUATOR,"}"))
                advance();
            return;
        }

        consume(TokenType::PUNCTUATOR, "(");

        if(match(TokenType::KEYWORD, "int")) {
            parseParam();
            while(match(TokenType::PUNCTUATOR,",")) {
                advance();
                parseParam();
            }
        }

        consume(TokenType::PUNCTUATOR, ")");
        parseBlock();
    }

    // 形参 Param → “int” ID
    void parseParam(){
        consume(TokenType::KEYWORD, "int");
        consume(TokenType::IDENTIFIER);
    }

    // 语句块 Block → “{” Stmt* “}”
    void parseBlock() {
        if (!consume(TokenType::PUNCTUATOR, "{")) {
            return;
        }
        while (!match(TokenType::PUNCTUATOR,"}") &&
                getCurrentToken().type != TokenType::END_OF_FILE){
                    parseStmt();
                }

        consume(TokenType::PUNCTUATOR, "}");
    }

    /*
    语句 Stmt → Block | “;” | Expr “;” | ID “=” Expr “;”
           | “int” ID “=” Expr “;”
           | “if ” “(” Expr “)” Stmt (“else” Stmt)?
           | “while” “(” Expr “)” Stmt
           | “break” “;” | “continue” “;” | “return” Expr “;”
    */
    void parseStmt() {
        if(match(TokenType::KEYWORD, "int")) {
            advance();
            consume(TokenType::IDENTIFIER);
            if(match(TokenType::OPERATOR,"="))
            {
                advance();
                parseExpr();
            }
            while(match(TokenType::PUNCTUATOR,",")) {
                advance();
                consume(TokenType::IDENTIFIER);
                if(match(TokenType::OPERATOR,"=")) {
                    advance();
                    parseExpr();
                }
            }
            consume(TokenType::PUNCTUATOR, ";");
        } else if(match(TokenType::KEYWORD, "if")) {
            advance();
            consume(TokenType::PUNCTUATOR, "(");
            parseExpr();
            consume(TokenType::PUNCTUATOR, ")");
            parseStmt();
            if(match(TokenType::KEYWORD, "else")) {
                advance();
                parseStmt();
            }
        } else if(match(TokenType::KEYWORD, "while")) {
            advance();
            consume(TokenType::PUNCTUATOR, "(");
            parseExpr();
            consume(TokenType::PUNCTUATOR, ")");
            parseStmt();
        } else if(match(TokenType::KEYWORD, "break")) {
            advance();
            consume(TokenType::PUNCTUATOR, ";");
        } else if (match(TokenType::KEYWORD, "continue")) {
            advance();
            consume(TokenType::PUNCTUATOR, ";");
        } else if( match(TokenType::KEYWORD, "return")) {
            advance();
            if(!(match(TokenType::PUNCTUATOR, ";"))) {
                parseExpr();
            }
            consume(TokenType::PUNCTUATOR, ";");
        } else if (match(TokenType::PUNCTUATOR, ";") ){
            parseBlock();
        } else if(getCurrentToken().type == TokenType::IDENTIFIER) {
            advance();
            if(match(TokenType::OPERATOR,"=")) {
                advance();
                parseExpr();
                consume(TokenType::PUNCTUATOR, ";");
            } else if(match(TokenType::PUNCTUATOR,"(")) {
                advance();
                if(! (match(TokenType::PUNCTUATOR,")"))) {
                    parseExpr();
                    while( match(TokenType::PUNCTUATOR,",")){
                        advance();
                        parseExpr();
                    }
                }
                consume(TokenType::PUNCTUATOR, ")");
                consume(TokenType::PUNCTUATOR, ";");
            } else {
                consume(TokenType::PUNCTUATOR, ";");
            }
        } else if(match(TokenType::PUNCTUATOR, ";")) {
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
        while(match(TokenType::OPERATOR, "||")){
            advance();
            parseLAndExpr();
        }
    }

    void parseLAndExpr() {
        parseRelExpr();
        while( getCurrentToken().type == TokenType::OPERATOR && getCurrentToken().value == "&&") 
        {
            advance();
            parseRelExpr();
        }
    }

    void parseRelExpr() {
        parseAddExpr();
        while(getCurrentToken().type == TokenType::OPERATOR &&
    (getCurrentToken().value == "<" || getCurrentToken().value == "<=" || getCurrentToken().value == ">" || getCurrentToken().value == ">=" || getCurrentToken().value == "=="|| getCurrentToken().value == "!="))
        {
            advance();
            parseAddExpr();
        }
    }

    void parseAddExpr() {
        parseMulExpr();
        while(getCurrentToken().type == TokenType::OPERATOR && (getCurrentToken().value == "+" || getCurrentToken().value == "-")) {
            advance();
            parseMulExpr();
        }
    }

    void parseMulExpr() {
        parseUnaryExpr();
        while(getCurrentToken().type == TokenType::OPERATOR &&
    (getCurrentToken().value == "*" || getCurrentToken().value == "/" || getCurrentToken().value == "%"))
    {
        advance();
        parseUnaryExpr();
    }
    }

    void parseUnaryExpr() {
        if(match(TokenType::OPERATOR, "+") || match(TokenType::OPERATOR, "-") || match(TokenType::OPERATOR, "!"))
        {
            advance();
            parseUnaryExpr();
        } else {
            parsePrimaryExpr();
        }
    }

    void parsePrimaryExpr() {
        if(match(TokenType::IDENTIFIER))
        {
            advance();
            if(match(TokenType::PUNCTUATOR, "("))
            {
                advance();
                if(!match(TokenType::PUNCTUATOR, ")")){
                    parseExpr();
                    while(match(TokenType::PUNCTUATOR, ","))
                    {
                        advance();
                        parseExpr();
                    }
                }
                consume(TokenType::PUNCTUATOR, ")");
            }
        }
        else if(match(TokenType::INTEGER_LITERAL))
        {
            advance();
        }
        else if (match(TokenType::PUNCTUATOR, "("))
        {
            advance();
            parseExpr();
            consume(TokenType::PUNCTUATOR, ")");
        }
        else 
        {
            error();
            if(!match(TokenType::END_OF_FILE) && !match(TokenType::PUNCTUATOR, ";")) {
                advance();
            }
        }
    }

public:
    SyntaxAnalyzer(const vector<Token>&toks): tokens(toks), pos(0) {}

    bool parse() {
        parseCompUnit();
        return errorLines.empty();
    }

    set<int> getErrors() {return errorLines;}
};


int main()
{
    string input, line;
    while(getline(cin, line)) {
        input += line + "\n";
    }

    LexicalAnalyzer lexer(input);
    vector<Token> tokens = lexer.tokenize();

    SyntaxAnalyzer parser(tokens);
    parser.parse();
    set<int> Errors = parser.getErrors();

    if(Errors.empty()){
        cout<<"accept" <<endl;
    } else {
        cout << "reject" <<endl;
        for (const auto& e : Errors) {
            cout << e << endl;
        }
    }
    return 0;
}
