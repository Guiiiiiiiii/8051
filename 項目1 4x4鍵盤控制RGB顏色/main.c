#include <reg52.h>
#include "LCM.h"
#include "delay.h"
#include "type.h"
#define KEYP		P2
// RGB 使用變數
sbit RED   = P3^3;
sbit GREEN = P3^4;
sbit BLUE  = P3^5;
u16 on, off;
u8 pwm = 1;
u8 LEDColor;
bit flag = 0;
char code dispColor[][6] = {{"Red   "}, {"Green "}, {"Blue  "},
{"Purple"}, {"Yellow"}, {"Cyan  "}, {"White "}, {"Black "}};
// LCM 使用變數
char code line1[] = "PWM&LCD Testing.";
// KEYP 使用變數
char code key[16] = { 0, 10, 11, 15,		// F, E, D, C		4*4鍵盤上的數字
		      1, 2, 3, 14,		// B, 2, 6, 9
		      4, 5, 6, 13,		// A, 2, 5, 8
		      7, 8, 9, 12 };		// 0, 1, 4, 7
u8 scanKey[] = {0xef, 0xdf, 0xbf, 0x7f};
u8 mode = 0;
u8 g_key;
u8 keyIn[2];
// RGB Function
void init_PWM(void)
{
	IE = 0x82;	// 啟用TIM0
	TMOD = 0x01;
	TR0 = 1;
}
// 設定工作週期
void SetDuty(u16 count)
{
	u16 cnt;
	
	cnt = count * 20;
	on  = (65536 - (cnt));
	off = (65536 - (2000 - cnt));
}
// RGB調色
void RGB_Toning(void) interrupt 1
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
		
		// 設定 R,G,B PWM
		if (LEDColor == 0) 	{RED = 0; GREEN = 1; BLUE = 1;}
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
// End
//--------------------------------------------------------------------
// 4*4鍵盤 Function
// 讀取鍵盤值
void KeyScan(void)
{
	u8 col, row;
	u8 rowkey, kcode;
	
	g_key = 16;	// 清除上一筆按鍵值
	for (col=0;col<4;col++) {
		KEYP = scanKey[col];
		rowkey = ~KEYP & 0x0f;
		if (rowkey != 0) {
			if (rowkey == 0x01) row = 0;
			else if (rowkey == 0x02) row = 1;
			else if (rowkey == 0x04) row = 2;
			else if (rowkey == 0x08) row = 3;
			kcode = 4*col + row;
			g_key = key[kcode];	// 將按鍵值 轉換成 4*4鍵盤上的數字
			while (rowkey != 0)
				rowkey = ~KEYP & 0x0f;
		}
		Delay1ms(1);
	}
}
// 設定鍵盤功能
void KeyLCM(void)
{
	u8 i;
	
	// 等待新的按鍵值
	if (g_key < 16) {
		if ((mode == 0) && (g_key < 8)) {
			LEDColor = g_key;
			write_inst(0xc0);
			write_char(0x30+LEDColor);			// 顯示第N個顏色
			write_inst(0xc3);
			for (i=0;i<6;i++)
				write_char(' ');
			write_inst(0xc3);
			for (i=0;i<6;i++)
				write_char(dispColor[LEDColor][i]);	// 顯示目前顏色
		}
		/* 按鍵功能			描述
		-------------------------------------
		   按鍵A			設定輸入模式
						0:不進行輸入
						1:十位數輸入
					    	2:個位數輸入
					    	3:等待送出
		   按鍵B			確定鍵
		   按鍵C			PWM減10%
		   按鍵D			PWM加10%
		   按鍵F			清除PWM值
		*/
		switch (g_key) {
			case 10: 
				mode = (mode<3)? mode+1:3; 
			case 11:
				if (mode == 3) {
					pwm = keyIn[0]*10 + keyIn[1];
					mode = 0;
				}
				break;
			case 12:
				if (mode == 0) pwm = (pwm>1)?  pwm-10:1;
			break;
			case 13:
				if (mode == 0) pwm = (pwm<99)? pwm+10:99;
			break;
			case 15:
				keyIn[0] = 0;
				keyIn[1] = 1;
				pwm = 1;
			break;
		}
		if ((mode != 0) && (mode != 3) && (g_key < 10)) {
			keyIn[mode-1] = g_key;
		}
		SetDuty(pwm);
	}
}
// End
/*************************** Main Loop **********************************/
main()
{
	u8 i;
	
	init_LCM();
	init_PWM();
	RED = GREEN = BLUE = 1;
	write_inst(0x80);
	// 設定LCM第一行文字
	for (i=0;i<16;i++)
		write_char(line1[i]);
	write_inst(0xc0);
	while(1)
	{
		KeyScan();
		KeyLCM();
		/*	mode>0，顯示目前正在輸入值
			mode=0，顯示目前PWM值		*/
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


