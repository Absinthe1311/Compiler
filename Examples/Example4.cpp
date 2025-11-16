#include <bits/stdc++.h>
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

struct Token {
    TokenType type;
    string value;
    int line;
    Token(TokenType t=TokenType::UNKNOWN, const string& v="", int l=0) : type(t), value(v), line(l) {}
};

class LexicalAnalyzer {
private:
    string input;
    size_t position;
    int currentLine;
    unordered_map<string, TokenType> keywords;

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

    bool isWhitespace(char c) { return c==' '||c=='\t'||c=='\n'||c=='\r'; }
    bool isAlpha(char c) { return (c>='a'&&c<='z')||(c>='A'&&c<='Z'); }
    bool isDigit(char c) { return c>='0' && c<='9'; }
    bool isAlphaNumeric(char c) { return isAlpha(c) || isDigit(c); }
    bool isOperator(char c) {
        return c=='+'||c=='-'||c=='*'||c=='/'||c=='%'||c=='='||c=='<'||c=='>'||c=='!'||c=='&'||c=='|';
    }
    bool isPunctuator(char c) { return c=='('||c==')'||c=='{'||c=='}'||c==';'||c==','; }
    bool isUnderscore(char c) { return c=='_'; }

    void skipLineComment() {
        if(position+1 < input.size() && input[position]=='/' && input[position+1]=='/') position+=2;
        while(position < input.size() && input[position] != '\n') position++;
        if(position < input.size() && input[position] == '\n') { position++; currentLine++; }
    }

    void skipBlockComment() {
        if(position+1 < input.size() && input[position]=='/' && input[position+1]=='*') position+=2;
        while(position+1 < input.size()) {
            if(input[position]=='\n') currentLine++;
            if(input[position]=='*' && input[position+1]=='/') { position+=2; return; }
            position++;
        }
        position = input.size();
    }

    string getNextWord() {
        size_t start = position;
        while(position < input.size() && (isAlphaNumeric(input[position]) || isUnderscore(input[position]))) position++;
        return input.substr(start, position-start);
    }

    string getNextNumber() {
        size_t start = position;
        while(position < input.size() && isDigit(input[position])) position++;
        return input.substr(start, position-start);
    }

    string getNextOperator() {
        size_t start = position;
        if(start+1 < input.size()) {
            string two = input.substr(start,2);
            if(two=="=="||two=="!="||two=="<="||two==">="||two=="&&"||two=="||") { position+=2; return two; }
        }
        position++;
        return input.substr(start,1);
    }

    string getNextPunctuator() {
        size_t start = position;
        position++;
        return input.substr(start,1);
    }

public:
    LexicalAnalyzer(const string& src) : input(src), position(0), currentLine(1) { initKeywords(); }

    vector<Token> tokenize() {
        vector<Token> tokens;
        while(position < input.size()) {
            char c = input[position];
            if(isWhitespace(c)) {
                if(c=='\n') currentLine++;
                position++;
                continue;
            }
            if(isAlpha(c) || isUnderscore(c)) {
                string w = getNextWord();
                if(keywords.find(w) != keywords.end()) tokens.emplace_back(TokenType::KEYWORD, w, currentLine);
                else tokens.emplace_back(TokenType::IDENTIFIER, w, currentLine);
                continue;
            }
            if(isDigit(c)) {
                string n = getNextNumber();
                tokens.emplace_back(TokenType::INTEGER_LITERAL, n, currentLine);
                continue;
            }
            if(c == '/') {
                if(position+1 < input.size() && input[position+1]=='/') { skipLineComment(); continue; }
                if(position+1 < input.size() && input[position+1]=='*') { skipBlockComment(); continue; }
                string op = getNextOperator();
                tokens.emplace_back(TokenType::OPERATOR, op, currentLine);
                continue;
            }
            if(isOperator(c)) {
                string op = getNextOperator();
                tokens.emplace_back(TokenType::OPERATOR, op, currentLine);
                continue;
            }
            if(isPunctuator(c)) {
                string p = getNextPunctuator();
                tokens.emplace_back(TokenType::PUNCTUATOR, p, currentLine);
                continue;
            }
            // unknown
            tokens.emplace_back(TokenType::UNKNOWN, string(1,c), currentLine);
            position++;
        }
        return tokens;
    }
};

