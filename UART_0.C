#include "define.h"



bank2 volatile unsigned char uartinitflag=2;
bank2 volatile unsigned char getover=0;
bank2 volatile unsigned char rxtimer=0;
bank2 volatile unsigned char sendcount=0;
bank2 volatile unsigned char getcount=0;
bank2 volatile unsigned char sendmax=0;
bank2 volatile unsigned char waittime=0;
bank3 volatile unsigned char getdata[]={0,0,0,0,0,0,0,0};
bank3 volatile unsigned char senddata[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};


const int baudcode[]={300,600,1200,2400,4800,9600,14400,19200,38400};
const unsigned char  waitcode[]={35,19,10,6,3,2,2,2,2};

void Uart0_Init(void)        //��ʱ����USART�ĳ�ʼ�� 
{
	unsigned int baud=0;	
	unsigned char i=0;	
	baud=configItem.baud;	
	if(baud<300)
	{
		waittime=35;
	}
	else if(baud>38400)
	{
		waittime=2;
	}
	else
	{
		for(i=0;i<8;i++)
		{
			if(baud>=baudcode[i] && baud<baudcode[i+1])
			{
				waittime=waitcode[i];
				break;
			}
		}
	}
	baud/=100;
	baud=27648/baud-1;
	/*RCSTA�Ĵ���*/	
	SPEN=0;//����ʹ��λ��λ 
	RX9=0;//8λ��������
	CREN=1;//��������

	
	/*TXSTA�Ĵ���*/
	TX9=0;//8λ��������
	TXEN=0;//��ֹ����
	SYNC=0;//ѡ���첽��ʽ
	BRGH=1;//���ٲ�����
	TRMT=0;//������λ�Ĵ�����0  


	
	BRG16=1;
	RCIE=1;//�����ж�ʹ��λ��λ
	TXIE=0;//�����ж�ʹ��λ��λ
	SPBRG=(unsigned char)baud;//OSC/16/BAUD-1  �Ǹ��ٲ�������ΪOSC/64/BAUD-1
    SPBRGH=(unsigned char)(baud>>8);
	FERR=0;
	OERR=0;
	SDIEN_OUT;
	SDI_RECEIVE;//RS485���շ��� 
	SPEN=1;//����ʹ��λ��λ  
}


