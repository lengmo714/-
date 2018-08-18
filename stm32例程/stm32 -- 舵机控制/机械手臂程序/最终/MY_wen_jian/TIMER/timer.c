#include "timer.h"
//#include "led.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;

////////////////////////////////////////////////////////////////////////////////// 	  
 

//ͨ�ö�ʱ���жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��3!

extern int time_js;
extern int compare0;
extern int compare;
extern int compare1;
extern int compare2;
extern int compare3;
extern int compare4;






#define LED1 PDout(2)	// PD2	


//0~7 17
void TIM3_Int_Init(u16 arr,u16 psc,int i)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��

	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������5000Ϊ500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(  //ʹ�ܻ���ʧ��ָ����TIM�ж�
		TIM3, //TIM2
		TIM_IT_Update ,
		ENABLE  //ʹ��
		);
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = i;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIMx����
							 
}
//11 12 18
void TIM2_Int_Init(u16 arr,u16 psc,int i)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //ʱ��ʹ��

	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������5000Ϊ500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(  //ʹ�ܻ���ʧ��ָ����TIM�ж�
		TIM2, //TIM2
		TIM_IT_Update ,
		ENABLE  //ʹ��
		);
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = i;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	TIM_Cmd(TIM2, ENABLE);  //ʹ��TIMx����
	
	//DISABLE
							 
}






void TIM2_IRQHandler(void)
{
  	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
		{
	
			
		}
}



void TIM3_IRQHandler(void)   //TIM3�ж�
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
		{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //���TIMx���жϴ�����λ:TIM �ж�Դ 
	//	LED1=!LED1;
			
			time_js ++;
			//PA6
					if(time_js > compare0)
			{
			  GPIO_ResetBits(GPIOA,GPIO_Pin_6); 
		//		LED1 = 1;				
			}
			else
			{
			 GPIO_SetBits(GPIOA,GPIO_Pin_6); 
	//		 LED1 = 0;
			}


						if(time_js > compare)
			{
			  GPIO_ResetBits(GPIOA,GPIO_Pin_4); 
			
			}
			else
			{
			 GPIO_SetBits(GPIOA,GPIO_Pin_4); 

			}
			
			
				if(time_js > compare1)
			{
			  GPIO_ResetBits(GPIOC,GPIO_Pin_3); 
			
			}
			else
			{
			 GPIO_SetBits(GPIOC,GPIO_Pin_3); 

			}
			
						if(time_js > compare2)
			{
			  GPIO_ResetBits(GPIOC,GPIO_Pin_1); 
			
			}
			else
			{
			 GPIO_SetBits(GPIOC,GPIO_Pin_1); 

			}
			
				if(time_js > compare3)
			{
			  GPIO_ResetBits(GPIOC,GPIO_Pin_13); 
	//			LED1 = 1;				
			}
			else
			{
			 GPIO_SetBits(GPIOC,GPIO_Pin_13); 
	//		 LED1 = 0;
			}
			
				if(time_js > compare4)
			{
			  GPIO_ResetBits(GPIOB,GPIO_Pin_15); 
	//			LED1 = 1;				
			}
			else
			{
			 GPIO_SetBits(GPIOB,GPIO_Pin_15); 
	//		 LED1 = 0;
			}
			
						
			if(time_js>=2000)
			{
			  time_js = 0;
				LED1 = !LED1;
			
			}
			
	
			
		}
}












