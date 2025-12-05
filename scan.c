#include "defs.h"
#include "data.h"
#include "decl.h"

// return the position of character c
static int chrpos(char *s, int c) {
    char *p;

    p = strchr(s, c);
    return (p? p-s:-1);
}

// Store an unwanted character
static void putback(int c){
    Putback = c; 
}

// Get the next character from the input file
static int next(void) {
    int c;

    if(Putback){
        c = Putback;
        Putback = 0;
        return c;
    }

    c = fgetc(Infile);
    if('\n' == c)
        Line ++;

    return c;
}

// skip the whitespace
static int skip(void) {
    int c;
    c = next();

    while(' ' == c || '\n' == c || '\t' == c || '\r' == c || '\f' == c)
        c = next();
    
    return c;
}

int scanint(int c) {
    int k, val = 0;

    while((k = chrpos("0123456789", c)) >= 0) {
        val = val * 10 + k;
        c = next();
    }

    putback(c);
    return val;
}

int scan(struct token* t){
    int c;

    // skip whitespace
    c = skip();

    switch(c) {
        case EOF:
            return (0);
        case '+':
            t->token = T_PLUS;
            break;
        case '-':
            t->token = T_MINUS;
            break;
        case '*':
            t->token = T_STAR;
            break;
        case '/':
            t->token = T_SLASH;
            break;
        default:
            if(isdigit(c)) {
                t -> intvalue = scanint(c);
                t -> token = T_INTLIT;
                break;
            }

            printf("Unrecognized character %c on line %d\n", c, Line);
            exit(1);
    }
}