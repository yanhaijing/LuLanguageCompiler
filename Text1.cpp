#include<iostream.h>
void main()
{
	struct stu{
		int a;
		char ch[5];
	}; 
	struct stu   s[2][2]={{{1,"ab"},NULL},{{2,"cd"},{2,"cd"}}};
	struct stu  x[2][2]={{{0}}};
	const a=3;
	int l[a];
	if(s[1][1].a==NULL)
		cout<<"hello";
}