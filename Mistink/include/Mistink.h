#ifndef __MISTINK_H__
#define __MISTINK_H__
/**
  ******************************************************************************
  * @file    Mistink.h
  * @author  ��㻭�ģ�chxzking��
  * @version V1.0
  * @date    7-2-2025
  * @brief   ����������ͼ�ο���Ϊ��ˮī��Ⱦ����ר������ΪMistink ,����һ��ɢ����īˮ��
	*									 ��ͼ�ο�ּ��ʵ�ֻ�����ʾ��ʵ��Ϊ���ĵĶ�����������������������λ
	*									 ��Ⱦ�����и���ֲ�����Զ������ԣ�����֧��ͬʱ���������ͬЭ�鲻ͬ
	*									 ��С����Ļ��
	*					 �ļ����������ļ��а������������ⲿ��API
  ******************************************************************************
  */
	
/* Includes ------------------------------------------------------------------*/
#include "Mistink_Config.h"


/*����-----------------------------------------------------------------*/
/** 
  * @brief  ��ʾ�����������
  */
typedef struct MistinkDisplayer_t MistinkDisplayer_t;

/** 
  * @brief  Ӳ�������ӿ�
  */
typedef struct MistinkPlatformDrivers_t{
	
	void (*__Init)(void);							 										/*!< Ӳ����ʼ�������ӿ�*/
	
	void (*__BufferOutput)(const unsigned char* buffer,\
				Buffer_t arrRow, Buffer_t arrCol);              /*!< ���ݴ��ڴ滺����ˢ�µ���Ļ�Ľӿ�*/
	
	void (*__Clear)(void);																/*!< �����Ļ�ϵ���ʾ���ݵĽӿ�*/
	
}MistinkPltfrmDrv_t;

/** 
  * @brief  ���ص�״̬�������򿪻��߹ر�
  */
typedef enum PixelStatus_t{
	PixelClose,
	PixelOpen
}PixelStatus_t;



/*��ʾ������-----------------------------------------------------------------*/
//����һ����ʾ��ʵ��
MistinkDisplayer_t* MistinkDisplayer_Create(Buffer_t Pixel_Row,Buffer_t Pixel_Col,const MistinkPltfrmDrv_t* MistinkPlatformDrivers);
//����ָ����ʾ��ʵ��
void MistinkDisplayer_Free(MistinkDisplayer_t* displayer);
//���ָ����ʾ����Ļ��ʾ
void MistinkDisplayer_Clear(MistinkDisplayer_t* displayer);
//��ָ������������ˢ�µ���Ļ��
void MistinkDisplayer_BufferOutput(MistinkDisplayer_t* displayer);



/*���ص����------------------------------------------------------------------*/
int MistinkPixel_Operate(MistinkDisplayer_t* displayer,Buffer_t __ROW,Buffer_t __Columns,PixelStatus_t status);
int MistinkPixel_RegisterOP(MistinkDisplayer_t* displayer,Buffer_t __ROW,Buffer_t __Columns,unsigned char Register_value);//�ԼĴ���Ϊ��λ�������ص�





/*����������--------------------------------------------------------------------*/

//���û�����
void MistinkOP_BufferReset(MistinkDisplayer_t* displayer);
//���ػ�����
unsigned char* MistinkOP_GetBufferAPI(MistinkDisplayer_t* displayer);

//����ͼƬ����������ͼƬλ�ã�ͼƬ�����ݣ�ͼƬ�ĳ���
int M_PictureLoad(MistinkDisplayer_t* displayer,Pos_t __ROW,Pos_t __Columns,const unsigned char* picture,PictureSize_t picture_length,PictureSize_t picture_width);
//�ַ�����ӡ
void M_String(MistinkDisplayer_t* displayer, Pos_t __ROW,Pos_t __Columns,char* str);
//������ӡ
void M_Int(MistinkDisplayer_t* displayer, Pos_t __ROW,Pos_t __Columns,int num);




/*ͼ�����㷨--------------------------------------------------------------------*/






#endif