/********************************************************************************************************
������:void Uart0_Handle
����:��鴮���Ƿ�������  �����������ݴ���
����:��
����:״̬  =1�ɹ�
********************************************************************************************************/
void Uart0_Handle(void)
{
	unsigned int inttmp1=0,inttmp2=0;
	unsigned int me_crc=0,it_crc=0;
	unsigned char cop=0;
	unsigned int meteraddr=0,meterlong=0;
	unsigned char partype=0,parchg=0;	
	unsigned char i=0,j=0;	
	if(getcount!=8 && getcount!=3)
	{getcount=0;return;}
	if(getcount==8)cop=1;
	getcount=0;		
	if(getdata[0]!=configItem.addr)
	{
		if(getdata[1]!=WRITE || cop!=1 || getdata[0]!=0)
		return;	
	}
	if(cop)
	{
		if((getdata[1]!=READ)&&(getdata[1]!=READONLY)&&(getdata[1]!=WRITE))return;
		me_crc=modbusCRC((unsigned char *)getdata,6);	
		it_crc=getdata[7]<<8|getdata[6];
		if(it_crc!=me_crc)return;
		meteraddr=getdata[2]<<8|getdata[3];
		meterlong=getdata[4]<<8|getdata[5];
		partype=meteraddr/ADDR_PAR_COUNT;
		parchg=meteraddr%ADDR_PAR_COUNT;
		switch(getdata[1])
		{			
			case READONLY://������Ĵ���
				if(meterlong==0)return;
				if((meteraddr+meterlong)>CH_MAX)return;
				for(i=0;i<meterlong;i++)
				{
					senddata[3+i*2]=(unsigned char)(adInfo.pv[meteraddr+i]>>8);
					senddata[4+i*2]=(unsigned char)(adInfo.pv[meteraddr+i]);
				}			
				break; 		
/*
������ʾ�� ��ַ����
addr	0x00
baud	0x01
tsc		0x02
hsc		0x03
tbl		0x00+ADDR_PAR_COUNT
tbh		0x01+ADDR_PAR_COUNT
tha		0x02+ADDR_PAR_COUNT
tyh		0x03+ADDR_PAR_COUNT
tla		0x04+ADDR_PAR_COUNT
tyl		0x05+ADDR_PAR_COUNT

hbl		
hbh		
hha		0x06+ADDR_PAR_COUNT
hyh		0x07+ADDR_PAR_COUNT
hla		0x08+ADDR_PAR_COUNT
hyl		0x09+ADDR_PAR_COUNT

t-4		0x00+ADDR_PAR_COUNT+ADDR_PAR_COUNT
t-20	0x01+ADDR_PAR_COUNT+ADDR_PAR_COUNT
h-4		0x02+ADDR_PAR_COUNT+ADDR_PAR_COUNT
h-20	0x03+ADDR_PAR_COUNT+ADDR_PAR_COUNT
jzflag  0x04+ADDR_PAR_COUNT+ADDR_PAR_COUNT
outflag 0x05+ADDR_PAR_COUNT+ADDR_PAR_COUNT
year	0x00+ADDR_PAR_COUNT+ADDR_PAR_COUNT+ADDR_PAR_COUNT
mon		0x01+ADDR_PAR_COUNT+ADDR_PAR_COUNT+ADDR_PAR_COUNT
day		0x02+ADDR_PAR_COUNT+ADDR_PAR_COUNT+ADDR_PAR_COUNT
*/
			case READ:			//modbusЭ��ָ��   �����ּĴ���
				if(meteraddr>=4*ADDR_PAR_COUNT)return;
				if(meterlong==0)return;		
				if(partype==0)
				{
					if(parchg+meterlong>6)return;
				}
				else if(partype==1)
				{
					if(parchg+meterlong>10)return;
				}
				else if(partype==2)
				{
					if(parchg+meterlong>6)return;
				}
				else if(partype==3)
				{
					if(parchg+meterlong>3)return;
				}
				else
				{return;}				
				for(i=0;i<meterlong;i++)
				{		
					if(partype==0)
					{
						if(parchg+i==ADDR_ADDR || parchg+i==BAUD_ADDR)//��ַ�Ͳ�����
						{
							inttmp1=1;
							inttmp2=(parchg+i);
						}
						else if(parchg+i==2)//��һ·����
						{
							inttmp1=2;
							inttmp2=0;
						}
                        else if(parchg+i==4) //�ɼ�Ƶ��
						{
							inttmp1=5;
							inttmp2=3;
						}
                        else if(parchg+i==5) //�汾��
						{
							inttmp1=5;
							inttmp2=4;
						}
						else				//�ڶ�·����
						{
							inttmp1=3;
							inttmp2=0;
						}
					}
					else if(partype==1)
					{
						if(parchg+i>=6)//ʪ�Ȳ���
						{		
							inttmp1=3;					
							inttmp2=parchg+i-6+humihidecount+1;//humididecount=2
						}
						else//�¶Ȳ���
						{
							inttmp1=2;
							inttmp2=parchg+i+1;
						}
					}
					else //partype=2,�¶ȱ��͵Ͷˣ��¶ȱ��͸߶ˣ�ʪ�ȱ��͵Ͷˣ�ʪ�ȱ��͸߶�,jzflag,outflag
					{	 //partype=3,��������������
						inttmp1=partype+2;
						inttmp2=parchg+i;
					}
                    I2_Read(menu_par_ee_addr[inttmp1]+inttmp2*2,(unsigned char *)&senddata[3+i*2],2);
                    j=senddata[3+i*2];
                    senddata[3+i*2]=senddata[4+i*2];
                    senddata[4+i*2]=j;

				//I2_Read(menu_par_ee_addr[inttmp1]+inttmp2*2,(unsigned char *)&senddata[4+i*2],1);
				//	I2_Read(menu_par_ee_addr[inttmp1]+inttmp2*2+1,(unsigned char *)&senddata[3+i*2],1);
				}			
				break;
			case WRITE:			
				if(meteraddr>=4*ADDR_PAR_COUNT)return;
				if(partype==0)
				{
					if(parchg>6)return;//�޸�
				}
				else if(partype==1)
				{
					if(parchg>=10)return;
				}
				else if(partype==2)
				{
					if(parchg>=6)return;
				}
				else if(partype==3)
				{
					if(parchg>=3)return;
				}	
				if(partype==0)
				{
					if(parchg==ADDR_ADDR || parchg==BAUD_ADDR)
					{
						if(parchg==BAUD_ADDR)
						{
							for(i=0;i<8;i++)
							{
								if(meterlong==baudcode[i])break;
							}
							if(i==8)return;
						}
						else if(parchg==ADDR_ADDR)
						{
							if(meterlong>255||meterlong<1) return;//�����ַ������Χ�����޸�
						}
						inttmp1=1;
						inttmp2=parchg;
						uartinitflag=1;
						*(&configItem.addr+parchg)=meterlong;	
					}
                   else if(parchg==4)//�޸Ĳɼ�ʱ��
                   {
                         adcoltime= meterlong;
                         I2_Write(menu_par_ee_addr[collectcount]+6,(unsigned char *)&adcoltime,2);
                   }
					else if(parchg==5) return;//������޸İ汾�ţ��˳�
					else if(parchg==2||parchg==3)//�޸�����
					{
						inttmp1=parchg;
						inttmp2=0;
						*(&parItem[parchg%2].sc)=meterlong;
					}
				}
				else if(partype==1)
				{
					if(parchg>=6)//ʪ�Ȳ���
					{
						inttmp1=3;
						inttmp2=parchg;
						inttmp2+=-6+humihidecount+1;						
						*(&parItem[1].sc+inttmp2)=meterlong;
					}
					else//�¶Ȳ���
					{
						inttmp1=2;
						inttmp2=parchg+1;
						*(&parItem[0].sc+inttmp2)=meterlong;
					}
				}
				else//partype=2,partype=3
				{
					inttmp1=partype+2;
					inttmp2=parchg;
					if(partype==2)
					{		
						if(parchg>=4)
						{
							*(&configItem.jzflag+parchg-4)=meterlong;
						}
						else
						{
							*(&parItem[(parchg)/2].bslvalue+(parchg)%2)=meterlong;	
						}
					}
					else if(partype==3)	{return;} //�������޸���������	
				}
				I2_Write(menu_par_ee_addr[inttmp1]+inttmp2*2,(unsigned char *)&meterlong,2);			
				break;
		}	
		if(getdata[0]==0)return;//�㲥��ַ����������
		senddata[0]=getdata[0];
		senddata[1]=getdata[1];	
		if(getdata[1]==READ || getdata[1]==READONLY)
		{			
			senddata[2]=meterlong*2;
			me_crc=modbusCRC((unsigned char *)senddata,3+senddata[2]);
			senddata[3+senddata[2]]=(unsigned char)me_crc;
			senddata[4+senddata[2]]=(unsigned char)(me_crc>>8);	
			sendmax=5+senddata[2];	
		}
		else	
		{
			for(i=2;i<8;i++)
			{
				senddata[i]=getdata[i];
			}
			sendmax=8;
		}
	}
	sendcount=0;	
	CREN=0;	
	RCIE=0;
	SDI_SEND;		
	TXEN=1;	
	TXREG=senddata[sendcount];	
	TXIE=1;	
}
/********************************************************************************************************
������:modbusCRC
����:��ȡMODBUSЭ��16λCRCУ��
����:daBuf�������   len����
����:CRC16
********************************************************************************************************/
unsigned int modbusCRC(unsigned char *daBuf,unsigned char len)
{
   unsigned char BitFg=0;
   unsigned char i=0,j=0;
   unsigned int CRCBuf = 0xffff;
   for(j=0;j<len;j++)
   {
      CRCBuf ^= daBuf[j];
      for(i=0;i<8;i++)
      {
         BitFg = CRCBuf&1;
         CRCBuf >>= 1;        
         if(BitFg==1)CRCBuf ^= 0xa001;
      }
    }   
	return CRCBuf;
}