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
        <<":"<<"\"" << token.value << "\"" << endl;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////

// 语法分析器类
class SyntaxAnalyzer {
private:
    vector<Token> tokens;
    size_t current;
    set<int> errorLines;
    vector<string> errorMessages;

    // 获取当前token
    Token getCurrentToken() {
        if (current < tokens.size())
            return tokens[current];
        return Token(TokenType::UNKNOWN, "", -1);
    }

    // 移动到下一个token
    void advance() {
        if (current < tokens.size())
            current++;
    }

    // 检查当前token是否匹配期望的值
    bool match(TokenType type, const string& value = "") {
        Token token = getCurrentToken();
        if (token.type != type)
            return false;
        if (!value.empty() && token.value != value)
            return false;
        return true;
    }

    // 期望一个特定的token，如果不匹配则报错
    bool expect(TokenType type, const string& value = "") {
        if (match(type, value)) {
            advance();
            return true;
        }
        Token token = getCurrentToken();
        errorLines.insert(token.line);
        string msg = "Line" + to_string(token.line) + ": Expected ";
        if(!value.empty())
            msg += "'" + value + "'";
        else 
            msg += "token type";
        msg += " but got '"  + token.value + "'";
        errorMessages.push_back(msg);
        return false;
    }

    void reportError(const string& msg) {
        Token token = getCurrentToken();
        errorLines.insert(token.line);
        errorMessages.push_back("Line " + to_string(token.line) + ": " + msg);
    }


    // 语法规则：CompUnit -> {FuncDef}
    bool parseCompUnit() {
        while (current < tokens.size()) {
            parseFuncDef();
        }
        return errorLines.empty();
    }

    // FuncDef → (“int” | “void”) ID “(” (Param (“,” Param)*)? “)” Block
    bool parseFuncDef()
    {
        if(!match(TokenType::KEYWORD, "int") && !match(TokenType::KEYWORD, "void")){
            reportError("Expected function return type(int or void)");
            return false;
        }
        
        advance();

        if(!expect(TokenType::IDENTIFIER))
            return false;

        if(!expect(TokenType::PUNCTUATOR, "("))
            return false;

        if(!match(TokenType::PUNCTUATOR, ")")) 
        {
            if(!parseParam())
                return false;
            
            while(match(TokenType::PUNCTUATOR, ","))
            {
                advance();

                if(!parseParam())
                    return false;
            }
        }

        if(!expect(TokenType::PUNCTUATOR, ")"))
            return false;
        
        return parseBlock();
    }

    // Block → “{” Stmt* “}”
    bool parseBlock()
    {
        if(!expect(TokenType::PUNCTUATOR, "{"))
            return false;
        
        while(!match(TokenType::PUNCTUATOR, "}") && current < tokens.size()){
            parseStmt();
        }

        return expect(TokenType::PUNCTUATOR, "}");
    }

    /*
    语句 Stmt → Block | “;” | Expr “;” | ID “=” Expr “;”
           | “int” ID “=” Expr “;”
           | “if ” “(” Expr “)” Stmt (“else” Stmt)?
           | “while” “(” Expr “)” Stmt
           | “break” “;” | “continue” “;” | “return” Expr “;”
    */
   bool parseStmt()
   {
    if(match(TokenType::PUNCTUATOR, "{"))
        return parseBlock();

    if(match(TokenType::PUNCTUATOR, ";"))
    {
        advance();
        return true;
    }

    if(match(TokenType::KEYWORD, "int"))
    {
        advance();

        if(!expect(TokenType::IDENTIFIER))
            return false;

        if(!expect(TokenType::OPERATOR, "="))
            return false;
        
        if(!parseExpr())
            return false;

        return expect(TokenType::PUNCTUATOR, ";");
    }

    size_t savedPos = current;

    if(match(TokenType::IDENTIFIER))
    {
        advance();

        if(match(TokenType::OPERATOR, "="))
        {
            advance();
            if(!parseExpr())
                return false;
            return expect(TokenType::PUNCTUATOR, ";");
        }

        current = savedPos;
        if(!parseExpr())
            return false;

        return expect(TokenType::PUNCTUATOR, ";");
    }

    if(match(TokenType::KEYWORD, "if"))
    {
        advance();

        if(!expect(TokenType::PUNCTUATOR, "("))
            return false;
        
        if(!parseExpr())
            return false;

        if(!expect(TokenType::PUNCTUATOR, ")"))
            return false;

        if(!parseStmt())
            return false;
        
        if(match(TokenType::KEYWORD, "else"))
        {
            advance();

            if(!parseStmt())
                return false;
        }

        return true;
    }

    if(match(TokenType::KEYWORD, "while"))
    {
        advance();

        if(!expect(TokenType::PUNCTUATOR, "("))
            return false;

        if(!parseExpr())
            return false;

        if(!expect(TokenType::PUNCTUATOR, ")"))
            return false;

        return parseStmt();
    }

    if(match(TokenType::KEYWORD, "break"))
    {
        advance();

        if(!expect(TokenType::PUNCTUATOR, ";"))
            return false;
        return true;
    }

    if(match(TokenType::KEYWORD, "continue"))
    {
        advance();

        if(!expect(TokenType::PUNCTUATOR, ";"))
            return false;

        return true;
    }

    if(match(TokenType::KEYWORD, "return"))
    {
        advance();

        if(!parseExpr())
            return false;

        if(!expect(TokenType::PUNCTUATOR, ";"))
            return false;

        return true;
    }

    if(!parseExpr())
        return false;

    return expect(TokenType::PUNCTUATOR, ";");
   }

