#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <process.h> 
#include <windows.h>
#define TICKETS 100//������Ʊ���� 
#define DATA_MAX 260 //���ڴ������ݶ��ڴ����������ֵ
 
HANDLE hMutex,hMutex_state;//��������� 
struct code{//����νṹ��
	int thread_id;//�̵߳�id��������Ʊ���������ֻ��ֵ��ͷ��� 
	int dec_code;//����ʮ����ԭ��
	int dec_ope_code;//ʮ���Ʋ���ԭ��(1-16λ�� 
	int cmd_ope_code;//ʮ����ָ�ǰ8λ�� 
	int pre_ope_code;//9-12λ 
	int bac_ope_code;//13-16λ 
	int dec_imm_code;//ʮ����������(17-32)
	char bin_code[36];//������ԭ���ַ�������
	struct code *next;//ָ������
};
typedef struct code *ptr;//����ṹ��ָ��
int data[DATA_MAX];//���ڴ��θ������ƣ�����ʹ��ȫ�ֱ��� 

void load_code(struct code*,FILE*);//������� 
unsigned __stdcall run_code(void*);//���г��� 
int trans(char*,int,int,int);//���н���ת�� 
void sta(struct code*,int*,int*,int*,int*);//ִ����һ��ָ������״̬ 
void all_sta(struct code*,struct code*);//����ڴ����Ϣ 

ptr mov(struct code*,struct code*,int*,int*,int*,int*);//���ݴ��� 
ptr cul(struct code*,struct code*,int*,int*,int*,int*);//�������� 
ptr lgc(struct code*,struct code*,int*,int*,int*,int*);//�߼����� 
ptr cmp(struct code*,struct code*,int*,int*,int*,int*);//�Ƚ�ָ�� 
ptr jmp(struct code*,struct code*,int*,int*,int*,int*);//��תָ�� 
ptr scp(struct code*,struct code*,int*,int*,int*,int*);//��׼������� 
ptr stp(struct code*,struct code*,int*,int*,int*,int*);//��׼������� 
ptr mtd(struct code*,struct code*,int*,int*,int*,int*);//���߳���ز��� 

