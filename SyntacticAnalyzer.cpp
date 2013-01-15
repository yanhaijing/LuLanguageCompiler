#include"MyHeader.h"
#include<stdio.h>
#include<string.h>

#define GRAMMARLEN 14//定义问法表长度
#define FIRSTTABLELEN 12//定义优先表长度
#define VTLEN 10//定义vp集的长度
#define STACLSIZE 100//第一堆栈大小
#define NULL {1000,1000,"end"}


extern struct sBinaryRelation LexicalAnalyzer(char *pcSubName);//来自lexicalanalyzer中的函数

struct sQuarternaryForm //四元式数组
{
	int iOperateScript;//操作符的下标
	struct sBinaryRelation sArgLeft;
	struct sBinaryRelation sArgRight;
	struct sBinaryRelation sArgResult;
	//next;//指向下一个
};

struct sVp//vt集的结构体
{
	struct sBinaryRelation sHead;//标识符
	struct sBinaryRelation asVt[VTLEN];//vp集
};

struct sStack//栈结构体
{
	char cType;
	struct sBinaryRelation sPlace;
};

static struct sBinaryRelation aasQuarternaryForm[GRAMMARLEN][10]={
	{{4,0,"start"},{4,0,"define"},{3,8,""},{4,0,"operate"},NULL},//非终结符start开始符号全部归结到start程序的开始符号start->define;operate;
	{{4,0,"type"},{5,0,"special"},{4,0,"name"},NULL},//define->type special name
	{{4,0,"type"},{0,0,""},NULL},//type->key
	{{4,0,"name"},{1,0,""},NULL},//name->var
	{{4,0,"operate"},{1,0,""},{3,1,""},{4,0,"operater"},{3,8,""},{4,0,"operate"},NULL},//operate->var=operater;operate
	{{4,0,"operate"},{1,0,""},{3,1,""},{4,0,"operater"},NULL},//operate->var=operater
	{{4,0,"operater"},{4,0,"left"},{3,2,""},{4,0,"right"},NULL},//operater->left+right
	{{4,0,"operater"},{4,0,"left"},NULL},//operater->left
	{{4,0,"operater"},{4,0,"left"},{3,3,""},{4,0,"right"},NULL},//operater->left-right
	{{4,0,"left"},{4,0,"left"},{3,4,""},{4,0,"other"},NULL},//left->left*other
	{{4,0,"left"},{4,0,"left"},{3,5,""},{4,0,"other"},NULL},//left->left/other
	{{4,0,"left"},{4,0,"other"},NULL},//left->other
	{{4,0,"other"},{4,0,"name"},NULL},//other->name
	{{4,0,"other"},{2,0,""},NULL}//other->const
};//问法定义结束

static struct sBinaryRelation asFirstTableIndex[FIRSTTABLELEN]={
	NULL,{3,1,"="},{3,2,"+"},{3,3,"-"},{3,4,"*"},{3,5,"/"},{3,8,";"},{0,0,"key"},{1,0,"var"},{2,0,"const"},{5,0,"spe"},{3,9,"#"}
};//定义优先表索引函数数组下标即为在优先表中的下标

static char acFirstTable[FIRSTTABLELEN][FIRSTTABLELEN]={
	{'0','=','+','-','*','/',';','0','1','2','5','3'},//
	{'=',0},// =
	{'+',0},// +
	{'-',0},// -
	{'*',0},// *
	{'/',0},// /
	{';',0},// ;
	{'0',0},// key
	{'1',0},// var
	{'2',0},// const
	{'5',0},// spe
	{'3',0},// #
};//第7 8存放var和const的优先级

