
#ifndef MAIN_HPP  
#define MAIN_HPP  

#include <iostream>
#include <string>  
#include <stdio.h> 
#include <map>
#include<queue>
using namespace std;  


struct value
{
    char name;      
    int namenum;
    int* a=NULL;  //const数组值
    int* b=NULL;  //数组格式
    int arr; //是否是数组
    int val;  //值
    int cons; //是否是const
};
enum type {compunit,exp,addexp,primaryexp,constdefs,constexps,constinitval,constinitvals,constexp,exps,vardef,vardefs,decl,funcdef,funcparams,funcparam,block,blockitem,stmt,arrysize, unaryexp,functionrparams,cond,unaryop}; 

class node
{  
	public:
    node *pass=NULL;   
    node *p[3];         //子节点
    type TY;           //左侧非终结符号类型
    int choose;        //同一个非终结符号的产生式选择
    string id;         
    int val;  
    string opsymbol; 
	void doo(int,int*);
    int varnum2,temvarnum2;
};  

void inser();
int retu();
#define YYSTYPE node//把YYSTYPE(即yylval变量)重定义为node类型，这样lex就能向yacc返回更多的数据了  


#endif  



