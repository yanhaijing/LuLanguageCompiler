/*Lu语言词法分析器 颜海镜 11.12.30
代码已通过编译，尚未优化
没次调用正确返回一个符合词法规则的单词
源文件保存在.lu文件
支持形如变量定义（仅支持int类型），基本加减乘除法，不支持,支持单行注视支持()
可扩展：支持N个关键字，单个标识符最大支持256个字符，支持运算符扩展，支持函数名扩展
int a=b+c;
int a=100;
//颜海镜
*/

#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>
#include"MyHeader.h"

static char acScanBufL[SBUFSIZE]={0};//扫描缓冲区
static char acScanBufR[SBUFSIZE]={0};
static long lnOffset=0;//偏移量，记录当前文件指针距离开头的距离

char CheckChar(FILE * pfSubSourceFile,char ** ppcSubCurrent,char cSubTemp,char fcSubBlank)//检查字符功能，具有合并空格，去除回车
{
	if(cSubTemp==EOF) 
	{
		return 0x20;
	}//判断是否到文件末尾，结束返回空格

	if(cSubTemp!=10&&isspace(cSubTemp))//判断是否为空格,合并多个空格为一个
	{
		if(fcSubBlank=='N') 
		{
			**ppcSubCurrent=0x20;
			(*ppcSubCurrent)++;
			fcSubBlank='Y';//修改空格标志位
		}
		cSubTemp=fgetc(pfSubSourceFile);//空格的话再读入一个字符
		cSubTemp=CheckChar(pfSubSourceFile,ppcSubCurrent,cSubTemp,fcSubBlank);//递归检查
	}

	if(cSubTemp=='/')//判断注视
	{
		char cTemp;//临时变量，用于检查下一个是否为'/'

		cTemp=fgetc(pfSubSourceFile);//再读入一个字符

		if(cTemp=='/') //若为注视一直读入知道换行符，否则退回刚才读入的字符
		{
			while(fgetc(pfSubSourceFile)!='\n');
			//遇到注视，在注视结尾返回空格
			cSubTemp='\n';
		}
		else ungetc(cTemp,pfSubSourceFile);//退回刚才读入的字符

	}

	return cSubTemp;//返回字符
}

int PreProcess(char *pcSubName)//预处理子程序，完成功能每次向ScanBuffer中装入固定字长的源程序代码
{
	static char fcFlag='L';
	int i;

	//将源程序中读入剔除空格注视等放到buffer
	char * pcCurrent=0;//只是当前要赋值的字节
	char ** ppcCurrent=&pcCurrent;//指向指针的指针
	char * pcStart;//指向数组的开始，计算偏移量用
	char * pcTemp;//临时变量，初始化用
	FILE * pfSourceFile;//指向要打开的源程序文件

	//初始化pcCurrent确认当前要装入的缓冲区
	if(fcFlag=='L') 
	{
		pcCurrent=acScanBufL;
		pcStart=acScanBufL;
	}
	else
	{
		pcCurrent=acScanBufR;
		pcStart=acScanBufR;
	}

	//初始化当前缓冲区为空字符
	pcTemp=pcCurrent;
	for(i=0;i<SBUFSIZE;i++)
	{
		*pcTemp=0;
		pcTemp++;
	}

	//打开文件
	pfSourceFile=fopen("test.txt","r");
	if(pfSourceFile==NULL) 
	{
		printf("The file %s was not opened\n",pcSubName);//判断文件打开是否成功
		exit(0);//装入失败退出
	}
	else//打开成功读入
	{
		if(fseek(pfSourceFile,lnOffset,SEEK_SET))//移动文件指针到应该的位置
		{
			perror("Fseek failed");
			exit(1);//移动光标失败退出
		}

		while((pcCurrent-pcStart)!=SBUFSIZE)//循环读入指定长度字符
		{
			char cTemp;//临时变量
			cTemp=fgetc(pfSourceFile);//读入一个字符
			cTemp=CheckChar(pfSourceFile,ppcCurrent,cTemp,'N');//获取一个合法的字符

			if(cTemp==0x20)
			{
				*pcCurrent=cTemp;
				pcCurrent++;
				*pcCurrent='#';//程序结束
				break;//判断是否到文件末尾
			}

			*pcCurrent=cTemp;//若刚才输入的不为空格也没结束则输入到缓冲区
			pcCurrent++;
		
		}	

		//修改偏移量为当前偏移量，为下次读入用
		lnOffset=ftell(pfSourceFile);
		
		//关闭文件
		fclose(pfSourceFile);

		//修改fcFlag为下次再次装入，更改缓冲区
		if(fcFlag=='L') fcFlag='R';
		else fcFlag='L';
	}

	return 3;
}

