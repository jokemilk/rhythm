#ifndef _ALG_
#define _ALG_
//�궨��

#define SAMPLE_RATE	200	//������ 200HZ
#define INI_SAMPLE_PERIOD SAMPLE_RATE*20	//��ʼ������ʱ��
#define BUFFER_SZ	200	//��������С
#define FILTER_SZ_1	3//�˲�����
#define FILTER_SZ	5//�˲�����
#define uint unsigned int
#define uchar unsigned char
#define SUCCESS 1
#define FAIL 0
#define YULIANG 5.0f
#define YULIANG_1 3.0f
#define HCLOSED 1
#define BOTTOM 0
#define TOP 2
//��������

int sample();//������/��������
int startsample();//��ʼ����
int stopsample();//ֹͣ����
void filter();//�˲�����
uint read();//��IO
int init_sample();//��ʼ������
void resetbuffer();
void resetcnt();
uint openfile();
void resetFlags();
void test();
void closefile();
#endif