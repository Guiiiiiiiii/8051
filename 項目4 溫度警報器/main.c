#include <reg52.h>
#include <stdio.h>
#include "LCM.h"
#include "delay.h"
#include "type.h"
sbit buzzer = P3^7;		// 蜂鳴器

/* TC77 使用變數 */
sbit CK 		= P3^3;		// TC77脈波輸出位元	SCK
sbit DT 		= P3^4;		// TC77資料輸入位元	SIO
sbit CS 		= P3^5;		// TC77晶片選擇位元	CS

u8 setT = 29;
u16 temp;
u16 tempC;						// 攝氏溫度
u16 tempF;						// 華氏溫度

/* 鍵盤使用變數 */
#define KEYP		P2
char code key[16] = { 0, 10, 11, 15,		// F, E, D, C		4*4鍵盤上的數字
											1, 2, 3, 14,			// B, 3, 6, 9
											4, 5, 6, 13,			// A, 2, 5, 8
											7, 8, 9, 12 };		// 0, 1, 4, 7
u8 scanKey[]={ 0xef, 0xdf ,0xbf ,0x7f };
u8 g_key;
u8 keyIn[2];
u8 n = 0;
bit mode = 0;

/* 蜂鳴器副程式 */
void Beep(u8 x)
{
	u8 i, j;
	
	for (i=0;i<x;i++) {
		for (j=0;j<100;j++) {
			buzzer = 0; Delay10us(50);
			buzzer = 1; Delay10us(50);
		}
		Delay1ms(100);
	}
}

/* 讀取1byte資料 */
u8 ShiftIn(void)
{
	u8 dat = 0;
	u8 i, j;
	
	DT = 1;
	for (i=0;i<8;i++) {
		CK = 1;
		j = DT;
		dat |= (j<<(7-i));
		CK = 0;
	}
	
	return dat;
}
/* 讀取TC77溫度 */
void TC77_GetData(void)
{
	u8 dat[2];
	
	CS = 0;
	dat[0] = ShiftIn();								// 高8位元
	dat[1] = ShiftIn();								// 低8位元
	CS = 1;
	temp  = (dat[0]<<8) | dat[1];
	tempC = ((temp>>3)*6.25);					// 放大100倍處理小數點
	tempF = tempC*1.8+3200;						// 攝氏轉華氏
	if (tempC > (setT*100)) Beep(2);	// 放大100倍處理小數點
	Delay1ms(50);
}

/* 顯示目前/設定溫度畫面 */
void Display(void)
{
	char line1[16];
	char line2[16];
	u8 i;
	
	n = 0;					// 清除上次輸入位置
	//							0123456789abcdef
	//							NOW_T= 12.34 °C
	//							TS=29  12.34 °F
	sprintf(line1, "NOW_T= %.2f °C", tempC/100.0);
	sprintf(line2, "TS=%bu  %.2f °F", setT, tempF/100.0);
	
	write_inst(0x80);
	for (i=0;i<16;i++)
		write_char(line1[i]);
	for (i=0;i<16;i++)
		write_char(line2[i]);
}
/* 讀取鍵盤值 */
void KeyScan(void)
{
	u8 col, row;
	u8 rowkey, kcode;
	
	g_key = 16;				// 清除上一筆按鍵值
	for (col=0;col<4;col++) {
		KEYP = scanKey[col];
		rowkey = ~KEYP & 0x0f;
		if (rowkey != 0) {
			if (rowkey == 0x01) row = 0;
			else if (rowkey == 0x02) row = 1;
			else if (rowkey == 0x04) row = 2;
			else if (rowkey == 0x08) row = 3;
			kcode = 4 * col+row;
			g_key = key[kcode];	// 將按鍵值 轉換成 4*4鍵盤上的數字
			Beep(1);
			while (rowkey != 0)
				rowkey = ~KEYP & 0x0f;
		}
		Delay1ms(1);
	}
}
/* 設定鍵盤功能 */
void KeyLCM(void)
{
	//									 0123456789abcdef
	char code line1[] = "TEMP UPPER SET  ";
	char code line2[] = "     TS=   °C   ";
	u8 i;

	if (!mode) TC77_GetData();
	// 等待新的按鍵值
	if (g_key < 16) {
		if ((mode != 0) && (g_key < 10)) {
			write_inst(0xc8+n);					// 移動游標位置
			keyIn[n] = g_key;
			write_char(0x30+keyIn[n]);	// 顯示目前輸入值
			n = 1;											// n = 0:十位數輸入
																	// n = 1:個位數輸入
		}
		/* 按鍵功能			描述
		   -------------------------------------
			 按鍵A			mode = 0:不進行更改
									mode = 1:進行設定溫度
		   按鍵B			確定更改
		*/
		if (g_key == 10) {
			mode = 1;								// 進入設定溫度模式
			write_inst(0x01);				// 清除螢幕
			write_inst(0x80);
			for (i=0;i<16;i++)
				write_char(line1[i]);	// 設定第一行文字
			for (i=0;i<16;i++)
				write_char(line2[i]); // 設定第二行文字
		} else if (g_key == 11) {
			setT = keyIn[0]*10 + keyIn[1];
			mode = 0;								// 離開設定模式
			write_inst(0x01);				// 清除螢幕
			Display();
		}
	}
}

/*************************** Main Loop **********************************/
main()
{
	init_LCM();
	Display();
	while(1)
	{
		KeyScan();
		KeyLCM();
	}
}