// ---------- Syntax Analyzer ----------
class SyntaxAnalyzer {
private:
    vector<Token> tokens;
    size_t current;
    set<int> errorLines;
    vector<string> errorMessages;

    Token getCurrentToken() {
        if(current < tokens.size()) return tokens[current];
        return Token(TokenType::UNKNOWN, "", -1);
    }
    void advance() { if(current < tokens.size()) current++; }
    bool match(TokenType type, const string& value="") {
        Token t = getCurrentToken();
        if(t.type != type) return false;
        if(!value.empty() && t.value != value) return false;
        return true;
    }

    // 同步到语句边界或函数起点（只用于在出错后快速跳过）
    void synchronizeStmt() {
        while(current < tokens.size()) {
            Token t = getCurrentToken();
            if(t.type == TokenType::PUNCTUATOR && (t.value == ";" || t.value == "}" )) {
                // 吃掉边界分隔符，避免下次停在同一位置
                advance();
                return;
            }
            if(t.type == TokenType::KEYWORD && (t.value == "int" || t.value == "void")) {
                // 到达下一个函数/声明的开头，停在该 token（不要吃掉）
                return;
            }
            advance();
        }
    }

    // expect：如果不匹配，记录错误行号并同步到下一个语句边界
    bool expect(TokenType type, const string& value="") {
        if(match(type,value)) { advance(); return true; }
        Token t = getCurrentToken();
        int lineno = t.line;
        if(lineno <= 0 && !tokens.empty()) lineno = tokens.back().line;
        errorLines.insert(lineno);
        // 记录错误信息（可选）
        string msg = "Line " + to_string(lineno) + ": expected ";
        if(!value.empty()) msg += "'" + value + "'";
        else msg += "token";
        msg += " but got '" + t.value + "'";
        errorMessages.push_back(msg);
        // 同步（跳到 ';' 或 '}' 或 下一个函数定义）
        synchronizeStmt();
        return false;
    }

    void reportErrorAtCurrent(const string& msg) {
        Token t = getCurrentToken();
        int lineno = t.line;
        if(lineno <=0 && !tokens.empty()) lineno = tokens.back().line;
        errorLines.insert(lineno);
        errorMessages.push_back("Line " + to_string(lineno) + ": " + msg);
    }

    // 语法规则
    bool parseCompUnit() {
        // 允许多个函数定义
        while(current < tokens.size()) {
            // 如果当前不是 int/void，跳过直到下一个可能的函数开始
            if(!match(TokenType::KEYWORD, "int") && !match(TokenType::KEYWORD, "void")) {
                // 如果遇到 EOF 跳出
                if(current >= tokens.size()) break;
                // 向前找下一个 int/void
                advance();
                continue;
            }
            if(!parseFuncDef()) {
                // 若解析函数失败，synchronizeStmt 已经把 current 移到合适位置
                // 继续尝试下一个函数
                continue;
            }
        }
        return errorLines.empty();
    }

    // FuncDef → (“int” | “void”) ID “(” (Param (“,” Param)*)? “)” Block
    bool parseFuncDef() {
        if(!match(TokenType::KEYWORD, "int") && !match(TokenType::KEYWORD, "void")) {
            reportErrorAtCurrent("Expected function return type");
            return false;
        }
        advance(); // int or void

        if(!expect(TokenType::IDENTIFIER)) return false;

        if(!expect(TokenType::PUNCTUATOR, "(")) return false;

        if(!match(TokenType::PUNCTUATOR, ")")) {
            if(!parseParam()) return false;
            while(match(TokenType::PUNCTUATOR, ",")) {
                advance();
                if(!parseParam()) return false;
            }
        }

        if(!expect(TokenType::PUNCTUATOR, ")")) return false;

        // parse block (function body)
        return parseBlock();
    }

