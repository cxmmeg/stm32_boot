#include "Menu.h"
#include <string.h>
#include "Common.h"
#include "flash_if.h"
#include "Fw_Upgrade.h"


typedef void (*pFunction)(void);
pFunction Jump_To_Application;
uint32_t JumpAddress;
uint8_t Buffer1024[1024] = { 0 };

void ExecuteApp(void);
void RebootIap(void);
void CountdownGetKey(uint8_t *c);

void MenuMain(void)
{
	uint8_t key;
	
		if(Firmware_Check())		// 检测是否有固件要更新
		{
			SerialPutString("There is new Firmware\r\n");
			Firmware_Write();
		}
		
		//ExecuteApp();
		
		CountdownGetKey(&key);
		
	switch (key) {
	case '1':
		SerialPutString("serial input 1.\n");
		//SerialDownload();
		break;
	case '2':
		SerialPutString("serial input 2.\n");
		SerialPutString("It is not supported.\n");
		break;
	case '3':
		SerialPutString("serial input 3.\n");
		ExecuteApp();
		break;
	case '4':
		SerialPutString("serial input 4.\n");
		RebootIap();
		break;
	default:
		SerialPutString("go to default.\n");
		ExecuteApp();
		break;
	}
}



void ExecuteApp(void)
{
	if (((*(__IO uint32_t*) APPLICATION_ADDRESS ) & 0x2FFE0000) == 0x20000000) 
	{
		SerialPutString("Execute user Program.\n");

		JumpAddress = *(__IO uint32_t*) (APPLICATION_ADDRESS + 4);
		/* Jump to user application */
		Jump_To_Application = (pFunction) JumpAddress;
		/* Initialize user application's Stack Pointer */
		__set_MSP(*(__IO uint32_t*) APPLICATION_ADDRESS);
		Jump_To_Application();
	} 
	else 
	{
		SerialPutString("No user Program.\n");
	}
}

void RebootIap(void)
{
	if (((*(__IO uint32_t*) IAP_ADDRESS ) & 0x2FFE0000) == 0x20000000) {
		SerialPutString("Reboot IAP Program.\n");

		JumpAddress = *(__IO uint32_t*) (IAP_ADDRESS + 4);
		/* Jump to user application */
		Jump_To_Application = (pFunction) JumpAddress;
		/* Initialize user application's Stack Pointer */
		__set_MSP(*(__IO uint32_t*) IAP_ADDRESS);
		Jump_To_Application();
	} else {
		SerialPutString("Error to reboot.\n");
	}
}

void CountdownGetKey(uint8_t *c)
{
	uint32_t timer = 4;
	char *disp_str[4] = { "0..", "1..", "2..", "3.." };

	while (timer--) {
		SerialPutString(disp_str[timer]);
		if (!SerialReceiveByte(c, 900000)) {
			return;
		}
	}

	*c = '0';
}



