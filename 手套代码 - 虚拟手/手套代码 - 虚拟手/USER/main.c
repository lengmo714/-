#include "led.h"
#include "delay.h"
#include "sys.h"
#include "includes.h"
#include "stdio.h"
#include "Lcd_Driver.h"
#include "GUI.h"
#include "TFT_demo.h"
#include "adc.h"
#include "dma.h"
#include "usart.h"
#include "hc05.h"
#include "mpu6050.h"
#include "inv_mpu.h" 

//-------------------------------------UCOSII��������----------------------------------------------
//START ����
#define START_TASK_PRIO      						6     //�����������ȼ�,��ʼ��������ȼ�����Ϊ���
#define START_STK_SIZE  								64		//���������ջ��С
OS_STK START_TASK_STK[START_STK_SIZE];				//�����ջ	
void start_task(void *pdata);									//������
 			   
//LED����
#define LED_TASK_PRIO   			    			2      //�����������ȼ�,��ʼ��������ȼ�����Ϊ���
#define LED_STK_SIZE  		    					64      //���������ջ��С
OS_STK LED_TASK_STK[LED_STK_SIZE];             //�����ջ         
void led_task(void *pdata);                     //������

//GUI����
#define GUI_TASK_PRIO       						4       //�����������ȼ�,��ʼ��������ȼ�����Ϊ���
#define GUI_STK_SIZE  									128      //���������ջ��С
OS_STK GUI_TASK_STK[GUI_STK_SIZE];
void gui_task(void *pdata);

//ADCת������
#define ADC_TASK_PRIO 									3        //�����������ȼ�,��ʼ��������ȼ�����Ϊ���
#define ADC_STK_SIZE										64       //���������ջ��С
OS_STK ADC_TASK_STK[ADC_STK_SIZE];
void adc_task(void *pdata);

//MPU6050����
#define MPU6050_TASK_PRIO								1          //�����������ȼ�,��ʼ��������ȼ�����Ϊ���
#define MPU6050_STK_SIZE								128        //���������ջ��С
OS_STK MPU6050_TASK_STK[MPU6050_STK_SIZE];
void mpu6050_task(void *pdata);

//HC05����
#define HC05_TASK_PRIO									5           //�����������ȼ�,��ʼ��������ȼ�����Ϊ���
#define HC05_STK_SIZE										64          //���������ջ��С
OS_STK HC05_TASK_STK[HC05_STK_SIZE];
void hc05_task(void *pdata);


//-------------------------------------UCOSII�������ý���----------------------------------------------

//-----------------------------------------ȫ�ֱ���----------------------------------------------------

#define ADC_CHLS 5 							        //5·ADC
#define ADC_CNTS 10						         //ÿ��ADCͨ��ȡ10��ֵ
u16 ADC_VALUES[ADC_CNTS][ADC_CHLS];			//�洢ADCת����M*N�������������� 
u16 ADC_VALUES_AVER[ADC_CHLS];					//ÿ��ADCͨ����ƽ��ֵ
float pitch, roll, yaw;
short pitch_s, roll_s, yaw_s;
short aacx, aacy, aacz;
short aacx_t, aacy_t, aacz_t;
char  gps_rec_data1[10];
char  gps_rec_data2[10];
u8 t;
u8 len;
int count = 0;
int m = 0;
int n = 0;
int figure1,figure2,figure3,figure4,figure5;
int i,j;
long int sum1=0;
long int sum2=0;
long int sum3=0;
long int sum4=0;
long int sum5=0;
u8 send_buf[15];
//---------------------------------------ȫ�ֱ����������---------------------------------------------

//ADC
void ADC_Filter(void)
{
	u8 i, j;
	u16 sum = 0;											
	for(i=0; i < ADC_CHLS; i++)							//ÿ��ͨ������ѭ���˲�
	{
		for(j = 0; j < ADC_CNTS; j++)					//����N��ѭ��
		{
			 sum += ADC_VALUES[j][i];						//�����N�β���ֵ���ܺ�
    }
    ADC_VALUES_AVER[i]= sum /10;          //���ͨ����������ƽ��ֵ
    sum = 0;                              //��������´����¼���
  }
}
 
