#include "algorithm.h"



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
}Fg;

struct buffer_index{
uint* start;
uint* end;
//uint* now;
int now;
}buffer_raw_index,buffer_filtered_inedx;

//�˲�������
static double Filter[FILTER_SZ]={0.025,0.0905,0.1669,0.2176,0.2176,0.1669,0.0905,0.025};


//������
int init_sample()
{
	Fg.Fini = 1;
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

int sample()
{
	uint temp;
	static int num = 0;
	temp = read();
	buffer_raw[buffer_raw_index.now++] = temp;
	if(0 == buffer_raw_index.now % BUFFER_SZ)
	{
		buffer_raw_index.now = 0;
	}
	filter();
	if(Fg.Fini)
	{
		if(INI_SAMPLE_PERIOD == ++num);//��þ�ֵ
		{
			Fg.Fini_ok = 1;
			num = 0;
			Tz.Ave = (Tz.Max+Tz.Min)/2;
		}
		if(temp > Tz.Max)//��ȡ���ֵ
		{
			Tz.Max = temp;	
		}else if(temp <Tz.Min)//��ȡ��Сֵ
			Tz.Min = temp;
		if(Tz.useful)//��ȡ����
		{
			
		}
	}
	if(!Tz.useful)
	{
		if(SAMPLE_RATE == num)
			Tz.useful = 1;
	}
	return SUCCESS;
}

int startsample()
{
	restbuffer();
	return SUCCESS;
}

int stopsample()
{
	restbuffer();
	Tz.updown = 0;
	Tz.useful = 0;
	return SUCCESS;
}

uint read()
{
	return FAIL;
}

void filter()
{
	uint i;
	int index = buffer_raw_index.now - 1;
	double temp = 0;
	index = index ? index:(index + BUFFER_SZ);
	for(i=0;i<FILTER_SZ;i++)
	{
		index = index - i;
		index = (index < 0)?(index + BUFFER_SZ):(index);
		temp+=buffer_raw[index]*Filter[i];
	}
	buffer_filtered[buffer_filtered_inedx.now++] = (uint)temp;
	if(0 == buffer_filtered_inedx.now%BUFFER_SZ)
		buffer_filtered_inedx.now = 0;
}

void restbuffer()
{
	int i;
	uint *p1,*p2;
	p1 = buffer_raw_index.start;
	p2 = buffer_filtered_inedx.start;
	buffer_raw_index.start = buffer_raw;
	buffer_raw_index.now = 0;
	buffer_raw_index.end = buffer_raw_index.start + BUFFER_SZ -1;
	buffer_filtered_inedx.start = buffer_filtered;
	buffer_filtered_inedx.now = 0;
	buffer_filtered_inedx.end = buffer_filtered_inedx.start + BUFFER_SZ -1;
	for(i=0;i<BUFFER_SZ;i++)
	{
		*p1++ = 0;
		*p2++ = 0;
	}
}