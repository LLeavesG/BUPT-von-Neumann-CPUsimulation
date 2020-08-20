#ifndef _HEADERNAME_H
#define _HEADERNAME_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <process.h> 
#include <windows.h>
#endif
#include "high_design.h"//涉及高层设计（全局）包含结构体部分 
#include "load.c"//指令加载函数部分 
#include "analyse.c"//指令分析函数部分 
#include "run.c"//运行函数部分 

int main()
{
	ptr code_link1,code_link2; 
	HANDLE thread1,thread2;//线程句柄 
	code_link1=(ptr)malloc(sizeof(struct code));//申请代码段头结点 (此处是线程一的头结点 
	code_link1->next=NULL;
	code_link1->thread_id=1;// 线程一代码段链表头结点的线程id赋为1 
	code_link2=(ptr)malloc(sizeof(struct code));//申请代码段头结点 (此处是线程二的头结点 
	code_link2->next=NULL;
	code_link2->thread_id=2;
	FILE *code_file1=fopen("dict1.dic","r");//开启文件并读入文件的指令 
	FILE *code_file2=fopen("dict2.dic","r");
	load_code(code_link1,code_file1);//载入代码（进程1 
	load_code(code_link2,code_file2);//载入代码（进程2 
	fclose(code_file1);
	fclose(code_file2);//关闭开启的文件 
	data[0]=TICKETS; //初始化票数为100 
	thread1=(HANDLE)_beginthreadex(NULL,0,run_code,(void*)code_link1,0,NULL);
	thread2=(HANDLE)_beginthreadex(NULL,0,run_code,(void*)code_link2,0,NULL);//创建两个线程（进行指针强转 
	WaitForSingleObject(thread1, INFINITE);
	CloseHandle (thread1);
	WaitForSingleObject(thread2, INFINITE); //等待结束 
	CloseHandle (thread2);//关闭线程
	all_sta(code_link1,code_link2);//输出内存段数据 
	return 0;
}