int CheckOperate(char cCheck)//检查是否运算符，返回在运算符数组中的偏移量
{
	int i;
	for(i=0;i<ARROPELEN;i++)
	{
		if(cCheck==apcOperator[i]) return i;//返回在运算符数组中的偏移量		
	}
	return 0;//不是运算符
}

int CheckNewLine(char cCheck,int *piSubLine)//检测字符是否为回车
{
	if(cCheck=='\n') 
	{
		(*piSubLine)++;
		return 1;
	}
	return 0;
}

int CheckKeyWords(char acCheckWords[])//检查单词是否为关键字若是返回在关键字列表中的下标，不是返回0
{
	int i;
	for(i=0;i<ARRKEYLEN;i++)
	{
		if(!strcmp(acCheckWords, apcKeyWords[i])) return i;
	}
	return 0;
}

int Error(int iSubLine)//错误处理函数，buffer质控
{
	//打印错误信息
	printf("第%d行",iSubLine);

	lnOffset=0;//初始化文件偏移量
	return 0;
}

int CheckEndBuffer(char **ppcCheckPoint,char *pcSubName)//检测是否越界,并自动切换缓冲区，bufferL越界返回1，bufferR越界返回2，没有越界返回0
{
	static char fcBuffer ='L';//初始化第一次扫描bufferl
	int iTempOffset;//临时偏移量变量
	fcBuffer=='L'?(iTempOffset=*ppcCheckPoint-acScanBufL):(iTempOffset=*ppcCheckPoint-acScanBufR);//计算偏移量
	if(iTempOffset>=SBUFSIZE-1)//越界
	{
		if(fcBuffer=='L')
		{
			PreProcess(pcSubName);//装入Buffer
			*ppcCheckPoint=acScanBufR;//修改当前的指示器到下一个缓冲区的开始
			fcBuffer='R';//修改buffer标志，当前指针在bufferR中
			return 1;//bufferL越界返回1
		}
		else
		{
			PreProcess(pcSubName);//装入Buffer
			*ppcCheckPoint=acScanBufL;//修改当前的指示器到下一个缓冲区的开始
			fcBuffer='L';//修改buffer标志，当前指针在bufferL中
			return 2;//bufferR越界返回2
		}
	}
	return 0;//没有越界返回0
}

struct sBinaryRelation LexicalAnalyzer(char *pcSubName)//词法分析器
{
	static char fcFirst='Y';
	//初始化扫描指示器为第一个元素
	static char *fpcStart;
	char **fppcStart=&fpcStart;//取fpcstart的地址
	static char *fpcSerching;
	char **fppcfpcSerching=&fpcSerching;//取fpcSerching的地址
	static int iLine=1;//记录当前的行数
	int *piLine=&iLine;//指向行数的指针
	struct sBinaryRelation sTempResult;//存放返回结果

	if(fcFirst=='Y')//第一次调用装入bufferl
	{
		PreProcess(pcSubName);
		fcFirst='N';
		fpcStart=acScanBufL;//将start和serch都指向第一个地址
	}

	while(*fpcStart==0x20) 
	{
		if(!CheckEndBuffer(fppcStart,pcSubName)) fpcStart++;//检查是否越界,若没越界将start指针指向一个不为空格的字符
	}
	fpcSerching=fpcStart;//serching指向start
	//fpcSerching++;//serching指向start的下一个位置

