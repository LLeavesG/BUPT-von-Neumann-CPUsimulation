#ifndef _HEADERNAME_H
#define _HEADERNAME_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <process.h> 
#include <windows.h>
#endif
#include "high_design.h"

int trans(char*,int,int,int);//二进制转10进制，状态1为补码转化换 
void load_code(struct code*,FILE*);//读入指令函数 

void load_code(struct code *link,FILE *fp){//载入指令 
	int i=0;
	char temp[36],ope[36],c;
	ptr apply,now=link;
	c=fgetc(fp);
	while(c!=EOF){
		while(c!=EOF&&c!='\n'){//读入一行二进制数储存在temp字符串中 
			temp[i]=c;
			c=fgetc(fp);
			i++;
		}
		i=0;
		temp[32]='\0'; 
		apply=(ptr)malloc(sizeof(struct code));//申请一个新节点 
		strcmp(apply->bin_code,temp);
		apply->dec_code=trans(temp,1,32,0);//此处开始将二进制转化为十进制 
		apply->dec_imm_code=trans(temp,17,32,1);//标志为1则补码转化 
		apply->dec_ope_code=trans(temp,1,16,0);//标志为0则原码转化 
		apply->cmd_ope_code=trans(temp,1,8,0);
		apply->pre_ope_code=trans(temp,9,12,0);
		apply->bac_ope_code=trans(temp,13,16,0); 
		now->next=apply;
		apply->next=NULL;
		now=apply;
		c=fgetc(fp);
	}
}


int trans(char *s,int start,int end,int tag){//二进制转10进制 
	int i=0,a=1;
	int sum=0;
	int num[36];
	if(tag==1){//补码转化 
		if(s[start-1]=='1'){//首位为1，进行对应转化 
			for(i=start-1;i<end;i++){
				if(s[i]-'0'==1) num[i]=0;
				else num[i]=1;
			}
			for(i=31;i>=16;i--){
				sum=sum+num[i]*a;
				a=a*2;
			}
			sum=-(sum+1);//取负值 
		}
		else{
			for(i=start-1;i<end;i++){
				num[i]=s[i]-'0';
			}
			for(i=31;i>=16;i--){
				sum=sum+num[i]*a;
				a=a*2;
			}
		}
	}
	else{//原码转化 
		for(i=end-1;i>=start-1;i--){
			sum=sum+(s[i]-'0')*a;
			a=a*2;
		}
	}
	return sum;//返回结果 
}
