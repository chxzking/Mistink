#ifndef __M_STM32F103C8T6_H__
#define __M_STM32F103C8T6_H__

	//��ʼ����Ļ
void OLED_Init(void);
//�����ʾ
void OLED_Clear(void);
//���������е������������Ļ�ϣ�
void OLED_BufferOutput(const unsigned char* buffer, unsigned char arrRow, unsigned char arrCol);
#endif
