%{  

#include "glo.h"//lex和yacc要共用的头文件，里面包含了一些头文件，重定义了YYSTYPE  
#include<fstream>
extern "C"  
{    
    void yyerror(const char *s);  
    extern int yylex(void);
}  

node *tou;

%}  


%token  NUM ID  IF ELSE INT VOID CONST WHILE BK CONT RETURN ERROR 
%token  OR AND EE EQ NE LESS GREAT LE GE PLUS MINUS TIMES OVER REMAIN INVERSE

%token LPAREN RPAREN LBRACK RBRACK LCBRACK RCBRACK SEMI COM
%token STARTTIME STOPTIME PUTF PUTFSTYLE

%%
compunit : compunit decl {node *k=new node;k->TY=compunit;k->choose=1;k->p[0]=$1.pass; k->p[1]=$2.pass; k->pass=k;$$=*k;tou=k;tou=k;}
	   |compunit funcdef{node *k=new node;k->TY=compunit;k->choose=2;k->p[0]=$1.pass; k->p[1]=$2.pass; k->pass=k;$$=*k;tou=k;tou=k;}
	    |funcdef {node *k=new node;k->TY=compunit;k->choose=3;k->p[0]=$1.pass;  k->pass=k;$$=*k;tou=k;}
	    |decl {node *k=new node;k->TY=compunit;k->choose=4;k->p[0]=$1.pass;  k->pass=k;$$=*k;tou=k;}
	    ;


  
decl       : CONST INT constdefs SEMI  {node *k=new node;k->TY=decl;k->choose=1;k->p[0]=$3.pass;  k->pass=k;$$=*k;}
	    | INT vardefs SEMI         {node *k=new node;k->TY=decl;k->choose=2;k->p[0]=$2.pass;  k->pass=k;$$=*k;}
	    ;
constdefs   : constdefs COM ID constexps EQ constinitval {node *k=new node;k->TY=constdefs;k->choose=1; k->id=$3.id; k->p[2]=$1.pass; k->p[0]=$4.pass; k->p[1]=$6.pass; k->pass=k;$$=*k; }
		| ID constexps EQ constinitval{node *k=new node;k->TY=constdefs;k->choose=2; k->id=$1.id; k->p[0]=$2.pass; k->p[1]=$4.pass;  k->pass=k;$$=*k; }
	    ;

	    
constexps   : constexps LBRACK constexp RBRACK{node *k=new node;k->TY=constexps;k->choose=1;k->p[0]=$1.pass; k->p[1]=$3.pass; k->pass=k;$$=*k;}
	    | {node *k=new node;k->TY=constexps;k->choose=2; k->pass=NULL;$$=*k;}
	    ;
constinitval: constexp  {node *k=new node;k->TY=constinitval;k->choose=1;k->p[0]=$1.pass;  k->pass=k;$$=*k;}
	    | LCBRACK constinitvals RCBRACK{node *k=new node;k->TY=constinitval;k->choose=2;k->p[0]=$2.pass;  k->pass=k;$$=*k;}				
	    | LCBRACK RCBRACK   {node *k=new node;k->TY=constinitval;k->choose=3;  k->pass=k;$$=*k;}						                                                 
	    ;
constinitvals: constinitvals COM constinitval{node *k=new node;k->TY=constinitvals;k->choose=1;k->p[0]=$1.pass; k->p[1]=$3.pass; k->pass=k;$$=*k;}
	    | constinitval    {node *k=new node;k->TY=constinitvals;k->choose=2;k->p[0]=$1.pass;  k->pass=k;$$=*k;}   
	    
	    ;

vardefs     : vardefs COM vardef {node *k=new node;k->TY=vardefs;k->choose=1;k->p[0]=$1.pass; k->p[1]=$3.pass; k->pass=k;$$=*k;}
	    | vardef   {node *k=new node;k->TY=vardefs;k->choose=2;k->p[0]=$1.pass;  k->pass=k;$$=*k;}
	    ;
vardef    : ID constexps EQ initval  {node *k=new node;k->TY=vardef;k->choose=1; k->id=$1.id; k->p[0]=$2.pass; k->p[1]=$4.pass;  k->pass=k;$$=*k; }
	    | ID constexps           {node *k=new node;k->TY=vardef;k->choose=2; k->id=$1.id; k->p[0]=$2.pass;   k->pass=k;$$=*k; }
	    ;
