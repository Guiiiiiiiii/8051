/*
	AT指令				描述
	------------------------------------------------
	AT+RST				重新啟動ESP8266	
	AT+CWMODE			設定Wi-Fi模式
					1：Station 				模式
					2：SoftAP 				模式
					3：SoftAP+Station 模式
	AT+CWJAP			連接AP
	AT+CIPMUX			設定 TCP/UDP 連線模式
					0：單一連線				模式
					1：多重連線				模式
	AT+CIPSTART			建立TCP連接，UDP傳輸或者SSL連接
	AT+CIPSEND			發送數據
	AT+CIPCLOSE			關閉TCP/UDP/SSL傳輸
 */
#include <stdio.h>
#include <string.h>
#include "delay.h"
#include "type.h"
// 設定WiFi資料
const char code SSID[] 		= "SHENG";
const char code PASSWORD[] 	= "103403059";
const char code SERVER[]	= "184.106.153.149";	// Thingspeak ip
const char code WRITE_API_KEY[] = "FEZ1ZIIYB1BPM4HQ";	// Thingspeak API Key
// WiFi連線
void WiFi_SetNetwork(void)
{
	printf("AT+RST\n\r");
	// 等待回應
	Delay1ms(2000);
	printf("AT+CWMODE=1\n\r");
	printf("AT+CWJAP=");
	printf("\"%s\",\"%s\"", SSID, PASSWORD);
	printf("\n\r");
}
// 建立TCP連接
void WiFi_UpdateDHT(u8 dat1, u8 dat2)
{
	char idata CmdBuffer[80];
	u8 len;

	printf("AT+CIPMUX=0\n\r");
	
	strcpy(CmdBuffer, "AT+CIPSTART=\"TCP\",\"");
	strcat(CmdBuffer, SERVER);
	strcat(CmdBuffer, "\",80\n\r");
	printf(CmdBuffer);
	// 等待回應
	Delay1ms(1000);
	sprintf(CmdBuffer, "GET /update?api_key=%s&field1=%bu&field2=%bu",
		WRITE_API_KEY, dat1, dat2);
	
	len = strlen(CmdBuffer) + 2;	// 換行+迴車
	printf("AT+CIPSEND=%bu\n\r", len);
	// 等待回應
	Delay1ms(1000);
	printf(CmdBuffer);
	Delay1ms(1000);			// 防busy
	printf("\n\r");
	Delay1ms(1000);
	printf("AT+CIPCLOSE\n\r");
}
