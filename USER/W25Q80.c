
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
�տ�ʼ���Ը��ݶ�ȡ(0X9F)��ȡ�豸ID������оƬ���ͻ�ȡоƬ���֧�ֵ������� 
Ĭ��������СΪ(4Kbyte)
W25Q80 = 16(BLOCK)x16(SECTOR)=256x(4Kbyte)=1024Mbyte
W25Q80 = 32(BLOCK)x16(SECTOR)=512x(4Kbyte)=2048Mbyte

************************************************************************************/

/************************************ Ӳ������ ************************************/

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
* Description    : �ܽ�Ӳ����ʼ��
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

/*********************************** ENDӲ������ **********************************/


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
* Description    : �ڶ���д֮ǰ�����ж�FLASH�Ƿ�BUSY,BUSY��ԭ���ǲ���������������д
				���û�����Ϸ�ʽ�������ж�����д��  
* Input Para     : 
* Output Para	   : 
* Return Value   : 
================================================================================*/
static unsigned char W25X_BUSY_OrNot (void) 
{				      
	unsigned char k;   
	CS_L();
	spi_flash_send_byte(W25X_ReadStatus);  //��״̬�Ĵ���
	k=spi_flash_receive_byte();  //��һ���ֽ�
	k=k&0x01;
	CS_H();
	if(k)return (W25X_BUSY);
	else return (W25X_NotBUSY);
}
   

/*===============================================================================
* Function       : W25X_SectorErase
* Description    : //��������ͼʾ��4KB�ռ�
* Input Para     : SectorNum:�������
* Output Para	   : 
* Return Value   : 
================================================================================*/
void W25X_SectorErase(unsigned long Addre24)	
{
	unsigned char Addre3;
	unsigned char Addre2;
	unsigned char Addre1;

	while(W25X_BUSY_OrNot ());  //��BUSY	�ȵ�Flash�в��ܲ���
	WriteEnable();   //д����
	Addre1=Addre24;
	Addre24=Addre24>>8;
	Addre2=Addre24;
	Addre24=Addre24>>8;
	Addre3=Addre24;		 //�ѵ�ַ����

	CS_L();
	spi_flash_send_byte(W25X_S_Erase);//���Ȳ�������
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

		while(W25X_BUSY_OrNot ());	//��BUSY	�ȵ�Flash�в��ܲ���
		WriteEnable();	 //д����
		Addre1=Addre24;
		Addre24=Addre24>>8;
		Addre2=Addre24;
		Addre24=Addre24>>8;
		Addre3=Addre24; 	 //�ѵ�ַ����
	
		CS_L();
		spi_flash_send_byte(W25X_B_Erase);//���Ȳ�������
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
	
	while(W25X_BUSY_OrNot ());  //��BUSY	�ȵ�Flash�в��ܲ���
	WriteEnable();   //д����
	Addre1=Addre24;
	Addre24=Addre24>>8;
	Addre2=Addre24;
	Addre24=Addre24>>8;
	Addre3=Addre24;		 //�ѵ�ַ����

	CS_L();
	spi_flash_send_byte(W25X_S_Erase);//���Ȳ�������
	spi_flash_send_byte(Addre3);
	spi_flash_send_byte(Addre2);
	spi_flash_send_byte(Addre1);
	CS_H();
	W25QXX_Delay(1);
}



/*===============================================================================
* Function       : W25X_Flash_Byte_Write
* Description    : ҳд�룬��FLASH��дһ�������ֽ�, //����д��ʱ�򣬵�ַ����λ����
				��00��ʼд*���256���ֽڶ�������д�������ֽڵ�ַ���һ���ֽڱ���Ϊ0*
				���򵱵�ַԽ������ʱ����ӱ�ҳ��00���¿�ʼ����ɴ�д
* Input Para     : 
* Output Para	   : 
* Return Value   : 
================================================================================*/
void W25X_Flash_Byte_Write(unsigned long Addre24, unsigned char *pdat, unsigned short Quantity) 
{						
	unsigned char Addre3;	 
	unsigned char Addre2;
	unsigned char Addre1;
	unsigned short i;   //���ֽ���

	while(W25X_BUSY_OrNot ());  //��BUSY	�ȵ�Flash�в��ܲ���

	WriteEnable();   //д����
	Addre1=Addre24;
	Addre24=Addre24>>8;
	Addre2=Addre24;
	Addre24=Addre24>>8;
	Addre3=Addre24;		    //�ѵ�ַ����
   
	CS_L();
	spi_flash_send_byte(W25X_Write);  //����
	spi_flash_send_byte(Addre3);
	spi_flash_send_byte(Addre2);
	spi_flash_send_byte(Addre1);

	for (i=0;i<Quantity;i++)
	{
		spi_flash_send_byte(*pdat++); //д�ֽ�
	}

	CS_H();
	W25QXX_Delay(1);

}	 
  

