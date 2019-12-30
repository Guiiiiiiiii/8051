#define  LCD  P0		//	�w�qLCM ��ƶ׬y�Ʊ���P0
sbit  RS	=	P3^2;	//	�Ȧs����ܦ줸(0:���O,1:���)
sbit  RW	=	P3^1;	//	�]�wŪ�g�줸 (0:�g�J,1:Ū��)
sbit  E	=	P3^0;		//	�P��줸 (0:�T��,1:�P��)
sbit  BF	=	P0^7;	//	���L�ˬd�줸(0:����,1:���L)
void init_LCM(void);	//	��l�]�w��� 
void write_inst(char);	//	�g�J���O��� 
void write_char(char);	//	�g�J�r����ƨ�� 
void check_BF(void);	//	�ˬd���L��� 
//====��l�]�w���(8�줸�ǿ�Ҧ�)===================
void init_LCM(void)
{	write_inst(0x30);	// �]�w�\��-8�줸-�򥻫��O 
	write_inst(0x30);	// �]�w�\��-8�줸-�򥻫��O 
	write_inst(0x30);	// �^��LCM�ۮe�]�w�A����LCM�i���� 
	write_inst(0x38);	// �^��LCM�]�w��C�A����LCM�i���� 
	write_inst(0x08);	// ��ܥ\��-����ܹ�-�L���-��Ф��{ 
	write_inst(0x01);	// �M����ܹ�(��0x20,I/D=1)
	write_inst(0x06);	// ��J�Ҧ�-��}���W-����ܹ� 
	write_inst(0x0c);	// ��ܥ\��-�}��ܹ�-�L���-��Ф��{ 
}	// init_LCM()��Ƶ��� 
//==== �g�J���O��� ================================
void write_inst(char inst)
{	check_BF();			// �ˬd�O�_���L 
	LCD = inst;			// LCMŪ�JMPU���O 
	RS = 0; RW = 0; E = 1;	// �g�J���O��LCM
	check_BF();			// �ˬd�O�_���L 
}	// write_inst()��Ƶ��� 
//==== �g�J�r����ƨ�� ============================
void write_char(char chardata)
{	check_BF();			// �ˬd�O�_���L 
	LCD = chardata;		// LCMŪ�J�r�� 
	RS = 1; RW = 0 ;E = 1;	// �g�J��Ʀ�LCM
	check_BF();			// �ˬd�O�_���L 
}						// write_char()��Ƶ��� 	
//====�ˬd���L���================================
void check_BF(void)
{	E=0;				// �T��Ū�g�ʧ@ 
	do					// do-while�j��}�l 
	{ BF=1;				// �]�wBF����J 
  	RS = 0; RW = 1;E = 1;	// Ū��BF��AC
	}while(BF);			// ���L�~�� 
}						// check_BF()��Ƶ��� 
