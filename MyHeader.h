#pragma once 

#define SBUFSIZE 256//定义扫描缓冲区的大小
#define ARROPELEN 9//定义运算符数组的长度
#define ARRKEYLEN 2//定义关键字数组的长度


struct sBinaryRelation//返回值若为关键字或变量
{
	int iId;//区分是什么
	int iSubScript;//存放操作符和关键字的偏移量，若是常量返回为常量的整型值
	char acTempValName[SBUFSIZE];//存放变量的值
};

char * apcKeyWords[]={"","int"};
char apcOperator[]={' ','=','+','-','*','/','(',')',';','#'};
char * apcWordList[]={"key","var","const","operate","nonTerminalSymbol","special"};