int main()
{
	ptr code_link1,code_link2; //����ͷ������ 
	HANDLE thread1,thread2;//�߳̾�� 
	code_link1=(ptr)malloc(sizeof(struct code));//��������ͷ��� (�˴����߳�һ��ͷ��� 
	code_link1->next=NULL;
	code_link1->thread_id=1;//�߳�һid��ֵ 
	code_link2=(ptr)malloc(sizeof(struct code));//��������ͷ��� (�˴����̶߳���ͷ��� 
	code_link2->next=NULL;
	code_link2->thread_id=2;
	FILE *code_file1=fopen("dict1.dic","r");//���ļ� 
	FILE *code_file2=fopen("dict2.dic","r");
	load_code(code_link1,code_file1);//������루����1 
	load_code(code_link2,code_file2);//������루����2 
	fclose(code_file1);//�ر��ļ� 
	fclose(code_file2);
	data[0]=TICKETS; //��ʼ��16384��ַָ���ڴ�Ʊ��Ϊ100 
	thread1=(HANDLE)_beginthreadex(NULL,0,run_code,(void*)code_link1,0,NULL);
	thread2=(HANDLE)_beginthreadex(NULL,0,run_code,(void*)code_link2,0,NULL);//���������߳�
	WaitForSingleObject(thread1, INFINITE);
	CloseHandle (thread1);
	WaitForSingleObject(thread2, INFINITE); //�ȴ����� 
	CloseHandle (thread2);//�ر��߳�
	all_sta(code_link1,code_link2);//����ڴ���Ϣ 
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

unsigned __stdcall run_code(void *pArguments){
	ptr link=(ptr)pArguments;
	hMutex=CreateMutex (NULL, FALSE, "tickets");//������������ 
	hMutex_state=CreateMutex (NULL, FALSE, "states");//״̬������� 
	int prf1[4]={0};//0-2�������Ĵ�����ָ��Ĵ�������־�Ĵ��� ���߳�һʹ�õļĴ��� 
	int axn1[9]={0}; //0λ ��0��1-4�������ݼĴ�����5-8�����ַ�Ĵ��� (�߳�һʹ�õļĴ��� 
	int prf2[4]={0};//�̶߳�ʹ�õļĴ���
	int axn2[9]={0};//�̶߳�ʹ�õļĴ��� 
	ptr current=link->next;
	while(current!=NULL){
		if(current->cmd_ope_code==1) current=mov(link,current,prf1,axn1,prf2,axn2);//ת������ 
		else if(current->cmd_ope_code<=5&&current->cmd_ope_code>=2) current=cul(link,current,prf1,axn1,prf2,axn2);//��ѧ���� 
		else if(current->cmd_ope_code<=8&&current->cmd_ope_code>=6) current=lgc(link,current,prf1,axn1,prf2,axn2);//�߼����� 
		else if(current->cmd_ope_code==9) current=cmp(link,current,prf1,axn1,prf2,axn2);//�Ƚϲ��� 
		else if(current->cmd_ope_code==10) current=jmp(link,current,prf1,axn1,prf2,axn2);//��ת���� 
		else if(current->cmd_ope_code==11||current->cmd_ope_code==12) current=scp(link,current,prf1,axn1,prf2,axn2);//��׼������� 
		else if(current->cmd_ope_code>=13&&current->cmd_ope_code<=15) current=mtd(link,current,prf1,axn1,prf2,axn2);//���̲߳��� 
		else current=stp(link,current,prf1,axn1,prf2,axn2);//ͣ�� 
		WaitForSingleObject(hMutex_state,INFINITE);//�ȴ��߳������� 
		sta(link,prf1,axn1,prf2,axn2);//���״̬
		ReleaseMutex(hMutex_state); //�ͷ�״̬���������� 
	}
	_endthreadex(0);
}

int trans(char *s,int start,int end,int tag){
	int i=0,a=1;
	int sum=0;
	int num[36];
	if(tag==1){//����ת�� 
		if(s[start-1]=='1'){//��λΪ1�����ж�Ӧת�� 
			for(i=start-1;i<end;i++){
				if(s[i]-'0'==1) num[i]=0;
				else num[i]=1;
			}
			for(i=31;i>=16;i--){
				sum=sum+num[i]*a;
				a=a*2;
			}
			sum=-(sum+1);//ȡ��ֵ 
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

ptr scp(struct code *link,struct code *now,int *prf1,int *axn1,int *prf2,int *axn2){
	int number;
	int *axn,*prf;//����int����ָ�룬����ָ�����׼Ĵ��� 
	if(link->thread_id==1){//����ͷ�����߳�id��ѡ��ͬ�Ĵ��� 
		axn=axn1;prf=prf1;//ָ��Ĵ���һ 
	} 
	else{
		axn=axn2;prf=prf2;//ָ��Ĵ����� 
	}
	if(now->cmd_ope_code==11){//���� 
		printf("in:\n");
		scanf("%d",&number);
		axn[now->pre_ope_code]=number;
	}
	else{//��� 
		printf("id = %d    out: %d\n",link->thread_id,axn[now->pre_ope_code]);
	}
	prf[1]=now->dec_ope_code;
	prf[0]+=4;
	return now->next;
}

ptr mov(struct code *link,struct code *now,int *prf1,int *axn1,int *prf2,int *axn2){
	int dword=0;//����ƫ���� 
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
	int dword;
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
	else{//���������� 
		Sleep(now->dec_imm_code);
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

void sta(struct code *link,int *prf1,int *axn1,int *prf2,int *axn2){//���״̬ 
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
	printf("codeSegment :\n");//���������ڴ� 
	for(row=0;row<16;row++){
		if(row==8) temp=link2->next;//���һ��֮������̶߳��Ĵ���� 
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