	while(*fpcSerching!=0x00)
	{
		if(CheckNewLine(*fpcStart,piLine))//检查回车，记录行数加一
		{
			//待扩建是否将\n也算作标识符
			fpcStart++;
			fpcSerching=fpcStart;//serching指向start
		}
		else if(isalpha(*fpcStart))//第一个字符是字母，为关键字
		{
			char cTempResult[SBUFSIZE]={0};
			char *pcCurrent=cTempResult;

			//检查是否为变量或关键字
			while(isalnum(*fpcSerching))//下一个是字符或数字
			{
				//if(isspace(*fpcSerching)) break;//若为空格结束

				*(pcCurrent++)=*fpcSerching;//将字符保存到临时数组，并且指针后移

				if(!CheckEndBuffer(fppcfpcSerching,pcSubName)) fpcSerching++; //检查是否越界,若没越界将serching++
			}//循环完成时fpceSerching指向当前标识符的下一个位置	
			//将start和serching重合
			fpcStart=fpcSerching;

			//返回当前的标识符放到结构体中
			int KeyWordScript;//关键字的下标
			if((KeyWordScript=CheckKeyWords(cTempResult))!=0)//检查是否为关键字还是普通变量
			{
				sTempResult.iId=0;//标志符置为0，关键字的下标
				sTempResult.iSubScript=KeyWordScript;
				sTempResult.acTempValName[0]='\0';//赋值
			}
			else//普通变量
			{
				sTempResult.iId=1;//单词为变量
				int i=-1;
				while(cTempResult[++i]!='\0')//将变量的值赋给变量名字数组
				{
					sTempResult.acTempValName[i]=cTempResult[i];
				}
				sTempResult.acTempValName[i]=cTempResult[i];//赋值个'\0'
				sTempResult.iSubScript=0;//赋值
			}
			//返回结构体
			
			return sTempResult;
		}
		else if(isdigit(*fpcStart))//如果是数字常量
		{
			int iTemp=0;
			while(*fpcSerching!=0x20)//下面字符都要是数字
			{
				if((!isdigit(*fpcSerching))&&(isalpha(*fpcSerching)))//若后面是字幕或者不是操作符
				{
					printf("词法分析：");
					Error(iLine);//不是数字报错
					printf(" %d %c非法字符\n",iTemp,*fpcSerching);
				}
				iTemp=iTemp*10+int(*fpcSerching-0x30);//将数字字符转化为整形
				if(!CheckEndBuffer(fppcfpcSerching,pcSubName)) fpcSerching++; //检查是否越界,若没越界将serching++
				//判断是否是；若下一个字符是分号或者操作符就结束
				if(*fpcSerching==';') break;
				if(CheckOperate(*fpcSerching)) break;
			}
			//循环完成时fpceSerching指向当前标识符的下一个位置
			fpcStart=fpcSerching;//将start和serching重合

			//返回当前的标识符放到结构体中
			sTempResult.iId=2;
			sTempResult.iSubScript=iTemp;
			sTempResult.acTempValName[0]='\0';//赋值
			return sTempResult;

		}
		else if(CheckOperate(*fpcStart)) //判断运算符
		{
			int iTempOperateOffset;//记录标识符下标
			iTempOperateOffset=CheckOperate(*fpcStart);
			//不需要因为serching本来就在start的下一个fpceSerching++;
			if(!CheckEndBuffer(fppcStart,pcSubName)) fpcStart=++fpcSerching;//检查serching是否越界,若没越界将start和serching重合
			
			//返回当前的标识符放到结构体中
			sTempResult.iId=3;
			sTempResult.iSubScript=iTempOperateOffset;
			sTempResult.acTempValName[0]='\0';//赋值
			return sTempResult;

		}
		else
		{
			printf("词法分析：");
			Error(iLine);//不是数字报错
			printf(" %c非法字符\n",*fpcSerching);
			exit(1);//退出程序
		}
				 
	}
	if(*fpcSerching!=0) printf("词法分析：未知错误\n");//若不为结束符，显示未知错误
	exit(1);//失败返回空
}

int DebugLexicalAnalyzer(char *pcSubName)
{
	struct sBinaryRelation sDebugResult;//存放返回结果
	int i;
	for(i=0;i<500;i++)
	{
		sDebugResult=LexicalAnalyzer(pcSubName);
		printf("%d  %d %d %s\n",i,sDebugResult.iId,sDebugResult.iSubScript,sDebugResult.acTempValName);
	}
	return 0;
}