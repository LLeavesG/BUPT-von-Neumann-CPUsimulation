#ifndef _HEADERNAME_H
#define _HEADERNAME_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#endif
#include "struct.h"//结构体定义部分 
#include "load.c"//指令加载函数部分 
#include "analyse.c"//指令分析函数部分 
#include "run.c"//运行函数部分 

#define DATA_MAX 260 //数据段内存的最大值 

int main()
{
	ptr code_link;
	int data[DATA_MAX]={0};//申请数组用于储存数据段内存 
	code_link=(ptr)malloc(sizeof(struct code));//申请代码段头结点 
	code_link->next=NULL;
	FILE *code_file=fopen("dict.dic","r");
	load_code(code_link,code_file);//载入代码
	run_code(code_link,data);//运行代码 
	fclose(code_file); 
	return 0;
}
