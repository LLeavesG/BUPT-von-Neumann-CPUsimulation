#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <process.h> 
#include <windows.h>
#define TICKETS 100//定义卖票数量 
#define DATA_MAX 260 //用于储存数据段内存的数组的最大值
 
HANDLE hMutex,hMutex_state;//互斥对象句柄 
struct code{//代码段结构体
	int thread_id;//线程的id，由于设计便利，可以只赋值给头结点 
	int dec_code;//储存十进制原码
	int dec_ope_code;//十进制操作原码(1-16位） 
	int cmd_ope_code;//十进制指令（前8位） 
	int pre_ope_code;//9-12位 
	int bac_ope_code;//13-16位 
	int dec_imm_code;//十进制立即数(17-32)
	char bin_code[36];//二进制原码字符型数组
	struct code *next;//指针连接
};
typedef struct code *ptr;//代码结构体指针
int data[DATA_MAX];//由于传参个数限制，这里使用全局变量 

void load_code(struct code*,FILE*);//载入代码 
unsigned __stdcall run_code(void*);//运行程序 
int trans(char*,int,int,int);//进行进制转换 
void sta(struct code*,int*,int*,int*,int*);//执行完一条指令后输出状态 
void all_sta(struct code*,struct code*);//输出内存段信息 

ptr mov(struct code*,struct code*,int*,int*,int*,int*);//数据传送 
ptr cul(struct code*,struct code*,int*,int*,int*,int*);//算术运算 
ptr lgc(struct code*,struct code*,int*,int*,int*,int*);//逻辑运算 
ptr cmp(struct code*,struct code*,int*,int*,int*,int*);//比较指令 
ptr jmp(struct code*,struct code*,int*,int*,int*,int*);//跳转指令 
ptr scp(struct code*,struct code*,int*,int*,int*,int*);//标准输入输出 
ptr stp(struct code*,struct code*,int*,int*,int*,int*);//标准输入输出 
ptr mtd(struct code*,struct code*,int*,int*,int*,int*);//多线程相关操作 

int main()
{
	ptr code_link1,code_link2; //创建头结点变量 
	HANDLE thread1,thread2;//线程句柄 
	code_link1=(ptr)malloc(sizeof(struct code));//申请代码段头结点 (此处是线程一的头结点 
	code_link1->next=NULL;
	code_link1->thread_id=1;//线程一id赋值 
	code_link2=(ptr)malloc(sizeof(struct code));//申请代码段头结点 (此处是线程二的头结点 
	code_link2->next=NULL;
	code_link2->thread_id=2;
	FILE *code_file1=fopen("dict1.dic","r");//打开文件 
	FILE *code_file2=fopen("dict2.dic","r");
	load_code(code_link1,code_file1);//载入代码（进程1 
	load_code(code_link2,code_file2);//载入代码（进程2 
	fclose(code_file1);//关闭文件 
	fclose(code_file2);
	data[0]=TICKETS; //初始化16384地址指向内存票数为100 
	thread1=(HANDLE)_beginthreadex(NULL,0,run_code,(void*)code_link1,0,NULL);
	thread2=(HANDLE)_beginthreadex(NULL,0,run_code,(void*)code_link2,0,NULL);//创建两个线程
	WaitForSingleObject(thread1, INFINITE);
	CloseHandle (thread1);
	WaitForSingleObject(thread2, INFINITE); //等待结束 
	CloseHandle (thread2);//关闭线程
	all_sta(code_link1,code_link2);//输出内存信息 
	return 0;
}

void load_code(struct code *link,FILE *fp){
	int i=0;
	char temp[36],ope[36],c;
	ptr apply,now=link;
	c=fgetc(fp);
	while(c!=EOF){
		while(c!=EOF&&c!='\n'){//读入一行二进制数储存在temp字符串中 
			temp[i]=c;
			c=fgetc(fp);
			i++;
		}
		i=0;
		temp[32]='\0'; 
		apply=(ptr)malloc(sizeof(struct code));//申请一个新节点 
		strcmp(apply->bin_code,temp);
		apply->dec_code=trans(temp,1,32,0);//此处开始将二进制转化为十进制 
		apply->dec_imm_code=trans(temp,17,32,1);//标志为1则补码转化 
		apply->dec_ope_code=trans(temp,1,16,0);//标志为0则原码转化 
		apply->cmd_ope_code=trans(temp,1,8,0);
		apply->pre_ope_code=trans(temp,9,12,0);
		apply->bac_ope_code=trans(temp,13,16,0); 
		now->next=apply;
		apply->next=NULL;
		now=apply;
		c=fgetc(fp);
	}
}

unsigned __stdcall run_code(void *pArguments){
	ptr link=(ptr)pArguments;
	hMutex=CreateMutex (NULL, FALSE, "tickets");//创建互斥类型 
	hMutex_state=CreateMutex (NULL, FALSE, "states");//状态输出互斥 
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
		ReleaseMutex(hMutex_state); //释放状态输出互斥对象 
	}
	_endthreadex(0);
}

int trans(char *s,int start,int end,int tag){
	int i=0,a=1;
	int sum=0;
	int num[36];
	if(tag==1){//补码转化 
		if(s[start-1]=='1'){//首位为1，进行对应转化 
			for(i=start-1;i<end;i++){
				if(s[i]-'0'==1) num[i]=0;
				else num[i]=1;
			}
			for(i=31;i>=16;i--){
				sum=sum+num[i]*a;
				a=a*2;
			}
			sum=-(sum+1);//取负值 
		}
		else{
			for(i=start-1;i<end;i++){
				num[i]=s[i]-'0';
			}
			for(i=31;i>=16;i--){
				sum=sum+num[i]*a;
				a=a*2;
			}
		}
	}
	else{//原码转化 
		for(i=end-1;i>=start-1;i--){
			sum=sum+(s[i]-'0')*a;
			a=a*2;
		}
	}
	return sum;//返回结果 
}

ptr scp(struct code *link,struct code *now,int *prf1,int *axn1,int *prf2,int *axn2){
	int number;
	int *axn,*prf;//设置int类型指针，方便指向两套寄存器 
	if(link->thread_id==1){//根据头结点的线程id，选择不同寄存器 
		axn=axn1;prf=prf1;//指向寄存器一 
	} 
	else{
		axn=axn2;prf=prf2;//指向寄存器二 
	}
	if(now->cmd_ope_code==11){//输入 
		printf("in:\n");
		scanf("%d",&number);
		axn[now->pre_ope_code]=number;
	}
	else{//输出 
		printf("id = %d    out: %d\n",link->thread_id,axn[now->pre_ope_code]);
	}
	prf[1]=now->dec_ope_code;
	prf[0]+=4;
	return now->next;
}

ptr mov(struct code *link,struct code *now,int *prf1,int *axn1,int *prf2,int *axn2){
	int dword=0;//设置偏移量 
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
	int dword;
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
	else{//休眠立即数 
		Sleep(now->dec_imm_code);
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

void sta(struct code *link,int *prf1,int *axn1,int *prf2,int *axn2){//输出状态 
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

