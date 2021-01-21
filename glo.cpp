#include "glo.h"
#include <iostream>
#include<map>
#include<stack>
#include<queue>
#include<vector>
using namespace std;

#define MAXL 200
#define eeyore 0
#define tigger 1
#define riscv 2

map<string, value> mp[10];
map<string,value>::iterator iter;
int nowmap=0;

stack<char> stkchar;
stack<int> stkint;
stack<char> stkchar2;
stack<int> stkint2;
queue<string> quchar[2],qustore;
queue< node* > qunode;

int status=0;    
int varnum=0;    //变量T
int temvarnum=0; //临时变量t
int returnnum[3];
char returnvar;
int A[MAXL]; //数组维度
char B[MAXL];//数组格式
int pos;  //数组位置
int pos2;  //数组位置
int Rnum; //可匹配右括号
int pnum; //记参数个数
int label=0; //标签
int brlab=-1; //break跳转
int conlab=-1; //continue跳转
int havewhile=0; //在while里
int modes =1;  //编译生成模式
int func1 = true; 
int sizeForRisc; //翻译函数的大小
void inser()
{	value tes{'T',0,NULL,NULL,4,0,0};
	mp[0]["f_getint"]=tes;
	value tes2{'T',0,NULL,NULL,4,0,0};
	mp[0]["f_getch"]=tes2;
	value tes3{'T',0,NULL,NULL,4,0,0};
	mp[0]["f_getarray"]=tes3;
	value tes4{'T',0,NULL,NULL,3,0,0};
	mp[0]["f_putint"]=tes4;
	value tes5{'T',0,NULL,NULL,3,0,0};
	mp[0]["f_putch"]=tes5;
	value tes6{'T',0,NULL,NULL,3,0,0};
	mp[0]["f_putarray"]=tes6;
	value tes7{'T',0,NULL,NULL,3,0,0};
	mp[0]["f_starttime"]=tes7;
	value tes8{'T',0,NULL,NULL,3,0,0};
	mp[0]["f_stoptime"]=tes8;
	value tes9{'T',0,NULL,NULL,3,0,0};
	mp[0]["f_putf"]=tes9;
	
}
int retu()
{
	iter=mp[0].find("ui");
	if(iter!=mp[0].end()) return iter->second.namenum;
	else return -1;
}

void draw(int i,int max)
{
	if(i==0)
	{
		B[pos++]='{';
		draw(i+1,max);
		B[pos++]='}';
		B[pos]='\0';
	}
	else
	{
		if(i==max)
		{
			for(int j=1;j<=A[i];j++)
			B[pos++]='0';
		}
		else
		{
			for(int j=1;j<=A[i];j++)
			{
				B[pos++]='{';
				draw(i+1,max);
				B[pos++]='}';
			}
		}
		
	}
	
}
//////////////////////////////////////////////////////////////////////////////// 
//                         RISC -V
string regName[28] = { "a0","a1","a2","a3","a4","a5","a6","a7","t0","t1","t2","t3","t4","t5","t6","s0","s1","s2" ,
	"s3","s4","s5" ,"s6","s7","s8" ,"s9","s10","s11" ,"x0"};

void risc_func(string name)
{
	cout<<endl;
	cout<<"	.text"<<endl;
	cout<<"	.align 2"<<endl;
	cout<<"	.global "<<name<<endl;
	cout<<"	.type "<<name<<",@function"<<endl;
	cout<<name<<":"<<endl;
	cout<<"	add sp,sp,"<<-sizeForRisc<<endl;
	cout<<"	sw ra,"<<sizeForRisc-4<<"(sp)"<<endl;
}
void risc_endFunc(string name)
{
	cout<<"	.size "<<name<<",.-"<<name<<endl;
}
void risc_return()
{
	cout<<"	lw ra,"<<sizeForRisc-4<<"(sp)"<<endl;
	cout<<"	add sp,sp,"<<sizeForRisc<<endl;
	cout<<"	jr ra"<<endl;
}
void risc_gloArr(string name,int size)
{
	cout<<"	.comm "<<name<<","<<size*4<<",4"<<endl;
}
void risc_gloVar(string name,int val)
{
	cout<<"	.global "<<name<<endl;
	cout<<"	.section .sdata"<<endl;
	cout<<"	.align 2"<<endl;
	cout<<"	.type "<<name<<",@object"<<endl;
	cout<<"	.size "<<name<<",4"<<endl;
	cout<<name<<":"<<endl;
	cout<<"	.word "<<val<<endl;
}
void risc_integer(string reg,string val)
{
	cout<<"	li "<<reg<<","<<val<<endl;
}
void risc_op(string reg1,string reg2,string reg3,string op)
{
	if(op=="+"||op=="-"||op=="*"||op=="/"||op=="%"||op=="<"||op==">")
	{
		if(op=="+")
			cout<<"	add ";
		else if(op=="-")
			cout<<"	sub ";
		else if(op=="*")
			cout<<"	mul ";
		else if(op=="/")
			cout<<"	div ";
		else if(op=="%")
			cout<<"	rem ";
		else if(op=="<")
			cout<<"	slt ";
		else if(op==">")
			cout<<"	sgt ";
		cout<<reg1<<","<<reg2<<","<<reg3<<endl;
		return;
	}
	if(op=="&&")
	{
		cout<<"	and "<<reg1<<","<<reg2<<","<<reg3<<endl;
		cout<<"	snez "<<reg1<<","<<reg1<<endl;
	}
	if(op=="||")
	{
		cout<<"	or "<<reg1<<","<<reg2<<","<<reg3<<endl;
		cout<<"	snez "<<reg1<<","<<reg1<<endl;
	}
	if(op=="!=")
	{
		cout<<"	xor "<<reg1<<","<<reg2<<","<<reg3<<endl;
		cout<<"	snez "<<reg1<<","<<reg1<<endl;
	}
	if(op=="==")
	{
		cout<<"	xor "<<reg1<<","<<reg2<<","<<reg3<<endl;
		cout<<"	snez "<<reg1<<","<<reg1<<endl;
		cout<<"	add "<<reg1<<","<<reg1<<",-1"<<endl;
	}
}
void risc_uOp(string reg1,string reg2,string op)
{
	if(op=="!")
	{
		cout<<"	snez "<<reg1<<","<<reg2<<endl;
		cout<<"	neg "<<reg1<<","<<reg1<<endl;
		cout<<"	add "<<reg1<<","<<reg1<<","<<"1"<<endl;
	}
	if(op=="-")
		cout<<"	neg "<<reg1<<","<<reg2<<endl;
}
void risc_mv(string reg1,string reg2)
{
	cout<<"	mv "<<reg1<<","<<reg2<<endl;
}
void risc_arrAssign(string reg1,string pos,string reg2)
{
	cout<<"	sw "<<reg2<<","<<pos<<"("<<reg1<<")"<<endl;
}
void risc_arrUse(string reg1,string reg2,string pos)
{
	cout<<"	lw "<<reg1<<","<<pos<<"("<<reg2<<")"<<endl;
}
void risc_store(string reg,string pos)
{	
	int posint=atoi(pos.c_str());
	cout<<"	sw "<<reg<<","<<posint*4<<"(sp)"<<endl;
}
void risc_load(string mempos,string reg)
{
	if(mempos[0]=='v')
	{
		cout<<"	lui "<<reg<<",%hi("<<mempos<<")"<<endl;
		cout<<"	lw "<<reg<<",%lo("<<mempos<<")("<<reg<<")"<<endl;
	}
	else
	{
		int posint=atoi(mempos.c_str());
		cout<<"	lw "<<reg<<","<<posint*4<<"(sp)"<<endl;
	}
}
void risc_loadAddr(string mempos,string reg)
{
	if(mempos[0]=='v')
	{
		cout<<"	lui "<<reg<<",%hi("<<mempos<<")"<<endl;
		cout<<"	add "<<reg<<","<<reg<<",%lo("<<mempos<<")"<<endl;
	}
	else
	{
		int posint=atoi(mempos.c_str());
		cout<<"	add "<<reg<<",sp,"<<posint*4<<endl;
	}
}
/////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////// 
//                              Tigger
//s10 :替换 ,s11 :4,  x0 : 0
int gloNum = 0;
int temNum = 0;
int gtNum=0;

int regInStack[26]; //内存中位置
struct eeyoreVar {
	bool canDel =false;
	bool saved = true;
	string mem="0";
	int reg=-1;
	int type = 0;
	bool arr;
};
struct regi {
	bool canUse= true;
	int reg = -1;
	int val = 0;
	vector<string> varVector;
	
	bool operator<(const regi& r) const {
		return val < r.val;
	}
};
priority_queue<regi>regs;
map<string, eeyoreVar>EMap;
map<string, eeyoreVar>NMap;
regi Reg[28];

//解析变量
string jiexi(char v1,int v2)
{
	if(v1==' ') return(to_string(v2));
	if(v1=='p') return ("p"+to_string(v2));
	if(v1=='T') return ("T"+to_string(v2));
	if(v1=='t') return ("t"+to_string(v2));
}
void pri() {
	for (int i = 0; i <= 27; i++)
	{
		cout << regName[i] << " :" << endl;
		cout << "		"<<Reg[i].reg<<endl;
		cout << "		";
		for (vector<string>::iterator it = Reg[i].varVector.begin(); it != Reg[i].varVector.end(); it++)
			cout << *it<< "  ";
		cout << endl;
	}
}

