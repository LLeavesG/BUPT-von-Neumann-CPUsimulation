#ifndef _HEADERNAME_H
#define _HEADERNAME_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <process.h> 
#include <windows.h>
#endif
#include "high_design.h"

int trans(char*,int,int,int);//������ת10���ƣ�״̬1Ϊ����ת���� 
void load_code(struct code*,FILE*);//����ָ��� 

void load_code(struct code *link,FILE *fp){//����ָ�� 
	int i=0;
	char temp[36],ope[36],c;
	ptr apply,now=link;
	c=fgetc(fp);
	while(c!=EOF){
		while(c!=EOF&&c!='\n'){//����һ�ж�������������temp�ַ����� 
			temp[i]=c;
			c=fgetc(fp);
			i++;
		}
		i=0;
		temp[32]='\0'; 
		apply=(ptr)malloc(sizeof(struct code));//����һ���½ڵ� 
		strcmp(apply->bin_code,temp);
		apply->dec_code=trans(temp,1,32,0);//�˴���ʼ��������ת��Ϊʮ���� 
		apply->dec_imm_code=trans(temp,17,32,1);//��־Ϊ1����ת�� 
		apply->dec_ope_code=trans(temp,1,16,0);//��־Ϊ0��ԭ��ת�� 
		apply->cmd_ope_code=trans(temp,1,8,0);
		apply->pre_ope_code=trans(temp,9,12,0);
		apply->bac_ope_code=trans(temp,13,16,0); 
		now->next=apply;
		apply->next=NULL;
		now=apply;
		c=fgetc(fp);
	}
}


int trans(char *s,int start,int end,int tag){//������ת10���� 
	int i=0,a=1;
	int sum=0;
	int num[36];
	if(tag==1){//����ת�� 
		if(s[start-1]=='1'){//��λΪ1�����ж�Ӧת�� 
			for(i=start-1;i<end;i++){
				if(s[i]-'0'==1) num[i]=0;
				else num[i]=1;
			}
			for(i=31;i>=16;i--){
				sum=sum+num[i]*a;
				a=a*2;
			}
			sum=-(sum+1);//ȡ��ֵ 
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
	else{//ԭ��ת�� 
		for(i=end-1;i>=start-1;i--){
			sum=sum+(s[i]-'0')*a;
			a=a*2;
		}
	}
	return sum;//���ؽ�� 
}
