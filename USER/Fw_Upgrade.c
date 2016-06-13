
#include "Fw_Upgrade.h"
#include <string.h>
#include "W25Q80.h"
#include "flash_if.h"

#ifndef bool 
typedef enum
{
	FALSE,
	TRUE
}bool;
#endif

#define  Application_Addr   0x8004000
#define Firmware_InforAddr  (318*4096UL)
#define  Firmware_StoreAddr (320*4096UL)
#define  FWPackSize	(256)

typedef union{
	uint16_t halword[64];
	uint8_t  bytes[256];
}ImageType;

static Firmware_Infor_t FW_Infor;


static uint16_t CRC16_Calc(const char* array, uint8_t array_len)
{
	 uint16_t CRCFull = 0xFFFF;
	 char CRCLSB;
	 int i=0,j=0;
	 
	 for(i = 0;i< array_len  ;i++)
	 {
	 	  CRCFull = (uint16_t)(CRCFull^array[i]);
		  for(j =0;j< 8;j++)
		  {
		  	  CRCLSB = (char)(CRCFull&0x0001);
			  CRCFull = (uint16_t)((CRCFull >> 1) & 0x7FFF);
			   if(CRCLSB == 1)
                {
                  CRCFull = (uint16_t)(CRCFull ^ 0xA001);
                }
		  }
	 }
	 return CRCFull;
}



static int  Firmware_Infor_Read(Firmware_Infor_t *pFw)
{
	unsigned char array[7]; 
	unsigned short crcbuf;

	W25X_Flash_Byte_Read(Firmware_InforAddr, (unsigned char*)array, 7);
	crcbuf = CRC16_Calc((const char*)array, 5);
	
	if((array[5] == (char)(crcbuf>>8))&&(array[6] == (char)(crcbuf&0x00ff)))
	{
		pFw->size = array[0];
		pFw->size <<= 8;
		pFw->size |= array[1];
		pFw->size <<= 8;
		pFw->size |= array[2];
		pFw->size <<= 8;
		pFw->size |= array[3];
		
		pFw->state = array[4];
		
		return 1;
	}
	
	return 0;
}



static void Firmware_Infor_Write(Firmware_Infor_t *pFw)
{
	unsigned char array[7]; 
	unsigned short CRC_buf;
	if(0 == pFw)	return ;
    W25X_SectorErase(Firmware_InforAddr);
	memset(array, 0, 7);

	array[0] = (unsigned char)(pFw->size >> 24);
	array[1] = (unsigned char)((pFw->size >> 16)&0x000000ff);
	array[2] = (unsigned char)((pFw->size >> 8)&0x000000ff);
	array[3] = (unsigned char)(pFw->size &0x000000ff);

	array[4] = pFw->state;

	CRC_buf = CRC16_Calc((char*)array, 5);
	array[5] = (unsigned char)((CRC_buf>>8)&0x00ff);
	array[6] = (unsigned char)(CRC_buf&0x00ff);

	SPI_Flash_Write_NoCheck(Firmware_InforAddr, array, 7);	
}



unsigned char Firmware_Check(void)
{
		W25QXX_Init();
	
		if(Firmware_Infor_Read(&FW_Infor))
		{
				if(FW_Infor.state == 1)	
					return 1;
				else 
					return 0;
		}
		
		return 0;
}



/**
  * @brief  Calculate the number of pages
  * @param  Size: The image size
  * @retval The number of pages
  */
unsigned long FLASH_PagesMask(unsigned long Size)
{
  unsigned long pagenumber = 0x0;
  unsigned long size = Size;

  if ((size % PAGE_SIZE) != 0)
  {
    pagenumber = (size / PAGE_SIZE) + 1;
  }
  else
  {
    pagenumber = size / PAGE_SIZE;
  }
  return pagenumber;

}

static bool ProgramUserCode(uint32_t DesAddr, uint32_t ImageAddr, uint32_t size)
{
	ImageType datbuf;
	uint16_t i;
	uint16_t tmp;
	FLASH_Status err;

	for(i = 0; i < size/FWPackSize; i++)
	{
	      
		W25X_Flash_Byte_Read( ImageAddr+(i*FWPackSize), datbuf.bytes, FWPackSize);
		err = ProgramDatatoFlash(DesAddr, datbuf.halword, FWPackSize);
		if(DesAddr == 0x8005000)
		{
			DesAddr = 0x8005000;
		}
		//Delay(10);
		if(err != FLASH_COMPLETE)
		{
			return FALSE;
		}
		DesAddr +=FWPackSize;
	}

	return  TRUE;
	
}


void Firmware_Write(void)
{
	FLASH_Status fls_stat;
	unsigned long  totalPage;
	
	totalPage = FLASH_PagesMask(FW_Infor.size);
	
	if(totalPage > 100)	return ;
	
	fls_stat  = ErasePage(16, 16+totalPage);
	
	if(FLASH_COMPLETE == fls_stat)
	{
			if(ProgramUserCode(Application_Addr, Firmware_StoreAddr, FW_Infor.size))      
			{
					FW_Infor.state = 0;
					Firmware_Infor_Write(&FW_Infor);

					return;
			}		
	}
}
