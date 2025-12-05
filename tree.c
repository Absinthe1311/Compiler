#include "defs.h"
#include "data.h"
#include "decl.h"

// Build and return a generic AST node
struct ASTnode *mkastnode(int op, struct ASTnode* left, struct ASTnode* right, int intvalue)
{
    struct ASTnode *n;

    // Malloc a new ASTnode
    n = (struct ASTnode *) malloc(sizeof(struct ASTnode));
    if(n == NULL)
    {
        fprintf(stderr, "Unable to malloc in mkastnode()");
        exit(1);
    }

    // copy in the field values and return 
    n->op = op;
    n->left = left;
    n->right = right;
    n->intvalue = intvalue;

    return n;
}

struct ASTnode *mkastleaf(int op, int intvalue) {
    return (mkastnode(op, NULL, NULL, intvalue));
}

struct ASTnode *mkastunary(int op, struct ASTnode* left, int intvalue) {
    return mkastnode(op, left, NULL, intvalue);
}