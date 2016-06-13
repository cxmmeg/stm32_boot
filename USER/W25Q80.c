
/***************************************************************************************
****************************************************************************************
* FILE		: W25Q80.c
* Description	: 
*			  
* Copyright (c) 2013 by Chuango. All Rights Reserved.
* 
* History:
* Version		Name       		Date			Description
   0.1		     Sunil	   2013/07/05	Initial Version
   
****************************************************************************************
****************************************************************************************/

#include "W25Q80.h"
#include "string.h"

/***********************************************************************************
刚开始可以根据读取(0X9F)获取设备ID，根据芯片类型获取芯片最多支持的扇区数 
默认扇区大小为(4Kbyte)
W25Q80 = 16(BLOCK)x16(SECTOR)=256x(4Kbyte)=1024Mbyte
W25Q80 = 32(BLOCK)x16(SECTOR)=512x(4Kbyte)=2048Mbyte

************************************************************************************/

/************************************ 硬件配置 ************************************/

#define CS_H()				GPIO_SetBits(GPIOA, GPIO_Pin_4)	
#define CS_L()				GPIO_ResetBits(GPIOA, GPIO_Pin_4)

#define CLK_H()				GPIO_SetBits(GPIOA, GPIO_Pin_5)
#define CLK_L()				GPIO_ResetBits(GPIOA, GPIO_Pin_5)

#define DO_H()				GPIO_SetBits(GPIOA, GPIO_Pin_7)
#define DO_L()				GPIO_ResetBits(GPIOA, GPIO_Pin_7)
	
#define DI()				GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6)

#define W25QXX_Delay(a)		DelayuS(a)



static void DelayuS(unsigned short n)
{
	unsigned short i=0;
	while(n--)
	{
		i=10;
		while(i--) ;
	}
}

/*===============================================================================
* Function       : W25QXX_Init
* Description    : 管脚硬件初始化
* Input Para     : 
* Output Para	   : 
* Return Value   : 
================================================================================*/
int W25QXX_Init(void)
{
	GPIO_InitTypeDef   GPIO_InitStructure;

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE );
	
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_7|GPIO_Pin_5; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	CLK_H();
	CS_H();

	return 0;
}

/*********************************** END硬件配置 **********************************/


/*===============================================================================
* Function       : spi_flash_send_byte
* Description    : 
* Input Para     : 
* Output Para	   : 
* Return Value   : 
================================================================================*/
static void spi_flash_send_byte( unsigned char DATA8 )
{
	unsigned char i=0;
	CLK_H();
	for(i=0; i<8; i++)
	{
		CLK_L();
		if(DATA8&0x80)DO_H();
		else DO_L();
		W25QXX_Delay(1);
		CLK_H();
		DATA8=DATA8<<1;
	}
}


/*===============================================================================
* Function       : spi_flash_receive_byte
* Description    : 
* Input Para     : 
* Output Para	   : 
* Return Value   : 
================================================================================*/
static unsigned char spi_flash_receive_byte(void)
{
	unsigned char DATA8;
	unsigned char x;

	CLK_H();
	DATA8=0x00;
	for (x=0;x<8;x++)
	{ 
		W25QXX_Delay(1);
		CLK_L();
		DATA8=DATA8<<1;
		if(DI())DATA8=DATA8|0x01;
		CLK_H(); 
	}
	return (DATA8);
}


/*===============================================================================
* Function       : WriteEnable
* Description    : 
* Input Para     : 
* Output Para	   : 
* Return Value   : 
================================================================================*/
static void WriteEnable  (void)
{
	CS_L();
	spi_flash_send_byte(W25X_WriteEnable);  
	CS_H();
}


/*===============================================================================
* Function       : W25X_BUSY_OrNot
* Description    : 在读和写之前得先判断FLASH是否BUSY,BUSY的原因是擦除，或是连续读写
				如果没有以上方式，不必判定可以写读  
* Input Para     : 
* Output Para	   : 
* Return Value   : 
================================================================================*/
static unsigned char W25X_BUSY_OrNot (void) 
{				      
	unsigned char k;   
	CS_L();
	spi_flash_send_byte(W25X_ReadStatus);  //读状态寄存器
	k=spi_flash_receive_byte();  //读一个字节
	k=k&0x01;
	CS_H();
	if(k)return (W25X_BUSY);
	else return (W25X_NotBUSY);
}
   

/*===============================================================================
* Function       : W25X_SectorErase
* Description    : //擦除资料图示的4KB空间
* Input Para     : SectorNum:扇区序号
* Output Para	   : 
* Return Value   : 
================================================================================*/
void W25X_SectorErase(unsigned long Addre24)	
{
	unsigned char Addre3;
	unsigned char Addre2;
	unsigned char Addre1;

	while(W25X_BUSY_OrNot ());  //判BUSY	等到Flash闲才能操作
	WriteEnable();   //写允许
	Addre1=Addre24;
	Addre24=Addre24>>8;
	Addre2=Addre24;
	Addre24=Addre24>>8;
	Addre3=Addre24;		 //把地址拆开来

	CS_L();
	spi_flash_send_byte(W25X_S_Erase);//整扇擦除命令
	spi_flash_send_byte(Addre3);
	spi_flash_send_byte(Addre2);
	spi_flash_send_byte(Addre1);
	CS_H();
	W25QXX_Delay(1);

} 