initval: exp     {node *k=new node;k->TY=constinitval;k->choose=1;k->p[0]=$1.pass;  k->pass=k;$$=*k;}
	    | LCBRACK initvals RCBRACK	 {node *k=new node;k->TY=constinitval;k->choose=2;k->p[0]=$2.pass;  k->pass=k;$$=*k;}				
	    | LCBRACK RCBRACK		{node *k=new node;k->TY=constinitval;k->choose=3;  k->pass=k;$$=*k;}		
	    ;
initvals: initvals COM initval  {node *k=new node;k->TY=constinitvals;k->choose=1;k->p[0]=$1.pass; k->p[1]=$3.pass; k->pass=k;$$=*k;}
	    | initval        {node *k=new node;k->TY=constinitvals;k->choose=2;k->p[0]=$1.pass;  k->pass=k;$$=*k;} 
	    ;


funcdef    : VOID ID LPAREN funcparams RPAREN block {node *k=new node;k->TY=funcdef;k->choose=1;k->p[0]=$4.pass;k->p[1]=$6.pass; k->id=$2.id; k->pass=k;$$=*k;}
	   | INT ID LPAREN funcparams RPAREN block {node *k=new node;k->TY=funcdef;k->choose=2;k->p[0]=$4.pass;k->p[1]=$6.pass; k->id=$2.id; k->pass=k;$$=*k;}
	   | VOID ID LPAREN RPAREN block {node *k=new node;k->TY=funcdef;k->choose=3;k->p[1]=$5.pass; k->id=$2.id; k->pass=k;$$=*k;}
	   | INT ID LPAREN RPAREN block  {node *k=new node;k->TY=funcdef;k->choose=4;k->p[1]=$5.pass; k->id=$2.id; k->pass=k;$$=*k;}        
	   ;

funcparams : funcparams COM funcparam {node *k=new node;k->TY=funcparams;k->choose=1;k->p[0]=$1.pass;k->p[1]=$3.pass;  k->pass=k;$$=*k;}
	   | funcparam{node *k=new node;k->TY=funcparams;k->choose=2;k->p[0]=$1.pass;  k->pass=k;$$=*k;}
	   ;
funcparam : INT ID {node *k=new node;k->TY=funcparam;k->choose=1;k->id=$2.id;  k->pass=k;$$=*k;}
	 | INT ID arrysize{node *k=new node;k->TY=funcparam;k->choose=2;k->id=$2.id; k->p[0]=$3.pass; k->pass=k;$$=*k;}
	  ;
arrysize : arrysize LBRACK exp RBRACK{node *k=new node;k->TY=arrysize;k->choose=1; k->p[0]=$1.pass;k->p[1]=$3.pass; k->pass=k;$$=*k;}
	 | LBRACK RBRACK{node *k=new node;k->TY=arrysize;k->choose=2;  k->pass=NULL;$$=*k;}
	 ;
	  

block : LCBRACK blockitem RCBRACK  {node *k=new node;k->TY=block;k->choose=1;k->p[0]=$2.pass;  k->pass=k;$$=*k;}
	;

blockitem: blockitem decl  {node *k=new node;k->TY=blockitem;k->choose=1;k->p[0]=$1.pass; k->p[1]=$2.pass; k->pass=k;$$=*k;}
	 | blockitem stmt  {node *k=new node;k->TY=blockitem;k->choose=2;k->p[0]=$1.pass; k->p[1]=$2.pass; k->pass=k;$$=*k;}
	 | 		   {node *k=new node;k->TY=blockitem;k->choose=3;  k->pass=NULL;$$=*k;}
	 ;

stmt : ID exps EQ exp SEMI{node *k=new node;k->TY=stmt;k->choose=1;k->p[0]=$2.pass;k->p[1]=$4.pass; k->id=$1.id; k->pass=k;$$=*k;}
	| exp SEMI	{node *k=new node;k->TY=stmt;k->choose=2;k->p[0]=$1.pass;  k->pass=k;$$=*k;}
	| SEMI 			{node *k=new node;k->TY=stmt;k->choose=3; k->pass=k;$$=*k;}
	| block 		{node *k=new node;k->TY=stmt;k->choose=4; k->p[0]=$1.pass;k->pass=k;$$=*k;}
	| IF LPAREN cond RPAREN stmt ELSE stmt 			{node *k=new node;k->TY=stmt;k->choose=5;k->p[0]=$3.pass;k->p[1]=$5.pass;k->p[2]=$7.pass;  k->pass=k;$$=*k;}
	| IF LPAREN cond RPAREN stmt 				{node *k=new node;k->TY=stmt;k->choose=6;k->p[0]=$3.pass;k->p[1]=$5.pass;  k->pass=k;$$=*k;}
	| WHILE LPAREN cond RPAREN stmt 			{node *k=new node;k->TY=stmt;k->choose=7;k->p[0]=$3.pass;k->p[1]=$5.pass;  k->pass=k;$$=*k;}
	| BK SEMI			{node *k=new node;k->TY=stmt;k->choose=8; k->pass=k;$$=*k;}
	| CONT SEMI			{node *k=new node;k->TY=stmt;k->choose=9; k->pass=k;$$=*k;}
	| RETURN exp SEMI		{node *k=new node;k->TY=stmt;k->choose=10;k->p[0]=$2.pass; k->pass=k;$$=*k;}
	| RETURN SEMI			{node *k=new node;k->TY=stmt;k->choose=11; k->pass=k;$$=*k;}
	
	;

