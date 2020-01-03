#include <reg52.h>
#include "type.h"

/* PWM 使用變數 */
sbit LED1 = P0^5;
sbit LED2 = P0^6;
sbit LED3 = P0^7;
typedef struct {
	u16 on;
	u16 off;
} PWM;
PWM pwm1, pwm2, pwm3;

/* PWM Freq: 500hz */
void PWM_Init(void)
{
	/* 啟用TMR 中斷 */
	IE = 0xAA;
	TMOD = 0x11;
	/* TMR 0 */
	TH0 = 0xF8;
	TL0 = 0x30;
	TR0 = 1;
	/* TMR 1 */
	TH1 = 0xF8;
	TL1 = 0x30;
	TR1 = 1;
	/* TMR 2 */
	CP_RL2 = 0;
	EXEN2 = 1;
	TH2 = 0xF8;
	TL2 = 0x30;
	TR2 = 1;
}

/* 設定3路PWM的工作周期 */
void SetDuty(PWM *pwm, u16 count)
{
	pwm->on  = (65536 - count);
	pwm->off = (65536 - (2000-count));
}

/* TMR0 PWM */
void TMR0_PWM(void) interrupt 1
{
	TR0 = 0;
	if (LED1) {
		// 填入TMR0 On時間的計數量
		TH0 = pwm1.on/256;
		TL0 = pwm1.on%256;
		LED1 = 0;
	} else {
		// 填入TMR0 Off時間的計數量
		TH0 = pwm1.off/256;
		TL0 = pwm1.off%256;
		LED1 = 1;
	}
	TR0 = 1;
}

/* TMR1 PWM */
void TMR1_PWM(void) interrupt 3
{
	TR1 = 1;
	if (LED2) {
		// 填入TMR1 On時間的計數量
		TH1 = pwm2.on/256;
		TL1 = pwm2.on%256;
		LED2 = 0;
	} else {
		// 填入TMR1 Off時間的計數量
		TH1 = pwm2.off/256;
		TL1 = pwm2.off%256;
		LED2 = 1;
	}
	TR1 = 1;
}

/* TMR2 PWM */
void TIM2_PWM(void) interrupt 5
{
	TF2 = 0;	// 手動清除
	TR2 = 0;
	if (LED3) {
		// 填入TMR2 On時間的計數量
		TH2 = pwm3.on/256;
		TL2 = pwm3.on%256;
		LED3 = 0;
	} else {
		// 填入TMR2 Off時間的計數量
		TH2 = pwm3.off/256;
		TL2 = pwm3.off%256;
		LED3 = 1;
	}
	TR2 = 1;
}
/*************************** Main Loop **********************************/
main()
{
	PWM_Init();
	while(1)
	{
		SetDuty(&pwm1, 200);	// 10% duty cycle
		SetDuty(&pwm2, 1000);	// 50% duty cycle
		SetDuty(&pwm3, 1800);	// 90% duty cycle
	}
}
