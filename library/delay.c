#include "delay.h"

void Delay1ms(u16 x)
{
	while (x) {
		Delay10us(112);
		x--;
	}
}
/* ����1���ɶ�8-9us��10us */
void Delay10us(u16 x)
{
	while (x) {
		x--;
	}
}