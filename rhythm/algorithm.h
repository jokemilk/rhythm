#ifndef _ALG_
#define _ALG_
//�궨��

#define SAMPLE_RATE	200	//������ 200HZ
#define INI_SAMPLE_PERIOD SAMPLE_RATE*5	//��ʼ������ʱ��
#define BUFFER_SZ	200	//��������С
#define FILTER_SZ	8//�˲�����
#define uint unsigned int
#define uchar unsigned char
#define SUCCESS 1
#define FAIL 0
//��������

int sample();//������/��������
int startsample();//��ʼ����
int stopsample();//ֹͣ����
void filter();//�˲�����
uint read();//��IO
int init_sample();//��ʼ������
void restbuffer();

#endif