cond : lorexp {node *k=new node;k->TY=cond;k->choose=1;k->p[0]=$1.pass;  k->pass=k;$$=*k;}
	;

constexp : addexp {node *k=new node;k->TY=constexp;k->choose=1;k->p[0]=$1.pass;  k->pass=k;$$=*k;}
	;
exp : addexp {node *k=new node;k->TY=exp;k->choose=1;k->p[0]=$1.pass;  k->pass=k;$$=*k;}
	;

exps   :   exps LBRACK exp RBRACK     {node *k=new node;k->TY=exps;k->choose=1;  k->p[0]=$1.pass;k->p[1]=$3.pass; k->val=$1.val+1;  k->pass=k; $$=*k;}                  
	    | {node *k=new node;k->TY=exps;k->choose=2;   k->val=0;  k->pass=NULL; $$=*k;}
	    ;

primaryexp    : LPAREN exp RPAREN     {$$=$2;}                  // '('exp')'
 		|ID exps {node *k=new node;k->TY=primaryexp;k->choose=2; k->id=$1.id; k->p[0]=$2.pass; k->val=$2.val;  k->pass=k; $$=*k;}
	    	| NUM {node *k=new node;k->TY=primaryexp;k->choose=3; k->val=$1.val;    k->pass=k; $$=*k;}
		
	   	 ;

functionrparams: functionrparams COM exp{node *k=new node;k->TY=functionrparams;k->choose=1;  k->p[0]=$1.pass;k->p[1]=$3.pass; k->val=$1.val+1;  k->pass=k; $$=*k;} 
	       | exp                    {node *k=new node;k->TY=functionrparams;k->choose=2;  k->p[0]=$1.pass; k->val=1;  k->pass=k; $$=*k;} 
	       ;
unaryexp    : primaryexp {$$=$1;}
		| unaryop unaryexp {node *k=new node;k->TY=unaryexp;k->choose=2; k->p[0]=$1.pass;k->p[1]=$2.pass;     k->pass=k; $$=*k;}
		| ID LPAREN RPAREN {node *k=new node;k->TY=unaryexp;k->choose=3; k->id=$1.id;  k->val=$1.val;  k->pass=k; $$=*k;}
	    	| ID LPAREN functionrparams RPAREN  {node *k=new node;k->TY=unaryexp;k->choose=4; k->id=$1.id; k->p[0]=$3.pass; k->val=$3.val;  k->pass=k; $$=*k;}
		//| PUTF LPAREN PUTFSTYLE COM functionrparams RPAREN {cout<<"----unaryexp"<<endl;node *k=new node;k->TY=unaryexp;k->choose=5;  k->id="putf";k->p[0]=$5.pass; k->val=$5.val;  k->pass=k; $$=*k;}
	     ;
unaryop   : PLUS{node *k=new node;k->TY=unaryop;k->choose=1; k->opsymbol="+";   k->pass=k;$$=*k;}
	|  MINUS{node *k=new node;k->TY=unaryop;k->choose=2;  k->opsymbol="-";   k->pass=k;$$=*k;}
	|  INVERSE{node *k=new node;k->TY=unaryop;k->choose=3;  k->opsymbol="!";   k->pass=k;$$=*k;}
	;
mulexp      : unaryexp {$$=$1;}
	    | mulexp TIMES unaryexp{node *k=new node;k->TY=addexp;k->choose=1; k->p[0]=$1.pass;k->p[1]=$3.pass; k->opsymbol="*";   k->pass=k;$$=*k;}
	    | mulexp OVER unaryexp {node *k=new node;k->TY=addexp;k->choose=1; k->p[0]=$1.pass;k->p[1]=$3.pass; k->opsymbol="/";   k->pass=k;$$=*k;}
	    | mulexp REMAIN unaryexp{node *k=new node;k->TY=addexp;k->choose=1; k->p[0]=$1.pass;k->p[1]=$3.pass; k->opsymbol="%";   k->pass=k;$$=*k;}
	    ;
