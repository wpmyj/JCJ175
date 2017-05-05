#include "define.h"
#define LEDMAX 11
#define LED_DP 0x80
#define LED_G  0x20
bank2 volatile unsigned char displaychar[]={0,0,0,0,0,0,0,0,0,0,0};	
const unsigned char shuzi[]={0x5F,0x50,0x3D,0x79,0x72,0x6B,0x6F,0x51,0x7F,0x7B};
const unsigned char zimu[]={0x77,0x6E,0x2C,0x7C,0x2F,0x27,0x7B,0x76,0x50,0x58,0x66,0xE,0x65,0x64,0x6C,0x37,0x73,0x24,0x6B,0x2E,0x5E,0x4C,0x1A,0x76,0x7A,0x3d};
const unsigned char  LedIndex[]={10,9,8,7,6,5,4,3};
const unsigned char PosUnit[][2]={{2,2},{1,6}};
const unsigned char ValueUnit[][2]={{0x80,0x08},{0x08,0x08}};
//const unsigned char dispUnit[]={2,1};
enum{NONE,HUMIUNIT,TEMPUNIT};

/*************************************************************************************************************
������:LED_Init
����:��ʾ����Ӧ������ʼ��
����:��
����:��
*************************************************************************************************************/
void LED_Init(void)
{	
	Ht1621_Init();
}
/*************************************************************************************************************
������:LED_DispUnit
����:��ʾ��λ
����:windows:			��ʾ����,��Χ0-1
	 unit   :           ��λ����  0:��  1:ʪ�� 2:�¶�
����:��
*************************************************************************************************************/
void LED_DispUnit(unsigned char windows,unsigned char unit)
{	
	switch(unit)
	{
		case NONE:
			displaychar[PosUnit[windows][0]]&=~ValueUnit[windows][0];
			displaychar[PosUnit[windows][1]]&=~ValueUnit[windows][1];	
			break;
		case HUMIUNIT:
			displaychar[PosUnit[windows][0]]|=ValueUnit[windows][0];
			displaychar[PosUnit[windows][1]]&=~ValueUnit[windows][1];	
			break;
		case TEMPUNIT:
			displaychar[PosUnit[windows][0]]&=~ValueUnit[windows][0];
			displaychar[PosUnit[windows][1]]|=ValueUnit[windows][1];	
			break;
	}
}
/*************************************************************************************************************
������:LED_DispLight1
����:��ʾ����1
����:light  :			����ֵ
	 lighten:           ʹ��λ =1��Ч
����:��
*************************************************************************************************************/
/*void LED_DispLight1(unsigned char light,unsigned char lighten)
{
	const unsigned char LightCode[]={0x80,0xc0,0xe0,0xf0,0xf8,0xf9};
	displaychar[0]&=0x06;
	if(lighten)
	{		
		displaychar[0]|=LightCode[light>5?5:light];
	}		
}
*/
/*************************************************************************************************************
������:LED_DispLight2
����:��ʾ����2
����:light  :			����ֵ
	 lighten:           ʹ��λ =1��Ч
����:��
*************************************************************************************************************/
/*void LED_DispLight2(unsigned char light,unsigned char lighten)
{
	const unsigned char LightCode[]={0x80,0x90,0xb0,0xf0};
	displaychar[1]&=0x0f;
	if(lighten)
	{		
		displaychar[1]|=LightCode[light>3?3:light];
	}		
}*/
/*************************************************************************************************************
������:LED_DispMode
����:��ʾ����ģʽ
����:mode   :			ģʽֵ  
	 bit7 �ֶ�
	 bit6 �Զ�
	 bit5 ����
	 bit4 ��ˮ
	 bit3 ��ˮ
	 bit2 ����
	 bit1 ��ʪ
	 bit0 ��ʪ
	 =1��Ч
����:��
*************************************************************************************************************/
void LED_DispMode(unsigned char mode)
{
	const unsigned char AutoManCode[]={0x04,0x02};
	const unsigned char ControlCode[]={0x04,0x02,0x01,0x40,0x20,0x10};
	displaychar[0]&=0xf9;
	displaychar[1]&=0xf8;
	displaychar[2]&=0x8f;	
	if(mode&0x80)displaychar[0]|=AutoManCode[0];
	if(mode&0x40)displaychar[0]|=AutoManCode[1];
	if(mode&0x20)displaychar[1]|=ControlCode[0];
	if(mode&0x10)displaychar[1]|=ControlCode[1];
	if(mode&0x08)displaychar[1]|=ControlCode[2];
	if(mode&0x04)displaychar[2]|=ControlCode[3];
	if(mode&0x02)displaychar[2]|=ControlCode[4];
	if(mode&0x01)displaychar[2]|=ControlCode[5];		
}
/*************************************************************************************************************
������:LED_DispNumeric
����:����������ʾ
����:windows:			��ʾ����,��Χ0-1
	 dispVal:			��ʾ��ֵ
	 dotGlintPos:		С���㶯̬��ʾλ��
	 dotPos:			С������ʾλ��
����:��
*************************************************************************************************************/
void LED_DispNumeric(char windows,int disp_val,unsigned char dotPos)
{
   	unsigned char i=0,j=0;
	unsigned char disptmp=0;
	unsigned int temp=0;
	unsigned char flag=0;
	unsigned char c=0;	
	if(disp_val<0)
	{
		disp_val=-disp_val;	
		if(windows==0)
		{
			disp_val=0;
		}
		else
		{
			if(disp_val>1999)
			{
				disp_val=200;
				dotPos=0;
			}	
			flag=1;//���ű�־
		}
	}
	if(disp_val>999)
	{
		if(windows==0)
		{
			if(disp_val>9999)
			{
				disp_val+=50;
				disp_val/=100;
			}
			else
			{
				disp_val+=5;
				disp_val/=10;
			}
			dotPos=0;
		}
		else
		{			
			if(disp_val>9999)
			{
				disp_val+=5;
				disp_val/=10;
				dotPos=0;
			}					
		}		
		flag|=2;		
	}
	temp=1000;
	for(i=0;i<4;i++)
	{ 	
		j=disp_val/temp;
		disp_val%=temp;
		temp/=10;
		disptmp=shuzi[j];
		if(flag&0x01 && disptmp==shuzi[0] && dotPos==3 && i==0 ){disptmp=0;c=1;}
		if(j)c=1;
		if((c==0)&&(i<3-dotPos)&&(!(flag&0x02)))disptmp=0;
		if((i==3-dotPos)&&(dotPos))disptmp|=LED_DP;
		if((i==0)&&(flag&0x01))disptmp|=LED_G;			
		displaychar[LedIndex[i+windows*4]]&=0x0f;
		displaychar[LedIndex[i+windows*4]]|=disptmp<<4;	
		displaychar[LedIndex[i+windows*4]-1]&=0xf0;
		displaychar[LedIndex[i+windows*4]-1]|=disptmp>>4;	
	}
}