char RequestVt(struct sVp asVt[],int iIndex,int iVtIndex,char fcVt)//返回要求的vt集的
{
	asVt[iIndex].sHead=aasQuarternaryForm[iIndex][0];//赋值给是谁的vt值

	if(fcVt=='F')//求firstvt
	{
		if (aasQuarternaryForm[iIndex][1].iId==4)//Q...
		{
			if(aasQuarternaryForm[iIndex][2]!=NULL)//Qa
			{
				asVt[iIndex].asVt[iVtIndex]=aasQuarternaryForm[iIndex][2];//将a付
				RequestVt(asVt,iIndex,iVtIndex+1,'F');//递归调用求F（q）
			}
			else//Q
			{
				RequestVt(asVt,iIndex,iVtIndex,'F');//递归调用求F（q）
			}
		}
		else//a....
		{
			asVt[iIndex].asVt[iVtIndex]=aasQuarternaryForm[iIndex][1];//赋值
			asVt[iIndex].asVt[iVtIndex+1]=NULL;//将下一项赋值为空
		}
		return 'F';
	}
	else//求lastvt
	{
		int i=0;
		while(aasQuarternaryForm[iIndex][i+1]!=NUll) i++;//结束是i=....Q的下标

		if(aasQuarternaryForm[iIndex][i].iId==4)//...Q
		{
			if(i!=1)//...aQ
			{
				asVt[iIndex].asVt[iVtIndex]=aasQuarternaryForm[iIndex][i-1];//赋值a
				RequestVt(asVt,iIndex,iVtIndex+1,'L');//求lastvt
			}
			else//...Q
			{
				RequestVt(asVt,iIndex,iVtIndex,'L');//求lastvt
			}
		}
		else//.......a
		{
			asVt[iIndex].asVt[iVtIndex]=aasQuarternaryForm[iIndex][i];//赋值a
			asVt[iIndex].asVt[iVtIndex+1]=NULL;//赋值NULL
		}
		return 'L';
	}
	return 'E';//出错
}

int RequestFirstTableScript(static struct sBinaryRelation RequestScript)//返回终结符在firsttable中的下标
{
	int i;
	for(i=1;i<FIRSTTABLELEN;i++)//循环查找firsttableindex表中的每一项
	{
		if(asFirstTableIndex[i].iId==RequestScript.iId&&asFirstTableIndex[i].iSubScript==RequestScript.iSubScript) return i;
	}
	return 0;//没找到失败
}

int MyRequestQuarternaryFormScript(static struct sBinaryRelation RequestScript)//返回非终结符在文发表中的偏移量
{
	int i;
	for(i=0;i<GRAMMARLEN;i++)//循环查找文发表
	{
		if(asFirstTableIndex[0][i]==RequestScript) return i;
	}
	return -1;//错误
}

int MyRequestVtLength(struct sVp RequestVtItem)//返回vt级中一项的长度
{
	int iLength=0;

	while(RequestVtItem.asVt[iLength]!=NULL) iLength++;

	return iLength;//返回vt的长度
}

int CheckGrammar(char CheckChar,int iRowScript,int iLineScript)//检查优先表是否复过值从而判断问法是否正确，若未赋值返回1付过值退出
{
	if(CheckChar==0) return 1;
	printf("优先表错误，非算符优先文法 %s %s 以有关系不能再有有限关系\n",asFirstTableIndex[iRowScript].acTempValName,asFirstTableIndex[iLineScript].acTempValName,acFirstTable[iRowScript][iLineScript]);
	exit(0);//退出
	return 0;
}

