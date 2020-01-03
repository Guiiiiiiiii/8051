#include <reg52.h>
#include <stdio.h>
#include "delay.h"
#include "type.h"
/* DHT副程式 */
extern void DHT_Start(void);
extern u8 DHT_Read(void);
/* 設定UART鮑率為9600bps */
void UART_Init(void)
{
	TMOD = 0x21;
	PCON&= 0x7f;
	TH1 = TL1 = 0xFD;
	TR1 = 1;
	SCON = 0x50;
	TI = 1;
}
/*************************** Main Loop **********************************/
main()
{
	UART_Init();
	while(1)
	{
		int temp, hum, checkSum;
		u8 dat[4];
		u8 i;
		
		DHT_Start();
		/* 讀取5bytes資料
		   濕度整數+濕度小數點
		   溫度整數+溫度小數點
		   驗證碼
		*/
		for (i=0;i<4;i++) dat[i] = DHT_Read();
		checkSum = DHT_Read();
		hum  = dat[0] + dat[1];
		temp = dat[2] + dat[3];
		
		if (hum + temp == checkSum) {
			printf("temperature:%d°C\t", temp);
			printf("humidity:%d%%\n", hum);
		} else {
			printf("Read Error\n");
		}
		Delay1ms(1000);
	}
}