void W25X_Erase_64K(unsigned long Addre24)
{
		unsigned char Addre3;
		unsigned char Addre2;
		unsigned char Addre1;

		while(W25X_BUSY_OrNot ());	//判BUSY	等到Flash闲才能操作
		WriteEnable();	 //写允许
		Addre1=Addre24;
		Addre24=Addre24>>8;
		Addre2=Addre24;
		Addre24=Addre24>>8;
		Addre3=Addre24; 	 //把地址拆开来
	
		CS_L();
		spi_flash_send_byte(W25X_B_Erase);//整扇擦除命令
		spi_flash_send_byte(Addre3);
		spi_flash_send_byte(Addre2);
		spi_flash_send_byte(Addre1);
		CS_H();
		W25QXX_Delay(1);

}



void W25X_SectorErase_Disk(unsigned long SectorNum)
{
	unsigned char Addre3;
	unsigned char Addre2;
	unsigned char Addre1;
	unsigned long Addre24;

	Addre24 = SectorNum*FLASH_SECTOR_SIZE;
	
	while(W25X_BUSY_OrNot ());  //判BUSY	等到Flash闲才能操作
	WriteEnable();   //写允许
	Addre1=Addre24;
	Addre24=Addre24>>8;
	Addre2=Addre24;
	Addre24=Addre24>>8;
	Addre3=Addre24;		 //把地址拆开来

	CS_L();
	spi_flash_send_byte(W25X_S_Erase);//整扇擦除命令
	spi_flash_send_byte(Addre3);
	spi_flash_send_byte(Addre2);
	spi_flash_send_byte(Addre1);
	CS_H();
	W25QXX_Delay(1);
}



/*===============================================================================
* Function       : W25X_Flash_Byte_Write
* Description    : 页写入，往FLASH里写一个或多个字节, //连续写的时候，地址最后八位必须
				从00开始写*如果256个字节都读满或写满，三字节地址最后一个字节必须为0*
				否则当地址越过分面时，会从本页的00从新开始，造成错写
* Input Para     : 
* Output Para	   : 
* Return Value   : 
================================================================================*/
void W25X_Flash_Byte_Write(unsigned long Addre24, unsigned char *pdat, unsigned short Quantity) 
{						
	unsigned char Addre3;	 
	unsigned char Addre2;
	unsigned char Addre1;
	unsigned short i;   //计字节数

	while(W25X_BUSY_OrNot ());  //判BUSY	等到Flash闲才能操作

	WriteEnable();   //写允许
	Addre1=Addre24;
	Addre24=Addre24>>8;
	Addre2=Addre24;
	Addre24=Addre24>>8;
	Addre3=Addre24;		    //把地址拆开来
   
	CS_L();
	spi_flash_send_byte(W25X_Write);  //命令
	spi_flash_send_byte(Addre3);
	spi_flash_send_byte(Addre2);
	spi_flash_send_byte(Addre1);

	for (i=0;i<Quantity;i++)
	{
		spi_flash_send_byte(*pdat++); //写字节
	}

	CS_H();
	W25QXX_Delay(1);

}	 
  

/*===============================================================================
* Function       : W25X_Flash_Byte_Read
* Description    : 从Flash里读出Quantity个字节
* Input Para     : 
* Output Para	   : 
* Return Value   : 
================================================================================*/
void W25X_Flash_Byte_Read (unsigned long Addre24, unsigned char *pbuf, unsigned short Quantity)  
{
	unsigned short i=0;	 //计字节数
	unsigned char Addre3;
	unsigned char Addre2;
	unsigned char Addre1;
   
	while(W25X_BUSY_OrNot ());  //判BUSY

	Addre1=Addre24;
	Addre24=Addre24>>8;
	Addre2=Addre24;
	Addre24=Addre24>>8;
	Addre3=Addre24;		 //把地址拆开来
   
	CS_L();
	spi_flash_send_byte(W25X_ReadDATA8);//命令读
	spi_flash_send_byte(Addre3);
	spi_flash_send_byte(Addre2);
	spi_flash_send_byte(Addre1);

	for (i=0;i<Quantity;i++)
	     *pbuf++=spi_flash_receive_byte();	 //读一个字节

	CS_H();
	W25QXX_Delay(1);
	
}	  

static void w25qxx_read_id(unsigned char *buffer)
{	
	
	CS_L();
	spi_flash_send_byte(W25X_JEDEC_ID);
	*buffer = spi_flash_receive_byte();
	buffer++;
	*buffer = spi_flash_receive_byte();
	buffer++;
	*buffer = spi_flash_receive_byte();
	CS_H();
}