    // Block → “{” Stmt* “}”
    bool parseBlock() {
        if(!expect(TokenType::PUNCTUATOR, "{")) return false;

        while(!match(TokenType::PUNCTUATOR, "}") && current < tokens.size()) {
            // 如果看见下一个函数的开始，说明 '}' 丢失（函数体未闭合）
            Token t = getCurrentToken();
            if(t.type == TokenType::KEYWORD && (t.value == "int" || t.value == "void")) {
                // 报错位置为当前 token 的行（如样例中第16行）
                errorLines.insert(t.line);
                errorMessages.push_back("Line " + to_string(t.line) + ": missing '}' for previous function");
                return false;
            }

            if(!parseStmt()) {
                // parseStmt 会通过 expect() 在失败时同步至语句边界
                // 这里直接继续以避免死循环
                continue;
            }
        }

        if(!expect(TokenType::PUNCTUATOR, "}")) return false;
        return true;
    }

    // Param → “int” ID
    bool parseParam() {
        if(!expect(TokenType::KEYWORD, "int")) return false;
        if(!expect(TokenType::IDENTIFIER)) return false;
        return true;
    }

    // Stmt 的实现（尽量保持和你原来语法一致）
    bool parseStmt() {
        // Block
        if(match(TokenType::PUNCTUATOR, "{")) return parseBlock();

        // empty stmt
        if(match(TokenType::PUNCTUATOR, ";")) { advance(); return true; }

        // int declaration: int id [= expr] ;
        if(match(TokenType::KEYWORD, "int")) {
            advance();
            if(!expect(TokenType::IDENTIFIER)) return false;
            if(match(TokenType::OPERATOR, "=")) {
                advance();
                if(!parseExpr()) return false;
            }
            return expect(TokenType::PUNCTUATOR, ";");
        }

        // if
        if(match(TokenType::KEYWORD, "if")) {
            advance();
            if(!expect(TokenType::PUNCTUATOR, "(")) return false;
            if(!parseExpr()) return false;
            if(!expect(TokenType::PUNCTUATOR, ")")) return false;
            if(!parseStmt()) return false;
            if(match(TokenType::KEYWORD, "else")) {
                advance();
                if(!parseStmt()) return false;
            }
            return true;
        }

        // while
        if(match(TokenType::KEYWORD, "while")) {
            advance();
            if(!expect(TokenType::PUNCTUATOR, "(")) return false;
            if(!parseExpr()) return false;
            if(!expect(TokenType::PUNCTUATOR, ")")) return false;
            return parseStmt();
        }

        // break/continue
        if(match(TokenType::KEYWORD, "break")) { advance(); return expect(TokenType::PUNCTUATOR, ";"); }
        if(match(TokenType::KEYWORD, "continue")) { advance(); return expect(TokenType::PUNCTUATOR, ";"); }

        // return [expr] ;
        if(match(TokenType::KEYWORD, "return")) {
            advance();
            if(match(TokenType::PUNCTUATOR, ";")) { advance(); return true; }
            if(!parseExpr()) return false;
            return expect(TokenType::PUNCTUATOR, ";");
        }

        // assignment starting with identifier: ID = Expr ;
        if(match(TokenType::IDENTIFIER)) {
            size_t saved = current;
            advance();
            if(match(TokenType::OPERATOR, "=")) {
                advance();
                if(!parseExpr()) return false;
                return expect(TokenType::PUNCTUATOR, ";");
            }
            // not assignment — 回溯并作为 expression ;
            current = saved;
            if(!parseExpr()) return false;
            return expect(TokenType::PUNCTUATOR, ";");
        }

        // otherwise attempt to parse expression statement
        if(!parseExpr()) return false;
        return expect(TokenType::PUNCTUATOR, ";");
    }

    // Expression family (maintain original structure)
    bool parseExpr() { return parseLOrExpr(); }

    bool parseLOrExpr() {
        if(!parseLAndExpr()) return false;
        while(match(TokenType::OPERATOR, "||")) {
            advance();
            if(!parseLAndExpr()) return false;
        }
        return true;
    }

    bool parseLAndExpr() {
        if(!parseRelExpr()) return false;
        while(match(TokenType::OPERATOR, "&&")) {
            advance();
            if(!parseRelExpr()) return false;
        }
        return true;
    }