/*===============================================================================
* Function       : W25X_Flash_Byte_Read
* Description    : ��Flash�����Quantity���ֽ�
* Input Para     : 
* Output Para	   : 
* Return Value   : 
================================================================================*/
void W25X_Flash_Byte_Read (unsigned long Addre24, unsigned char *pbuf, unsigned short Quantity)  
{
	unsigned short i=0;	 //���ֽ���
	unsigned char Addre3;
	unsigned char Addre2;
	unsigned char Addre1;
   
	while(W25X_BUSY_OrNot ());  //��BUSY

	Addre1=Addre24;
	Addre24=Addre24>>8;
	Addre2=Addre24;
	Addre24=Addre24>>8;
	Addre3=Addre24;		 //�ѵ�ַ����
   
	CS_L();
	spi_flash_send_byte(W25X_ReadDATA8);//�����
	spi_flash_send_byte(Addre3);
	spi_flash_send_byte(Addre2);
	spi_flash_send_byte(Addre1);

	for (i=0;i<Quantity;i++)
	     *pbuf++=spi_flash_receive_byte();	 //��һ���ֽ�

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
* Description    : ����оƬ���ͻ�ȡоƬ���֧�ֵ������� W25Q80 = 16(BLOCK)x16(SECTOR)=256
				W25Q80 = 32(BLOCK)x16(SECTOR)=512
* Input Para     : 
* Output Para	   : 
* Return Value   : 
================================================================================*/
unsigned short w25qxx_get_sector_num(void)
{
	unsigned char id_recv[3];
	unsigned short memory_type_capacity ,sector_count;
	w25qxx_read_id(id_recv); //��FLASH ID
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
* Description    : �޼���дSPI FLASH,����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,
				�����ڷ�0XFF��д������ݽ�ʧ��!�����Զ���ҳ����,��ָ����ַ
				��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
* Input Para     : pBuffer:���ݴ洢��,WriteAddr:��ʼд��ĵ�ַ(24bit)
				NumByteToWrite:Ҫд����ֽ���(���65535)
* Output Para	   : 
* Return Value   : 
================================================================================*/
void SPI_Flash_Write_NoCheck(unsigned long WriteAddr, u8* pBuffer, u16 NumByteToWrite)   
{ 			 		 
	u16 pageremain;	   
	
	pageremain=256-WriteAddr%256; //��ҳʣ����ֽ���		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//������256���ֽ�
	while(1)
	{	   
		W25X_Flash_Byte_Write(WriteAddr,pBuffer,pageremain);
		if(NumByteToWrite==pageremain)break;//д�������
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain; //��ȥ�Ѿ�д���˵��ֽ���
			if(NumByteToWrite>256)pageremain=256; //һ�ο���д��256���ֽ�
			else pageremain=NumByteToWrite;  //����256���ֽ���
		}
	};	    
} 



/*
	��ȡһ����������
*/
void W25X_Read_Sector(unsigned long SectorNum, unsigned char *pbuf)
{
	unsigned short i=0;	 //���ֽ���
	unsigned char Addre3;	 
	unsigned char Addre2;
	unsigned char Addre1;	

	unsigned long addr = SectorNum*FLASH_SECTOR_SIZE;

	Addre1=addr;
	addr=addr>>8;
	Addre2=addr;
	addr=addr>>8;
	Addre3=addr;		 //�ѵ�ַ����	
	

	while(W25X_BUSY_OrNot ());	//��BUSY	�ȵ�Flash�в��ܲ���
	WriteEnable();	 //д����

	CS_L();
	spi_flash_send_byte(W25X_ReadDATA8);//�����
	spi_flash_send_byte(Addre3);
	spi_flash_send_byte(Addre2);
	spi_flash_send_byte(Addre1);

	for (i=0;i<FLASH_SECTOR_SIZE;i++)
	 	*pbuf++=spi_flash_receive_byte();  //��һ���ֽ�

	CS_H();
	W25QXX_Delay(1);

}




void W25X_Write_Sector(unsigned long SectorNum, unsigned char *pbuf)
{

	int i,j;
	SectorNum *= FLASH_SECTOR_SIZE;

	while(W25X_BUSY_OrNot ());  //��BUSY	�ȵ�Flash�в��ܲ���

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


