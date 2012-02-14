#include "algorithm.h"
#include <math.h>
#include <stdio.h>


//��������󳤶�


//��������
//�����������
 unsigned int buffer_raw[BUFFER_SZ];
 unsigned int buffer_filtered[BUFFER_SZ];
//��׼��λ
//ԭʼ����
//ǰ���˲����
//����ֵ
struct Tezhen{
uint Max;//���ֵ
uint Period;//����
uint Min;//��Сֵ
uint RelaxPeriod;//�滺�ڳ���
uint Ave; //ƽ��ֵ
uint useful;//�����ǹ���Ч
uint updown;//0->������ 1->�½���
float delt;
}Tz;

struct Flag{
uint Fini ;//���ó�ʼ��
uint Fini_ok ;
uint Restcnt;
uint GotPoint;//��ȡ������رյ�
}Fg;

struct File
{
	uint IsOpen;
	uint cnt;
	FILE *fp;
}Fl;
struct buffer_index{
uint* start;
uint* end;
//uint* now;
int now;
}buffer_raw_index,buffer_filtered_inedx;

//�˲�������
static double Filter[FILTER_SZ]={0.025,0.0905,0.1669,0.2176,0.2176,0.1669,0.0905,0.025};


//������
//��ʼ���������������ڻ�ò��ε�ʱ������
int init_sample()
{
	Fg.Fini = 1;
	resetcnt();
	Tz.Ave = 0;
	Tz.Max = 0;
	Tz.Min = 65535;
	Tz.useful = 0;
	Tz.updown = 0;
	Tz.Period = Tz.RelaxPeriod = 0;
	startsample();
	while(!Fg.Fini_ok)
		sample();
	stopsample();
	return SUCCESS;
}
//�������������˲�����
int sample()
{
	static uint count = 0;
	static int count1 = 0;
//����ԭʼ����
	uint temp;
	temp = read();
	buffer_raw[buffer_raw_index.now++] = temp;
//�˲�
	filter();
//�˲����������Ƿ���Ч
	if(Tz.useful)
	{
//Ԥ����
		Tz.delt = buffer_filtered[(buffer_filtered_inedx.now+BUFFER_SZ-1)%BUFFER_SZ]-buffer_filtered[(buffer_filtered_inedx.now+BUFFER_SZ-5)%BUFFER_SZ];
		if(abs(Tz.delt) <= YULIANG && count1 == 0)
		{
			count1++;
//��ȡ��ֵ
//��ȡ����ֵ
			if(temp >0.6*Tz.Max)
			{
				Tz.Max = temp;
				Tz.updown = DOWN;
//��ȡ����
				Tz.Period = count;
//���¿�ʼ��ʱ
				count = 0 ;
			}
//��ȡ��Сֵ
			else if(count > Tz.Period/2)
			{
				Tz.Min =temp;
//��ȡƽ��ֵ
				Tz.Ave = (Tz.Max+Tz.Min)/2;
				Tz.updown = UP;
			}
//�����ж�
			if(!Fg.GotPoint && Tz.updown == DOWN)
			{
				Fg.GotPoint = 0;
			}

		}else
		{
			count++;
			count1 = 0;
		}
	}else if(Fg.Restcnt)
	{
		Fg.Restcnt = 0;
		count = 0;
	}else if(count++ == SAMPLE_RATE)
	{
		Tz.useful = 1;
//		count = 0;	
	}if(count == INI_SAMPLE_PERIOD)
	{
		count = 0;
		Fg.Fini_ok = 1;
	}else
	{
		if(temp > Tz.Max)
		{
			Tz.Max = temp;	
		}else if(temp <Tz.Min)
			Tz.Min = temp;		
	}
	return SUCCESS;
}
//��ʼ����
int startsample()
{
	resetbuffer();
	return SUCCESS;
}
//ֹͣ����
int stopsample()
{
	resetbuffer();
	Tz.updown = 0;
	Tz.useful = 0;
	return SUCCESS;
}

uint read()
{
	uint ret;
	if(Fl.IsOpen)
	{
		fscanf(Fl.fp,"%d",&ret);
		Fl.cnt++;
		return ret;
	}
	return FAIL;
}
//�˲�������ƽ��
void filter()
{
	uint i;
	int index = buffer_raw_index.now;
	double temp = 0;
	index = index ? index:(index + BUFFER_SZ);
	for(i=0;i<FILTER_SZ;i++)
	{
		index = index - 1;
		index = (index < 0)?(index + BUFFER_SZ):(index);
		temp+=buffer_raw[index]*Filter[i];
	}
	buffer_filtered[buffer_filtered_inedx.now++] = (uint)temp;
	if(0 == buffer_filtered_inedx.now%BUFFER_SZ)
		buffer_filtered_inedx.now = 0;
}
//�Ի������㡢ָ�븴λ
void resetbuffer()
{
	int i;
	uint *p1,*p2;
	buffer_raw_index.start = buffer_raw;
	buffer_raw_index.now = 0;
	buffer_raw_index.end = buffer_raw_index.start + BUFFER_SZ -1;
	buffer_filtered_inedx.start = buffer_filtered;
	buffer_filtered_inedx.now = 0;
	buffer_filtered_inedx.end = buffer_filtered_inedx.start + BUFFER_SZ -1;
	p1 = buffer_raw_index.start;
	p2 = buffer_filtered_inedx.start;
	for(i=0;i<BUFFER_SZ;i++)
	{
		*p1++ = 0;
		*p2++ = 0;
	}
}
//��ն�ʱ��������
void resetcnt()
{
	Fg.Restcnt = 1;
}
uint openfile()
{
	Fl.fp = fopen("data.txt","r");
	if(Fl.fp == NULL)
	{
		return FAIL;
	}
	Fl.IsOpen = 1;
	Fl.cnt = 0;
	return SUCCESS;
}

void resetFlags()
{
	Fg.Fini = 0;
	Fg.Fini_ok = 0;
	Fg.GotPoint = 0;
	Fg.Restcnt = 0;
}

void test()
{
	resetFlags();
	resetbuffer();
	openfile();
	init_sample();
}