#ifndef _HEADERNAME_H
#define _HEADERNAME_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <process.h> 
#include <windows.h>
#endif
#include "high_design.h"
//以下函数的参数一致
//分别是当前链表头节点，当前节点 ，线程一寄存器，线程二寄存器 
 
ptr mov(struct code*,struct code*,int*,int*,int*,int*);//数据传送 
ptr cul(struct code*,struct code*,int*,int*,int*,int*);//算术运算 
ptr lgc(struct code*,struct code*,int*,int*,int*,int*);//逻辑运算 
ptr cmp(struct code*,struct code*,int*,int*,int*,int*);//比较指令 
ptr jmp(struct code*,struct code*,int*,int*,int*,int*);//跳转指令 
ptr scp(struct code*,struct code*,int*,int*,int*,int*);//标准输入输出 
ptr stp(struct code*,struct code*,int*,int*,int*,int*);//标准输入输出 
ptr mtd(struct code*,struct code*,int*,int*,int*,int*);//多线程相关操作 

ptr scp(struct code *link,struct code *now,int *prf1,int *axn1,int *prf2,int *axn2){
	int number;
	int *axn,*prf;//申请整数类型指针，方便根据线程id切换寄存器 
	if(link->thread_id==1){
		axn=axn1;prf=prf1;//使用第一套寄存器 
	}
	else{
		axn=axn2;prf=prf2;//使用第二套寄存器 
	}
	if(now->cmd_ope_code==11){//输入 
		printf("in:\n");
		scanf("%d",&number);
		axn[now->pre_ope_code]=number;
	}
	else{//输出 
		printf("id = %d    out: %d\n",link->thread_id,axn[now->pre_ope_code]);//输出线程id及对应的值 
	}
	prf[1]=now->dec_ope_code;//指令寄存器的值储存 
	prf[0]+=4;// 程序计数器值加4 
	return now->next;//返回下一个节点的地址 
}

ptr mov(struct code *link,struct code *now,int *prf1,int *axn1,int *prf2,int *axn2){
	int dword=0;//设置偏移量 ，方便对内存进行读写 
	int *axn,*prf; 
	if(link->thread_id==1){
		axn=axn1;prf=prf1;
	}
	else{
		axn=axn2;prf=prf2;
	}
	if(now->bac_ope_code==0) axn[now->pre_ope_code]=now->dec_imm_code;//对立即数进行转移 
	else if(now->bac_ope_code>=5){//将数据段内存转移至寄存器 
		dword=(axn[now->bac_ope_code]-16384)/2;//两字节一储存 
		axn[now->pre_ope_code]=data[dword];
	}
	else{//将寄存器内容转移至数据段 
		dword=(axn[now->pre_ope_code]-16384)/2;
		data[dword]=axn[now->bac_ope_code];
	}
	prf[1]=now->dec_ope_code;
	prf[0]+=4;
	return now->next;
}

ptr cul(struct code *link,struct code *now,int *prf1,int *axn1,int *prf2,int *axn2){
	int dword=0;
	int *axn,*prf;
	if(link->thread_id==1){
		axn=axn1;prf=prf1;
	} 
	else{
		axn=axn2;prf=prf2;
	}
	if(now->cmd_ope_code==2){
		if(now->bac_ope_code==0) axn[now->pre_ope_code]=axn[now->pre_ope_code]+now->dec_imm_code;//加法运算 
		else{
			dword=(axn[now->bac_ope_code]-16384)/2;
			axn[now->pre_ope_code]=axn[now->pre_ope_code]+data[dword];
		}
	}
	else if(now->cmd_ope_code==3){
		if(now->bac_ope_code==0) axn[now->pre_ope_code]=axn[now->pre_ope_code]-now->dec_imm_code;//减法运算 
		else{
			dword=(axn[now->bac_ope_code]-16384)/2;
			axn[now->pre_ope_code]=axn[now->pre_ope_code]-data[dword];
		}
	}
	else if(now->cmd_ope_code==4){
		if(now->bac_ope_code==0) axn[now->pre_ope_code]=axn[now->pre_ope_code]*now->dec_imm_code;//乘法运算 
		else{
			dword=(axn[now->bac_ope_code]-16384)/2;
			axn[now->pre_ope_code]=axn[now->pre_ope_code]*data[dword];
		}
	}
	else{
		if(now->bac_ope_code==0) axn[now->pre_ope_code]=axn[now->pre_ope_code]/now->dec_imm_code;//除法运算 
		else{
			dword=(axn[now->bac_ope_code]-16384)/2;
			axn[now->pre_ope_code]=axn[now->pre_ope_code]/data[dword];
		}
	}
	prf[1]=now->dec_ope_code;
	prf[0]+=4;
	return now->next;
}

