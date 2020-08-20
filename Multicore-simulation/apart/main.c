#ifndef _HEADERNAME_H
#define _HEADERNAME_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <process.h> 
#include <windows.h>
#endif
#include "high_design.h"//�漰�߲���ƣ�ȫ�֣������ṹ�岿�� 
#include "load.c"//ָ����غ������� 
#include "analyse.c"//ָ������������� 
#include "run.c"//���к������� 

int main()
{
	ptr code_link1,code_link2; 
	HANDLE thread1,thread2;//�߳̾�� 
	code_link1=(ptr)malloc(sizeof(struct code));//��������ͷ��� (�˴����߳�һ��ͷ��� 
	code_link1->next=NULL;
	code_link1->thread_id=1;// �߳�һ���������ͷ�����߳�id��Ϊ1 
	code_link2=(ptr)malloc(sizeof(struct code));//��������ͷ��� (�˴����̶߳���ͷ��� 
	code_link2->next=NULL;
	code_link2->thread_id=2;
	FILE *code_file1=fopen("dict1.dic","r");//�����ļ��������ļ���ָ�� 
	FILE *code_file2=fopen("dict2.dic","r");
	load_code(code_link1,code_file1);//������루����1 
	load_code(code_link2,code_file2);//������루����2 
	fclose(code_file1);
	fclose(code_file2);//�رտ������ļ� 
	data[0]=TICKETS; //��ʼ��Ʊ��Ϊ100 
	thread1=(HANDLE)_beginthreadex(NULL,0,run_code,(void*)code_link1,0,NULL);
	thread2=(HANDLE)_beginthreadex(NULL,0,run_code,(void*)code_link2,0,NULL);//���������̣߳�����ָ��ǿת 
	WaitForSingleObject(thread1, INFINITE);
	CloseHandle (thread1);
	WaitForSingleObject(thread2, INFINITE); //�ȴ����� 
	CloseHandle (thread2);//�ر��߳�
	all_sta(code_link1,code_link2);//����ڴ������ 
	return 0;
}
