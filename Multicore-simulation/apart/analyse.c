#ifndef _HEADERNAME_H
#define _HEADERNAME_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <process.h> 
#include <windows.h>
#endif
#include "high_design.h"
//���º����Ĳ���һ��
//�ֱ��ǵ�ǰ����ͷ�ڵ㣬��ǰ�ڵ� ���߳�һ�Ĵ������̶߳��Ĵ��� 
 
ptr mov(struct code*,struct code*,int*,int*,int*,int*);//���ݴ��� 
ptr cul(struct code*,struct code*,int*,int*,int*,int*);//�������� 
ptr lgc(struct code*,struct code*,int*,int*,int*,int*);//�߼����� 
ptr cmp(struct code*,struct code*,int*,int*,int*,int*);//�Ƚ�ָ�� 
ptr jmp(struct code*,struct code*,int*,int*,int*,int*);//��תָ�� 
ptr scp(struct code*,struct code*,int*,int*,int*,int*);//��׼������� 
ptr stp(struct code*,struct code*,int*,int*,int*,int*);//��׼������� 
ptr mtd(struct code*,struct code*,int*,int*,int*,int*);//���߳���ز��� 

ptr scp(struct code *link,struct code *now,int *prf1,int *axn1,int *prf2,int *axn2){
	int number;
	int *axn,*prf;//������������ָ�룬��������߳�id�л��Ĵ��� 
	if(link->thread_id==1){
		axn=axn1;prf=prf1;//ʹ�õ�һ�׼Ĵ��� 
	}
	else{
		axn=axn2;prf=prf2;//ʹ�õڶ��׼Ĵ��� 
	}
	if(now->cmd_ope_code==11){//���� 
		printf("in:\n");
		scanf("%d",&number);
		axn[now->pre_ope_code]=number;
	}
	else{//��� 
		printf("id = %d    out: %d\n",link->thread_id,axn[now->pre_ope_code]);//����߳�id����Ӧ��ֵ 
	}
	prf[1]=now->dec_ope_code;//ָ��Ĵ�����ֵ���� 
	prf[0]+=4;// ���������ֵ��4 
	return now->next;//������һ���ڵ�ĵ�ַ 
}

ptr mov(struct code *link,struct code *now,int *prf1,int *axn1,int *prf2,int *axn2){
	int dword=0;//����ƫ���� ��������ڴ���ж�д 
	int *axn,*prf; 
	if(link->thread_id==1){
		axn=axn1;prf=prf1;
	}
	else{
		axn=axn2;prf=prf2;
	}
	if(now->bac_ope_code==0) axn[now->pre_ope_code]=now->dec_imm_code;//������������ת�� 
	else if(now->bac_ope_code>=5){//�����ݶ��ڴ�ת�����Ĵ��� 
		dword=(axn[now->bac_ope_code]-16384)/2;//���ֽ�һ���� 
		axn[now->pre_ope_code]=data[dword];
	}
	else{//���Ĵ�������ת�������ݶ� 
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
		if(now->bac_ope_code==0) axn[now->pre_ope_code]=axn[now->pre_ope_code]+now->dec_imm_code;//�ӷ����� 
		else{
			dword=(axn[now->bac_ope_code]-16384)/2;
			axn[now->pre_ope_code]=axn[now->pre_ope_code]+data[dword];
		}
	}
	else if(now->cmd_ope_code==3){
		if(now->bac_ope_code==0) axn[now->pre_ope_code]=axn[now->pre_ope_code]-now->dec_imm_code;//�������� 
		else{
			dword=(axn[now->bac_ope_code]-16384)/2;
			axn[now->pre_ope_code]=axn[now->pre_ope_code]-data[dword];
		}
	}
	else if(now->cmd_ope_code==4){
		if(now->bac_ope_code==0) axn[now->pre_ope_code]=axn[now->pre_ope_code]*now->dec_imm_code;//�˷����� 
		else{
			dword=(axn[now->bac_ope_code]-16384)/2;
			axn[now->pre_ope_code]=axn[now->pre_ope_code]*data[dword];
		}
	}
	else{
		if(now->bac_ope_code==0) axn[now->pre_ope_code]=axn[now->pre_ope_code]/now->dec_imm_code;//�������� 
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
		if(now->bac_ope_code==0) axn[now->pre_ope_code]=axn[now->pre_ope_code]&&now->dec_imm_code;//�߼��� 
		else{
			dword=(axn[now->bac_ope_code]-16384)/2;
			axn[now->pre_ope_code]=axn[now->pre_ope_code]&&data[dword];
		}
	}
	else if(now->cmd_ope_code==7){
		if(now->bac_ope_code==0) axn[now->pre_ope_code]=axn[now->pre_ope_code]||now->dec_imm_code;//�߼��� 
		else{
			dword=(axn[now->bac_ope_code]-16384)/2;
			axn[now->pre_ope_code]=axn[now->pre_ope_code]||data[dword];
		}
	}
	else{
		if(now->bac_ope_code==0) axn[now->pre_ope_code]=!axn[now->pre_ope_code];//�߼��� 
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
	int dword;//����ƫ���� 
	int *axn,*prf;
	if(link->thread_id==1){
		axn=axn1;prf=prf1;
	} 
	else{
		axn=axn2;prf=prf2;
	}
	if(now->bac_ope_code==0){//���������Ƚ� 
		if(axn[now->pre_ope_code]==now->dec_imm_code) prf[2]=0;//��־�Ĵ�����ֵ 
		else if(axn[now->pre_ope_code]>now->dec_imm_code) prf[2]=1;
		else prf[2]=-1;
	}
	else{//��Ĵ���ָ����ڴ�����ݱȽ� 
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
	ptr jump_node=link,result;//��ת��Ŀ��ڵ� 
	int count=0,state=0;//��������״̬ 
	int jump_number;//��ת�ڵ���� 
	int *axn,*prf;
	if(link->thread_id==1){
		axn=axn1;prf=prf1;
	} 
	else{
		axn=axn2;prf=prf2;
	}
	if(now->dec_imm_code<0){//���ֽ���ת 
		jump_number=prf[0]/4+now->dec_imm_code/4;
		for(count=0;count<=jump_number;count++){
			jump_node=jump_node->next;
		}
	}
	else{// �����ֽ���ת 
		jump_number=now->dec_imm_code/4;
		jump_node=now;
		for(count=0;count<jump_number;count++){
			jump_node=jump_node->next;
		}
	}
	if(now->bac_ope_code==0) state=1;//ǿ����ת
	else if(now->bac_ope_code==1){//������ת1 
		if(prf[2]==0) state=1;
		else state=0;
	}
	else if(now->bac_ope_code==2){//������ת2 
		if(prf[2]==1) state=1;
		else state=0;
	}
	else{//������ת3 
		if(prf[2]==-1) state=1;
		else state=0;
	}
	if(state==1){//��ת�ɹ� 
		result=jump_node;
		prf[0]=prf[0]+now->dec_imm_code;
	}
	else{//δ������ת 
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
	if(now->cmd_ope_code==13){//��ס�ڴ� 
		WaitForSingleObject(hMutex,INFINITE);
	}
	else if(now->cmd_ope_code==14){//�ͷ��ڴ� 
		ReleaseMutex (hMutex); 
	}
	else{
		Sleep(now->dec_imm_code);//���������� 
	}
	prf[1]=now->dec_ope_code;
	prf[0]+=4;
	return now->next;
}

ptr stp(struct code *link,struct code *now,int *prf1,int *axn1,int *prf2,int *axn2){//ִ��ͣ��ָ�� 
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