ptr lgc(struct code *link,struct code *now,int *prf1,int *axn1,int *prf2,int *axn2){
	int dword;
	int *axn,*prf;
	if(link->thread_id==1){
		axn=axn1;prf=prf1;
	} 
	else{
		axn=axn2;prf=prf2;
	}
	if(now->cmd_ope_code==6){
		if(now->bac_ope_code==0) axn[now->pre_ope_code]=axn[now->pre_ope_code]&&now->dec_imm_code;//逻辑与 
		else{
			dword=(axn[now->bac_ope_code]-16384)/2;
			axn[now->pre_ope_code]=axn[now->pre_ope_code]&&data[dword];
		}
	}
	else if(now->cmd_ope_code==7){
		if(now->bac_ope_code==0) axn[now->pre_ope_code]=axn[now->pre_ope_code]||now->dec_imm_code;//逻辑或 
		else{
			dword=(axn[now->bac_ope_code]-16384)/2;
			axn[now->pre_ope_code]=axn[now->pre_ope_code]||data[dword];
		}
	}
	else{
		if(now->bac_ope_code==0) axn[now->pre_ope_code]=!axn[now->pre_ope_code];//逻辑非 
		else{
			dword=(axn[now->bac_ope_code]-16384)/2;
			data[dword]=!data[dword];
		}
	}
	prf[1]=now->dec_ope_code;
	prf[0]+=4;
	return now->next;
}

ptr cmp(struct code *link,struct code *now,int *prf1,int *axn1,int *prf2,int *axn2){
	int dword;//设置偏移量 
	int *axn,*prf;
	if(link->thread_id==1){
		axn=axn1;prf=prf1;
	} 
	else{
		axn=axn2;prf=prf2;
	}
	if(now->bac_ope_code==0){//与立即数比较 
		if(axn[now->pre_ope_code]==now->dec_imm_code) prf[2]=0;//标志寄存器赋值 
		else if(axn[now->pre_ope_code]>now->dec_imm_code) prf[2]=1;
		else prf[2]=-1;
	}
	else{//与寄存器指向的内存段数据比较 
		dword=(axn[now->bac_ope_code]-16384)/2;
		if(axn[now->pre_ope_code]==data[dword]) prf[2]=0;
		else if(axn[now->pre_ope_code]>data[dword]) prf[2]=1;
		else prf[2]=-1;
	}
	prf[1]=now->dec_ope_code;
	prf[0]+=4;
	return now->next;
}

ptr jmp(struct code *link,struct code *now,int *prf1,int *axn1,int *prf2,int *axn2){
	ptr jump_node=link,result;//跳转的目标节点 
	int count=0,state=0;//计数器和状态 
	int jump_number;//跳转节点个数 
	int *axn,*prf;
	if(link->thread_id==1){
		axn=axn1;prf=prf1;
	} 
	else{
		axn=axn2;prf=prf2;
	}
	if(now->dec_imm_code<0){//向负字节跳转 
		jump_number=prf[0]/4+now->dec_imm_code/4;
		for(count=0;count<=jump_number;count++){
			jump_node=jump_node->next;
		}
	}
	else{// 向正字节跳转 
		jump_number=now->dec_imm_code/4;
		jump_node=now;
		for(count=0;count<jump_number;count++){
			jump_node=jump_node->next;
		}
	}
	if(now->bac_ope_code==0) state=1;//强制跳转
	else if(now->bac_ope_code==1){//条件跳转1 
		if(prf[2]==0) state=1;
		else state=0;
	}
	else if(now->bac_ope_code==2){//条件跳转2 
		if(prf[2]==1) state=1;
		else state=0;
	}
	else{//条件跳转3 
		if(prf[2]==-1) state=1;
		else state=0;
	}
	if(state==1){//跳转成功 
		result=jump_node;
		prf[0]=prf[0]+now->dec_imm_code;
	}
	else{//未进行跳转 
		prf[0]+=4;
		result=now->next;
	}
	prf[1]=now->dec_ope_code;
	return result;
}

ptr mtd(struct code *link,struct code *now,int *prf1,int *axn1,int *prf2,int *axn2){
	int *axn,*prf;
	if(link->thread_id==1){
		axn=axn1;prf=prf1;
	}
	else{
		axn=axn2;prf=prf2;
	}
	if(now->cmd_ope_code==13){//锁住内存 
		WaitForSingleObject(hMutex,INFINITE);
	}
	else if(now->cmd_ope_code==14){//释放内存 
		ReleaseMutex (hMutex); 
	}
	else{
		Sleep(now->dec_imm_code);//休眠立即数 
	}
	prf[1]=now->dec_ope_code;
	prf[0]+=4;
	return now->next;
}

ptr stp(struct code *link,struct code *now,int *prf1,int *axn1,int *prf2,int *axn2){//执行停机指令 
	int *axn,*prf;
	if(link->thread_id==1){
		axn=axn1;prf=prf1;
	}
	else{
		axn=axn2;prf=prf2;
	}
	prf[1]=now->dec_ope_code;
	prf[0]+=4;
	return now->next;
}