int IniFirstTable()//初始化firsttable优先表,每次规约前必须初始化
{
	struct sVp asFirstVt[GRAMMARLEN];//firstvt的集合，每项代表一个开始符，下标和文发表兼容
	struct sVp asLastVt[GRAMMARLEN];//lastvt的集合
	int i;

	for(i=1;i<FIRSTTABLELEN;i++)//初始化firsttable表中的#
	{
		acFirstTable[i][FIRSTTABLELEN-1]='>';
		acFirstTable[FIRSTTABLELEN-1][i]='<';
	}

	acFirstTable[FIRSTTABLELEN-1][FIRSTTABLELEN-1]='=';//#和#的关系为等于

	for(i=0;i<GRAMMARLEN;i++)//循环求firstvt和lastvt
	{
		RequestVt(asFirstVt,i,0,'F');//求firstvt
		RequestVt(asLastVt,i,0,'L');//求lastvt
	}

	for(i=0;i<GRAMMARLEN;i++)//循环求优先表
	{
		int iGrammarItemLen=0;//记录每项表项长度
		int j;
		while(aasQuarternaryForm[i][iGrammarItemLen]!=NULL) iGrammarItemLen++;//求的当前表项的长度

		for(j=0;j<iGrammarItemLen;j++)//循环求
		{
			if(aasQuarternaryForm[i][j].iId!=4&&aasQuarternaryForm[i][j+1].iId!=4)//j和j+1都为终结符j==j+1
			{
				int ij;//j的下标
				int ij1;//j+1的下标

				ij=RequestFirstTableScript(aasQuarternaryForm[i][j]);
				ij1=RequestFirstTableScript(aasQuarternaryForm[i][j+1]);

				if(CheckGrammar(acFirstTable[ij][ij1],ij,ij1)) acFirstTable[ij][ij1]='=';//有限关系相等
			}
			
			if((j<iGrammarItemLen-1)&&(aasQuarternaryForm[i][j].iId!=4&&aasQuarternaryForm[i][j+2].iId!=4))//xj和xj+2都为终结符
			{
				if(aasQuarternaryForm[i][j+1]==4)//xj+1为为非终结符
				{
					int ij;//j的下标
					int ij2;//j+1的下标

					ij=RequestFirstTableScript(aasQuarternaryForm[i][j]);
					ij2=RequestFirstTableScript(aasQuarternaryForm[i][j+2]);

					if(CheckGrammar(acFirstTable[ij][ij2],ij,ij2)) acFirstTable[ij][ij2]='=';//有限关系相等
				}
			}

			if(aasQuarternaryForm[i][j].iId!=4&&aasQuarternaryForm[i][j+1].iId==4)//xj为终结符xj+1为非终结符
			{
				int iVtIdenx;//非终结符的下标
				int iVtLength;//非终结符的firstvt的长度
				int i;

				iIdenx=MyRequestQuarternaryFormScript(aasQuarternaryForm[i][j+1]);//返回非终结符的在问法表中的偏移量也就是在vt中的偏移量
				iVtLength=MyRequestVtLength(asFirstVt[iIdenx]);

				for(i=0;i<iVtLength;i++)//firstvt(xj+1)的每个符号优先级大于xj
				{
					int ij;//j的下标
					int ij1;//j+1的下标
					ij=RequestFirstTableScript(aasQuarternaryForm[i][j]);//返回xj终结符在优先表中的下标
					ij1=RequestFirstTableScript(asFirstVt[iIdenx].asVt[i]);//返回xj+1非终结符的first中的每项在优先表中的下标
					if(CheckGrammar(acFirstTable[ij][ij1],ij,ij1))//判断是否优先关系已经存在
					{
						acFirstTable[ij][ij1]='<';//优先关系小于
					}
				}
			}

			if(aasQuarternaryForm[i][j].iId==4&&aasQuarternaryForm[i][j+1].iId!=4)//xj为非终结符，xj+1为终结符
			{
				int iVtIdenx;
				int iVtLength;
				int i;
				iIdenx=MyRequestQuarternaryFormScript(aasQuarternaryForm[i][j]);//返回非终结符的在问法表中的偏移量也就是在vt中的偏移量
				iVtLength=MyRequestVtLength(asLastVt[iIdenx]);

				for(i=0;i<iVtLength;i++)//firstvt(xj+1)的每个符号优先级大于xj
				{
					int ij;//j的下标
					int ij1;//j+1的下标
					ij=RequestFirstTableScript(asLastVt[iIdenx].asVt[i]);//返回xj终结符在优先表中的下标
					ij1=RequestFirstTableScript(aasQuarternaryForm[i][j+1]);//返回xj+1非终结符的first中的每项在优先表中的下标
					if(CheckGrammar(acFirstTable[ij][ij1],ij,ij1))//判断是否优先关系已经存在
					{
						acFirstTable[ij][ij1]='>';//优先关系小于
					}
				}
			}
		}
	}
	return 1;//初始化成功
}

