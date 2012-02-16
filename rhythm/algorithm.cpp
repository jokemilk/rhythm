#include "algorithm.h"
#include <math.h>
#include <stdio.h>


//缓冲区最大长度


//变量定义
//缓冲变量池子
 unsigned int buffer_raw[BUFFER_SZ];
 float buffer_filtered[BUFFER_SZ];
//标准单位
//原始数据
//前项滤波结果
//特征值
struct Tezhen{
uint Max;//最大值
uint Period;//周期
uint Min;//最小值
uint RelaxPeriod;//舒缓期长度
uint Ave; //平均值
uint useful;//数据是够有效
uint updown;//0->上升期 1->下降期
uint ClosePoint;
float delt[2];
}Tz;

struct Flag{
uint Fini ;//采用初始化
uint Fini_ok ;
uint Restcnt;
uint GotPoint;//获取动脉瓣关闭点
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

//滤波器参数
static double Filter[FILTER_SZ]={0.0505,0.0027,0.0505,-1.5013,0.6051};


//函数体
//初始化采样函数，用于获得波形的时域特征
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
//采样函数，有滤波功能
int sample()
{
	static int count1 = 0;
	static int count2 = 0;
//	static int count3 = 0;
//采样原始数据
	uint temp;
	temp = read();
	buffer_raw[buffer_raw_index.now++] = temp;
	if(buffer_raw_index.now == BUFFER_SZ)
	{
		buffer_raw_index.now = 0;
	}
//滤波
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
//采样原始数据
	if(Tz.useful == 1)
	{
//预处理
		Tz.delt[0] = Tz.delt[1];
		Tz.delt[1] = buffer_filtered[(buffer_filtered_inedx.now+BUFFER_SZ-1)%BUFFER_SZ]-buffer_filtered[(buffer_filtered_inedx.now+BUFFER_SZ-4)%BUFFER_SZ];		 
		if(Tz.delt[0]*Tz.delt[1] <0)
		{
//			if(count3 == 0)
//			{
//				count3++;
	//获取极值
	//获取极大值
			if(temp >Tz.Ave && (uint)count1 > (Tz.Period/2)&& Tz.updown == BOTTOM)
				{
					Tz.Max = temp;
					Tz.updown = TOP;
					//if(Tz.updown == DOWN)
					//	Tz.updown = UP;
					//else
					//	Tz.updown = DOWN;
	//获取周期
					Tz.Period = count1;
	//重新开始计时
					count1 = 0 ;
					Fg.GotPoint = 0;
				}
	//获取极小值
				else if((uint)count1 > (Tz.ClosePoint+Tz.RelaxPeriod*0.66) && (uint)count1 > (Tz.Period*0.5)&& Tz.delt[1] > 0 && Tz.updown == HCLOSED)
				{
					Tz.Min =temp;
	//获取平均值
					Tz.Ave = (Tz.Max+Tz.Min)/2;
					Tz.updown = BOTTOM;
					Tz.RelaxPeriod = count1 - Tz.ClosePoint;
				}

//			}
//			count1++;
		}else
		{
	//特征判断
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
//开始采样
int startsample()
{
	resetbuffer();
	Tz.updown = 0;
	Tz.useful = 0;
	return SUCCESS;
}
//停止采样
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
//滤波函数，平滑
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
//对缓冲清零、指针复位
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
//清空定时器计数器
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