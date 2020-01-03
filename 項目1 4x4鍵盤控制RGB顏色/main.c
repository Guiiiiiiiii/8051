#include <reg52.h>
#include "LCM.h"
#include "delay.h"
#include "type.h"
/* PWM �ϥ��ܼ� */
sbit RED   = P3^3;
sbit GREEN = P3^4;
sbit BLUE  = P3^5;

bit flag = 0;
u8 pwm = 1;
u8 LEDColor;
u16 on, off;

/* LCM �ϥ��ܼ� */
char code dispColor[][6] = {{"Red   "}, {"Green "}, {"Blue  "},
{"Purple"}, {"Yellow"}, {"Cyan  "}, {"White "}, {"Black "}};
char code line1[] = "PWM&LCD Testing.";

/* ��L�ϥ��ܼ� */
#define KEYP		P2
char code key[16] = { 0, 10, 11, 15,		// F, E, D, C		4*4��L�W���Ʀr
											1, 2, 3, 14,			// B, 3, 6, 9
											4, 5, 6, 13,			// A, 2, 5, 8
											7, 8, 9, 12 };		// 0, 1, 4, 7
u8 scanKey[] = {0xef, 0xdf, 0xbf, 0x7f};
u8 mode = 0;
u8 g_key;
u8 keyIn[2];

/* PWM Freq: 500hz */
void PWM_Init(void)
{
	IE = 0x82;	// �ҥ�Timer0 ���_
	TMOD = 0x01;
	TH0 = 0xf8;
	TL0 = 0x30;
	TR0 = 1;
}
/* �]�wPWM �u�@�g�� */
void PWM_setDuty(u16 duty)
{
	u16 cnt;
	
	cnt = duty * 20;
	on  = (65536 - (cnt));
	off = (65536 - (2000 - cnt));
}
/* RGB�զ� */
void RGB_Tunings(void) interrupt 1
{
	TR0 = 0;
	
	flag = !flag;
	if (!flag) {
		TH0 = on/256;
		TL0 = on%256;
		RED = GREEN = BLUE = 1;
	} else {
		TH0 = off/256;
		TL0 = off%256;	
		// �]�w RGB PWM
		if (LEDColor == 0) 			{RED = 0; GREEN = 1; BLUE = 1;}
		else if (LEDColor == 1) {RED = 1; GREEN = 0; BLUE = 1;}
		else if (LEDColor == 2) {RED = 1; GREEN = 1; BLUE = 0;}
		else if (LEDColor == 3) {RED = 0; GREEN = 1; BLUE = 0;}
		else if (LEDColor == 4) {RED = 0; GREEN = 0; BLUE = 1;}
		else if (LEDColor == 5) {RED = 1; GREEN = 0; BLUE = 0;}
		else if (LEDColor == 6) {RED = 0; GREEN = 0; BLUE = 0;}
		else if (LEDColor == 7) {RED = 1; GREEN = 1; BLUE = 1;}
	}
	
	TR0 = 1;
}

/* Ū����L�� */
void KeyScan(void)
{
	u8 col, row;
	u8 rowkey, kcode;
	
	g_key = 16;	// �M���W�@�������
	for (col=0;col<4;col++) {
		KEYP = scanKey[col];
		rowkey = ~KEYP & 0x0f;
		if (rowkey != 0) {
			if (rowkey == 0x01) row = 0;
			else if (rowkey == 0x02) row = 1;
			else if (rowkey == 0x04) row = 2;
			else if (rowkey == 0x08) row = 3;
			kcode = 4*col + row;
			g_key = key[kcode];	// �N������ഫ��4*4��L�W���Ʀr
			while (rowkey != 0)
				rowkey = ~KEYP & 0x0f;
		}
		Delay1ms(1);
	}
}
/* �]�w��L�\�� */
void KeyLCM(void)
{
	u8 i;
	
	/* ���ݷs������� */
	if (g_key < 16) {
		if ((mode == 0) && (g_key < 8)) {
			LEDColor = g_key;											// ���LED�C��[0:7]
			write_inst(0xc0);
			write_char(0x30+LEDColor);						// ����C�⸹�X
			write_inst(0xc3);
			for (i=0;i<6;i++)
				write_char(' ');
			write_inst(0xc3);
			for (i=0;i<6;i++)
				write_char(dispColor[LEDColor][i]);	// ��ܥثe�C��
		}
		/* ����\��			�y�z
		-------------------------------------
			 ����A			mode = 0:���i����
									mode = 1:�Q��ƿ�J
									mode = 2:�Ӧ�ƿ�J
									mode = 3:���ݧ��
		   ����B			�T�w���
		   ����C			��10%LED�G��
		   ����D			�[10%LED�G��
		   ����F			�M��PWM��
		*/
		switch (g_key) {
			case 10: 
				mode = 1;
			case 11:
				if (mode == 3) {
					pwm = keyIn[0]*10 + keyIn[1];
					mode = 0;
				}
				break;
			case 12:
				if (mode == 0) pwm = (pwm<=9)?  1:pwm-10;
			break;
			case 13:
				if (mode == 0) pwm = (pwm>=89)? 99:pwm+10;
			break;
			case 15:
				keyIn[0] = 0;
				keyIn[1] = 1;
				pwm = 1;
			break;
		}
		if ((mode != 0) && (mode != 3) && (g_key < 10)) {
			keyIn[mode-1] = g_key;
			mode++;
		}
		PWM_setDuty(pwm);	// �]�wRGB�G��
	}
}
/*************************** Main Loop **********************************/
main()
{
	u8 i;
	
	init_LCM();
	PWM_Init();
	RED = GREEN = BLUE = 1;
	write_inst(0x80);
	// �]�wLCM�Ĥ@���r
	for (i=0;i<16;i++)
		write_char(line1[i]);
	write_inst(0xc0);
	while(1)
	{
		KeyScan();
		KeyLCM();
		/*	mode>0�A��ܥثe��J��
				mode=0�A��ܥثe�G��		*/
		if (mode != 0) {
			write_inst(0xca);
			write_char(0x30+keyIn[0]);
			write_char(0x30+keyIn[1]);
			write_char('%');
		} else {
			write_inst(0xca);
			write_char(0x30+(pwm/10));
			write_char(0x30+(pwm%10));
			write_char('%');
		}
	}
}


