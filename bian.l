%{

#include "glo.h"
#include "past2.tab.h" 

extern "C"  
{   
    int yywrap(void);  
    int yylex(void);  
}  

int num();
int Onum();
int Hnum();
 //ID  NUM 
%}

%x COMMENT
%x COMMENTS

digit    [0-9]
nonzero_digit    [1-9]
octal_digit    [0-7]
hex_digit    [0-9a-fA-F]
nondigit    [a-zA-Z]
idnondigit    [_a-zA-Z]
blank    [ \t\v\f\r]+
newline    \n
 
%%
"//" {cout<<"//   "<<yylineno<<endl;BEGIN COMMENT;}
"/*"  {cout<<"/*   "<<yylineno<<endl;BEGIN COMMENTS;}
"int" {cout<<"int"<<endl;return INT;}
"void" {cout<<"int"<<endl;return VOID;}
"const" {cout<<"const"<<endl;return CONST;}
"if" {cout<<"if"<<endl;return IF;}
"else" {cout<<"else"<<endl;return ELSE;}
"while" {cout<<"while"<<endl;return WHILE;}
"break" {cout<<"break"<<endl;return BK;}
"continue" {cout<<"continue"<<endl;return CONT;}
"return" {cout<<"return"<<endl;return RETURN;}
"||" {cout<<"||"<<endl;return OR;}
"&&" {cout<<"&&"<<endl;return AND;}
"==" {cout<<"=="<<endl;return EE;}
"="  {cout<<"="<<endl;return EQ;}
"!=" {cout<<"!="<<endl;return NE;}
"<" {cout<<"<"<<endl;return LESS;}
">" {cout<<">"<<endl;return GREAT;}
"<=" {cout<<"<="<<endl;return LE;}
">=" {cout<<">="<<endl;return GE;}
"+" {cout<<"+"<<endl;return PLUS;}
"-" {cout<<"-"<<endl;return MINUS;}
"*" {cout<<"*"<<endl;return TIMES;}
"/" {cout<<"/"<<endl;return OVER;}
"%" {cout<<"%"<<endl;return REMAIN;}
"!" {cout<<"!"<<endl;return INVERSE;}
";"  {cout<<";"<<endl;return SEMI;}
"["  {cout<<"["<<endl;return LBRACK;}
"]"  {cout<<"]"<<endl;return RBRACK;}
"("  {cout<<"("<<endl;return LPAREN;}
")"  {cout<<")"<<endl;return RPAREN;}
"{"  {cout<<"{"<<endl;return LCBRACK;}
"}"  {cout<<"}"<<endl;return RCBRACK;} 
","  {cout<<","<<endl;return COM;}
{idnondigit}({digit}|{idnondigit})*    {yylval.id=yytext;yylval.val=yylineno;cout<<"ID-------"<<yylineno<<endl;return ID;}
{nonzero_digit}{digit}*    {yylval.val=num();return NUM;}
"0"{octal_digit}*    {yylval.val=Onum();return NUM;}
("0x"|"0X"){hex_digit}+    {yylval.val=Hnum();return NUM;}
{blank} {cout<<"blank"<<endl;}
{newline}  {cout<<"newline"<<endl;yylineno++;}
.   {return ERROR;}
<COMMENT>{newline}   {cout<<"//END   "<<yylineno<<endl;   yylineno++;//注释状态下的规则，只有当前切换到COMMENT状态才会去匹配  
            BEGIN INITIAL;//在注释状态下，当遇到换行符时，表明注释结束了，返回初始态  
        }
<COMMENT>.   {   //注释状态下的规则，只有当前切换到COMMENT状态才会去匹配  
            
        }
<COMMENTS>"*/"   {   cout<<"*/   "<<yylineno<<endl;  
            BEGIN INITIAL;
        }
<COMMENTS>{newline}   {  yylineno++;   
            
        }
<COMMENTS>.   {     
            
        }

%%
int num()
{
    
int y=0,i=0;
    
    while(yytext[i]!='\0')
    {
      y=y*10+(yytext[i]-'0');
      i++;
    }
    cout<<y<<endl;
    return y;
}

int Onum()
{
    
int y=0,i=0;
    
    while(yytext[i]!='\0')
    {
      y=y*8+(yytext[i]-'0');
      i++;
    }
    cout<<y<<endl;
    return y;
}

int Hnum()
{
    
int y=0,i=2;
    
    while(yytext[i]!='\0')
    {
char x = yytext[i];
if((x-'0')>=0&&('9'-x)>=0)
{
      y=y*16+(x-'0');
      i++;
}
if((x-'a')>=0&&('f'-x)>=0)
{
      y=y*16+(x-'a')+10;
      i++;
}
if((x-'A')>=0&&('F'-x)>=0)
{
      y=y*16+(x-'A')+10;
      i++;
}
    }
    cout<<y<<endl;
    return y;
}


int yywrap(void)  
{  
    puts("-----the file is end");  
    return 1; 
} 
