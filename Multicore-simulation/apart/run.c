#ifndef _HEADERNAME_H
#define _HEADERNAME_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <process.h> 
#include <windows.h>
#endif
#include "high_design.h" 
 
void sta(struct code*,int*,int*,int*,int*);//执行完一条指令后输出状态 
unsigned __stdcall run_code(void*);//运行程序 
void all_sta(struct code*,struct code*);// 输出内存（包含代码段和数据段 

void sta(struct code *link,int *prf1,int *axn1,int *prf2,int *axn2){//输出寄存器状态 
	int *axn,*prf;
	if(link->thread_id==1){
		axn=axn1;prf=prf1;
	}
	else{
		axn=axn2;prf=prf2;
	}
	if(link->thread_id==1)	printf("id = %d\nip = %d\nflag = %d\nir = %d\n",link->thread_id,prf[0],prf[2],prf[1]);
	else printf("id = %d\nip = %d\nflag = %d\nir = %d\n",link->thread_id,prf[0]+256,prf[2],prf[1]);
	printf("ax1 = %d ax2 = %d ax3 = %d ax4 = %d\n",axn[1],axn[2],axn[3],axn[4]); 
	printf("ax5 = %d ax6 = %d ax7 = %d ax8 = %d\n",axn[5],axn[6],axn[7],axn[8]);
}
unsigned __stdcall run_code(void *pArguments){ 
	ptr link=(ptr)pArguments;
	hMutex=CreateMutex (NULL, FALSE, "tickets");//创建互斥类型 
	hMutex_state=CreateMutex (NULL, FALSE, "states");//创建互斥类型 
	int prf1[4]={0};//0-2代表程序寄存器，指令寄存器，标志寄存器 （线程一使用的寄存器 
	int axn1[9]={0}; //0位 置0，1-4代表数据寄存器，5-8代表地址寄存器 (线程一使用的寄存器 
	int prf2[4]={0};//线程二使用的寄存器
	int axn2[9]={0};//线程二使用的寄存器 
	ptr current=link->next;
	while(current!=NULL){
		if(current->cmd_ope_code==1) current=mov(link,current,prf1,axn1,prf2,axn2);//转移数据 
		else if(current->cmd_ope_code<=5&&current->cmd_ope_code>=2) current=cul(link,current,prf1,axn1,prf2,axn2);//数学运算 
		else if(current->cmd_ope_code<=8&&current->cmd_ope_code>=6) current=lgc(link,current,prf1,axn1,prf2,axn2);//逻辑运算 
		else if(current->cmd_ope_code==9) current=cmp(link,current,prf1,axn1,prf2,axn2);//比较操作 
		else if(current->cmd_ope_code==10) current=jmp(link,current,prf1,axn1,prf2,axn2);//跳转操作 
		else if(current->cmd_ope_code==11||current->cmd_ope_code==12) current=scp(link,current,prf1,axn1,prf2,axn2);//标准输入输出 
		else if(current->cmd_ope_code>=13&&current->cmd_ope_code<=15) current=mtd(link,current,prf1,axn1,prf2,axn2);//多线程操作 
		else current=stp(link,current,prf1,axn1,prf2,axn2);//停机 
		WaitForSingleObject(hMutex_state,INFINITE);//等待线程输出完毕 
		sta(link,prf1,axn1,prf2,axn2);//输出状态
		ReleaseMutex(hMutex_state); //释放互斥对象 
	}
	_endthreadex(0);
}

void all_sta(struct code *link1,struct code *link2){
	ptr temp=link1->next;
	int row=0,col=0,i=0;
	printf("\n"); 
	printf("codeSegment :\n");//输出代码段内存 
	for(row=0;row<16;row++){
		if(row==8) temp=link2->next;//输出一半之后输出线程二的代码段 
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
