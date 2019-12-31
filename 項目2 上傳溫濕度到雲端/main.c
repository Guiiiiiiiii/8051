/* 每20秒上傳一筆溫濕度到雲端 */
#include <reg52.h>
#include <stdio.h>
#include "delay.h"
#include "type.h"
// WiFi Function
extern void WiFi_SetNetwork(void);
extern void WiFi_UpdateDHT(u8, u8);
// DHT11 Function
extern void DHT_GetData(u8 *h, u8 *t, u8 *chk);
// 設定UART鮑率為9600bps
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
	WiFi_SetNetwork();
	while(1)
	{
		u8 hum, temp, checkSum;
		static u8 seconds = 0;
		
		DHT_GetData(&hum, &temp, &checkSum);
		if (hum + temp == checkSum && seconds == 20) {
			seconds = 0;
			WiFi_UpdateDHT(hum, temp);
			//printf("humidity:%bu\t", hum);
			//printf("temperature:%bu\n", temp);
		}
		Delay1ms(1000);
		seconds++;
	}
}
