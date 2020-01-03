#include "delay.h"

void Delay1ms(u16 x)
{
	while (x) {
		Delay10us(112);
		x--;
	}
}
/* 執行1次時間8-9us約10us */
void Delay10us(u16 x)
{
	while (x) {
		x--;
	}
}