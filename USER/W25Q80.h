
/***************************************************************************************
****************************************************************************************
* FILE		: W25Q80.h
* Description	: 
*			  
* Copyright (c) 2013 by Chuango. All Rights Reserved.
* 
* History:
* Version		Name       		Date			Description
   0.1		     Sunil	   2013/07/05	Initial Version
   
****************************************************************************************
****************************************************************************************/


#ifndef _W25Q80_H_
#define _W25Q80_H_

#include "stm32f10x_gpio.h"


//MyFlash
#define STAADD							4096*3
#define STROE_FLAG						0XF8
#define OPT_NUM							3
#define MAX_STORE_ADD					100 //���֧�ֵ������ַ

//����CODE
#define MF_ID           						0xEF		//JEDEC Manufacturer��s ID
#define MTC_W25Q80_BV        				0x4014		//W25Q80DW
#define MTC_W25Q16_BV_CL_CV			0x4015		//W25Q16BV W25Q16CL W25Q16CV
#define MTC_W25Q16_DW					0x6015		//W25Q16DW
#define MTC_W25Q32_BV					0x4016		//W25Q32BV
#define MTC_W25Q32_DW					0x6016		//W25Q32DW
#define MTC_W25Q64_BV_CV				0x4017		//W25Q64BV W25Q64CV
#define MTC_W25Q64_DW					0x4017		//W25Q64DW
#define MTC_W25Q128_BV					0x4018		//W25Q128BV

//ָ���
#define W25X_ReadStatus       				0x05		//��״̬�Ĵ���
#define W25X_WriteStatus      				0x01		//д״̬�Ĵ���
#define W25X_ReadDATA8        			0x03		//�ն�_����
#define W25X_FastRead         				0x0B		//���_����
#define W25X_DualOutput       				0x3B		//���_˫���
#define W25X_Write            				0x02		//д_����_0~255���ֽ�
#define W25X_S_Erase         				0x20		//��������4KB
#define W25X_B_Erase          				0xD8		//��������64KB
#define W25X_C_Erase          				0xC7		//��Ƭ��ʽ��
#define W25X_PowerDown       				0xB9		//����
#define W25X_PowerON_ID       			0xAB		//�������Ƕ�ID
#define W25X_JEDEC_ID         				0x9F		//ʮ��λ��JEDEC_ID
#define W25X_WriteEnable      				0x06		//д����
#define W25X_WriteDisable     				0x04		//д��ֹ

//����ָ��
#define W25X_BUSY             				0x01		//FLASHæ
#define W25X_NotBUSY          				0x00		//FLASH��

#define FLASH_PAGE_SIZE		256
#define FLASH_SECTOR_SIZE	4096
#define FLASH_SECTOR_COUNT	512
#define FLASH_BLOCK_SIZE	65536
#define FLASH_PAGES_PER_SECTOR	FLASH_SECTOR_SIZE/FLASH_PAGE_SIZE



int W25QXX_Init(void);

void W25X_SectorErase(unsigned long Addre24);

void W25X_Erase_64K(unsigned long Addre24);

void W25X_Flash_Byte_Read (unsigned long Addre24, unsigned char *pbuf, unsigned short Quantity);

void W25X_Flash_Byte_Write(unsigned long Addre24, unsigned char *pdat, unsigned short Quantity);

void SPI_Flash_Write_NoCheck(unsigned long WriteAddr, u8* pBuffer, u16 NumByteToWrite);


void W25X_SectorErase_Disk(unsigned long SectorNum);

void W25X_Read_Sector(unsigned long SectorNum, unsigned char *pbuf);

void W25X_Write_Sector(unsigned long SectorNum, unsigned char *pbuf);


#endif /*_W25Q80_H_*/

