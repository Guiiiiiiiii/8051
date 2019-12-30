#define  LCD  P0		//	定義LCM 資料匯流排接至P0
sbit  RS	=	P3^2;	//	暫存器選擇位元(0:指令,1:資料)
sbit  RW	=	P3^1;	//	設定讀寫位元 (0:寫入,1:讀取)
sbit  E	=	P3^0;		//	致能位元 (0:禁能,1:致能)
sbit  BF	=	P0^7;	//	忙碌檢查位元(0:不忙,1:忙碌)
void init_LCM(void);	//	初始設定函數 
void write_inst(char);	//	寫入指令函數 
void write_char(char);	//	寫入字元資料函數 
void check_BF(void);	//	檢查忙碌函數 
//====初始設定函數(8位元傳輸模式)===================
void init_LCM(void)
{	write_inst(0x30);	// 設定功能-8位元-基本指令 
	write_inst(0x30);	// 設定功能-8位元-基本指令 
	write_inst(0x30);	// 英文LCM相容設定，中交LCM可忽略 
	write_inst(0x38);	// 英文LCM設定兩列，中交LCM可忽略 
	write_inst(0x08);	// 顯示功能-關顯示幕-無游標-游標不閃 
	write_inst(0x01);	// 清除顯示幕(填0x20,I/D=1)
	write_inst(0x06);	// 輸入模式-位址遞增-關顯示幕 
	write_inst(0x0c);	// 顯示功能-開顯示幕-無游標-游標不閃 
}	// init_LCM()函數結束 
//==== 寫入指令函數 ================================
void write_inst(char inst)
{	check_BF();			// 檢查是否忙碌 
	LCD = inst;			// LCM讀入MPU指令 
	RS = 0; RW = 0; E = 1;	// 寫入指令至LCM
	check_BF();			// 檢查是否忙碌 
}	// write_inst()函數結束 
//==== 寫入字元資料函數 ============================
void write_char(char chardata)
{	check_BF();			// 檢查是否忙碌 
	LCD = chardata;		// LCM讀入字元 
	RS = 1; RW = 0 ;E = 1;	// 寫入資料至LCM
	check_BF();			// 檢查是否忙碌 
}						// write_char()函數結束 	
//====檢查忙碌函數================================
void check_BF(void)
{	E=0;				// 禁止讀寫動作 
	do					// do-while迴圈開始 
	{ BF=1;				// 設定BF為輸入 
  	RS = 0; RW = 1;E = 1;	// 讀取BF及AC
	}while(BF);			// 忙碌繼續等 
}						// check_BF()函數結束 
