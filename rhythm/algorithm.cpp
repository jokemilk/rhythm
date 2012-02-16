#include "algorithm.h"
#include <math.h>
#include <stdio.h>


//��������󳤶�


//��������
//�����������
 unsigned int buffer_raw[BUFFER_SZ];
 float buffer_filtered[BUFFER_SZ];
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
uint ClosePoint;
float delt[2];
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
}Fl,Fr;
struct buffer_raw_index{
uint* start;
uint* end;
//uint* now;
int now;
}buffer_raw_index;
struct buffer_filter_index{
float* start;
float* end;
//uint* now;
int now;
}buffer_filtered_inedx;

//�˲�������
static double Filter[FILTER_SZ]={0.0505,0.0027,0.0505,-1.5013,0.6051};


//������
//��ʼ���������������ڻ�ò��ε�ʱ������
int init_sample()
{
	Fg.Fini = 1;
//	resetcnt();
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
	static int count1 = 0;
	static int count2 = 0;
//	static int count3 = 0;
//����ԭʼ����
	uint temp;
	temp = read();
	buffer_raw[buffer_raw_index.now++] = temp;
	if(buffer_raw_index.now == BUFFER_SZ)
	{
		buffer_raw_index.now = 0;
	}
//�˲�
	filter();
	temp = buffer_filtered[(buffer_filtered_inedx.now+BUFFER_SZ-1)%BUFFER_SZ];
	if(Fg.Fini)
	{
		if(count2++ == INI_SAMPLE_PERIOD)
		{
			count2 = 0;
			count1 = 0;
			Fg.Fini_ok = 1;
		}
	}
	if(Fg.Restcnt)
	{
		Fg.Restcnt = 0;
		count1 = 0;
	}
//����ԭʼ����
	if(Tz.useful == 1)
	{
//Ԥ����
		Tz.delt[0] = Tz.delt[1];
		Tz.delt[1] = buffer_filtered[(buffer_filtered_inedx.now+BUFFER_SZ-1)%BUFFER_SZ]-buffer_filtered[(buffer_filtered_inedx.now+BUFFER_SZ-4)%BUFFER_SZ];		 
		if(Tz.delt[0]*Tz.delt[1] <0)
		{
//			if(count3 == 0)
//			{
//				count3++;
	//��ȡ��ֵ
	//��ȡ����ֵ
			if(temp >Tz.Ave && (uint)count1 > (Tz.Period/2)&& Tz.updown == BOTTOM)
				{
					Tz.Max = temp;
					Tz.updown = TOP;
					//if(Tz.updown == DOWN)
					//	Tz.updown = UP;
					//else
					//	Tz.updown = DOWN;
	//��ȡ����
					Tz.Period = count1;
	//���¿�ʼ��ʱ
					count1 = 0 ;
					Fg.GotPoint = 0;
				}
	//��ȡ��Сֵ
				else if((uint)count1 > (Tz.ClosePoint+Tz.RelaxPeriod*0.66) && (uint)count1 > (Tz.Period*0.5)&& Tz.delt[1] > 0 && Tz.updown == HCLOSED)
				{
					Tz.Min =temp;
	//��ȡƽ��ֵ
					Tz.Ave = (Tz.Max+Tz.Min)/2;
					Tz.updown = BOTTOM;
					Tz.RelaxPeriod = count1 - Tz.ClosePoint;
				}

//			}
//			count1++;
		}else
		{
	//�����ж�
			if(!Fg.GotPoint && Tz.updown == TOP && abs(Tz.delt[1]) < YULIANG && count1 > 10)
			{
				Fg.GotPoint = 1;
				Tz.updown = HCLOSED;
				Tz.ClosePoint = count1;
			}
			count1 = count1+1;
//			count3 = 0;
		}	
	}else if(Tz.useful == 0)
	{
		count1 = count1+1;
		if(count1 == SAMPLE_RATE)
		{
			Tz.useful = 1;
			count1 = 0;
		}else
		{
			if(temp > Tz.Max)
			{
				Tz.Max = temp;	
			}else if(temp <Tz.Min)
				Tz.Min = temp,Tz.Ave = (Tz.Max+Tz.Min)/2;			
		}
	}
	if(Tz.updown ==TOP)
		fprintf(Fr.fp,"%d  \n",100);
	else if(Tz.updown == HCLOSED)
		fprintf(Fr.fp,"%d  \n",50);
	else 
		fprintf(Fr.fp,"%d  \n",0);
	return SUCCESS;
}
//��ʼ����
int startsample()
{
	resetbuffer();
	Tz.updown = 0;
	Tz.useful = 0;
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
	for(i=0;i<FILTER_SZ_1;i++)
	{
		index = index - 1;
		index = (index < 0)?(index + BUFFER_SZ):(index);
		temp+=buffer_raw[index]*Filter[i];
	}
	index = buffer_filtered_inedx.now;
	for(i=0;i<(FILTER_SZ-FILTER_SZ_1);i++)
	{
		index = index - 1;
		index = (index < 0)?(index + BUFFER_SZ):(index);
		temp-=buffer_filtered[index]*Filter[i+FILTER_SZ_1];
	}
	buffer_filtered[buffer_filtered_inedx.now++] = temp;
	if(0 == buffer_filtered_inedx.now%BUFFER_SZ)
		buffer_filtered_inedx.now = 0;
}
//�Ի������㡢ָ�븴λ
void resetbuffer()
{
	int i;
	uint *p1;
	float*p2;
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
	Fr.fp = fopen("output.txt","w");
	if(Fr.fp == NULL)
	{
		return FAIL;
	}
	Fl.IsOpen = 1;
	Fl.cnt = 0;
	Fr.IsOpen = 1;
	Fr.cnt = 0;
	return SUCCESS;
}

void resetFlags()
{
	Fg.Fini = 0;
	Fg.Fini_ok = 0;
	Fg.GotPoint = 0;
	Fg.Restcnt = 0;
}
void closefile()
{
	fclose(Fl.fp);
	fclose(Fr.fp);
	Fl.IsOpen = 0;
	Fl.cnt = 0;
	Fr.IsOpen = 0;
	Fr.cnt = 0;
}
void test()
{
	resetFlags();
	resetbuffer();
	openfile();
	init_sample();
	closefile();
}