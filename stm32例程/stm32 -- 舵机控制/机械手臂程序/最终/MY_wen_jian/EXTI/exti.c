#include "exti.h"
//#include "led.h"
//#include "key.h"
#include "delay.h"
#include "usart.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;

 
 
//�ⲿ�жϳ�ʼ������
void EXTIX_Init(void)
{
 
 	  EXTI_InitTypeDef EXTI_InitStructure;
 	  NVIC_InitTypeDef NVIC_InitStructure;

  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//�ⲿ�жϣ���Ҫʹ��AFIOʱ��

	//  KEY_Init();//��ʼ��������Ӧioģʽ

    //GPIOC.5 �ж����Լ��жϳ�ʼ������
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource1);

  	EXTI_InitStructure.EXTI_Line=EXTI_Line1;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//�½��ش���
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���

   


 
  	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;			//ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//��ռ���ȼ�2 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;					//�����ȼ�1
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure);  	  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
		

 
}

 
void EXTI1_IRQHandler(void)
{

	USART_SendData(USART1,0xFF);   //�ô���3����һ���ֽ�
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);  //�ȴ�����3�������
	
  USART_SendData(USART1,0xAA);   //�ô���3����һ���ֽ�
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);  //�ȴ�����3�������
	
  USART_SendData(USART1,0x52);   //�ô���3����һ���ֽ�
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);  //�ȴ�����3�������
	
	EXTI_ClearITPendingBit(EXTI_Line1);  //���EXTI0��·����λ
}

