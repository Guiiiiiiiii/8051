#include <reg52.h>
#include "type.h"
sbit DHT = P2^0;

static void Delay20ms(void)
{
	TH0 = 0xB1;
	TL0 = 0xE0;
	TR0 = 1;
	while (!TF0);
	TR0 = 0;
	TF0 = 0;
}

static void Delay30us(void)
{
	TH0 = 0xFF;
	TL0 = 0xE2;
	TR0 = 1;
	while (!TF0);
	TR0 = 0;
	TF0 = 0;
}

static void DHT_Start(void)
{
	// 發出啟始信號
	DHT = 0;
	Delay20ms();
	DHT = 1;
	// 接收DHT信號
	while (DHT == 1);		// 等待DHT回應
	while (DHT == 0);		// 用while迴圈跳過80us響應時間
	while (DHT == 1);		// 用while迴圈跳過80us響應時間
}
/*     ___
 *  __|	  |__     26-28us的HIGH時間，表示資料為0
 *     _______
 *  __|	      |__ 70us的HIGH時間，表示資料為1
 * 
 * 只要延時超過28us且小於70us，就可知道資料為多少
 */
static u8 DHT_Read(void)
{
	u8 i;
	u8 dat = 0;
	
	for(i = 0; i < 8; i++) {
		while (DHT == 0);	// 信號變1時，開始接收1位元資料
		Delay30us();
		if (DHT == 1)
			dat = (dat<<1) | 0x01;
		else
			dat = (dat<<1);
		while (DHT == 1);	// 信號變0時，準備接收下1位元資料
	}
	
	return dat;
}

void DHT_GetData(u8 *h, u8 *t, u8 *chk)
{
	u8 dat[5];
	u8 i;
	
	DHT_Start();
	for (i=0;i<5;i++) dat[i] = DHT_Read();
	*h   = dat[0] + dat[1];
	*t   = dat[2] + dat[3];
	*chk = dat[4];
}