//��ӡ�ڴ�
void priM() {
	for (map<string, eeyoreVar>::iterator it = EMap.begin(); it != EMap.end(); it++) {
		cout << it->first << "  :"<<endl;
		cout << "	mem:" << it->second.mem << "  reg:";
		if (it->second.reg == -1) cout << -1;
		else cout << regName[it->second.reg];
		cout << "  type:" << it->second.type << endl;
	}
	for (map<string, eeyoreVar>::iterator it = NMap.begin(); it != NMap.end(); it++) {
		cout << it->first <<"    ";	
	}cout << endl;
}
void clearAll()
{
	for (int i = 0; i <= 27; i++)
	{
		Reg[i].varVector.clear();
	}
	for (map<string, eeyoreVar>::iterator it = EMap.begin(); it != EMap.end(); it++)
	{
		it->second.reg = -1;
	}
	NMap.clear();
	Reg[26].varVector.push_back("4");       //第27个寄存器s11固定存4
	Reg[27].varVector.push_back("0");       //第28个寄存器x0只能是0
	NMap["4"]={false,true,"-1",26};         //数字地址中加入4所在的寄存器
	NMap["0"]={false,true,"-1",27};			//数字地址中加入0所在的寄存器
}
//找可用寄存器
int findReg(int st=0,string t1=" ",string t2=" ") {
	
	for (int i = 0; i <= 24; i++)      //在前25个寄存器中找，第26个寄存器s10用来防止寄存器不够，s11和x0固定存4和0，这三个不可用  
	{
		if (Reg[i].canUse == true)    //寄存器可用时查看此寄存器
		{
			int j = 0;   //初始代价 j 为0，越小代价越高，最后选j最大的寄存器作为选择
			for (vector<string>::iterator it = Reg[i].varVector.begin(); it != Reg[i].varVector.end(); it++)
			{
				
				string var = *it;
				if ((var[0] - '0' >= 0 && var[0] - '0' <= 9)||var[0]=='-') { j =j- 1;  }  //存的是数字
				if (var[0] == 'A') j -= 1;              //存的是地址
				if (var[0] == 'T' || var[0] == 'p') {   //全局变量T，局部变量T，函数参数P
					if(st==0)    //找寄存器不是用来运算时
					{
						if (EMap.find(var)->second.saved == true) j =j- 1;
						else
						{
							j=j-2;
							if(EMap.find(var)->second.type==0&&EMap.find("A_"+var)->second.reg==-1)
							j=j-2;
						}
					}
					if(st==1)   //找寄存器用来运算
					{
						if(!(var==t1&&var!=t2))
						{
							if (EMap.find(var)->second.saved == true) j =j- 1;
							else
							{
								j=j-2;
								if(EMap.find(var)->second.type==0&&EMap.find("A_"+var)->second.reg==-1)
								j=j-2;
							}
							if(var==t2)
							{
								j=j-2;
							}
						}
					}
					
				}
				if (var[0] == 't') //生成eeyore时增加的t
				{
					if (EMap.find(var)->second.canDel == false) 
					{
						j =j- 1;
						if (EMap.find(var)->second.saved == false) j =j- 1;
					}
				}
			}
			Reg[i].val = j;
			regs.push(Reg[i]);
		}
	}
	int k = 0,chooseReg= -1;
	while (!regs.empty()) {
		if (k == 0)
		{
			k = 1;
			chooseReg = regs.top().reg;
		}
		//cout << regName[regs.top().reg]  ;
		//cout << "	"<<regs.top().val << endl;
		regs.pop();
	}
	if (chooseReg == -1) return -1;
	for (vector<string>::iterator it = Reg[chooseReg].varVector.begin(); it != Reg[chooseReg].varVector.end(); it++)
	{
		string var = *it;
		if ((var[0] - '0' >= 0 && var[0] - '0' <= 9)||var[0]=='-')
		{
			NMap.erase(NMap.find(var));
		}
		if (var[0] == 'A')
		{
			EMap.find(var)->second.reg = -1;
		}
		if (var[0] == 'T' || var[0] == 'p') 
		{
			if (EMap.find(var)->second.saved == false)
			{
				if (EMap.find(var)->second.type == 0)
				{
					int addReg = EMap.find("A_" + var)->second.reg;
					if ( addReg!= -1)
					{
						if(modes==tigger&&func1==false)cout<<regName[addReg]<<"[ 0 ] = "<< regName[chooseReg] <<endl;
						if(modes==riscv&&func1==false)risc_arrAssign(regName[addReg],"0",regName[chooseReg]);
					}
					else
					{
						Reg[chooseReg].canUse = false;
						int i = findReg();
						if (i == -1) i = 25;
						Reg[chooseReg].canUse = true;
						Reg[i].varVector.push_back("A_"+var);
						EMap.find("A_" + var)->second.reg = i;
						if(modes==tigger&&func1==false)cout << "loadaddr " << EMap.find(var)->second.mem << " " << regName[i] << endl;
						if(modes==tigger&&func1==false)cout << regName[i] << "[ 0 ] = " << regName[chooseReg] << endl;
						if(modes==riscv&&func1==false)risc_loadAddr( EMap.find(var)->second.mem,regName[i]);
						if(modes==riscv&&func1==false)risc_arrAssign(regName[i],"0",regName[chooseReg]);
					}
				}
				else{
					if(modes==tigger&&func1==false)cout << "store " << regName[chooseReg] << " " << EMap.find(var)->second.mem << endl;
					if(modes==riscv&&func1==false)risc_store(regName[chooseReg],EMap.find(var)->second.mem);
				}
			}
			EMap.find(var)->second.reg = -1;
		}
		if (var[0] == 't')
		{
			if (EMap.find(var)->second.canDel == false)
			{
				if (EMap.find(var)->second.saved == false)
				{
					if(modes==tigger&&func1==false)cout << "store " << regName[chooseReg] << " " << EMap.find(var)->second.mem << endl;
					if(modes==riscv&&func1==false)risc_store(regName[chooseReg],EMap.find(var)->second.mem);
				}
			}EMap.find(var)->second.reg = -1;
		}
	}
	Reg[chooseReg].varVector.clear();
	//cout<<"---------------"<<chooseReg<<"-------------"<<endl;
	return chooseReg;
}
//调用者保存
void saveReg()
{
	for(int j=0;j<=14;j++){
		if(modes==tigger&&func1==false)
		cout<<"store "<<regName[j]<<" "<<regInStack[j]<<endl;
		if(modes==riscv&&func1==false) risc_store(regName[j],to_string(regInStack[j]));
	}
}
//调用者恢复
void returnReg()
{
	for(int j=0;j<=14;j++){
		if(modes==tigger&&func1==false)
		cout<<"load "<<regInStack[j]<<" "<< regName[j]<<endl;
		if(modes==riscv&&func1==false)risc_load(to_string(regInStack[j]),regName[j]);
	}
}
//调函数传参
void loadInReg(string evar,int reg)
{
	if ((evar[0] - '0' >= 0 && evar[0] - '0' <= 9)||evar[0]=='-')
	{
		if(modes==tigger&&func1==false)cout<<regName[reg]<<" = "<<evar<<endl;
		if(modes==riscv&&func1==false)risc_integer(regName[reg],evar);
		return;
	}
	if (EMap.find(evar)->second.arr==false)
	{
		map<string, eeyoreVar>::iterator iter = EMap.find(evar);
		if (iter->second.reg != -1)
		{
			if(iter->second.reg>=15)
			{
				if(modes==tigger&&func1==false)cout<<regName[reg]<<" = "<<regName[iter->second.reg]<<endl;
				if(modes==riscv&&func1==false)risc_mv(regName[reg],regName[iter->second.reg]);
			}
			else
			{
				if(modes==tigger&&func1==false)cout<<"load "<<regInStack[iter->second.reg]<<" "<<regName[reg]<<endl;
				if(modes==riscv&&func1==false)risc_load(to_string(regInStack[iter->second.reg]),regName[reg]);
			}
		} 
		else{
			if(modes==tigger&&func1==false)cout << "load " << iter->second.mem << " " << regName[reg] << endl;
			if(modes==riscv&&func1==false)risc_load(iter->second.mem,regName[reg]);
		}
	}
	if (EMap.find(evar)->second.arr==true)
	{
		map<string, eeyoreVar>::iterator iter;
		if (evar[0] == 'T'&&EMap.find(evar)->second.type == 0)
		{
			iter = EMap.find("A_"+evar);
			if (iter->second.reg != -1)
			{
				if(iter->second.reg>=15)
				{
					if(modes==tigger&&func1==false)cout<<regName[reg]<<" = "<<regName[iter->second.reg]<<endl;
					if(modes==riscv&&func1==false)risc_mv(regName[reg],regName[iter->second.reg]);
				}
				else
				{
					if(modes==tigger&&func1==false)cout<<"load "<<regInStack[iter->second.reg]<<" "<<regName[reg]<<endl;
					if(modes==riscv&&func1==false)risc_load(to_string(regInStack[iter->second.reg]),regName[reg]);
				}
			}
			else
			{
				if(modes==tigger&&func1==false)cout << "loadaddr " << iter->second.mem << " " << regName[reg] << endl;
				if(modes==riscv&&func1==false)risc_loadAddr(iter->second.mem,regName[reg]);
			}
		}
		if (evar[0] == 'T'&&EMap.find(evar)->second.type == 1)
		{
			iter = EMap.find(evar);
			if (iter->second.reg != -1)
			{
				if(iter->second.reg>=15)
				{
					if(modes==tigger&&func1==false)cout<<regName[reg]<<" = "<<regName[iter->second.reg]<<endl;
					if(modes==riscv&&func1==false)risc_mv(regName[reg],regName[iter->second.reg]);
				}
				else
				{
					if(modes==tigger&&func1==false)cout<<"load "<<regInStack[iter->second.reg]<<" "<<regName[reg]<<endl;
					if(modes==riscv&&func1==false)risc_load(to_string(regInStack[iter->second.reg]),regName[reg]);
				}
			}
			else
			{
				if(modes==tigger&&func1==false)cout << "loadaddr " << iter->second.mem << " " << regName[reg] << endl;
				if(modes==riscv&&func1==false)risc_loadAddr(iter->second.mem,regName[reg]);
			}
		}
	}

}
//装入寄存器
int load(string evar,bool arry,bool addyunn=false,string t1=" ",string t2=" ") {
	if ((evar[0] - '0' >= 0 && evar[0] - '0' <= 9)||evar[0]=='-')
	{
		map<string, eeyoreVar>::iterator iter = NMap.find(evar);
		if (iter != NMap.end()) return iter->second.reg;
		else {
			int i;
			if(addyunn==false)
			i=findReg();
			if(addyunn==true )
			i=findReg(1,t1,t2);
			NMap[evar] = {false,true,"0",i};
			Reg[i].varVector.push_back(evar);
			if(modes==tigger&&func1==false)cout << regName[i] << " = " << evar << endl;
			if(modes==riscv&&func1==false)risc_integer(regName[i],evar);
			return i;
		}
	}
	if (EMap.find(evar)->second.arr==false)
	{
		map<string, eeyoreVar>::iterator iter = EMap.find(evar);
		if (iter->second.reg != -1) return iter->second.reg;
		int i;
			if(addyunn==false)
			i=findReg();
			if(addyunn==true )
			i=findReg(1,t1,t2);
		if(modes==tigger&&func1==false)cout << "load " << iter->second.mem << " " << regName[i] << endl;
		if(modes==riscv&&func1==false)risc_load(iter->second.mem,regName[i]);
		iter->second.reg = i;
		iter->second.saved = true;
		Reg[i].varVector.push_back(evar);
		return i;
	}
	if (EMap.find(evar)->second.arr==true)
	{
		map<string, eeyoreVar>::iterator iter;
		if (evar[0] == 'p')
		{
			iter = EMap.find(evar);
			if(iter->second.reg!=-1) return iter->second.reg;
			else
			{
				int i = findReg();
				if(modes==tigger&&func1==false)cout << "load " << iter->second.mem << " " << regName[i] << endl;
				if(modes==riscv&&func1==false)risc_load(iter->second.mem,regName[i]);
				iter->second.reg = i;
				iter->second.saved = true;
				Reg[i].varVector.push_back(evar);
				return i;
			}
		}
		if (evar[0] == 'T'&&EMap.find(evar)->second.type == 0)
		{
			iter = EMap.find("A_"+evar);
			if (iter->second.reg != -1) return iter->second.reg;
			else
			{
				int i = findReg();
				if(modes==tigger&&func1==false)cout << "loadaddr " << iter->second.mem << " " << regName[i] << endl;
				if(modes==riscv&&func1==false)risc_loadAddr(iter->second.mem,regName[i]);
				iter->second.reg = i;
				iter->second.saved = true;
				Reg[i].varVector.push_back("A_" + evar);
				return i;
			}
		}
		if (evar[0] == 'T'&&EMap.find(evar)->second.type == 1)
		{
			iter = EMap.find(evar);
			if (iter->second.reg != -1) return iter->second.reg;
			else
			{
				int i = findReg();
				if(modes==tigger&&func1==false)cout << "loadaddr " << iter->second.mem << " " << regName[i] << endl;
				if(modes==riscv&&func1==false)risc_loadAddr(iter->second.mem,regName[i]);
				iter->second.reg = i;
				iter->second.saved = true;
				Reg[i].varVector.push_back(evar);
				return i;
			}
		}
	}
}
//调函数前只保存全局变量
void storeGlo()
{
	for (int chooseReg = 0; chooseReg <= 27; chooseReg++)
	{
		for (vector<string>::iterator it = Reg[chooseReg].varVector.begin(); it != Reg[chooseReg].varVector.end(); it++)
		{
			string var = *it;

			if(var[0] == 'T'&&EMap.find(var)->second.saved == false&&EMap.find(var)->second.type == 0)
			{
				int addReg = EMap.find("A_" + var)->second.reg;
				if (addReg != -1)
				{
					if(modes==tigger&&func1==false)cout << regName[addReg] << "[ 0 ] = " << regName[chooseReg] << endl;
					if(modes==riscv)risc_arrAssign(regName[addReg],"0",regName[chooseReg]);
				}
				else
				{
					Reg[chooseReg].canUse = false;
					int i = findReg();
					if (i == -1) i = 25;
					Reg[chooseReg].canUse = true;
					Reg[i].varVector.push_back("A_" + var);
					EMap.find("A_" + var)->second.reg = i;
					if(modes==tigger&&func1==false)cout << "loadaddr " << EMap.find(var)->second.mem << " " <<regName[i]  << endl;
					if(modes==tigger&&func1==false)cout << regName[i] << "[ 0 ] = " << regName[chooseReg] << endl;
					if(modes==riscv&&func1==false)risc_loadAddr(EMap.find(var)->second.mem,regName[i]);
					if(modes==riscv&&func1==false)risc_arrAssign(regName[i],"0",regName[chooseReg]);
				}
				EMap.find(var)->second.saved=true;
			}
		}
	}
}
//逻辑块寄存器回存
void storeReg()
{
	for (int o = 0; o <= 1; o++)
	{
		for (int chooseReg = 0; chooseReg <= 27; chooseReg++)
		{
			for (vector<string>::iterator it = Reg[chooseReg].varVector.begin(); it != Reg[chooseReg].varVector.end(); it++)
			{
				string var = *it;
				if ((var[0] - '0' >= 0 && var[0] - '0' <= 9)||var[0]=='-')
				{
					NMap.erase(NMap.find(var));
				}
				if (var[0] == 'A')
				{
					EMap.find(var)->second.reg = -1;
				}
				if (var[0] == 'T' || var[0] == 'p')
				{
					if (EMap.find(var)->second.saved == false)
					{
						if (EMap.find(var)->second.type == 0)
						{
							int addReg = EMap.find("A_" + var)->second.reg;
							if (addReg != -1)
							{
								if(modes==tigger&&func1==false)cout << regName[addReg] << "[ 0 ] = " << regName[chooseReg] << endl;
								if(modes==riscv&&func1==false)risc_arrAssign(regName[addReg],"0",regName[chooseReg]);
							}
							else
							{
								Reg[chooseReg].canUse = false;
								int i = findReg();
								if (i == -1) i = 25;
								Reg[chooseReg].canUse = true;
								Reg[i].varVector.push_back("A_" + var);
								EMap.find("A_" + var)->second.reg = i;
								if(modes==tigger&&func1==false)cout << "loadaddr " << EMap.find(var)->second.mem << " " <<regName[i]  << endl;
								if(modes==tigger&&func1==false)cout << regName[i] << "[ 0 ] = " << regName[chooseReg] << endl;
								if(modes==riscv&&func1==false)risc_loadAddr(EMap.find(var)->second.mem,regName[i]);
								if(modes==riscv&&func1==false)risc_arrAssign(regName[i],"0",regName[chooseReg]);
							}
						}
						else
						{
							if(modes==tigger&&func1==false)cout << "store " << regName[chooseReg] << " " << EMap.find(var)->second.mem << endl;
							if(modes==riscv&&func1==false)risc_store(regName[chooseReg],EMap.find(var)->second.mem);
						}
					}
					EMap.find(var)->second.reg = -1;
				}
				if (var[0] == 't')
				{
					if (EMap.find(var)->second.canDel == false)
					{
						if (EMap.find(var)->second.saved == false)
						{
							if(modes==tigger&&func1==false)cout << "store " << regName[chooseReg] << " " << EMap.find(var)->second.mem << endl;
							if(modes==riscv&&func1==false)risc_store(regName[chooseReg],EMap.find(var)->second.mem);
						}
					}EMap.find(var)->second.reg = -1;
				}
			}
			Reg[chooseReg].varVector.clear();
		}
	}
}
//赋值
void varAssign(string var1,string var2)
{//cout<<"-------------"<<var1<<"    "<<var2<<endl;
	int i = load(var2,false);
	if(var2[0]=='t')
		EMap.find(var2)->second.canDel = true;
	int reg = EMap.find(var1)->second.reg;
	
	if (reg != -1)
	{
		vector<string>::iterator iter;
		for (iter = Reg[reg].varVector.begin(); iter != Reg[reg].varVector.end(); iter++)
		{
			if (*iter == var1) { iter=Reg[reg].varVector.erase(iter)-1; }
		}
	}
	EMap.find(var1)->second.reg = i;
	EMap.find(var1)->second.saved = false;
	Reg[i].varVector.push_back(var1);
}
//使用数组
void arrUse(string t,string arr,string arrIndex)
{
	if ((arrIndex[0] - '0' >= 0 && arrIndex[0] - '0' <= 9)||arrIndex[0]=='-')
	{
		int index = atoi(arrIndex.c_str());
		if (arr[0] == 'T'&&EMap.find(arr)->second.type == 1)
		{
			int i = findReg();
			int j = atoi((EMap.find(arr)->second.mem).c_str());
			if(modes==tigger&&func1==false)cout << "load "  << index/4 + j << "  "<<regName[i] << endl;
			if(modes==riscv&&func1==false)risc_load(to_string(index/4 + j),regName[i]);
			Reg[i].varVector.push_back(t);
			EMap.find(t)->second.reg = i;
			EMap.find(t)->second.saved = false;
		}
		else
		{
			int i = load(arr, true);
			int j= findReg();
			if(modes==tigger&&func1==false)cout << regName[j] << " = " << regName[i] << "[ "<< index <<" ] " << endl;
			if(modes==riscv&&func1==false)risc_arrUse(regName[j],regName[i],to_string(index));
			Reg[j].varVector.push_back(t);
			EMap.find(t)->second.reg = j;
			EMap.find(t)->second.saved = false;
		}
	}
	else
	{
		int i = load(arr, true);
		Reg[i].canUse = false;
		int j = load(arrIndex, false);
		if (arrIndex[0] == 't')
			EMap.find(arrIndex)->second.canDel = true;
		int k = findReg();
		Reg[i].canUse = true;
		//cout << regName[k] << " = " << regName[j] << " * 4" << endl;
		if(modes==tigger&&func1==false)cout<< regName[k] << " = " << regName[i] << " + " <<regName[j]<< endl;
		if(modes==tigger&&func1==false)cout << regName[k] << " = " << regName[k] << "[ 0 ] " << endl;
		if(modes==riscv&&func1==false)risc_op(regName[k],regName[i],regName[j],"+");
		if(modes==riscv&&func1==false)risc_arrUse(regName[k],regName[k],"0");
		Reg[k].varVector.push_back(t);
		EMap.find(t)->second.reg = k;
		EMap.find(t)->second.saved = false;
	}
}
//取数组
void arrAssig(string t, string arr, string arrIndex)
{
	if ((arrIndex[0] - '0' >= 0 && arrIndex[0] - '0' <= 9)||arrIndex[0]=='-')
	{
		int index = atoi(arrIndex.c_str());
		if (arr[0] == 'T'&&EMap.find(arr)->second.type == 1)
		{
			int i = load(t,false);
			if (t[0] == 't')
				EMap.find(t)->second.canDel = true;
			int j = atoi((EMap.find(arr)->second.mem).c_str());
			if(modes==tigger&&func1==false)cout << "store " << regName[i] << "  " << index/4 + j << endl;
			if(modes==riscv&&func1==false)risc_store(regName[i],to_string(index/4 + j));
		}
		else
		{
			int i = load(arr, true);
			Reg[i].canUse = false;
			int j = load(t,false);
			if (t[0] == 't')
				EMap.find(t)->second.canDel = true;
			Reg[i].canUse = true;
			if(modes==tigger&&func1==false)cout << regName[i] << "[ " << index << " ] = " <<regName[j]<< endl;
			if(modes==riscv&&func1==false)risc_arrAssign(regName[i],to_string(index),regName[j]);
		}
	}
	else
	{
		int i = load(arr, true);
		Reg[i].canUse = false;
		int j = load(arrIndex, false); //cout << "-----" << j << endl;
		if (arrIndex[0] == 't')
			EMap.find(arrIndex)->second.canDel = true;
		int k = findReg();
		Reg[i].canUse = true;
		//cout << regName[k] << " = " << regName[j] << " * 4" << endl;
		if(modes==tigger&&func1==false)cout << regName[k] << " = " << regName[i] << " + " << regName[j] << endl;
		if(modes==riscv&&func1==false)risc_op(regName[k],regName[i],regName[j],"+");
		Reg[k].canUse = false;
		int m= load(t,false);
		Reg[k].canUse = true;
		if(t[0]=='t')
			EMap.find(t)->second.canDel = true;
		if(modes==tigger&&func1==false)cout << regName[k] << "[ 0 ] = " <<regName[m]<< endl;
		if(modes==riscv&&func1==false)risc_arrAssign(regName[k],"0",regName[m]);
	}
}
//运算
void addyun(string v0,string v1,string v2,string yun)
{//cout<<v0<<"    "<<v1<<"   "<<v2<<endl;
	int i=load(v1,false,true,v0,v2);
	Reg[i].canUse = false;
	int j=load(v2,false);
	Reg[i].canUse = true;
	if(v1[0]=='t'&&yun!=">"&&yun!="<")
		EMap.find(v1)->second.canDel = true;
	if(v2[0]=='t'&&yun!=">"&&yun!="<")
		EMap.find(v2)->second.canDel = true;
	int k=findReg();
	if(modes==tigger&&func1==false)cout<<regName[k]<<" = "<<regName[i]<<' '<<yun<<' '<<regName[j]<<endl;
	if(modes==riscv&&func1==false)risc_op(regName[k],regName[i],regName[j],yun);
	EMap.find(v0)->second.reg = k;
	EMap.find(v0)->second.saved = false;
	Reg[k].varVector.push_back(v0);
}
//单目运算
void opyun(string v0,string v1,string yun)
{
	int i=load(v1,false);
	if(v1[0]=='t')
		EMap.find(v1)->second.canDel = true;
	int k=findReg();
	if(modes==tigger&&func1==false)cout<<regName[k]<<" = "<<yun<<" "<<regName[i]<<endl;
	if(modes==riscv&&func1==false)risc_uOp(regName[k],regName[i],yun);
	EMap.find(v0)->second.reg = k;
	EMap.find(v0)->second.saved = false;
	Reg[k].varVector.push_back(v0);
}
//翻译时变量存入hash
void eVarMap(string varName,int len=0,int va=0) {         
	bool arr;
	if(len>1)arr=true;
	else
	arr=false;
	   
	if (status == 0) {
		if(varName[0]=='T')
		{
			EMap["A_"+varName] = { false,true,"v" +to_string(gloNum),-1,0 ,arr};
			EMap[ varName] = { false,true,"v" + to_string(gloNum),-1,0 ,arr };
			if (len == 1) 
			{
				if(modes==tigger&&func1==false)cout << "v" << gloNum << " = " << va << endl;
				if(modes==riscv&&func1==false)risc_gloVar("v"+to_string(gloNum),va);
			}
			else
			{
				if(modes==tigger&&func1==false)cout << "v" << gloNum << " = malloc " << 4*len << endl;
				if(modes==riscv&&func1==false)risc_gloArr("v"+to_string(gloNum),4*len);
			}
			gloNum++;
		}
		if(varName[0]=='t')
		{
			EMap[ varName] = { false,true, to_string(gtNum),-1,0 ,arr };
			gtNum++;
		}
	}
	if (status == 1) {
		if (varName[0] == 'p') {
				EMap[varName] = { false,false,to_string(temNum) ,temNum ,2 ,arr};
				
				Reg[temNum].varVector.push_back(varName);
				temNum += 1;
		}
		else {
			EMap[varName] = { false,true,to_string(temNum) ,-1 ,1 ,arr};
			temNum += len;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////
void node::doo(int cosnum=0 , int *constarr=NULL)
	{
		char leftvar,rightvar;
		int leftnum,rightnum;
		
		switch (TY)
		{
		case compunit:
		{
			int z = 0;
			for (z = 0; z <= 27; z++) {
			vector<string> varVector;
			Reg[z] = {true,z,0,varVector };
			}
            if(choose==1||choose==2){p[0]->doo();p[1]->doo();}
            if(choose==4||choose==3)p[0]->doo();
		}
            break;
		case decl:
			p[0]->doo();
		break;
		case vardefs:
            	if(choose==1)
            	{
                	p[0]->doo();
                	p[1]->doo();
            	}
            	else p[0]->doo();
            	break;
		case exp:
		    p[0]->doo(cosnum);
			
		    break;
		case constexp:
		    p[0]->doo(cosnum);
			
		    break;

		case constdefs:              //定义节点
			{
				
				if(status==0)
				{
					varnum2=varnum;
					temvarnum2=temvarnum;
					qunode.push(pass);
				}
				if(status==3)
				{varnum =varnum2;temvarnum=temvarnum2;}
			}
			if(choose==1)
			{
				p[2]->doo(1);
			}
			if(p[0]==NULL)
			{
				//cout<<"INT"<<endl;
				if(p[1]->choose==1)
				{
					p[1]->p[0]->doo(1);
					//cout<<"  var T"<<varnum++<<endl;
					if(status==0) 
					{
						if(modes==0)cout<<"var T"<<varnum<<endl;
						if(modes==tigger||modes==riscv)eVarMap("T"+to_string(varnum),1,returnnum[0]);
					}
					if(status==1)   //记录变量
					{
						quchar[status].push("var T"+to_string(varnum));
						eVarMap("T"+to_string(varnum),1,returnnum[0]);
					} varnum++;
					if(status==2||status==3)
					{
						if(modes==0)cout<<"  T"<<varnum-1<<" = "<<returnnum[0]<<endl;
						if(status==2&&(modes==tigger||modes==riscv))
						{
							varAssign("T"+to_string(varnum-1),to_string(returnnum[0]));
						}
					}
					value tes{'T',varnum-1,NULL,NULL,0,returnnum[0],1};
					mp[nowmap][id]=tes;

				}
				else {cout<<"NOT INT"<<endl;exit(0);}
			}
			else
			{
				if(p[1]->choose==1){cout<<"NOT ARRY"<<endl;exit(0);}
				A[0]=0;
				p[0]->doo(1);
				int weidu=A[0];
				int *constarry2=new int[MAXL]; //为用数组
				int j=1,j2;
				for(int i=weidu;i>=1;i--)
				{
					j2=A[i]; 
					constarry2[i]=j;
					j*=j2;
				}
				pos=0;
				constarry2[0]=weidu;
				draw(0,weidu);
				
				//cout<<B<<endl;
				int length = 1;
				for(int i=1;i<=A[0];i++)length*=A[i];
				if(status==0)
				{
					if(modes==0)cout<<"  var "<<4*length<<" T"<<varnum<<endl;
					if(modes==tigger||modes==riscv)eVarMap("T"+to_string(varnum),length);
				}
				if(status==1)   //记录变量
				{
					quchar[status].push("var "+to_string(4*length)+" T"+to_string(varnum));
					if(modes==tigger||modes==riscv)eVarMap("T"+to_string(varnum),length);
				}
				int *constarry=new int[MAXL];
				pos=0;
				pos2=0;
				Rnum=0;
				p[1]->doo(1,constarry);
				value tes{'T',varnum++,constarry,constarry2,1,0,1};
				mp[nowmap][id]=tes;
			}
			break;
        
        case vardef:              //定义非const节点
			  
				
				if(status==0)
				{     
					varnum2=varnum; 
					temvarnum2=temvarnum;
					qunode.push(pass);
				}
				
				if(status==3)
				{varnum =varnum2;temvarnum=temvarnum2;}
			
			if(p[0]==NULL)
			{
				//cout<<"  INT"<<endl;
                if(choose==2)         //无赋值
                {
                    if(status==0)
					{
						if(modes==0)cout<<"  var T"<<varnum<<endl;
						if(modes==tigger||modes==riscv)eVarMap("T"+to_string(varnum),1,0);
					}
					if(status==1)   //记录变量
					{
						quchar[status].push("var T"+to_string(varnum));
						if(modes==tigger||modes==riscv)eVarMap("T"+to_string(varnum),1);
					} varnum++;
					if(status==3)
					{
						if(modes==0)cout<<"  T"<<varnum-1<<" = "<<0<<endl;
						if(modes==tigger||modes==riscv)varAssign("T"+to_string(varnum-1),to_string(0));
					}
					value tes{'T',varnum-1,NULL,NULL,0,0};
					mp[nowmap][id]=tes; 
                }
				else if(p[1]->choose==1)   //有赋值的
				{
					p[1]->p[0]->doo(0);
					if(status==0)
					{
						if(modes==0)cout<<"  var T"<<varnum<<endl;
						if(modes==tigger||modes==riscv)eVarMap("T"+to_string(varnum),1,0);
					}
					if(status==1)   //记录变量
					{
						quchar[status].push("var T"+to_string(varnum));
						if(modes==tigger||modes==riscv)eVarMap("T"+to_string(varnum),1);
					} varnum++;
					if(status==2||status==3)
					{
						if(modes==0)cout<<"  T"<<varnum-1<<" = "<<returnvar<<returnnum[0]<<endl;
						if(modes==tigger||modes==riscv)varAssign("T"+to_string(varnum-1),jiexi(returnvar,returnnum[0]));
					}
					value tes{'T',varnum-1,NULL,NULL,0,0,0};
					mp[nowmap][id]=tes;

				}
				else {cout<<"NOT INT"<<endl;exit(0);}
			}
			else
			{
				if(choose==1&&p[1]->choose==1){cout<<"NOT ARRY"<<endl;exit(0);}
				A[0]=0;
				p[0]->doo(1);
				int weidu=A[0];
				int *constarry2=new int[MAXL]; //为用数组
				int j=1,j2;
				for(int i=weidu;i>=1;i--)
				{
					j2=A[i]; 
					constarry2[i]=j;
					j*=j2;
				}
				pos=0;
				constarry2[0]=weidu;
				draw(0,weidu);   
				
				//cout<<B<<endl;   //画数组
				int length = 1;
				for(int i=1;i<=A[0];i++)length*=A[i];
				if(status==0)
				{
					if(modes==0)cout<<"  var "<<4*length<<" T"<<varnum<<endl;
					if(modes==tigger||modes==riscv)eVarMap("T"+to_string(varnum),length);
				}
				if(status==1)   //记录变量
				{
					quchar[status].push("var "+to_string(4*length)+" T"+to_string(varnum));
					if(modes==tigger||modes==riscv)eVarMap("T"+to_string(varnum),length);
				}
				if(choose==1)
                {
				    pos=0;
				    pos2=0;
				    Rnum=0;  //剩余右括号数
				    p[1]->doo(0);
                }
				else
				{
					if(status==3)
					{
						j2=0;
					 	while(j2<j)
						 {
							 if(modes==0)cout<<"  T"<<varnum<<" ["<<j2*4<<"] = "<<0<<endl;
							 j2++;
						 }
					}
				}
				value tes{'T',varnum++,NULL,constarry2,1,0,0};
				mp[nowmap][id]=tes;
			}
			break;

		case constexps:
			if(p[0]!=NULL) p[0]->doo(1);
			p[1]->doo(1);
			//cout<<returnnum[0]<<endl;
			A[0]++;
			A[A[0]]=returnnum[0];

			break;
		case constinitval:
			if(choose==1)
			{
				while(1)
				{
					if(B[pos]=='{')
					{
						Rnum++; pos++;
					}
					if(B[pos]=='0') break;
					if(B[pos]=='}')
					{
						if(Rnum>0)
						{
							Rnum--;pos++;
						}
						else {cout<<"数对右括号"<<endl;exit(0);}
					}
				}
				p[0]->doo(cosnum);
				if(cosnum==1)//记录常数组
                {
                    constarr[pos2]=returnnum[0]; 
				   if(status==2||status==3)
				   {
					    if(modes==0)cout<<"  T"<<varnum<<" ["<<pos2*4<<"] = "<<returnnum[0]<<endl;
						if(!(status==3&&returnnum[0]==0))
						{
							
							if(modes==tigger||modes==riscv)arrAssig(to_string(returnnum[0]),"T"+to_string(varnum),to_string(pos2*4));   
						}
				   }
                    pos2++;
				    pos++;
                }
                if(cosnum==0)
                {
                    if(status==2||status==3)
					{
						if(modes==0)cout<<"  T"<<varnum<<" ["<<pos2*4<<"] = "<<returnvar<<returnnum[0]<<endl;
						if(!(status==3&&jiexi(returnvar,returnnum[0])=="0") )
						{
							if(modes==tigger||modes==riscv)arrAssig(jiexi(returnvar,returnnum[0]),"T"+to_string(varnum),to_string(pos2*4));
						}  
					}
                    pos2++;
                    pos++;
                }
			}
			if(choose==2)
			{
				while(1)
				{
					if(B[pos]=='{')
					{
						pos++; break;
					}
					if(B[pos]=='0') {cout<<"左括号对数"<<endl;exit(0);}
					if(B[pos]=='}')
					{
						if(Rnum>0)
						{
							Rnum--;pos++;
						}
						else {cout<<"左括号对右括号"<<endl;exit(0);}
					}
				}
				p[0]->doo(cosnum,constarr);
				while(1)
				{
					if(B[pos]=='{')
					{
						Rnum++; pos++;
					}
					if(B[pos]=='0')
					{
						if(cosnum==1)constarr[pos2]=0; //常数组填值0
				 		if(status==2||status==3)
						 {
							 if(modes==0)cout<<"  T"<<varnum<<" ["<<pos2*4<<"] = "<<'0'<<endl;
							 if(status==2)
							 {
							 	if(modes==tigger||modes==riscv)arrAssig(to_string(0),"T"+to_string(varnum),to_string(pos2*4));
							 }
						 }
						 pos++; pos2++;
					}
					if(B[pos]=='}')
					{
						if(Rnum>0)
						{
							Rnum--; pos++;
						}
						else{pos++;break;}
					}
				}
			}
			if(choose==3)
			{
				while(1)
				{
					if(B[pos]=='{')
					{
						pos++; break;
					}
					if(B[pos]=='0') {cout<<"左括号对数"<<endl;break;}
					if(B[pos]=='}')
					{
						if(Rnum>0)
						{
							Rnum--;pos--;
						}
						else {cout<<"左括号对右括号"<<endl;break;}
					}
				}
				while(1)
				{
					if(B[pos]=='{')
					{
						Rnum++; pos++;
					}
					if(B[pos]=='0')
					{
						if(cosnum==1)constarr[pos2]=0;
				 		if(status==2||status==3)
						 {
							 if(modes==0)cout<<"  T"<<varnum<<" ["<<pos2*4<<"] = "<<'0'<<endl;
							 if(status==2)
							 {
							 	if(modes==tigger||modes==riscv)arrAssig(to_string(0),"T"+to_string(varnum),to_string(pos2*4));
							 }
						 }
						 pos++; pos2++;
					}
					if(B[pos]=='}')
					{
						if(Rnum>0)
						{
							Rnum--; pos++;
						}
						else{pos++;break;}
					}
				}
			}
			break;
		case constinitvals:
			if(choose==1)
			{
				p[0]->doo(cosnum,constarr);
				p[1]->doo(cosnum,constarr);
			}
			else p[0]->doo(cosnum,constarr);

			break;
		case cond:
			p[0]->doo(cosnum);
			break;
		case addexp:
		   
		   int temlab ,temlab2;
			p[0]->doo(cosnum);
			leftnum=returnnum[0];
			leftvar=returnvar;
			if(opsymbol=="&&")
			{	
				if(leftvar==' '&&leftnum==0)
				{
					returnnum[0]=0;
					returnvar=' ';
					break;
				}
				
                    temlab=label; temlab2=label+1;
                    
				if(status==2||status==3)
				{
					 label+=2;
					 if(modes==0)cout<<"  if "<<leftvar<<leftnum<<" == "<<0<<" goto l"<<temlab<<endl;
					 if(modes==tigger||modes==riscv)
					 {
					 	storeReg();
					 	clearAll();
					 	int v1=load(jiexi(leftvar,leftnum),false);
					 	//int v2=load("0",false);
						if(modes==tigger&&func1==false)cout<<" if "<<regName[v1]<<" == "<<"x0"<<" goto l"<<temlab<<endl;
						if(modes==riscv&&func1==false)cout<<"	beq "<<regName[v1]<<",x0,.l"<<temlab<<endl;
					 }
					 
				}
				
			}
			if(opsymbol=="||")
			{	
				if(leftvar==' '&&leftnum==1)
				{
					returnnum[0]=1;
					returnvar=' ';
					break;
				}
				
                    temlab=label; temlab2=label+1;
                    
				if(status==2||status==3)
				{
					 label+=2;
					 if(modes==0)cout<<"  if "<<leftvar<<leftnum<<" != "<<0<<" goto l"<<temlab<<endl;
					 if(modes==tigger||modes==riscv)
					 {
					 	storeReg();
					 	clearAll();
					 	int v1=load(jiexi(leftvar,leftnum),false);
						if(modes==tigger&&func1==false)cout<<" if "<<regName[v1]<<" != "<<"x0"<<" goto l"<<temlab<<endl;
						if(modes==riscv&&func1==false)cout<<"	bne "<<regName[v1]<<",x0,.l"<<temlab<<endl;
					 }
					 	 
				}
				
			}
			p[1]->doo(cosnum);
			rightnum=returnnum[0];
			rightvar=returnvar;

			
			if(leftvar==' '&&rightvar==' '&&(opsymbol=="+"||opsymbol=="-"||opsymbol=="*"||opsymbol=="/"||opsymbol=="%")){//
				int ReturnNum;
				if(opsymbol=="+") ReturnNum = leftnum + rightnum;
				if(opsymbol=="*") ReturnNum = leftnum * rightnum;
				//if(opsymbol=="||") ReturnNum = leftnum || rightnum;
				//if(opsymbol=="&&") ReturnNum = leftnum && rightnum;
				if(opsymbol=="==") ReturnNum = leftnum == rightnum;
				if(opsymbol=="!=") ReturnNum = leftnum != rightnum;
				if(opsymbol=="<") ReturnNum = leftnum < rightnum;
				if(opsymbol==">") ReturnNum = leftnum > rightnum;
				if(opsymbol=="-") ReturnNum = leftnum - rightnum;
				if(opsymbol=="/") ReturnNum = leftnum / rightnum;
				if(opsymbol=="%") ReturnNum = leftnum % rightnum;
				if(opsymbol=="<=")ReturnNum = leftnum <= rightnum;
				if(opsymbol==">=")ReturnNum = leftnum >= rightnum;
				
				returnnum[0] = ReturnNum;
				returnvar=' ';
			}
		    else
		    {
				if(opsymbol!="<="&&opsymbol!=">=")
				{//cout<<"  var t"<<temvarnum++<<endl;
				
					if(status==0||status==1)   //记录变量
					{
						quchar[status].push("var t"+to_string(temvarnum));
						if(modes==tigger||modes==riscv)eVarMap("t"+to_string(temvarnum),1);
					} temvarnum++;
				
					if(status==2||status==3)
					{
						if(modes==0)cout<<"  t"<<temvarnum-1<<" = "<<leftvar<<leftnum<<' '<<opsymbol<<' '<<rightvar<<rightnum<<endl;
						
						if(modes==tigger||modes==riscv)addyun(jiexi('t',temvarnum-1),jiexi(leftvar,leftnum),jiexi(rightvar,rightnum),opsymbol);
						if(opsymbol=="&&")
						{
							if(modes==tigger||modes==riscv)
							{
								storeReg();
								clearAll();
							}
							if(modes==tigger&&func1==false)
							{
								cout<<"  goto l"<<temlab2<<endl;
					 			cout<<"l"<<temlab<<':'<<endl;
							}
							if(modes==riscv&&func1==false)
							{
								cout<<"	j .l"<<temlab2<<endl;
					 			cout<<".l"<<temlab<<':'<<endl;
							}
					 		if(modes==0)cout<<"  t"<<temvarnum-1<<" = "<<leftvar<<leftnum<<endl;
							if(modes==tigger||modes==riscv)
							{
								varAssign("t"+to_string(temvarnum-1),jiexi(leftvar,leftnum));
								storeReg();
								clearAll();
							}
							if(modes==tigger&&func1==false)
							{
					 			cout<<"l"<<temlab2<<":"<<endl;
							}
							if(modes==riscv&&func1==false)
							{
					 			cout<<".l"<<temlab2<<":"<<endl;
							}
						}
						if(opsymbol=="||")
						{
							if(modes==tigger||modes==riscv)
							{
								storeReg();
								clearAll();
							}
							if(modes==tigger&&func1==false)
							{
								cout<<"  goto l"<<temlab2<<endl;
					 			cout<<"l"<<temlab<<':'<<endl;
							}
							if(modes==riscv&&func1==false)
							{
								cout<<"	j .l"<<temlab2<<endl;
					 			cout<<".l"<<temlab<<':'<<endl;
							}
					 		if(modes==0)cout<<"  t"<<temvarnum-1<<" = "<<leftvar<<leftnum<<endl;
							if(modes==tigger||modes==riscv)
							{
								varAssign("t"+to_string(temvarnum-1),jiexi(leftvar,leftnum));
								storeReg();
								clearAll();
							}
							if(modes==tigger&&func1==false)
							{
					 			cout<<"l"<<temlab2<<":"<<endl;
							}
							if(modes==riscv&&func1==false)
							{
					 			cout<<".l"<<temlab2<<":"<<endl;
							}
						}
					}
					returnvar='t';
					returnnum[0]=temvarnum-1;

				}
				if(opsymbol=="<=")
				{
					if(status==0||status==1)   //记录变量
					{
						quchar[status].push("var t"+to_string(temvarnum));
						if(modes==tigger||modes==riscv)eVarMap("t"+to_string(temvarnum),1);
					} temvarnum++;
					if(status==2||status==3)
					{
						if(modes==0)cout<<"  t"<<temvarnum-1<<" = "<<leftvar<<leftnum<<" < "<<rightvar<<rightnum<<endl;
						if(modes==tigger||modes==riscv)addyun(jiexi('t',temvarnum-1),jiexi(leftvar,leftnum),jiexi(rightvar,rightnum),"<");
					}
					if(status==0||status==1)   //记录变量
					{
						quchar[status].push("var t"+to_string(temvarnum));
						if(modes==tigger||modes==riscv)eVarMap("t"+to_string(temvarnum),1);
					} temvarnum++;
					if(status==2||status==3)
					{
						if(modes==0)cout<<"  t"<<temvarnum-1<<" = "<<leftvar<<leftnum<<" == "<<rightvar<<rightnum<<endl;
						if(modes==tigger||modes==riscv)addyun(jiexi('t',temvarnum-1),jiexi(leftvar,leftnum),jiexi(rightvar,rightnum),"==");
					}
					if(status==0||status==1)   //记录变量
					{
						quchar[status].push("var t"+to_string(temvarnum));
						if(modes==tigger||modes==riscv)eVarMap("t"+to_string(temvarnum),1);
					} temvarnum++;
					if(status==2||status==3)
					{
						if(modes==0)cout<<"  t"<<temvarnum-1<<" = "<<"t"<<temvarnum-2<<" || "<<"t"<<temvarnum-3<<endl;
						if(modes==tigger||modes==riscv)addyun(jiexi('t',temvarnum-1),jiexi('t',temvarnum-2),jiexi('t',temvarnum-3),"||");
					}
					returnvar='t';
					returnnum[0]=temvarnum-1;
				}
				if(opsymbol==">=")
				{
					if(status==0||status==1)   //记录变量
					{
						quchar[status].push("var t"+to_string(temvarnum));
						if(modes==tigger||modes==riscv)eVarMap("t"+to_string(temvarnum),1);
					} temvarnum++;
					if(status==2||status==3)
					{
						if(modes==0)cout<<"  t"<<temvarnum-1<<" = "<<leftvar<<leftnum<<" > "<<rightvar<<rightnum<<endl;
						if(modes==tigger||modes==riscv)addyun(jiexi('t',temvarnum-1),jiexi(leftvar,leftnum),jiexi(rightvar,rightnum),">");
					}
					if(status==0||status==1)   //记录变量
					{
						quchar[status].push("var t"+to_string(temvarnum));
						if(modes==tigger||modes==riscv)eVarMap("t"+to_string(temvarnum),1);
					} temvarnum++;
					if(status==2||status==3)
					{
						if(modes==0)cout<<"  t"<<temvarnum-1<<" = "<<leftvar<<leftnum<<" == "<<rightvar<<rightnum<<endl;
						if(modes==tigger||modes==riscv)addyun(jiexi('t',temvarnum-1),jiexi(leftvar,leftnum),jiexi(rightvar,rightnum),"==");
					}
					if(status==0||status==1)   //记录变量
					{
						quchar[status].push("var t"+to_string(temvarnum));
						if(modes==tigger||modes==riscv)eVarMap("t"+to_string(temvarnum),1);
					} temvarnum++;
					if(status==2||status==3)
					{
						if(modes==0)cout<<"  t"<<temvarnum-1<<" = "<<"t"<<temvarnum-2<<" || "<<"t"<<temvarnum-3<<endl;
						if(modes==tigger||modes==riscv)addyun(jiexi('t',temvarnum-1),jiexi('t',temvarnum-2),jiexi('t',temvarnum-3),"||");
					}
					returnvar='t';
					returnnum[0]=temvarnum-1;
				}
		    }

		    break;
		case primaryexp:
		    if(choose==1)
            {
                p[0]->doo(cosnum);
            }
		   if(choose==2)
            {   
                int i=nowmap;
                
                while(i>=0)
                {
                    iter=mp[i].find(id);
                    
                    if(iter!=mp[i].end())    //找出来id
                    {
                        map<string,value>::iterator temiter = iter; //保存当前指针
						leftnum=iter->second.namenum; //cout<<leftnum<<endl;
						leftvar=iter->second.name;
                        if(iter->second.cons==0) //不是const id
                        {
						
							if(cosnum==1){cout<<"NOT CONST"<<endl;exit(0);}
							if(p[0]==NULL)
							{
                            	returnnum[0]=leftnum;
                            	returnvar=leftvar;
                            	break;
							}
							else
							{
								p[0]->doo(cosnum,iter->second.b);
								
								rightnum=returnnum[0];
								rightvar=returnvar;
								if(rightvar==' ') rightnum=4*rightnum;
								else
								{
									//cout<<"  var t"<<temvarnum++<<endl;
									if(status==0||status==1)   //记录变量
									{
										quchar[status].push("var t"+to_string(temvarnum));
										if(modes==tigger||modes==riscv)eVarMap("t"+to_string(temvarnum),1);
									} temvarnum++;
									if(status==2||status==3)
									{
										if(modes==0)cout<<"  t"<<temvarnum-1<<" = "<<"4 * "<<rightvar<<rightnum<<endl;
										if(modes==tigger||modes==riscv)addyun("t"+to_string(temvarnum-1),"4",jiexi(rightvar,rightnum),"*");
									}
									rightvar='t';
									rightnum=temvarnum-1;
								}
								//cout<<"  var t"<<temvarnum++<<endl;
								if(status==0||status==1)   //记录变量
								{
									quchar[status].push("var t"+to_string(temvarnum));
									if(modes==tigger||modes==riscv)eVarMap("t"+to_string(temvarnum),1);
								} temvarnum++;
								if(val==temiter->second.b[0])
								{
									if(status==2||status==3)
									{
										if(modes==0)cout<<"  t"<<temvarnum-1<<" = "<<leftvar<<leftnum<<" ["<<rightvar<<rightnum<<']'<<endl;
										if(modes==tigger||modes==riscv)arrUse("t"+to_string(temvarnum-1),jiexi(leftvar,leftnum),jiexi(rightvar,rightnum));
									}
								}
								else
								{
									if(status==2||status==3)
									{
										if(modes==0)cout<<"  t"<<temvarnum-1<<" = "<<leftvar<<leftnum<<" + "<<rightvar<<rightnum<<endl;
										if(modes==tigger||modes==riscv)addyun("t"+to_string(temvarnum-1),jiexi(leftvar,leftnum),jiexi(rightvar,rightnum),"+");
									}
								}
								
								returnvar='t';
								returnnum[0]=temvarnum-1;
								break;
							}
							
                        }
                        else //是const id
                        {
                            if(p[0]==NULL) 
                            {
                            	returnnum[0]=iter->second.val;
								returnvar=' ';
                            	
                            }
							else
							{
								p[0]->doo(cosnum,iter->second.b);
								
								rightnum=returnnum[0];
								rightvar=returnvar;       //cout<<rightnum<<endl;
								if(rightvar!=' ') //数组参数非数
								{
									if(cosnum==1){cout<<"NOT CONST"<<endl;exit(0);}
									//cout<<"  var t"<<temvarnum++<<endl;
									if(status==0||status==1)   //记录变量
									{
										quchar[status].push("var t"+to_string(temvarnum));
										if(modes==tigger||modes==riscv)eVarMap("t"+to_string(temvarnum),1);
									} temvarnum++;
									if(status==2||status==3)
									{
										
										if(modes==0)cout<<"  t"<<temvarnum-1<<" = "<<"4 * "<<rightvar<<rightnum<<endl;
										if(modes==tigger||modes==riscv)addyun("t"+to_string(temvarnum-1),"4",jiexi(rightvar,rightnum),"*");
										
									}
									rightvar='t';
									rightnum=temvarnum-1;
									//cout<<"  var t"<<temvarnum++<<endl;
									if(status==0||status==1)   //记录变量
									{
										quchar[status].push("var t"+to_string(temvarnum));
										if(modes==tigger||modes==riscv)eVarMap("t"+to_string(temvarnum),1);
									} temvarnum++;
									if(status==2||status==3)
									{
										if(val==temiter->second.b[0])
										{
											if(modes==0)cout<<"  t"<<temvarnum-1<<" = "<<leftvar<<leftnum<<" ["<<rightvar<<rightnum<<']'<<endl;
											if(modes==tigger||modes==riscv)arrUse("t"+to_string(temvarnum-1),jiexi(leftvar,leftnum),jiexi(rightvar,rightnum));
										}
										else
										{
											if(modes==0)cout<<"  t"<<temvarnum-1<<" = "<<leftvar<<leftnum<<" + "<<rightvar<<rightnum<<endl;
											if(modes==tigger||modes==riscv)addyun("t"+to_string(temvarnum-1),jiexi(leftvar,leftnum),jiexi(rightvar,rightnum),"+");
										}
									}
									returnvar='t';
									returnnum[0]=temvarnum-1;
								}
								else  //数组参数为数
								{
									if(val==temiter->second.b[0])
									{
										returnnum[0]=temiter->second.a[rightnum];
										returnvar=' ';
									}
									else
									{
										rightnum=4*rightnum;
										if(status==0||status==1)   //记录变量
										{
											quchar[status].push("var t"+to_string(temvarnum));
											if(modes==tigger||modes==riscv)eVarMap("t"+to_string(temvarnum),1);
										} temvarnum++;
										if(status==2||status==3)
										{
											if(modes==0)cout<<"  t"<<temvarnum-1<<" = "<<leftvar<<leftnum<<" + "<<rightvar<<rightnum<<endl;
											if(modes==tigger||modes==riscv)addyun("t"+to_string(temvarnum-1),jiexi(leftvar,leftnum),jiexi(rightvar,rightnum),"+");
										}
									}
									
								}
							}
							break;
                        }
                        
                    }
                    i--;    
                }
                if(i==-1){cout<<"NO ID"<<endl;exit(0);}
			} 
		    else if(choose==3)
           	 {
			returnvar=' ';
			returnnum[0]=val;//cout<<returnnum[0]<<endl;
			}

		    
		    break;

		case exps:
			if(p[0]==NULL) //最左
			{
				p[1]->doo(cosnum);
				rightnum = returnnum[0];  //cout<<val<<endl;
				rightvar = returnvar;
				if(rightvar==' ')
				{
					rightnum=constarr[val]*rightnum; returnnum[0]=rightnum; returnvar=' ';
				}
				else
				{
					//cout<<"  var t"<<temvarnum++<<endl;
					if(status==0||status==1)   //记录变量
					{
						quchar[status].push("var t"+to_string(temvarnum));
						if(modes==tigger||modes==riscv)eVarMap("t"+to_string(temvarnum),1);
					} temvarnum++;
					if(status==2||status==3)
					{
						if(modes==0)cout<<"  t"<<temvarnum-1<<" = "<<constarr[val]<<" * "<<rightvar<<rightnum<<endl;
						if(modes==tigger||modes==riscv)addyun("t"+to_string(temvarnum-1),to_string(constarr[val]),jiexi(rightvar,rightnum),"*");
					}
					returnnum[0]=temvarnum-1;
					returnvar='t';
				}
			}
			else    //后续
			{
				p[0]->doo(cosnum,constarr);
				leftnum = returnnum[0]; 
				leftvar = returnvar;
				p[1]->doo(cosnum);
				rightnum = returnnum[0];  //cout<<val<<endl;
				rightvar = returnvar;
				if(rightvar==' ')
				{
					rightnum=constarr[val]*rightnum;
					if(leftvar==' ')
					{
						returnnum[0]=leftnum+rightnum;
						returnvar=' ';
					}
					else
					{
						//cout<<"  var t"<<temvarnum++<<endl;
						if(status==0||status==1)   //记录变量
						{
							quchar[status].push("var t"+to_string(temvarnum));
							if(modes==tigger||modes==riscv)eVarMap("t"+to_string(temvarnum),1);
						} temvarnum++;
						if(status==2||status==3)
						{
							if(modes==0)cout<<"  t"<<temvarnum-1<<" = "<<rightnum<<" + "<<leftvar<<leftnum<<endl;
							if(modes==tigger||modes==riscv)addyun("t"+to_string(temvarnum-1),to_string(rightnum),jiexi(leftvar,leftnum),"+");
						}
						returnnum[0]=temvarnum-1;
						returnvar='t';
					}
					
				}
				else
				{
					//cout<<"  var t"<<temvarnum++<<endl;
					if(status==0||status==1)   //记录变量
					{
						quchar[status].push("var t"+to_string(temvarnum));
						if(modes==tigger||modes==riscv)eVarMap("t"+to_string(temvarnum),1);
					} temvarnum++;
					if(status==2||status==3)
					{
						if(modes==0)cout<<"  t"<<temvarnum-1<<" = "<<constarr[val]<<" * "<<rightvar<<rightnum<<endl;
						if(modes==tigger||modes==riscv)addyun("t"+to_string(temvarnum-1),to_string(constarr[val]),jiexi(rightvar,rightnum),"*");
					}
					//cout<<"  var t"<<temvarnum++<<endl;
					if(status==0||status==1)   //记录变量
					{
						quchar[status].push("var t"+to_string(temvarnum));
						if(modes==tigger||modes==riscv)eVarMap("t"+to_string(temvarnum),1);
					} temvarnum++;
					if(status==2||status==3)
					{
						if(modes==0)cout<<"  t"<<temvarnum-1<<" = "<<'t'<<temvarnum-2<<" + "<<leftvar<<leftnum<<endl;
						if(modes==tigger||modes==riscv)addyun("t"+to_string(temvarnum-1),"t"+to_string(temvarnum-2),jiexi(leftvar,leftnum),"+");
					}
					returnnum[0]=temvarnum-1;
					returnvar='t';
				}
				
			}

			break;
		     case funcdef:
			 {	
				 //cout<<"--------"<<endl;
				 int ii=0; int varstore,numstore,labstore;
				 while(ii<2)
				 {
					 if(ii==0){varstore=varnum; numstore=temvarnum; labstore=label; func1=false;}
					 else{varnum=varstore; temvarnum=numstore; label=labstore; func1=true;}
				 if(modes==tigger||modes==riscv)clearAll();
				 temNum=0;//栈初始位置
				 varnum2 =varnum,temvarnum2=temvarnum;
                 		nowmap=1;
				 status=1;
				 pnum=0;
				 if(choose==1||choose==2)p[0]->doo();
				 int arr =3; //函数无返回值
				if(choose==2||choose==4) arr=4;
                value tes{'F',0,NULL,NULL,arr,0,0};
				mp[0]["f_"+id]=tes;
				nowmap=0; //block前降
				p[1]->doo();                                //遍历函数获取var
				mp[1].erase(mp[1].begin(),mp[1].end());   //消除改变

				status=2; 
				varnum=varnum2; temvarnum=temvarnum2;
				int sta;  //栈大小 /4
				if(id=="main")
				{
					 sta=temNum+gtNum;     //函数内变量  全局t
					 for(map<string,eeyoreVar>::iterator iters=EMap.begin();iters!=EMap.end();iters++)
					 {
						 if(iters->first[0]=='t'&&iters->second.type==0)
						 {
							 string ss=iters->second.mem;
							 iters->second.mem=to_string(atoi(ss.c_str())+temNum);  //全局t，初始化时产生，函数内位置在参数和变量后面
						 }
					 }
				}
				else sta=temNum;

				for(int i=0;i<=25;i++)    //寄存器位置在最后
				{
					regInStack[i]=sta++;
				}
				pnum=0;
				 if(choose==1||choose==2)
				 {
                	nowmap=1;
                	p[0]->doo();
                	if(modes==0)cout<<"f_"<<id<<" ["<<pnum<<']'<<endl;
					if(modes==tigger&&func1==false)cout<<"f_"<<id<<" ["<<pnum<<']'<<" ["<<sta<<']'<<endl;
					sizeForRisc=(sta/4+1)*16;
					if(modes== riscv&&func1==false)risc_func(id);
				 }
				 else 
				 {
					 if(modes==0)cout<<"f_"<<id<<" [0"<<']'<<endl;
					 if(modes==tigger&&func1==false)cout<<"f_"<<id<<" [0"<<']'<<" ["<<sta<<']'<<endl;
					 sizeForRisc=(sta/4+1)*16;
					 if(modes== riscv&&func1==false)risc_func(id);
				 }

				for(int i=15;i<=25;i++)     
				{
					if(modes==tigger&&func1==false)cout<<"store "<<regName[i]<<" "<<regInStack[i]<<endl;
					if(modes== riscv&&func1==false)risc_store(regName[i],to_string(regInStack[i]));
				}
				if(modes==tigger&&func1==false)cout<<"s11 = 4"<<endl;
				if(modes==riscv&&func1==false)risc_integer("s11","4");
				if(id=="main")                        //main开始输出全局var
				  while(!quchar[0].empty())
				{  if(modes==0)cout<<"  "<<quchar[0].front()<<endl; qustore.push(quchar[0].front()); quchar[0].pop(); }
				while(!qustore.empty())
				{ quchar[0].push(qustore.front()); qustore.pop();}
				 while(!quchar[1].empty())           //函数内var
				{  if(modes==0)cout<<"  "<<quchar[1].front()<<endl; quchar[1].pop(); }

				if(id=="main")
				{	
					varnum2=varnum,temvarnum2=temvarnum;
					status=3;
					//mp[0].erase(mp[0].begin(),mp[0].end());
					nowmap=0;
					while(!qunode.empty())
					{
						qunode.front()->doo();       //再访问一遍全局var，输出
						qunode.pop();
					}
					varnum=varnum2; temvarnum=temvarnum2;
					nowmap=1;
				}

				status=2;              //第二次函数遍历，输出

				
                nowmap=0;
                p[1]->doo();
				
				if(modes==tigger&&func1==false)cout<<"end f_"<<id<<endl;
				if(modes==riscv&&func1==false)risc_endFunc(id);
				nowmap=0;  status = 0;
				mp[1].erase(mp[1].begin(),mp[1].end());

				 ii++;}
			 }
                break;
            case funcparams:
                if(choose==1){p[0]->doo();p[1]->doo();}
                if(choose==2)p[0]->doo();
                break;
            case funcparam:
                if(choose==1)
                {
					if(status==1)
					{
						if(modes==tigger||modes==riscv) eVarMap("p"+to_string(pnum),1);
					}
                    value tes{'p',pnum++,NULL,NULL,0,0};
					mp[nowmap][id]=tes;
					
                }
				if(choose==2)
				{
					if(status==1) 
					{
						if(modes==tigger||modes==riscv)eVarMap("p"+to_string(pnum),1);
					}
					if(p[0]==NULL) //一个维度
					{
						int *constarry2=new int[MAXL]; //为用数组
						constarry2[0]=1;
						constarry2[1]=1;
						value tes{'p',pnum++,NULL,constarry2,1,0,0};
						mp[nowmap][id]=tes;
					}
					else
					{
						A[0]=1;A[1]=0;
				    	p[0]->doo(1);
						int weidu=A[0];
						int *constarry2=new int[MAXL]; //为用数组
						int j=1,j2;
						for(int i=weidu;i>=1;i--)
						{	
							j2=A[i]; 
							constarry2[i]=j;
							j*=j2;
						}
						constarry2[0]=weidu;
						value tes{'p',pnum++,NULL,constarry2,1,0,0};
						mp[nowmap][id]=tes;
					}
				}
                break;

			case arrysize:
				if(p[0]!=NULL)p[0]->doo(1);
				A[0]++;
				p[1]->doo(1);
				A[A[0]]=returnnum[0];
				break;
			case block:
				nowmap++;
                if(p[0]!=NULL)p[0]->doo();
				mp[nowmap].erase(mp[nowmap].begin(),mp[nowmap].end());
				nowmap--;
                break;
            case blockitem:
                if(p[0]!=NULL)p[0]->doo();
                p[1]->doo();
                break;
            case stmt:
                if(choose==1)
                {
                    int i=nowmap;
                
                    while(i>=0)
                    {
                        iter=mp[i].find(id);
                    
                        if(iter!=mp[i].end())    //找出来id
                        {
                            map<string,value>::iterator temiter = iter;

                            if(iter->second.cons==1){cout<<"const不能赋值"<<endl;exit(0);}
                            if(p[0]==NULL)   //数赋值
                            {
                                if(iter->second.arr==1){cout<<"数组需要维度"<<endl;exit(0);}
                                p[1]->doo(0);
                                rightnum=returnnum[0];
                                rightvar=returnvar;
                                if(status==2||status==3)
								{
									if(modes==0)cout<<"  "<<temiter->second.name<<temiter->second.namenum<<" = "<<rightvar<<rightnum<<endl;
								
									if(modes==tigger||modes==riscv)varAssign(jiexi(temiter->second.name,temiter->second.namenum),jiexi(rightvar,rightnum));
								}
                            }
                            else //数组赋值
                            {
                                if(iter->second.arr==0){cout<<"数不能有维度"<<endl;exit(0);}
                                p[0]->doo(0,iter->second.b);//cout<<"1111"<<endl;
                                leftvar=returnvar;
                                leftnum=returnnum[0];
                                p[1]->doo(0);
                                rightvar=returnvar;
                                rightnum=returnnum[0];
                                if(leftvar==' ')
                                {
                                    leftnum*=4;
                                }
                                else
                                {
                                    //cout<<"  var t"<<temvarnum++<<endl;
									if(status==0||status==1)   //记录变量
									{
										quchar[status].push("var t"+to_string(temvarnum));
										if(modes==tigger||modes==riscv)eVarMap("t"+to_string(temvarnum),1);
									} temvarnum++;
                                    if(status==2||status==3)
									{
										if(modes==0)cout<<"  t"<<temvarnum-1<<" = "<<"4 * "<<leftvar<<leftnum<<endl;
										if(modes==tigger||modes==riscv)addyun("t"+to_string(temvarnum-1),"4",jiexi(leftvar,leftnum),"*");
									}
                                    leftvar='t';leftnum=temvarnum-1;
                                }
                                if(status==2||status==3){if(modes==0)cout<<"  "<<temiter->second.name<<temiter->second.namenum<<" [";}
                                if(status==2||status==3){if(modes==0)cout<<leftvar<<leftnum<<"] "<<" = "<<rightvar<<rightnum<<endl;}
                                if(status==2||status==3)
								{
								if(modes==tigger||modes==riscv)
								arrAssig(jiexi(rightvar,rightnum),jiexi(temiter->second.name,temiter->second.namenum),jiexi(leftvar,leftnum));
								}
                            }break;
                            
                        }
                        i--;
                    }
                    if(i==-1){cout<<"NO ID"<<endl;exit(0);}
                }
		      if(choose==2)
                {
                    p[0]->doo(0);
                    
                }
                if(choose==4)
                {
                    p[0]->doo(0);
                }
                if(choose==5)
                {
                    int temlab ,temlab2;
                    temlab=label; temlab2=label+1;
                    if(status==2||status==3)
					{
						label+=2;
						if(modes==tigger||modes==riscv)storeReg();
						if(modes==tigger||modes==riscv)clearAll();
					}
                    p[0]->doo(0);
                    leftvar=returnvar;
                    leftnum=returnnum[0];
                    if(status==2||status==3)
					{
						if(modes==0)
						cout<<"  if "<<leftvar<<leftnum<<" == 0 goto l"<<temlab<<endl;

						if(modes==tigger||modes==riscv)
						{
							int v1=load(jiexi(leftvar,leftnum),false);
					 		if(modes==tigger&&func1==false)cout<<" if "<<regName[v1]<<" == "<<"x0"<<" goto l"<<temlab<<endl;
							if(modes==riscv&&func1==false) cout<<"	beq "<<regName[v1]<<",x0,.l"<<temlab<<endl;
						}
					}
                    p[1]->doo(0);
                    if(status==2||status==3)
					{
						if(modes==tigger||modes==riscv)storeReg();
						if(modes==tigger||modes==riscv)clearAll();
						if(modes==tigger&&func1==false)cout<<"  goto l"<<temlab2<<endl;
						if(modes==riscv&&func1==false)cout<<"	j .l"<<temlab2<<endl;
					}
                    if(status==2||status==3)
					{
						if(modes==tigger&&func1==false)
						cout<<"l"<<temlab<<':'<<endl;
						if(modes==riscv&&func1==false)
						cout<<".l"<<temlab<<':'<<endl;
					}
                    p[2]->doo(0);
                    if(status==2||status==3)
					{
						if(modes==tigger||modes==riscv)storeReg();
						if(modes==tigger||modes==riscv)clearAll();
						if(modes==tigger&&func1==false)cout<<'l'<<temlab2<<':'<<endl;
						if(modes==riscv&&func1==false)cout<<".l"<<temlab2<<':'<<endl;
					}
                }
                if(choose==6)
                {
                    int temlab;
                    temlab=label;
                    if(status==2||status==3)
					{
						label+=1;
						if(modes==tigger||modes==riscv)storeReg();
						if(modes==tigger||modes==riscv)clearAll();
					}
                    p[0]->doo(0);
                    leftvar=returnvar;
                    leftnum=returnnum[0];
                    if(status==2||status==3)
					{
						if(modes==0)
						cout<<"  if "<<leftvar<<leftnum<<" == 0 goto l"<<temlab<<endl;
						if(modes==tigger||modes==riscv)
						{
							int v1=load(jiexi(leftvar,leftnum),false);
					 		if(modes==tigger&&func1==false)cout<<" if "<<regName[v1]<<" == "<<"x0"<<" goto l"<<temlab<<endl;
							if(modes==riscv&&func1==false)
							cout<<"	beq "<<regName[v1]<<",x0,.l"<<temlab<<endl;
						}
					}
                    p[1]->doo(0);
                    
                    if(status==2||status==3)
					{
						if(modes==tigger||modes==riscv)storeReg();
						if(modes==tigger||modes==riscv)clearAll();
						if(modes==tigger&&func1==false)cout<<"l"<<temlab<<':'<<endl;
						if(modes==riscv&&func1==false)cout<<".l"<<temlab<<':'<<endl;
					}
                    
                }
                     if(choose==7)
                {
                    int temlab ,temlab2;
                    temlab=label; temlab2=label+1;
                    if(status==2||status==3)
					{
						if(modes==tigger||modes==riscv)storeReg();
						if(modes==tigger||modes==riscv)clearAll();
						label+=2;
					}
                    int Fbrlab = brlab,Fconlab = conlab,Fhavewhile = havewhile;
                    havewhile=1;
                    brlab=temlab2;
                    conlab=temlab;
                    if(status==2||status==3)
					{
						if(modes==tigger&&func1==false)
						cout<<"l"<<temlab<<':'<<endl;
						if(modes==riscv&&func1==false)cout<<".l"<<temlab<<':'<<endl;
					}
                    p[0]->doo(0);
                    leftvar=returnvar;
                    leftnum=returnnum[0];
                    if(status==2||status==3)
					{
						if(modes==0)
						cout<<"  if "<<leftvar<<leftnum<<" == 0 goto l"<<temlab2<<endl;

						if(modes==tigger||modes==riscv)
						{
							int v1=load(jiexi(leftvar,leftnum),false);
					 		if(modes==tigger&&func1==false)cout<<" if "<<regName[v1]<<" == "<<"x0"<<" goto l"<<temlab2<<endl;
							if(modes==riscv&&func1==false)
							cout<<"	beq "<<regName[v1]<<",x0,.l"<<temlab2<<endl;
						}
					}
                    p[1]->doo(0);
                    if(status==2||status==3)
					{
						if(modes==tigger||modes==riscv)storeReg();
						if(modes==tigger||modes==riscv)clearAll();
						if(modes==tigger&&func1==false)cout<<"  goto l"<<temlab<<endl;
						if(modes==riscv&&func1==false)cout<<"	j .l"<<temlab<<endl;
					}
                    if(status==2||status==3)
					{
						if(modes==tigger&&func1==false)cout<<"l"<<temlab2<<':'<<endl;
						if(modes==riscv&&func1==false)cout<<".l"<<temlab2<<':'<<endl;
					}

                    brlab = Fbrlab,conlab = Fconlab,havewhile = Fhavewhile;
                }
                if(choose==8)
                {
                    if(havewhile==1) 
                        if(status==2||status==3)
						{
							if(modes==tigger||modes==riscv)storeReg();
							if(modes==tigger||modes==riscv)clearAll();
							if(modes==tigger&&func1==false)cout<<"  goto l"<<brlab<<endl;
							if(modes==riscv&&func1==false)cout<<"	j .l"<<brlab<<endl;
						}
                }
                if(choose==9)
                {
                    if(havewhile==1) 
                        if(status==2||status==3)
						{
							if(modes==tigger||modes==riscv)storeReg();
							if(modes==tigger||modes==riscv)clearAll();
							if(modes==tigger&&func1==false)cout<<"  goto l"<<conlab<<endl;
							if(modes==riscv&&func1==false)cout<<"	j .l"<<conlab<<endl;
						}
                }
				if(choose==10)
				{
					p[0]->doo(0);
					if(status==2||status==3)
					{
						if(modes==0)
						cout<<"  return "<<returnvar<<returnnum[0]<<endl;
						if(modes==tigger||modes==riscv)storeReg();
						if(modes==tigger||modes==riscv)clearAll();
						if(modes==tigger||modes==riscv)
						{
							int reReg=load(jiexi(returnvar,returnnum[0]),true);
							if(regName[reReg]!="a0")
							{
								if(modes==tigger&&func1==false)cout<<"a0 = "<<regName[reReg]<<endl;
								if(modes==riscv&&func1==false)risc_mv("a0",regName[reReg]);
							}
							for(int i=15;i<=25;i++)     
							{
								if(modes==tigger&&func1==false)cout<<"load "<<regInStack[i]<<" "<<regName[i]<<endl;
								if(modes==riscv&&func1==false)risc_load(to_string(regInStack[i]),regName[i]);
							}
							if(modes==tigger&&func1==false)cout<<"  return"<<endl;
							if(modes==riscv&&func1==false)risc_return();
						}
					}
				}
				if(choose==11)
				{
					if(status==2||status==3)
					{
						if(modes==0)
						cout<<"  return"<<endl;
						if(modes==tigger||modes==riscv)storeReg();
						if(modes==tigger||modes==riscv)clearAll();
						for(int i=15;i<=25;i++)     
						{
							if(modes==tigger&&func1==false)cout<<"load "<<regInStack[i]<<" "<<regName[i]<<endl;
							if(modes==riscv&&func1==false)risc_load(to_string(regInStack[i]),regName[i]);
						}
						if(modes==tigger&&func1==false)cout<<"  return"<<endl;
						if(modes==riscv&&func1==false)risc_return();
					}
				}
                break;
			
		case unaryexp:
			
			if(choose==2)
			{
				if(p[0]->choose==1){p[1]->doo(cosnum);}
				if(p[0]->choose==2)
				{
					p[1]->doo(cosnum);
					rightvar=returnvar;
					rightnum=returnnum[0];
					if(rightvar==' ')
					{
						returnnum[0]=-rightnum;
						returnvar=' ';
						break;
					}
					if(status==0||status==1)   //记录变量
					{
						quchar[status].push("var t"+to_string(temvarnum));
						if(modes==tigger||modes==riscv)eVarMap("t"+to_string(temvarnum),1);
					} temvarnum++;
					if(status==2||status==3)
					{
						if(modes==0)cout<<"  t"<<temvarnum-1<<" = -"<<rightvar<<rightnum<<endl;
						if(modes==tigger||modes==riscv)opyun("t"+to_string(temvarnum-1),jiexi(rightvar,rightnum),"-");
					}
					
					returnvar='t';  returnnum[0]=temvarnum-1;
				}
				if(p[0]->choose==3)
				{
					p[1]->doo(cosnum);
					rightvar=returnvar;
					rightnum=returnnum[0];
					if(rightvar==' ')
					{
						returnnum[0]=!rightnum;
						returnvar=' ';
						break;
					}
					if(status==0||status==1)   //记录变量
					{
						quchar[status].push("var t"+to_string(temvarnum));
						if(modes==tigger||modes==riscv)eVarMap("t"+to_string(temvarnum),1);
					} temvarnum++;
					if(status==2||status==3)
					{
						if(modes==0)cout<<"  t"<<temvarnum-1<<" = !"<<rightvar<<rightnum<<endl;
						if(modes==tigger||modes==riscv)opyun("t"+to_string(temvarnum-1),jiexi(rightvar,rightnum),"!");
					}

					returnvar='t';  returnnum[0]=temvarnum-1;
				}
			}
			if(choose==3)
			{	
				iter=mp[0].find("f_"+id);
                if(iter==mp[0].end())   {  cout<<"NO FUNC ID"<<endl; exit(0);}
				if(iter->second.arr==3)
				{
					if(id=="starttime")
					{
						
						if(status==2||status==3)
						{
							if(modes==0)cout<<"  param"<<' '<<val<<endl;
							if(modes==0)cout<<"  call f__sysy_starttime"<<endl;
							if(modes==tigger||modes==riscv)storeGlo();
							if(modes==tigger||modes==riscv)saveReg();
							if(modes==tigger&&func1==false)cout<<"a0 = "<<val<<endl;
							if(modes==riscv&&func1==false)risc_integer("a0",to_string(val));
							if(modes==tigger&&func1==false)cout<<"  call f__sysy_starttime"<<endl;
							if(modes==riscv&&func1==false)cout<<"  call _sysy_starttime"<<endl;
							if(modes==tigger||modes==riscv)returnReg();
						}
						break;
					}
					if(id=="stoptime")
					{
						
						if(status==2||status==3)
						{
							if(modes==0)cout<<"  param"<<' '<<val<<endl;
							if(modes==0)cout<<"  call f__sysy_stoptime"<<endl;
							if(modes==tigger||modes==riscv)storeGlo();
							if(modes==tigger||modes==riscv)saveReg();
							if(modes==tigger&&func1==false)cout<<"a0 = "<<val<<endl;
							if(modes==riscv&&func1==false)risc_integer("a0",to_string(val));
							if(modes==tigger&&func1==false)cout<<"  call f__sysy_stoptime"<<endl;
							if(modes==riscv&&func1==false)cout<<"  call _sysy_stoptime"<<endl;
							if(modes==tigger||modes==riscv)returnReg();
						}
						break;
					}
                	if(status==2||status==3)
					{
						if(modes==0)cout<<"call f_"<<id<<endl;
						if(modes==tigger||modes==riscv)storeGlo();
						if(modes==tigger||modes==riscv)saveReg();
						if(modes==tigger&&func1==false)cout<<"call f_"<<id<<endl;
						if(modes==riscv&&func1==false)cout<<"  call "<<id<<endl;
						if(modes==tigger||modes==riscv)returnReg();
					}
				}
				else if(iter->second.arr==4)
				{
					//cout<<"  var t"<<temvarnum++<<endl;
					if(status==0||status==1)   //记录变量
					{
						quchar[status].push("var t"+to_string(temvarnum));
						if(modes==tigger||modes==riscv)eVarMap("t"+to_string(temvarnum),1);
					} temvarnum++;
					if(status==2||status==3)
					{
						if(modes==0)cout<<"  t"<<temvarnum-1<<" = call f_"<<id<<endl;
						if(modes==tigger||modes==riscv)storeGlo();
						if(modes==tigger||modes==riscv)saveReg();
						if(modes==tigger&&func1==false)cout<< "call f_"<<id<<endl;
						if(modes==riscv&&func1==false)cout<< "  call "<<id<<endl;
						if(modes==tigger&&func1==false)cout<<"store a0 "<<EMap.find("t"+to_string(temvarnum-1))->second.mem<<endl;
						if(modes==riscv&&func1==false)risc_store("a0",EMap.find("t"+to_string(temvarnum-1))->second.mem);
						if(modes==tigger||modes==riscv)returnReg();
					}
					returnvar='t';
					returnnum[0]=temvarnum-1;
				}
				else {  cout<<id<<"ID不是 FUNC"<<endl; exit(0);}
				
			}
			if(choose==4||choose==5)
			{
				iter=mp[0].find("f_"+id);
                if(iter==mp[0].end()) {cout<<"NO FUNC ID"<<endl;exit(0);}
				int arr=iter->second.arr;
				if(arr!=3&&arr!=4)
				{  cout<<id<<"ID不是 FUNC"<<endl; exit(0);}
				p[0]->doo(0);
				int i=val; //char stvar; int stnum;
				while(i>0)
				{
					stkchar2.push(stkchar.top());
					stkchar.pop();
					stkint2.push(stkint.top());
					stkint.pop();
					i--;
				}
				i=val; 
				if(status==2||status==3)
				{
					if(modes==tigger||modes==riscv)storeGlo();
					if(modes==tigger||modes==riscv)saveReg();
				}
				int countReg=0;  //第几个传参寄存器
				while(i>0)
				{
					if(status==2||status==3)
					{
						if(modes==0)cout<<"  param "<<stkchar2.top()<<stkint2.top()<<endl;
						//cout<<"-------"<<jiexi(stkchar2.top(),stkint2.top())<<endl;
						if(modes==tigger||modes==riscv)loadInReg(jiexi(stkchar2.top(),stkint2.top()),countReg);
						countReg++;
					}
					stkchar2.pop(); stkint2.pop();
					i--;
				}
				if(arr==3)
				{
                	if(status==2||status==3)
					{
						if(modes==tigger&&func1==false)cout<<"  call f_"<<id<<endl;
						if(modes==riscv&&func1==false)cout<<"  call "<<id<<endl;
						if(modes==tigger||modes==riscv)returnReg();
					}
				}
				else if(arr==4)
				{
					//cout<<"  var t"<<temvarnum++<<endl;
					if(status==0||status==1)   //记录变量
					{
						quchar[status].push("var t"+to_string(temvarnum));
						if(modes==tigger||modes==riscv)eVarMap("t"+to_string(temvarnum),1);
					} temvarnum++;
					if(status==2||status==3)
					{
						if(modes==0)cout<<"  t"<<temvarnum-1<<" = call f_"<<id<<endl;
						if(modes==tigger&&func1==false)cout<<"  call f_"<<id<<endl;
						if(modes==riscv&&func1==false)cout<<"  call "<<id<<endl;
						if(modes==tigger&&func1==false)cout<<"store a0 "<<EMap.find("t"+to_string(temvarnum-1))->second.mem<<endl;
						if(modes==riscv&&func1==false)risc_store("a0",EMap.find("t"+to_string(temvarnum-1))->second.mem);
						if(modes==tigger||modes==riscv)returnReg();
					}
					returnvar='t';
					returnnum[0]=temvarnum-1;
				}
			}
			break;
		case functionrparams:
			if(choose==1)
			{
				p[0]->doo();
				p[1]->doo(0);
				stkchar.push(returnvar);
				stkint.push(returnnum[0]);
			}
			else
			{
				p[0]->doo(0);
				stkchar.push(returnvar);
				stkint.push(returnnum[0]);
			}
			
			break;
		default:
		    break;
		}
	} 