char CheckTerminalPriority(struct sBinaryRelation sLeft,struct sBinaryRelation sRight)//返回参数的有限关系< > = 若为其他返回E
{
	int iLeftOfFirstScript;
	int iRightOfFirstScript;
	iLeftOfFirstScript=RequestFirstTableScript(sLeft);
	iRightOfFirstScript=RequestFirstTableScript(sRight);
	if(acFirstTable[iLeftOfFirstScript][iRightOfFirstScript]=='<') return '<';
	if(acFirstTable[iLeftOfFirstScript][iRightOfFirstScript]=='>') return '>';
	if(acFirstTable[iLeftOfFirstScript][iRightOfFirstScript]=='=') return '=';
	return 'E';//返回错误标志
}

int FindStackTopTerminal(struct sStack asSubStack[],int iSubStackTop,int iSubStackBottom)//返回栈顶的终结符，返回值为栈顶终结符的下标
{
	int i;
	for(i=iSubStackTop;i>=iSubStackBottom;i--)//从栈顶开始向栈底找
	{
		if(asSubStack[i].cType=='T') return i;
	}
	printf("语法分析：未知错误，搜寻栈顶终结符出错");
	exit(0);
	return -1;
}

int FindTermHead(struct sStack asSubStack[],int iSubTermTail,int iSubTempHead)//求规约中的规约头的栈下标
{
	int iTempTermHead;//临时变量存放termhead
	char cTempCheck;//临时存放终结符的优先关系

	iTempTermHead=FindStackTopTerminal(asSubStack,iSubTempHead,0);
	cTempCheck=CheckTerminalPriority(asSubStack[iSubTermTail],asSubStack[iTempTermHead]);//求的当前终结符和栈顶终结符的优先关系
	iSubTempHead=iTempTermHead-1;

	if(cTempCheck=='<')//若关系为小于
	{
		iTempTermHead=FindTermHead(asSubStack,iSubTermTail,iSubTempHead);//递归求下一个终结符
	}
	return iTempTermHead;
}

int MyFindGrammarTableHead(struct sStack asSubStack[],int iSubTermHead,int iSubTermTail)//查找可归约串若返回值小于grammar则查找成功若等于则失败，成功i为文法在文法表中的下标
{
	int i;
	for(i=0;i<GRAMMARLEN;i++)//循环扫描整个文法表
	{
		if(aasQuarternaryForm[i][1]==asSubStack[iSubTermHead])//找到可规约头部
		{
			int j;
			int k=2;
			char cFlag='Y';
			for(j=iSubTermHead+1;j<=iSubTermTail;j++,k++)//循环查找后边是否相等
			{
				if(aasQuarternaryForm[i][k]==NULL)//若文法结束尚照完则错误
				{
					cFlag='N';
					break;	
				}
				if(asSubStack[j].cType=='N'&&aasQuarternaryForm[i][k].iId==4) continue;//若当前符号为非终结符且当前文法表中文法符号也为非终结符号规约
				if(asSubStack[j].sPlace==aasQuarternaryForm[i][k]) continue //若为终结符，且相等
				
				//执行到此与本条文法不相等
				cFlag='N';
				break;	
			}
			if(cFlag=='Y') return i;//找到了i为下标
		}
	}
	return i;//失败返回grammar
}

