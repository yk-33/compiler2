%{

#include "glo.h"//lex和yacc要共用的头文件，里面包含了一些头文件，重定义了YYSTYPE  
#include "past2.tab.h"//用yacc编译yacc.y后生成的C头文件，内含%token、YYSTYPE、yylval等定义(都是C宏)，供lex.yy.c和yacc.tab.c使用  

extern "C"//为了能够在C++程序里面调用C函数，必须把每一个需要使用的C函数，其声明都包括在extern "C"{}块里面，这样C++链接时才能成功链接它们。extern "C"用来在C++环境下设置C链接类型。  
{   //yacc.y中也有类似的这段extern "C"，可以把它们合并成一段，放到共同的头文件main.h中  
    int yywrap(void);  
    int yylex(void);//这个是lex生成的词法分析函数，yacc的yyparse()里会调用它，如果这里不声明，生成的yacc.tab.c在编译时会找不到该函数  
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
<COMMENTS>"*/"   {   cout<<"*/   "<<yylineno<<endl;//注释状态下的规则，只有当前切换到COMMENT状态才会去匹配  
            BEGIN INITIAL;//在注释状态下，当遇到换行符时，表明注释结束了，返回初始态  
        }
<COMMENTS>{newline}   {  yylineno++; //注释状态下的规则，只有当前切换到COMMENT状态才会去匹配  
            
        }
<COMMENTS>.   {   //注释状态下的规则，只有当前切换到COMMENT状态才会去匹配  
            
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
    return 1;//返回1表示读取全部结束。如果要接着读其它文件，可以这里fopen该文件，
             //文件指针赋给yyin，并返回0  
} 
