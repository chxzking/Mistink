#include "M_stm32f103c8t6.h"
//ƽ̨��׼��
#include "stm32f10x.h"  



//Ӳ��SPI
#define PIN_SPI_CLK				GPIO_Pin_5//PA5
#define GPIO_SPI_CLK			GPIOA

#define PIN_SPI_SDA				GPIO_Pin_7//PA7
#define GPIO_SPI_SDA			GPIOA

#define PIN_SPI_CS				GPIO_Pin_2//PA2
#define GPIO_SPI_CS				GPIOA

#define PIN_SPI_DC				GPIO_Pin_3//PA3
#define GPIO_SPI_DC				GPIOA

#define PIN_SPI_RST				GPIO_Pin_4//PA4
#define GPIO_SPI_RST			GPIOA

#define SPI_CMD	0
#define SPI_DATA	1

/*******************************************
*	��SPIЭ��׼����
*	[��������������]
*	[Ӳ����Ļ��ʼ��]
********************************************/

void OLED_Port_Init(void){
	//ʱ�ӿ���
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	//��ͨ�˿�
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = PIN_SPI_CS | PIN_SPI_DC | PIN_SPI_RST;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	//���ó�ʼ״̬
	GPIO_SetBits(GPIO_SPI_CS, PIN_SPI_CS);//����Ƭѡ
	GPIO_SetBits(GPIO_SPI_RST, PIN_SPI_RST);//���߸�λ
	GPIO_ResetBits(GPIO_SPI_DC, PIN_SPI_DC);//�����ź�ѡ��
	
	//SPI�˿�
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = PIN_SPI_CLK | PIN_SPI_SDA;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	//SPI����
	SPI_InitTypeDef SPI_InitStruct;
	SPI_InitStruct.SPI_BaudRatePrescaler  = SPI_BaudRatePrescaler_2;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;  // CPHA=0
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;    // CPOL=0
	SPI_InitStruct.SPI_CRCPolynomial = 7;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_Direction = SPI_Direction_1Line_Tx;//���߷���
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
	SPI_Init(SPI1,&SPI_InitStruct);
	
	//����SPI
	SPI_Cmd(SPI1, ENABLE);
}

void OLED_WR_Byte(uint8_t data, uint8_t mode) {
    if (mode == SPI_CMD)
        GPIO_ResetBits(GPIO_SPI_DC, PIN_SPI_DC);
    else
        GPIO_SetBits(GPIO_SPI_DC, PIN_SPI_DC);

    GPIO_ResetBits(GPIO_SPI_CS, PIN_SPI_CS); // CS����

    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET); // �ȴ����ͻ�������
    SPI_I2S_SendData(SPI1, data);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);   // �ȴ��������

    GPIO_SetBits(GPIO_SPI_CS, PIN_SPI_CS); // CS����
}
//��ʼ����Ļ
void OLED_Init(void){
OLED_Port_Init();
	GPIO_ResetBits(GPIO_SPI_RST, PIN_SPI_RST);
	for(int i = 0;i<1000000;i++){
	}
	GPIO_SetBits(GPIO_SPI_RST, PIN_SPI_RST);
	
	    /* ��ʼ���������� */
	OLED_WR_Byte(0xAE, SPI_CMD); // �ر���ʾ
	OLED_WR_Byte(0xD5, SPI_CMD); // ����ʱ�ӷ�Ƶ
	OLED_WR_Byte(0x80, SPI_CMD);
	OLED_WR_Byte(0xA8, SPI_CMD); // ���ø�����
	OLED_WR_Byte(0x3F, SPI_CMD); // 1/64
	OLED_WR_Byte(0xD3, SPI_CMD); // ��ʾƫ��
	OLED_WR_Byte(0x00, SPI_CMD);
	OLED_WR_Byte(0x40, SPI_CMD); // ��ʼ��
	OLED_WR_Byte(0x8D, SPI_CMD); // ��ɱ�
	OLED_WR_Byte(0x14, SPI_CMD); // ������ɱ�
	OLED_WR_Byte(0x20, SPI_CMD); // �ڴ�ģʽ
	OLED_WR_Byte(0x00, SPI_CMD); // ˮƽ��ַģʽ
	OLED_WR_Byte(0xA1, SPI_CMD); // ����ӳ��
	OLED_WR_Byte(0xC8, SPI_CMD); // COMɨ�跽��
	OLED_WR_Byte(0xDA, SPI_CMD); // COMӲ������
	OLED_WR_Byte(0x12, SPI_CMD);
	OLED_WR_Byte(0x81, SPI_CMD); // �Աȶ�
	OLED_WR_Byte(0xCF, SPI_CMD);
	OLED_WR_Byte(0xD9, SPI_CMD); // Ԥ�������
	OLED_WR_Byte(0xF1, SPI_CMD);
	OLED_WR_Byte(0xDB, SPI_CMD); // VCOMH
	OLED_WR_Byte(0x40, SPI_CMD);
	OLED_WR_Byte(0xA4, SPI_CMD); // ��ʾȫ��ON
	OLED_WR_Byte(0xA6, SPI_CMD); // ������ʾ
	OLED_WR_Byte(0xAF, SPI_CMD); // ������ʾ
	
}




/************************************************
*	����Ļ������
*	[��ӡ��������Ϣ������Ļ]
*	[�����Ļ��ʾ]
*************************************************/

//�����ʾ
void OLED_Clear(void){  
	uint8_t i,n;
    for(i=0;i<8;i++)
    {
        OLED_WR_Byte(0xB0+i, SPI_CMD);
        OLED_WR_Byte(0x00, SPI_CMD);
        OLED_WR_Byte(0x10, SPI_CMD);
        for(n=0;n<128;n++)
            OLED_WR_Byte(0xff, SPI_DATA);
    }
}

//���������е������������Ļ�ϣ�
void OLED_BufferOutput(const unsigned char* buffer, unsigned char arrRow, unsigned char arrCol){
	uint8_t i,n;
    for(i=0;i<arrRow;i++)
    {
        OLED_WR_Byte(0xB0+i, SPI_CMD);
        OLED_WR_Byte(0x00, SPI_CMD);
        OLED_WR_Byte(0x10, SPI_CMD);
        for(n=0;n<arrCol;n++)
            OLED_WR_Byte(buffer[i * arrCol + n], SPI_DATA);
    }
}




