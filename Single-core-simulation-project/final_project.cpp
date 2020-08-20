#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct code{//����νṹ��
	int dec_code;//ʮ����ԭ��
	int dec_ope_code;//ʮ���Ʋ���ԭ��
	int cmd_ope_code;//ʮ����ָ�ǰ8λ�� 
	int pre_ope_code;//9-12λ 
	int bac_ope_code;//13-16λ 
	int dec_imm_code;//ʮ����������
	char bin_code[36];//������ԭ���ַ�������
	struct code *next;//ָ������
};
typedef struct code *ptr;//����ṹ��ָ��

void load_code(struct code*,FILE*);
void run_code(struct code*,int*);
int trans(char*,int,int,int);
void sta(int*,int*);//ִ����һ��ָ������״̬ 

ptr mov(struct code*,struct code*,int*,int*,int*);//���ݴ��� 
ptr cul(struct code*,struct code*,int*,int*,int*);//�������� 
ptr lgc(struct code*,struct code*,int*,int*,int*);//�߼����� 
ptr cmp(struct code*,struct code*,int*,int*,int*);//�Ƚ�ָ�� 
ptr jmp(struct code*,struct code*,int*,int*,int*);//��תָ�� 
ptr scp(struct code*,struct code*,int*,int*,int*);//��׼������� 
ptr stp(struct code*,struct code*,int*,int*,int*);//��׼������� 

int main()
{
	ptr code_link;
	int data[260]={0};
	code_link=(ptr)malloc(sizeof(struct code));//��������ͷ��� 
	code_link->next=NULL;
	FILE *code_file=fopen("dict.dic","r");
	load_code(code_link,code_file);//�������
	run_code(code_link,data);//���д��� 
	fclose(code_file); 
	return 0;
}

void load_code(struct code *link,FILE *fp){
	int i=0;
	char temp[36],ope[36],c;
	ptr apply,now=link;
	c=fgetc(fp);
	while(c!=EOF){
		while(c!=EOF&&c!='\n'){//����һ�ж�������������temp�ַ����� 
			temp[i]=c;
			c=fgetc(fp);
			i++;
		}
		i=0;
		temp[32]='\0'; 
		apply=(ptr)malloc(sizeof(struct code));//����һ���½ڵ� 
		strcmp(apply->bin_code,temp);
		apply->dec_code=trans(temp,1,32,0);//�˴���ʼ��������ת��Ϊʮ���� 
		apply->dec_imm_code=trans(temp,17,32,1);//��־Ϊ1����ת�� 
		apply->dec_ope_code=trans(temp,1,16,0);//��־Ϊ0��ԭ��ת�� 
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
		sta(prf,axn);//���״̬ 
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

int trans(char *s,int start,int end,int tag){
	int i=0,a=1;
	int sum=0;
	int num[36];
	if(tag==1){//����ת�� 
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
	else{//ԭ��ת�� 
		for(i=end-1;i>=start-1;i--){
			sum=sum+(s[i]-'0')*a;
			a=a*2;
		}
	}
	return sum;//���ؽ�� 
}

ptr scp(struct code *link,struct code *now,int *prf,int *axn,int *data){
	int number;
	if(now->cmd_ope_code==11){//���� 
		printf("in:\n");
		scanf("%d",&number);
		axn[now->pre_ope_code]=number;
	}
	else{//��� 
		printf("out: %d\n",axn[now->pre_ope_code]);
	}
	prf[1]=now->dec_ope_code;
	prf[0]+=4;
	return now->next;
}

ptr mov(struct code *link,struct code *now,int *prf,int *axn,int *data){
	int dword=0;//����ƫ���� 
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

ptr lgc(struct code *link,struct code *now,int *prf,int *axn,int *data){
	int dword;
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
	ptr jump_node=link,result;//��ת��Ŀ��ڵ� 
	int count=0,state=0;//��������״̬ 
	int jump_number;//��ת�ڵ���� 
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
	//���״̬ 
	printf("ip = %d\n",prf[0]);
	printf("flag = %d\n",prf[2]);
	printf("ir = %d\n",prf[1]);
	printf("ax1 = %d ax2 = %d ax3 = %d ax4 = %d\n",axn[1],axn[2],axn[3],axn[4]);
	printf("ax5 = %d ax6 = %d ax7 = %d ax8 = %d\n",axn[5],axn[6],axn[7],axn[8]);
}