//int ADC_figure(int i)                    //�����ȴ������ĳ�ʼֵ
//{
//	u8 j;
//	int figure;
//	long int sum = 0;
//		for(j = 0; j < ADC_CNTS; j++)				  	//����N��ѭ��
//		{
//			  sum += ADC_VALUES[j][i];						//�����N�β���ֵ���ܺ�
//    }
//    figure = sum / ADC_CNTS;                //���ͨ����������ƽ��ֵ
//    sum = 0;                                //��������´����¼���
//		return figure;
//}

//------------------------------------------������-----------------------------------------------------

 int main(void)
 {	
	delay_init();	    	 //��ʱ������ʼ��	
  NVIC_Configuration();	 
	uart_init(115200);
	Init_LEDpin();		  	//��ʼ����LED���ӵ�Ӳ���ӿ�
	Lcd_Init();  //LCD��ʼ��
	Adc_Init();   //ģ������ת������ʼ��
	MyDMA_Init(DMA1_Channel1,(u32)&ADC1->DR, (u32)&ADC_VALUES, ADC_CHLS*ADC_CNTS);	//��ʼ��DMA DMA�洢��ֱ�ӷ���
	while(MPU_Init() != 0) printf("mpu init error!");
	while(mpu_dmp_init() != 0)printf("mpu dmp init error!");
	OSInit(); //ϵͳ��ʼ�� ucos ii��ʼ��   
 	OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//������ʼ����
	OSStart();	//����ʼ
 }

	  
//��ʼ����
void start_task(void *pdata) //pdataָ��
{
  OS_CPU_SR cpu_sr=0;   
	pdata = pdata;       
  OS_ENTER_CRITICAL();			//�����ٽ���(�޷����жϴ��)    
 	OSTaskCreate(led_task,(void *)0,(OS_STK*)&LED_TASK_STK[LED_STK_SIZE-1],LED_TASK_PRIO);						   
 	OSTaskCreate(gui_task,(void *)0,(OS_STK*)&GUI_TASK_STK[GUI_STK_SIZE-1],GUI_TASK_PRIO);	
	OSTaskCreate(adc_task,(void *)0,(OS_STK*)&ADC_TASK_STK[ADC_STK_SIZE-1],ADC_TASK_PRIO);	 
	OSTaskCreate(mpu6050_task,(void *)0,(OS_STK*)&MPU6050_TASK_STK[MPU6050_STK_SIZE-1],MPU6050_TASK_PRIO);	 				   
	OSTaskCreate(hc05_task,(void *)0,(OS_STK*)&HC05_TASK_STK[HC05_STK_SIZE-1],HC05_TASK_PRIO);	   				   
	OSTaskSuspend(START_TASK_PRIO);	//������ʼ����.
	OS_EXIT_CRITICAL();				//�˳��ٽ���(���Ա��жϴ��)
}

//gui���� 
void gui_task(void *pdata)
#include <stm32f10x.h>
{	
	char pitch_str[16];
	char roll_str[16];
	char yaw_str[16];

	LCD_LED_SET;
	Lcd_Clear(GRAY0);
	Gui_DrawFont_GBK16(5,20,BLUE,GRAY0,"pitch:");
	Gui_DrawFont_GBK16(5,40,BLUE,GRAY0," roll:");     
	Gui_DrawFont_GBK16(5,60,BLUE,GRAY0," yaw :");
	
	while(1){
		
			if(USART_RX_STA&0x8000)
		{	
	
			len=USART_RX_STA&0x3fff;   //�õ��˴ν��յ������ݳ���
			for(t=1;t<len;t++)
			{
				if(USART_RX_BUF[t]==',')
				{	count++; 
					continue;
				 }
				if(count%2 == 1)
				{
				 gps_rec_data2[n] = USART_RX_BUF[t];
				 n++;
				}
				if(count%2 == 0)
				{	
				  gps_rec_data1[m] = USART_RX_BUF[t];
					m++;
				}
			}
			 for(;m<10;m++)  gps_rec_data1[m]=' ';
	  	 for(;n<10;n++)  gps_rec_data2[n]=' ';
			  m = 0;
			  n = 0;
			  count = 0;
        USART_RX_STA=0;
		}
		
		Gui_DrawFont_GBK16(70, 20, GRAY0, GRAY0, "          ");
		Gui_DrawFont_GBK16(70, 40, GRAY0, GRAY0, "          ");
		Gui_DrawFont_GBK16(70, 60, GRAY0, GRAY0, "          ");

		sprintf(pitch_str, "%+4d" , pitch_s);
		sprintf(roll_str, "%+4d", roll_s);
		sprintf(yaw_str, "%+4d", yaw_s);
		
		Gui_DrawFont_GBK16(70, 20, RED, GRAY0, pitch_str);
		Gui_DrawFont_GBK16(70, 40, RED, GRAY0, roll_str);
		Gui_DrawFont_GBK16(70, 60, RED, GRAY0, yaw_str);
		
		  delay_ms(50);
	}
}

