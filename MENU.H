
#ifndef _MENU_
#define _MENU_

typedef  struct {
	unsigned char curState;						//��ǰ���е�״̬,bit8=1�Ӳ����˵��˻ص������˵�,bit0-bit7:=1����״̬,=2����״̬
	unsigned char parType;						//��x�����
	unsigned char parChg;							//������
	unsigned int  parTime;						//
}_menuInfo;

extern volatile _menuInfo menuInfo;

/**************************************************************
	�˵���ʱ��

**************************************************************/


/**************************************************************

**************************************************************/
extern void Menu_KeyOnHandle(void);
extern void Menu_ExitPar(void);
extern void Menu_Init(void);

#endif