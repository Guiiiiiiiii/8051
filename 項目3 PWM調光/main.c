#include <reg52.h>
#include "delay.h"
#include "type.h"

/* 7段顯示器使用的變數 */
#define SEG7P		P2
#define SCANP		P1
char code TAB[]  = {0xc0, 0xf9, 0xa4, 0xb0, 0x99,	// 0-4
										0x92, 0x82, 0xf8, 0x80, 0x98};	// 5-9
u8 scan[3] = {0x04, 0x02, 0x01};
u8 disp[3];
sbit PB1 = P3^2;
sbit PB2 = P3^3;

/* PWM 使用的變數 */
sbit LED = P0^7;
u16 on, off;
u16 pwm = 0;

/* 設定外部中斷、PWM */
void ISR_Init(void)
{
	IE = 0x87;	// 啟用 INT0,INT1, TF0
	TCON = 0x00;	// INT0, INT1採低態動作
	TMOD = 0x01;
	TH0 = 0xf8;
	TL0 = 0x30;
	TR0 = 1;
}
/* 設定PWM工作周期 */
void SetDuty(u16 count)
{
	u8 i;
	/* 顯示目前PWM */
	disp[0] = TAB[(pwm/100)%10];
	disp[1] = TAB[(pwm/10)%10];
	disp[2] = TAB[pwm%10];
	for (i=0;i<3;i++) {
		SCANP = scan[i];
		SEG7P = disp[i];
		Delay1ms(4);
	}
	
	if (pwm == 100) {
		TR0 = 0;
		LED = 0;
	} else if (pwm == 0) {
		TR0 = 0;
		LED = 1;
	} else {
		TR0 = 1;
		on 	= (65536 - count);
		off = (65536 - (2000-count));
	}
}
/* PWM 中斷副程式 */
void TIM0_PWM(void) interrupt 1
{
	if (LED) {
		// 填入On時間的計數量
		TR0 = 0;
		TH0 = on/256;
		TL0 = on%256;
		TR0 = 1;
		LED = 0;
	} else {
		// 填入Off時間的計數量
		TR0 = 0;
		TH0 = off/256;
		TL0 = off%256;
		TR0 = 1;
		LED = 1;
	}
}
/* 防彈跳20ms */
void Debouncer(void)
{
	u16 i;
	for (i=0;i<2400;i++);
}
/* 減少10%LED亮度 */
void INT0_ISR(void) interrupt 0
{
	pwm = (pwm>0)? pwm-10:0;
	while (!PB1);
	Debouncer();
}
/* 增加10%LEDPWM */
void INT1_ISR(void) interrupt 2
{
	pwm = (pwm<100)? pwm+10:100;
	while (!PB2);
	Debouncer();
}
/*************************** Main Loop **********************************/
main()
{
	ISR_Init();
	LED = 0;
	while(1)
	{
		SetDuty(pwm * 20);
	}
}
