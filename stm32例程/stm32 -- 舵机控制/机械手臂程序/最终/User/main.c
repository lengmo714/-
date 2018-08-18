#include "stm32f10x.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "MY_time_Conf.h"
//#include "uart_1.h"
//#include "dma.h"
//#include "RS485.h"
//#include "74hc595.h"
#include "string.h"
#include "timer.h"
#include "exti.h"
/**************************
ģ��������Ϣ��
ѡ��оƬ�� stm32f103vc
������Ϣ��stm32f10x_hd.s
�̼��⣺stm32�̼���v3.5
***************************/

//#define led_PB12   PBout(12)	   //λ�󶨣�λ����PB12����ΪLED����������ٶ����
//#define led_PB1   PBout(1)	   //λ�󶨣�λ����PB1����ΪLED����������ٶ����
//#define PA9   PAout(10)
//#define CLK1   PDout(0)
//#define CWW1   PDout(1)
//#define CLK2   PDout(2)
//#define CWW2   PDout(3)


//uint8_t i = 0;    //������

float a[3],w[3],angle[3],T,finger[5];
extern unsigned char Re_buf[15],temp_buf[15],counter;
extern unsigned char sign;


int time_js = 0;
int compare0 = 150;//PWM���
int compare = 150;//PWM���
int compare1 = 150;//PWM���
int compare2 = 150;//PWM���
int compare3 = 150;//PWM���
int compare4 = 150;//PWM���

#define KEY0  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)//��ȡ����1

int sure;



/*************************************************
����: int main(void)
����: main������
����: ��
����: ��
**************************************************/

