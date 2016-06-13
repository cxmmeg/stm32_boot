#include "main.h"
#include "Menu.h"

static void UsartInit(void);

int main(void)
{
	UsartInit();
	while (1) {
//		SerialPutString("Hello eclipse.\n");
//		Delay_mS(500);
		MenuMain();
	}
}

void UsartInit(void)
{
	USART_InitTypeDef usart;
	GPIO_InitTypeDef gpio;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);

	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio.GPIO_Pin = GPIO_Pin_2;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpio);

	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpio.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOA, &gpio);

	usart.USART_BaudRate = 115200;
	usart.USART_WordLength = USART_WordLength_8b;
	usart.USART_StopBits = USART_StopBits_1;
	usart.USART_Parity = USART_Parity_No;
	usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	usart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &usart);

	USART_Cmd(USART2, ENABLE);
}