//LED���� 
void led_task(void *pdata)
{	  
	while(1)
	{
		LED1=0;
		delay_ms(300);
		LED1=1;
		delay_ms(300);
	};
}

void adc_task(void *pdata){
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������
	DMA_Cmd(DMA1_Channel1, ENABLE);			    	//ʹ��DMAͨ��
	delay_ms(2000);
//	for(j=0;j<10;j++)
//	{
//	sum1 = sum1 + ADC_figure(0);
//	figure1 = sum1 /10 ;
//	sum2 = sum2 + ADC_figure(1);
//	figure2 = sum2 /10 ;
//	sum3 = sum3 + ADC_figure(2);
//	figure3 = sum3 /10 ;
//	sum4 = sum4 + ADC_figure(3);
//	figure4 = sum4 /10 ;
//	sum5 = sum5 + ADC_figure(4);
//	figure5 = sum5 /10 ;
//	}
	while(1){
		ADC_Filter();
		delay_ms(20);
	}
}

void mpu6050_task(void *pdata){
	
	while(1){
		if(mpu_dmp_get_data(&pitch,&roll,&yaw)==0){
			MPU_Get_Accelerometer(&aacx,&aacy,&aacz);
			pitch_s = -(short)pitch ;
			roll_s = (short)roll ;
			yaw_s = (short)yaw ;
			aacx_t = aacx / 10 + 3276;
			aacy_t = aacy / 10 + 3276;
			aacz_t = aacz / 10 + 3276;
			delay_ms(30);
		}
	}
}

void hc05_task(void *pdata){
	while(1){	
	send_buf[0] = 0X55;										//֡ͷ
	send_buf[1] = 0X53;
	send_buf[8] = 0Xaa;			
	send_buf[14]= 0Xbb;
	send_buf[2]=aacx;
	send_buf[3]=pitch_s;
	send_buf[4]=aacy;
	send_buf[5]=roll_s; 
	send_buf[6]=aacz;
	send_buf[7]=yaw_s;	 
	for(j=0;j<5;j++)
	{
		send_buf[j+9]=ADC_VALUES_AVER[j];
	}	
	for(j=0; j<15; j++)USART1_Send_Char(send_buf[j]);			      	//�������� 
	}

//	while(1){
//		if((ADC_VALUES_AVER[2]>figure3+200)&&pitch_s>20)           //��Ĵָ
//			printf("G91 C05 F2000\n\r");
//		else if((ADC_VALUES_AVER[2]>figure3+200)&&pitch_s<-20)
//			printf("G91 -C05 F2000\n\r");
//		else if((ADC_VALUES_AVER[1]<figure2+100)&&roll_s>20)        //��ָ
//			printf("G91 D05 F2000\n\r");
//		else if((ADC_VALUES_AVER[1]<figure2+100)&&roll_s<-20)
//			printf("G91 -D05 F2000\n\r");
//		else if(roll_s>20)
//			printf("G91 A05 F2000\n\r");
//		else if(roll_s<-20)
//			printf("G91 -A05 F2000\n\r");
//		else if(pitch_s>20)
//			printf("G91 B05 F2000\n\r");
//		else if(pitch_s<-20)
//			printf("G91 -B05 F2000\n\r");
//		delay_ms(500);
//	}
}
