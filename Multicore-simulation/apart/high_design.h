#ifndef STRUCT_CLOCK//宏定义锁 
#define STRUCT_CLOCK
#define DATA_MAX 260 //数据段内存的最大值 
#define TICKETS 100//需要卖出的票数 
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
HANDLE hMutex;//互斥对象句柄 
HANDLE hMutex_state;//避免输出不完整 
#endif
