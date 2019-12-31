#include <reg52.h>
#include "type.h"
sbit LED1 = P0^5;
sbit LED2 = P0^6;
sbit LED3 = P0^7;
typedef struct {
	u16 on;
	u16 off;
} PWM;
PWM pwm1, pwm2, pwm3;

void PWM_Init(void)
{
	// EA = ET2 = ET1 = ET0 = 1
	IE = 0xAA;
	TMOD = 0x11;
	// TIM 0 Mode 1
	TH0 = 0xF8;
	TL0 = 0x30;
	TR0 = 1;
	// TIM 1 Mode 1
	TH1 = 0xF8;
	TL1 = 0x30;
	TR1 = 1;
	// TIM 2 Mode 16位元自動載入
	CP_RL2 = 0;
	EXEN2 = 1;
	TH2 = 0xF8;
	TL2 = 0x30;
	TR2 = 1;
}
// 設定3個Channel工作週期副程式
void SetDuty(PWM *pwm, u16 count)
{
	pwm->on  = (65536 - count);
	pwm->off = (65536 - (2000-count));
}
// 設定TIM 0,1,2為500hz PWM
void TIM0_PWM(void) interrupt 1
{
	TR0 = 0;
	if (LED1) {
		// 填入TIM 0 On時間的計數量
		TH0 = pwm1.on/256;
		TL0 = pwm1.on%256;
		LED1 = 0;
	} else {
		// 填入TIM 0 Off時間的計數量
		TH0 = pwm1.off/256;
		TL0 = pwm1.off%256;
		LED1 = 1;
	}
	TR0 = 1;
}

void TIM1_PWM(void) interrupt 3
{
	TR1 = 1;
	if (LED2) {
		// 填入TIM 1 On時間的計數量
		TH1 = pwm2.on/256;
		TL1 = pwm2.on%256;
		LED2 = 0;
	} else {
		// 填入TIM 1 Off時間的計數量
		TH1 = pwm2.off/256;
		TL1 = pwm2.off%256;
		LED2 = 1;
	}
	TR1 = 1;
}

void TIM2_PWM(void) interrupt 5
{
	TF2 = 0;	// 手動清除
	TR2 = 0;
	if (LED3) {
		// 填入TIM 2 On時間的計數量
		TH2 = pwm3.on/256;
		TL2 = pwm3.on%256;
		LED3 = 0;
	} else {
		// 填入TIM 2 Off時間的計數量
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
