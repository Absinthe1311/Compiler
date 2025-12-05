#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// token type
// T_INTLIT integer literal
enum{
    T_PLUS, T_MINUS, T_STAR, T_SLASH, T_INTLIT
};

// AST node type
enum{
    A_ADD, A_SUBTRACT, A_MULTIPLY, A_DIVIDE, A_INTLIT
};

// AST structure
struct ASTnode{
    int op;
    struct ASTnode* left;
    struct ASTnode* right;
    int intvalue;
};

// structure for token
struct token {
    int token;
    int intvalue;
};