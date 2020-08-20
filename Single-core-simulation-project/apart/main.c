#ifndef _HEADERNAME_H
#define _HEADERNAME_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#endif
#include "struct.h"//�ṹ�嶨�岿�� 
#include "load.c"//ָ����غ������� 
#include "analyse.c"//ָ������������� 
#include "run.c"//���к������� 

#define DATA_MAX 260 //���ݶ��ڴ�����ֵ 

int main()
{
	ptr code_link;
	int data[DATA_MAX]={0};//�����������ڴ������ݶ��ڴ� 
	code_link=(ptr)malloc(sizeof(struct code));//��������ͷ��� 
	code_link->next=NULL;
	FILE *code_file=fopen("dict.dic","r");
	load_code(code_link,code_file);//�������
	run_code(code_link,data);//���д��� 
	fclose(code_file); 
	return 0;
}
