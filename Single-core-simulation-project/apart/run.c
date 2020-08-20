#ifndef _HEADERNAME_H
#define _HEADERNAME_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#endif
#include "struct.h" 

void state_print(int*,int*);
void run_code(struct code*,int*);

void state_print(int *prf,int *axn){//输出状态 
	printf("ip = %d\n",prf[0]);
	printf("flag = %d\n",prf[2]);
	printf("ir = %d\n",prf[1]);
	printf("ax1 = %d ax2 = %d ax3 = %d ax4 = %d\n",axn[1],axn[2],axn[3],axn[4]);
	printf("ax5 = %d ax6 = %d ax7 = %d ax8 = %d\n",axn[5],axn[6],axn[7],axn[8]);
}

void run_code(struct code *link,int *data){//运行
	int row=0,col=0,i=0;
	int prf[4]={0};//0-2代表程序寄存器，指令寄存器，标志寄存器 
	int axn[9]={0}; //0位 置0，1-4代表数据寄存器，5-8代表地址寄存器 
	ptr current=link->next;
	ptr temp=link->next;
	while(current!=NULL){
		if(current->cmd_ope_code==1) current=mov(link,current,prf,axn,data);//转移数据 
		else if(current->cmd_ope_code<=5&&current->cmd_ope_code>=2) current=cul(link,current,prf,axn,data);//数学运算 
		else if(current->cmd_ope_code<=8&&current->cmd_ope_code>=6) current=lgc(link,current,prf,axn,data);//逻辑运算 
		else if(current->cmd_ope_code==9) current=cmp(link,current,prf,axn,data);//比较操作 
		else if(current->cmd_ope_code==10) current=jmp(link,current,prf,axn,data);//跳转操作 
		else if(current->cmd_ope_code==11||current->cmd_ope_code==12) current=scp(link,current,prf,axn,data);//标准输入输出 
		else current=stp(link,current,prf,axn,data);//停机 
		state_print(prf,axn);//输出状态 
	}
	printf("\n"); 
	printf("codeSegment :\n");//输出代码段内存 
	for(row=0;row<16;row++){
		for(col=0;col<8;col++){
			if(temp!=NULL){
				printf("%d ",temp->dec_code);
				temp=temp->next;
			}
			else printf("0 ");
		}
		printf("\n");
	}
	printf("\n");
	printf("dataSegment :\n");//输出数据段内存 
	for(i=0;i<256;i++){
		printf("%d ",data[i]);
		if((i+1)%16==0) printf("\n");
	}
}
