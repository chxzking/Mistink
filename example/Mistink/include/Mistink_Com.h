#ifndef __MISTINK_COM_H__
#define __MISTINK_COM_H__
/**
  ******************************************************************************
  * @file    Mistink_Com.h
  * @author  ��㻭�ģ�chxzking��
  * @version V1.0
  * @date    7-2-2025
  * @brief   ����ļ����ǿ���ڲ�����ͷ�ļ��������˿��ڲ���װ����Ϣ
  ******************************************************************************
  */
	
/* Includes ------------------------------------------------------------------*/	
#include "Mistink.h"
#include "Mistink_Font.h"
//��׼��
#include <math.h>
#include <string.h>
#include <stdio.h>


/* ���� ----------------------------------------------------------------------*/

/** 
  * @brief  �����Ƕ���ʾ���豸�����Ľṹ�壬��������ʾ����Ϣ��Ӳ�������ӿڡ��ڴ滺���� 
  */
struct MistinkDisplayer_t{

	Buffer_t Pixel_Row;													/*!< ��ʾ�����߼������������ظ���Ϊ��λ��*/
	
	Buffer_t Pixel_Col;													/*!< ��ʾ�����߼������������ظ���Ϊ��λ��*/
	
	Buffer_t real_Row;													/*!< ��ʾ�������ʵ����������������������������ֽ�Ϊ��λ��*/
	
	MistinkPltfrmDrv_t MistinkPlatformDrivers;	/*!< Ӳ�����������ĳ�Ա������ @ref MistinkPltfrmDrv_t */
	
	unsigned char* buffer;											/*!< ��ʾ���Ļ�������ַ����ָ������Ļ������������cache�����ʿ��ǣ�
																										����������ṹ��һ�����룬���Բ��ɼ���ʽ�����ڽṹ��β����
																										bufferָ�뽫ָ��˿ռ䣬����ṹ���£�
																										+---------------------------+
																										|		MistinkDisplayer_t			|
																										+---------------------------+
																										|				buffer ptr					| --+
																										+---------------------------+		|
																										|				buffer mem					|	<-+
																										+---------------------------+
	*/
};



/*�ڲ�API-----------------------------------------------------------------*/

//��ģ��ȡ
int Font_Read(unsigned char ROW,unsigned char Columns,char serial_numbers);
//�޸�ָ���ֽڵ�λ
int Bit_Modification(unsigned char* __format,unsigned char __serial__number,PixelStatus_t __binary_number);
//����������ָ��,��ţ�����������۽���ĳһ���ֽڣ��������ָ���ֽ��е�ָ��λ��ֵ���������ִ��ʧ�ܻ᷵��-1
signed char Bit_loading(const unsigned char* __format,unsigned char __serial__number);


#endif
