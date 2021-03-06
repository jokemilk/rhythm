#ifndef _ALG_
#define _ALG_
//宏定义

#define SAMPLE_RATE	200	//采样率 200HZ
#define INI_SAMPLE_PERIOD SAMPLE_RATE*20	//初始化采用时间
#define BUFFER_SZ	200	//缓存区大小
#define FILTER_SZ_1	3//滤波长度
#define FILTER_SZ	5//滤波长度
#define uint unsigned int
#define uchar unsigned char
#define SUCCESS 1
#define FAIL 0
#define YULIANG 5.0f
#define YULIANG_1 3.0f
#define HCLOSED 1
#define BOTTOM 0
#define TOP 2
//函数定义

int sample();//读函数/采样函数
int startsample();//开始采样
int stopsample();//停止采样
void filter();//滤波函数
uint read();//读IO
int init_sample();//初始化函数
void resetbuffer();
void resetcnt();
uint openfile();
void resetFlags();
void test();
void closefile();
#endif