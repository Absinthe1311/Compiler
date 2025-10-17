%{
#include <stdio.h>
int word_count = 0;
%}

%%
[ \t\n]+          ;     // 忽略空白符
[A-Za-z]+         { word_count++; }
.                 ;
%%

int main(void) {
    yylex();
    printf("Word count: %d\n", word_count);
    return 0;
}

