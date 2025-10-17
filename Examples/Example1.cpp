#include<bits/stdc++.h>
#define endl '\n'
using namespace std;
char c; 
int Line = 1; 
string ans = "",tem = ""; 
bool EFlag = 0,PositiveNegativeFlag; 
set<char> Bracket = { '(', ')', '{', '}', '[', ']', ';', ',', '"' };
set<char> PositiveNegative = {'+','-'}; 
set<char> OneOperator = {'+','-','*','/','>','<','=','|','!'}; 
set<char> TypeIdentify = {'%','&'};  
set<char> BlankCharacter = {' ','\f','\r','\t','\v'}; 
set<string> TwoOperator = {"+=","-=","*=","/=","<=",">=","==","<<",">>"};
set<string> KeyWord = {"main","double","return","float","if","else","do","while","for","scanf","printf","char","sqrt","abs" };
set<string> Type = {"int","double","short","char","void","long"};
void MyPrint(string newstr){
    string str = "line" + to_string(Line) + ":(" + newstr + ", " + tem + ")\n";
    ans += str; 
} 

int main(){  
    c = getchar();
StartLabel: 
    if(c == EOF) goto EndOfFileLabel;
    if(BlankCharacter.count(c)){
        c = getchar();
        goto StartLabel;
    }
    tem = "";
    PositiveNegativeFlag = 0;
    EFlag = 0;
    if(c == '\n') {
        Line++;  
        c = getchar();
        goto StartLabel; 
    }
    if( isalpha(c) || c == '_' ) goto AlphaLabel;            //字母 
    else if( isdigit(c) ) goto IntegerLabel;                 //数字 
    else if( OneOperator.count(c) ) goto OperatorLabel;      //运算符 
    else if( Bracket.count(c) ) goto BracketLabel;           //间隔符 
    else if( TypeIdentify.count(c) ) goto TypeIdentifyLabel;
    
IntegerLabel:  //数字(无符号整形)
    tem += c;
    c = getchar();
    if( c=='.' )  goto DecimalLabel;  
    else if(isdigit(c)) goto IntegerLabel;
    else if( c == 'e' || c == 'E') {
        EFlag = 1; 
        goto EintegerLabel;
    }
    else{
        MyPrint("integer");
        goto StartLabel;
    }
    
EintegerLabel: //整形科学计数法 
    tem += c;
    if(c == 'e' || c == 'E'){//第一个e后面必须加数字否则非法 
        c = getchar();
        if(PositiveNegative.count(c) && PositiveNegativeFlag == 0){
            PositiveNegativeFlag = 1; 
            goto EintegerLabel;
        }
        else if(isdigit(c)){
            goto EintegerLabel;
        } 
        else {
            MyPrint("Wrong"); 
            goto StartLabel;
        }
    } 
    c = getchar(); 
    if(isdigit(c)) {
        goto EintegerLabel;
    }
    else if( PositiveNegative.count( tem[tem.size()-1] ) ){
        MyPrint("Wrong"); 
        goto StartLabel;
    }
    else {
        MyPrint("integer"); 
        goto StartLabel;        
    } 
         
    
DecimalLabel:  //小数 
    tem += c;
    if(c=='.'){  //后面必须加数字 
        c = getchar();
        if(isdigit(c)) goto DecimalLabel; 
        else{
            MyPrint("Wrong");
            goto StartLabel;
        }
    } 
    c = getchar();
    if( isdigit(c) ) { 
        goto DecimalLabel;
    }
    else if( c == 'E' || c == 'e' ){
        goto EdecimalLabel;
    }
    else{
        MyPrint("decimal"); 
        goto StartLabel;
    }
EdecimalLabel:
    tem += c;
    //E后面必须加数字或者+-号
    if( c == 'e' || c == 'E' ) {
        c = getchar();
        if(PositiveNegative.count(c) && PositiveNegativeFlag == 0){
            PositiveNegativeFlag = 1; 
            goto EdecimalLabel;
        }
        else if(isdigit(c)){
            goto EdecimalLabel;
        } 
        else {
//            MyPrint("Wrong"); //浮点输出这样写，其他的不知道 ******************************************************************************************
            ans = "Error at Line "+ to_string(3) + ": Illegal floating point number \""+ tem + "\".";
            cout<<ans<<endl;
            return 0;
        }
    }
    c = getchar(); 
    if(isdigit(c)) {
        goto EdecimalLabel;
    }
    else if( PositiveNegative.count( tem[tem.size()-1] ) ){
        MyPrint("Wrong"); 
        goto StartLabel;
    }
    else {
        MyPrint("float"); 
        goto StartLabel;        
    } 
    
AlphaLabel://字母(字符串)
    tem += c; 
    c = getchar();
    if( isalpha(c) || c=='_' || isdigit(c) ) goto AlphaLabel;
    else{
        if( Type.count(tem) ){
            MyPrint("type");  
        }
        else if( KeyWord.count(tem) ){
            MyPrint("keyword");
        }
        else{
            MyPrint("identify");
        }
        goto StartLabel;
    } 
OperatorLabel://运算符 
     tem += c; 
     c = getchar();  
    if( TwoOperator.count(tem+c) ){
        tem += c;
        MyPrint("OPT");
        c = getchar();
        goto StartLabel; 
    }
    else if( tem+c == "/*" ){
        c = getchar();
        while(c!='*' && c!=EOF)  c = getchar();
        while(c!='/' && c!=EOF) c= getchar();
        if( c== EOF) {cout<<"注释匹配错误"<<endl; goto EndOfFileLabel;} 
        c = getchar();
        goto StartLabel;
    }
    else if( tem+c =="//" ){
        c = getchar();
        while(c!='/n' && c != EOF) 
            c = getchar();
        if(c == EOF){ goto EndOfFileLabel;} 
    } 
    else{  
        MyPrint("OPT");
        goto StartLabel; 
    }
TypeIdentifyLabel:
    tem +=c;
    c = getchar();
    if( isalpha(c) || c=='_' || isdigit(c) ) goto TypeIdentifyLabel;
    else{
        if(tem.size() == 1){
            MyPrint("Wrong");
            goto StartLabel;
        }else{
            MyPrint("typeidentify");
            goto StartLabel;
        }
    } 
    
BracketLabel://间隔符 
    tem += c;
    c = getchar();
    MyPrint("bracket");
    goto StartLabel;
    
EndOfFileLabel:
    cout<<ans<<endl; 
    return 0;
}

/************************ 
y=-2;
y=a-2;
*************************/