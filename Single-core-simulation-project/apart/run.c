#ifndef _HEADERNAME_H
#define _HEADERNAME_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#endif
#include "struct.h" 

void state_print(int*,int*);
void run_code(struct code*,int*);

void state_print(int *prf,int *axn){//���״̬ 
	printf("ip = %d\n",prf[0]);
	printf("flag = %d\n",prf[2]);
	printf("ir = %d\n",prf[1]);
	printf("ax1 = %d ax2 = %d ax3 = %d ax4 = %d\n",axn[1],axn[2],axn[3],axn[4]);
	printf("ax5 = %d ax6 = %d ax7 = %d ax8 = %d\n",axn[5],axn[6],axn[7],axn[8]);
}

void run_code(struct code *link,int *data){//����
	int row=0,col=0,i=0;
	int prf[4]={0};//0-2�������Ĵ�����ָ��Ĵ�������־�Ĵ��� 
	int axn[9]={0}; //0λ ��0��1-4�������ݼĴ�����5-8�����ַ�Ĵ��� 
	ptr current=link->next;
	ptr temp=link->next;
	while(current!=NULL){
		if(current->cmd_ope_code==1) current=mov(link,current,prf,axn,data);//ת������ 
		else if(current->cmd_ope_code<=5&&current->cmd_ope_code>=2) current=cul(link,current,prf,axn,data);//��ѧ���� 
		else if(current->cmd_ope_code<=8&&current->cmd_ope_code>=6) current=lgc(link,current,prf,axn,data);//�߼����� 
		else if(current->cmd_ope_code==9) current=cmp(link,current,prf,axn,data);//�Ƚϲ��� 
		else if(current->cmd_ope_code==10) current=jmp(link,current,prf,axn,data);//��ת���� 
		else if(current->cmd_ope_code==11||current->cmd_ope_code==12) current=scp(link,current,prf,axn,data);//��׼������� 
		else current=stp(link,current,prf,axn,data);//ͣ�� 
		state_print(prf,axn);//���״̬ 
	}
	printf("\n"); 
	printf("codeSegment :\n");//���������ڴ� 
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
	printf("dataSegment :\n");//������ݶ��ڴ� 
	for(i=0;i<256;i++){
		printf("%d ",data[i]);
		if((i+1)%16==0) printf("\n");
	}
}
