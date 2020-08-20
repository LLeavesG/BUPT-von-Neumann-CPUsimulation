#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct code{//代码段结构体
	int dec_code;//十进制原码
	int dec_ope_code;//十进制操作原码
	int cmd_ope_code;//十进制指令（前8位） 
	int pre_ope_code;//9-12位 
	int bac_ope_code;//13-16位 
	int dec_imm_code;//十进制立即数
	char bin_code[36];//二进制原码字符型数组
	struct code *next;//指针连接
};
typedef struct code *ptr;//代码结构体指针

void load_code(struct code*,FILE*);
void run_code(struct code*,int*);
int trans(char*,int,int,int);
void sta(int*,int*);//执行完一条指令后输出状态 

ptr mov(struct code*,struct code*,int*,int*,int*);//数据传送 
ptr cul(struct code*,struct code*,int*,int*,int*);//算术运算 
ptr lgc(struct code*,struct code*,int*,int*,int*);//逻辑运算 
ptr cmp(struct code*,struct code*,int*,int*,int*);//比较指令 
ptr jmp(struct code*,struct code*,int*,int*,int*);//跳转指令 
ptr scp(struct code*,struct code*,int*,int*,int*);//标准输入输出 
ptr stp(struct code*,struct code*,int*,int*,int*);//标准输入输出 

int main()
{
	ptr code_link;
	int data[260]={0};
	code_link=(ptr)malloc(sizeof(struct code));//申请代码段头结点 
	code_link->next=NULL;
	FILE *code_file=fopen("dict.dic","r");
	load_code(code_link,code_file);//载入代码
	run_code(code_link,data);//运行代码 
	fclose(code_file); 
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

void run_code(struct code *link,int *data){
	int row=0,col=0,i=0;
	int prf[4]={0};//0-2代表程序寄存器，指令寄存器，标志寄存器 
	int axn[9]={0}; //0位 置0，1-4代表数据寄存器，5-8代表地址寄存器 
	ptr current=link->next;
	ptr temp=link->next;
	while(current!=NULL){
		if(current->cmd_ope_code==1) current=mov(link,current,prf,axn,data);//转移数据 
		else if(current->cmd_ope_code<=5&&current->cmd_ope_code>=2) current=cul(link,current,prf,axn,data);//数学运算 
		else if(current->cmd_ope_code<=8&&current->cmd_ope_code>=6) current=lgc(link,current,prf,axn,data);//逻辑运算 
		else if(current->cmd_ope_code==9) current=cmp(link,current,prf,axn,data);//比较操作 
		else if(current->cmd_ope_code==10) current=jmp(link,current,prf,axn,data);//跳转操作 
		else if(current->cmd_ope_code==11||current->cmd_ope_code==12) current=scp(link,current,prf,axn,data);//标准输入输出 
		else current=stp(link,current,prf,axn,data);//停机 
		sta(prf,axn);//输出状态 
	}
	printf("\n"); 
	printf("codeSegment :\n");//输出代码段内存 
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
	printf("dataSegment :\n");//输出数据段内存 
	for(i=0;i<256;i++){
		printf("%d ",data[i]);
		if((i+1)%16==0) printf("\n");
	}
}

int trans(char *s,int start,int end,int tag){
	int i=0,a=1;
	int sum=0;
	int num[36];
	if(tag==1){//补码转化 
		if(s[start-1]=='1'){
			for(i=start-1;i<end;i++){
				if(s[i]-'0'==1) num[i]=0;
				else num[i]=1;
			}
			for(i=31;i>=16;i--){
				sum=sum+num[i]*a;
				a=a*2;
			}
			sum=-(sum+1);
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

ptr scp(struct code *link,struct code *now,int *prf,int *axn,int *data){
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

ptr mov(struct code *link,struct code *now,int *prf,int *axn,int *data){
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

ptr cul(struct code *link,struct code *now,int *prf,int *axn,int *data){
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

ptr lgc(struct code *link,struct code *now,int *prf,int *axn,int *data){
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

ptr cmp(struct code *link,struct code *now,int *prf,int *axn,int *data){
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

ptr jmp(struct code *link,struct code *now,int *prf,int *axn,int *data){
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

ptr stp(struct code *link,struct code *now,int *prf,int *axn,int *data){
	prf[1]=now->dec_ope_code;
	prf[0]+=4;
	return now->next;
}

void sta(int *prf,int *axn){
	//输出状态 
	printf("ip = %d\n",prf[0]);
	printf("flag = %d\n",prf[2]);
	printf("ir = %d\n",prf[1]);
	printf("ax1 = %d ax2 = %d ax3 = %d ax4 = %d\n",axn[1],axn[2],axn[3],axn[4]);
	printf("ax5 = %d ax6 = %d ax7 = %d ax8 = %d\n",axn[5],axn[6],axn[7],axn[8]);
}


