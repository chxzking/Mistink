/**
  ******************************************************************************
  * @file    Mistink_Displayer.c
  * @author  ��㻭�ģ�chxzking��
  * @version V1.0
  * @date    7-2-2025
  * @brief   ����ļ�ʵ������ʾ��Ӳ������ʾ��ʵ���йصĲ���
  ******************************************************************************
  */
	
	
/* Includes ------------------------------------------------------------------*/	
#include "Mistink_Com.h"

//��̬�ڴ��
#include "heap_solution_1.h"

//Ϊ�˼��ݵ�������̬�ڴ�����Ľӿ�
#define malloc(size)			c_malloc(size)		
#define free(ptr)					c_free(ptr)		

#define Mistink_NULL 			((void*)0)

/* ����ʵ�� ------------------------------------------------------------------*/	
/**
  * @brief  ʵ����һ����ʾ������
  * @param  Pixel_Row: ������ʾ���е����ظ���
  * @param  Pixel_Col: ������ʾ���е����ظ���
	* @param  MistinkPlatformDrivers: Ϊʵ����������飬���Ķ�������ڴ˴��鿴 @ref MistinkPltfrmDrv_t
  * @retval ʵ�����ɹ����ض����ַ��ʵ����ʧ�ܷ��ؿյ�ַ
  */
MistinkDisplayer_t* MistinkDisplayer_Create(Buffer_t Pixel_Row,Buffer_t Pixel_Col,const MistinkPltfrmDrv_t* MistinkPlatformDrivers){
		memory_pool_init();
		//�����߼�ʵ���������
		Buffer_t R_Row = Pixel_Row / 8;
		if(Pixel_Row % 8 != 0){
				R_Row++;
		}
		
		//������ʾ��ʵ��
		MistinkDisplayer_t* MistinkDisplayer = (MistinkDisplayer_t*)malloc(sizeof(MistinkDisplayer_t) + sizeof(unsigned char) * R_Row * Pixel_Col); 
		if(MistinkDisplayer == Mistink_NULL){//����ʧ��
				return MistinkDisplayer;
		}
		//��ʼ��
		MistinkDisplayer->Pixel_Row = Pixel_Row;
		MistinkDisplayer->Pixel_Col = Pixel_Col;
		MistinkDisplayer->real_Row = R_Row;
		//Ӳ������д��
		memcpy(&MistinkDisplayer->MistinkPlatformDrivers,MistinkPlatformDrivers,sizeof(MistinkPltfrmDrv_t));
		//��������λ
		MistinkDisplayer->buffer = ((unsigned char*)MistinkDisplayer) + sizeof(MistinkDisplayer_t);
		
		//Ӳ����ʼ��
		(MistinkDisplayer->MistinkPlatformDrivers.__Init)();
		return MistinkDisplayer;
}

/**
  * @brief  ����һ����ʾ������
  * @param  displayer: ��ʾ������
  * @retval none
  */
void MistinkDisplayer_Free(MistinkDisplayer_t* displayer){
		free(displayer);
		return;
}

/**
  * @brief  �����ʾ����ʾ������
  * @param  displayer: ��ʾ������
  * @retval none
  */
void MistinkDisplayer_Clear(MistinkDisplayer_t* displayer){
	(displayer->MistinkPlatformDrivers.__Clear)();
	return;
}
/**
  * @brief  ���������������������Ļ��
  * @param  displayer: ��ʾ������
  * @retval none
  */
void MistinkDisplayer_BufferOutput(MistinkDisplayer_t* displayer){
	(displayer->MistinkPlatformDrivers.__BufferOutput)(displayer->buffer,displayer->real_Row,displayer->Pixel_Col);
	return;
}
