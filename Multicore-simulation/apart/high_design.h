#ifndef STRUCT_CLOCK//�궨���� 
#define STRUCT_CLOCK
#define DATA_MAX 260 //���ݶ��ڴ�����ֵ 
#define TICKETS 100//��Ҫ������Ʊ�� 
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
HANDLE hMutex;//��������� 
HANDLE hMutex_state;//������������� 
#endif