addexp    : mulexp{$$=$1;}
	  | addexp PLUS mulexp {node *k=new node;k->TY=addexp;k->choose=1;k->p[0]=$1.pass;k->p[1]=$3.pass; k->opsymbol="+"; k->pass=k;$$=*k; }
	  | addexp MINUS mulexp{node *k=new node;k->TY=addexp;k->choose=1; k->p[0]=$1.pass;k->p[1]=$3.pass; k->opsymbol="-";   k->pass=k;$$=*k;}
	  ;
relexp   : addexp {$$=$1;}
	| relexp LESS  addexp  {node *k=new node;k->TY=addexp;k->choose=1; k->p[0]=$1.pass;k->p[1]=$3.pass; k->opsymbol="<";   k->pass=k;$$=*k;}
	| relexp GREAT  addexp {node *k=new node;k->TY=addexp;k->choose=1; k->p[0]=$1.pass;k->p[1]=$3.pass; k->opsymbol=">";   k->pass=k;$$=*k;}
	| relexp LE  addexp    {node *k=new node;k->TY=addexp;k->choose=1; k->p[0]=$1.pass;k->p[1]=$3.pass; k->opsymbol="<=";   k->pass=k;$$=*k;}
	| relexp  GE addexp    {node *k=new node;k->TY=addexp;k->choose=1; k->p[0]=$1.pass;k->p[1]=$3.pass; k->opsymbol=">=";   k->pass=k;$$=*k;}
	;

eqexp   : relexp{$$=$1;}
	| eqexp EE relexp      {node *k=new node;k->TY=addexp;k->choose=1; k->p[0]=$1.pass;k->p[1]=$3.pass; k->opsymbol="==";   k->pass=k;$$=*k;}
	| eqexp NE relexp      {node *k=new node;k->TY=addexp;k->choose=1; k->p[0]=$1.pass;k->p[1]=$3.pass; k->opsymbol="!=";   k->pass=k;$$=*k;}
	;
landexp  : eqexp{$$=$1;}
	| landexp AND eqexp    {node *k=new node;k->TY=addexp;k->choose=1; k->p[0]=$1.pass;k->p[1]=$3.pass; k->opsymbol="&&";   k->pass=k;$$=*k;}
	;
lorexp  : landexp{$$=$1;}
	| lorexp OR  landexp   {node *k=new node;k->TY=addexp;k->choose=1; k->p[0]=$1.pass;k->p[1]=$3.pass; k->opsymbol="||";   k->pass=k;$$=*k;}
	;



%%  
extern map<string, value> mp[3];
extern map<string,value>::iterator iter;
extern int nowmap;
extern queue<string>quchar[2];
void yyerror(const char *s)  
{  
    cerr<<s<<endl; 
}  



int main(int argc, char* argv[])  
{  
    const char* sFile=argv[2];//打开要读取的文本文件  "file.c";
	
    FILE* fp=fopen(sFile, "r");  
    if(fp==NULL)  
    {  
        printf("cannot open %s\n", sFile);  
        return -1;  
    }      
    extern FILE* yyin;   
    yyin=fp;//yacc会从yyin读取输入，yyin默认是标准输入，
            //这里改为磁盘文件。yacc默认向yyout输出，可修改yyout改变输出目的  

     
    yyparse();//使yacc开始读取输入和解析，它会调用lex的yylex()读取记号  
    
	fclose(fp); 
	ofstream outfile(argv[4]);
	//ofstream outfile("/home/guate/snap/MiniVM/build/ofile.s");
	//ofstream outfile("/home/guate/runn/output.s");
     	streambuf *oldbuf = cout.rdbuf(); 
	inser();       
	cout.rdbuf(outfile.rdbuf());
	tou->doo(0,NULL);
	while(!quchar[0].empty())
{  cout<<quchar[0].front()<<endl; quchar[0].pop(); }
	cout.rdbuf(oldbuf);
	//iter=mp[1].find("y");
//if(iter!=mp[nowmap].end())
//cout<<iter->second.b[0]<<' '<<iter->second.b[1]<<' '<<iter->second.b[3]<<endl;
    return 0;  
}  