/*************************************************************************************************************
������:LED_DispChar
����:�����ַ���ʾ
����:windows:			��ʾ����,��Χ0-1
	 pStrBuf:			��ʾ�ַ���ָ��
	 dotGlintPos:		С���㶯̬��ʾλ��
	 dotPos:			С������ʾλ��	 
����:��
*************************************************************************************************************/
void LED_DispChar(char windows,char *pStrBuf,unsigned char dotPos)
{
	unsigned char i=0;
	unsigned char charled=0;
	unsigned char disptmp=0;		
	for(i=0;i<4;i++)
	{			
		charled=*(pStrBuf+i);
		if((charled>=0x30)&&(charled<=0x39))			 //����
		{
			disptmp=shuzi[charled-0x30];
		}
	//	else if((charled>=0x61)&&(charled<=0x7a))		 //Сд��ĸ
	//	{
	//		disptmp=zimu[charled-0x61];
	//	}
		else if((charled>=0x41)&&(charled<=0x5a))		 //��д��ĸ
		{
			disptmp=zimu[charled-0x41]; 
		}	
		else if(charled==0x2d)//-��
		{
			disptmp=LED_G;	
		}
		else
		{
			disptmp=0x0;
		}
		if((dotPos)&&(3-dotPos==i))disptmp|=LED_DP;//С����λ��	
		displaychar[LedIndex[i+windows*4]]&=0x0f;
		displaychar[LedIndex[i+windows*4]]|=disptmp<<4;
		displaychar[LedIndex[i+windows*4]-1]&=0xf0;
		displaychar[LedIndex[i+windows*4]-1]|=disptmp>>4;
	}
}