    //  Param → “int” ID
    bool parseParam()
    {
        if(!expect(TokenType::KEYWORD, "int"))
            return false;

        if(!expect(TokenType::IDENTIFIER))
            return false;

        return true;
    }

    // Expr → LOrExpr
    bool parseExpr()
    {
        return parseLOrExpr();
    }

    // LOrExpr → LAndExpr | LOrExpr “||” LAndExpr
    // LOReXPR -> LAndExpr ("||" LAndExpr)*
    bool parseLOrExpr()
    {
        if(!parseLAndExpr())
            return false;

        while(match(TokenType::OPERATOR, "||"))
        {
            advance();

            if(!parseLAndExpr())
                return false;
        }

        return true;
    }

    //  LAndExpr → RelExpr | LAndExpr “&&” RelExpr
    //  LAndExpr -> RelExpr ("&&" RelExpr)*
    bool parseLAndExpr()
    {
        if(!parseRelExpr())
            return false;

        while(match(TokenType::OPERATOR, "&&"))
        {
            if(!parseRelExpr())
                return false;
        }
        
        return true;
    }

    // RelExpr → AddExpr | RelExpr (“<” | “>” | “<=” | “>=” | “==” | “!=”) AddExpr
    // RelExpr -> AddExpr (("<" | ">" | "<=" | ">=" | "==" | "!=") AddExpr)*
    bool parseRelExpr()
    {
        if(!parseAddExpr())
            return false;
        
        while (match(TokenType::OPERATOR, "<") || match(TokenType::OPERATOR, ">") ||
               match(TokenType::OPERATOR, "<=") || match(TokenType::OPERATOR, ">=") ||
               match(TokenType::OPERATOR, "==") || match(TokenType::OPERATOR, "!="))
        {
            advance();
            if (!parseAddExpr())
                return false;
        }
        return true;
    }

    //AddExpr → MulExpr | AddExpr (“+” | “-”) MulExpr
    //AddExpr -> MulExpr (("+" | "-") MulExpr)*
    bool parseAddExpr()
    {
        if(!parseMulExpr())
            return false;

        while(match(TokenType::OPERATOR, "+") || match(TokenType::OPERATOR, "-"))
        {
            advance();

            if(!parseMulExpr())
                return false;
        }

        return true;
    }

    // MulExpr → UnaryExpr | MulExpr (“*” | “/” | “%”) UnaryExpr
    // MulExpr -> UnaryExpr (("*" | "/" | "%") UnaryExpr)*
    bool parseMulExpr()
    {
        if(!parseUnaryExpr())
            return false;
        
        while(match(TokenType::OPERATOR, "*") || match(TokenType::OPERATOR, "/") || match(TokenType::OPERATOR, "%"))
        {
            advance();
            if(!parseUnaryExpr())
                return false;
        }

        return true;
    }

    // UnaryExpr → PrimaryExpr | (“+” | “-” | “!”) UnaryExpr
    bool parseUnaryExpr()
    {
        if(match(TokenType::OPERATOR, "+") || match(TokenType::OPERATOR, "-") || match(TokenType::OPERATOR, "!"))
        {
            advance();

            return parseUnaryExpr();
        }

        return parsePrimaryExpr();
    }

    // PrimaryExpr → ID | NUMBER | “(” Expr “)” | ID “(” (Expr (“,” Expr)*)? “)”
    bool parsePrimaryExpr()
    {
        if(match(TokenType::IDENTIFIER))
        {
            advance();

            if(match(TokenType::PUNCTUATOR, "(")) {
                advance();

                if(!match(TokenType::PUNCTUATOR, ")"))
                {
                    if(!parseExpr())
                        return false;

                    while(match(TokenType::PUNCTUATOR, ","))
                    {
                        advance();
                        if(!parseExpr())
                            return false;
                    }
                }
                return expect(TokenType::PUNCTUATOR, ";");
            }
            return true;
        }

        if(match(TokenType::INTEGER_LITERAL))
        {
            advance();
            return true;
        }

        if(match(TokenType::PUNCTUATOR, "("))
        {
            advance();

            if(!parseExpr())
                return false;

            if(!expect(TokenType::PUNCTUATOR, ")"))
                return false;
            
            return true;
        }
    }

public:
    SyntaxAnalyzer(const vector<Token>& toks)
        : tokens(toks)
        , current(0)
    {}

    bool analyze() {
        return parseCompUnit();
    }

    bool hasErrors() const {
        return !errorMessages.empty();
    }

    set<int> getErrorLine() const {
        return errorLines;
    }

    vector<string> getErrorMessage() const {
        return errorMessages;
    }
};

int main() {
    string line;
    string input;
    
    // 读取所有输入
    while (getline(cin, line)) {
        input += line + "\n";
    }

    // 词法分析
    LexicalAnalyzer lexer(input);
    vector<Token> tokens = lexer.tokenize();

    // 语法分析
    SyntaxAnalyzer parser(tokens);
    bool success = parser.analyze();

    if (success && !parser.hasErrors()) {
        cout << "accept" << endl;
    } else {
        cout << "reject" << endl;
        
        // 输出所有错误的行号
        set<int> errorLines = parser.getErrorLine();

        cout<<"Error Lines: ";
        bool first = true;
        for(int line : errorLines)
        {
            if(!first) cout <<", ";
            cout << line;
            first = false;
        }
        cout << endl;
    }
    return 0;
}
