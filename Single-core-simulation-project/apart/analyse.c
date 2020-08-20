#ifndef _HEADERNAME_H
#define _HEADERNAME_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#endif
#include "struct.h"
//以下函数的参数一致
//分别是头结点地址 当前节点地址 程序指令标志寄存器 数据地址寄存器 数据段内存
 
ptr mov(struct code*,struct code*,int*,int*,int*);//数据传送 
ptr cul(struct code*,struct code*,int*,int*,int*);//算术运算 
ptr lgc(struct code*,struct code*,int*,int*,int*);//逻辑运算 
ptr cmp(struct code*,struct code*,int*,int*,int*);//比较指令 
ptr jmp(struct code*,struct code*,int*,int*,int*);//跳转指令 
ptr scp(struct code*,struct code*,int*,int*,int*);//标准输入输出 
ptr stp(struct code*,struct code*,int*,int*,int*);//标准输入输出 

ptr scp(struct code *link,struct code *now,int *prf,int *axn,int *data){//标准输入输出处理 
	int number;
	if(now->cmd_ope_code==11){//输入 
		printf("in:\n");
		scanf("%d",&number);
		axn[now->pre_ope_code]=number;
	}
	else{//输出 
		printf("out: %d\n",axn[now->pre_ope_code]);
	}
	prf[1]=now->dec_ope_code;
	prf[0]+=4;
	return now->next;
}

ptr mov(struct code *link,struct code *now,int *prf,int *axn,int *data){//数据转移操作 
	int dword=0;//设置偏移量 
	if(now->bac_ope_code==0) axn[now->pre_ope_code]=now->dec_imm_code;
	else if(now->bac_ope_code>=5){
		dword=(axn[now->bac_ope_code]-16384)/2;
		axn[now->pre_ope_code]=data[dword];
	}
	else{
		dword=(axn[now->pre_ope_code]-16384)/2;
		data[dword]=axn[now->bac_ope_code];
	}
	prf[1]=now->dec_ope_code;
	prf[0]+=4;
	return now->next;
}

ptr cul(struct code *link,struct code *now,int *prf,int *axn,int *data){//四则运算操作 
	int dword=0;
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

ptr lgc(struct code *link,struct code *now,int *prf,int *axn,int *data){//逻辑运算操作 
	int dword;
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

ptr cmp(struct code *link,struct code *now,int *prf,int *axn,int *data){//比较操作 
	int dword;
	if(now->bac_ope_code==0){
		if(axn[now->pre_ope_code]==now->dec_imm_code) prf[2]=0;
		else if(axn[now->pre_ope_code]>now->dec_imm_code) prf[2]=1;
		else prf[2]=-1;
	}
	else{
		dword=(axn[now->bac_ope_code]-16384)/2;
		if(axn[now->pre_ope_code]==data[dword]) prf[2]=0;
		else if(axn[now->pre_ope_code]>data[dword]) prf[2]=1;
		else prf[2]=-1;
	}
	prf[1]=now->dec_ope_code;
	prf[0]+=4;
	return now->next;
}

ptr jmp(struct code *link,struct code *now,int *prf,int *axn,int *data){//跳转操作 
	ptr jump_node=link,result;//跳转的目标节点 
	int count=0,state=0;//计数器和状态 
	int jump_number;//跳转节点个数 
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
	if(state==1){
		result=jump_node;
		prf[0]=prf[0]+now->dec_imm_code;
	}
	else{
		prf[0]+=4;
		result=now->next;
	}
	prf[1]=now->dec_ope_code;
	return result;
}

ptr stp(struct code *link,struct code *now,int *prf,int *axn,int *data){//停机操作 
	prf[1]=now->dec_ope_code;
	prf[0]+=4;
	return now->next;
}