int SyntacticAnalyzer(char *pcSubName)//语法分析器
{
	struct sStack asStack[STACLSIZE]={NULL};
	struct sStack sTempItem=NULL;//临时存放可规约的字符

	int iStackTop=0;//栈顶
	int iStackBottom=0;//栈底
	int iStackTopTerminal;//栈顶的终结符
	//int iStackNearTopTerminal=iStackBottom;//靠近栈顶的非终结符
	//int iTopTerminalInFirstScript;//栈顶非终结符在firsttable中的下标
	//int iOutTerminalInFirstScript;//栈外终结符在firsttable中的下标

	//初始化符号栈栈底有个#
	asStack[0].cType='T';//终结符
	asStack[0].sPlace.iId=3;//为运算符
	asStack[0].sPlace.iSubScript=9;//#
	//iTopTerminalInFirstScript=FIRSTTABLELEN-1;//初始化top为#
	
	if(IniFirstTable())//如果初始化优先关系表成功
	{
		while(1)//规约
		{
			if(sTempItem==NULL)//判断sTempItem是否为空
			{
				struct sBinaryRelation sTemp;
				sTemp=LexicalAnalyzer(char *pcSubName);//从词法分析拿来一个字符

				if(sTemp.iId==4)//若是非终结符
				{
					sTempItem.cType='N';
				}
				else//是终结符
				{
					sTempItem.cType='T';
				}
				sTempItem.sPlace=sTemp;
			}
			//判断是否进站还是规约及如何规约进栈
			//RequestFirstTableScript(static struct sBinaryRelation RequestScript)
			char cTemp;
			iStackTopTerminal=FindStackTopTerminal(asStack,iStackTop,iStackBottom);
			cTemp=CheckTerminalPriority(asStack[iStackTopTerminal].sPlace,sTempItem.sPlace);
			//判断cTemp为大于小于还是等于或空
			if(cTemp=='<'||cTemp=='=')//若栈顶终结符优先级小于或等于栈外终结符入站
			{
				iStackTop++;//栈顶指针++
				asStack[iStackTop]=sTempItem;//入栈
				if(asStack[iStackTop].sPlace.iId==3&&asStack[iStackTop].sPlace.iSubScript==9) exit();//程序结束
				//初始化stemp为下次做准备
				sTempItem=NULL;
			}
			else if(cTemp=='>')//规约
			{
				//规约代码
				int iTermTail;//存放规约的尾
				int iTermHead;//存放规约的头
				//开始规约
			
				iTermTail=FindStackTopTerminal(asStack,iStackTop,iStackBottom);//找到栈顶部首个的终结符
				iTermHead=FindTermHead(asSubStack,iTermTail,iTermTail-1)+1;//找到有限关系小于等于栈顶终结符的下标
				iTermTail=iStackTop;//重新负值规约尾为栈顶
				//规约
				if(iTermHead==iTermTail)//若规约var这样只有一个符号的
				{
					//查找文发表看能否规约
					int i;
					char cFlag='N';
					for(i=0;i<GRAMMARLEN;i++)//循环扫描整个文法表
					{
						if(aasQuarternaryForm[i][1]==asStack[iTermHead].sPlace)//找到可规约头部
						{
							cFlag='Y';//设置i=文法表长度+1此时i为标志位若循环完毕i=grammar时可以规约
							break;		
						}
					}
					if(cFlag=='Y')//可以规约
					{
						asStack[iTermHead].cType='N';//规约为非终结符
					}
				}
				else//非是一项规约a=b+c；这样的
				{
					//循环查找文发表找到规约地点
					int iTemp;
					iTemp=MyFindGrammarTableHead(asStack,iTermHead,iTermTail);//查找文发表查找可规约文法
					if(iTemp<GRAMMARLEN)//规约成功
					{
						//规约成功
						//调用函数知道产生四元式

						//调整堆栈
						asStack[iTermHead].cType='T';
						asStack[iTermHead].sPlace=aasQuarternaryForm[iTemp][0];

						iStackTop=iTermHead;//调整堆栈
					}
					else
					{
						//规约失败打印错误信息
						printf("规约过程中未在文法标中查到规约串");
					}
				}
			}
			else//优先关系为空错误
			{
				//报错
				printf("规约过程中有限关系出错");
			}
		}
	}

	return 0;
}