int main(void)
{
	 GPIO_InitTypeDef  GPIO_InitStructure;
   unsigned char Temp[15];
   //u8 i;

   RCC_Configuration();      //��ʼ��ʱ��
   GPIO_Configuration();      //��ʼ��IO�˿�
   delay_init(72);            //��ʼ����ʱ����
   NVIC_Configuration();      //�ж����ȼ���ʼ��

   USART1_Config();   //��ʼ�� ����USART1
 //  USART2_Config();   //��ʼ�� ����USART2 �����ڽ���MPU6050���ݣ�
   TIM3_Int_Init(9,71,1);//0.01ms�ж�һ�Σ�����PWMͨ�������ͨ���жϲ���pwm
	
		EXTIX_Init();		//�ⲿ�жϳ�ʼ��
   delay_ms(200);
	
 
 	
  // RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //ʹ��PD�˿�ʱ��
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOD, ENABLE);	 //ʹ��PD�˿�ʱ��
	
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//�ر�jtag��ʹ��SWD��������SWDģʽ����
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;				 //LED0-->PA.8 �˿�����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
  GPIO_Init(GPIOA, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOA.8
  GPIO_SetBits(GPIOA,GPIO_Pin_6);						 //PA.8 �����

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;	    		 //LED1-->PD.2 �˿�����, �������

  GPIO_Init(GPIOA, &GPIO_InitStructure);	  				 //������� ��IO���ٶ�Ϊ50MHz
  GPIO_ResetBits(GPIOA,GPIO_Pin_4);						 //PD.2 ����� 	 
	
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_1|GPIO_Pin_15|GPIO_Pin_13;				 //LED0-->PA.8 �˿�����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
  GPIO_Init(GPIOC, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOA.8
  GPIO_SetBits(GPIOC,GPIO_Pin_3|GPIO_Pin_1|GPIO_Pin_15|GPIO_Pin_13);						 //PA.8 �����
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;				 //LED0-->PA.8 �˿�����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
  GPIO_Init(GPIOD, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOA.8
  GPIO_SetBits(GPIOD,GPIO_Pin_2);						 //PA.8 �����
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;				 //LED0-->PA.8 �˿�����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
  GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOA.8
 
	
	 
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;            
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ;                     
 GPIO_Init(GPIOB, &GPIO_InitStructure); 


	
//	delay_ms(2000);		
      
   while (1)
   {
		 //�����Ƕ�ȡ����
      if(sign)
      {  
         memcpy(Temp,Re_buf,15);
         sign=0;
         if((Re_buf[0]==0x55)&&(Re_buf[1]==0x53))       //���֡ͷ
         {  

                 angle[0] = ((short)(Temp[3]<<8| Temp[2]))/32768.0*180;   //X���ת�ǣ�x �ᣩ
                 angle[1] = ((short)(Temp[5]<<8| Temp[4]))/32768.0*180;   //Y�ḩ���ǣ�y �ᣩ
                 angle[2] = ((short)(Temp[7]<<8| Temp[6]))/32768.0*180;   //Z��ƫ���ǣ�z �ᣩ
    	
						
	       //�ֱ۶����ǣ�����ǣ������Ǻ�ƫת�Ƿֱ���Ʋ�ͬ���
						compare0 =150 + angle[2];
						compare = 150 + 0.3*angle[0];
						compare1 =150 - 0.5*angle[0];	
						compare2 =150 - angle[0];	
						compare3 =150 + angle[1];	
						
					
						
						       
						}
				 
		
						//��ָ�����ǣ�����ֻ������һ�������Ҳֻ����һ����ָ����ʵ��finger������������5��ָ�����ȣ�����ʹ��
						  if(Re_buf[8]==0xaa)       
           {  
              finger[0] = Temp[9] ;
					 
					  	compare4 =150 + finger[0]*0.5;
						    
						}
						
						
						
				
											
    //	delay_ms(20);		
						
						
						
						
         }
         
      }
     // delay_ms(50);
   }
 



/*
int main(void)
{
	
	//u8 i;

	RCC_Configuration();	   //��ʼ��ʱ��
	GPIO_Configuration();	   //��ʼ��IO�˿�
	delay_init(72);			   //��ʼ����ʱ����
	NVIC_Configuration();	   //�ж����ȼ���ʼ��

	USART1_Config();   //��ʼ�� ����USART1
	USART2_Config();   //��ʼ�� ����USART2 �����ڽ���MPU6050���ݣ�
	
	delay_ms(200);

		
	while (1)
	{
		if(sign)
		{  
			sign=0;
//			if(Re_buf[0]==0x55)       //���֡ͷ
//			{  
//				switch(Re_buf[1])
//				{
//					case 0x51: //��ʶ������Ǽ��ٶȰ�
//						a[0] = ((short)(Re_buf[3]<<8 | Re_buf[2]))/32768.0*16;		//X����ٶ�
//						a[1] = ((short)(Re_buf[5]<<8 | Re_buf[4]))/32768.0*16;		//Y����ٶ�
//						a[2] = ((short)(Re_buf[7]<<8 | Re_buf[6]))/32768.0*16;		//Z����ٶ�
//						T    = ((short)(Re_buf[9]<<8 | Re_buf[8]))/340.0+36.25;		//�¶�
//						break;
//					case 0x52: //��ʶ������ǽ��ٶȰ�
//						w[0] = ((short)(Re_buf[3]<<8| Re_buf[2]))/32768.0*2000;		//X����ٶ�
//						w[1] = ((short)(Re_buf[5]<<8| Re_buf[4]))/32768.0*2000;		//Y����ٶ�
//						w[2] = ((short)(Re_buf[7]<<8| Re_buf[6]))/32768.0*2000;		//Z����ٶ�
//						T    = ((short)(Re_buf[9]<<8| Re_buf[8]))/340.0+36.25;		//�¶�
//						break;
//					case 0x53: //��ʶ������ǽǶȰ�
//						angle[0] = ((short)(Re_buf[3]<<8| Re_buf[2]))/32768.0*180;	//X���ת�ǣ�x �ᣩ
//						angle[1] = ((short)(Re_buf[5]<<8| Re_buf[4]))/32768.0*180;	//Y�ḩ���ǣ�y �ᣩ
//						angle[2] = ((short)(Re_buf[7]<<8| Re_buf[6]))/32768.0*180;	//Z��ƫ���ǣ�z �ᣩ
//						T        = ((short)(Re_buf[9]<<8| Re_buf[8]))/340.0+36.25;	//�¶�
//
//						//printf("X��Ƕȣ�%.2f   Y��Ƕȣ�%.2f   Z��Ƕȣ�%.2f\r\n",angle[0],angle[1],angle[2]);
//						printf("%.2f   %.2f   %.2f\r\n",angle[0],angle[1],angle[2]);
//						break;
//					default:  break;
//				}
//			}
			//printf("X�Ƕȣ�%.2f  Y�Ƕȣ�%.2f  Z�Ƕȣ�%.2f  X�ٶȣ�%.2f  Y�ٶȣ�%.2f  Z�ٶȣ�%.2f\r\n",angle[0],angle[1],angle[2],w[0],w[1],w[2]);

//			if(temp_buf[0]==0x55)       //���֡ͷ
//			{  
//				temp_buf[0] = 0;
//				switch(temp_buf[1])
//				{
//					case 0x51: //��ʶ������Ǽ��ٶȰ�
//						a[0] = ((short)(temp_buf[3]<<8 | temp_buf[2]))/32768.0*16;		//X����ٶ�
//						a[1] = ((short)(temp_buf[5]<<8 | temp_buf[4]))/32768.0*16;		//Y����ٶ�
//						a[2] = ((short)(temp_buf[7]<<8 | temp_buf[6]))/32768.0*16;		//Z����ٶ�
//						T    = ((short)(temp_buf[9]<<8 | temp_buf[8]))/340.0+36.25;		//�¶�
//						break;
//					case 0x52: //��ʶ������ǽ��ٶȰ�
//						w[0] = ((short)(temp_buf[3]<<8| temp_buf[2]))/32768.0*2000;		//X����ٶ�
//						w[1] = ((short)(temp_buf[5]<<8| temp_buf[4]))/32768.0*2000;		//Y����ٶ�
//						w[2] = ((short)(temp_buf[7]<<8| temp_buf[6]))/32768.0*2000;		//Z����ٶ�
//						T    = ((short)(temp_buf[9]<<8| temp_buf[8]))/340.0+36.25;		//�¶�
//						break;
//					case 0x53: //��ʶ������ǽǶȰ�
//						angle[0] = ((short)(temp_buf[3]<<8| temp_buf[2]))/32768.0*180;	//X���ת�ǣ�x �ᣩ
//						angle[1] = ((short)(temp_buf[5]<<8| temp_buf[4]))/32768.0*180;	//Y�ḩ���ǣ�y �ᣩ
//						angle[2] = ((short)(temp_buf[7]<<8| temp_buf[6]))/32768.0*180;	//Z��ƫ���ǣ�z �ᣩ
//						T        = ((short)(temp_buf[9]<<8| temp_buf[8]))/340.0+36.25;	//�¶�
//
//						printf("X��Ƕȣ�%.2f   Y��Ƕȣ�%.2f   Z��Ƕȣ�%.2f\r\n",angle[0],angle[1],angle[2]);
//						break;
//					default:  break;
//				}
//			}
		}

		//printf("X��Ƕȣ�%.2f   Y��Ƕȣ�%.2f   Z��Ƕȣ�%.2f\r\n",angle[0],angle[1],angle[2]);
		delay_ms(50);
	}
}	 */


