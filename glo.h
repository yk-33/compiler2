
#ifndef MAIN_HPP  
#define MAIN_HPP  

#include <iostream>//使用C++库  
#include <string>  
#include <stdio.h>//printf和FILE要用的  
#include <map>
#include<queue>
using namespace std;  

/*当lex每识别出一个记号后，是通过变量yylval向yacc传递数据的。默认情况下yylval是int类型，也就是只能传递整型数据。 
yylval是用YYSTYPE宏定义的，只要重定义YYSTYPE宏，就能重新指定yylval的类型(可参见yacc自动生成的头文件yacc.tab.h)。 
在我们的例子里，当识别出标识符后要向yacc传递这个标识符串，yylval定义成整型不太方便(要先强制转换成整型，yacc里再转换回char*)。 
这里把YYSTYPE重定义为struct Type，可存放多种信息*/ 

struct value
{
    char name;
    int namenum;
    int* a=NULL;  //const数组值
    int* b=NULL;  //数组格式
    int arr; //是否是数组
    int val;
    int cons;
};
enum type {compunit,exp,addexp,primaryexp,constdefs,constexps,constinitval,constinitvals,constexp,exps,vardef,vardefs,decl,funcdef,funcparams,funcparam,block,blockitem,stmt,arrysize, unaryexp,functionrparams,cond,unaryop}; 

class node//通常这里面每个成员，每次只会使用其中一个，一般是定义成union以节省空间(但这里用了string等复杂类型造成不可以)  
{  
	public:
    node *pass=NULL;
    node *p[3];
    type TY;
    int choose;
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