    bool parseRelExpr() {
        if(!parseAddExpr()) return false;
        while(match(TokenType::OPERATOR, "<") || match(TokenType::OPERATOR, ">") ||
              match(TokenType::OPERATOR, "<=") || match(TokenType::OPERATOR, ">=") ||
              match(TokenType::OPERATOR, "==") || match(TokenType::OPERATOR, "!=")) {
            advance();
            if(!parseAddExpr()) return false;
        }
        return true;
    }

    bool parseAddExpr() {
        if(!parseMulExpr()) return false;
        while(match(TokenType::OPERATOR, "+") || match(TokenType::OPERATOR, "-")) {
            string op = getCurrentToken().value;
            advance();
            // 特殊处理：如果是 '-' 且后面直接是 ';' 或 ')' ，这就是缺少操作数的情况 -> 报错并同步到 ';'
            if(op == "-" && (match(TokenType::PUNCTUATOR, ";") || match(TokenType::PUNCTUATOR, ")"))) {
                // 报当前行错误（如样例中的第6行）
                Token t = getCurrentToken();
                int lineno = t.line;
                if(lineno <= 0 && !tokens.empty()) lineno = tokens.back().line;
                errorLines.insert(lineno);
                errorMessages.push_back("Line " + to_string(lineno) + ": missing operand after '-'");
                // 不尝试继续在本表达式解析；同步到语句边界
                synchronizeStmt();
                return false;
            }
            if(!parseMulExpr()) return false;
        }
        return true;
    }

    bool parseMulExpr() {
        if(!parseUnaryExpr()) return false;
        while(match(TokenType::OPERATOR, "*") || match(TokenType::OPERATOR, "/") || match(TokenType::OPERATOR, "%")) {
            advance();
            if(!parseUnaryExpr()) return false;
        }
        return true;
    }

    bool parseUnaryExpr() {
        if(match(TokenType::OPERATOR, "+") || match(TokenType::OPERATOR, "-") || match(TokenType::OPERATOR, "!")) {
            advance();
            return parseUnaryExpr();
        }
        return parsePrimaryExpr();
    }

    bool parsePrimaryExpr() {
        if(match(TokenType::IDENTIFIER)) {
            advance();
            // 可能是函数调用
            if(match(TokenType::PUNCTUATOR, "(")) {
                advance(); // consume '('
                if(!match(TokenType::PUNCTUATOR, ")")) {
                    if(!parseExpr()) return false;
                    while(match(TokenType::PUNCTUATOR, ",")) {
                        advance();
                        if(!parseExpr()) return false;
                    }
                }
                // expect ')', 若缺失则 expect 会记录错误并同步
                if(!expect(TokenType::PUNCTUATOR, ")")) return false;
                return true;
            }
            return true;
        }

        if(match(TokenType::INTEGER_LITERAL)) { advance(); return true; }

        if(match(TokenType::PUNCTUATOR, "(")) {
            advance();
            if(!parseExpr()) return false;
            if(!expect(TokenType::PUNCTUATOR, ")")) return false;
            return true;
        }

        // 无法识别 primary expression -> 报错并前进一个 token 避免死循环
        Token t = getCurrentToken();
        int lineno = t.line;
        if(lineno <= 0 && !tokens.empty()) lineno = tokens.back().line;
        errorLines.insert(lineno);
        errorMessages.push_back("Line " + to_string(lineno) + ": expected primary expression but got '" + t.value + "'");
        // 向前一步，交由上层决定如何恢复
        advance();
        return false;
    }

public:
    SyntaxAnalyzer(const vector<Token>& toks) : tokens(toks), current(0) {}

    bool analyze() {
        return parseCompUnit();
    }

    set<int> getErrorLines() const { return errorLines; }
    vector<string> getErrorMessages() const { return errorMessages; }
};

// ---------------- main ----------------
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string line, input;
    while(getline(cin, line)) {
        input += line + "\n";
    }

    LexicalAnalyzer lexer(input);
    vector<Token> tokens = lexer.tokenize();

    SyntaxAnalyzer parser(tokens);
    bool ok = parser.analyze();
    set<int> errs = parser.getErrorLines();

    if(ok && errs.empty()) {
        cout << "accept\n";
    } else {
        cout << "reject\n";
        // 按升序逐行输出（每个行号单独一行）
        for(int ln : errs) {
            cout << ln << "\n";
        }
    }
    return 0;
}
