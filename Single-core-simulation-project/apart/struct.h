#ifndef STRUCT_CLOCK//�궨���� 
#define STRUCT_CLOCK
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
typedef struct code *ptr;//�ṹ������ָ��
#endif
