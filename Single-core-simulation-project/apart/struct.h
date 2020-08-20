#ifndef STRUCT_CLOCK//宏定义锁 
#define STRUCT_CLOCK
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
typedef struct code *ptr;//结构体类型指针
#endif