/*===============================================================================
* Function       : w25qxx_get_sector_num
* Description    : 根据芯片类型获取芯片最多支持的扇区数 W25Q80 = 16(BLOCK)x16(SECTOR)=256
				W25Q80 = 32(BLOCK)x16(SECTOR)=512
* Input Para     : 
* Output Para	   : 
* Return Value   : 
================================================================================*/
unsigned short w25qxx_get_sector_num(void)
{
	unsigned char id_recv[3];
	unsigned short memory_type_capacity ,sector_count;
	w25qxx_read_id(id_recv); //读FLASH ID
	if(id_recv[0] != MF_ID)
		return 0;
	memory_type_capacity = (id_recv[1] << 8) | id_recv[2];
	if(memory_type_capacity == MTC_W25Q128_BV)
		sector_count = 4096;
	else if(memory_type_capacity == MTC_W25Q64_BV_CV)
		sector_count = 2048;
	else if(memory_type_capacity == MTC_W25Q64_DW)
		sector_count = 2048;
	else if(memory_type_capacity == MTC_W25Q32_BV)
		sector_count = 1024;
	else if(memory_type_capacity == MTC_W25Q32_DW)
		sector_count = 1024;
	else if(memory_type_capacity == MTC_W25Q16_BV_CL_CV)
		sector_count = 512;
	else if(memory_type_capacity == MTC_W25Q16_DW)
		sector_count = 512;
	else if(memory_type_capacity == MTC_W25Q80_BV)
		sector_count = 256;
	else
		sector_count = 0;

	return  sector_count*8;
}
	    

/*===============================================================================
* Function       : SPI_Flash_Write_NoCheck
* Description    : 无检验写SPI FLASH,必须确保所写的地址范围内的数据全部为0XFF,
				否则在非0XFF处写入的数据将失败!具有自动换页功能,在指定地址
				开始写入指定长度的数据,但是要确保地址不越界!
* Input Para     : pBuffer:数据存储区,WriteAddr:开始写入的地址(24bit)
				NumByteToWrite:要写入的字节数(最大65535)
* Output Para	   : 
* Return Value   : 
================================================================================*/
void SPI_Flash_Write_NoCheck(unsigned long WriteAddr, u8* pBuffer, u16 NumByteToWrite)   
{ 			 		 
	u16 pageremain;	   
	
	pageremain=256-WriteAddr%256; //单页剩余的字节数		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//不大于256个字节
	while(1)
	{	   
		W25X_Flash_Byte_Write(WriteAddr,pBuffer,pageremain);
		if(NumByteToWrite==pageremain)break;//写入结束了
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain; //减去已经写入了的字节数
			if(NumByteToWrite>256)pageremain=256; //一次可以写入256个字节
			else pageremain=NumByteToWrite;  //不够256个字节了
		}
	};	    
} 



/*
	读取一个扇区数据
*/
void W25X_Read_Sector(unsigned long SectorNum, unsigned char *pbuf)
{
	unsigned short i=0;	 //计字节数
	unsigned char Addre3;	 
	unsigned char Addre2;
	unsigned char Addre1;	

	unsigned long addr = SectorNum*FLASH_SECTOR_SIZE;

	Addre1=addr;
	addr=addr>>8;
	Addre2=addr;
	addr=addr>>8;
	Addre3=addr;		 //把地址拆开来	
	

	while(W25X_BUSY_OrNot ());	//判BUSY	等到Flash闲才能操作
	WriteEnable();	 //写允许

	CS_L();
	spi_flash_send_byte(W25X_ReadDATA8);//命令读
	spi_flash_send_byte(Addre3);
	spi_flash_send_byte(Addre2);
	spi_flash_send_byte(Addre1);

	for (i=0;i<FLASH_SECTOR_SIZE;i++)
	 	*pbuf++=spi_flash_receive_byte();  //读一个字节

	CS_H();
	W25QXX_Delay(1);

}




void W25X_Write_Sector(unsigned long SectorNum, unsigned char *pbuf)
{

	int i,j;
	SectorNum *= FLASH_SECTOR_SIZE;

	while(W25X_BUSY_OrNot ());  //判BUSY	等到Flash闲才能操作

	for(j = 0;j < FLASH_PAGES_PER_SECTOR; j++)
	{
		WriteEnable();				   //SET WEL
	 
		CS_L();  
		spi_flash_send_byte(W25X_Write);
		spi_flash_send_byte(((SectorNum & 0xFFFFFF) >> 16));
		spi_flash_send_byte(((SectorNum & 0xFFFF) >> 8));
		spi_flash_send_byte(SectorNum & 0xFF);
	
		for(i=0;i<FLASH_PAGE_SIZE;i++)								
			spi_flash_send_byte(pbuf[i]);
			
		pbuf += FLASH_PAGE_SIZE;
		SectorNum += FLASH_PAGE_SIZE;

		CS_H();
		while(W25X_BUSY_OrNot ());
	}

}


